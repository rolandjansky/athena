
# $Id: xAODTrigBjetConverter_jobOptions.py 621657 2014-10-14 11:13:03Z lidiaz $
#
# A small example of converting AOD style BjetTrig objects to xAOD style.
#

# Set up the reading of a file:
#FNAME = "test.RAW"
#include ("AthenaPython/iread_file.py")
FNAME = "AOD.pool.root"
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ FNAME ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Add the converter algorithm(s) to the job:
from xAODBTaggingCnv.xAODBTaggingCnvConf import *
alg = xAODMaker__TrigBjetCnvAlg()
alg.AODKey = "HLT_EFBjetFex"
alg.xAODKey = "TrigBjet" 
alg.OutputLevel = VERBOSE
theJob += alg

# Create an output file:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
stream = MSMgr.NewPoolStream( "StreamAOD", "xAOD.pool.root" )

# Add the MET types to it:
stream.AddItem( "xAOD::BTaggingContainer_v1#*" )
stream.AddItem( "xAOD::BTaggingAuxContainer_v1#*" )

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 10
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000
#ServiceMgr.StoreGateSvc.Dump = True
