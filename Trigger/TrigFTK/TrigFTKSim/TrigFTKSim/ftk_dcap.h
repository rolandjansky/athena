/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef WRAP_FTK_DCAP_H
#define WRAP_FTK_DCAP_H

// FTK DCAP 
// ================================================================
// boost::iostreams::source and sink classes for files accessible with
// libdcap (dCache) and/or gzip, bzip2.
// ================================================================
// 2009-06-18 Added seek operations.
// 2009-05-29 Antonio Boveia (boveia@hep.uchicago.edu)

#include "TrigFTKSim/ftk_dcap_helper.h"

#include <iosfwd>
#include <string>
#ifdef HAVE_BOOST_IOSTREAMS
#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#endif

namespace 
ftk_dcap
{

#ifdef HAVE_BOOST_IOSTREAMS_AND_LIBDCAP

  class
  ftk_dcap_source
  {
  public:
    typedef char char_type;
    typedef boost::iostreams::seekable_device_tag category;
    typedef boost::iostreams::stream_offset size_type;
  private:
    std::string _filename;
    int _fd;
    size_type _pos;
    bool _debug;
  public:
    ftk_dcap_source( const std::string& filename );
    virtual ~ftk_dcap_source();
    void debug() { _debug = true; }
    void nodebug() { _debug = false; }
    const bool open();
    const bool is_open() const { return _fd>=0; }
    std::streamsize read( char* s , std::streamsize n );
    std::streamsize write( const char* s , std::streamsize n ) { return 0; }
    boost::iostreams::stream_offset seek( boost::iostreams::stream_offset off ,
                                          std::ios_base::seekdir dir );
  };

  class
  ftk_dcap_sink
  {
  public:
    typedef char char_type;
    typedef boost::iostreams::seekable_device_tag category;
    typedef boost::iostreams::stream_offset size_type;
  private:
    std::string _filename;
    int _fd;
    size_type _pos;
    bool _debug;
  public:
    ftk_dcap_sink( const std::string& filename );
    virtual ~ftk_dcap_sink();
    void debug() { _debug = true; }
    void nodebug() { _debug = false; }
    const bool open();
    const bool is_open() const { return _fd>=0; }
    std::streamsize read( const char* s , std::streamsize n ) { return 0; }
    std::streamsize write( const char* s , std::streamsize n );
    boost::iostreams::stream_offset seek( boost::iostreams::stream_offset off ,
                                          std::ios_base::seekdir dir );
  };

#endif // end if HAVE_BOOST_IOSTREAMS_AND_DCAP


}

#endif // WRAP_FTK_DCAP_H

