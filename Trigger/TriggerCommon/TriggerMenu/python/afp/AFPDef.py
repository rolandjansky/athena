# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" AFP slice signatures """

__author__  = "L. Zivkovic, C.Bernius"
__version__ = "" 
__doc__="Implementation of AFP slice in new TM framework "
##########################################################

from TriggerMenu.menu.HltConfig import L2EFChainDef
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenu.afp.AFPDef")


from TriggerMenu.menu import DictFromChainName

# Jet imports
from TriggerMenu.jet.JetDef import generateHLTChainDef

# AFP imports
from TrigAFPReco.TrigAFPRecoConfig import AFPSiTrkReco
from TrigAFPHypo.TrigAFPJetAllTEConfig import AFPJetAllTE

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

        self.L2InputL1Item = self.chainPartL1Item or self.chainL1Item
        if self.L2InputL1Item:
            self.L2InputTE = ''

        # Actually configure chains
        self.createChains()

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


    def addJetsToTrigger(self):
        """Adds jet trigger elements to sequence and returns TE with jets.

        This method adds all algorithms needed to reconstruct jets and
        all trigger elements that are needed. It returns the last
        trigger element in the chain which contains reconstructed
        jets.

        @return trigger element with jets
        """

        #----------------------------------------------------
        # Obtaining the needed jet TEs from the jet code
        #----------------------------------------------------

        jetCalib    = self.chainPart['jetCalib']
        calibration = self.chainPart['calib']
    
        chain = ['j0_{0}_{1}'.format(calibration, jetCalib), '', [], ["Main"], ['RATE:SingleJet', 'BW:Jet'], -1]
        
        theDictFromChainName = DictFromChainName.DictFromChainName()
        jetChainDict = theDictFromChainName.getChainDict(chain)
        
        jetChainDict['chainCounter'] = 9151
        jetChainDef = generateHLTChainDef(jetChainDict)
        
        #obtaining DummyUnseededAllTEAlgo/RoiCreator 
        input0=jetChainDef.sequenceList[0]['input']
        output0 =jetChainDef.sequenceList[0]['output']
        algo0 =jetChainDef.sequenceList[0]['algorithm']
        self.EFsequenceList +=[[ input0,algo0,  output0 ]]
        self.EFsignatureList +=[[ [output0] ]]
        
        #obtaing TrigCaloCellMaker/FS 
        input1=jetChainDef.sequenceList[1]['input']
        output1 =jetChainDef.sequenceList[1]['output']
        algo1 =jetChainDef.sequenceList[1]['algorithm']
        self.EFsequenceList +=[[ input1,algo1,  output1 ]]
        self.EFsignatureList +=[[ [output1] ]]
        
        #obtaining TrigCaloClusterMaker
        input2=jetChainDef.sequenceList[2]['input']
        output2 =jetChainDef.sequenceList[2]['output']
        algo2 =jetChainDef.sequenceList[2]['algorithm']
        self.EFsequenceList +=[[ input2,algo2,  output2 ]]
        self.EFsignatureList +=[[ [output2] ]]
        
        #obtaining TrigHLTEnergyDensity
        input3=jetChainDef.sequenceList[3]['input']
        output3 =jetChainDef.sequenceList[3]['output']
        algo3 =jetChainDef.sequenceList[3]['algorithm']
        self.EFsequenceList +=[[ input3,algo3,  output3 ]]
        self.EFsignatureList +=[[ [output3] ]]
        
        #obtaining TrigHLTJetRecFromCluster
        input4=jetChainDef.sequenceList[4]['input']
        output4 =jetChainDef.sequenceList[4]['output']
        algo4 =jetChainDef.sequenceList[4]['algorithm']
        self.EFsequenceList +=[[ input4,algo4,  output4 ]]
        self.EFsignatureList +=[[ [output4] ]]
        #---End of obtaining jet TEs------------------------------

        return output4

    def addAFPTracksToTrigger (self, inputTE):
        """Adds TrigAFPSiTrkReco FEX algorithm, which reconstructs AFP tracks to the sequence.

        The method returns output trigger element, but it should not
        be used, because the AFP tracks are saved to StoreGate.

        @param inputTE:  input trigger element, can be anything, because it is not used, but cannot be empty string
        """
        
        outputTE='EF__afpSiTrkReco'
        self.EFsequenceList += [[ inputTE, [AFPSiTrkReco()], outputTE ]]
        self.EFsignatureList += [[ [outputTE ] ]]

        return outputTE
   

    def addExclusiveJetAllTEToTrigger (self, inputTE):
        """Adds TrigAFPJetAllTE, which makes vertex selection, to the sequence.

        It compares proton position in AFP reconstructed using jets
        and tracks from AFP.

        @param inputTE: trigger element with jets

        @return outputTE: output trigger element - currently empty
        """

        outputTE = 'EF_afpJetAllTE'
        self.EFsequenceList += [[ inputTE, [AFPJetAllTE()], outputTE ]]
        self.EFsignatureList += [[ [outputTE ] ]]

        return outputTE

    def createChains(self):
        """craetes the chains based on trigger name.
        """

        if 'jetexc' == self.chainPart['extra']:
            jetTE = self.addJetsToTrigger()
            self.addAFPTracksToTrigger (jetTE)
            self.addExclusiveJetAllTEToTrigger (jetTE)

