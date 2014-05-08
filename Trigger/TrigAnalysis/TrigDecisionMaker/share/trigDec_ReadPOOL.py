from AthenaCommon.AlgSequence import AlgSequence
TopAlg = AlgSequence()

########################
# Generate config XML files
########################

include("pureNewSteering_menu.py")

########################
# TrigDecTest
########################

from TrigDecisionMaker.TrigDecisionMakerConfig import TrigDecisionTest
from TrigDecision.TrigDecisionConfig import TrigDecisionTool_TrigDB

TrigDecTest = TrigDecisionTest()
TrigDecTest.OutputLevel = DEBUG
TrigDecTest.TrigDecisionTool = TrigDecisionTool_TrigDB()
TrigDecTest.TrigDecisionTool.OutputLevel = INFO

TopAlg += TrigDecTest

########################
# Load POOL support
########################

from TrigDecisionMaker.TrigDecisionMakerConfig import ReadTrigDecisionFromFile
ReadTrigDecisionFromFile("HLT.root", "xmlcatalog_file:Catalog1.xml")

############################
# Set output level threshold
############################

svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.defaultLimit = 100000

EventSelector.OutputLevel = DEBUG
