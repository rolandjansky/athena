################################################################################
# Package: InDetIdentifier
################################################################################

# Declare the package name:
atlas_subdir( InDetIdentifier )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/IdDict
                          DetectorDescription/Identifier
                          PRIVATE
                          DetectorDescription/IdDictParser
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( Boost COMPONENTS unit_test_framework )

# Component(s) in the package:
atlas_add_library( InDetIdentifier
                   src/PixelID.cxx
                   src/SCT_ID.cxx
                   src/TRT_ID.cxx
                   src/SiliconID.cxx
                   PUBLIC_HEADERS InDetIdentifier
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaKernel AtlasDetDescr IdDict Identifier
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} IdDictParser GaudiKernel )

atlas_add_dictionary( InDetIdentifierDict
                      InDetIdentifier/InDetIdentifierDict.h
                      InDetIdentifier/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AtlasDetDescr IdDict Identifier IdDictParser GaudiKernel InDetIdentifier )
                      
atlas_add_test( SCT_ID_test                
                SOURCES
                test/SCT_ID_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}  
                LINK_LIBRARIES ${Boost_LIBRARIES} AtlasDetDescr IdDict Identifier IdDictParser GaudiKernel InDetIdentifier 
                POST_EXEC_SCRIPT "nopost.sh"                
                )
set_target_properties( InDetIdentifier_SCT_ID_test  PROPERTIES ENABLE_EXPORTS True )
