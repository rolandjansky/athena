# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

""" Tau slice signatures """

__authors__  = "P.O. DeViveiros, FTK modifications by P. McNamara"
__version__ = "" 
__doc__="Implementation of Tau slice in new TM framework\n now with FTK chains"

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

from TrigCaloRec.TrigCaloRecConfig import TrigCaloCellMaker_tau, TrigCaloClusterMaker_topo
from TrigFTK_RecAlgs.TrigFTK_RecAlgs_Config import TrigFTK_VxPrimary_EF
from TrigInDetConf.TrigInDetFTKSequence import TrigInDetFTKSequence
from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
from TrigTauHypo.TrigTauHypoBase import HLTTrackTauHypo_rejectNoTracks
from TrigTauHypo.TrigTauHypoConf import HLTTauCaloRoiUpdater, HLTTauTrackRoiUpdater
from TrigTauRec.TrigTauRecConfig import (TrigTauRecMerged_TauCaloOnly,
                                         TrigTauRecMerged_TauCaloOnlyMVA,
                                         TrigTauRecMerged_TauFTK,
                                         TrigTauRecMerged_TauPrecision,
                                         TrigTauRecMerged_TauPreselection,
                                         TrigTauRecMerged_TauPrecisionMVA)
from TrigTauRec.TrigTauRecCosmicsConfig import TrigTauRecCosmics_Tau2012
from TriggerMenu.menu.HltConfig import L2EFChainDef, mergeRemovingOverlap


####################
# Helper functions #
####################

def L1InputTE(name):
    tmpte = name.split('_')[1]
    #tmpte = 'HA'+tmpte.strip('TAU')
    tmpte = tmpte.replace("TAU", "HA")
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

        # Run-I setup no longer supported
        assert('r1' not in selection and 'r1' not in preselection), "Run-1 chains no longer supported! " + self.chainName

        # Run-II setup
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

    #create the TrigTauRec Calorimeter only sequence    
    def addTrigTauRecCaloOnlyMVASequence(self,threshold,selection,preselection):  # MVA TES (ATR-17687)

        # Run TrigTauRec, calorimeter only (to get proper calibration, and cell-based vars)
        caloRec = TrigTauRecMerged_TauCaloOnlyMVA()

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
        theTrigFTK_VxPrimary_EF.vertexContainerName= 'PrimVertexFTK'
        vertexAlgorithms = [theTrigFTK_VxPrimary_EF]

        self.EFsequenceList += [[[ self.currentItem ], vertexAlgorithms, self.continueChain('L2', 'vertex')]]
        
    #create the TrigTauRec preselection sequence       
    def addTrigTauRecTauPreselectionSequence(self,threshold,selection,preselection,idperf):              
        # Run TrigTauRec to store pre-selected taus
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
        tauRoiUpdater.z0HalfWidth = 7.0

        #ftracks = trkcore+[tauRoiUpdater]+trkiso
        if not idperf and preselection != 'tracktwoMVA':
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
        needsCaloPre  = ['calo', 'ptonly',
                         'track', 'tracktwo', 'tracktwoEF']
        needsCaloMVAPre = ['tracktwoMVA', 'tracktwoEFmvaTES' ]
        # Strategies which need fast-track finding
        needsTrackTwoPre = ['tracktwo', 'tracktwoonly']
        needsTrackTwoNoPre = ['tracktwoEF','tracktwoMVA', 'tracktwoEFmvaTES']
        needsTrackPre    = ['track', 'FTK', 'FTKRefit', 'FTKNoPrec']
        # Strategies which need Run-II final hypo
        needsRun2Hypo = ['calo', 'ptonly', 'track', 'tracktwo', 'tracktwoEF', 'tracktwoMVA', 'FTK', 'FTKRefit', 'FTKNoPrec', 'tracktwoEFmvaTES']
        fastTrackingUsed = needsTrackPre + needsTrackTwoPre + needsTrackTwoNoPre


        # 2018 preselection
        preselection2018 = needsCaloMVAPre + needsTrackTwoNoPre
        # MVA TES for preselection and precision steps
        MVATES = preselection in needsCaloMVAPre
        # evaluate RNN for tracktwoMVA chains
        RNN = selection in ['verylooseRNN', 'looseRNN', 'mediumRNN', 'tightRNN'] or preselection in ['tracktwoMVA']
        # chains using 2018 features
        needsAlgo2018 = (preselection in preselection2018) or MVATES or RNN 
        # give unique name to precision sequence
        # we could now live with a single flag, but will keep two in case modularity is needed for Run3 triggers
        # tracktwoEF is (MVATES=false, RNN=false) and tracktwoMVA is (MVATES=true, RNN=true)
        use = {True:'', False:'no'}
        MVAprefix = '{0}MVATES_{1}RNN'.format(use[MVATES], use[RNN])

        
        #Set the default values
        [trkcore, trkprec] = TrigInDetSequence("Tau", "tau", "IDTrig").getSequence()
    
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
            elif preselection in needsCaloMVAPre:
                self.addCaloSequence(threshold, selection, preselection)
                self.addTrigTauRecCaloOnlyMVASequence(threshold,selection,preselection)
                self.addCaloHypoSequence(threshold,selection,preselection)
            # Two step fast-tracking
            if preselection in needsTrackTwoPre:
                self.addTwoStepTrackingSequence(threshold,selection,preselection,idperf, trkprec)
                if preselection == 'tracktwo':
                    self.addTrigTauRecTauPreselectionSequence(threshold,selection,preselection,idperf)
                    self.addTwoStepTrackingSelectionSequence(threshold,selection,preselection,idperf)
                else:
                    self.addTwoStepTrackingSelectionSequence(threshold,selection,preselection,idperf)
                    self.addTrigTauRecTauPreselectionSequence(threshold,selection,preselection,idperf)
            # Two-step tracking but no tau reco, no selection
            if preselection in needsTrackTwoNoPre:
                self.addTwoStepTrackingSequence(threshold,selection,preselection,idperf, trkprec)
            # One step fast-tracking
            if preselection in needsTrackPre:
                self.addTrackingSequence(threshold,selection,preselection,idperf,trkprec)
                self.addTrackingSelectionSequence(threshold,selection,preselection,idperf,trkprec) 
                self.addTrigTauRecTauPreselectionSequence(threshold,selection,preselection,idperf)

        if preselection in needsRun2Hypo:
            # Only run tracking and tau-rec : no need for topoclustering
            if selection == 'cosmic':
                theEFHypo       = self.hypoProvider.GetHypo('EF', threshold, 'perf', '', 'r1')
            else: 
                theEFHypo       = self.hypoProvider.GetHypo('EF', threshold, selection, '', 'r1')


            # Change track selection if we're running on cosmics...
            if selection == 'cosmic':
                recmerged    = TrigTauRecCosmics_Tau2012()
            else:
                if needsAlgo2018:
                    recmerged    = TrigTauRecMerged_TauPrecisionMVA(name='TrigTauMVA_{}'.format(MVAprefix), doMVATES=MVATES, doRNN=RNN)
                else:
                    recmerged    = TrigTauRecMerged_TauPrecision()

            efidinsideout = trkprec

            # Only run the fast-tracking if it wasn't run at pre-selection
            # Is two-step preselection good enough?
            if preselection not in fastTrackingUsed:
                efidinsideout = trkcore+trkprec

            # Precision tracking
            self.EFsequenceList += [[[ self.currentItem ],
                                     efidinsideout,
                                     self.continueChain('EF', 'tr')]]

            # TrigTauRec and Hypo (BDT not needed, but now included in TrigTauRecMerged_TauPrecision)
            if selection in [ 'kaonpi1', 'kaonpi2', 'dipion1', 'dipion1loose', 'dipion2', 'dipion3', 'dikaon', 'dikaontight', 'dikaonmass', 'dikaonmasstight', 'singlepion', 'singlepiontight' ]:
                self.EFsequenceList += [[[ self.currentItem ],
                                         [recmerged, theEFHypo],
                                         self.continueChain('EF', 'effinal')]]

            elif needsAlgo2018:
                
                self.EFsequenceList += [[[ self.currentItem ],
                                         [recmerged],
                                         self.continueChain('EF', 'taurecef')]]

                if not idperf:
                    theHLTTrackPre   = self.hypoProvider.GetHypo('L2', threshold, selection, 'id', preselection)
                    self.EFsequenceList += [[[ self.currentItem ],
                                         [theHLTTrackPre],
                                         self.continueChain('EF', 'trackpre')]]

                self.EFsequenceList += [[[ self.currentItem ],
                                         [theEFHypo],
                                         self.continueChain('EF', 'effinal')]]
     
            else:
                # TrigTauRec (now includes BDT) and Hypo
                self.EFsequenceList += [[[ self.currentItem ],
                                         [recmerged, theEFHypo],
                                         self.continueChain('EF', 'effinal')]]

