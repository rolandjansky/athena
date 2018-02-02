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
allViewAlgorithms = AthSequencer( "allViewAlgorithms" )
allViewAlgorithms.ModeOR = False
allViewAlgorithms.Sequential = False
allViewAlgorithms.StopOverride = False
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
makeViewSequence.make_alg.ViewNodeName = "allViewAlgorithms"
makeViewSequence.make_alg.Scheduler = AlgScheduler.getScheduler()

# View algorithm
ViewTest = CfgMgr.AthViews__ViewTestAlg("view_test")
allViewAlgorithms += ViewTest

# Add the view algorithms to the job
makeViewSequence += allViewAlgorithms

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
