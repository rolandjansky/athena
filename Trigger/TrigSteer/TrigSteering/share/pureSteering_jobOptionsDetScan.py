MessageSvc.Format = "% F%32W%S%7W%R%T %0W%M"

### for (python) messaging ###
from AthenaCommon.Logging import logging  # loads logger
log = logging.getLogger( 'PureSteeringJob' )


# Some global settings (typically, you *only* want to
# do this in the final options file, never in a fragment):
theApp.EvtMax = 2


# This Athena job consists of algorithms that loop over events;
# here, the (default) top sequence is used:
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()



### Create the Menu ###
#######################
from TriggerMenuPython.TriggerPythonConfig import *

menu = TriggerPythonConfig('pureSteering_menu_DetScan.xml', 'l1.xml' )
# L1 necessary
menu.addItem('MU20', prescale='0', ctpid='122')
menu.addMuonThreshold('MU20', value='6')
from TrigSteeringTest.TrigSteeringTestConf import PESA__dummyAlgo, PESA__newDummyAlgo2To1, PESA__newDummyAlgoAllTEs, PESA__newDummyAlgoScan

# L2 Sequences
Muon = PESA__dummyAlgo('Muon')
Muon2 = PESA__dummyAlgo('Muon2')

menu.addSequence("MU20" , [Muon, Muon2 ] ,"mu20" )
menu.addSequence("mu20" ,"PESA::dummyAlgo/MuonAdv" ,"mu20'" )      # both are equivalent
menu.addSequence("mu20'" ,"PESA::dummyAlgo/MuonMoore" ,"muon20" )
menu.addSequence("muon20" ,"PESA::dummyAlgo/xuxMuon" ,"muon20'" )

muonDetScan = PESA__newDummyAlgoScan('muonDetScan')
menu.addSequence("", muonDetScan, "lowPtMuonCandidate") # Unseeded sequence !!!
menu.addSequence("lowPtMuonCandidate" ,["PESA::dummyAlgo/lowPtMuonTracker","PESA::dummyAlgo/lowPtMuonTrackerHypo"] ,"lowPtMuon" )

# chains
chain = HLTChain( chain_name="mu20_L2", chain_counter="1", lower_chain_name="MU20", level="L2", prescale="1", pass_through="0")
chain.addHLTSignature( "mu20" ).addHLTSignature( "mu20'" ).addHLTSignature( "muon20" ).addHLTSignature( "muon20'" )
chain.addTriggerTypeBit('10')
chain.addStreamTag('muons', prescale='1')
menu.addHLTChain(chain)

chain = HLTChain( chain_name="lowPtMuons_mu20_L2", chain_counter="2", lower_chain_name="MU20", level="L2", prescale="1", pass_through="0")
chain.addHLTSignature( "mu20" ).addHLTSignature( "mu20'" ).addHLTSignature( ["muon20","lowPtMuonCandidate"] ).addHLTSignature( ["muon20'","lowPtMuon"] )
chain.addTriggerTypeBit('10')
chain.addStreamTag('muons', prescale='1')
menu.addHLTChain(chain)

# write XML file
menu.writeConfigFiles();


### L2 TopAlgorithm from configurable ###
#########################################
from TrigSteering.TestingTrigSteeringConfig import TestingTrigSteer_L2
log.info("setting up TestingTrigSteer_L2 configurable:")
hltSteer_L2 = TestingTrigSteer_L2('hltSteer_L2','pureSteering_menu_DetScan.xml')

job += hltSteer_L2




### print the job ###
#####################
log.info("print job:")
print job

### print services ###
######################
log.info("print theApp.services():")
print theApp.services()
log.info("print ServicesMgr:")
print ServiceMgr
