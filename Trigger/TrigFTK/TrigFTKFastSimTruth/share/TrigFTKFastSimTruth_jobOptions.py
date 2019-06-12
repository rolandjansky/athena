##############################################################
#
# DumpSp job options file
#
#==============================================================

from PerfMonComps.PerfMonFlags import jobproperties as pmon_properties
pmon_properties.PerfMonFlags.doSemiDetailedMonitoring=True
pmon_properties.PerfMonFlags.doSemiDetailedMonitoringFullPrint=True

from RecExConfig.RecFlags import rec
rec.doPerfMon.set_Value_and_Lock(False)
rec.doDetailedPerfMon.set_Value_and_Lock(False)
rec.doSemiDetailedPerfMon.set_Value_and_Lock(False)

import glob
# Helper function from transforms 

from PyJobTransforms.trfUtils import findFile
pmap8L_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/raw_8LcIbl3D123.pmap')
pmap12L_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/raw_12LiblHW3D.pmap')
print "Using PMAP 8L:", pmap8L_path
print "Using PMAP 12L:", pmap12L_path
rmap_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/raw_12Libl64TmodB_3D_t13.tmap')
print "Using RMAP:", rmap_path


from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

from TrigFTKFastSimTruth.TrigFTKFastSimTruthConf import TrigFTKFastSimTruth
from AthenaCommon.AppMgr import ToolSvc


from TrigFTKSim.TrigFTKSimConf import FTK_SGHitInput

FTKSGInput = FTK_SGHitInput( maxEta= 3.2, minPt= 0.8*GeV)
FTKSGInput.OutputLevel = VERBOSE
FTKSGInput.ReadTruthTracks = False
FTKSGInput.DoOutFileRawHits=False

ToolSvc += FTKSGInput


sectors = []
ntower = 64
for i in range(ntower):
  sectors += glob.glob("/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/FTK/FitConstants/Run2_x00_y00_Reb64_UseNominalOrigin_v2/sectors_raw_12L_reg"+str(i)+".patt.bz2")
    

from TrigFTKFastSimTruth.TrigFTKFastSimTruthConf import  TrigFTKSectorMatchTool
FTKTrajectorSelector = TrigFTKSectorMatchTool()

FTKTrajectorSelector.PMapPath8L = pmap8L_path
FTKTrajectorSelector.PMapPath12L = pmap12L_path
FTKTrajectorSelector.RMapPath = rmap_path
FTKTrajectorSelector.SectorFiles = sectors
FTKTrajectorSelector.OutputLevel=VERBOSE
ToolSvc+=FTKTrajectorSelector

###############  Truth Track Builder

from TrigFTKFastSimTruth.TrigFTKFastSimTruthConf import  TrigFTKTrackBuilderTool
FTKTruthTrackBuilder = TrigFTKTrackBuilderTool()
FTKTruthTrackBuilder.OutputLevel = INFO
FTKTruthTrackBuilder.PMapPath = pmap12L_path

FTKTruthTrackBuilder.OutputLevel=VERBOSE
ToolSvc+=FTKTruthTrackBuilder


################ Final job
theTrigFTKFastSimTruth = TrigFTKFastSimTruth(OutputLevel = INFO, 
                     PMapPath_8L = pmap8L_path,
                     PMapPath_12L = pmap12L_path,
                     SectorMatchTool = FTKTrajectorSelector,
                     TrackBuilderTool  = FTKTruthTrackBuilder)

theTrigFTKFastSimTruth.IBLMode = 2
theTrigFTKFastSimTruth.HitInputTool = FTKSGInput
theTrigFTKFastSimTruth.Clustering = True
theTrigFTKFastSimTruth.PixelClusteringMode = 101
theTrigFTKFastSimTruth.OutputLevel=VERBOSE
theJob += theTrigFTKFastSimTruth

from AthenaServices.AthenaServicesConf import AtRanluxGenSvc
theFTKrandomSvc = AtRanluxGenSvc("TrigFTKFastSimTruthRandom")
theFTKrandomSvc.EventReseeding = True
theFTKrandomSvc.UseOldBrokenSeeding = False
ServiceMgr += theFTKrandomSvc


print theJob
###############################################################
