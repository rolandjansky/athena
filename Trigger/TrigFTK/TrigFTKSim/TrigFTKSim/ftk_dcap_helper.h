/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef WRAP_FTK_DCAP_HELPER_H
#define WRAP_FTK_DCAP_HELPER_H

// FTK DCAP 
// ================================================================
// see ftk_dcap.h for more information.
// ================================================================
// 2009-06-20 Antonio Boveia (boveia@hep.uchicago.edu)

#ifndef HAVE_BOOST_IOSTREAMS_AND_LIBDCAP
#ifdef HAVE_BOOST_IOSTREAMS
#ifdef HAVE_LIBDCAP
#define HAVE_BOOST_IOSTREAMS_AND_LIBDCAP
#endif
#endif
#endif

#ifdef HAVE_BOOST_IOSTREAMS
#include <boost/iostreams/filtering_stream.hpp>
#else
#include <fstream>
#endif

namespace 
ftk_dcap
{

#ifdef HAVE_BOOST_IOSTREAMS

  typedef boost::iostreams::filtering_istream istream;
  typedef boost::iostreams::filtering_ostream ostream;
#else
  typedef std::ifstream istream;
  typedef std::ofstream ostream;
#endif // end if / else HAVE_BOOST_IOSTREAMS_AND_DCAP

  ftk_dcap::istream* open_for_read( const std::string& filename );
  ftk_dcap::ostream* open_for_write( const std::string& filename );
  bool open_for_read( const std::string& filename , ftk_dcap::istream& );
  bool open_for_write( const std::string& filename , ftk_dcap::ostream& );

}

#endif // WRAP_FTK_DCAP_HELPER_H

