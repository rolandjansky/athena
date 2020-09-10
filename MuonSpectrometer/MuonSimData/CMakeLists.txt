################################################################################
# Package: MuonSimData
################################################################################

# Declare the package name:
atlas_subdir( MuonSimData )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonSimData
                   src/*.cxx
                   PUBLIC_HEADERS MuonSimData
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthenaKernel GeoPrimitives Identifier GeneratorObjects )

atlas_add_dictionary( MuonSimDataDict
                      MuonSimData/MuonSimDataDict.h
                      MuonSimData/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthenaKernel GeoPrimitives Identifier GeneratorObjects MuonSimData )

