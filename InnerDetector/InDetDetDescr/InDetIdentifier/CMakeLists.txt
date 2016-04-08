################################################################################
# Package: InDetIdentifier
################################################################################

# Declare the package name:
atlas_subdir( InDetIdentifier )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
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
atlas_add_library( InDetIdentifier
                   src/PixelID.cxx
                   src/SCT_ID.cxx
                   src/TRT_ID.cxx
                   src/SiliconID.cxx
                   PUBLIC_HEADERS InDetIdentifier
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES SGTools AtlasDetDescr IdDict Identifier
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} IdDictParser GaudiKernel )

atlas_add_dictionary( InDetIdentifierDict
                      InDetIdentifier/InDetIdentifierDict.h
                      InDetIdentifier/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} SGTools AtlasDetDescr IdDict Identifier IdDictParser GaudiKernel InDetIdentifier )

atlas_add_executable( test_indet_id
                      test/test_indet_id.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} SGTools AtlasDetDescr IdDict Identifier IdDictParser GaudiKernel InDetIdentifier )

