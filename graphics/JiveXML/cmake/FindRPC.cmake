# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
# Helper module for finding the location of the <rpc/*.h> headers and the
# corresponding library.
#
# Since this is the only package that depends on this system library, this
# module is defined here, instead of in AtlasCMake.
#
# Defines:
#   RPC_INCLUDE_DIRS
#   RPC_LIBRARIES
#   RPC_FOUND
#
# Can be steered using RPC_ATROOT.
#

# Load the AtlasLCG helper code. Unfortunately AtlasCMake is not good
# at picking up stuff from /usr. Something to be improved at one point.
include( LCGFunctions )

# Declare the module:
lcg_external_module( NAME RPC
   INCLUDE_SUFFIXES include include/tirpc
   INCLUDE_NAMES rpc/rpc.h
   LIBRARY_SUFFIXES lib lib32 lib64
   DEFAULT_COMPONENTS tirpc )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( RPC DEFAULT_MSG RPC_INCLUDE_DIR
   RPC_INCLUDE_DIRS RPC_LIBRARIES )
mark_as_advanced( RPC_FOUND RPC_INCLUDE_DIR )
