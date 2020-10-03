# Enable xAOD making:
from RecExConfig.RecFlags import rec 
rec.doWritexAOD = True
#FNAME="/afs/cern.ch/work/a/azemla/xAODDevelop/rel_2_test/test/MinBiasSliceAthenaTrigRDO/AOD.pool.root"
FNAME="AOD.pool.root"

include( "AthenaPython/iread_file.py" )

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Create a logger object:
from AthenaCommon.Logging import logging
logger = logging.getLogger( "xAODForwardCnv_jobOptions" )

# Create a POOL output file with the StoreGate contents:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolStream( "StreamAOD", "xAOD.pool.root" )

# Create some xAOD::TrigMinBias objects:
from xAODForwardCnv.xAODMBTSModuleCreator import  xAODMBTSModuleCreator
xAODMBTSModuleCreator()

from xAODForwardCnv.xAODForwardEventInfoCreator import xAODForwardEventInfoCreator
xAODForwardEventInfoCreator()

# Check what happened to the stream:
xaodStream.Print()

# Split all branches:
ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [
        "DEFAULT_SPLITLEVEL='1'" ]

# Force POOL to just simply use the StoreGate keys as branch names:
ServiceMgr.AthenaPoolCnvSvc.SubLevelBranchName = "<key>"

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 10 
ServiceMgr.MessageSvc.OutputLevel = VERBOSE
ServiceMgr.MessageSvc.defaultLimit = 1000

