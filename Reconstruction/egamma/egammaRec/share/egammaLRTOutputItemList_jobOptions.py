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
        addAuxContainer(egammaLRTAODList, cType, cKey,
                        auxOptionAll, auxOptionAOD)

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
egammaLRTESDList.append(getItem(
    egammaKeys.outputTrackType(),
    egammaKeys.outputTrackKey()).replace('#', '#LRT'))
egammaLRTESDList.append(getItem(egammaKeys.outputTopoSeededCellLinkType(
), egammaKeys.outputTopoSeededCellLinkKey()).replace('#', '#LRT'))

logEgammaLRTOutputItemList_jobOptions = logging.getLogger(
    'egammaLRTOutputItemList_jobOptions')
logEgammaLRTOutputItemList_jobOptions.info(
    'egammaLRTESDList: %s',  egammaLRTESDList)
logEgammaLRTOutputItemList_jobOptions.info(
    'egammaLRTAODList: %s',  egammaLRTAODList)
