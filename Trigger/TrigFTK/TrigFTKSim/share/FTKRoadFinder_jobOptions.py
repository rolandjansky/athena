from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 100 # -1 means all events

from AthenaCommon.AlgSequence import AlgSequence
alg = AlgSequence()

#--------------------------------------------------------------
# FTK algorithm inclusions
#--------------------------------------------------------------
from AthenaCommon.AppMgr import ToolSvc 
from TrigFTKSim.TrigFTKSimConf import *
import os
import sys

# Check where where the TrigFTKSim directory is to get the configuration files
if "TestArea" in os.environ :
   TrigFTKSimDir = os.environ["TestArea"]+"/Trigger/TrigFTK/TrigFTKSim"
   DataDir = os.environ["TestArea"]+"/testdata"
else :
   print "Impossible to setup TrigFTKSimDir"
   sys.exit(1)

# Adding the roadfinder algorithm only
FTKRoadFinder = FTKRoadFinderAlgo( "FTKRoadFinderAlgo" , OutputLevel=INFO)
FTKRoadFinder.NBanks = 8
FTKRoadFinder.NSubRegions = 16
FTKRoadFinder.pmap_path = TrigFTKSimDir+"/config/map_file/raw_7L.pmap"
FTKRoadFinder.pmapunused_path = TrigFTKSimDir+"/config/map_file/raw_7L_unused.pmap"
FTKRoadFinder.rmap_path = TrigFTKSimDir+"/config/map_file/raw_7L.rmap"
FTKRoadFinder.ssmap_path = TrigFTKSimDir+"/config/ss_file/raw_48x40x36.ss"
FTKRoadFinder.ssmaptsp_path = TrigFTKSimDir+"/config/ss_file/raw_24x20x36.ss"
FTKRoadFinder.ssmapunused_path = TrigFTKSimDir+"/config/ss_file/raw_24x20x36unused.ss"
#FTKRoadFinder.badmap_path = "badModulemap_FTK.bmap" # if = "" dont use WC 
FTKRoadFinder.badmap_path_for_hit = TrigFTKSimDir+"/config/bad_modules/empty.bmap" # if = "" dont use WC 
FTKRoadFinder.UseTSPBank = True
FTKRoadFinder.DBBankLevel = 1
FTKRoadFinder.TSPSimulationLevel = 2
FTKRoadFinder.TSPMinCoverage = 0
FTKRoadFinder.MakeCache = False
FTKRoadFinder.CachePath = "bankcache.root"
FTKRoadFinder.DoRoadFile = True
FTKRoadFinder.RoadFilesDir = DataDir+"/output"
FTKRoadFinder.SaveRoads = True

FTKRoadFinder.bankpath = [DataDir+"/user.gvolpi.002641._00001.inp0_reg0_sub0_loop0.patterns_raw_7L_24x20x36_16M_reg0_sub0.patt.root"]
FTKRoadFinder.bankregion = [0]
FTKRoadFinder.bankpatterns = [6250000]

import glob
FTKRoadFinder.InputFromWrapper = True
FTKRoadFinder.WrapperFiles = glob.glob(DataDir+"/*.dat.bz2")

alg += FTKRoadFinder   # 1 alg, named "MyNewPackage"


