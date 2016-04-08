################################################################################
# Package: TrigSerializeResult
################################################################################

# Declare the package name:
atlas_subdir( TrigSerializeResult )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/SGTools
                          GaudiKernel
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/DataModelRoot
                          Tools/PathResolver )

# External dependencies:
find_package( ROOT COMPONENTS RIO Core Tree MathCore Hist pthread )

# Component(s) in the package:
atlas_add_library( TrigSerializeResultLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigSerializeResult
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES SGTools GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools AthenaBaseComps AthenaKernel DataModelRoot PathResolver )

atlas_add_component( TrigSerializeResult
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} SGTools GaudiKernel TestTools AthenaBaseComps AthenaKernel DataModelRoot PathResolver TrigSerializeResultLib )

atlas_add_dictionary( TrigSerializeResultDict
                      TrigSerializeResult/TrigSerializeResultDict.h
                      TrigSerializeResult/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} SGTools GaudiKernel TestTools AthenaBaseComps AthenaKernel DataModelRoot PathResolver TrigSerializeResultLib )

atlas_add_test( remap_test
                SOURCES
                test/remap_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} SGTools GaudiKernel TestTools AthenaBaseComps AthenaKernel DataModelRoot PathResolver TrigSerializeResultLib
                EXTRA_PATTERNS ".*" )

# Install files from the package:
atlas_install_runtime( share/bs-streamerinfos*.root )

