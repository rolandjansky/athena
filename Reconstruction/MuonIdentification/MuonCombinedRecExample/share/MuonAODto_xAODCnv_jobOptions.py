# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from MuonCombinedRecExample.MuonCombinedKeys import MuonCombinedKeys as MuonCbKeys
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
from RecExConfig.RecFlags import rec
from RecExConfig.ObjKeyStore   import cfgKeyStore

if (not rec.readRDO() or muonCombinedRecFlags.doAOD()) and not muonCombinedRecFlags.doxAOD():

  from RecExConfig.ObjKeyStore                  import cfgKeyStore

  #SA
  if not cfgKeyStore.isInInput ('xAOD::TrackParticleContainer', MuonCbKeys.ExtrapolatedMSParticles()):
    from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
    alg1 = xAODMaker__TrackParticleCnvAlg("TrackParticleCnvAlg_ExtrapolatedMS")
    alg1.AODContainerName = 'ExtrapolatedMuonSpectrometerParticles' #ExtrapolatedMuonSpectrometerParticles
    alg1.xAODContainerName = MuonCbKeys.ExtrapolatedMSParticles()
    topSequence += alg1
    print "Added convertor for ",MuonCbKeys.ExtrapolatedMSParticles()
  
  # StatCombined
  if not cfgKeyStore.isInInput ('xAOD::TrackParticleContainer', MuonCbKeys.StatCombinedParticles()):
    from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
    alg3 = xAODMaker__TrackParticleCnvAlg("TrackParticleCnvAlg_StatCombined")
    alg3.AODContainerName = 'StatCombinedMuonParticles' #xStatCombinedMuonParticles
    alg3.xAODContainerName = MuonCbKeys.StatCombinedParticles()
    topSequence += alg3
  
  # Combined
  if not cfgKeyStore.isInInput ('xAOD::TrackParticleContainer', MuonCbKeys.CombinedFitParticles()):
    from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
    alg4 = xAODMaker__TrackParticleCnvAlg("TrackParticleCnvAlg_CombinedFit")
    alg4.AODContainerName = 'CombinedFitMuonParticles' #CombinedFitParticles
    alg4.xAODContainerName = MuonCbKeys.CombinedFitParticles()
    topSequence += alg4
  
  # ST
  if not cfgKeyStore.isInInput ('xAOD::TrackParticleContainer', MuonCbKeys.SegmentTagTrackParticles()):
    from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
    alg5 = xAODMaker__TrackParticleCnvAlg("TrackParticleCnvAlg_SegmentTagTrackParticles")
    alg5.AODContainerName = 'SegmentTagTrackParticles' #SegmentTagTrackParticles
    alg5.xAODContainerName = MuonCbKeys.SegmentTagTrackParticles()
    topSequence += alg5

  # MuonSegments
  if (not cfgKeyStore.isInInput ('xAOD::MuonSegmentContainer', 'MuonSegments')):
    from xAODMuonCnv.xAODMuonCnvConf import xAODMaker__MuonSegmentCnvAlg
    muonSegAlg = xAODMaker__MuonSegmentCnvAlg("MuonSegmentCnvAlg")
    topSequence += muonSegAlg
  
  #Muons
  from xAODMuonCnv.xAODMuonCnvConf import xAODMaker__MuonCnvAlg
  muonAlg = xAODMaker__MuonCnvAlg("MuonCnvAlg")
  topSequence += muonAlg
