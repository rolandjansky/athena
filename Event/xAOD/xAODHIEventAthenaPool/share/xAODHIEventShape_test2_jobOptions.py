# $Id: xAODHIEventShape_test2_jobOptions.py 693667 2015-09-08 11:05:12Z krasznaa $

# Read in a data file holding the v2 EDM:
import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = [
    "minixAOD1.pool.root" ]

# Create an output file:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
outputStream = MSMgr.NewPoolRootStream( "StreamAOD", "minixAOD2.pool.root" )

# Copy all xAOD::HIEventShape containers to the output:
outputStream.AddItem( [ "xAOD::HIEventShapeContainer#*",
                        "xAOD::HIEventShapeAuxContainer#*" ] )

# Some additional tweaks:
theApp.EvtMax = -1
svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.defaultLimit = 1000000
