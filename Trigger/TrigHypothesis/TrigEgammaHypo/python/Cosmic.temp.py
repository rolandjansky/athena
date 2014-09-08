# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from string import atoi
from AthenaCommon.Include import include
from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import GeV
from TriggerJobOpts.TriggerFlags  import TriggerFlags
from TriggerMenuPython.TriggerPythonConfig import *
from TriggerJobOpts.TriggerFlags  import TriggerFlags

log = logging.getLogger( 'Cosmic.py' )


def StreamTagName(TEName):
    return 'Cosmics_'+TEName



class LocalFlags:
    DontWriteOutAllTEStream=True
    UseSingleBeam7BitL1Menu=True
    UseSingleBeam3BitL1Menu=False
    UseStartup2009L1Menu=False
    Use3Streams=False
    T2CaloFakeLVL1InLVL2=False
    doIDHitsHypo=False
    doEFIDCosmics=False
    useUniqueMuonEFInstance=True
    doM3Data=True
    useUniqueMuFastInstance=True
    useUniqueMuonEFInstance=True
    doMuonCalibrationStream=False



L2SequencesList= {}
MuFastInstances = {}
TriggerTypes = {}
TriggerTypeMasks = {}

#MBTSThresholdList=['NIM11','NIM12','NIM13','NIM14','NIM15','NIM16','NIM17','NIM18']
MBTSThresholdList_SideA=[
                   'MBTS_A0',
                   'MBTS_A1',
                   'MBTS_A2',
                   'MBTS_A3',
                   'MBTS_A4',
                   'MBTS_A5',
                   'MBTS_A6',
                   'MBTS_A7',
                   'MBTS_A8',
                   'MBTS_A9',
                   'MBTS_A10',
                   'MBTS_A11',
                   'MBTS_A12',
                   'MBTS_A13',
                   'MBTS_A14',
                   'MBTS_A15',
                   ]

MBTSThresholdList_SideC=[
                   'MBTS_C0',
                   'MBTS_C1',
                   'MBTS_C2',
                   'MBTS_C3',
                   'MBTS_C4',
                   'MBTS_C5',
                   'MBTS_C6',
                   'MBTS_C7',
                   'MBTS_C8',
                   'MBTS_C9',
                   'MBTS_C10',
                   'MBTS_C11',
                   'MBTS_C12',
                   'MBTS_C13',
                   'MBTS_C14',
                   'MBTS_C15',
                   ]


MBTSItemList_SideA=[
                   'L1_MBTSA0',
                   'L1_MBTSA1',
                   'L1_MBTSA2',
                   'L1_MBTSA3',
                   'L1_MBTSA4',
                   'L1_MBTSA5',
                   'L1_MBTSA6',
                   'L1_MBTSA7',
                   'L1_MBTSA8',
                   'L1_MBTSA9',
                   'L1_MBTSA10',
                   'L1_MBTSA11',
                   'L1_MBTSA12',
                   'L1_MBTSA13',
                   'L1_MBTSA14',
                   'L1_MBTSA15',
                   ]

MBTSItemList_SideC=[
                   'L1_MBTSC0',
                   'L1_MBTSC1',
                   'L1_MBTSC2',
                   'L1_MBTSC3',
                   'L1_MBTSC4',
                   'L1_MBTSC5',
                   'L1_MBTSC6',
                   'L1_MBTSC7',
                   'L1_MBTSC8',
                   'L1_MBTSC9',
                   'L1_MBTSC10',
                   'L1_MBTSC11',
                   'L1_MBTSC12',
                   'L1_MBTSC13',
                   'L1_MBTSC14',
                   'L1_MBTSC15',
                   ]

MBTSThresholdList = MBTSThresholdList_SideC + MBTSThresholdList_SideA


MBTSItemList_SideA_V2=['L1_MBTS_A_EMPTY']
MBTSItemList_SideC_V2=['L1_MBTS_C_EMPTY']

MBTSThresholdList_SideA_V2=[
                   'MBTS_A0'
                   ]

MBTSThresholdList_SideC_V2=[
                   'MBTS_C'
                   ]

MBTSThresholdList_V2 = MBTSThresholdList_SideC_V2 + MBTSThresholdList_SideA_V2

#L1CaloThresholdList=['J1','J2','J3','J4','J5','J6','J7','J8',\
#                     'EM1','EM2','EM3','EM4','EM5','EM6','EM7',\
#                     'EM8','TAU1','TAU2','TAU3','TAU4','TAU5','TAU6','TAU7','TAU8',\
#                     'JE1','JE2','JE3','JE4',\
#                     'JF1','JF2','JF3','JF4',\
#                     'JE','TE','XE'
#                     ]


#L1CaloThresholdList_J=['J5','J10','J15','J20','J30','J40','J50','J100']
#L1CaloThresholdList_EM=['EM5','EM10','EM15','EM20','EM30','EM40','EM50','EM100']
#L1CaloThresholdList_TAU=['TAU5','TAU10','TAU15','TAU20','TAU30','TAU40','TAU50','TAU100']
#L1CaloThresholdList_JB=['JB5','JB10','JB15','JB20']
#L1CaloThresholdList_JF=['JF5','JF10','JF15','JF20']
#L1CaloThresholdList_TE=['TE1','TE2','TE3','TE4']
#L1CaloThresholdList_XE=['XE1','XE2','XE3','XE4','XE5','XE6','XE7','XE8']

L1CaloThresholdList_J=['J5','J10','J10_win6','J18', 'J20', 'J23', 'J40', 'J35','J42','J70', 'J100', 'J130' ]
L1CaloThresholdList_EM=['EM3','EM4','EM7','EM10','EM13','EM13I','EM18','EM7I']
L1CaloThresholdList_TAU=['TAU5','TAU6','TAU9','TAU9I','TAU16','TAU16I','TAU40','TAU25']
L1CaloThresholdList_JB=['JB18','JB3']
L1CaloThresholdList_JF=['JF18','JF3']
L1CaloThresholdList_TE=['TE150','TE360','TE650']
L1CaloThresholdList_XE=['XE15', 'XE20','XE25','XE30','XE35','XE50','XE80']
L1CaloThresholdList_JE=['JE120','JE280','JE340']


L1CaloThresholdList=L1CaloThresholdList_J+L1CaloThresholdList_EM+L1CaloThresholdList_TAU+L1CaloThresholdList_JB+L1CaloThresholdList_JF+L1CaloThresholdList_TE+L1CaloThresholdList_XE+L1CaloThresholdList_JE

L1CaloThresholdList_NoXE=L1CaloThresholdList_J+L1CaloThresholdList_EM+L1CaloThresholdList_TAU+L1CaloThresholdList_JB+L1CaloThresholdList_JF+L1CaloThresholdList_TE+L1CaloThresholdList_JE


L1CaloItemList_J  =['L1_J5','L1_J10','L1_J10_win6','L1_J18', 'L1_J20', 'L1_J23', 'L1_J40', 'L1_J35','L1_J42','L1_J70', 'L1_J100', 'L1_J130' ]
L1CaloItemList_EM =['L1_EM3','L1_EM4','L1_EM7','L1_EM10','L1_EM13','L1_EM13I','L1_EM18','L1_EM7I']
L1CaloItemList_TAU=['L1_TAU5','L1_TAU6','L1_TAU9','L1_TAU9I','L1_TAU16','L1_TAU16I','L1_TAU40','L1_TAU25']
L1CaloItemList_JB =['L1_JB18','L1_JB3']
L1CaloItemList_JF =['L1_JF18','L1_JF3']
L1CaloItemList_TE =['L1_TE150','L1_TE360','L1_TE650']
L1CaloItemList_XE =['L1_XE15', 'L1_XE20','L1_XE25','L1_XE30','L1_XE35','L1_XE50','L1_XE80']
L1CaloItemList_JE =['L1_JE120','L1_JE280','L1_JE340']


L1CaloItemList=L1CaloItemList_J+L1CaloItemList_EM+L1CaloItemList_TAU+L1CaloItemList_JB+L1CaloItemList_JF+L1CaloItemList_TE+L1CaloItemList_XE+L1CaloItemList_JE

L1CaloItemList_NoXE=L1CaloItemList_J+L1CaloItemList_EM+L1CaloItemList_TAU+L1CaloItemList_JB+L1CaloItemList_JF+L1CaloItemList_TE+L1CaloItemList_JE

L1CaloItemList_J_V2=['L1_J5_EMPTY', 'L1_J10_EMPTY', 'L1_J10_win6_EMPTY',
                     'L1_J20_EMPTY', 'L1_J40_EMPTY', 'L1_J70_EMPTY',
                     'L1_J100_EMPTY']
L1CaloItemList_EM_V2  =['L1_EM3_EMPTY', 'L1_EM4_EMPTY', 'L1_EM7_EMPTY',
                        'L1_EM7I_EMPTY', 'L1_EM10_EMPTY', 'L1_EM13_EMPTY',
                        'L1_EM13I_EMPTY', 'L1_EM18_EMPTY', 'L1_2EM3_EMPTY',
                        'L1_2EM4_EMPTY', 'L1_2EM7_EMPTY', 'L1_2EM7I_EMPTY',
                        'L1_2EM10_EMPTY']
L1CaloItemList_TAU_V2 =['L1_TAU5_EMPTY', 'L1_TAU6_EMPTY', 'L1_TAU9_EMPTY',
                        'L1_TAU9I_EMPTY', 'L1_TAU16_EMPTY', 'L1_TAU16I_EMPTY', 'L1_TAU25_EMPTY',
                        'L1_2TAU9_EMPTY', 'L1_2TAU9I_EMPTY', 'L1_2TAU16_EMPTY',
                        'L1_2TAU16I_EMPTY']
L1CaloItemList_JF_V2  =['L1_FJ3_EMPTY', 'L1_FJ18_EMPTY']
L1CaloItemList_TE_V2  =['L1_TE150_EMPTY', 'L1_TE360_EMPTY']
L1CaloItemList_XE_V2  =['L1_XE15_EMPTY', 'L1_XE25_EMPTY', 'L1_XE35_EMPTY',
                        'L1_XE50_EMPTY']
L1CaloItemList_JE_V2  =['L1_JE120_EMPTY', 'L1_JE280_EMPTY']

L1CaloItemList_NoXE_V2=L1CaloItemList_J_V2+L1CaloItemList_EM_V2+L1CaloItemList_TAU_V2+L1CaloItemList_JF_V2+L1CaloItemList_TE_V2+L1CaloItemList_JE_V2

#This hack is here to enable athena run combined from xml and python
defaultSignatures = {
    'Signature'                 : 'L1_MU6_RPC',# L1_MU6_EMPTY
    'StandaloneSignature'       : 'L1_EM3', # L1_EM3_EMPTY
    'CaloSignature'             : 'L1_J5', # L1_J5_EMPTY
    'RPCSignature'              : 'L1_MU6_RPC',# L1_MU6_EMPTY / L1_MU0_HIGH_RPC_EMPTY
    'TGCSignature'              : 'L1_MU0_TGC',# L1_MU0_EMPTY
    'TrackingSignature'         : 'L1_EM3', # L1_EM3_EMPTY
    'WidePatternRPCSignature'   : 'L1_MU0_LOW_RPC',# L1_MU0_EMPTY
    'CalibrationSignature'      : 'L1_EM3', # L1_EM#_EMPTY
    'Signature_V2'              : 'L1_MU6_EMPTY',
    'StandaloneSignature_V2'    : 'L1_EM3_EMPTY', 
    'CaloSignature_V2'          : 'L1_J5_EMPTY', 
    'MuonSignature_V2'          : 'L1_MU0_EMPTY',
    'TrackingSignature_V2'      : 'L1_EM3_EMPTY', 
    'CalibrationSignature_V2'   : 'L1_EM3_EMPTY' 
    }

defaultThresholds = {
    'Threshold'                 : 'MU6_RPC', # MU6
    'StandaloneThreshold'       : 'EM3',
    'CaloThreshold'             : 'J5',
    'RPCThreshold'              : 'MU6_RPC', # MU6
    'TGCThreshold'              : 'MU0_TGC', # MU0
    'TrackingThreshold'         : 'EM3',
    'WidePatternRPCThreshold'   : 'MU0_LOW_RPC', # MU0
    'CalibrationThreshold'      : 'EM3',
    'Threshold_V2'              : 'MU6',
    'StandaloneThreshold_V2'    : 'EM3', 
    'CaloThreshold_V2'          : 'J5', 
    'MuonThreshold_V2'          : 'MU0',
    'TrackingThreshold_V2'      : 'EM3', 
    'CalibrationThreshold_V2'   : 'EM3' 
    }


def LastFreeCounter(CID):
    if (CID!='-1'):
        globals()['LFC']=int(CID)
        if (TriggerFlags.CosmicSlice.testCosmic()==True):
            log.info('setting LFC')
    else:
        globals()['LFC']=globals()['LFC']+1
    return str(globals()['LFC'])

class CosmicTemplate:
    def __init__(self, sig_id, l1item, l1threshold, counterId,StreamName,streamtype='physics',L2prescale=1.0,EFprescale=1.0,groupNames=["Cosmic"],myExpressStreamPrescale=-1):

        counterId=LastFreeCounter(counterId)

        mlog = logging.getLogger( 'Cosmic.py:CosmicTemplate' )
        self.groupNames=groupNames
        self.sig_id = sig_id
        self.l1threshold = l1threshold
        self.TriggerTypeBit=int(counterId)


        self.L2prescale='%f' % L2prescale
        self.EFprescale='%f' % EFprescale

        if (TriggerFlags.CosmicSlice.testCosmic()==True):
            mlog.info("prescales for chain %s " % sig_id)
            mlog.info(self.L2prescale)
            mlog.info(self.EFprescale)
        
        self.L2SequenceList = []
        self.EFSequenceList = []

        self.L2SequenceHash = {}
        self.EFSequenceHash = {}

        #self.configLVL2=TriggerFlags.doLVL2() #WTF commenting this out since it was overwritten anyway
        self.configLVL2=True

        #self.configEF=TriggerFlags.doEF()
        self.configEF=True

        if (StreamName==''):
            if (type(l1threshold)==type([])):
                StreamName='HLT_Cosmics'
                for th in l1threshold:
                    StreamName+='_'+th
            else:
                StreamName='HLT_Cosmics_'+l1threshold

        if (l1threshold==TriggerFlags.CosmicSlice.CaloThreshold() and streamtype=='physics'):
            StreamName = "HLT_Cosmic_L1Calo"

        if (sig_id=="L1CaloFilter"):
            StreamName = "HLT_Cosmic_L1Calo"
        if (sig_id=="L1CaloEMFilter"):
            StreamName = "HLT_Cosmic_L1Calo"
        if (sig_id=="L1CaloTauFilter"):
            StreamName = "HLT_Cosmic_L1Calo"
        if (sig_id=="L1CaloJetFilter"):
            StreamName = "HLT_Cosmic_L1Calo"

        if (sig_id=="L1CaloPassthrough"):
            StreamName = "HLT_Cosmic_L1Calo"

        if (sig_id=="PhysicsPassthrough"):
            StreamName = "HLT_Cosmic_Physics"
            streamtype='physics'

        if (sig_id=="CalibrationPassthrough"):
            StreamName = "HLT_Cosmic_Calibration"
            streamtype='calibration'

        if (sig_id=="L2CalibrationPassthrough"):
            StreamName = "HLT_Cosmic_L2_Calibration"
            streamtype='calibration'

        for i in [3,4,5,7,10,13,15,18,20,30,40,50,100]:            
            if ((sig_id=='EM%i' % i+'Passthrough') or \
                (sig_id=='EM%iI' % i+'Passthrough')):
                StreamName = "HLT_Cosmic_EM_L1Calo"

        for i in [3,4,5,7,10,13,15,18,20,30,40,50,100]:
            if ((sig_id=='EM%i' % i+'Dummy') or \
                (sig_id=='EM%iI' % i+'Dummy')):
                StreamName = "HLT_Cosmic_EM_L1Calo"

        for i in [5,9,10,15,16,20,25,30,40,50,100]:
            if ((sig_id=='JET%i' % i+'Dummy') or \
                (sig_id=='TAU%i' % i+'Dummy') or \
                (sig_id=='JET%iI' % i+'Dummy') or \
                (sig_id=='TAU%iI' % i+'Dummy')):
                StreamName = "HLT_Cosmic_L1Calo"
        for i in [3,5,10,15,18,20,35]:
            if ((sig_id=='JB%i' % i+'Dummy') or \
                (sig_id=='JF%i' % i+'Dummy') or \
                (sig_id=='JB%iI' % i+'Dummy') or \
                (sig_id=='JF%iI' % i+'Dummy')):
                StreamName = "HLT_Cosmic_L1Calo"

        if (sig_id=="MBTSPassthrough"):
            StreamName = "HLT_Cosmic_MBTS"

        self.L2streamtype=streamtype
        self.EFstreamtype=streamtype

        if (sig_id=="EFCalibrationPassthrough"):
            StreamName = "HLT_Cosmic_EF_Calibration"
            self.L2streamtype='physics'
            self.EFstreamtype='calibration'

        L2StreamName=StreamName
        EFStreamName=StreamName

        if (sig_id.count("AllTe") and not sig_id.count("CosmicsAllTe")):
            StreamName = "HLT_Cosmic_AllTe"
            L2StreamName=StreamName
            if (LocalFlags.DontWriteOutAllTEStream):
                EFStreamName = ""
            else:
                EFStreamName=StreamName

        if (sig_id=="AllTeTRTxK_TRTTrkHypo"):
            StreamName = StreamName + "IDSelected"
            L2StreamName=StreamName
            EFStreamName=StreamName

        if (sig_id=="AllTeTRTxK_TRTTrkHypo_Pixel"):
            StreamName = StreamName + "Pixel"
            L2StreamName=StreamName
            EFStreamName=StreamName

        if (sig_id in ["CosmicsAllTeTRTxK",
                       "CosmicsAllTeTRTxK_TRTTrkHypo",
                       "CosmicsAllTeTRTxK_TRTTrkHypo_Pixel",
                       "CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics",        
                       "CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_NoField",        
                       "CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_PEB",
                       'CosmicsAllTeIDSCAN',         
                       'CosmicsAllTeSiTrack',        
                       'CosmicsAllTeIDSCAN_TrkHypo',         
                       'CosmicsAllTeSiTrack_TrkHypo',        
                       'CosmicsAllTeIDSCAN_AllPhysics_TrkHypo',         
                       'CosmicsAllTeSiTrack_AllPhysics_TrkHypo',        
                       'CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo',         
                       'CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo',        
                       'CosmicsAllTeIDSCANCombined', 
                       'CosmicsAllTeSiTrackCombined',
                       'CosmicsAllTeEFID']):
            StreamName = "IDSelected"
            L2StreamName=StreamName
            EFStreamName=''
            if (sig_id.count("Hypo")==1 or sig_id=='CosmicsAllTeEFID'):
                EFStreamName=StreamName

        if (sig_id=="TRTPassthrough" ):
            StreamName = "IDCosmic"
            L2StreamName=StreamName
            EFStreamName=StreamName

        if (sig_id=="TilePassthrough" ):
            StreamName = "CosmicMuons"
            L2StreamName=StreamName
            EFStreamName=StreamName

        if (sig_id=="Cosmic_pixelnoise" ):
            StreamName = "pixelnoise"
            L2StreamName=StreamName
            EFStreamName=StreamName
            streamtype='calibration'

        if (sig_id=="Cosmic_rateanalysis" ):
            StreamName = "rateanalysis"
            L2StreamName=StreamName
            EFStreamName=StreamName
            streamtype='calibration'
            
            

        ########################################  This part is the same for all sequences (begin) ############################################################
        # defining the L2 chain for signatures
        if self.configLVL2:
            if (sig_id.count("AllTe")):
                from TrigEFIDCosmic.TrigEFIDCosmicConf import Cosmic__DummyUnseededAllTEAlgo
                self.AllTEDummyFakeROI = Cosmic__DummyUnseededAllTEAlgo("Cosmic"+sig_id+"AllTEDummy")
                from TrigEFIDCosmic.TrigEFIDCosmicConf import L1InfoHypo

                self.L1InfoHypo = L1InfoHypo("L1InfoHypo"+sig_id) # here we created an instance
                self.L1InfoHypoRNDMReject = L1InfoHypo("L1InfoHypoRNDMReject"+sig_id) # here we created an instance

                self.L1InfoHypoRNDMReject.InvertSelection=True
                self.L1InfoHypoRNDMReject.TriggerTypeBit = 0x81
                self.L1InfoHypoRNDMReject.TriggerTypeBitMask=0xff
                self.L2SequenceHash[self.AllTEDummyFakeROI.name()]=[self.AllTEDummyFakeROI,self.L1InfoHypo,self.L1InfoHypoRNDMReject]
                self.L2SequenceList+=[self.AllTEDummyFakeROI.name()]

            if (TriggerFlags.CosmicSlice.testCosmic()==True):
                mlog.info("Creating new HLTchain L2_"+sig_id+" with lower chain name:"+str(l1item))
            self.l2chain = HLTChain( chain_name="L2_"+sig_id,chain_counter=counterId,lower_chain_name=l1item,level="L2",prescale=self.L2prescale,pass_through='0')
            self.l2chain.addStreamTag(L2StreamName, type=self.L2streamtype, obeyLB='yes', prescale='1')
            self.l2chain.addTriggerTypeBit(self.TriggerTypeBit)
            self.l2chain.addGroup(self.groupNames)
            
            if sig_id.find('passHLT') >= 0: self.l2chain.pass_through='1' 
            if sig_id.find('passL2' ) >= 0: self.l2chain.pass_through='1' 
        
        # defining the EF chain for signatures
        if self.configEF:
            self.efchain = HLTChain( chain_name="EF_"+sig_id,chain_counter=counterId,lower_chain_name="L2_"+sig_id,level="EF",prescale=self.EFprescale,pass_through='0')
            if (EFStreamName!=""):
                self.efchain.addStreamTag(EFStreamName, type=self.EFstreamtype, obeyLB='yes', prescale='1')
            else:
                self.efchain.addStreamTag("DISCARD", type=self.EFstreamtype, obeyLB='yes', prescale='1')
                from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
                self.Reject_Dummy = DummyFEX("Cosmic_RejectAllDummy_%s" % sig_id)
                from TrigEFIDCosmic.TrigEFIDCosmicConf import L1InfoHypo
                self.RejectAll = L1InfoHypo("RejectAll"+sig_id) 
                self.RejectAll.TriggerTypeBitMask=0x0
                self.RejectAll.TriggerTypeBit    =0x0
                self.RejectAll.InvertSelection   =True
                self.EFSequenceHash[self.RejectAll.name()]=[self.Reject_Dummy,self.RejectAll]
                self.EFSequenceList+=[self.RejectAll.name()]

            if (myExpressStreamPrescale>0):
                self.efchain.addStreamTag('express', 'reserved', 'yes', '%i' % myExpressStreamPrescale)

            self.efchain.addTriggerTypeBit(self.TriggerTypeBit)
            self.efchain.addGroup(self.groupNames)
            if sig_id.find('passHLT') >= 0: self.efchain.pass_through='1' 
            if sig_id.find('passEF' ) >= 0: self.efchain.pass_through='1' 

    def generateMenu(self, triggerPythonConfig):
        mlog = logging.getLogger( 'Cosmic.py:CosmicTemplate' )

        TE = self.l1threshold
        if self.configLVL2:
            if eval(self.Flag):
                if (TriggerFlags.CosmicSlice.testCosmic()==True):
                    mlog.info("Generating L2 sequence for %s" % self.sig_id)
                for x in self.L2SequenceList:
                    if (TriggerFlags.CosmicSlice.testCosmic()==True):
                        mlog.info(self.sig_id+", "+x+", "+str(self.L2SequenceHash[x]))
                    if (x not in L2SequencesList):
                        if (TriggerFlags.CosmicSlice.testCosmic()==True):
                            mlog.info("Creating new sequence "+str(self.L2SequenceHash[x])+", input, "+str(TE)+", output, "+x)
                        L2SequencesList[x]=[TE,self.L2SequenceHash[x]]
                        TE = triggerPythonConfig.addSequence(TE,self.L2SequenceHash[x],x)
                        self.l2chain.addHLTSignature(TE)
                    else:
                        TE=x
                        self.l2chain.addHLTSignature(TE)
                        if (TriggerFlags.CosmicSlice.testCosmic()==True):
                            mlog.info( "Assuming sequence "+str(self.L2SequenceHash[x])+" already exists, setting input TE for next chain to: "+str(TE))
                triggerPythonConfig.registerHLTChain(self.sig_id,self.l2chain)
                if (TriggerFlags.CosmicSlice.testCosmic()==True):
                    mlog.info("Registered chain %s:" % self.sig_id)
                    mlog.info( self.l2chain)
            else:
                if (TriggerFlags.CosmicSlice.testCosmic()==True):
                    mlog.info("L2 sequence for %s not enabled" % self.sig_id)
        else:
            if (TriggerFlags.CosmicSlice.testCosmic()==True):
                mlog.info("configLVL2 for %s not enabled" % self.sig_id)
 
        if self.configEF:
            if eval(self.Flag):
                for x in self.EFSequenceList:
                    TE = triggerPythonConfig.addSequence(TE, self.EFSequenceHash[x],x)
                    self.efchain.addHLTSignature(TE)
            else:
                if (TriggerFlags.CosmicSlice.testCosmic()==True):
                    mlog.info("EF sequence for %s not enabled" % self.sig_id)
            triggerPythonConfig.registerHLTChain(self.sig_id, self.efchain)
        else:
            if (TriggerFlags.CosmicSlice.testCosmic()==True):
                mlog.info("configEF for %s not enabled" % self.sig_id)
        ########################################  This part is the same for all sequences (end) ###############################################################

 


########################################  Dummy slice  ###############################################################
class CosmicStandalone(CosmicTemplate):
    def __init__(self, sig_id, l1item, l1threshold, counterId,StreamName,streamtype='physics',L2prescale=1,EFprescale=1):
        CosmicTemplate.__init__(self, sig_id, l1item, l1threshold, counterId,StreamName,streamtype,L2prescale,EFprescale,groupNames=["Cosmic","Cosmic_Dummies"])
        mlog = logging.getLogger( 'Cosmic.py:CosmicStandalone' )

        if l1threshold not in TriggerFlags.CosmicSlice.UsedLVL1Thresholds.StoredValue:
            TriggerFlags.CosmicSlice.UsedLVL1Thresholds.StoredValue += [l1threshold]
        if l1item not in TriggerFlags.CosmicSlice.UsedLVL1Signatures.StoredValue:
            TriggerFlags.CosmicSlice.UsedLVL1Signatures.StoredValue += [l1item]

        self.__supported = ["StandaloneDummy","CaloDummy","RPCDummy","TGCDummy","Dummy","MU1Dummy","MU2Dummy","MU3Dummy","MU11Dummy"]        

        for i in range(1,9):
            self.__supported += ['JET%iDummy' % i]
            self.__supported += ['TAU%iDummy' % i]
            self.__supported += ['EM%iDummy' % i]
            TriggerTypes['JET%i' % i]=0x90
            TriggerTypes['TAU%i' % i]=0x90
            TriggerTypes['EM%i' % i]=0x90
        for i in range(1,5):
            self.__supported += ['JB%iDummy' % i]
            self.__supported += ['JF%iDummy' % i]
            TriggerTypes['JB%i' % i]=0x90
            TriggerTypes['JF%i' % i]=0x90

        if sig_id not in self.__supported:
            raise Exception("ERROR: "+sig_id+" not supprted"+str(self.__supported) )

        self.Flag='TriggerFlags.CosmicSlice.do%s()' % sig_id
        if self.configLVL2:
            if eval(self.Flag):
                from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
                self.L2_Dummy = DummyFEX("Cosmic_L2_%s" % sig_id)
                self.L2SequenceHash[self.L2_Dummy.name()]=[self.L2_Dummy]
                self.L2SequenceList += [self.L2_Dummy.name()]

        if self.configEF:
            if eval(self.Flag):
                from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
                self.EF_Dummy = DummyFEX("Cosmic_EF_%s" % sig_id)
                self.EFSequenceHash[self.EF_Dummy.name()]=[self.EF_Dummy]
                self.EFSequenceList += [self.EF_Dummy.name()]

########################################  Passthrough slice  ###############################################################
class CosmicCalibration(CosmicTemplate):
    def __init__(self, sig_id, l1item, l1threshold, counterId,StreamName,streamtype='physics',L2prescale=1,EFprescale=1,ExpressStreamPrescale=-1):
        if (sig_id.count('Passthrough') or sig_id.count('SingleBeam')):
            streamtype='physics'
        else:
            streamtype='calibration'

        
        if (LocalFlags.UseSingleBeam7BitL1Menu or LocalFlags.UseStartup2009L1Menu):
            if (sig_id=="SingleBeamTriggerType0"       ):
                StreamName='ERROR_TriggerType0_ERROR'
            if (sig_id=="SingleBeamRNDM"       ):
                StreamName='RNDM'
            if (sig_id=="SingleBeamBPTX"       ):
                StreamName='BPTX'
            if (sig_id=="SingleBeamL1Calo"     ):
                StreamName='L1Calo'
            if (sig_id=="SingleBeamL1CaloEMFilter"     ):
                StreamName='L1CaloEM'
            if (sig_id=="SingleBeamL1CaloEMFilter_V2"     ):
                StreamName='L1CaloEM'
            if (sig_id=="SingleBeamTGCwBeam"   ):
                StreamName='TGCwBeam'
            if (sig_id=="SingleBeamRPCwBeam"   ):
                StreamName='RPCwBeam'
            if (sig_id=="SingleBeamMBTS"       ):
                StreamName='MBTS_BCM_LUCID'
            if (sig_id=="SingleBeamMBTSFilter" ):
                StreamName='MBTS_BCM_LUCID'
            if (sig_id=="SingleBeamBCM"        ):
                StreamName='MBTS_BCM_LUCID'
            if (sig_id=="SingleBeamLUCID"      ):
                StreamName='MBTS_BCM_LUCID'
            if (sig_id=="SingleBeamCosmicMuons"):
                StreamName='CosmicMuons'

        if (LocalFlags.UseSingleBeam3BitL1Menu or LocalFlags.Use3Streams):
            if (sig_id=="SingleBeamTriggerType0"       ):
                StreamName='ERROR_TriggerType0_ERROR'
            if (sig_id=="SingleBeamRNDM"       ):
                StreamName='RNDM'
            if (sig_id=="SingleBeamBPTX"       ):
                StreamName='BPTX'
            if (sig_id=="SingleBeamL1Calo"     ):
                StreamName='DetectorActivity'
            if (sig_id=="SingleBeamL1CaloEMFilter"     ):
                StreamName='DetectorActivity'
            if (sig_id=="SingleBeamTGCwBeam"   ):
                StreamName='DetectorActivity'
            if (sig_id=="SingleBeamRPCwBeam"   ):
                StreamName='DetectorActivity'
            if (sig_id=="SingleBeamMBTS"       ):
                StreamName='DetectorActivity'
            if (sig_id=="SingleBeamMBTSFilter" ):
                StreamName='DetectorActivity'
            if (sig_id=="SingleBeamBCM"        ):
                StreamName='DetectorActivity'
            if (sig_id=="SingleBeamLUCID"      ):
                StreamName='DetectorActivity'
            if (sig_id=="SingleBeamCosmicMuons"):
                StreamName='DetectorActivity'
            
        

        CosmicTemplate.__init__(self, sig_id, l1item, l1threshold, counterId,StreamName,streamtype,L2prescale,EFprescale,groupNames=["Cosmic","Cosmic_Passthroughs"],myExpressStreamPrescale=ExpressStreamPrescale)
        mlog = logging.getLogger( 'Cosmic.py:CosmicEmpty' )

        self.__supported = ["StandaloneCalibration","CaloCalibration",
                            "RPCCalibration","TGCCalibration","Calibration",
                            "L1CaloPassthrough","MBTSPassthrough",
                            "NIM0Passthrough","NIM4Passthrough","TRTPassthrough", "TilePassthrough", 
                            "CALREQ0Calibration","CALREQ1Calibration","CALREQ2Calibration",
                            "PhysicsPassthrough","CalibrationPassthrough","L2CalibrationPassthrough",
                            "EFCalibrationPassthrough","RNDM0Passthrough","RNDM1Passthrough","BCMPassthrough","LUCIDPassthrough",
                            "SingleBeamTriggerType0",
                            "SingleBeamRNDM",       
                            "SingleBeamBPTX",       
                            "SingleBeamL1Calo",     
                            "SingleBeamL1CaloEMFilter",
                            "SingleBeamL1CaloEMFilter_V2",
                            "SingleBeamTGCwBeam",   
                            "SingleBeamRPCwBeam",
                            "SingleBeamMBTS",       
                            "SingleBeamMBTSFilter",       
                            "SingleBeamBCM",        
                            "SingleBeamLUCID",      
                            "SingleBeamCosmicMuons",
                            ]        
        for i in [5,10,15,20,30,40,50,100]:
            self.__supported += ['JET%iPassthrough' % i]
            self.__supported += ['TAU%iPassthrough' % i]
            self.__supported += ['EM%iPassthrough' % i]
            TriggerTypes['JET%i' % i]=0x90
            TriggerTypes['TAU%i' % i]=0x90
            TriggerTypes['EM%i' % i]=0x90
        for i in [5,10,15,20]:
            self.__supported += ['JB%iPassthrough' % i]
            self.__supported += ['JF%iPassthrough' % i]
            TriggerTypes['JB%i' % i]=0x90
            TriggerTypes['JF%i' % i]=0x90

        TriggerTypes['RNDMPassthrough']=0x88
        TriggerTypes['MBTSPassthrough']=0xc0

        TriggerTypes['BCMPassthrough']  =0xc1
        TriggerTypes['LUCIDPassthrough']=0xc2

        TriggerTypes['NIM0Passthrough']=0x81
        TriggerTypes['NIM4Passthrough']=0xa0
        TriggerTypes['TRTPassthrough']=0xc0
        TriggerTypeMasks['TRTPassthrough']=0xc0

        TriggerTypes['TilePassthrough']=0xc0
        TriggerTypeMasks['TilePassthrough']=0xc0
        
        TriggerTypes['CALREQ0Calibration']=0x31
        TriggerTypes['CALREQ1Calibration']=0x32
        TriggerTypes['CALREQ2Calibration']=0x34
        

        if (LocalFlags.UseSingleBeam7BitL1Menu or LocalFlags.UseStartup2009L1Menu):
            TriggerTypes['SingleBeamTriggerType0'  ]     =0x00
            TriggerTypeMasks['SingleBeamTriggerType0'  ] =0x7f
            TriggerTypes['SingleBeamRNDM'  ]     =0x81
            TriggerTypes['SingleBeamBPTX'  ]     =0x82
            TriggerTypes['SingleBeamL1Calo']     =0x84
            TriggerTypes['SingleBeamL1CaloEMFilter']     =0x84
            TriggerTypes['SingleBeamL1CaloEMFilter_V2']     =0x84
            TriggerTypes['SingleBeamTGCwBeam']   =0x88
            TriggerTypes['SingleBeamRPCwBeam']   =0x90
            TriggerTypes['SingleBeamMBTS']       =0xa0
            TriggerTypes['SingleBeamMBTSFilter'] =0xa0
            TriggerTypes['SingleBeamBCM']        =0xa0
            TriggerTypes['SingleBeamLUCID']      =0xa0
            TriggerTypes['SingleBeamCosmicMuons']=0xc0
            

            TriggerTypeMasks['SingleBeamRNDM'  ]     =0x81
            TriggerTypeMasks['SingleBeamBPTX'  ]     =0x82
            TriggerTypeMasks['SingleBeamL1Calo']     =0x84
            TriggerTypeMasks['SingleBeamL1CaloEMFilter']     =0x84
            TriggerTypeMasks['SingleBeamTGCwBeam']   =0x88
            TriggerTypeMasks['SingleBeamRPCwBeam']   =0x90
            TriggerTypeMasks['SingleBeamMBTS']       =0xa0
            TriggerTypeMasks['SingleBeamMBTSFilter'] =0xa0
            TriggerTypeMasks['SingleBeamBCM']        =0xa0
            TriggerTypeMasks['SingleBeamLUCID']      =0xa0
            TriggerTypeMasks['SingleBeamCosmicMuons']=0xc0
            

        if (LocalFlags.UseSingleBeam3BitL1Menu):
            TriggerTypes['SingleBeamTriggerType0'  ]     =0x00
            TriggerTypeMasks['SingleBeamTriggerType0'  ] =0x7f

            TriggerTypes['SingleBeamRNDM'  ]     =0x81
            TriggerTypes['SingleBeamBPTX'  ]     =0x82
            TriggerTypes['SingleBeamL1Calo']     =0x84
            TriggerTypes['SingleBeamL1CaloEMFilter']     =0x84
            TriggerTypes['SingleBeamTGCwBeam']   =0x84
            TriggerTypes['SingleBeamRPCwBeam']   =0x84
            TriggerTypes['SingleBeamMBTS']       =0x84
            TriggerTypes['SingleBeamMBTSFilter'] =0x84
            TriggerTypes['SingleBeamBCM']        =0x84
            TriggerTypes['SingleBeamLUCID']      =0x84
            TriggerTypes['SingleBeamCosmicMuons']=0x84

            TriggerTypeMasks['SingleBeamRNDM'  ]     =0x81
            TriggerTypeMasks['SingleBeamBPTX'  ]     =0x82
            TriggerTypeMasks['SingleBeamL1Calo']     =0x84
            TriggerTypeMasks['SingleBeamL1CaloEMFilter']     =0x84
            TriggerTypeMasks['SingleBeamTGCwBeam']   =0x84
            TriggerTypeMasks['SingleBeamRPCwBeam']   =0x84
            TriggerTypeMasks['SingleBeamMBTS']       =0x84
            TriggerTypeMasks['SingleBeamMBTSFilter'] =0x84
            TriggerTypeMasks['SingleBeamBCM']        =0x84
            TriggerTypeMasks['SingleBeamLUCID']      =0x84
            TriggerTypeMasks['SingleBeamCosmicMuons']=0x84



        if sig_id not in self.__supported:
            raise Exception("ERROR: "+sig_id+" not supprted"+str(self.__supported) )

        self.Flag='TriggerFlags.CosmicSlice.do%s()' % sig_id

        if self.configLVL2:
            if eval(self.Flag):
                from TrigEFIDCosmic.TrigEFIDCosmicConf import Cosmic__DummyUnseededAllTEAlgo
                self.AllTEDummyFakeROI = Cosmic__DummyUnseededAllTEAlgo("Cosmic"+sig_id+"AllTEDummy")
                from TrigEFIDCosmic.TrigEFIDCosmicConf import L1InfoHypo
                self.L1InfoHypo = L1InfoHypo("L1InfoHypo"+sig_id) # here we created an instance
                if (sig_id=='SingleBeamMBTSFilter'):
                    self.SecondL1InfoHypo = L1InfoHypo("L1InfoHypo_2"+sig_id) # here we created an instance
                    self.SecondL1InfoHypo.TriggerTypeBitMask=0x5f
                    self.SecondL1InfoHypo.TriggerTypeBit    =0x00
                    self.SecondL1InfoHypo.InvertSelection   =True

                    #self.SecondL1InfoHypo.OutputLevel   =4
                    #self.L1InfoHypo.OutputLevel   =4

                    self.L2SequenceHash[self.AllTEDummyFakeROI.name()]=[self.AllTEDummyFakeROI,self.L1InfoHypo,self.SecondL1InfoHypo]
                else:
                    self.L2SequenceHash[self.AllTEDummyFakeROI.name()]=[self.AllTEDummyFakeROI,self.L1InfoHypo]

                self.L2SequenceList+=[self.AllTEDummyFakeROI.name()]

                self.AllTEDummyFakeROI.FakeROIEta=0.0
                self.AllTEDummyFakeROI.FakeROIPhi=0.0
                #    sig_id.count("MBTS") or
                if (sig_id=="RNDM0Passthrough" or
                    sig_id=="RNDM4Passthrough" or
                    sig_id=="BCMPassthrough" or
                    sig_id=="LUCIDPassthrough" or
                    sig_id=="L1CaloPassthrough" or
                    sig_id=="CALREQ0Calibration" or
                    sig_id=="CALREQ1Calibration" or
                    sig_id=="CALREQ2Calibration" or
                    sig_id=="TRTPassthrough" or
                    sig_id=="TilePassthrough" 
                    ):
                    self.L1InfoHypo.L1ItemNames    += self.l1threshold

                if (sig_id=="SingleBeamRNDM"       ):
                    self.L1InfoHypo.L1ItemNames=[]
                if (sig_id=="SingleBeamBPTX"       ):
                    self.L1InfoHypo.L1ItemNames=[]
                if (sig_id=="SingleBeamL1Calo"     ):
                    self.L1InfoHypo.L1ItemNames=[]
                if (sig_id=="SingleBeamTGCwBeam"   ):
                    self.L1InfoHypo.L1ItemNames=[]
                if (sig_id=="SingleBeamRPCwBeam"   ):
                    self.L1InfoHypo.L1ItemNames=[]
                if (sig_id=="SingleBeamMBTS"       ):
                    self.L1InfoHypo.L1ItemNames=[]
                if (sig_id=="SingleBeamMBTSFilter" ):
                    self.L1InfoHypo.L1ItemNames=[]
                if (sig_id=="SingleBeamBCM"        ):
                    self.L1InfoHypo.L1ItemNames=[]
                if (sig_id=="SingleBeamLUCID"      ):
                    self.L1InfoHypo.L1ItemNames=[]
                if (sig_id=="SingleBeamCosmicMuons"):
                    self.L1InfoHypo.L1ItemNames=[]

                if (sig_id=="SingleBeamRNDM" or sig_id=="SingleBeamBPTX" or sig_id=="SingleBeamL1Calo" or sig_id=="SingleBeamTGCwBeam" or sig_id=="SingleBeamRPCwBeam" or sig_id=="SingleBeamMBTS" or sig_id=="SingleBeamBCM" or sig_id=="SingleBeamLUCID" or sig_id=="SingleBeamCosmicMuons"):
                    self.L1InfoHypo.L1ItemNames=[]
                #if (sig_id=='SingleBeamL1CaloEMFilter'):
                #    self.L1InfoHypo.L1ItemNames    += self.l1threshold
                    
                #self.AllTEDummyFakeROI.FakeROIEta=1.7
                #self.AllTEDummyFakeROI.FakeROIPhi=1.5


                for x in TriggerTypes:
                    if (sig_id.count(x)):
                        self.L1InfoHypo.TriggerTypeBitMask=0xff
                        self.L1InfoHypo.TriggerTypeBit    =TriggerTypes[x]

                for x in TriggerTypeMasks:
                    if (sig_id.count(x)):
                        self.L1InfoHypo.TriggerTypeBitMask=TriggerTypeMasks[x]

                if (sig_id=="L1CaloPassthrough"):
                        self.L1InfoHypo.TriggerTypeBitMask=0xff
                        self.L1InfoHypo.TriggerTypeBit    =0x90

                self.L1InfoHypo.L1TriggerBitMask=0x0
                self.L1InfoHypo.L1TriggerBit    =0x01

                if (sig_id=="CalibrationPassthrough"):
                        self.L1InfoHypo.TriggerTypeBitMask=0x80
                        self.L1InfoHypo.TriggerTypeBit    =0x00

                if (sig_id=="PhysicsPassthrough"):
                        self.L1InfoHypo.TriggerTypeBitMask=0x80
                        self.L1InfoHypo.TriggerTypeBit    =0x80

                if (sig_id=="L2CalibrationPassthrough"):
                        self.L1InfoHypo.TriggerTypeBitMask=0x80
                        self.L1InfoHypo.TriggerTypeBit    =0x80

                if (sig_id=="EFCalibrationPassthrough"):
                        self.L1InfoHypo.TriggerTypeBitMask=0x80
                        self.L1InfoHypo.TriggerTypeBit    =0x80


########################################  Empty slice  ###############################################################
class CosmicEmpty(CosmicTemplate):
    def __init__(self, sig_id, l1item, l1threshold, counterId,StreamName):
        CosmicTemplate.__init__(self, sig_id, l1item, l1threshold, counterId,StreamName,groupNames=["Cosmic","Cosmic_EmptyChains"])
        mlog = logging.getLogger( 'Cosmic.py:CosmicEmpty' )

        self.__supported = ["StandaloneEmpty","CaloEmpty","RPCEmpty","TGCEmpty","Empty","MU3Empty","MU11Empty"]        
        if sig_id not in self.__supported:
            raise Exception("ERROR: "+sig_id+" not supprted"+str(self.__supported) )

        self.Flag='TriggerFlags.CosmicSlice.do%s()' % sig_id

 ########################################  Tile Calibration slice  ###############################################################

class CosmicTileCalibration(CosmicTemplate):
    def __init__(self, sig_id, l1item, l1threshold, counterId, StreamName):
        CosmicTemplate.__init__(self, sig_id, l1item, l1threshold, counterId,StreamName,streamtype='calibration',groupNames=["Cosmic","Cosmic_TileCalibration"])
        mlog = logging.getLogger( 'Cosmic.py:CosmicTileCalibration' )

        self.__supported = ["TileCalib_ped","TileCalib_laser","TileCalib_cis"]
        if sig_id not in self.__supported:
            raise Exception("ERROR: "+sig_id+" not supprted"+str(self.__supported) )

        self.Flag='TriggerFlags.CosmicSlice.do%s()' % sig_id

        if self.configLVL2:
            if eval(self.Flag):
#                if (sig_id.count("AllTe")):
#                    self.L1InfoHypo.InvertSelection   =True
#                    self.L1InfoHypo.TriggerTypeBitMask=0xff
#                    self.L1InfoHypo.TriggerTypeBit    =0x88
#                    self.AllTEDummyFakeROI.FakeROIEta=0.0
#                    self.AllTEDummyFakeROI.FakeROIPhi=0.0
                from TrigDetCalib.TrigDetCalibConfig import TileSubDetListWriter
                self.l2_tileSubDetListWriter = TileSubDetListWriter("Cosmic"+sig_id+"TileSubDetListWriter")
                self.l2_tileSubDetListWriter.Subdetectors = "Tile"
                self.l2_tileSubDetListWriter.MaxRoIsPerEvent = 1
                self.L2SequenceHash[self.l2_tileSubDetListWriter.name()] = [self.l2_tileSubDetListWriter]
                self.L2SequenceList+=[self.l2_tileSubDetListWriter.name()]
                
        if self.configEF:
            if eval(self.Flag):
                from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
                self.EF_Dummy = DummyFEX("EF_%s" % sig_id)
                self.EFSequenceHash[self.EF_Dummy.name()]=[self.EF_Dummy]
                self.EFSequenceList+=[self.EF_Dummy.name()]

    def generateMenu(self,triggerPythonConfig):
        CosmicTemplate.generateMenu(self,triggerPythonConfig)

########################################  LArCalib slice  ###############################################################
class CosmicLArCalib(CosmicTemplate):
    def __init__(self, sig_id, l1item, l1threshold, counterId,StreamName,streamtype='calibration',L2prescale=1,EFprescale=1):
        CosmicTemplate.__init__(self, sig_id, l1item, l1threshold, counterId,StreamName,streamtype,L2prescale,EFprescale,groupNames=["Cosmic","Cosmic_LArCalibration"])
        mlog = logging.getLogger( 'Cosmic.py:CosmicLArCalib' )

        self.__supported = ["LArCalib","LArCalib_V2"]
        if sig_id not in self.__supported:
            raise Exception("ERROR: "+sig_id+" not supported"+str(self.__supported) )

        self.Flag='TriggerFlags.CosmicSlice.do%s()' % sig_id

        if self.configLVL2:
            if eval(self.Flag):
               from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_eGamma
               self.l2_T2CaloEgamma = T2CaloEgamma_eGamma("Cosmic"+sig_id+"T2CaloEgamma_eGamma")

               if (LocalFlags.T2CaloFakeLVL1InLVL2):
                 self.l2_T2CaloEgamma.L1ForcePhi=-1.5
                 self.l2_T2CaloEgamma.L1ForceEta=1.7
               from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_g7
               from TrigEgammaHypo.TrigL2PhotonFexConfig import L2PhotonFex_1
               from TrigEgammaHypo.TrigL2PhotonHypoConfig import L2PhotonHypo_g10_loose
               self.l2_L2PhotonHypo_g10_loose = L2PhotonHypo_g10_loose("Cosmic"+sig_id+"L2PhotonHypo_g10_loose")
               self.l2_L2PhotonFex_1 = L2PhotonFex_1("Cosmic"+sig_id+"L2PhotonFex_1")
               self.l2_L2CaloHypo_g7 = L2CaloHypo_g7("Cosmic"+sig_id+"L2CaloHypo_g7")

               self.l2_L2PhotonHypo_g10_loose.ETthr = [-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV]
               self.l2_L2PhotonHypo_g10_loose.AcceptAll=False
               self.l2_L2CaloHypo_g7.ETthr = [-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV]
               self.l2_L2CaloHypo_g7.AcceptAll=False

               # from TrigDetCalib.TrigDetCalibConfig import L2PhotonROBListWriter
               from TrigDetCalib.TrigDetCalibConfig import LArL2ROBListWriter
               self.L2PhotonROBListWriter = LArL2ROBListWriter("Cosmic"+sig_id+"LArL2ROBListWriter")

               self.L2SequenceHash[self.l2_T2CaloEgamma.name()]=[self.l2_T2CaloEgamma,self.l2_L2CaloHypo_g7]
               self.L2SequenceList+=[self.l2_T2CaloEgamma.name()]
               self.L2SequenceHash["Cosmic"+sig_id+"L2PhotonHypo"]=[self.l2_L2PhotonFex_1,self.l2_L2PhotonHypo_g10_loose]
               self.L2SequenceList+=["Cosmic"+sig_id+"L2PhotonHypo"]
               self.L2SequenceHash["Cosmic"+sig_id+"L2Photon"]=[self.L2PhotonROBListWriter]
               self.L2SequenceList+=["Cosmic"+sig_id+"L2Photon"]
               
               include("TrigT2CaloCommon/LArCellBuilding.py")

            if self.configEF:
               from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
               self.EF_Dummy = DummyFEX("Cosmic_EF_%s" % sig_id)
               self.EFSequenceHash[self.EF_Dummy.name()]=[self.EF_Dummy]
               self.EFSequenceList+=[self.EF_Dummy.name()]


    def generateMenu(self,triggerPythonConfig):
        CosmicTemplate.generateMenu(self,triggerPythonConfig)
#        if (eval(self.Flag)):
#            include("TrigT2CaloCommon/LArCellBuilding.py")

########################################  Pixel noise Calibration slice  ###############################################################

class CosmicPixelCalibration(CosmicTemplate):
    def __init__(self, sig_id, l1item, l1threshold, counterId, StreamName):
        CosmicTemplate.__init__(self, sig_id, l1item, l1threshold, counterId,StreamName,streamtype='calibration',groupNames=["Cosmic","Cosmic_PixelCalibration"])
        mlog = logging.getLogger( 'Cosmic.py:CosmicPixelCalibration' )

        self.__supported = ["Cosmic_pixelnoise","Cosmic_pixelnoise_V2"]
        if sig_id not in self.__supported:
            raise Exception("ERROR: "+sig_id+" not supprted"+str(self.__supported) )

        self.Flag='TriggerFlags.CosmicSlice.do%s()' % sig_id
       
        
        if self.configLVL2:
            if eval(self.Flag):   

                from TrigDetCalib.TrigDetCalibConfig import TrigSubDetListWriter
                self.l2_pixelSubDetListWriter = TrigSubDetListWriter("Cosmic"+sig_id+"PixelSubDetListWriter")
                self.l2_pixelSubDetListWriter.Subdetectors = "Pixel"
                self.l2_pixelSubDetListWriter.extraROBs = []
                #self.l2_pixelSubDetListWriter.MaxRoIsPerEvent = 1
                self.L2SequenceHash[self.l2_pixelSubDetListWriter.name()] = [self.l2_pixelSubDetListWriter]
                self.L2SequenceList+=[self.l2_pixelSubDetListWriter.name()]

        if self.configEF:
            if eval(self.Flag):
                from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
                self.EF_Dummy = DummyFEX("EF_%s" % sig_id)
                self.EFSequenceHash[self.EF_Dummy.name()]=[self.EF_Dummy]
                self.EFSequenceList+=[self.EF_Dummy.name()]

    def generateMenu(self,triggerPythonConfig):
        CosmicTemplate.generateMenu(self,triggerPythonConfig)

########################################  rate slice  ###############################################################

class CosmicRateAnalysis(CosmicTemplate):
    def __init__(self, sig_id, l1item, l1threshold, counterId, StreamName):
        CosmicTemplate.__init__(self, sig_id, l1item, l1threshold, counterId,StreamName,streamtype='calibration',groupNames=["Cosmic","Cosmic_RateAnalysis"])
        mlog = logging.getLogger( 'Cosmic.py:RateAnalysis' )

        self.__supported = ["Cosmic_rateanalysis"]
        if sig_id not in self.__supported:
            raise Exception("ERROR: "+sig_id+" not supprted"+str(self.__supported) )

        self.Flag='TriggerFlags.CosmicSlice.do%s()' % sig_id

        if self.configLVL2:
            if eval(self.Flag):

                from TrigDetCalib.TrigDetCalibConfig import TrigSubDetListWriter
                self.l2_rateListWriter = TrigSubDetListWriter("Cosmic"+sig_id+"RateListWriter")
                self.l2_rateListWriter.Subdetectors = ""
                self.l2_rateListWriter.extraROBs = [ 7798784 ]
                #self.l2_pixelSubDetListWriter.MaxRoIsPerEvent = 1
                self.L2SequenceHash[self.l2_rateListWriter.name()] = [self.l2_rateListWriter]
                self.L2SequenceList+=[self.l2_rateListWriter.name()]

        if self.configEF:
            if eval(self.Flag):
                from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
                self.EF_Dummy = DummyFEX("EF_%s" % sig_id)
                self.EFSequenceHash[self.EF_Dummy.name()]=[self.EF_Dummy]
                self.EFSequenceList+=[self.EF_Dummy.name()]

    def generateMenu(self,triggerPythonConfig):
        CosmicTemplate.generateMenu(self,triggerPythonConfig)

 ########################################  IDSCAN slice  ###############################################################
class CosmicIDSCAN(CosmicTemplate):
    def __init__(self, sig_id, l1item, l1threshold, counterId,StreamName):
        CosmicTemplate.__init__(self, sig_id, l1item, l1threshold, counterId,StreamName,groupNames=["Cosmic","Cosmic_Tracking"])
        mlog = logging.getLogger( 'Cosmic.py:CosmicIDSCAN' )

        self.__supported = ["AllTeIDSCAN",
                            "AllTeIDSCANCombined",
                            "CosmicsAllTeIDSCAN",
                            "CosmicsAllTeIDSCAN_TrkHypo",
                            "CosmicsAllTeIDSCAN_AllPhysics_TrkHypo",
                            "CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo",
                            "CosmicsAllTeIDSCANCombined"
                            ]        
        if sig_id not in self.__supported:
            raise Exception("ERROR: "+sig_id+" not supprted"+str(self.__supported) )

        self.Flag='TriggerFlags.CosmicSlice.do%s()' % sig_id

        if self.configLVL2:
            if eval(self.Flag):
                if (sig_id.count("AllTe")):
                    if (0):
                        #(StreamName=='CosmicMuons' or StreamName=='IDSelected'):
                        self.L1InfoHypo.TriggerTypeBitMask=0xc0
                        self.L1InfoHypo.TriggerTypeBit    =0xc0
                    else:
                        self.L1InfoHypo.InvertSelection   =False
                        self.L1InfoHypo.TriggerTypeBitMask=0x80
                        self.L1InfoHypo.TriggerTypeBit    =0x80

                    self.AllTEDummyFakeROI.FakeROIEta=0.0
                    self.AllTEDummyFakeROI.FakeROIPhi=0.0
                    #self.AllTEDummyFakeROI.FakeROIEta=1.7
                    #self.AllTEDummyFakeROI.FakeROIPhi=1.5

                if (sig_id.count('AllPhysics')):
                    # Run on every physics trigger
                    self.L1InfoHypo.TriggerTypeBitMask=0x80
                    self.L1InfoHypo.TriggerTypeBit    =0x80
                    self.L1InfoHypo.InvertSelection   =False

                from TrigIDSCAN.TrigIDSCAN_Config import TrigIDSCAN_Cosmics
                self.idscanCosmics = TrigIDSCAN_Cosmics("CosmicIDSCANSequence"+sig_id) # here we created an instance
                self.idscanCosmics.doTRTpropagation = True
                if (sig_id.count('4Hits')):
                    self.idscanCosmics.MinHits = 4
                    self.idscanCosmics.TrigHitFilter.LayerThreshold = 3.5
                else:
                    self.idscanCosmics.MinHits = 3
                    self.idscanCosmics.TrigHitFilter.LayerThreshold = 2.5

                self.L2SequenceHash[self.idscanCosmics.name()]=[self.idscanCosmics]
                self.L2SequenceList += [self.idscanCosmics.name()]

                if (sig_id.count('TrkHypo')):
                    from TrigL2CosmicMuonHypo.TrigL2CosmicMuonHypo_Config import CosmicTrtHypo_Cosmic
                    self.trkhypoCosmics = CosmicTrtHypo_Cosmic("Cosmic"+sig_id+"TrkHypo") 
                    self.trkhypoCosmics.NTrthitsCut = -1
                    self.L2SequenceHash[self.idscanCosmics.name()]+=[self.trkhypoCosmics]

                if (sig_id.count("Combined")):
                    from TrigL2CosmicCombined.TrigL2CosmicCombined_Config import CombinedIDHalfTracks_Cosmic
                    self.htcombinedCosmics = CombinedIDHalfTracks_Cosmic("Cosmic"+sig_id+"CombinedIDHalfTracks") # here we created an instance
                    from TrigL2CosmicMuonHypo.TrigL2CosmicMuonHypo_Config import CombinedIDHalfTracksHypo_Cosmic
                    self.htcombinedCosmicsHypo = CombinedIDHalfTracksHypo_Cosmic("Cosmic"+sig_id+"CombinedIDHalfTracksHypo") # here we created an instance
                    self.L2SequenceHash[self.htcombinedCosmics.name()]=[self.htcombinedCosmics,self.htcombinedCosmicsHypo]
                    self.L2SequenceList += [self.htcombinedCosmics.name()]

                if (LocalFlags.doIDHitsHypo):
                    from TrigEFIDCosmic.IDHitsHypo_Config import IDHitsHypo_IDSCAN_L2
                    self.IDHitsHypo_L2=IDHitsHypo_IDSCAN_L2(sig_id+"CosmicIDSCANIDHitsHypoL2")
                    self.L2Sequencehash[self.IDHitsHypo_L2.name()]=[self.IDHitsHypo_L2]
                    self.L2SequenceList+=[self.IDHitsHypo_L2.name()]

        if self.configEF:
            if eval(self.Flag):
                if (LocalFlags.doEFIDCosmics):
                    from TrigEFIDCosmic.TrigEFIDCosmic_Config import TrigEFIDCosmicAlgo_IDSCAN_EF
                    self.TrigEFIDCosmicAlgo_IDSCAN_EF=TrigEFIDCosmicAlgo_IDSCAN_EF(sig_id+"TrigEFIDCosmicAlgo_IDSCAN")
                    self.EFSequencehash[self.TrigEFIDCosmicAlgo_IDSCAN_EF.name()]=[self.TrigEFIDCosmicAlgo_IDSCAN_EF]
                    self.EFSequenceList+=[self.TrigEFIDCosmicAlgo_IDSCAN_EF.name()]
                    if (LocalFlags.doIDHitsHypo):
                        from TrigEFIDCosmic.IDHitsHypo_Config import IDHitsHypo_IDSCAN_EF
                        self.IDHitsHypo_EF=IDHitsHypo_IDSCAN_EF(sig_id+"IDHitsHypo_IDSCAN_EF")
                        self.EFSequenceHash[self.IDHitsHypo_EF.name()]=[self.IDHitsHypo_EF]
                        self.EFSequenceList+=[self.IDHitsHypo_EF.name()]

########################################  SiTrack slice  ###############################################################
class CosmicSiTrack(CosmicTemplate):
    def __init__(self, sig_id, l1item, l1threshold, counterId,StreamName):
        CosmicTemplate.__init__(self, sig_id, l1item, l1threshold, counterId,StreamName,groupNames=["Cosmic","Cosmic_Tracking"])
        mlog = logging.getLogger( 'Cosmic.py:CosmicSiTrack' )

        self.__supported = ["AllTeSiTrack",
                            "AllTeSiTrackCombined",
                            "CosmicsAllTeSiTrack",
                            "CosmicsAllTeSiTrack_TrkHypo",
                            "CosmicsAllTeSiTrack_AllPhysics_TrkHypo",
                            "CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo",
                            "CosmicsAllTeSiTrackCombined"
                            ]        
        if sig_id not in self.__supported:
            raise Exception("ERROR: "+sig_id+" not supprted"+str(self.__supported) )

        self.Flag='TriggerFlags.CosmicSlice.do%s()' % sig_id

        if self.configLVL2:
            if eval(self.Flag):
                if (sig_id.count("AllTe")):
                    if (0):
                        #(StreamName=='CosmicMuons' or StreamName=='IDSelected'):
                        self.L1InfoHypo.TriggerTypeBitMask=0xc0
                        self.L1InfoHypo.TriggerTypeBit    =0xc0
                    else:
                        self.L1InfoHypo.InvertSelection   =False
                        self.L1InfoHypo.TriggerTypeBitMask=0x80
                        self.L1InfoHypo.TriggerTypeBit    =0x80


                    self.AllTEDummyFakeROI.FakeROIEta=0.0
                    self.AllTEDummyFakeROI.FakeROIPhi=0.0
                    #self.AllTEDummyFakeROI.FakeROIEta=1.7
                    #self.AllTEDummyFakeROI.FakeROIPhi=1.5

                if (sig_id.count('AllPhysics')):
                    # Run on every physics trigger
                    self.L1InfoHypo.TriggerTypeBitMask=0x80
                    self.L1InfoHypo.TriggerTypeBit    =0x80
                    self.L1InfoHypo.InvertSelection   =False

                #from TrigIDSCAN.TrigIDSCAN_Config import TrigIDSCAN_Cosmics
                #self.idscanCosmics = TrigIDSCAN_Cosmics("CosmicIDSCANSequence") # here we created an instance
                #self.idscanCosmics.doTRTpropagation = True
                #self.L2SequenceHash[self.idscanCosmics.name()]=[self.idscanCosmics]
                #self.L2SequenceList+=[self.idscanCosmics.name()]

                from TrigSiTrack.TrigSiTrack_Config import TrigSiTrack_Cosmics
                self.sitrackCosmics = TrigSiTrack_Cosmics("Cosmic"+sig_id+"TrigSiTrack") # here we created an instance
                #self.sitrackCosmics.OutputLevel=DEBUG
                #self.sitrackCosmics.Map_UsePixel = False
                self.L2SequenceHash[self.sitrackCosmics.name()]=[self.sitrackCosmics]
                self.L2SequenceList+=[self.sitrackCosmics.name()]

                if (sig_id.count('4Hits')):
                    self.sitrackCosmics.Extension_SpacePoints = 4
                else:
                    self.sitrackCosmics.Extension_SpacePoints = 3

                if (sig_id.count('TrkHypo')):
                    from TrigL2CosmicMuonHypo.TrigL2CosmicMuonHypo_Config import CosmicTrtHypo_Cosmic
                    self.trkhypoCosmics = CosmicTrtHypo_Cosmic("Cosmic"+sig_id+"TrkHypo") 
                    self.trkhypoCosmics.NTrthitsCut = -1
                    self.L2SequenceHash[self.sitrackCosmics.name()]+=[self.trkhypoCosmics]

                if (sig_id.count("Combined")):
                    from TrigL2CosmicCombined.TrigL2CosmicCombined_Config import CombinedIDHalfTracks_Cosmic
                    self.htcombinedCosmics = CombinedIDHalfTracks_Cosmic("Cosmic"+sig_id+"CombinedIDHalfTracks") # here we created an instance
                    from TrigL2CosmicMuonHypo.TrigL2CosmicMuonHypo_Config import CombinedIDHalfTracksHypo_Cosmic
                    self.htcombinedCosmicsHypo = CombinedIDHalfTracksHypo_Cosmic("Cosmic"+sig_id+"CombinedIDHalfTracksHypo") # here we created an instance
                    self.L2SequenceHash[self.htcombinedCosmics.name()]=[self.htcombinedCosmics,self.htcombinedCosmicsHypo]
                    self.L2SequenceList += [self.htcombinedCosmics.name()]

                if (LocalFlags.doIDHitsHypo):
                    from TrigEFIDCosmic.IDHitsHypo_Config import IDHitsHypo_SiTrack_L2
                    self.IDHitsHypo_L2=IDHitsHypo_SiTrack_L2("Cosmic"+sig_id+"IDHitsHypoL2")
                    self.L2SequenceHash[self.sitrackCosmics.name()]+=[self.IDHitsHypo_L2]

        if self.configEF:
            if eval(self.Flag):
                if (LocalFlags.doEFIDCosmics):
                    from TrigEFIDCosmic.TrigEFIDCosmic_Config import TrigEFIDCosmicAlgo_SiTrack_EF
                    self.TrigEFIDCosmicAlgo_EF=TrigEFIDCosmicAlgo_SiTrack_EF("Cosmic"+sig_id+"TrigEFIDCosmicAlgo")
                    self.EFSequenceHash[self.TrigEFIDCosmicAlgo_EF.name()]=[self.TrigEFIDCosmicAlgo_EF]
                    self.EFSequenceList+=[self.TrigEFIDCosmicAlgo_EF.name()]
                    if (LocalFlags.doIDHitsHypo):
                        from TrigEFIDCosmic.IDHitsHypo_Config import IDHitsHypo_SiTrack_EF
                        self.IDHitsHypo_EF=IDHitsHypo_SiTrack_EF("Cosmic"+sig_id+"IDHitsHypoEF")
                        self.EFSequenceHash[self.IDHitsHypo_EF.name()]=[self.IDHitsHypo_EF]
                        self.EFSequenceList+=[self.IDHitsHypo_EF.name()]

########################################  EFID slice  ###############################################################
class CosmicEFID(CosmicTemplate):
    def __init__(self, sig_id, l1item, l1threshold, counterId,StreamName,L2prescale=1,EFprescale=1):
        CosmicTemplate.__init__(self, sig_id, l1item, l1threshold, counterId,StreamName,L2prescale,EFprescale,groupNames=["Cosmic","Cosmic_Tracking"])
        mlog = logging.getLogger( 'Cosmic.py:CosmicTracking' )

        self.__supported = ["CosmicsAllTeEFID"]        
        if sig_id not in self.__supported:
            raise Exception("ERROR: "+sig_id+" not supprted"+str(self.__supported) )

        self.Flag='TriggerFlags.CosmicSlice.do%s()' % sig_id

        if self.configLVL2:
            if eval(self.Flag):
                if (sig_id.count('AllTe')):
                    self.L1InfoHypo.InvertSelection   =False
                    # self.L1InfoHypo.TriggerTypeBitMask=0x80
                    self.L1InfoHypo.TriggerTypeBitMask=0x80
                    self.L1InfoHypo.TriggerTypeBit    =0x80 # Select physics triggers
                    self.AllTEDummyFakeROI.FakeROIEta=-0.5
                    self.AllTEDummyFakeROI.FakeROIPhi=1.5

        if self.configEF:
            if eval(self.Flag):
                from InDetTrigRecExample.EFInDetConfig import TrigEFIDInsideOut_CosmicsN
                self.EFIDTracking=TrigEFIDInsideOut_CosmicsN()
                self.EFIDSequence=self.EFIDTracking.getSequence()
                mlog.info(str(self.EFIDSequence))
                

                from TrigMinBias.TrigMinBiasConfig import MbTrkFex_1,MbTrkHypo_1
                self.trackcnt =  MbTrkFex_1("MbTrkFex_"+sig_id)
                self.EFIDSequence += [self.trackcnt]
                
                self.efidcosmhypo = MbTrkHypo_1("MbTrkHypo_"+sig_id)
                self.efidcosmhypo.AcceptAll_EF=False
                self.efidcosmhypo.Required_ntrks=1
                self.efidcosmhypo.Max_z0=1000.0
                self.EFIDSequence += [self.efidcosmhypo]

                self.EFSequenceHash["TrigEFIDInsideOut_CosmicsN"]=self.EFIDSequence
                self.EFSequenceList+=["TrigEFIDInsideOut_CosmicsN"]

########################################  TRTxK slices  ###############################################################
class CosmicTRTxK(CosmicTemplate):
    def __init__(self, sig_id, l1item, l1threshold, counterId,StreamName):
        if (sig_id.count('PEB')):
            CosmicTemplate.__init__(self, sig_id, l1item, l1threshold, counterId,StreamName,groupNames=["Cosmic","Cosmic_Tracking"],streamtype='calibration')
        else:
            CosmicTemplate.__init__(self, sig_id, l1item, l1threshold, counterId,StreamName,groupNames=["Cosmic","Cosmic_Tracking"])
        mlog = logging.getLogger( 'Cosmic.py:CosmicTRTxK' )

        self.__supported = ["AllTeTRTxK","AllTeTRTxK_TRTTrkHypo","AllTeTRTxK_TRTTrkHypo_Pixel",
                            "CosmicsAllTeTRTxK",
                            "CosmicsAllTeTRTxK_TRTTrkHypo",
                            "CosmicsAllTeTRTxK_TRTTrkHypo_Pixel",
                            "CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics",        
                            "CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_NoField",        
                            "CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_PEB"]        
        if sig_id not in self.__supported:
            raise Exception("ERROR: "+sig_id+" not supprted"+str(self.__supported) )

        self.Flag='TriggerFlags.CosmicSlice.do%s()' % sig_id

        if self.configLVL2:
            if eval(self.Flag):
                if (sig_id.count("AllTe")):
                    if (0):
                        #(StreamName=='CosmicMuons' or StreamName=='IDSelected'):
                        self.L1InfoHypo.TriggerTypeBitMask=0xc0
                        self.L1InfoHypo.TriggerTypeBit    =0xc0
                    else:
                        self.L1InfoHypo.InvertSelection   =False
                        self.L1InfoHypo.TriggerTypeBitMask=0x80
                        self.L1InfoHypo.TriggerTypeBit    =0x80

                    self.AllTEDummyFakeROI.FakeROIEta=0.0
                    self.AllTEDummyFakeROI.FakeROIPhi=0.0
                    #self.AllTEDummyFakeROI.FakeROIEta=1.7
                    #self.AllTEDummyFakeROI.FakeROIPhi=1.5

                #from TrigTRTxK.TrigTRTxK_Config import TrigTRTxK_Cosmics
                #self.trtxk  = TrigTRTxK_Cosmics("Cosmic"+sig_id+"TrigTRTxK")

                if (sig_id.count('oldAlgo')):
                    from TrigL2TRTSegFinder.TrigTRTSegFinder_Config import TrigTRTSegFinder_Cosmics
                    self.trtsegm  = TrigTRTSegFinder_Cosmics("Cosmic"+sig_id+"TrigTRTSegFinder")
                else:
                    if (sig_id.count('NoField')):
                        from TrigL2TRTSegFinder.TrigTRTSegFinder_Config import TrigTRTSegFinder_Cosmics_NewAlgo_NoField
                        self.trtsegm  = TrigTRTSegFinder_Cosmics_NewAlgo_NoField("Cosmic"+sig_id+"TrigTRTSegFinder")
                        self.trtsegm.SegmentsMakerTool.IsMagneticFieldOn = False
                    else:
                        from TrigL2TRTSegFinder.TrigTRTSegFinder_Config import TrigTRTSegFinder_Cosmics_NewAlgo
                        self.trtsegm  = TrigTRTSegFinder_Cosmics_NewAlgo("Cosmic"+sig_id+"TrigTRTSegFinder")
                        self.trtsegm.SegmentsMakerTool.IsMagneticFieldOn = True

                self.trtsegm.RoIhalfWidthDeltaPhi = 3.14
                self.trtsegm.RoIhalfWidthDeltaEta = 3.
                self.L2SequenceHash[self.trtsegm.name()]=[self.trtsegm]
                self.L2SequenceList+=[self.trtsegm.name()]
                
                #ToolSvc.InDetL2_TRT_TrackSegmentsMaker.OutputLevel=4
                #ToolSvc.InDetL2_TRT_TrackSegmentsMaker_BarrelCosmics.OutputLevel=4

                if (sig_id.count('TRTTrkHypo')):
                    from TrigL2CosmicMuonHypo.TrigL2CosmicMuonHypo_Config import CosmicTrtHypo_Cosmic
                    self.trthypoCosmics = CosmicTrtHypo_Cosmic("Cosmic"+sig_id+"TrtHypo") 
                    self.trthypoCosmics.NTrthitsCut = 15
                    self.trtsegm.SegmentsMakerTool.MinimalNumberOfTRTHits = 15
                    self.trtsegm.pTmin = 100.0

                    self.L2SequenceHash[self.trtsegm.name()]+=[self.trthypoCosmics]
                    if (sig_id.count('Pixel')):
                        self.trthypoCosmics.TrtSegD0Cut=250.0

                if (LocalFlags.doIDHitsHypo):
                    from TrigEFIDCosmic.IDHitsHypo_Config import IDHitsHypo_TRTXK_L2
                    self.IDHitsHypo_L2=IDHitsHypo_TRTXK_L2("Cosmic"+sig_id+"IDHitsHypo_TRTXK_L2")
                    self.L2SequenceHash[self.trtsegm.name()]+=[self.IDHitsHypo_L2]

                if (sig_id.count('AllPhysics')):
                    # Run on every physics trigger
                    self.L1InfoHypo.TriggerTypeBitMask=0x80
                    self.L1InfoHypo.TriggerTypeBit    =0x80
                    self.L1InfoHypo.InvertSelection   =False

                if (sig_id.count('PEB')):
                    from TrigDetCalib.TrigDetCalibConfig import TrkROBListWriter
                    self.robListWriter = TrkROBListWriter('TrkROBListWriter')
                    self.robListWriter.RoIDescriptorLabels = [""]
                    self.L2SequenceHash[self.trtsegm.name()]+=[self.robListWriter]
                mlog.info(self.trtsegm)
        if self.configEF:
            if eval(self.Flag):
                if (LocalFlags.doEFIDCosmics):
                    from TrigEFIDCosmic.TrigEFIDCosmic_Config import TrigEFIDCosmicAlgo_TRTXK_EF
                    self.TrigEFIDCosmicAlgo_EF=TrigEFIDCosmicAlgo_TRTXK_EF("Cosmic"+sig_id+"TrigEFIDCosmicAlgo_TRTXK_EF")
                    self.EFSequenceHash[self.TrigEFIDCosmicAlgo_EF.name()]=[self.TrigEFIDCosmicAlgo_EF]
                    self.EFSequenceList+=[self.TrigEFIDCosmicAlgo_EF.name()]
                    if (LocalFlags.doIDHitsHypo):
                        from TrigEFIDCosmic.IDHitsHypo_Config import IDHitsHypo_TRTXK_EF
                        self.IDHitsHypo_EF=IDHitsHypo_TRTXK_EF("COsmic"+sig_id+"IDHitsHypoEF")
                        self.EFSequenceHash[self.IDHitsHypo_EF.name()]=[self.IDHitsHypo_EF]
                        self.EFSequenceList+=[self.IDHitsHypo_EF.name()]

########################################  tileRODMu slice  ###############################################################
class CosmicTileRODMu(CosmicTemplate):
    def __init__(self, sig_id, l1item, l1threshold, counterId,StreamName):
        CosmicTemplate.__init__(self, sig_id, l1item, l1threshold, counterId,StreamName,groupNames=["Cosmic","Cosmic_TileAlgos"])
        mlog = logging.getLogger( 'Cosmic.py:CosmicTileRODMu' )

        self.__supported = ["TileRODMu","StandaloneTileRODMu","CaloTileRODMu","CaloTileRODMu_V2","AllTeTileRODMu"]        
        if sig_id not in self.__supported:
            raise Exception("ERROR: "+sig_id+" not supprted"+str(self.__supported) )

        self.Flag='TriggerFlags.CosmicSlice.do%s()' % sig_id
        if self.configLVL2:
            if eval(self.Flag):
                if (sig_id.count("AllTe")):
                    self.L1InfoHypo.InvertSelection   =False
                    self.L1InfoHypo.TriggerTypeBitMask=0x80
                    self.L1InfoHypo.TriggerTypeBit    =0x80
                    self.AllTEDummyFakeROI.FakeROIEta=0.0
                    self.AllTEDummyFakeROI.FakeROIPhi=0.0
                    #self.AllTEDummyFakeROI.FakeROIEta=1.7
                    #self.AllTEDummyFakeROI.FakeROIPhi=1.5

                from TrigTileMuId.TrigTileRODMuAlgConfig import TrigTileRODMuAlg_L2
                self.TrigTileRODMuAlg = TrigTileRODMuAlg_L2("Cosmic"+sig_id+"TileRODMuAlg")
                self.L2SequenceHash[self.TrigTileRODMuAlg.name()]=[self.TrigTileRODMuAlg]
                self.L2SequenceList+=[self.TrigTileRODMuAlg.name()]

        if self.configEF:
            if eval(self.Flag):
                from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
                self.EF_Dummy = DummyFEX("EF_%s" % sig_id)
                self.EFSequenceHash[self.EF_Dummy.name()]=[self.EF_Dummy]
                self.EFSequenceList+=[self.EF_Dummy.name()]

########################################  tilelookForMu slice  ############################################################
class CosmicTileLookForMu(CosmicTemplate):
    def __init__(self, sig_id, l1item, l1threshold, counterId,StreamName):
        CosmicTemplate.__init__(self, sig_id, l1item, l1threshold, counterId,StreamName,groupNames=["Cosmic","Cosmic_TileAlgos"])
        mlog = logging.getLogger( 'Cosmic.py:CosmicTileLookForMu' )

        self.__supported = ["TileLookForMu","StandaloneTileLookForMu","CaloTileLookForMu","CaloTileLookForMu_V2","AllTeTileLookForMu"]        
        if sig_id not in self.__supported:
            raise Exception("ERROR: "+sig_id+" not supprted"+str(self.__supported) )

        self.Flag='TriggerFlags.CosmicSlice.do%s()' % sig_id
        if self.configLVL2:
            if eval(self.Flag):
                if (sig_id.count('AllTe')):
                    self.L1InfoHypo.InvertSelection   =False
                    self.L1InfoHypo.TriggerTypeBitMask=0x80
                    self.L1InfoHypo.TriggerTypeBit    =0x80
                    self.AllTEDummyFakeROI.FakeROIEta=0.0
                    self.AllTEDummyFakeROI.FakeROIPhi=0.0
                    #self.AllTEDummyFakeROI.FakeROIEta=1.7
                    #self.AllTEDummyFakeROI.FakeROIPhi=1.5

                from TrigTileMuId.TrigTileLookForMuAlgConfig import TrigTileLookForMuAlg_L2
                self.TrigTileLookForMuAlg = TrigTileLookForMuAlg_L2("Cosmic"+sig_id+"TileLookForMuAlg")
                self.L2SequenceHash[self.TrigTileLookForMuAlg.name()]=[self.TrigTileLookForMuAlg]
                self.L2SequenceList+=[self.TrigTileLookForMuAlg.name()]

        if self.configEF:
            if eval(self.Flag):
                from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
                self.EF_Dummy = DummyFEX("EF_%s" % sig_id)
                self.EFSequenceHash[self.EF_Dummy.name()]=[self.EF_Dummy]
                self.EFSequenceList+=[self.EF_Dummy.name()]

########################################  Egamma slice  ###############################################################
class CosmicEgamma(CosmicTemplate):
    def __init__(self, sig_id, l1item, l1threshold, counterId,StreamName):
        CosmicTemplate.__init__(self, sig_id, l1item, l1threshold, counterId,StreamName,groupNames=["Cosmic","Cosmic_Calo"])
        mlog = logging.getLogger( 'Cosmic.py:CosmicEgamma' )

        self.__supported = ["Egamma","Egamma_V2","Egamma_noHad","AllTeEgamma"]        
        if sig_id not in self.__supported:
            raise Exception("ERROR: "+sig_id+" not supprted"+str(self.__supported) )

        self.Flag='TriggerFlags.CosmicSlice.do%s()' % sig_id

        if self.configLVL2:
            if eval(self.Flag):
                if (sig_id.count('AllTe')):
                    self.L1InfoHypo.InvertSelection   =False
                    self.L1InfoHypo.TriggerTypeBitMask=0x80
                    self.L1InfoHypo.TriggerTypeBit    =0x80
                    #self.AllTEDummyFakeROI.FakeROIEta=0.0
                    #self.AllTEDummyFakeROI.FakeROIPhi=0.0
                    self.AllTEDummyFakeROI.FakeROIEta=-0.5
                    self.AllTEDummyFakeROI.FakeROIPhi=1.5
                    
                if (sig_id.count('noHad')):
                    from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_eGamma_NoHad
                    self.l2_T2CaloEgamma = T2CaloEgamma_eGamma_NoHad("Cosmic"+sig_id+"T2CaloEgamma_eGamma_noHad")
                else:
                    from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_eGamma
                    self.l2_T2CaloEgamma = T2CaloEgamma_eGamma("Cosmic"+sig_id+"T2CaloEgamma_eGamma")

                if (LocalFlags.T2CaloFakeLVL1InLVL2):
                    self.l2_T2CaloEgamma.L1ForcePhi=-1.5
                    self.l2_T2CaloEgamma.L1ForceEta=1.7

                from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_g7
                from TrigEgammaHypo.TrigL2PhotonFexConfig import L2PhotonFex_1
                from TrigEgammaHypo.TrigL2PhotonHypoConfig import L2PhotonHypo_g10_loose
                self.l2_L2PhotonHypo_g10_loose = L2PhotonHypo_g10_loose("Cosmic"+sig_id+"L2PhotonHypo_g10_loose")
                self.l2_L2PhotonFex_1 = L2PhotonFex_1("Cosmic"+sig_id+"L2PhotonFex_1")
                self.l2_L2CaloHypo_g7 = L2CaloHypo_g7("Cosmic"+sig_id+"L2CaloHypo_g7")

                self.l2_L2PhotonHypo_g10_loose.ETthr = [-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV]
                self.l2_L2PhotonHypo_g10_loose.AcceptAll=False
                self.l2_L2CaloHypo_g7.ETthr = [-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV]
                self.l2_L2CaloHypo_g7.AcceptAll=False
                

                self.L2SequenceHash[self.l2_T2CaloEgamma.name()]=[self.l2_T2CaloEgamma,self.l2_L2CaloHypo_g7]
                self.L2SequenceList+=[self.l2_T2CaloEgamma.name()]
                self.L2SequenceHash["Cosmic"+sig_id+"L2Photon"]=[self.l2_L2PhotonFex_1,self.l2_L2PhotonHypo_g10_loose]
                self.L2SequenceList+=["Cosmic"+sig_id+"L2Photon"]
                
                include("TrigT2CaloCommon/LArCellBuilding.py")

        if self.configEF:
            if eval(self.Flag):
                if (sig_id.count('noHad')):
                    from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
                    self.EF_Dummy = DummyFEX("Cosmic_EF_%s" % sig_id)
                    self.EFSequenceHash[self.EF_Dummy.name()]=[self.EF_Dummy]
                    self.EFSequenceList+=[self.EF_Dummy.name()]
                else:
                    #from TrigEgammaHypo.TrigEFEgammaHypoConfig import *

                    from TrigEgammaRec.TrigEgammaRecConfig import TrigEgammaRec_eGamma ,TrigEgammaRec_NoIDEF_eGamma
                    self.TrigEgammaRec_NoIDEF_eGamma=TrigEgammaRec_NoIDEF_eGamma("Cosmic"+sig_id+"TrigEgammaRec_NoIDEF_eGamma")
                    
                    from TrigEgammaHypo.TrigEFPhotonHypoConfig import EFPhotonHypo_g10_loose
                    self.EFPhotonHypo_g10_loose=EFPhotonHypo_g10_loose("Cosmic"+sig_id+"EFPhotonHypo_g10_loose")
                    self.EFPhotonHypo_g10_loose.etCut = [-7.*GeV, -7.*GeV, -7.*GeV, -7.*GeV, -7.*GeV]
                    self.EFPhotonHypo_g10_loose.AcceptAll=False
                    
                    from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_eGamma, TrigCaloTowerMaker_eGamma, TrigCaloClusterMaker_slw
                    self.TrigCaloCellMaker_eGamma =TrigCaloCellMaker_eGamma("Cosmic"+sig_id+"TrigCaloCellMaker_eGamma")
                    self.TrigCaloTowerMaker_eGamma=TrigCaloTowerMaker_eGamma("Cosmic"+sig_id+"TrigCaloTowerMaker_eGamma")
                    self.TrigCaloClusterMaker_slw=TrigCaloClusterMaker_slw("Cosmic"+sig_id+"TrigCaloClusterMaker_slw")

                    #from CaloRec.CaloRecConf import CaloClusterBuilderSW
                    #self.CaloClusterBuilderSW = CaloClusterBuilderSW("trigslw"+sig_id)
                    #self.CaloClusterBuilderSW.e_threshold=-1.*GeV
                    #
                    #self.TrigCaloClusterMaker_slw.ClusterMakerTools=[self.CaloClusterBuilderSW.getFullName()]
                    #self.TrigCaloClusterMaker_slw+=self.CaloClusterBuilderSW
                    self.TrigCaloClusterMaker_slw.trigslw.e_threshold=-1.*GeV

                    self.EFSequenceHash["Cosmic_"+sig_id+"EF_g10calo"]=[self.TrigCaloCellMaker_eGamma,self.TrigCaloTowerMaker_eGamma,self.TrigCaloClusterMaker_slw]
                    self.EFSequenceList+=["Cosmic_"+sig_id+"EF_g10calo"]

                    self.EFSequenceHash["Cosmic_"+sig_id+"EF_g10"]=[self.TrigEgammaRec_NoIDEF_eGamma,self.EFPhotonHypo_g10_loose]
                    self.EFSequenceList+=["Cosmic_"+sig_id+"EF_g10"]

                    

        if self.configEF:
            if eval(self.Flag):
                from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
                self.EF_Dummy = DummyFEX("EF_%s" % sig_id)
                self.EFSequenceHash[self.EF_Dummy.name()]=[self.EF_Dummy]
                self.EFSequenceList+=[self.EF_Dummy.name()]

    def generateMenu(self,triggerPythonConfig):
        CosmicTemplate.generateMenu(self,triggerPythonConfig)
#        if (eval(self.Flag)):
#            include("TrigT2CaloCommon/LArCellBuilding.py")

########################################  Jet slice  ###############################################################
class CosmicJet(CosmicTemplate):
    def __init__(self, sig_id, l1item, l1threshold, counterId,StreamName):
        CosmicTemplate.__init__(self, sig_id, l1item, l1threshold, counterId,StreamName,groupNames=["Cosmic","Cosmic_Jet"])
        mlog = logging.getLogger( 'Cosmic.py:CosmicJet' )

        self.__supported = ["Jet","Jet_V2","AllTeJet"]        
        if sig_id not in self.__supported:
            raise Exception("ERROR: "+sig_id+" not supprted"+str(self.__supported) )

        self.Flag='TriggerFlags.CosmicSlice.do%s()' % sig_id

        if self.configLVL2:
            if eval(self.Flag):
                if (sig_id.count('AllTe')):
                    self.L1InfoHypo.InvertSelection   =False
                    self.L1InfoHypo.TriggerTypeBitMask=0x80
                    self.L1InfoHypo.TriggerTypeBit    =0x80
                    #self.AllTEDummyFakeROI.FakeROIEta=0.0
                    #self.AllTEDummyFakeROI.FakeROIPhi=0.0
                    self.AllTEDummyFakeROI.FakeROIEta=-0.5
                    self.AllTEDummyFakeROI.FakeROIPhi=1.5
                from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloJet_Jet
                self.l2_T2CaloJet_Jet = T2CaloJet_Jet("Cosmic"+sig_id+"T2CaloJet_Jet")
                #self.l2_T2CaloJet_Jet.OutputLevel=DEBUG
                from TrigJetHypo.TrigJetHypoConfig import L2JetHypo
                self.l2_TrigL2JetHypo_Jet = L2JetHypo("Cosmic"+sig_id+"L2JetHypo")
                self.l2_TrigL2JetHypo_Jet.Etcut_L2 = -1*GeV
                #self.TrigCaloTauHypo_tau10.AcceptAll=True
                self.L2SequenceHash[self.l2_T2CaloJet_Jet.name()]=[self.l2_T2CaloJet_Jet,self.l2_TrigL2JetHypo_Jet]
                self.L2SequenceList+=[self.l2_T2CaloJet_Jet.name()]
                
                include("TrigT2CaloCommon/LArCellBuilding.py")

        if self.configEF:
            if eval(self.Flag):
                from TrigJetRec.TrigJetRecConfig import TrigJetRec_Cone
                from TrigJetHypo.TrigJetHypoConfig import EFJetHypo
                from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_jet, TrigCaloTowerMaker_jet, TrigCaloClusterMaker_slw
                self.TrigCaloCellMaker_jet =TrigCaloCellMaker_jet("Cosmic"+sig_id+"TrigCaloCellMaker_jet")
                self.TrigCaloTowerMaker_jet=TrigCaloTowerMaker_jet("Cosmic"+sig_id+"TrigCaloTowerMaker_jet")
                self.TrigJetRec_Cone=TrigJetRec_Cone()
                #self.JetRecFinalEtCutTool=self.TrigJetRec_Cone.FinalEtCut
                #self.JetRecFinalEtCutTool.MinimumSignal=0.*GeV
                self.EFJetHypo = EFJetHypo("Cosmic"+sig_id+"EFJetHypo_j10",ef_thr=-1*GeV)
                
                self.EFSequenceHash["Cosmic_"+sig_id+"j10"]=[self.TrigCaloCellMaker_jet,self.TrigCaloTowerMaker_jet,self.TrigJetRec_Cone,self.EFJetHypo]
                self.EFSequenceList+=["Cosmic_"+sig_id+"j10"]
                # from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
                # self.EF_Dummy = DummyFEX("Cosmic_EF_%s" % sig_id)
                # self.EFSequenceHash[self.EF_Dummy.name()]=[self.EF_Dummy]
                # self.EFSequenceList+=[self.EF_Dummy.name()]

    def generateMenu(self,triggerPythonConfig):
        CosmicTemplate.generateMenu(self,triggerPythonConfig)
#        if (eval(self.Flag)):
#            include("TrigT2CaloCommon/LArCellBuilding.py")

########################################  Tau slice  ###############################################################
class CosmicTau(CosmicTemplate):
    def __init__(self, sig_id, l1item, l1threshold, counterId,StreamName):
        CosmicTemplate.__init__(self, sig_id, l1item, l1threshold, counterId,StreamName,groupNames=["Cosmic","Cosmic_Tau"])
        mlog = logging.getLogger( 'Cosmic.py:CosmicTau' )

        self.__supported = ["Tau","Tau_V2","AllTeTau"]        
        if sig_id not in self.__supported:
            raise Exception("ERROR: "+sig_id+" not supprted"+str(self.__supported) )

        self.Flag='TriggerFlags.CosmicSlice.do%s()' % sig_id

        if self.configLVL2:
            if eval(self.Flag):
                if (sig_id.count('AllTe')):
                    self.L1InfoHypo.InvertSelection   =False
                    self.L1InfoHypo.TriggerTypeBitMask=0x80
                    self.L1InfoHypo.TriggerTypeBit    =0x80
                    #self.AllTEDummyFakeROI.FakeROIEta=0.0
                    #self.AllTEDummyFakeROI.FakeROIPhi=0.0
                    self.AllTEDummyFakeROI.FakeROIEta=-0.5
                    self.AllTEDummyFakeROI.FakeROIPhi=1.5
                from TrigT2CaloTau.TrigT2CaloTauConfig import T2CaloTau_Tau
                self.l2_T2CaloTau_Tau = T2CaloTau_Tau("Cosmic"+sig_id+"T2CaloTau_Tau")
                #self.l2_T2CaloTau_Tau.OutputLevel=DEBUG
                from TrigTauHypo.TrigTauHypoConfig_tauNoCut import T2CaloTauHypo_tauNoCut
                self.l2_T2CaloTauHypo_tauNoCut=T2CaloTauHypo_tauNoCut("Cosmic"+sig_id+"T2CaloTauHypo_tauNoCut")
                #TrigCaloTauHypo_tau10.AcceptAll=True
                self.L2SequenceHash[self.l2_T2CaloTau_Tau.name()]=[self.l2_T2CaloTau_Tau,self.l2_T2CaloTauHypo_tauNoCut]
                self.L2SequenceList+=[self.l2_T2CaloTau_Tau.name()]
                
                include("TrigT2CaloCommon/LArCellBuilding.py")

        if self.configEF:
            if eval(self.Flag):
                from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
                self.EF_Dummy = DummyFEX("Cosmic_EF_%s" % sig_id)
                self.EFSequenceHash[self.EF_Dummy.name()]=[self.EF_Dummy]
                self.EFSequenceList+=[self.EF_Dummy.name()]

    def generateMenu(self,triggerPythonConfig):
        CosmicTemplate.generateMenu(self,triggerPythonConfig)
#        if (eval(self.Flag)):
#            include("TrigT2CaloCommon/LArCellBuilding.py")

########################################  Calo slice  ###############################################################
class CosmicCalo(CosmicTemplate):
    def __init__(self, sig_id, l1item, l1threshold, counterId,StreamName):
        CosmicTemplate.__init__(self, sig_id, l1item, l1threshold, counterId,StreamName,groupNames=["Cosmic","Cosmic_Calo"])
        mlog = logging.getLogger( 'Cosmic.py:CosmicCalo' )

        self.__supported = ["Calo","AllTeCalo"]        
        if sig_id not in self.__supported:
            raise Exception("ERROR: "+sig_id+" not supprted"+str(self.__supported) )

        self.Flag='TriggerFlags.CosmicSlice.do%s()' % sig_id

        if self.configLVL2:
            if eval(self.Flag):
                if (sig_id.count('AllTe')):
                    self.L1InfoHypo.InvertSelection   =False
                    self.L1InfoHypo.TriggerTypeBitMask=0x80
                    self.L1InfoHypo.TriggerTypeBit    =0x80
                    #self.AllTEDummyFakeROI.FakeROIEta=0.0
                    #self.AllTEDummyFakeROI.FakeROIPhi=0.0
                    self.AllTEDummyFakeROI.FakeROIEta=1.7
                    self.AllTEDummyFakeROI.FakeROIPhi=1.5
                    self.AllTEDummyFakeROI.CheckForInputTE=True
                from TrigT2CaloCosmic.TrigT2CaloCosmicConfig import T2CaloCosmic_Cosmic
                self.l2_T2CaloCosmic_Cosmic = T2CaloCosmic_Cosmic("Cosmic"+sig_id+"T2CaloCosmic_Cosmic")
                self.l2_T2CaloCosmic_Cosmic.EtaMin=[-1.0, -1.0, -1.0, -1.0]
                self.l2_T2CaloCosmic_Cosmic.EtaMax=[1.0, 1.0, 1.0, 1.0 ]
                self.l2_T2CaloCosmic_Cosmic.PhiMin=[1.3, -1.8, 1.3, -1.8]
                self.l2_T2CaloCosmic_Cosmic.PhiMax=[1.8, -1.3, 1.8, -1.3]
                self.l2_T2CaloCosmic_Cosmic.EtaWidth = 0.5
                self.l2_T2CaloCosmic_Cosmic.PhiWidth = 0.5
                #self.l2_T2CaloCosmic_Cosmic.OutputLevel=DEBUG
                self.L2SequenceHash[self.l2_T2CaloCosmic_Cosmic.name()]=[self.l2_T2CaloCosmic_Cosmic]
                self.L2SequenceList+=[self.l2_T2CaloCosmic_Cosmic.name()]
                
                include("TrigT2CaloCommon/LArCellBuilding.py")
                
        if self.configEF:
            if eval(self.Flag):
                from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
                self.EF_Dummy = DummyFEX("Cosmic_EF_%s" % sig_id)
                self.EFSequenceHash[self.EF_Dummy.name()]=[self.EF_Dummy]
                self.EFSequenceList+=[self.EF_Dummy.name()]

    def generateMenu(self,triggerPythonConfig):
        CosmicTemplate.generateMenu(self,triggerPythonConfig)
#        if (eval(self.Flag)):
#            include("TrigT2CaloCommon/LArCellBuilding.py")


########################################  cosmicmuon slice  ############################################################
class CosmicMuon(CosmicTemplate):
    def __init__(self, sig_id, l1item, l1threshold, counterId,StreamName):
        CosmicTemplate.__init__(self, sig_id, l1item, l1threshold, counterId,StreamName,groupNames=["Cosmic","Cosmic_Muon"])
        mlog = logging.getLogger( 'Cosmic.py:CosmicMuon' )

        
        #MooreRoI is deprecated. 
#        self.__supported = ["MuonOnly","Muon","MuonOnly_TGC","Muon_Moore","Muon_Moore_TGC","Muon_MuonEF","Muon_MuonEF_TGC"]        
        self.__supported = ["MuonOnly","Muon","MuonOnly_TGC","Muon_MuonEF","Muon_MuonEF_TGC","MuonOnly_V2","Muon_V2","Muon_MuonEF_V2",]

        if sig_id not in self.__supported:
            raise Exception("ERROR: "+sig_id+" not supprted"+str(self.__supported) )

        self.Flag='TriggerFlags.CosmicSlice.do%s()' % sig_id

        
        
        if self.configLVL2:
            if eval(self.Flag):
                if (TriggerFlags.CosmicSlice.testCosmic()==True):
                    mlog.info( "Now initializing L2 %s" % sig_id)
                
                from TrigL2CosmicMuon.TrigL2CosmicMuon_Config import TrigL2CosmicMuon_Cosmic
                from TrigL2CosmicMuonHypo.TrigL2CosmicMuonHypo_Config import CosmicMuonHypo_Cosmic

                self.l2_TrigL2CosmicMuon = TrigL2CosmicMuon_Cosmic("Cosmic"+sig_id+"TrigL2CosmicMuon")
                self.l2_CosmicMuonHypo = CosmicMuonHypo_Cosmic("Cosmic"+sig_id+"CosmicMuonHypo")
                self.L2SequenceHash[self.l2_TrigL2CosmicMuon.name()]=[self.l2_TrigL2CosmicMuon,self.l2_CosmicMuonHypo]
                self.L2SequenceList+=[self.l2_TrigL2CosmicMuon.name()]

                if (sig_id.count('TGC')):
                    
                    self.l2_TrigL2CosmicMuon.UseTgcSeed = True
                    self.l2_TrigL2CosmicMuon.UseMdtTimingCut = False
                    #from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram
                    #self.l2_TrigL2CosmicMuon.AthenaMonTools[0].Histograms[6] = defineHistogram('MdtT0', 'TH1F', 'EXPERT', 'MDT T0 (ns)', 250, 0, 1000)
                    self.l2_CosmicMuonHypo.NRpcHits = 1
                    self.l2_CosmicMuonHypo.NMuons = 1
                else:
                    
                    self.l2_TrigL2CosmicMuon.UseTgcSeed = False
                    self.l2_TrigL2CosmicMuon.UseMdtTimingCut = False
                    #from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram
                    #self.l2_TrigL2CosmicMuon.AthenaMonTools[0].Histograms[6] = defineHistogram('MdtT0', 'TH1F', 'EXPERT', 'MDT T0 (ns)', 250, 0, 1000)
                    self.l2_CosmicMuonHypo.NRpcHits = 1
                    self.l2_CosmicMuonHypo.NMuons = 1
                
                if (sig_id.count('muonOnly')==0):
                    
                    if (LocalFlags.doIDHitsHypo):
                        
                        from TrigEFIDCosmic.IDHitsHypo_Config import IDHitsHypo_muon_L2
                        self.IDHitsHypo_L2=IDHitsHypo_muon_L2(sig_id+"IDHitsHypo_muon_L2")
                        self.L2SequenceHash[self.IDHitsHypo_L2.name()]=[self.IDHitsHypo_L2]
                        self.L2SequenceList+=[self.IDHitsHypo_L2.name()]

                from AthenaCommon.AppMgr import ToolSvc
                ToolSvc.L2MdtTool.FitMethod = 2
                if (LocalFlags.doM3Data):
                    
                    ToolSvc.L2MdtTool.AddOneToCsmId = True
                else:
                    ToolSvc.L2MdtTool.AddOneToCsmId = False
                
                #ToolSvc.L2DataLoader.MdtRobMapperFile = 'RoiROBmap_P03.data' 
                ToolSvc.L2DataLoader.MdtRobMapperFile = 'RoiROBmap.data' 
                ToolSvc.MdtTimingTool.NominalT0 = 624
                ToolSvc.MdtTimingTool.RoadCut = 0.5
                ToolSvc.MdtTimingTool.SegmentChi2 = 0.2
                ToolSvc.MdtTimingTool.HitMaxChi2 = 0.2
                ToolSvc.L2RpcTool.AllowOnePairTrack = True
                    

        if self.configEF:
            
            if eval(self.Flag):
                
                if (TriggerFlags.CosmicSlice.testCosmic()==True):
                    mlog.info( "Now initializing EF %s" % sig_id)
                if (sig_id.count('muonOnly')):
                    
                    from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
                    self.EF_Dummy = DummyFEX("EF_%s" % sig_id)
                    self.EFSequenceHash[self.EF_Dummy.name()]=[self.EF_Dummy]
                    self.EFSequenceList+=[self.EF_Dummy.name()]
                else:
                    
#TrigMoore is deprecated
#                    if (sig_id.count('Moore')):

#                        from TrigMoore.TrigMooreConfig import TrigMooreConfig_CS

#                        if (TriggerFlags.CosmicSlice.useUniqueMooreInstance()):
#                            self.ef_TrigMooreConfig_CS = TrigMooreConfig_CS('Cosmic'+'TrigMooreConfig_CS')
#                        else:
#                            self.ef_TrigMooreConfig_CS = TrigMooreConfig_CS('Cosmic'+sig_id+'TrigMooreConfig_CS')

                        #include("TrigMoore/jobOfragment_TrigMoore.py")
#                        self.EFSequenceHash[self.ef_TrigMooreConfig_CS.name()+"_"+l1item+"_"+self.sig_id]=[self.ef_TrigMooreConfig_CS]
#                        self.EFSequenceList+=[self.ef_TrigMooreConfig_CS.name()+"_"+l1item+"_"+self.sig_id]

                    if (sig_id.count('MuonEF')):
                        from TrigMuonEF.TrigMuonEFConfig import TrigMuonEFSegmentFinderConfig,TrigMuonEFTrackBuilderConfig
                        if (LocalFlags.useUniqueMuonEFInstance):
                            self.ef_TrigMuonEFSegmentFinderConfig = TrigMuonEFSegmentFinderConfig('Cosmic'+'TrigMuonEFSegmentFinderConfig')
                            self.ef_TrigMuonEFTrackBuilderConfig = TrigMuonEFTrackBuilderConfig('Cosmic'+'TrigMuonEFTrackBuilderConfig')
                        else:
                            self.ef_TrigMuonEFSegmentFinderConfig = TrigMuonEFSegmentFinderConfig('Cosmic'+sig_id+'TrigMuonEFSegmentFinderConfig')
                            self.ef_TrigMuonEFTrackBuilderConfig = TrigMuonEFTrackBuilderConfig('Cosmic'+sig_id+'TrigMuonEFTrackBuilderConfig')
                        self.EFSequenceHash[self.ef_TrigMuonEFSegmentFinderConfig.name()+"_"+l1item+"_"+self.sig_id]=[self.ef_TrigMuonEFSegmentFinderConfig,self.ef_TrigMuonEFTrackBuilderConfig]
                        self.EFSequenceList+=[self.ef_TrigMuonEFSegmentFinderConfig.name()+"_"+l1item+"_"+self.sig_id]
                        #self.EFSequenceHash[self.ef_TrigMuonEFTrackBuilderConfig.name()+"_"+l1item+"_"+self.sig_id]=[self.ef_TrigMuonEFTrackBuilderConfig]
                        #self.EFSequenceList+=[self.ef_TrigMuonEFTrackBuilderConfig.name()+"_"+l1item+"_"+self.sig_id]

                        
                    else:
                        if (LocalFlags.doEFIDCosmics):
                            from TrigEFIDCosmic.TrigEFIDCosmic_Config import TrigEFIDCosmicAlgo_muon_EF
                            self.TrigEFIDCosmicAlgo_EF=TrigEFIDCosmicAlgo_muon_EF(sig_id+"TrigEFIDCosmicAlgo_muon_EF")
                            self.EFSequenceHash[self.TrigEFIDCosmicAlgo_EF.name()]=[self.TrigEFIDCosmicAlgo_EF]
                            self.EFSequenceList+=[self.TrigEFIDCosmicAlgo_EF.name()]

                            if (LocalFlags.doIDHitsHypo):
                                from TrigEFIDCosmic.IDHitsHypo_Config import IDHitsHypo_muon_EF
                                self.IDHitsHypo_EF=IDHitsHypo_muon_EF(sig_id+"IDHitsHypo_muon_EF")
                                self.EFSequenceHash[self.IDHitsHypo_EF.name()]=[self.IDHitsHypo_EF]
                                self.EFSequenceList+=[self.IDHitsHypo_EF.name()]
                        else:
                            from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
                            self.EF_Dummy = DummyFEX("EF_%s" % sig_id)
                            self.EFSequenceHash[self.EF_Dummy.name()]=[self.EF_Dummy]
                            self.EFSequenceList+=[self.EF_Dummy.name()]



    def generateMenu(self,triggerPythonConfig):
        
        if eval(self.Flag):
            
            CosmicTemplate.generateMenu(self,triggerPythonConfig)
#            from AthenaCommon.AppMgr import ToolSvc
#            ToolSvc.L2MdtTool.FitMethod = 2
#            if (LocalFlags.doM3Data):
#                
#                ToolSvc.L2MdtTool.AddOneToCsmId = True
#            else:
#                ToolSvc.L2MdtTool.AddOneToCsmId = False
#
#            #ToolSvc.L2DataLoader.MdtRobMapperFile = 'RoiROBmap_P03.data' 
#            ToolSvc.L2DataLoader.MdtRobMapperFile = 'RoiROBmap.data' 
#            ToolSvc.MdtTimingTool.NominalT0 = 624
#            ToolSvc.MdtTimingTool.RoadCut = 0.5
#            ToolSvc.MdtTimingTool.SegmentChi2 = 0.2
#            ToolSvc.MdtTimingTool.HitMaxChi2 = 0.2
#            ToolSvc.L2RpcTool.AllowOnePairTrack = True

########################################  cosmicmufast slice  ############################################################
class CosmicMufast(CosmicTemplate):
    def __init__(self, sig_id, l1item, l1threshold, counterId,StreamName):
        CosmicTemplate.__init__(self, sig_id, l1item, l1threshold, counterId,StreamName,groupNames=["Cosmic","Cosmic_Muon"])
        mlog = logging.getLogger( 'Cosmic.py:CosmicMufast' )

#   TrigMoore is deprecated
#        self.__supported = ["Mufast","Mufast_TGC","Mufast_Moore","Mufast_Moore_TGC","Mufast_MuIso","Mufast_MuonEF",
#                            "Mufast_MuonEF_TGC","Mufast_WideRPC","Mufast_Moore_WideRPC","Mufast_MuonEF_WideRPC"]
        self.__supported = ["Mufast","Mufast_TGC","Mufast_MuIso","Mufast_MuonEF",
                            "Mufast_MuonEF_TGC","Mufast_WideRPC","Mufast_MuonEF_WideRPC",
                            "Mufast_V2","Mufast_MuIso_V2","Mufast_MuonEF_V2"]        
        if sig_id not in self.__supported:
            raise Exception("ERROR: "+sig_id+" not supprted"+str(self.__supported) )

        self.Flag='TriggerFlags.CosmicSlice.do%s()' % sig_id
        if self.configLVL2:
            if eval(self.Flag):
                from TrigMuonHypo.TrigMuonHypoConfig import MufastHypoConfig
                from TrigmuFast.TrigmuFastConfig import TrigmuFastConfig

                if (l1item not in MuFastInstances):
                    if (TriggerFlags.CosmicSlice.testCosmic()==True):
                        mlog.info( "creating mufast instance based on item "+str(l1item))
                    if (LocalFlags.useUniqueMuFastInstance):
                        self.TrigmuFastConfig = TrigmuFastConfig("Cosmic_TrigmuFastConfig_900GeV")
                        self.MufastHypoConfig = MufastHypoConfig("Cosmic_MufastHypoConfig_900Gev",'2GeV')
                        sequencename=l1item+self.TrigmuFastConfig.name()+self.sig_id
                    else:
                        self.TrigmuFastConfig = TrigmuFastConfig("Cosmic"+sig_id+"TrigmuFastConfig_900GeV")
                        self.MufastHypoConfig = MufastHypoConfig("Cosmic"+sig_id+"MufastHypoConfig_900Gev",'2GeV')
                        sequencename=self.TrigmuFastConfig.name()

                    self.L2SequenceHash[sequencename]=[self.TrigmuFastConfig,self.MufastHypoConfig]
                    self.L2SequenceList+=[sequencename]
                    MuFastInstances[l1item]=[self.TrigmuFastConfig,self.MufastHypoConfig]
                    self.ownMufast=True
                else:
                    if (TriggerFlags.CosmicSlice.testCosmic()==True):
                        mlog.info( "mufast instance based on item "+str(l1item)+" already existing")
                    self.L2SequenceHash[l1item+MuFastInstances[l1item][0].name()+self.sig_id]=MuFastInstances[l1item]
                    self.L2SequenceList+=[l1item+MuFastInstances[l1item][0].name()+self.sig_id]
                    self.ownMufast=False
                    
                if (sig_id.count('MuIso')):
                    from TrigMuonHypo.TrigMuonHypoConfig import MuisoHypoConfig
                    from TrigmuIso.TrigmuIsoConfig import muIsoConfig
                    self.muIsoConfig=muIsoConfig("Cosmic"+sig_id+"muIsoConfig")
                    self.muIsoConfig.UseCombinedMuon = False
                    self.MuisoHypoConfig=MuisoHypoConfig("Cosmic"+sig_id+"MuisoHypoConfig")
                    self.L2SequenceHash[self.muIsoConfig.name()]=[self.muIsoConfig,self.MuisoHypoConfig]
                    self.L2SequenceList+=[self.muIsoConfig.name()]

                if (self.ownMufast):
                    muFast=self.TrigmuFastConfig
                    muFast.OnlineRun=LocalFlags.doMuonCalibrationStream
                    
                    muFast.UseCalibration=True
                    muFast.MaskUncCMAch = False
                    muFast.CosmicRun   = True
                    muFast.Timing      = False
                    #if (self.sig_id.count('TGC')):
                    #    muFast.RemoveTgcTriggers=False
                    #else:
                    #    muFast.RemoveTgcTriggers=True

                    
                if (self.sig_id.count('MuIso')):
                    include("TrigT2CaloCommon/LArCellBuilding.py")
                #if (sig_id.count('TGC')):
                #else:

                
        if self.configEF:
            if eval(self.Flag):
                if (sig_id=='Mufast' or sig_id.count('MuIso')):
                    from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
                    self.EF_Dummy = DummyFEX("EF_%s" % sig_id)
                    self.EFSequenceHash[self.EF_Dummy.name()]=[self.EF_Dummy]
                    self.EFSequenceList+=[self.EF_Dummy.name()]
                elif (sig_id.count('MuonEF')):
                    if (TriggerFlags.CosmicSlice.testCosmic()==True):
                        mlog.info( "creating MuonEF instance based on item "+self.sig_id)
                    from TrigMuonEF.TrigMuonEFConfig import TrigMuonEFSegmentFinderConfig,TrigMuonEFTrackBuilderConfig
                    if (LocalFlags.useUniqueMuonEFInstance):
                        self.ef_TrigMuonEFSegmentFinderConfig = TrigMuonEFSegmentFinderConfig('Cosmic'+'TrigMuonEFSegmentFinderConfig')
                        self.ef_TrigMuonEFTrackBuilderConfig = TrigMuonEFTrackBuilderConfig('Cosmic'+'TrigMuonEFTrackBuilderConfig')
                    else:
                        self.ef_TrigMuonEFSegmentFinderConfig = TrigMuonEFSegmentFinderConfig('Cosmic'+sig_id+'TrigMuonEFSegmentFinderConfig')
                        self.ef_TrigMuonEFTrackBuilderConfig = TrigMuonEFTrackBuilderConfig('Cosmic'+sig_id+'TrigMuonEFTrackBuilderConfig')
                        
                    self.EFSequenceHash[self.ef_TrigMuonEFSegmentFinderConfig.name()+"_"+l1item+"_"+self.sig_id]=[self.ef_TrigMuonEFSegmentFinderConfig]
                    self.EFSequenceList+=[self.ef_TrigMuonEFSegmentFinderConfig.name()+"_"+l1item+"_"+self.sig_id]

                    self.EFSequenceHash[self.ef_TrigMuonEFTrackBuilderConfig.name()+"_"+l1item+"_"+self.sig_id]=[self.ef_TrigMuonEFTrackBuilderConfig]
                    self.EFSequenceList+=[self.ef_TrigMuonEFTrackBuilderConfig.name()+"_"+l1item+"_"+self.sig_id]

# TrigMoore is deprecated.
#                 else:
#                     if (sig_id.count('Moore')):
#                         from TrigMoore.TrigMooreConfig import TrigMooreConfig_MS
#                         if (TriggerFlags.CosmicSlice.useUniqueMooreInstance()):                        
#                             self.ef_TrigMooreConfig_MS = TrigMooreConfig_MS('Cosmic'+'TrigMooreConfig_MS')
#                         else:
#                             self.ef_TrigMooreConfig_MS = TrigMooreConfig_MS('Cosmic'+sig_id+'TrigMooreConfig_MS')
#                         #self.ef_TrigMooreConfig_MS.cosmic_moore=True
#                         self.EFSequenceHash[self.ef_TrigMooreConfig_MS.name()+"_"+l1item+"_"+self.sig_id]=[self.ef_TrigMooreConfig_MS]
#                         self.EFSequenceList+=[self.ef_TrigMooreConfig_MS.name()+"_"+l1item+"_"+self.sig_id]

                    
    def generateMenu(self,triggerPythonConfig):
        if eval(self.Flag):
            CosmicTemplate.generateMenu(self,triggerPythonConfig)
            #from AthenaCommon.AppMgr import ToolSvc
            #ToolSvc.L2MdtTool.FitMethod = 2
            #if (TriggerFlags.CosmicSlice.doM3Data()):
            #    ToolSvc.L2MdtTool.AddOneToCsmId = True
            #else:
            #    ToolSvc.L2MdtTool.AddOneToCsmId = False
            #ToolSvc.L2DataLoader.MdtRobMapperFile = 'RoiROBmap_P03.data' 
            #ToolSvc.L2DataLoader.MdtRobMapperFile = 'RoiROBmap.data' 
            #ToolSvc.MdtTimingTool.NominalT0 = 624
            #ToolSvc.MdtTimingTool.RoadCut = 0.5
            #ToolSvc.MdtTimingTool.SegmentChi2 = 0.2
            #ToolSvc.MdtTimingTool.HitMaxChi2 = 0.2
            #ToolSvc.L2RpcTool.AllowOnePairTrack = True

#            if (self.ownMufast):
#                muFast=self.TrigmuFastConfig
#                muFast.OnlineRun=LocalFlags.doMuonCalibrationStream
#
#                muFast.UseCalibration=True
#                muFast.MaskUncCMAch = False
#                muFast.CosmicRun   = True
#                muFast.Timing      = False
#                #if (self.sig_id.count('TGC')):
#                #    muFast.RemoveTgcTriggers=False
#                #else:
#                #    muFast.RemoveTgcTriggers=True
#
#
#            if (self.sig_id.count('MuIso')):
#                include("TrigT2CaloCommon/LArCellBuilding.py")
            
########################################  MEt slice  ###############################################################
class CosmicMEt(CosmicTemplate):
    def __init__(self, sig_id, l1item, l1threshold, counterId,StreamName):
        CosmicTemplate.__init__(self, sig_id, l1item, l1threshold, counterId,StreamName,groupNames=["Cosmic","Cosmic_MEt"])
        mlog = logging.getLogger( 'Cosmic.py:CosmicMEt' )

        self.__supported = ["AllTeMEt","AllTeMEt_EFOnly",
                            "MEt","MEt_EFOnly","MEt_EFOnly_Mufast","MEt_Mufast","MEt_EFOnly_Mufast_MuonEF","MEt_Mufast_MuonEF",
                            "MEt_V2","MEt_EFOnly_V2","MEt_EFOnly_Mufast_V2","MEt_Mufast_V2","MEt_EFOnly_Mufast_MuonEF_V2","MEt_Mufast_MuonEF_V2",
                            "AllTe_MEt_EFOnly_Mufast_MuonEF", "AllTe_MEt_Mufast_MuonEF","AllTe_MEt_EFOnly_MuonEF", "AllTe_MEt_MuonEF"]        
        if sig_id not in self.__supported:
            raise Exception("ERROR: "+sig_id+" not supprted"+str(self.__supported) )

        self.Flag='TriggerFlags.CosmicSlice.do%s()' % sig_id
        if self.configLVL2:
            if eval(self.Flag):
                if (sig_id.count("AllTe")):
                    self.L1InfoHypo.InvertSelection   =False
                    self.L1InfoHypo.TriggerTypeBitMask=0x80
                    self.L1InfoHypo.TriggerTypeBit    =0x80
                    self.AllTEDummyFakeROI.FakeROIEta=0.0
                    self.AllTEDummyFakeROI.FakeROIPhi=0.0
                    #self.AllTEDummyFakeROI.FakeROIEta=1.7
                    #self.AllTEDummyFakeROI.FakeROIPhi=1.5

                if (sig_id.count('Mufast')):
                    if (l1item not in MuFastInstances):
                        if (TriggerFlags.CosmicSlice.testCosmic()==True):
                            mlog.info( "creating mufast instance based on item "+str(l1item))
                        from TrigMuonHypo.TrigMuonHypoConfig import MufastHypoConfig
                        from TrigmuFast.TrigmuFastConfig import TrigmuFastConfig
                        self.TrigmuFastConfig = TrigmuFastConfig("Cosmic"+sig_id+"TrigmuFastConfig_900GeV")
                        self.MufastHypoConfig = MufastHypoConfig("Cosmic"+sig_id+"MufastHypoConfig_900Gev",'2GeV')
                        #self.L2SequenceHash[self.TrigmuFastConfig.name()]=[self.TrigmuFastConfig,self.MufastHypoConfig]
                        self.L2SequenceHash[self.TrigmuFastConfig.name()]=[self.TrigmuFastConfig]
                        self.L2SequenceList+=[self.TrigmuFastConfig.name()]
                        MuFastInstances[l1item]=[self.TrigmuFastConfig,self.MufastHypoConfig]
                        self.ownMufast=True
                    else:
                        if (TriggerFlags.CosmicSlice.testCosmic()==True):
                            mlog.info("mufast instance based on item "+str(l1item)+" already existing")
                        self.L2SequenceHash[l1item+MuFastInstances[l1item][0].name()+self.sig_id]=MuFastInstances[l1item]
                        self.L2SequenceList+=[l1item+MuFastInstances[l1item][0].name()+self.sig_id]
                        self.ownMufast=False

                if (sig_id.count('EFOnly')):
                    from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
                    self.L2_Dummy = DummyFEX("Cosmic_L2_%s" % sig_id)
                    self.L2SequenceHash[self.L2_Dummy.name()]=[self.L2_Dummy]
                    self.L2SequenceList+=[self.L2_Dummy.name()]
                else:
                    from TrigL2MissingET.TrigL2MissingETConfig import L2MissingET_Fex
                    from TrigMissingETHypo.TrigMissingETHypoConfig import EFMissingETHypoBase

                    self.l2Fex = L2MissingET_Fex('Cosmic'+sig_id+'L2METfex')
                    from AthenaCommon.SystemOfUnits import GeV
                    self.l2Hypo = EFMissingETHypoBase('Cosmic'+sig_id+'L2METhypo')
                    self.l2Hypo.MissingETCut=1*GeV
                    self.l2Hypo.SumETCut=1*GeV
                    self.l2Hypo.CutType=-2.0
                    self.L2SequenceHash[self.l2Fex.name()]=[self.l2Fex, self.l2Hypo]
                    self.L2SequenceList+=[self.l2Fex.name()]

                    include("TrigT2CaloCommon/LArCellBuilding.py")

        if self.configEF:
            # diego would like to run this on every event coming through from l2
            if eval(self.Flag):

                from TrigEFMissingET.TrigEFMissingETConfig import EFMissingET_Fex
                from TrigMissingETHypo.TrigMissingETHypoConfig import EFMissingETHypoBase
                self.efcaloMET = EFMissingET_Fex('Cosmic'+sig_id+'EFMETfex')
                
                self.EFSequenceHash[self.efcaloMET.name()]=[]
                self.EFSequenceList=[]
                
                if (sig_id.count('MuonEF')):
                    if (TriggerFlags.CosmicSlice.testCosmic()==True):
                        mlog.info( "creating MuonEF instance based on item "+self.sig_id)
                    from TrigMuonEF.TrigMuonEFConfig import TrigMuonEFSegmentFinderConfig,TrigMuonEFTrackBuilderConfig
                    if (LocalFlags.useUniqueMuonEFInstance):
                        self.ef_TrigMuonEFSegmentFinderConfig = TrigMuonEFSegmentFinderConfig('Cosmic'+'TrigMuonEFSegmentFinderConfig')
                        self.ef_TrigMuonEFTrackBuilderConfig = TrigMuonEFTrackBuilderConfig('Cosmic'+'TrigMuonEFTrackBuilderConfig')
                    else:
                        self.ef_TrigMuonEFSegmentFinderConfig = TrigMuonEFSegmentFinderConfig('Cosmic'+sig_id+'TrigMuonEFSegmentFinderConfig')
                        self.ef_TrigMuonEFTrackBuilderConfig = TrigMuonEFTrackBuilderConfig('Cosmic'+sig_id+'TrigMuonEFTrackBuilderConfig')
                        
                    self.EFSequenceHash[self.ef_TrigMuonEFSegmentFinderConfig.name()+"_"+l1item+"_"+self.sig_id]=[self.ef_TrigMuonEFSegmentFinderConfig]
                    self.EFSequenceList+=[self.ef_TrigMuonEFSegmentFinderConfig.name()+"_"+l1item+"_"+self.sig_id]
                    
                    self.EFSequenceHash[self.ef_TrigMuonEFTrackBuilderConfig.name()+"_"+l1item+"_"+self.sig_id]=[self.ef_TrigMuonEFTrackBuilderConfig]
                    self.EFSequenceList+=[self.ef_TrigMuonEFTrackBuilderConfig.name()+"_"+l1item+"_"+self.sig_id]
                
                
                
                from AthenaCommon.SystemOfUnits import GeV
                self.efcaloMEThypo = EFMissingETHypoBase('Cosmic'+sig_id+'EFMEThypo')
                self.efcaloMEThypo.MissingETCut=15*GeV
                self.efcaloMEThypo.SumETCut=150*GeV
                self.efcaloMEThypo.CutType=-1.0 # AND of both thresholds

                self.EFSequenceHash[self.efcaloMET.name()]+=[self.efcaloMET, self.efcaloMEThypo ]
                self.EFSequenceList+=[self.efcaloMET.name()]
                
##                if (sig_id.count("AllTe")):
##                    from TrigEFIDCosmic.TrigEFIDCosmicConf import L1InfoHypo
##                    self.RejectAll = L1InfoHypo("RejectAll"+sig_id) 
##                    self.RejectAll.TriggerTypeBitMask=0x0
##                    self.RejectAll.TriggerTypeBit    =0x0
##                    self.RejectAll.InvertSelection   =True
##                    self.EFSequenceHash[self.RejectAll.name()]=[self.Reject_Dummy,self.RejectAll]
##                    self.EFSequenceList+=[self.RejectAll.name()]
##                
                

    def generateMenu(self,triggerPythonConfig):
        if (eval(self.Flag)):
            CosmicTemplate.generateMenu(self,triggerPythonConfig)
            if (self.sig_id.count('Mufast') and self.ownMufast):
                            muFast=self.TrigmuFastConfig
                            muFast.OnlineRun=LocalFlags.doMuonCalibrationStream

                            muFast.UseCalibration=True
                            muFast.MaskUncCMAch = False
                            muFast.CosmicRun   = True
                            muFast.Timing      = False
                            #if (self.sig_id.count('TGC')):
                            #    muFast.RemoveTgcTriggers=False
                            #else:
                            #    muFast.RemoveTgcTriggers=True

#            include("TrigT2CaloCommon/LArCellBuilding.py")

########################################  MBiasCalo slice  ###############################################################
class CosmicMBiasCalo(CosmicTemplate):
    def __init__(self, sig_id, l1item, l1threshold, counterId,StreamName):
        CosmicTemplate.__init__(self, sig_id, l1item, l1threshold, counterId,StreamName,groupNames=["Cosmic","Cosmic_MBiasCalo"])
        mlog = logging.getLogger( 'Cosmic.py:CosmicMBiasCalo' )

        self.__supported = ["AllTeMBiasCalo_EndCapA","AllTeMBiasCalo_EndCapC","MBiasCalo_EndCapA_V2","MBiasCalo_EndCapC_V2"]  
        if sig_id not in self.__supported:
            raise Exception("ERROR: "+sig_id+" not suported"+str(self.__supported) )

        self.Flag='TriggerFlags.CosmicSlice.do%s()' % sig_id

        if self.configLVL2:
            if eval(self.Flag):
                if (sig_id.count("AllTe")):
                    self.L1InfoHypo.TriggerTypeBitMask=0xff
                    self.L1InfoHypo.TriggerTypeBit    =0xc0  #MBTS

                    if (sig_id.count("EndCapA")):
                        self.L1InfoHypo.L1ItemNames    +=MBTSItemList_SideA
                        
                    if (sig_id.count("EndCapC")):
                        self.L1InfoHypo.L1ItemNames    +=MBTSItemList_SideC

                    self.AllTEDummyFakeROI.FakeROIPhi=-1.5
                    self.AllTEDummyFakeROI.CheckForInputTE=False
                    if sig_id.count('EndCapA'):
                        self.AllTEDummyFakeROI.FakeROIEta=1.6
                    else:
                        self.AllTEDummyFakeROI.FakeROIEta=-1.6
                
                from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_eGamma
                self.l2_T2CaloEgamma_eGamma = T2CaloEgamma_eGamma("Cosmic"+sig_id+"T2CaloEgama_eGamma")
                self.l2_T2CaloEgamma_eGamma.EtaWidth = 0.2
                self.l2_T2CaloEgamma_eGamma.PhiWidth = 1.6

                from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_g7
                from TrigEgammaHypo.TrigL2PhotonFexConfig import L2PhotonFex_1
                from TrigEgammaHypo.TrigL2PhotonHypoConfig import L2PhotonHypo_g10_loose
                self.l2_L2PhotonHypo_g10_loose = L2PhotonHypo_g10_loose("Cosmic"+sig_id+"L2PhotonHypo_g10_loose")
                self.l2_L2PhotonFex_1 = L2PhotonFex_1("Cosmic"+sig_id+"L2PhotonFex_1")
                self.l2_L2CaloHypo_g7 = L2CaloHypo_g7("Cosmic"+sig_id+"L2CaloHypo_g7")

                self.l2_L2PhotonHypo_g10_loose.ETthr = [-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV]
                self.l2_L2PhotonHypo_g10_loose.AcceptAll=False
                self.l2_L2CaloHypo_g7.ETthr = [-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV,-7.*GeV]
                self.l2_L2CaloHypo_g7.AcceptAll=False
                
                self.L2SequenceHash[self.l2_T2CaloEgamma_eGamma.name()]=[self.l2_T2CaloEgamma_eGamma,self.l2_L2CaloHypo_g7]
                self.L2SequenceList+=[self.l2_T2CaloEgamma_eGamma.name()]
                self.L2SequenceHash["Cosmic"+sig_id+"L2Photon"]=[self.l2_L2PhotonFex_1,self.l2_L2PhotonHypo_g10_loose]
                self.L2SequenceList+=["Cosmic"+sig_id+"L2Photon"]


                include("TrigT2CaloCommon/LArCellBuilding.py")

        if self.configEF:
            if eval(self.Flag):
                from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
                self.EF_Dummy = DummyFEX("Cosmic_EF_%s" % sig_id)
                self.EFSequenceHash[self.EF_Dummy.name()]=[self.EF_Dummy]
                self.EFSequenceList+=[self.EF_Dummy.name()]

    def generateMenu(self,triggerPythonConfig):
        CosmicTemplate.generateMenu(self,triggerPythonConfig)
#        if (eval(self.Flag)):
#            include("TrigT2CaloCommon/LArCellBuilding.py")
            
########################################  minBias slice  ###############################################################

class CosmicMinBias(CosmicTemplate):
    def __init__(self, sig_id, l1item, l1threshold, counterId,StreamName):
        CosmicTemplate.__init__(self, sig_id, l1item, l1threshold, counterId,StreamName,groupNames=["Cosmic","Cosmic_MinBias"])
        mlog = logging.getLogger( 'Cosmic.py:MinBias' )

        self.__supported = ["AllTeMBSpacePoints","AllTeMBSpacePoints_V2","AllTeMBStandaloneSpacePoints"]
        if sig_id not in self.__supported:
            raise Exception("ERROR: "+sig_id+" not supprted"+str(self.__supported) )

        self.Flag='TriggerFlags.CosmicSlice.do%s()' % sig_id
        if self.configLVL2:
            if eval(self.Flag):
                if (TriggerFlags.CosmicSlice.testCosmic()==True):
                    mlog.info( "Now initializing L2 %s" % sig_id)

                if (sig_id.count("AllTe")):
                    self.L1InfoHypo.InvertSelection   =False
                    self.L1InfoHypo.TriggerTypeBitMask=0x80
                    self.L1InfoHypo.TriggerTypeBit    =0x80
                    self.AllTEDummyFakeROI.FakeROIEta=0.0
                    self.AllTEDummyFakeROI.FakeROIPhi=0.0
                    self.AllTEDummyFakeROI.CheckForInputTE=True
                    #self.AllTEDummyFakeROI.FakeROIEta=1.7
                    #self.AllTEDummyFakeROI.FakeROIPhi=1.5


                from TrigT2MinBias.TrigT2MinBiasConfig import MbSpFex_1, MbSpHypo_1

                self.l2_mbSpFex_DM = MbSpFex_1("MbSpFex_"+sig_id)
                self.l2_mbSpFex_DM.PixelModuleThreshold = 50
                self.l2_mbSpFex_DM.SCTModuleThreshold = 100
                self.l2_mbSpFex_DM.MaxNModIdentifier = 100


                self.l2_minbiashypo = MbSpHypo_1("MbSpHypo_"+sig_id)
                
                self.l2_minbiashypo.AcceptAll = False
                self.l2_minbiashypo.PixelCLToTCut = 20.
                self.l2_minbiashypo.TriggerTypeAND = True
                self.l2_minbiashypo.TotalPixelClus = -1.
                self.l2_minbiashypo.PixelClusRatioA = -1.
                self.l2_minbiashypo.PixelClusRatioB = -1.
                self.l2_minbiashypo.PixelClusEndcapC = -1.
                self.l2_minbiashypo.PixelClusBarrel = -1.
                self.l2_minbiashypo.PixelClusEndcapA = -1.
                self.l2_minbiashypo.PixelClusEndcapC_max = -1.
                self.l2_minbiashypo.PixelClusBarrel_max = -1.
                self.l2_minbiashypo.PixelClusEndcapA_max = -1.
                self.l2_minbiashypo.TotalSctSp = -1.
                self.l2_minbiashypo.SctSpEndcapC = -1.
                self.l2_minbiashypo.SctSpBarrel = -1.
                self.l2_minbiashypo.SctSpEndcapA = -1.
                self.l2_minbiashypo.SctSpEndcapC_max = -1.
                self.l2_minbiashypo.SctSpBarrel_max = -1.
                self.l2_minbiashypo.SctSpEndcapA_max = -1.
                
                
                self.L2SequenceHash[self.l2_mbSpFex_DM.name()]=[self.l2_mbSpFex_DM, self.l2_minbiashypo]
                self.L2SequenceList+=[self.l2_mbSpFex_DM.name()]

        if self.configEF:
            if eval(self.Flag):
                if (TriggerFlags.CosmicSlice.testCosmic()==True):
                    mlog.info( "Now initializing EF %s" % sig_id)
                from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
                self.EF_Dummy = DummyFEX("EF_%s" % sig_id)
                self.EFSequenceHash[self.EF_Dummy.name()]=[self.EF_Dummy]
                self.EFSequenceList+=[self.EF_Dummy.name()]

    def generateMenu(self,triggerPythonConfig):
        if eval(self.Flag):
            CosmicTemplate.generateMenu(self,triggerPythonConfig)
            from AthenaCommon.AppMgr import ToolSvc

########################################  ROI filter slice  ###############################################################

class CosmicROIFilter(CosmicTemplate):
    def __init__(self, sig_id, l1item, l1threshold, counterId,StreamName):
        CosmicTemplate.__init__(self, sig_id, l1item, l1threshold, counterId,StreamName,groupNames=["Cosmic","Cosmic_ROI"])
        mlog = logging.getLogger( 'Cosmic.py:CosmicROIFilter' )

        self.__supported = ["CosmicDownwardMuonRPCROI","CosmicDownwardMuonTGCROI"]
        if sig_id not in self.__supported:
            raise Exception("ERROR: "+sig_id+" not supprted"+str(self.__supported) )

        self.Flag='TriggerFlags.CosmicSlice.do%s()' % sig_id
        if self.configLVL2:
            if eval(self.Flag):
                if (TriggerFlags.CosmicSlice.testCosmic()==True):
                    mlog.info( "Now initializing L2 %s" % sig_id)
                from TrigEFIDCosmic.L1CaloROIAlgo_Config import L1CaloROIAlgo_Instance
                self.ROIAlgo=L1CaloROIAlgo_Instance("CosmicROIFilter_"+sig_id)
                self.ROIAlgo.MinimumDeltaEta=-100.0
                self.ROIAlgo.MinimumDeltaPhi=-100.0
                self.ROIAlgo.MinimumDeltaR  =0.5

                self.ROIAlgo.NROI  = 1
                self.ROIAlgo.MinimumEta=-1.1
                self.ROIAlgo.MaximumEta= 1.1

                # 225 to 292.5 degrees
                self.ROIAlgo.MinimumPhi=3.925
                self.ROIAlgo.MaximumPhi=5.105
                
                #self.ROIAlgo.OutputLevel=2

                from TrigEFIDCosmic.TrigEFIDCosmicConf import L1InfoHypo
                self.L1InfoHypo_ROIFilter = L1InfoHypo("L1InfoHypo"+sig_id) # here we created an instance
                self.L1InfoHypo_ROIFilter.TriggerTypeBitMask=0x80
                self.L1InfoHypo_ROIFilter.TriggerTypeBit    =0x80
                self.L1InfoHypo_ROIFilter.L1TriggerBitMask=0x0
                self.L1InfoHypo_ROIFilter.L1TriggerBit    =0x0
                self.L1InfoHypo_ROIFilter.L1ItemNames    += self.l1threshold

                self.L1InfoHypoRNDMReject = L1InfoHypo("L1InfoHypoRNDMReject"+sig_id) # here we created an instance
                self.L1InfoHypoRNDMReject.TriggerTypeBitMask=0xff # ignore RNDM stream
                self.L1InfoHypoRNDMReject.TriggerTypeBit    =0x81
                self.L1InfoHypoRNDMReject.InvertSelection   =True

                self.L2SequenceHash[self.ROIAlgo.name()]=[self.ROIAlgo,self.L1InfoHypo_ROIFilter,self.L1InfoHypoRNDMReject]
                self.L2SequenceList+=[self.ROIAlgo.name()]

        if self.configEF:
            if eval(self.Flag):
                if (TriggerFlags.CosmicSlice.testCosmic()==True):
                    mlog.info( "Now initializing EF %s" % sig_id)
                from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
                self.EF_Dummy = DummyFEX("EF_%s" % sig_id)
                self.EFSequenceHash[self.EF_Dummy.name()]=[self.EF_Dummy]
                self.EFSequenceList+=[self.EF_Dummy.name()]

    def generateMenu(self,triggerPythonConfig):
        if eval(self.Flag):
            CosmicTemplate.generateMenu(self,triggerPythonConfig)
            from AthenaCommon.AppMgr import ToolSvc

########################################  L1Calo filter slice  ###############################################################

class CosmicL1CaloFilter(CosmicTemplate):
    def __init__(self, sig_id, l1item, l1threshold, counterId,StreamName):
        CosmicTemplate.__init__(self, sig_id, l1item, l1threshold, counterId,StreamName,groupNames=["Cosmic","Cosmic_L1Calo"])
        mlog = logging.getLogger( 'Cosmic.py:L1CaloFilter' )

        self.__supported = ["L1CaloFilter","L1CaloEMFilter","L1CaloTauFilter","L1CaloJetFilter",
                            "L1CaloFilter_V2","L1CaloEMFilter_V2","L1CaloTauFilter_V2","L1CaloJetFilter_V2"]
        if sig_id not in self.__supported:
            raise Exception("ERROR: "+sig_id+" not supprted"+str(self.__supported) )

        self.Flag='TriggerFlags.CosmicSlice.do%s()' % sig_id
        if self.configLVL2:
            if eval(self.Flag):
                if (TriggerFlags.CosmicSlice.testCosmic()==True):
                    mlog.info( "Now initializing L2 %s" % sig_id)
                from TrigEFIDCosmic.L1CaloROIAlgo_Config import L1CaloROIAlgo_Instance
                self.L1CaloROIAlgo=L1CaloROIAlgo_Instance("L1CaloROIAlgo_"+sig_id)
                self.L1CaloROIAlgo.MinimumDeltaEta=-100.0
                self.L1CaloROIAlgo.MinimumDeltaPhi=-100.0
                self.L1CaloROIAlgo.MinimumDeltaR  =0.5
                #self.L1CaloROIAlgo.OutputLevel=2

                #from TrigEFIDCosmic.TrigEFIDCosmicConf import L1InfoHypo
                #self.L1InfoHypo_L1Calo = L1InfoHypo("L1InfoHypo"+sig_id) # here we created an instance
                #self.L1InfoHypo_L1Calo.TriggerTypeBitMask=0x84
                #self.L1InfoHypo_L1Calo.TriggerTypeBit    =0x84

                #self.L1InfoHypo_L1Calo.L1TriggerBitMask=0x0
                #self.L1InfoHypo_L1Calo.L1TriggerBit    =0x0
                #self.L1InfoHypo_L1Calo.L1ItemNames    += self.l1threshold

                #self.L2SequenceHash[self.L1CaloROIAlgo.name()]=[self.L1CaloROIAlgo,self.L1InfoHypo_L1Calo]
                self.L2SequenceHash[self.L1CaloROIAlgo.name()]=[self.L1CaloROIAlgo]
                self.L2SequenceList+=[self.L1CaloROIAlgo.name()]

        if self.configEF:
            if eval(self.Flag):
                if (TriggerFlags.CosmicSlice.testCosmic()==True):
                    mlog.info("Now initializing EF %s" % sig_id)
                from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
                self.EF_Dummy = DummyFEX("EF_%s" % sig_id)
                self.EFSequenceHash[self.EF_Dummy.name()]=[self.EF_Dummy]
                self.EFSequenceList+=[self.EF_Dummy.name()]

    def generateMenu(self,triggerPythonConfig):
        if eval(self.Flag):
            CosmicTemplate.generateMenu(self,triggerPythonConfig)
            from AthenaCommon.AppMgr import ToolSvc

########################################  HLTBeamLineFitterTest slice  ###############################################################

class HLTBeamLineFitterTest(CosmicTemplate):
    def __init__(self, sig_id, l1item, l1threshold, counterId,StreamName):
        CosmicTemplate.__init__(self, sig_id, l1item, l1threshold, counterId,StreamName,groupNames=["Cosmic","Cosmic_Test"])
        mlog = logging.getLogger( 'Cosmic.py:HLTBeamLineFittertest' )

        self.__supported = ["HLTBeamLineFitterTestFake","HLTBeamLineFitterTest"]
        if sig_id not in self.__supported:
            raise Exception("ERROR: "+sig_id+" not supprted"+str(self.__supported) )

        self.Flag='TriggerFlags.CosmicSlice.do%s()' % sig_id
        if self.configLVL2:
            if eval(self.Flag):
                if (TriggerFlags.CosmicSlice.testCosmic()==True):
                    mlog.info( "Now initializing L2 %s" % sig_id)

                if (sig_id.count("Fake")):
                    from TrigEFIDCosmic.HLTBeamLineFitterFex_Config import Cosmic__HLTBeamLineFitterFex_Instance
                    self.HLTBeamLineFitter=Cosmic__HLTBeamLineFitterFex_Instance("HLTBeamLineFitterFex_"+sig_id)
                    #self.HLTBeamLineFitter.???=-1.0

                    from TrigEFIDCosmic.FakeHLTTracks_Config import FakeHLTTracks_Instance
                    self.FakeHLTTracks=FakeHLTTracks_Instance("FakeHLTTracks_"+sig_id)
                    self.FakeHLTTracks.x0=-1.0
                    self.FakeHLTTracks.y0=-1.0
                    self.FakeHLTTracks.z0=0.5
                    self.FakeHLTTracks.dxdz= 0.001
                    self.FakeHLTTracks.dydz=-0.01
                    self.L2SequenceHash[self.FakeHLTTracks.name()]=[self.FakeHLTTracks]
                    self.L2SequenceList+=[self.FakeHLTTracks.name()]
                    self.L2SequenceHash[self.HLTBeamLineFitter.name()]=[self.HLTBeamLineFitter]
                    self.L2SequenceList+=[self.HLTBeamLineFitter.name()]
                else:
                    from TrigEFIDCosmic.HLTBeamLineFitter_Config import Cosmic__HLTBeamLineFitter_Instance
                    self.HLTBeamLineFitter=Cosmic__HLTBeamLineFitter_Instance("HLTBeamLineFitter_"+sig_id)
                    #self.HLTBeamLineFitter.???=-1.0
                    self.L2SequenceHash[self.HLTBeamLineFitter.name()]=[self.HLTBeamLineFitter]
                    self.L2SequenceList+=[self.HLTBeamLineFitter.name()]


        if self.configEF:
            if eval(self.Flag):
                if (TriggerFlags.CosmicSlice.testCosmic()==True):
                    mlog.info( "Now initializing EF %s" % sig_id)
                from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
                self.EF_Dummy = DummyFEX("EF_%s" % sig_id)
                self.EFSequenceHash[self.EF_Dummy.name()]=[self.EF_Dummy]
                self.EFSequenceList+=[self.EF_Dummy.name()]

    def generateMenu(self,triggerPythonConfig):
        if eval(self.Flag):
            CosmicTemplate.generateMenu(self,triggerPythonConfig)
            from AthenaCommon.AppMgr import ToolSvc

########################################  slice  ###############################################################


#             name          item       thr     counter  stream
Cosmics = [
    CosmicStandalone     ('MU1Dummy'                         ,  'L1_MU1',            ['MU1'],            '3000' ,''),
    #CosmicStandalone     ('MU1Dummy'                         ,  'L1_MU1',            ['MU1'],            '-1' ,''),
    CosmicStandalone     ('MU2Dummy'                         ,  'L1_MU2',            ['MU2'],            '-1' ,''),
    CosmicStandalone     ('MU3Dummy'                         ,  'L1_MU3',            ['MU3'],            '-1' ,''),
    CosmicStandalone     ('MU11Dummy'                         ,  'L1_MU11',           ['MU11'],            '-1' ,''),
    CosmicEmpty          ('MU3Empty'                      ,  'L1_MU3' ,['MU3'] ,'-1' ,''),
    CosmicEmpty          ('MU11Empty'                     ,  'L1_MU11',['MU11'],'-1' ,''),
    CosmicStandalone     ('Dummy'                         ,  TriggerFlags.CosmicSlice.Signature() or defaultSignatures['Signature'] ,            TriggerFlags.CosmicSlice.Threshold() or defaultThresholds['Threshold'],            '-1' ,''),
    CosmicEmpty          ('Empty'                         ,  TriggerFlags.CosmicSlice.Signature() or defaultSignatures['Signature'],            TriggerFlags.CosmicSlice.Threshold() or defaultThresholds['Threshold'],            '-1' ,''),
    CosmicStandalone     ('StandaloneDummy'               ,  TriggerFlags.CosmicSlice.StandaloneSignature() or defaultSignatures['StandaloneSignature'],  TriggerFlags.CosmicSlice.StandaloneThreshold() or defaultThresholds['StandaloneThreshold'],  '-1' ,''),
    CosmicEmpty          ('StandaloneEmpty'               ,  TriggerFlags.CosmicSlice.StandaloneSignature() or defaultSignatures['StandaloneSignature'],  TriggerFlags.CosmicSlice.StandaloneThreshold() or defaultThresholds['StandaloneThreshold'],  '-1' ,''),
    CosmicStandalone     ('CaloDummy'                     ,  TriggerFlags.CosmicSlice.CaloSignature() or defaultSignatures['CaloSignature'],        TriggerFlags.CosmicSlice.CaloThreshold() or defaultThresholds['CaloThreshold'],        '-1' ,''),
    CosmicEmpty          ('CaloEmpty'                     ,  TriggerFlags.CosmicSlice.CaloSignature() or defaultSignatures['CaloSignature'],        TriggerFlags.CosmicSlice.CaloThreshold() or defaultThresholds['CaloThreshold'],        '-1' ,''),
    CosmicStandalone     ('RPCDummy'                      ,  TriggerFlags.CosmicSlice.RPCSignature() or defaultSignatures['RPCSignature'],         TriggerFlags.CosmicSlice.RPCThreshold() or defaultThresholds['RPCThreshold'],         '-1' ,''),
    CosmicEmpty          ('RPCEmpty'                      ,  TriggerFlags.CosmicSlice.RPCSignature() or defaultSignatures['RPCSignature'],         TriggerFlags.CosmicSlice.RPCThreshold() or defaultThresholds['RPCThreshold'],         '-1' ,''),
    CosmicStandalone     ('TGCDummy'                      ,  TriggerFlags.CosmicSlice.TGCSignature() or defaultSignatures['TGCSignature'],         TriggerFlags.CosmicSlice.TGCThreshold() or defaultThresholds['TGCThreshold'],         '-1' ,''),
    CosmicEmpty          ('TGCEmpty'                      ,  TriggerFlags.CosmicSlice.TGCSignature() or defaultSignatures['TGCSignature'],         TriggerFlags.CosmicSlice.TGCThreshold() or defaultThresholds['TGCThreshold'],         '-1' ,''),
    
    CosmicIDSCAN         ('AllTeIDSCAN'                        ,  '',         TriggerFlags.CosmicSlice.TrackingThreshold() or defaultThresholds['TrackingThreshold'],         '-1' ,'s'),
    CosmicSiTrack        ('AllTeSiTrack'                       ,  '',         TriggerFlags.CosmicSlice.TrackingThreshold() or defaultThresholds['TrackingThreshold'],         '-1' ,'s'),
    
    CosmicIDSCAN         ('AllTeIDSCANCombined'                ,  '',         TriggerFlags.CosmicSlice.TrackingThreshold() or defaultThresholds['TrackingThreshold'],         '-1' ,''),
    CosmicSiTrack        ('AllTeSiTrackCombined'               ,  '',         TriggerFlags.CosmicSlice.TrackingThreshold() or defaultThresholds['TrackingThreshold'],         '-1' ,''),
    
    CosmicIDSCAN         ('CosmicsAllTeIDSCAN'                        ,  '',         '',         '-1' ,'CosmicMuons'),
    CosmicSiTrack        ('CosmicsAllTeSiTrack'                       ,  '',         '',         '-1' ,'CosmicMuons'),
                           
    CosmicIDSCAN         ('CosmicsAllTeIDSCAN_TrkHypo'                        ,  '',         '',         '-1' ,'IDSelected'),
    CosmicSiTrack        ('CosmicsAllTeSiTrack_TrkHypo'                       ,  '',         '',         '-1' ,'IDSelected'),
    CosmicIDSCAN         ('CosmicsAllTeIDSCAN_AllPhysics_TrkHypo'                        ,  '',         '',         '-1' ,'IDSelected'),
    CosmicSiTrack        ('CosmicsAllTeSiTrack_AllPhysics_TrkHypo'                       ,  '',         '',         '-1' ,'IDSelected'),
    
    CosmicIDSCAN         ('CosmicsAllTeIDSCAN_AllPhysics_4Hits_TrkHypo'                        ,  '',         '',         '-1' ,'IDSelected'),
    CosmicSiTrack        ('CosmicsAllTeSiTrack_AllPhysics_4Hits_TrkHypo'                       ,  '',         '',         '-1' ,'IDSelected'),
    
    CosmicIDSCAN         ('CosmicsAllTeIDSCANCombined'                ,  '',         '',         '-1' ,'IDSelected'),
    CosmicSiTrack        ('CosmicsAllTeSiTrackCombined'               ,  '',         '',         '-1' ,'IDSelected'),
    
    CosmicEFID           ('CosmicsAllTeEFID'                          ,  '',         '',         '-1' ,'IDSelected',L2prescale=1,EFprescale=1),
    
    #CosmicTRTxK          ('AllTeTRTxK'                         ,  TriggerFlags.CosmicSlice.TrackingSignature() or defaultSignatures['TrackingSignature'],         TriggerFlags.CosmicSlice.TrackingThreshold() or defaultThresholds['TrackingThreshold'],         '-1' ,''),
    #CosmicTRTxK          ('AllTeTRTxK_TRTTrkHypo'              ,  TriggerFlags.CosmicSlice.TrackingSignature() or defaultSignatures['TrackingSignature'],         TriggerFlags.CosmicSlice.TrackingThreshold() or defaultThresholds['TrackingThreshold'],         '-1' ,''),
    CosmicTRTxK          ('AllTeTRTxK'                 ,  TriggerFlags.CosmicSlice.TrackingSignature() or defaultSignatures['TrackingSignature'],         TriggerFlags.CosmicSlice.TrackingThreshold() or defaultThresholds['TrackingThreshold'],         '-1' ,''),
    CosmicTRTxK          ('AllTeTRTxK_TRTTrkHypo'      ,  TriggerFlags.CosmicSlice.TrackingSignature() or defaultSignatures['TrackingSignature'],         TriggerFlags.CosmicSlice.TrackingThreshold() or defaultThresholds['TrackingThreshold'],         '-1' ,''),
    CosmicTRTxK          ('AllTeTRTxK_TRTTrkHypo_Pixel',  TriggerFlags.CosmicSlice.TrackingSignature() or defaultSignatures['TrackingSignature'],         TriggerFlags.CosmicSlice.TrackingThreshold() or defaultThresholds['TrackingThreshold'],         '-1' ,''),
    
    CosmicTRTxK          ('CosmicsAllTeTRTxK'                 ,  '',         '',         '-1' ,'CosmicMuons'),
    CosmicTRTxK          ('CosmicsAllTeTRTxK_TRTTrkHypo'      ,  '',         '',         '-1' ,'CosmicMuons'),
    CosmicTRTxK          ('CosmicsAllTeTRTxK_TRTTrkHypo_Pixel',  '',         '',         '-1' ,'CosmicMuons'),
    CosmicTRTxK          ('CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics'  ,  '',         '',         '-1' ,'CosmicMuons'),
    CosmicTRTxK          ('CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_NoField'  ,  '',         '',         '-1' ,'CosmicMuons'),
    CosmicTRTxK          ('CosmicsAllTeTRTxK_TRTTrkHypo_AllPhysics_PEB'  ,  '',         '',         '-1' ,'CosmicMuons'),
    
    CosmicTileRODMu      ('AllTeTileRODMu'                ,                                       '',                                             'J50',         '-1' ,''),
    CosmicTileRODMu      ('TileRODMu'                     ,  TriggerFlags.CosmicSlice.RPCSignature() or defaultSignatures['RPCSignature'],         TriggerFlags.CosmicSlice.RPCThreshold() or defaultThresholds['RPCThreshold'],         '-1' ,''),
    CosmicTileRODMu      ('CaloTileRODMu'                 ,  TriggerFlags.CosmicSlice.CaloSignature() or defaultSignatures['CaloSignature'],            TriggerFlags.CosmicSlice.CaloThreshold() or defaultThresholds['CaloThreshold'],        '-1' ,'L1Calo'),
    CosmicTileRODMu      ('CaloTileRODMu_V2'                 ,  TriggerFlags.CosmicSlice.CaloSignature_V2() or defaultSignatures['CaloSignature_V2'],            TriggerFlags.CosmicSlice.CaloThreshold_V2() or defaultThresholds['CaloThreshold_V2'],        '-1' ,'L1Calo'),
    CosmicTileRODMu      ('StandaloneTileRODMu'           ,  TriggerFlags.CosmicSlice.StandaloneSignature() or defaultSignatures['StandaloneSignature'],  TriggerFlags.CosmicSlice.StandaloneThreshold() or defaultThresholds['StandaloneThreshold'],  '-1' ,''),
    #
    CosmicTileLookForMu  ('AllTeTileLookForMu'            ,  '',         ['J50'],         '-1' ,''),
    CosmicTileLookForMu  ('TileLookForMu'                 ,  TriggerFlags.CosmicSlice.RPCSignature() or defaultSignatures['RPCSignature'],         TriggerFlags.CosmicSlice.RPCThreshold() or defaultThresholds['RPCThreshold'],         '-1' ,''),
    CosmicTileLookForMu  ('CaloTileLookForMu'             ,  TriggerFlags.CosmicSlice.CaloSignature() or defaultSignatures['CaloSignature'],            TriggerFlags.CosmicSlice.CaloThreshold() or defaultThresholds['CaloThreshold'],        '-1' ,'L1Calo'),
    CosmicTileLookForMu  ('CaloTileLookForMu_V2'             ,  TriggerFlags.CosmicSlice.CaloSignature_V2() or defaultSignatures['CaloSignature_V2'],            TriggerFlags.CosmicSlice.CaloThreshold_V2() or defaultThresholds['CaloThreshold_V2'],        '-1' ,'L1Calo'),
    CosmicTileLookForMu  ('StandaloneTileLookForMu'       ,  TriggerFlags.CosmicSlice.StandaloneSignature() or defaultSignatures['StandaloneSignature'],  TriggerFlags.CosmicSlice.StandaloneThreshold() or defaultThresholds['StandaloneThreshold'],  '-1' ,''),
    #
    CosmicEgamma         ('AllTeEgamma'                   ,  '',            ['J50'],            '-1' ,''),
    CosmicEgamma         ('Egamma'                        ,  TriggerFlags.CosmicSlice.CaloSignature() or defaultSignatures['CaloSignature'],            TriggerFlags.CosmicSlice.CaloThreshold() or defaultThresholds['CaloThreshold'],            '-1' ,'L1Calo'),
    CosmicEgamma         ('Egamma_V2'                        ,  TriggerFlags.CosmicSlice.CaloSignature_V2() or defaultSignatures['CaloSignature_V2'],            TriggerFlags.CosmicSlice.CaloThreshold_V2() or defaultThresholds['CaloThreshold_V2'],            '-1' ,'L1Calo'),
    CosmicEgamma         ('Egamma_noHad'                  ,  TriggerFlags.CosmicSlice.CaloSignature() or defaultSignatures['CaloSignature'],            TriggerFlags.CosmicSlice.CaloThreshold() or defaultThresholds['CaloThreshold'],            '-1' ,''),
    #
    CosmicJet            ('AllTeJet'                           ,  '',            TriggerFlags.CosmicSlice.Threshold() or defaultThresholds['Threshold'],            '-1' ,''),
    CosmicTau            ('AllTeTau'                           ,  '',            TriggerFlags.CosmicSlice.Threshold() or defaultThresholds['Threshold'],            '-1' ,''),
    CosmicCalo           ('AllTeCalo'                          ,  '',            TriggerFlags.CosmicSlice.Threshold() or defaultThresholds['Threshold'],            '-1' ,''),
    #
    CosmicJet            ('Jet'                           ,  TriggerFlags.CosmicSlice.CaloSignature() or defaultSignatures['CaloSignature'],            TriggerFlags.CosmicSlice.CaloThreshold() or defaultThresholds['CaloThreshold'],            '-1' ,'L1Calo'),
    CosmicJet            ('Jet_V2'                           ,  TriggerFlags.CosmicSlice.CaloSignature_V2() or defaultSignatures['CaloSignature_V2'],            TriggerFlags.CosmicSlice.CaloThreshold_V2() or defaultThresholds['CaloThreshold_V2'],            '-1' ,'L1Calo'),
    CosmicTau            ('Tau'                           ,  TriggerFlags.CosmicSlice.CaloSignature() or defaultSignatures['CaloSignature'],            TriggerFlags.CosmicSlice.CaloThreshold() or defaultThresholds['CaloThreshold'],            '-1' ,'L1Calo'),
    CosmicTau            ('Tau_V2'                           ,  TriggerFlags.CosmicSlice.CaloSignature_V2() or defaultSignatures['CaloSignature_V2'],            TriggerFlags.CosmicSlice.CaloThreshold_V2() or defaultThresholds['CaloThreshold_V2'],            '-1' ,'L1Calo'),
    CosmicCalo           ('Calo'                          ,  TriggerFlags.CosmicSlice.CaloSignature() or defaultSignatures['CaloSignature'],            TriggerFlags.CosmicSlice.CaloThreshold() or defaultThresholds['CaloThreshold'],            '-1' ,''),
    
    CosmicMuon           ('MuonOnly'                      ,  TriggerFlags.CosmicSlice.WidePatternRPCSignature() or defaultSignatures['WidePatternRPCSignature'],         TriggerFlags.CosmicSlice.WidePatternRPCThreshold() or defaultThresholds['WidePatternRPCThreshold'],         '-1' ,''),
    CosmicMuon           ('MuonOnly_V2'                      ,  TriggerFlags.CosmicSlice.MuonSignature_V2() or defaultSignatures['MuonSignature_V2'],         TriggerFlags.CosmicSlice.MuonThreshold_V2() or defaultThresholds['MuonThreshold_V2'],         '-1' ,''),
    CosmicMuon           ('MuonOnly_TGC'                  ,  TriggerFlags.CosmicSlice.TGCSignature() or defaultSignatures['TGCSignature'],         TriggerFlags.CosmicSlice.TGCThreshold() or defaultThresholds['TGCThreshold'],         '-1' ,''),
    CosmicMuon           ('Muon'                          ,  TriggerFlags.CosmicSlice.WidePatternRPCSignature() or defaultSignatures['WidePatternRPCSignature'],         TriggerFlags.CosmicSlice.WidePatternRPCThreshold() or defaultThresholds['WidePatternRPCThreshold'],         '-1' ,''),
    CosmicMuon           ('Muon_V2'                          ,  TriggerFlags.CosmicSlice.MuonSignature_V2() or defaultSignatures['MuonSignature_V2'],         TriggerFlags.CosmicSlice.MuonThreshold_V2() or defaultThresholds['MuonThreshold_V2'],         '-1' ,''),
    
#    CosmicMuon           ('Muon_Moore'                    ,  TriggerFlags.CosmicSlice.WidePatternRPCSignature() or defaultSignatures['WidePatternRPCSignature'],         TriggerFlags.CosmicSlice.WidePatternRPCThreshold() or defaultThresholds['WidePatternRPCThreshold'],         '-1' ,''),
#    CosmicMuon           ('Muon_Moore_TGC'                ,  TriggerFlags.CosmicSlice.TGCSignature() or defaultSignatures['TGCSignature'],         TriggerFlags.CosmicSlice.TGCThreshold() or defaultThresholds['TGCThreshold'],         '-1' ,''),
    
    CosmicMuon           ('Muon_MuonEF'                    ,  TriggerFlags.CosmicSlice.WidePatternRPCSignature() or defaultSignatures['WidePatternRPCSignature'],         TriggerFlags.CosmicSlice.WidePatternRPCThreshold() or defaultThresholds['WidePatternRPCThreshold'],         '-1' ,''),
    CosmicMuon           ('Muon_MuonEF_V2'                    ,  TriggerFlags.CosmicSlice.MuonSignature_V2() or defaultSignatures['MuonSignature_V2'],         TriggerFlags.CosmicSlice.MuonThreshold_V2() or defaultThresholds['MuonThreshold_V2'],         '-1' ,''),
    
    CosmicMuon           ('Muon_MuonEF_TGC'                ,  TriggerFlags.CosmicSlice.TGCSignature() or defaultSignatures['TGCSignature'],         TriggerFlags.CosmicSlice.TGCThreshold() or defaultThresholds['TGCThreshold'],         '-1' ,''),
    
    CosmicMufast         ('Mufast_WideRPC'                        ,  TriggerFlags.CosmicSlice.WidePatternRPCSignature() or defaultSignatures['WidePatternRPCSignature'],         TriggerFlags.CosmicSlice.WidePatternRPCThreshold() or defaultThresholds['WidePatternRPCThreshold'],         '-1' ,''),
    
    CosmicMufast         ('Mufast'                        ,  TriggerFlags.CosmicSlice.RPCSignature() or defaultSignatures['RPCSignature'],         TriggerFlags.CosmicSlice.RPCThreshold() or defaultThresholds['RPCThreshold'],         '-1' ,''),
    CosmicMufast         ('Mufast_V2'                        ,  TriggerFlags.CosmicSlice.MuonSignature_V2() or defaultSignatures['MuonSignature_V2'],         TriggerFlags.CosmicSlice.MuonThreshold_V2() or defaultThresholds['MuonThreshold_V2'],         '-1' ,''),
    
    CosmicMufast         ('Mufast_TGC'                    ,  TriggerFlags.CosmicSlice.TGCSignature() or defaultSignatures['TGCSignature'],         TriggerFlags.CosmicSlice.TGCThreshold() or defaultThresholds['TGCThreshold'],         '-1' ,''),
    CosmicMufast         ('Mufast_MuIso'                  ,  TriggerFlags.CosmicSlice.RPCSignature() or defaultSignatures['RPCSignature'],         TriggerFlags.CosmicSlice.RPCThreshold() or defaultThresholds['RPCThreshold'],         '-1' ,''),
    CosmicMufast         ('Mufast_MuIso_V2'                  ,  TriggerFlags.CosmicSlice.MuonSignature_V2() or defaultSignatures['MuonSignature_V2'],         TriggerFlags.CosmicSlice.MuonThreshold_V2() or defaultThresholds['MuonThreshold_V2'],         '-1' ,''),
    
#    CosmicMufast         ('Mufast_Moore_TGC'              ,  TriggerFlags.CosmicSlice.TGCSignature() or defaultSignatures['TGCSignature'],         TriggerFlags.CosmicSlice.TGCThreshold() or defaultThresholds['TGCThreshold'],         '-1' ,''),
#    CosmicMufast         ('Mufast_Moore'                  ,  TriggerFlags.CosmicSlice.RPCSignature() or defaultSignatures['RPCSignature'],         TriggerFlags.CosmicSlice.RPCThreshold() or defaultThresholds['RPCThreshold'],         '-1' ,''),
    
    CosmicMufast         ('Mufast_MuonEF_TGC'              ,  TriggerFlags.CosmicSlice.TGCSignature() or defaultSignatures['TGCSignature'],         TriggerFlags.CosmicSlice.TGCThreshold() or defaultThresholds['TGCThreshold'],         '-1' ,''),
    CosmicMufast         ('Mufast_MuonEF'                  ,  TriggerFlags.CosmicSlice.RPCSignature() or defaultSignatures['RPCSignature'],         TriggerFlags.CosmicSlice.RPCThreshold() or defaultThresholds['RPCThreshold'],         '-1' ,''),
    CosmicMufast         ('Mufast_MuonEF_V2'                  ,  TriggerFlags.CosmicSlice.MuonSignature_V2() or defaultSignatures['MuonSignature_V2'],         TriggerFlags.CosmicSlice.MuonThreshold_V2() or defaultThresholds['MuonThreshold_V2'],         '-1' ,''),
#     CosmicMufast         ('Mufast_Moore_WideRPC'          ,  TriggerFlags.CosmicSlice.WidePatternRPCSignature() or defaultSignatures['WidePatternRPCSignature'],         TriggerFlags.CosmicSlice.WidePatternRPCThreshold() or defaultThresholds['WidePatternRPCThreshold'],         '-1' ,''),
    CosmicMufast         ('Mufast_MuonEF_WideRPC'          ,  TriggerFlags.CosmicSlice.WidePatternRPCSignature() or defaultSignatures['WidePatternRPCSignature'],         TriggerFlags.CosmicSlice.WidePatternRPCThreshold() or defaultThresholds['WidePatternRPCThreshold'],         '-1' ,''),
    
    CosmicMBiasCalo      ('AllTeMBiasCalo_EndCapA'                     , '' , MBTSThresholdList_SideA,           '-1' ,'MBTS_BCM_LUCID'),
    CosmicMBiasCalo      ('AllTeMBiasCalo_EndCapC'                     , '' , MBTSThresholdList_SideA,           '-1' ,'MBTS_BCM_LUCID'),
    
    CosmicMBiasCalo      ('MBiasCalo_EndCapA_V2'                     ,  ','.join(MBTSItemList_SideA_V2), MBTSThresholdList_SideA_V2,           '-1' ,'MBTS_BCM_LUCID'),
    CosmicMBiasCalo      ('MBiasCalo_EndCapC_V2'                     ,  ','.join(MBTSItemList_SideC_V2), MBTSThresholdList_SideC_V2,           '-1' ,'MBTS_BCM_LUCID'),
    #
#   # CosmicMinBias        ('AllTeMBStandaloneSpacePoints'       ,  TriggerFlags.CosmicSlice.StandaloneSignature() or defaultSignatures['StandaloneSignature'],  TriggerFlags.CosmicSlice.StandaloneThreshold() or defaultThresholds['StandaloneThreshold'],  '-1' ,''),
    CosmicMinBias        ('AllTeMBSpacePoints'                 ,  TriggerFlags.CosmicSlice.Signature() or defaultSignatures['Signature'],            TriggerFlags.CosmicSlice.Threshold() or defaultThresholds['Threshold'],            '-1' ,''),
    CosmicMinBias        ('AllTeMBSpacePoints_V2'                 ,  TriggerFlags.CosmicSlice.Signature_V2() or defaultSignatures['Signature_V2'],            TriggerFlags.CosmicSlice.Threshold_V2() or defaultThresholds['Threshold_V2'],            '-1' ,''),
    #
#   # CosmicMinBias        ('AllTeMBStandaloneSpacePoints'       ,  '', '',  '-1' ,''),
    CosmicCalibration    ("SingleBeamTriggerType0" ,  '',            '', '-1' ,'',ExpressStreamPrescale=10000000),
    CosmicCalibration    ("SingleBeamRNDM"         ,  '',            '', '-1' ,'',ExpressStreamPrescale=10000000),
    CosmicCalibration    ("SingleBeamBPTX"         ,  '',            '', '-1' ,'',ExpressStreamPrescale=10000000),
    CosmicCalibration    ("SingleBeamL1Calo"       ,  '',            '', '-1' ,'',ExpressStreamPrescale=10000000),
    CosmicCalibration    ("SingleBeamL1CaloEMFilter"       ,  ','.join(L1CaloItemList_EM),             '', '-1' ,'',ExpressStreamPrescale=10000000),
    CosmicCalibration    ("SingleBeamL1CaloEMFilter_V2"       ,  ','.join(L1CaloItemList_EM_V2),          '', '-1' ,'',ExpressStreamPrescale=10000000),
    CosmicCalibration    ("SingleBeamTGCwBeam"     ,  '',            '', '-1' ,'',ExpressStreamPrescale=10000000),
    CosmicCalibration    ("SingleBeamRPCwBeam"     ,  '',            '', '-1' ,'',ExpressStreamPrescale=10000000),
    CosmicCalibration    ("SingleBeamMBTS"         ,  '',            '', '-1' ,'',ExpressStreamPrescale=10000000),
    CosmicCalibration    ("SingleBeamMBTSFilter"   ,  '',            '', '-1' ,'',ExpressStreamPrescale=10000000),
    CosmicCalibration    ("SingleBeamBCM"          ,  '',            '', '-1' ,'',ExpressStreamPrescale=10000000),
    CosmicCalibration    ("SingleBeamLUCID"        ,  '',            '', '-1' ,'',ExpressStreamPrescale=10000000),
    CosmicCalibration    ("SingleBeamCosmicMuons"  ,  '',            '', '-1' ,'',ExpressStreamPrescale=10000000),
    #
    #
    #
    CosmicCalibration    ('StandaloneCalibration'         ,  TriggerFlags.CosmicSlice.CalibrationSignature() or defaultSignatures['CalibrationSignature'], TriggerFlags.CosmicSlice.CalibrationThreshold() or defaultThresholds['CalibrationThreshold'], '-1' ,''),
    CosmicCalibration    ('Calibration'                   ,  TriggerFlags.CosmicSlice.Signature() or defaultSignatures['Signature'],            TriggerFlags.CosmicSlice.Threshold() or defaultThresholds['Threshold']           , '-1' ,''),
    
    CosmicCalibration    ('NIM0Passthrough'                  ,  '',            ['NIM0'], '-1' ,''),
    CosmicCalibration    ('NIM4Passthrough'                  ,  '',            ['NIM4'], '-1' ,''),
    CosmicCalibration    ('TRTPassthrough'                  ,  '',           ['TRT'], '-1' ,'IDCosmic'),
    
    CosmicCalibration    ('TilePassthrough'                  ,  '',           ['Tile'], '-1' ,'CosmicMuons'),
    
    CosmicCalibration    ('CALREQ0Calibration'                 ,  '',             ['CALREQ0'], '-1' ,''),
    CosmicCalibration    ('CALREQ1Calibration'                 ,  '',             ['CALREQ1'], '-1' ,''),
    CosmicCalibration    ('CALREQ2Calibration'                 ,  '',             ['CALREQ2'], '-1' ,''),
    
    CosmicCalibration    ('EM5Passthrough'                   ,                  'L1_EM5'  ,    ['EM5']  , '-1' ,''),
    CosmicCalibration    ('EM10Passthrough'                   ,                 'L1_EM10' ,    ['EM10'] , '-1' ,''),
    CosmicCalibration    ('EM15Passthrough'                   ,                 'L1_EM15' ,    ['EM15'] , '-1' ,''),
    CosmicCalibration    ('EM20Passthrough'                   ,                 'L1_EM20' ,    ['EM20'] , '-1' ,''),
    CosmicCalibration    ('EM30Passthrough'                   ,                 'L1_EM30' ,    ['EM30'] , '-1' ,''),
    CosmicCalibration    ('EM40Passthrough'                   ,                 'L1_EM40' ,    ['EM40'] , '-1' ,''),
    CosmicCalibration    ('EM50Passthrough'                   ,                 'L1_EM50' ,    ['EM50'] , '-1' ,''),
    CosmicCalibration    ('EM100Passthrough'                   ,                'L1_EM100',    ['EM100'], '-1' ,''),
                                                                                                                                                                 
    CosmicCalibration    ('JET5Passthrough'                    ,                 'L1_J5'  ,     ['J5']  , '-1' ,''),
    CosmicCalibration    ('JET10Passthrough'                    ,                'L1_J10' ,     ['J10'] , '-1' ,''),
    CosmicCalibration    ('JET15Passthrough'                    ,                'L1_J15' ,     ['J15'] , '-1' ,''),
    CosmicCalibration    ('JET20Passthrough'                    ,                'L1_J20' ,     ['J20'] , '-1' ,''),
    CosmicCalibration    ('JET30Passthrough'                    ,                'L1_J30' ,     ['J30'] , '-1' ,''),
    CosmicCalibration    ('JET40Passthrough'                    ,                'L1_J40' ,     ['J40'] , '-1' ,''),
    CosmicCalibration    ('JET50Passthrough'                    ,                'L1_J50' ,     ['J50'] , '-1' ,''),
    CosmicCalibration    ('JET100Passthrough'                    ,               'L1_J100',     ['J100'], '-1' ,''),
                                                                                                                                                                 
    CosmicCalibration    ('TAU5Passthrough'                  ,                 'L1_TAU5'  ,   ['TAU5']  , '-1' ,''),
    CosmicCalibration    ('TAU10Passthrough'                  ,                'L1_TAU10' ,   ['TAU10'] , '-1' ,''),
    CosmicCalibration    ('TAU15Passthrough'                  ,                'L1_TAU15' ,   ['TAU15'] , '-1' ,''),
    CosmicCalibration    ('TAU20Passthrough'                  ,                'L1_TAU20' ,   ['TAU20'] , '-1' ,''),
    CosmicCalibration    ('TAU30Passthrough'                  ,                'L1_TAU30' ,   ['TAU30'] , '-1' ,''),
    CosmicCalibration    ('TAU40Passthrough'                  ,                'L1_TAU40' ,   ['TAU40'] , '-1' ,''),
    CosmicCalibration    ('TAU50Passthrough'                  ,                'L1_TAU50' ,   ['TAU50'] , '-1' ,''),
    CosmicCalibration    ('TAU100Passthrough'                  ,               'L1_TAU100',   ['TAU100'], '-1' ,''),
                                                                                                                                                                 
    CosmicCalibration    ('JF5Passthrough'                   ,                  'L1_JF5'  ,    ['JF5']  , '-1' ,''),
    CosmicCalibration    ('JF10Passthrough'                   ,                 'L1_JF10' ,    ['JF10'] , '-1' ,''),
    CosmicCalibration    ('JF15Passthrough'                   ,                 'L1_JF15' ,    ['JF15'] , '-1' ,''),
    CosmicCalibration    ('JF20Passthrough'                   ,                 'L1_JF20' ,    ['JF20'] , '-1' ,''),
                                                                                                                                                                 
    CosmicCalibration    ('JB5Passthrough'                   ,                  'L1_JB5'  ,    ['JB5']  , '-1' ,''),
    CosmicCalibration    ('JB10Passthrough'                   ,                 'L1_JB10' ,    ['JB10'] , '-1' ,''),
    CosmicCalibration    ('JB15Passthrough'                   ,                 'L1_JB15' ,    ['JB15'] , '-1' ,''),
    CosmicCalibration    ('JB20Passthrough'                   ,                 'L1_JB20' ,    ['JB20'] , '-1' ,''),
    
    CosmicCalibration    ('MBTSPassthrough'                ,                     '',MBTSThresholdList, '-1' ,''),
    CosmicCalibration    ('L1CaloPassthrough'                ,                     '',L1CaloThresholdList, '-1' ,''),
    
    CosmicCalibration    ('BCMPassthrough'               ,                        '',['BCM_Halo','BCM_Wide','BCM_Ideal'], '-1' ,'','',1,1),
    CosmicCalibration    ('LUCIDPassthrough'             ,                        '',['LUCID_A','LUCID_C','LUCID_A_C'], '-1' ,'','',1,1),
    
    CosmicCalibration    ('RNDM0Passthrough'             ,                        '','RNDM0', '-1' ,'','',-1,1),
    CosmicCalibration    ('RNDM1Passthrough'             ,                        '','RNDM1', '-1' ,'','',-1,1),
    CosmicCalibration    ('PhysicsPassthrough'             ,                     '','', '-1' ,'','',1,1),
    CosmicCalibration    ('CalibrationPassthrough'         ,                     '','', '-1' ,'','',1,1),
    CosmicCalibration    ('L2CalibrationPassthrough'       ,                     '','', '-1' ,'','',1,1),
    CosmicCalibration    ('EFCalibrationPassthrough'       ,                     '','', '-1' ,'','',1,1),
    #
    CosmicROIFilter  ('CosmicDownwardMuonRPCROI'            ,        '',["MU0_LOW_RPC","MU0_LOW_RPC_SPARE","MU0_LOW_RPC_EMPTY","MU0_LOW_RPC_BPTX","MU0_HIGH_RPC","MU0_HIGH_RPC_EMPTY","MU0_HIGH_RPC_BPTX","MU6_RPC","MU6_RPC_BPTX"],'-1','CosmicDownwardMuon'), 
    CosmicROIFilter  ('CosmicDownwardMuonTGCROI'            ,        '',["MU0_TGC_HALO","MU0_TGC_HALO_SPARE","MU0_TGC_EMPTY","MU0_TGC_HALO_EMPTY","MU0_TGC_HALO_BPTX","MU0_TGC","MU0_TGC_BPTX","MU6_TGC","MU6_TGC_BPTX"],'-1','CosmicDownwardMuon'), 
    #
    CosmicL1CaloFilter  ('L1CaloFilter'               ,        ','.join(L1CaloItemList_NoXE)  , ''   ,'-1',''), 
    CosmicL1CaloFilter  ('L1CaloEMFilter'             ,        ','.join(L1CaloItemList_EM),     ''      ,'-1',''), 
    CosmicL1CaloFilter  ('L1CaloTauFilter'            ,        ','.join(L1CaloItemList_TAU),    ''     ,'-1',''), 
    CosmicL1CaloFilter  ('L1CaloJetFilter'            ,        ','.join(L1CaloItemList_J),      ''       ,'-1',''),
    #
    CosmicL1CaloFilter  ('L1CaloFilter_V2'               ,        ','.join(L1CaloItemList_NoXE_V2),  ''    ,'-1',''), 
    CosmicL1CaloFilter  ('L1CaloEMFilter_V2'             ,        ','.join(L1CaloItemList_EM_V2),    ''    ,'-1',''), 
    CosmicL1CaloFilter  ('L1CaloTauFilter_V2'            ,        ','.join(L1CaloItemList_TAU_V2),   ''    ,'-1',''), 
    CosmicL1CaloFilter  ('L1CaloJetFilter_V2'            ,        ','.join(L1CaloItemList_J_V2),     ''    ,'-1',''), 
    #
    CosmicTileCalibration  ('TileCalib_ped'               ,  'L1_CALREQ0',  'CALREQ0',  '-1' ,'tilecalib'),
    CosmicTileCalibration  ('TileCalib_cis'               ,  'L1_CALREQ1',  'CALREQ1',  '-1' ,'tilecalib'),
    CosmicTileCalibration  ('TileCalib_laser'               ,'L1_CALREQ2',  'CALREQ2',  '-1' ,'tilecalib'),
    
    CosmicPixelCalibration  ('Cosmic_pixelnoise'            ,'L1_RD0_FILLED','RNDM0',  '-1' ,'pixelnoise'),
    CosmicPixelCalibration  ('Cosmic_pixelnoise_V2'            ,'L1_RD0_EMPTY','RNDM0',  '-1' ,'pixelnoise'),
    CosmicRateAnalysis      ('Cosmic_rateanalysis'            ,'','',  '-1' ,'rateanalysis'),
    CosmicLArCalib       ('LArCalib'                      ,  TriggerFlags.CosmicSlice.CaloSignature() or defaultSignatures['CaloSignature'],            TriggerFlags.CosmicSlice.CaloThreshold() or defaultThresholds['CaloThreshold'],            '-1' ,'LAr_calibration','calibration',10000,1),
    CosmicLArCalib       ('LArCalib_V2'                      ,  TriggerFlags.CosmicSlice.CaloSignature_V2() or defaultSignatures['CaloSignature_V2'],            TriggerFlags.CosmicSlice.CaloThreshold_V2() or defaultThresholds['CaloThreshold_V2'],            '-1' ,'LAr_calibration','calibration',10000,1),

    CosmicMEt           ('AllTeMEt'                    ,  '',         '',         '-1' ,''),
    CosmicMEt           ('AllTeMEt_EFOnly'             ,  '',         '',         '-1' ,''),
    CosmicMEt           ('MEt'                    ,  TriggerFlags.CosmicSlice.RPCSignature() or defaultSignatures['RPCSignature'],         TriggerFlags.CosmicSlice.RPCThreshold() or defaultThresholds['RPCThreshold'],         '-1' ,''),
    CosmicMEt           ('MEt_EFOnly'             ,  TriggerFlags.CosmicSlice.RPCSignature() or defaultSignatures['RPCSignature'],         TriggerFlags.CosmicSlice.RPCThreshold() or defaultThresholds['RPCThreshold'],         '-1' ,''),
    CosmicMEt           ('MEt_EFOnly_Mufast'      ,  TriggerFlags.CosmicSlice.RPCSignature() or defaultSignatures['RPCSignature'],         TriggerFlags.CosmicSlice.RPCThreshold() or defaultThresholds['RPCThreshold'],         '-1' ,''),
    CosmicMEt           ('MEt_Mufast'             ,  TriggerFlags.CosmicSlice.RPCSignature() or defaultSignatures['RPCSignature'],         TriggerFlags.CosmicSlice.RPCThreshold() or defaultThresholds['RPCThreshold'],         '-1' ,''),
    CosmicMEt           ('MEt_EFOnly_Mufast_MuonEF'      ,  TriggerFlags.CosmicSlice.RPCSignature() or defaultSignatures['RPCSignature'],         TriggerFlags.CosmicSlice.RPCThreshold() or defaultThresholds['RPCThreshold'],         '-1' ,''),
    CosmicMEt           ('MEt_Mufast_MuonEF'             ,  TriggerFlags.CosmicSlice.RPCSignature() or defaultSignatures['RPCSignature'],         TriggerFlags.CosmicSlice.RPCThreshold() or defaultThresholds['RPCThreshold'],         '-1' ,''),
    #
    CosmicMEt           ('MEt_V2'                    ,  TriggerFlags.CosmicSlice.MuonSignature_V2() or defaultSignatures['MuonSignature_V2'],         TriggerFlags.CosmicSlice.MuonThreshold_V2() or defaultThresholds['MuonThreshold_V2'],         '-1' ,''),
    CosmicMEt           ('MEt_EFOnly_V2'             ,  TriggerFlags.CosmicSlice.MuonSignature_V2() or defaultSignatures['MuonSignature_V2'],         TriggerFlags.CosmicSlice.MuonThreshold_V2() or defaultThresholds['MuonThreshold_V2'],         '-1' ,''),
    CosmicMEt           ('MEt_EFOnly_Mufast_V2'      ,  TriggerFlags.CosmicSlice.MuonSignature_V2() or defaultSignatures['MuonSignature_V2'],         TriggerFlags.CosmicSlice.MuonThreshold_V2() or defaultThresholds['MuonThreshold_V2'],         '-1' ,''),
    CosmicMEt           ('MEt_Mufast_V2'             ,  TriggerFlags.CosmicSlice.MuonSignature_V2() or defaultSignatures['MuonSignature_V2'],         TriggerFlags.CosmicSlice.MuonThreshold_V2() or defaultThresholds['MuonThreshold_V2'],         '-1' ,''),
    CosmicMEt           ('MEt_EFOnly_Mufast_MuonEF_V2'      ,  TriggerFlags.CosmicSlice.MuonSignature_V2() or defaultSignatures['MuonSignature_V2'],         TriggerFlags.CosmicSlice.MuonThreshold_V2() or defaultThresholds['MuonThreshold_V2'],         '-1' ,''),
    CosmicMEt           ('MEt_Mufast_MuonEF_V2'             ,  TriggerFlags.CosmicSlice.MuonSignature_V2() or defaultSignatures['MuonSignature_V2'],         TriggerFlags.CosmicSlice.MuonThreshold_V2() or defaultThresholds['MuonThreshold_V2'],         '-1' ,''),
    #
    CosmicMEt           ('AllTe_MEt_EFOnly_Mufast_MuonEF'      , '',         '',         '-1' ,''), 
    CosmicMEt           ('AllTe_MEt_Mufast_MuonEF'             , '',         '',         '-1' ,''),
    CosmicMEt           ('AllTe_MEt_EFOnly_MuonEF'      , '',         '',         '-1' ,''), 
    CosmicMEt           ('AllTe_MEt_MuonEF'             , '',         '',         '-1' ,''),
    #
    #
    HLTBeamLineFitterTest  ('HLTBeamLineFitterTest'            ,        '',     '',  '-1',''), 
    HLTBeamLineFitterTest  ('HLTBeamLineFitterTestFake'        ,        '',     '',  '-1',''), 
]

CosmicSlices=[]
for slice in Cosmics: CosmicSlices += [ slice.sig_id ]

#################################################################################
### Chech that Cosmic flags are up to date with respect to what's implemented ###
#################################################################################

sortedSliceNames=CosmicSlices
sortedSliceNames.sort()

sortedSliceNamesFromFlags=TriggerFlags.CosmicSlice.SliceList.StoredValue
sortedSliceNamesFromFlags.sort()

if (sortedSliceNames!=sortedSliceNamesFromFlags):
    if (TriggerFlags.CosmicSlice.testCosmic()==True):
        log.info("Something is wrong: you most likely added")
        log.info("a new slice to Cosmic.py but forgot to uupdate")
        log.info("the list in Trigger/TriggerCommon/TriggerMenuPython/python/CosmicSliceFlags.py")
        log.info(str(sortedSliceNames))
        log.info(str(sortedSliceNamesFromFlags))
        log.info("missing from sortedSliceNamesFromFlags")
        for x in sortedSliceNames:
            if (x not in sortedSliceNamesFromFlags):
                log.info(x)
        log.info("missing from sortedSliceNames")
        for x in sortedSliceNamesFromFlags:
            if (x not in sortedSliceNames):
                log.info(x)

def setupCosmics():
    from TriggerMenuPython.Lvl1 import Lvl1
    from TriggerMenuPython.Lvl1Flags import Lvl1Flags
    from TriggerMenuPython.TriggerPythonConfig import TriggerPythonConfig
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    from AthenaCommon.Logging import logging

    #TriggerFlags.Slices_all_setOff()
    #lvl1Flags.thresholds.allowedValues = TriggerFlags.CosmicSlice.UsedLVL1Thresholds.StoredValue
    if (TriggerFlags.CosmicSlice.testCosmic()==True):
        log.info(str( Lvl1Flags.thresholds.allowedValues ))
    Lvl1Flags.thresholds.StoredValue = TriggerFlags.CosmicSlice.UsedLVL1Thresholds.StoredValue

    Lvl1FlagList=[ "L1_%s" % x for x in TriggerFlags.CosmicSlice.UsedLVL1Signatures.StoredValue]

    Lvl1Flags.items.StoredValue=[]
    for x in Lvl1FlagList:
        if (x!="L1_"):
            Lvl1Flags.items.StoredValue += [ x ]
        #else:
        #    Lvl1Flags.items.StoredValue += ['']

    StreamConfig = {'physics' : [] }
    for x in Cosmics:
        StreamConfig['physics'] += [ x.sig_id ]

    if (TriggerFlags.CosmicSlice.testCosmic()==True):
        log.info( "used Items")
        log.info(str( Lvl1Flags.thresholds()))
        log.info(str(Lvl1Flags.items()))
        log.info(str(StreamConfig))
