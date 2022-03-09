#
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
    from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
    from MuonCombinedRecExample.MuonCombinedAlgs import GetCombinedTrackParticles
    colsTP, cols = GetCombinedTrackParticles()
    ### Schedule the truth tracks for the 
    topSequence+= MuonDetailedTrackTruthMaker("MuonCombinedDetailedTrackTruthMaker",
                                              TrackCollectionNames = cols,
                                              PRD_TruthNames =["RPC_TruthMap", "TGC_TruthMap", "MDT_TruthMap"] + 
                                                   (["CSC_TruthMap"] if MuonGeometryFlags.hasCSC() else []) + 
                                                   (["MM_TruthMap"]if MuonGeometryFlags.hasMM() else []) + 
                                                   (["STGC_TruthMap"] if MuonGeometryFlags.hasSTGC() else [])                                              )
    
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
    if InDetFlags.doR3LargeD0():
        topSequence += MuonTruthAssociationAlg("MuonTruthAssociationAlgLRT",
                                                MuonContainerName="MuonsLRT",
                                                RecoLinkName="recoMuonLinkLRT",
                                                TrackContainers = colsTP)
    if muonRecFlags.runCommissioningChain():
        topSequence += MuonTruthAssociationAlg("MuonTruthAssociationAlg_EMEO",
                                                MuonContainerName="EMEO_Muons",
                                                RecoLinkName="", # Do not decorate the truth particle
                                                TrackContainers = colsTP)
      
    if muonCombinedRecFlags.doMuGirl() and muonCombinedRecFlags.doMuGirlLowBeta():
        topSequence += MuonTruthAssociationAlg("MuonTruthAssociationAlgStau",
                                                MuonContainerName="Staus",
                                                RecoLinkName="", # Do not decorate the truth particle
                                                TrackContainers = colsTP)
        
        
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
