################################################################################
# Package: iPatRecExample
################################################################################

# Declare the package name:
atlas_subdir( iPatRecExample )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Reconstruction/TRT_Rec
                          Reconstruction/iPat/iPatGeometryTools
                          Reconstruction/iPat/iPatQuality
                          Reconstruction/iPat/iPatRecAlgs
                          Reconstruction/iPat/iPatTrackFinder
                          Reconstruction/iPat/iPatTrackFitter
                          Reconstruction/iPat/iPatTrackFollower
                          Reconstruction/iPat/iPatTruthTrajectory
                          Tracking/TrkFitter/TrkiPatFitter
                          Tracking/TrkFitter/TrkiPatFitterUtils )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/iPatSetup.sh )

