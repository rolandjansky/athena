# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
# Additional CMake settings for the build. Used by Projects/.
#
# Sets:
#   ATLAS_GCC_CHECKERS_CONFIG
#
# Can be steered by:
#   CHECKERGCCPLUGINS_PEDANTIC - enable pedantic checkergcc settings
#

# Setup compiler settings for the checkergcc plugins:

# Resolve relative path to make it look "nicer":
get_filename_component( _baseDir
   "${CxxUtilsSettings_DIR}/../share" ABSOLUTE )

# Default configuration file:
set( _config "${_baseDir}/checkergcc-base.config" )

# Append build specific configuration file:
if( CHECKERGCCPLUGINS_PEDANTIC )
   set( _config "${_config}:${_baseDir}/checkergcc-pedantic.config" )
else()
   set( _config "${_config}:${_baseDir}/checkergcc.config" )
endif()

# Configure the checker:
set( ATLAS_GCC_CHECKERS_CONFIG ${_config}
   CACHE STRING "Configuration file(s) for the GCC checker plugins" FORCE )
