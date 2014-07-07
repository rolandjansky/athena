###########################################################################
#
# LArShape2Ntuple_POOL_jobOptions.py dumps the contet of a Wave POOL files
# to ROOT TTree.
#
# Author: Marco Delmastro <Marco.Delmastro@cern.ch>
#
# Last update: 2008-06-18
#
###########################################################################

import commands

Shape2NtupleLog = logging.getLogger( "Shape2NtupleLog" )

if not 'PoolFileName' in dir():
	Shape2NtupleLog.fatal("Please setup the input POOL file ('PoolFileName')!")
	theApp.exit(-1)

if not 'ShapeKey' in dir():
	ShapeKey = "LArShape"

if not 'RootFileName' in dir():
	RootFileName = "LArShape2Ntuple_POOL.root"

###########################################################################

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

###########################################################################

include("LArCalibProcessing/LArCalib_MinimalSetup.py")

###########################################################################

include("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")
include("LArCondAthenaPool/LArCondAthenaPool_joboptions.py")

from AthenaCommon.ConfigurableDb import getConfigurable
svcMgr += getConfigurable( "ProxyProviderSvc" )()
svcMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
svcMgr += getConfigurable( "CondProxyProvider" )()
svcMgr.CondProxyProvider.InputCollections += [ PoolFileName ]

from LArCalibTools.LArCalibToolsConf import LArShape2Ntuple

LArShape2Ntuple = LArShape2Ntuple("LArShape2Ntuple")
LArShape2Ntuple.ContainerKey = ShapeKey 	   

topSequence += LArShape2Ntuple

theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import NTupleSvc
svcMgr += NTupleSvc()
svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+RootFileName+"' OPT='NEW'" ]
   
##########################################################################

theApp.EvtMax = 1

###########################################################################

#svcMgr.MessageSvc.OutputLevel  = INFO
#svcMgr.MessageSvc.defaultLimit = 10000
#svcMgr.MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"

#svcMgr += CfgMgr.AthenaEventLoopMgr(OutputLevel = VERBOSE)

###########################################################################

#from AthenaCommon.AppMgr import theAuditorSvc
#from AthenaCommon.ConfigurableDb import getConfigurable
#theAuditorSvc += getConfigurable("MemStatAuditor")(OutputLevel = DEBUG)
#theAuditorSvc += getConfigurable("ChronoAuditor")()
#theAuditorSvc += getConfigurable("NameAuditor")()

###########################################################################
