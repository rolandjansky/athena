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
ftkLog = logging.getLogger('FTKMatrixMerge')

# force the maximum number of events to 0
theApp.EvtMax = 0 # -1 means all events

import glob
#-----------------------------------------------------------------------------
# Algorithms
#-----------------------------------------------------------------------------
from TrigFTKBankGen.TrigFTKBankGenConf import *

FTKConstGenAlgo = FTKConstGenAlgo('FTKConstGenAlgo', OutputLevel = INFO)
FTKConstGenAlgo.merging=True
if not hasattr(runArgs, "genconst") :
    setattr(runArgs, "genconst", False)
FTKConstGenAlgo.genconst= runArgs.genconst
FTKConstGenAlgo.nbank=runArgs.NBanks
if not hasattr(runArgs, "allregions") :
    setattr(runArgs, "allregions", False)
FTKConstGenAlgo.allregion = runArgs.allregions 
if not hasattr(runArgs, "bankregion") :
    setattr(runArgs, "bankregion", 0)
FTKConstGenAlgo.region=runArgs.bankregion # if the previous option is True this line is not important
if hasattr(runArgs,'FTKMatrixFileRegEx') :
    if hasattr(runArgs,'MaxInputFiles'):
        FTKConstGenAlgo.file_path = glob.glob(runArgs.FTKMatrixFileRegEx)[:runArgs.MaxInputFiles]
    else:
        FTKConstGenAlgo.file_path = glob.glob(runArgs.FTKMatrixFileRegEx)
else :
  FTKConstGenAlgo.file_path=runArgs.inputFTKMatrixFile
FTKConstGenAlgo.OutputFileName = runArgs.outputMergedFTKMatrixFile
job += FTKConstGenAlgo
#-----------------------------------------------------------------------------
