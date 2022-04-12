# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( JiveXMLServer )

# External dependencies:
find_package( tdaq COMPONENTS ipc cmdline owl omniORB4 omnithread)
find_package( tdaq-common COMPONENTS ers)

# Component(s) in the package:
atlas_add_library( JiveXMLServer
                   src/*.cxx
                   PUBLIC_HEADERS JiveXMLServer
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${TDAQ_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${TDAQ_LIBRARIES} CxxUtils GaudiKernel JiveXMLLib )

atlas_add_executable( jivexmlserver
                      bin/jivexmlserver.cc
                      INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${TDAQ_INCLUDE_DIRS}
                      LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${TDAQ_LIBRARIES} GaudiKernel JiveXMLLib JiveXMLServer )

