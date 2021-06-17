# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Resilience import protectedInclude
protectedInclude('Digitization/ForceUseOfPileUpTools.py')
protectedInclude('SimulationJobOptions/preInclude.PileUpBunchTrainsMC15_2015_25ns_Config1.py')
protectedInclude('RunDependentSimData/configLumi_run284500_mc20a.py')

from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.numberOfLowPtMinBias = 44.42397
digitizationFlags.numberOfHighPtMinBias = 0.07603015
digitizationFlags.initialBunchCrossing = -32
digitizationFlags.finalBunchCrossing = 6

digitizationFlags.OldBeamSpotZSize = 42
