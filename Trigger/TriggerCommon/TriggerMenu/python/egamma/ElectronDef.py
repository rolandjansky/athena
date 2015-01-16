# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Electron trigger slice signatures  """

__author__  = 'Moritz Backes & Catrin Bernius'
__version__=""
__doc__="Implementation of Electron Signature"

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
logElectronDef = logging.getLogger("TriggerMenu.egamma.ElectronDef")

from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenu.menu.HltConfig import L2EFChainDef, mergeRemovingOverlap
from TriggerMenu.menu.Lvl1Flags import Lvl1Flags

#from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_cells

#theT2CaloEgamma_cells_e           = T2CaloEgamma_cells("T2CaloEgamma_cells")


from InDetTrigRecExample.EFInDetConfig import TrigEFIDSequence
theTrigEFIDInsideOut_Electron           = TrigEFIDSequence("Electron","electron","InsideOut").getSequence()
theTrigEFIDDataPrep_Electron            = TrigEFIDSequence("Electron","electron","DataPrep").getSequence()
theTrigEFIDInsideOutMerged_Electron     = TrigEFIDSequence("Electron","electron","InsideOutMerged").getSequence()
from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
[theFastTrackFinderxAOD]                  = TrigInDetSequence("Electron","electron","FastxAOD").getSequence()
[theL2StarxAOD]                           = TrigInDetSequence("Electron","electron","L2StarxAOD").getSequence()

#theTrigEFIDOutsideInTRTOnly_Electron    = TrigEFIDSequence("Electron","electron","TRTOnly").getSequence()
#theTrigEFIDCombined_Electron            = TrigEFIDCombined("Electron","photon", "Combined").getSequence()
from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_eGamma as TrigFastTrackFinder_Electron
theTrigFastTrackFinder_Electron = TrigFastTrackFinder_Electron()

# EF Electron FEX
from TrigEgammaRec.TrigEgammaRecConfig       import TrigEgammaRec
theTrigEgammaRec_eGamma                  = TrigEgammaRec(name = "TrigEgammaRec_eGamma")
theTrigEgammaRec_Conv_eGamma                  = TrigEgammaRec(name = "TrigEgammaRec_Conv_eGamma", doConversions = True)

from TrigGenericAlgs.TrigGenericAlgsConf import PrescaleAlgo
terminateAlgo = PrescaleAlgo('terminateAlgo')

##########################################################################################
#
# L2 & EF Chain configuration helper classes
#
##########################################################################################

class L2EFChain_e(L2EFChainDef):
   
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
        self.mult = int(self.chainPart['multiplicity'])
        self.chainName = chainDict['chainName']
        self.chainPartName = self.chainPart['chainPartName']
        self.chainPartNameNoMult = self.chainPartName[1:] if self.mult > 1 else self.chainPartName
        self.chainPartNameNoMult += "_"+self.chainL1Item

        
        self.L2InputTE = self.chainPartL1Item or self.chainL1Item
        # cut of L1_, _EMPTY,..., & multiplicity
        self.L2InputTE = self.L2InputTE.replace("L1_","")
        self.L2InputTE = self.L2InputTE.split("_")[0]
        self.L2InputTE = self.L2InputTE[1:] if self.L2InputTE[0].isdigit() else self.L2InputTE

                
        # eXXvh_ID type chains:
        
        if self.chainPart['trkInfo']=='idperf': #True:# self.chainPart['IDinfo'] \
           #and not self.chainPart['isoInfo'] \
           #and not self.chainPart['FSinfo'] \
           #and not self.chainPart['recoAlg'] \
           #and not self.chainPart['hypoInfo'] \
           #and not self.chainPart['trkInfo'] \
           #and not self.chainPart['reccalibInfo']:
          self.setup_eXXvh_idperf()
        elif self.chainPart['L2IDAlg']:
           self.setup_eXXvh_ID()
        elif self.chainPart['caloInfo']=='L2EFCalo':
           self.setup_eXXvh_ID_L2EFCalo()
        elif self.chainPart['caloInfo']=='HLTCalo':
           self.setup_eXXvh_ID_HLTCalo()
        else:
           self.setup_eXXvh_ID_run2()
            
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




############################### DEFINE GROUPS OF CHAINS HERE ##############################


    def setup_eXXvh_ID(self):
        threshold = self.chainPart['threshold']
        IDinfo = self.chainPart['IDinfo']
        isoInfo = self.chainPart['isoInfo']
        addInfo = self.chainPart['addInfo']
        trkInfo = self.chainPart['trkInfo']
        L2IDAlg = self.chainPart['L2IDAlg']
        
        logElectronDef.debug('setup_eXXvh_ID')
        logElectronDef.debug('threshold: %s',threshold)
        logElectronDef.debug('isoInfo: %s',isoInfo)
        logElectronDef.debug('IDinfo: %s',IDinfo)
        logElectronDef.debug('addInfo: %s',addInfo)
        logElectronDef.debug('trkInfo: %s',trkInfo)
       
        # common imports required for EtCut and Electron ID chains
        # L2 Calo FEX
        from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_eGamma
        theT2CaloEgamma_eGamma            = T2CaloEgamma_eGamma()
        
        # L2 Tracking FEX
        from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_eGammaA
        from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_eGammaB
        from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_eGammaC
        
        # L2 Elecgtron FEX
        from TrigEgammaHypo.TrigL2ElectronFexConfig  import L2ElectronFex_L2StarA
        from TrigEgammaHypo.TrigL2ElectronFexConfig  import L2ElectronFex_L2StarB
        from TrigEgammaHypo.TrigL2ElectronFexConfig  import L2ElectronFex_L2StarC
        from TrigEgammaHypo.TrigL2ElectronFexConfig  import L2ElectronFex_all_NoCut
        # Depends on L2 Strategy
        logElectronDef.debug('L2IDAlg: %s', self.chainPart['L2IDAlg'])
        if self.chainPart['L2IDAlg']: 
            if L2IDAlg == 'L2StarA': 
                theL2TrackingFex = TrigL2SiTrackFinder_eGammaA()
                theL2ElectronFex = L2ElectronFex_L2StarA()
            elif L2IDAlg == 'L2StarB': 
                theL2TrackingFex = TrigL2SiTrackFinder_eGammaB()
                theL2ElectronFex = L2ElectronFex_L2StarB()
            elif L2IDAlg == 'L2StarC': 
                theL2TrackingFex = TrigL2SiTrackFinder_eGammaC()
                theL2ElectronFex = L2ElectronFex_L2StarC()
            else:
                logElectronDef.info('Incorrect L2IDAlg')
        # Default
        else:
            logElectronDef.debug('Use default L2StarA tracking')
            theL2TrackingFex = TrigL2SiTrackFinder_eGammaA()
            theL2ElectronFex = L2ElectronFex_L2StarA()
 
        

        # EF Calo
        from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_eGamma, TrigCaloTowerMaker_eGamma, TrigCaloClusterMaker_slw
        theTrigCaloCellMaker_eGamma      = TrigCaloCellMaker_eGamma()
        theTrigCaloTowerMaker_eGamma     = TrigCaloTowerMaker_eGamma()
        theTrigCaloClusterMaker_slw      = TrigCaloClusterMaker_slw()

        from TrigEgammaHypo.TrigEFCaloCalibFexConfig import TrigEFCaloCalibFex_Electron
        theTrigEFCaloCalibFex = TrigEFCaloCalibFex_Electron()
        
        # EF Tracking
        from InDetTrigRecExample.EFInDetConfig import TrigEFIDInsideOut_Electron
        theEFElectronIDFex           = TrigEFIDInsideOut_Electron("Electron").getSequence()
        
        # EF Electron FEX
        # from TrigEgammaRec.TrigEgammaRecConfig       import TrigEgammaRec
        # theTrigEgammaRec_eGamma                  = TrigEgammaRec(name = "TrigEgammaRec_eGamma")
        #print 'ESETUP', self.chainPart
        # these can be made more configurable later (according to tracking algorithms etc...)
        if 'etcut' in self.chainPart['addInfo']:

            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_NoCut
            from TrigEgammaHypo.TrigL2ElectronHypoConfig import L2ElectronHypo_e_NoCut
            from TrigEgammaHypo.TrigEFTrackHypoConfig import EFTrackHypo_e_NoCut
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_EtCut
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_EtCut
            theL2CaloHypo      = L2CaloHypo_NoCut("L2CaloHypo_e"+str(threshold)+"_NoCut",threshold ) 
            theL2ElectronHypo  = L2ElectronHypo_e_NoCut("L2ElectronHypo_e"+str(threshold)+"_NoCut",threshold ) 
            theEFTrackHypo     = EFTrackHypo_e_NoCut("EFTrackHypo_e"+str(threshold)+"_NoCut",threshold) 
            theTrigEFCaloHypo = TrigEFCaloHypo_EtCut("TrigEFCaloHypo_e"+str(threshold)+"_EtCut",threshold);
            theEFElectronHypo  = TrigEFElectronHypo_e_EtCut("TrigEFElectronHypo_e"+str(threshold)+"_EtCut",threshold)
            
        elif 'perf' in self.chainPart['addInfo']:
            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_NoCut
            from TrigEgammaHypo.TrigL2ElectronHypoConfig import L2ElectronHypo_e_NoCut
            from TrigEgammaHypo.TrigEFTrackHypoConfig import EFTrackHypo_e_NoCut
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_All
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_NoCut
            theL2CaloHypo      = L2CaloHypo_NoCut("L2CaloHypo_e"+str(threshold)+"_NoCut",threshold ) 
            theL2ElectronHypo  = L2ElectronHypo_e_NoCut("L2ElectronHypo_e"+str(threshold)+"_NoCut",threshold ) 
            theEFTrackHypo     = EFTrackHypo_e_NoCut("EFTrackHypo_e"+str(threshold)+"_NoCut",threshold) 
            theTrigEFCaloHypo = TrigEFCaloHypo_All("TrigEFCaloHypo_e"+str(threshold)+"_NoCut",threshold);
            # EF Electron
            theEFElectronHypo  = TrigEFElectronHypo_e_NoCut("TrigEFElectronHypo_e"+str(threshold)+"_NoCut",threshold)
        
        elif self.chainPart['IDinfo']:
            algoSuffix = "e%s_%s()" % (str(threshold),IDinfo)
            logElectronDef.debug('chain suffix: %s', algoSuffix)
            #if 'mvt' in algoSuffix: 
            #    algoSuffix = algoSuffix.replace('mvt','')
            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_e_ID
            from TrigEgammaHypo.TrigL2ElectronHypoConfig import L2ElectronHypo_e_ID
            from TrigEgammaHypo.TrigL2ElectronHypoConfig import L2ElectronHypo_e_ID_L2TrkAlg
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_e_ID
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_ID
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_Iso
            from TrigEgammaHypo.TrigEFTrackHypoConfig import EFTrackHypo_e
            
            # L2 Calo
            theL2CaloHypo = L2CaloHypo_e_ID("TrigL2CaloHypo_e"+str(threshold)+"_"+str(IDinfo),threshold,IDinfo)
            
            # L2 Electron
            if self.chainPart['L2IDAlg']:
                theL2ElectronHypo  = L2ElectronHypo_e_ID_L2TrkAlg("TrigL2ElectronHypo_e"+str(threshold)+"_"+str(IDinfo)+"_"+str(L2IDAlg),threshold,IDinfo,L2IDAlg)
            else:
                theL2ElectronHypo  = L2ElectronHypo_e_ID("TrigL2ElectronHypo_e"+str(threshold)+"_"+str(IDinfo),threshold,IDinfo)
            
            # EF Track
            theEFTrackHypo     = EFTrackHypo_e("EFTrackHypo_e"+str(threshold)+"_"+str(IDinfo),threshold)
            
            # EF Calo
            theTrigEFCaloHypo = TrigEFCaloHypo_e_ID("TrigEFCaloHypo_e"+str(threshold)+"_"+str(IDinfo),threshold,IDinfo);
            
            # EF Electron
            if self.chainPart['isoInfo']:
                theEFElectronHypo  = TrigEFElectronHypo_e_Iso("TrigEFElectronHypo_e"+str(threshold)+"_"+str(IDinfo)+"_"+str(isoInfo),threshold,IDinfo,isoInfo)
            else: 
                theEFElectronHypo  = TrigEFElectronHypo_e_ID("TrigEFElectronHypo_e"+str(threshold)+"_"+str(IDinfo),threshold,IDinfo)
        else:
            algoSuffix = "e%s_%s()" % (str(threshold),IDinfo)
            logElectronDef.error('Chain %s could not be assembled', self.chainPartName)
            logElectronDef.erro('chain suffix: %s', algoSuffix)
            return False
        
        ########### Sequences ###########
        
        self.L2sequenceList += [[self.L2InputTE, 
                                 [theT2CaloEgamma_eGamma, theL2CaloHypo], 
                                 'L2_e_step1']]
        
        self.L2sequenceList += [[['L2_e_step1'],    
                                 [theL2TrackingFex]+theL2StarxAOD, 
                                 'L2_e_step2']]
        
        self.L2sequenceList += [[['L2_e_step2'], 
                                 [theL2ElectronFex, theL2ElectronHypo], 
                                 'L2_e_step3']]
        
        self.EFsequenceList += [[['L2_e_step3'], 
                                 [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw], 
                                 'EF_e_step1']]
        
        self.EFsequenceList += [[['EF_e_step1'], 
                                 [theTrigEFCaloCalibFex,theTrigEFCaloHypo], 
                                 'EF_e_step2']]
        
        self.EFsequenceList += [[['EF_e_step2'], 
                                 theEFElectronIDFex+[ theEFTrackHypo],
                                 'EF_e_step3']]
        
        self.EFsequenceList += [[['EF_e_step3'], 
                                 [theTrigEgammaRec_eGamma, theEFElectronHypo], 
                                 'EF_e_step4']]

        ########### Signatures ###########

        self.L2signatureList += [ [['L2_e_step1']*self.mult] ]
        self.L2signatureList += [ [['L2_e_step2']*self.mult] ]
        self.L2signatureList += [ [['L2_e_step3']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step3']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step4']*self.mult] ]

        ########### TE renaming ###########

        self.TErenamingDict = {
            'L2_e_step1': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'cl'),
            'L2_e_step2': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'id'),
            'L2_e_step3': mergeRemovingOverlap('L2_', self.chainPartNameNoMult),
            'EF_e_step1': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'calo'),
            'EF_e_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'calocalib'),
            'EF_e_step3': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'id'),
            'EF_e_step4': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
            }


    def setup_eXXvh_idperf(self):

        threshold = self.chainPart['threshold']
        IDinfo = self.chainPart['IDinfo']
        isoInfo = self.chainPart['isoInfo']
        run1 = self.chainPart['trkInfo']
       
        logElectronDef.debug('setup_eXXvh_idperf')
        logElectronDef.debug('threshold: %s',threshold)
        logElectronDef.debug('isoInfo: %s',isoInfo)
        logElectronDef.debug('IDinfo: %s',IDinfo)
        logElectronDef.debug('trkInfo: %s',run1)
        
        # L2 Tracking FEX
        from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_eGammaA
        from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_eGammaB
        from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_eGammaC
        theTrigL2SiTrackFinder_eGammaA=TrigL2SiTrackFinder_eGammaA()
        theTrigL2SiTrackFinder_eGammaB=TrigL2SiTrackFinder_eGammaB()
        theTrigL2SiTrackFinder_eGammaC=TrigL2SiTrackFinder_eGammaC()
        # EF Calo
        from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_eGamma_cells
        theTrigCaloCellMaker_eGamma_cells= TrigCaloCellMaker_eGamma_cells()
        from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_eGamma, TrigCaloTowerMaker_eGamma, TrigCaloClusterMaker_slw
        theTrigCaloCellMaker_eGamma      = TrigCaloCellMaker_eGamma()
        theTrigCaloTowerMaker_eGamma     = TrigCaloTowerMaker_eGamma()
        theTrigCaloClusterMaker_slw      = TrigCaloClusterMaker_slw()
        
        from TrigEgammaHypo.TrigEFCaloCalibFexConfig import TrigEFCaloCalibFex_Electron
        theTrigEFCaloCalibFex = TrigEFCaloCalibFex_Electron()
        # EF Tracking
        from InDetTrigRecExample.EFInDetConfig import TrigEFIDInsideOut_Electron
        theEFElectronIDFex           = TrigEFIDInsideOut_Electron("Electron").getSequence()

        # Ringer FEX for L2 Calo
        from TrigT2CaloEgamma.TrigT2CaloEgammaConfig        import T2CaloEgamma_Ringer
        theT2CaloEgamma_Ringer  = T2CaloEgamma_Ringer()
        # EF Electron FEX
        # from TrigEgammaRec.TrigEgammaRecConfig       import TrigEgammaRec
        #theTrigEgammaRec_eGamma                  = TrigEgammaRec(name = "TrigEgammaRec_eGamma")
        #print 'ESETUP', self.chainPart
        # these can be made more configurable later (according to tracking algorithms etc...)
        
        algoSuffix = "e%s_%s()" % (str(threshold),IDinfo)
        if 'etcut' in self.chainPart['addInfo']:
            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_NoCut
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_EtCut
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_EtCut
            theL2CaloHypo      = L2CaloHypo_NoCut("L2CaloHypo_e"+str(threshold)+"_NoCut",threshold ) 
            theTrigEFCaloHypo = TrigEFCaloHypo_EtCut("TrigEFCaloHypo_e"+str(threshold)+"_EtCut",threshold);
            theEFElectronHypo  = TrigEFElectronHypo_e_EtCut("TrigEFElectronHypo_e"+str(threshold)+"_EtCut",threshold)
        elif self.chainPart['IDinfo']:
            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_e_ID
            theL2CaloHypo = L2CaloHypo_e_ID("TrigL2CaloHypo_e"+str(threshold)+"_"+str(IDinfo),threshold,IDinfo)
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_e_ID
            # EF Calo
            theTrigEFCaloHypo = TrigEFCaloHypo_e_ID("TrigEFCaloHypo_e"+str(threshold)+"_"+str(IDinfo),threshold,IDinfo);
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_ID_CaloOnly
            theEFElectronHypo  = \
                TrigEFElectronHypo_e_ID_CaloOnly("TrigEFElectronHypo_e"+str(threshold)+"_"+str(IDinfo)+"_CaloOnly",threshold,IDinfo)

        if 'conv' in self.chainPart['addInfo']:
            theTrigEgammaFex = theTrigEgammaRec_Conv_eGamma
        else :
            theTrigEgammaFex = theTrigEgammaRec_eGamma


        ########### Sequences ###########
        trkseq = list(theTrigEFIDDataPrep_Electron)
        trkseq.append(theTrigFastTrackFinder_Electron)
        trkseq += theFastTrackFinderxAOD
        trkseq+=theTrigEFIDInsideOutMerged_Electron

        #calo thresholds here?
        self.L2sequenceList += [[self.L2InputTE, 
                                 [theT2CaloEgamma_Ringer, theL2CaloHypo], 
                                 'L2_e_step1']]
        
        self.EFsequenceList += [[['L2_e_step1'], 
                                 [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw], 
                                 'EF_e_step1']]
        
        self.EFsequenceList += [[['EF_e_step1'], 
                                 [theTrigEFCaloCalibFex,theTrigEFCaloHypo], 
                                 'EF_e_step2']]
        
        if 'L2Star' in self.chainPart['addInfo']:
            self.EFsequenceList += [[['EF_e_step2'], 
                                     [theTrigL2SiTrackFinder_eGammaA]+theL2StarxAOD+
                                     [theTrigL2SiTrackFinder_eGammaB]+theL2StarxAOD+
                                     [theTrigL2SiTrackFinder_eGammaC]+theL2StarxAOD+theEFElectronIDFex,
                                     'EF_e_step3']]
        else:
            self.EFsequenceList += [[['EF_e_step2'], 
                                     trkseq,
                                     'EF_e_step3']]
        
        self.EFsequenceList += [[['EF_e_step3'], 
                                 [theTrigEgammaFex, theEFElectronHypo], 
                                 'EF_e_step4']]

        ########### Signatures ###########

        self.L2signatureList += [ [['L2_e_step1']*self.mult] ]
        #self.L2signatureList += [ [['L2_e_step2']*self.mult] ]
        #self.L2signatureList += [ [['L2_e_step3']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step3']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step4']*self.mult] ]

        ########### TE renaming ###########

        self.TErenamingDict = {
            'L2_e_step1': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'cl'),
            'EF_e_step1': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'cl'),
            'EF_e_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'calocalib'),
            'EF_e_step3': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'trk'),
            'EF_e_step4': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
            }


    def setup_eXXvh_ID_L2EFCalo(self):
        threshold = self.chainPart['threshold']
        IDinfo = self.chainPart['IDinfo']
        isoInfo = self.chainPart['isoInfo']
        addInfo = self.chainPart['addInfo']
        trkInfo = self.chainPart['trkInfo']
      
        logElectronDef.debug('setup_eXXvh_ID_run2')
        logElectronDef.debug('threshold: %s',threshold)
        logElectronDef.debug('isoInfo: %s',isoInfo)
        logElectronDef.debug('IDinfo: %s',IDinfo)
        logElectronDef.debug('trkInfo: %s',trkInfo)
       
        # common imports required for EtCut and Electron ID chains
        # L2 Calo FEX
        from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_eGamma
        theT2CaloEgamma_eGamma            = T2CaloEgamma_eGamma()
        
        # Ringer FEX for L2 Calo
        from TrigT2CaloEgamma.TrigT2CaloEgammaConfig        import T2CaloEgamma_Ringer
        theT2CaloEgamma_Ringer  = T2CaloEgamma_Ringer()

        from TrigMultiVarHypo.TrigMultiVarHypoConfig        import TrigRingerNeuralHypoConfig
        from TrigMultiVarHypo.TrigMultiVarHypoConfig        import TrigRingerNeuralFexConfig

        theL2CaloFex_Ringer            = TrigRingerNeuralFexConfig("TrigRingerNeuralFex_e"+str(threshold)  )
        # EF Calo
        from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_eGamma, TrigCaloTowerMaker_eGamma, TrigCaloClusterMaker_slw
        theTrigCaloCellMaker_eGamma      = TrigCaloCellMaker_eGamma()
        theTrigCaloTowerMaker_eGamma     = TrigCaloTowerMaker_eGamma()
        theTrigCaloClusterMaker_slw      = TrigCaloClusterMaker_slw()
       
        from TrigEgammaHypo.TrigEFCaloCalibFexConfig import TrigEFCaloCalibFex_Electron
        theTrigEFCaloCalibFex = TrigEFCaloCalibFex_Electron()
        
        # these can be made more configurable later (according to tracking algorithms etc...)
        if 'etcut' in self.chainPart['addInfo']:

            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_NoCut
            from TrigEgammaHypo.TrigEFTrackHypoConfig import EFTrackHypo_e_NoCut
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_EtCut
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_EtCut
            theL2CaloHypo      = L2CaloHypo_NoCut("L2CaloHypo_e"+str(threshold)+"_NoCut",threshold ) 
            theTrigEFCaloHypo = TrigEFCaloHypo_EtCut("TrigEFCaloHypo_e"+str(threshold)+"_EtCut",threshold);
            theEFTrackHypo     = EFTrackHypo_e_NoCut("EFTrackHypo_e"+str(threshold)+"_NoCut",threshold) 
            theEFElectronHypo  = TrigEFElectronHypo_e_EtCut("TrigEFElectronHypo_e"+str(threshold)+"_EtCut",threshold)
        elif 'perf' in self.chainPart['addInfo']:
            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_NoCut
            from TrigEgammaHypo.TrigEFTrackHypoConfig import EFTrackHypo_e_NoCut
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_NoCut
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_All
            theL2CaloHypo      = L2CaloHypo_NoCut("L2CaloHypo_e"+str(threshold)+"_NoCut",threshold ) 
            theTrigEFCaloHypo = TrigEFCaloHypo_All("TrigEFCaloHypo_e"+str(threshold)+"_NoCut",threshold);
            theEFTrackHypo     = EFTrackHypo_e_NoCut("EFTrackHypo_e"+str(threshold)+"_NoCut",threshold) 
            # EF Electron
            theEFElectronHypo  = TrigEFElectronHypo_e_NoCut("TrigEFElectronHypo_e"+str(threshold)+"_NoCut",threshold)

        elif self.chainPart['IDinfo']:
            algoSuffix = "e%s_%s()" % (str(threshold),IDinfo)
            logElectronDef.debug('chain suffix: %s', algoSuffix)
            #if 'mvt' in algoSuffix: 
            #    algoSuffix = algoSuffix.replace('mvt','')
            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_e_ID
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_ID
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_Iso
            from TrigEgammaHypo.TrigEFTrackHypoConfig import EFTrackHypo_e
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_e_ID
            # L2 Calo
            if 'ringer' in self.chainPart['addInfo']:
                theL2CaloHypo           = TrigRingerNeuralHypoConfig("TrigRingerNeuralHypo_e"+str(threshold)  )
            else:
                theL2CaloHypo = L2CaloHypo_e_ID("TrigL2CaloHypo_e"+str(threshold)+"_"+str(IDinfo),threshold,IDinfo)
            
            # EF Calo
            theTrigEFCaloHypo = TrigEFCaloHypo_e_ID("TrigEFCaloHypo_e"+str(threshold)+"_"+str(IDinfo),threshold,IDinfo);
           
            # EF Track
            theEFTrackHypo     = EFTrackHypo_e("EFTrackHypo_e"+str(threshold)+"_"+str(IDinfo),threshold)
            
            # EF Electron
            if self.chainPart['isoInfo']:
                theEFElectronHypo  = TrigEFElectronHypo_e_Iso("TrigEFElectronHypo_e"+str(threshold)+"_"+str(IDinfo)+"_"+str(isoInfo),threshold,IDinfo,isoInfo)
            else: 
                theEFElectronHypo  = TrigEFElectronHypo_e_ID("TrigEFElectronHypo_e"+str(threshold)+"_"+str(IDinfo),threshold,IDinfo)
        else:
            algoSuffix = "e%s_%s()" % (str(threshold),IDinfo)
            logElectronDef.error('Chain %s could not be assembled', self.chainPartName)
            logElectronDef.erro('chain suffix: %s', algoSuffix)
            return False
        
        ########### Sequences ###########
        
        trkcomb1st = list(theTrigEFIDDataPrep_Electron)
        trkcomb1st.append(theTrigFastTrackFinder_Electron)
        trkcomb1st += theFastTrackFinderxAOD
        trkcombfull = list(trkcomb1st)
        trkcombfull += theTrigEFIDInsideOutMerged_Electron

        if 'ringer' in self.chainPart['addInfo']:
            self.L2sequenceList += [[self.L2InputTE, 
                                     [theT2CaloEgamma_Ringer, theL2CaloFex_Ringer, theL2CaloHypo], 
                                     'L2_e_step1']]
        elif 'perf' in self.chainPart['addInfo']:
            self.L2sequenceList += [[self.L2InputTE, 
                                     [theT2CaloEgamma_Ringer, theL2CaloHypo], 
                                     'L2_e_step1']]
        else:
            self.L2sequenceList += [[self.L2InputTE, 
                                     [theT2CaloEgamma_eGamma, theL2CaloHypo], 
                                     'L2_e_step1']]
        
        self.EFsequenceList += [[['L2_e_step1'], 
                                 [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
                                 'EF_e_step1']]
        
        self.EFsequenceList += [[['EF_e_step1'], 
                                 [theTrigEFCaloCalibFex,theTrigEFCaloHypo], 
                                 'EF_e_step2']]
        
        self.EFsequenceList += [[['EF_e_step2'], 
                                 trkcombfull+[ theEFTrackHypo],
                                 'EF_e_step3']]
        
        self.EFsequenceList += [[['EF_e_step3'], 
                                 [theTrigEgammaRec_eGamma, theEFElectronHypo], 
                                 'EF_e_step4']]

        ########### Signatures ###########

        self.L2signatureList += [ [['L2_e_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step3']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step4']*self.mult] ]
        ########### TE renaming ###########

        self.TErenamingDict = {
            'L2_e_step1': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'cl'),
            'EF_e_step1': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'calo'),
            'EF_e_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'calocalib'),
            'EF_e_step3': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'id'),
            'EF_e_step4': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
            }
    def setup_eXXvh_ID_HLTCalo(self):
        threshold = self.chainPart['threshold']
        IDinfo = self.chainPart['IDinfo']
        isoInfo = self.chainPart['isoInfo']
        addInfo = self.chainPart['addInfo']
        trkInfo = self.chainPart['trkInfo']
      
        logElectronDef.debug('setup_eXXvh_ID_run2')
        logElectronDef.debug('threshold: %s',threshold)
        logElectronDef.debug('isoInfo: %s',isoInfo)
        logElectronDef.debug('IDinfo: %s',IDinfo)
        logElectronDef.debug('trkInfo: %s',trkInfo)
       
        # L2 Elecgtron FEX
        from TrigEgammaHypo.TrigL2ElectronFexConfig  import L2ElectronFex_1
        # Depends on L2 Strategy
        theL2ElectronFex = L2ElectronFex_1()
 
        # EF Calo
        from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_eGamma, TrigCaloTowerMaker_eGamma, TrigCaloClusterMaker_slw
        theTrigCaloCellMaker_eGamma      = TrigCaloCellMaker_eGamma()
        theTrigCaloTowerMaker_eGamma     = TrigCaloTowerMaker_eGamma()
        theTrigCaloClusterMaker_slw      = TrigCaloClusterMaker_slw()
       
        from TrigEgammaHypo.TrigEFCaloCalibFexConfig import TrigEFCaloCalibFex_Electron
        theTrigEFCaloCalibFex = TrigEFCaloCalibFex_Electron()
        
        # EF Tracking
        theEFElectronIDFex           = theTrigEFIDInsideOutMerged_Electron
        # EF Electron FEX
        #from TrigEgammaRec.TrigEgammaRecConfig       import TrigEgammaRec
        #theTrigEgammaRec_eGamma                  = TrigEgammaRec(name = "TrigEgammaRec_eGamma")
        #print 'ESETUP', self.chainPart
        # these can be made more configurable later (according to tracking algorithms etc...)
        if 'etcut' in self.chainPart['addInfo']:
            from TrigEgammaHypo.TrigEFTrackHypoConfig import EFTrackHypo_e_NoCut
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_EtCut
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_EtCut
            theTrigEFCaloHypo = TrigEFCaloHypo_EtCut("TrigEFCaloHypo_e"+str(threshold)+"_EtCut",threshold);
            theEFTrackHypo     = EFTrackHypo_e_NoCut("EFTrackHypo_e"+str(threshold)+"_NoCut",threshold) 
            theEFElectronHypo  = TrigEFElectronHypo_e_EtCut("TrigEFElectronHypo_e"+str(threshold)+"_EtCut",threshold)
        elif 'perf' in self.chainPart['addInfo']:
            from TrigEgammaHypo.TrigEFTrackHypoConfig import EFTrackHypo_e_NoCut
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_NoCut
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_All
            theTrigEFCaloHypo = TrigEFCaloHypo_All("TrigEFCaloHypo_e"+str(threshold)+"_NoCut",threshold);
            theEFTrackHypo     = EFTrackHypo_e_NoCut("EFTrackHypo_e"+str(threshold)+"_NoCut",threshold) 
            # EF Electron
            theEFElectronHypo  = TrigEFElectronHypo_e_NoCut("TrigEFElectronHypo_e"+str(threshold)+"_NoCut",threshold)

        elif self.chainPart['IDinfo']:
            algoSuffix = "e%s_%s()" % (str(threshold),IDinfo)
            logElectronDef.debug('chain suffix: %s', algoSuffix)
            #if 'mvt' in algoSuffix: 
            #    algoSuffix = algoSuffix.replace('mvt','')
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_ID
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_Iso
            from TrigEgammaHypo.TrigEFTrackHypoConfig import EFTrackHypo_e
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_e_ID
            
            # EF Calo
            theTrigEFCaloHypo = TrigEFCaloHypo_e_ID("TrigEFCaloHypo_e"+str(threshold)+"_"+str(IDinfo),threshold,IDinfo);
           
            # EF Track
            theEFTrackHypo     = EFTrackHypo_e("EFTrackHypo_e"+str(threshold)+"_"+str(IDinfo),threshold)
            
            # EF Electron
            if self.chainPart['isoInfo']:
                theEFElectronHypo  = TrigEFElectronHypo_e_Iso("TrigEFElectronHypo_e"+str(threshold)+"_"+str(IDinfo)+"_"+str(isoInfo),threshold,IDinfo,isoInfo)
            else: 
                theEFElectronHypo  = TrigEFElectronHypo_e_ID("TrigEFElectronHypo_e"+str(threshold)+"_"+str(IDinfo),threshold,IDinfo)
        else:
            algoSuffix = "e%s_%s()" % (str(threshold),IDinfo)
            logElectronDef.error('Chain %s could not be assembled', self.chainPartName)
            logElectronDef.erro('chain suffix: %s', algoSuffix)
            return False
        
        ########### Sequences ###########
        
        trkcomb1st = list(theTrigEFIDDataPrep_Electron)
        trkcomb1st.append(theTrigFastTrackFinder_Electron)
        trkcomb1st += theFastTrackFinderxAOD
        trkcombfull = list(trkcomb1st)
        trkcombfull += theTrigEFIDInsideOutMerged_Electron

        self.EFsequenceList += [[self.L2InputTE, 
                                 [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
                                 'EF_e_step1']]
        
        self.EFsequenceList += [[['EF_e_step1'], 
                                 [theTrigEFCaloCalibFex,theTrigEFCaloHypo], 
                                 'EF_e_step2']]
        
        self.EFsequenceList += [[['EF_e_step2'], 
                                 trkcombfull+[ theEFTrackHypo],
                                 'EF_e_step3']]
        
        self.EFsequenceList += [[['EF_e_step3'], 
                                 [theTrigEgammaRec_eGamma, theEFElectronHypo], 
                                 'EF_e_step4']]

        ########### Signatures ###########

        self.EFsignatureList += [ [['EF_e_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step3']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step4']*self.mult] ]
        ########### TE renaming ###########

        self.TErenamingDict = {
            'EF_e_step1': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'calo'),
            'EF_e_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'calocalib'),
            'EF_e_step3': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'id'),
            'EF_e_step4': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
            }

    def setup_eXXvh_ID_run2(self):
        threshold = self.chainPart['threshold']
        IDinfo = self.chainPart['IDinfo']
        isoInfo = self.chainPart['isoInfo']
        addInfo = self.chainPart['addInfo']
        trkInfo = self.chainPart['trkInfo']
      
        logElectronDef.debug('setup_eXXvh_ID_run2')
        logElectronDef.debug('threshold: %s',threshold)
        logElectronDef.debug('isoInfo: %s',isoInfo)
        logElectronDef.debug('IDinfo: %s',IDinfo)
        logElectronDef.debug('trkInfo: %s',trkInfo)
       
        # common imports required for EtCut and Electron ID chains
        # L2 Calo FEX
        from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_eGamma
        theT2CaloEgamma_eGamma            = T2CaloEgamma_eGamma()
       
        # Ringer FEX for L2 Calo
        from TrigT2CaloEgamma.TrigT2CaloEgammaConfig        import T2CaloEgamma_Ringer
        theT2CaloEgamma_Ringer  = T2CaloEgamma_Ringer()

        from TrigMultiVarHypo.TrigMultiVarHypoConfig        import TrigRingerNeuralHypoConfig
        from TrigMultiVarHypo.TrigMultiVarHypoConfig        import TrigRingerNeuralFexConfig

        theL2CaloFex_Ringer            = TrigRingerNeuralFexConfig("TrigRingerNeuralFex_e"+str(threshold)  )
        # L2 Elecgtron FEX
        from TrigEgammaHypo.TrigL2ElectronFexConfig  import L2ElectronFex_1
        from TrigEgammaHypo.TrigL2ElectronFexConfig         import L2ElectronFex_Ringer  
        # Depends on L2 Strategy
        theL2ElectronFex = L2ElectronFex_1()
 
        # EF Calo
        from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_eGamma, TrigCaloTowerMaker_eGamma, TrigCaloClusterMaker_slw
        theTrigCaloCellMaker_eGamma      = TrigCaloCellMaker_eGamma()
        theTrigCaloTowerMaker_eGamma     = TrigCaloTowerMaker_eGamma()
        theTrigCaloClusterMaker_slw      = TrigCaloClusterMaker_slw()
       
        from TrigEgammaHypo.TrigEFCaloCalibFexConfig import TrigEFCaloCalibFex_Electron
        theTrigEFCaloCalibFex = TrigEFCaloCalibFex_Electron()
        
        # EF Tracking
        theEFElectronIDFex           = theTrigEFIDInsideOutMerged_Electron
        # EF Electron FEX
        #from TrigEgammaRec.TrigEgammaRecConfig       import TrigEgammaRec
        #theTrigEgammaRec_eGamma                  = TrigEgammaRec(name = "TrigEgammaRec_eGamma")
        #print 'ESETUP', self.chainPart
        # these can be made more configurable later (according to tracking algorithms etc...)
        if 'etcut' in self.chainPart['addInfo']:

            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_NoCut
            from TrigEgammaHypo.TrigL2ElectronHypoConfig import L2ElectronHypo_e_NoCut
            from TrigEgammaHypo.TrigEFTrackHypoConfig import EFTrackHypo_e_NoCut
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_EtCut
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_EtCut
            theL2CaloHypo      = L2CaloHypo_NoCut("L2CaloHypo_e"+str(threshold)+"_NoCut",threshold ) 
            theL2ElectronHypo  = L2ElectronHypo_e_NoCut("L2ElectronHypo_e"+str(threshold)+"_NoCut",threshold ) 
            theTrigEFCaloHypo = TrigEFCaloHypo_EtCut("TrigEFCaloHypo_e"+str(threshold)+"_EtCut",threshold);
            theEFTrackHypo     = EFTrackHypo_e_NoCut("EFTrackHypo_e"+str(threshold)+"_NoCut",threshold) 
            theEFElectronHypo  = TrigEFElectronHypo_e_EtCut("TrigEFElectronHypo_e"+str(threshold)+"_EtCut",threshold)
        elif 'perf' in self.chainPart['addInfo']:
            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_NoCut
            from TrigEgammaHypo.TrigL2ElectronHypoConfig import L2ElectronHypo_e_NoCut
            from TrigEgammaHypo.TrigEFTrackHypoConfig import EFTrackHypo_e_NoCut
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_NoCut
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_All
            theL2CaloHypo      = L2CaloHypo_NoCut("L2CaloHypo_e"+str(threshold)+"_NoCut",threshold ) 
            theL2ElectronHypo  = L2ElectronHypo_e_NoCut("L2ElectronHypo_e"+str(threshold)+"_NoCut",threshold ) 
            theTrigEFCaloHypo = TrigEFCaloHypo_All("TrigEFCaloHypo_e"+str(threshold)+"_NoCut",threshold);
            theEFTrackHypo     = EFTrackHypo_e_NoCut("EFTrackHypo_e"+str(threshold)+"_NoCut",threshold) 
            # EF Electron
            theEFElectronHypo  = TrigEFElectronHypo_e_NoCut("TrigEFElectronHypo_e"+str(threshold)+"_NoCut",threshold)

        elif self.chainPart['IDinfo']:
            algoSuffix = "e%s_%s()" % (str(threshold),IDinfo)
            logElectronDef.debug('chain suffix: %s', algoSuffix)
            #if 'mvt' in algoSuffix: 
            #    algoSuffix = algoSuffix.replace('mvt','')
            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_e_ID
            from TrigEgammaHypo.TrigL2ElectronHypoConfig import L2ElectronHypo_e_ID
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_ID
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_Iso
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_ID_EtIsEM
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_LH
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_LH_Iso
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_ID_EtIsEM_Iso
            from TrigEgammaHypo.TrigEFTrackHypoConfig import EFTrackHypo_e
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_e_ID
            # L2 Calo
            if 'ringer' in self.chainPart['addInfo']:
                theL2CaloHypo           = TrigRingerNeuralHypoConfig("TrigRingerNeuralHypo_e"+str(threshold)  )
                theL2ElectronFex        = L2ElectronFex_Ringer("L2ElectronFex_Ringer_e"+str(threshold)  )
            else:
                theL2CaloHypo = L2CaloHypo_e_ID("TrigL2CaloHypo_e"+str(threshold)+"_"+str(IDinfo),threshold,IDinfo)
            
            # L2 Electron
            theL2ElectronHypo  = L2ElectronHypo_e_ID("TrigL2ElectronHypo_e"+str(threshold)+"_"+str(IDinfo),threshold,IDinfo)
            
            # EF Calo
            theTrigEFCaloHypo = TrigEFCaloHypo_e_ID("TrigEFCaloHypo_e"+str(threshold)+"_"+str(IDinfo),threshold,IDinfo);
           
            # EF Track
            theEFTrackHypo     = EFTrackHypo_e("EFTrackHypo_e"+str(threshold)+"_"+str(IDinfo),threshold)
            
            # EF Electron
            if self.chainPart['isoInfo']:
                if 'etisem' in self.chainPart['addInfo']:
                    theEFElectronHypo  = TrigEFElectronHypo_e_ID_EtIsEM_Iso("TrigEFElectronHypo_e"+str(threshold)+"_"+str(IDinfo)+"_EtIsEM"+"_"+str(isoInfo),threshold,IDinfo)
                elif self.chainPart['lhInfo']:
                    lhInfo = self.chainPart['lhInfo']
                    theEFElectronHypo = TrigEFElectronHypo_e_LH_Iso("TrigEFElectronHypo_e"+str(threshold)+"_"+str(IDinfo)+"_"+str(lhInfo)+"_"+str(isoInfo),threshold,lhInfo)
                else:
                    theEFElectronHypo  = TrigEFElectronHypo_e_Iso("TrigEFElectronHypo_e"+str(threshold)+"_"+str(IDinfo)+"_"+str(isoInfo),threshold,IDinfo,isoInfo)
            elif 'etisem' in self.chainPart['addInfo']:
                theEFElectronHypo  = TrigEFElectronHypo_e_ID_EtIsEM("TrigEFElectronHypo_e"+str(threshold)+"_"+str(IDinfo)+"_EtIsEM",threshold,IDinfo)
            elif self.chainPart['lhInfo']:
                lhInfo = self.chainPart['lhInfo']
                theEFElectronHypo = TrigEFElectronHypo_e_LH("TrigEFElectronHypo_e"+str(threshold)+"_"+str(IDinfo)+"_"+str(lhInfo),threshold,lhInfo)
            else: 
                theEFElectronHypo  = TrigEFElectronHypo_e_ID("TrigEFElectronHypo_e"+str(threshold)+"_"+str(IDinfo),threshold,IDinfo)
        else:
            algoSuffix = "e%s_%s()" % (str(threshold),IDinfo)
            logElectronDef.error('Chain %s could not be assembled', self.chainPartName)
            logElectronDef.erro('chain suffix: %s', algoSuffix)
            return False
        
        if 'conv' in self.chainPart['addInfo']:
            theTrigEgammaFex = theTrigEgammaRec_Conv_eGamma
        else :
            theTrigEgammaFex = theTrigEgammaRec_eGamma

        ########### Sequences ###########
        
        trkcomb1st = list(theTrigEFIDDataPrep_Electron)
        trkcomb1st.append(theTrigFastTrackFinder_Electron)
        trkcomb1st += theFastTrackFinderxAOD
        trkcombfull = list(trkcomb1st)
        trkcombfull += theTrigEFIDInsideOutMerged_Electron
        
        if 'ringer' in self.chainPart['addInfo']:
            self.L2sequenceList += [[self.L2InputTE, 
                                     [theT2CaloEgamma_Ringer, theL2CaloFex_Ringer, theL2CaloHypo], 
                                     'L2_e_step1']]
        elif 'perf' in self.chainPart['addInfo']:
            self.L2sequenceList += [[self.L2InputTE, 
                                     [theT2CaloEgamma_Ringer, theL2CaloHypo], 
                                     'L2_e_step1']]
        else:
            self.L2sequenceList += [[self.L2InputTE, 
                                     [theT2CaloEgamma_eGamma, theL2CaloHypo], 
                                     'L2_e_step1']]
        
        self.L2sequenceList += [[['L2_e_step1'],    
                                 trkcomb1st, 
                                 'L2_e_step2']]
        
        self.L2sequenceList += [[['L2_e_step2'], 
                                 [theL2ElectronFex, theL2ElectronHypo], 
                                 'L2_e_step3']]
        
        self.EFsequenceList += [[['L2_e_step3'], 
                                 [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
                                 'EF_e_step1']]
        
        self.EFsequenceList += [[['EF_e_step1'], 
                                 [theTrigEFCaloCalibFex,theTrigEFCaloHypo], 
                                 'EF_e_step2']]
        
        self.EFsequenceList += [[['EF_e_step2'], 
                                 theEFElectronIDFex+[ theEFTrackHypo],
                                 #trkcombfull+[ theEFTrackHypo],
                                 'EF_e_step3']]
        
        self.EFsequenceList += [[['EF_e_step3'], 
                                 [theTrigEgammaFex, theEFElectronHypo], 
                                 'EF_e_step4']]

        ########### Signatures ###########

        self.L2signatureList += [ [['L2_e_step1']*self.mult] ]
        self.L2signatureList += [ [['L2_e_step2']*self.mult] ]
        self.L2signatureList += [ [['L2_e_step3']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step3']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step4']*self.mult] ]
        ########### TE renaming ###########

        self.TErenamingDict = {
            'L2_e_step1': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'cl'),
            'L2_e_step2': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'id'),
            'L2_e_step3': mergeRemovingOverlap('L2_', self.chainPartNameNoMult),
            'EF_e_step1': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'calo'),
            'EF_e_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'calocalib'),
            'EF_e_step3': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'id'),
            'EF_e_step4': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
            }
