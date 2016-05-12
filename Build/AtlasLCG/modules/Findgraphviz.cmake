# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Findgraphviz.cmake 724137 2016-02-16 09:01:57Z krasznaa $
#
# Defines:
#  - GRAPHVIZ_FOUND
#  - GRAPHVIZ_INCLUDE_DIRS
#  - GRAPHVIZ_LIBRARY_DIRS
#  - GRAPHVIZ_<component>_FOUND
#  - GRAPHVIZ_<component>_LIBRARY
#  - GRAPHVIZ_LIBRARIES
#
# Can be steered by GRAPHVIZ_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME graphviz
   INCLUDE_SUFFIXES include INCLUDE_NAMES graphviz/graphviz_version.h
   LIBRARY_SUFFIXES lib
   DEFAULT_COMPONENTS gvc )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( graphviz DEFAULT_MSG GRAPHVIZ_INCLUDE_DIR
   GRAPHVIZ_LIBRARIES )
mark_as_advanced( GRAPHVIZ_FOUND GRAPHVIZ_INCLUDE_DIR GRAPHVIZ_INCLUDE_DIRS
   GRAPHVIZ_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( graphviz )
