################################################################################
# Package: TrkMaterialOnTrack
################################################################################

# Declare the package name:
atlas_subdir( TrkMaterialOnTrack )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Tracking/TrkEvent/TrkEventPrimitives
                          PRIVATE
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkSurfaces )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrkMaterialOnTrack
                   src/*.cxx
                   PUBLIC_HEADERS TrkMaterialOnTrack
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES TrkEventPrimitives
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel TrkSurfaces )

atlas_add_dictionary( TrkMaterialOnTrackDict
                      TrkMaterialOnTrack/TrkMaterialOnTrackDict.h
                      TrkMaterialOnTrack/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} TrkEventPrimitives GaudiKernel TrkSurfaces TrkMaterialOnTrack )

