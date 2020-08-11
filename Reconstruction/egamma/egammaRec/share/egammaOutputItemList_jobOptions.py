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
               if i not in ('EgammaRec', 'PhotonSuperRec',
                            'ElectronSuperRec', 'TopoSeededCellLink',
                            'FwdClusterCellLink', 'EgammaLargeClusters',
                            'EgammaLargeClustersCellLink')]


ESD_outputs = [i for i, j in egammaKeysDict.outputs.items()
               if i not in ('EgammaRec', 'PhotonSuperRec',
                            'ElectronSuperRec', 'TopoSeededCellLink',
                            'FwdClusterCellLink')]

# Define egammaAODList in the proper format (<type>#<key><option>),
# including aux containers
egammaAODList = []
egammaESDList = []
for i in AOD_outputs:
    cType, cKey, auxOptionAll, auxOptionAOD = egammaKeysDict.outputs[i]

    # Skip truth if doTruth = False
    if not rec.doTruth() and 'Truth' in cKey:
        continue

    # Skip Trk::Tracks in xAOD
    if egammaKeys.outputTrackType() in cType:
        continue

    addContainer(egammaAODList, cType, cKey)

    # Add aux containers for xAOD containers
    if 'xAOD::' in cType:
        addAuxContainer(egammaAODList, cType, cKey, auxOptionAll, auxOptionAOD)

for i in ESD_outputs:
    cType, cKey, auxOptionAll, auxOptionAOD = egammaKeysDict.outputs[i]

    # Skip truth if doTruth = False
    if not rec.doTruth() and 'Truth' in cKey:
        continue

    # Skip Trk::Tracks in xAOD
    if egammaKeys.outputTrackType() in cType:
        continue

    addContainer(egammaESDList, cType, cKey)

    # Add aux containers for xAOD containers
    if 'xAOD::' in cType:
        addAuxContainer(egammaESDList, cType, cKey, auxOptionAll)


# Add the non xAOD kind of  collection in the ESD
egammaESDList.append(getItem(egammaKeys.outputTrackType(),
                             egammaKeys.outputTrackKey()))
egammaESDList.append(getItem(egammaKeys.outputFwdClusterCellLinkType(
), egammaKeys.outputFwdClusterCellLinkKey()))
egammaESDList.append(getItem(egammaKeys.outputTopoSeededCellLinkType(
), egammaKeys.outputTopoSeededCellLinkKey()))

logEgammaOutputItemList_jobOptions = logging.getLogger(
    'egammaOutputItemList_jobOptions')
logEgammaOutputItemList_jobOptions.info('egammaESDList: %s',  egammaESDList)
logEgammaOutputItemList_jobOptions.info('egammaAODList: %s',  egammaAODList)
