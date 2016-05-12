# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindLhapdf.cmake 723149 2016-02-10 16:26:22Z krasznaa $
#
# Module finding Lhapdf in the LCG release. Defines:
#  - LHAPDF_FOUND
#  - LHAPDF_INCLUDE_DIR
#  - LHAPDF_INCLUDE_DIRS
#  - LHAPDF_LIBRARIES
#  - LHAPDF_LIBRARY_DIRS
#
# Can be steered by LHAPDF_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME Lhapdf
   INCLUDE_SUFFIXES include INCLUDE_NAMES LHAPDF/LHAPDF.h
   LIBRARY_SUFFIXES lib
   COMPULSORY_COMPONENTS LHAPDF )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Lhapdf DEFAULT_MSG LHAPDF_INCLUDE_DIR
   LHAPDF_LIBRARIES )
mark_as_advanced( LHAPDF_FOUND LHAPDF_INCLUDE_DIR LHAPDF_INCLUDE_DIRS
   LHAPDF_LIBRARIES LHAPDF_LIBRARY_DIRS )

# Environment settings:
set( LHAPDF_ENVIRONMENT
   APPEND LHAPDF_DATA_PATH ${LHAPDF_ROOT}/share/LHAPDF
   APPEND LHAPDF_DATA_PATH
   /cvmfs/atlas.cern.ch/repo/sw/Generators/lhapdfsets/current
   APPEND LHAPATH ${LHAPDF_ROOT}/share/LHAPDF
   APPEND LHAPATH
   /cvmfs/atlas.cern.ch/repo/sw/Generators/lhapdfsets/current )

# Set up the RPM dependency:
lcg_need_rpm( lhapdf )
