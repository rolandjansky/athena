# $Id: xAODMissingETCnv_jobOptions.py 586557 2014-03-06 17:56:05Z krasznaa $
#
# A small example of converting AOD style MET objects to xAOD style.
#

# Set up the reading of a file:
FNAME = "AOD.pool.root"
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ FNAME ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Add the converter algorithm(s) to the job:
from xAODMissingETCnv.xAODMissingETCnvConf import *
alg = xAODMaker__MissingETCnvAlg()
alg.AODKeys = [ "MET_RefFinal" ]
alg.xAODKey = "MET_Ref"
alg.OutputLevel = VERBOSE
theJob += alg

# Create an output file:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
stream = MSMgr.NewPoolStream( "StreamAOD", "xAOD.pool.root" )

# Add the MET types to it:
stream.AddItem( "xAOD::MissingETContainer_v1#*" )
stream.AddItem( "xAOD::MissingETAuxContainer_v1#*" )
stream.AddItem( "xAOD::MissingETComponentMap_v1#*" )
stream.AddItem( "xAOD::MissingETAuxComponentMap_v1#*" )

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 10
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000
#ServiceMgr.StoreGateSvc.Dump = True
