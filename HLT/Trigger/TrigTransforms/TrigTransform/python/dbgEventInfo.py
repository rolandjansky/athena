#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#### This is the dbgEventInfo class for the Debug Stream event analysis

import logging
msg = logging.getLogger("PyJobTransforms." + __name__)

import eformat
import sys
import os
import commands
from TrigTransform.dbgHltResult import hltResult
import PyCintex

from PyUtils import RootUtils
ROOT = RootUtils.import_root()
import ROOT
from ROOT import *

class dbgEventInfo:

    def __init__(self,dbgStep="_Default"):
        self.Run_Number                            = 0
        self.Stream_Tag_Name                       = 'None' 
        self.Stream_Tag_Type                       = 'None' 
        self.Lvl1_ID                               = 0
        self.Event_Counter_Lvl1_ID                 = 0
        self.Event_Counter_Reset_Counter_Lvl1_ID   = 0
        self.Global_ID                             = 0
        self.Lumiblock                             = 0
        self.Node_ID                               = 0
        self.EF_PSC_Result                         = 'None'
        self.EF_Status                             = 'None'
        self.EF_Result                             = []
        self.EF_Overall                            = 'none'
        self.EF_ErrorChain                         = 'None'
        self.HLT_Action                            = 'None' 
        self.HLT_Reason                            = 'None' 
        self.HLT_Internal                          = 'None' 
        self.HLT_Overall                           = 'None' 
        self.HLT_Result                            = 'None' 
        self.HLT_PSC_Result                        = 'None' 
        self.HLT_Status_Result                     = [] 
        self.HLT_Status_Overall                    = 'None' 
        self.HLT_ErrorChain                        = 'None' 
        self.HLT_ErrorStep                         = -1 
        self.HLTPassedChains                       = []
        self.SuperMasterKey                        = 0
        self.HLT_Result_list                       = []
        self.L1_Chain_Names                        = []
        self.HLT_Chain_Names                       = []
        self.HLT_res = hltResult()
        self.root_definitions(dbgStep)


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
                                    'Reserved']
        
    ### ************************************ ###
    #L2 Status Names:#
#        self.HLT_L2_Status_Names = ['Normal Lvl2',
#                                    'Dummy Lvl2',
#                                    'Normal Truncated',
#                                    'Dummy Truncated',
#                                    'New Status1',
#                                    'New Status2']
#        self.PSC_L2_Status_Names = ['Error_Unclassified',
#                                    'NO_L1_Result',
#                                    'SG_Clear_Failed',
#                                    'No_Event_Info',
#                                    'No_L2_Found',
#                                    'No_L2_Retrieved',
#                                    'Invalid_CPT_Result',
#                                    'NEW?1',
#                                    'NEW?2']
#        

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
        
        self.cnt = 0
        
        self.HLT_Chain_DBInfo = []
        self.L1_Chain_DBInfo  = []
        self.L1TriggerBP      = []
        self.L1TriggerAV      = [] 
        self.L1TriggerAVIDs   = []
        self.EventStatusNames = []


    def event_count(self,event):
        self.cnt += 1
        msg.info('Running debug_stream analysis on event :{0}'.format(self.cnt))


    def chainsPassed(self,info):
        chains=[]
        cnt=0;
        for word in info:
            for i in range(32):
                if word&(1<<i):
                    chains+=[cnt]
                cnt+=1
        return chains
  
      
    def event_info(self,event, L1Chain_Names, HLTChain_Names ):    

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
        
        self.EventStatus(event)
        
        self.Lvl1_Info(event, L1Chain_Names)
        
        self.HLTPassedChains=self.chainsPassed(event.event_filter_info())

        self.hlt_result(event)     
        
        #    self.L2_ErrorChain = self.getName(self.L2_Result,HLTChain_Names,"L2")
        #self.EF_ErrorChain = self.getName(self.EF_Result,HLTChain_Names,"HLT")
        self.HLT_ErrorChain = self.getName(self.EF_Result,HLTChain_Names,"HLT")
        self.Get_HLT_Error(self.EF_Result)
        #    self.Get_L2_Error(self.L2_Status_Result)
        #    self.Get_EF_Error(self.EF_Status_Result)
        

    def EventStatus(self,event):
        status_list = []
        j = -1
        for i in range(16,32):
            j+=1
            status = (event.status()[0] >> i ) & 0x1
            if (status==1):
                status_list.append(self.EventSpecificStatus[j])
        
        self.EventStatusNames = ','.join(str(name) for name in status_list)
        msg.info('Event Status :{0}'.format(self.EventStatusNames))

    def Lvl1_Info(self,event,L1Chain_Names):
        #This function extracts LVL1 info for BP AV and AV-TrigIDs from the event
        self.L1TriggerBP    = []
        self.L1TriggerAV    = []
        self.L1TriggerAVIDs = []
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
                        self.L1TriggerBP.append(L1Chain_Names[id])
                    if (l==3):
                        self.L1TriggerAV.append(L1Chain_Names[id])
                        self.L1TriggerAVIDs.append(id)
        msg.info('LVL1 Trigger IDs AV :{0}'.format(self.L1TriggerAVIDs))

    def get_chain(self,counter, s):
        ch = PyCintex.makeClass('HLT::Chain')(s)
        #ch.deserialize(s)
        print ".... chain %-3d : %s Counter:%-4d Passed: %d (Raw:%d Prescaled: %d PassThrough:%d) Rerun: %d LastStep: %d Err: %s"\
            % ( counter, self.HLT_Chain_Names[ch.getChainCounter()], ch.getChainCounter(), ch.chainPassed(), ch.chainPassedRaw(), ch.isPrescaled(), ch.isPassedThrough(), ch.isResurrected(), ch.getChainStep(), ch.getErrorCode().str())

    def get_all_chains(self,blob):
        print "... chains:"
        for i in range(len(blob)):
            self.get_chain(i, blob[i])

    def hlt_result(self,event):
        EFResult = []
        HLTResult = None
        HLTStatus = None
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
                    print "version   : ",version
                    print "l1id      : ",l1id
                    print "accepted  : ",acc
                    print "status    : ",status
                    print "passthr   : ",pt
                    print "cnvstatus : ",cnvstatus
                    print "level     : ",level
                    print "nosigs    : ",nosigs
                    print "bad       : ",bad
                    print "trunc     : ",trunc
                    chains_data = list(self.HLT_res.getChainResult())
                    nchains = chains_data[0] if chains_data else 0
                    nav_data = list(self.HLT_res.getNavigationResult())
                    nnav = len(nav_data)
                    nver = nav_data[0] if nav_data else 0
                    print "nchains   : ",nchains
                    print "nver      : ",nver
                    print "chains    : ",chains_data

                    self.Node_ID = rob.source_id().module_id()
                    HLTStatus =rob.rod_status() #list(rob.status())
                    data = rob.rod_data()
                    if len(data)>=14:
                        #print data[0:14]
                        EFResult=data[0:14]
                    if EFResult:
                        HLTResult=EFResult[4]
                        
                    self.get_all_chains(chains_data[1:])

                    found = True
                    ef_event_count += 1
                    
                except Exception, ex:
                    print '... **** problems in analyzing payload', ex
                    print '... **** raw data[:10]', list(rob.rod_data())[:10]
                    print ".. EOF HLTResult for EF"
        if not found:
            msg.info("No HLTResult for EF")

        self.EF_Result = EFResult
        self.HLT_Result =  HLTResult
        self.HLT_Status_Result   = HLTStatus
        if self.HLT_Status_Result:
            self.HLT_Status_Overall = 'HLT_Status_Found'
        else:
            self.HLT_Status_Overall = 'No_HLT_Status'

        msg.info("HLT_Status_Overall :%s" % self.HLT_Status_Overall)
        msg.info("HLT_Result :%s" % self.HLT_Result)
        msg.info("EF_Result :%s" % self.EF_Result)




    def getName(self,result,HLTChain_Name,whatToLookFor):    
        #Get the name of the HLT chain if exist
        if (result):
            self.SuperMasterKey =  result[12]
            ChainName = HLTChain_Name[int(result[8])]
            if (ChainName!=0):
                if (whatToLookFor == 'L2'):
                    self.L2_ErrorStep = result[9]
                if (whatToLookFor == 'EF'):
                    self.EF_ErrorStep = result[9]
                if (whatToLookFor == 'HLT'):
                    self.HLT_ErrorStep = result[9]
            else:
                ChainName = "No Error in Chain"
                
        else:
            ChainName = "No %s Result" %whatToLookFor

        return ChainName


    def Get_HLT_Error(self,result):
        print "\nHLT RESULT : ", result
        if len(result)==0:
            reasonCode=0
            actionCode=0
            internalCode=0
            reason = 0 
            
        elif result:    
            HLT_Result = result[4]
            try:
                reasonCode=HLT_Result&0xf
                actionCode=(HLT_Result&0x30)>>4
                internalCode=(HLT_Result&0xffff00)>>8
                
                self.HLT_Action = self.actionNames[actionCode]
                self.HLT_Reason = self.reasonNames[reasonCode]
                self.HLT_Internal = self.internalNames[internalCode]
                
            except IndexError:
                self.HLT_Action   = 'Error_In_Processing'
                self.HLT_Reason   = 'Error_In_Processing'
                self.HLT_Internal = 'Error_In_Processing'
                
        else:
            self.HLT_Action   ='No HLT Result'
            self.HLT_Reason   ='No HLT Result'
            self.HLT_Internal ='No HLT Result'

    def Get_EF_Error(self,EF_Result):
    
        if EF_Result:

            if EF_Result[0]==0:
                self.EF_Overall = 'OK'
            else:
                self.EF_Overall = 'ERROR'
        
            try:
                self.EF_Status = self.EF_Status_Word[EF_Result[2]]
      
            except IndexError:
                self.EF_PSC_Result = 'Error_In_Processing'

        else:

            self.EF_Overall ='No EF Status'
            self.EF_PSC_Result ='No EF Status'


#    def fillTree(self,Event_Info,event_info_tree):
    def fillTree(self):   
        self.Event_Info.Run_Number             = self.Run_Number
        self.Event_Info.Stream_Tag_Name        = self.Stream_Tag_Name
        self.Event_Info.Stream_Tag_Type        = self.Stream_Tag_Type
        self.Event_Info.Lvl1_ID                = self.Lvl1_ID
        self.Event_Info.Event_Counter_Lvl1_ID  = self.Event_Counter_Lvl1_ID
        self.Event_Info.Event_Counter_Reset_Counter_Lvl1_ID  = self.Event_Counter_Reset_Counter_Lvl1_ID 
        self.Event_Info.Global_ID              = self.Global_ID
        self.Event_Info.Node_ID                = self.Node_ID
        self.Event_Info.Lumiblock              = self.Lumiblock
        self.Event_Info.HLT_Action             = self.HLT_Action
        self.Event_Info.HLT_Reason             = self.HLT_Reason
        self.Event_Info.HLT_Internal           = self.HLT_Internal
#        self.Event_Info.L2_Overall             = self.L2_Overall
#        self.Event_Info.L2_HLT_Result          = self.L2_HLT_Result
#        self.Event_Info.L2_PSC_Result          = self.L2_PSC_Result
        self.Event_Info.EF_Overall             = self.EF_Overall
        self.Event_Info.EF_PSC_Result          = self.EF_PSC_Result
#        self.Event_Info.L2_Status_Result       = self.L2_Status_Overall
#        self.Event_Info.EF_Status_Result       = self.EF_Status_Overall
#        self.Event_Info.Chain_Name_L2          = self.L2_ErrorChain
#        self.Event_Info.Chain_Name_EF          = self.EF_ErrorChain
        self.Event_Info.Chain_Name_HLT          = self.HLT_ErrorChain
#        self.Event_Info.Chain_Step_L2          = self.L2_ErrorStep
#        self.Event_Info.Chain_Step_EF          = self.EF_ErrorStep
        self.Event_Info.Chain_Step_HLT          = self.HLT_ErrorStep
        
        self.Event_Info.EventStatusNames       = self.EventStatusNames
        self.event_info_tree.Fill()        


    def root_definitions(self,dbgStep):

        gROOT.Reset()
        gStyle.SetCanvasColor(0)
        gStyle.SetOptStat(000000)
        gROOT.SetStyle("Plain")
  
  
        gROOT.ProcessLine(
            "struct EventInfoTree {\
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
            Char_t  HLT_L2_Action[50];\
            Char_t  HLT_L2_Reason[50];\
            Char_t  HLT_L2_Internal[50];\
            Char_t  HLT_EF_Action[50];\
            Char_t  HLT_EF_Reason[50];\
            Char_t  HLT_EF_Internal[50];\
            Char_t  L2_Overall[50];\
            Char_t  L2_HLT_Result[50];\
            Char_t  L2_PSC_Result[50];\
            Char_t  EF_Overall[50];\
            Char_t  EF_PSC_Result[50];\
            Char_t  L2_Status_Result[50];\
            Char_t  EF_Status_Result[50];\
            Char_t  Chain_Name_L2[50];\
            Char_t  Chain_Name_HLT[50];\
            Int_t   Chain_Step_L2;\
            Int_t   Chain_Step_HLT;\
            Char_t  EventStatusNames[50];\
            };" );
  

        self.L1Items    = ROOT.std.vector( ROOT.std.string )()
        self.L1ItemsBP  = ROOT.std.vector( ROOT.std.string )()
        self.L1ItemsIDs = ROOT.std.vector( int )()
  
        self.HLTTriggered   = ROOT.std.vector( ROOT.std.string )()
        self.HLTTriggeredID = ROOT.std.vector( int )()
  
  
        from ROOT import EventInfoTree
        self.Event_Info = EventInfoTree()

        self.event_info_tree = TTree('Event_Info'+dbgStep, 'Event Info '+dbgStep)
        self.event_info_tree._L1Items     = self.L1Items
        self.event_info_tree._L1ItemsBP   = self.L1ItemsBP
        self.event_info_tree._L1ItemsIDs  = self.L1ItemsIDs
        self.event_info_tree._HLTTriggered   = self.HLTTriggered
        self.event_info_tree._HLTTriggeredID = self.HLTTriggeredID
  
        self.event_info_tree.Branch('Run_Number',      AddressOf(self.Event_Info,'Run_Number'),       'run_Number/I')
        self.event_info_tree.Branch('Stream_Tag_Name', AddressOf(self.Event_Info,'Stream_Tag_Name'),  'stream_Tag_Name/C')
        self.event_info_tree.Branch('Stream_Tag_Type', AddressOf(self.Event_Info,'Stream_Tag_Type'),  'stream_Tag_Type/C')
        
        self.event_info_tree.Branch('Lvl1_ID',                             AddressOf(self.Event_Info,'Lvl1_ID'),                               'lvl1_ID/I')
        self.event_info_tree.Branch('Event_Counter_Lvl1_ID',               AddressOf(self.Event_Info,'Event_Counter_Lvl1_ID'),                 'event_Counter_Lvl1_ID/I')
        self.event_info_tree.Branch('Event_Counter_Reset_Counter_Lvl1_ID', AddressOf(self.Event_Info,'Event_Counter_Reset_Counter_Lvl1_ID'),   'event_Counter_Reset_Counter_Lvl1_ID/I')
        self.event_info_tree.Branch('Global_ID',    AddressOf(self.Event_Info,'Global_ID'),  'global_ID/I')
        self.event_info_tree.Branch('Lumiblock',    AddressOf(self.Event_Info,'Lumiblock'),  'lumiblock/I')
        self.event_info_tree.Branch('Node_ID',    AddressOf(self.Event_Info,'Node_ID'),  'node_ID/I')
        
        self.event_info_tree.Branch('HLT_L2_Action',    AddressOf(self.Event_Info,'HLT_L2_Action'),     'hLT_L2_Action/C')
        self.event_info_tree.Branch('HLT_L2_Reason',    AddressOf(self.Event_Info,'HLT_L2_Reason'),     'hLT_L2_Reason/C')
        self.event_info_tree.Branch('HLT_L2_Internal',  AddressOf(self.Event_Info,'HLT_L2_Internal'),   'hLT_L2_Internal/C')
        self.event_info_tree.Branch('HLT_EF_Action',    AddressOf(self.Event_Info,'HLT_EF_Action'),     'hLT_EF_Action/C')
        self.event_info_tree.Branch('HLT_EF_Reason',    AddressOf(self.Event_Info,'HLT_EF_Reason'),     'hLT_EF_Reason/C')
        self.event_info_tree.Branch('HLT_EF_Internal',  AddressOf(self.Event_Info,'HLT_EF_Internal'),   'hLT_EF_Internal/C')

        self.event_info_tree.Branch('L2_Overall',            AddressOf(self.Event_Info,'L2_Overall'),      'l2_Overall/C')
        self.event_info_tree.Branch('L2_HLT_Result',         AddressOf(self.Event_Info,'L2_HLT_Result'),   'l2_HLT_Result/C')
        self.event_info_tree.Branch('L2_PSC_Result',         AddressOf(self.Event_Info,'L2_PSC_Result'),   'l2_PSC_Result/C')

        self.event_info_tree.Branch('EF_Overall',       AddressOf(self.Event_Info,'EF_Overall'),    'eF_Overall/C')
        self.event_info_tree.Branch('EF_PSC_Result',    AddressOf(self.Event_Info,'EF_PSC_Result'), 'eF_PSC_Result/C')

        self.event_info_tree.Branch('L2_Status_Result',   AddressOf(self.Event_Info,'L2_Status_Result'),  'l2_Status_Result/C')
        self.event_info_tree.Branch('EF_Status_Result',   AddressOf(self.Event_Info,'EF_Status_Result'),  'eF_Status_Result/C')
        self.event_info_tree.Branch('Chain_Name_L2',      AddressOf(self.Event_Info,'Chain_Name_L2'),     'chain_Name_L2/C')
        self.event_info_tree.Branch('Chain_Name_HLT',      AddressOf(self.Event_Info,'Chain_Name_HLT'),     'chain_Name_HLT/C')
        self.event_info_tree.Branch('Chain_Step_L2',      AddressOf(self.Event_Info,'Chain_Step_L2'),     'chain_Step_L2/I')
        self.event_info_tree.Branch('Chain_Step_HLT',      AddressOf(self.Event_Info,'Chain_Step_HLT'),     'chain_Step_HLT/I')  
        self.event_info_tree.Branch('EventStatusNames',   AddressOf(self.Event_Info,'EventStatusNames'),  'eventStatusNames/C')
        self.event_info_tree.Branch('L1Items',     self.L1Items)
        self.event_info_tree.Branch('L1ItemsBP',   self.L1ItemsBP)
        self.event_info_tree.Branch('L1ItemsIDs',  self.L1ItemsIDs)
        self.event_info_tree.Branch('HLTTriggered',  self.HLTTriggered)
        self.event_info_tree.Branch('HLTTriggeredID',self.HLTTriggeredID)
        
        self.Items={'L1Items':self.L1Items,'L1ItemsIDs':self.L1ItemsIDs,'L1ItemsBP':self.L1ItemsBP,'HLTTriggered':self.HLTTriggered,'HLTTriggeredID':self.HLTTriggeredID}
                    

#        return event_info_tree, Items, Event_Info


#  def Get_L2_Error(self,L2_Status_Result):
#    
#    if L2_Status_Result:
#
#      if int(L2_Status_Result[0])==0:
#        self.L2_Overall = 'OK'
#      else:
#        self.L2_Overall = 'ERROR'
#
#      try:
#        self.HLT_L2_status = self.HLT_L2_Status_Names[L2_Status_Result[1]]
#        self.PSC_L2_status = self.PSC_L2_Status_Names[L2_Status_Result[2]]
#       
#      except IndexError:      
#        self.L2_HLT_Result = 'Error_In_Processing'
#        self.L2_PSC_Result = 'Error_In_Processing'
#        
#    else:
#      self.L2_Overall = 'No L2 Status'
#      self.L2_HLT_Result = 'No L2 Status'
#      self.L2_PSC_Result =  'No L2 Status'
#
#
#
#
#  def Get_EF_Error(self,EF_Result):
#    
#    if EF_Result:
#
#      if EF_Result[0]==0:
#        self.EF_overall = 'OK'
#      else:
#        self.EF_overall = 'ERROR'
#        
#      try:
#        self.PSC_EF_status = self.EF_Status_Word[EF_Result[2]]
#      
#      except IndexError:
#        self.EF_PSC_Result = 'Error_In_Processing'
#
#    else:
#
#      self.EF_Overall ='No EF Status'
#      self.EF_PSC_Result ='No EF Status'
#
#
#

