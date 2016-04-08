# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###########################################################################
# Configure L2 and EF monitoring chains
###########################################################################

from AthenaCommon.Logging import logging
log = logging.getLogger( "MonitorDef.py" )

from TriggerMenuPython.TriggerPythonConfig import TriggerPythonConfig, HLTChain, HLTSequence
from TriggerMenuPython.Lvl1Flags import Lvl1Flags
from TriggerMenuPython.HltConfig import *

from TrigDetCalib.TrigDetCalibConfig import TDAQ_L1SubDetListWriter
from TrigDetCalib.TrigDetCalibConfig import TDAQ_L2SubDetListWriter
from TrigDetCalib.TrigDetCalibConfig import TDAQ_EFSubDetListWriter

from TrigIDSCAN.TrigIDSCAN_Config import TrigIDSCAN_FullScan
from TrigSiTrack.TrigSiTrack_Config import TrigSiTrack_FullScan
#from TrigL2CosmicMuonHypo.TrigL2CosmicMuonHypo_Config import CosmicTrtHypo_Cosmic

from InDetTrigRecExample.EFInDetConfig import *
from TrigEgammaHypo.TrigEFTrackHypoConfig import EFTrackHypo_TRTOnly_FullScan, EFTrackHypo_InsideOut_FullScan 
from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo as DummyRoI
from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX

from TrigGenericAlgs.TrigGenericAlgsConf import ROBRequestAlgo
from TrigGenericAlgs.TrigGenericAlgsConf import TimeBurner

from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_FullScanA, TrigL2SiTrackFinder_FullScanB, TrigL2SiTrackFinder_FullScanC
theTrigL2SiTrackFinder_FullScanA = TrigL2SiTrackFinder_FullScanA()
theTrigL2SiTrackFinder_FullScanB = TrigL2SiTrackFinder_FullScanB()
theTrigL2SiTrackFinder_FullScanC = TrigL2SiTrackFinder_FullScanC()

try:
    from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_FullScanBC
    theTrigL2SiTrackFinder_FullScanBC = TrigL2SiTrackFinder_FullScanBC()
except:
    theTrigL2SiTrackFinder_FullScanBC = None 

try:
    from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_FullScan
    theTrigFastTrackFinder_FullScan = TrigFastTrackFinder_FullScan()
except:
    theTrigFastTrackFinder_FullScan = None


# get L2 algorithm configurables
if TriggerFlags.doLVL2() or True:
    theTDAQ_L2SubDetListWriter = TDAQ_L2SubDetListWriter()

# get EF algorithm configurables
if TriggerFlags.doEF() or True:
    theTDAQ_EFSubDetListWriter = TDAQ_EFSubDetListWriter()
    try:
        from TrigDetCalib.TrigDetCalibConfig import ScoutTestAlg
        theScoutTestAlg = ScoutTestAlg()
        theDummyUnseededAllTEAlgo = DummyRoI("ScoutingAllTEDummy")
    except:
        log.warning("Could not import ScoutTestAlg from TrigDetCalib")

class MonitorChain:

    def __init__(self, sig_id, level,
                 counter = 1022,
                 streamPrefix = 'HLT_monitor',
                 groupList = ['Supporting','Calibration','BW:Other']):

        # store variables for configuration
        self.sig_id       = sig_id # required by GenerateMenu.py
        self.level        = level
        self.counter      = counter
        self.streamPrefix = streamPrefix
        self.groupList    = groupList
        self.algos        = []
        
    def generateMenu(self, triggerPythonConfig):

       log = logging.getLogger( "MonitorDef.py" )
       log.info('Running generateMenu...')
    
       # sequence name for partial event building (peb)
       seq_peb = self.level+'_'+self.sig_id+'_peb'

       if self.level == 'L2':
           if self.sig_id.find('rate_monitor')!=-1:
               if self.sig_id.find('rejAll')!=-1:
                 alg = TDAQ_L1SubDetListWriter('TDAQ_L1SubDetListWriter_rejAll')
                 alg.rejectAll = True
                 self.algos += [alg]
                 triggerPythonConfig.addSequence([''], [alg], seq_peb)
               else:
                 alg = TDAQ_L1SubDetListWriter('TDAQ_L1SubDetListWriter')
                 self.algos += [alg]
                 triggerPythonConfig.addSequence([''], [alg], seq_peb)
           elif self.sig_id.find('cost_monitor')!=-1:
               if self.sig_id.find('rejAll')!=-1:
                 alg = TDAQ_L2SubDetListWriter('TDAQ_L2SubDetListWriter_rejAll')
                 alg.rejectAll = True
                 self.algos += [alg]
                 triggerPythonConfig.addSequence([''], [alg], seq_peb)
               else:
                 alg = TDAQ_L2SubDetListWriter('TDAQ_L2SubDetListWriter')
                 self.algos += [alg]
                 triggerPythonConfig.addSequence([''], [alg], seq_peb)
       elif self.level == 'EF':
           if self.sig_id.find('rejAll')!=-1:
             alg = TDAQ_EFSubDetListWriter('TDAQ_EFSubDetListWriter_rejAll')
             alg.rejectAll = True
             self.algos += [alg]
             triggerPythonConfig.addSequence([''], [alg], seq_peb)
           else:
             alg = TDAQ_EFSubDetListWriter('TDAQ_EFSubDetListWriter')
             self.algos += [alg]
             triggerPythonConfig.addSequence([''], [alg], seq_peb)
       else:
           log.error('Unknown level: '+self.level)
       
       # construct HLT chains used by TrigConfigSvc
       chain = HLTChain(self.level+'_'+self.sig_id, str(self.counter), '', self.level)

       # add sequence, group(s) and stream
       chain.addHLTSignature([seq_peb])
       chain.addGroup(self.groupList)
       chain.addStreamTag(self.level+'CostMonitoring', 'calibration')

       # register chain with TrigConfigSvc
       triggerPythonConfig.registerHLTChain(self.sig_id, chain)

#
# list of available chains
#
MonitorChainList = [ MonitorChain('rate_monitor','L2'),
                     MonitorChain('cost_monitor','L2'),
                     MonitorChain('cost_monitor','EF'),
                     MonitorChain('rate_monitor_rejAll','L2',1050),
                     MonitorChain('cost_monitor_rejAll','L2',1050),
                     MonitorChain('cost_monitor_rejAll','EF',1050) ]

#### --------------------------------------------

# HLT tracking monitoring chains
class L2EFChain_TRTOnly_FullScan_EF(L2EFChainDef):
    def __init__(self, sig_id,
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter,
                 l2_inputTEs, config,
                 prescale=1, pass_through=0):

        L2EFChainDef.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter,
                              l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config,
                              prescale, pass_through)

    class L2Config:
        def __init__(self, suffix): 
            self.suffix = suffix

            # dummy AllTE to create a common execution point for caching
            self.dummyRoI = DummyRoI("DummyRoI")
            self.dummyRoI.createRoIDescriptors = False
            self.dummyRoI.NumberOfOutputTEs=1

    class EFConfig:
        def __init__(self, suffix, TrigEFTrackHypo_1=None):
            self.suffix = suffix
            self.TrigEFTrackHypo_1  = TrigEFTrackHypo_1  or TrigEFTrackHypo('TrigEFTrackHypo_'+suffix)
            # dummy AllTE
            self.dummy = DummyFEX("DummyFEX_eftrk")
            
            
    class Config:
        def __init__(self, L2config, EFconfig):
            self.L2Config = L2config
            self.EFConfig = EFconfig
            
    def defineSequences(self, config):
        #L2 sequences
        self.addL2Sequence("",
                           [self.config.L2Config.dummyRoI],
                           "L2_" )
        
        #EF sequences
        self.addEFSequence(self.inputTEs,
                           [self.config.EFConfig.dummy] +
                           [config.EFConfig.TrigEFTrackHypo_1],
                           'EF_')
        
    def defineSignatures(self):
        self.addL2Signature(['L2_'])
        self.addEFSignature(['EF_'])
        
    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_':             'L2_' + self.config.L2Config.suffix,
            'EF_':             'EF_' + self.config.EFConfig.suffix,
            }
        pass
        

    def defineStreamGroupTriggerType(self):
        #self.physics_streams = ['minbias']
        #self.calib_streams = []
        self.physics_streams = ['minbias']
        self.groups = ['RATE:EFTracking','BW:Detector']
        self.trigger_type = ['hello']


Config   = L2EFChain_TRTOnly_FullScan_EF.Config
L2create = L2EFChain_TRTOnly_FullScan_EF.L2Config
EFcreate = L2EFChain_TRTOnly_FullScan_EF.EFConfig

MonitorChainList += [
    L2EFChain_TRTOnly_FullScan_EF('EFTRTOnly_FS_allL1',
                                  'L2_EFTRTOnly_FS_allL1', 5102, '','EF_EFTRTOnly_FS_allL1', 5102, ['EF_FStracks_TRTOnly'],
                                  Config(L2create('EFTRTOnly_FS_allL1'),
                                         EFcreate('EFTRTOnly_FS_allL1', EFTrackHypo_TRTOnly_FullScan()))),


    L2EFChain_TRTOnly_FullScan_EF('EFTRTOnly_FS_MB2',
                                  'L2_EFTRTOnly_FS_MB2', 5106, 'L1_MBTS_2','EF_EFTRTOnly_FS_MB2', 5106, ['EF_FStracks_TRTOnly'],
                                  Config(L2create('EFTRTOnly_FS_MB2'),
                                         EFcreate('EFTRTOnly_FS_MB2', EFTrackHypo_TRTOnly_FullScan())),
                                  prescale=1, pass_through=1),

    L2EFChain_TRTOnly_FullScan_EF('EFTRTOnly_FS',
                                  'L2_EFTRTOnly_FS', 5113, 'L1_MBTS_1','EF_EFTRTOnly_FS', 5113, ['EF_FStracks_TRTOnly'],
                                  Config(L2create('EFTRTOnly_FS'),
                                         EFcreate('EFTRTOnly_FS', EFTrackHypo_TRTOnly_FullScan())),
                                  prescale=1, pass_through=1),
    ]

class L2EFChain_InsideOut_FullScan_EF(L2EFChainDef):
    def __init__(self, sig_id,
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter,
                 l2_inputTEs, config,
                 prescale=1, pass_through=0):

        L2EFChainDef.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter,
                              l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config,
                              prescale, pass_through)

    class L2Config:
        def __init__(self, suffix): 
            self.suffix = suffix

            # dummy AllTE to create a common execution point for caching
            self.dummyRoI = DummyRoI("DummyRoI")
            self.dummyRoI.createRoIDescriptors = False
            self.dummyRoI.NumberOfOutputTEs=1

    class EFConfig:
        def __init__(self, suffix, TrigEFTrackHypo_1=None):
            self.suffix = suffix
            # dummy AllTE
            self.dummy = DummyFEX("DummyFEX_eftrk")
            self.TrigEFTrackHypo_1  = TrigEFTrackHypo_1  or TrigEFTrackHypo('TrigEFTrackHypo_'+suffix)

    class Config:
        def __init__(self, L2config, EFconfig):
            self.L2Config = L2config
            self.EFConfig = EFconfig
            
    def defineSequences(self, config):
        #EF sequences
        self.addEFSequence(self.inputTEs,
                           [self.config.EFConfig.dummy] +
                           [config.EFConfig.TrigEFTrackHypo_1],
                           'EF_')
        
    def defineSignatures(self):
        self.addEFSignature(['EF_'])
        
    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_':             'L2_' + self.config.L2Config.suffix,
            'EF_':             'EF_' + self.config.EFConfig.suffix,
            }
        pass
        

    def defineStreamGroupTriggerType(self):
        #self.physics_streams = ['minbias']
        #self.calib_streams = []
        self.physics_streams = ['minbias']
        self.groups = ['RATE:EFTracking','BW:Detector']
        self.trigger_type = ['hello']

Config   = L2EFChain_InsideOut_FullScan_EF.Config
L2create = L2EFChain_InsideOut_FullScan_EF.L2Config
EFcreate = L2EFChain_InsideOut_FullScan_EF.EFConfig

MonitorChainList += [
    L2EFChain_InsideOut_FullScan_EF('EFInsideOut_FS_allL1',
                                    'L2_EFInsideOut_FS_allL1', 5103, '','EF_EFInsideOut_FS_allL1', 5103, ['EF_FStracks'],
                                    Config(L2create('EFInsideOut_FS_allL1'),
                                           EFcreate('EFInsideOut_FS_allL1', EFTrackHypo_InsideOut_FullScan()))),
    ]

MonitorChainList += [
    L2EFChain_InsideOut_FullScan_EF('EFInsideOut_FS_MB2',
                                    'L2_EFInsideOut_FS_MB2', 5107, 'L1_MBTS_2','EF_EFInsideOut_FS_MB2', 5107, ['EF_FStracks'],
                                    Config(L2create('EFInsideOut_FS_MB2'),
                                           EFcreate('EFInsideOut_FS_MB2', EFTrackHypo_InsideOut_FullScan())),
                                    prescale=1, pass_through=1),
    
    L2EFChain_InsideOut_FullScan_EF('EFInsideOut_FS',
                                    'L2_EFInsideOut_FS', 5110, 'L1_MBTS_1','EF_EFInsideOut_FS', 5110, ['EF_FStracks'],
                                    Config(L2create('EFInsideOut_FS'),
                                           EFcreate('EFInsideOut_FS', EFTrackHypo_InsideOut_FullScan())),
                                    prescale=1, pass_through=1),
]

#FullScan versions of IdScan and SiTrack
class L2EFChain_L2Tracking_FullScan(L2EFChainDef):
    def __init__(self, sig_id,
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter,
                 l2_inputTEs, config,
                 prescale=1, pass_through=0):

        L2EFChainDef.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter,
                              l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config,
                              prescale, pass_through)

    class L2Config:
        #def __init__(self, suffix):
        def __init__(self, suffix, TrigL2TrackHypo_1=None): 
            self.suffix = suffix

            # dummy AllTE 
            self.dummy = DummyFEX("DummyFEX_trk")
            self.TrigL2TrackHypo_1  = TrigL2TrackHypo_1  #or CosmicTrtHypo_Cosmic("Beam_"+self.suffix+"_TrkHypo")
#            self.TrigL2TrackHypo_1.NTrthitsCut = -1
            
    class EFConfig:
        def __init__(self, suffix):
            self.suffix = suffix

    class Config:
        def __init__(self, L2config, EFconfig):
            self.L2Config = L2config
            self.EFConfig = EFconfig
            
    def defineSequences(self, config):
        #L2 sequences
        self.addL2Sequence(self.inputTEs,
                           [self.config.L2Config.dummy] +
                           [config.L2Config.TrigL2TrackHypo_1],
                           'L2_' )
                           
    def defineSignatures(self):
        self.addL2Signature(['L2_'])
        
    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_':             'L2_' + self.config.L2Config.suffix,
            'EF_':             'EF_' + self.config.EFConfig.suffix,
            }
        pass
    
    def defineStreamGroupTriggerType(self):
        self.physics_streams = ['minbias']
        self.groups = ['RATE:L2Tracking','BW:Detector']
        self.trigger_type = ['hello']


Config   = L2EFChain_L2Tracking_FullScan.Config
L2create = L2EFChain_L2Tracking_FullScan.L2Config
EFcreate = L2EFChain_L2Tracking_FullScan.EFConfig

MonitorChainList += [
    L2EFChain_L2Tracking_FullScan('L2IdScan_FS_allL1',
                 'L2_L2IdScan_FS_allL1', 5104, '','EF_L2IdScan_FS_allL1', 5104, ['L2_FStracks'],
                 Config(L2create('L2IdScan_FS_allL1'),
                        EFcreate('L2IdScan_FS_allL1'))),
    ]

MonitorChainList += [
    L2EFChain_L2Tracking_FullScan('L2SiTrack_FS_allL1',
                 'L2_L2SiTrack_FS_allL1', 5105, '','EF_L2SiTrack_FS_allL1', 5105, ['L2_FStracks_SiTrk'],
                 Config(L2create('L2SiTrack_FS_allL1'),
                        EFcreate('L2SiTrack_FS_allL1'))),
    ]

MonitorChainList += [
    L2EFChain_L2Tracking_FullScan('L2IdScan_FS_MB2',
                                  'L2_L2IdScan_FS_MB2', 5108, 'L1_MBTS_2','EF_L2IdScan_FS_MB2', 5108, ['L2_FStracks'],
                                  Config(L2create('L2IdScan_FS_MB2'),
                                         EFcreate('L2IdScan_FS_MB2')),
                                  prescale=1, pass_through=1),

    L2EFChain_L2Tracking_FullScan('L2SiTrack_FS_MB2',
                                  'L2_L2SiTrack_FS_MB2', 5109, 'L1_MBTS_2','EF_L2SiTrack_FS_MB2', 5109, ['L2_FStracks_SiTrk'],
                                  Config(L2create('L2SiTrack_FS_MB2'),
                                         EFcreate('L2SiTrack_FS_MB2')),
                                  prescale=1, pass_through=1),


    L2EFChain_L2Tracking_FullScan('L2IdScan_FS',
                                  'L2_L2IdScan_FS', 5111, 'L1_MBTS_1','EF_L2IdScan_FS', 5111, ['L2_FStracks'],
                                  Config(L2create('L2IdScan_FS'),
                                         EFcreate('L2IdScan_FS')),
                                  prescale=1, pass_through=1),

    L2EFChain_L2Tracking_FullScan('L2SiTrack_FS',
                                  'L2_L2SiTrack_FS', 5112, 'L1_MBTS_1','EF_L2SiTrack_FS', 5112, ['L2_FStracks_SiTrk'],
                                  Config(L2create('L2SiTrack_FS'),
                                         EFcreate('L2SiTrack_FS')),
                                  prescale=1, pass_through=1),
]

class L2EFChain_InDetMonitor_FullScan(L2EFChainDef):
    def __init__(self, sig_id,
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter,
                 l2_inputTEs, config,
                 prescale=1, pass_through=0):
        
        L2EFChainDef.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter,
                              l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config,
                              prescale, pass_through)

    class L2Config:
        def __init__(self, suffix): 
            self.suffix = suffix
            # dummy AllTE
            self.dummy = DummyRoI("DummyRoI_L2InDetMon")
                        
    class EFConfig:
        def __init__(self, suffix):
            self.suffix = suffix
            # dummy AllTE 
            self.dummy = DummyRoI("DummyRoI_EFInDetMon")
            
    class Config:
        def __init__(self, L2config, EFconfig):
            self.L2Config = L2config
            self.EFConfig = EFconfig
            
    def defineSequences(self, config):
        #L2 sequences
        self.addL2Sequence(self.inputTEs[0],
                           [self.config.L2Config.dummy,
                            theTrigL2SiTrackFinder_FullScanA,
                            theTrigL2SiTrackFinder_FullScanB,
                            theTrigL2SiTrackFinder_FullScanBC],
                            #theTrigL2SiTrackFinder_FullScanC],
                           'L2_FStracks_L2StarAB')
        #EF sequences
        self.addEFSequence("",
                           [self.config.EFConfig.dummy ]+
                           TrigEFIDSequence("FullScan","fullScan","TRTOnly").getSequence(),
                           'EF_FStracks_TRTOnly')
                           
    def defineSignatures(self):
        if '_R1' in self.sig_id:
            self.addL2Signature(['L2_FStracks', 'L2_FStracks_SiTrk', 'L2_FStracks_L2StarAB'])
        else:
            self.addL2Signature([ 'L2_FStracks_L2StarAB'])
        
        self.addEFSignature(['EF_FStracks', 'EF_FStracks_TRTOnly'])
        
    def defineTErenaming(self):
        self.TErenamingMap = {
            }
        pass
    
    def defineStreamGroupTriggerType(self):
        self.physics_streams = ['jets']
        self.groups = ['RATE:InDetTracking','BW:Detector']
        self.trigger_type = ['hello']


Config   = L2EFChain_InDetMonitor_FullScan.Config
L2create = L2EFChain_InDetMonitor_FullScan.L2Config
EFcreate = L2EFChain_InDetMonitor_FullScan.EFConfig

seed_indetmon_fs = 'L1_4J15'
if TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv2Menus() or \
       TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv3Menus() or \
       TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv1Menus(): 
    seed_indetmon_fs = 'L1_3J10'
elif  TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv2Menus():
    seed_indetmon_fs = 'L1_MBTS_2_2'
    
class L2EFChain_InDetMonitor_NoTRT_FullScan(L2EFChain_InDetMonitor_FullScan):
    #def defineSequences(self, config):
        #self.addEFSequence("",
        #                   [self.config.EFConfig.dummy ]+
        #                   TrigEFIDSequence("FullScanHI","heavyIonFS","InsideOutNoTRT").getSequence(),
        #                   'EF_FStracks_NoTRT')
    
    def defineSignatures(self):
        self.addL2Signature(['L2_FStracks',
                             'L2_FStracks_SiTrk',
                             'L2_FStracks_L2StarAB'])
        self.addEFSignature(['EF_FStracksHI'])

    def defineStreamGroupTriggerType(self):
        self.physics_streams = ['minbias']
        self.groups = ['RATE:InDetTracking','BW:Detector']
        self.trigger_type = ['hello']
        
Config   = L2EFChain_InDetMonitor_NoTRT_FullScan.Config
L2create = L2EFChain_InDetMonitor_NoTRT_FullScan.L2Config
EFcreate = L2EFChain_InDetMonitor_NoTRT_FullScan.EFConfig


class L2EFChain_InDetMonitor_InsideOutNoTRT_FullScan(L2EFChain_InDetMonitor_FullScan):
    def defineSequences(self, config):
        self.addL2Sequence(self.inputTEs[0],
                           [self.config.L2Config.dummy,
                            theTrigL2SiTrackFinder_FullScanA,
                            theTrigL2SiTrackFinder_FullScanB,
                            theTrigL2SiTrackFinder_FullScanBC
                            ],
                            #theTrigL2SiTrackFinder_FullScanC],
                           'L2_FStracks_L2StarAB')
        self.addEFSequence("",
                           [self.config.EFConfig.dummy ]+
                           TrigEFIDSequence("FullScan","fullScan","InsideOutNoTRT").getSequence(),
                           'EF_FStracks_NoTRT')
        
    def defineSignatures(self):
        self.addL2Signature(['L2_FStracks', 'L2_FStracks_SiTrk', 'L2_FStracks_L2StarAB'])
        self.addEFSignature(['EF_FStracks', 'EF_FStracks_NoTRT'])   

    
Config   = L2EFChain_InDetMonitor_InsideOutNoTRT_FullScan.Config
L2create = L2EFChain_InDetMonitor_InsideOutNoTRT_FullScan.L2Config
EFcreate = L2EFChain_InDetMonitor_InsideOutNoTRT_FullScan.EFConfig

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv1Menus():
    seeds = "L1_ZDC_A_C_VTE50"
elif TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv2Menus():
    seeds = "L1_MBTS_2_2"

if TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv1Menus():
    MonitorChainList += [
        
        L2EFChain_InDetMonitor_NoTRT_FullScan('InDetMon_HI_FS',
                                              'L2_InDetMon_FS', 5114, seeds,'EF_InDetMon_FS', 5114, [''],
                                              Config(L2create('InDetMon_FS'),
                                                     EFcreate('InDetMon_FS'))),
        ]
elif TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv2Menus():
    MonitorChainList += [
        L2EFChain_InDetMonitor_InsideOutNoTRT_FullScan('InDetMon_FS',
                                                       'L2_InDetMon_FS', 5114, seed_indetmon_fs,'EF_InDetMon_FS', 5114, [''],
                                                       Config(L2create('InDetMon_FS'),
                                                              EFcreate('InDetMon_FS'))),
        ]
else:
    MonitorChainList += [
        L2EFChain_InDetMonitor_FullScan('InDetMon_FS',
                                        'L2_InDetMon_FS', 5114, seed_indetmon_fs,'EF_InDetMon_FS', 5114, [''],
                                        Config(L2create('InDetMon_FS'),
                                               EFcreate('InDetMon_FS'))),
        
        L2EFChain_InDetMonitor_FullScan('InDetMon_FS_R1',
                                        'L2_InDetMon_FS_R1', 3488, seed_indetmon_fs,'EF_InDetMon_FS_R1', 3488, [''],
                                        Config(L2create('InDetMon_FS_R1'),
                                               EFcreate('InDetMon_FS_R1'))),
        

]



class L2EFChain_InDetMonitor_InsideOutNoTRT_FullScan_MergedDP(L2EFChain_InDetMonitor_FullScan):
    def defineSequences(self, config):
        self.addL2Sequence(self.inputTEs[0],
                           [self.config.L2Config.dummy]+
                           TrigEFIDSequence("FullScan","fullScan","DataPrep").getSequence()+
                           [theTrigFastTrackFinder_FullScan],
                           'L2_InDetMonMergeDP_FS')
        self.addEFSequence("",
                           [self.config.EFConfig.dummy ]+
                           TrigEFIDSequence("FullScan","fullScan","InsideOutNoTRT").getSequence(),
                           'EF_InDetMonMergeDP_FS')
        
    def defineSignatures(self):
        self.addL2Signature(['L2_InDetMonMergeDP_FS'])
        self.addEFSignature(['EF_InDetMonMergeDP_FS'])   

    
Config   = L2EFChain_InDetMonitor_InsideOutNoTRT_FullScan_MergedDP.Config
L2create = L2EFChain_InDetMonitor_InsideOutNoTRT_FullScan_MergedDP.L2Config
EFcreate = L2EFChain_InDetMonitor_InsideOutNoTRT_FullScan_MergedDP.EFConfig

MonitorChainList += [
    L2EFChain_InDetMonitor_InsideOutNoTRT_FullScan_MergedDP('InDetMonMergeDP_FS',
                                                            'L2_InDetMonMergeDP_FS', 5116, '',
                                                            'EF_InDetMonMergeDP_FS', 5116, [''],
                                                            Config(L2create('InDetMonMergeDP_FS'),
                                                                   EFcreate('InDetMonMergeDP_FS'))),
]


class L2EFChain_MonitorROBRequest(L2EFChainDef):
    def __init__(self, sig_id,
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter,
                 l2_inputTEs, config,
                 prescale=1, pass_through=0):
        
        L2EFChainDef.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter,
                              l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config,
                              prescale, pass_through)

    class L2Config:
        def __init__(self, suffix): 
            self.suffix = suffix
            self.ROBRequester = ROBRequestAlgo("DummyROBRequest")

    class EFConfig:
        def __init__(self, suffix):
            self.suffix = suffix
            
    class Config:
        def __init__(self, L2config, EFconfig):
            self.L2Config = L2config
            self.EFConfig = EFconfig
            
    def defineSequences(self, config):
        #L2 sequences
        self.addL2Sequence(self.inputTEs,
                           [self.config.L2Config.ROBRequester], 
                           'L2_DummyROBRequest' )
        #EF sequences

    def defineSignatures(self):
        self.addL2Signature(['L2_DummyROBRequest'])
        
    def defineTErenaming(self):
        self.TErenamingMap = {
            }
        pass
    
    def defineStreamGroupTriggerType(self):
        self.physics_streams = ['DISCARD']
        self.groups = ['RATE:DISCARD','BW:DISCARD']
        self.trigger_type = ['hello']


Config   = L2EFChain_MonitorROBRequest.Config
L2create = L2EFChain_MonitorROBRequest.L2Config
EFcreate = L2EFChain_MonitorROBRequest.EFConfig

MonitorChainList += [
    
    L2EFChain_MonitorROBRequest('ROBRequestMon',
                                          'L2_ROBRequestMon', 5115, 'L1_RD0_EMPTY','EF_ROBRequestMon', 5115, [''],
                                          Config(L2create('ROBRequestMon'),
                                                 EFcreate('ROBRequestMon'))),

    L2EFChain_MonitorROBRequest('ROBRequestMonUnseeded',
                                'L2_ROBRequestMonUnseeded', 763, '','EF_ROBRequestMonUnseeded', 763, [''],
                                Config(L2create('ROBRequestMonUnseeded'),
                                       EFcreate('ROBRequestMonUnseeded'))),
    ]

#------------------------------------------------------
class L2EFChain_InDetMonitor_L2Star_FS(L2EFChainDef):
    def __init__(self, sig_id,
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter,
                 l2_inputTEs, config,
                 prescale=1, pass_through=0):
        
        L2EFChainDef.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter,
                              l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config,
                              prescale, pass_through)

    class L2Config:
        def __init__(self, suffix): 
            self.suffix = suffix
            # dummy AllTE 
            self.dummy = DummyRoI("DummyRoI_L2InDetMon")
            
    class EFConfig:
        def __init__(self, suffix):
            self.suffix = suffix
            # dummy AllTE 
            self.dummy = DummyRoI("DummyRoI_EFInDetMon")
            
    class Config:
        def __init__(self, L2config, EFconfig):
            self.L2Config = L2config
            self.EFConfig = EFconfig
            
    def defineSequences(self, config):
        #L2 sequences
        self.addL2Sequence(self.inputTEs[0],
                           [self.config.L2Config.dummy,
                            theTrigL2SiTrackFinder_FullScanA,
                            theTrigL2SiTrackFinder_FullScanB,
                            theTrigL2SiTrackFinder_FullScanC,
                            theTrigL2SiTrackFinder_FullScanBC
                            ],
                           'L2_FStracks_L2Star')
        #EF sequences
        self.addEFSequence("",
                           [self.config.EFConfig.dummy ]+
                           TrigEFIDSequence("FullScan","fullScan","TRTOnly").getSequence(),
                           'EF_FStracks_TRTOnly')
                           
    def defineSignatures(self):
        self.addL2Signature(['L2_FStracks_L2Star'])
        self.addEFSignature(['EF_FStracks', 'EF_FStracks_TRTOnly'])
        
    def defineTErenaming(self):
        self.TErenamingMap = {
            }
        pass
    
    def defineStreamGroupTriggerType(self):
        self.physics_streams = ['jets']
        self.groups = ['RATE:InDetTracking','BW:Detector']
        self.trigger_type = ['hello']


Config   = L2EFChain_InDetMonitor_L2Star_FS.Config
L2create = L2EFChain_InDetMonitor_L2Star_FS.L2Config
EFcreate = L2EFChain_InDetMonitor_L2Star_FS.EFConfig


MonitorChainList += [
    L2EFChain_InDetMonitor_L2Star_FS('InDetMon_L2Star_FS',
                                     'L2_InDetMon_L2Star_FS', 1310, 'L1_3J10','EF_InDetMon_L2Star_FS', 1310, [''],
                                     Config(L2create('InDetMon_L2Star_FS'),
                                            EFcreate('InDetMon_L2Star_FS'))),
    ]

#-----------------------------------------------------------
class L2EFChain_TimeBurner(L2EFChainDef):
    def __init__(self, sig_id,
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter,
                 l2_inputTEs, config,
                 prescale=1, pass_through=0):
        
        L2EFChainDef.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter,
                              l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config,
                              prescale, pass_through)

    class L2Config:
        def __init__(self, suffix): 
            self.suffix = suffix
            self.TimeBurner= TimeBurner("DummyTimeBurner")

    class EFConfig:
        def __init__(self, suffix):
            self.suffix = suffix
            
    class Config:
        def __init__(self, L2config, EFconfig):
            self.L2Config = L2config
            self.EFConfig = EFconfig
            
    def defineSequences(self, config):
        #L2 sequences
        self.addL2Sequence(self.inputTEs,
                           [self.config.L2Config.TimeBurner], 
                           'L2_DummyTimeBurner' )
        #EF sequences

    def defineSignatures(self):
        self.addL2Signature(['L2_DummyTimeBurner'])
        
    def defineTErenaming(self):
        self.TErenamingMap = {
            }
        pass
    
    def defineStreamGroupTriggerType(self):
        self.physics_streams = ['DISCARD']
        self.groups = ['RATE:DISCARD','BW:DISCARD']
        self.trigger_type = ['hello']


Config   = L2EFChain_TimeBurner.Config
L2create = L2EFChain_TimeBurner.L2Config
EFcreate = L2EFChain_TimeBurner.EFConfig

MonitorChainList += [
    
    L2EFChain_TimeBurner('HLTTimeBurner',
                                          'L2_HLTTimeBurner', 5162, '','EF_HLTTimeBurner', 5162, [''],
                                          Config(L2create('HLTTimeBurner'),
                                                 EFcreate('HLTTimeBurner')), prescale=-1,),
    ]

from ChainTemplate import *
L2EFChain_TRTMonitoring = ChainTemplate


if ( TriggerFlags.triggerMenuSetup() in Lvl1Flags.PPv4Menus()\
     or TriggerFlags.triggerMenuSetup() in Lvl1Flags.ALFAv2Menus()\
     or TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv2Menus()
     ) :
    MonitorChainList += [
        L2EFChain_TRTMonitoring('IDMon_filled', l2=Chain(counter=745, seed='L1_RD0_FILLED'), ef=None).addGroups('RATE:DISCARD','BW:DISCARD').addStreams('IDMonitoring'),
        L2EFChain_TRTMonitoring('IDMon_empty', l2=Chain(counter=746, seed='L1_RD0_EMPTY'), ef=None).addGroups('RATE:DISCARD','BW:DISCARD').addStreams('IDMonitoring'),
        L2EFChain_TRTMonitoring('IDMon_unpaired_iso', l2=Chain(counter=747, seed='L1_RD0_UNPAIRED_ISO'), ef=None).addGroups('RATE:DISCARD','BW:DISCARD').addStreams('IDMonitoring'),
        ]


else:
    MonitorChainList += [
    L2EFChain_TRTMonitoring('TRTMon_filled', l2=Chain(counter=5610, seed='L1_RD0_FILLED'), ef=None).addGroups('RATE:DISCARD','BW:DISCARD').addStreams('TRTMonitoring'),
    L2EFChain_TRTMonitoring('TRTMon_empty', l2=Chain(counter=5611, seed='L1_RD0_EMPTY'), ef=None).addGroups('RATE:DISCARD','BW:DISCARD').addStreams('TRTMonitoring'),
    L2EFChain_TRTMonitoring('TRTMon_unpaired_iso', l2=Chain(counter=5612, seed='L1_RD0_UNPAIRED_ISO'), ef=None).addGroups('RATE:DISCARD','BW:DISCARD').addStreams('TRTMonitoring'),
    ]

