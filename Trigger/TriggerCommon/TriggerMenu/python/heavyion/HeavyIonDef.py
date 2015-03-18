# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

########################################################################
#
# SliceDef file for heavy ion chains/signatures
#
#########################################################################
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)

logHeavyIonDef = logging.getLogger("TriggerMenu.heavyion.HeavyIonDef")

from AthenaCommon import CfgGetter
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

import re

from TriggerJobOpts.TriggerFlags            import TriggerFlags

from TriggerMenu.heavyion.HeavyIonSliceFlags  import HeavyIonSliceFlags

from TriggerMenu.menu.HltConfig import *

from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo

#theTrigEFIDInsideOut_FullScan = TrigEFIDSequence("FullScan","fullScan")

from TrigT2MinBias.TrigT2MinBiasConfig import *
from InDetTrigRecExample.EFInDetConfig import TrigEFIDSequence
#fexes.efid = TrigEFIDSequence("minBias","minBias","InsideOut").getSequence()
#fexes.efid2P = TrigEFIDSequence("minBias2P","minBias2","InsideOutLowPt").getSequence()

efiddataprep = TrigEFIDSequence("minBias","minBias","DataPrep").getSequence()
efid = TrigEFIDSequence("minBias","minBias","InsideOut").getSequence()
efid_heavyIon = TrigEFIDSequence("heavyIonFS","heavyIonFS","InsideOut").getSequence()
efid2P = TrigEFIDSequence("minBias2P","minBias2","InsideOutLowPt").getSequence()

from TrigMinBias.TrigMinBiasConfig import *


from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo as DummyRoI
from TrigGenericAlgs.TrigGenericAlgsConf import PrescaleAlgo
dummyRoI=DummyRoI(name='MinBiasDummyRoI', createRoIDescriptors = True, NumberOfOutputTEs=1)
terminateAlgo = PrescaleAlgo('terminateAlgo')


# for HI
from TrigHIHypo.TrigHIHypoConfig import HIEFTrackHypo_AtLeastOneTrack
atLeastOneTrack = HIEFTrackHypo_AtLeastOneTrack(name='HIEFTrackHypo_AtLeastOneTrack')

from TrigHIHypo.TrigHIHypoConfig import *
#hypos.update(hi_hypos)

#L2 pileup suppression
from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_FullScan_ZF_OnlyA  #TrigL2SiTrackFinder_FullScanA_ZF_OnlyA

theL2PileupSup = TrigL2SiTrackFinder_FullScan_ZF_OnlyA()

###########################################################################
#  All min bias
###########################################################################
class L2EFChain_HI(L2EFChainDef):
    
    def __init__(self, chainDict):

        self.L2sequenceList   = []
        self.EFsequenceList   = []
        self.L2signatureList  = []
        self.EFsignatureList  = []
        self.TErenamingDict   = []
        
        self.chainPart = chainDict['chainParts']
        
        self.chainL1Item = chainDict['L1item']        
        self.chainPartL1Item = self.chainPart['L1item']
        
        self.chainCounter = chainDict['chainCounter']       
        self.L2Name = 'L2_'+self.chainPart['chainPartName']
        self.EFName = 'EF_'+self.chainPart['chainPartName']
        #self.mult = int(self.chainPart['multiplicity'])
        self.chainName = chainDict['chainName']
        self.chainPartName = self.chainPart['chainPartName']
        #self.chainPartNameNoMult = self.chainPartName[1:] if self.mult > 1 else self.chainPartName
        
        self.L2InputTE = self.chainPartL1Item or self.chainL1Item
        # cut of L1_, _EMPTY,..., & multiplicity
        self.L2InputTE = self.L2InputTE.replace("L1_","")
        self.L2InputTE = self.L2InputTE.split("_")[0]
        self.L2InputTE = self.L2InputTE[1:] if self.L2InputTE[0].isdigit() else self.L2InputTE

        if "v2" in self.chainPart['eventShape']:
            self.setup_hi_eventshape()
        elif "v3" in self.chainPart['eventShape']:
            self.setup_hi_eventshape()
        
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

############################### HELPER FUNCTIONS ##############################
    def setup_hi_eventshape(self):

        EShypo_temp = self.chainPart['extra']
        ESth=EShypo_temp.lstrip('th')
        #print 'igb: ES threshold:', ESth

        if 'v2' in self.chainPart['eventShape']:
            from TrigHIHypo.VnHypos import V2
            chainSuffix = 'v2_th'+ESth
            EShypo=V2("V2_"+ESth, int(ESth)) 
        elif 'v3' in self.chainPart['eventShape']:
            from TrigHIHypo.VnHypos import V3
            chainSuffix = 'v3_th'+ESth 
            EShypo=V3("V3_"+ESth, int(ESth)) 

        from TrigHIHypo.UE import theUEMaker, theFSCellMaker

        ########### Sequence List ##############
        self.L2sequenceList += [["",
                                 [dummyRoI],
                                 'L2_hi_step1']] 
        self.EFsequenceList += [[['L2_hi_step1'], 
                                     [theFSCellMaker], 'EF_hi_step1_fs']]

        self.EFsequenceList += [[['EF_hi_step1_fs'], 
                                 [theUEMaker], 'EF_hi_step1_ue']]

        self.EFsequenceList += [[['EF_hi_step1_ue'], 
                                 [EShypo], 'EF_hi_step2']]

        ########### Signatures ###########
        self.L2signatureList += [ [['L2_hi_step1']] ]
        self.EFsignatureList += [ [['EF_hi_step1_fs']] ]
        self.EFsignatureList += [ [['EF_hi_step1_ue']] ]
        self.EFsignatureList += [ [['EF_hi_step2']] ]
    
        self.TErenamingDict = {
            'L2_hi_step1': mergeRemovingOverlap('L2_hi_step1_', chainSuffix),
            'EF_hi_step1_fs': mergeRemovingOverlap('EF_hi_fs_', chainSuffix),
            'EF_hi_step1_ue': mergeRemovingOverlap('EF_hi_ue_', chainSuffix),
            'EF_hi_step2': mergeRemovingOverlap('EF_hi_', chainSuffix),
            }

###########################
        
#####################################################################
    
#if __name__ == '__main__':
#    triggerPythonConfig = TriggerPythonConfig('hlt.xml', None)
#    for m in Muons:
#        m.generateMenu(triggerPythonConfig)
#    triggerPythonConfig.writeConfigFiles()
    
