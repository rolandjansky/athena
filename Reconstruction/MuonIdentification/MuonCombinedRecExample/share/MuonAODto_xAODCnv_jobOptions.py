from MuonCombinedRecExample.MuonCombinedKeys import MuonCombinedKeys as MuonCbKeys
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
from RecExConfig.RecFlags import rec

if (not rec.readRDO() or muonCombinedRecFlags.doAOD()) and not muonCombinedRecFlags.doxAOD():
#SA
  from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
  alg1 = xAODMaker__TrackParticleCnvAlg("TrackParticleCnvAlg_ExtrapolatedMS")
  alg1.AODContainerName = 'ExtrapolatedMuonSpectrometerParticles' #ExtrapolatedMuonSpectrometerParticles
  alg1.xAODContainerName = MuonCbKeys.ExtrapolatedMSParticles()
  # alg1.OutputLevel = VERBOSE
  topSequence += alg1
  print "Added convertor for ",MuonCbKeys.ExtrapolatedMSParticles()
  
  # StatCombined
  from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
  alg3 = xAODMaker__TrackParticleCnvAlg("TrackParticleCnvAlg_StatCombined")
  alg3.AODContainerName = 'StatCombinedMuonParticles' #xStatCombinedMuonParticles
  alg3.xAODContainerName = MuonCbKeys.StatCombinedParticles()
  # alg3.OutputLevel = VERBOSE
  topSequence += alg3
  
  # Combined
  from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
  alg4 = xAODMaker__TrackParticleCnvAlg("TrackParticleCnvAlg_CombinedFit")
  alg4.AODContainerName = 'CombinedFitMuonParticles' #CombinedFitParticles
  alg4.xAODContainerName = MuonCbKeys.CombinedFitParticles()
  # alg4.OutputLevel = VERBOSE
  topSequence += alg4
  
  # ST
  from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
  alg5 = xAODMaker__TrackParticleCnvAlg("TrackParticleCnvAlg_SegmentTagTrackParticles")
  alg5.AODContainerName = 'SegmentTagTrackParticles' #SegmentTagTrackParticles
  alg5.xAODContainerName = MuonCbKeys.SegmentTagTrackParticles()
  # alg5.OutputLevel = VERBOSE
  topSequence += alg5

  # MuonSegments
  from xAODMuonCnv.xAODMuonCnvConf import xAODMaker__MuonSegmentCnvAlg
  muonSegAlg = xAODMaker__MuonSegmentCnvAlg("MuonSegmentCnvAlg")
  # muonAlg.OutputLevel = VERBOSE
  topSequence += muonSegAlg
  
  #Muons
  from xAODMuonCnv.xAODMuonCnvConf import xAODMaker__MuonCnvAlg
  muonAlg = xAODMaker__MuonCnvAlg("MuonCnvAlg")
  # muonAlg.OutputLevel = VERBOSE
  topSequence += muonAlg
  
