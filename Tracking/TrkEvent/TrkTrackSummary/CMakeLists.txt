################################################################################
# Package: TrkTrackSummary
################################################################################

# Declare the package name:
atlas_subdir( TrkTrackSummary )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/Identifier
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrkTrackSummary
                   src/*.cxx
                   PUBLIC_HEADERS TrkTrackSummary
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES Identifier
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel )

atlas_add_dictionary( TrkTrackSummaryDict
                      TrkTrackSummary/SummaryDict.h
                      TrkTrackSummary/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} Identifier GaudiKernel TrkTrackSummary )

