# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Missing ET slice signatures """

__author__  = "E.Pianori"
__version__ = "" 
__doc__="Implementation of Missing ET slice in new TM framework "
##########################################################

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
logMETDef = logging.getLogger("TriggerMenu.met.MissingETDef")

from TriggerMenu.menu.HltConfig import L2EFChainDef, mergeRemovingOverlap
from AthenaCommon.SystemOfUnits import GeV

#############################################################################
class L2EFChain_met(L2EFChainDef):
     
    def __init__(self, chainDict):
        self.L2sequenceList   = []
        self.EFsequenceList   = []
        self.L2signatureList  = []
        self.EFsignatureList  = []
        self.TErenamingDict   = []
    
        l1_item_name = chainDict['L1item']        
        self.l2_input_tes = ''    ##MET is un-seeded
        chain_counter = chainDict['chainCounter']
        self.chainPart = chainDict['chainParts']
        l2_name = 'L2_'+self.chainPart['chainPartName']
        ef_name = 'EF_'+self.chainPart['chainPartName']
        self.mult = int(self.chainPart['multiplicity'])
        chainName = chainDict['chainName']
        sig_id = self.chainPart['chainPartName']
        self.sig_id_noMult = sig_id[1:] if self.mult > 1 else sig_id
               
         
        self.setup_xeXX()
            
        L2EFChainDef.__init__(self, chainName, l2_name, chain_counter, l1_item_name, ef_name, chain_counter, self.l2_input_tes)
        
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
    def setup_xeXX(self):

        ##EF only chain, run FEB or topo_cluster
        ##NoAlg at L2
        ##if at a certain point different steps are used at EF, 
        ## we need a way to encode the information of the threshold at both the two steps
        ##could be a dict here, or adding something to the SignatureDictionary
        
        threshold   = int(self.chainPart['threshold'])
        calibration = self.chainPart['calib']
        L2recoAlg   = self.chainPart['L2recoAlg']
        EFrecoAlg   = self.chainPart['EFrecoAlg']        
        L2muon      = self.chainPart['L2muonCorr']
        EFmuon      = self.chainPart['EFmuonCorr']

        #muonSeed="EF_SuperEF_mu8"
        muonSeed="EF_mu_step2"
   

        logMETDef.info("Creating muon sequence")
        
        ## --- HARD-CODED MUON CHAIN ---
        muThreshold=8
        L2AlgName = 'Muon'
        muFastThresh = '6GeV_v11a'
        muCombThresh = str(muThreshold)+"GeV"
        #id_alg_output = "STRATEGY_A"
        id_alg_output = "TrigFastTrackFinder_Muon"
        EFRecoAlgName = "Muon"
        
        from TrigL2MuonSA.TrigL2MuonSAConfig import TrigL2MuonSAConfig
        theL2StandAloneAlg  = TrigL2MuonSAConfig(L2AlgName)
        from TrigMuonHypo.TrigMuonHypoConfig import MufastHypoConfig
        theL2StandAloneHypo = MufastHypoConfig(L2AlgName, muFastThresh)

        from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_Muon
        theTrigFastTrackFinder_Muon = TrigFastTrackFinder_Muon()
        from InDetTrigRecExample.EFInDetConfig import TrigEFIDSequence
        theTrigEFIDDataPrep_Muon = TrigEFIDSequence("Muon","muon","DataPrep").getSequence()

        #from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_MuonA
        #theTrigL2SiTrackFinder_MuonA = TrigL2SiTrackFinder_MuonA()
        #theL2IDAlg = theTrigL2SiTrackFinder_MuonA 

        from TrigmuComb.TrigmuCombConfig import TrigmuCombConfig
        theL2CombinedAlg  = TrigmuCombConfig(L2AlgName, id_alg_output)
        from TrigMuonHypo.TrigMuonHypoConfig import MucombHypoConfig
        theL2CombinedHypo = MucombHypoConfig(L2AlgName, muCombThresh)

        from InDetTrigRecExample.EFInDetConfig import TrigEFIDSequence
        theTrigEFIDInsideOut_Muon = TrigEFIDSequence("Muon","muon","InsideOutMerged")
        #theTrigEFIDInsideOut_Muon = TrigEFIDSequence("Muon","muon") ##Run1 tracking

        from AthenaCommon import CfgGetter
        theTrigMuSuperEF = CfgGetter.getAlgorithm("TrigMuSuperEF")
        theEFAlg = theTrigMuSuperEF 
        from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFCombinerHypoConfig
        theTrigMuonEFCombinerHypoConfig = TrigMuonEFCombinerHypoConfig(EFRecoAlgName,str(muThreshold)+"GeV")


        L2_mu_step1_TE = 'muonstandalone_'+L2AlgName+'_muFast'+muFastThresh
        L2muonInputTE = 'MU6'
        self.L2sequenceList += [[L2muonInputTE,
                                 [theL2StandAloneAlg , theL2StandAloneHypo],
                                 'L2_mu_step1']]
        self.L2sequenceList += [[['L2_mu_step1'],
                                 theTrigEFIDDataPrep_Muon+
                                 [theTrigFastTrackFinder_Muon,
                                  theL2CombinedAlg,
                                  theL2CombinedHypo],
                                 'L2_mu_step2']]
              
        self.EFsequenceList += [[['L2_mu_step2'],
                                 theTrigEFIDInsideOut_Muon.getSequence(),
                                 'EF_mu_step1']]

        self.EFsequenceList += [[['EF_mu_step1'],
                                 [ theEFAlg, theTrigMuonEFCombinerHypoConfig],
                                 'EF_mu_step2']]


        ########### Imports for hypos and fexes  ###########
        ##L1 MET 
        from TrigL2MissingET.TrigL2MissingETConfig import L2MissingET_Fex
        theL2Fex     = L2MissingET_Fex()
        from TrigMissingETMuon.TrigMissingETMuonConfig import L2TrigMissingETMuon_Fex
        theL2MuonFex = L2TrigMissingETMuon_Fex()
        
        ##FEB MET at L2
        from TrigL2MissingET.TrigL2MissingETConfig import L2CaloMissingET_Fex_ReadL2L1
        theL2FEBL1Check  =  L2CaloMissingET_Fex_ReadL2L1()
        
        from TrigMissingETMuon.TrigMissingETMuonConfig import L2CaloTrigMissingETMuon_Fex
        theL2FEBMuonFex  =  L2CaloTrigMissingETMuon_Fex() 

        mucorr=  '_wMu' if L2muon else '' 
        if L2recoAlg=="L2FS":
            from TrigMissingETHypo.TrigMissingETHypoConfig import L2MetHypoFEBXE
            theL2MuonHypo  =  L2MetHypoFEBXE(name='L2MetHypo_xe%d%s_FEB'%(threshold,mucorr),l2_thr=threshold*GeV)
        else:            
            from TrigMissingETHypo.TrigMissingETHypoConfig import L2MetHypoXE
            theL2MuonHypo  =  L2MetHypoXE('L2MetHypo_xe_noL2%s' %mucorr,l2_thr=threshold*GeV)
        
        ##MET with topo-cluster
        if EFrecoAlg=='tc' or EFrecoAlg=='pueta':

            ##Topo-cluster
            if EFrecoAlg=='tc':
                #MET fex
                from TrigEFMissingET.TrigEFMissingETConfig import EFMissingET_Fex_topoClusters 
                theEFMETFex     = EFMissingET_Fex_topoClusters()                         
                #Muon correction fex
                from TrigMissingETMuon.TrigMissingETMuonConfig import EFTrigMissingETMuon_Fex_topocl
                theEFMETMuonFex = EFTrigMissingETMuon_Fex_topocl()        

                #TC hypo
                mucorr=  '_wMu' if EFmuon else ''      
                from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypoTCXE           
                theEFMETHypo = EFMetHypoTCXE('EFMetHypo_TC_xe%s_tc%s%s'%(threshold,calibration,mucorr),ef_thr=float(threshold)*GeV)  

        
            ##Topo-cluster with Pile-up suppression
            if EFrecoAlg=='pueta':
                #MET fex
                from TrigEFMissingET.TrigEFMissingETConfig import EFMissingET_Fex_topoClustersPS 
                theEFMETFex = EFMissingET_Fex_topoClustersPS() 
                #Muon correction fex
                from TrigMissingETMuon.TrigMissingETMuonConfig import EFTrigMissingETMuon_Fex_topoclPS
                theEFMETMuonFex = EFTrigMissingETMuon_Fex_topoclPS()        

                mucorr=  '_wMu' if EFmuon else ''      
                from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypoTCPSXE                          
                theEFMETHypo = EFMetHypoTCPSXE('EFMetHypo_TCPS_xe%s_tc%s%s'%(threshold,calibration,mucorr),ef_thr=float(threshold)*GeV)

        ##2-SidedNoise Cell
        elif EFrecoAlg=='cell':
            #MET fex
            from TrigEFMissingET.TrigEFMissingETConfig import EFMissingET_Fex_2sidednoiseSupp
            theEFMETFex = EFMissingET_Fex_2sidednoiseSupp()

            #Muon correction fex
            from TrigMissingETMuon.TrigMissingETMuonConfig import EFTrigMissingETMuon_Fex
            theEFMETMuonFex = EFTrigMissingETMuon_Fex()        
            
            #Hypo
            from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypoXE 
            theEFMETHypo = EFMetHypoXE('EFMetHypo_xe%s%s'%(threshold,mucorr),ef_thr=float(threshold)*GeV)  
        else:
            logMETDef.warning("MET EF algorithm not recognised")
        
        from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo as DummyAlgo
        roi_topo = DummyAlgo('RoiCreator')            
        from TrigCaloRec.TrigCaloRecConfig import TrigCaloCellMaker_jet_fullcalo
        cell_maker_fullcalo_topo = TrigCaloCellMaker_jet_fullcalo("CellMakerFullCalo_topo",doNoise=0, AbsE=True, doPers=True) 
        from TrigCaloRec.TrigCaloRecConfig import TrigCaloClusterMaker_topo
        topocluster_maker_fullcalo = TrigCaloClusterMaker_topo('TrigCaloClusterMaker_topo_fullscan',doMoments=True,doLC=True) 

    
                   
        ########### Sequences ###########
        
        #Run L2-like algos only for l2fsperf and L2FS chains
        if L2recoAlg=="l2fsperf" or L2recoAlg=="L2FS":

            ##L1 MET
            self.L2sequenceList += [[ self.l2_input_tes,              [theL2Fex],                      'L2_xe_step1']]
            ##Moun Correction to L1 MET
            self.L2sequenceList += [[ ['L2_xe_step1', muonSeed],  [theL2MuonFex],                  'L2_xe_step2']]
            ##FEB Met
            self.L2sequenceList += [[ 'L2_xe_step2',                 [theL2FEBL1Check],               'L2_xe_step3']]

            if L2recoAlg=="l2fsperf":
                #Only execute Muon FEB MET and muon correction
                self.L2sequenceList += [[ ['L2_xe_step3',muonSeed],   [theL2FEBMuonFex], 'L2_xe_step4']]            
            if L2recoAlg=="L2FS":
                #Hypo on FEB MET
                self.L2sequenceList += [[ ['L2_xe_step3',muonSeed],   [theL2FEBMuonFex,theL2MuonHypo], 'L2_xe_step4']]

        
        # --- EF ---                
        #topocluster
        if EFrecoAlg=='tc' or EFrecoAlg=='pueta':            
            self.EFsequenceList +=[[ '',[roi_topo],'EF_full']]
            self.EFsequenceList +=[[ 'EF_full',[cell_maker_fullcalo_topo, topocluster_maker_fullcalo],'EF_full_cluster']]            
            self.EFsequenceList +=[[ ['EF_full_cluster'],          [theEFMETFex],  'EF_xe_step1' ]]            
            self.EFsequenceList +=[[ ['EF_xe_step1',muonSeed],     [theEFMETMuonFex, theEFMETHypo],  'EF_xe_step2' ]]

        #cell based MET
        elif EFrecoAlg=='cell':
            self.EFsequenceList +=[[ [''],          [theEFMETFex],  'EF_xe_step1' ]]  
            self.EFsequenceList +=[[ ['EF_xe_step1',muonSeed],     [theEFMETMuonFex, theEFMETHypo],  'EF_xe_step2' ]]
            
            
        ########### Signatures ###########
        #if L2muonSeed or EFmuonSeed:        
        #    self.L2signatureList += [ [['L2_mu_step1']*self.mult] ]
        #    self.L2signatureList += [ [['L2_mu_step2']*self.mult] ]            
        #    self.EFsignatureList += [ [['EF_mu_step1']*self.mult] ]
        #    self.EFsignatureList += [ [['EF_mu_step2']*self.mult] ]

        if L2recoAlg=="l2fsperf" or  L2recoAlg=="L2FS" :
            self.L2signatureList += [ [['L2_xe_step1']] ]
            self.L2signatureList += [ [['L2_xe_step2']] ]
            self.L2signatureList += [ [['L2_xe_step3']] ]
            self.L2signatureList += [ [['L2_xe_step4']] ]

        #if EFrecoAlg=='tc': 
            #self.EFsignatureList += [ [['EF_FSTopoClusters']] ]
            #self.EFsignatureList += [ [['EF_topocluster_step1']] ]
            #self.EFsignatureList += [ [['EF_topocluster_step2']] ]

        self.EFsignatureList += [ [['EF_xe_step1']] ]
        self.EFsignatureList += [ [['EF_xe_step2']] ]


        ########### TE renaming ###########

        self.TErenamingDict = {}
        if L2muon or EFmuon:        
            muchain = 'mu8'      
            self.TErenamingDict['L2_mu_step1']= mergeRemovingOverlap('L2_mu_SA_',L2AlgName+muFastThresh+'_'+L2muonInputTE)
            self.TErenamingDict['L2_mu_step2']= mergeRemovingOverlap('L2_mucomb_',muchain +'_'+L2muonInputTE) 
            self.TErenamingDict['EF_mu_step1']= mergeRemovingOverlap('EF_EFIDInsideOut_' ,muchain)
            self.TErenamingDict['EF_mu_step2']= mergeRemovingOverlap('EF_SuperEF_', muchain)
            
            #self.TErenamingDict['L2_mu_step1']= mergeRemovingOverlap('L2_', "muon_standalone_"+ muchain)
            #self.TErenamingDict['L2_mu_step2']= mergeRemovingOverlap('L2_', muchain)
            #self.TErenamingDict['EF_mu_step1']= mergeRemovingOverlap('EFID_', muchain)
            #self.TErenamingDict['EF_mu_step2']= mergeRemovingOverlap('EF_', muchain)
            
        if L2recoAlg=="l2fsperf" or  L2recoAlg=="L2FS" :
            self.TErenamingDict['L2_xe_step1']= mergeRemovingOverlap('L2_', self.sig_id_noMult+'_step1')
            self.TErenamingDict['L2_xe_step2']= mergeRemovingOverlap('L2_', self.sig_id_noMult+'_step2')
            self.TErenamingDict['L2_xe_step3']= mergeRemovingOverlap('L2_', self.sig_id_noMult+'_step3')
            self.TErenamingDict['L2_xe_step4']= mergeRemovingOverlap('L2_', self.sig_id_noMult+'_step4')
            
        self.TErenamingDict['EF_xe_step1']= mergeRemovingOverlap('EF_', self.sig_id_noMult+'_step1')
        self.TErenamingDict['EF_xe_step2']= mergeRemovingOverlap('EF_', self.sig_id_noMult)
            
