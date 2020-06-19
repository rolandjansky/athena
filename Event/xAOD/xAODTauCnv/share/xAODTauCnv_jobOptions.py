

# $Id: xAODTauCnv_jobOptions.py 579532 2014-01-22 12:40:33Z janus $

# Turn off Tau reconstruction manually for now...
from RecExConfig.RecFlags import rec
#rec.doTau = False

# Set up the reading of a file:
FNAME = "AOD.pool.root"
include( "AthenaPython/iread_file.py" )

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Create a POOL output file with the StoreGate contents:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolStream( "StreamAOD", "xAOD.pool.root" )

# Create the xAOD taus:
from xAODTauCnv.xAODTauConverter import xAODTauConverter
xAODTauConverter()


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
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000


#theJob.AntiKt4LCTopoJetsCnv.Enable = False
#theJob.AntiKt6LCTopoJetsCnv.Enable = False
#theJob.AntiKt4TopoEMJetsCnv.Enable = False
#theJob.AntiKt6TopoEMJetsCnv.Enable = False
