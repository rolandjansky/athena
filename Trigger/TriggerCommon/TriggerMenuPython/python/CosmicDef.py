# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################
#
# SliceDef file for HLT signatures used exclusively in the cosmic slice
#
#########################################################################

from TriggerMenuPython.HltConfig import *

from AthenaCommon.Include import include
from AthenaCommon.SystemOfUnits import GeV
from TriggerJobOpts.TriggerFlags  import TriggerFlags

from ChainTemplate import *

class LocalFlags:
    DontWriteOutAllTEStream=True
    UseSingleBeam7BitL1Menu=True
    UseSingleBeam3BitL1Menu=False
    UseStartup2009L1Menu=False
    Use3Streams=False
    T2CaloFakeLVL1InLVL2=False
    useUniqueMuonEFInstance=True
    doM3Data=True
    useUniqueMuFastInstance=True
    useUniqueMuonEFInstance=True
    doMuonCalibrationStream=False

MuFastInstances = {}

defaultSignatures = {
    'Signature'                 : 'L1_MU6_RPC',# L1_MU6_EMPTY
    'StandaloneSignature'       : 'L1_EM3', # L1_EM3_EMPTY
    'CaloSignature'             : 'L1_J10', # L1_J5_EMPTY
    'RPCSignature'              : 'L1_MU6_RPC',# L1_MU6_EMPTY / L1_MU0_HIGH_RPC_EMPTY
    'TGCSignature'              : 'L1_MU0_TGC',# L1_MU0_EMPTY
    'TrackingSignature'         : 'L1_EM3', # L1_EM3_EMPTY
    'WidePatternRPCSignature'   : 'L1_MU0_LOW_RPC',# L1_MU0_EMPTY
    'CalibrationSignature'      : 'L1_EM3', # L1_EM#_EMPTY
    'Signature_V2'              : 'L1_MU6_EMPTY',
    'StandaloneSignature_V2'    : 'L1_EM3_EMPTY', 
    'CaloSignature_V2'          : 'L1_J10_EMPTY', 
    'MuonSignature_V2'          : 'L1_MU0_EMPTY',
    'TrackingSignature_V2'      : 'L1_EM3_EMPTY', 
    'CalibrationSignature_V2'   : 'L1_EM3_EMPTY' 
    }

defaultThresholds = {
    'Threshold'                 : 'MU6_RPC', # MU6
    'StandaloneThreshold'       : 'EM3',
    'CaloThreshold'             : 'J10',
    'RPCThreshold'              : 'MU6_RPC', # MU6
    'TGCThreshold'              : 'MU0_TGC', # MU0
    'TrackingThreshold'         : 'EM3',
    'WidePatternRPCThreshold'   : 'MU0_LOW_RPC', # MU0
    'CalibrationThreshold'      : 'EM3',
    'Threshold_V2'              : 'MU6',
    'StandaloneThreshold_V2'    : 'EM3', 
    'CaloThreshold_V2'          : 'J10', 
    'MuonThreshold_V2'          : 'MU0',
    'TrackingThreshold_V2'      : 'EM3', 
    'CalibrationThreshold_V2'   : 'EM3' 
    }


L1CaloItemList_J_V2    = ['L1_J5_EMPTY', 'L1_J10_EMPTY', 'L1_J10_win6_EMPTY',
                         'L1_J20_EMPTY', 'L1_J40_EMPTY', 'L1_J70_EMPTY',
                         'L1_J100_EMPTY']

L1CaloItemList_EM_V2   = ['L1_EM3_EMPTY', 'L1_EM4_EMPTY', 'L1_EM7_EMPTY',
                         'L1_EM7I_EMPTY', 'L1_EM10_EMPTY', 'L1_EM13_EMPTY',
                         'L1_EM13I_EMPTY', 'L1_EM18_EMPTY', 'L1_2EM3_EMPTY',
                         'L1_2EM4_EMPTY', 'L1_2EM7_EMPTY', 'L1_2EM7I_EMPTY',
                         'L1_2EM10_EMPTY']

L1CaloItemList_TAU_V2  = ['L1_TAU5_EMPTY', 'L1_TAU6_EMPTY', 'L1_TAU9_EMPTY',
                         'L1_TAU9I_EMPTY', 'L1_TAU16_EMPTY', 'L1_TAU16I_EMPTY', 'L1_TAU25_EMPTY',
                         'L1_2TAU9_EMPTY', 'L1_2TAU9I_EMPTY', 'L1_2TAU16_EMPTY',
                         'L1_2TAU16I_EMPTY']

L1CaloItemList_JF_V2   = ['L1_FJ3_EMPTY', 'L1_FJ18_EMPTY']

L1CaloItemList_TE_V2   = ['L1_TE150_EMPTY', 'L1_TE360_EMPTY']

L1CaloItemList_XE_V2   = ['L1_XE15_EMPTY', 'L1_XE25_EMPTY', 'L1_XE35_EMPTY', 'L1_XE50_EMPTY']

L1CaloItemList_JE_V2   = ['L1_JE120_EMPTY', 'L1_JE280_EMPTY']

L1CaloItemList_NoXE_V2 = L1CaloItemList_J_V2 + L1CaloItemList_EM_V2 + L1CaloItemList_TAU_V2 + L1CaloItemList_JF_V2 + L1CaloItemList_TE_V2 + L1CaloItemList_JE_V2

MBTSItemList_SideA_V2=['L1_MBTS_A_EMPTY']
MBTSItemList_SideC_V2=['L1_MBTS_C_EMPTY']

MBTSThresholdList_SideA_V2=[
                   'MBTS_A'
                   ]

MBTSThresholdList_SideC_V2=[
                   'MBTS_C'
                   ]

MBTSThresholdList_V2 = MBTSThresholdList_SideC_V2 + MBTSThresholdList_SideA_V2

###################################################################################
class L2EFChain_CosmicTemplate(L2EFChainDef):
    def __init__(self, sig_id, chain_counter, l2_lower_chain_name, inputTEs, config,
                 prescale=1, pass_through=0):
        if sig_id not in self._supported:
            raise Exception("ERROR: "+sig_id+" not supprted"+str(self._supported) )
        self.Flag='TriggerFlags.CosmicSlice.do%s()' % sig_id
        L2EFChainDef.__init__(self, sig_id,
                              "L2_"+sig_id, chain_counter,
                              l2_lower_chain_name,
                              "EF_"+sig_id, chain_counter,
                              inputTEs, config,
                              prescale, pass_through)
        mlog = logging.getLogger( 'CosmicDef.py:L2EFChain_CosmicTemplate' )
        mlog.verbose('in L2EFChain_CosmicTemplate constructor for %s' % sig_id)
        
    class L2Config:
        def __init__(self, suffix):
            self.suffix=suffix
            self.L2SequenceList = []
            self.L2SequenceHash = {}
            pass
    
    class EFConfig:
        def __init__(self, suffix):
            self.suffix=suffix
            self.EFSequenceList = []
            self.EFSequenceHash = {}
            pass
                        
    class Config:
        def __init__(self, L2Config, EFConfig):
            self.L2Config = L2Config
            self.EFConfig = EFConfig
            pass
        
    def defineSequences(self, config):
        
        mlog = logging.getLogger( 'CosmicDef.py:L2EFChain_CosmicTemplate.defineSequences' )
        mlog.verbose('defineSequences for %s' % self.sig_id)
        mlog.verbose('sequences list: %s %s' %(config.L2Config.L2SequenceList, config.EFConfig.EFSequenceList) )
        mlog.verbose('SequenceHash are %s %s' % (config.L2Config.L2SequenceHash, config.EFConfig.EFSequenceHash) )
        if not eval(self.Flag):
            mlog.debug('Not declaring any sequences for %s since %s is False' %(self.sig_id, self.Flag))
            return
        
        next_input_TE=self.inputTEs[0]
        mlog.verbose('starting to build a sequence list inputTE: %s ' %  self.inputTEs[0])
        
        for sequenceName in config.L2Config.L2SequenceList:
            self.addL2Sequence(next_input_TE,config.L2Config.L2SequenceHash[sequenceName],sequenceName)
            mlog.verbose('added L2 sequence %s %s' % (sequenceName, config.L2Config.L2SequenceHash[sequenceName] ) )
            next_input_TE=sequenceName
        
        for sequenceName in config.EFConfig.EFSequenceList:
            self.addEFSequence(next_input_TE,config.EFConfig.EFSequenceHash[sequenceName],sequenceName)
            mlog.verbose('added EF sequence %s %s' % (sequenceName, config.EFConfig.EFSequenceHash[sequenceName] ) )
            next_input_TE=sequenceName
    
    def defineSignatures(self):
        mlog = logging.getLogger( 'CosmicDef.py:L2EFChain_CosmicTemplate.defineSignatures' )
        mlog.verbose('defineSignatures for %s' % self.sig_id)
        
        if not eval(self.Flag):
            mlog.debug('Not declaring any signatures for %s since %s is False' %(self.sig_id, self.Flag))
            return

        mlog.verbose
        for sequenceName in self.config.L2Config.L2SequenceList:
            self.addL2Signature([sequenceName])
            mlog.verbose('added L2 signature %s' % sequenceName)
        for sequenceName in self.config.EFConfig.EFSequenceList:
            self.addEFSignature([sequenceName])
            mlog.verbose('added EF signature %s' % sequenceName)

    def defineTErenaming(self):
        #Simply define identity map 
        self.TErenamingMap={}
        for sequenceName in self.config.L2Config.L2SequenceList:
            self.TErenamingMap[sequenceName]=sequenceName
        for sequenceName in self.config.EFConfig.EFSequenceList:
            self.TErenamingMap[sequenceName]=sequenceName

###################################################################################
class L2EFChain_CosmicTileCalibration(L2EFChain_CosmicTemplate):
    def __init__(self, sig_id, chain_counter, l2_lower_chain_name, l2_inputTEs, config,
                 prescale=1, pass_through=0):
        self._supported = ["TileCalib_laser","TileCalib_cis","TileCalib_laser_rejAll","TileCalib_cis_rejAll"]
        L2EFChain_CosmicTemplate.__init__(self, sig_id, chain_counter, l2_lower_chain_name, l2_inputTEs, config,
                 prescale=1, pass_through=0)
        
    class L2Config(L2EFChain_CosmicTemplate.L2Config):
        def __init__(self, suffix):
            L2EFChain_CosmicTemplate.L2Config.__init__(self,suffix)
            
            from TrigDetCalib.TrigDetCalibConfig import TileSubDetListWriter
            self.l2_tileSubDetListWriter = TileSubDetListWriter("L2_Cosmic"+suffix+"TileSubDetListWriter")
            self.l2_tileSubDetListWriter.Subdetectors = "Tile"
            self.l2_tileSubDetListWriter.MaxRoIsPerEvent = 1
            self.l2_tileSubDetListWriter.addL1Calo = True
            if(suffix.count('_rejAll')):
               self.l2_tileSubDetListWriter.rejectAll = True

            self.L2SequenceList=[self.l2_tileSubDetListWriter.name()]
            self.L2SequenceHash[self.l2_tileSubDetListWriter.name()]=[self.l2_tileSubDetListWriter]
            pass

    class EFConfig(L2EFChain_CosmicTemplate.EFConfig):
        def __init__(self, suffix):
            L2EFChain_CosmicTemplate.EFConfig.__init__(self,suffix)

            from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
            self.EF_Dummy = DummyFEX("EF_%s" % suffix)

            self.EFSequenceList=[self.EF_Dummy.name()]
            self.EFSequenceHash[self.EF_Dummy.name()]=[self.EF_Dummy]
            pass

    class Config(L2EFChain_CosmicTemplate.Config):
        def __init__(self, L2Config, EFConfig):
            L2EFChain_CosmicTemplate.Config.__init__(self, L2Config, EFConfig)
            pass

    def defineStreamGroupTriggerType(self):
        self.calib_streams = ['Tile']
        self.groups = ['RATE:Calibration', 'RATE:Cosmic_TileCalibration','BW:Detector']
        pass

Config   = L2EFChain_CosmicTileCalibration.Config
L2create = L2EFChain_CosmicTileCalibration.L2Config
EFcreate = L2EFChain_CosmicTileCalibration.EFConfig

Cosmics = [

    L2EFChain_CosmicTileCalibration("TileCalib_laser", 5000, 'L1_CALREQ2', ['NIM30'],
                                    Config(L2create("TileCalib_laser"),
                                           EFcreate("TileCalib_laser"))),
    
    L2EFChain_CosmicTileCalibration("TileCalib_cis",  5001,  'L1_CALREQ1', ['NIM29'],
                                    Config(L2create("TileCalib_cis"),
                                           EFcreate("TileCalib_cis"))),

    L2EFChain_CosmicTileCalibration("TileCalib_laser_rejAll", 5110, 'L1_CALREQ2', ['NIM30'],
                                    Config(L2create("TileCalib_laser_rejAll"),
                                           EFcreate("TileCalib_laser_rejAll"))),
    
    L2EFChain_CosmicTileCalibration("TileCalib_cis_rejAll",  5121,  'L1_CALREQ1', ['NIM29'],
                                    Config(L2create("TileCalib_cis_rejAll"),
                                           EFcreate("TileCalib_cis_rejAll")))
    ]

###################################################################################

from TrigDetCalib.TrigDetCalibConfig import TrigSubDetListWriter

l2_pixelSubDetListWriter = TrigSubDetListWriter("CosmicPixelNoiseSubDetListWriter")
l2_pixelSubDetListWriter.Subdetectors = "Pixel"
l2_pixelSubDetListWriter.extraROBs = []

l2_pixelSCTSubDetListWriter = TrigSubDetListWriter("CosmicPixelSCTNoiseSubDetListWriter")
l2_pixelSCTSubDetListWriter.Subdetectors = "Pixel,SCT"
l2_pixelSCTSubDetListWriter.extraROBs = []

l2_pixelSubDetListWriter_rejAll = TrigSubDetListWriter("CosmicPixelNoise_rejAllSubDetListWriter")
l2_pixelSubDetListWriter_rejAll.Subdetectors = "Pixel"
l2_pixelSubDetListWriter_rejAll.extraROBs = []
l2_pixelSubDetListWriter_rejAll.rejectAll = True


l2_SCTSubDetListWriter = TrigSubDetListWriter("CosmicSCTNoiseSubDetListWriter")
l2_SCTSubDetListWriter.Subdetectors = "SCT"
l2_SCTSubDetListWriter.extraROBs = []

l2_SCTSubDetListWriter_rejAll = TrigSubDetListWriter("CosmicSCTNoise_rejAllSubDetListWriter")
l2_SCTSubDetListWriter_rejAll.Subdetectors = "SCT"
l2_SCTSubDetListWriter_rejAll.extraROBs = []
l2_SCTSubDetListWriter_rejAll.rejectAll = True

class L2EFChain_CosmicIDNoiseCalibration(L2EFChainDef):

    def __init__(self, sig_id,
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter,
                 l2_inputTEs, config):

        L2EFChainDef.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config)

        self.robWriter = []

    class L2Config:
        def __init__(self, suffix):
            self.suffix = suffix           
    
    class EFConfig:
        def __init__(self, suffix):
            self.suffix = suffix

    class Config:
        def __init__(self, L2config, EFconfig):
            self.L2Config = L2config
            self.EFConfig = EFconfig
    
    def defineStreamGroupTriggerType(self):
        self.physics_streams = []
        if 'PixelNoise' in self.sig_id: 
            self.calib_streams = ['PixelNoise']
            self.groups = ['RATE:PixelCalibration','BW:Detector']
        if 'PixelBeam' in self.sig_id: 
            self.calib_streams = ['PixelBeam']
            self.groups = ['RATE:PixelBeamCalibration','BW:Detector']
        if 'SCT' in self.sig_id: 
            self.calib_streams = ['SCTNoise']
            self.groups = [ 'RATE:SCTCalibration','BW:Detector']

    def defineSequences(self, config):
        if ('Pixel' and 'rejAll') in self.sig_id:
            self.robWriter  = [l2_pixelSubDetListWriter_rejAll]
        elif ('PixelBeam') in self.sig_id:
            self.robWriter  = [l2_pixelSCTSubDetListWriter]
        elif ('Pixel') in self.sig_id:
            self.robWriter  = [l2_pixelSubDetListWriter]
        elif ('SCT' and 'rejAll') in self.sig_id:
            self.robWriter = [l2_SCTSubDetListWriter_rejAll]
        elif ('SCT') in self.sig_id:
            self.robWriter = [l2_SCTSubDetListWriter]
        else :
            mlog = logging.getLogger( 'CosmicDef.py:' )
            mlog.error("Can not configure"+self.sig_id+" unknown input")
            exit
            
        self.addL2Sequence(self.inputTEs,
                           self.robWriter,
                           'L2_')
        #mlog = logging.getLogger( 'CosmicDef.py:BETTA' )
        #mlog.setLevel(logging.NOSTAT)
        #mlog.verbose('added L2 sequence %s %s' % (self.inputTEs, self.robWriter) )
        
    def defineSignatures(self):
        self.addL2Signature(['L2_'])


    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_':     'L2_Calib' + self.sig_id,
            }
        pass

Config   = L2EFChain_CosmicIDNoiseCalibration.Config
L2create = L2EFChain_CosmicIDNoiseCalibration.L2Config
EFcreate = L2EFChain_CosmicIDNoiseCalibration.EFConfig

Cosmics += [
   
    L2EFChain_CosmicIDNoiseCalibration("PixelNoise",
                                       "L2_PixelNoise", 5002,  'L1_RD0_EMPTY', '', None,[''],
                                       Config(L2create(None),
                                              EFcreate(None))),

    L2EFChain_CosmicIDNoiseCalibration("PixelBeam",
                                       "L2_PixelBeam", 5003,  'L1_RD0_FILLED', '', None,[''],
                                       Config(L2create(None),
                                              EFcreate(None))),

    L2EFChain_CosmicIDNoiseCalibration("PixelNoise_rejAll",
                                       "L2_PixelNoise_rejAll", 5101,  'L1_RD0_EMPTY', '', None,[''],
                                       Config(L2create(None),
                                              EFcreate(None))),

    L2EFChain_CosmicIDNoiseCalibration("SCTNoise",
                                       "L2_SCTNoise", 5060,  'L1_RD0_EMPTY', '', None,[''],
                                       Config(L2create(None),
                                              EFcreate(None))),
    
    L2EFChain_CosmicIDNoiseCalibration("SCTNoise_rejAll",
                                       "L2_SCTNoise_rejAll", 5061,  'L1_RD0_EMPTY', '', None,[''],
                                       Config(L2create(None),
                                              EFcreate(None))),


    ]


l2_VdMSubDetListWriter = TrigSubDetListWriter("VdMSubDetListWriter")
l2_VdMSubDetListWriter.SubdetId = ['TDAQ_LVL2','TDAQ_CTP','FORWARD_LUCID','FORWARD_ZDC','FORWARD_BCM','SiOnly']
l2_VdMSubDetListWriter.MaxRoIsPerEvent=1

        
        

class L2EFChain_VdMCalibration(L2EFChainDef):

    def __init__(self, sig_id,
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter,
                 l2_inputTEs, config):

        L2EFChainDef.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config)

        self.robWriter = []

    class L2Config:
        def __init__(self, suffix):
            self.suffix = suffix           
    
    class EFConfig:
        def __init__(self, suffix):
            self.suffix = suffix

    class Config:
        def __init__(self, L2config, EFconfig):
            self.L2Config = L2config
            self.EFConfig = EFconfig
    
    def defineStreamGroupTriggerType(self):
        self.physics_streams = []
        if 'VdM' in self.sig_id:
            self.calib_streams = ['VdM']
            self.groups = [ 'RATE:VdMCalibration','BW:Detector']
            

    def defineSequences(self, config):
        if ('VdM') in self.sig_id:
            self.robWriter = [l2_VdMSubDetListWriter]
        else :
            mlog = logging.getLogger( 'CosmicDef.py:' )
            mlog.error("Can not configure "+self.sig_id+" unknown input")
            exit
            
        self.addL2Sequence(self.inputTEs,
                           self.robWriter,
                           'L2_')

    def defineSignatures(self):
        self.addL2Signature(['L2_'])


    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_':     'L2_Calib_' + self.sig_id,
            }
        pass

Config   = L2EFChain_VdMCalibration.Config
L2create = L2EFChain_VdMCalibration.L2Config
EFcreate = L2EFChain_VdMCalibration.EFConfig



#FEXES
fexes = FreeStore()
from TrigT2MinBias.TrigT2MinBiasConfig import fexes as l2fexes
fexes.update(l2fexes)

hypos = FreeStore()
from TrigT2MinBias.TrigT2MinBiasConfig import hypos as l2hypos
hypos.update(l2hypos)

class L2EFChain_VdMCalibrationHI(L2EFChain_VdMCalibration):

    def __init__(self, sig_id,
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter,
                 l2_inputTEs, config):
        self.robWriter = [l2_VdMSubDetListWriter]        
        L2EFChain_VdMCalibration.__init__(self, sig_id,
                                          l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                                          ef_chain_name, ef_chain_counter,
                                          l2_inputTEs, config)
        
    def defineSequences(self, config):

        self.addL2Sequence(self.inputTEs,
                           [fexes.L2MbMbtsFex, hypos.L2MbMbtsHypo_1_1],
                           'L2_mbMbts_1_1_VdM' )
        
        self.addL2Sequence('L2_mbMbts_1_1_VdM',
                           self.robWriter,
                           'L2_')

    def defineSignatures(self):
        self.addL2Signature(['L2_mbMbts_1_1_VdM'])
        self.addL2Signature(['L2_'])



Cosmics += [
    #VdM_mbMbts_1_1_MBTS2_BGRP7 [ie. L1_MBTS_2_BGRP7 --> L2_mbMbts_1_1 + VdM streamed]
    L2EFChain_VdMCalibrationHI("VdM_mbMbts_1_1_MBTS2_BRGP7",
                               "L2_VdM_mbMbts_1_1_MBST2_BGRP7", 5086,  'L1_MBTS_2_BGRP7', '', None,[''],
                               Config(L2create(None),
                                      EFcreate(None))),
    
    ]


Cosmics += [
   
    L2EFChain_VdMCalibration("VdM_RD0_UNPAIRED_ISO",
                             "L2_VdM_RD0_UNPAIRED_ISO", 5012,  'L1_RD0_UNPAIRED_ISO', '', None,[''],
                             Config(L2create(None),
                                    EFcreate(None))),
    
    L2EFChain_VdMCalibration("VdM_MBTS_2_UNPAIRED_ISO",
                             "L2_VdM_MBTS_2_UNPAIRED_ISO", 5013,  'L1_MBTS_2_UNPAIRED_ISO', '', None,[''],
                             Config(L2create(None),
                                    EFcreate(None))),

        
    L2EFChain_VdMCalibration("VdM_MBTS_1_1_UNPAIRED_ISO",
                             "L2_VdM_MBTS_1_1_UNPAIRED_ISO", 5014,  'L1_MBTS_1_1_UNPAIRED_ISO', '', None,[''],
                             Config(L2create(None),
                                    EFcreate(None))),
    
    L2EFChain_VdMCalibration("VdM_BGRP7",
                             "L2_VdM_BGRP7", 5080,  'L1_BGRP7', '', None,[''],
                             Config(L2create(None),
                                    EFcreate(None))),

    L2EFChain_VdMCalibration("VdM_MBTS_2_BGRP7",
                             "L2_VdM_MBTS_2_BGRP7", 5081,  'L1_MBTS_2_BGRP7', '', None,[''],
                             Config(L2create(None),
                                    EFcreate(None))),

    L2EFChain_VdMCalibration("VdM_LUCID_BGRP7",
                             "L2_VdM_LUCID_BGRP7", 5082,  'L1_LUCID_BGRP7', '', None,[''],
                             Config(L2create(None),
                                    EFcreate(None))),

    L2EFChain_VdMCalibration("VdM_ZDC_A_C_BGRP7",
                             "L2_VdM_ZDC_A_C_BGRP7", 5083,  'L1_ZDC_A_C_BGRP7', '', None,[''],
                             Config(L2create(None),
                                    EFcreate(None))),

    L2EFChain_VdMCalibration("VdM_ZDC_A_C_VTE50",
                             "L2_VdM_ZDC_A_C_VTE50", 5084,  'L1_ZDC_A_C_VTE50', '', None,[''],
                             Config(L2create(None),
                                    EFcreate(None))),
 
    L2EFChain_VdMCalibration("VdM_ZDC_A_BGRP7",
                             "L2_VdM_ZDC_A_BGRP7", 5085,  'L1_ZDC_A_BGRP7', '', None,[''],
                             Config(L2create(None),
                                    EFcreate(None))),

    L2EFChain_VdMCalibration("VdM_ZDC_C_BGRP7",
                             "L2_VdM_ZDC_C_BGRP7", 5087,  'L1_ZDC_C_BGRP7', '', None,[''],
                             Config(L2create(None),
                                    EFcreate(None))),

    ]




###################################################################################
class L2EFChain_CosmicLArCalib(L2EFChain_CosmicTemplate):
    def __init__(self, sig_id, chain_counter, l2_lower_chain_name, l2_inputTEs, config,
                 prescale=1, pass_through=0):
        self._supported = ["LArCalib_V2"]
        L2EFChain_CosmicTemplate.__init__(self, sig_id, chain_counter, l2_lower_chain_name, l2_inputTEs, config,
                 prescale=1, pass_through=0)
        mlog = logging.getLogger( 'CosmicDef.py:L2EFChain_CosmicLArCalib' )

    class L2Config(L2EFChain_CosmicTemplate.L2Config):
        def __init__(self, suffix):
            L2EFChain_CosmicTemplate.L2Config.__init__(self,suffix)
            
            from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_eGamma
            self.l2_T2CaloEgamma = T2CaloEgamma_eGamma("Cosmic"+suffix+"T2CaloEgamma_eGamma")
            if (LocalFlags.T2CaloFakeLVL1InLVL2):
                self.l2_T2CaloEgamma.L1ForcePhi=-1.5
                self.l2_T2CaloEgamma.L1ForceEta=1.7

            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_g7
            self.l2_L2CaloHypo_g7 = L2CaloHypo_g7("Cosmic"+suffix+"L2CaloHypo_g7")
            self.l2_L2CaloHypo_g7.ETthr = [-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV]
            self.l2_L2CaloHypo_g7.AcceptAll=False
            
            from TrigEgammaHypo.TrigL2PhotonFexConfig import L2PhotonFex_1
            self.l2_L2PhotonFex_1 = L2PhotonFex_1("Cosmic"+suffix+"L2PhotonFex_1")
            
            from TrigEgammaHypo.TrigL2PhotonHypoConfig import L2PhotonHypo_g10_loose
            self.l2_L2PhotonHypo_g10_loose = L2PhotonHypo_g10_loose("Cosmic"+suffix+"L2PhotonHypo_g10_loose")
            self.l2_L2PhotonHypo_g10_loose.ETthr = [-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV]
            self.l2_L2PhotonHypo_g10_loose.AcceptAll=False

            from TrigDetCalib.TrigDetCalibConfig import LArL2ROBListWriter
            self.L2PhotonROBListWriter = LArL2ROBListWriter("Cosmic"+suffix+"LArL2ROBListWriter")

            self.L2SequenceHash[self.l2_T2CaloEgamma.name()]=[self.l2_T2CaloEgamma,self.l2_L2CaloHypo_g7]
            self.L2SequenceList.append(self.l2_T2CaloEgamma.name())
            self.L2SequenceHash["Cosmic"+suffix+"L2PhotonHypo"]=[self.l2_L2PhotonFex_1,self.l2_L2PhotonHypo_g10_loose]
            self.L2SequenceList.append("Cosmic"+suffix+"L2PhotonHypo")
            self.L2SequenceHash["Cosmic"+suffix+"L2Photon"]=[self.L2PhotonROBListWriter]
            self.L2SequenceList.append("Cosmic"+suffix+"L2Photon")
            
            #include("TrigT2CaloCommon/LArCellBuilding.py")
                
    class EFConfig(L2EFChain_CosmicTemplate.EFConfig):
        def __init__(self, suffix):
            L2EFChain_CosmicTemplate.EFConfig.__init__(self,suffix)
            
            from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
            self.EF_Dummy = DummyFEX("Cosmic_EF_%s" % suffix)

            self.EFSequenceHash[self.EF_Dummy.name()]=[self.EF_Dummy]
            self.EFSequenceList.append(self.EF_Dummy.name())
            
    class Config(L2EFChain_CosmicTemplate.Config):
        def __init__(self, L2Config, EFConfig):
            L2EFChain_CosmicTemplate.Config.__init__(self, L2Config, EFConfig)
            pass

    def defineStreamGroupTriggerType(self):
        self.groups = ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration']
        pass

Config   = L2EFChain_CosmicLArCalib.Config
L2create = L2EFChain_CosmicLArCalib.L2Config
EFcreate = L2EFChain_CosmicLArCalib.EFConfig

Cosmics += [

    L2EFChain_CosmicLArCalib("LArCalib_V2",  5003,
                             TriggerFlags.CosmicSlice.CaloSignature_V2() or defaultSignatures['CaloSignature_V2'],
                             [TriggerFlags.CosmicSlice.CaloThreshold_V2() or defaultThresholds['CaloThreshold_V2']],
                             Config(L2create("LArCalib_V2"),
                                    EFcreate("LArCalib_V2")))
    
    ]

###################################################################################
class L2EFChain_CosmicEgamma(L2EFChain_CosmicTemplate):
    
    def __init__(self, sig_id, chain_counter, l2_lower_chain_name, l2_inputTEs, config,
                 prescale=1, pass_through=0):
        self._supported = ["Egamma_V2"]
        L2EFChain_CosmicTemplate.__init__(self, sig_id, chain_counter, l2_lower_chain_name, l2_inputTEs, config,
                 prescale=1, pass_through=0)
        mlog = logging.getLogger( 'CosmicDef.py:L2EFChain_CosmicEgamma' )

    class L2Config(L2EFChain_CosmicTemplate.L2Config):
        def __init__(self, suffix):
            L2EFChain_CosmicTemplate.L2Config.__init__(self,suffix)

            from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_eGamma
            self.l2_T2CaloEgamma = T2CaloEgamma_eGamma("Cosmic"+suffix+"T2CaloEgamma_eGamma")
            if (LocalFlags.T2CaloFakeLVL1InLVL2): #ideally don't want any flags
                self.l2_T2CaloEgamma.L1ForcePhi=-1.5
                self.l2_T2CaloEgamma.L1ForceEta=1.7

            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_g7
            self.l2_L2CaloHypo_g7 = L2CaloHypo_g7("Cosmic"+suffix+"L2CaloHypo_g7")
            self.l2_L2CaloHypo_g7.ETthr = [-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV]
            self.l2_L2CaloHypo_g7.AcceptAll=False
            
            from TrigEgammaHypo.TrigL2PhotonFexConfig import L2PhotonFex_1
            self.l2_L2PhotonFex_1 = L2PhotonFex_1("Cosmic"+suffix+"L2PhotonFex_1")
            
            from TrigEgammaHypo.TrigL2PhotonHypoConfig import L2PhotonHypo_g10_loose
            self.l2_L2PhotonHypo_g10_loose = L2PhotonHypo_g10_loose("Cosmic"+suffix+"L2PhotonHypo_g10_loose")
            self.l2_L2PhotonHypo_g10_loose.ETthr = [-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV]
            self.l2_L2PhotonHypo_g10_loose.AcceptAll=False
            
            self.L2SequenceHash[self.l2_T2CaloEgamma.name()]=[self.l2_T2CaloEgamma,self.l2_L2CaloHypo_g7]
            self.L2SequenceList+=[self.l2_T2CaloEgamma.name()]
            self.L2SequenceHash["Cosmic"+suffix+"L2Photon"]=[self.l2_L2PhotonFex_1,self.l2_L2PhotonHypo_g10_loose]
            self.L2SequenceList+=["Cosmic"+suffix+"L2Photon"]
                
            #include("TrigT2CaloCommon/LArCellBuilding.py") why do we include this?

    class EFConfig(L2EFChain_CosmicTemplate.EFConfig):
        def __init__(self, suffix):
            L2EFChain_CosmicTemplate.EFConfig.__init__(self,suffix)

            from TrigEgammaRec.TrigEgammaRecConfig import TrigEgammaRec_eGamma ,TrigEgammaRec_NoIDEF_eGamma
            self.TrigEgammaRec_NoIDEF_eGamma=TrigEgammaRec_NoIDEF_eGamma("Cosmic"+suffix+"TrigEgammaRec_NoIDEF_eGamma")
                    
            from TrigEgammaHypo.TrigEFPhotonHypoConfig import EFPhotonHypo_g10_loose
            self.EFPhotonHypo_g10_loose=EFPhotonHypo_g10_loose("Cosmic"+suffix+"EFPhotonHypo_g10_loose")
            self.EFPhotonHypo_g10_loose.etCut = [-7.*GeV, -7.*GeV, -7.*GeV, -7.*GeV, -7.*GeV]
            self.EFPhotonHypo_g10_loose.AcceptAll=False
            
            from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_eGamma, TrigCaloTowerMaker_eGamma, TrigCaloClusterMaker_slw
            self.TrigCaloCellMaker_eGamma =TrigCaloCellMaker_eGamma("Cosmic"+suffix+"TrigCaloCellMaker_eGamma")
            self.TrigCaloTowerMaker_eGamma=TrigCaloTowerMaker_eGamma("Cosmic"+suffix+"TrigCaloTowerMaker_eGamma")
            self.TrigCaloClusterMaker_slw=TrigCaloClusterMaker_slw("Cosmic"+suffix+"TrigCaloClusterMaker_slw")
            self.TrigCaloClusterMaker_slw.trigslw.e_threshold=-1.*GeV

            from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
            self.EF_Dummy = DummyFEX("EF_%s" % suffix)
                
            self.EFSequenceHash["Cosmic_"+suffix+"EF_g10calo"]=[self.TrigCaloCellMaker_eGamma,self.TrigCaloTowerMaker_eGamma,self.TrigCaloClusterMaker_slw]
            self.EFSequenceList+=["Cosmic_"+suffix+"EF_g10calo"]
            self.EFSequenceHash["Cosmic_"+suffix+"EF_g10"]=[self.TrigEgammaRec_NoIDEF_eGamma,self.EFPhotonHypo_g10_loose]
            self.EFSequenceList+=["Cosmic_"+suffix+"EF_g10"]
            self.EFSequenceHash[self.EF_Dummy.name()]=[self.EF_Dummy]
            self.EFSequenceList+=[self.EF_Dummy.name()]

    class Config(L2EFChain_CosmicTemplate.Config):
        def __init__(self, L2Config, EFConfig):
            L2EFChain_CosmicTemplate.Config.__init__(self, L2Config, EFConfig)
            pass

    def defineStreamGroupTriggerType(self):
        self.groups = ['RATE:CosmicSlice', 'RATE:Cosmic_Calo']
        pass
    
Config   = L2EFChain_CosmicEgamma.Config
L2create = L2EFChain_CosmicEgamma.L2Config
EFcreate = L2EFChain_CosmicEgamma.EFConfig

Cosmics += [

    L2EFChain_CosmicEgamma("Egamma_V2",  5004,
                             TriggerFlags.CosmicSlice.CaloSignature_V2() or defaultSignatures['CaloSignature_V2'],
                             [TriggerFlags.CosmicSlice.CaloThreshold_V2() or defaultThresholds['CaloThreshold_V2']],
                             Config(L2create("Egamma_V2"),
                                    EFcreate("Egamma_V2")))
    ]

###################################################################################
class L2EFChain_CosmicJet(L2EFChain_CosmicTemplate):
    
    def __init__(self, sig_id, chain_counter, l2_lower_chain_name, l2_inputTEs, config,
                 prescale=1, pass_through=0):
        self._supported = ["Jet_V2"]
        L2EFChain_CosmicTemplate.__init__(self, sig_id, chain_counter, l2_lower_chain_name, l2_inputTEs, config,
                 prescale=1, pass_through=0)
        mlog = logging.getLogger( 'CosmicDef.py:L2EFChain_CosmicJet' )

    class L2Config(L2EFChain_CosmicTemplate.L2Config):
        def __init__(self, suffix):
            L2EFChain_CosmicTemplate.L2Config.__init__(self,suffix)
            from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloJet_Jet
            self.l2_T2CaloJet_Jet = T2CaloJet_Jet("Cosmic"+suffix+"T2CaloJet_Jet")
            self.l2_T2CaloJet_Jet.jetOutputKey = "TrigT2CosmicJet"
            
            from TrigJetHypo.TrigJetHypoConfig import L2JetHypo
            self.l2_TrigL2JetHypo_Jet = L2JetHypo("Cosmic"+suffix+"L2JetHypo")
            self.l2_TrigL2JetHypo_Jet.Etcut_L2 = -1*GeV
            
            self.L2SequenceHash[self.l2_T2CaloJet_Jet.name()]=[self.l2_T2CaloJet_Jet,self.l2_TrigL2JetHypo_Jet]
            self.L2SequenceList+=[self.l2_T2CaloJet_Jet.name()]
            

    class EFConfig(L2EFChain_CosmicTemplate.EFConfig):
        def __init__(self, suffix):
            L2EFChain_CosmicTemplate.EFConfig.__init__(self,suffix)
            from TrigJetRec.TrigJetRecConfig import TrigJetRec_Getter
            self.TrigJetRec_Cone=TrigJetRec_Getter('TrigCosmicJetRec_Cone','Cone','H1Tower',0.7)
            self.TrigJetRec_Cone.jetInputKey="TrigT2CosmicJet"
            self.TrigJetRec_Cone.jetOutputKey= "TrigCosmicJetRec"        

            
            from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_jet, TrigCaloTowerMaker_jet, TrigCaloClusterMaker_slw
            self.TrigCaloCellMaker_jet =TrigCaloCellMaker_jet("Cosmic"+suffix+"TrigCaloCellMaker_jet")
            self.TrigCaloTowerMaker_jet=TrigCaloTowerMaker_jet("Cosmic"+suffix+"TrigCaloTowerMaker_jet")
            
            from TrigJetHypo.TrigJetHypoConfig import EFJetHypo
            self.EFJetHypo = EFJetHypo("Cosmic"+suffix+"EFJetHypo_j10",ef_thr=-1*GeV)
            
            self.EFSequenceHash["Cosmic_"+suffix+"j10"]=[self.TrigCaloCellMaker_jet,self.TrigCaloTowerMaker_jet,self.TrigJetRec_Cone,self.EFJetHypo]
            self.EFSequenceList+=["Cosmic_"+suffix+"j10"]


    class Config(L2EFChain_CosmicTemplate.Config):
        def __init__(self, L2Config, EFConfig):
            L2EFChain_CosmicTemplate.Config.__init__(self, L2Config, EFConfig)
            pass

    def defineStreamGroupTriggerType(self):
        self.groups = ['RATE:CosmicSlice', 'RATE:Cosmic_Jet']
        pass
    
Config   = L2EFChain_CosmicJet.Config
L2create = L2EFChain_CosmicJet.L2Config
EFcreate = L2EFChain_CosmicJet.EFConfig

Cosmics += [

    L2EFChain_CosmicJet("Jet_V2",  5005,
                        TriggerFlags.CosmicSlice.CaloSignature_V2() or defaultSignatures['CaloSignature_V2'],
                        [TriggerFlags.CosmicSlice.CaloThreshold_V2() or defaultThresholds['CaloThreshold_V2']],
                        Config(L2create("Jet_V2"),
                               EFcreate("Jet_V2")))
    
    ]


###################################################################################
class L2EFChain_CosmicMufast(L2EFChain_CosmicTemplate):
    
    def __init__(self, sig_id, chain_counter, l2_lower_chain_name, l2_inputTEs, config,
                 prescale=1, pass_through=0):
        self._supported = ["Mufast_MuIso_V2","Mufast_V2","Mufast_MuonEF_V2"]
        L2EFChain_CosmicTemplate.__init__(self, sig_id, chain_counter, l2_lower_chain_name, l2_inputTEs, config,
                 prescale=1, pass_through=0)
        mlog = logging.getLogger( 'CosmicDef.py:L2EFChain_CosmicMufast' )

    class L2Config(L2EFChain_CosmicTemplate.L2Config):
        def __init__(self, suffix):
            L2EFChain_CosmicTemplate.L2Config.__init__(self,suffix)

            from TrigMuonHypo.TrigMuonHypoConfig import MufastHypoConfig
            from TrigmuFast.TrigmuFastConfig import TrigmuFastConfig

            l1item = TriggerFlags.CosmicSlice.MuonSignature_V2() or defaultSignatures['MuonSignature_V2']
            if (l1item not in MuFastInstances):
                if (LocalFlags.useUniqueMuFastInstance):
                    self.TrigmuFastConfig = TrigmuFastConfig("Cosmic_TrigmuFastConfig_900GeV")
                    self.MufastHypoConfig = MufastHypoConfig("Cosmic_MufastHypoConfig_900Gev",'2GeV')
                    sequencename=l1item+self.TrigmuFastConfig.name()+suffix
                else:
                    self.TrigmuFastConfig = TrigmuFastConfig("Cosmic"+suffix+"TrigmuFastConfig_900GeV")
                    self.MufastHypoConfig = MufastHypoConfig("Cosmic"+suffix+"MufastHypoConfig_900Gev",'2GeV')
                    sequencename=self.TrigmuFastConfig.name()

                self.L2SequenceHash[sequencename]=[self.TrigmuFastConfig,self.MufastHypoConfig]
                self.L2SequenceList+=[sequencename]

                MuFastInstances[l1item]=[self.TrigmuFastConfig,self.MufastHypoConfig]
                self.ownMufast=True
                
            else:
                self.L2SequenceHash[l1item+MuFastInstances[l1item][0].name()+suffix]=MuFastInstances[l1item]
                self.L2SequenceList+=[l1item+MuFastInstances[l1item][0].name()+suffix]

                self.ownMufast=False
                                    
            if (suffix.count('MuIso')):
                from TrigMuonHypo.TrigMuonHypoConfig import MuisoHypoConfig
                self.MuisoHypoConfig=MuisoHypoConfig("Cosmic"+suffix+"MuisoHypoConfig")

                from TrigmuIso.TrigmuIsoConfig import muIsoConfig
                self.muIsoConfig=muIsoConfig("Cosmic"+suffix+"muIsoConfig")
                self.muIsoConfig.UseCombinedMuon = False
                
                self.L2SequenceHash[self.muIsoConfig.name()]=[self.muIsoConfig,self.MuisoHypoConfig]
                self.L2SequenceList+=[self.muIsoConfig.name()]
                
            if (self.ownMufast):
                muFast=self.TrigmuFastConfig
                muFast.OnlineRun=LocalFlags.doMuonCalibrationStream
                muFast.UseCalibration=True
                muFast.MaskUncCMAch = False
                muFast.CosmicRun   = True
                muFast.Timing      = False

            #if (suffix.count('MuIso')):
            #    include("TrigT2CaloCommon/LArCellBuilding.py")

    class EFConfig(L2EFChain_CosmicTemplate.EFConfig):
        def __init__(self, suffix):
            L2EFChain_CosmicTemplate.EFConfig.__init__(self,suffix)

            if (suffix.count('MuIso')):
                from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
                self.EF_Dummy = DummyFEX("EF_%s" % suffix)

                self.EFSequenceHash[self.EF_Dummy.name()]=[self.EF_Dummy]
                self.EFSequenceList+=[self.EF_Dummy.name()]

            elif (suffix.count('MuonEF')):
                from TrigMuonEF.TrigMuonEFConfig import TrigMuonEFSegmentFinderConfig,TrigMuonEFTrackBuilderConfig

                if (LocalFlags.useUniqueMuonEFInstance):
                    self.ef_TrigMuonEFSegmentFinderConfig = TrigMuonEFSegmentFinderConfig('Cosmic'+'TrigMuonEFSegmentFinderConfig')
                    self.ef_TrigMuonEFTrackBuilderConfig  =  TrigMuonEFTrackBuilderConfig('Cosmic'+'TrigMuonEFTrackBuilderConfig')
                else:
                    self.ef_TrigMuonEFSegmentFinderConfig = TrigMuonEFSegmentFinderConfig('Cosmic'+suffix+'TrigMuonEFSegmentFinderConfig')
                    self.ef_TrigMuonEFTrackBuilderConfig  =  TrigMuonEFTrackBuilderConfig('Cosmic'+suffix+'TrigMuonEFTrackBuilderConfig')

                l1item = TriggerFlags.CosmicSlice.MuonSignature_V2() or defaultSignatures['MuonSignature_V2']
                self.EFSequenceHash[self.ef_TrigMuonEFSegmentFinderConfig.name()+"_"+l1item+"_"+suffix]=[self.ef_TrigMuonEFSegmentFinderConfig]
                self.EFSequenceList+=[self.ef_TrigMuonEFSegmentFinderConfig.name()+"_"+l1item+"_"+suffix]
                
                self.EFSequenceHash[self.ef_TrigMuonEFTrackBuilderConfig.name()+"_"+l1item+"_"+suffix]=[self.ef_TrigMuonEFTrackBuilderConfig]
                self.EFSequenceList+=[self.ef_TrigMuonEFTrackBuilderConfig.name()+"_"+l1item+"_"+suffix]

    class Config(L2EFChain_CosmicTemplate.Config):
        def __init__(self, L2Config, EFConfig):
            L2EFChain_CosmicTemplate.Config.__init__(self, L2Config, EFConfig)
            pass

    def defineStreamGroupTriggerType(self):
        self.groups = ['RATE:CosmicSlice', 'RATE:Cosmic_Muon']
        pass
    
Config   = L2EFChain_CosmicMufast.Config
L2create = L2EFChain_CosmicMufast.L2Config
EFcreate = L2EFChain_CosmicMufast.EFConfig

Cosmics += [

# comment out to remove the MuonDef_v2v3.py   
#    L2EFChain_CosmicMufast("Mufast_V2",  5009,
#                           TriggerFlags.CosmicSlice.MuonSignature_V2() or defaultSignatures['MuonSignature_V2'],
#                           [TriggerFlags.CosmicSlice.MuonThreshold_V2() or defaultThresholds['MuonThreshold_V2']],
#                           Config(L2create("Mufast_V2"),
#                                  EFcreate("Mufast_V2"))),

#    L2EFChain_CosmicMufast("Mufast_MuIso_V2",  5010,
#                           TriggerFlags.CosmicSlice.MuonSignature_V2() or defaultSignatures['MuonSignature_V2'],
#                           [TriggerFlags.CosmicSlice.MuonThreshold_V2() or defaultThresholds['MuonThreshold_V2']],
#                           Config(L2create("Mufast_MuIso_V2"),
#                                  EFcreate("Mufast_MuIso_V2"))),

#    L2EFChain_CosmicMufast("Mufast_MuonEF_V2",  5011,
#                           TriggerFlags.CosmicSlice.MuonSignature_V2() or defaultSignatures['MuonSignature_V2'],
#                           [TriggerFlags.CosmicSlice.MuonThreshold_V2() or defaultThresholds['MuonThreshold_V2']],
#                           Config(L2create("Mufast_MuonEF_V2"),
#                                  EFcreate("Mufast_MuonEF_V2")))
    ]



###################################################################################
class L2EFChain_CosmicTileLookForMu(L2EFChain_CosmicTemplate):
    def __init__(self, sig_id, chain_counter, l2_lower_chain_name, l2_inputTEs, config,
                 prescale=1, pass_through=0):
        self._supported = ["CaloTileLookForMu_V2", "CaloTileRODMu_V2"]
        L2EFChain_CosmicTemplate.__init__(self, sig_id, chain_counter, l2_lower_chain_name, l2_inputTEs, config,
                 prescale=1, pass_through=0)
        
    class L2Config(L2EFChain_CosmicTemplate.L2Config):
        def __init__(self, suffix):
            L2EFChain_CosmicTemplate.L2Config.__init__(self,suffix)

            from TrigTileMuId.TrigTileLookForMuAlgConfig import TrigTileLookForMuAlg_L2
            self.TrigTileLookForMuAlg = TrigTileLookForMuAlg_L2("Cosmic"+suffix+"TileLookForMuAlg")

            self.L2SequenceHash[self.TrigTileLookForMuAlg.name()]=[self.TrigTileLookForMuAlg]
            self.L2SequenceList+=[self.TrigTileLookForMuAlg.name()]

    class EFConfig(L2EFChain_CosmicTemplate.EFConfig):
        def __init__(self, suffix):
            L2EFChain_CosmicTemplate.EFConfig.__init__(self,suffix)

            from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
            self.EF_Dummy = DummyFEX("EF_%s" % suffix)

            self.EFSequenceHash[self.EF_Dummy.name()]=[self.EF_Dummy]
            self.EFSequenceList+=[self.EF_Dummy.name()]
            pass
        
    class Config(L2EFChain_CosmicTemplate.Config):
        def __init__(self, L2Config, EFConfig):
            L2EFChain_CosmicTemplate.Config.__init__(self, L2Config, EFConfig)
            pass

    def defineStreamGroupTriggerType(self):
        self.calib_streams = ['Tile']
        self.groups = ['RATE:CosmicSlice', 'RATE:Cosmic_TileAlgos']
        pass

Config=L2EFChain_CosmicTileLookForMu.Config
L2create = L2EFChain_CosmicTileLookForMu.L2Config
EFcreate = L2EFChain_CosmicTileLookForMu.EFConfig

Cosmics += [
    
#   L2EFChain_CosmicTileLookForMu("CaloTileLookForMu_V2",  5044,
#                                  TriggerFlags.CosmicSlice.CaloSignature_V2() or defaultSignatures['CaloSignature_V2'],
#                                  [TriggerFlags.CosmicSlice.CaloThreshold_V2() or defaultThresholds['CaloThreshold_V2']],
#                                  Config(L2create("CaloTileLookForMu_V2"),
#                                         EFcreate("CaloTileLookForMu_V2"))),
    
#    L2EFChain_CosmicTileLookForMu("CaloTileRODMu_V2",  5045,
#                                   TriggerFlags.CosmicSlice.MuonSignature_V2() or defaultSignatures['MuonSignature_V2'],
#                                  [TriggerFlags.CosmicSlice.MuonThreshold_V2() or defaultThresholds['MuonThreshold_V2']],
#                                  Config(L2create("CaloTileRODMu_V2"),
#                                         EFcreate("CaloTileRODMu_V2")))
    
    ]

###################################################################################
class L2EFChain_CosmicTau(L2EFChain_CosmicTemplate):
    def __init__(self, sig_id, chain_counter, l2_lower_chain_name, l2_inputTEs, config,
                 prescale=1, pass_through=0):
        self._supported = ["Tau_V2"]
        L2EFChain_CosmicTemplate.__init__(self, sig_id, chain_counter, l2_lower_chain_name, l2_inputTEs, config,
                 prescale=1, pass_through=0)
        
    class L2Config(L2EFChain_CosmicTemplate.L2Config):
        def __init__(self, suffix):
            L2EFChain_CosmicTemplate.L2Config.__init__(self,suffix)

            from TrigT2CaloTau.TrigT2CaloTauConfig import T2CaloTau_Tau
            self.l2_T2CaloTau_Tau = T2CaloTau_Tau("Cosmic"+suffix+"T2CaloTau_Tau")

            from TrigTauHypo.TrigTauHypoConfig_tauNoCut import T2CaloTauHypo_tauNoCut
            self.l2_T2CaloTauHypo_tauNoCut=T2CaloTauHypo_tauNoCut("Cosmic"+suffix+"T2CaloTauHypo_tauNoCut")

            self.L2SequenceHash[self.l2_T2CaloTau_Tau.name()]=[self.l2_T2CaloTau_Tau,self.l2_T2CaloTauHypo_tauNoCut]
            self.L2SequenceList+=[self.l2_T2CaloTau_Tau.name()]
                
            include("TrigT2CaloCommon/LArCellBuilding.py")

    class EFConfig(L2EFChain_CosmicTemplate.EFConfig):
        def __init__(self, suffix):
            L2EFChain_CosmicTemplate.EFConfig.__init__(self,suffix)

            from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
            self.EF_Dummy = DummyFEX("Cosmic_EF_%s" % suffix)

            self.EFSequenceHash[self.EF_Dummy.name()]=[self.EF_Dummy]
            self.EFSequenceList+=[self.EF_Dummy.name()]

            pass
        
    class Config(L2EFChain_CosmicTemplate.Config):
        def __init__(self, L2Config, EFConfig):
            L2EFChain_CosmicTemplate.Config.__init__(self, L2Config, EFConfig)
            pass

    def defineStreamGroupTriggerType(self):
        self.groups = ['RATE:CosmicSlice', 'RATE:Cosmic_Tau']
        pass

Config=L2EFChain_CosmicTau.Config
L2create = L2EFChain_CosmicTau.L2Config
EFcreate = L2EFChain_CosmicTau.EFConfig

#Cosmics += [
#    
#    L2EFChain_CosmicTau("Tau_V2",  5046,
#                        TriggerFlags.CosmicSlice.CaloSignature_V2() or defaultSignatures['CaloSignature_V2'],
#                        [TriggerFlags.CosmicSlice.CaloThreshold_V2() or defaultThresholds['CaloThreshold_V2']],
#                        Config(L2create("Tau_V2"),
#                               EFcreate("Tau_V2"))),
#    ]

###################################################################################
class L2EFChain_CosmicMBiasCalo(L2EFChain_CosmicTemplate):
    def __init__(self, sig_id, chain_counter, l2_lower_chain_name,
                 l2_inputTEs, config, prescale=1, pass_through=0):
        self._supported = ["MBiasCalo_EndCapA_V2","MBiasCalo_EndCapC_V2"]
        L2EFChain_CosmicTemplate.__init__(self, sig_id, chain_counter, l2_lower_chain_name, l2_inputTEs, config,
                 prescale=1, pass_through=0)
        
    class L2Config(L2EFChain_CosmicTemplate.L2Config):
        def __init__(self, suffix):
            L2EFChain_CosmicTemplate.L2Config.__init__(self,suffix)

            from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_eGamma
            self.l2_T2CaloEgamma_eGamma = T2CaloEgamma_eGamma("Cosmic"+suffix[1:]+"T2CaloEgama_eGamma")
            self.l2_T2CaloEgamma_eGamma.EtaWidth = 0.2
            self.l2_T2CaloEgamma_eGamma.PhiWidth = 1.6

            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_g7
            self.l2_L2CaloHypo_g7 = L2CaloHypo_g7("Cosmic"+suffix[1:]+"L2CaloHypo_g7")
            self.l2_L2CaloHypo_g7.ETthr = [-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV]
            self.l2_L2CaloHypo_g7.AcceptAll=False
            
            from TrigEgammaHypo.TrigL2PhotonFexConfig import L2PhotonFex_1
            self.l2_L2PhotonFex_1 = L2PhotonFex_1("Cosmic"+suffix[1:]+"L2PhotonFex_1")

            from TrigEgammaHypo.TrigL2PhotonHypoConfig import L2PhotonHypo_g10_loose
            self.l2_L2PhotonHypo_g10_loose = L2PhotonHypo_g10_loose("Cosmic"+suffix[1:]+"L2PhotonHypo_g10_loose")
            self.l2_L2PhotonHypo_g10_loose.ETthr = [-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV]
            self.l2_L2PhotonHypo_g10_loose.AcceptAll=False
            
            self.L2SequenceHash[self.l2_T2CaloEgamma_eGamma.name()]=[self.l2_T2CaloEgamma_eGamma,self.l2_L2CaloHypo_g7]
            self.L2SequenceList+=[self.l2_T2CaloEgamma_eGamma.name()]
            self.L2SequenceHash["Cosmic"+suffix[1:]+"L2Photon"]=[self.l2_L2PhotonFex_1,self.l2_L2PhotonHypo_g10_loose]
            self.L2SequenceList+=["Cosmic"+suffix[1:]+"L2Photon"]
            
            include("TrigT2CaloCommon/LArCellBuilding.py")

    class EFConfig(L2EFChain_CosmicTemplate.EFConfig):
        def __init__(self, suffix):
            L2EFChain_CosmicTemplate.EFConfig.__init__(self,suffix)

            from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
            self.EF_Dummy = DummyFEX("Cosmic_EF_%s" % suffix[1:])

            self.EFSequenceHash[self.EF_Dummy.name()]=[self.EF_Dummy]
            self.EFSequenceList+=[self.EF_Dummy.name()]

    class Config(L2EFChain_CosmicTemplate.Config):
        def __init__(self, L2Config, EFConfig):
            L2EFChain_CosmicTemplate.Config.__init__(self, L2Config, EFConfig)
            pass

    def defineStreamGroupTriggerType(self):
        self.calib_streams = ['Tile']
        self.groups = ['RATE:CosmicSlice', 'RATE:Cosmic_MinbiasCalo']
        pass


Config   = L2EFChain_CosmicMBiasCalo.Config
L2create = L2EFChain_CosmicMBiasCalo.L2Config
EFcreate = L2EFChain_CosmicMBiasCalo.EFConfig

Cosmics += [

    L2EFChain_CosmicMBiasCalo("MBiasCalo_EndCapA_V2", 5048,
                              ','.join(MBTSItemList_SideA_V2),
                              MBTSThresholdList_SideA_V2,
                              Config(L2create("_MBiasCalo_EndCapA_V2"),
                                     EFcreate("_MBiasCalo_EndCapA_V2"))),
    

    L2EFChain_CosmicMBiasCalo("MBiasCalo_EndCapC_V2", 5049,
                              ','.join(MBTSItemList_SideC_V2),
                              MBTSThresholdList_SideC_V2,
                              Config(L2create("_MBiasCalo_EndCapC_V2"),
                                     EFcreate("_MBiasCalo_EndCapC_V2")))
    ]


###################################################################################
class L2EFChain_CosmicRateAnalysis(L2EFChain_CosmicTemplate):
    def __init__(self, sig_id, chain_counter, l2_lower_chain_name, l2_inputTEs, config,
                 prescale=1, pass_through=0):
        self._supported = ["Cosmic_rateanalysis"]
        L2EFChain_CosmicTemplate.__init__(self, sig_id, chain_counter, l2_lower_chain_name, l2_inputTEs, config,
                 prescale=1, pass_through=0)

    class L2Config(L2EFChain_CosmicTemplate.L2Config):
        def __init__(self, suffix):
            L2EFChain_CosmicTemplate.L2Config.__init__(self,suffix)

            from TrigDetCalib.TrigDetCalibConfig import TrigSubDetListWriter
            self.l2_rateListWriter = TrigSubDetListWriter("Cosmic"+suffix+"RateListWriter")
            self.l2_rateListWriter.Subdetectors = ""
            self.l2_rateListWriter.extraROBs = [ 7798784 ]
            
            self.L2SequenceHash[self.l2_rateListWriter.name()] = [self.l2_rateListWriter]
            self.L2SequenceList+=[self.l2_rateListWriter.name()]
            pass
        
    class EFConfig(L2EFChain_CosmicTemplate.EFConfig):
        def __init__(self, suffix):
            L2EFChain_CosmicTemplate.EFConfig.__init__(self,suffix)

            from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
            self.EF_Dummy = DummyFEX("EF_%s" % suffix)

            self.EFSequenceHash[self.EF_Dummy.name()]=[self.EF_Dummy]
            self.EFSequenceList+=[self.EF_Dummy.name()]
            pass

    class Config(L2EFChain_CosmicTemplate.Config):
        def __init__(self, L2Config, EFConfig):
            L2EFChain_CosmicTemplate.Config.__init__(self, L2Config, EFConfig)
            pass


    def defineStreamGroupTriggerType(self):
        self.groups = ["RATE:CosmicSlice","RATE:Cosmic_RateAnalysis"]
        pass


Config   = L2EFChain_CosmicRateAnalysis.Config
L2create = L2EFChain_CosmicRateAnalysis.L2Config
EFcreate = L2EFChain_CosmicRateAnalysis.EFConfig
Cosmics += [

    L2EFChain_CosmicRateAnalysis("Cosmic_rateanalysis", 5052,
                            '',[''],
                            Config(L2create("Cosmic_rateanalysis"),
                                   EFcreate("Cosmic_rateanalysis")))]

###################################################################################
###################################################################################
#We use a different, common definition for the AllTE chains 
#
from TrigHypoCommonTools.TrigHypoCommonToolsConf import L1InfoHypo
from TrigIDSCAN.TrigIDSCAN_Config import TrigIDSCAN_Cosmics
from TrigL2CosmicMuonHypo.TrigL2CosmicMuonHypo_Config import CosmicTrtHypo_Cosmic  
from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
from TrigSiTrack.TrigSiTrack_Config import TrigSiTrack_Cosmics
from TrigL2TRTSegFinder.TrigTRTSegFinder_Config import TrigTRTSegFinder_Cosmics_NewAlgo_NoField
from TrigL2TRTSegFinder.TrigTRTSegFinder_Config import TrigTRTSegFinder_Cosmics_NewAlgo
from TrigDetCalib.TrigDetCalibConfig import TrkROBListWriter
from TrigL2CosmicMuonHypo.TrigL2CosmicMuonHypo_Config import CosmicTrtHypo_Cosmic
from InDetTrigRecExample.EFInDetConfig import TrigEFIDInsideOut_CosmicsN
from TrigMinBias.TrigMinBiasConfig import MbTrkFex_1,MbTrkHypo_1
from TrigT2MinBias.TrigT2MinBiasConfig import MbSpFex_1, MbSpHypo_1
from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX


from TrigDetCalib.TrigDetCalibConf import TrigSubDetListWriter

class ID_SubDetListWriter(TrigSubDetListWriter):
  __slots__ = []
  def __init__(self, name):
    super( ID_SubDetListWriter, self ).__init__( name )
    self.SubdetId= ["HLTResult","TDAQ_CTP","InnerDetector"]
    self.MaxRoIsPerEvent = 1
#    self.addL1Calo = True

theTrigIDSubDetListWriter = ID_SubDetListWriter('TrigSubDetListWriter')





class L2EFChain_CosmicAllTETemplate(L2EFChainDef):

    def __init__(self, sig_id,
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter,
                 l2_inputTEs, config):
        
        L2EFChainDef.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter,
                              l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config)
        
    class L2Config:
        def __init__(self, suffix):
            self.suffix = suffix
            
    class EFConfig:
        def __init__(self, suffix):
            self.suffix = suffix

    class Config:
        def __init__(self, L2config, EFconfig):
            self.L2Config = L2config
            self.EFConfig = EFconfig

    def defineSequences(self, config):
        #The common L2 sequence
        from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo
        theAllTEDummyFakeROI = PESA__DummyUnseededAllTEAlgo("Cosmic"+self.config.L2Config.suffix+"AllTEDummy")
#        theAllTEDummyFakeROI = Cosmic__DummyUnseededAllTEAlgo("Cosmic"+self.config.L2Config.suffix+"AllTEDummy")
#        theAllTEDummyFakeROI.FakeROIEta=0.0
#        theAllTEDummyFakeROI.FakeROIPhi=0.0

#        if (self.config.L2Config.suffix.count('EFID')):
#            theAllTEDummyFakeROI.FakeROIEta=-0.5
#            theAllTEDummyFakeROI.FakeROIPhi= 1.5

#        if (self.config.L2Config.suffix.count('AllTeMBSpacePoints')):
#            theAllTEDummyFakeROI.CheckForInputTE=True

        theL1InfoHypo = L1InfoHypo("L1InfoHypo"+self.config.L2Config.suffix)
        theL1InfoHypo.TriggerTypeBitMask=0x80
        theL1InfoHypo.TriggerTypeBit=0x80
        theL1InfoHypo.InvertSelection=False
        if (self.config.L2Config.suffix.count('AllPhysics')):
            # Run on every physics trigger
            theL1InfoHypo.TriggerTypeBitMask=0x80
            theL1InfoHypo.TriggerTypeBit    =0x80
            theL1InfoHypo.InvertSelection   =False
                    
        theL1InfoHypoRNDMReject = L1InfoHypo("L1InfoHypoRNDMReject"+self.config.L2Config.suffix) 
        theL1InfoHypoRNDMReject.InvertSelection=True
        theL1InfoHypoRNDMReject.TriggerTypeBit = 0x81
        theL1InfoHypoRNDMReject.TriggerTypeBitMask=0xff

#        self.addL2Sequence(self.inputTEs,
#                           [theL1InfoHypo, theL1InfoHypoRNDMReject],
#                           'L2_ih')

        self.addL2Sequence(self.inputTEs,
                           [theAllTEDummyFakeROI, theL1InfoHypo, theL1InfoHypoRNDMReject],
                           'L2_ih')

    def defineStreamGroupTriggerType(self):
        if (self.config.L2Config.suffix.count("AllTeTRTxK_TRTTrkHypo_Pixel")):
            self.physics_streams = ['IDCosmics']
        else:
            if self.config.L2Config.suffix.count('TrkHypo'):
                self.physics_streams = ['IDCosmics']
            else:
                self.physics_streams = ['DISCARD']
                
        if (self.config.L2Config.suffix.count("AllTeMBSpacePoints_V2")):  
            self.groups = ['RATE:CosmicSlice', 'RATE:Cosmic_Minbias']
        else:
            self.groups = ['RATE:CosmicSlice', 'RATE:Cosmic_Tracking','BW:Detector']
        pass

Config   = L2EFChain_CosmicAllTETemplate.Config
L2create = L2EFChain_CosmicAllTETemplate.L2Config
EFcreate = L2EFChain_CosmicAllTETemplate.EFConfig

###################################################################################
#IDscan slice is specific case of L2EFChain_CosmicAllTETemplate
class L2EFChain_CosmicIDSCAN(L2EFChain_CosmicAllTETemplate):

    def defineSequences(self, config):

        #L2 SEQUENCES
        #Add the standard L2 sequence
        L2EFChain_CosmicAllTETemplate.defineSequences(self, config)

        #Add the idscan sequence
        theIdScanCosmics = TrigIDSCAN_Cosmics("CosmicIDSCANSequence"+self.config.L2Config.suffix)
        theIdScanCosmics.doTRTpropagation = True
        if (self.config.L2Config.suffix.count('4Hits')):
            theIdScanCosmics.MinHits = 4
            theIdScanCosmics.TrigHitFilter.LayerThreshold = 3.5
        else:
            theIdScanCosmics.MinHits = 3
            theIdScanCosmics.TrigHitFilter.LayerThreshold = 2.5

        self.addL2Sequence('L2_ih',
                           [theIdScanCosmics],
                           'L2_track')
        TE='L2_track'
                           
        #Use the trk hypo if required
        if(self.config.L2Config.suffix.count('TrkHypo')):
            theTrkhypoCosmics = CosmicTrtHypo_Cosmic("Cosmic"+self.config.L2Config.suffix+"TrkHypo") 
            theTrkhypoCosmics.NTrthitsCut = -1

            #add algo to sequence
            self.l2chain.sequences[1].algos.append(theTrkhypoCosmics)
            

        #EF SEQUENCES
        #Add the reject all seq if required (corresponds to discard stream)
        if (not self.config.EFConfig.suffix.count('TrkHypo')):
            
            theReject_Dummy = DummyFEX("Cosmic_RejectAllDummy_%s" % self.config.EFConfig.suffix)
                        
            theRejectAll = L1InfoHypo("RejectAll"+self.config.EFConfig.suffix )
            theRejectAll.TriggerTypeBitMask=0x0
            theRejectAll.TriggerTypeBit    =0x0
            theRejectAll.InvertSelection   =True

            self.addEFSequence(TE,
                               [theReject_Dummy,theRejectAll],
                               'EF_rej')
            TE='EF_rej'


        pass

    def defineSignatures(self):
        #Add L2 signatures
        self.addL2Signature(['L2_ih'])
        self.addL2Signature(['L2_track'])
        if(self.config.L2Config.suffix.count("Combined")):
            self.addL2Signature(['L2_comb'])
        #Add EF signatures
        if (not self.config.EFConfig.suffix.count('TrkHypo')):
            self.addEFSignature(['EF_rej'])

    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_ih':      "L2_Cosmic"+self.config.L2Config.suffix+"AllTEDummy",
            'L2_track':   "L2_CosmicIDSCANSequence"+self.config.L2Config.suffix,
            'L2_hits':    self.config.L2Config.suffix+"CosmicIDSCANIDHitsHypoL2",
            'EF_rej':     "EF_RejectAll"+self.config.EFConfig.suffix,
            'EF_hits':    self.config.EFConfig.suffix+"IDHitsHypo_IDSCAN_EF",
            }
        pass

Cosmics += [

    L2EFChain_CosmicIDSCAN("CosmicsAllTeIDSCAN",
                           "L2_CosmicsAllTeIDSCAN", 5023, '', "EF_CosmicsAllTeIDSCAN", 5023, '',
                           Config(L2create("CosmicsAllTeIDSCAN"),
                                  EFcreate("CosmicsAllTeIDSCAN"))),

    L2EFChain_CosmicIDSCAN("CosmicsAllTeIDSCAN_TrkHypo",
                           "L2_CosmicsAllTeIDSCAN_TrkHypo", 5024, '', "EF_CosmicsAllTeIDSCAN_TrkHypo", 5024, '',
                           Config(L2create("CosmicsAllTeIDSCAN_TrkHypo"),
                                  EFcreate("CosmicsAllTeIDSCAN_TrkHypo"))),

    L2EFChain_CosmicIDSCAN("CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo",
                           "L2_CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo", 5025, '', "EF_CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo", 5025, '',
                           Config(L2create("CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo"),
                                  EFcreate("CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo"))),

    L2EFChain_CosmicIDSCAN("CosmicsAllTeIDSCAN_AllPhysics_TrkHypo",
                           "L2_CosmicsAllTeIDSCAN_AllPhysics_TrkHypo", 5026, '', "EF_CosmicsAllTeIDSCAN_AllPhysics_TrkHypo", 5026, '',
                           Config(L2create("CosmicsAllTeIDSCAN_AllPhysics_TrkHypo"),
                                  EFcreate("CosmicsAllTeIDSCAN_AllPhysics_TrkHypo"))),

    ]




###################################################################################
#SiTrack slice is specific case of L2EFChain_CosmicAllTETemplate
class L2EFChain_CosmicSiTrack(L2EFChain_CosmicIDSCAN):

    def defineSequences(self, config):
        L2EFChain_CosmicIDSCAN.defineSequences(self, config)
        
        theSiTrackCosmics = TrigSiTrack_Cosmics("Cosmic"+self.config.L2Config.suffix+"TrigSiTrack") # here we created an instance
        if (self.config.L2Config.suffix.count('4Hits')):
            theSiTrackCosmics.Extension_SpacePoints = 4
        else:
            theSiTrackCosmics.Extension_SpacePoints = 3

        self.l2chain.sequences[1].algos[0] = theSiTrackCosmics

        
    def defineTErenaming(self):
        L2EFChain_CosmicIDSCAN.defineTErenaming(self)
        self.TErenamingMap['L2_track'] = "L2_Cosmic"+self.config.L2Config.suffix+"TrigSiTrack"

Cosmics += [

    L2EFChain_CosmicSiTrack("CosmicsAllTeSiTrack",
                           "L2_CosmicsAllTeSiTrack", 5028, '', "EF_CosmicsAllTeSiTrack", 5028, '',
                           Config(L2create("CosmicsAllTeSiTrack"),
                                  EFcreate("CosmicsAllTeSiTrack"))),

    L2EFChain_CosmicSiTrack("CosmicsAllTeSiTrack_TrkHypo",
                           "L2_CosmicsAllTeSiTrack_TrkHypo", 5029, '', "EF_CosmicsAllTeSiTrack_TrkHypo", 5029, '',
                           Config(L2create("CosmicsAllTeSiTrack_TrkHypo"),
                                  EFcreate("CosmicsAllTeSiTrack_TrkHypo"))),

    L2EFChain_CosmicSiTrack("CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo",
                           "L2_CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo", 5030, '', "EF_CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo", 5030, '',
                           Config(L2create("CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo"),
                                  EFcreate("CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo"))),

    L2EFChain_CosmicSiTrack("CosmicsAllTeSiTrack_AllPhysics_TrkHypo",
                           "L2_CosmicsAllTeSiTrack_AllPhysics_TrkHypo", 5031, '', "EF_CosmicsAllTeSiTrack_AllPhysics_TrkHypo", 5031, '',
                           Config(L2create("CosmicsAllTeSiTrack_AllPhysics_TrkHypo"),
                                  EFcreate("CosmicsAllTeSiTrack_AllPhysics_TrkHypo"))),

    ]



###################################################################################
#TRKxK slice is specific case of L2EFChain_CosmicAllTETemplate
class L2EFChain_CosmicTRTxK(L2EFChain_CosmicAllTETemplate):


    def defineStreamGroupTriggerType(self):
        
        if 'IDCalib' in self.sig_id:
            self.physics_streams = []
            self.calib_streams = ['IDTracks']
            self.groups = ['RATE:IDTracks','BW:Detector']
        else:
            self.physics_streams = ['IDCosmic']
            self.groups = ['RATE:CosmicSlice','BW:Detector']
             

        pass


    def defineSequences(self, config):

        #L2 SEQUENCES
        #Add the standard L2 sequence
        L2EFChain_CosmicAllTETemplate.defineSequences(self, config)

        #Add the segment finder sequence
        if (self.config.L2Config.suffix.count('NoField')):
            thetrtsegm  = TrigTRTSegFinder_Cosmics_NewAlgo_NoField("Cosmic"+self.config.L2Config.suffix+"TrigTRTSegFinder")
            thetrtsegm.SegmentsMakerTool.IsMagneticFieldOn = False
        else:
            thetrtsegm  = TrigTRTSegFinder_Cosmics_NewAlgo("Cosmic"+self.config.L2Config.suffix+"TrigTRTSegFinder")
            thetrtsegm.SegmentsMakerTool.IsMagneticFieldOn = True

        thetrtsegm.RoIhalfWidthDeltaPhi = 3.14
        thetrtsegm.RoIhalfWidthDeltaEta = 3.

        self.addL2Sequence('L2_ih',
                           [thetrtsegm],
                           'L2_seg')
        TE='L2_seg'
        
        #Use the trk hypo if required
        if(self.config.L2Config.suffix.count('TrkHypo')):
            theTrthypoCosmics = CosmicTrtHypo_Cosmic("Cosmic"+self.config.L2Config.suffix+"TrtHypo") 
            theTrthypoCosmics.NTrthitsCut = 15
            thetrtsegm.SegmentsMakerTool.MinimalNumberOfTRTHits = 15
            thetrtsegm.pTmin = 100.0
            if (self.config.L2Config.suffix.count('Pixel')):
                theTrthypoCosmics.TrtSegD0Cut=250.0
            if (self.config.L2Config.suffix.count('AllID')):
                theTrthypoCosmics.TrtSegD0Cut=600.0

            #add algo to sequence
            self.l2chain.sequences[1].algos.append(theTrthypoCosmics)

        #Use TrkROBListWriter if required
        if (self.config.L2Config.suffix.count('PEB')):
            therobListWriter = TrkROBListWriter('TrkROBListWriter')
            therobListWriter.RoIDescriptorLabels = [""]            

            #add algo to sequence
            self.l2chain.sequences[1].algos.append(therobListWriter)

        #Use SubDetListWriter if required
        if (self.config.L2Config.suffix.count('IDCalib')):
            #add algo to sequence
            self.addL2Sequence('L2_seg',
                               [theTrigIDSubDetListWriter],
                               'L2_peb')

        #EF SEQUENCES
        #Add the reject all seq if required (corresponds to discard stream)
        if (not self.config.EFConfig.suffix.count('TrkHypo')):
            
            theReject_Dummy = DummyFEX("Cosmic_RejectAllDummy_%s" % self.config.EFConfig.suffix)
            
            theRejectAll = L1InfoHypo("RejectAll"+self.config.EFConfig.suffix )
            theRejectAll.TriggerTypeBitMask=0x0
            theRejectAll.TriggerTypeBit    =0x0
            theRejectAll.InvertSelection   =True
            
            self.addEFSequence(TE,
                               [theReject_Dummy,theRejectAll],
                               'EF_rej')
            TE='EF_rej'


        pass

    def defineSignatures(self):
        #Add L2 signatures
        self.addL2Signature(['L2_ih'])
        self.addL2Signature(['L2_seg'])
        if (self.config.L2Config.suffix.count('IDCalib')):
            self.addL2Signature(['L2_peb'])
        #Add EF signatures

        if (self.config.EFConfig.suffix == None):
            pass
        else:
            if (not self.config.EFConfig.suffix.count('TrkHypo')):
                self.addEFSignature(['EF_rej'])

    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_ih':    "L2_Cosmic"+self.config.L2Config.suffix+"AllTEDummy",
            'L2_seg':   "L2_Cosmic"+self.config.L2Config.suffix+"TrigTRTSegFinder",
            'L2_peb':   "L2_Cosmic"+self.config.L2Config.suffix+"ID_PEB",
            'EF_rej':   "EF_RejectAll"+self.config.EFConfig.suffix,
            'EF_hits':  self.config.EFConfig.suffix+"IDHitsHypo_IDSCAN_EF",
            }
        pass


Cosmics += [

    L2EFChain_CosmicTRTxK("CosmicsAllTeTRTxK",
                          "L2_CosmicsAllTeTRTxK", 5033, '', "EF_CosmicsAllTeTRTxK", 5033, '',
                          Config(L2create("CosmicsAllTeTRTxK"),
                                 EFcreate("CosmicsAllTeTRTxK"))),

    L2EFChain_CosmicTRTxK("CosmicsAllTeTRTxK_TRTTrkHypo",
                          "L2_CosmicsAllTeTRTxK_TRTTrkHypo", 5038, '', "EF_CosmicsAllTeTRTxK_TRTTrkHypo", 5038, '',
                          Config(L2create("CosmicsAllTeTRTxK_TRTTrkHypo"),
                                 EFcreate("CosmicsAllTeTRTxK_TRTTrkHypo"))),
    
    L2EFChain_CosmicTRTxK("CosmicsAllTeTRTxK_TRTTrkHypo_Pixel",
                          "L2_CosmicsAllTeTRTxK_TRTTrkHypo_Pixel", 5034, '', "EF_CosmicsAllTeTRTxK_TRTTrkHypo_Pixel", 5034, '',
                          Config(L2create("CosmicsAllTeTRTxK_TRTTrkHypo_Pixel"),
                                 EFcreate("CosmicsAllTeTRTxK_TRTTrkHypo_Pixel"))),
    
    L2EFChain_CosmicTRTxK("CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics",
                          "L2_CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics", 5035, '', "EF_CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics", 5035, '',
                          Config(L2create("CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics"),
                                 EFcreate("CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics"))),
    
    L2EFChain_CosmicTRTxK("CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_PEB",
                          "L2_CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_PEB", 5036, '', "EF_CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_PEB", 5036, '',
                          Config(L2create("CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_PEB"),
                                 EFcreate("CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_PEB"))),
    
    L2EFChain_CosmicTRTxK("CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_NoField",
                          "L2_CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_NoField", 5037, '', "EF_CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_NoField", 5037, '',
                          Config(L2create("CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_NoField"),
                                 EFcreate("CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_NoField"))),
    
    L2EFChain_CosmicTRTxK("Cosmics_MU0_COMM_TRTxK_TRTTrkHypo_Pixel",
                          "L2_Cosmics_MU0_COMM_TRTxK_TRTTrkHypo_Pixel", 5038, 'L1_MU0_COMM_EMPTY', "EF_Cosmics_MU0_COMM_TRTxK_TRTTrkHypo_Pixel", 5038, 'MU0_COMM',
                          Config(L2create("Cosmics_MU0_COMM_TRTxK_TRTTrkHypo_Pixel"),
                                 EFcreate("Cosmics_MU0_COMM_TRTxK_TRTTrkHypo_Pixel"))),

    L2EFChain_CosmicTRTxK("Cosmics_MU0_COMM_TRTxK_TRTTrkHypo_AllID",
                          "L2_Cosmics_MU0_COMM_TRTxK_TRTTrkHypo_AllID", 5045, 'L1_MU0_COMM_EMPTY', "EF_Cosmics_MU0_COMM_TRTxK_TRTTrkHypo_AllID", 5045, 'MU0_COMM',
                          Config(L2create("Cosmics_MU0_COMM_TRTxK_TRTTrkHypo_AllID"),
                                 EFcreate("Cosmics_MU0_COMM_TRTxK_TRTTrkHypo_AllID"))),

    L2EFChain_CosmicTRTxK("Cosmics_MU11_TRTxK_TRTTrkHypo_Pixel",
                          "L2_Cosmics_MU11_TRTxK_TRTTrkHypo_Pixel", 5038, 'L1_MU11_EMPTY', "EF_Cosmics_MU11_TRTxK_TRTTrkHypo_Pixel", 5038, 'MU11',
                          Config(L2create("Cosmics_MU11_TRTxK_TRTTrkHypo_Pixel"),
                                 EFcreate("Cosmics_MU11_TRTxK_TRTTrkHypo_Pixel"))),

    L2EFChain_CosmicTRTxK("Cosmics_MU11_TRTxK_TRTTrkHypo_AllID",
                          "L2_Cosmics_MU11_TRTxK_TRTTrkHypo_AllID", 5045, 'L1_MU11_EMPTY', "EF_Cosmics_MU11_TRTxK_TRTTrkHypo_AllID", 5045, 'MU11',
                          Config(L2create("Cosmics_MU11_TRTxK_TRTTrkHypo_AllID"),
                                 EFcreate("Cosmics_MU11_TRTxK_TRTTrkHypo_AllID"))),

    L2EFChain_CosmicTRTxK("Cosmics_MU0_COMM_TRTxK_TRTTrkHypo_Pixel_IDCalib",
                          "L2_Cosmics_MU0_COMM_TRTxK_TRTTrkHypo_Pixel_IDCalib", 567, 'L1_MU0_COMM_EMPTY', '', None, 'MU0_COMM',
                          Config(L2create("Cosmics_MU0_COMM_TRTxK_TRTTrkHypo_Pixel_IDCalib"),
                                 EFcreate("None"))),


    ]


###################################################################################
#EFID slice is specific case of L2EFChain_CosmicAllTETemplate
class L2EFChain_CosmicEFID(L2EFChain_CosmicAllTETemplate):

    def defineSequences(self, config):

        #L2 SEQUENCES
        #Add the standard L2 sequence
        L2EFChain_CosmicAllTETemplate.defineSequences(self, config)
   
        #EF Sequences
        theEFIDTracking=TrigEFIDInsideOut_CosmicsN()

        #trk cnt algo
        thetrackcnt =  MbTrkFex_1("MbTrkFex_"+self.config.EFConfig.suffix)
        
        # hypo
        theefidcosmhypo = MbTrkHypo_1("MbTrkHypo_"+self.config.EFConfig.suffix)
        theefidcosmhypo.AcceptAll_EF=False
        theefidcosmhypo.Required_ntrks=1
        theefidcosmhypo.Max_z0=1000.0
        
        self.addEFSequence('L2_ih',
                           theEFIDTracking.getSequence()+
                           [thetrackcnt,
                           theefidcosmhypo],
                           'EF_efid')
        
        pass

    def defineSignatures(self):
        #Add L2 signatures
        self.addL2Signature(['L2_ih'])
        #Add EF signatures
        self.addEFSignature(['EF_efid'])

    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_ih':    "L2_Cosmic"+self.config.L2Config.suffix+"AllTEDummy",
            'EF_efid':  "EF_TrigEFIDInsideOut_CosmicsN",
            }
        pass


Cosmics += [
    
    L2EFChain_CosmicEFID("CosmicsAllTeEFID",
                         "L2_CosmicsAllTeEFID", 5039, '', "EF_CosmicsAllTeEFID", 5039, '',
                         Config(L2create("CosmicsAllTeEFID"),
                                EFcreate("CosmicsAllTeEFID"))),
    
    ]


###################################################################################
#MBSpacePoints slice is specific case of L2EFChain_CosmicAllTETemplate
class L2EFChain_CosmicMinBias(L2EFChain_CosmicAllTETemplate):

    def defineSequences(self, config):

        #L2 SEQUENCES
        #Add the standard L2 sequence
        L2EFChain_CosmicAllTETemplate.defineSequences(self, config)
        
        thel2_mbSpFex_DM = MbSpFex_1("MbSpFex_"+self.config.L2Config.suffix)
        thel2_mbSpFex_DM.PixelModuleThreshold = 50
        thel2_mbSpFex_DM.SCTModuleThreshold = 100
        thel2_mbSpFex_DM.MaxNModIdentifier = 100
                
        thel2_minbiashypo = MbSpHypo_1("MbSpHypo_"+self.config.L2Config.suffix)
        thel2_minbiashypo.AcceptAll = False
        thel2_minbiashypo.PixelCLToTCut = 20.
        thel2_minbiashypo.TriggerTypeAND = True
        thel2_minbiashypo.TotalPixelClus = -1.
        thel2_minbiashypo.PixelClusRatioA = -1.
        thel2_minbiashypo.PixelClusRatioB = -1.
        thel2_minbiashypo.PixelClusEndcapC = -1.
        thel2_minbiashypo.PixelClusBarrel = -1.
        thel2_minbiashypo.PixelClusEndcapA = -1.
        thel2_minbiashypo.PixelClusEndcapC_max = -1.
        thel2_minbiashypo.PixelClusBarrel_max = -1.
        thel2_minbiashypo.PixelClusEndcapA_max = -1.
        thel2_minbiashypo.TotalSctSp = -1.
        thel2_minbiashypo.SctSpEndcapC = -1.
        thel2_minbiashypo.SctSpBarrel = -1.
        thel2_minbiashypo.SctSpEndcapA = -1.
        thel2_minbiashypo.SctSpEndcapC_max = -1.
        thel2_minbiashypo.SctSpBarrel_max = -1.
        thel2_minbiashypo.SctSpEndcapA_max = -1.
        
        self.addL2Sequence('L2_ih',
                           [thel2_mbSpFex_DM,
                            thel2_minbiashypo],
                           'L2_mb')
        
        #EF Sequences
        theReject_Dummy = DummyFEX("Cosmic_RejectAllDummy_%s" % self.config.EFConfig.suffix)
        theRejectAll = L1InfoHypo("RejectAll"+self.config.EFConfig.suffix )
        theRejectAll.TriggerTypeBitMask=0x0
        theRejectAll.TriggerTypeBit    =0x0
        theRejectAll.InvertSelection   =True
        self.addEFSequence('L2_mb',
                           [theReject_Dummy,theRejectAll],
                           'EF_rej')

        theEF_Dummy = DummyFEX("EF_%s" % self.config.EFConfig.suffix)
        self.addEFSequence('EF_rej',
                           [theEF_Dummy],
                           'EF_')

        pass

    def defineSignatures(self):
        #Add L2 signatures
        self.addL2Signature(['L2_ih'])
        self.addL2Signature(['L2_mb'])
        #Add EF signatures
        self.addEFSignature(['EF_rej'])
        self.addEFSignature(['EF_'])

    def defineTErenaming(self):
        self.TErenamingMap = {
            'L2_ih': "L2_Cosmic"+self.config.L2Config.suffix+"AllTEDummy",
            'L2_mb': "L2_MbSpFex_"+self.config.L2Config.suffix,
            'EF_rej':"EF_RejectAll"+self.config.EFConfig.suffix,
            'EF_':   "EF_%s" % self.config.EFConfig.suffix,
            }
        pass
    
    
Cosmics += [
        
    L2EFChain_CosmicMinBias("AllTeMBSpacePoints_V2",
                            "L2_AllTeMBSpacePoints_V2", 5047,
                            TriggerFlags.CosmicSlice.Signature_V2() or defaultSignatures['Signature_V2'],
                            "EF_AllTeMBSpacePoints_V2", 5047,
                            [TriggerFlags.CosmicSlice.Threshold_V2() or defaultThresholds['Threshold_V2']],
                            Config(L2create("AllTeMBSpacePoints_V2"),
                                   EFcreate("AllTeMBSpacePoints_V2"))),
    ]


###################################################################################

class L2EFChain_CosmicLArPreSNoise(L2EFChain_CosmicTemplate):
    def __init__(self, sig_id, chain_counter, l2_lower_chain_name, l2_inputTEs, config,
                 prescale=1, pass_through=0):
        self._supported = ["PreS_EM_loose","PreS_L1EM3_empty", "PreS_L1EM5_empty",
                           #"PreS_loose","PreS_tight", "PreS_loose_hi","PreS_tight_hi","PreS_L1EM5_empty"]
                           "PreS_loose","PreS_tight", "PreS_loose_hi","PreS_tight_hi","PreS_L1EM6_empty"]
        L2EFChain_CosmicTemplate.__init__(self, sig_id, chain_counter, l2_lower_chain_name, l2_inputTEs, config,
                 prescale=1, pass_through=0)
        
    class L2Config(L2EFChain_CosmicTemplate.L2Config):
        def __init__(self, suffix):
            L2EFChain_CosmicTemplate.L2Config.__init__(self,suffix)
            
            from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_AllEm
            from TrigEgammaHypo.TrigL2CaloHypoConfig import TrigL2CaloLayersHypo_PreS_080
            from TrigEgammaHypo.TrigL2CaloHypoConfig import TrigL2CaloLayersHypo_PreS_092
            
            if(suffix.count('loose')):
                self.L2SequenceHash['L2_TrigL2CaloLayersHypo_'+suffix+'_092']=[T2CaloEgamma_AllEm(),TrigL2CaloLayersHypo_PreS_092('TrigL2CaloLayersHypo_'+suffix+'_092')]
                self.L2SequenceList+=['L2_TrigL2CaloLayersHypo_'+suffix+'_092']
            else:    
                self.L2SequenceHash['L2_TrigL2CaloLayersHypo_'+suffix+'_080']=[T2CaloEgamma_AllEm(),TrigL2CaloLayersHypo_PreS_080('TrigL2CaloLayersHypo_'+suffix+'_080')]
                self.L2SequenceList+=['L2_TrigL2CaloLayersHypo_'+suffix+'_080']
            pass
        
    class Config(L2EFChain_CosmicTemplate.Config):
        def __init__(self, L2Config, EFConfig):
            L2EFChain_CosmicTemplate.Config.__init__(self, L2Config, EFConfig)
            pass

    def defineStreamGroupTriggerType(self):
        self.physics_streams = ['CosmicCalo']
        self.groups = ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector']
        pass

Config   = L2EFChain_CosmicLArPreSNoise.Config
L2create = L2EFChain_CosmicLArPreSNoise.L2Config
EFcreate = L2EFChain_CosmicLArPreSNoise.EFConfig

Cosmics += [

    L2EFChain_CosmicLArPreSNoise("PreS_loose", 5055,
                                 'L1_EM5_EMPTY',
                                 ['EM5'],
                                 Config(L2create("PreS_loose"),
                                        EFcreate("PreS_loose"))),
    
    L2EFChain_CosmicLArPreSNoise("PreS_tight", 5056,
                                 'L1_EM5_EMPTY',
                                 ['EM5'],
                                 Config(L2create("PreS_tight"),
                                        EFcreate("PreS_tight"))),

    L2EFChain_CosmicLArPreSNoise("PreS_loose_hi", 5055,
                                 'L1_EM3_EMPTY',
                                 ['EM3'],
                                 Config(L2create("PreS_loose"),
                                        EFcreate("PreS_loose"))),
    
    L2EFChain_CosmicLArPreSNoise("PreS_tight_hi", 5056,
                                 'L1_EM3_EMPTY',
                                 ['EM3'],
                                 Config(L2create("PreS_tight"),
                                        EFcreate("PreS_tight"))),

    L2EFChain_CosmicLArPreSNoise("PreS_EM_loose", 5055,
                                 'L1_EM5_EMPTY',
                                 ['EM5'],
                                 Config(L2create("PreS_EM_loose"),
                                        EFcreate("PreS_EM_loose"))),
    
    L2EFChain_CosmicLArPreSNoise("PreS_L1EM5_empty", 5056,
                                 'L1_EM5_EMPTY',
                                 ['EM5'],
                                 Config(L2create("PreS_L1EM5_empty"),
                                        EFcreate("PreS_L1EM5_empty"))),

    L2EFChain_CosmicLArPreSNoise("PreS_L1EM6_empty", 5067,
                                 'L1_EM6_EMPTY',
                                 ['EM6'],
                                 Config(L2create("PreS_L1EM6_empty"),
                                        EFcreate("PreS_L1EM6_empty"))),


    L2EFChain_CosmicLArPreSNoise("PreS_L1EM3_empty", 5065,
                                 'L1_EM3_EMPTY',
                                 ['EM3'],
                                 Config(L2create("PreS_L1EM3_empty"),
                                        EFcreate("PreS_L1EM3_empty"))),

    ]

###################################################################################

class L2EFChain_CosmicLArPreSNoiseHad(L2EFChain_CosmicTemplate):
    def __init__(self, sig_id, chain_counter, l2_lower_chain_name, l2_inputTEs, config,
                 prescale=1, pass_through=0):
        self._supported = ["PreS_TAU8_loose","PreS_L1TAU8_empty", "PreS_J10_loose", "PreS_L1J5_empty", "PreS_L1J10_empty", "PreS_L1J30_empty",
                           "PreS_L1J10_firstempty", "PreS_L1J30_firstempty"]
        L2EFChain_CosmicTemplate.__init__(self, sig_id, chain_counter, l2_lower_chain_name, l2_inputTEs, config,
                 prescale=1, pass_through=0)
        
    class L2Config(L2EFChain_CosmicTemplate.L2Config):
        def __init__(self, suffix):
            L2EFChain_CosmicTemplate.L2Config.__init__(self,suffix)
            
            from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_All
            from TrigEgammaHypo.TrigL2CaloHypoConfig import TrigL2CaloLayersHypo_PreS_080 
            from TrigEgammaHypo.TrigL2CaloHypoConfig import TrigL2CaloLayersHypo_PreS_092 


            if(suffix.count('loose')):
                self.L2SequenceHash['L2_TrigL2CaloLayersHypo_'+suffix+'_092']=[T2CaloEgamma_All(),TrigL2CaloLayersHypo_PreS_092('TrigL2CaloLayersHypo_'+suffix+'_092')]
                self.L2SequenceList+=['L2_TrigL2CaloLayersHypo_'+suffix+'_092']
            else:
                self.L2SequenceHash['L2_TrigL2CaloLayersHypo_'+suffix+'_080']=[T2CaloEgamma_All(),TrigL2CaloLayersHypo_PreS_080('TrigL2CaloLayersHypo_'+suffix+'_080')]
                self.L2SequenceList+=['L2_TrigL2CaloLayersHypo_'+suffix+'_080']
            pass
        
    class Config(L2EFChain_CosmicTemplate.Config):
        def __init__(self, L2Config, EFConfig):
            L2EFChain_CosmicTemplate.Config.__init__(self, L2Config, EFConfig)
            pass

    def defineStreamGroupTriggerType(self):
        self.physics_streams = ['CosmicCalo']
        self.groups = ['RATE:CosmicSlice', 'RATE:Cosmic_LArCalibration', 'BW:Detector']
        pass

Config   = L2EFChain_CosmicLArPreSNoiseHad.Config
L2create = L2EFChain_CosmicLArPreSNoiseHad.L2Config
EFcreate = L2EFChain_CosmicLArPreSNoiseHad.EFConfig

Cosmics += [

    L2EFChain_CosmicLArPreSNoiseHad("PreS_TAU8_loose", 5057,
                                    'L1_TAU8_EMPTY',
                                    ['HA8'],
                                    Config(L2create("PreS_TAU8_loose"),
                                           EFcreate("PreS_TAU8_loose"))),
    
    L2EFChain_CosmicLArPreSNoiseHad("PreS_L1TAU8_empty", 5058,
                                    'L1_TAU8_EMPTY',
                                    ['HA8'],
                                    Config(L2create("PreS_L1TAU8_empty"),
                                           EFcreate("PreS_L1TAU8_empty"))),
                                        
    L2EFChain_CosmicLArPreSNoiseHad("PreS_J10_loose", 5059,
                                    'L1_J10_EMPTY',
                                    ['J10'],
                                    Config(L2create("PreS_J10_loose"),
                                           EFcreate("PreS_J10_loose"))),

    L2EFChain_CosmicLArPreSNoiseHad("PreS_L1J5_empty", 5059,
                                    'L1_J5_EMPTY',
                                    ['J5'],
                                    Config(L2create("PreS_L1J5_empty"),  
                                           EFcreate("PreS_L1J5_empty"))),

    L2EFChain_CosmicLArPreSNoiseHad("PreS_L1J10_empty", 5062,
                                    'L1_J10_EMPTY',
                                    ['J10'],
                                    Config(L2create("PreS_L1J10_empty"),
                                           EFcreate("PreS_L1J10_empty"))),

    L2EFChain_CosmicLArPreSNoiseHad("PreS_L1J30_empty", 5063,
                                    'L1_J30_EMPTY',
                                    ['J30'],
                                    Config(L2create("PreS_L1J30_empty"),
                                           EFcreate("PreS_L1J30_empty"))),

    L2EFChain_CosmicLArPreSNoiseHad("PreS_L1J10_firstempty", 5064,
                                    'L1_J10_FIRSTEMPTY',
                                    ['J10'],
                                    Config(L2create("PreS_L1J10_firstempty"),
                                           EFcreate("PreS_L1J10_firstempty"))),

    L2EFChain_CosmicLArPreSNoiseHad("PreS_L1J30_firstempty", 5066,
                                    'L1_J30_FIRSTEMPTY',
                                    ['J30'],
                                    Config(L2create("PreS_L1J30_firstempty"),
                                           EFcreate("PreS_L1J30_firstempty"))),

    ]
