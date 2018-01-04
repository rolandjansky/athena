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

# Control flow
from AthenaCommon.AlgSequence import AthSequencer
l2ViewAlgorithms = AthSequencer( "l2ViewAlgorithms" )
l2ViewAlgorithms.ModeOR = False
l2ViewAlgorithms.Sequential = False
l2ViewAlgorithms.StopOverride = False
l1ViewAlgorithms = AthSequencer( "l1ViewAlgorithms" )
l1ViewAlgorithms.ModeOR = False
l1ViewAlgorithms.Sequential = True
l1ViewAlgorithms.StopOverride = False
makeViewSequence = AthSequencer( "makeViewSequence" )
makeViewSequence.ModeOR = False
makeViewSequence.Sequential = True
makeViewSequence.StopOverride = False

# Event-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Make views
makeViewSequence += CfgMgr.AthViews__ViewSubgraphAlg("make_alg")
makeViewSequence.make_alg.ViewBaseName = "view"
makeViewSequence.make_alg.ViewNumber = 10
makeViewSequence.make_alg.ViewNodeName = l1ViewAlgorithms.name()
makeViewSequence.make_alg.Scheduler = AlgScheduler.getScheduler()

# View algorithms
ViewTest = CfgMgr.AthViews__ViewTestAlg("view_test")
l1ViewAlgorithms += ViewTest
#
l1ViewAlgorithms += CfgMgr.AthViews__ViewSubgraphAlg("nest_alg")
l1ViewAlgorithms.nest_alg.ViewBaseName = "viewView"
l1ViewAlgorithms.nest_alg.ViewNumber = 10
l1ViewAlgorithms.nest_alg.ViewNodeName = l2ViewAlgorithms.name()
l1ViewAlgorithms.nest_alg.Scheduler = AlgScheduler.getScheduler()
#
ViewViewTest = CfgMgr.AthViews__ViewTestAlg("viewView_test")
l2ViewAlgorithms += ViewViewTest

# Add the view algorithms to the job
l1ViewAlgorithms += l2ViewAlgorithms
makeViewSequence += l1ViewAlgorithms

# Merge views
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
