# $Id: RODHeaderCopy_test1_jobOptions.py 694352 2015-09-11 12:05:29Z krasznaa $

# Read in a data file holding the v1 EDM:
import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = [
    "AOD.pool.root" ]

# Create an output file:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
outputStream = MSMgr.NewPoolRootStream( "StreamAOD", "minixAOD1.pool.root" )

# Copy all xAOD::HIEventShape containers to the output:
outputStream.AddItem( [ "xAOD::RODHeaderContainer#*",
                        "xAOD::RODHeaderAuxContainer#*" ] )

# Some additional tweaks:
theApp.EvtMax = 10
svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.defaultLimit = 1000000
