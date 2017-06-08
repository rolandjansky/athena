# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" AFP slice signatures """

__author__  = "L. Zivkovic, C.Bernius"
__version__ = "" 
__doc__="Implementation of AFP slice in new TM framework "
##########################################################

#from AthenaCommon.SystemOfUnits import GeV
from TriggerMenu.menu.HltConfig import L2EFChainDef
#from TriggerMenu.menu.HltConfig import L2EFChainDef,mergeRemovingOverlap
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenu.afp.AFPDef")

## Here probably HLT goes?
## Import from AFPHypo?

#from TriggerJobOpts.TriggerFlags import TriggerFlags


#from TrigAFPHypo.TrigAFPJetAllTEConfig import TrigAFPJetAllTE



##  More may go here

#############################################################################
class L2EFChain_afp(L2EFChainDef):
     
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

# These two lines should eventually be uncommented
        self.L2InputL1Item = self.chainPartL1Item or self.chainL1Item
        if self.L2InputL1Item:
##             self.L2InputTE = getInputTEfromL1Item(self.L2InputL1Item)
##         else:
            self.L2InputTE = ''


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
