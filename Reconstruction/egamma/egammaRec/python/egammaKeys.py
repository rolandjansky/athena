# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

__doc__ = "egammaKeys.py"
__author__ = "Bruno Lenzi"

# Name (i.e Electron) = [
# type =type of the collection, key = name of the collection,
# option = for aux container applied in ALL cases (ESD and AOD),
# option =  for additional suppression for  AOD only]

from IsolationAlgs.IsoUpdatedTrackCones import iso_vars


class egammaKeysDict:
    inputs = dict(
    )

    ShowerShapesSuppress = '.-e033.-e011.-e333.-e335.-e337.-e377'
    PhotonisemSupress = '.-isEMLoose.-isEMTight'
    ElectronisemSupress = '.-isEMLHLoose.-isEMLHTight.-isEMLHMedium.-isEMLoose.-isEMMultiLepton.-isEMMedium.-isEMTight'
    FwdElectronisemSupress = '.-isEMTight.-isEMMedium.-isEMLoose'
    # Strip off the leading dot
    isovar_suppress = "-" + ".-".join(iso_vars())

    outputs = dict(
        Conversion=[
            'xAOD::VertexContainer',
            'GSFConversionVertices',
            '-vxTrackAtVertex.',
            ''],
        Cluster=[
            'xAOD::CaloClusterContainer',
            'egammaClusters',
            '',
            ''],
        EgammaLargeClusters=[
            'xAOD::CaloClusterContainer',
            'egamma711Clusters', '', ''],
        EgammaLargeFWDClusters=[
            'xAOD::CaloClusterContainer',
            'egamma66FWDClusters',
            '',
            ''],
        TopoSeededCluster=[
            'xAOD::CaloClusterContainer',
            'egammaTopoSeededClusters',
            '',
            '-CellLink'],
        Electron=[
            'xAOD::ElectronContainer',
            'Electrons',
            isovar_suppress,
            ShowerShapesSuppress+ElectronisemSupress],
        EgammaRec=['egammaRecContainer',
                   'egammaRecCollection',
                   '',
                   ''],
        PhotonSuperRec=[
            'egammaRecContainer',
            'PhotonSuperRecCollection', '', ''],
        ElectronSuperRec=[
            'egammaRecContainer',
            'ElectronSuperRecCollection', '', ''],
        FwdElectron=['xAOD::ElectronContainer',
                     'ForwardElectrons',
                     isovar_suppress,
                     FwdElectronisemSupress],
        FwdCluster=['xAOD::CaloClusterContainer',
                    'ForwardElectronClusters',
                    '-SisterCluster', ''],
        Photon=['xAOD::PhotonContainer',
                'Photons', isovar_suppress,
                ShowerShapesSuppress+PhotonisemSupress],
        TrackParticle=[
            'xAOD::TrackParticleContainer',
            'GSFTrackParticles',
            '-caloExtension.-cellAssociation.-perigeeExtrapEta.-perigeeExtrapPhi', ''],
        Track=['TrackCollection',
               'GSFTracks',
               '',
               ''],
        Truth=['xAOD::TruthParticleContainer',
               'egammaTruthParticles',
               '-caloExtension',
               '']
    )
    #
    outputs['CellLink'] = [
        'CaloClusterCellLinkContainer',
        outputs['Cluster'][1] + '_links', '', '']
    outputs['TopoSeededCellLink'] = [
        'CaloClusterCellLinkContainer',
        outputs['TopoSeededCluster'][1] +
        '_links',
        '', '']
    outputs['FwdClusterCellLink'] = [
        'CaloClusterCellLinkContainer',
        outputs['FwdCluster'][1] + '_links',
        '', '']
    outputs['EgammaLargeClustersCellLink'] = [
        'CaloClusterCellLinkContainer',
        outputs['EgammaLargeClusters'][1] +
        '_links',
        '', '']
    outputs['EgammaLargeFWDClustersCellLink'] = [
        'CaloClusterCellLinkContainer',
        outputs['EgammaLargeFWDClusters'][1] +
        '_links',
        '', '']
    #


# Create methods to return the types and keys in inputs and outputs
# e.g. egammaKeys.outputElectronKey()
for i, j in egammaKeysDict.inputs.items():
    exec("def input%sType(): return '%s'" % (i, j[0]))
    exec("def input%sKey(): return '%s'" % (i, j[1]))

for i, j in egammaKeysDict.outputs.items():
    exec("def output%sType(): return '%s'" % (i, j[0]))
    exec("def output%sKey(): return '%s'" % (i, j[1]))
    exec("def %sKey(): return '%s'" % (i, j[1]))
del i, j


def caloCellKey():
    "Return the default key for CaloCells, depending on the job configuration"
    from egammaRec import egammaRecFlags as egRecFlags
    egammaRecFlags = egRecFlags.jobproperties.egammaRecFlags
    if egammaRecFlags.cellContainerName.statusOn:
        return egammaRecFlags.cellContainerName()
    from RecExConfig.RecFlags import rec
    if rec.readAOD:
        return "AODCellContainer"
    else:
        return "AllCalo"


def truthParticleKey():
    return 'TruthParticles'


def truthEventKey():
    return 'TruthEvents'


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
