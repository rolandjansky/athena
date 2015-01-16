# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Photon trigger slice signatures  """

__author__  = 'Moritz Backes & Catrin Bernius'
__version__=""
__doc__="Implementation of Photon Signature"

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
logPhotonDef = logging.getLogger("TriggerMenu.egamma.PhotonDef")


from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenu.menu.TriggerPythonConfig import *
from TriggerMenu.menu.HltConfig import *

##################
#
# L2 & EF Chain configuration
#
##################

# default class, no ID at EF level

from TrigEgammaRec.TrigEgammaRecConfig import TrigEgammaRec
theTrigEgammaRec_NoIDEF_eGamma         = TrigEgammaRec(name = "TrigEgammaRec_NoIDEF_eGamma",doTrackMatching = False,doTrackIsolation = False)
theTrigEgammaRec_Conv_eGamma         = TrigEgammaRec(name = "TrigEgammaRec_Conv_eGamma",doTrackMatching = False,doConversions = True)
#-----------------------------------
class L2EFChain_g(L2EFChainDef):
#-----------------------------------

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


                
        # gXX_ID type chains:
        #if True: #not self.chainPart['extra'] \
           #and self.chainPart['IDinfo'] \
           #and not self.chainPart['isoInfo'] \
           #and not self.chainPart['FSinfo'] \
           #and not self.chainPart['recoAlg'] \
           #and not self.chainPart['hypoInfo'] \
           #and not self.chainPart['trkInfo'] \
           #and not self.chainPart['reccalibInfo'] \
           #and not self.chainPart['isoInfo'] \
           #and not self.chainPart['addInfo']:
        if "hiptrt" in self.chainPart['addInfo']:
            self.setup_gnocut_hiptrt()
        elif self.chainPart['caloInfo']=='HLTCalo':
           self.setup_gXX_ID_HLTCalo()
        else:
            self.setup_gXX_ID()
        #else:
        #    logPhotonDef.error('Chain %s could not be assembled' % (self.chainPartName))
        #    return False
            
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

    def setup_gXX_ID(self):
        threshold = self.chainPart['threshold']
        IDinfo = self.chainPart['IDinfo']
        algoSuffix = "g%s_%s()" % (str(threshold),IDinfo)
       
        from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_eGamma
        theT2CaloEgamma_eGamma             = T2CaloEgamma_eGamma()
        
        # common imports required for EtCut and Electron ID chains
        # L2 Calo FEX
        from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_eGamma
        theT2CaloEgamma_eGamma            = T2CaloEgamma_eGamma()
        
        # Ringer FEX for L2 Calo
        from TrigT2CaloEgamma.TrigT2CaloEgammaConfig        import T2CaloEgamma_Ringer
        theT2CaloEgamma_Ringer  = T2CaloEgamma_Ringer()

        from TrigMultiVarHypo.TrigMultiVarHypoConfig        import TrigRingerNeuralHypoConfig
        from TrigMultiVarHypo.TrigMultiVarHypoConfig        import TrigRingerNeuralFexConfig

        theL2CaloFex_Ringer            = TrigRingerNeuralFexConfig("TrigRingerNeuralFex_g"+str(threshold)  )
        
        from TrigEgammaHypo.TrigL2PhotonFexConfig import L2PhotonFex_1
        theL2PhotonFex = L2PhotonFex_1()

        from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_eGamma, TrigCaloTowerMaker_eGamma, TrigCaloClusterMaker_slw
        theTrigCaloCellMaker_eGamma        = TrigCaloCellMaker_eGamma()
        theTrigCaloTowerMaker_eGamma       = TrigCaloTowerMaker_eGamma()
        theTrigCaloClusterMaker_slw        = TrigCaloClusterMaker_slw()

        from TrigEgammaHypo.TrigEFCaloCalibFexConfig import TrigEFCaloCalibFex_Photon
        theTrigEFCaloCalibFex = TrigEFCaloCalibFex_Photon()
    
        if 'etcut' in self.chainPart['addInfo']:
            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_g_nocut
            from TrigEgammaHypo.TrigEFPhotonHypoConfig import EFPhotonHypo_g_EtCut
            from TrigEgammaHypo.TrigL2PhotonHypoConfig import L2PhotonHypo_g_EtCut
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_EtCut
            theL2CaloHypo      = L2CaloHypo_g_nocut()
            theL2PhotonHypo  = L2PhotonHypo_g_EtCut("L2PhotonHypo_g"+str(threshold)+"_EtCut",threshold ) 
            theTrigEFCaloHypo = TrigEFCaloHypo_EtCut("TrigEFCaloHypo_g"+str(threshold)+"_EtCut",threshold);
            theEFPhotonHypo  = EFPhotonHypo_g_EtCut("TrigEFPhotonHypo_g"+str(threshold)+"_EtCut",threshold)
        elif 'perf' in self.chainPart['addInfo']:
            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_g_nocut
            from TrigEgammaHypo.TrigEFPhotonHypoConfig import EFPhotonHypo_g_NoCut
            from TrigEgammaHypo.TrigL2PhotonHypoConfig import L2PhotonHypo_g_NoCut
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_All
            theL2CaloHypo      = L2CaloHypo_g_nocut()
            theL2PhotonHypo  = L2PhotonHypo_g_NoCut("L2PhotonHypo_g"+str(threshold)+"_NoCut",threshold ) 
            theTrigEFCaloHypo = TrigEFCaloHypo_All("TrigEFCaloHypo_g"+str(threshold)+"_NoCut",threshold);
            theEFPhotonHypo  = EFPhotonHypo_g_NoCut("TrigEFPhotonHypo_g"+str(threshold)+"_NoCut",threshold)
        elif self.chainPart['IDinfo']:
            from TrigEgammaHypo.TrigEFPhotonHypoConfig import EFPhotonHypo_g_ID_CaloOnly
            from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_g7
            from TrigEgammaHypo.TrigL2PhotonHypoConfig import *
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_g_ID
            # L2 Calo
            if 'ringer' in self.chainPart['addInfo']:
                theL2CaloHypo           = TrigRingerNeuralHypoConfig("TrigRingerNeuralHypo_g"+str(threshold)  )
            else:
                theL2CaloHypo = L2CaloHypo_g7()
            #theL2PhotonHypo = eval("L2PhotonHypo_"+algoSuffix)
            theL2PhotonHypo  = L2PhotonHypo_g_ID("L2PhotonHypo_g"+str(threshold)+"_"+str(IDinfo),threshold,IDinfo)
            # EF Calo
            theTrigEFCaloHypo = TrigEFCaloHypo_g_ID("TrigEFCaloHypo_g"+str(threshold)+"_"+str(IDinfo),threshold,IDinfo);
            theEFPhotonHypo  = EFPhotonHypo_g_ID_CaloOnly("EFPhotonHypo_g"+str(threshold)+"_"+str(IDinfo),threshold,IDinfo)
        else:
            log.error('Chain %s could not be assembled' % (self.chainPartName))
            return False
            # these can be made more configurable later 
            #theL2CaloHypo   = L2CaloHypo_g7()
            #theL2PhotonHypo = eval("L2PhotonHypo_"+algoSuffix)
            #theEFPhotonHypo = eval("EFPhotonHypo_"+algoSuffix) 
        
        if 'conv' in self.chainPart['addInfo']:
            theTrigEgammaFex = theTrigEgammaRec_Conv_eGamma
        else :
            theTrigEgammaFex = theTrigEgammaRec_NoIDEF_eGamma

        ########### Sequences ###########
        
        if 'ringer' in self.chainPart['addInfo']:
            self.L2sequenceList += [[self.L2InputTE, 
                                     [theT2CaloEgamma_Ringer, theL2CaloFex_Ringer, theL2CaloHypo], 
                                     'L2_g_step1']]
        elif 'perf' in self.chainPart['addInfo']:
            self.L2sequenceList += [[self.L2InputTE, 
                                     [theT2CaloEgamma_Ringer, theL2CaloHypo], 
                                     'L2_g_step1']]
        else:
            self.L2sequenceList += [[self.L2InputTE, 
                                     [theT2CaloEgamma_eGamma, theL2CaloHypo], 
                                     'L2_g_step1']]
        
        self.L2sequenceList += [[['L2_g_step1'],    
                                 [theL2PhotonFex, theL2PhotonHypo], 
                                 'L2_g_step2']]
        
        self.EFsequenceList += [[['L2_g_step2'], 
                                 [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw], 
                                 'EF_g_step1']]
        
        self.EFsequenceList += [[['EF_g_step1'], 
                                 [theTrigEFCaloCalibFex,theTrigEFCaloHypo], 
                                 'EF_g_step2']]
        
        self.EFsequenceList += [[['EF_g_step2'], 
                                 [theTrigEgammaFex, theEFPhotonHypo],
                                 'EF_g_step3']]

        ########### Signatures ###########

        self.L2signatureList += [ [['L2_g_step1']*self.mult] ]
        self.L2signatureList += [ [['L2_g_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step3']*self.mult] ]

        ########### TE renaming ###########

        self.TErenamingDict = {
            'L2_g_step1': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'_calo'),
            'L2_g_step2': mergeRemovingOverlap('L2_', self.chainPartNameNoMult),
            'EF_g_step1': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_calo'),
            'EF_g_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_calocalib'),
            'EF_g_step3': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
            }

    def setup_gXX_ID_HLTCalo(self):
        threshold = self.chainPart['threshold']
        IDinfo = self.chainPart['IDinfo']
        algoSuffix = "g%s_%s()" % (str(threshold),IDinfo)
       
        from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_eGamma, TrigCaloTowerMaker_eGamma, TrigCaloClusterMaker_slw
        theTrigCaloCellMaker_eGamma        = TrigCaloCellMaker_eGamma()
        theTrigCaloTowerMaker_eGamma       = TrigCaloTowerMaker_eGamma()
        theTrigCaloClusterMaker_slw        = TrigCaloClusterMaker_slw()

        from TrigEgammaHypo.TrigEFCaloCalibFexConfig import TrigEFCaloCalibFex_Photon
        theTrigEFCaloCalibFex = TrigEFCaloCalibFex_Photon()
    
        if 'etcut' in self.chainPart['addInfo']:
            from TrigEgammaHypo.TrigEFPhotonHypoConfig import EFPhotonHypo_g_EtCut
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_EtCut
            theTrigEFCaloHypo = TrigEFCaloHypo_EtCut("TrigEFCaloHypo_g"+str(threshold)+"_EtCut",threshold);
            theEFPhotonHypo  = EFPhotonHypo_g_EtCut("TrigEFPhotonHypo_g"+str(threshold)+"_EtCut",threshold)
        elif 'perf' in self.chainPart['addInfo']:
            from TrigEgammaHypo.TrigEFPhotonHypoConfig import EFPhotonHypo_g_NoCut
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_All
            theTrigEFCaloHypo = TrigEFCaloHypo_All("TrigEFCaloHypo_g"+str(threshold)+"_NoCut",threshold);
            theEFPhotonHypo  = EFPhotonHypo_g_NoCut("TrigEFPhotonHypo_g"+str(threshold)+"_NoCut",threshold)
        elif self.chainPart['IDinfo']:
            from TrigEgammaHypo.TrigEFPhotonHypoConfig import EFPhotonHypo_g_ID_CaloOnly
            from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_g_ID
            # EF Calo
            theTrigEFCaloHypo = TrigEFCaloHypo_g_ID("TrigEFCaloHypo_g"+str(threshold)+"_"+str(IDinfo),threshold,IDinfo);
            theEFPhotonHypo  = EFPhotonHypo_g_ID_CaloOnly("EFPhotonHypo_g"+str(threshold)+"_"+str(IDinfo),threshold,IDinfo)
        else:
            log.error('Chain %s could not be assembled' % (self.chainPartName))
            return False
            # these can be made more configurable later 
            #theL2CaloHypo   = L2CaloHypo_g7()
            #theL2PhotonHypo = eval("L2PhotonHypo_"+algoSuffix)
            #theEFPhotonHypo = eval("EFPhotonHypo_"+algoSuffix) 
        
        if 'conv' in self.chainPart['addInfo']:
            theTrigEgammaFex = theTrigEgammaRec_Conv_eGamma
        else :
            theTrigEgammaFex = theTrigEgammaRec_NoIDEF_eGamma

        ########### Sequences ###########
        
        self.EFsequenceList += [[self.L2InputTE, 
                                 [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw], 
                                 'EF_g_step1']]
        
        self.EFsequenceList += [[['EF_g_step1'], 
                                 [theTrigEFCaloCalibFex,theTrigEFCaloHypo], 
                                 'EF_g_step2']]
        
        self.EFsequenceList += [[['EF_g_step2'], 
                                 [theTrigEgammaFex, theEFPhotonHypo],
                                 'EF_g_step3']]

        ########### Signatures ###########

        self.EFsignatureList += [ [['EF_g_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step3']*self.mult] ]

        ########### TE renaming ###########

        self.TErenamingDict = {
            'EF_g_step1': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_calo'),
            'EF_g_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_calocalib'),
            'EF_g_step3': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
            }

    def setup_gnocut_hiptrt(self):
#        log.error('In the hip trt function')
       
        from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_eGamma
        theT2CaloEgamma_eGamma             = T2CaloEgamma_eGamma()
        
        from TrigEgammaHypo.TrigL2PhotonFexConfig import L2PhotonFex_1
        theL2PhotonFex = L2PhotonFex_1()

        from TrigCaloRec.TrigCaloRecConfig import  TrigCaloCellMaker_eGamma, TrigCaloTowerMaker_eGamma, TrigCaloClusterMaker_slw
        theTrigCaloCellMaker_eGamma        = TrigCaloCellMaker_eGamma()
        theTrigCaloTowerMaker_eGamma       = TrigCaloTowerMaker_eGamma()
        theTrigCaloClusterMaker_slw        = TrigCaloClusterMaker_slw()

    
     #   if 'hiptrt' in self.chainPart['addInfo']:
        from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_g_nocut
        from TrigEgammaHypo.TrigEFPhotonHypoConfig import EFPhotonHypo_g_NoCut
        from TrigTRTHighTHitCounter.TrigTRTHighTHitCounterConf import TrigTRTHTHCounter,TrigTRTHTHhypo
        theL2CaloHypo      = L2CaloHypo_g_nocut()
        theEFPhotonHypo  = EFPhotonHypo_g_NoCut("TrigEFPhotonHypo_g_hiptrt_NoCut",0)
        theTrigTRTHTHCounter = TrigTRTHTHCounter()
        theTrigTRTHTHhypo = TrigTRTHTHhypo()
            


        ########### Sequences ###########
        
        self.L2sequenceList += [[self.L2InputTE, 
                                 [theT2CaloEgamma_eGamma, theL2CaloHypo], 
                                 'L2_g_step1']]

        self.L2sequenceList += [[['L2_g_step1'], 
                                  [theTrigTRTHTHCounter, theTrigTRTHTHhypo],
                                  'L2_g_step2']]

        self.EFsequenceList += [[['L2_g_step2'], 
                                 [theTrigCaloCellMaker_eGamma, theTrigCaloTowerMaker_eGamma, theTrigCaloClusterMaker_slw], 
                                 'EF_g_step1']]
        
        self.EFsequenceList += [[['EF_g_step1'], 
                                 [theTrigEgammaRec_NoIDEF_eGamma, theEFPhotonHypo],
                                 'EF_g_step2']]

        ########### Signatures ###########

        self.L2signatureList += [ [['L2_g_step1']*self.mult] ]
        self.L2signatureList += [ [['L2_g_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step2']*self.mult] ]

        ########### TE renaming ###########

        self.TErenamingDict = {
            'L2_g_step1': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'_calo'),
            'L2_g_step2': mergeRemovingOverlap('L2_', self.chainPartNameNoMult),
            'EF_g_step1': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_calo'),
            'EF_g_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
            }
