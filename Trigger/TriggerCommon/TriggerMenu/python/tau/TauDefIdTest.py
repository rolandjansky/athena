# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Tau slice signatures """

__author__  = "P.O. DeViveiros"
__version__ = "" 
__doc__="Implementation of Tau slice in new TM framework "

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenu.tau.TauDef")

from TriggerMenu.menu.HltConfig import L2EFChainDef, mergeRemovingOverlap

##########################################################
#
# Import and create fexes 
#
##########################################################



def L1InputTE(name):
    tmpte = name.split('_')[1]
    #tmpte = 'HA'+tmpte.strip('TAU')
    tmpte = tmpte.replace("TAU", "HA");
    log.debug('L1 input TE : %s', tmpte)
    return tmpte


class L2EFChain_tau(L2EFChainDef):

    def __init__(self, chainDict, theHypos):

        self.L2sequenceList   = []
        self.EFsequenceList   = []
        self.L2signatureList  = []
        self.EFsignatureList  = []
        self.TErenamingDict   = {}

        self.hypoProvider = theHypos

        self.chainPart = chainDict['chainParts']
        
        #if chainDict['stream'] == ['Combined']:
        #    self.chainL1Item = self.chainPart['L1item']
        #else:
        #    self.chainL1Item = chainDict['L1item']


        if self.chainPart['L1item']:
            self.chainL1Item = self.chainPart['L1item']
        else:
            self.chainL1Item = chainDict['L1item']            


        
        self.chainCounter = chainDict['chainCounter']       
        self.L2Name = 'L2_'+self.chainPart['chainPartName']
        self.EFName = 'EF_'+self.chainPart['chainPartName']
        self.chainName = chainDict['chainName']
        self.chainPartName = self.chainPart['chainPartName']

        self.L2InputTE = L1InputTE(self.chainL1Item)

        # Book-keeping for updating sequences
        self.currentItem = self.L2InputTE
        self.currentStep = 0

        self.setup_tauChain()
       
        L2EFChainDef.__init__(self, self.chainName, self.L2Name, self.chainCounter, chainDict['L1item'], self.EFName, self.chainCounter, self.L2InputTE)

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
#

    # Increment the step counter, set the proper adjusted TE name and return it
    # Also update the Signature list and TErenamingDict as it goes along
    def continueChain(self, level, identifier):
        lastLevel = self.currentItem.split('_')[0]
        if lastLevel != level:
            self.currentStep = 1

        self.currentItem = level+'_tau_step'+str(self.currentStep)

        if level=='L2':
            self.L2signatureList += [ [[self.currentItem]] ]
        elif level=='EF':
            self.EFsignatureList += [ [[self.currentItem]] ]

        self.TErenamingDict[self.currentItem] = mergeRemovingOverlap(level+'_', self.chainPartName+'_'+identifier)
            

        self.currentStep += 1
        return self.currentItem


    # Define the full tau chain
    # Self-configuring based on the dictionary parameters
    def setup_tauChain(self):

        threshold   = self.chainPart['threshold']
        selection   = self.chainPart['selection']
        preselection= self.chainPart['preselection']
        idperf      = "idperf" in self.chainPart['trkInfo']
        
        # Temporary hack to handle naming scheme
        if 'r1' in selection:
            preselection = 'r1'
            selection = selection.replace('r1', '')

        # Go in Sequence
        if preselection == 'r1':
            # Try new hypo extraction method
            theL2CaloHypo   = self.hypoProvider.GetHypo('L2', threshold, selection, 'calo', 'r1')
            theL2IDHypo     = self.hypoProvider.GetHypo('L2', threshold, selection, 'id', 'r1')
            theL2FinalHypo  = self.hypoProvider.GetHypo('L2', threshold, selection, '', 'r1')        
            # Get the necessary fexes
            from TrigT2CaloTau.TrigT2CaloTauConfig import T2CaloTau_Tau_Med
            from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_TauB
            from TrigT2IDTau.T2IDTauConfig import T2IDTau_Tau_1GeV_dZ02_dR0103
            from TrigT2Tau.T2TauFinalConfig import T2TauFinal_Tau_dR03_1GeV_dZ02

            t2calo_2012 = T2CaloTau_Tau_Med()
            l2sitrkfinder_tauB = TrigL2SiTrackFinder_TauB()
            t2id_2012 = T2IDTau_Tau_1GeV_dZ02_dR0103()
            t2final_2012 = T2TauFinal_Tau_dR03_1GeV_dZ02()

            if idperf:
                from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_TauA, TrigL2SiTrackFinder_TauB,TrigL2SiTrackFinder_TauC
                try:
                    from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_TauBC
                    l2sitrkfinder_tauBC = TrigL2SiTrackFinder_TauBC()
                except:
                    l2sitrkfinder_tauBC = None 
                l2sitrkfinder_tauA = TrigL2SiTrackFinder_TauA()
                l2sitrkfinder_tauC = TrigL2SiTrackFinder_TauC()
            
            # L2 configuration
            self.L2sequenceList += [[[ self.currentItem ],
                                     [t2calo_2012, theL2CaloHypo], 
                                     self.continueChain('L2', 'calo')]]
            
            if idperf:
                self.L2sequenceList += [[[ self.currentItem ],
                                         [l2sitrkfinder_tauA, l2sitrkfinder_tauB, l2sitrkfinder_tauC, l2sitrkfinder_tauBC, t2id_2012, theL2IDHypo], 
                                         self.continueChain('L2', 'id')]]
            else:
                self.L2sequenceList += [[[ self.currentItem ],
                                         [l2sitrkfinder_tauB, t2id_2012, theL2IDHypo], 
                                         self.continueChain('L2', 'id')]]
            
            self.L2sequenceList += [[[ self.currentItem ],
                                     [t2final_2012, theL2FinalHypo],
                                     self.continueChain('L2', 'l2final')]]


        if preselection == 'calo' or preselection == 'ptonly' or preselection == 'mvonly' or preselection == 'caloonly' or preselection == 'track' or preselection == 'trackonly':
            # Test 2015 approach
            log.info("Calo-based pre-selection configuration is not quite ready yet!")
            log.info("Very preliminary version!!")

            theHLTPre   = self.hypoProvider.GetHypo('L2', threshold, selection, 'calo', preselection)

            from TrigCaloRec.TrigCaloRecConfig import TrigCaloCellMaker_tau
            from TrigCaloRec.TrigCaloRecConfig import TrigCaloClusterMaker_topo

            cellmaker         = TrigCaloCellMaker_tau()
            clustermaker_topo = TrigCaloClusterMaker_topo()

            # Run topoclustering
            self.EFsequenceList += [[[ self.currentItem ],
                                     [cellmaker, clustermaker_topo],
                                     self.continueChain('EF', 'clf0')]]

            # Run the calo-based pre-selection
            self.EFsequenceList += [[[ self.currentItem ],
                                     [theHLTPre],
                                     self.continueChain('EF', 'calopre')]]


        if preselection == 'track' or preselection == 'trackonly':
    
            theHLTTrackPre   = self.hypoProvider.GetHypo('L2', threshold, selection, 'id', preselection)

            # Get the necessary fexes
            from InDetTrigRecExample.EFInDetConfig import  TrigEFIDSequence
            from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_Tau
    

            theTrigFastTrackFinder_Tau = TrigFastTrackFinder_Tau()
            theTrigEFIDDataPrep_Tau    = TrigEFIDSequence("Tau","tau","DataPrep").getSequence()


            ftracks = theTrigEFIDDataPrep_Tau+[theTrigFastTrackFinder_Tau]
            
            
            # Run fast-tracking
            self.EFsequenceList += [[[ self.currentItem ],
                                     ftracks,
                                     self.continueChain('EF', 'trfast')]]
            
            # Run the track-based pre-selection
            self.EFsequenceList += [[[ self.currentItem ],
                                     [theHLTTrackPre],
                                     self.continueChain('EF', 'trackpre')]]

        if preselection == 'r1':

            theEFHypo       = self.hypoProvider.GetHypo('EF', threshold, selection, '', 'r1')

            # Get the necessary fexes
            from TrigCaloRec.TrigCaloRecConfig import TrigCaloCellMaker_tau
            from TrigCaloRec.TrigCaloRecConfig import TrigCaloClusterMaker_topo
            from InDetTrigRecExample.EFInDetConfig import  TrigEFIDInsideOut_Tau
            from TrigTauRec.TrigTauRecConfig import TrigTauRecMerged_Tau2012
            from TrigTauDiscriminant.TrigTauDiscriGetter import TrigTauDiscriGetter

            cellmaker         = TrigCaloCellMaker_tau()
            clustermaker_topo = TrigCaloClusterMaker_topo()
            efidinsideout     = TrigEFIDInsideOut_Tau().getSequence()
            recmerged_2012    = TrigTauRecMerged_Tau2012()
            efmv              = TrigTauDiscriGetter()
                
            self.EFsequenceList += [[[ self.currentItem ], 
                                     [cellmaker, clustermaker_topo],
                                     self.continueChain('EF', 'clf0')]]
            
            self.EFsequenceList += [[[ self.currentItem ], 
                                     efidinsideout,
                                     self.continueChain('EF', 'tr')]]
            
            self.EFsequenceList += [[[ self.currentItem ], 
                                     [recmerged_2012, efmv, theEFHypo],
                                     self.continueChain('EF', 'effinal')]]

        if preselection == 'calo' or preselection == 'ptonly' or preselection == 'mvonly' or preselection == 'caloonly' or preselection == 'trackonly' or preselection == 'track':
            # Only run tracking and tau-rec : no need for topoclustering
            if preselection == 'caloonly' or preselection == 'trackonly':
                theEFHypo       = self.hypoProvider.GetHypo('EF', threshold, 'perf', '', 'r1')
            else: 
                theEFHypo       = self.hypoProvider.GetHypo('EF', threshold, selection, '', 'r1')

            # Get the necessary fexes
            from InDetTrigRecExample.EFInDetConfig import  TrigEFIDSequence
            from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_Tau
            from TrigTauRec.TrigTauRecConfig import TrigTauRecMerged_Tau2012
            from TrigTauDiscriminant.TrigTauDiscriGetter import TrigTauDiscriGetter2015

            theTrigFastTrackFinder_Tau = TrigFastTrackFinder_Tau()
            theTrigEFIDDataPrep_Tau            = TrigEFIDSequence("Tau","tau","DataPrep").getSequence()
            theTrigEFIDInsideOutMerged_Tau     = TrigEFIDSequence("Tau","tau","InsideOutMerged").getSequence()

            efidinsideout     = theTrigEFIDDataPrep_Tau+[theTrigFastTrackFinder_Tau]+theTrigEFIDInsideOutMerged_Tau
            recmerged_2012    = TrigTauRecMerged_Tau2012()
            efmv              = TrigTauDiscriGetter2015()


            self.EFsequenceList += [[[ self.currentItem ],
                                     efidinsideout,
                                     self.continueChain('EF', 'tr')]]

            if not idperf:
                self.EFsequenceList += [[[ self.currentItem ],
                                         [recmerged_2012, efmv, theEFHypo],
                                         self.continueChain('EF', 'effinal')]]
        
        ########### Signatures ###########
        # Commented out: Now done automatically through the continueChain function
        
