#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

###############################################################
#
# A demonstration of inheriting data from a parent EventView
# and the use of ViewDataVerifier to allow DataFlow with
# the inherited data
#
#==============================================================

#--------------------------------------------------------------
# ATLAS default Application Configuration options
#--------------------------------------------------------------

totalViews = 3

# Configure the scheduler
from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.ShowControlFlow( True )
AlgScheduler.ShowDataDependencies( True )
AlgScheduler.EnableVerboseViews( True )

# Control flow
from AthenaCommon.AlgSequence import AthSequencer
view_1 = AthSequencer( "view_1" )
view_1.ModeOR = False
view_1.Sequential = False
view_1.StopOverride = False
view_2 = AthSequencer( "view_2" )
view_2.ModeOR = False
view_2.Sequential = False
view_2.StopOverride = False
viewSequence = AthSequencer( "viewSequence" )
viewSequence.ModeOR = False
viewSequence.Sequential = True
viewSequence.StopOverride = False

# Event-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Make views
view_make_1 = CfgMgr.AthViews__ViewSubgraphAlg("view_make_1")
view_make_1.ViewBaseName = "view_1"
view_make_1.ViewStart = "view_data_1"
view_make_1.ViewNumber = totalViews
view_make_1.AllViews = "view_collection_1"
view_make_1.ViewNodeName = view_1.name()
view_make_1.Scheduler = AlgScheduler.getScheduler()
viewSequence += view_make_1

# View 1 algorithm
view_verify_1 = CfgMgr.AthViews__ViewDataVerifier("view_verify_1")
view_verify_1.DataObjects = [ ('int','view_data_1') ]
view_1 += view_verify_1

# Add the view node to the job
viewSequence += view_1

# Make child views
view_make_2 = CfgMgr.AthViews__ViewSubgraphAlg("view_make_2")
view_make_2.ViewBaseName = "view_2"
view_make_2.ViewStart = "view_data_2"
view_make_2.ViewNumber = totalViews
view_make_2.AllViews = "view_collection_2"
view_make_2.ParentViews = "view_collection_1"
view_make_2.ViewNodeName = view_2.name()
view_make_2.Scheduler = AlgScheduler.getScheduler()
viewSequence += view_make_2

# View 2 algorithm - should find both pieces of data
view_verify_2 = CfgMgr.AthViews__ViewDataVerifier("view_verify_2")
view_verify_2.DataObjects = [ ('int','view_data_1'), ('int', 'view_data_2') ]
view_test = CfgMgr.AthViews__ViewTestAlg("view_test")
view_2 += view_verify_2
view_2 += view_test

# Add the view node to the job
viewSequence += view_2

job += viewSequence

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 10

#==============================================================
#
# End of job options file
#
###############################################################
