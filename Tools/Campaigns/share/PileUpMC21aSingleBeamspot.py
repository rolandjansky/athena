# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Resilience import protectedInclude
protectedInclude('Digitization/ForceUseOfPileUpTools.py')
protectedInclude('SimulationJobOptions/preInlcude.PileUpBunchTrainsMC16c_2017_Config1.py')

if 'userRunLumiOverride' in dir():
    protectedInclude('RunDependentSimData/configLumi_muRange.py')
else:
    protectedInclude('RunDependentSimData/configLumi_run330000_mc21a_SingleBeamspot.py')

from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.numberOfLowPtMinBias = 51.898
digitizationFlags.numberOfHighPtMinBias = 0.102
digitizationFlags.initialBunchCrossing = -32
digitizationFlags.finalBunchCrossing = 6
