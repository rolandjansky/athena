# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################
#
# SliceDef file for HLT signatures used for streaming based on TT
#
#########################################################################
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

from TriggerMenu.menu.HltConfig import L2EFChainDef

#########################################################################
class L2EFChain_SeededStreamerTemplate(L2EFChainDef):
    def __init__(self, chainDict):
        self.L2sequenceList   = []
        self.EFsequenceList   = []
        self.L2signatureList  = []
        self.EFsignatureList  = []
        self.TErenamingDict   = []
        
        self.chainName = chainDict['chainName']
        self.chainPart = chainDict['chainParts']  
        self.chainL1Item = chainDict['L1item']       
        self.chainCounter = chainDict['chainCounter']       
        self.L2Name = 'L2_'+self.chainPart['chainPartName']
        self.EFName = 'EF_'+self.chainPart['chainPartName']
        self.chainPartName = self.chainPart['chainPartName']             
        self.streamingInfo = self.chainPart['streamingInfo']             


        self.L2InputTE = self.chainL1Item 
        newL1items = self.L2InputTE 



        # sequence/signature/TErenaming
        self.L2sequenceList += [[self.L2InputTE.replace("TAU","HA"), [] , 'L2_']]
        #self.L2signatureList += [[['L2_']]]
        self.TErenamingDict = {
            'L2_':     'L2_Streamer_' + self.chainName,
            }


        L2EFChainDef.__init__(self, self.chainName, self.L2Name, self.chainCounter,
                              newL1items, self.EFName, self.chainCounter, self.L2InputTE)

            
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


        
