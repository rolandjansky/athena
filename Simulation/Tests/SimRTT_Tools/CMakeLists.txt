################################################################################
# Package: SimRTT_Tools
################################################################################

# Declare the package name:
atlas_subdir( SimRTT_Tools )

# Install files from the package:
atlas_install_runtime( share/DCubeFlexibleRunner.py share/FlexibleDiffPoolFilesRunner.py share/SimCoreFileGrepper.py )
atlas_install_scripts( scripts/parse_TruthEvent.py scripts/grabRttRootFile.py share/simCoreFileGrepperScript.py )

