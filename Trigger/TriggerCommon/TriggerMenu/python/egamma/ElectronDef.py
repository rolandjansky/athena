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
TrigEgammaRec_eGamma                  = TrigEgammaRec.copy(name = "TrigEgammaRec_eGamma",doPrint=False)
TrigEgammaRec_Conv_eGamma                  = TrigEgammaRec.copy(name = "TrigEgammaRec_Conv_eGamma", doConversions = True,doPrint=False)
TrigEgammaRec_NoIDEF_eGamma         = TrigEgammaRec.copy(name = "TrigEgammaRec_NoIDEF_eGamma",doTrackMatching = False,doTrackIsolation = False,doPrint=False)

from TriggerMenu.egamma.EgammaCleanMonitoring import *
from TriggerMenu.menu.CleanMonitoring import *

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
            if 'ion' in self.chainPart['extra']:
                self.setup_eXXvh_idperf_heavyIon()
            else:
                self.setup_eXXvh_idperf()
        elif self.chainPart['L2IDAlg']:
           self.setup_eXXvh_ID()       
        elif 'ringer' in self.chainPart['addInfo']:
           self.setup_eXXvh_ID_ringer()
        elif self.chainPart['caloInfo']=='L2EFCalo':
           self.setup_eXXvh_ID_L2EFCalo()
        elif self.chainPart['caloInfo']=='HLTCalo':
           self.setup_eXXvh_ID_HLTCalo()
        else:
            if 'ion' in self.chainPart['extra']:
                self.setup_eXXvh_ID_run2_heavyIon()
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
        theT2CaloEgamma_eGamma  = T2CaloEgamma_eGamma()
        # Ringer FEX for L2 Calo
        from TrigT2CaloEgamma.TrigT2CaloEgammaConfig        import T2CaloEgamma_Ringer
        theT2CaloEgamma_Ringer  = T2CaloEgamma_Ringer()
        
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
        theTrigEgammaRec_eGamma                  = TrigEgammaRec_eGamma()
        #print 'ESETUP', self.chainPart
        # these can be made more configurable later (according to tracking algorithms etc...)
        if 'etcut' in self.chainPart['addInfo']:

            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_NoCut
            from TrigEgammaHypo.TrigL2ElectronHypoConfig import L2ElectronHypo_e_NoCut
            from TrigEgammaHypo.TrigEFTrackHypoConfig import EFTrackHypo_e_NoCut
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_EtCut
            #from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_EtCut

            theL2CaloHypo      = L2CaloHypo_NoCut("L2CaloHypo_e"+str(threshold)+"_NoCut",threshold ) 
            theL2ElectronHypo  = L2ElectronHypo_e_NoCut("L2ElectronHypo_e"+str(threshold)+"_NoCut",threshold ) 
            theEFTrackHypo     = EFTrackHypo_e_NoCut("EFTrackHypo_e"+str(threshold)+"_NoCut",threshold) 
            theTrigEFCaloHypo = TrigEFCaloHypo_EtCut("TrigEFCaloHypo_e"+str(threshold)+"_EtCut",threshold);
            #theEFElectronHypo  = TrigEFElectronHypo_e_EtCut("TrigEFElectronHypo_e"+str(threshold)+"_EtCut",threshold)
            if 'trkcut' in self.chainPart['addInfo']:
                from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_WTP
                theEFElectronHypo = TrigEFElectronHypo_e_WTP("TrigEFElectronHypo_e"+str(threshold)+"_WTP",threshold)
            else:
                from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_NoCut
                theEFElectronHypo = TrigEFElectronHypo_e_NoCut("TrigEFElectronHypo_e"+str(threshold)+"_NoCut",threshold)
            
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
       
        if 'perf' in self.chainPart['addInfo'] or 'etcut' in self.chainPart['addInfo']:
            print 'TrigT2CaloEgamma_Ringer was enabled'
            self.L2sequenceList += [[self.L2InputTE, 
                                     [theT2CaloEgamma_Ringer, theL2CaloHypo], 
                                     'L2_e_step1']]
        else: 
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
        # GSF refit
        #from TrigEgammaHypo.TrigGSFTrackFexConfig import TrigGSFTrackFex_Electron
        #theGSFtrackfex = TrigGSFTrackFex_Electron()

        # Ringer FEX for L2 Calo
        from TrigT2CaloEgamma.TrigT2CaloEgammaConfig        import T2CaloEgamma_Ringer
        theT2CaloEgamma_Ringer  = T2CaloEgamma_Ringer()
        
        
        #print 'ESETUP', self.chainPart
        # these can be made more configurable later (according to tracking algorithms etc...)
        
        algoSuffix = "e%s_%s()" % (str(threshold),IDinfo)
        if 'etcut' in self.chainPart['addInfo']:
            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_NoCut
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_EtCut
            # No cut after FEX
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_NoCut
            theL2CaloHypo      = L2CaloHypo_NoCut("L2CaloHypo_e"+str(threshold)+"_NoCut",threshold ) 
            theTrigEFCaloHypo = TrigEFCaloHypo_EtCut("TrigEFCaloHypo_e"+str(threshold)+"_EtCut",threshold);
            theEFElectronHypo  = TrigEFElectronHypo_e_NoCut("TrigEFElectronHypo_e"+str(threshold)+"_NoCut",threshold)
        elif self.chainPart['IDinfo']:
            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_e_ID
            theL2CaloHypo = L2CaloHypo_e_ID("TrigL2CaloHypo_e"+str(threshold)+"_"+str(IDinfo),threshold,IDinfo)
            # EF Calo
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_e_ID
            theTrigEFCaloHypo = TrigEFCaloHypo_e_ID("TrigEFCaloHypo_e"+str(threshold)+"_"+str(IDinfo),threshold,IDinfo);
            # No cut after FEX
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_NoCut    
            theEFElectronHypo  = TrigEFElectronHypo_e_NoCut("TrigEFElectronHypo_e"+str(threshold)+"_NoCut",threshold)
        
        # EF Electron FEX
        if 'conv' in self.chainPart['addInfo']:
            theTrigEgammaFex = TrigEgammaRec_Conv_eGamma()
        else :
            theTrigEgammaFex = TrigEgammaRec_eGamma()


        ########### Sequences ###########
        from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
        [trkfast, trkprec] = TrigInDetSequence("Electron", "electron", "IDTrig").getSequence()

        trkseq=trkfast+trkprec

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
        elif 'gsf' in self.chainPart['addInfo']:
            logElectronDef.error("GSF chains are not configured yet")
        #    self.EFsequenceList += [[['EF_e_step2'], 
        #                             trkseq + [theGSFTrackFex],
        #                             'EF_e_step3']]
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

    def setup_eXXvh_idperf_heavyIon(self):

        threshold = self.chainPart['threshold']
        IDinfo = self.chainPart['IDinfo']
        isoInfo = self.chainPart['isoInfo']
        run1 = self.chainPart['trkInfo']
       
        logElectronDef.debug('setup_eXXvh_idperf_heavyIon')
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
        from TrigHIHypo.UE import theUEMaker, theFSCellMaker, theTrigCaloCellMaker_eGamma, theElectronUEMonitoring
        print "imported UE stuff here"

        from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_eGamma, TrigCaloTowerMaker_eGamma, TrigCaloClusterMaker_slw
        #theTrigCaloCellMaker_eGamma      = TrigCaloCellMaker_eGamma("TrigCaloCellMaker_eGamma_heavyIon")
        theTrigCaloTowerMaker_eGamma     = TrigCaloTowerMaker_eGamma("TrigCaloTowerMaker_eGamma_heavyIon")
        theTrigCaloClusterMaker_slw      = TrigCaloClusterMaker_slw("TrigCaloClusterMaker_slw")
        
        from TrigEgammaHypo.TrigEFCaloCalibFexConfig import TrigEFCaloCalibFex_Electron
        theTrigEFCaloCalibFex = TrigEFCaloCalibFex_Electron()
        # EF Tracking
        from InDetTrigRecExample.EFInDetConfig import TrigEFIDInsideOut_Electron
        theEFElectronIDFex           = TrigEFIDInsideOut_Electron("Electron").getSequence()
        # GSF refit
        #from TrigEgammaHypo.TrigGSFTrackFexConfig import TrigGSFTrackFex_Electron
        #theGSFtrackfex = TrigGSFTrackFex_Electron()

        # Ringer FEX for L2 Calo
        from TrigT2CaloEgamma.TrigT2CaloEgammaConfig        import T2CaloEgamma_Ringer
        theT2CaloEgamma_Ringer  = T2CaloEgamma_Ringer()
        
        #print 'ESETUP', self.chainPart
        # these can be made more configurable later (according to tracking algorithms etc...)
        
        algoSuffix = "e%s_%s()" % (str(threshold),IDinfo)
        if 'etcut' in self.chainPart['addInfo']:
            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_NoCut
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_NoCut
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_EtCut
            theL2CaloHypo      = L2CaloHypo_NoCut("L2CaloHypo_e"+str(threshold)+"_NoCut",threshold ) 
            theTrigEFCaloHypo = TrigEFCaloHypo_EtCut("TrigEFCaloHypo_e"+str(threshold)+"_EtCut_heavyIon",threshold);
            theEFElectronHypo  = TrigEFElectronHypo_e_NoCut("TrigEFElectronHypo_e"+str(threshold)+"_NoCut_heavyIon",threshold)
        elif self.chainPart['IDinfo']:
            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_NoCut
            theL2CaloHypo = L2CaloHypo_NoCut("L2CaloHypo_e"+str(threshold)+"_NoCut",threshold )
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_e_ID
            # EF Calo
            theTrigEFCaloHypo = TrigEFCaloHypo_e_ID("TrigEFCaloHypo_e"+str(threshold)+"_"+str(IDinfo)+"_heavyIon",threshold,IDinfo);
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_NoCut
            theEFElectronHypo  = \
                TrigEFElectronHypo_e_NoCut("TrigEFElectronHypo_e"+str(threshold)+"_NoCut_heavyIon",threshold)

        # EF Electron FEX
        if 'conv' in self.chainPart['addInfo']:
            theTrigEgammaFex = TrigEgammaRec_Conv_eGamma()
        else :
            theTrigEgammaFex = TrigEgammaRec_eGamma()


        ########### Sequences ##########
        from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
        [trkfast, trkprec] = TrigInDetSequence("Electron", "electron", "IDTrig").getSequence()
        trkseq=trkfast+trkprec

        #calo thresholds here?
        self.L2sequenceList += [[self.L2InputTE, 
                                 [theT2CaloEgamma_Ringer, theL2CaloHypo], 
                                 'L2_e_step1']]
        self.EFsequenceList += [[['L2_e_step1'], 
                                     [theFSCellMaker], 'EF_e_step1_fs']]

        self.EFsequenceList += [[['EF_e_step1_fs'], 
                                 [theUEMaker], 'EF_e_step1_ue']]

        self.EFsequenceList += [[['L2_e_step1'], 
                                 [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw], 
                                 'EF_e_step1']]
        
        self.EFsequenceList += [[['EF_e_step1'], 
                                 [theTrigEFCaloCalibFex, theElectronUEMonitoring, theTrigEFCaloHypo], 
                                 'EF_e_step2']]
        
        if 'L2Star' in self.chainPart['addInfo']:
            self.EFsequenceList += [[['EF_e_step2'], 
                                     [theTrigL2SiTrackFinder_eGammaA]+theL2StarxAOD+
                                     [theTrigL2SiTrackFinder_eGammaB]+theL2StarxAOD+
                                     [theTrigL2SiTrackFinder_eGammaC]+theL2StarxAOD+theEFElectronIDFex,
                                     'EF_e_step3']]
        elif 'gsf' in self.chainPart['addInfo']:
            logElectronDef.error("GSF chains are not configured yet")
        #    self.EFsequenceList += [[['EF_e_step2'], 
        #                             trkseq + [theGSFTrackFex],
        #                             'EF_e_step3']]
        else:
            self.EFsequenceList += [[['EF_e_step2'], 
                                     trkseq,
                                     'EF_e_step3']]
        
        self.EFsequenceList += [[['EF_e_step3'], 
                                 [theTrigEgammaFex, theEFElectronHypo], 
                                 'EF_e_step4']]

        ########### Signatures ###########

        self.L2signatureList += [ [['L2_e_step1']*self.mult] ]
        self.L2signatureList += [ [['EF_e_step1_fs']] ]
        self.L2signatureList += [ [['EF_e_step1_ue']] ]
        #self.L2signatureList += [ [['L2_e_step2']*self.mult] ]
        #self.L2signatureList += [ [['L2_e_step3']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step3']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step4']*self.mult] ]

        ########### TE renaming ###########

        self.TErenamingDict = {
            'L2_e_step1': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'cl'),
            'EF_e_step1_fs': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'fs'),
            'EF_e_step1_ue': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'ue'),
            'EF_e_step1': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'cl'),
            'EF_e_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'calocalib'),
            'EF_e_step3': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'trk'),
            'EF_e_step4': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
            }

    def setup_eXXvh_ID_ringer(self):

        threshold = self.chainPart['threshold']
        IDinfo    = self.chainPart['IDinfo']
        isoInfo   = self.chainPart['isoInfo']
        addInfo   = self.chainPart['addInfo']
        trkInfo   = self.chainPart['trkInfo']
      
        logElectronDef.debug('setup_eXXvh_ID_run2')
        logElectronDef.debug('threshold: %s',threshold)
        logElectronDef.debug('isoInfo: %s',isoInfo)
        logElectronDef.debug('IDinfo: %s',IDinfo)
        logElectronDef.debug('trkInfo: %s',trkInfo)
      

        # common imports required for EtCut and Electron ID chains
        # Ringer FEX for L2 Calo
        from TrigT2CaloEgamma.TrigT2CaloEgammaConfig        import T2CaloEgamma_Ringer
        theT2CaloEgamma_Ringer  = T2CaloEgamma_Ringer()

        # EF Calo
        from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_eGamma, TrigCaloTowerMaker_eGamma, TrigCaloClusterMaker_slw
        theTrigCaloCellMaker_eGamma      = TrigCaloCellMaker_eGamma()
        theTrigCaloTowerMaker_eGamma     = TrigCaloTowerMaker_eGamma()
        theTrigCaloClusterMaker_slw      = TrigCaloClusterMaker_slw()
       
        from TrigEgammaHypo.TrigEFCaloCalibFexConfig import TrigEFCaloCalibFex_Electron
        theTrigEFCaloCalibFex = TrigEFCaloCalibFex_Electron()
        
        # these can be made more configurable later (according to tracking algorithms etc...)
        if 'etcut' in self.chainPart['addInfo']:

            from TrigEgammaHypo.TrigL2CaloRingerHypoConfig  import TrigL2CaloRingerFexHypo_e_EtCut
            from TrigEgammaHypo.TrigEFTrackHypoConfig       import EFTrackHypo_e_NoCut
            from TrigEgammaHypo.TrigEFElectronHypoConfig    import TrigEFElectronHypo_e_NoCut

            [theL2CaloRingerFex,theL2CaloRingerHypo] = TrigL2CaloRingerFexHypo_e_EtCut(threshold)
            theEFTrackHypo      = EFTrackHypo_e_NoCut("EFTrackHypo_e"+str(threshold)+"_NoCut",threshold) 
            
            if 'trkcut' in self.chainPart['addInfo']:
                from TrigMultiVarHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_WTP
                theEFElectronHypo = TrigEFElectronHypo_e_WTP("TrigEFElectronHypo_e"+str(threshold)+"_WTP",threshold)
            else:
                from TrigMultiVarHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_NoCut
                theEFElectronHypo = TrigEFElectronHypo_e_NoCut("TrigEFElectronHypo_e"+str(threshold)+"_NoCut",threshold)

        elif 'perf' in self.chainPart['addInfo']:
            
            from TrigEgammaHypo.TrigEFTrackHypoConfig         import EFTrackHypo_e_NoCut
            from TrigEgammaHypo.TrigEFElectronHypoConfig      import TrigEFElectronHypo_e_NoCut
            from TrigEgammaHypo.TrigEFCaloHypoConfig          import TrigEFCaloHypo_All
            from TrigMultiVarHypo.TrigL2CaloRingerHypoConfig  import TrigL2CaloRingerFexHypo_e_NoCut

            [theL2CaloRingerFex,theL2CaloRingerHypo] = TrigL2CaloRingerFexHypo_e_NoCut(threshold)
            theTrigEFCaloHypo   = TrigEFCaloHypo_All("TrigEFCaloHypo_e"+str(threshold)+"_NoCut",threshold);
            theEFTrackHypo      = EFTrackHypo_e_NoCut("EFTrackHypo_e"+str(threshold)+"_NoCut",threshold) 
            theEFElectronHypo   = TrigEFElectronHypo_e_NoCut("TrigEFElectronHypo_e"+str(threshold)+"_NoCut",threshold)


        elif self.chainPart['IDinfo']:

            algoSuffix = "e%s_%s()" % (str(threshold),IDinfo)
            logElectronDef.debug('chain suffix: %s', algoSuffix)
            
            from TrigEgammaHypo.TrigEFElectronHypoConfig    import TrigEFElectronHypo_e_ID
            from TrigEgammaHypo.TrigEFElectronHypoConfig    import TrigEFElectronHypo_e_Iso
            from TrigEgammaHypo.TrigEFElectronHypoConfig    import TrigEFElectronHypo_e_LH
            from TrigEgammaHypo.TrigEFElectronHypoConfig    import TrigEFElectronHypo_e_LH_Iso
            from TrigEgammaHypo.TrigEFTrackHypoConfig       import EFTrackHypo_e

            # L2 Calo
            from TrigMultiVarHypo.TrigL2CaloRingerHypoConfig  import TrigL2CaloRingerFexHypo_e_ID
            [theL2CaloRingerFex, theL2CaloRingerHypo] = TrigL2CaloRingerFexHypo_e_ID(threshold,IDinfo,self.chainName)
            # EF Track
            theEFTrackHypo     = EFTrackHypo_e("EFTrackHypo_e"+str(threshold)+"_"+str(IDinfo),threshold)
            # EF Electron
            if self.chainPart['isoInfo']:
                if self.chainPart['lhInfo']:
                    lhInfo = self.chainPart['lhInfo']
                    theEFElectronHypo = TrigEFElectronHypo_e_LH_Iso("TrigEFElectronHypo_e"+str(threshold)+\
                            "_"+str(IDinfo)+"_"+str(lhInfo)+"_"+str(isoInfo),threshold,IDinfo,lhInfo,isoInfo)
                else:
                    theEFElectronHypo  = TrigEFElectronHypo_e_Iso("TrigEFElectronHypo_e"+str(threshold)+\
                            "_"+str(IDinfo)+"_"+str(isoInfo),threshold,IDinfo,isoInfo)
            else: 
                if self.chainPart['lhInfo']:
                    lhInfo = self.chainPart['lhInfo']
                    theEFElectronHypo = TrigEFElectronHypo_e_LH("TrigEFElectronHypo_e"+str(threshold)+\
                            "_"+str(IDinfo)+"_"+str(lhInfo),threshold,IDinfo,lhInfo)
                else:
                    theEFElectronHypo  = TrigEFElectronHypo_e_ID("TrigEFElectronHypo_e"+str(threshold)+\
                                                                 "_"+str(IDinfo),threshold,IDinfo)
        
        else:
            algoSuffix = "e%s_%s()" % (str(threshold),IDinfo)
            logElectronDef.error('Chain %s could not be assembled', self.chainPartName)
            logElectronDef.erro('chain suffix: %s', algoSuffix)
            return False
        
        # EF Electron FEX
        if 'conv' in self.chainPart['addInfo']:
            theTrigEgammaFex = TrigEgammaRec_Conv_eGamma()
        else :
            theTrigEgammaFex = TrigEgammaRec_eGamma()
        ########### Sequences ###########
        
        from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
        [trkfast, trkprec] = TrigInDetSequence("Electron", "electron", "IDTrig").getSequence()

        self.L2sequenceList += [[self.L2InputTE, 
                                     [theT2CaloEgamma_Ringer,theL2CaloRingerFex,theL2CaloRingerHypo], 
                                     'L2_e_step1']]
        
        self.EFsequenceList += [[['L2_e_step1'], 
                                 [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
                                 'EF_e_step1']]
        
        self.EFsequenceList += [[['EF_e_step1'], 
                                 [theTrigEFCaloCalibFex], 
                                 'EF_e_step2']]
        
        self.EFsequenceList += [[['EF_e_step2'], 
                                 trkfast+trkprec+[ theEFTrackHypo],
                                 'EF_e_step3']]
        
        self.EFsequenceList += [[['EF_e_step3'], 
                                 [theTrigEgammaFex, theEFElectronHypo], 
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
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_NoCut
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_EtCut
            theL2CaloHypo      = L2CaloHypo_NoCut("L2CaloHypo_e"+str(threshold)+"_NoCut",threshold ) 
            theTrigEFCaloHypo = TrigEFCaloHypo_EtCut("TrigEFCaloHypo_e"+str(threshold)+"_EtCut",threshold);
            theEFTrackHypo     = EFTrackHypo_e_NoCut("EFTrackHypo_e"+str(threshold)+"_NoCut",threshold) 
            #theEFElectronHypo  = TrigEFElectronHypo_e_NoCut("TrigEFElectronHypo_e"+str(threshold)+"_NoCut",threshold)
            if 'trkcut' in self.chainPart['addInfo']:
                from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_WTP
                theEFElectronHypo = TrigEFElectronHypo_e_WTP("TrigEFElectronHypo_e"+str(threshold)+"_WTP",threshold)
            else:
                from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_NoCut
                theEFElectronHypo = TrigEFElectronHypo_e_NoCut("TrigEFElectronHypo_e"+str(threshold)+"_NoCut",threshold)
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
        
        # EF Electron FEX
        if 'conv' in self.chainPart['addInfo']:
            theTrigEgammaFex = TrigEgammaRec_Conv_eGamma()
        else :
            theTrigEgammaFex = TrigEgammaRec_eGamma()
        ########### Sequences ###########
        
        from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
        [trkfast, trkprec] = TrigInDetSequence("Electron", "electron", "IDTrig").getSequence()

        if 'perf' in self.chainPart['addInfo'] or 'etcut' in self.chainPart['addInfo']:
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
                                 trkfast+trkprec+[ theEFTrackHypo],
                                 'EF_e_step3']]
        
        self.EFsequenceList += [[['EF_e_step3'], 
                                 [theTrigEgammaFex, theEFElectronHypo], 
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
        #print 'ESETUP', self.chainPart
        # these can be made more configurable later (according to tracking algorithms etc...)
        if 'etcut' in self.chainPart['addInfo']:
            from TrigEgammaHypo.TrigEFTrackHypoConfig import EFTrackHypo_e_NoCut
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_NoCut
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_EtCut
            theTrigEFCaloHypo = TrigEFCaloHypo_EtCut("TrigEFCaloHypo_e"+str(threshold)+"_EtCut",threshold);
            theEFTrackHypo     = EFTrackHypo_e_NoCut("EFTrackHypo_e"+str(threshold)+"_NoCut",threshold) 
            #theEFElectronHypo  = TrigEFElectronHypo_e_NoCut("TrigEFElectronHypo_e"+str(threshold)+"_NoCut",threshold)
            if 'trkcut' in self.chainPart['addInfo']:
                from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_WTP
                theEFElectronHypo = TrigEFElectronHypo_e_WTP("TrigEFElectronHypo_e"+str(threshold)+"_WTP",threshold)
            else:
                from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_NoCut
                theEFElectronHypo = TrigEFElectronHypo_e_NoCut("TrigEFElectronHypo_e"+str(threshold)+"_NoCut",threshold)
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
        
        # EF Electron FEX
        if 'conv' in self.chainPart['addInfo']:
            theTrigEgammaFex = TrigEgammaRec_Conv_eGamma()
        else :
            theTrigEgammaFex = TrigEgammaRec_eGamma()
        ########### Sequences ###########
        
        trkcomb1st = list(theTrigEFIDDataPrep_Electron)
        trkcomb1st.append(theTrigFastTrackFinder_Electron)
        trkcomb1st += theFastTrackFinderxAOD
        trkcombfull = list(trkcomb1st)
        trkcombfull += theTrigEFIDInsideOutMerged_Electron

        from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
        [trkfast, trkprec] = TrigInDetSequence("Electron", "electron", "IDTrig").getSequence()
        trkseq=trkfast+trkprec
        
        self.EFsequenceList += [[self.L2InputTE, 
                                 [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
                                 'EF_e_step1']]
        
        self.EFsequenceList += [[['EF_e_step1'], 
                                 [theTrigEFCaloCalibFex,theTrigEFCaloHypo], 
                                 'EF_e_step2']]
        
        self.EFsequenceList += [[['EF_e_step2'], 
                                 trkseq+[ theEFTrackHypo],
                                 'EF_e_step3']]
        
        self.EFsequenceList += [[['EF_e_step3'], 
                                 [theTrigEgammaFex, theEFElectronHypo], 
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
        disableMon = not KeepMonitoring(self.chainName,EgammaChainsToKeepMonitoring)    
        
        # common imports required for EtCut and Electron ID chains
        # L2 Calo FEX
        from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_eGamma
        theT2CaloEgamma_eGamma            = T2CaloEgamma_eGamma()
       
        # Ringer FEX for L2 Calo
        from TrigT2CaloEgamma.TrigT2CaloEgammaConfig        import T2CaloEgamma_Ringer
        theT2CaloEgamma_Ringer  = T2CaloEgamma_Ringer()

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
        
        # GSF refit
        #from TrigEgammaHypo.TrigGSFTrackFexConfig import TrigGSFTrackFex_Electron
        #theGSFTrackFex = TrigGSFTrackFex_Electron()
        # EF Electron FEX
        #from TrigEgammaRec.TrigEgammaRecConfig       import TrigEgammaRec
        #theTrigEgammaRec_eGamma                  = TrigEgammaRec(name = "TrigEgammaRec_eGamma")
        #print 'ESETUP', self.chainPart
        # these can be made more configurable later (according to tracking algorithms etc...)
        if 'etcut' in self.chainPart['addInfo']:
            logElectronDef.debug('setup_eXXvh_ID_run2 etcut')
            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_NoCut
            from TrigEgammaHypo.TrigL2ElectronHypoConfig import L2ElectronHypo_e_NoCut
            from TrigEgammaHypo.TrigEFTrackHypoConfig import EFTrackHypo_e_NoCut
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_NoCut
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_EtCut
            theL2CaloHypo      = L2CaloHypo_NoCut("L2CaloHypo_e"+str(threshold)+"_NoCut",threshold ) 
            theL2ElectronHypo  = L2ElectronHypo_e_NoCut("L2ElectronHypo_e"+str(threshold)+"_NoCut",threshold ) 
            theTrigEFCaloHypo = TrigEFCaloHypo_EtCut("TrigEFCaloHypo_e"+str(threshold)+"_EtCut",threshold);
            theEFTrackHypo     = EFTrackHypo_e_NoCut("EFTrackHypo_e"+str(threshold)+"_NoCut",threshold) 
            #theEFElectronHypo  = TrigEFElectronHypo_e_NoCut("TrigEFElectronHypo_e"+str(threshold)+"_NoCut",threshold)
            if 'trkcut' in self.chainPart['addInfo']:
                logElectronDef.debug('setup_eXXvh_ID_run2 etcut_trkcut')
                from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_WTP
                theEFElectronHypo = TrigEFElectronHypo_e_WTP("TrigEFElectronHypo_e"+str(threshold)+"_WTP",threshold)
            else:
                from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_NoCut
                theEFElectronHypo = TrigEFElectronHypo_e_NoCut("TrigEFElectronHypo_e"+str(threshold)+"_NoCut",threshold)
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
                    theEFElectronHypo = TrigEFElectronHypo_e_LH_Iso("TrigEFElectronHypo_e"+str(threshold)+"_"+str(IDinfo)+"_"+str(lhInfo)+"_"+str(isoInfo),threshold,IDinfo,lhInfo,isoInfo)
                else:
                    theEFElectronHypo  = TrigEFElectronHypo_e_Iso("TrigEFElectronHypo_e"+str(threshold)+"_"+str(IDinfo)+"_"+str(isoInfo),threshold,IDinfo,isoInfo)
            elif 'etisem' in self.chainPart['addInfo']:
                theEFElectronHypo  = TrigEFElectronHypo_e_ID_EtIsEM("TrigEFElectronHypo_e"+str(threshold)+"_"+str(IDinfo)+"_EtIsEM",threshold,IDinfo)
            elif self.chainPart['lhInfo']:
                lhInfo = self.chainPart['lhInfo']
                theEFElectronHypo = TrigEFElectronHypo_e_LH("TrigEFElectronHypo_e"+str(threshold)+"_"+str(IDinfo)+"_"+str(lhInfo),threshold,IDinfo,lhInfo)
            else: 
                theEFElectronHypo  = TrigEFElectronHypo_e_ID("TrigEFElectronHypo_e"+str(threshold)+"_"+str(IDinfo),threshold,IDinfo)
        else:
            algoSuffix = "e%s_%s()" % (str(threshold),IDinfo)
            logElectronDef.error('Chain %s could not be assembled', self.chainPartName)
            logElectronDef.erro('chain suffix: %s', algoSuffix)
            return False
        
        if 'conv' in self.chainPart['addInfo']:
            theTrigEgammaFex = TrigEgammaRec_Conv_eGamma()
        else :
            theTrigEgammaFex = TrigEgammaRec_eGamma()

        ########### Sequences ###########
        
        trkcomb1st = list(theTrigEFIDDataPrep_Electron)
        trkcomb1st.append(theTrigFastTrackFinder_Electron)
        trkcomb1st += theFastTrackFinderxAOD
        trkcombfull = list(trkcomb1st)
        trkcombfull += theTrigEFIDInsideOutMerged_Electron
       
        if ( disableMon ) : 
            theL2CaloHypo.AthenaMonTools = DisableMonitoringButValAndTime(theL2CaloHypo.AthenaMonTools)



 
        from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
        [trkfast, trkprec] = TrigInDetSequence("Electron", "electron", "IDTrig").getSequence()
        trkseq=trkfast+trkprec
        
        if 'perf' in self.chainPart['addInfo'] or 'etcut' in self.chainPart['addInfo']:
            self.L2sequenceList += [[self.L2InputTE, 
                                     [theT2CaloEgamma_Ringer, theL2CaloHypo], 
                                     'L2_e_step1']]
        else:
            self.L2sequenceList += [[self.L2InputTE, 
                                     [theT2CaloEgamma_eGamma, theL2CaloHypo], 
                                     'L2_e_step1']]
        
        self.L2sequenceList += [[['L2_e_step1'],    
                                 trkfast, 
                                 'L2_e_step2']]
      
        if ( disableMon ) : theL2ElectronFex.AthenaMonTools=DisableMonitoringButValAndTime(theL2ElectronFex.AthenaMonTools)
        if ( disableMon ) : theL2ElectronHypo.AthenaMonTools=DisableMonitoringButValAndTime(theL2ElectronHypo.AthenaMonTools)
  
        self.L2sequenceList += [[['L2_e_step2'], 
                                 [theL2ElectronFex, theL2ElectronHypo], 
                                 'L2_e_step3']]
        
        self.EFsequenceList += [[['L2_e_step3'], 
                                 [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
                                 'EF_e_step1']]
        
        if ( disableMon ) : theTrigEFCaloHypo.AthenaMonTools=DisableMonitoringButValAndTime(theTrigEFCaloHypo.AthenaMonTools)

        self.EFsequenceList += [[['EF_e_step1'], 
                                 [theTrigEFCaloCalibFex,theTrigEFCaloHypo], 
                                 'EF_e_step2']]
        if 'gsf' in self.chainPart['addInfo']:
            logElectronDef.error("GSF chains are not configured yet")
        #    self.EFsequenceList += [[['EF_e_step2'], 
        #                             theEFElectronIDFex+[ theGSFTrackFex],
        #                             'EF_e_step3']]
        else:
            self.EFsequenceList += [[['EF_e_step2'], 
                                     trkprec+[ theEFTrackHypo],
                                     #trkcombfull+[ theEFTrackHypo],
                                     'EF_e_step3']]
     
        #if ( disableMon ) : theTrigEgammaFex.AthenaMonTools=DisableMonitoringButValAndTime(theTrigEgammaFex.AthenaMonTools)
        if ( disableMon ) : theEFElectronHypo.AthenaMonTools=DisableMonitoringButValAndTime(theEFElectronHypo.AthenaMonTools)
   
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

    def setup_eXXvh_ID_run2_heavyIon(self):
        threshold = self.chainPart['threshold']
        IDinfo = self.chainPart['IDinfo']
        isoInfo = self.chainPart['isoInfo']
        addInfo = self.chainPart['addInfo']
        trkInfo = self.chainPart['trkInfo']
      
        logElectronDef.debug('setup_eXXvh_ID_run2_heavyIon')
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

        # L2 Elecgtron FEX
        from TrigEgammaHypo.TrigL2ElectronFexConfig  import L2ElectronFex_1
        # Depends on L2 Strategy
        theL2ElectronFex = L2ElectronFex_1()
 
        # EF Calo
        from TrigHIHypo.UE import theUEMaker, theFSCellMaker, theTrigCaloCellMaker_eGamma, theElectronUEMonitoring
        print "imported UE stuff there"
        from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_eGamma, TrigCaloTowerMaker_eGamma, TrigCaloClusterMaker_slw
        #theTrigCaloCellMaker_eGamma      = TrigCaloCellMaker_eGamma("TrigCaloCellMaker_eGamma_heavyIon")
        theTrigCaloTowerMaker_eGamma     = TrigCaloTowerMaker_eGamma("TrigCaloTowerMaker_eGamma_heavyIon")
        theTrigCaloClusterMaker_slw      = TrigCaloClusterMaker_slw("TrigCaloClusterMaker_slw")
       
        from TrigEgammaHypo.TrigEFCaloCalibFexConfig import TrigEFCaloCalibFex_Electron
        theTrigEFCaloCalibFex = TrigEFCaloCalibFex_Electron()
        
        # EF Tracking
        theEFElectronIDFex           = theTrigEFIDInsideOutMerged_Electron
        
        # GSF refit
        #from TrigEgammaHypo.TrigGSFTrackFexConfig import TrigGSFTrackFex_Electron
        #theGSFTrackFex = TrigGSFTrackFex_Electron()
        # EF Electron FEX
        #from TrigEgammaRec.TrigEgammaRecConfig       import TrigEgammaRec
        #theTrigEgammaRec_eGamma                  = TrigEgammaRec(name = "TrigEgammaRec_eGamma")
        #print 'ESETUP', self.chainPart
        # these can be made more configurable later (according to tracking algorithms etc...)
        if 'etcut' in self.chainPart['addInfo']:

            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_EtCut
            from TrigEgammaHypo.TrigL2ElectronHypoConfig import L2ElectronHypo_e_NoCut
            from TrigEgammaHypo.TrigEFTrackHypoConfig import EFTrackHypo_e_NoCut
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_NoCut
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_EtCut
            theL2CaloHypo      = L2CaloHypo_EtCut("L2CaloHypo_e"+str(threshold)+"_EtCut",threshold ) 
            theL2ElectronHypo  = L2ElectronHypo_e_NoCut("L2ElectronHypo_e"+str(threshold)+"_NoCut",threshold ) 
            theTrigEFCaloHypo = TrigEFCaloHypo_EtCut("TrigEFCaloHypo_e"+str(threshold)+"_EtCut_heavyIon",threshold);
            theEFTrackHypo     = EFTrackHypo_e_NoCut("EFTrackHypo_e"+str(threshold)+"_NoCut_heavyIon",threshold) 
            #theEFElectronHypo  = TrigEFElectronHypo_e_NoCut("TrigEFElectronHypo_e"+str(threshold)+"_NoCut_heavyIon",threshold)
            if 'trkcut' in self.chainPart['addInfo']:
                from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_WTP
                theEFElectronHypo = TrigEFElectronHypo_e_WTP("TrigEFElectronHypo_e"+str(threshold)+"_WTP",threshold)
            else:
                from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_NoCut
                theEFElectronHypo = TrigEFElectronHypo_e_NoCut("TrigEFElectronHypo_e"+str(threshold)+"_NoCut",threshold)
        elif 'perf' in self.chainPart['addInfo']:
            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_NoCut
            from TrigEgammaHypo.TrigL2ElectronHypoConfig import L2ElectronHypo_e_NoCut
            from TrigEgammaHypo.TrigEFTrackHypoConfig import EFTrackHypo_e_NoCut
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_NoCut
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_All
            theL2CaloHypo      = L2CaloHypo_NoCut("L2CaloHypo_e"+str(threshold)+"_NoCut",threshold ) 
            theL2ElectronHypo  = L2ElectronHypo_e_NoCut("L2ElectronHypo_e"+str(threshold)+"_NoCut",threshold ) 
            theTrigEFCaloHypo = TrigEFCaloHypo_All("TrigEFCaloHypo_e"+str(threshold)+"_NoCut",threshold);
            theEFTrackHypo     = EFTrackHypo_e_NoCut("EFTrackHypo_e"+str(threshold)+"_NoCut_heavyIon",threshold) 
            # EF Electron
            theEFElectronHypo  = TrigEFElectronHypo_e_NoCut("TrigEFElectronHypo_e"+str(threshold)+"_NoCut_heavyIon",threshold)

        elif self.chainPart['IDinfo']:
            algoSuffix = "e%s_%s()" % (str(threshold),IDinfo)
            logElectronDef.debug('chain suffix: %s', algoSuffix)
            #if 'mvt' in algoSuffix: 
            #    algoSuffix = algoSuffix.replace('mvt','')
            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_EtCut
            from TrigEgammaHypo.TrigL2ElectronHypoConfig import L2ElectronHypo_e_ID
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_ID_HI
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_Iso
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_ID_EtIsEM_HI
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_LH
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_LH_Iso
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_ID_EtIsEM_Iso
            from TrigEgammaHypo.TrigEFTrackHypoConfig import EFTrackHypo_e
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_All
            # L2 Calo
            theL2CaloHypo =  L2CaloHypo_EtCut("L2CaloHypo_e"+str(threshold)+"_EtCut",threshold )
            
            # L2 Electron
            theL2ElectronHypo  = L2ElectronHypo_e_ID("TrigL2ElectronHypo_e"+str(threshold)+"_"+str(IDinfo),threshold,IDinfo)
            
            # EF Calo
            theTrigEFCaloHypo = TrigEFCaloHypo_All("TrigEFCaloHypo_e"+str(threshold)+"_heavyIon",threshold);
           
            # EF Track
            theEFTrackHypo     = EFTrackHypo_e("EFTrackHypo_e"+str(threshold)+"_"+str(IDinfo),threshold)
            
            # EF Electron
            if self.chainPart['isoInfo']:
                if 'etisem' in self.chainPart['addInfo']:
                    theEFElectronHypo  = TrigEFElectronHypo_e_ID_EtIsEM_Iso("TrigEFElectronHypo_e"+str(threshold)+"_"+str(IDinfo)+"_EtIsEM"+"_"+str(isoInfo)+"_heavyIon",threshold,IDinfo)
                elif self.chainPart['lhInfo']:
                    lhInfo = self.chainPart['lhInfo']
                    theEFElectronHypo = TrigEFElectronHypo_e_LH_Iso("TrigEFElectronHypo_e"+str(threshold)+"_"+str(IDinfo)+"_"+str(lhInfo)+"_"+str(isoInfo)+"_heavyIon",threshold,lhInfo)
                else:
                    theEFElectronHypo  = TrigEFElectronHypo_e_Iso("TrigEFElectronHypo_e"+str(threshold)+"_"+str(IDinfo)+"_"+str(isoInfo)+"_heavyIon",threshold,IDinfo,isoInfo)
            elif 'etisem' in self.chainPart['addInfo']:
                theEFElectronHypo  = TrigEFElectronHypo_e_ID_EtIsEM_ID("TrigEFElectronHypo_e"+str(threshold)+"_"+str(IDinfo)+"_EtIsEM_heavyIon",threshold,IDinfo)
            elif self.chainPart['lhInfo']:
                lhInfo = self.chainPart['lhInfo']
                theEFElectronHypo = TrigEFElectronHypo_e_LH("TrigEFElectronHypo_e"+str(threshold)+"_"+str(IDinfo)+"_"+str(lhInfo)+"_heavyIon",threshold,lhInfo)
            else: 
                theEFElectronHypo  = TrigEFElectronHypo_e_ID_HI("TrigEFElectronHypo_e"+str(threshold)+"_"+str(IDinfo)+"_heavyIon",threshold,IDinfo)
        else:
            algoSuffix = "e%s_%s()" % (str(threshold),IDinfo)
            logElectronDef.error('Chain %s could not be assembled', self.chainPartName)
            logElectronDef.erro('chain suffix: %s', algoSuffix)
            return False
        
        if 'conv' in self.chainPart['addInfo']:
            theTrigEgammaFex = TrigEgammaRec_Conv_eGamma()
        else :
            theTrigEgammaFex = TrigEgammaRec_eGamma()

        ########### Sequences ###########
        
        trkcomb1st = list(theTrigEFIDDataPrep_Electron)
        trkcomb1st.append(theTrigFastTrackFinder_Electron)
        trkcomb1st += theFastTrackFinderxAOD
        trkcombfull = list(trkcomb1st)
        trkcombfull += theTrigEFIDInsideOutMerged_Electron
        
        from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
        [trkfast, trkprec] = TrigInDetSequence("Electron", "electron", "IDTrig").getSequence()
        trkseq=trkfast+trkprec
        
        if 'perf' in self.chainPart['addInfo'] or 'etcut' in self.chainPart['addInfo']:
            self.L2sequenceList += [[self.L2InputTE, 
                                     [theT2CaloEgamma_Ringer, theL2CaloHypo], 
                                     'L2_e_step1']]
        else:
            self.L2sequenceList += [[self.L2InputTE, 
                                     [theT2CaloEgamma_eGamma, theL2CaloHypo], 
                                     'L2_e_step1']]
        
        self.L2sequenceList += [[['L2_e_step1'],    
                                 trkfast, 
                                 'L2_e_step2']]
        
        self.L2sequenceList += [[['L2_e_step2'], 
                                 [theL2ElectronFex, theL2ElectronHypo], 
                                 'L2_e_step3']]
        
        self.EFsequenceList += [[['L2_e_step3'], 
                                 [theFSCellMaker], 'EF_e_step1_fs']]

        self.EFsequenceList += [[['EF_e_step1_fs'], 
                                 [theUEMaker], 'EF_e_step1_ue']]

        self.EFsequenceList += [[['L2_e_step3'], 
                                 [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
                                 'EF_e_step1']]
        
        self.EFsequenceList += [[['EF_e_step1'], 
                                 [theTrigEFCaloCalibFex, theElectronUEMonitoring, theTrigEFCaloHypo], 
                                 'EF_e_step2']]
        if 'gsf' in self.chainPart['addInfo']:
            logElectronDef.error("GSF chains are not configured yet")
        #    self.EFsequenceList += [[['EF_e_step2'], 
        #                             theEFElectronIDFex+[ theGSFTrackFex],
        #                             'EF_e_step3']]
        else:
            self.EFsequenceList += [[['EF_e_step2'], 
                                     trkprec+[ theEFTrackHypo],
                                     #trkcombfull+[ theEFTrackHypo],
                                     'EF_e_step3']]
        
        self.EFsequenceList += [[['EF_e_step3'], 
                                 [theTrigEgammaFex, theEFElectronHypo], 
                                 'EF_e_step4']]

        ########### Signatures ###########

        self.L2signatureList += [ [['L2_e_step1']*self.mult] ]
        self.L2signatureList += [ [['L2_e_step2']*self.mult] ]
        self.L2signatureList += [ [['L2_e_step3']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step1_fs']] ]
        self.EFsignatureList += [ [['EF_e_step1_ue']] ]
        self.EFsignatureList += [ [['EF_e_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step3']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step4']*self.mult] ]
        ########### TE renaming ###########

        self.TErenamingDict = {
            'L2_e_step1': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'cl'),
            'L2_e_step2': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'id'),
            'L2_e_step3': mergeRemovingOverlap('L2_', self.chainPartNameNoMult),
            'EF_e_step1_fs': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'fs'),
            'EF_e_step1_ue': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'ue'),
            'EF_e_step1': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'calo'),
            'EF_e_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'calocalib'),
            'EF_e_step3': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'id'),
            'EF_e_step4': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
            }
