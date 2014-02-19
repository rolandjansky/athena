#ifndef FPTRACKER_OPENFILE_TPL
#define FPTRACKER_OPENFILE_TPL


#include <string>
#include <stdexcept>

namespace FPTracker{

  template<class streamptr>
  inline void openFile(std::string dir, const std::string& fn, streamptr& strm){

    if (dir.empty()){
	dir = "/" ;	
    }
    
    if (dir[dir.size()-1] != '/'){
	dir += "/";
    }

		
    std::string fullname = dir+fn;
    strm->open( fullname.c_str() );

    if( !(strm->good()) )
    {
      std::string msg = "Error opening file "+ fullname;
      throw std::runtime_error(msg);
    }
  }
}
#endif
