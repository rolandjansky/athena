# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
import traceback

from AthenaCommon.AppMgr import ToolSvc

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from egammaRec.Factories import ToolFactory, AlgFactory
from egammaRec import egammaKeys

from xAODEgammaCnv.xAODEgammaCnvConf import \
  xAODMaker__ElectronCnvTool, xAODMaker__ElectronCnvAlg, \
  xAODMaker__PhotonCnvTool, xAODMaker__PhotonCnvAlg


#Cluster Converter
from xAODCaloEventCnv.xAODCaloEventCnvConf import ClusterCreator
egClusterCnvAlg = AlgFactory(ClusterCreator, name="egCaloCluster2xAOD")

#GSF Track Particle converter
from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
GSFTrackParticleCnvAlg = AlgFactory(xAODMaker__TrackParticleCnvAlg,
                                    name = "egGSFTrackParticle2xAOD", 
                                    AODContainerName="GSFTrackParticleCandidate",
                                    xAODContainerName=egammaKeys.TrackParticleKey())

#GSF track map Converter
from xAODEgammaCnv.xAODEgammaCnvConf import xAODMaker__GSFTrackParticleMapCnvAlg
GSFTrackParticleMapCnvAlg = AlgFactory(xAODMaker__GSFTrackParticleMapCnvAlg,
                                       name = "GSFTrackParticleMap2xAOD",
                                       AODContainerName = "TrackParticleCandidate",
                                       AODContainerNameGSF = "GSFTrackParticleCandidate",
                                       xAODContainerName = egammaKeys.inputTrackParticleKey(),
                                       xAODContainerNameGSF = egammaKeys.TrackParticleKey(),
                                       TrackMapName = "GSFTrackAssociation")
                
#Vertex converter
from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__VertexCnvAlg
VertexCnvAlg = AlgFactory(xAODMaker__VertexCnvAlg,
                          name = "egVertex2xAOD", 
                          AODContainerName="AllPhotonsVxCandidates",
                          xAODContainerName=egammaKeys.ConversionKey(),
                          TPContainerName=egammaKeys.TrackParticleKey())

#Electron Converter
Electrons2xAODTool=ToolFactory(xAODMaker__ElectronCnvTool,
                               name = "Electrons2xAODTool")

ElectronCnvAlg = AlgFactory(xAODMaker__ElectronCnvAlg, 
                            name = "Electrons2xAOD",
                            CnvTool = Electrons2xAODTool,
                            xAODContainerName = egammaKeys.ElectronKey(),
                            xAODContainerFrwdName = egammaKeys.FwdElectronKey())

#Photon Converter
Photons2xAODTool = ToolFactory(xAODMaker__PhotonCnvTool, 
                               name="Photons2xAODTool")

PhotonCnvAlg = AlgFactory(xAODMaker__PhotonCnvAlg,
                          name = "Photons2xAOD", 
                          CnvTool = Photons2xAODTool,
                          xAODContainerName = egammaKeys.PhotonKey())

