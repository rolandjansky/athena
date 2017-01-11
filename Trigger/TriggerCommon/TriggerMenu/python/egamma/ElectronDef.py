# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Electron trigger slice signatures  """

__author__  = 'Moritz Backes & Catrin Bernius'
__version__=""
__doc__="Implementation of Electron Signature"

from AthenaCommon.Logging import logging

from TriggerMenu.menu.HltConfig                import L2EFChainDef, mergeRemovingOverlap
from TriggerMenu.egamma.EgammaCleanMonitoring  import *
from TriggerMenu.menu.CleanMonitoring          import *
from TrigHIHypo.UE import theUEMaker, theFSCellMaker, theElectronUEMonitoring
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenu.egamma.ElectronDef")
##########################################################################################
#
# L2 & EF Chain configuration helper classes
#
##########################################################################################

class L2EFChain_e(L2EFChainDef):

    # Define frequently used instances here as class variables

    def __init__(self, chainDict, electron_seq):

        self.L2sequenceList   = []
        self.EFsequenceList   = []
        self.L2signatureList  = []
        self.EFsignatureList  = []
        self.TErenamingDict   = []
    
        self.chainPart = chainDict['chainParts']
        self.seqObj = electron_seq
        self.sequences = electron_seq.get_sequences()

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

                
        # eXXvh_ID type chains:
        if self.chainPart['trkInfo']=='idperf': #True:# self.chainPart['IDinfo'] \
           #and not self.chainPart['isoInfo'] \
           #and not self.chainPart['FSinfo'] \
           #and not self.chainPart['recoAlg'] \
           #and not self.chainPart['hypoInfo'] \
           #and not self.chainPart['trkInfo'] \
           #and not self.chainPart['reccalibInfo']:
            if 'ion' in self.chainPart['extra']:
                self.setup_eXXvh_idperf_heavyIon()
            else:
                self.setup_eXXvh_idperf()
        elif 'ringer' in self.chainPart['addInfo']:
           self.setup_eXXvh_ID_ringer()
        else:
            if 'ion' in self.chainPart['extra']:
                self.setup_eXXvh_ID_run2_heavyIon()
            else:
                self.setup_eXXvh_ID_run2()

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
    def setup_eXXvh_idperf(self):

        threshold = self.chainPart['threshold']
        IDinfo = self.chainPart['IDinfo']
        isoInfo = self.chainPart['isoInfo']
        run1 = self.chainPart['trkInfo']
       
        log.debug('setup_eXXvh_idperf')
        log.debug('threshold: %s',threshold)
        log.debug('isoInfo: %s',isoInfo)
        log.debug('IDinfo: %s',IDinfo)
        log.debug('trkInfo: %s',run1)
        
        self.L2sequenceList += [[self.L2InputTE, 
            self.sequences['fastcalo'],                     
                                 'L2_e_step1']]
        
        self.EFsequenceList += [[['L2_e_step1'], 
            self.sequences['precisecalo'],                     
                                 'EF_e_step1']]
        
        self.EFsequenceList += [[['EF_e_step1'], 
            self.sequences['precisecalocalib'],                     
                                 'EF_e_step2']]
        
        #if 'L2Star' in self.chainPart['addInfo']:
        #    self.EFsequenceList += [[['EF_e_step2'], 
        #                             [theTrigL2SiTrackFinder_eGammaA]+self.theL2StarxAOD+
        #                             [theTrigL2SiTrackFinder_eGammaB]+self.theL2StarxAOD+
        #                             [theTrigL2SiTrackFinder_eGammaC]+self.theL2StarxAOD+theEFElectronIDFex,
        #                             'EF_e_step3']]
        self.EFsequenceList += [[['EF_e_step2'], 
                                 self.sequences['trackrec'],
                                 'EF_e_step3']]
        
        self.EFsequenceList += [[['EF_e_step3'], 
            self.sequences['preciserec'],                     
                                 'EF_e_step4']]

        ########### Signatures ###########

        self.L2signatureList += [ [['L2_e_step1']*self.mult] ]
        #self.L2signatureList += [ [['L2_e_step2']*self.mult] ]
        #self.L2signatureList += [ [['L2_e_step3']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step3']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step4']*self.mult] ]

        ########### TE renaming ###########

        self.TErenamingDict = {
            'L2_e_step1': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'cl'),
            'EF_e_step1': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'cl'),
            'EF_e_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'calocalib'),
            'EF_e_step3': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'trk'),
            'EF_e_step4': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
            }

    def setup_eXXvh_idperf_heavyIon(self):

        threshold = self.chainPart['threshold']
        IDinfo = self.chainPart['IDinfo']
        isoInfo = self.chainPart['isoInfo']
        run1 = self.chainPart['trkInfo']
       
        log.debug('setup_eXXvh_idperf_heavyIon')
        log.debug('threshold: %s',threshold)
        log.debug('isoInfo: %s',isoInfo)
        log.debug('IDinfo: %s',IDinfo)
        log.debug('trkInfo: %s',run1)
        
        precisecalocalib =  self.sequences['precisecalocalib']
        precisecalocalib.insert(1,theElectronUEMonitoring) 
        self.L2sequenceList += [[self.L2InputTE, 
            self.sequences['fastcalo'],                     
                                 'L2_e_step1']]
        self.EFsequenceList += [[['L2_e_step1'], 
                                     [theFSCellMaker], 'EF_e_step1_fs']]

        self.EFsequenceList += [[['EF_e_step1_fs'], 
                                 [theUEMaker], 'EF_e_step1_ue']]

        self.EFsequenceList += [[['L2_e_step1'], 
            self.sequences['precisecalo'],                     
                                 'EF_e_step1']]
        
        self.EFsequenceList += [[['EF_e_step1'], 
            precisecalocalib,
                                 'EF_e_step2']]
        
        self.EFsequenceList += [[['EF_e_step2'], 
            self.sequences['trackrec'],
                                 'EF_e_step3']]
        
        self.EFsequenceList += [[['EF_e_step3'], 
            self.sequences['preciserec'],                     
                                 'EF_e_step4']]

        ########### Signatures ###########

        self.L2signatureList += [ [['L2_e_step1']*self.mult] ]
        self.L2signatureList += [ [['EF_e_step1_fs']] ]
        self.L2signatureList += [ [['EF_e_step1_ue']] ]
        self.EFsignatureList += [ [['EF_e_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step3']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step4']*self.mult] ]

        ########### TE renaming ###########

        self.TErenamingDict = {
            'L2_e_step1': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'cl'),
            'EF_e_step1_fs': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'fs'),
            'EF_e_step1_ue': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'ue'),
            'EF_e_step1': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'cl'),
            'EF_e_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'calocalib'),
            'EF_e_step3': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'trk'),
            'EF_e_step4': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
            }

    def setup_eXXvh_ID_ringer(self):

        threshold = self.chainPart['threshold']
        IDinfo    = self.chainPart['IDinfo']
        isoInfo   = self.chainPart['isoInfo']
        addInfo   = self.chainPart['addInfo']
        trkInfo   = self.chainPart['trkInfo']
      
        log.debug('setup_eXXvh_ID_ringer')
        log.debug('threshold: %s',threshold)
        log.debug('isoInfo: %s',isoInfo)
        log.debug('IDinfo: %s',IDinfo)
        log.debug('trkInfo: %s',trkInfo)
              
        self.L2sequenceList += [[self.L2InputTE, 
                                self.sequences['fastringer'],                     
                                 'L2_e_step1']]
        
        self.EFsequenceList += [[['L2_e_step1'], 
            self.sequences['precisecalo'],                     
                                 'EF_e_step1']]
        
        self.EFsequenceList += [[['EF_e_step1'], 
            self.sequences['precisecalocalib'],                     
                                 'EF_e_step2']]
        
        self.EFsequenceList += [[['EF_e_step2'], 
            self.sequences['trackrec'],                     
                                 'EF_e_step3']]
        
        self.EFsequenceList += [[['EF_e_step3'], 
            self.sequences['preciserec'],                     
                                 'EF_e_step4']]

        ########### Signatures ###########

        self.L2signatureList += [ [['L2_e_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step3']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step4']*self.mult] ]
        ########### TE renaming ###########

        self.TErenamingDict = {
            'L2_e_step1': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'cl'),
            'EF_e_step1': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'calo'),
            'EF_e_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'calocalib'),
            'EF_e_step3': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'id'),
            'EF_e_step4': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
            }

    def setup_eXXvh_ID_run2(self):
        threshold = self.chainPart['threshold']
        IDinfo = self.chainPart['IDinfo']
        isoInfo = self.chainPart['isoInfo']
        addInfo = self.chainPart['addInfo']
        trkInfo = self.chainPart['trkInfo']
      
        log.debug('setup_eXXvh_ID_run2')
        log.debug('threshold: %s',threshold)
        log.debug('isoInfo: %s',isoInfo)
        log.debug('IDinfo: %s',IDinfo)
        log.debug('trkInfo: %s',trkInfo)
        self.L2sequenceList += [[self.L2InputTE, 
                                 self.sequences['fastcalo'],
                                 'L2_e_step1']]
        
        self.L2sequenceList += [[['L2_e_step1'],    
                                 self.sequences['fasttrack'], 
                                 'L2_e_step2']]
      
        self.L2sequenceList += [[['L2_e_step2'], 
                                 self.sequences['fastrec'], 
                                 'L2_e_step3']]
        
        self.EFsequenceList += [[['L2_e_step3'], 
                                 self.sequences['precisecalo'], 
                                 'EF_e_step1']]
        
        self.EFsequenceList += [[['EF_e_step1'], 
                                 self.sequences['precisecalocalib'], 
                                 'EF_e_step2']]
        
        self.EFsequenceList += [[['EF_e_step2'], 
                                 self.sequences['precisetrack'], 
                                 'EF_e_step3']]
     
        self.EFsequenceList += [[['EF_e_step3'], 
                                 self.sequences['preciserec'], 
                                 'EF_e_step4']]

        ########### Signatures ###########

        self.L2signatureList += [ [['L2_e_step1']*self.mult] ]
        self.L2signatureList += [ [['L2_e_step2']*self.mult] ]
        self.L2signatureList += [ [['L2_e_step3']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step3']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step4']*self.mult] ]
        ########### TE renaming ###########

        self.TErenamingDict = {
            'L2_e_step1': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'cl'),
            'L2_e_step2': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'id'),
            'L2_e_step3': mergeRemovingOverlap('L2_', self.chainPartNameNoMult),
            'EF_e_step1': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'calo'),
            'EF_e_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'calocalib'),
            'EF_e_step3': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'id'),
            'EF_e_step4': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
            }

    def setup_eXXvh_ID_run2_heavyIon(self):
        threshold = self.chainPart['threshold']
        IDinfo = self.chainPart['IDinfo']
        isoInfo = self.chainPart['isoInfo']
        addInfo = self.chainPart['addInfo']
        trkInfo = self.chainPart['trkInfo']
      
        log.debug('setup_eXXvh_ID_run2_heavyIon')
        log.debug('threshold: %s',threshold)
        log.debug('isoInfo: %s',isoInfo)
        log.debug('IDinfo: %s',IDinfo)
        log.debug('trkInfo: %s',trkInfo)
        
        precisecalocalib =  self.sequences['precisecalocalib']
        precisecalocalib.insert(1,theElectronUEMonitoring) 
        self.L2sequenceList += [[self.L2InputTE, 
            self.sequences['fastcalo'],                     
                                 'L2_e_step1']]
        
        self.L2sequenceList += [[['L2_e_step1'],    
            self.sequences['fasttrack'],                     
                                 'L2_e_step2']]
        
        self.L2sequenceList += [[['L2_e_step2'], 
            self.sequences['fastrec'],                     
                                 'L2_e_step3']]
        
        self.EFsequenceList += [[['L2_e_step3'], 
                                 [theFSCellMaker], 'EF_e_step1_fs']]

        self.EFsequenceList += [[['EF_e_step1_fs'], 
                                 [theUEMaker], 'EF_e_step1_ue']]

        self.EFsequenceList += [[['L2_e_step3'], 
            self.sequences['precisecalo'],                     
                                 'EF_e_step1']]
        
        self.EFsequenceList += [[['EF_e_step1'], 
            precisecalocalib,                     
                                 'EF_e_step2']]
        self.EFsequenceList += [[['EF_e_step2'], 
            self.sequences['precisetrack'],                     
                                 'EF_e_step3']]
        
        self.EFsequenceList += [[['EF_e_step3'], 
            self.sequences['preciserec'],                     
                                 'EF_e_step4']]

        ########### Signatures ###########

        self.L2signatureList += [ [['L2_e_step1']*self.mult] ]
        self.L2signatureList += [ [['L2_e_step2']*self.mult] ]
        self.L2signatureList += [ [['L2_e_step3']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step1_fs']] ]
        self.EFsignatureList += [ [['EF_e_step1_ue']] ]
        self.EFsignatureList += [ [['EF_e_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step3']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step4']*self.mult] ]
        ########### TE renaming ###########

        self.TErenamingDict = {
            'L2_e_step1': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'cl'),
            'L2_e_step2': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'id'),
            'L2_e_step3': mergeRemovingOverlap('L2_', self.chainPartNameNoMult),
            'EF_e_step1_fs': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'fs'),
            'EF_e_step1_ue': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'ue'),
            'EF_e_step1': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'calo'),
            'EF_e_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'calocalib'),
            'EF_e_step3': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'id'),
            'EF_e_step4': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
            }
