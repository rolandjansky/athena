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
    cols = [ "ExtrapolatedMuonTracks", "CombinedMuonTracks", "MSOnlyExtrapolatedMuonTracks" ]
    topSequence+= MuonDetailedTrackTruthMaker("MuonCombinedDetailedTrackTruthMaker",
                                              TrackCollectionNames = cols )
        
    from TrkTruthAlgs.TrkTruthAlgsConf import TrackParticleTruthAlg
    for i in range(0, len(cols)):
        topSequence += TrackTruthSelector(name= cols[i] + "Selector",
                                          DetailedTrackTruthName   = cols[i] + "Truth",
                                          OutputName               = cols[i] + "Truth" )
        topSequence += TrackParticleTruthAlg(name = cols[i]+"TruthAlg",
                                             TrackTruthName=cols[i]+"Truth",
                                             TrackParticleName = colsTP[i] )
    topSequence.CombinedMuonTracksTruthAlg.OutputLevel=VERBOSE

    from MuonTruthAlgs.MuonTruthAlgsConf import MuonTruthAssociationAlg
    topSequence += MuonTruthAssociationAlg("MuonTruthAssociationAlg")

    try:
        from RecExConfig.InputFilePeeker import inputFileSummary
        truthStrategy = inputFileSummary['metadata']['/Simulation/Parameters']['TruthStrategy']
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
    
