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
svcMgr.ForwardSchedulerSvc.CheckDependencies = True

# Make a separate alg pool for the view algs
# We don't actually need to do this if we attach the algs to job
# I'm hoping we can keep the mechanism for later, alternatively do the lazy init
from GaudiHive.GaudiHiveConf import AlgResourcePool
viewAlgPoolName = "ViewAlgPool"
svcMgr += AlgResourcePool( viewAlgPoolName )
#svcMgr.ViewAlgPool.TopAlg = [ "AthViews::DFlowAlg1/dflow_alg1", "AthViews::DFlowAlg2/dflow_alg2", "AthViews::DFlowAlg3/dflow_alg3" ] #algs will be instantiated with default config
svcMgr.ViewAlgPool.TopAlg = [ "dflow_alg1", "dflow_alg2", "dflow_alg3" ] #use existing instances

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

viewList = []
for viewNumber in range( 5 ):
	viewList += [ "view" + str( viewNumber ) ]

# Make views
job += CfgMgr.AthViews__ViewSubgraphAlg("make_alg")
job.make_alg.ViewNames = viewList
job.make_alg.AlgPoolName = viewAlgPoolName

# Algorithms for one view
job += CfgMgr.AthViews__DFlowAlg1("dflow_alg1")
job.dflow_alg1.RequireView = True
#
job += CfgMgr.AthViews__DFlowAlg2("dflow_alg2")
job.dflow_alg2.RequireView = True
#
job += CfgMgr.AthViews__DFlowAlg3("dflow_alg3")
job.dflow_alg3.RequireView = True

# Merge views
job += CfgMgr.AthViews__ViewMergeAlg("merge_alg")
job.merge_alg.ExtraInputs += [ ( 'int', 'all_views_done_dflow' ) ]

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 10

#==============================================================
#
# End of job options file
#
###############################################################

