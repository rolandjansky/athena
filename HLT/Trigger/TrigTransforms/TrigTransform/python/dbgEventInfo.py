#!/usr/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# @brief: This is the dbgEventInfo class for the Debug Stream event analysis

from __future__ import print_function

import eformat
from TrigTransform.dbgHltResult import hltResult as hltResult
import ROOT
from ROOT import gStyle, gROOT, addressof

import logging
msg = logging.getLogger('PyJobTransforms.' + __name__)


class dbgEventInfo:

    def __init__(self, dbgStep='_Default', inputFile=''):
        # Event Info Tree values
        self.Run_Number                            = 0
        self.Stream_Tag_Name                       = 'None'
        self.Stream_Tag_Type                       = 'None'
        self.Lvl1_ID                               = 0
        self.Event_Counter_Lvl1_ID                 = 0
        self.Event_Counter_Reset_Counter_Lvl1_ID   = 0
        self.Global_ID                             = 0
        self.Lumiblock                             = 0
        self.Node_ID                               = 0
        self.HLT_Action                            = 'None'
        self.HLT_Reason                            = 'None'
        self.HLT_Internal                          = 'None'
        self.EF_Overall                            = 'None'
        self.EF_PSC_Result                         = 'None'
        self.EF_Status_Result                      = []
        self.EF_Status_Overall                     = 'None'
        self.EF_ErrorChain                         = 'None'
        self.EF_ErrorStep                          = -1
        self.EF_Result                             = []
        self.HLT_Result                            = 'None'
        self.SuperMasterKey                        = 0
        self.HLT_PSC_Key                           = 0
        self.HLT_Application                       = 'None'
        self.HLT_Decision                          = False
        self.L1_Chain_Names                        = []
        self.HLT_Chain_Names                       = []
        self.EventStatusNames                      = []

        self.eventCounter = 0
        self.HLTRes = hltResult()
        self.rootDefinitions(dbgStep, inputFile)

        # Event Specific Status
        self.EventSpecificStatus = ['Reserved',
                                    'Reserved',
                                    'Reserved',
                                    'HLTPU_Dupl',
                                    'Reserved',
                                    'Reserved',
                                    'Reserved',
                                    'Reserved',
                                    'DCM_TO',
                                    'HLTPU_TO',
                                    'DCM_Dupl',
                                    'DCM_Reco',
                                    'HLTPSC_Pbl',
                                    'DCM_FA',
                                    'Reserved',
                                    'PARTIAL_Evt']

        # EF Status Names:
        #  First word indicates if fragmant is valid
        #  Second word seems to be always 0
        #  Third word contains definition of errors
        self.EF_Status_Word = ['INVALID_FULL_EVENT_FRAGMENT',
                               'MIN_TRIG_EVENT_LOOP_MGR',
                               'NO_EVENT_INFO',
                               'NO_HLT_RESULT',
                               'NO_EF_TRIG_INFO',
                               'NO_STREAM_TAG',
                               'NO_EF_ROB_FRAG',
                               'DUMMY_EVENT_INFO',
                               'DUMMY_HLT_RESULT',
                               'DUMMY_STREAM_TAG']

        # These are copied from TrigSteeringEvent/Enums.h
        self.actionNames = ['CONTINUE', 'ABORT_CHAIN',
                            'ABORT_EVENT', 'ABORT_JOB']
        self.reasonNames = ['UNKNOWN=0', 'MISSING_FEATURE', 'GAUDI_EXCEPTION',
                            'EFORMAT_EXCEPTION', 'STD_EXCEPTION',
                            'UNKNOWN_EXCEPTION', 'NAV_ERROR',
                            'MISSING_ROD', 'CORRUPTED_ROD',
                            'TIMEOUT', 'BAD_JOB_SETUP',
                            'USERDEF_1', 'USERDEF_2',
                            'USERDEF_3', 'USERDEF_4', 'USERDEF_5']

        self.internalNames = ['UNKNOWN=0', 'NO_LVL1_ITEMS',
                              'NO_LVL2_CHAINS', 'NO_LVL1_RESULT',
                              'WRONG_HLT_RESULT', 'NO_HLT_RESULT',
                              'ALGO_ERROR', 'TIMEOUT', 'BAD_JOB_SETUP']


    def eventCount(self, event):
        self.eventCounter += 1
        msg.info('Running debug_stream analysis on event :{0}'.format(self.eventCounter))


    def eventInfo(self, event, L1ChainNames, HLTChainNames):
        # Main function in this class: calls most methods in right order
        self.Run_Number = event.run_no()

        self.L1_Chain_Names = L1ChainNames
        self.HLT_Chain_Names = HLTChainNames

        streamtagNames = ','.join([tag.name for tag in event.stream_tag()])
        self.Stream_Tag_Name = streamtagNames

        streamtagTypes = ','.join([tag.type for tag in event.stream_tag()])
        self.Stream_Tag_Type = streamtagTypes

        self.Lvl1_ID = event.lvl1_id()

        self.Event_Counter_Lvl1_ID = self.Lvl1_ID & 0xffffff
        self.Event_Counter_Reset_Counter_Lvl1_ID = (self.Lvl1_ID & 0xff000000) >> 24

        self.Global_ID = event.global_id()
        self.Lumiblock = event.lumi_block()

        self.eventStatus(event)
        self.lvl1Info(event, L1ChainNames)
        self.hltInfo(event, HLTChainNames)
        self.hltResult(event)
        self.getName(self.EF_Result, HLTChainNames)
        self.getHLTError(self.HLT_Result)
        self.getEFError(self.EF_Status_Result)


    # TODO is always 0
    def eventStatus(self, event):
        # Get event specific status
        statusList = []
        j = -1
        for i in range(16, 32):
            j += 1
            status = (event.status()[0] >> i) & 0x1
            if (status == 1):
                statusList.append(self.EventSpecificStatus[j])

        self.EventStatusNames = ','.join(str(name) for name in statusList)
        msg.info('Event Status :{0}'.format(self.EventStatusNames))


    def lvl1Info(self, event, L1ChainNames):
        # Get LVL1 info for BP AV and AV-TrigIDs and store them in ROOT vectors
        self.L1_Triggered_BP.clear()
        self.L1_Triggered_AV.clear()
        self.L1_Triggered_IDs.clear()

        # Decode Lvl1 trigger info
        # Information about items is saved as a continuous memory and divided into 4 groups:
        #   passed before prescale, passed after prescale, passed after veto and passed (?)
        # Each group consist of 8 32-bit integers
        def decodeGroup(group, event):
            idList = []
            try:
                # There are 8 bits per group
                for k in range(8):
                    # Index in continuous lvl1_trigger_info
                    eventInfoIdx = group * 8 + k

                    # Check the bits
                    for i in range(32):
                        L1IDed = (event.lvl1_trigger_info()[eventInfoIdx] >> i) & 0x1
                        if L1IDed:
                            id = i + k * 32
                            idList.append(id)
                            
            except IndexError:
                msg.info('Missing LVL1 Triggered ID Info!')

            return idList

        bpIds = decodeGroup(0, event) # before prescale
        avIds = decodeGroup(2, event) # after veto

        for id in bpIds:
            self.L1_Triggered_BP.push_back(L1ChainNames[id])

        for id in avIds:
            self.L1_Triggered_AV.push_back(L1ChainNames[id])
            self.L1_Triggered_IDs.push_back(id)

        msg.info('LVL1 Triggered ID Chains AV : %s', avIds)


    def hltInfo(self, event, HLTChain_Names):
        # Get HLT info and store it in ROOT vectors
        self.HLT_Triggered_Names.clear()
        self.HLT_Triggered_IDs.clear()

        # Check if chains saved in EF info were triggeres
        chainIdx = 0
        chainList = []
        for word in event.event_filter_info():
            for i in range(32):
                if word & (1 << i):
                    chainList.append(chainIdx)
                chainIdx += 1

        for idx in chainList:
            self.HLT_Triggered_IDs.push_back(idx)
            self.HLT_Triggered_Names.push_back(HLTChain_Names[idx])

        msg.info('HLT Triggered ID Chains  : %s', chainList)


    def getChain(self, counter, s):
        # Prints chains and their information
        from cppyy.gbl import HLT
        ch = HLT.Chain(s)
        # ch.deserialize(s)
        msg.info('.... chain {0:-3d}: {1} Counter: {2:-4d} Passed: {3} (Raw: {4} Prescaled: {5} PassThrough: {6}) Rerun: {7} LastStep: {8} Err: {9}'
                .format(counter, self.HLT_Chain_Names[ch.getChainCounter()], ch.getChainCounter(), ch.chainPassed(), ch.chainPassedRaw(),
                ch.isPrescaled(), ch.isPassedThrough(), ch.isResurrected(), ch.getChainStep(), ch.getErrorCode().str()))


    def getAllChains(self, blob):
        msg.info('... chains:')
        for i in range(len(blob)):
            self.getChain(i, blob[i])


    def hltResult(self, event):
        # Get the hlt result, status, decision etc if hlt_result exist
        HLTResult = None
        EFResult = None
        EFStatus = None
        hltResultFound = False
        for rob in event.children():
            if rob.source_id().subdetector_id() == eformat.helper.SubDetector.TDAQ_EVENT_FILTER:
                msg.info('.. {0} source: {1} source ID: 0x{2:x} size: {3}'
                        .format(rob.__class__.__name__, rob.source_id(), rob.source_id().code(), rob.fragment_size_word()*4))
                try:
                    self.HLTRes.load(rob)
                    version = self.HLTRes.getHLTResultClassVersion()
                    l1id = self.HLTRes.getLvl1Id()
                    acc = self.HLTRes.isAccepted()
                    status = self.HLTRes.getHLTStatus().str()
                    pt = self.HLTRes.isPassThrough()
                    cnvstatus = self.HLTRes.getLvlConverterStatus().str()
                    level = self.HLTRes.getHLTLevel()
                    nosigs = self.HLTRes.getNumOfSatisfiedSigs()
                    bad = self.HLTRes.isCreatedOutsideHLT()
                    trunc = self.HLTRes.isHLTResultTruncated()

                    msg.info('version   :%s', version)
                    msg.info('l1id      :%s', l1id)
                    msg.info('accepted  :%s', acc)
                    msg.info('status    :%s', status)
                    msg.info('passthr   :%s', pt)
                    msg.info('cnvstatus :%s', cnvstatus)
                    msg.info('level     :%s', level)
                    msg.info('nosigs    :%s', nosigs)
                    msg.info('bad       :%s', bad)
                    msg.info('trunc     :%s', trunc)

                    chains_data = list(self.HLTRes.getChainResult())
                    nchains = chains_data[0] if chains_data else 0
                    nav_data = list(self.HLTRes.getNavigationResult())
                    nver = nav_data[0] if nav_data else 0
                    msg.info('nchains   :%s', nchains)
                    msg.info('nver      :%s', nver)
                    msg.info('chains    :%s', chains_data)

                    self.Node_ID = rob.source_id().module_id()
                    EFStatus = rob.rod_status()
                    data = rob.rod_data()
                    if len(data) >= 14:
                        EFResult = data[0:14]
                        msg.debug(self.HLTRes)
                    if EFResult:
                        HLTResult = EFResult[4]

                    self.get_all_chains(chains_data[1:])
                    self.HLT_Decision = self.HLTRes.isAccepted()
                    self.HLT_PSC_Key = self.HLTRes.getConfigPrescalesKey()
                    self.SuperMasterKey = self.HLTRes.getConfigSuperMasterKey()
                    self.HLT_Application = self.HLTRes.appName()
                    hltResultFound = True

                except Exception as ex:
                    msg.info('... **** problems in analyzing payload %s', ex)
                    msg.info('... **** raw data[:10] %s', list(rob.rod_data())[:10])
                    msg.info('.. EOF HLTResult for EF')

        if not hltResultFound:
            msg.info('No HLT Result for EF')

        self.EF_Result = EFResult
        self.EF_Status_Result = EFStatus
        self.HLT_Result = HLTResult
        if self.EF_Status_Result :
            self.EF_Status_Overall = 'EF_Status_Found'
        else:
            self.EF_Status_Overall = 'No_EF_Status'

        msg.info('EF_Status_Overall :%s', self.EF_Status_Overall)
        msg.info('HLT_Result :%s', self.HLT_Result)
        msg.info('HLT Decision :%s', self.HLT_Decision)
        msg.info('HLT_Configuration  smk:{0}   hltpskey :{1}'.format(self.SuperMasterKey, self.HLT_PSC_Key))
        msg.info('Application name :%s', self.HLT_Application)


    def getName(self, result, HLTChainName):    
        # Set step and the name of the HLT chain that caused error if exist
        ChainName = ''
        ErrorStep = 0

        if (result and HLTChainName):
            ChainName = HLTChainName[int(result[8])]
            if ChainName != 0:
                ErrorStep = result[9]
            else:
                ChainName = 'No Error in Chain'
        else:
            ChainName = 'No HLT Result'
        
        self.EF_ErrorChain = ChainName
        self.EF_ErrorStep = ErrorStep


    def getHLTError(self, HLTResult):
        # Set the code of HLT error if hltResult exist
        if HLTResult is not None:
            reasonCode = 0
            actionCode = 0
            internalCode = 0
            try:
                reasonCode = HLTResult & 0xf
                actionCode = (HLTResult & 0x30) >> 4
                internalCode = (HLTResult & 0xffff00) >> 8
                self.HLT_Action = self.actionNames[actionCode]
                self.HLT_Reason = self.reasonNames[reasonCode]
                self.HLT_Internal = self.internalNames[internalCode]
                msg.debug('HLT_Result actionCode   :{0}'.format(actionCode))
                msg.debug('HLT_Result reasonCode   :{0}'.format(reasonCode))
                msg.debug('HLT_Result internalCode :{0}'.format(internalCode))
                
            except IndexError:
                self.HLT_Action   = 'Error_In_Processing'
                self.HLT_Reason   = 'Error_In_Processing'
                self.HLT_Internal = 'Error_In_Processing'

        else:
            self.HLT_Action   ='No HLT Result'
            self.HLT_Reason   ='No HLT Result'
            self.HLT_Internal ='No HLT Result'

        msg.info("HLT Action:{0}  Reason:{1}  Internal:{2}".format(self.HLT_Action, self.HLT_Reason, self.HLT_Internal))


    def getEFError(self, EFResult):
        # Set overall EF error status
        if EFResult:
            if EFResult[0] == 0:
                self.EF_Overall = 'OK'
            else:
                self.EF_Overall = 'ERROR'
        
                try:
                    self.EF_PSC_Result = self.EF_Status_Word[EFResult[2]]
                    msg.info('******** EF_Status_Word[EF_Result[2]]: {0}'.format(self.EF_Status_Word[EFResult[2]]))
                except IndexError:
                    self.EF_PSC_Result = 'Error_In_Processing'

        else:
            self.EF_Overall = 'No EF Status'
            self.EF_PSC_Result = 'No EF Status'


    def fillTree(self):
        # Fill Event_Info Tree
        self.Event_Info.Run_Number             = self.Run_Number
        self.Event_Info.Stream_Tag_Name        = self.Stream_Tag_Name
        self.Event_Info.Stream_Tag_Type        = self.Stream_Tag_Type
        self.Event_Info.Lvl1_ID                = self.Lvl1_ID
        self.Event_Info.Event_Counter_Lvl1_ID  = self.Event_Counter_Lvl1_ID
        self.Event_Info.Event_Counter_Reset_Counter_Lvl1_ID  = self.Event_Counter_Reset_Counter_Lvl1_ID 
        self.Event_Info.Global_ID              = self.Global_ID
        self.Event_Info.Node_ID                = self.Node_ID
        self.Event_Info.Lumiblock              = self.Lumiblock
        self.Event_Info.SuperMasterKey         = self.SuperMasterKey
        self.Event_Info.HLT_PSC_Key            = self.HLT_PSC_Key
        self.Event_Info.HLT_Action             = self.HLT_Action
        self.Event_Info.HLT_Reason             = self.HLT_Reason
        self.Event_Info.HLT_Internal           = self.HLT_Internal
        self.Event_Info.HLT_Decision           = self.HLT_Decision
        self.Event_Info.HLT_Application        = self.HLT_Application
        self.Event_Info.EF_Overall             = self.EF_Overall
        self.Event_Info.EF_PSC_Result          = self.EF_PSC_Result
        self.Event_Info.EF_Status_Result       = self.EF_Status_Overall
        self.Event_Info.Chain_Name_EF          = self.EF_ErrorChain
        self.Event_Info.Chain_Step_EF          = self.EF_ErrorStep
        self.Event_Info.EventStatusNames       = self.EventStatusNames
        
        self.event_info_tree.Fill()        


    def rootDefinitions(self,dbgStep,inputFile):
        gStyle.SetCanvasColor(0)
        gStyle.SetOptStat(000000)
        gROOT.SetStyle("Plain")
  
        # Create new ROOT Tree to store debug info
        if dbgStep == "_Pre" :
            gROOT.ProcessLine("struct EventInfoTree {\
            Char_t  Code_File[30];\
            Int_t   Run_Number;\
            Char_t  Stream_Tag_Name[80];\
            Char_t  Stream_Tag_Type[80];\
            UInt_t  Lvl1_ID;\
            Int_t   Event_Counter_Lvl1_ID;\
            Int_t   Event_Counter_Reset_Counter_Lvl1_ID;\
            Int_t   Global_ID;\
            Int_t   Lumiblock;\
            Int_t   Node_ID;\
            Int_t   SuperMasterKey;\
            Int_t   HLT_PSC_Key;\
            Char_t  HLT_Action[50];\
            Char_t  HLT_Reason[50];\
            Char_t  HLT_Internal[50];\
            Bool_t  HLT_Decision;\
            Char_t  HLT_Application[50];\
            Char_t  EF_Overall[50];\
            Char_t  EF_PSC_Result[50];\
            Char_t  EF_Status_Result[50];\
            Char_t  Chain_Name_EF[50];\
            Int_t   Chain_Step_EF;\
            Char_t  EventStatusNames[50];\
            };" )
  
        # Bind the ROOT tree with EventInfo class members
        from ROOT import EventInfoTree
        self.Event_Info = EventInfoTree()
        self.event_info_tree = ROOT.TTree('Event_Info' + dbgStep, inputFile)
  
        self.event_info_tree.Branch('Run_Number',       addressof(self.Event_Info, 'Run_Number'),       'run_Number/I')
        self.event_info_tree.Branch('Stream_Tag_Name',  addressof(self.Event_Info, 'Stream_Tag_Name'),  'stream_Tag_Name/C')
        self.event_info_tree.Branch('Stream_Tag_Type',  addressof(self.Event_Info, 'Stream_Tag_Type'),  'stream_Tag_Type/C')
        self.event_info_tree.Branch('Lvl1_ID',                             addressof(self.Event_Info, 'Lvl1_ID'),                               'lvl1_ID/I')
        self.event_info_tree.Branch('Event_Counter_Lvl1_ID',               addressof(self.Event_Info, 'Event_Counter_Lvl1_ID'),                 'event_Counter_Lvl1_ID/I')
        self.event_info_tree.Branch('Event_Counter_Reset_Counter_Lvl1_ID', addressof(self.Event_Info, 'Event_Counter_Reset_Counter_Lvl1_ID'),   'event_Counter_Reset_Counter_Lvl1_ID/I')
        self.event_info_tree.Branch('Global_ID',        addressof(self.Event_Info, 'Global_ID'),  'global_ID/I')
        self.event_info_tree.Branch('Lumiblock',        addressof(self.Event_Info, 'Lumiblock'),  'lumiblock/I')
        self.event_info_tree.Branch('Node_ID',          addressof(self.Event_Info, 'Node_ID'),  'node_ID/I')
        self.event_info_tree.Branch('SuperMasterKey',   addressof(self.Event_Info, 'SuperMasterKey'),    'supperMasterKey/I')
        self.event_info_tree.Branch('HLT_PSC_Key',      addressof(self.Event_Info, 'HLT_PSC_Key'),       'hLT_PSC_Key/I')
        self.event_info_tree.Branch('HLT_Action',       addressof(self.Event_Info, 'HLT_Action'),     'hLT_Action/C')
        self.event_info_tree.Branch('HLT_Reason',       addressof(self.Event_Info, 'HLT_Reason'),     'hLT_Reason/C')
        self.event_info_tree.Branch('HLT_Internal',     addressof(self.Event_Info, 'HLT_Internal'),   'hLT_Internal/C')
        self.event_info_tree.Branch('HLT_Decision',     addressof(self.Event_Info, 'HLT_Decision'),    'hLT_Decision/B')
        self.event_info_tree.Branch('HLT_Application',  addressof(self.Event_Info, 'HLT_Application'),   'HLT_Application/C')
        self.event_info_tree.Branch('EF_Overall',       addressof(self.Event_Info, 'EF_Overall'),    'eF_Overall/C')
        self.event_info_tree.Branch('EF_PSC_Result',    addressof(self.Event_Info, 'EF_PSC_Result'), 'eF_PSC_Result/C')
        self.event_info_tree.Branch('EF_Status_Result', addressof(self.Event_Info, 'EF_Status_Result'),  'eF_Status_Result/C')
        self.event_info_tree.Branch('Chain_Name_EF',    addressof(self.Event_Info, 'Chain_Name_EF'),     'chain_Name_EF/C')
        self.event_info_tree.Branch('Chain_Step_EF',    addressof(self.Event_Info, 'Chain_Step_EF'),     'chain_Step_EF/I')  
        self.event_info_tree.Branch('EventStatusNames', addressof(self.Event_Info, 'EventStatusNames'),  'eventStatusNames/C')

        # Setup vector data
        self.L1_Triggered_AV  = ROOT.std.vector( ROOT.std.string )()
        self.L1_Triggered_BP  = ROOT.std.vector( ROOT.std.string )()
        self.L1_Triggered_IDs = ROOT.std.vector( int )()
        self.HLT_Triggered_Names     = ROOT.std.vector( ROOT.std.string )()
        self.HLT_Triggered_IDs = ROOT.std.vector( int )()

        self.event_info_tree._L1_Triggered_BP     = self.L1_Triggered_BP
        self.event_info_tree._L1_Triggered_AV     = self.L1_Triggered_AV
        self.event_info_tree._L1_Triggered_IDs    = self.L1_Triggered_IDs
        self.event_info_tree._HLT_Triggered_Names = self.HLT_Triggered_Names
        self.event_info_tree._HLT_Triggered_IDs   = self.HLT_Triggered_IDs

        self.event_info_tree.Branch('L1_Triggered_BP',  self.L1_Triggered_BP)
        self.event_info_tree.Branch('L1_Triggered_AV',  self.L1_Triggered_AV)
        self.event_info_tree.Branch('L1_Triggered_IDs', self.L1_Triggered_IDs)
        self.event_info_tree.Branch('HLT_Triggered_Names', self.HLT_Triggered_Names)
        self.event_info_tree.Branch('HLT_Triggered_IDs', self.HLT_Triggered_IDs)
        
