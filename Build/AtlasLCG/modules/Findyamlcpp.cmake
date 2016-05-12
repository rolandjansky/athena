# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Findyamlcpp.cmake 724137 2016-02-16 09:01:57Z krasznaa $
#
# Defines:
#  - YAMLCPP_FOUND
#  - YAMLCPP_INCLUDE_DIR
#  - YAMLCPP_INCLUDE_DIRS
#  - YAMLCPP_<component>_FOUND
#  - YAMLCPP_<component>_LIBRARY
#  - YAMLCPP_LIBRARIES
#  - YAMLCPP_LIBRARY_DIR
#  - YAMLCPP_LIBRARY_DIRS
#
# The script can be steered by YAMLCPP_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME yamlcpp
   INCLUDE_SUFFIXES include INCLUDE_NAMES yaml-cpp/yaml.h
   LIBRARY_SUFFIXES lib
   DEFAULT_COMPONENTS yaml-cpp )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( yamlcpp DEFAULT_MSG YAMLCPP_INCLUDE_DIR
   YAMLCPP_LIBRARIES )
mark_as_advanced( YAMLCPP_FOUND YAMLCPP_INCLUDE_DIR YAMLCPP_INCLUDE_DIRS
   YAMLCPP_LIBRARIES YAMLCPP_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( yamlcpp )
