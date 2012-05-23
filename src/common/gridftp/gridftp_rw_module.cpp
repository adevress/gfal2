/*
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string>
#include <sstream>

#include "gridftp_plugin_loader.h"
#include "gridftp_rw_module.h"


const Glib::Quark scope_open("GridftpModule::open");
const Glib::Quark scope_read("GridftpModule::read");
const Glib::Quark scope_internal_pread("GridftpModule::internal_pread");
const Glib::Quark scope_write("GridftpModule::write");
const Glib::Quark scope_internal_pwrite("GridftpModule::internal_pwrite");
const Glib::Quark scope_lseek("GridftpModule::lseek");

const Glib::Quark scope_close("GridftpModule::close");

const size_t readdir_len = 65000;

struct GridFTP_File_desc{
	std::auto_ptr<GridFTP_stream_state> stream;
	int open_flags;
	off_t current_offset;
	std::string url;
	Glib::Mutex lock;
		
	GridFTP_File_desc(GridFTP_stream_state * s, const std::string & _url, int flags) : stream(s){		
		gfal_log(GFAL_VERBOSE_TRACE, "create descriptor for %s", _url.c_str());		
		this->open_flags = flags;
		current_offset = 0;
		url = _url;
	}
	
	virtual ~GridFTP_File_desc(){		
		gfal_log(GFAL_VERBOSE_TRACE, "destroy descriptor for %s", url.c_str());
	}	
	
	bool is_not_seeked(){
		return (stream.get() != NULL && 
						current_offset == stream->offset)?true:false;
	}
	
	bool is_eof(){
		return stream->eof;
	}
	
};

inline bool is_read_only(int open_flags){
	return ((open_flags & O_RDONLY) || (( open_flags & (O_WRONLY |  O_RDWR)) ==0));
}

inline bool is_write_only(int open_flags){
	return (open_flags & ( O_WRONLY | O_CREAT ));
}

inline int gridftp_rw_commit_put(const Glib::Quark & scope, GridFTP_File_desc* desc){
	char buffer[2];
	if(is_write_only(desc->open_flags)){
		gfal_log(GFAL_VERBOSE_TRACE," commit change for the current stream PUT ... ");	
		gridftp_write_stream(scope_write, (desc->stream.get()),
				buffer, 0, true);
		gridftp_wait_for_callback(scope, static_cast<GridFTP_Request_state*>(desc->stream.get()));
		gfal_log(GFAL_VERBOSE_TRACE," commited with success ... ");						
	}
	return 0;
}

inline int gridftp_rw_valid_get(const Glib::Quark & scope, GridFTP_File_desc* desc){
	if(is_read_only(desc->open_flags)){
		if(desc->is_eof()){
			gridftp_wait_for_callback(scope, static_cast<GridFTP_Request_state*>(desc->stream.get()));			
		}else{
				gfal_log(GFAL_VERBOSE_TRACE," not a full read -> kill the connexion ");
				try{
					globus_ftp_client_abort(desc->stream->sess->get_ftp_handle());
					gridftp_wait_for_callback(scope, desc->stream.get());	
				}catch(Glib::Error & e ){
					// silent !!
				}
		}					
	}
	return 0;
}

// internal pread, do a read query with offset on a different descriptor, do not change the position of the current one.
ssize_t gridftp_rw_internal_pread(GridFTPFactoryInterface * factory, GridFTP_File_desc* desc, void* buffer, size_t s_buff,
									off_t offset){ // throw Gfal::CoreException								
	gfal_log(GFAL_VERBOSE_TRACE," -> [GridftpModule::internal_pread]");										
	std::auto_ptr<GridFTP_stream_state> stream(new GridFTP_stream_state(factory->gfal_globus_ftp_take_handle(gridftp_hostname_from_url(desc->url.c_str()))));
	
	globus_result_t res = globus_ftp_client_partial_get( // start req
				stream->sess->get_ftp_handle(),
				desc->url.c_str(),
				NULL,
				NULL,
				offset,
				offset + s_buff,
				globus_basic_client_callback,
    			static_cast<GridFTP_Request_state*>(stream.get()));
	gfal_globus_check_result(scope_internal_pread, res);
	
	ssize_t r_size= gridftp_read_stream(scope_internal_pread, stream.get(),
				buffer, s_buff); // read a block

	gridftp_wait_for_callback(scope_internal_pread, static_cast<GridFTP_Request_state*>(stream.get()));					
	gfal_log(GFAL_VERBOSE_TRACE,"[GridftpModule::internal_pread] <-");	
	return r_size;
					
}

// internal pwrite, do a write query with offset on a different descriptor, do not change the position of the current one.
ssize_t gridftp_rw_internal_pwrite(GridFTPFactoryInterface * factory, GridFTP_File_desc* desc, const void* buffer, size_t s_buff,
									off_t offset){ // throw Gfal::CoreException								
	gfal_log(GFAL_VERBOSE_TRACE," -> [GridftpModule::internal_pwrite]");										
	std::auto_ptr<GridFTP_stream_state> stream(new GridFTP_stream_state(factory->gfal_globus_ftp_take_handle(gridftp_hostname_from_url(desc->url.c_str()))));
	
	globus_result_t res = globus_ftp_client_partial_put( // start req
				stream->sess->get_ftp_handle(),
				desc->url.c_str(),
				NULL,
				NULL,
				offset,
				offset + s_buff,
				globus_basic_client_callback,
    			static_cast<GridFTP_Request_state*>(stream.get()));
	gfal_globus_check_result(scope_internal_pwrite, res);
	
	ssize_t r_size= gridftp_write_stream(scope_internal_pwrite, stream.get(),
				buffer, s_buff, false); // write block

	gridftp_wait_for_callback(scope_internal_pwrite, static_cast<GridFTP_Request_state*>(stream.get()));				
	gfal_log(GFAL_VERBOSE_TRACE,"[GridftpModule::internal_pwrite] <-");	
	return r_size;
					
}
										


// gridFTP open is restricted by the protocol : READ or Write but not both
//
gfal_file_handle GridftpModule::open(const char* url, int flag, mode_t mode)
{
	
	std::auto_ptr<GridFTP_File_desc> desc(new GridFTP_File_desc(
							new GridFTP_stream_state(_handle_factory->gfal_globus_ftp_take_handle(gridftp_hostname_from_url(url))),
							url,
							flag)
						);
	gfal_log(GFAL_VERBOSE_TRACE," -> [GridftpModule::open] ");	
	globus_result_t res	;
	
	if( is_read_only(desc->open_flags) ){	// portability hack for O_RDONLY mask // bet on a full read
		 gfal_log(GFAL_VERBOSE_TRACE," -> initialize FTP GET global operations... ");	
		 res = globus_ftp_client_get( // start req
					desc->stream->sess->get_ftp_handle(),
					url,
					NULL,
					NULL,
					globus_basic_client_callback,
					static_cast<GridFTP_Request_state*>(desc->stream.get()));
		gfal_globus_check_result(scope_open, res);							
	}else if( is_write_only(desc->open_flags) ){
		 gfal_log(GFAL_VERBOSE_TRACE," -> initialize FTP PUT global operations ... ");		
		res = globus_ftp_client_put( // bet on a full write
					desc->stream->sess->get_ftp_handle(),
					url,
					NULL,
					NULL,
					globus_basic_client_callback,
					static_cast<GridFTP_Request_state*>(desc->stream.get()));	
		gfal_globus_check_result(scope_open, res);							
	}else{
		 gfal_log(GFAL_VERBOSE_TRACE," -> no operation initialization, switch to partial read/write mode...");
		 desc->stream.reset();	
	}

	gfal_log(GFAL_VERBOSE_TRACE," <- [GridftpModule::open] ");	
	return gfal_file_handle_ext_new(plugin_name(), (gpointer) desc.release(), NULL);
}

ssize_t GridftpModule::read(gfal_file_handle handle, void* buffer, size_t count){
	GridFTP_File_desc* desc = static_cast<GridFTP_File_desc*>(handle->fdesc);
	ssize_t ret;
	
	Glib::Mutex::Lock locker(desc->lock);		
	if( desc->is_not_seeked() &&
		 is_read_only(desc->open_flags)
		&& desc->stream.get() != NULL){	
		gfal_log(GFAL_VERBOSE_TRACE," read in the GET main flow ... ");				
		ret = gridftp_read_stream(scope_read, (desc->stream.get()),
				buffer, count);	
	}else{
		gfal_log(GFAL_VERBOSE_TRACE," read with a pread ... ");				
		ret = gridftp_rw_internal_pread(_handle_factory, desc, buffer, count, desc->current_offset);
	}
	desc->current_offset +=	ret;			
	return ret;
}


ssize_t GridftpModule::write(gfal_file_handle handle, const void* buffer, size_t count){
	GridFTP_File_desc* desc = static_cast<GridFTP_File_desc*>(handle->fdesc);
	ssize_t ret;
	
	Glib::Mutex::Lock locker(desc->lock);		
	if( desc->is_not_seeked() &&
		is_write_only(desc->open_flags) 
		&& desc->stream.get() != NULL){	
		gfal_log(GFAL_VERBOSE_TRACE," write in the PUT main flow ... ");				
		ret = gridftp_write_stream(scope_write, (desc->stream.get()),
				buffer, count, false);	
	}else{
		gfal_log(GFAL_VERBOSE_TRACE," write with a pwrite ... ");				
		ret = gridftp_rw_internal_pwrite(_handle_factory, desc, buffer, count, desc->current_offset);
	}
	desc->current_offset +=	ret;			
	return ret;
}


		
ssize_t GridftpModule::pread(gfal_file_handle handle, void* buffer, size_t count, off_t offset){
	GridFTP_File_desc* desc = static_cast<GridFTP_File_desc*>(handle->fdesc);	
	return gridftp_rw_internal_pread(_handle_factory, desc, buffer, count, offset);
}

ssize_t GridftpModule::pwrite(gfal_file_handle handle, const void* buffer, size_t count, off_t offset){
	GridFTP_File_desc* desc = static_cast<GridFTP_File_desc*>(handle->fdesc);	
	return gridftp_rw_internal_pwrite(_handle_factory, desc, buffer, count, offset);
}

off_t GridftpModule::lseek(gfal_file_handle handle, off_t offset, int whence){
	GridFTP_File_desc* desc = static_cast<GridFTP_File_desc*>(handle->fdesc);		
	
	Glib::Mutex::Lock locker(desc->lock);
	switch(whence){
		case SEEK_SET:
			desc->current_offset = offset;
			break;
		case SEEK_CUR:
			desc->current_offset += offset;
			break;	
		case SEEK_END: // not supported for now ( no meaning in write-once files ... )
		default:
			std::ostringstream o;
			throw Gfal::CoreException(scope_lseek, "Invalid whence", EINVAL);
	}
	return desc->current_offset;
}

int GridftpModule::close(gfal_file_handle handle){
	GridFTP_File_desc* desc = static_cast<GridFTP_File_desc*>(handle->fdesc);	
	if(desc){
		gridftp_rw_commit_put(scope_close, desc);
		gridftp_rw_valid_get(scope_close, desc);
		delete desc;
	}
	return 0;
}
		

// open C bind
extern "C" gfal_file_handle gfal_gridftp_openG(plugin_handle handle, const char* url, int flag, mode_t mode, GError** err){
	g_return_val_err_if_fail( handle != NULL && url != NULL
			, NULL, err, "[gfal_gridftp_openG][gridftp] einval params");

	GError * tmp_err=NULL;
	gfal_file_handle ret = NULL;
	gfal_log(GFAL_VERBOSE_TRACE, "  -> [gfal_gridftp_openG]");
	CPP_GERROR_TRY
		ret = ((static_cast<GridftpModule*>(handle))->open(url,flag, mode));
	CPP_GERROR_CATCH(&tmp_err);
	gfal_log(GFAL_VERBOSE_TRACE, "  [gfal_gridftp_openG]<-");
	G_RETURN_ERR(ret, tmp_err, err);	
}

extern "C" int gfal_gridftp_readG(plugin_handle ch , gfal_file_handle fd, void* buff, size_t s_buff, GError** err){
	g_return_val_err_if_fail( ch != NULL && fd != NULL
			, -1, err, "[gfal_gridftp_readG][gridftp] einval params");

	GError * tmp_err=NULL;
	int ret = -1;
	gfal_log(GFAL_VERBOSE_TRACE, "  -> [gfal_gridftp_readG]");
	CPP_GERROR_TRY
	ret = (int) ((static_cast<GridftpModule*>(ch))->read(fd, buff, s_buff));
	CPP_GERROR_CATCH(&tmp_err);
	gfal_log(GFAL_VERBOSE_TRACE, "  [gfal_gridftp_readG]<-");
	G_RETURN_ERR(ret, tmp_err, err);	
}


extern "C" int gfal_gridftp_writeG(plugin_handle ch , gfal_file_handle fd, const void* buff, size_t s_buff, GError** err){
	g_return_val_err_if_fail( ch != NULL && fd != NULL
			, -1, err, "[gfal_gridftp_writeG][gridftp] einval params");

	GError * tmp_err=NULL;
	int ret = -1;
	gfal_log(GFAL_VERBOSE_TRACE, "  -> [gfal_gridftp_writeG]");
	CPP_GERROR_TRY
		ret = (int) ((static_cast<GridftpModule*>(ch))->write(fd, buff, s_buff));
	CPP_GERROR_CATCH(&tmp_err);
	gfal_log(GFAL_VERBOSE_TRACE, "  [gfal_gridftp_writeG] <-");
	G_RETURN_ERR(ret, tmp_err, err);	
}

extern "C" int gfal_gridftp_closeG(plugin_handle ch, gfal_file_handle fd, GError** err){
	g_return_val_err_if_fail( ch != NULL && fd != NULL
			, -1, err, "[gfal_gridftp_closeG][gridftp] einval params");

	GError * tmp_err=NULL;
	int ret = -1;
	gfal_log(GFAL_VERBOSE_TRACE, "  -> [gfal_gridftp_closeG]");
	CPP_GERROR_TRY
		ret = ((static_cast<GridftpModule*>(ch))->close(fd));
	CPP_GERROR_CATCH(&tmp_err);
	gfal_log(GFAL_VERBOSE_TRACE, "  [gfal_gridftp_closeG]<-");
	G_RETURN_ERR(ret, tmp_err, err);	
	
}

extern "C" int gfal_gridftp_lseekG(plugin_handle ch , gfal_file_handle fd, off_t offset, int whence, GError** err){
	g_return_val_err_if_fail( ch != NULL && fd != NULL
			, -1, err, "[gfal_gridftp_lseekG][gridftp] einval params");

	GError * tmp_err=NULL;
	off_t ret = -1;
	gfal_log(GFAL_VERBOSE_TRACE, "  -> [gfal_gridftp_lseekG]");
	CPP_GERROR_TRY
		ret = ((static_cast<GridftpModule*>(ch))->lseek(fd, offset, whence));
	CPP_GERROR_CATCH(&tmp_err);
	gfal_log(GFAL_VERBOSE_TRACE, "  [gfal_gridftp_lseekG]<-");
	G_RETURN_ERR(ret, tmp_err, err);		
	
}
