# $Id: xAODTrigMinBiasCnv_jobOptions.py 592539 2014-04-11 10:17:29Z krasznaa $

# Enable xAOD making:
from RecExConfig.RecFlags import rec
rec.doWritexAOD = True
#FNAME="/afs/cern.ch/work/a/azemla/xAODDevelop/rel_2_test/test/MinBiasSliceAthenaTrigRDO/AOD.pool.root"
FNAME="/afs/cern.ch/user/a/azemla/workdir/xAODDevelop/DATA/mc11_7TeV.105200.T1_McAtNlo_Jimmy.merge.AOD.e835_s1272_s1274_r2730_r2780_tid541650_00/AOD.541650._000544.pool.root.1"

include( "AthenaPython/iread_file.py" )

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Create a logger object:
from AthenaCommon.Logging import logging
logger = logging.getLogger( "xAODTrigMinBiasCnv_jobOptions" )

# Create a POOL output file with the StoreGate contents:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolStream( "StreamAOD", "xAOD.pool.root" )

# Create some xAOD::TrigMinBias objects:
from xAODTrigMinBiasCnv.xAODTrigTrackCountsCreator import  xAODTrigTrackCountsCreator
xAODTrigTrackCountsCreator()

from xAODTrigMinBiasCnv.xAODTrigVertexCountsCreator import  xAODTrigVertexCountsCreator
xAODTrigVertexCountsCreator()

from xAODTrigMinBiasCnv.xAODTrigT2MbtsBitsCreator import  xAODTrigT2MbtsBitsCreator
xAODTrigT2MbtsBitsCreator()

from xAODTrigMinBiasCnv.xAODTrigSpacePointCountsCreator import  xAODTrigSpacePointCountsCreator
xAODTrigSpacePointCountsCreator()

# Check what happened to the stream:
xaodStream.Print()

# Split all branches:
ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [
    "DEFAULT_SPLITLEVEL='1'" ]

# Force POOL to just simply use the StoreGate keys as branch names:
ServiceMgr.AthenaPoolCnvSvc.SubLevelBranchName = "<key>"

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 100
ServiceMgr.MessageSvc.OutputLevel = VERBOSE
ServiceMgr.MessageSvc.defaultLimit = 1000
