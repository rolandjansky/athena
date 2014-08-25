/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAMPTOOLS_CFIH
#define ATHENAMPTOOLS_CFIH 1

//FIXME vile hack while icc fixes scoped enums used by boost::copy_file
#ifdef __INTEL_COMPILER
  #include <cstdlib> /* system */
  #define COPY_FILE_HACK(_src, _dest) \
    {std::string _tpcpcommand("cp _src _dest"); system(_tpcpcommand.c_str()); }
#else
  #include <boost/filesystem.hpp>
  #define COPY_FILE_HACK(_src, _dest) \
    boost::filesystem::copy_file(_src, _dest);      
#endif

#endif
