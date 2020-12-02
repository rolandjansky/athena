# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__ = """Add containers to ESD/AOD ItemList using
the definitions from egammaKeys"""
from egammaRec import egammaKeys
from egammaRec.egammaKeys import egammaKeysDict
from AthenaCommon.Logging import logging
from RecExConfig.RecFlags import rec


def getItem(cType, cKey):
    """getItem(cType, cKey) -> Return item to be added
    to the output list: <cType>#<cKey>"""
    return '%s#%s' % (cType, cKey)


def getAuxItem(cType, cKey, auxOptionAll='', auxOptionAOD=''):
    """getAuxItem(cType, cKey, auxOption='')
    -> <cType>#<cKey>Aux.<auxOption>"""
    auxType = cType.replace('Container', 'AuxContainer')
    auxKey = cKey + 'Aux.'
    return '%s#%s%s%s' % (auxType, auxKey, auxOptionAll, auxOptionAOD)


def addContainer(outputList, cType, cKey):
    """addContainer(outputList, cType, cKey)
    -> Add container to outputList"""
    # Skip containers if already in outputList
    item = getItem(cType, cKey)
    if item not in outputList:
        outputList.append(item)


def addAuxContainer(outputList, cType, cKey, auxOptionAll='', auxOptionAOD=''):
    """addAux(outputList, cType, cKey, auxOption='')
     -> Add aux container to outputList"""
    item = getAuxItem(cType, cKey, auxOptionAll, auxOptionAOD)
    if item not in outputList:
        outputList.append(item)


# List for of keys to be written to AOD.
# All egammaKeys.outputs but EgammaRec and TopoSeededCellLink

AOD_outputs = [i for i, j in egammaKeysDict.outputs.items()
               if i not in ('EgammaRec', 
                            'ElectronSuperRec', 
                            'EgammaLargeClusters',
                            'EgammaLargeClustersCellLink',
                            'CellLink',
                            'TopoSeededCellLink',
                            'EgammaLargeClusters', 
                            'EgammaLargeClustersCellLink', 
                            'Photon', 
                            'PhotonSuperRec', 
                            'FwdElectron', 
                            'FwdCluster', 
                            'FwdClusterCellLink', 
                            'Conversion'
                        )]


ESD_outputs = [i for i, j in egammaKeysDict.outputs.items()
               if i not in ('EgammaRec', 
                            'ElectronSuperRec', 
                            'EgammaLargeClusters',
                            'EgammaLargeClustersCellLink',
                            'TopoSeededCellLink',
                            'CellLink',
                            'EgammaLargeClusters', 
                            'EgammaLargeClustersCellLink', 
                            'Photon', 
                            'PhotonSuperRec', 
                            'FwdElectron', 
                            'FwdCluster', 
                            'FwdClusterCellLink', 
                            'Conversion'
                        )]

# Define egammaLRTAODList in the proper format (<type>#<key><option>),
# including aux containers
egammaLRTAODList = []
egammaLRTESDList = []
for i in AOD_outputs:
    cType, cKey, auxOptionAll, auxOptionAOD = egammaKeysDict.outputs[i]

    # Tag LRT containers
    cKey = 'LRT' + cKey

    # Skip truth if doTruth = False
    if not rec.doTruth() and 'Truth' in cKey:
        continue

    # Skip Trk::Tracks in xAOD
    if egammaKeys.outputTrackType() in cType:
        continue

    addContainer(egammaLRTAODList, cType, cKey)

    # Add aux containers for xAOD containers
    if 'xAOD::' in cType:
        addAuxContainer(egammaLRTAODList, cType, cKey, auxOptionAll, auxOptionAOD)

for i in ESD_outputs:
    cType, cKey, auxOptionAll, auxOptionAOD = egammaKeysDict.outputs[i]

    # Tag LRT containers
    cKey = 'LRT' + cKey


    # Skip truth if doTruth = False
    if not rec.doTruth() and 'Truth' in cKey:
        continue

    # Skip Trk::Tracks in xAOD
    if egammaKeys.outputTrackType() in cType:
        continue

    addContainer(egammaLRTESDList, cType, cKey)

    # Add aux containers for xAOD containers
    if 'xAOD::' in cType:
        addAuxContainer(egammaLRTESDList, cType, cKey, auxOptionAll)


# Add the non xAOD kind of  collection in the ESD
egammaLRTESDList.append(getItem(egammaKeys.outputTrackType(),
                                egammaKeys.outputTrackKey()).replace('#','#LRT'))
egammaLRTESDList.append(getItem(egammaKeys.outputTopoSeededCellLinkType(
), egammaKeys.outputTopoSeededCellLinkKey()).replace('#','#LRT'))

logEgammaLRTOutputItemList_jobOptions = logging.getLogger(
    'egammaLRTOutputItemList_jobOptions')
logEgammaLRTOutputItemList_jobOptions.info('egammaLRTESDList: %s',  egammaLRTESDList)
logEgammaLRTOutputItemList_jobOptions.info('egammaLRTAODList: %s',  egammaLRTAODList)
