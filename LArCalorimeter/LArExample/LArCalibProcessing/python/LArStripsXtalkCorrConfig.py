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
    theLArStripsCrossTalkCorrector.DontUseForXtalkCorr=["short","peculiarCalibrationLine", "deadReadout", "deadPhys"]
    theLArStripsCrossTalkCorrector.AcceptableDifference=25.0 #in per-cent
    #theLArStripsCrossTalkCorrector.PedestalKey="LArPedestal"
    result.addEventAlgo(theLArStripsCrossTalkCorrector)
    
    return result
