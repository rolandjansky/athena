#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#### This is the dbgEventInfo class for the Debug Stream event analysis

import logging
msg = logging.getLogger("PyJobTransforms." + __name__)

import eformat
import sys
import os
import commands
from TrigTransform.dbgHltResult import * 
#hltResult
import cppyy

from PyUtils import RootUtils
ROOT = RootUtils.import_root()
import ROOT
from ROOT import *

class dbgEventInfo:

    def __init__(self,dbgStep="_Default",inputFile=""):
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
        self.cnt                                   = 0
        self.HLT_res = hltResult()
        self.root_definitions(dbgStep,inputFile)



    ### ******************************* ###
    #Event Specific Status
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
        

    ### ************************************ ###
    #EF Status Names:#
     ##First word indicates if fragmant is valid##
    ##Second word seems to be always 0##
    ##Third word contains definition of errors##
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
        
    ### ************************************ ###  
    #these are copied from TrigSteeringEvent/Enums.h#
        self.actionNames=['CONTINUE','ABORT_CHAIN','ABORT_EVENT','ABORT_JOB']
        self.reasonNames=['UNKNOWN=0','MISSING_FEATURE','GAUDI_EXCEPTION',
                          'EFORMAT_EXCEPTION','STD_EXCEPTION',
                          'UNKNOWN_EXCEPTION','NAV_ERROR',
                          'MISSING_ROD','CORRUPTED_ROD',
                          'TIMEOUT','BAD_JOB_SETUP',
                          'USERDEF_1','USERDEF_2',
                          'USERDEF_3','USERDEF_4','USERDEF_5']
        self.internalNames=['UNKNOWN=0','NO_LVL1_ITEMS',
                            'NO_LVL2_CHAINS','NO_LVL1_RESULT',
                            'WRONG_HLT_RESULT','NO_HLT_RESULT',
                            'ALGO_ERROR','TIMEOUT','BAD_JOB_SETUP']
        

    def event_count(self,event):
        self.cnt += 1
        msg.info('Running debug_stream analysis on event :{0}'.format(self.cnt))


    def event_info(self,event, L1Chain_Names, HLTChain_Names ):    
        #Main function in this class: calls most methods in right order 
        self.Run_Number = event.run_no()
    
        self.L1_Chain_Names = L1Chain_Names
        self.HLT_Chain_Names = HLTChain_Names
        
        all_name_streamtags_of_an_event = ','.join([k.name for k in event.stream_tag()])
        self.Stream_Tag_Name = all_name_streamtags_of_an_event
        
        all_type_streamtags_of_an_event = ','.join([l.type for l in event.stream_tag()])
        self.Stream_Tag_Type = all_type_streamtags_of_an_event
        
        self.Lvl1_ID = event.lvl1_id()
        
        self.Event_Counter_Lvl1_ID = self.Lvl1_ID&0xffffff
        self.Event_Counter_Reset_Counter_Lvl1_ID = (self.Lvl1_ID&0xff000000)>>24
        
        self.Global_ID = event.global_id()
        self.Lumiblock = event.lumi_block()
        
        self.event_status(event)
        
        self.lvl1_info(event, L1Chain_Names)
        
        self.hlt_info(event, HLTChain_Names)

        self.hlt_result(event)     
        
        self.get_name(self.EF_Result,HLTChain_Names)

        self.get_hlt_error(self.HLT_Result)
        
        self.get_ef_error(self.EF_Status_Result)

    def event_status(self,event):
        #Get event specific status
        status_list = []
        j = -1
        for i in range(16,32):
            j+=1
            status = (event.status()[0] >> i ) & 0x1
            if (status==1):
                status_list.append(self.EventSpecificStatus[j])
        self.EventStatusNames = ','.join(str(name) for name in status_list)
        msg.info('Event Status :{0}'.format(self.EventStatusNames))

    def lvl1_info(self,event,L1Chain_Names):
        #Gets LVL1 info for BP AV and AV-TrigIDs and stores is in vectors
        self.L1_Triggered_BP.clear()
        self.L1_Triggered_AV.clear()
        self.L1_Triggered_IDs.clear()

        try :
            lvl1_trigger0 = event.lvl1_trigger_info()[0]
            k=-1
            l=1
            for j in range(24):
                k+=1
                if k==8: ## Count the three different sets of words - that is counter l
                    k=0
                    l+=1
                for i in range(32):
                    L1IDed = ( event.lvl1_trigger_info()[j] >> i) & 0x1
                    id = i+k*32
                #print 'L1 Item ID, WordSet, Name:',id, l, i,k #, L1Chain_Names[id]
                    if (L1IDed):
                        id = i+k*32
                    #print 'L1 Item ID, WordSet, Name:',id, l , L1Chain_Names[id]
                        if (l==1):
                            self.L1_Triggered_BP.push_back(L1Chain_Names[id])
                        if (l==3):
                            self.L1_Triggered_AV.push_back(L1Chain_Names[id])
                            self.L1_Triggered_IDs.push_back(id)
        except IndexError:
            msg.info('Missing LVL1 Triggered ID Info')
        
        list_ids = []
        for ids in range(self.L1_Triggered_IDs.size()):
            list_ids.append(self.L1_Triggered_IDs.at(ids))
        msg.info('LVL1 Triggered ID Chains AV :{0}'.format(list_ids))



    def hlt_info(self,event, HLTChain_Names):
        #Gets HLT info and stores it in vectors
        info = event.event_filter_info()
        self.HLT_Triggered_Names.clear()
        self.HLT_Triggered_IDs.clear()
        cnt=0;
        for word in info:
            for i in range(32):
                if word&(1<<i):
                    self.HLT_Triggered_IDs.push_back(cnt)
                    self.HLT_Triggered_Names.push_back(HLTChain_Names[cnt])
                cnt+=1
        
        list_ids = []
        for ids in range(self.HLT_Triggered_IDs.size()):
            list_ids.append(self.HLT_Triggered_IDs.at(ids))
        msg.info('HLT Triggered ID Chains  :{0}'.format(list_ids))
        
    def get_chain(self,counter, s):
        #Prints chains and their information
        ch = cppyy.makeClass('HLT::Chain')(s)
        #ch.deserialize(s)
        print ".... chain %-3d : %s Counter:%-4d Passed: %d (Raw:%d Prescaled: %d PassThrough:%d) Rerun: %d LastStep: %d Err: %s"\
            % ( counter, self.HLT_Chain_Names[ch.getChainCounter()], ch.getChainCounter(), ch.chainPassed(), ch.chainPassedRaw(), ch.isPrescaled(), ch.isPassedThrough(), ch.isResurrected(), ch.getChainStep(), ch.getErrorCode().str())

    def get_all_chains(self,blob):
        print "... chains:"
        for i in range(len(blob)):
            self.get_chain(i, blob[i])

    def hlt_result(self,event):
        #Get the hlt result,status,decision etc if hlt_result exist
        HLTResult = None
        HLTDecision = False
        EFResult = None
        EFStatus = None
        found=False
        ef_event_count = 0 
        for rob  in event.children():
            if rob.source_id().subdetector_id() == eformat.helper.SubDetector.TDAQ_EVENT_FILTER:
                print '..', rob.__class__.__name__, 'source:', rob.source_id(), 'source ID: 0x%x' % rob.source_id().code(), 'size:', rob.fragment_size_word()*4
                try:
                    
                    self.HLT_res.load(rob)
                    version = self.HLT_res.getHLTResultClassVersion()
                    l1id = self.HLT_res.getLvl1Id()
                    acc = self.HLT_res.isAccepted()
                    status = self.HLT_res.getHLTStatus().str()
                    pt = self.HLT_res.isPassThrough()
                    cnvstatus = self.HLT_res.getLvlConverterStatus().str()
                    level = self.HLT_res.getHLTLevel()
                    nosigs = self.HLT_res.getNumOfSatisfiedSigs()
                    bad = self.HLT_res.isCreatedOutsideHLT()
                    trunc = self.HLT_res.isHLTResultTruncated()
                    msg.info("version   :%s" % version)
                    msg.info("l1id      :%s" % l1id)
                    msg.info("accepted  :%s" % acc)
                    msg.info("status    :%s" % status)
                    msg.info("passthr   :%s" % pt)
                    msg.info("cnvstatus :%s" % cnvstatus)
                    msg.info("level     :%s" % level)
                    msg.info("nosigs    :%s" % nosigs)
                    msg.info("bad       :%s" % bad)
                    msg.info("trunc     :%s" % trunc)
                    chains_data = list(self.HLT_res.getChainResult())
                    nchains = chains_data[0] if chains_data else 0
                    nav_data = list(self.HLT_res.getNavigationResult())
                    nnav = len(nav_data)
                    nver = nav_data[0] if nav_data else 0
                    msg.info("nchains   :%s" % nchains)
                    msg.info("nver      :%s" % nver)
                    msg.info("chains    :%s" % chains_data)
                    self.Node_ID = rob.source_id().module_id()
                    EFStatus = rob.rod_status()
                    data = rob.rod_data()
                    if len(data)>=14:
                        EFResult=data[0:14]
                        ####### uncomment next line  for debugging 
                        #print_HLTResult( self.HLT_res)
                    if EFResult:
                        HLTResult=EFResult[4]
                        
                    self.get_all_chains(chains_data[1:])
                    self.HLT_Decision = self.HLT_res.isAccepted()
                    self.HLT_PSC_Key = self.HLT_res.getConfigPrescalesKey()                    
                    self.SuperMasterKey = self.HLT_res.getConfigSuperMasterKey()                    
                    self.HLT_Application = self.HLT_res.appName()
                    found = True
                    ef_event_count += 1
                    
                except Exception, ex:
                    print '... **** problems in analyzing payload', ex
                    print '... **** raw data[:10]', list(rob.rod_data())[:10]
                    print ".. EOF HLTResult for EF"
        if not found:
            msg.info("No HLT Result for EF")

        self.EF_Result = EFResult
        self.EF_Status_Result = EFStatus
        self.HLT_Result = HLTResult
        if self.EF_Status_Result :
            self.EF_Status_Overall = 'EF_Status_Found'
        else:
            self.EF_Status_Overall = 'No_EF_Status'

        msg.info("EF_Status_Overall :%s" % self.EF_Status_Overall)
        msg.info("HLT_Result :%s" % self.HLT_Result)
        msg.info("HLT Decision :%s" % self.HLT_Decision)
        msg.info("HLT_Configuration  smk:{0}   hltpskey :{1}".format(self.SuperMasterKey, self.HLT_PSC_Key))
        msg.info("Application name :%s" % self.HLT_Application)

        

    def get_name(self,result,HLTChain_Name):    
        #Get the name of the HLT chain that caused error if exist
        ChainName = ""
        ErrorStep = 0

        if (result and HLTChain_Name):
            ChainName = HLTChain_Name[int(result[8])]
            if (ChainName!=0):
                ErrorStep = result[9]
            else:
                ChainName = "No Error in Chain"
        else:
            ChainName = "No HLT Result"
        
        self.EF_ErrorChain = ChainName
        self.EF_ErrorStep = ErrorStep


    def get_hlt_error(self,HLT_Result):
        #Get the code of HLT errors if hlt_result exist
        if HLT_Result is not None:
            reasonCode=0
            actionCode=0
            internalCode=0
            try:
                reasonCode=HLT_Result&0xf
                actionCode=(HLT_Result&0x30)>>4
                internalCode=(HLT_Result&0xffff00)>>8
                self.HLT_Action = self.actionNames[actionCode]
                self.HLT_Reason = self.reasonNames[reasonCode]
                self.HLT_Internal = self.internalNames[internalCode]
                msg.debug("HLT_Result actionCode   :{0}".format(actionCode))
                msg.debug("HLT_Result reasonCode   :{0}".format(reasonCode))
                msg.debug("HLT_Result internalCode :{0}".format(internalCode))
                
            except IndexError:
                self.HLT_Action   = 'Error_In_Processing'
                self.HLT_Reason   = 'Error_In_Processing'
                self.HLT_Internal = 'Error_In_Processing'

        else:
            self.HLT_Action   ='No HLT Result'
            self.HLT_Reason   ='No HLT Result'
            self.HLT_Internal ='No HLT Result'

        msg.info("HLT Action :{0}  Reason :{1}  Internal:{2}".format(self.HLT_Action, self.HLT_Reason, self.HLT_Internal))


    def get_ef_error(self,EF_Result):
        #Get overall EF error status
        if EF_Result:
            if EF_Result[0]==0:
                self.EF_Overall = 'OK'
            else:
                self.EF_Overall = 'ERROR'
        
                try:
                    self.EF_PSC_Result = self.EF_Status_Word[EF_Result[2]]
                    msg.info('******** EF_Status_Word[EF_Result[2]]: '.format(self.EF_Status_Word[EF_Result[2]]))
                except IndexError:
                    self.EF_PSC_Result = 'Error_In_Processing'

        else:
            self.EF_Overall ='No EF Status'
            self.EF_PSC_Result ='No EF Status'


    def fillTree(self):
        #Fill Event_Info Tree
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


    def root_definitions(self,dbgStep,inputFile):
        gStyle.SetCanvasColor(0)
        gStyle.SetOptStat(000000)
        gROOT.SetStyle("Plain")
  
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
            };" );

        self.L1_Triggered_AV  = ROOT.std.vector( ROOT.std.string )()
        self.L1_Triggered_BP  = ROOT.std.vector( ROOT.std.string )()
        self.L1_Triggered_IDs = ROOT.std.vector( int )()
        self.HLT_Triggered_Names     = ROOT.std.vector( ROOT.std.string )()
        self.HLT_Triggered_IDs = ROOT.std.vector( int )()
  
        from ROOT import EventInfoTree
        self.Event_Info = EventInfoTree()
        self.event_info_tree = TTree('Event_Info'+dbgStep, inputFile)

        self.event_info_tree._L1_Triggered_BP     = self.L1_Triggered_BP
        self.event_info_tree._L1_Triggered_AV     = self.L1_Triggered_AV
        self.event_info_tree._L1_Triggered_IDs    = self.L1_Triggered_IDs
        self.event_info_tree._HLT_Triggered_Names = self.HLT_Triggered_Names
        self.event_info_tree._HLT_Triggered_IDs   = self.HLT_Triggered_IDs
  
        self.event_info_tree.Branch('Run_Number',       AddressOf(self.Event_Info,'Run_Number'),       'run_Number/I')
        self.event_info_tree.Branch('Stream_Tag_Name',  AddressOf(self.Event_Info,'Stream_Tag_Name'),  'stream_Tag_Name/C')
        self.event_info_tree.Branch('Stream_Tag_Type',  AddressOf(self.Event_Info,'Stream_Tag_Type'),  'stream_Tag_Type/C')
        self.event_info_tree.Branch('Lvl1_ID',                             AddressOf(self.Event_Info,'Lvl1_ID'),                               'lvl1_ID/I')
        self.event_info_tree.Branch('Event_Counter_Lvl1_ID',               AddressOf(self.Event_Info,'Event_Counter_Lvl1_ID'),                 'event_Counter_Lvl1_ID/I')
        self.event_info_tree.Branch('Event_Counter_Reset_Counter_Lvl1_ID', AddressOf(self.Event_Info,'Event_Counter_Reset_Counter_Lvl1_ID'),   'event_Counter_Reset_Counter_Lvl1_ID/I')
        self.event_info_tree.Branch('Global_ID',        AddressOf(self.Event_Info,'Global_ID'),  'global_ID/I')
        self.event_info_tree.Branch('Lumiblock',        AddressOf(self.Event_Info,'Lumiblock'),  'lumiblock/I')
        self.event_info_tree.Branch('Node_ID',          AddressOf(self.Event_Info,'Node_ID'),  'node_ID/I')
        self.event_info_tree.Branch('SuperMasterKey',   AddressOf(self.Event_Info,'SuperMasterKey'),    'supperMasterKey/I')
        self.event_info_tree.Branch('HLT_PSC_Key',      AddressOf(self.Event_Info,'HLT_PSC_Key'),       'hLT_PSC_Key/I')
        self.event_info_tree.Branch('HLT_Action',       AddressOf(self.Event_Info,'HLT_Action'),     'hLT_Action/C')
        self.event_info_tree.Branch('HLT_Reason',       AddressOf(self.Event_Info,'HLT_Reason'),     'hLT_Reason/C')
        self.event_info_tree.Branch('HLT_Internal',     AddressOf(self.Event_Info,'HLT_Internal'),   'hLT_Internal/C')
        self.event_info_tree.Branch('HLT_Decision',     AddressOf(self.Event_Info,'HLT_Decision'),    'hLT_Decision/B')
        self.event_info_tree.Branch('HLT_Application',  AddressOf(self.Event_Info,'HLT_Application'),   'HLT_Application/C')
        self.event_info_tree.Branch('EF_Overall',       AddressOf(self.Event_Info,'EF_Overall'),    'eF_Overall/C')
        self.event_info_tree.Branch('EF_PSC_Result',    AddressOf(self.Event_Info,'EF_PSC_Result'), 'eF_PSC_Result/C')
        self.event_info_tree.Branch('EF_Status_Result', AddressOf(self.Event_Info,'EF_Status_Result'),  'eF_Status_Result/C')
        self.event_info_tree.Branch('Chain_Name_EF',    AddressOf(self.Event_Info,'Chain_Name_EF'),     'chain_Name_EF/C')
        self.event_info_tree.Branch('Chain_Step_EF',    AddressOf(self.Event_Info,'Chain_Step_EF'),     'chain_Step_EF/I')  
        self.event_info_tree.Branch('EventStatusNames', AddressOf(self.Event_Info,'EventStatusNames'),  'eventStatusNames/C')

        self.event_info_tree.Branch('L1_Triggered_BP',  self.L1_Triggered_BP)
        self.event_info_tree.Branch('L1_Triggered_AV',  self.L1_Triggered_AV)
        self.event_info_tree.Branch('L1_Triggered_IDs', self.L1_Triggered_IDs)
        self.event_info_tree.Branch('HLT_Triggered_Names', self.HLT_Triggered_Names)
        self.event_info_tree.Branch('HLT_Triggered_IDs', self.HLT_Triggered_IDs)
        
