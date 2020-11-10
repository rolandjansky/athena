#####################################################################
#
## @file EventSplitDecision_jo.py
## @brief
## Second part of AthenaPoolMultiTest gmake check
##<br><br>
## @author JackCranshaw (Jack.Cranshaw@cern.ch)
## $Id: EventNoSplit_jo.py,v 1.27 2009-05-20 13:34:21 cranshaw Exp $
#====================================================================

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AthSequencer
topSequence = AthSequencer("AthAlgSeq")
athOutSeq = AthSequencer("AthOutSeq")

from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODMaker__EventInfoCnvAlg
alg = xAODMaker__EventInfoCnvAlg()
topSequence += alg

from EventBookkeeperTools.CutFlowHelpers import CreateCutFlowSvc
CreateCutFlowSvc( svcName="CutFlowSvc", seq=topSequence, addMetaDataToAllOutputFiles=True )

from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 200000

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool

from AthenaCommon.AppMgr import ServiceMgr as svcMgr

from PoolSvc.PoolSvcConf import PoolSvc
svcMgr += PoolSvc()
PoolSvc = svcMgr.PoolSvc
PoolSvc.ReadCatalog = ["xmlcatalog_file:EventSplit.xml"]
 
svcMgr.EventSelector.InputCollections =  ["AthenaPoolMultiTest_StreamAll2.root"]

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# include stream transform
include( "AthenaPoolMultiTest/ExampleStreamConfig.py" )

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
# Unfiltered stream
StreamAll = AthenaPoolOutputStream( "StreamAllP", "AthenaPoolMultiTest_Propagate.root", False, noTag=False, decisionFilter="Stream" )
StreamAll.CheckNumberOfWrites = False
StreamAll.WritingTool.AttributeListKey="SimpleTag"
StreamAll.WritingTool.SaveDecisions = False
StreamAll.TakeItemsFromInput = True

#EventInfoTagBuilder.FilterString = "Stream"
#EventInfoTagBuilder.PropagateInput=True

# Add the outputstreams to the execution sequence
#athOutSeq+=StreamAll

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.debugLimit = 5000
import AthenaCommon.CfgMgr as CfgMgr
if not hasattr(svcMgr, 'DecisionSvc'): svcMgr += CfgMgr.DecisionSvc()
svcMgr.DecisionSvc.CalcStats = True
if not hasattr(svcMgr, 'ItemListSvc'): svcMgr += CfgMgr.ItemListSvc()
svcMgr.ItemListSvc.OutputLevel = DEBUG
#==============================================================
#
# End of job options file
#
###############################################################
