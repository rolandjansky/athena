/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// FTK DCAP 
// ================================================================
// boost::iostreams::source and sink classes for files accessible with
// libdcap (dCache) and/or gzip, bzip2.
// ================================================================
// 2009-06-18 Added seek operations.
// 2009-05-29 Antonio Boveia (boveia@hep.uchicago.edu)

#include "TrigFTKSim/ftk_dcap.h"

#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <ios>
#ifdef HAVE_BOOST_IOSTREAMS
#include <boost/array.hpp>
#include <boost/filesystem.hpp> 
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#ifdef HAVE_LIBDCAP
#include <dcap.h>
#else
#include <boost/iostreams/device/file.hpp>
#endif
#endif

using namespace std;
using namespace ftk_dcap;


string getExt(const string &filename, size_t &lastdot)
{
  lastdot = filename.rfind(".");
  
  if (lastdot!=string::npos) return filename.substr(lastdot+1);
  return "";
}

/** This string return true if the text is all composed by decimanl digits */
bool isANum(const string &text)
{
  string::const_iterator ic = text.begin();
  for (;ic!=text.end();++ic) {
    char c = *ic;
    if (!isdigit(c)) return false;
  }

  return true;
}

#ifdef HAVE_BOOST_IOSTREAMS_AND_LIBDCAP

ftk_dcap::ftk_dcap_source::ftk_dcap_source( const std::string& filename )
  :
  _filename( filename )
  , _fd( -1 )
  , _pos( 0 )
  , _debug( false )
{
}

ftk_dcap::ftk_dcap_source::~ftk_dcap_source()
{
  if( _fd>=0 ) {
    dc_close( _fd );
  }
}

/* An approach suggested by Charles to set dcache open() timeout */
int dc_open_safe( const char *filename, int oflag, mode_t mode) {
  int fd = -1;
  dc_setOpenTimeout(10);
  for (int retry=0; retry<5; ++retry) {
    fd = dc_open(filename,oflag,mode);
    if (fd >= 0)
      break;
    else {
      cout << "Failed to open file " << filename << " (attempt #" << retry << ")" << endl;
      sleep(10);
      // Reset debug level to avoid dcap spam
      dc_setDebugLevel(1);
    }
  }
  return fd;
}

const bool
ftk_dcap::ftk_dcap_source::open()
{
  if( _debug ) { 
    dc_setDebugLevel(2); 
  }
  // check that file exists in the filesystem - dcache bz2 file would hang otherwise
  if(!boost::filesystem::exists(_filename.c_str())) {
    if( _debug ) { cerr << " ftk_dcap_source: file " << _filename << " does not exist in the filesystem." << endl; }
    return false;
  }
  // check that file exists and is readable
  if( dc_access( _filename.c_str() , R_OK ) != 0 ) { 
    if( _debug ) { cerr << " ftk_dcap_source: file " << _filename << " does not exist or is unreadable." << endl; }
    return false;
  } 
  // open the file read-only
  _fd = dc_open_safe( _filename.c_str() , O_RDONLY, S_IRUSR );
  if( _fd < 0 ) { 
    if( _debug ) { cerr << " ftk_dcap_source: file " << _filename << " cannot be opened." << endl; }
    _fd = -1;
    return false;
  }
  if( _debug ) { cerr << " ftk_dcap_source: file " << _filename << " is open." << endl; }
  _pos = 0;
  return true;
}

// move read position
boost::iostreams::stream_offset
ftk_dcap::ftk_dcap_source::seek( boost::iostreams::stream_offset off ,
                                 std::ios_base::seekdir dir )
{
  if( !is_open() ) { 
    if( !open() ) { return -1; }
  }
  boost::iostreams::stream_offset result;
  if( dir == ios::beg ) {
    result = dc_lseek( _fd , off , SEEK_SET );
  } else if( dir == ios::cur ) {
    result = dc_lseek( _fd , off , SEEK_CUR );
  } else if( dir == ios::end ) {
    result = dc_lseek( _fd , off , SEEK_END );
  } else {
    throw ios::failure("bad seek direction");
  }
  _pos = result;
  return result;
}

// read up to n characters from the underlying data source into the
// result buffer, returning the number of characters read; return -1
// to indicate EOF.
std::streamsize
ftk_dcap::ftk_dcap_source::read( char* result , std::streamsize nmax ) 
{
  if( !is_open() ) { 
    if( !open() ) { return -1; }
  }
  std::streamsize n = dc_read( _fd , result , nmax );
  return n;
}



ftk_dcap::ftk_dcap_sink::ftk_dcap_sink( const std::string& filename )
  : _filename( filename )
  , _fd( -1 )
  , _pos( 0 )
  , _debug( false )
{
}

ftk_dcap::ftk_dcap_sink::~ftk_dcap_sink()
{
  if( _fd>=0 ) {
    dc_close( _fd );
  }
}

const bool
ftk_dcap::ftk_dcap_sink::open()
{
  if( _debug ) { 
    dc_setDebugLevel(2); 
  }
  // try to delete the file if it exists
  if( dc_access( _filename.c_str() , R_OK ) == 0 ) { 
    if( _debug ) { cerr << " ftk_dcap_sink: file " << _filename << " already exists. deleting it." << endl; }
    if( dc_access( _filename.c_str() , W_OK ) != 0 ) { 
      if( _debug ) { cerr << " ftk_dcap_sink: cannot delete " << _filename << " (read only)." << endl; }
      return false;
    }
    dc_unlink( _filename.c_str() );
    if( dc_access( _filename.c_str() , R_OK ) == 0 ) { 
      if( _debug ) { cerr << " ftk_dcap_sink: file " << _filename << " still exists after attempt to delete it." << endl; }
      return false;
    }
  }
  // open the file write-only
  _fd = dc_open_safe( _filename.c_str() , O_WRONLY | O_CREAT | O_TRUNC , 0x1A4 );
  if( _fd < 0 ) { 
    if( _debug ) { cerr << " ftk_dcap_sink: file " << _filename << " cannot be opened." << endl; }
    _fd = -1;
    return false;
  }
  if( _debug ) { cerr << " ftk_dcap_sink: file " << _filename << " is open." << endl; }
  _pos = 0;
  return true;
}

// move write position
boost::iostreams::stream_offset
ftk_dcap::ftk_dcap_sink::seek( boost::iostreams::stream_offset off ,
                                 std::ios_base::seekdir dir )
{
  if( !is_open() ) { 
    if( !open() ) { return -1; }
  }
  boost::iostreams::stream_offset result;
  if( dir == ios::beg ) {
    result = dc_lseek( _fd , off , SEEK_SET );
  } else if( dir == ios::cur ) {
    result = dc_lseek( _fd , off , SEEK_CUR );
  } else if( dir == ios::end ) {
    result = dc_lseek( _fd , off , SEEK_END );
  } else {
    throw ios::failure("bad seek direction");
  }
  _pos = result;
  return result;
}

// write up to n characters from result into the underlying data sink
// returning the number of characters written.
std::streamsize
ftk_dcap::ftk_dcap_sink::write( const char* result , std::streamsize nmax ) 
{
  if( !is_open() ) { 
    if( !open() ) { return -1; }
  }
  const std::streamsize n_to_write( nmax );
  return( dc_write( _fd , result , n_to_write ) );
}

ftk_dcap::istream*
ftk_dcap::open_for_read( const std::string& filename )
{
  boost::iostreams::filtering_istream* inf = 0;
  try {
    inf = new boost::iostreams::filtering_istream;
  } catch(...) {
    return 0;
  } 
  if( !inf ) { return 0; }
  const bool ok = open_for_read( filename , *inf );
  if( !ok ) { delete inf; return 0; }
  return inf;  
}

bool
ftk_dcap::open_for_read( const std::string& filename , ftk_dcap::istream& inf )
{
  inf.reset();
  std::string basefilename = filename;
  if(!boost::filesystem::exists(filename.c_str())) {
    return false;
  }
  // Remove grid-generated numbers like .bz2.1
  std::vector<std::string> splitresult;
  boost::algorithm::split( splitresult, filename, 
			   boost::algorithm::is_any_of(".") );
  std::string laststr( splitresult.back() );
  if( splitresult.size() && laststr.size() &&
      boost::algorithm::all( laststr, boost::algorithm::is_digit() ) ) {
    basefilename = filename.substr(0, filename.size() - laststr.size() - 1);
  }
  // check the filename extension and turn any decompression that is needed.
  if( boost::algorithm::iends_with(basefilename,".gz") ) { inf.push( boost::iostreams::gzip_decompressor() ); }
  else if( boost::algorithm::iends_with(basefilename,".bz2") ) { inf.push( boost::iostreams::bzip2_decompressor() ); }
  inf.push( ftk_dcap_source(filename) ); // open the file
  return true;
}


ftk_dcap::ostream*
ftk_dcap::open_for_write( const std::string& filename )
{
  boost::iostreams::filtering_ostream* outf = 0;
  try {
    outf = new boost::iostreams::filtering_ostream;
  } catch(...) {
    return 0;
  }
  if( !outf ) { return 0; }
  const bool ok = open_for_write( filename , *outf );
  if( !ok ) { delete outf; return 0; }
  return outf;  
}

bool
ftk_dcap::open_for_write( const std::string& filename , ftk_dcap::ostream& outf )
{
  outf.reset();
  // check the filename extension and turn any compression that is needed.
  if( boost::algorithm::iends_with(filename,".gz") ) { 
    // passing parameters to gzip is not implemented in boost 1.33 / Athena 13.0.40
    //     boost::iostreams::gzip::gzip_params params;
    //     params.level = 9;
    //     outf->push( boost::iostreams::gzip_compressor(params) ); 
    outf.push( boost::iostreams::gzip_compressor() ); 
  } else if( boost::algorithm::iends_with(filename,".bz2") ) { 
    outf.push( boost::iostreams::bzip2_compressor() ); 
  }
  outf.push( ftk_dcap_sink(filename) ); // open the file
  return true;
}

#else // HAVE_BOOST_IOSTREAMS_AND_DCAP is not defined
#ifdef HAVE_BOOST_IOSTREAMS


ftk_dcap::istream*
ftk_dcap::open_for_read( const std::string& filename )
{
  boost::iostreams::filtering_istream* inf = 0;
  try {
    inf = new boost::iostreams::filtering_istream;
  } catch(...) {
    return 0;
  } 
  if( !inf ) { return 0; }
  const bool ok = open_for_read( filename , *inf );
  if( !ok ) { delete inf; return 0; }
  return inf;  
}

bool
ftk_dcap::open_for_read( const std::string& filename , ftk_dcap::istream& inf )
{
  inf.reset();
  std::string basefilename = filename;
  if(!boost::filesystem::exists(filename.c_str())) {
    return false;
  }
  // Remove grid-generated numbers like .bz2.1
#if 0
  std::vector<std::string> splitresult;
  boost::algorithm::split( splitresult, filename, 
			   boost::algorithm::is_any_of(".") );
  std::string laststr( splitresult.back() );
  if( splitresult.size() && laststr.size() &&
      boost::algorithm::all( laststr, boost::algorithm::is_digit() ) ) {
    basefilename = filename.substr(0, filename.size() - laststr.size() - 1);
  }

  // check the filename extension and turn any decompression that is needed.
  if( boost::algorithm::iends_with(basefilename,".gz") ) { inf.push( boost::iostreams::gzip_decompressor() ); }
  else if( boost::algorithm::iends_with(basefilename,".bz2") ) { inf.push( boost::iostreams::bzip2_decompressor() ); }
#else
  size_t dpos(filename.size());
  string fext = getExt(filename,dpos);
  if (dpos!=string::npos) {
    // check if the extension is a number
    if (isANum(fext)) {
      basefilename = filename.substr(0,dpos-1);
    }
  }

  // check the filename extension and turn any decompression that is needed.
  fext = getExt(basefilename,dpos);
  if( fext == "gz" ) { inf.push( boost::iostreams::gzip_decompressor() ); }
  else if( fext == "bz2" ) { inf.push( boost::iostreams::bzip2_decompressor() ); }
#endif

  boost::iostreams::file_source ifile(filename); // open the file
  if (!ifile.is_open()) return false;

  inf.push(ifile); // push the file in the chain
  
  return true;
}


ftk_dcap::ostream*
ftk_dcap::open_for_write( const std::string& filename )
{
  boost::iostreams::filtering_ostream* outf = 0;
  try {
    outf = new boost::iostreams::filtering_ostream;
  } catch(...) {
    return 0;
  }
  if( !outf ) { return 0; }
  const bool ok = open_for_write( filename , *outf );
  if( !ok ) { delete outf; return 0; }
  return outf;  
}

bool
ftk_dcap::open_for_write( const std::string& filename , ftk_dcap::ostream& outf )
{
  outf.reset();
  // check the filename extension and turn any compression that is needed.
  if( boost::algorithm::iends_with(filename,".gz") ) { 
    // passing parameters to gzip is not implemented in boost 1.33 / Athena 13.0.40
    //     boost::iostreams::gzip::gzip_params params;
    //     params.level = 9;
    //     outf->push( boost::iostreams::gzip_compressor(params) ); 
    outf.push( boost::iostreams::gzip_compressor() ); 
  } else if( boost::algorithm::iends_with(filename,".bz2") ) { 
    outf.push( boost::iostreams::bzip2_compressor() ); 
  }
  outf.push( boost::iostreams::file_sink(filename) ); // open the file
  return true;
}

#else

ftk_dcap::ostream* 
ftk_dcap::open_for_write( const std::string& filename )
{
  try {
    return new ofstream( filename.c_str() );
  } catch(...) {
    return 0;
  }
}

bool
ftk_dcap::open_for_read( const std::string& filename , ftk_dcap::istream& inf ) 
{
  if( inf.is_open() ) { inf.close(); }
  inf.clear();

  /* if the exstension is bz2 is a problem, if this function is used
     the compressed files are not supported */
  if (filename.substr(filename.size()-3) == "bz2" ||
      filename.substr(filename.size()-2) == "gz" ) {
    cerr << "Compressed files are not supported" << endl;
    return false;
  }

  try {
     inf.open( filename.c_str() );
  } catch(...) {
    cout << " open file " << filename << " failed." << endl;
    return false;
  }
  return true;
}


bool
ftk_dcap::open_for_write( const std::string& filename , ftk_dcap::ostream& outf )
{
  outf.clear();

  /* if the exstension is bz2 is a problem, if this function is used
     the compressed files are not supported */
  if (filename.substr(filename.size()-3) == "bz2" ||
      filename.substr(filename.size()-2) == "gz" ) {
    cerr << "Compressed files are not supported" << endl;
    return false;
  }

  try {
    outf.open( filename.c_str() );
  } catch(...) { 
    return false; 
  }
  return true;
}
#endif // HAVE_BOOST_IOSTREAMS
#endif // end if / else HAVE_BOOST_IOSTREAMS_AND_DCAP
