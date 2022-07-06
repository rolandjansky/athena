# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Resilience import protectedInclude
protectedInclude('Digitization/ForceUseOfPileUpTools.py')
protectedInclude('SimulationJobOptions/preInclude.PileUpBunchTrainsFill_7314_BCMS_Pattern_Flat.py')

if 'userRunLumiOverride' in dir():
    protectedInclude('RunDependentSimData/configLumi_muRange.py')
else:
    protectedInclude('RunDependentSimData/configLumi_run410000_mc21a_SingleBeamspot.py')

from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.numberOfLowPtMinBias = 84.335
digitizationFlags.numberOfHighPtMinBias = 0.165
digitizationFlags.initialBunchCrossing = -32
digitizationFlags.finalBunchCrossing = 6
