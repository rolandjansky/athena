# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from Digitization.DigitizationFlags import digitizationFlags
# ensure better randomisation of high-pt minbias events
digitizationFlags.HighPtMinBiasInputColOffset = -1
# ensure we do not do beam spot reweighting
digitizationFlags.OldBeamSpotZSize.set_Value_and_Lock(0)

from AthenaCommon.Resilience import protectedInclude
protectedInclude('Campaigns/MC21a.py')
protectedInclude('RunDependentSimData/configEvtNbr_sequential.py')
protectedInclude('Campaigns/PileUpMC21aSingleBeamspot.py')
