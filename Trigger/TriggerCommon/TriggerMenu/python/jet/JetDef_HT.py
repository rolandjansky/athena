# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

########################################################################
#
# SliceDef file for HT chains/signatures
#
#########################################################################
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)

logJetHTDef = logging.getLogger("TriggerMenu.jet.JetDef_HT")

from TriggerMenu.menu.HltConfig import L2EFChainDef, mergeRemovingOverlap

from AthenaCommon.SystemOfUnits import GeV

#-----------------------------------
class L2EFChain_HT(L2EFChainDef):
#-----------------------------------
  
    def __init__(self, chainDict):

        #self.HLTsequenceList = []
        #self.HLTsignatureList = []
        self.L2sequenceList = []
        self.L2signatureList = []
        self.EFsequenceList = []
        self.EFsignatureList = []
        self.TErenamingDict   = []

        self.chainPart = chainDict['chainParts'][0]

        self.chainL1Item = chainDict['L1item']
        self.chainPartL1Item = self.chainPart['L1item']

        self.chainCounter = chainDict['chainCounter']
        self.L2Name = 'L2_'+self.chainPart['chainPartName']
        self.EFName = 'EF_'+self.chainPart['chainPartName']
        self.mult = int(self.chainPart['multiplicity'])
        self.chainName = chainDict['chainName']
        self.chainPartName = self.chainPart['chainPartName']
        self.chainPartNameNoMult = self.chainPartName[1:] if self.mult > 1 else self.chainPartName
        
        self.L2InputTE = self.chainPartL1Item or self.chainL1Item
        # cut of L1_, _EMPTY,..., & multiplicity
        self.L2InputTE = self.L2InputTE.replace("L1_","")
        self.L2InputTE = self.L2InputTE.split("_")[0]
        self.L2InputTE = self.L2InputTE[1:] if self.L2InputTE[0].isdigit() else self.L2InputTE

        #if ():
        self.setup_htXX()
        #else:
        #   logJetHTDef.error('Chain %s could not be assembled' % (self.chainPartName))
        #    return False
    
        L2EFChainDef.__init__(self, self.chainName, self.L2Name, self.chainCounter, 
                              self.chainL1Item, self.EFName, self.chainCounter, self.L2InputTE)
        

    def defineSequences(self):        
        for sequence in self.L2sequenceList:
            self.addL2Sequence(*sequence)

        for sequence in self.EFsequenceList:
            self.addEFSequence(*sequence)

#        for sequence in self.HLTsequenceList:
#            self.addSequence(*sequence)
      
    def defineSignatures(self):        
        for signature in self.L2signatureList:
            self.addL2Signature(*signature)
        for signature in self.EFsignatureList:
            self.addEFSignature(*signature)
    #    for signature in self.HLTsignatureList:
    #        self.addEFSignature(*signature)
                  
    def defineTErenaming(self):
        self.TErenamingMap = self.TErenamingDict



############################### DEFINE GROUPS OF CHAINS HERE ##############################


    def setup_htXX(self):
        
        #import and configuration of fexes and hypos
        from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo as DummyAlgo
        theDummyRoiCreator = DummyAlgo('RoiCreator')

        #TrigCaloCellMaker/CellMakerFullCalo_topo 
        from TrigCaloRec.TrigCaloRecConfig import TrigCaloCellMaker_jet_fullcalo	
        theTrigCaloCellMaker_jet_fullcalo = TrigCaloCellMaker_jet_fullcalo("CellMakerFullCalo_topo", doNoise=0, AbsE=True, doPers=True)
        
        #TrigCaloClusterMaker/TrigCaloClusterMaker_topo_fullscan
        from TrigCaloRec.TrigCaloRecConfig import TrigCaloClusterMaker_topo
        theTrigCaloClusterMaker_topo = TrigCaloClusterMaker_topo('TrigCaloClusterMaker_topo_fullscan', doMoments=True, doLC=True)
                
        #TrigHLTJetRec/TrigHLTJetRec_AntiKt04
        antiktsize = 0
        if ('a4' in self.chainPart['recoAlg']):
            antiktsize = 4
            from TrigHLTJetRec.TrigHLTJetRecConfig import TrigHLTJetRec_AntiKt04
            theTrigHLTJetRec_AntiKt = TrigHLTJetRec_AntiKt04()
        elif ('a10' in self.chainPart['recoAlg'] ):
            antiktsize = 10
            from TrigHLTJetRec.TrigHLTJetRecConfig import TrigHLTJetRec_AntiKt10
            theTrigHLTJetRec_AntiKt = TrigHLTJetRec_AntiKt10()
        else:
            logJetHTDef.error("NO HLT JET REC ALG GIVEN")
            theTrigHLTJetRec_AntiKt = None

        #TrigEFHTHypo/EFHT_HAD_ht500 -> uses ET cut on jets of 45 GeV!!
        efht_thresh = self.chainPart['threshold']
        from TrigJetHypo.TrigEFHTHypoConfig import EFHT_HAD
        theEFHThypo_ht =  EFHT_HAD("EFHT_HAD_ht%s"%efht_thresh,HT_cut=int(efht_thresh)*GeV)
        
        # Adding sequences
        self.L2sequenceList += [['', [theDummyRoiCreator], 'EF_full']]
        self.L2sequenceList += [[['EF_full'], [theTrigCaloCellMaker_jet_fullcalo, theTrigCaloClusterMaker_topo ], 'EF_full_cluster']]
        self.L2sequenceList += [[['EF_full_cluster'], [theTrigHLTJetRec_AntiKt], 'AntiKt']]

        self.L2sequenceList += [[['AntiKt'],[theEFHThypo_ht], 'jet_hypo']]

        # Adding signatures
        self.L2signatureList += [ [['EF_full']] ]
        self.EFsignatureList += [ [['EF_full_cluster']] ]
        self.EFsignatureList += [ [['AntiKt']] ]
        self.EFsignatureList += [ [['jet_hypo']] ]
        
        # Renaming TE
        self.TErenamingDict = {
            'AntiKt'        :  mergeRemovingOverlap('HLT_full__cluster__', 'jr_antikt'+str(antiktsize)+'tc_had' ),
            'jet_hypo'      :  mergeRemovingOverlap('HLT_full__cluster__', 'jr_antikt'+str(antiktsize)+'tc_had_ht'+efht_thresh ),
        }
