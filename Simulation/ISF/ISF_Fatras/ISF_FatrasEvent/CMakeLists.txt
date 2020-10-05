################################################################################
# Package: ISF_FatrasEvent
################################################################################

# Declare the package name:
atlas_subdir( ISF_FatrasEvent )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( ISF_FatrasEvent
                   src/*.cxx
                   PUBLIC_HEADERS ISF_FatrasEvent
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks AthenaKernel CxxUtils Identifier InDetPrepRawData ISF_FatrasDetDescrModel TrkPrepRawData TrkRIO_OnTrack EventContainers
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

