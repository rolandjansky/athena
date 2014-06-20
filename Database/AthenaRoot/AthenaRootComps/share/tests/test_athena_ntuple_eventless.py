## get a handle on the default top-level algorithm sequence
## @author Jack Cranshaw (Jack.Cranshaw@cern.ch)
## $Id: CheckExplicit_jo.py,v 1.9 2009-05-20 13:34:21 cranshaw Exp $
###############################################################
#
# Job options file
#
#==============================================================
## basic job configuration
import AthenaCommon.AtlasUnixStandardJob

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AthSequencer
topSequence = AthSequencer("AthAlgSeq")
athOutSeq = AthSequencer("AthOutSeq")

from AthenaCommon.AppMgr import theApp

from AthenaCommon.AppMgr import ServiceMgr as svcMgr

import AthenaRootComps.ReadAthenaRoot

theApp.EvtMax = 5

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------

#svcMgr.EventSelector.InputCollections = [ "root://eosatlas//eos/atlas/user/b/binet/utests/utests/filter-d3pd/small.ntuple.0.root" ]
svcMgr.EventSelector.InputCollections = [ "root://eosatlas//eos/atlas/user/b/binet/utests/utests/filter-d3pd/ntuple.0.root"]
svcMgr.EventSelector.TupleName = "egamma"

#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------

#from AthenaPoolMultiTest.AthenaPoolMultiTestConf import *
#PassAllFilter      = PassAllFilter("PassAllFilter")
#topSequence        += PassAllFilter
#PassNoneFilter      = PassNoneFilter("PassNoneFilter")
#topSequence        += PassNoneFilter

import AthenaCommon.CfgMgr as CfgMgr
from AthenaRootComps.WriteAthenaRoot import createNtupleOutputStream
from AthenaRootComps.AthenaRootCompsConf import Athena__RootOutputStreamTool as AthenaRootOutputStreamTool
from AthenaRootComps.AthenaRootCompsConf import Athena__RootNtupleOutputMetadataTool as RootNtupleOutputMetadataTool

StreamName = "StreamNTUP"
FileName = "testNTUP.root"
tupleName = "test"
Eventless = createNtupleOutputStream( StreamName, FileName, tupleName, False)
metadataTool = RootNtupleOutputMetadataTool( StreamName+"Meta" )
metadataTool.OutputFile = FileName
metadataTool.StreamName = StreamName
metadataTool.TupleName = tupleName
theApp.serviceMgr().ToolSvc += metadataTool
Eventless.HelperTools = [metadataTool]
Eventless.ForceRead = True;
Eventless.RequireAlgs += ["PassNoneFilter"]
#Eventless.RequireAlgs += ["PassAllFilter"]

#from NTUPtoNTUPCore import NTUPUtils
#MNSMgr = NTUPUtils.MNSMgr()
#Eventless = MNSMgr.NewNTUPStream("StreamNTUP","testNTUP.root","test")
#Eventless.GetStream().ForceRead = True
#Eventless.AddRequireAlgs("PassNoneFilter")
#athOutSeq += Eventless

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = 3
svcMgr.MessageSvc.defaultLimit = 100000

#svcMgr.MetaDataSvc.OutputLevel = 2
svcMgr.EventSelector.OutputLevel = 2

#
# End of job options file
#
###############################################################
