# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject

from TrigCaloD3PDMaker.TrigCaloD3PDMakerConf \
     import D3PD__TriggerTowerFillerTool, D3PD__TriggerTowerFillerTool

TriggerTowerCollection_sgkey = 'TriggerTowers'
TriggerTowerD3PDObject = make_SGDataVector_D3PDObject('DataVector<LVL1::TriggerTower>',
                                                       TriggerTowerCollection_sgkey,
                                                       'tt_',
                                                      'TriggerTowerD3PDObject')

# Level 0
TriggerTowerD3PDObject.defineBlock(0, 'Basics',
                                    D3PD__TriggerTowerFillerTool)
