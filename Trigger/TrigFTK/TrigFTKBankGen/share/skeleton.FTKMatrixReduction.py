#-----------------------------------------------------------------------------
# Athena imports
#-----------------------------------------------------------------------------
from AthenaCommon.Constants import *
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr
#import AthenaPoolCnvSvc.ReadAthenaPool

from PerfMonComps.PerfMonFlags import jobproperties as pmjp
pmjp.PerfMonFlags.doSemiDetailedMonitoring = True # to enable monitoring

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from AthenaCommon.Logging import logging
ftkLog = logging.getLogger('FTKMatrixReduction')

# force the maximum number of events to 0
theApp.EvtMax = 0 # -1 means all events

import glob
#-----------------------------------------------------------------------------
# Algorithms
#-----------------------------------------------------------------------------
from TrigFTKBankGen.TrigFTKBankGenConf import *

FTKConstGenAlgo = FTKConstGenAlgo('FTKConstGenAlgo', OutputLevel = INFO)

if not hasattr(runArgs, "eightLayer") :
    setattr(runArgs, "eightLayer", True)
FTKConstGenAlgo.eightLayer= runArgs.eightLayer

FTKConstGenAlgo.nbank=runArgs.NBanks

if not hasattr(runArgs, "ITkMode") :
    setattr(runArgs, "ITkMode", False)
FTKConstGenAlgo.ITkMode= runArgs.ITkMode

if not hasattr(runArgs, "allregions") :
    setattr(runArgs, "allregions", False)
FTKConstGenAlgo.allregion = runArgs.allregions 

if not hasattr(runArgs, "bankregion") :
    setattr(runArgs, "bankregion", 0)
FTKConstGenAlgo.region=runArgs.bankregion # if the previous option is True this line is not important

from PyJobTransforms.trfUtils import findFile

pmap_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/' + runArgs.pmap_12L)# 12L pmap for OfflineID
#pmap_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/raw_12LiblHW.pmap')# 12L pmap for HWlikeID
print "Using PMAP 12L:", pmap_path

pmap_8L_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/' + runArgs.pmap_8L)# 8L pmap for OfflineID
#pmap_8L_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/raw_8LcIbl123.pmap')# 8L pmap for HWlikeID
print "Using PMAP 8L :", pmap_8L_path

FTKConstGenAlgo.pmap_path=pmap_path;
FTKConstGenAlgo.pmap_8L_path=pmap_8L_path;

FTKConstGenAlgo.LayerChange_path=runArgs.inputFTKMatrixFile[0]
#FTKConstGenAlgo.OutputFileName = runArgs.outputFTKGoodMatrixFile
job += FTKConstGenAlgo
#-----------------------------------------------------------------------------
