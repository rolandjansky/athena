################################################################################
# Package: HGTD_Identifier
################################################################################

# Declare the package name:
atlas_subdir( HGTD_Identifier )

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
atlas_add_library( HGTD_Identifier
                   src/HGTD_ID.cxx
                   PUBLIC_HEADERS HGTD_Identifier
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES SGTools AtlasDetDescr IdDict Identifier
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} IdDictParser GaudiKernel )

atlas_add_dictionary( HGTD_IdentifierDict
                      HGTD_Identifier/HGTD_IdentifierDict.h
                      HGTD_Identifier/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} SGTools AtlasDetDescr IdDict Identifier IdDictParser GaudiKernel HGTD_Identifier )
