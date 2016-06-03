# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = "egammaKeys.py"
__author__ = "Bruno Lenzi"

# Temporary solution just to emulate what was being done in egammaGetter.py
from InDetRecExample.InDetKeys import InDetKeys

class egammaKeysDict:
  # type, key, option for aux container
  inputs = dict(
    Cluster  =      [ 'xAOD::CaloClusterContainer', 'LArClusterEM', '' ],
    TopoCluster =   [ 'xAOD::CaloClusterContainer', 'EMTopoCluster430', '' ],
    TrackParticle = [ 'xAOD::TrackParticleContainer', '', '' ]
    )
  
  outputs = dict(
    Conversion =    [ 'xAOD::VertexContainer', 'GSFConversionVertices','-vxTrackAtVertex' ],
    Cluster  =      [ 'xAOD::CaloClusterContainer', 'egammaClusters', '' ],
    TopoSeededCluster  =      [ 'xAOD::CaloClusterContainer', 'egammaTopoSeededClusters', '-CellLink' ],
    Electron =      [ 'xAOD::ElectronContainer', 'Electrons', '' ],
    EgammaRec =     [ 'egammaRecContainer', 'egammaRecCollection', '' ],
    FwdElectron =   [ 'xAOD::ElectronContainer', 'ForwardElectrons', '-CellLink' ],
    FwdCluster  =   [ 'xAOD::CaloClusterContainer', 'ForwardElectronClusters','-SisterCluster' ],
    Photon   =      [ 'xAOD::PhotonContainer', 'Photons', '' ],
    TrackParticle = [ 'xAOD::TrackParticleContainer', 'GSFTrackParticles', '-caloExtension.-cellAssociation.-bremPoints'],
    Track         = [ 'TrackCollection', 'GSFTracks', ''],
    Truth         = [ 'xAOD::TruthParticleContainer', 'egammaTruthParticles', '-caloExtension']
    )  
  inputs['TrackParticle'][1] = InDetKeys.xAODTrackParticleContainer()
  outputs['CellLink'] = ['CaloClusterCellLinkContainer', outputs['Cluster'][1] + '_links', '']
  outputs['TopoSeededCellLink'] = ['CaloClusterCellLinkContainer', outputs['TopoSeededCluster'][1] + '_links', '']
  outputs['FwdClusterCellLink'] = ['CaloClusterCellLinkContainer', outputs['FwdCluster'][1] + '_links', '']
  # NB: Cell links for topo-seeded and fwd clusters will written in ESD but not in AOD
  # (see egammaOutputItemList_jobOptions.py)

# Create methods to return the types and keys in inputs and outputs
# e.g. egammaKeys.outputElectronKey()

for i,j in egammaKeysDict.inputs.items():
  exec "def input%sType(): return '%s'" % (i, j[0])
  exec "def input%sKey(): return '%s'" % (i, j[1])

for i,j in egammaKeysDict.outputs.items():
  exec "def output%sType(): return '%s'" % (i, j[0])
  exec "def output%sKey(): return '%s'" % (i, j[1])
  exec "def %sKey(): return '%s'" % (i, j[1])

del i,j

def caloCellKey():
  "Return the default key for CaloCells, depending on the job configuration"
  from egammaRec import egammaRecFlags as egRecFlags
  egammaRecFlags = egRecFlags.jobproperties.egammaRecFlags
  if egammaRecFlags.cellContainerName.statusOn:
    return egammaRecFlags.cellContainerName()
  from RecExConfig.RecFlags  import rec
  if rec.readAOD:
    return "AODCellContainer"
  else: 
    return "AllCalo"

def truthParticleKey():
  return 'TruthParticles'

def truthEventKey():
  return 'TruthEvents'
