# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Photon trigger slice signatures  """

__author__  = 'Moritz Backes & Catrin Bernius'
__version__=""
__doc__="Implementation of Photon Signature"

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenu.egamma.PhotonDef")

from TriggerJobOpts.TriggerFlags import TriggerFlags
from TriggerMenu.menu.TriggerPythonConfig import *
from TriggerMenu.menu.HltConfig import *
from TriggerMenu.egamma.EgammaCleanMonitoring import *
from TriggerMenu.menu.CleanMonitoring import *

from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
from TrigEgammaRec.TrigEgammaRecConfig import TrigEgammaRec

from TrigCaloRec.TrigCaloRecConfig import TrigCaloCellMaker_eGamma, TrigCaloTowerMaker_eGamma, TrigCaloClusterMaker_slw
from TrigCaloRec.TrigCaloRecConfig import TrigCaloCellMaker_jet_fullcalo, TrigCaloClusterMaker_topo
from TrigEgammaHypo.TrigEFCaloCalibFexConfig import TrigEFCaloCalibFex_Photon
from TrigEgammaHypo.TrigEFCaloHypoConfig import TrigEFCaloHypo_All, TrigEFCaloHypo_EtCut, TrigEFCaloHypo_g_ID
from TrigEgammaHypo.TrigEFPhotonHypoConfig import EFPhotonHypo_g_EtCut, EFPhotonHypo_g_ID_CaloOnly, EFPhotonHypo_g_NoCut
from TrigEgammaHypo.TrigL2CaloHypoConfig import L2CaloHypo_g7, L2CaloHypo_g_nocut
from TrigEgammaHypo.TrigL2PhotonFexConfig import L2PhotonFex_1
from TrigEgammaHypo.TrigL2PhotonHypoConfig import L2PhotonHypo_g_EtCut, L2PhotonHypo_g_ID, L2PhotonHypo_g_NoCut

from TrigGenericAlgs.TrigGenericAlgsConf import (PESA__DummyCombineAlgo,
                                                 PESA__DummyUnseededAllTEAlgo as DummyAlgo)

from TrigMultiVarHypo.TrigL2CaloRingerHypoConfig import (TrigL2CaloRingerFexHypo_g_EtCut,
                                                         TrigL2CaloRingerFexHypo_g_ID,
                                                         TrigL2CaloRingerFexHypo_g_NoCut)

from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_eGamma, T2CaloEgamma_Ringer


##################
#
# L2 & EF Chain configuration
#
##################

# default class, no ID at EF level

class L2EFChain_g(L2EFChainDef):
    
    # Define frequently used instances here as class variables
    theTrigEgammaRec_NoIDEF_eGamma = TrigEgammaRec.copy(name = "TrigEgammaRec_NoIDEF_eGamma",
                                                        doTrackMatching = False,doTrackIsolation = False,doPrint=False)()
    theTrigEgammaRec_Iso_eGamma    = TrigEgammaRec.copy(name = "TrigEgammaRec_Iso_eGamma",
                                                        doTrackMatching = False,doTrackIsolation = False,
                                                        doCaloTopoIsolation=True,doPrint=False)()
    theTrigEgammaRec_Conv_eGamma   = TrigEgammaRec.copy(name = "TrigEgammaRec_Conv_eGamma",
                                                        doTrackMatching = False,doConversions = True,doPrint=False)()

    theTrigCaloCellMaker_eGamma    = TrigCaloCellMaker_eGamma()
    theTrigCaloTowerMaker_eGamma   = TrigCaloTowerMaker_eGamma()
    theTrigCaloClusterMaker_slw    = TrigCaloClusterMaker_slw()
    theL2PhotonFex                 = L2PhotonFex_1()
    theT2CaloEgamma_eGamma         = T2CaloEgamma_eGamma()    
    theT2CaloEgamma_Ringer         = T2CaloEgamma_Ringer()
    theTrigEFCaloCalibFex          = TrigEFCaloCalibFex_Photon()
        
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
        elif 'ringer' in self.chainPart['addInfo']:
           self.setup_gXX_ID_ringer()
        elif 'iloose' in self.chainPart['isoInfo']: 
           self.setup_gXX_ID_iso()
        elif self.chainPart['caloInfo']=='HLTCalo':
           self.setup_gXX_ID_HLTCalo()
        else:
            if 'ion' in self.chainPart['extra']:
                self.setup_gXX_ID_heavyIon()
            else:
                self.setup_gXX_ID()
        #else:
        #    log.error('Chain %s could not be assembled' % (self.chainPartName))
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
       
        disableMon = not KeepMonitoring(self.chainName,EgammaChainsToKeepMonitoring)

        if 'etcut' in self.chainPart['addInfo']:
            theL2CaloHypo      = L2CaloHypo_g_nocut()
            theL2PhotonHypo  = L2PhotonHypo_g_EtCut("L2PhotonHypo_g"+str(threshold)+"_EtCut",threshold ) 
            theTrigEFCaloHypo = TrigEFCaloHypo_EtCut("TrigEFCaloHypo_g"+str(threshold)+"_EtCut",threshold);
            theEFPhotonHypo  = EFPhotonHypo_g_EtCut("TrigEFPhotonHypo_g"+str(threshold)+"_EtCut",threshold)
        elif 'perf' in self.chainPart['addInfo']:
            theL2CaloHypo      = L2CaloHypo_g_nocut()
            theL2PhotonHypo  = L2PhotonHypo_g_NoCut("L2PhotonHypo_g"+str(threshold)+"_NoCut",threshold ) 
            theTrigEFCaloHypo = TrigEFCaloHypo_All("TrigEFCaloHypo_g"+str(threshold)+"_NoCut",threshold);
            theEFPhotonHypo  = EFPhotonHypo_g_NoCut("TrigEFPhotonHypo_g"+str(threshold)+"_NoCut",threshold)
        elif self.chainPart['IDinfo']:
            # L2 Calo
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
            theTrigEgammaFex = self.theTrigEgammaRec_Conv_eGamma
        else :
            theTrigEgammaFex = self.theTrigEgammaRec_NoIDEF_eGamma


        ########### Sequences ###########
        if ( disableMon ) : theL2CaloHypo.AthenaMonTools=DisableMonitoringButValAndTime(theL2CaloHypo.AthenaMonTools)
        
        if 'perf' in self.chainPart['addInfo'] or 'etcut' in self.chainPart['addInfo']:
            self.L2sequenceList += [[self.L2InputTE, 
                                     [self.theT2CaloEgamma_Ringer, theL2CaloHypo], 
                                     'L2_g_step1']]
        else:
            self.L2sequenceList += [[self.L2InputTE, 
                                     [self.theT2CaloEgamma_eGamma, theL2CaloHypo], 
                                     'L2_g_step1']]
        
        self.L2sequenceList += [[['L2_g_step1'],    
                                 [self.theL2PhotonFex, theL2PhotonHypo], 
                                 'L2_g_step2']]
        
        self.EFsequenceList += [[['L2_g_step2'], 
                                 [self.theTrigCaloCellMaker_eGamma, self.theTrigCaloTowerMaker_eGamma, self.theTrigCaloClusterMaker_slw], 
                                 'EF_g_step1']]
        
        #if ( disableMon ) : self.theTrigEFCaloCalibFex.AthenaMonTools=DisableMonitoringButValAndTime(self.theTrigEFCaloCalibFex.AthenaMonTools)
        if ( disableMon ) : theTrigEFCaloHypo.AthenaMonTools=DisableMonitoringButValAndTime(theTrigEFCaloHypo.AthenaMonTools)

        self.EFsequenceList += [[['EF_g_step1'], 
                                 [self.theTrigEFCaloCalibFex,theTrigEFCaloHypo], 
                                 'EF_g_step2']]
        
        #if ( disableMon ) : theTrigEgammaFex.AthenaMonTools=DisableMonitoringButValAndTime(theTrigEgammaFex.AthenaMonTools)
        if ( disableMon ) : theEFPhotonHypo.AthenaMonTools=DisableMonitoringButValAndTime(theEFPhotonHypo.AthenaMonTools)
        
        self.EFsequenceList += [[['EF_g_step2'], 
                                 [theTrigEgammaFex, theEFPhotonHypo],
                                 'EF_g_step3']]

        if 'larpeb' in self.chainPart['addInfo']:
            from TrigDetCalib.TrigDetCalibConfig import LArEFROBListWriter
            self.EFsequenceList += [[['EF_g_step3'], 
                                     [ LArEFROBListWriter('LArEFROBListWriter_' + self.chainName, addCTPResult = True, addL2Result = True, addEFResult = True) ],
                                     'EF_g_step4']]
            
        ########### Signatures ###########

        self.L2signatureList += [ [['L2_g_step1']*self.mult] ]
        self.L2signatureList += [ [['L2_g_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step3']*self.mult] ]
        
        if 'larpeb' in self.chainPart['addInfo']:
            self.EFsignatureList += [ [['EF_g_step4']*self.mult] ]

        ########### TE renaming ###########

        self.TErenamingDict = {
            'L2_g_step1': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'_calo'),
            'L2_g_step2': mergeRemovingOverlap('L2_', self.chainPartNameNoMult),
            'EF_g_step1': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_calo'),
            'EF_g_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_calocalib'),
            'EF_g_step3': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
            }

        if 'larpeb' in self.chainPart['addInfo']:
            self.TErenamingDict ['EF_g_step4']=  mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_larpeb')


    def setup_gXX_ID_iso(self):
        threshold = self.chainPart['threshold']
        IDinfo = self.chainPart['IDinfo']
        algoSuffix = "g%s_%s()" % (str(threshold),IDinfo)
       
        disableMon = not KeepMonitoring(self.chainName,EgammaChainsToKeepMonitoring)

        if 'etcut' in self.chainPart['addInfo']:
            theL2CaloHypo      = L2CaloHypo_g_nocut()
            theL2PhotonHypo  = L2PhotonHypo_g_EtCut("L2PhotonHypo_g"+str(threshold)+"_EtCut",threshold ) 
            theTrigEFCaloHypo = TrigEFCaloHypo_EtCut("TrigEFCaloHypo_g"+str(threshold)+"_EtCut",threshold);
            theEFPhotonHypo  = EFPhotonHypo_g_EtCut("TrigEFPhotonHypo_g"+str(threshold)+"_EtCut",threshold)
        elif 'perf' in self.chainPart['addInfo']:
            theL2CaloHypo      = L2CaloHypo_g_nocut()
            theL2PhotonHypo  = L2PhotonHypo_g_NoCut("L2PhotonHypo_g"+str(threshold)+"_NoCut",threshold ) 
            theTrigEFCaloHypo = TrigEFCaloHypo_All("TrigEFCaloHypo_g"+str(threshold)+"_NoCut",threshold);
            theEFPhotonHypo  = EFPhotonHypo_g_NoCut("TrigEFPhotonHypo_g"+str(threshold)+"_NoCut",threshold)
        elif self.chainPart['IDinfo']:
            # L2 Calo
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
        
        theTrigEgammaFex = self.theTrigEgammaRec_Iso_eGamma

        cell_maker_fullcalo_topo = TrigCaloCellMaker_jet_fullcalo("CellMakerFullCalo_topo_egamma",doNoise=0, AbsE=True, doPers=True)
        fullClusterMaker = TrigCaloClusterMaker_topo('TrigCaloClusterMaker_topo_fullscan_egamma')
        DummyMergerAlgo = PESA__DummyCombineAlgo("DummyMergerAlgo")
        theDummyRoiCreator = DummyAlgo('RoiCreatorEl')

        from TriggerMenu.egamma.EgammaEDConfig import TrigHLTEnergyDensityCentral, TrigHLTEnergyDensityForward
        ED_Central = TrigHLTEnergyDensityCentral("TrigHLTEnergyDensityCentral");
        ED_Forward = TrigHLTEnergyDensityForward("TrigHLTEnergyDensityForward");


        ########### Sequences ###########
        if ( disableMon ) : theL2CaloHypo.AthenaMonTools=DisableMonitoringButValAndTime(theL2CaloHypo.AthenaMonTools)
        
        if 'perf' in self.chainPart['addInfo'] or 'etcut' in self.chainPart['addInfo']:
            self.L2sequenceList += [[self.L2InputTE, 
                                     [self.theT2CaloEgamma_Ringer, theL2CaloHypo], 
                                     'L2_g_step1']]
        else:
            self.L2sequenceList += [[self.L2InputTE, 
                                     [self.theT2CaloEgamma_eGamma, theL2CaloHypo], 
                                     'L2_g_step1']]
        
        self.L2sequenceList += [[['L2_g_step1'],    
                                 [self.theL2PhotonFex, theL2PhotonHypo], 
                                 'L2_g_step2']]
        
        self.EFsequenceList += [[['L2_g_step2'], 
                                 [self.theTrigCaloCellMaker_eGamma, self.theTrigCaloTowerMaker_eGamma, self.theTrigCaloClusterMaker_slw], 
                                 'EF_g_step1']]
        
        #if ( disableMon ) : self.theTrigEFCaloCalibFex.AthenaMonTools=DisableMonitoringButValAndTime(self.theTrigEFCaloCalibFex.AthenaMonTools)
        if ( disableMon ) : theTrigEFCaloHypo.AthenaMonTools=DisableMonitoringButValAndTime(theTrigEFCaloHypo.AthenaMonTools)

        self.EFsequenceList += [[['EF_g_step1'], 
                                 [self.theTrigEFCaloCalibFex,theTrigEFCaloHypo], 
                                 'EF_g_step2']]
        
        #if ( disableMon ) : theTrigEgammaFex.AthenaMonTools=DisableMonitoringButValAndTime(theTrigEgammaFex.AthenaMonTools)
        if ( disableMon ) : theEFPhotonHypo.AthenaMonTools=DisableMonitoringButValAndTime(theEFPhotonHypo.AthenaMonTools)
        
        self.EFsequenceList += [[ '',[theDummyRoiCreator],'EF_InputRoI']]
        self.EFsequenceList += [[ ['EF_InputRoI'], [cell_maker_fullcalo_topo, fullClusterMaker], 'EF_TopoClustersFromFullCalo' ]]
        self.EFsequenceList += [[ ['EF_TopoClustersFromFullCalo'], [ED_Central, ED_Forward], 'EF_TopoClustersFromFullCaloED' ]]

        self.EFsequenceList += [[ ['EF_g_step2','EF_TopoClustersFromFullCaloED'],
                                     [DummyMergerAlgo],
                                      'EF_gCache_step2']]

        self.EFsequenceList += [[['EF_gCache_step2'],
                                     [theTrigEgammaFex, theEFPhotonHypo],
                                 'EF_g_step3']]

        if 'larpeb' in self.chainPart['addInfo']:
            from TrigDetCalib.TrigDetCalibConfig import LArEFROBListWriter
            self.EFsequenceList += [[['EF_g_step3'], 
                                     [ LArEFROBListWriter('LArEFROBListWriter_' + self.chainName, addCTPResult = True, addL2Result = True, addEFResult = True) ],
                                     'EF_g_step4']]
            
        ########### Signatures ###########

        self.L2signatureList += [ [['L2_g_step1']*self.mult] ]
        self.L2signatureList += [ [['L2_g_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_gCache_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step3']*self.mult] ]
        
        if 'larpeb' in self.chainPart['addInfo']:
            self.EFsignatureList += [ [['EF_g_step4']*self.mult] ]

        ########### TE renaming ###########

        self.TErenamingDict = {
            'L2_g_step1': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'_calo'),
            'L2_g_step2': mergeRemovingOverlap('L2_', self.chainPartNameNoMult),
            'EF_g_step1': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_calo'),
            'EF_g_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_calocalib'),
            'EF_gCache_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'ED'),
            'EF_g_step3': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
            }

        if 'larpeb' in self.chainPart['addInfo']:
            self.TErenamingDict ['EF_g_step4']=  mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_larpeb')
    
    def setup_gXX_ID_heavyIon(self):
        threshold = self.chainPart['threshold']
        IDinfo = self.chainPart['IDinfo']
        algoSuffix = "g%s_%s()" % (str(threshold),IDinfo)
       
        from TrigHIHypo.UE import theUEMaker, theFSCellMaker, theTrigCaloCellMaker_eGamma as theTrigCaloCellMaker_eGammaHI

        #theTrigCaloCellMaker_eGamma      = TrigCaloCellMaker_eGamma("TrigCaloCellMaker_eGamma_heavyIon")
        theTrigCaloTowerMaker_eGammaHI    = TrigCaloTowerMaker_eGamma("TrigCaloTowerMaker_eGamma_heavyIon")
    
        if 'etcut' in self.chainPart['addInfo']:
            theL2CaloHypo      = L2CaloHypo_g_nocut()
            theL2PhotonHypo  = L2PhotonHypo_g_EtCut("L2PhotonHypo_g"+str(threshold)+"_EtCut",threshold ) 
            theTrigEFCaloHypo = TrigEFCaloHypo_EtCut("TrigEFCaloHypo_g"+str(threshold)+"_EtCut_heavyIon",threshold);
            theEFPhotonHypo  = EFPhotonHypo_g_EtCut("TrigEFPhotonHypo_g"+str(threshold)+"_EtCut_heavyIon",threshold)
        elif 'perf' in self.chainPart['addInfo']:
            theL2CaloHypo      = L2CaloHypo_g_nocut()
            theL2PhotonHypo  = L2PhotonHypo_g_NoCut("L2PhotonHypo_g"+str(threshold)+"_NoCut",threshold ) 
            theTrigEFCaloHypo = TrigEFCaloHypo_All("TrigEFCaloHypo_g"+str(threshold)+"_NoCut_heavyIon",threshold);
            theEFPhotonHypo  = EFPhotonHypo_g_NoCut("TrigEFPhotonHypo_g"+str(threshold)+"_NoCut_heavyIon",threshold)
        elif self.chainPart['IDinfo']:
            # L2 Calo
            theL2CaloHypo = L2CaloHypo_g_nocut()
            #theL2PhotonHypo = eval("L2PhotonHypo_"+algoSuffix)
            theL2PhotonHypo  = L2PhotonHypo_g_EtCut("L2PhotonHypo_g"+str(threshold)+"_EtCut",threshold )
            # EF Calo
            theTrigEFCaloHypo = TrigEFCaloHypo_EtCut("TrigEFCaloHypo_g"+str(threshold)+"_EtCut_heavyIon",threshold);
            theEFPhotonHypo  = EFPhotonHypo_g_ID_CaloOnly("EFPhotonHypo_g"+str(threshold)+"_"+str(IDinfo)+"_heavyIon",threshold,IDinfo)
        else:
            log.error('Chain %s could not be assembled' % (self.chainPartName))
            return False
            # these can be made more configurable later 
            #theL2CaloHypo   = L2CaloHypo_g7()
            #theL2PhotonHypo = eval("L2PhotonHypo_"+algoSuffix)
            #theEFPhotonHypo = eval("EFPhotonHypo_"+algoSuffix) 
        
        if 'conv' in self.chainPart['addInfo']:
            theTrigEgammaFex = self.theTrigEgammaRec_Conv_eGamma
        else :
            theTrigEgammaFex = self.theTrigEgammaRec_NoIDEF_eGamma

        ########### Sequences ###########
        
        if 'perf' in self.chainPart['addInfo'] or 'etcut' in self.chainPart['addInfo']:
            self.L2sequenceList += [[self.L2InputTE, 
                                     [self.theT2CaloEgamma_Ringer, theL2CaloHypo], 
                                     'L2_g_step1']]
        else:
            self.L2sequenceList += [[self.L2InputTE, 
                                     [self.theT2CaloEgamma_eGamma, theL2CaloHypo], 
                                     'L2_g_step1']]
        
        self.L2sequenceList += [[['L2_g_step1'],    
                                 [self.theL2PhotonFex, theL2PhotonHypo], 
                                 'L2_g_step2']]

        self.EFsequenceList += [[['L2_g_step2'], 
                                 [theFSCellMaker], 'EF_g_step1_fs']]

        self.EFsequenceList += [[['EF_g_step1_fs'], 
                                 [theUEMaker], 'EF_g_step1_ue']]

        self.EFsequenceList += [[['L2_g_step2'], 
                                 [theTrigCaloCellMaker_eGammaHI, theTrigCaloTowerMaker_eGammaHI, self.theTrigCaloClusterMaker_slw], 
                                 'EF_g_step1']]
        
        self.EFsequenceList += [[['EF_g_step1'], 
                                 [self.theTrigEFCaloCalibFex,theTrigEFCaloHypo], 
                                 'EF_g_step2']]
        
        self.EFsequenceList += [[['EF_g_step2'], 
                                 [theTrigEgammaFex, theEFPhotonHypo],
                                 'EF_g_step3']]

        ########### Signatures ###########

        self.L2signatureList += [ [['L2_g_step1']*self.mult] ]
        self.L2signatureList += [ [['L2_g_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step1_fs']] ]
        self.EFsignatureList += [ [['EF_g_step1_ue']] ]
        self.EFsignatureList += [ [['EF_g_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step3']*self.mult] ]

        ########### TE renaming ###########

        self.TErenamingDict = {
            'L2_g_step1': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'_calo'),
            'L2_g_step2': mergeRemovingOverlap('L2_', self.chainPartNameNoMult),
            'EF_g_step1_fs': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_fs'),
            'EF_g_step1_ue': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_ue'),
            'EF_g_step1': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_calo'),
            'EF_g_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_calocalib'),
            'EF_g_step3': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
            }


    def setup_gXX_ID_HLTCalo(self):
        threshold = self.chainPart['threshold']
        IDinfo = self.chainPart['IDinfo']
        algoSuffix = "g%s_%s()" % (str(threshold),IDinfo)
       
        if 'etcut' in self.chainPart['addInfo']:
            theTrigEFCaloHypo = TrigEFCaloHypo_EtCut("TrigEFCaloHypo_g"+str(threshold)+"_EtCut",threshold);
            theEFPhotonHypo  = EFPhotonHypo_g_EtCut("TrigEFPhotonHypo_g"+str(threshold)+"_EtCut",threshold)
        elif 'perf' in self.chainPart['addInfo']:
            theTrigEFCaloHypo = TrigEFCaloHypo_All("TrigEFCaloHypo_g"+str(threshold)+"_NoCut",threshold);
            theEFPhotonHypo  = EFPhotonHypo_g_NoCut("TrigEFPhotonHypo_g"+str(threshold)+"_NoCut",threshold)
        elif self.chainPart['IDinfo']:
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
            theTrigEgammaFex = self.theTrigEgammaRec_Conv_eGamma
        else :
            theTrigEgammaFex = self.theTrigEgammaRec_NoIDEF_eGamma

        ########### Sequences ###########
        
        self.EFsequenceList += [[self.L2InputTE, 
                                 [self.theTrigCaloCellMaker_eGamma, self.theTrigCaloTowerMaker_eGamma, self.theTrigCaloClusterMaker_slw], 
                                 'EF_g_step1']]
        
        self.EFsequenceList += [[['EF_g_step1'], 
                                 [self.theTrigEFCaloCalibFex,theTrigEFCaloHypo], 
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
        
        # EF Calo
        theTrigEFCaloHypo = TrigEFCaloHypo_All("TrigEFCaloHypo_g_hiptrt_NoCut",0);
    
        theL2CaloHypo      = L2CaloHypo_g_nocut()
        theEFPhotonHypo  = EFPhotonHypo_g_NoCut("TrigEFPhotonHypo_g_hiptrt_NoCut",0)
        [theTRTDataPrep] = TrigInDetSequence("Electron","electron","TRTdata").getSequence()

        from TrigTRTHighTHitCounter.TrigTRTHighTHitCounterConf import TrigTRTHTHCounter, TrigTRTHTHhypo
        theTrigTRTHTHCounter = TrigTRTHTHCounter()
        theTrigTRTHTHhypo = TrigTRTHTHhypo()
            
        if 'conv' in self.chainPart['addInfo']:
            theTrigEgammaFex = self.theTrigEgammaRec_Conv_eGamma
        else :
            theTrigEgammaFex = self.theTrigEgammaRec_NoIDEF_eGamma


        ########### Sequences ###########
      
        self.L2sequenceList += [[self.L2InputTE, 
                                  theTRTDataPrep+[theTrigTRTHTHCounter, theTrigTRTHTHhypo],
                                  'L2_g_step1']]

        self.EFsequenceList += [[['L2_g_step1'], 
                                 [self.theTrigCaloCellMaker_eGamma, self.theTrigCaloTowerMaker_eGamma, self.theTrigCaloClusterMaker_slw], 
                                 'EF_g_step1']]
        
        self.EFsequenceList += [[['EF_g_step1'], 
#                                 [theTrigEgammaRec_NoIDEF_eGamma, theEFPhotonHypo],
                                 [self.theTrigEFCaloCalibFex,theTrigEFCaloHypo], 
                                 'EF_g_step2']]

        self.EFsequenceList += [[['EF_g_step2'],
                                 [theTrigEgammaFex, theEFPhotonHypo],
                                 'EF_g_step3']]


        ########### Signatures ###########

        self.L2signatureList += [ [['L2_g_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step3']*self.mult] ]
        ########### TE renaming ###########

        self.TErenamingDict = {
            'L2_g_step1': mergeRemovingOverlap('L2_', self.chainPartNameNoMult),
            'EF_g_step1': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_calo'),
#            'EF_g_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
            'EF_g_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_calocalib'),
            'EF_g_step3': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
            }


    def setup_gXX_ID_ringer(self):

        threshold = self.chainPart['threshold']
        IDinfo = self.chainPart['IDinfo']
        algoSuffix = "g%s_%s()" % (str(threshold),IDinfo)
       
        if 'etcut' in self.chainPart['addInfo']:
            [theL2CaloRingerFex,theL2CaloRingerHypo]  = TrigL2CaloRingerFexHypo_g_EtCut( threshold  )
            theEFPhotonHypo      = EFPhotonHypo_g_EtCut("TrigEFPhotonHypo_g"+str(threshold)+"_EtCut",threshold)

        elif 'perf' in self.chainPart['addInfo']:
            [theL2CaloRingerFex,theL2CaloRingerHypo]   = TrigL2CaloRingerFexHypo_g_NoCut( threshold  )
            theEFPhotonHypo       = EFPhotonHypo_g_NoCut("TrigEFPhotonHypo_g"+str(threshold)+"_NoCut",threshold)
 
        elif self.chainPart['IDinfo']:
            [theL2CaloRingerFex,theL2CaloRingerHypo]= TrigL2CaloRingerFexHypo_g_ID( threshold, IDinfo, self.chainName  )

            # EF Calo
            theEFPhotonHypo  = EFPhotonHypo_g_ID_CaloOnly("EFPhotonHypo_g"+str(threshold)+"_"+str(IDinfo),threshold,IDinfo)
        else:
            log.error('Chain %s could not be assembled' % (self.chainPartName))
            return False
        
        if 'conv' in self.chainPart['addInfo']:
            theTrigEgammaFex = self.theTrigEgammaRec_Conv_eGamma
        else :
            theTrigEgammaFex = self.theTrigEgammaRec_NoIDEF_eGamma

        ########### Sequences ###########
       
        self.L2sequenceList += [[self.L2InputTE, 
                                [self.theT2CaloEgamma_Ringer,theL2CaloRingerFex,theL2CaloRingerHypo], 
                                'L2_g_step1']] 

        self.EFsequenceList += [[['L2_g_step1'], 
                                 [self.theTrigCaloCellMaker_eGamma, self.theTrigCaloTowerMaker_eGamma, self.theTrigCaloClusterMaker_slw], 
                                 'EF_g_step1']]
        
        self.EFsequenceList += [[['EF_g_step1'], 
                                 [self.theTrigEFCaloCalibFex], 
                                 'EF_g_step2']]
        
        self.EFsequenceList += [[['EF_g_step2'], 
                                 [theTrigEgammaFex, theEFPhotonHypo],
                                 'EF_g_step3']]

        ########### Signatures ###########
        self.L2signatureList += [ [['L2_g_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step1']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step2']*self.mult] ]
        self.EFsignatureList += [ [['EF_g_step3']*self.mult] ]

        ########### TE renaming ###########

        self.TErenamingDict = {
            'L2_g_step1': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'_calo'),
            'EF_g_step1': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_calo'),
            'EF_g_step2': mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_calocalib'),
            'EF_g_step3': mergeRemovingOverlap('EF_', self.chainPartNameNoMult),
            }



