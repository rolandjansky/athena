# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Resilience import protectedInclude
protectedInclude('Digitization/ForceUseOfPileUpTools.py')
protectedInclude('SimulationJobOptions/preInlcude.PileUpBunchTrainsMC16c_2017_Config1.py')
protectedInclude('RunDependentSimData/configLumi_run300000_mc16d.py')

from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.numberOfLowPtMinBias = 80.290021063135
digitizationFlags.numberOfHighPtMinBias = 0.2099789464
digitizationFlags.initialBunchCrossing = -32
digitizationFlags.finalBunchCrossing = 6
