from MuonCombinedRecExample.MuonCombinedKeys import MuonCombinedKeys as MuonCbKeys
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
from RecExConfig.RecFlags import rec

if (not rec.readRDO() or muonCombinedRecFlags.doAOD()) and not muonCombinedRecFlags.doxAOD():

  from RecExConfig.ObjKeyStore                  import cfgKeyStore

  # from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
  # truthAlg = xAODMaker__xAODTruthCnvAlg("GEN_AOD2xAOD")
  # # truthAlg.OutputLevel = VERBOSE
  # topSequence += truthAlg

  # # InDet
  # from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
  # alg = xAODMaker__TrackParticleCnvAlg()
  # alg.AddTruthLink=True
  # alg.AODContainerName = InDetKeys.TrackParticles()
  # alg.xAODContainerName="InDetTrackParticles"
  # alg.AODTruthContainerName = InDetKeys.TrackParticlesTruth()
  # # alg.OutputLevel = VERBOSE
  # topSequence += alg

  # # MuGirl
  # from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
  # alg2 = xAODMaker__TrackParticleCnvAlg("TrackParticleCnvAlg_MuGirlTrackParticles")
  # alg2.AODContainerName = 'MuGirlRefittedTrackParticles'
  # alg2.xAODContainerName = MuonCbKeys.MuGirlMuons()
  # # alg2.OutputLevel = VERBOSE
  # topSequence += alg2

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
  if (not cfgKeyStore.isInInput ('xAOD::MuonSegmentContainer', 'MuonSegments')):
    from xAODMuonCnv.xAODMuonCnvConf import xAODMaker__MuonSegmentCnvAlg
    muonSegAlg = xAODMaker__MuonSegmentCnvAlg("MuonSegmentCnvAlg")
    # muonAlg.OutputLevel = VERBOSE
    topSequence += muonSegAlg
  
  #Muons
  from xAODMuonCnv.xAODMuonCnvConf import xAODMaker__MuonCnvAlg
  muonAlg = xAODMaker__MuonCnvAlg("MuonCnvAlg")
  ## For conversion from release 17, uncomment the following two lines:
  # muonAlg.DoConversionFromRel17 = True ## @@@
  # muonAlg.xAODMuGirlCombinedTrackParticleContainerName = MuonCbKeys.MuGirlMuons() ## @@@

  # muonAlg.OutputLevel = VERBOSE
  topSequence += muonAlg
  
  # # MuonTruthParticles
  # from MuonTruthAlgs.MuonTruthAlgsConf import Muon__MuonTruthDecorationAlg
  # muonTruthDecoAlg = Muon__MuonTruthDecorationAlg("MuonTruthDecorationAlg")
  # # muonTruthDecoAlg.OutputLevel = VERBOSE
  # topSequence += muonTruthDecoAlg
  
  # # Associate MuonTruthParticles with reco muons
  # from MuonTruthAlgs.MuonTruthAlgsConf import MuonTruthAssociationAlg
  # muonTruthAssocAlg = MuonTruthAssociationAlg("MuonTruthAssociationAlg")
  # muonTruthAssocAlg.AssociateWithInDetTP = True ## needed in conversion from rel 17
  # #muonTruthAssocAlg.OutputLevel = VERBOSE
  # topSequence += muonTruthAssocAlg

  # from MuonTruthAlgs.MuonTruthAlgsConf import Muon__MuonSegmentTruthAssociationAlg
  # muonTruthSegAssocAlg = Muon__MuonSegmentTruthAssociationAlg("MuonSegmentTruthAssociationAlg")
  # muonTruthSegAssocAlg.OutputLevel = VERBOSE
  # topSequence += muonTruthSegAssocAlg
