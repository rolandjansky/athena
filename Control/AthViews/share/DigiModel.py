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

# Make an extra StoreGate for the pileup events
import StoreGate.StoreGateConf as StoreGateConf
DigiStore = StoreGateConf.StoreGateSvc( "digi_store" )
DigiStore.OutputLevel = VERBOSE
svcMgr += DigiStore

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
DigiSetup = CfgMgr.AthViews__DigiDemoSetupAlg("digi_setup")
DigiSetup.ViewBaseName = "view"
DigiSetup.ViewNumber = 10
DigiSetup.ViewNodeName = "allViewAlgorithms"
DigiSetup.Scheduler = AlgScheduler.getScheduler()
DigiSetup.DigiStore = DigiStore
DigiSetup.DigiData = "dflow_ints"
makeViewSequence += DigiSetup

# View algorithms run transparently in the digi store events
ViewTest = CfgMgr.AthViews__ViewTestAlg("view_test")
allViewAlgorithms += ViewTest

# Add the view algorithms to the job
makeViewSequence += allViewAlgorithms

# Merge data from many digi store events
DigiMerge = CfgMgr.AthViews__ViewMergeAlg("digi_merge")
makeViewSequence += DigiMerge

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
