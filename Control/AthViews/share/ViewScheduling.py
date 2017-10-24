#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

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

# Configure the scheduler
from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.ShowControlFlow( True )
AlgScheduler.ShowDataDependencies( True )

# Make a separate alg pool for the view algs
#from GaudiHive.GaudiHiveConf import AlgResourcePool
#viewAlgPoolName = "ViewAlgPool"
#svcMgr += AlgResourcePool( viewAlgPoolName )

# Set of view algs
from AthenaCommon.AlgSequence import AthSequencer
allViewAlgorithms = AthSequencer( "allViewAlgorithms" )
allViewAlgorithms.ModeOR = False
allViewAlgorithms.Sequential = True
allViewAlgorithms.StopOverride = False
makeViewSequence = AthSequencer( "makeViewSequence" )
makeViewSequence.ModeOR = False
makeViewSequence.Sequential = True
makeViewSequence.StopOverride = False

# Filter to stop view algs from running on whole event
#allViewAlgorithms += CfgMgr.AthPrescaler( "alwaysFail" )
#allViewAlgorithms.alwaysFail.PercentPass = 0.0

# Method to set up a view algorithm
#def DeclareViewAlgorithm( viewAlg ):
#	global svcMgr, allViewAlgorithms
	#svcMgr.ViewAlgPool.TopAlg += [ viewAlg.name() ]
#	allViewAlgorithms += viewAlg

# Event-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()


# Make views
makeViewSequence += CfgMgr.AthViews__ViewSubgraphAlg("make_alg")
makeViewSequence.make_alg.ViewBaseName = "view"
makeViewSequence.make_alg.ViewNumber = 5
#job.make_alg.AlgPoolName = viewAlgPoolName
#job.make_alg.AlgorithmNameSequence = [ "dflow_alg1", "dflow_alg2", "dflow_alg3" ] #Eventually scheduler will do this

ViewTest = CfgMgr.AthViews__ViewTestAlg("view_test")
#DeclareViewAlgorithm( ViewTest )
allViewAlgorithms += ViewTest

# View algorithms
dflow_alg1 = CfgMgr.AthViews__DFlowAlg1("dflow_alg1")
#DeclareViewAlgorithm( dflow_alg1 )
allViewAlgorithms += dflow_alg1
#
dflow_alg2 = CfgMgr.AthViews__DFlowAlg2("dflow_alg2")
#DeclareViewAlgorithm( dflow_alg2 )
allViewAlgorithms += dflow_alg2
#
dflow_alg3 = CfgMgr.AthViews__DFlowAlg3("dflow_alg3")
#DeclareViewAlgorithm( dflow_alg3 )
allViewAlgorithms += dflow_alg3


# Add the view algorithms to the job
makeViewSequence += allViewAlgorithms

# Merge views
#job += CfgMgr.AthViews__ViewMergeAlg("merge_alg")
makeViewSequence += CfgMgr.AthViews__ViewMergeAlg("merge_alg")

job += makeViewSequence

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 10

#==============================================================
#
# End of job options file
#
###############################################################

#svcMgr.StoreGateSvc.Dump = True
#svcMgr.StoreGateSvc.OutputLevel = VERBOSE
