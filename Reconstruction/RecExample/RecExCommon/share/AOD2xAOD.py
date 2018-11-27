include.block("RecExCommon/AOD2xAOD.py")

from MuonCombinedRecExample.MuonCombinedKeys import MuonCombinedKeys as MuonCbKeys

from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg


if (objKeyStore.isInInput("<McEventCollection","TruthEvent")):
    from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
    topSequence += xAODMaker__xAODTruthCnvAlg("GEN_AOD2xAOD")

#EventInfo:
    if objKeyStore.isInInput( "EventInfo" ) and not objKeyStore.isInInput( "xAOD::EventInfo" ):
        if not hasattr( topSequence, "xAODMaker::EventInfoCnvAlg" ):
            from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
            topSequence += xAODMaker__EventInfoCnvAlg()
            pass
    else:
        if not hasattr( topSequence, "xAODMaker::EventInfoNonConstCnvAlg" ):
            topSequence += CfgMgr.xAODMaker__EventInfoNonConstCnvAlg()
            pass

#CaloClusters:
if (objKeyStore.isInInput("CaloClusterContainer")):
    from xAODCaloEventCnv.xAODCaloEventCnvConf import ClusterCreator
    topSequence+=ClusterCreator() #Converts all CaloClusterContainers it finds in SG to xAOD 
    pass

#SA
if objKeyStore.isInInput("Rec::TrackParticleContainer",MuonCbKeys.ExtrapolatedMSParticles()):
    from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
    alg1 = xAODMaker__TrackParticleCnvAlg("TrackParticleCnvAlg_ExtrapolatedMS")
    alg1.AODContainerName = MuonCbKeys.ExtrapolatedMSParticles() #ExtrapolatedMuonSpectrometerParticles
    alg1.xAODContainerName = MuonCbKeys.ExtrapolatedMSParticles()
    #alg1.OutputLevel = VERBOSE
    topSequence += alg1
    pass

# StatCombined
if objKeyStore.isInInput("Rec::TrackParticleContainer", MuonCbKeys.StatCombinedParticles()):
    from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
    alg3 = xAODMaker__TrackParticleCnvAlg("TrackParticleCnvAlg_StatCombined")
    alg3.AODContainerName = MuonCbKeys.StatCombinedParticles() #xStatCombinedMuonParticles
    alg3.xAODContainerName = MuonCbKeys.StatCombinedParticles()
    # alg3.OutputLevel = VERBOSE
    topSequence += alg3
    pass

# Combined
if objKeyStore.isInInput("Rec::TrackParticleContainer",MuonCbKeys.CombinedFitParticles()):
    from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
    alg4 = xAODMaker__TrackParticleCnvAlg("TrackParticleCnvAlg_CombinedFit")
    alg4.AODContainerName = MuonCbKeys.CombinedFitParticles() #CombinedFitParticles
    alg4.xAODContainerName = MuonCbKeys.CombinedFitParticles()
    # alg4.OutputLevel = VERBOSE
    topSequence += alg4
    pass

# ST
if objKeyStore.isInInput("Rec::TrackParticleContainer",MuonCbKeys.SegmentTagTrackParticles()):
    from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
    alg5 = xAODMaker__TrackParticleCnvAlg("TrackParticleCnvAlg_SegmentTagTrackParticles")
    alg5.AODContainerName = MuonCbKeys.SegmentTagTrackParticles() #SegmentTagTrackParticles
    alg5.xAODContainerName = MuonCbKeys.SegmentTagTrackParticles()
    # alg5.OutputLevel = VERBOSE
    topSequence += alg5
    pass
 
# MuonSegments
if objKeyStore.isInInput("Trk::SegmentCollection","MuonSegments"):
    from xAODMuonCnv.xAODMuonCnvConf import xAODMaker__MuonSegmentCnvAlg
    muonSegAlg = xAODMaker__MuonSegmentCnvAlg("MuonSegmentCnvAlg")
    # muonAlg.OutputLevel = VERBOSE
    topSequence += muonSegAlg
    pass

#Muons
if objKeyStore.isInInput("Analysis::MuonContainer","Muons"):
   from xAODMuonCnv.xAODMuonCnvConf import xAODMaker__MuonCnvAlg
   muonAlg = xAODMaker__MuonCnvAlg("MuonCnvAlg")
   # muonAlg.OutputLevel = VERBOSE
   topSequence += muonAlg
   pass
   
if rec.doTruth() and rec.readRDO():
    cols = [ MuonCbKeys.ExtrapolatedMSTracks(), 
             MuonCbKeys.CombinedFitTracks() ]
    colsTP = [ MuonCbKeys.ExtrapolatedMSParticles(), 
               MuonCbKeys.CombinedFitParticles() ]
   
    from TrkTruthAlgs.TrkTruthAlgsConf import TrackParticleTruthAlg
    for i in range(0, len(cols)):
        if objKeyStore.isInInput("Rec::TrackParticleContainer",colTP[i]) and  objKeyStore.isInInput("TrackTruthCollection",cols[i]+"Truth"):
            topSequence += TrackParticleTruthAlg(name = cols[i]+"TruthAlg",
                                                    TrackTruthName=cols[i]+"Truth",
                                                    TrackParticleName = colsTP[i] )
            pass
        pass
    pass
