# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Photon trigger slice signatures  """

__author__  = 'Moritz Backes & Catrin Bernius'
__version__=""
__doc__="Implementation of Photon Signature"
import collections
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenu.egamma.PhotonDef")

from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenu.menu.HltConfig import L2EFChainDef, mergeRemovingOverlap
from TrigHIHypo.UE import theUEMaker, theFSCellMaker
from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyCopyAllTEAlgo
from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo as DummyRoI
from TriggerMenu.commonUtils.makeCaloSequences import getFullScanCaloSequences
from TrigTRTHighTHitCounter.TrigTRTHighTHitCounterConf import TrigTRTHTHCounter, TrigTRTHTHhypo
##################
#
# L2 & EF Chain configuration
#
##################

# default class, no ID at EF level

class L2EFChain_g(L2EFChainDef):
    
    # Define frequently used instances here as class variables
    [theTRTDataPrep] = TrigInDetSequence("Electron","electron","TRTdata").getSequence()
    hip_trt_seq = theTRTDataPrep+[TrigTRTHTHCounter(), TrigTRTHTHhypo()]
    DummyMergerAlgo = PESA__DummyCopyAllTEAlgo("DummyMergerAlgo")
    fullScanSeqMap = getFullScanCaloSequences()
    def __init__(self, chainDict, seqObj):
        self.L2sequenceList   = []
        self.EFsequenceList   = []
        self.L2signatureList  = []
        self.EFsignatureList  = []
        self.TErenamingDict   = {}

        self.chainPart = chainDict['chainParts']
        self.seqObj = seqObj
        self.ph_sequences = seqObj.get_sequences()

        self.chainL1Item = chainDict['L1item']  
        self.chainPartL1Item = self.chainPart['L1item']

        self.chainCounter = chainDict['chainCounter']       
        self.L2Name = 'L2_'+self.chainPart['chainPartName']
        self.EFName = 'EF_'+self.chainPart['chainPartName']
        self.mult = int(self.chainPart['multiplicity'])
        self.chainName = chainDict['chainName']
        self.chainPartName = self.chainPart['chainPartName']
        self.chainPartNameNoMult = self.chainPartName[1:] if self.mult > 1 else self.chainPartName
        self.chainPartNameNoMult += "_"+self.chainL1Item
        
        self.L2InputTE = self.chainPartL1Item or self.chainL1Item
        # cut of L1_, _EMPTY,..., & multiplicity
        self.L2InputTE = self.L2InputTE.replace("L1_","")
        if 'AFP' in self.L2InputTE:
           self.L2InputTE = self.L2InputTE.replace("AFP_C_","")
        self.L2InputTE = self.L2InputTE.split("_")[0]
        self.L2InputTE = self.L2InputTE[1:] if self.L2InputTE[0].isdigit() else self.L2InputTE

        self.use_v7=False
        #if TriggerFlags.run2Config()=='2017':
        if '_v7' in TriggerFlags.triggerMenuSetup() or '_v8' in TriggerFlags.triggerMenuSetup():
            self.use_v7=True
        
        self.doIsolation=False
        caloiso = [x for x in self.chainPart['isoInfo'] if 'icalo' in x]
        if len(caloiso) > 0:
            self.doIsolation=True
        
        log.debug('Sequences %s ', self.ph_sequences)
                
        # gXX_ID type chains:
        if "hiptrt" in self.chainPart['addInfo']:
            self.setup_gnocut_hiptrt()
        elif self.use_v7:
            self.setup_photon()
        else:
            if 'ringer' in self.chainPart['addInfo']:
               self.setup_gXX_ID_ringer()
            elif self.doIsolation: 
               self.setup_gXX_ID_iso()
            else:
                if 'ion' in self.chainPart['extra']:
                    self.setup_gXX_ID_heavyIon()
                else:
                    self.setup_gXX_ID()
        #else:
        #    log.error('Chain %s could not be assembled' % (self.chainPartName))
        #    return False
            
        L2EFChainDef.__init__(self, 
                              self.chainName, 
                              self.L2Name, 
                              self.chainCounter, 
                              self.chainL1Item, 
                              self.EFName, 
                              self.chainCounter, 
                              self.L2InputTE)
        

    def defineSequences(self):

        for sequence in self.L2sequenceList:
            self.addL2Sequence(*sequence)

        for sequence in self.EFsequenceList:
            self.addEFSequence(*sequence)
                
    def defineSignatures(self):
       
        for signature in self.L2signatureList:
            self.addL2Signature(*signature)

        for signature in self.EFsignatureList:
            self.addEFSignature(*signature)

    def defineTErenaming(self):
        self.TErenamingMap = self.TErenamingDict
    
    def setupFromDict(self, seq_te_dict, seq_dict=None):
        ''' 
        Dictionary completely defines steps, algos, sequences, etc..
        pass a dictionary with sequence key values
        and tuple with te_out and alias for TE renaming
        i.e. seq_te_dict['fastringer']=('L2_e_step1','cl')
        '''
        # Use default sequence map if not passed modified map
        if seq_dict is None:
            seq_dict=self.ph_sequences
        #log.info('%s'%seq_dict)
        te_in=self.L2InputTE
        for step in seq_te_dict:
            level=seq_te_dict[step][0].split('_')[0]
            te_out=seq_te_dict[step][0]
            alias=seq_te_dict[step][-1]
            algos=seq_dict[step]
            #log.info('%s %s %s %s'%(level,te_in,algos,te_out))
            if level == 'L2':
                self.L2sequenceList += [[te_in,algos,te_out]]                     
                self.L2signatureList += [ [[te_out]*self.mult] ]
            elif level == 'EF':
                self.EFsequenceList += [[te_in,algos,te_out]]                     
                self.EFsignatureList += [ [[te_out]*self.mult] ]
            else:
                log.error('Sequence TE dictionary incorrect')
            te_in=[te_out]
            self.TErenamingDict[te_out] = mergeRemovingOverlap(level+'_',self.chainPartNameNoMult+alias)
    
############################### DEFINE GROUPS OF CHAINS HERE ##############################
    def setup_photon(self):
        #log.info('Setup Photon for v7 %s'%self.chainName)
        seq_te_dict = collections.OrderedDict()
        
        # required to preserve some backward compatibility
        # need to hack a bit the fastcalo step to use the T2CaloRinger
        fastcaloseq = self.ph_sequences['fastcalorec']
        log.debug('Photon L2 sequence %s', fastcaloseq)
        fastcaloseq.extend(self.ph_sequences['fastcalohypo'])
        log.debug('Photon L2 sequence %s', fastcaloseq)
        seq_dict = self.ph_sequences
        seq_dict['fastcalo']=fastcaloseq

        seq_te_dict['fastcalo']=('L2_g_step1','_calo')
        seq_te_dict['fastrec']=('L2_g_step2','')
        seq_te_dict['precisecalo']=('EF_g_step1','_calo')
        seq_te_dict['precisecalocalib']=('EF_g_step2','_calocalib')    
        if not self.doIsolation:
            seq_te_dict['preciserec']=('EF_g_step3','')

        self.setupFromDict(seq_te_dict,seq_dict)
        
        if self.doIsolation:
            # full scan topo and merging w/ RoI TEs
            te_in=''
            pos=2
            for step in self.fullScanSeqMap:
                self.EFsequenceList.insert(pos,[[te_in],self.fullScanSeqMap[step],step])
                self.EFsignatureList.insert(pos,[[step]] )
                te_in=step
                pos+=1
            
            self.EFsequenceList.insert(pos,[ ['EF_g_step2',te_in],[self.DummyMergerAlgo],'EF_gCache_step2'])
            self.EFsignatureList.insert(pos,[['EF_gCache_step2']])
            self.EFsequenceList.insert(pos+1,[['EF_gCache_step2'],self.ph_sequences['preciserec'],'EF_g_step3'])
            self.EFsignatureList.insert(pos+1,[['EF_g_step3']*self.mult])

            ########### TE renaming ###########

            self.TErenamingDict['EF_full']=mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_fs')
            self.TErenamingDict['EF_full_cell']=mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_fscalocell')
            self.TErenamingDict['EF_FSTopoClusters']=mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_fscalotopo')
            self.TErenamingDict['EF_FSTopoClustersED']=mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_fscalotopoed')
            self.TErenamingDict['EF_gCache_step2']=mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'ED')
            self.TErenamingDict['EF_g_step3']=mergeRemovingOverlap('EF_', self.chainPartNameNoMult)

        # larpeb
        if 'larpeb' in self.chainPart['addInfo']:
            from TrigDetCalib.TrigDetCalibConfig import LArEFROBListWriter
            self.EFsequenceList += [[['EF_g_step3'], 
                                     [ LArEFROBListWriter('LArEFROBListWriter_' + self.chainName, 
                                         addCTPResult = True, addL2Result = True, addEFResult = True) ],
                                     'EF_g_step4']]
        
        if 'larpeb' in self.chainPart['addInfo']:
            self.EFsignatureList += [ [['EF_g_step4']*self.mult] ]

        if 'larpeb' in self.chainPart['addInfo']:
            self.TErenamingDict ['EF_g_step4']=  mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_larpeb')

    def setup_gXX_ID(self):
        
        seq_te_dict = collections.OrderedDict()
        seq_te_dict['fastcalo']=('L2_g_step1','_calo')
        seq_te_dict['fastrec']=('L2_g_step2','')
        seq_te_dict['precisecalo']=('EF_g_step1','_calo')
        seq_te_dict['precisecalocalib']=('EF_g_step2','_calocalib')    
        seq_te_dict['preciserec']=('EF_g_step3','')

        self.setupFromDict(seq_te_dict)

        if 'larpeb' in self.chainPart['addInfo']:
            from TrigDetCalib.TrigDetCalibConfig import LArEFROBListWriter
            self.EFsequenceList += [[['EF_g_step3'], 
                                     [ LArEFROBListWriter('LArEFROBListWriter_' + self.chainName, addCTPResult = True, addL2Result = True, addEFResult = True) ],
                                     'EF_g_step4']]
        
        if 'larpeb' in self.chainPart['addInfo']:
            self.EFsignatureList += [ [['EF_g_step4']*self.mult] ]

        if 'larpeb' in self.chainPart['addInfo']:
            self.TErenamingDict ['EF_g_step4']=  mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_larpeb')

    def setup_gXX_ID_iso(self):
        
        seq_te_dict = collections.OrderedDict()
        seq_te_dict['fastcalo']=('L2_g_step1','_calo')
        seq_te_dict['fastrec']=('L2_g_step2','')
        seq_te_dict['precisecalo']=('EF_g_step1','_calo')
        seq_te_dict['precisecalocalib']=('EF_g_step2','_calocalib')    
        #seq_te_dict['preciserec']=('EF_g_step3','')

        self.setupFromDict(seq_te_dict)
        
        # full scan topo and merging w/ RoI TEs
        te_in=''
        pos=2
        for step in self.fullScanSeqMap:
            self.EFsequenceList.insert(pos,[[te_in],self.fullScanSeqMap[step],step])
            self.EFsignatureList.insert(pos,[[step]] )
            te_in=step
            pos+=1
        
        self.EFsequenceList.insert(pos,[ ['EF_g_step2',te_in],[self.DummyMergerAlgo],'EF_gCache_step2'])
        self.EFsignatureList.insert(pos,[['EF_gCache_step2']])
        self.EFsequenceList.insert(pos+1,[['EF_gCache_step2'],self.ph_sequences['preciserec'],'EF_g_step3'])
        self.EFsignatureList.insert(pos+1,[['EF_g_step3']*self.mult])

        if 'larpeb' in self.chainPart['addInfo']:
            from TrigDetCalib.TrigDetCalibConfig import LArEFROBListWriter
            self.EFsequenceList += [[['EF_g_step3'], 
                                     [ LArEFROBListWriter('LArEFROBListWriter_' + self.chainName, 
                                                          addCTPResult = True, addL2Result = True, addEFResult = True) ],
                                     'EF_g_step4']]
        
        if 'larpeb' in self.chainPart['addInfo']:
            self.EFsignatureList += [ [['EF_g_step4']*self.mult] ]

        ########### TE renaming ###########

        self.TErenamingDict['EF_full']=mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_fs')
        self.TErenamingDict['EF_full_cell']=mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_fscalocell')
        self.TErenamingDict['EF_FSTopoClusters']=mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_fscalotopo')
        self.TErenamingDict['EF_FSTopoClustersED']=mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_fscalotopoed')
        self.TErenamingDict['EF_gCache_step2']=mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'ED')
        self.TErenamingDict['EF_g_step3']=mergeRemovingOverlap('EF_', self.chainPartNameNoMult)

        if 'larpeb' in self.chainPart['addInfo']:
            self.TErenamingDict ['EF_g_step4']=  mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_larpeb')
    
    def setup_gXX_ID_heavyIon(self):
        seq_te_dict = collections.OrderedDict()
        seq_te_dict['fastcalo']=('L2_g_step1','_calo')
        seq_te_dict['fastrec']=('L2_g_step2','')
        seq_te_dict['precisecalo']=('EF_g_step1','_calo')
        seq_te_dict['precisecalocalib']=('EF_g_step2','_calocalib')    
        seq_te_dict['preciserec']=('EF_g_step3','')
        
        # First set the normal sequence 
        self.setupFromDict(seq_te_dict)
        
        # Now insert additional steps
        self.EFsequenceList.insert(0,[[""],[DummyRoI("MinBiasDummyRoI")], 'L2_dummy_sp'])
        self.EFsequenceList.insert(1,[['L2_dummy_sp'],[theFSCellMaker], 'EF_AllCells'])
        self.EFsequenceList.insert(2,[['EF_AllCells'],[theUEMaker], 'EF_UE'])
        self.EFsignatureList.insert(0, [['L2_dummy_sp']] )
        self.EFsignatureList.insert(1, [['EF_AllCells']] )
        self.EFsignatureList.insert(2, [['EF_UE']] )


    def setup_gnocut_hiptrt(self):
        # list of required key values for sequences
        required_seq = ['precisecalo','precisecalocalib','preciserec']
        # Check for required sequences
        for item in required_seq:
            if item not in self.ph_sequences:
                log.error('%s sequence not defined', item)
            else:
                log.debug('%s', self.seqObj.print_sequence(item))
            
        ########### Sequences ###########
      
        self.L2sequenceList += [[self.L2InputTE,self.hip_trt_seq,'L2_g_step1']] 
        self.EFsequenceList += [[['L2_g_step1'],self.ph_sequences['precisecalo'],'EF_g_step1']]                       
        self.EFsequenceList += [[['EF_g_step1'],self.ph_sequences['precisecalocalib'],'EF_g_step2']]                       
        self.EFsequenceList += [[['EF_g_step2'],self.ph_sequences['preciserec'],'EF_g_step3']]                      

        ########### Signatures ###########

        self.L2signatureList += [ [['L2_g_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step3']*self.mult] ]
        
        ########### TE renaming ###########

        self.TErenamingDict = {
            'L2_g_step1': mergeRemovingOverlap('L2_', self.chainPartNameNoMult),
            'EF_g_step1': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_calo'),
            'EF_g_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_calocalib'),
            'EF_g_step3': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
        }

    def setup_gXX_ID_ringer(self):

        ########### Sequences ###########
       
        self.L2sequenceList += [[self.L2InputTE,self.ph_sequences['fastringer'],'L2_g_step1']]   
        self.EFsequenceList += [[['L2_g_step1'],self.ph_sequences['preciserec'],'EF_g_step1']]  
        self.EFsequenceList += [[['EF_g_step1'],[ self.ph_sequences['precisecalocalib'][0] ],'EF_g_step2']]  
        self.EFsequenceList += [[['EF_g_step2'],self.ph_sequences['preciserec'],'EF_g_step3']]  

        ########### Signatures ###########
        self.L2signatureList += [ [['L2_g_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step3']*self.mult] ]

        ########### TE renaming ###########

        self.TErenamingDict = {
            'L2_g_step1': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'_calo'),
            'EF_g_step1': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_calo'),
            'EF_g_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_calocalib'),
            'EF_g_step3': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
        }



