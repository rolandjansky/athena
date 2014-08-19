###############################################################
#
# MuGirl jobOptions fragment
# authors: Shlomit Tarem, Zvi Tarem, Natalia Panikashvili
#
#==============================================================

include.block("MuGirl/MuGirl_jobOptions.py")

if not 'doMdtHough' in dir():
    doMdtHough = True
if not 'doMuGirlNTuple' in dir():
    doMuGirlNTuple = False
if not 'doMuGirlLHR' in dir():
    doMuGirlLHR = False

#include("MdtDriftCircleOnTrackCreator/MdtDriftCircleOnTrackCreator.py")
#include("MuonClusterOnTrackCreator/MuonClusterOnTrackCreator.py")

#if not 'MuonClusterOnTrackCreator' in theApp.Dlls:
#    theApp.Dlls += ["MuonClusterOnTrackCreator"]

include("MuidCaloEnergyTools/MuidCaloEnergyTool_jobOptions.py")

Service("RegSelSvc").enableMuon = True

from MuGirl.MuGirlConf import MuGirl
MuGirl = MuGirl()
MuGirl.doMdtHough  = doMdtHough
MuGirl.doNTuple = doMuGirlNTuple
MuGirl.doTruth  = doTruth and doMuGirlNTuple
MuGirl.inDetTrackCollection = "Tracks"
#MuGirl.MDTSegmentMaker = "Muon::DCMathSegmentMaker"

MuGirl.doLHR = doMuGirlLHR

#if doMuGirlLHR:
#    from TrackToCalo.ExtrapolTrackToCaloToolBase import ExtrapolTrackToCaloToolFactory
#    MuGirlExtrapolTrackToCaloTool = ExtrapolTrackToCaloToolFactory(depth="showerdefault")
#    ToolSvc += MuGirlExtrapolTrackToCaloTool
#    MuGirl.ExtrapolTrackToCaloTool = MuGirlExtrapolTrackToCaloTool
 
from MuGirlPerformanceTools.MuGirlPerformanceToolsConf import MuGirlNS__MuGirlPerformanceTruthTool
MuGirlPerformanceTruthTool = MuGirlNS__MuGirlPerformanceTruthTool()
if (DetDescrVersion.startswith("DC") or DetDescrVersion.startswith("Rome") or DetDescrVersion.startswith("ATLAS-DC3")):
   MuGirlPerformanceTruthTool.TrackRecordCollection = "MuonEntryRecord"
else:
   MuGirlPerformanceTruthTool.TrackRecordCollection = "MuonEntryLayer"
ToolSvc += MuGirlPerformanceTruthTool
MuGirl.MuGirlPerformanceTruthTool = MuGirlPerformanceTruthTool

from __main__ import topSequence
topSequence += MuGirl

