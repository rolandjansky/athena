include.block("TrigBphysHypo/TriggerConfig_EFID.py")

include("InDetTrigRecExample/TriggerConfig_InDetEF.py")

def TriggerConfig_EFID(type,TE,slice):

    
    TE = sequenceFile.addSequence("EF", TE,
                                  TriggerConfig_SCT_Clustering("EF",type).classAndInstanceName(),
                                  "EFSCTClusters_"+slice)

    TE = sequenceFile.addSequence("EF", TE,
                                  TriggerConfig_PixelClustering("EF",type).classAndInstanceName(),
                                  "EFPixelClusters_"+slice)

    TE = sequenceFile.addSequence("EF", TE,
                                  TriggerConfig_TRT_DriftCircleMaker("EF",type).classAndInstanceName(),
                                  "EFDriftCircles_"+slice)

    TE = sequenceFile.addSequence("EF", TE,
                                  TriggerConfig_SiSpacePointFinder("EF",type).classAndInstanceName(),
                                  "EFSpPoints_"+slice)

    TE = sequenceFile.addSequence("EF", TE,
                                  TriggerConfig_SiTrackFinder("EF",type).classAndInstanceName(),
                                  "EFTrkSeeds_"+slice)

    TE = sequenceFile.addSequence("EF", TE,
                                  TriggerConfig_InDetAmbiguitySolver("EF",type).classAndInstanceName(),
                                  "EFAmbSolv_"+slice)

    TE = sequenceFile.addSequence("EF", TE,
                                  TriggerConfig_TRT_TrackExtension("EF",type).classAndInstanceName(),
                                  "EFTRTExt_"+slice)

    TE = sequenceFile.addSequence("EF", TE, TriggerConfig_InDetExtensProcessor("EF",type).classAndInstanceName(),
                                  "EFIDExtProc_"+slice)

    TE = sequenceFile.addSequence("EF", TE, TriggerConfig_InDetVxPrimary("EF",type).classAndInstanceName(),
                                  "EFPrimVert_"+slice)

    TE = sequenceFile.addSequence("EF", TE, TriggerConfig_InDetParticleCreator("EF",type).classAndInstanceName(),
                                  "EFparticle_"+slice)

