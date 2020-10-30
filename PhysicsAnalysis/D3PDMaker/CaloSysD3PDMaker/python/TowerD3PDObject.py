# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# $Id: TowerD3PDObject.py 470396 2011-11-24 16:42:13Z krasznaa $

import CaloD3PDMaker
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject

def makeTowerD3PDObject (name, prefix, object_name='TowerD3PDObject', getter = None,
                           sgkey = None,
                           label = None):
    if sgkey == None: sgkey = 'CombinedTower'
    if label == None: label = prefix
    
    ContainerType='CaloTowerContainer'
    
    if sgkey== 'TopoTower': ContainerType='CaloTopoTowerContainer'
    
    print(" makeTowerD3PDObject: name = ", name)
    print(" makeTowerD3PDObject: prefix = ", prefix)
    print(" makeTowerD3PDObject: object_name = ", object_name)
    print(" makeTowerD3PDObject: sgkey = ", sgkey)

    if not getter:
        getter = D3PDMakerCoreComps.SGDataVectorGetterTool \
                 (name + '_Getter',
                  TypeName = ContainerType,
                  SGKey = sgkey,
                  Label = label)
        
    # create the selected cells
    from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
    return D3PDMakerCoreComps.VectorFillerTool (name,
                                                Prefix = prefix,
                                                Getter = getter,
                                                ObjectName = object_name,
                                                SaveMetadata = \
                                                D3PDMakerFlags.SaveObjectMetadata())




def getTowerD3PDObject (maker, prefix,object_name) :

    towerD3PDObject = D3PDObject (maker, prefix, object_name)

    towerD3PDObject.defineBlock (0, 'Detail0',
                                CaloD3PDMaker.TowerFillerTool,
                                SaveNCellConstituents=False
                                )
    
    towerD3PDObject.defineBlock (1, 'Detail1',
                                CaloD3PDMaker.TowerFillerTool,
                                SaveNCellConstituents=True
                                )
    return towerD3PDObject 



# All Tower cels
TowerD3PDObject    = getTowerD3PDObject(makeTowerD3PDObject,'tower_','TowerD3PDObject')


