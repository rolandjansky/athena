################################################################################
# Package: LArIdentifier
################################################################################

# Declare the package name:
atlas_subdir( LArIdentifier )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/SGTools
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/IdDict
                          DetectorDescription/Identifier
                          PRIVATE
                          Calorimeter/CaloIdentifier
                          Control/AthenaKernel
                          Control/StoreGate
                          DetectorDescription/IdDictParser
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( LArIdentifier
                   src/*.cxx
                   PUBLIC_HEADERS LArIdentifier
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES SGTools AtlasDetDescr IdDict Identifier StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} CaloIdentifier AthenaKernel IdDictParser GaudiKernel )

atlas_add_dictionary( LArIdentifierDict
                      LArIdentifier/LArIdentifierDict.h
                      LArIdentifier/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} SGTools AtlasDetDescr IdDict Identifier CaloIdentifier AthenaKernel StoreGateLib SGtests IdDictParser GaudiKernel LArIdentifier )

atlas_add_executable( test_laronlineid
                      test/test_laronlineid.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} SGTools AtlasDetDescr IdDict Identifier CaloIdentifier AthenaKernel StoreGateLib SGtests IdDictParser GaudiKernel LArIdentifier )

atlas_add_executable( test_larhvid
                      test/test_larhvid.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} SGTools AtlasDetDescr IdDict Identifier CaloIdentifier AthenaKernel StoreGateLib SGtests IdDictParser GaudiKernel LArIdentifier )

