# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

########################################################################
#
# SliceDef file for muon chains/signatures
#
#########################################################################
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)

logMuonDef = logging.getLogger("TriggerMenu.muon.MuonDef")

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
import re
from TriggerMenu.menu.HltConfig import *


#-----------------------------------
class L2EFChain_mu(L2EFChainDef):
#-----------------------------------
  
  def __init__(self, chainDict, asymDiMuonChain = False, AllMuons = []):
    self.L2sequenceList   = []
    self.EFsequenceList   = []
    self.L2signatureList  = []
    self.EFsignatureList  = []
    self.TErenamingDict   = []

    self.chainName = chainDict['chainName']
    self.chainL1Item = chainDict['L1item']
    self.chainCounter = chainDict['chainCounter']

    self.chainPart = chainDict['chainParts']
    self.chainPartL1Item = self.chainPart['L1item']
    self.L2Name = 'L2_'+self.chainPart['chainPartName']
    self.EFName = 'EF_'+self.chainPart['chainPartName']
    self.mult = int(self.chainPart['multiplicity'])

    self.chainPartName = self.chainPart['chainPartName']
    self.chainPartNameNoMult = self.chainPartName[1:] if self.mult > 1 else self.chainPartName

    self.L2InputTE = self.chainPartL1Item or self.chainL1Item
    if self.L2InputTE:      # cut of L1_, _EMPTY,..., & multiplicity
      self.L2InputTE = self.L2InputTE.replace("L1_","")
      self.L2InputTE = self.L2InputTE.split("_")[0]
      self.L2InputTE = self.L2InputTE[1:] if self.L2InputTE[0].isdigit() else self.L2InputTE
    
    # --- used to configure hypos for FS muon chains
    self.allMuThrs=AllMuons

    # --- when to run with ovlp removal ---
    self.ovlpRm = self.chainPart['overlapRemoval']
    self.asymDiMuonChain = asymDiMuonChain

    self.doOvlpRm = False
    if (self.mult > 1) & ('wOvlpRm' in self.ovlpRm):
      self.doOvlpRm = True
    elif (self.asymDiMuonChain) and (self.mult > 1) and not self.chainPart['extra'] and not self.chainPart['reccalibInfo'] :
      self.doOvlpRm = True
    else: self.doOvlpRm = False
 
      
    # --- call corresponding functions to set up chains ---
    if not self.chainPart['extra'] \
          and not self.chainPart['FSinfo'] \
          and not self.chainPart['hypoInfo'] \
          and not self.chainPart['reccalibInfo'] \
          and "cosmicEF" not in self.chainPart['addInfo']:
      self.setup_muXX_ID()
    elif "JpsimumuL2" in self.chainPart['FSinfo']:
      self.setup_muXX_JPsiL2()
    elif "nscan" in self.chainPart['FSinfo']:
      self.setup_muXX_NS()
    elif self.chainPart['extra'] or "JpsimumuFS" in self.chainPart['FSinfo']:
      self.setup_muXX_noL1()
    elif self.chainPart['reccalibInfo'] == "idperf":
      self.setup_muXX_idperf()
    elif self.chainPart['reccalibInfo'] == "msonly"  and "cosmicEF" not in self.chainPart['addInfo'] :
      self.setup_muXX_MSOnly()
    elif self.chainPart['reccalibInfo'] == "muoncalib"  and "cosmicEF" not in self.chainPart['addInfo'] :
      self.setup_muXX_muoncalib()
    elif "cosmicEF" in self.chainPart['addInfo']:
      self.setup_muXX_cosmicEF()
    else:
      logMuonDef.error('Chain %s could not be assembled' % (self.chainPartName))
      return False

    # this is for Datascouting, sequence gets added at the end of a "normal" muon configuration
    if (("ds1" in self.chainPart['addInfo']) or ("ds2" in self.chainPart['addInfo'])  or ("ds3" in self.chainPart['addInfo'])) and (self.mult==1):
      self.addDataScoutingSequence()
    

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


############################### HELPER FUNCTIONS ##############################
  def getMuFastThresh(self):
    if "idperf" in self.chainPart['reccalibInfo']:
      muFastThresh = str(self.chainPart['threshold'])+ "GeV" + "_v11a"
    elif "perf" in self.chainPart['addInfo']:
      muFastThresh = "passthrough"
    elif "0eta105" in self.chainPart['etaRange']:
      muFastThresh = str(self.chainPart['threshold'])+ "GeV" + "_barrelOnly" + "_v11a"
    elif int(self.chainPart['threshold'])  == 4:
      muFastThresh = '4GeV_v11a'
    elif int(self.chainPart['threshold']) == 2:
      muFastThresh = '2GeV'
    else:
      muFastThresh = '6GeV_v11a'
    return muFastThresh
    

  def getL2AlgName(self):
    if int(self.chainPart['threshold'])  <= 4:
      L2AlgName = '900GeV'
    else:
      L2AlgName = 'Muon'    

    if (self.chainPart['reccalibInfo'] == "muoncalib"):

      if "ds3" in self.chainPart['addInfo']: 
        L2AlgName= 'MuonCalibDataScouting' 
      else:
        L2AlgName= 'MuonCalib'
        
    return L2AlgName


  def getMuCombThresh(self):
    if "idperf" in self.chainPart['reccalibInfo']:
      muCombThresh = "passthrough"
    elif "perf" in self.chainPart['addInfo']:
      muCombThresh = "passthrough"
    elif int(self.chainPart['threshold']) >= 24:
      muCombThresh = '22GeV'
    else:
      muCombThresh = str(self.chainPart['threshold'])+ "GeV"
    return muCombThresh


  def getEFExtrapolatorThresh(self):
    if "0eta105" in self.chainPart['etaRange']:
      EFExtrapolatorThresh = str(self.chainPart['threshold'])+ "GeV" + "_barrelOnly"
    else:
      EFExtrapolatorThresh = str(self.chainPart['threshold'])+ "GeV"
    return EFExtrapolatorThresh


  def getEFCombinerThresh(self):
    if "idperf" in self.chainPart['reccalibInfo']:
      EFCombinerThresh = "passthrough"
    elif "perf" in self.chainPart['addInfo']:
      EFCombinerThresh = "passthrough"
    else:
      EFCombinerThresh = str(self.chainPart['threshold'])+ "GeV"
    return EFCombinerThresh
############################### HELPER FUNCTIONS - END ##############################




############################### DEFINE GROUPS OF CHAINS HERE ##############################

  def setup_muXX_ID(self):

    L2AlgName = self.getL2AlgName()
    muFastThresh = self.getMuFastThresh()
    EFCombinerThresh = self.getEFCombinerThresh()
		    
    #--- L2 algos ---
    if "l2muonSA" in self.chainPart['L2SAAlg']:
      from TrigL2MuonSA.TrigL2MuonSAConfig import TrigL2MuonSAConfig
      theL2StandAloneAlg  = TrigL2MuonSAConfig(L2AlgName)
      from TrigMuonHypo.TrigMuonHypoConfig import MufastHypoConfig
      theL2StandAloneHypo = MufastHypoConfig(L2AlgName, muFastThresh)
    else:
      logMuonDef.error("Chain built with %s but so far only l2muonSA is supported." % (self.chainPart['L2SAAlg']))
      return False

    from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_Muon
    theTrigFastTrackFinder_Muon = TrigFastTrackFinder_Muon()
    from InDetTrigRecExample.EFInDetConfig import TrigEFIDSequence
    theTrigEFIDDataPrep_Muon = TrigEFIDSequence("Muon","muon","DataPrep").getSequence()
    
    #if "L2StarA" in self.chainPart['L2IDAlg']:                           # ---> this is Run1 tracking - keep it here
    #  from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_MuonA
    #  theTrigL2SiTrackFinder_MuonA = TrigL2SiTrackFinder_MuonA()
    #  from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_muonIsoA
    #  theTrigL2SiTrackFinder_muonIsoA = TrigL2SiTrackFinder_muonIsoA()
    #  id_alg_output = "STRATEGY_A" #depends on id_alg
    #else:
    #  logMuonDef.error("Chain built with %s but so far only L2StarA,B and C are supported." % (self.chainPart['L2IDAlg']))
    #  return False    
    #id_alg_output = "STRATEGY_A"  

    id_alg_output = "TrigFastTrackFinder_Muon" 
    if "muComb" in self.chainPart['L2CBAlg']:
      muCombThresh = self.getMuCombThresh()
      from TrigmuComb.TrigmuCombConfig import TrigmuCombConfig
      if "r1extr" in self.chainPart['addInfo']:
        theL2CombinedAlg  = TrigmuCombConfig(L2AlgName, id_alg_output,"OLDextr")
      else:
        theL2CombinedAlg  = TrigmuCombConfig(L2AlgName, id_alg_output)
      from TrigMuonHypo.TrigMuonHypoConfig import MucombHypoConfig
      theL2CombinedHypo = MucombHypoConfig(L2AlgName, muCombThresh)
    else:
      logMuonDef.error("Chain built with %s but so far only muComb is supported." % (self.chainPart['L2CBAlg']))
      return False
           
    if "wOvlpRm" in self.chainPart['overlapRemoval']:
      from TrigMuonHypo.TrigL2MuonOverlapRemoverConfig import TrigL2MuonOverlapRemoverConfig
      theL2OvlpRmConfig_mufast = TrigL2MuonOverlapRemoverConfig('Mufast','nominal')
      theL2OvlpRmConfig_mucomb = TrigL2MuonOverlapRemoverConfig('Mucomb','nominal') 

        
    #--- EF algos ---
    if 'SuperEF' in self.chainPart['EFAlg']:
      from AthenaCommon import CfgGetter
      theTrigMuSuperEF = CfgGetter.getAlgorithm("TrigMuSuperEF")
      EFRecoAlgName = "Muon"

      if ("ds2" in self.chainPart['addInfo']):
        theEFAlg= theTrigMuSuperEF
        theEFAlg.MuonContName = "HLT_MuonEFInfoDSOnly"
      else:
        theEFAlg = theTrigMuSuperEF 

    else:
      logMuonDef.error("Chain built with %s but so far only SuperEF is supported." % (self.chainPart['EFAlg']))
      return False
    
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFCombinerHypoConfig
    theTrigMuonEFCombinerHypoConfig = TrigMuonEFCombinerHypoConfig(EFRecoAlgName,EFCombinerThresh)

    from TrigMuonEF.TrigMuonEFConfig import TrigMuonEFTrackIsolationConfig
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFTrackIsolationHypoConfig

    from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
    [theFastTrackFinderxAOD] = TrigInDetSequence("Muon","muon","FastxAOD").getSequence()
    
    from InDetTrigRecExample.EFInDetConfig import TrigEFIDSequence
    #theTrigEFIDInsideOut_Muon = TrigEFIDSequence("Muon","muon")          # ---> this is Run1 tracking - keep it here
    theTrigEFIDInsideOutMerged_Muon = TrigEFIDSequence("Muon","muon","InsideOutMerged")
    theTrigEFIDInsideOut_MuonIso = TrigEFIDSequence("MuonIso","muonIso","InsideOut") 

    #----Sequence list---
    self.L2sequenceList += [[self.L2InputTE,
                             [theL2StandAloneAlg , theL2StandAloneHypo],
                             'L2_mu_step1']] 

    EFinputTE = ''

    if (self.doOvlpRm):
      self.L2sequenceList += [[['L2_mu_step1'], [theL2OvlpRmConfig_mufast ],'L2_step1a_wOvlpRm']]
      #self.L2sequenceList += [[['L2_muon_standalone_wOvlpRm'],           # ---> this is Run1 tracking - keep it here
      #                         [theTrigL2SiTrackFinder_MuonA, 
      #                          theL2CombinedAlg, theL2CombinedHypo],
      #                         'L2_step1a_wOvlpRm']]
      self.L2sequenceList += [[['L2_step1a_wOvlpRm'], theTrigEFIDDataPrep_Muon+
                               [theTrigFastTrackFinder_Muon]+theFastTrackFinderxAOD+[theL2CombinedAlg, theL2CombinedHypo],
                               'L2_step1b_wOvlpRm']]
      self.L2sequenceList += [[['L2_step1b_wOvlpRm'], [ theL2OvlpRmConfig_mucomb ], 'L2_step2_wOvlpRm']]

      EFinputTE = 'L2_step2_wOvlpRm'
    else:
      #self.L2sequenceList += [[['L2_mu_step1'],                          # ---> this is Run1 tracking - keep it here
      #                           [theTrigL2SiTrackFinder_MuonA, 
      #                            theL2CombinedAlg, theL2CombinedHypo],
      #                            #],
      #                           'L2_mu_step2']]
      self.L2sequenceList += [[['L2_mu_step1'],
                                theTrigEFIDDataPrep_Muon+
                               [theTrigFastTrackFinder_Muon]+theFastTrackFinderxAOD+
                               [theL2CombinedAlg,
                                theL2CombinedHypo],
                               'L2_mu_step2']]
      EFinputTE = 'L2_mu_step2'


    #self.EFsequenceList += [[[EFinputTE],                                # ---> this is Run1 tracking - keep it here
    #                          theTrigEFIDInsideOut_Muon.getSequence(),
    #                          'EF_mu_step1']]
    self.EFsequenceList += [[[EFinputTE],
                              theTrigEFIDInsideOutMerged_Muon.getSequence(),
                              'EF_mu_step1']]

    self.EFsequenceList += [[['EF_mu_step1'],
    	  		       [theEFAlg, theTrigMuonEFCombinerHypoConfig],
    	  		        'EF_mu_step2']]
       
    if self.chainPart['isoInfo']:
      if self.chainPart['isoInfo'] == "iloose":
        theTrigMuonEFTrackIsolationHypoConfig = TrigMuonEFTrackIsolationHypoConfig("Muon","RelEFOnlyMedium")
      elif self.chainPart['isoInfo'] == "imedium":
        theTrigMuonEFTrackIsolationHypoConfig = TrigMuonEFTrackIsolationHypoConfig("Muon","RelEFOnlyTightWide")
      else:
        logMuonDef.error("Isolation %s not yet supported." % (self.chainPart['isoInfo']))
        return False

      self.EFsequenceList += [[['EF_mu_step2'],
                               theTrigEFIDInsideOut_MuonIso.getSequence(), 
                               'EF_mu_step3']]
      
      self.EFsequenceList += [[['EF_mu_step3'],
                               [TrigMuonEFTrackIsolationConfig("TrigMuonEFTrackIsolation"),theTrigMuonEFTrackIsolationHypoConfig],
                               'EF_mu_step4']]



    #--- adding signatures ----
    self.L2signatureList += [ [['L2_mu_step1']*self.mult] ]
    if (self.doOvlpRm):
      self.L2signatureList += [ [['L2_step1a_wOvlpRm']*self.mult] ]
      self.L2signatureList += [ [['L2_step1b_wOvlpRm']*self.mult] ]
      self.L2signatureList += [ [['L2_step2_wOvlpRm']*self.mult] ]
    else:
      self.L2signatureList += [ [['L2_mu_step2']*self.mult] ]
      
    self.EFsignatureList += [ [['EF_mu_step1']*self.mult] ]
    self.EFsignatureList += [ [['EF_mu_step2']*self.mult] ]

    if (self.chainPart['isoInfo']):# == "iloose" or self.chainPart['isoInfo'] == "imedium":
      self.EFsignatureList += [ [['EF_mu_step3']*self.mult] ]
      self.EFsignatureList += [ [['EF_mu_step4']*self.mult] ]

    if "ds" in self.chainPart['addInfo']:
      self.EFsignatureList += [ [['EF_mu_ds']] ]


    #--- renaming TEs ---
    self.TErenamingDict = {
      'L2_mu_step1': mergeRemovingOverlap('L2_mu_SA_', L2AlgName+muFastThresh+'_'+self.L2InputTE),
      'L2_mu_step2': mergeRemovingOverlap('L2_mucomb_',   self.chainPartNameNoMult.replace('_'+self.chainPart['isoInfo'], '')+'_'+self.L2InputTE),
      'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', self.chainPartNameNoMult+'_'+self.L2InputTE),
      'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   self.chainPartNameNoMult+'_'+self.L2InputTE),
      }    

    if (("ds1" in self.chainPart['addInfo'])):
      chainPartNameNoMultNoDS = self.chainPartNameNoMult.replace('_ds1', '')
    elif (("ds2" in self.chainPart['addInfo'])):
      chainPartNameNoMultNoDS = self.chainPartNameNoMult.replace('_ds2', '')
    elif ("ds3" in self.chainPart['addInfo']): 
      chainPartNameNoMultNoDS = self.chainPartNameNoMult.replace('_ds3', '') 
    else:
      chainPartNameNoMultNoDS = self.chainPartNameNoMult

    if (self.chainPart['isoInfo']):
      self.TErenamingDict.update({'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', chainPartNameNoMultNoDS.replace('_'+self.chainPart['isoInfo'],'')),
                                  'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   chainPartNameNoMultNoDS.replace('_'+self.chainPart['isoInfo'],'')),
                                  'EF_mu_step3': mergeRemovingOverlap('EF_muI_efid_',    chainPartNameNoMultNoDS),
                                  'EF_mu_step4': mergeRemovingOverlap('EF_trkIso_',       chainPartNameNoMultNoDS)}) 
    if self.doOvlpRm:
      self.TErenamingDict.update({'L2_step1a_wOvlpRm'  : mergeRemovingOverlap('L2_mu_SAOvlpRm_',    L2AlgName+muFastThresh+'_'+self.L2InputTE+'_wOvlpRm' ),
                                  'L2_step1b_wOvlpRm'  : mergeRemovingOverlap('L2_muon_comb',       L2AlgName+muCombThresh+'_'+self.L2InputTE+'_wOvlpRm' ),
                                  'L2_step2_wOvlpRm'   : mergeRemovingOverlap('L2_mu_combOvlpRm_',  L2AlgName+muCombThresh+'_'+self.L2InputTE+'_wOvlpRm'),
                                  'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', chainPartNameNoMultNoDS+'_wOvlpRm'),
                                  'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   chainPartNameNoMultNoDS+'_wOvlpRm')})
    if self.doOvlpRm and self.chainPart['isoInfo']:
      self.TErenamingDict.update({'L2_step1a_wOvlpRm'  : mergeRemovingOverlap('L2_mu_SAOvlpRm_',    L2AlgName+muFastThresh+'_'+self.L2InputTE+'_wOvlpRm' ),
                                  'L2_step1b_wOvlpRm'  : mergeRemovingOverlap('L2_muon_comb',       L2AlgName+muCombThresh+'_'+self.L2InputTE+'_wOvlpRm' ),
                                  'L2_step2_wOvlpRm'   : mergeRemovingOverlap('L2_mu_combOvlpRm_',  L2AlgName+muCombThresh+'_'+self.L2InputTE+'_wOvlpRm'),
                                  'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', chainPartNameNoMultNoDS+'_wOvlpRm'),
                                  'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   chainPartNameNoMultNoDS+'_wOvlpRm'),
                                  'EF_mu_step3': mergeRemovingOverlap('EF_muI_efid_',    chainPartNameNoMultNoDS+'_wOvlpRm'),
                                  'EF_mu_step4': mergeRemovingOverlap('EF_trkIso_',       chainPartNameNoMultNoDS+'_wOvlpRm')}) 
     


                                      
  #################################################################################################
  #################################################################################################

  def setup_muXX_JPsiL2(self):

    L2AlgName = self.getL2AlgName()
    muFastThresh = self.getMuFastThresh()
		    
    #--- L2 algos ---
    if "l2muonSA" in self.chainPart['L2SAAlg']:
      from TrigL2MuonSA.TrigL2MuonSAConfig import TrigL2MuonSAConfig
      theL2StandAloneAlg  = TrigL2MuonSAConfig(L2AlgName)
      from TrigMuonHypo.TrigMuonHypoConfig import MufastHypoConfig
      theL2StandAloneHypo = MufastHypoConfig(L2AlgName, muFastThresh)
    else:
      logMuonDef.error("Chain built with %s but so far only l2muonSA is supported." % (self.chainPart['L2SAAlg']))
      return False

    from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_Muon
    theTrigFastTrackFinder_Muon = TrigFastTrackFinder_Muon()
    from InDetTrigRecExample.EFInDetConfig import TrigEFIDSequence
    theTrigEFIDDataPrep_Muon = TrigEFIDSequence("Muon","muon","DataPrep").getSequence()
    
    id_alg_output = "TrigFastTrackFinder_Muon" 
    if "muComb" in self.chainPart['L2CBAlg']:
      muCombThresh = self.getMuCombThresh()
      from TrigmuComb.TrigmuCombConfig import TrigmuCombConfig
      theL2CombinedAlg  = TrigmuCombConfig(L2AlgName, id_alg_output)
      from TrigMuonHypo.TrigMuonHypoConfig import MucombHypoConfig
      theL2CombinedHypo = MucombHypoConfig(L2AlgName, muCombThresh)
    else:
      logMuonDef.error("Chain built with %s but so far only muComb is supported." % (self.chainPart['L2CBAlg']))
      return False
                   
    from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
    [theFastTrackFinderxAOD] = TrigInDetSequence("Muon","muon","FastxAOD").getSequence()
    
    from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_Jpsi
    theL2JpsimumuAlgo = L2BMuMuFex_Jpsi()
    from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_Jpsi
    theL2JpsimumuHypo = L2BMuMuHypo_Jpsi()

    #----Sequence list---
    self.L2sequenceList += [[self.L2InputTE,
                             [theL2StandAloneAlg , theL2StandAloneHypo],
                             'L2_mu_step1']] 

    EFinputTE = ''

    self.L2sequenceList += [[['L2_mu_step1'],
    			      theTrigEFIDDataPrep_Muon+
    			     [theTrigFastTrackFinder_Muon]+theFastTrackFinderxAOD+
    			     [theL2CombinedAlg,
    			      theL2CombinedHypo],
    			     'L2_mu_step2']]
			     
    self.L2sequenceList += [[['L2_mu_step2']*self.mult,
                             [theL2JpsimumuAlgo,theL2JpsimumuHypo],
                             'L2_mu_step3']]

    #--- adding signatures ----
    self.L2signatureList += [ [['L2_mu_step1']*self.mult] ]
    self.L2signatureList += [ [['L2_mu_step2']*self.mult] ]
    self.L2signatureList += [ [['L2_mu_step3']] ]
      
    #--- renaming TEs ---
    self.TErenamingDict = {
      'L2_mu_step1': mergeRemovingOverlap('L2_mu_SA_', L2AlgName+muFastThresh+'_'+self.L2InputTE),
      'L2_mu_step2': mergeRemovingOverlap('L2_mucomb_',   self.chainPartNameNoMult.replace('_'+self.chainPart['isoInfo'], '')+'_'+self.L2InputTE),
      'L2_mu_step3': mergeRemovingOverlap('L2_JPsimumu_',   self.chainPartNameNoMult.replace('_'+self.chainPart['isoInfo'], '')+'_'+self.L2InputTE),
      }    

                                      
  #################################################################################################
  #################################################################################################

  def setup_muXX_idperf(self):

    L2AlgName = self.getL2AlgName()
    muFastThresh = self.getMuFastThresh()
    EFExtrapolatorThresh = self.getEFExtrapolatorThresh()
    EFCombinerThresh = self.getEFCombinerThresh()
		    
    ########### L2 algos  #################

    if "l2muonSA" in self.chainPart['L2SAAlg']:
      from TrigL2MuonSA.TrigL2MuonSAConfig import TrigL2MuonSAConfig
      theL2StandAloneAlg  = TrigL2MuonSAConfig(L2AlgName)
      from TrigMuonHypo.TrigMuonHypoConfig import MufastHypoConfig
      theL2StandAloneHypo = MufastHypoConfig(L2AlgName, muFastThresh)
    else:
      logMuonDef.error("Chain built with %s but so far only l2muonSA is supported." % (self.chainPart['L2SAAlg']))
      return False

    from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_MuonA     # ---> this is Run1 tracking - keep it here
    theTrigL2SiTrackFinder_MuonA = TrigL2SiTrackFinder_MuonA()
    from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_MuonB
    theTrigL2SiTrackFinder_MuonB = TrigL2SiTrackFinder_MuonB()
    from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_MuonC
    theTrigL2SiTrackFinder_MuonC = TrigL2SiTrackFinder_MuonC()

    from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_Muon
    theTrigFastTrackFinder_Muon = TrigFastTrackFinder_Muon()
    from InDetTrigRecExample.EFInDetConfig import TrigEFIDSequence
    from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
    [theFastTrackFinderxAOD] = TrigInDetSequence("Muon","muon","FastxAOD").getSequence()
    theTrigEFIDDataPrep_Muon = TrigEFIDSequence("Muon","muon","DataPrep").getSequence()
    
    #id_alg_output = "STRATEGY_A"
    id_alg_output = "TrigFastTrackFinder_Muon" 

    if "muComb" in self.chainPart['L2CBAlg']:
      muCombThresh = self.getMuCombThresh()
      from TrigmuComb.TrigmuCombConfig import TrigmuCombConfig
      theL2CombinedAlg  = TrigmuCombConfig(L2AlgName, id_alg_output)
      from TrigMuonHypo.TrigMuonHypoConfig import MucombHypoConfig
      theL2CombinedHypo = MucombHypoConfig(L2AlgName, muCombThresh)
    else:
      logMuonDef.error("Chain built with %s but so far only muComb is supported." % (self.chainPart['L2CBAlg']))
      return False
        
    ########### EF algos  #################

    if 'SuperEF' in self.chainPart['EFAlg']:
      from AthenaCommon import CfgGetter
      theTrigMuSuperEF = CfgGetter.getAlgorithm("TrigMuSuperEF_TMEFonly")
      theEFAlg = theTrigMuSuperEF 
      EFRecoAlgName = "Muon"
    else:
      logMuonDef.error("Chain built with %s but so far only SuperEF is supported." % (self.chainPart['EFAlg']))
      return False

    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFCombinerHypoConfig
    theTrigMuonEFCombinerHypoConfig = TrigMuonEFCombinerHypoConfig(EFRecoAlgName,EFCombinerThresh)
   
    from InDetTrigRecExample.EFInDetConfig import TrigEFIDSequence
    theTrigEFIDInsideOutMerged_Muon     = TrigEFIDSequence("Muon","muon","InsideOutMerged")
    theTrigEFIDInsideOut_Muon = TrigEFIDSequence("Muon","muon")          # ---> this is Run1 tracking - keep it here
    #theTrigEFIDInsideOut_Muon_MergedDP = TrigEFIDSequence("Muon","muon","DataPrep")
    theTrigEFIDOutsideInTRTOnly_Muon = TrigEFIDSequence("Muon","muon","TRTOnly")

    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFExtrapolatorHypoConfig
    theTrigMuonEFExtrapolatorHypoConfig = TrigMuonEFExtrapolatorHypoConfig(EFRecoAlgName, EFExtrapolatorThresh)

    ########### Sequence List ##############

    self.L2sequenceList += [[self.L2InputTE,
                             [theL2StandAloneAlg , theL2StandAloneHypo],
                             'L2_mu_step1']] 

    EFinputTE = ''

    if "L2Star" in self.chainPart['L2IDAlg']:                             # ---> this is Run1 tracking - keep it here
      self.L2sequenceList += [[['L2_mu_step1'],
                               [theTrigL2SiTrackFinder_MuonA, 
                                theTrigL2SiTrackFinder_MuonB,
                                theTrigL2SiTrackFinder_MuonC, 
                                theL2CombinedAlg, theL2CombinedHypo],
                               'L2_mu_step2']]
    else:
      self.L2sequenceList += [[['L2_mu_step1'],
                                theTrigEFIDDataPrep_Muon+
                               [theTrigFastTrackFinder_Muon]+theFastTrackFinderxAOD+
                               [theL2CombinedAlg,theL2CombinedHypo],
                                'L2_mu_step2']]

    EFinputTE = 'L2_mu_step2'

      
    if "L2Star" in self.chainPart['L2IDAlg']:                             # ---> this is Run1 tracking - keep it here
      self.EFsequenceList += [[[EFinputTE],
                                theTrigEFIDInsideOut_Muon.getSequence(),
                               'EF_mu_step1']]
    else:
      self.EFsequenceList += [[[EFinputTE],
                                theTrigEFIDInsideOutMerged_Muon.getSequence(),
                               'EF_mu_step1']]

    #self.EFsequenceList += [[['EF_mu_step1'],
    #	  		      [theTrigEFIDOutsideInTRTOnly_Muon.getSequence()],
    #	  		       'EF_mu_step2']]

    self.EFsequenceList += [[['EF_mu_step1'],
    	  		     [theEFAlg, theTrigMuonEFExtrapolatorHypoConfig],
    	  		      'EF_mu_step3']]

    self.EFsequenceList += [[['EF_mu_step3'],
    	  		     [theTrigMuonEFCombinerHypoConfig],
    	  		      'EF_mu_step4']]

    ########### Signatures ###########
      
    self.L2signatureList += [ [['L2_mu_step1']*self.mult] ]
    self.L2signatureList += [ [['L2_mu_step2']*self.mult] ]
      
    self.EFsignatureList += [ [['EF_mu_step1']*self.mult] ]
    #self.EFsignatureList += [ [['EF_mu_step2']*self.mult] ]
    self.EFsignatureList += [ [['EF_mu_step3']*self.mult] ]
    self.EFsignatureList += [ [['EF_mu_step4']*self.mult] ]

    ########### TE renaming ##########

    self.TErenamingDict = {
      'L2_mu_step1': mergeRemovingOverlap('L2_mu_SA_',  L2AlgName+muFastThresh+'_'+self.L2InputTE),
      'L2_mu_step2': mergeRemovingOverlap('L2_mucomb_', self.chainPartNameNoMult.replace('_'+self.chainPart['isoInfo'], '')+'_'+self.L2InputTE),
      'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOutMerged_', self.chainPartNameNoMult),
      #'EF_mu_step2': mergeRemovingOverlap('EF_TRT_',   self.chainPartNameNoMult),
      'EF_mu_step3': mergeRemovingOverlap('EF_SuperEF_TMEFOnly_',   self.chainPartNameNoMult),
      'EF_mu_step4': mergeRemovingOverlap('EF_Comb_',   self.chainPartNameNoMult),
     }
    
  #################################################################################################
  #################################################################################################

  def setup_muXX_MSOnly(self):

    L2AlgName = self.getL2AlgName()
    muFastThresh = self.getMuFastThresh()
    EFExtrapolatorThresh = self.getEFExtrapolatorThresh()
	
    ########### L2 algos  #################

    if "l2muonSA" in self.chainPart['L2SAAlg']:
      from TrigL2MuonSA.TrigL2MuonSAConfig import TrigL2MuonSAConfig
      theL2StandAloneAlg  = TrigL2MuonSAConfig(L2AlgName)
      from TrigMuonHypo.TrigMuonHypoConfig import MufastHypoConfig
      theL2StandAloneHypo = MufastHypoConfig(L2AlgName, muFastThresh)
    else:
      logMuonDef.error("Chain built with %s but so far only l2muonSA is supported." % (self.chainPart['L2SAAlg']))
      return False

    ########### EF algos  #################

    if 'SuperEF' in self.chainPart['EFAlg']:
      from AthenaCommon import CfgGetter
      theTrigMuSuperEF = CfgGetter.getAlgorithm("TrigMuSuperEF_SAonly")
      theEFAlg = theTrigMuSuperEF 
      EFRecoAlgName = "Muon"
    else:
      logMuonDef.error("Chain built with %s but so far only SuperEF is supported." % (self.chainPart['EFAlg']))
      return False

    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFExtrapolatorHypoConfig
    theTrigMuonEFExtrapolatorHypoConfig = TrigMuonEFExtrapolatorHypoConfig(EFRecoAlgName, EFExtrapolatorThresh)

    ########### Sequence List ##############

    self.L2sequenceList += [[self.L2InputTE,
                             [theL2StandAloneAlg, theL2StandAloneHypo],
                             'L2_mu_step1']] 

    self.EFsequenceList += [[['L2_mu_step1'],
    	  		       [theEFAlg, theTrigMuonEFExtrapolatorHypoConfig],
    	  		      'EF_mu_step1']]
       
    ########### Signatures ###########
      
    self.L2signatureList += [ [['L2_mu_step1']*self.mult] ]
    self.EFsignatureList += [ [['EF_mu_step1']*self.mult] ]

    ########### TE renaming ##########

    self.TErenamingDict = {
      'L2_mu_step1': mergeRemovingOverlap('L2_mu_SA_', L2AlgName+muFastThresh+'_'+self.L2InputTE),
      'EF_mu_step1': mergeRemovingOverlap('EF_SuperEF_',   self.chainPartNameNoMult),
      }

                                          

  #################################################################################################
  #################################################################################################
  def setup_muXX_noL1(self):

    ########### EF algos  #################

    from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo

    from AthenaCommon import CfgGetter

    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFExtrapolatorMultiHypoConfig, TrigMuonEFExtrapolatorHypoConfig

    ##Use list of muon threshold in the chain to correctly configure the FS hypos
    
    if len(self.allMuThrs) == 0:
      log.error("The list of allMuonThreshold is empty for a noL1 chain! It should never happen")

    if len(self.allMuThrs) == 1:
        theTrigMuonEFSA_FS_Hypo = TrigMuonEFExtrapolatorHypoConfig('Muon', '0GeV')
        hypocut = '0GeV'

    elif len(self.allMuThrs) == 2:
        theTrigMuonEFSA_FS_Hypo = TrigMuonEFExtrapolatorMultiHypoConfig('Muon', '0GeV','0GeV')
        hypocut = '0GeV_0GeV'

    elif len(self.allMuThrs) == 3:
        theTrigMuonEFSA_FS_Hypo = TrigMuonEFExtrapolatorMultiHypoConfig('Muon', '0GeV','0GeV','0GeV')
        hypocut = '0GeV_0GeV_0GeV'
    else:
      log.error("No MuonEFExtrapolatorHypo config yet for events with more than 3 muons")


    from InDetTrigRecExample.EFInDetConfig import TrigEFIDSequence
    from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
    theTrigEFIDDataPrep_Muon = TrigEFIDSequence("Muon","muon","DataPrep").getSequence()
    from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_Muon
    theTrigFastTrackFinder_Muon = TrigFastTrackFinder_Muon()
    theTrigEFIDInsideOutMerged_Muon = TrigEFIDSequence("Muon","muon","InsideOutMerged").getSequence()
    theTrigEFIDInsideOut_Muon = TrigEFIDSequence("Muon","muon","InsideOut").getSequence()
    [theFastTrackFinderxAOD] = TrigInDetSequence("Muon","muon","FastxAOD").getSequence()


    from TrigMuonEF.TrigMuonEFConfig import TrigMuonEFRoiAggregatorConfig
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFCombinerMultiHypoConfig, TrigMuonEFCombinerHypoConfig, TrigMuonEFCombinerDiMuonMassHypoConfig

 
    if "JpsimumuFS" in self.chainPart['FSinfo']:
      theTrigMuonEFCombinerMultiHypoConfig = TrigMuonEFCombinerDiMuonMassHypoConfig('Jpsi', "OS")
      hypocutEF="DiMuonMass_Jpsi" 
    else:    
      if len(self.allMuThrs) == 1:
        theTrigMuonEFCombinerMultiHypoConfig = TrigMuonEFCombinerHypoConfig('Muon', self.allMuThrs[0])
      
      elif len(self.allMuThrs) == 2:
        theTrigMuonEFCombinerMultiHypoConfig = TrigMuonEFCombinerMultiHypoConfig('Muon',self.allMuThrs[0], self.allMuThrs[1]) 
                                                                               
      elif len(self.allMuThrs) == 3:
        theTrigMuonEFCombinerMultiHypoConfig = TrigMuonEFCombinerMultiHypoConfig('Muon',self.allMuThrs[0],self.allMuThrs[1],self.allMuThrs[2])
                                                                                                                                                            
      else:
        log.error("No TrigMuonEFCombinerHypo config yet for events with more than 3 muons")
 
      hypocutEF="MultiComb"     
      for i in range(0,len(self.allMuThrs)):        
        hypocutEF +=  "_%s" %(self.allMuThrs[i])


    ########### Sequence List ##############

    self.EFsequenceList += [['',
                            [PESA__DummyUnseededAllTEAlgo("EFDummyAlgo")]+
                            [CfgGetter.getAlgorithm("TrigMuSuperEF_FSSA"),
                             theTrigMuonEFSA_FS_Hypo],
                             'EF_SA_FS']]
    self.EFsequenceList += [['EF_SA_FS',
                            [CfgGetter.getAlgorithm("TrigMuonEFFSRoiMaker")],
                             'EF_SAR_FS']]
    self.EFsequenceList += [['EF_SAR_FS',
                             theTrigEFIDDataPrep_Muon+[theTrigFastTrackFinder_Muon]+theFastTrackFinderxAOD+theTrigEFIDInsideOutMerged_Muon,                             #theTrigEFIDInsideOut_Muon,     #a fallback - it should be replaced by the previous line if it works
                             'EF_FStracksMuon']]
    self.EFsequenceList += [['EF_FStracksMuon',
                            [CfgGetter.getAlgorithm("TrigMuSuperEF_TMEFCombinerOnly")],
                             'EF_CB_FS_single']]
    self.EFsequenceList += [['EF_CB_FS_single',
                            [TrigMuonEFRoiAggregatorConfig('TrigMuonEFFSRoiAggregator'),
                             theTrigMuonEFCombinerMultiHypoConfig],
                             'EF_CB_FS']]

    ########### Signatures ###########
      
    #self.EFsignatureList += [ [['EF_SA_FS']*self.mult] ]
    #self.EFsignatureList += [ [['EF_SAR_FS']*self.mult] ]
    #self.EFsignatureList += [ [['EF_FStracksMuon']*self.mult] ]
    #self.EFsignatureList += [ [['EF_CB_FS_single']*self.mult] ]
    #self.EFsignatureList += [ [['EF_CB_FS']*self.mult] ]
    self.EFsignatureList += [ [['EF_SA_FS']] ]
    self.EFsignatureList += [ [['EF_SAR_FS']] ]
    self.EFsignatureList += [ [['EF_FStracksMuon']] ]
    self.EFsignatureList += [ [['EF_CB_FS_single']] ]
    self.EFsignatureList += [ [['EF_CB_FS']] ]

    ########### TE renaming ##########

    self.TErenamingDict = {
      'EF_SA_FS': mergeRemovingOverlap('EF_SA_FS_','SAFSHypo'+hypocut+'_'+hypocutEF),
      'EF_SAR_FS': mergeRemovingOverlap('EF_SAR_FS_','SAFSHypo'+hypocut+'_'+hypocutEF),
      'EF_FStracksMuon': mergeRemovingOverlap('EF_FStracksMuon_', 'SAFSHypo'+hypocut+'_'+hypocutEF),
      'EF_CB_FS_single': mergeRemovingOverlap('EF_CB_FS_single_','SAFSHypo'+hypocut+'_'+hypocutEF), 
      'EF_CB_FS': mergeRemovingOverlap('EF_CB_FS_', 'SAFSHypo'+hypocut+'_'+hypocutEF),

      }

  #################################################################################################
  #################################################################################################
  def setup_muXX_NS(self):

    ########### EF algos  #################

    from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo

    from AthenaCommon import CfgGetter

    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFExtrapolatorMultiHypoConfig, TrigMuonEFExtrapolatorHypoConfig

    ##Use list of muon threshold in the chain to correctly configure the NS hypos
    
    if len(self.allMuThrs) == 0:
      log.error("The list of allMuonThreshold is empty for a noL1 chain! It should never happen")

    if len(self.allMuThrs) == 1:
        theTrigMuonEFSA_NS_Hypo = TrigMuonEFExtrapolatorHypoConfig('Muon', '0GeV')
        hypocut = '0GeV'

    elif len(self.allMuThrs) == 2:
        theTrigMuonEFSA_NS_Hypo = TrigMuonEFExtrapolatorMultiHypoConfig('Muon', '0GeV','0GeV')
        hypocut = '0GeV_0GeV'

    elif len(self.allMuThrs) == 3:
        theTrigMuonEFSA_NS_Hypo = TrigMuonEFExtrapolatorMultiHypoConfig('Muon', '0GeV','0GeV','0GeV')
        hypocut = '0GeV_0GeV_0GeV'
    else:
      log.error("No MuonEFExtrapolatorHypo config yet for events with more than 3 muons")


    from InDetTrigRecExample.EFInDetConfig import TrigEFIDSequence
    from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
    theTrigEFIDDataPrep_Muon = TrigEFIDSequence("Muon","muon","DataPrep").getSequence()
    from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_Muon
    theTrigFastTrackFinder_Muon = TrigFastTrackFinder_Muon()
    theTrigEFIDInsideOutMerged_Muon = TrigEFIDSequence("Muon","muon","InsideOutMerged").getSequence()
    theTrigEFIDInsideOut_Muon = TrigEFIDSequence("Muon","muon","InsideOut").getSequence()
    theFastTrackFinderxAOD = TrigInDetSequence("Muon","muon","FastxAOD").getSequence()


    from TrigMuonEF.TrigMuonEFConfig import TrigMuonEFRoiAggregatorConfig
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFCombinerMultiHypoConfig, TrigMuonEFCombinerHypoConfig

 
    if len(self.allMuThrs) == 1:
      theTrigMuonEFCombinerMultiHypoConfig = TrigMuonEFCombinerHypoConfig('Muon', self.allMuThrs[0])
      
    elif len(self.allMuThrs) == 2:
      theTrigMuonEFCombinerMultiHypoConfig = TrigMuonEFCombinerMultiHypoConfig('Muon',self.allMuThrs[0], self.allMuThrs[1]) 
                                                                               
    elif len(self.allMuThrs) == 3:
      theTrigMuonEFCombinerMultiHypoConfig = TrigMuonEFCombinerMultiHypoConfig('Muon',self.allMuThrs[0],self.allMuThrs[1],self.allMuThrs[2])
                                                                                                                                                            
    else:
      log.error("No TrigMuonEFCombinerHypo config yet for events with more than 3 muons")
 
    hypocutEF="MultiComb"     
    for i in range(0,len(self.allMuThrs)):        
      hypocutEF +=  "_%s" %(self.allMuThrs[i])

    cone = ""
    ########### Sequence List ##############

    if "nscan03" in self.chainPart['FSinfo']:
      cone = "_cone03"
      self.EFsequenceList += [['',
                              [PESA__DummyUnseededAllTEAlgo("EFDummyAlgo")]+
                              [CfgGetter.getAlgorithm("TrigMuSuperEF_WideCone"),
                               theTrigMuonEFSA_NS_Hypo],
                               'EF_SA_NS']]
      self.EFsequenceList += [['EF_SA_NS',
                              [CfgGetter.getAlgorithm("TrigMuonEFFSRoiMaker")],
                               'EF_SAR_NS']]
      self.EFsequenceList += [['EF_SAR_NS',
                               theTrigEFIDDataPrep_Muon+[theTrigFastTrackFinder_Muon]+theFastTrackFinderxAOD+theTrigEFIDInsideOutMerged_Muon,                             
	   		       'EF_NStracksMuon']]
      self.EFsequenceList += [['EF_NStracksMuon',
                              [CfgGetter.getAlgorithm("TrigMuSuperEF_TMEFCombinerOnly")],
                               'EF_CB_NS_single']]
      self.EFsequenceList += [['EF_CB_NS_single',
                              [TrigMuonEFRoiAggregatorConfig('TrigMuonEFFSRoiAggregator'),
                               theTrigMuonEFCombinerMultiHypoConfig],
                               'EF_CB_NS']]
    elif "nscan05" in self.chainPart['FSinfo']:
      cone = "_cone05"
      self.EFsequenceList += [['',
                              [PESA__DummyUnseededAllTEAlgo("EFDummyAlgo")]+
                              [CfgGetter.getAlgorithm("TrigMuSuperEF_WideCone05"),
                               theTrigMuonEFSA_NS_Hypo],
                               'EF_SA_NS']]
      self.EFsequenceList += [['EF_SA_NS',
                              [TrigMuonEFRoiAggregatorConfig('TrigMuonEFFSRoiAggregator'),
                               theTrigMuonEFCombinerMultiHypoConfig],
                               'EF_NS']]
    else:
      log.error("No other cone than 05 or 03 was implemented")


    ########### Signatures ###########
      
    self.EFsignatureList += [ [['EF_SA_NS']] ]
    if "nscan03" in self.chainPart['FSinfo']:
      self.EFsignatureList += [ [['EF_SAR_NS']] ]
      self.EFsignatureList += [ [['EF_NStracksMuon']] ]
      self.EFsignatureList += [ [['EF_CB_NS_single']] ]
      self.EFsignatureList += [ [['EF_CB_NS']] ]
    if "nscan05" in self.chainPart['FSinfo']:
      self.EFsignatureList += [ [['EF_NS']] ]

    ########### TE renaming ##########

    if "nscan03" in self.chainPart['FSinfo']:
      self.TErenamingDict = {
        'EF_SA_NS': mergeRemovingOverlap('EF_SA_NS_','SANSHypo'+hypocut+'_'+hypocutEF+cone),
        'EF_SAR_NS': mergeRemovingOverlap('EF_SAR_NS_','SANSHypo'+hypocut+'_'+hypocutEF+cone),
        'EF_NStracksMuon': mergeRemovingOverlap('EF_NStracksMuon_', 'SANSHypo'+hypocut+'_'+hypocutEF+cone),
        'EF_CB_NS_single': mergeRemovingOverlap('EF_CB_NS_single_','SANSHypo'+hypocut+'_'+hypocutEF+cone), 
        'EF_CB_NS': mergeRemovingOverlap('EF_CB_NS_', 'SANSHypo'+hypocut+'_'+hypocutEF+cone),
      }
    if "nscan05" in self.chainPart['FSinfo']:
      self.TErenamingDict = {
        'EF_SA_NS': mergeRemovingOverlap('EF_SA_NS_','SANSHypo'+hypocut+'_'+hypocutEF+cone),
        'EF_NS': mergeRemovingOverlap('EF_NS_', 'SANSHypo'+hypocut+'_'+hypocutEF+cone),
      }

  #################################################################################################
  #################################################################################################
  def setup_muXX_cosmicEF(self):

    if 'SuperEF' in self.chainPart['EFAlg']:
      from AthenaCommon import CfgGetter
      theTrigMuSuperEF = CfgGetter.getAlgorithm("TrigMuSuperEF")
      theEFAlg = theTrigMuSuperEF 
      EFRecoAlgName = "Muon"
    else:
      logMuonDef.error("Chain built with %s but so far only SuperEF is supported." % (self.chainPart['EFAlg']))
      return False
    
    ##This is a problem.. SuperEF and msonly are not in the same chainPart...
    if 'msonly' in self.chainPart['reccalibInfo']:
      theTrigMuSuperEF = CfgGetter.getAlgorithm("TrigMuSuperEF_SAonly")
      theEFAlg = theTrigMuSuperEF       
        
      from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFExtrapolatorHypoConfig
      EFExtrapolatorThresh = self.getEFExtrapolatorThresh()
      theTrigMuonEFExtrapolatorHypoConfig = TrigMuonEFExtrapolatorHypoConfig(EFRecoAlgName, EFExtrapolatorThresh)
      ########### Sequence List ##############
      self.EFsequenceList += [[[self.L2InputTE],
    	  		       [theEFAlg, theTrigMuonEFExtrapolatorHypoConfig],
                               'EF_mu_step1']]
    else:
      EFCombinerThresh = self.getEFCombinerThresh()
      
      ########### EF algos  #################
      from InDetTrigRecExample.EFInDetConfig import TrigEFIDInsideOut_CosmicsN
      theEFIDTracking=TrigEFIDInsideOut_CosmicsN()
      
      from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFCombinerHypoConfig
      theTrigMuonEFCombinerHypoConfig = TrigMuonEFCombinerHypoConfig(EFRecoAlgName,EFCombinerThresh)
                  
      
      ########### Sequence List ##############
      self.EFsequenceList += [[[self.L2InputTE],
                               ## old ## theTrigEFIDDataPrep_Muon+[theTrigFastTrackFinder_Muon,theTrigEFIDInsideOutMerged_Muon.getSequence()],
                               #theTrigEFIDDataPrep_Muon+[theEFIDTracking,theTrigEFIDInsideOutMerged_Muon.getSequence()],
                               theEFIDTracking.getSequence(),
                               'EF_mu_step1']]
      
      self.EFsequenceList += [[['EF_mu_step1'],
    	  		       [theEFAlg, theTrigMuonEFCombinerHypoConfig],
                               'EF_mu_step2']]



    ########### Signatures ###########
    self.EFsignatureList += [ [['EF_mu_step1']*self.mult] ]
    if not (self.chainPart['reccalibInfo'] == "msonly"):
      self.EFsignatureList += [ [['EF_mu_step2']*self.mult] ]

    ########### TE renaming ##########
    if (self.chainPart['reccalibInfo'] == "msonly"):
      self.TErenamingDict = {
        'EF_mu_step1': mergeRemovingOverlap('EF_SuperEF_Extrapolator_', self.chainPartNameNoMult),
        }    
    else:
      self.TErenamingDict = {
        'EF_mu_step1': mergeRemovingOverlap('EF_CosmicsN_', self.chainPartNameNoMult),
        'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   self.chainPartNameNoMult),
        }    
    
  #################################################################################################
  #################################################################################################

  def setup_muXX_muoncalib(self):

    L2AlgName = self.getL2AlgName()

    from TrigL2MuonSA.TrigL2MuonSAConfig import TrigL2MuonSAConfig
    theL2StandAloneAlg  = TrigL2MuonSAConfig(L2AlgName)
    
    from TrigMuonHypo.TrigMuonHypoConfig import MufastCALHypoConfig
    
    self.L2sequenceList += [[self.L2InputTE,
                             [theL2StandAloneAlg , MufastCALHypoConfig('MuonCal','Reject') ],
                             'L2_mu_cal']]
    
    #self.calib_streams = ['Muon_Calibration']
        
       
    ########### Signatures ###########
      
    self.L2signatureList += [ [['L2_mu_cal']*self.mult] ]
    

    ########### TE renaming ##########

    self.TErenamingDict = {
      'L2_mu_cal': mergeRemovingOverlap('L2_', L2AlgName+'_'+self.L2InputTE),
      }

                                          

###########################################
## Adding the data scouting sequence at the end
###########################################
  def addDataScoutingSequence(self) : 
      #if (("ds1" in self.chainPart['addInfo']) or ("ds2" in self.chainPart['addInfo'])) and (self.mult==1):
    from TrigDetCalib.TrigDetCalibConf import ScoutingStreamWriter
    if ("ds1" in self.chainPart['addInfo']):
      dsAlg = ScoutingStreamWriter("MuonCosmicDataScouting")
      dsAlg.CollectionTypeName = ['xAOD::MuonContainer_v1#HLT_MuonEFInfo']
    elif ("ds2" in self.chainPart['addInfo']):
      dsAlg = ScoutingStreamWriter("MuonCosmicDataScouting")
      dsAlg.CollectionTypeName = ['xAOD::MuonContainer_v1#HLT_MuonEFInfoDSOnly']      
    elif ("ds3" in self.chainPart['addInfo']): 
      dsAlg = ScoutingStreamWriter("MuonCalibDataScouting")  
      dsAlg.CollectionTypeName = ['TrigComposite#MuonCalibrationStream']        
    else:
      logMuonDef.error("Datascouting configuration not defined for chain %s." % (self.chainName))
      
    
    #inputTE = self.EFsequenceList[-1][-1] # should be "EF_mu_step2" or "EF_mu_step4"
    
    try: 
      inputTE = self.EFsequenceList[-1][-1] # should be "EF_mu_step2" or "EF_mu_step4" 
    except: 
      inputTE = self.L2sequenceList[-1][-1] # should be "EF_mu_step2" or "EF_mu_step4" 
      log.debug("Use L2 algorithm as input to muon data scouting ") 

    outputTE = "EF_mu_ds"
      
    self.EFsequenceList += [[ [inputTE], [dsAlg], outputTE ] ]
    
    self.TErenamingDict.update({'EF_mu_ds'  : mergeRemovingOverlap('EF_',    self.chainPartNameNoMult ),})
