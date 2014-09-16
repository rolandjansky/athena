# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = "egammaKeys.py"
__author__ = "Bruno Lenzi"

# Temporary solution just to emulate what was being done in egammaGetter.py
from InDetRecExample.InDetKeys import InDetKeys

# type, key, option for aux container
inputs = dict(
  Cluster  =      [ "xAOD::CaloClusterContainer_v1", "LArClusterEM", "" ],
  TopoCluster =   [ "xAOD::CaloClusterContainer_v1", "EMTopoCluster430", "" ],
  TrackParticle = [ "xAOD::TrackParticleContainer_v1", "", "" ]
)

outputs = dict(
  Conversion =    [ "xAOD::VertexContainer_v1", "GSFConversionVertices",
                    "-vxTrackAtVertex" ],
  Cluster  =      [ "xAOD::CaloClusterContainer_v1", "egClusterCollection", "" ],
  Electron =      [ "xAOD::ElectronContainer", "ElectronCollection", "" ],
  EgammaRec =     [ "egammaRecContainer", "egammaRecCollection", "" ],
  FwdElectron =   [ "xAOD::ElectronContainer", "FwdElectrons", "" ],
  FwdCluster  =   [ "xAOD::CaloClusterContainer_v1", "LArClusterEMFrwd", "" ],
  Photon   =      [ "xAOD::PhotonContainer", "PhotonCollection", "" ],
  TrackParticle = [ "xAOD::TrackParticleContainer_v1", "GSFTrackParticles", "" ],
  Track         = [ "TrackCollection", "GSFTracks", "" ],
  Truth         = [ "xAOD::TruthParticleContainer_v1", "egammaTruthParticles", ""]
)  

inputs['TrackParticle'][1] = InDetKeys.xAODTrackParticleContainer()
outputs['CellLink'] = ["CaloClusterCellLinkContainer", outputs["Cluster"][1] + '_links', ""]

# Create methods to return the types and keys in inputs and outputs
# e.g. egammaKeys.outputElectronKey()

for i,j in inputs.iteritems():
  exec "def input%sType(): return '%s'" % (i, j[0])
  exec "def input%sKey(): return '%s'" % (i, j[1])

for i,j in outputs.iteritems():
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

# Also
#             from CaloRec.CaloRecFlags import jobproperties as caloJP
#             (ig_modname, ig_name) = \
#                    caloJP.CaloRecFlags.clusterCellGetterName().rsplit ('.', 1)
#             ig_mod = __import__ (ig_modname, fromlist = [ig_name])
#             theCaloCellGetter = getattr (ig_mod, ig_name)()

def truthParticleKey():
  return 'TruthParticle'
