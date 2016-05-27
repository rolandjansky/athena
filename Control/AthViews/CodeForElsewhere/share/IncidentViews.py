###############################################################
#
# Job options file
#
# Based on AthExStoreGateExamples
#
#==============================================================

#--------------------------------------------------------------
# ATLAS default Application Configuration options
#--------------------------------------------------------------

from GaudiHive.GaudiHiveConf import ForwardSchedulerSvc
svcMgr += ForwardSchedulerSvc()
#svcMgr.ForwardSchedulerSvc.CheckDependencies = True

# Use McEventSelector so we can run with AthenaMP
import AthenaCommon.AtlasUnixGeneratorJob

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

manualViewName1 = "view1"
manualViewName2 = "view2"

#Retrieve MC event info
#from SGComps.SGCompsConf import SGInputLoader
#job += SGInputLoader(OutputLevel=INFO, ShowEventDump=False)
#job.SGInputLoader.Load = [ ('EventInfo','McEventInfo') ]

#Make views
job += CfgMgr.AthViews__ViewIncidentMakeAlg("make_alg")
#job.make_alg.RequireNotView = True
job.make_alg.ViewNames = [ manualViewName1, manualViewName2 ]
#job.make_alg.ViewNames = [ manualViewName1 ]

#Make one view
job += CfgMgr.AthViews__DFlowAlg1("dflow_alg1")
#job.dflow_alg1.EvtInfo = "McEventInfo"
job.dflow_alg1.RequireView = True
#
job += CfgMgr.AthViews__DFlowAlg2("dflow_alg2")
job.dflow_alg2.RequireView = True
#
job += CfgMgr.AthViews__DFlowAlg3("dflow_alg3")
job.dflow_alg3.RequireView = True

#Merge views
job += CfgMgr.AthViews__ViewMergeAlg("merge_alg")
#job.merge_alg.RequireNotView = True

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 10

#==============================================================
#
# End of job options file
#
###############################################################

