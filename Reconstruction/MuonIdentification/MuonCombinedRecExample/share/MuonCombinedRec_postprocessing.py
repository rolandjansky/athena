#
include.block ("MuonCombinedRecExample/MuonCombinedRec_postprocessing.py")
from AthenaCommon import CfgMgr
    
if rec.doMuonCombined() and muonCombinedRecFlags.doCosmicSplitTracks() and muonCombinedRecFlags.doAnyMuons() \
  and jobproperties.Beam.beamType() == 'cosmics':
    include ("MuonCombinedRecExample/MuonCombinedTrackSplitting_jobOptions.py")


if rec.doMuonCombined() and muonRecFlags.doCalibNtuple():
    from MuonCombinedRecExample import MuonCombinedCalibConfig

#--------------------------------------------------------------------------
# Do track truth
#--------------------------------------------------------------------------
if rec.doTruth() and muonCombinedRecFlags.doxAOD() and rec.doMuonCombined():

    from MuonTruthAlgs.MuonTruthAlgsConf import MuonDetailedTrackTruthMaker
    from TrkTruthAlgs.TrkTruthAlgsConf import TrackTruthSelector

    colsTP = [ "ExtrapolatedMuonTrackParticles", "CombinedMuonTrackParticles", "MSOnlyExtrapolatedMuonTrackParticles" ]
    fcols = [ "ExtrapolatedMuonTracks", "CombinedMuonTracks", "MSOnlyExtrapolatedMuonTracks" ]
    cols = ["MuidMETracks","MuidCombinedTracks","MSOnlyExtrapolatedMuonTracks"]
    if muonCombinedRecFlags.doMuGirl():
        cols += ["MuGirlCombinedTracks","MuGirlMETracks"]
        if muonCombinedRecFlags.doMuGirlLowBeta():
            cols += ["MuGirlStauCombinedTracks"]
    topSequence+= MuonDetailedTrackTruthMaker("MuonCombinedDetailedTrackTruthMaker")
    topSequence.MuonCombinedDetailedTrackTruthMaker.TrackCollectionNames = cols 
    topSequence.MuonCombinedDetailedTrackTruthMaker.DetailedTrackTruthNames = fcols
    from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
    topSequence.MuonCombinedDetailedTrackTruthMaker.HasCSC = MuonGeometryFlags.hasCSC()
    topSequence.MuonCombinedDetailedTrackTruthMaker.HasSTgc = MuonGeometryFlags.hasSTGC()
    topSequence.MuonCombinedDetailedTrackTruthMaker.HasMM = MuonGeometryFlags.hasMM()
        
    from TrkTruthAlgs.TrkTruthAlgsConf import TrackParticleTruthAlg
    for i in range(0, len(fcols)):
        topSequence += TrackTruthSelector(name= fcols[i] + "Selector",
                                          DetailedTrackTruthName   = fcols[i] + "DetailedTruth",
                                          OutputName               = fcols[i] + "Truth" )
        topSequence += TrackParticleTruthAlg(name = fcols[i]+"TruthAlg",
                                             TrackTruthName=fcols[i]+"Truth",
                                             TrackParticleName = colsTP[i] )
        
    from MuonTruthAlgs.MuonTruthAlgsConf import MuonTruthAssociationAlg
    topSequence += MuonTruthAssociationAlg("MuonTruthAssociationAlg")

    try:
        from PyUtils.MetaReaderPeeker import metadata
        truthStrategy = metadata['TruthStrategy']

        if truthStrategy in ['MC15','MC18','MC18LLP']:
            topSequence.MuonTruthAssociationAlg.BarcodeOffset=10000000
    except:
        print "Failed to read /Simulation/Parameters/ metadata"
        pass

if muonCombinedRecFlags.doTrackPerformance:
    include("MuonCombinedRecExample/MuonCombinedTrackPerformance_jobOptions.py")

if( muonCombinedRecFlags.createTagAndProbeInput() ):
    from MuonCombinedRecExample.TagAndProbeInputSetup import setupTagAndProbeInput
    setupTagAndProbeInput()

if( muonCombinedRecFlags.createScaleCalibrationInput() ):
    from MuonCombinedRecExample.ScaleCalibrationInputSetup import setupScaleCalibrationInput
    setupScaleCalibrationInput()
    
if rec.doMuonCombined() and rec.doJetMissingETTag() and muonCombinedRecFlags.doxAOD():
    muonTCLinkAlg = CfgMgr.ClusterMatching__CaloClusterMatchLinkAlg("MuonTCLinks",ClustersToDecorate="MuonClusterCollection")
    topSequence += muonTCLinkAlg
