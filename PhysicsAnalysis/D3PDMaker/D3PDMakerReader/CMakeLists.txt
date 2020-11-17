# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( D3PDMakerReader )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( D3PDMakerReaderLib
                   src/*.cxx
                   NO_PUBLIC_HEADERS
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel D3PDMakerInterfaces D3PDMakerUtils )

atlas_add_component( D3PDMakerReader
                     src/components/*.cxx
                     LINK_LIBRARIES D3PDMakerReaderLib )

atlas_add_dictionary( D3PDMakerReaderDict
                      src/D3PDMakerReaderDict.h
                      D3PDMakerReader/selection.xml
                      LINK_LIBRARIES D3PDMakerReaderLib )

atlas_add_executable( d3pdReaderMaker
                      src/apps/d3pdReaderMaker.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel GaudiKernel D3PDMakerReaderLib )

atlas_add_executable( d3pdReadersFromFile
                      src/apps/d3pdReadersFromFile.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel D3PDMakerUtils GaudiKernel D3PDMakerReaderLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( share/d3pdReaderLibraryMaker.py share/d3pdReaderRootCoreMaker.py share/d3pdReaderSFrameMaker.py )

