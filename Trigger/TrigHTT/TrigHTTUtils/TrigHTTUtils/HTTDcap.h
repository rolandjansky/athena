/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef WRAP_HTT_DCAP_H
#define WRAP_HTT_DCAP_H

// HTT DCAP
// ================================================================
// boost::iostreams::source and sink classes for files accessible with
// libdcap (dCache) and/or gzip, bzip2.
// ================================================================
// 2009-06-18 Added seek operations.
// 2009-05-29 Antonio Boveia (boveia@hep.uchicago.edu)

#include <iosfwd>
#include <string>
#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/filtering_stream.hpp>

namespace htt_dcap
{

    typedef boost::iostreams::filtering_istream istream;
    typedef boost::iostreams::filtering_ostream ostream;

    htt_dcap::istream* open_for_read(const std::string& filename);
    htt_dcap::ostream* open_for_write(const std::string& filename);
    bool open_for_write(const std::string& filename, htt_dcap::ostream& os);
    bool open_for_read(const std::string& filename, htt_dcap::istream& is);

#ifdef HAVE_LIBDCAP

    class htt_dcap_source
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
            htt_dcap_source( const std::string& filename );
            virtual ~htt_dcap_source();
            void debug() { _debug = true; }
            void nodebug() { _debug = false; }
            const bool open();
            const bool is_open() const { return _fd>=0; }
            std::streamsize read( char* s , std::streamsize n );
            std::streamsize write( const char* s , std::streamsize n ) { return 0; }
            boost::iostreams::stream_offset seek( boost::iostreams::stream_offset off ,
                    std::ios_base::seekdir dir );
    };

    class htt_dcap_sink
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
            htt_dcap_sink( const std::string& filename );
            virtual ~htt_dcap_sink();
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

#endif // WRAP_HTT_DCAP_H

