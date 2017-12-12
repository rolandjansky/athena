################################################################################
# Package: ISF_FatrasEvent
################################################################################

# Declare the package name:
atlas_subdir( ISF_FatrasEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthLinks
                          Control/AthenaKernel
                          DetectorDescription/Identifier
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Simulation/ISF/ISF_Fatras/ISF_FatrasDetDescrModel
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( ISF_FatrasEvent
                   src/*.cxx
                   PUBLIC_HEADERS ISF_FatrasEvent
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks AthenaKernel Identifier InDetPrepRawData ISF_FatrasDetDescrModel TrkPrepRawData TrkRIO_OnTrack EventContainers
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

