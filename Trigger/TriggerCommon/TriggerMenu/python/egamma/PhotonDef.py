# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Photon trigger slice signatures  """

__author__  = 'Moritz Backes & Catrin Bernius'
__version__=""
__doc__="Implementation of Photon Signature"

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenu.egamma.PhotonDef")

from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenu.menu.TriggerPythonConfig import *
from TriggerMenu.menu.HltConfig import *

from TrigHIHypo.UE import theUEMaker, theFSCellMaker
from TrigInDetConf.TrigInDetSequence import TrigInDetSequence

from TrigGenericAlgs.TrigGenericAlgsConf import (PESA__DummyCombineAlgo,
                                                 PESA__DummyUnseededAllTEAlgo as DummyAlgo)

from TriggerMenu.commonUtils.makeCaloSequences import (fullScanTopoClusterSequence, 
        EgammaSlwClusterSequence, 
        EnergyDensitySequence, 
        getFullScanTopoClusterEDSequences)
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
    #theTrigTRTHTHCounter = TrigTRTHTHCounter()
    #theTrigTRTHTHhypo = TrigTRTHTHhypo()
    hip_trt_seq = theTRTDataPrep+[TrigTRTHTHCounter(), TrigTRTHTHhypo()]
    DummyMergerAlgo = PESA__DummyCombineAlgo("DummyMergerAlgo")
    def __init__(self, chainDict, photon_seq):

        self.L2sequenceList   = []
        self.EFsequenceList   = []
        self.L2signatureList  = []
        self.EFsignatureList  = []
        self.TErenamingDict   = []

        self.chainPart = chainDict['chainParts']
        self.seqObj = photon_seq
        self.sequences = photon_seq.get_sequences()

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

        log.debug('Sequences %s ' % (self.sequences))
                
        # gXX_ID type chains:
        #if True: #not self.chainPart['extra'] \
           #and self.chainPart['IDinfo'] \
           #and not self.chainPart['isoInfo'] \
           #and not self.chainPart['FSinfo'] \
           #and not self.chainPart['recoAlg'] \
           #and not self.chainPart['hypoInfo'] \
           #and not self.chainPart['trkInfo'] \
           #and not self.chainPart['reccalibInfo'] \
           #and not self.chainPart['isoInfo'] \
           #and not self.chainPart['addInfo']:
        if "hiptrt" in self.chainPart['addInfo']:
            self.setup_gnocut_hiptrt()
        elif 'ringer' in self.chainPart['addInfo']:
           self.setup_gXX_ID_ringer()
        elif 'ivloose' in self.chainPart['isoInfo']: 
           self.setup_gXX_ID_iso()
        elif 'iloose' in self.chainPart['isoInfo']: 
           self.setup_gXX_ID_iso()
        elif 'itight' in self.chainPart['isoInfo']: 
           self.setup_gXX_ID_iso()
        else:
            if 'ion' in self.chainPart['extra']:
                self.setup_gXX_ID_heavyIon()
            else:
                self.setup_gXX_ID()
        #else:
        #    log.error('Chain %s could not be assembled' % (self.chainPartName))
        #    return False
            
        L2EFChainDef.__init__(self, self.chainName, self.L2Name, self.chainCounter, self.chainL1Item, self.EFName, self.chainCounter, self.L2InputTE)
        

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

############################### DEFINE GROUPS OF CHAINS HERE ##############################

    def setup_gXX_ID(self):
        threshold = self.chainPart['threshold']
        IDinfo = self.chainPart['IDinfo']
        algoSuffix = "g%s_%s()" % (str(threshold),IDinfo)

        # list of required key values for sequences
        required_seq = ['fastcalo','fastrec','precisecalo','precisecalocalib','preciserec']

        # Check for required sequences
        for item in required_seq:
            if item not in self.sequences:
                log.error('%s sequence not defined', item)
            else:
                log.info('%s',self.sequences[item])
                log.info('%s', self.seqObj.print_sequence(item))
        
        # caloseq = EgammaSlwClusterSequence('L2_g_step2','EF_g_step1')
        ########### Sequences ###########
        self.L2sequenceList += [[self.L2InputTE, 
                                 self.sequences['fastcalo'],
                                 'L2_g_step1']]

        self.L2sequenceList += [[['L2_g_step1'],    
                                 self.sequences['fastrec'],
                                 'L2_g_step2']]
        
        self.EFsequenceList += [[['L2_g_step2'], 
                                self.sequences['precisecalo'],
                                'EF_g_step1']]
        
        #self.EFsequenceList += [caloseq]
        

        self.EFsequenceList += [[['EF_g_step1'], 
                                 self.sequences['precisecalocalib'],
                                 'EF_g_step2']]
        
        
        self.EFsequenceList += [[['EF_g_step2'], 
                                 self.sequences['preciserec'],
                                 'EF_g_step3']]

        if 'larpeb' in self.chainPart['addInfo']:
            from TrigDetCalib.TrigDetCalibConfig import LArEFROBListWriter
            self.EFsequenceList += [[['EF_g_step3'], 
                                     [ LArEFROBListWriter('LArEFROBListWriter_' + self.chainName, addCTPResult = True, addL2Result = True, addEFResult = True) ],
                                     'EF_g_step4']]
            
        ########### Signatures ###########

        self.L2signatureList += [ [['L2_g_step1']*self.mult] ]
        self.L2signatureList += [ [['L2_g_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step3']*self.mult] ]
        
        if 'larpeb' in self.chainPart['addInfo']:
            self.EFsignatureList += [ [['EF_g_step4']*self.mult] ]

        ########### TE renaming ###########

        self.TErenamingDict = {
            'L2_g_step1': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'_calo'),
            'L2_g_step2': mergeRemovingOverlap('L2_', self.chainPartNameNoMult),
            'EF_g_step1': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_calo'),
            'EF_g_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_calocalib'),
            'EF_g_step3': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
            }

        if 'larpeb' in self.chainPart['addInfo']:
            self.TErenamingDict ['EF_g_step4']=  mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_larpeb')

    def setup_gXX_ID_iso(self):
        threshold = self.chainPart['threshold']
        IDinfo = self.chainPart['IDinfo']
        isoInfo = self.chainPart['isoInfo']
        algoSuffix = "g%s_%s()" % (str(threshold),IDinfo)
       
        # list of required key values for sequences
        required_seq = ['fastcalo','fastrec','precisecalo','precisecalocalib','preciserec']
        
        fullScanTopoEDSequences =  getFullScanTopoClusterEDSequences()

        ########### Sequences ###########
        self.L2sequenceList += [[self.L2InputTE, 
                                 self.sequences['fastcalo'], 
                                 'L2_g_step1']]
        
        self.L2sequenceList += [[['L2_g_step1'],    
                                 self.sequences['fastrec'],
                                 'L2_g_step2']]
        
        self.EFsequenceList += [[['L2_g_step2'], 
                                 self.sequences['precisecalo'], 
                                 'EF_g_step1']]
        
        self.EFsequenceList += [[['EF_g_step1'], 
                                 self.sequences['precisecalocalib'], 
                                 'EF_g_step2']]
        
        self.EFsequenceList += [fullScanTopoEDSequences[0]]
        self.EFsequenceList += [fullScanTopoEDSequences[1]]

        self.EFsequenceList += [[ ['EF_g_step2',fullScanTopoEDSequences[1][-1]],
                                     [self.DummyMergerAlgo],
                                      'EF_gCache_step2']]

        self.EFsequenceList += [[['EF_gCache_step2'],
                                    self.sequences['preciserec'], 
                                 'EF_g_step3']]

        if 'larpeb' in self.chainPart['addInfo']:
            from TrigDetCalib.TrigDetCalibConfig import LArEFROBListWriter
            self.EFsequenceList += [[['EF_g_step3'], 
                                     [ LArEFROBListWriter('LArEFROBListWriter_' + self.chainName, addCTPResult = True, addL2Result = True, addEFResult = True) ],
                                     'EF_g_step4']]
            
        ########### Signatures ###########

        self.L2signatureList += [ [['L2_g_step1']*self.mult] ]
        self.L2signatureList += [ [['L2_g_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_gCache_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step3']*self.mult] ]
        
        if 'larpeb' in self.chainPart['addInfo']:
            self.EFsignatureList += [ [['EF_g_step4']*self.mult] ]

        ########### TE renaming ###########

        self.TErenamingDict = {
            'L2_g_step1': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'_calo'),
            'L2_g_step2': mergeRemovingOverlap('L2_', self.chainPartNameNoMult),
            'EF_g_step1': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_calo'),
            'EF_g_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_calocalib'),
            'EF_gCache_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'ED'),
            'EF_g_step3': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
            }

        if 'larpeb' in self.chainPart['addInfo']:
            self.TErenamingDict ['EF_g_step4']=  mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_larpeb')
    
    def setup_gXX_ID_heavyIon(self):
        threshold = self.chainPart['threshold']
        IDinfo = self.chainPart['IDinfo']
        algoSuffix = "g%s_%s()" % (str(threshold),IDinfo)
       
        # list of required key values for sequences
        required_seq = ['fastcalo','fastrec','precisecalo','precisecalocalib','preciserec']
        

        ########### Sequences ###########
        
        self.L2sequenceList += [[self.L2InputTE, 
                                 self.sequences['fastcalo'], 
                                 'L2_g_step1']]
        
        self.L2sequenceList += [[['L2_g_step1'],    
                                 self.sequences['fastrec'], 
                                 'L2_g_step2']]

        self.EFsequenceList += [[['L2_g_step2'], 
                                 [theFSCellMaker], 'EF_g_step1_fs']]

        self.EFsequenceList += [[['EF_g_step1_fs'], 
                                 [theUEMaker], 'EF_g_step1_ue']]

        self.EFsequenceList += [[['L2_g_step2'], 
                                 self.sequences['precisecalo'], 
                                 'EF_g_step1']]
        
        self.EFsequenceList += [[['EF_g_step1'], 
                                 self.sequences['precisecalocalib'], 
                                 'EF_g_step2']]
        
        self.EFsequenceList += [[['EF_g_step2'], 
                                 self.sequences['preciserec'], 
                                 'EF_g_step3']]

        ########### Signatures ###########

        self.L2signatureList += [ [['L2_g_step1']*self.mult] ]
        self.L2signatureList += [ [['L2_g_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step1_fs']] ]
        self.EFsignatureList += [ [['EF_g_step1_ue']] ]
        self.EFsignatureList += [ [['EF_g_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step3']*self.mult] ]

        ########### TE renaming ###########

        self.TErenamingDict = {
            'L2_g_step1': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'_calo'),
            'L2_g_step2': mergeRemovingOverlap('L2_', self.chainPartNameNoMult),
            'EF_g_step1_fs': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_fs'),
            'EF_g_step1_ue': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_ue'),
            'EF_g_step1': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_calo'),
            'EF_g_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_calocalib'),
            'EF_g_step3': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
            }

    def setup_gnocut_hiptrt(self):
        # list of required key values for sequences
        required_seq = ['precisecalo','precisecalocalib','preciserec']
        # Check for required sequences
        for item in required_seq:
            if item not in self.sequences:
                log.error('%s sequence not defined', item)
            else:
                log.info('%s', self.seqObj.print_sequence(item))
        # EF Calo
        # Reminder that names change for this chain
        #theTrigEFCaloHypo = TrigEFCaloHypo_All("TrigEFCaloHypo_g_hiptrt_NoCut",0);
        #theEFPhotonHypo  = EFPhotonHypo_g_NoCut("TrigEFPhotonHypo_g_hiptrt_NoCut",0)
            
        ########### Sequences ###########
      
        self.L2sequenceList += [[self.L2InputTE, 
                                  self.hip_trt_seq,
                                  'L2_g_step1']]

        self.EFsequenceList += [[['L2_g_step1'], 
                                    self.sequences['precisecalo'],                      
                                 'EF_g_step1']]
        
        self.EFsequenceList += [[['EF_g_step1'], 
                                 self.sequences['precisecalocalib'],                      
                                 'EF_g_step2']]

        self.EFsequenceList += [[['EF_g_step2'],
                                 self.sequences['preciserec'],                      
                                 'EF_g_step3']]


        ########### Signatures ###########

        self.L2signatureList += [ [['L2_g_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step3']*self.mult] ]
        ########### TE renaming ###########

        self.TErenamingDict = {
            'L2_g_step1': mergeRemovingOverlap('L2_', self.chainPartNameNoMult),
            'EF_g_step1': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_calo'),
#            'EF_g_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
            'EF_g_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_calocalib'),
            'EF_g_step3': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
            }

    def setup_gXX_ID_ringer(self):

        threshold = self.chainPart['threshold']
        IDinfo = self.chainPart['IDinfo']
        algoSuffix = "g%s_%s()" % (str(threshold),IDinfo)
       
        # list of required key values for sequences
        required_seq = ['fastringer','precisecalo','precisecalocalib','preciserec']

        ########### Sequences ###########
       
        self.L2sequenceList += [[self.L2InputTE, 
                                self.sequences['fastringer'], 
                                'L2_g_step1']] 

        self.EFsequenceList += [[['L2_g_step1'], 
                                 self.sequences['preciserec'], 
                                 'EF_g_step1']]
        
        self.EFsequenceList += [[['EF_g_step1'], 
                                 [ self.sequences['precisecalocalib'][0] ], 
                                 'EF_g_step2']]
        
        self.EFsequenceList += [[['EF_g_step2'], 
                                 self.sequences['preciserec'], 
                                 'EF_g_step3']]

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



