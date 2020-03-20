##############################################################
#
# TrigFTKFastSimTruth job options file
#
# This jobOptions schedule the TrigFTKFastSimTruth algorithm
# which provides FTK_RDO_Tracks based on smeared truth particles.
# Note: This algorithm has to run before trigger simulation, in
# order for the trigger to have access to FTK tracks.
#==============================================================

from FTK_RecExample.FTKJobProperties import FTKFlags
FTKFlags.init()

if FTKFlags.doTruthSmearing:
    from PyJobTransforms.trfUtils import findFile
    pmap8L_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/raw_8LcIbl3D123.pmap')
    pmap12L_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/raw_12LiblHW3D.pmap')
    print "Using PMAP 8L:", pmap8L_path
    print "Using PMAP 12L:", pmap12L_path
    rmap_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/raw_12Libl64TmodB_3D_t13.tmap')
    print "Using RMAP:", rmap_path

    import glob
    sectors = []
    ntower = 64
    for i in range(ntower):
      sectors += glob.glob("/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/FTK/FitConstants/ftk.64tower.2016.mc16.v2/sectors_raw_12L_reg"+str(i)+".patt.bz2")


    from TrigFTKSim.TrigFTKSimConf import FTK_SGHitInput
    FTKSGInput = FTK_SGHitInput( maxEta= 3.2, minPt= 0.8*GeV)
    FTKSGInput.ReadTruthTracks = False
    FTKSGInput.DoOutFileRawHits=False
    ToolSvc += FTKSGInput


    from TrigFTKFastSimTruth.TrigFTKFastSimTruthConf import  TrigFTKSectorMatchTool
    FTKTrajectorSelector = TrigFTKSectorMatchTool()
    FTKTrajectorSelector.PMapPath8L = pmap8L_path
    FTKTrajectorSelector.PMapPath12L = pmap12L_path
    FTKTrajectorSelector.RMapPath = rmap_path
    FTKTrajectorSelector.SectorFiles = sectors
    ToolSvc+=FTKTrajectorSelector


    from TrigFTKFastSimTruth.TrigFTKFastSimTruthConf import  TrigFTKTrackBuilderTool
    FTKTruthTrackBuilder = TrigFTKTrackBuilderTool()
    FTKTruthTrackBuilder.PMapPath = pmap12L_path
    ToolSvc+=FTKTruthTrackBuilder


    from AthenaServices.AthenaServicesConf import AtRanluxGenSvc
    theFTKrandomSvc = AtRanluxGenSvc("TrigFTKFastSimTruthRandom")
    theFTKrandomSvc.EventReseeding = True
    theFTKrandomSvc.UseOldBrokenSeeding = False
    ServiceMgr += theFTKrandomSvc


    from TrigFTKFastSimTruth.TrigFTKFastSimTruthConf import TrigFTKFastSimTruth
    theTrigFTKFastSimTruth = TrigFTKFastSimTruth(OutputLevel = INFO, 
                         PMapPath_8L = pmap8L_path,
                         PMapPath_12L = pmap12L_path,
                         SectorMatchTool = FTKTrajectorSelector,
                         TrackBuilderTool  = FTKTruthTrackBuilder)
    theTrigFTKFastSimTruth.IBLMode = 2
    theTrigFTKFastSimTruth.HitInputTool = FTKSGInput
    theTrigFTKFastSimTruth.Clustering = True
    theTrigFTKFastSimTruth.PixelClusteringMode = 101
    topSequence += theTrigFTKFastSimTruth
