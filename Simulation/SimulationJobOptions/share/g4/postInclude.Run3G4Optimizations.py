##########################################################
#
# SimulationJobOptions/postInclude.Run3G4Optimizations.py
# Marilena Bandieramonte
#
# Geant4 Optimization Options for Run3
#  
##########################################################


###########################################################
#
# EMEC custom solid SLICES variant
# Use the SLICES variant of the EMEC custom solid
# both for the inner and outer wheel. 
# More info: https://its.cern.ch/jira/browse/ATLASSIM-3778
#
###########################################################

from LArGeoAlgsNV.LArGeoAlgsNVConf import LArDetectorToolNV
LArDetectorToolNV.EMECVariantInner="Slices"
LArDetectorToolNV.EMECVariantOuter="Slices"
