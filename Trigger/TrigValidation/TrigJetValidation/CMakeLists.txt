################################################################################
# Package: TrigJetValidation
################################################################################

# Declare the package name:
atlas_subdir( TrigJetValidation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_runtime( test/TrigJetValidation_TestConfiguration.xml python/FileComparatorRunnerJet.py python/fileComparatorJet.py python/Diagnostics.py python/helperFunctions.py python/AtlasStyle.py python/plottingMacro.py python/clustersAndCells.py /afs/cern.ch/atlas/project/trigger/pesa-sw/validation/references/RTT/TrigJetValidation/HLTChain_ref_devval.txt /afs/cern.ch/atlas/project/trigger/pesa-sw/validation/references/RTT/TrigJetValidation/HLTChain_ref_dev.txt /afs/cern.ch/atlas/project/trigger/pesa-sw/validation/references/RTT/TrigJetValidation/HLTChain_ref_20.2.X.Y.Z-VAL.txt /afs/cern.ch/atlas/project/trigger/pesa-sw/validation/references/RTT/TrigJetValidation/HLTTE_ref_devval.txt /afs/cern.ch/atlas/project/trigger/pesa-sw/validation/references/RTT/TrigJetValidation/HLTTE_ref_dev.txt /afs/cern.ch/atlas/project/trigger/pesa-sw/validation/references/RTT/TrigJetValidation/HLTTE_ref_20.2.X.Y.Z-VAL.txt /afs/cern.ch/atlas/project/trigger/pesa-sw/validation/references/RTT/TrigJetValidation/clusters_ref_devval.txt /afs/cern.ch/atlas/project/trigger/pesa-sw/validation/references/RTT/TrigJetValidation/clusters_ref_dev.txt /afs/cern.ch/atlas/project/trigger/pesa-sw/validation/references/RTT/TrigJetValidation/clusters_ref_20.2.X.Y.Z-VAL.txt /afs/cern.ch/atlas/project/trigger/pesa-sw/validation/references/RTT/TrigJetValidation/cells_ref_devval.txt /afs/cern.ch/atlas/project/trigger/pesa-sw/validation/references/RTT/TrigJetValidation/cells_ref_dev.txt /afs/cern.ch/atlas/project/trigger/pesa-sw/validation/references/RTT/TrigJetValidation/cells_ref_20.2.X.Y.Z-VAL.txt )

