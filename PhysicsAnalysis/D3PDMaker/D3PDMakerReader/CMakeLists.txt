################################################################################
# Package: D3PDMakerReader
################################################################################

# Declare the package name:
atlas_subdir( D3PDMakerReader )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          GaudiKernel
                          PhysicsAnalysis/D3PDMaker/D3PDMakerInterfaces
                          PhysicsAnalysis/D3PDMaker/D3PDMakerUtils )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( D3PDMakerReaderLib
                   src/*.cxx
                   PUBLIC_HEADERS D3PDMakerReader
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel D3PDMakerUtils )

atlas_add_component( D3PDMakerReader
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel D3PDMakerUtils D3PDMakerReaderLib )

atlas_add_dictionary( D3PDMakerReaderDict
                      src/D3PDMakerReaderDict.h
                      D3PDMakerReader/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel D3PDMakerUtils D3PDMakerReaderLib )

atlas_add_executable( d3pdReaderMaker
                      src/apps/d3pdReaderMaker.cxx
                      src/CodeGenerator_v2.cxx
                      src/RootObjectMetadata.cxx
                      src/isPrimitive.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel D3PDMakerUtils D3PDMakerReaderLib )

atlas_add_executable( d3pdReadersFromFile
                      src/apps/d3pdReadersFromFile.cxx
                      src/CodeGenerator_v2.cxx
                      src/RootObjectMetadata.cxx
                      src/isPrimitive.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel D3PDMakerUtils D3PDMakerReaderLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( share/d3pdReaderLibraryMaker.py share/d3pdReaderRootCoreMaker.py share/d3pdReaderSFrameMaker.py )

