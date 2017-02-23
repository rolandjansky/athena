# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Electron trigger slice signatures  """

__author__  = 'Moritz Backes & Catrin Bernius'
__version__=""
__doc__="Implementation of Electron Signature"

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenu.egamma.ElectronDef")

from TriggerMenu.menu.HltConfig import L2EFChainDef, mergeRemovingOverlap

##########################################################################################
#
# Import and create fexes and hypos
# Move global imports to class methods
# Leave commented for now
########################################################################################## 
#from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_eGamma, T2CaloEgamma_SwSeed, T2CaloEgamma_SwSeed_NoCut
#from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_SwCluster, T2CaloEgamma_Ringer, T2CaloEgamma_cells
from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_cells
#from TrigL2MissingET.TrigL2MissingETConfig import L2CaloMissingET_Fex

#theT2CaloEgamma_eGamma            = T2CaloEgamma_eGamma()
#theT2CaloEgamma_SwSeed            = T2CaloEgamma_SwSeed()
#theT2CaloEgamma_SwSeed_NoCut      = T2CaloEgamma_SwSeed_NoCut()
#theT2CaloEgamma_SwCluster         = T2CaloEgamma_SwCluster()
#theT2CaloEgamma_Ringer            = T2CaloEgamma_Ringer()
#theL2CaloMissingET_Fex            = L2CaloMissingET_Fex() 
theT2CaloEgamma_cells_e           = T2CaloEgamma_cells("T2CaloEgamma_cells")

#from TrigIDSCAN.TrigIDSCAN_Config import TrigIDSCAN_eGamma
#from TrigSiTrack.TrigSiTrack_Config import TrigSiTrack_eGamma, TrigSiTrack_eGamma_robust
#from TrigL2TRTSegFinder.TrigTRTSegFinder_Config import TrigTRTSegFinder_eGamma

#theTrigIDSCAN_eGamma              = TrigIDSCAN_eGamma()
#theTrigSiTrack_eGamma             = TrigSiTrack_eGamma()
#theTrigSiTrack_eGamma_robust      = TrigSiTrack_eGamma_robust()
#theTrigTRTSegFinder_eGamma        = TrigTRTSegFinder_eGamma()

#from TrigEgammaHypo.TrigL2CaloHypoConfig     import *
#from TrigEgammaHypo.TrigL2ElectronFexConfig  import *
#from TrigEgammaHypo.TrigL2ElectronHypoConfig import *
#from TrigMultiVarHypo.TrigMultiVarHypoConfig import TrigRingerNeuralHypoConfig_e10
#from TrigMultiVarHypo.TrigMultiVarHypoConfig import TrigRingerNeuralHypoConfig_e5
#from TrigMultiVarHypo.TrigMultiVarHypoConfig import TrigRingerNeuralHypoConfig_e5_NoCut
#from TrigMultiVarHypo.TrigMultiVarHypoConfig import TrigRingerNeuralFexConfig_e5
#from TrigMultiVarHypo.TrigMultiVarHypoConfig import TrigRingerNeuralFexConfig_e10

#from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_eGamma,   TrigCaloCellMaker_eGamma_cells,  TrigCaloTowerMaker_eGamma, TrigCaloClusterMaker_slw
#theTrigCaloCellMaker_eGamma      = TrigCaloCellMaker_eGamma()
#theTrigCaloTowerMaker_eGamma     = TrigCaloTowerMaker_eGamma()
#theTrigCaloClusterMaker_slw      = TrigCaloClusterMaker_slw()

#from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_eGamma_cells
#theTrigCaloCellMaker_eGamma_cells= TrigCaloCellMaker_eGamma_cells()

from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
[trkfast, trkprec] = TrigInDetSequence("Electron", "electron", "IDTrig").getSequence()

from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
[theFastTrackFinderxAOD]                  = TrigInDetSequence("Electron","electron","FastxAOD").getSequence()
[theL2StarxAOD]                           = TrigInDetSequence("Electron","electron","L2StarxAOD").getSequence()

#theTrigEFIDOutsideInTRTOnly_Electron    = TrigEFIDSequence("Electron","electron","TRTOnly").getSequence()
#theTrigEFIDCombined_Electron            = TrigEFIDCombined("Electron","photon", "Combined").getSequence()
from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_eGamma as TrigFastTrackFinder_Electron
theTrigFastTrackFinder_Electron = TrigFastTrackFinder_Electron()

#from TrigEgammaHypo.TrigEFTrackHypoConfig    import *
#from TrigEgammaHypo.TrigEFElectronHypoConfig import *
#from TrigEgammaRec.TrigEgammaRecConfig       import *
#theTrigEgammaRec_eGamma                  = TrigEgammaRec_eGamma()


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
        
        log.info('setup_eXXvh_ID')
        log.info('threshold: %s',threshold)
        log.info('isoInfo: %s',isoInfo)
        log.info('IDinfo: %s',IDinfo)
        log.info('addInfo: %s',addInfo)
        log.info('trkInfo: %s',trkInfo)
       
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
        # Depends on L2 Strategy
        log.info('L2IDAlg: %s', self.chainPart['L2IDAlg'])
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
                log.info('Incorrect L2IDAlg')
        # Default
        else:
            log.info('Use default L2StarA tracking')
            theL2TrackingFex = TrigL2SiTrackFinder_eGammaA()
            theL2ElectronFex = L2ElectronFex_L2StarA()
 
        

        # EF Calo
        from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_eGamma, TrigCaloTowerMaker_eGamma, TrigCaloClusterMaker_slw
        theTrigCaloCellMaker_eGamma      = TrigCaloCellMaker_eGamma()
        theTrigCaloTowerMaker_eGamma     = TrigCaloTowerMaker_eGamma()
        theTrigCaloClusterMaker_slw      = TrigCaloClusterMaker_slw()
        
        # EF Tracking
        from InDetTrigRecExample.EFInDetConfig import TrigEFIDInsideOut_Electron
        theEFElectronIDFex           = TrigEFIDInsideOut_Electron("Electron").getSequence()
        
        # EF Electron FEX
        from TrigEgammaRec.TrigEgammaRecConfig       import TrigEgammaRec_eGamma
        theTrigEgammaRec_eGamma                  = TrigEgammaRec_eGamma()
        #print 'ESETUP', self.chainPart
        # these can be made more configurable later (according to tracking algorithms etc...)
        if 'etcut' in self.chainPart['addInfo']:

            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_NoCut
            from TrigEgammaHypo.TrigL2ElectronHypoConfig import L2ElectronHypo_e_NoCut
            from TrigEgammaHypo.TrigEFTrackHypoConfig import EFTrackHypo_e_NoCut
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_EtCut
            theL2CaloHypo      = L2CaloHypo_NoCut("L2CaloHypo_e"+str(threshold)+"_NoCut",threshold ) 
            theL2ElectronHypo  = L2ElectronHypo_e_NoCut("L2ElectronHypo_e"+str(threshold)+"_NoCut",threshold ) 
            theEFTrackHypo     = EFTrackHypo_e_NoCut("EFTrackHypo_e"+str(threshold)+"_NoCut",threshold) 
            theEFElectronHypo  = TrigEFElectronHypo_e_EtCut("TrigEFElectronHypo_e"+str(threshold)+"_EtCut",threshold)
            
        elif 'perf' in self.chainPart['addInfo']:
            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_NoCut
            from TrigEgammaHypo.TrigL2ElectronHypoConfig import L2ElectronHypo_e_NoCut
            from TrigEgammaHypo.TrigEFTrackHypoConfig import EFTrackHypo_e_NoCut
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_NoCut
            theL2CaloHypo      = L2CaloHypo_NoCut("L2CaloHypo_e"+str(threshold)+"_NoCut",threshold ) 
            theL2ElectronHypo  = L2ElectronHypo_e_NoCut("L2ElectronHypo_e"+str(threshold)+"_NoCut",threshold ) 
            theEFTrackHypo     = EFTrackHypo_e_NoCut("EFTrackHypo_e"+str(threshold)+"_NoCut",threshold) 
            # EF Electron
            theEFElectronHypo  = TrigEFElectronHypo_e_NoCut("TrigEFElectronHypo_e"+str(threshold)+"_NoCut",threshold)
        
        elif self.chainPart['IDinfo']:
            algoSuffix = "e%s_%s()" % (str(threshold),IDinfo)
            log.info('chain suffix: %s', algoSuffix)
            #if 'mvt' in algoSuffix: 
            #    algoSuffix = algoSuffix.replace('mvt','')
            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_e_ID
            from TrigEgammaHypo.TrigL2ElectronHypoConfig import L2ElectronHypo_e_ID
            from TrigEgammaHypo.TrigL2ElectronHypoConfig import L2ElectronHypo_e_ID_L2TrkAlg
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
            
            # EF Electron
            if self.chainPart['isoInfo']:
                theEFElectronHypo  = TrigEFElectronHypo_e_Iso("TrigEFElectronHypo_e"+str(threshold)+"_"+str(IDinfo)+"_"+str(isoInfo),threshold,IDinfo,isoInfo)
            else: 
                theEFElectronHypo  = TrigEFElectronHypo_e_ID("TrigEFElectronHypo_e"+str(threshold)+"_"+str(IDinfo),threshold,IDinfo)
        else:
            algoSuffix = "e%s_%s()" % (str(threshold),IDinfo)
            log.error('Chain %s could not be assembled', self.chainPartName)
            log.erro('chain suffix: %s', algoSuffix)
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
                                 theEFElectronIDFex+[ theEFTrackHypo],
                                 'EF_e_step2']]
        
        self.EFsequenceList += [[['EF_e_step2'], 
                                 [theTrigEgammaRec_eGamma, theEFElectronHypo], 
                                 'EF_e_step3']]

        ########### Signatures ###########

        self.L2signatureList += [ [['L2_e_step1']*self.mult] ]
        self.L2signatureList += [ [['L2_e_step2']*self.mult] ]
        self.L2signatureList += [ [['L2_e_step3']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step3']*self.mult] ]

        ########### TE renaming ###########

        self.TErenamingDict = {
            'L2_e_step1': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'cl'),
            'L2_e_step2': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'id'),
            'L2_e_step3': mergeRemovingOverlap('L2_', self.chainPartNameNoMult),
            'EF_e_step1': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'calo'),
            'EF_e_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'id'),
            'EF_e_step3': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
            }


    def setup_eXXvh_idperf(self):

        threshold = self.chainPart['threshold']
        IDinfo = self.chainPart['IDinfo']
        isoInfo = self.chainPart['isoInfo']
        run1 = self.chainPart['trkInfo']
       
        log.info('setup_eXXvh_idperf')
        log.info('threshold: %s',threshold)
        log.info('isoInfo: %s',isoInfo)
        log.info('IDinfo: %s',IDinfo)
        log.info('trkInfo: %s',run1)
        
        # L2 Tracking FEX
        from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_eGammaA
        from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_eGammaB
        from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_eGammaC
        theTrigL2SiTrackFinder_eGammaA=TrigL2SiTrackFinder_eGammaA()
        theTrigL2SiTrackFinder_eGammaB=TrigL2SiTrackFinder_eGammaB()
        theTrigL2SiTrackFinder_eGammaC=TrigL2SiTrackFinder_eGammaC()
        # EF Calo
        from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_eGamma_cells
        #theTrigCaloCellMaker_eGamma_cells= TrigCaloCellMaker_eGamma_cells()
        from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_eGamma, TrigCaloTowerMaker_eGamma, TrigCaloClusterMaker_slw
        theTrigCaloCellMaker_eGamma      = TrigCaloCellMaker_eGamma()
        theTrigCaloTowerMaker_eGamma     = TrigCaloTowerMaker_eGamma()
        theTrigCaloClusterMaker_slw      = TrigCaloClusterMaker_slw()
        
        # EF Tracking
        from InDetTrigRecExample.EFInDetConfig import TrigEFIDInsideOut_Electron
        theEFElectronIDFex           = TrigEFIDInsideOut_Electron("Electron").getSequence()

        # EF Electron FEX
        from TrigEgammaRec.TrigEgammaRecConfig       import TrigEgammaRec_eGamma
        theTrigEgammaRec_eGamma                  = TrigEgammaRec_eGamma()
        #print 'ESETUP', self.chainPart
        # these can be made more configurable later (according to tracking algorithms etc...)
        
        if 'etcut' in self.chainPart['addInfo']:
            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_NoCut
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_EtCut
            theL2CaloHypo      = L2CaloHypo_NoCut("L2CaloHypo_e"+str(threshold)+"_NoCut",threshold ) 
            theEFElectronHypo  = TrigEFElectronHypo_e_EtCut("TrigEFElectronHypo_e"+str(threshold)+"_EtCut",threshold)
        elif self.chainPart['IDinfo']:
            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_e_ID
            theL2CaloHypo = L2CaloHypo_e_ID("TrigL2CaloHypo_e"+str(threshold)+"_"+str(IDinfo),threshold,IDinfo)
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_ID_CaloOnly
            theEFElectronHypo  = \
                TrigEFElectronHypo_e_ID_CaloOnly("TrigEFElectronHypo_e"+str(threshold)+"_"+str(IDinfo)+"_CaloOnly",threshold,IDinfo)


        ########### Sequences ###########
        trkseq = trkfast+trkprec

        #calo thresholds here?
        self.L2sequenceList += [[self.L2InputTE, 
                                 [theT2CaloEgamma_cells_e, theL2CaloHypo], 
                                 'L2_e_step1']]
        
        self.EFsequenceList += [[['L2_e_step1'], 
                                 [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw], 
                                 'EF_e_step1']]
        
        if 'L2Star' in self.chainPart['addInfo']:
            self.EFsequenceList += [[['EF_e_step1'], 
                                     [theTrigL2SiTrackFinder_eGammaA]+theL2StarxAOD+
                                     [theTrigL2SiTrackFinder_eGammaB]+theL2StarxAOD+
                                     [theTrigL2SiTrackFinder_eGammaC]+theL2StarxAOD+theEFElectronIDFex,
                                     'EF_e_step2']]
        else:
            self.EFsequenceList += [[['EF_e_step1'], 
                                     trkseq,
                                     'EF_e_step2']]
        
        self.EFsequenceList += [[['EF_e_step2'], 
                                 [theTrigEgammaRec_eGamma, theEFElectronHypo], 
                                 'EF_e_step3']]

        ########### Signatures ###########

        self.L2signatureList += [ [['L2_e_step1']*self.mult] ]
        #self.L2signatureList += [ [['L2_e_step2']*self.mult] ]
        #self.L2signatureList += [ [['L2_e_step3']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step3']*self.mult] ]

        ########### TE renaming ###########

        self.TErenamingDict = {
            'L2_e_step1': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'cl'),
            'EF_e_step1': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'cl'),
            'EF_e_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'trk'),
            'EF_e_step3': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
            }


    def setup_eXXvh_ID_run2(self):
        threshold = self.chainPart['threshold']
        IDinfo = self.chainPart['IDinfo']
        isoInfo = self.chainPart['isoInfo']
        trkInfo = self.chainPart['trkInfo']
      
        log.info('setup_eXXvh_ID_run2')
        log.info('threshold: %s',threshold)
        log.info('isoInfo: %s',isoInfo)
        log.info('IDinfo: %s',IDinfo)
        log.info('trkInfo: %s',trkInfo)
       
        # common imports required for EtCut and Electron ID chains
        # L2 Calo FEX
        from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_eGamma
        theT2CaloEgamma_eGamma            = T2CaloEgamma_eGamma()
        
        # L2 Elecgtron FEX
        from TrigEgammaHypo.TrigL2ElectronFexConfig  import L2ElectronFex_1
        # Depends on L2 Strategy
        theL2ElectronFex = L2ElectronFex_1()
 
        # EF Calo
        from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_eGamma, TrigCaloTowerMaker_eGamma, TrigCaloClusterMaker_slw
        theTrigCaloCellMaker_eGamma      = TrigCaloCellMaker_eGamma()
        theTrigCaloTowerMaker_eGamma     = TrigCaloTowerMaker_eGamma()
        theTrigCaloClusterMaker_slw      = TrigCaloClusterMaker_slw()
        
        # EF Tracking
        theEFElectronIDFex           = theTrigEFIDInsideOutMerged_Electron
        # EF Electron FEX
        from TrigEgammaRec.TrigEgammaRecConfig       import TrigEgammaRec_eGamma
        theTrigEgammaRec_eGamma                  = TrigEgammaRec_eGamma()
        #print 'ESETUP', self.chainPart
        # these can be made more configurable later (according to tracking algorithms etc...)
        if 'etcut' in self.chainPart['addInfo']:

            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_NoCut
            from TrigEgammaHypo.TrigL2ElectronHypoConfig import L2ElectronHypo_e_NoCut
            from TrigEgammaHypo.TrigEFTrackHypoConfig import EFTrackHypo_e_NoCut
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_EtCut
            theL2CaloHypo      = L2CaloHypo_NoCut("L2CaloHypo_e"+str(threshold)+"_NoCut",threshold ) 
            theL2ElectronHypo  = L2ElectronHypo_e_NoCut("L2ElectronHypo_e"+str(threshold)+"_NoCut",threshold ) 
            theEFTrackHypo     = EFTrackHypo_e_NoCut("EFTrackHypo_e"+str(threshold)+"_NoCut",threshold) 
            theEFElectronHypo  = TrigEFElectronHypo_e_EtCut("TrigEFElectronHypo_e"+str(threshold)+"_EtCut",threshold)
        elif 'perf' in self.chainPart['addInfo']:
            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_NoCut
            from TrigEgammaHypo.TrigL2ElectronHypoConfig import L2ElectronHypo_e_NoCut
            from TrigEgammaHypo.TrigEFTrackHypoConfig import EFTrackHypo_e_NoCut
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_NoCut
            theL2CaloHypo      = L2CaloHypo_NoCut("L2CaloHypo_e"+str(threshold)+"_NoCut",threshold ) 
            theL2ElectronHypo  = L2ElectronHypo_e_NoCut("L2ElectronHypo_e"+str(threshold)+"_NoCut",threshold ) 
            theEFTrackHypo     = EFTrackHypo_e_NoCut("EFTrackHypo_e"+str(threshold)+"_NoCut",threshold) 
            # EF Electron
            theEFElectronHypo  = TrigEFElectronHypo_e_NoCut("TrigEFElectronHypo_e"+str(threshold)+"_NoCut",threshold)

        elif self.chainPart['IDinfo']:
            algoSuffix = "e%s_%s()" % (str(threshold),IDinfo)
            log.info('chain suffix: %s', algoSuffix)
            #if 'mvt' in algoSuffix: 
            #    algoSuffix = algoSuffix.replace('mvt','')
            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_e_ID
            from TrigEgammaHypo.TrigL2ElectronHypoConfig import L2ElectronHypo_e_ID
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_ID
            from TrigEgammaHypo.TrigEFElectronHypoConfig import TrigEFElectronHypo_e_Iso
            from TrigEgammaHypo.TrigEFTrackHypoConfig import EFTrackHypo_e
            
            # L2 Calo
            theL2CaloHypo = L2CaloHypo_e_ID("TrigL2CaloHypo_e"+str(threshold)+"_"+str(IDinfo),threshold,IDinfo)
            
            # L2 Electron
            theL2ElectronHypo  = L2ElectronHypo_e_ID("TrigL2ElectronHypo_e"+str(threshold)+"_"+str(IDinfo),threshold,IDinfo)
            
            # EF Track
            theEFTrackHypo     = EFTrackHypo_e("EFTrackHypo_e"+str(threshold)+"_"+str(IDinfo),threshold)
            
            # EF Electron
            if self.chainPart['isoInfo']:
                theEFElectronHypo  = TrigEFElectronHypo_e_Iso("TrigEFElectronHypo_e"+str(threshold)+"_"+str(IDinfo)+"_"+str(isoInfo),threshold,IDinfo,isoInfo)
            else: 
                theEFElectronHypo  = TrigEFElectronHypo_e_ID("TrigEFElectronHypo_e"+str(threshold)+"_"+str(IDinfo),threshold,IDinfo)
        else:
            algoSuffix = "e%s_%s()" % (str(threshold),IDinfo)
            log.error('Chain %s could not be assembled', self.chainPartName)
            log.erro('chain suffix: %s', algoSuffix)
            return False
        
        ########### Sequences ###########
        
        trkcomb1st = list(theTrigEFIDDataPrep_Electron)
        trkcomb1st.append(theTrigFastTrackFinder_Electron)
        trkcomb1st += theFastTrackFinderxAOD
        trkcombfull = list(trkcomb1st)
        trkcombfull += theTrigEFIDInsideOutMerged_Electron

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
                                 [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw], 
                                 'EF_e_step1']]
        
        self.EFsequenceList += [[['EF_e_step1'], 
                                 theEFElectronIDFex+[ theEFTrackHypo],
                                 #trkfast+trkprec+[ theEFTrackHypo],
                                 'EF_e_step2']]
        
        self.EFsequenceList += [[['EF_e_step2'], 
                                 [theTrigEgammaRec_eGamma, theEFElectronHypo], 
                                 'EF_e_step3']]

        ########### Signatures ###########

        self.L2signatureList += [ [['L2_e_step1']*self.mult] ]
        self.L2signatureList += [ [['L2_e_step2']*self.mult] ]
        self.L2signatureList += [ [['L2_e_step3']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_e_step3']*self.mult] ]

        ########### TE renaming ###########

        self.TErenamingDict = {
            'L2_e_step1': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'cl'),
            'L2_e_step2': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'id'),
            'L2_e_step3': mergeRemovingOverlap('L2_', self.chainPartNameNoMult),
            'EF_e_step1': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'calo'),
            'EF_e_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'id'),
            'EF_e_step3': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
            }

# class L2EFChain_e_NoIDTrkCut(L2EFChain_e):

#     def defineSequences(self, config):
#         ###### L2 ######
#         # Step 1: Calo Fex and Hypo
#         self.addL2Sequence(self.inputTEs,
#                            [theT2CaloEgamma_cells_e, config.L2Config.TrigL2CaloHypo_1],
#                            'L2_e_step1')
        
#         # Step 2: ID Fex
#         #append "_R1" to IDTrkNoCut chains running IDScan and SiTrack
#         if '_R1' in self.sig_id:
#             self.addL2Sequence(['L2_e_step1'],
#                                [theTrigL2SiTrackFinder_eGammaA,
#                                 theTrigL2SiTrackFinder_eGammaB,
#                                 theTrigL2SiTrackFinder_eGammaC,
#                                 theTrigL2SiTrackFinder_eGammaBC,
#                                 theTrigIDSCAN_eGamma, theTrigSiTrack_eGamma, theTrigTRTSegFinder_eGamma,
#                                 ],
#                                'L2_e_step2')
            
#         elif 'L2Star' in self.sig_id:
#             self.addL2Sequence(['L2_e_step1'],
#                                [theTrigL2SiTrackFinder_eGammaA, theTrigL2SiTrackFinder_eGammaB, theTrigL2SiTrackFinder_eGammaC],
#                                'L2_e_step2')

#         else:
#             #remove IDScan and SiTrack from IDTrkNoCut chains
#             self.addL2Sequence(['L2_e_step1'],
#                                [theTrigL2SiTrackFinder_eGammaA,
#                                 theTrigL2SiTrackFinder_eGammaB,
#                                 theTrigL2SiTrackFinder_eGammaC,
#                                 theTrigL2SiTrackFinder_eGammaBC,
#                                 theTrigTRTSegFinder_eGamma,
#                                 ],
#                                'L2_e_step2')
        
#         # Step 3: ID-Calo Fex and Hypo
#         if 'L2Star' in self.sig_id:
#             self.addL2Sequence(['L2_e_step2'], [config.L2Config.TrigL2ElectronFex_1], 'L2_e_step3')
#         else:
#             self.addL2Sequence(['L2_e_step2'],
#                                [config.L2Config.TrigL2ElectronFex_1, config.L2Config.TrigL2ElectronHypo_1],
#                                'L2_e_step3')
            
        
#         ###### EF ######
#         # Step 1: Calo Fex
#         self.addEFSequence('L2_e_step3', 
#                            [theTrigCaloCellMaker_eGamma_cells, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
#                            'EF_e_step1')
#         # Step 2: ID Fex and Hypo
#         self.addEFSequence('EF_e_step1', theTrigEFIDInsideOut_Electron, 'EF_e_step2')
#         self.addEFSequence('EF_e_step2', theTrigEFIDOutsideInTRTOnly_Electron, 'EF_e_step3')
#         # Step 3: ID-Calo Fex and Hypo
#         self.addEFSequence('EF_e_step3', 
#                            [theTrigEgammaRec_eGamma, config.EFConfig.TrigEFEGammaHypo_1],
#                            'EF_e_step4')


#     def defineSignatures(self):
#         self.addL2Signature(['L2_e_step1']*self.mult)
#         self.addL2Signature(['L2_e_step2']*self.mult)
#         self.addL2Signature(['L2_e_step3']*self.mult)
#         self.addEFSignature(['EF_e_step1']*self.mult)
#         self.addEFSignature(['EF_e_step2']*self.mult)
#         self.addEFSignature(['EF_e_step3']*self.mult)
#         self.addEFSignature(['EF_e_step4']*self.mult)

#     def defineTErenaming(self):
#         self.TErenamingMap = {
#             'L2_e_step1': mergeRemovingOverlap('L2_', self.sig_id_noMult+'cl'),
#             'L2_e_step2': mergeRemovingOverlap('L2_', self.sig_id_noMult+'id'),
#             'L2_e_step3': mergeRemovingOverlap('L2_', self.sig_id_noMult),
#             'EF_e_step1': mergeRemovingOverlap('EF_', self.sig_id_noMult+'calo'),
#             'EF_e_step2': mergeRemovingOverlap('EF_', self.sig_id_noMult+'id1'),
#             'EF_e_step3': mergeRemovingOverlap('EF_', self.sig_id_noMult+'id2'),
#             'EF_e_step4': mergeRemovingOverlap('EF_', self.sig_id_noMult),
#             }





# class L2EFChain_e_L2Star(L2EFChain_e):
#     def defineSequences(self, config):

#         lastpart = self.getlastpart(self.sig_id)

#         if   lastpart == 'L2StarA':
#             myTrigL2SiTrackFinder_eGamma_1 = theTrigL2SiTrackFinder_eGammaA
#             myl2electronfex = L2ElectronFex_L2StarA()
#         elif lastpart == 'L2StarB':
#             myTrigL2SiTrackFinder_eGamma_1 = theTrigL2SiTrackFinder_eGammaB
#             myl2electronfex = L2ElectronFex_L2StarB()
#         elif lastpart == 'L2StarC':
#             myTrigL2SiTrackFinder_eGamma_1 = theTrigL2SiTrackFinder_eGammaC
#             myl2electronfex = L2ElectronFex_L2StarC()
#         else:
#             myTrigL2SiTrackFinder_eGamma_1 = theTrigL2SiTrackFinder_eGammaA
#             myl2electronfex = L2ElectronFex_L2StarA()


#         l2electronhypo = TrigL2ElectronHypo('L2ElectronHypo_'+self.sig_id)

#         ###### L2 ######
#         if ('etcut_L2Star' in self.sig_id):
#             self.addL2Sequence(self.inputTEs,[theT2CaloEgamma_eGamma], 'L2_e_step1')
#             self.addL2Sequence(['L2_e_step1'], [myTrigL2SiTrackFinder_eGamma_1], 'L2_e_step2')
#             self.addL2Sequence(['L2_e_step2'], [myl2electronfex],  'L2_e_step3')            
#         else:
#             self.addL2Sequence(self.inputTEs,
#                                [theT2CaloEgamma_eGamma, config.L2Config.TrigL2CaloHypo_1],
#                                'L2_e_step1')
#             self.addL2Sequence(['L2_e_step1'], [myTrigL2SiTrackFinder_eGamma_1], 'L2_e_step2')
#             self.addL2Sequence(['L2_e_step2'], [myl2electronfex, l2electronhypo], 'L2_e_step3')


#         ###### EF ######
#         self.addEFSequence('L2_e_step3', 
#                            [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
#                            'EF_e_step1')

#         if ('etcut_L2Star' in self.sig_id):
#             self.addEFSequence('EF_e_step1', theTrigEFIDInsideOut_Electron, 'EF_e_step2')
#             self.addEFSequence('EF_e_step2', [theTrigEgammaRec_eGamma], 'EF_e_step3')
#         else:
#             self.addEFSequence('EF_e_step1', 
#                                theTrigEFIDInsideOut_Electron + [config.EFConfig.TrigEFTrackHypo_1],
#                                'EF_e_step2')
#             self.addEFSequence('EF_e_step2', 
#                                [theTrigEgammaRec_eGamma, config.EFConfig.TrigEFEGammaHypo_1],
#                                'EF_e_step3')
        


# ############################################# Savannah #101482 1)
# class L2EFChain_e_noL2ID(L2EFChain_e): 
#     def defineSequences(self, config): 
#         self.addL2Sequence(self.inputTEs, 
#                            [theT2CaloEgamma_eGamma, config.L2Config.TrigL2CaloHypo_1], 
#                            'L2_e_step1')
#         self.addEFSequence('L2_e_step1', 
#                            [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw], 
#                            'EF_e_step1') 
#         self.addEFSequence('EF_e_step1', 
#                            theTrigEFIDInsideOut_Electron + [config.EFConfig.TrigEFTrackHypo_1], 
#                            'EF_e_step2') 
#         self.addEFSequence('EF_e_step2', 
#                            [theTrigEgammaRec_eGamma, config.EFConfig.TrigEFEGammaHypo_1], 
#                            'EF_e_step3') 
        
#     def defineSignatures(self):
#         self.addL2Signature(['L2_e_step1']*self.mult)
#         self.addEFSignature(['EF_e_step1']*self.mult)
#         self.addEFSignature(['EF_e_step2']*self.mult)
#         self.addEFSignature(['EF_e_step3']*self.mult)

#     def defineTErenaming(self):
#         self.TErenamingMap = {
#             'L2_e_step1': mergeRemovingOverlap('L2_', self.sig_id_noMult),
#             'EF_e_step1': mergeRemovingOverlap('EF_', self.sig_id_noMult+'calo'),
#             'EF_e_step2': mergeRemovingOverlap('EF_', self.sig_id_noMult+'id'),
#             'EF_e_step3': mergeRemovingOverlap('EF_', self.sig_id_noMult),
#             }


# ############################################# Savannah #101482 2)
# class L2EFChain_e_noL2IDnoEFID(L2EFChain_e): 
#     def defineSequences(self, config): 
#         # L2 ######
#         # Step 1: Calo Fex and Hypo 
#         self.addL2Sequence(self.inputTEs, 
#                            [theT2CaloEgamma_eGamma, config.L2Config.TrigL2CaloHypo_1], 
#                            'L2_e_step1')

#         # EF ###### 
#         # Step 1: Calo Fex 
#         self.addEFSequence('L2_e_step1', 
#                            [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw], 
#                            'EF_e_step1') 

#         # Step 3: ID-Calo Fex and Hypo 
#         self.addEFSequence('EF_e_step1', 
#                            [theTrigEgammaRec_eGamma, config.EFConfig.TrigEFEGammaHypo_1], 
#                            'EF_e_step2') 
        
#     def defineSignatures(self):
#         self.addL2Signature(['L2_e_step1']*self.mult)
#         self.addEFSignature(['EF_e_step1']*self.mult)
#         self.addEFSignature(['EF_e_step2']*self.mult)

#     def defineTErenaming(self):
#         self.TErenamingMap = {
#             'L2_e_step1': mergeRemovingOverlap('L2_', self.sig_id_noMult),
#             'EF_e_step1': mergeRemovingOverlap('EF_', self.sig_id_noMult+'calo'),
#             'EF_e_step2': mergeRemovingOverlap('EF_', self.sig_id_noMult),
#             }

# ############################################# Savannah #101482 3)
# class L2EFChain_e_noL2IDnoEF(L2EFChain_e): 

#     def defineSequences(self, config): 
#         self.addL2Sequence(self.inputTEs, 
#                            [theT2CaloEgamma_eGamma, config.L2Config.TrigL2CaloHypo_1], 
#                            'L2_e_step1')
             
#     def defineSignatures(self):
#         self.addL2Signature(['L2_e_step1']*self.mult)

#     def defineTErenaming(self):
#         self.TErenamingMap = {
#             'L2_e_step1': mergeRemovingOverlap('L2_', self.sig_id_noMult),
#             }

# ############################################# Savannah #101482 4)
# class L2EFChain_e_noL2noEFID(L2EFChain_e): 

#     def defineSequences(self, config):       
#         self.addEFSequence(self.inputTEs, 
#                            [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw], 
#                            'EF_e_step1') 
#         self.addEFSequence('EF_e_step1', 
#                            [theTrigEgammaRec_eGamma, config.EFConfig.TrigEFEGammaHypo_1], 
#                            'EF_e_step2') 
        
#     def defineSignatures(self):
#         self.addEFSignature(['EF_e_step1']*self.mult)
#         self.addEFSignature(['EF_e_step2']*self.mult)

#     def defineTErenaming(self):
#         self.TErenamingMap = {
#             'EF_e_step1': mergeRemovingOverlap('EF_', self.sig_id_noMult+'calo'),
#             'EF_e_step2': mergeRemovingOverlap('EF_', self.sig_id_noMult),
#             }

# ############################################# Savannah #101482 5)
# class L2EFChain_e_noL2(L2EFChain_e): 

#     def defineSequences(self, config): 
#         self.addEFSequence(self.inputTEs, 
#                            [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw], 
#                            'EF_e_step1') 
#         self.addEFSequence('EF_e_step1', 
#                            theTrigEFIDInsideOut_Electron + [config.EFConfig.TrigEFTrackHypo_1], 
#                            'EF_e_step2') 
#         self.addEFSequence('EF_e_step2', 
#                            [theTrigEgammaRec_eGamma, config.EFConfig.TrigEFEGammaHypo_1], 
#                            'EF_e_step3')
        
#     def defineSignatures(self):
#         self.addEFSignature(['EF_e_step1']*self.mult)
#         self.addEFSignature(['EF_e_step2']*self.mult)
#         self.addEFSignature(['EF_e_step3']*self.mult)

#     def defineTErenaming(self):
#         self.TErenamingMap = {
#             'EF_e_step1': mergeRemovingOverlap('EF_', self.sig_id_noMult+'calo'),
#             'EF_e_step2': mergeRemovingOverlap('EF_', self.sig_id_noMult+'id'),
#             'EF_e_step3': mergeRemovingOverlap('EF_', self.sig_id_noMult),
#             }



##########################################################################################
#
# Helper class for writing python code, eg HLT to L1 maps
#
##########################################################################################
class ElectronCodeHelper:
    def __init__(self):
        
        global Electrons

        iwidth = int(0)
        for echain in Electrons:
            if iwidth < len(echain.l2chain.sig_id):
                iwidth = len(echain.l2chain.sig_id) 
            
        for echain in Electrons:
            l2name = '\''+echain.l2chain.sig_id.rstrip()+'\''
            l1name = '\''+echain.l2chain.lower_chain_name.rstrip()+'\''
            lempty = ''
            for istep in range(len(echain.l2chain.sig_id), iwidth):
                lempty += ' '
            
        for echain in Electrons:
            if echain.l2chain.lower_chain_name not in l1map:
                l1map[echain.l2chain.lower_chain_name] = echain.l2chain.inputTEs


##########################################################################################
#
# Exception class
#
##########################################################################################
class EgammaMapError(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return repr(self.value)


##########################################################################################
#
# Get list of L1 thresholds (L2 input TEs) for L1 item
#
##########################################################################################
# def getEM_InTEFromItemConfig(item):
    
#     log = logging.getLogger( "EgammaSliceFlags" )
#     tpc = TriggerPythonConfig.currentTriggerConfig()

#     if item not in tpc.allItems.keys():
#         raise EgammaMapError('L1 item is not among all keys: '+item)

#     resItem = tpc.allItems[item]
#     resTEs  = resItem.thresholdNames()

#     log.info('Found TEs for item: '+item+str(resTEs))
#     return resTEs




###############################
## not in v4 menu
###############################

# class L2EFChain_e_NoIDTrkCut_IDT(L2EFChain_e):

#     def defineSequences(self, config):
#         ###### L2 ######
#         # Step 1: Calo Fex and Hypo
#         self.addL2Sequence(self.inputTEs,
#                            [theT2CaloEgamma_cells_e, config.L2Config.TrigL2CaloHypo_1],
#                            'L2_e_step1')
        
#         #remove IDScan and SiTrakck from IDTrkNoCut chains
#         self.addL2Sequence(['L2_e_step1'],
#                            [theTrigL2SiTrackFinder_eGammaA,
#                             theTrigL2SiTrackFinder_eGammaB,
#                             theTrigL2SiTrackFinder_eGammaC,
#                             theTrigL2SiTrackFinder_eGammaBC,
#                             theTrigTRTSegFinder_eGamma,
#                             ],
#                            'L2_e_step2')
        
#         # Step 3: ID-Calo Fex and Hypo
#         self.addL2Sequence(['L2_e_step2'],
#                            [config.L2Config.TrigL2ElectronFex_1, config.L2Config.TrigL2ElectronHypo_1],
#                            'L2_e_step3')
                
#         ###### EF ######
#         # Step 1: Calo Fex
#         self.addEFSequence('L2_e_step3', 
#                            [theTrigCaloCellMaker_eGamma_cells, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
#                            'EF_e_step1')

#         # Step 3: ID Fex and Hypo
#         self.addEFSequence('EF_e_step1', theTrigEFIDInsideOut_Electron, 'EF_e_step2')
#         self.addEFSequence('EF_e_step2', theTrigEFIDOutsideInTRTOnly_Electron, 'EF_e_step3')

#         # Step 4: IDMerged Fex
#         try:
#             from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_Electron
#             theTrigFastTrackFinder_Electron = TrigFastTrackFinder_Electron()
#         except:
#             theTrigFastTrackFinder_Electron = None
       
  
#         trkseq = theTrigEFIDInsideOut_Electron_MergedDP.getSequence()
#         trkseq.append(theTrigFastTrackFinder_Electron)
#         theTrigEFIDInsideOutMerged_Electron = TrigEFIDSequence("Electron","electron","InsideOutMerged")
#         trkseq += theTrigEFIDInsideOutMerged_Electron.getSequence(),
#         self.addEFSequence('EF_e_step3',
#                            trkseq,
#                            'EF_e_step4')
        
#         # Step 3: ID-Calo Fex and Hypo
#         self.addEFSequence('EF_e_step4', 
#                            [theTrigEgammaRec_eGamma, config.EFConfig.TrigEFEGammaHypo_1],
#                            'EF_e_step5')

#     def defineSignatures(self):
#         self.addL2Signature(['L2_e_step1'])
#         self.addL2Signature(['L2_e_step2'])
#         self.addL2Signature(['L2_e_step3'])
#         self.addEFSignature(['EF_e_step1'])
#         self.addEFSignature(['EF_e_step2'])
#         self.addEFSignature(['EF_e_step3'])
#         self.addEFSignature(['EF_e_step4'])
#         self.addEFSignature(['EF_e_step5'])

#     def defineTErenaming(self):
#         self.TErenamingMap = {
#             'L2_e_step1': mergeRemovingOverlap('L2_', self.sig_id+'cl'),
#             'L2_e_step2': mergeRemovingOverlap('L2_', self.sig_id+'id'),
#             'L2_e_step3': mergeRemovingOverlap('L2_', self.sig_id),
#             'EF_e_step1': mergeRemovingOverlap('EF_', self.sig_id+'calo'),
#             'EF_e_step2': mergeRemovingOverlap('EF_', self.sig_id+'id1'),
#             'EF_e_step3': mergeRemovingOverlap('EF_', self.sig_id+'id2'),
#             'EF_e_step4': mergeRemovingOverlap('EF_', self.sig_id+'idt'),
#             'EF_e_step5': mergeRemovingOverlap('EF_', self.sig_id),
#             }


# class L2EFChain_e_TRT_Ringer(L2EFChain_e):

#     def defineSequences(self, config):
#         self.addL2Sequence(self.inputTEs,
#                            [theT2CaloEgamma_Ringer, config.L2Config.TrigL2CaloHypo_1],
#                            'L2_e_step1')
#         self.addL2Sequence(['L2_e_step1'],
#                            [theTrigTRTSegFinder_eGamma],
#                            'L2_e_step2')
#         self.addL2Sequence(['L2_e_step2'],
#                            [config.L2Config.TrigL2ElectronFex_1, config.L2Config.TrigL2ElectronHypo_1],
#                            'L2_e_step3')
#         self.addEFSequence('L2_e_step3',
#                            [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw],
#                            'EF_e_step1')    
#         self.addEFSequence('EF_e_step1',    
#                            theTrigEFIDOutsideInTRTOnly_Electron + [config.EFConfig.TrigEFTrackHypo_1],
#                            'EF_e_step2')
#         self.addEFSequence('EF_e_step2',
#                            [theTrigEgammaRec_eGamma, config.EFConfig.TrigEFEGammaHypo_1],
#                            'EF_e_step3')
        
        
