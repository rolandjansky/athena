# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Missing ET slice signatures """

__author__  = "E.Pianori, C.Bernius"
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

        #--------------------------------------
        #obtaining the muon sequences & signature:
        #--------------------------------------
        logMETDef.info("Creating muon sequence")
        
        chain = ['mu8', 'L1_MU6',  [], ["Main"], ['RATE:SingleMuon', 'BW:Muon'], -1]

        from TriggerMenu.menu import DictFromChainName
        theDictFromChainName = DictFromChainName.DictFromChainName()
        muonChainDict = theDictFromChainName.getChainDict(chain)
        from TriggerMenu.menu.MenuUtils import *
        listOfChainDicts = splitChainDict(muonChainDict)
        muDict = listOfChainDicts[0]
        from TriggerMenu.muon.MuonDef import L2EFChain_mu
        muDict['chainCounter'] = 9150
        muonthing = L2EFChain_mu(muDict, False, ['8GeV']) 
        muonSeed = muonthing.EFsignatureList[-1][0][0]

        for seq in muonthing.L2sequenceList:
             self.L2sequenceList += [seq]
        for seq in muonthing.EFsequenceList:
             self.EFsequenceList += [seq]
 

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


        mucorr=  '_wMu' if EFmuon else ''          
        ##MET with topo-cluster
        if EFrecoAlg=='tc' or EFrecoAlg=='pueta' or EFrecoAlg=='pufit' or EFrecoAlg=='mht':

            ##Topo-cluster
            if EFrecoAlg=='tc':
                #MET fex
                from TrigEFMissingET.TrigEFMissingETConfig import EFMissingET_Fex_topoClusters 
                theEFMETFex     = EFMissingET_Fex_topoClusters()                         
                #Muon correction fex
                from TrigMissingETMuon.TrigMissingETMuonConfig import EFTrigMissingETMuon_Fex_topocl
                theEFMETMuonFex = EFTrigMissingETMuon_Fex_topocl()        

                #TC hypo
                #mucorr=  '_wMu' if EFmuon else ''      
                from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypoTCXE           

                if self.chainPart['trigType'] == "xs":
                    from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypoXS_2sided
                    theEFMETHypo = EFMetHypoXS_2sided('EFMetHypo_xs_2sided_%i%s' % (threshold, mucorr),ef_thr=float(threshold)*0.1)
                elif  self.chainPart['trigType'] == "te":
                    from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypoTE
                    theEFMETHypo = EFMetHypoTE('EFMetHypo_te%d' % threshold,ef_thr=float(threshold)*GeV)
                else:               
                    theEFMETHypo = EFMetHypoTCXE('EFMetHypo_TC_xe%s_tc%s%s'%(threshold,calibration,mucorr),ef_thr=float(threshold)*GeV)  
                
            if EFrecoAlg=='pufit':
                #MET fex
                from TrigEFMissingET.TrigEFMissingETConfig import EFMissingET_Fex_topoClustersPUC
                theEFMETFex = EFMissingET_Fex_topoClustersPUC() 
                #Muon correction fex
                from TrigMissingETMuon.TrigMissingETMuonConfig import EFTrigMissingETMuon_Fex_topoclPUC
                theEFMETMuonFex = EFTrigMissingETMuon_Fex_topoclPUC()
                mucorr= '_wMu' if EFmuon else '' 
                from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypoTCPUCXE 
                theEFMETHypo = EFMetHypoTCPUCXE('EFMetHypo_TCPUC_xe%s_tc%s%s'%(threshold,calibration,mucorr),ef_thr=float(threshold)*GeV)

            ##MET based on trigger jets
            if EFrecoAlg=='mht':
                #MET fex
                from TrigEFMissingET.TrigEFMissingETConfig import EFMissingET_Fex_Jets
                theEFMETFex = EFMissingET_Fex_Jets()
                #Muon correction fex
                from TrigMissingETMuon.TrigMissingETMuonConfig import EFTrigMissingETMuon_Fex_Jets
                theEFMETMuonFex = EFTrigMissingETMuon_Fex_Jets()
                #mucorr= '_wMu' if EFmuon else ''
                from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypoJetsXE
                theEFMETHypo = EFMetHypoJetsXE('EFMetHypo_Jets_xe%s_tc%s%s'%(threshold,calibration,mucorr),ef_thr=float(threshold)*GeV)
                
        
            ##Topo-cluster with Pile-up suppression
            if EFrecoAlg=='pueta':
                #MET fex
                from TrigEFMissingET.TrigEFMissingETConfig import EFMissingET_Fex_topoClustersPS 
                theEFMETFex = EFMissingET_Fex_topoClustersPS() 
                #Muon correction fex
                from TrigMissingETMuon.TrigMissingETMuonConfig import EFTrigMissingETMuon_Fex_topoclPS
                theEFMETMuonFex = EFTrigMissingETMuon_Fex_topoclPS()        

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
            if self.chainPart['trigType'] == "xs":
                from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypoXS_2sided
                theEFMETHypo = EFMetHypoXS_2sided('EFMetHypo_xs_2sided_%d%s' % (threshold, mucorr),ef_thr=float(threshold)*0.1)                    
            elif  self.chainPart['trigType'] == "te":
                from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypoTE
                theEFMETHypo = EFMetHypoTE('EFMetHypo_te%d'% threshold,ef_thr=threshold*GeV)
            else:               
                from TrigMissingETHypo.TrigMissingETHypoConfig import EFMetHypoXE 
                theEFMETHypo = EFMetHypoXE('EFMetHypo_xe%s%s'%(threshold,mucorr),ef_thr=float(threshold)*GeV)  

        else:
            logMETDef.warning("MET EF algorithm not recognised")
        
        #----------------------------------------------------
        # Obtaining the needed jet TEs from the jet code
        #----------------------------------------------------
        chain = ['j0', '',  [], ["Main"], ['RATE:SingleJet', 'BW:Jet'], -1]
        
        from TriggerMenu.menu import DictFromChainName
        theDictFromChainName = DictFromChainName.DictFromChainName()
        jetChainDict = theDictFromChainName.getChainDict(chain)
        
        from TriggerMenu.jet.JetDef import generateHLTChainDef
        jetChainDict['chainCounter'] = 9151
        jetChainDef = generateHLTChainDef(jetChainDict)
            
        #for i in range(3):
        #    m_input[i] = jetChainDef.sequenceList[i]['input']
        #    m_output[i]= jetChainDef.sequenceList[i]['output']
        #    m_algo[i] =jetChainDef.sequenceList[i]['algorithm']

        #obtaining DummyUnseededAllTEAlgo/RoiCreator
        input0=jetChainDef.sequenceList[0]['input']
        output0 =jetChainDef.sequenceList[0]['output']
        algo0 =jetChainDef.sequenceList[0]['algorithm']

        #obtaining TrigCaloCellMaker/FS, TrigCaloClusterMaker, TrigHLTEnergyDensity
        input1=jetChainDef.sequenceList[1]['input']
        output1 =jetChainDef.sequenceList[1]['output']
        algo1 =jetChainDef.sequenceList[1]['algorithm']

        #obtaining TrigHLTJetRecFromCluster
        input2=jetChainDef.sequenceList[2]['input']
        output2 =jetChainDef.sequenceList[2]['output']
        algo2 =jetChainDef.sequenceList[2]['algorithm']


        #---End of obtaining jet TEs------------------------------
                   
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
        if EFrecoAlg=='tc' or EFrecoAlg=='pueta' or EFrecoAlg=='pufit':
            self.EFsequenceList +=[[ input0,algo0,  output0 ]]            
            self.EFsequenceList +=[[ input0,algo0,  output0 ]]            
            self.EFsequenceList +=[[ input1,algo1,  output1 ]]            
            self.EFsequenceList +=[[ [output1],          [theEFMETFex],  'EF_xe_step1' ]]            
            self.EFsequenceList +=[[ ['EF_xe_step1',muonSeed],     [theEFMETMuonFex, theEFMETHypo],  'EF_xe_step2' ]]
            
        #trigger-jet based MET
        elif EFrecoAlg=='mht': 
            self.EFsequenceList +=[[ input0,algo0,  output0 ]]            
            self.EFsequenceList +=[[ input1,algo1,  output1 ]]            
            self.EFsequenceList +=[[ input2,algo2,  output2 ]]            

            self.EFsequenceList +=[[ [output2], [theEFMETFex], 'EF_xe_step1' ]]
            self.EFsequenceList +=[[ ['EF_xe_step1',muonSeed], [theEFMETMuonFex, theEFMETHypo], 'EF_xe_step2' ]]

        #cell based MET
        elif EFrecoAlg=='cell':
            self.EFsequenceList +=[[ [''],          [theEFMETFex],  'EF_xe_step1' ]]  
            self.EFsequenceList +=[[ ['EF_xe_step1',muonSeed],     [theEFMETMuonFex, theEFMETHypo],  'EF_xe_step2' ]]
            
            
        ########### Signatures ###########
        if L2recoAlg=="l2fsperf" or  L2recoAlg=="L2FS" :
            self.L2signatureList += [ [['L2_xe_step1']] ]
            self.L2signatureList += [ [['L2_xe_step2']] ]
            self.L2signatureList += [ [['L2_xe_step3']] ]
            self.L2signatureList += [ [['L2_xe_step4']] ]


        self.EFsignatureList += [ [['EF_xe_step1']] ]
        self.EFsignatureList += [ [['EF_xe_step2']] ]


        ########### TE renaming ###########
        self.TErenamingDict = {}
            
        if L2recoAlg=="l2fsperf" or  L2recoAlg=="L2FS" :
            self.TErenamingDict['L2_xe_step1']= mergeRemovingOverlap('L2_', self.sig_id_noMult+'_step1')
            self.TErenamingDict['L2_xe_step2']= mergeRemovingOverlap('L2_', self.sig_id_noMult+'_step2')
            self.TErenamingDict['L2_xe_step3']= mergeRemovingOverlap('L2_', self.sig_id_noMult+'_step3')
            self.TErenamingDict['L2_xe_step4']= mergeRemovingOverlap('L2_', self.sig_id_noMult+'_step4')
            
        self.TErenamingDict['EF_xe_step1']= mergeRemovingOverlap('EF_', self.sig_id_noMult+'_step1')
        self.TErenamingDict['EF_xe_step2']= mergeRemovingOverlap('EF_', self.sig_id_noMult)
            
