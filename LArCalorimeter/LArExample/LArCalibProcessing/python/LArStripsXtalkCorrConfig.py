# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory 
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def LArStripsXtalkCorrCfg(flags,KeyList):
    result=ComponentAccumulator()
    
    #Need geometry for neighbors
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    result.merge(LArGMCfg(flags))

    theLArStripsCrossTalkCorrector = CompFactory.LArStripsCrossTalkCorrector()
    theLArStripsCrossTalkCorrector.KeyList = KeyList
    theLArStripsCrossTalkCorrector.ADCsaturation = 4095
    theLArStripsCrossTalkCorrector.NoXtalkCorr=["deadReadout","deadPhys","deadCalib","almostDead"]
    #Note: The old-style config for ramp runs does not include 'deadPhys' in the DontUseForXtalkCorr list, 
    #      while the old-style calib of delay runs does contain it. I assueme a mistake in the old config
    theLArStripsCrossTalkCorrector.DontUseForXtalkCorr=["short","peculiarCalibrationLine", "deadReadout", "deadPhys"] 
    theLArStripsCrossTalkCorrector.AcceptableDifference=25.0 #in per-cent
    #theLArStripsCrossTalkCorrector.PedestalKey="LArPedestal"
    result.addEventAlgo(theLArStripsCrossTalkCorrector)
    
    return result
