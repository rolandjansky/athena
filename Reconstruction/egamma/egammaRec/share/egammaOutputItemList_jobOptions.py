# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__ = """Add containers to ESD/AOD ItemList using
the definitions from egammaKeys"""
from egammaRec import egammaKeys
from egammaRec.egammaKeys import (
    egammaKeysDict,
    getItem,
    addContainer,
    addAuxContainer)
from AthenaCommon.Logging import logging
from RecExConfig.RecFlags import rec


# List for of keys to be written to AOD.
# All egammaKeys.outputs but EgammaRec and extra clusters
AOD_outputs = [i for i, j in egammaKeysDict.outputs.items()
               if i not in ('EgammaRec',
                            'PhotonSuperRec',
                            'ElectronSuperRec',
                            'EgammaLargeClusters',
                            'EgammaLargeClustersCellLink',
                            'EgammaLargeFWDClusters',
                            'EgammaLargeFWDClustersCellLink')]


ESD_outputs = [i for i, j in egammaKeysDict.outputs.items()
               if i not in ('EgammaRec',
                            'PhotonSuperRec',
                            'ElectronSuperRec')]

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

logEgammaOutputItemList_jobOptions = logging.getLogger(
    'egammaOutputItemList_jobOptions')

logEgammaOutputItemList_jobOptions.info('egammaESDList: %s',  egammaESDList)
logEgammaOutputItemList_jobOptions.info('egammaAODList: %s',  egammaAODList)
