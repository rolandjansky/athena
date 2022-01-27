# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon import SystemOfUnits as Units

def CaloTowerMakerCfg(flags):
    """ specifies CaloCell-> CaloTower for all calorimeters  
        @warning this is not standalone fragment, it needs calo cells (or entire calo reco to be prepared)
    """
    acc = ComponentAccumulator()

    lar = CompFactory.LArTowerBuilderTool(IncludedCalos= [ "LAREM","LARHEC" ])
    fcal = CompFactory.LArFCalTowerBuilderTool(MinimumEt = 0 * Units.MeV)  

    tile = CompFactory.TileTowerBuilderTool(CellContainerName = 'AllCalo',
                                            IncludedCalos = ['TILE'])

    builder = CompFactory.CaloTowerAlgorithm("CaloCombinedTowerBuilder", 
                                    TowerBuilderTools = [lar, fcal, tile],
                                    NumberOfPhiTowers=64,
                                    NumberOfEtaTowers=100,
                                    EtaMin=-5.0,
                                    EtaMax=5.0, 
                                    TowerContainerName = "CombinedTower" )

    acc.addEventAlgo(builder, primary=True)
    
    return acc