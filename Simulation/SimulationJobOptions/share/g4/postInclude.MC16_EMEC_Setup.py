###########################################################
#
# EMEC custom solid WHEEL variant
# Use the WHEEL variant of the EMEC custom solid
# both for the inner and outer wheel as in MC16/MC20.
# More info: https://its.cern.ch/jira/browse/ATLASSIM-3778
#
###########################################################

from LArGeoAlgsNV.LArGeoAlgsNVConf import LArDetectorToolNV
LArDetectorToolNV.EMECVariantInner="Wheel"
LArDetectorToolNV.EMECVariantOuter="Wheel"
