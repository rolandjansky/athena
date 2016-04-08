################################################################################
# Package: TrigInDetAnalysis
################################################################################

# Declare the package name:
atlas_subdir( TrigInDetAnalysis )

# External dependencies:
find_package( ROOT COMPONENTS Cint Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_root_dictionary( TrigInDetAnalysis
                           TrigInDetAnalysisDictSource
                           ROOT_HEADERS TrigInDetAnalysis/TIDARoi.h TrigInDetAnalysis/TIDAEvent.h TrigInDetAnalysis/TIDAChain.h TrigInDetAnalysis/TIDAVertex.h TrigInDetAnalysis/Track.h TrigInDetAnalysis/TIDARoiDescriptor.h TrigInDetAnalysis/TIDARoiParameters.h TrigInDetAnalysis/TrackTrigObject.h TrigInDetAnalysis/TFileString.h TrigInDetAnalysis/TrigInDetAnalysisLinkDef.h
                           EXTERNAL_PACKAGES ROOT )

atlas_add_library( TrigInDetAnalysis
                   src/*.cxx
                   ${TrigInDetAnalysisDictSource}
                   PUBLIC_HEADERS TrigInDetAnalysis
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} )

