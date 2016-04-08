################################################################################
# Package: InDetSimData
################################################################################

# Declare the package name:
atlas_subdir( InDetSimData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/SGTools
                          DetectorDescription/Identifier
                          Generators/GeneratorObjects )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( InDetSimData
                   src/InDetSimData.cxx
                   src/InDetSimDataCollection.cxx
                   PUBLIC_HEADERS InDetSimData
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES SGTools Identifier GeneratorObjects
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( InDetSimDataDict
                      InDetSimData/InDetSimDataDict.h
                      InDetSimData/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} SGTools Identifier GeneratorObjects InDetSimData )

