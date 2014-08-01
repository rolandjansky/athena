##########################################
# Comments
# This configuration is for debugging unexpected differences seen in Primary
# Tracking physics validation results when pile-up is added (only a cavern
# background input file should be provided on the command-line)
##########################################
from Digitization.DigitizationFlags import digitizationFlags

digitizationFlags.bunchSpacing = 900
digitizationFlags.numberOfCavern = 0
digitizationFlags.numberOfCollisions   = 0.0001
digitizationFlags.initialBunchCrossing = 0
digitizationFlags.finalBunchCrossing   = 0
digitizationFlags.numberOfBeamHalo   = 0.0001
digitizationFlags.numberOfBeamGas    = 0.0001
## h2r_postInclude='SimulationJobOptions/postOptions.VeryLowLumi_900ns_DigitConfig.py'
