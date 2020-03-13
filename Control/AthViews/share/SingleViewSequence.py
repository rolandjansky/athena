#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

###############################################################
#
# Job options file to demonstrate encapsulating algorithms
# within a single view, and retrieving data from that view
#
# Potentially useful for conflicting HLT sequences
#
#==============================================================

# Configure the scheduler
from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.ShowControlFlow( True )
AlgScheduler.ShowDataDependencies( True )

# Show the contents of StoreGate
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.StoreGateSvc.Dump = True

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
makeViewAlg = CfgMgr.AthViews__MinimalViewAlg( "makeViewAlg" )
makeViewAlg.ViewNodeName = allViewAlgorithms.name()
makeViewAlg.Scheduler = AlgScheduler.getScheduler()
makeViewSequence += makeViewAlg

# View algorithm
viewTestAlg = CfgMgr.AthViews__ViewTestAlg( "viewTestAlg" )
viewTestAlg.Output = "TestOutput"
allViewAlgorithms += viewTestAlg

# Add the view algorithms to the job
makeViewSequence += allViewAlgorithms

# Retrieve data from the view
viewOutputAlg = CfgMgr.AthViews__AliasOutOfView( "viewOutputAlg" )
viewOutputAlg.ViewName = allViewAlgorithms.name()
viewOutputAlg.OutputLevel = DEBUG
viewOutputAlg.DataObjects = [ ( "int", "TestOutput" ) ]
makeViewSequence += viewOutputAlg

job += makeViewSequence

theApp.EvtMax = 10

