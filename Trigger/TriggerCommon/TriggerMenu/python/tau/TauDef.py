# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Tau slice signatures """

__author__  = "P.O. DeViveiros"
__version__ = "" 
__doc__="Implementation of Tau slice in new TM framework "

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
logTauDef = logging.getLogger("TriggerMenu.tau.TauDef")


from TriggerMenu.menu.HltConfig import L2EFChainDef, mergeRemovingOverlap
from AthenaCommon.SystemOfUnits import GeV

####################
# Helper functions #
####################

def L1InputTE(name):
    tmpte = name.split('_')[1]
    #tmpte = 'HA'+tmpte.strip('TAU')
    tmpte = tmpte.replace("TAU", "HA");
    logTauDef.debug('L1 input TE : %s'%tmpte)
    return tmpte

##############
# Main Class #
##############

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

        selection    = self.chainPart['selection']
        preselection = self.chainPart['preselection']

        if 'r1' in selection or 'r1' in preselection:
            # Run-I set-up
            self.setup_tauChainRunOne()
        else:
            # Run-II set-up
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
        calibration = self.chainPart['calib']
        recoAlg     = self.chainPart['recoAlg'] 
        selection   = self.chainPart['selection']
        preselection= self.chainPart['preselection']
        idperf      = "idperf" in self.chainPart['trkInfo']


        # Cleaner if-statements
        # Strategies which need calorimeter pre-selection
        needsCaloPre  = ['calo', 'ptonly', 'mvonly', 'caloonly',
                         'track', 'trackonly', 'tracktwo']
        # Strategies which need fast-track finding
        needsTrackPre = ['track', 'trackonly']
        # Strategies which need Run-II final hypo
        needsRun2Hypo = ['calo', 'ptonly', 'mvonly', 'caloonly',
                         'trackonly', 'track', 'tracktwo']

        # Temporary hack to handle naming scheme
        if 'r1' in selection:
            preselection = 'r1'
            selection = selection.replace('r1', '')
    
        # Overrule the final EF selection
        if idperf:
            selection = 'perf'

        if preselection in needsCaloPre:
            # Run-II calo-based approach
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
            if selection != 'perf':
                self.EFsequenceList += [[[ self.currentItem ],
                                         [theHLTPre],
                                         self.continueChain('EF', 'calopre')]]
                
        # One step fast-tracking
        if preselection in needsTrackPre or (preselection != 'r1' and idperf):
            # Run-II tracking-based approach
            theHLTTrackPre   = self.hypoProvider.GetHypo('L2', threshold, selection, 'id', preselection)

            # Get the necessary fexes
            from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
            [trkcore, trkprec] = TrigInDetSequence("Tau", "tau", "IDTrig").getSequence()
            ftracks = trkcore
            
            # Run fast-tracking
            self.EFsequenceList += [[[ self.currentItem ],
                                     ftracks,
                                     self.continueChain('EF', 'trfast')]]
            
            # Run the track-based pre-selection
            # Only cut if we're not in idperf
            if not idperf:
                self.EFsequenceList += [[[ self.currentItem ],
                                         [theHLTTrackPre],
                                         self.continueChain('EF', 'trackpre')]]
             
            # Run TrigTauRec to store pre-selected taus
            from TrigTauRec.TrigTauRecConfig import TrigTauRecMerged_TauPreselection
            recPreselection = TrigTauRecMerged_TauPreselection()

            self.EFsequenceList += [[[ self.currentItem ],
                                     [recPreselection],
                                     self.continueChain('EF', 'storepre')]]

        # Two step fast-tracking
        if preselection == 'tracktwo':

            theHLTTrackPre   = self.hypoProvider.GetHypo('L2', threshold, selection, 'id', preselection)

            # Get the necessary fexes
            from TrigInDetConf.TrigInDetSequence import TrigInDetSequence

            [trkcore, trkiso, trkprec] = TrigInDetSequence("Tau", "tau", "IDTrig", "2step").getSequence()

            # Here we load our new tau-specific RoI Updater
            from TrigTauHypo.TrigTauHypoConf import HLTTauTrackRoiUpdater
            tauRoiUpdater = HLTTauTrackRoiUpdater()
            # This will add up to a tolerance of 5 mm
            # due to the extra 3mm tolerance from the FTF
            tauRoiUpdater.z0HalfWidth = 2.0
            # Cut in GeV
            tauRoiUpdater.minTrackPt = 1.0

            ftracks = trkcore+[tauRoiUpdater]+trkiso

            # Run fast-tracking
            self.EFsequenceList += [[[ self.currentItem ],
                                     ftracks,
                                     self.continueChain('EF', 'trfasttwo')]]

            # Run the track-based pre-selection
            # Only cut if we're not in idperf
            if not idperf:
                self.EFsequenceList += [[[ self.currentItem ],
                                         [theHLTTrackPre],
                                         self.continueChain('EF', 'trackpre')]]

            # Need a fix from offline tauRec
            from TrigTauRec.TrigTauRecConfig import TrigTauRecMerged_TauPreselection
            recPreselection = TrigTauRecMerged_TauPreselection()

            self.EFsequenceList += [[[ self.currentItem ],
                                     [recPreselection],
                                     self.continueChain('EF', 'storepre')]]

        if preselection in needsRun2Hypo:
            # Only run tracking and tau-rec : no need for topoclustering
            if preselection == 'caloonly' or preselection == 'trackonly' or selection == 'cosmic':
                theEFHypo       = self.hypoProvider.GetHypo('EF', threshold, 'perf', '', 'r1')
            else: 
                theEFHypo       = self.hypoProvider.GetHypo('EF', threshold, selection, '', 'r1')

            # Get the necessary fexes
            from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
            from TrigTauDiscriminant.TrigTauDiscriGetter import TrigTauDiscriGetter2015

            # Change track selection if we're running on cosmics...
            if selection == 'cosmic':
                from TrigTauRec.TrigTauRecCosmicsConfig import TrigTauRecCosmics_Tau2012
                recmerged_2012    = TrigTauRecCosmics_Tau2012()
            else:
                from TrigTauRec.TrigTauRecConfig import TrigTauRecMerged_Tau2012
                recmerged_2012    = TrigTauRecMerged_Tau2012()

            [trkcore, trkprec] = TrigInDetSequence("Tau", "tau", "IDTrig").getSequence()

            efidinsideout = trkprec

            # Only run the fast-tracking if it wasn't run at pre-selection
            if preselection != 'track' and preselection != 'trackonly' and not idperf:
                efidinsideout = trkcore+trkprec

            efmv              = TrigTauDiscriGetter2015()

            # Precision tracking
            self.EFsequenceList += [[[ self.currentItem ],
                                     efidinsideout,
                                     self.continueChain('EF', 'tr')]]
            # TrigTauRec, BDT and Hypo
            self.EFsequenceList += [[[ self.currentItem ],
                                     [recmerged_2012, efmv, theEFHypo],
                                     self.continueChain('EF', 'effinal')]]

    def setup_tauChainRunOne(self):

        threshold   = self.chainPart['threshold']
        calibration = self.chainPart['calib']
        recoAlg     = self.chainPart['recoAlg']
        selection   = self.chainPart['selection']
        preselection= self.chainPart['preselection']
        idperf      = "idperf" in self.chainPart['trkInfo']

        # Handle Run-II naming scheme
        if 'r1' in selection:
            preselection = 'r1'
            selection = selection.replace('r1', '')

        # Overrule the final EF selection
        if idperf:
            selection = 'perf'    

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
                from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_TauA,TrigL2SiTrackFinder_TauB,TrigL2SiTrackFinder_TauC
                l2sitrkfinder_tauA = TrigL2SiTrackFinder_TauA()
                l2sitrkfinder_tauC = TrigL2SiTrackFinder_TauC()

            # L2 configuration
            self.L2sequenceList += [[[ self.currentItem ],
                                     [t2calo_2012, theL2CaloHypo],
                                     self.continueChain('L2', 'calo')]]

            if idperf:
                self.L2sequenceList += [[[ self.currentItem ],
                                         [l2sitrkfinder_tauA, l2sitrkfinder_tauB, l2sitrkfinder_tauC, t2id_2012, theL2IDHypo],
                                         self.continueChain('L2', 'id')]]
            else:
                self.L2sequenceList += [[[ self.currentItem ],
                                         [l2sitrkfinder_tauB, t2id_2012, theL2IDHypo],
                                         self.continueChain('L2', 'id')]]

            self.L2sequenceList += [[[ self.currentItem ],
                                     [t2final_2012, theL2FinalHypo],
                                     self.continueChain('L2', 'l2final')]]

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
            

    # Prototype for TwoStep configuration
    def setup_tauChainTwoStep(self):

        threshold   = self.chainPart['threshold']
        calibration = self.chainPart['calib']
        recoAlg     = self.chainPart['recoAlg'] 
        selection   = self.chainPart['selection']
        preselection= self.chainPart['preselection']
        idperf      = "idperf" in self.chainPart['trkInfo']
        
        # Overrule the final EF selection
        if idperf:
            selection = 'perf'

        if preselection == 'calo' or preselection == 'ptonly' or preselection == 'mvonly' or preselection == 'caloonly' or preselection == 'track' or preselection == 'trackonly':
            # Test 2015 approach
            logTauDef.info("Calo-based pre-selection configuration is not quite ready yet!")
            logTauDef.info("Very preliminary version!!")

            theHLTPre   = self.hypoProvider.GetHypo('L2', threshold, selection, 'calo', preselection)

            from TrigCaloRec.TrigCaloRecConfig import TrigCaloCellMaker_tau
            from TrigCaloRec.TrigCaloRecConfig import TrigCaloClusterMaker_topo

            cellmaker         = TrigCaloCellMaker_tau()
            clustermaker_topo = TrigCaloClusterMaker_topo()

            # Run topoclustering
            self.EFsequenceList += [[[ self.currentItem ],
                                     [cellmaker, clustermaker_topo],
                                     self.continueChain('EF', 'clf0')]]
                

        if preselection == 'track' or preselection == 'trackonly' or (preselection != 'r1' and idperf):
    
            theHLTTrackPre  = self.hypoProvider.GetHypo('L2', threshold, selection, 'id', preselection)

            # Get the necessary fexes
            from InDetTrigRecExample.EFInDetConfig import  TrigEFIDSequence
            from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
            from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_Tau
    

            theTrigFastTrackFinder_Tau = TrigFastTrackFinder_Tau()
            theTrigEFIDDataPrep_Tau    = TrigEFIDSequence("Tau","tau","DataPrep").getSequence()
            theFastTrackFinderxAOD     = TrigInDetSequence("Tau","tau","FastxAOD").getSequence()


            ftracks = theTrigEFIDDataPrep_Tau+[theTrigFastTrackFinder_Tau]+theFastTrackFinderxAOD
            
            
            # Run fast-tracking
            self.EFsequenceList += [[[ self.currentItem ],
                                     ftracks,
                                     self.continueChain('EF', 'trfast')]]
            
        # Here we're running the TrigTauRec based on all the stuff that ran before.  Uh-oh, this is dangerous...
        # For now, assume fast-tracking is always run
        from TrigTauRec.TrigTauRecConfig import TrigTauRecMerged_TauPreselection
        
        recPreselection = TrigTauRecMerged_TauPreselection()

        self.EFsequenceList += [[[ self.currentItem ],
                                 recPreselection,
                                 self.continueChain('EF', 'prefinal')]]


        # Insert generic hypothesis
        
        
        
        if preselection == 'calo' or preselection == 'ptonly' or preselection == 'mvonly' or preselection == 'caloonly' or preselection == 'trackonly' or preselection == 'track':
            # Only run tracking and tau-rec : no need for topoclustering
            if preselection == 'caloonly' or preselection == 'trackonly' or selection == 'cosmic':
                theEFHypo       = self.hypoProvider.GetHypo('EF', threshold, 'perf', '', 'r1')
            else: 
                theEFHypo       = self.hypoProvider.GetHypo('EF', threshold, selection, '', 'r1')

            # Get the necessary fexes
            from InDetTrigRecExample.EFInDetConfig import  TrigEFIDSequence
            from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
            from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_Tau
            from TrigTauDiscriminant.TrigTauDiscriGetter import TrigTauDiscriGetter2015

            # Change track selection if we're running on cosmics...
            if selection == 'cosmic':
                from TrigTauRec.TrigTauRecCosmicsConfig import TrigTauRecCosmics_Tau2012
                recmerged_2012    = TrigTauRecCosmics_Tau2012()
            else:
                from TrigTauRec.TrigTauRecConfig import TrigTauRecMerged_TauPrecision
                recmerged_2012    = TrigTauRecMerged_TauPrecision()

            # Only run the fast-tracking if it wasn't run at pre-selection
            if preselection != 'track' and preselection != 'trackonly' and not idperf:
                theTrigFastTrackFinder_Tau = TrigFastTrackFinder_Tau()
                theTrigEFIDDataPrep_Tau            = TrigEFIDSequence("Tau","tau","DataPrep").getSequence()

            theTrigEFIDInsideOutMerged_Tau     = TrigEFIDSequence("Tau","tau","InsideOutMerged").getSequence()
            theFastTrackFinderxAOD             = TrigInDetSequence("Tau","tau","FastxAOD").getSequence()


            if preselection != 'track' and preselection != 'trackonly' and not idperf:
                efidinsideout     = theTrigEFIDDataPrep_Tau+[theTrigFastTrackFinder_Tau]+theFastTrackFinderxAOD+theTrigEFIDInsideOutMerged_Tau                
            else:
                efidinsideout     = theTrigEFIDInsideOutMerged_Tau
            
            efmv              = TrigTauDiscriGetter2015()


            self.EFsequenceList += [[[ self.currentItem ],
                                     efidinsideout,
                                     self.continueChain('EF', 'tr')]]

            self.EFsequenceList += [[[ self.currentItem ],
                                     [recmerged_2012, efmv, theEFHypo],
                                     self.continueChain('EF', 'effinal')]]
        
