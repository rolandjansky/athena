# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Tau slice signatures """

__authors__  = "P.O. DeViveiros, FTK modifications by P. McNamara"
__version__ = "" 
__doc__="Implementation of Tau slice in new TM framework\n now with FTK chains"

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

from InDetTrigRecExample.EFInDetConfig import TrigEFIDInsideOut_Tau
from TrigCaloRec.TrigCaloRecConfig import TrigCaloCellMaker_tau, TrigCaloClusterMaker_topo
from TrigFTK_RecAlgs.TrigFTK_RecAlgs_Config import TrigFTK_VxPrimary_EF
from TrigInDetConf.TrigInDetFTKSequence import TrigInDetFTKSequence
from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import (TrigL2SiTrackFinder_TauA,
                                                            TrigL2SiTrackFinder_TauB,
                                                            TrigL2SiTrackFinder_TauC)
from TrigT2CaloTau.TrigT2CaloTauConfig import T2CaloTau_Tau_Med
from TrigT2IDTau.T2IDTauConfig import T2IDTau_Tau_1GeV_dZ02_dR0103
from TrigT2Tau.T2TauFinalConfig import T2TauFinal_Tau_dR03_1GeV_dZ02
from TrigTauDiscriminant.TrigTauDiscriGetter import TrigTauDiscriGetter, TrigTauDiscriGetter2015
from TrigTauHypo.TrigTauHypoBase import HLTTrackTauHypo_rejectNoTracks
from TrigTauHypo.TrigTauHypoConf import HLTTauCaloRoiUpdater, HLTTauTrackRoiUpdater
from TrigTauRec.TrigTauRecConfig import (TrigTauRecMerged_Tau2012,
                                         TrigTauRecMerged_TauCaloOnly,
                                         TrigTauRecMerged_TauFTK,
                                         TrigTauRecMerged_TauPrecision,
                                         TrigTauRecMerged_TauPreselection,
                                         TrigTauRecMerged_TauPreselectionMva)
from TrigTauRec.TrigTauRecCosmicsConfig import TrigTauRecCosmics_Tau2012
from TriggerMenu.menu.HltConfig import L2EFChainDef, mergeRemovingOverlap


####################
# Helper functions #
####################

def L1InputTE(name):
    tmpte = name.split('_')[1]
    #tmpte = 'HA'+tmpte.strip('TAU')
    tmpte = tmpte.replace("TAU", "HA");
    log.debug('L1 input TE : %s', tmpte)
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
    # create calorimeter preselection sequence without TrigTauRec and Hypo sequences
    def addCaloSequence(self,threshold,selection,preselection):
        # Run-II calo-based approach

        cellmaker         = TrigCaloCellMaker_tau()
        clustermaker_topo = TrigCaloClusterMaker_topo()
        caloroiupdater    = HLTTauCaloRoiUpdater()

        # Run topoclustering
        self.EFsequenceList += [[[ self.currentItem ],
                                 [cellmaker, clustermaker_topo, caloroiupdater],
                                 self.continueChain('L2', 'clf0')]]
        
    #create the TrigTauRec Calorimeter only sequence    
    def addTrigTauRecCaloOnlySequence(self,threshold,selection,preselection):
        
        # Run TrigTauRec, calorimeter only (to get proper calibration, and cell-based vars)
        caloRec = TrigTauRecMerged_TauCaloOnly()

        self.EFsequenceList += [[[ self.currentItem ],
                                 [caloRec],
                                 self.continueChain('L2', 'calorec')]]
        
    #create the Calorimeter hypo (selection) sequence    
    def addCaloHypoSequence(self,threshold,selection,preselection):    
        theHLTPre   = self.hypoProvider.GetHypo('L2', threshold, selection, 'calo', preselection)
        # Run the calo-based pre-selection
        self.EFsequenceList += [[[ self.currentItem ],
                                 [theHLTPre],
                                 self.continueChain('L2', 'calopre')]]
        
    
    
    #create the tracking sequence and tracking selection sequence    
    def addTrackingSequence(self,threshold,selection,preselection,idperf,trkprec):        
        # Get the necessary fexes
        if preselection == 'FTK':
            # use [:] so the list trkprec is modified by this function
            [trkfast, trkprec[:]] = TrigInDetFTKSequence("Tau","tau",sequenceFlavour=["PT"]).getSequence()
        elif preselection == 'FTKRefit':
            [trkfast, trkprec[:]] = TrigInDetFTKSequence("Tau","tau",sequenceFlavour=["refit","PT"]).getSequence()
        elif preselection == 'FTKNoPrec':
            [trkfast] = TrigInDetFTKSequence("Tau","tau",sequenceFlavour=[""]).getSequence()
            [trkprec[:]] = TrigInDetFTKSequence("Tau","tau",sequenceFlavour=["refit"]).getSequence()
        else:
            [trkfast, trkprec[:]] = TrigInDetSequence("Tau", "tau", "IDTrig").getSequence()
        # Use cosmic-specific tracking algorithm
        if selection == 'cosmic':
            [trkfast] = TrigInDetSequence("Cosmics", "cosmics", "IDTrig", sequenceFlavour=["FTF"]).getSequence()

        # Run fast-tracking
        self.EFsequenceList += [[[ self.currentItem ],
                                 trkfast,
                                 self.continueChain('L2', 'trfast')]]
    
    def addTrackingSelectionSequence(self,threshold,selection,preselection,idperf,trkprec):
        # Run-II tracking-based approach
        theHLTTrackPre   = self.hypoProvider.GetHypo('L2', threshold, selection, 'id', preselection)  
        # Run the track-based pre-selection
        # Only cut if we're not in idperf
        if not idperf:
            self.EFsequenceList += [[[ self.currentItem ],
                                     [theHLTTrackPre],
                                     self.continueChain('L2', 'trackpre')]]
            
    #create the vertexing selection sequence        
    def addVertexSequence(self,threshold,selection,preselection,idperf):  
        #import FEX algorithm  
        theTrigFTK_VxPrimary_EF = TrigFTK_VxPrimary_EF("TauFTKVertex", "Tau")
        theTrigFTK_VxPrimary_EF.useRawTracks = False
        theTrigFTK_VxPrimary_EF.useRefittedTracks = False

        # to allow compatibility between release 20.11 and 21
#        from TrigInDetConf.TrigInDetSequence import vertexXAODCnvNeeded 
#        if vertexXAODCnvNeeded():
#           theTrigFTK_VxPrimary_EF.vxContainerName = 'PrimVxFTK'
#           theInDet__TrigVertexxAODCnv = InDet__TrigVertexxAODCnv(name="FTKTauVtxConversion")
#           theInDet__TrigVertexxAODCnv.InputVxContainerKey = 'PrimVxFTK'
#           theInDet__TrigVertexxAODCnv.OutputVxContainerKey = 'PrimVertexFTK'
#           vertexAlgorithms = [theTrigFTK_VxPrimary_EF, theInDet__TrigVertexxAODCnv]
#           self.EFsequenceList += [[[ self.currentItem ],vertexAlgorithms,self.continueChain('L2', 'vertex')]]          
#        else:
        theTrigFTK_VxPrimary_EF.vertexContainerName= 'PrimVertexFTK'
        vertexAlgorithms = [theTrigFTK_VxPrimary_EF]
        self.EFsequenceList += [[[ self.currentItem ], vertexAlgorithms, self.continueChain('L2', 'vertex')]]
        
    #create the TrigTauRec preselection sequence       
    def addTrigTauRecTauPreselectionSequence(self,threshold,selection,preselection,idperf):              
        # Run TrigTauRec to store pre-selected taus
        if 'mva' in preselection:
            recPreselection = TrigTauRecMerged_TauPreselectionMva()
        else:
            recPreselection = TrigTauRecMerged_TauPreselection()
            
        self.EFsequenceList += [[[ self.currentItem ],
                                 [recPreselection],
                                 self.continueChain('L2', 'storepre')]]
    
    #create the TrigTauRec FTK preselection sequence    
    def addTrigTauRecTauFTKSequence(self,threshold,selection,preselection,idperf):            
        # Run TrigTauRec to store pre-selected taus
        recPreselection = TrigTauRecMerged_TauFTK()

        self.EFsequenceList += [[[ self.currentItem ],
                                 [recPreselection],
                                 self.continueChain('L2', 'storepre')]]
        
    #create the two step tracking sequences
    def addTwoStepTrackingSequence(self,threshold,selection,preselection,idperf,trkprec): 
        # Get the necessary fexes
        # use [:] so the list trkprec is modified by this function
        [trkcore, trkiso, trkprec[:]] = TrigInDetSequence("Tau", "tau", "IDTrig", sequenceFlavour=["2step"]).getSequence()

        # Get the HLTTrackTauHypo_rejectNoTracks
        tauRejectEmpty = HLTTrackTauHypo_rejectNoTracks("TauRejectEmpty")

        # Here we load our new tau-specific RoI Updater
        tauRoiUpdater = HLTTauTrackRoiUpdater()
        # This will add up to a tolerance of 5 mm due to the extra 3mm tolerance from the FTF
        # tauRoiUpdater.z0HalfWidth = 2.0 # Temporarily widened to 10 mm
        tauRoiUpdater.z0HalfWidth = 7.0

        #ftracks = trkcore+[tauRoiUpdater]+trkiso
        if not idperf:
            ftracks = trkcore + [tauRoiUpdater, tauRejectEmpty] + trkiso
        else :
            ftracks = trkcore+[tauRoiUpdater]+trkiso

        # Run fast-tracking
        self.EFsequenceList += [[[ self.currentItem ],
                                 ftracks,
                                 self.continueChain('L2', 'trfasttwo')]]
        
    def addTwoStepTrackingSelectionSequence(self,threshold,selection,preselection,idperf): 
        theHLTTrackPre   = self.hypoProvider.GetHypo('L2', threshold, selection, 'id', preselection)
        
        # Run the track-based pre-selection
        # Only cut if we're not in idperf
        if not idperf:
            self.EFsequenceList += [[[ self.currentItem ],
                                     [theHLTTrackPre],
                                     self.continueChain('L2', 'trackpre')]]




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


        # Cleaner if-statements
        # Strategies which need calorimeter pre-selection
        needsCaloPre  = ['calo', 'ptonly', 'mvonly', 'caloonly',
                         'track', 'trackonly', 'tracktwo',
                         'trackcalo', 'tracktwocalo','tracktwo2015', 'tracktwomva']
        # Strategies which need fast-track finding
        needsTrackTwoPre = ['tracktwo', 'tracktwoonly', 'tracktwocalo','tracktwo2015', 'tracktwomva']
        needsTrackPre    = ['track', 'trackonly', 'trackcalo', 'FTK', 'FTKRefit', 'FTKNoPrec']
        # Strategies which need Run-II final hypo
        needsRun2Hypo = ['calo', 'ptonly', 'mvonly', 'caloonly',
                         'trackonly', 'track', 'tracktwo', 'tracktwocalo', 'trackcalo', 'FTK', 'FTKRefit', 'FTKNoPrec', 'tracktwo2015', 'tracktwomva']
        fastTrackingUsed = needsTrackPre + needsTrackTwoPre
        
        #Set the default values
        [trkcore, trkprec] = TrigInDetSequence("Tau", "tau", "IDTrig").getSequence()

        # Temporary hack to handle naming scheme
        if 'r1' in selection:
            preselection = 'r1'
            selection = selection.replace('r1', '')
    
        # Overrule the final EF selection
        if idperf:
            selection = 'perf'

        #Create FTK chain or other chains
        if preselection == 'FTK' or preselection == 'FTKRefit' or preselection == 'FTKNoPrec':           
            self.addCaloSequence(threshold, selection, preselection)
            self.addTrackingSequence(threshold,selection,preselection,idperf,trkprec)
            self.addVertexSequence(threshold,selection,preselection,idperf)
            self.addTrigTauRecTauFTKSequence(threshold,selection,preselection,idperf)
            self.addTwoStepTrackingSelectionSequence(threshold,selection,preselection,idperf)
            self.addCaloHypoSequence(threshold,selection,preselection)
        else:
            # Calorimeter
            if preselection in needsCaloPre:
                self.addCaloSequence(threshold, selection, preselection)
                self.addTrigTauRecCaloOnlySequence(threshold,selection,preselection)
                self.addCaloHypoSequence(threshold,selection,preselection)
            # Two step fast-tracking
            if preselection in needsTrackTwoPre:
                self.addTwoStepTrackingSequence(threshold,selection,preselection,idperf, trkprec)
                if preselection in ('tracktwo', 'tracktwomva'):
                    self.addTrigTauRecTauPreselectionSequence(threshold,selection,preselection,idperf)
                    self.addTwoStepTrackingSelectionSequence(threshold,selection,preselection,idperf)
                else:
                    self.addTwoStepTrackingSelectionSequence(threshold,selection,preselection,idperf)
                    self.addTrigTauRecTauPreselectionSequence(threshold,selection,preselection,idperf)
            # One step fast-tracking
            if preselection in needsTrackPre:
                self.addTrackingSequence(threshold,selection,preselection,idperf,trkprec)
                self.addTrackingSelectionSequence(threshold,selection,preselection,idperf,trkprec) 
                self.addTrigTauRecTauPreselectionSequence(threshold,selection,preselection,idperf)

        if preselection in needsRun2Hypo:
            # Only run tracking and tau-rec : no need for topoclustering
            if preselection == 'caloonly' or preselection == 'trackonly' or selection == 'cosmic':
                theEFHypo       = self.hypoProvider.GetHypo('EF', threshold, 'perf', '', 'r1')
            else: 
                theEFHypo       = self.hypoProvider.GetHypo('EF', threshold, selection, '', 'r1')


            # Change track selection if we're running on cosmics...
            if selection == 'cosmic':
                recmerged_2012    = TrigTauRecCosmics_Tau2012()
            else:
                recmerged_2012    = TrigTauRecMerged_TauPrecision()

            efidinsideout = trkprec

            # Only run the fast-tracking if it wasn't run at pre-selection
            # Is two-step preselection good enough?
            if preselection not in fastTrackingUsed:
                efidinsideout = trkcore+trkprec

            # Precision tracking
            self.EFsequenceList += [[[ self.currentItem ],
                                     efidinsideout,
                                     self.continueChain('EF', 'tr')]]

            # TrigTauRec and Hypo (no BDT)
            if selection == 'kaonpi1' or selection == 'kaonpi2' or selection == 'dipion1' or selection=='dipion1loose' or selection == 'dipion2' or selection == 'dipion3' or selection == 'dikaon' or selection == 'dikaontight' or selection == 'dikaonmass' or selection == 'dikaonmasstight' or selection == 'singlepion' or selection == 'singlepiontight':
                self.EFsequenceList += [[[ self.currentItem ],
                                         [recmerged_2012, theEFHypo],
                                         self.continueChain('EF', 'effinal')]]                
            else:
            # TrigTauRec, BDT and Hypo
                efmv              = TrigTauDiscriGetter2015()
                self.EFsequenceList += [[[ self.currentItem ],
                                         [recmerged_2012, efmv, theEFHypo],
                                         self.continueChain('EF', 'effinal')]]

    def setup_tauChainRunOne(self):
        
        threshold   = self.chainPart['threshold']
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

            t2calo_2012 = T2CaloTau_Tau_Med()
            l2sitrkfinder_tauB = TrigL2SiTrackFinder_TauB()
            t2id_2012 = T2IDTau_Tau_1GeV_dZ02_dR0103()
            t2final_2012 = T2TauFinal_Tau_dR03_1GeV_dZ02()

            if idperf:
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
        selection   = self.chainPart['selection']
        preselection= self.chainPart['preselection']
        idperf      = "idperf" in self.chainPart['trkInfo']
        
        # Overrule the final EF selection
        if idperf:
            selection = 'perf'

        if preselection == 'calo' or preselection == 'ptonly' or preselection == 'mvonly' or preselection == 'caloonly' or preselection == 'track' or preselection == 'trackonly':
            # Test 2015 approach
            log.info("Calo-based pre-selection configuration is not quite ready yet!")
            log.info("Very preliminary version!!")


            cellmaker         = TrigCaloCellMaker_tau()
            clustermaker_topo = TrigCaloClusterMaker_topo()

            # Run topoclustering
            self.EFsequenceList += [[[ self.currentItem ],
                                     [cellmaker, clustermaker_topo],
                                     self.continueChain('EF', 'clf0')]]
                

        if preselection == 'track' or preselection == 'trackonly' or (preselection != 'r1' and idperf):
    
            # Get the necessary fexes
            [trkfast, trkprec] = TrigInDetSequence("Tau", "tau", "IDTrig").getSequence()
            
            # Run fast-tracking
            self.EFsequenceList += [[[ self.currentItem ],
                                     trkfast,
                                     self.continueChain('EF', 'trfast')]]
            
        # Here we're running the TrigTauRec based on all the stuff that ran before.  Uh-oh, this is dangerous...
        # For now, assume fast-tracking is always run
        
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
            [trkfast, trkprec] = TrigInDetSequence("Tau", "tau", "IDTrig").getSequence()


            # Change track selection if we're running on cosmics...
            if selection == 'cosmic':
                recmerged_2012    = TrigTauRecCosmics_Tau2012()
            else:
                recmerged_2012    = TrigTauRecMerged_TauPrecision()

            # Only run the fast-tracking if it wasn't run at pre-selection
            if preselection != 'track' and preselection != 'trackonly' and not idperf:
                efidinsideout     = trkfast+trkprec
            else:
                efidinsideout     = trkprec
            
            efmv              = TrigTauDiscriGetter2015()


            self.EFsequenceList += [[[ self.currentItem ],
                                     efidinsideout,
                                     self.continueChain('EF', 'tr')]]

            self.EFsequenceList += [[[ self.currentItem ],
                                     [recmerged_2012, efmv, theEFHypo],
                                     self.continueChain('EF', 'effinal')]]
