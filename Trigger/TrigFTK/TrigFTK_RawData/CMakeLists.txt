################################################################################
# Package: TrigFTK_RawData
################################################################################

# Declare the package name:
atlas_subdir( TrigFTK_RawData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/AthContainers )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package(Eigen)

# Component(s) in the package:
atlas_add_library( TrigFTK_RawData
                   src/*.cxx
                   PUBLIC_HEADERS TrigFTK_RawData
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthContainers AthenaKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( TrigFTK_RawDataDict
                      TrigFTK_RawData/TrigFTK_RawDataDict.h
                      TrigFTK_RawData/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthContainers AthenaKernel TrigFTK_RawData
                      DATA_LINKS FTK_RawTrack FTK_RawPixelCluster FTK_RawSCT_Cluster
                      ELEMENT_LINKS FTK_RawTrackContainer )

