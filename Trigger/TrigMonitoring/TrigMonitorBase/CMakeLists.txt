# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigMonitorBase )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Tree MathCore Hist )

# Component(s) in the package:
atlas_add_library( TrigMonitorBaseLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigMonitorBase
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaMonitoringLib GaudiKernel TrigInterfacesLib )

atlas_add_component( TrigMonitorBase
                     src/components/*.cxx
                     LINK_LIBRARIES TrigMonitorBaseLib )

atlas_add_test( LBNHist_test
                SOURCES test/LBNHist_test.cxx
                LINK_LIBRARIES TrigMonitorBaseLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
