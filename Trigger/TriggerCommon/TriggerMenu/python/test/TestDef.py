# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################
#
# SliceDef file for HLT signatures used exclusively in the test slice
#
#########################################################################
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger( 'TriggerMenu.test.TestDef' )

from TriggerMenu.menu.HltConfig import L2EFChainDef,mergeRemovingOverlap

###################################################################################

def getInputTEfromL1Item(item):
    TE = item.replace("L1_","")
    TE = TE.split("_")[0]
    TE = TE[1:] if TE[0].isdigit() else TE
    return TE


class L2EFChain_test(L2EFChainDef):

    def __init__(self, chainDict):
 
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
        self.mult = int(self.chainPart['multiplicity'])
        self.chainName = chainDict['chainName']
        self.chainPartName = self.chainPart['chainPartName']


        self.L2InputL1Item = self.chainPartL1Item or self.chainL1Item
        if self.L2InputL1Item:
            self.L2InputTE = getInputTEfromL1Item(self.L2InputL1Item)
        else:
            self.L2InputTE = ''

        self.Flag='TriggerFlags.TestSlice.do%s()' % self.chainName


        self.setupTestChain()
        L2EFChainDef.__init__(self, self.chainName, self.L2Name, self.chainCounter,
                              self.chainL1Item, self.EFName, self.chainCounter, self.L2InputTE)

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
        self.TErenamingMap=self.TErenamingDict
      

    ##################################################################
    def setupTestChain(self):
        
        from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX
        self.EF_Dummy = DummyFEX("EF_%s" % self.chainName)
        
        # Sequences
        self.L2sequenceList += [[ self.L2InputTE, 
                                  [self.EF_Dummy],
                                  'L2_step1']]
        self.EFsequenceList += [[ ['L2_step1'], [self.EF_Dummy], 'EF_step1']]
        
        # Signatures
        self.L2signatureList += [ [['L2_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_step1']*self.mult] ]
        # TE renaming
        self.TErenamingDict = {
            'L2_step1': mergeRemovingOverlap('L2_', 'Test_'+self.chainName+'_step1'),
            'EF_step1': mergeRemovingOverlap('EF_', self.chainName),
            }               	         


