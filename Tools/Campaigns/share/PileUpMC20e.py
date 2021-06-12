# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Resilience import protectedInclude
protectedInclude('Digitization/ForceUseOfPileUpTools.py')
protectedInclude('SimulationJobOptions/preInlcude.PileUpBunchTrainsMC16c_2017_Config1.py')
protectedInclude('RunDependentSimData/configLumi_run310000_mc20e.py')

from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.numberOfLowPtMinBias = 99.33
digitizationFlags.numberOfHighPtMinBias = 0.17
digitizationFlags.initialBunchCrossing = -32
digitizationFlags.finalBunchCrossing = 6

digitizationFlags.OldBeamSpotZSize = 42
