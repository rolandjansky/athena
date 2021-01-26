#
include.block ("MuonCombinedRecExample/MuonCombinedRec_postprocessing.py")
from AthenaCommon import CfgMgr
    
if rec.doMuonCombined() and muonCombinedRecFlags.doCosmicSplitTracks() and muonCombinedRecFlags.doAnyMuons() \
  and jobproperties.Beam.beamType() == 'cosmics':
    include ("MuonCombinedRecExample/MuonCombinedTrackSplitting_jobOptions.py")


if rec.doMuonCombined() and muonRecFlags.doCalibNtuple():
    from MuonCombinedRecExample import MuonCombinedCalibConfig

#--------------------------------------------------------------------------
# Do segment truth association
#--------------------------------------------------------------------------
if rec.doTruth() and muonCombinedRecFlags.doxAOD():
    from MuonTruthAlgs.MuonTruthAlgsConf import Muon__MuonSegmentTruthAssociationAlg
    topSequence += Muon__MuonSegmentTruthAssociationAlg("MuonSegmentTruthAssociationAlg")
    try:
        from PyUtils.MetaReaderPeeker import metadata
        truthStrategy = metadata['TruthStrategy']
        if truthStrategy in ['MC15', 'MC18', 'MC18LLP']:
            topSequence.MuonSegmentTruthAssociationAlg.BarcodeOffset = 10000000
    except:
        printfunc ("Failed to read /Simulation/Parameters/ metadata")
        pass

#--------------------------------------------------------------------------
# Do track truth
#--------------------------------------------------------------------------
if rec.doTruth() and muonCombinedRecFlags.doxAOD() and rec.doMuonCombined():

    from MuonTruthAlgs.MuonTruthAlgsConf import MuonDetailedTrackTruthMaker
    from TrkTruthAlgs.TrkTruthAlgsConf import TrackTruthSelector

    colsTP = [ "ExtrapolatedMuonTrackParticles", "CombinedMuonTrackParticles", "MSOnlyExtrapolatedMuonTrackParticles" ]
    cols = [ "ExtrapolatedMuonTracks", "CombinedMuonTracks", "MSOnlyExtrapolatedTracks" ]
    topSequence+= MuonDetailedTrackTruthMaker("MuonCombinedDetailedTrackTruthMaker")
    topSequence.MuonCombinedDetailedTrackTruthMaker.TrackCollectionNames = cols 
    from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
    topSequence.MuonCombinedDetailedTrackTruthMaker.HasCSC = MuonGeometryFlags.hasCSC()
    topSequence.MuonCombinedDetailedTrackTruthMaker.HasSTgc = MuonGeometryFlags.hasSTGC()
    topSequence.MuonCombinedDetailedTrackTruthMaker.HasMM = MuonGeometryFlags.hasMM()
        
    from TrkTruthAlgs.TrkTruthAlgsConf import TrackParticleTruthAlg
    for i in range(0, len(cols)):
        topSequence += TrackTruthSelector(name= cols[i] + "Selector",
                                          DetailedTrackTruthName   = cols[i] + "DetailedTruth",
                                          OutputName               = cols[i] + "Truth" )
        topSequence += TrackParticleTruthAlg(name = cols[i]+"TruthAlg",
                                             TrackTruthName=cols[i]+"Truth",
                                             TrackParticleName = colsTP[i] )
        
    from MuonTruthAlgs.MuonTruthAlgsConf import MuonTruthAssociationAlg
    topSequence += MuonTruthAssociationAlg("MuonTruthAssociationAlg")

    try:
        from PyUtils.MetaReaderPeeker import metadata
        truthStrategy = metadata['TruthStrategy']

        if truthStrategy in ['MC15','MC18','MC18LLP']:
            topSequence.MuonTruthAssociationAlg.BarcodeOffset=10000000
    except:
        printfunc ("Failed to read /Simulation/Parameters/ metadata")
        pass

if muonCombinedRecFlags.doTrackPerformance:
    include("MuonCombinedRecExample/MuonCombinedTrackPerformance_jobOptions.py")

if rec.doMuonCombined() and rec.doJetMissingETTag() and muonCombinedRecFlags.doxAOD():
    muonTCLinkAlg = CfgMgr.ClusterMatching__CaloClusterMatchLinkAlg("MuonTCLinks",ClustersToDecorate="MuonClusterCollection")
    topSequence += muonTCLinkAlg
