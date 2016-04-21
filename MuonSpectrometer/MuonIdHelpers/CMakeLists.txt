################################################################################
# Package: MuonIdHelpers
################################################################################

# Declare the package name:
atlas_subdir( MuonIdHelpers )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/CLIDSvc
                          Control/SGTools
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/IdDict
                          DetectorDescription/Identifier
                          PRIVATE
                          DetectorDescription/IdDictParser
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonIdHelpersLib
                   Root/*.cxx
                   src/*.cxx
                   PUBLIC_HEADERS MuonIdHelpers
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps SGTools AtlasDetDescr IdDict Identifier
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} IdDictParser GaudiKernel )

atlas_add_component( MuonIdHelpers
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps SGTools AtlasDetDescr IdDict Identifier IdDictParser GaudiKernel MuonIdHelpersLib )

atlas_add_dictionary( MuonIdHelpersDict
                      MuonIdHelpers/MuonIdHelpersDict.h
                      MuonIdHelpers/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps SGTools AtlasDetDescr IdDict Identifier IdDictParser GaudiKernel MuonIdHelpersLib )

atlas_add_executable( test_muon_id
                      test/test_muon_id.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps SGTools AtlasDetDescr IdDict Identifier IdDictParser GaudiKernel MuonIdHelpersLib )

