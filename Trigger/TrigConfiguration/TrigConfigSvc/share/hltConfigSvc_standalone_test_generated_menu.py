#########################################################################
#                                                                       #
#        Standalone jobO for testing the HLT Config service             #
#        and the TriggerPythonConfig  menu generator                    #

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel  = VERBOSE
MessageSvc.defaultLimit = 10000000

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.inputHLTconfigFile = 'generated_hlt_menu.xml' # here we use it

from AthenaCommon.Logging import logging
logging.getLogger( 'Unit test: ' )
from TrigConfigSvc.TrigConfigSvcConfig import SetupTrigConfigSvc

log.info('Creating the trigger configuration service wrapper')
svc = SetupTrigConfigSvc()
svc.SetStates(["xml"]);
svc.hltXmlFile = TriggerFlags.inputHLTconfigFile()
svc.InitialiseSvc();

theApp.Dlls += [ "TrigConfigSvc" ]


#######################################################
##### Generate menu similar to the hlt_menu
from TriggerMenuPython.TriggerPythonConfig import *
menu = TriggerPythonConfig('generated_hlt_menu.xml', 'generated_lvl1_menu.xml')
#menu = TriggerPythonConfig('generated_hlt_menu.xml')

# fake L1
mu6 = menu.addLvl1Threshold('MU6', 'MUON', 0,'SLOT9','CON0')\
      .addMuonThresholdValue(6, -49, 49, 0, 64)

em1 = menu.addLvl1Threshold('EM1', 'EM', 0, 'SLOT7', 'CON1')\
      .addEMThresholdValue(7, -49, 49, 0, 64,\
                           IsolationOff,IsolationOff,IsolationOff)
em18 = menu.addLvl1Threshold('EM18', 'EM', 3, 'SLOT7', 'CON1')\
       .addEMThresholdValue(18, -49, 49, 0, 64, \
                            IsolationOff,IsolationOff,IsolationOff)

cond_1mu6 = mu6.condition(1)
cond_1em1 = em1.condition(1)
cond_1em18 = em18.condition(1)


item_1em1 = LVL1MenuItem('L1_1EM1', 0x0, '1', 100, 'LOW').setLogic(cond_1em1)
item_1em20 = LVL1MenuItem('L1_EM20', 0x3, '1', 1, 'LOW').setLogic(cond_1em18)
item_2em20 = LVL1MenuItem('L1_2EM20', 0x3, '1', 1, 'LOW').setLogic(cond_1em18&cond_1em18)

menu.addLVL1Item(item_1em1)
menu.addLVL1Item(item_1em20)
menu.addLVL1Item(item_2em20)

# sequence
menu.addSequence('EM1',  ['T2CaloRec', 'T2CaloHypo'], 'em10t2calo')
menu.addSequence('em10t2calo',['T2CaloIsol', 'T2CaloIsolHypo'],  'em10t2caloi')
menu.addSequence('em10t2caloi', ['IDScan', 'IDHypo'], 'em10t2track')
menu.addSequence('em10t2track', ['IDScan', 'IDHypo'], 'em10match')

menu.addSequence('EM18',  ['T2CaloRec', 'T2CaloHypo'], 'em20t2calo')
menu.addSequence('em20t2calo',['T2CaloIsol', 'T2CaloIsolHypo'],  'em20t2caloi')
menu.addSequence('em20t2caloi', ['IDScan', 'IDHypo'], 'em20t2track')
menu.addSequence('em20t2track', ['IDScan', 'IDHypo'], 'em20match')


#chains
chain = HLTChain('em10L2', '67', 'L1_1EM1', 'L2')
chain.addTriggerTypeBit(4)
chain.addStreamTag('electrons')
chain.addGroup('EM')
chain.addHLTSignature( 'em10t2calo', sigcounter=2 ).addHLTSignature( 'em10t2caloi' ).addHLTSignature( 'em10t2track' )
menu.addHLTChain(chain)
menu.registerHLTChain('em10', chain)



chain = HLTChain('myChain', '68', 'L1_2EM20', 'L2')
chain.addTriggerTypeBit(4)
chain.addTriggerTypeBit(8)
chain.addStreamTag('electrons')
chain.addGroup(['EM', 'calib'])
chain.addStreamTag('lar', type='calibration', obeyLB='no')
chain.addHLTSignature( ['em10t2calo', 'em20t2calo'] ).addHLTSignature( ['em10t2caloi', 'em20t2caloi'] )#.addHLTSignature( ['em10t2track', 'em20t2track' ] )
chain.evaluateSignatureAfter(['em10t2calo', 'em20t2calo'], 'em10L2', 'em10t2track')
chain.evaluateSignatureAfter(['em10t2calo', 'em20t2calo'], 'em10L2_All_OK_I_want_to_see_this_warning', 'em10t2track')
chain.evaluateSignatureAfter(['em10t2calo', 'em20t2calo'], 'em10L2', 'em10t2track_All_OK_I_want_to_see_warning')


menu.addSequence(['em20t2calo']*2, ['L2ZeeFex', 'L2ZeeHypo'], 'L2Zee')
menu.addSequence(['em20track']*2, ['L2ZeeFex', 'L2ZeeHypo'], 'NextL2Zee', 'L2Zee')


menu.addHLTChain(chain)
menu.registerHLTChain('my', chain)



chain = HLTChain('em10EF', '3', 'em10L2', 'EF')
chain.addTriggerTypeBit(4)
chain.addStreamTag('electrons')
chain.addGroup(['EM', 'fresh'])
chain.addHLTSignature( 'em10track' ).addHLTSignature( 'em10match' )
chain1 = chain # remember it for binding purpose
menu.addHLTChain(chain)
menu.registerHLTChain('em10', chain)

chain = HLTChain('g10EF', '4', 'em10L2', 'EF')
chain.addTriggerTypeBit(4)
chain.addStreamTag('photons')
chain.addHLTSignature( 'g10match' )
chain.evaluateSignatureAfter( 'g10match', 'em10EF', 'em10match')
menu.addHLTChain(chain)
menu.registerHLTChain('g10', chain)

# 2 chains starting from nowhere
chain = HLTChain('g10EFunseeded', chain_counter='5', lower_chain_name='', level='L2')
chain.addTriggerTypeBit(14)
chain.addStreamTag('photonsFreeHunter')
menu.addHLTChain(chain)
menu.registerHLTChain('g10', chain)


chain = HLTChain('gL2unseeded', chain_counter='5', lower_chain_name='', level='EF')
chain.addTriggerTypeBit(156)
chain.addStreamTag('photonsFreeHunter')
menu.addHLTChain(chain)
menu.registerHLTChain('unused',chain)

chain = HLTChain('eANDgL2', chain_counter='52', lower_chain_name='', level='L2')
chain.addTriggerTypeBit(1587)
chain.addHLTSignature('em10track')
chain.addStreamTag('exotics')
chain.mergeAndAppendChains([menu.getHLTChain('myChain'), menu.getHLTChain('em10L2')])
menu.addHLTChain(chain)
menu.registerHLTChain('eAND', chain)

menu.writeConfigFiles()
#######################################################


#HLTConfigSvc.XMLMenuFile     = 'generated_hlt_menu.xml' # here we use it
#HLTConfigSvc.OutputLevel         = VERBOSE
from AthenaCommon.AppMgr import ServiceMgr

theApp.EvtMax = 0
ServiceMgr.HLTConfigSvc.OutputLevel = VERBOSE
ServiceMgr.TrigConfigSvc.OutputLevel = VERBOSE

print ServiceMgr



#                                                                       #
#      End of standalone jobO for testing the HLTConfig service         #
#                                                                       #
#########################################################################
