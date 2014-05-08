MessageSvc.Format = "% F%52W%S%7W%R%T %0W%M"

from AthenaCommon.AlgSequence import AlgSequence
TopAlg = AlgSequence()

########################
# TrigDecTest
########################


from TrigConfigSvc.TrigConfigSvcConfig import TrigConfigSvc
log.info("setting up TrigConfigSvc:")
svc = TrigConfigSvc()

try:
    svc.SetState( 'aod' )
except:
    log.warning( 'failed to set state of TrigConfigSvc ...')
try:
    svc.InitialiseSvc()
except:
    log.warning( 'failed to activate TrigConfigSvc ...')


from TrigDecisionMaker.TrigDecisionMakerConfig import TrigDecisionTest

TrigDecTest = TrigDecisionTest()
TrigDecTest.OutputLevel = DEBUG
TrigDecTest.TrigDecisionTool.OutputLevel = VERBOSE
# NOTE: the following line is to test what happens when not getting the SG obj
# TrigDecTest.TrigDecisionTool.TrigDecisionKey = "NotWorking"

TopAlg += TrigDecTest

########################
# Load POOL support
########################

from TrigDecisionMaker.TrigDecisionMakerConfig import ReadTrigDecisionFromFile
ReadTrigDecisionFromFile("TrigDec.root", "xmlcatalog_file:Catalog1.xml")

############################
# Set output level threshold
############################

svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.defaultLimit = 100000

EventSelector.OutputLevel = DEBUG


### print the job ###
#####################
log.info("print job:")
print TopAlg

print ServiceMgr
