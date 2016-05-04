# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

########################################################################
#
# SliceDef file for muon chains/signatures
#
#########################################################################
from AthenaCommon import CfgGetter
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)

log = logging.getLogger("TriggerMenu.muon.MuonDef")

from TriggerMenu.menu.HltConfig import *

from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
from InDetTrigRecExample.EFInDetConfig import TrigEFIDSequence

from TrigmuComb.TrigmuCombConfig import TrigmuCombConfig
from TrigL2MuonSA.TrigL2MuonSAConfig import TrigL2MuonSAConfig
from TrigMuonHypo.TrigMuonHypoConfig import MucombHypoConfig, MufastHypoConfig

from TrigMuonEF.TrigMuonEFConfig import (TrigMuonEFTrackIsolationConfig,
                                         TrigMuonEFTrackIsolationVarConfig,
                                         TrigMuonEFCaloIsolationConfig,
                                         TrigMuonEFRoiAggregatorConfig)

from TrigMuonHypo.TrigMuonHypoConfig import (TrigMuonEFTrackIsolationHypoConfig,
                                             TrigMuonEFCombinerHypoConfig,
                                             TrigMuonEFCombinerDiMuonMassPtImpactsHypoConfig,
                                             TrigMuonEFCaloIsolationHypoConfig)

#-----------------------------------
class L2EFChain_mu(L2EFChainDef):
#-----------------------------------

  # Define frequently used instances here as class variables
  from TrigMuonHypo.TrigL2MuonOverlapRemoverConfig import TrigL2MuonOverlapRemoverConfig
  theL2OvlpRmConfig_mufast = TrigL2MuonOverlapRemoverConfig('Mufast','nominal')
  theL2OvlpRmConfig_mucomb = TrigL2MuonOverlapRemoverConfig('Mucomb','nominal') 

  # ---- This is for Calo isolation --------------------------------
  from TrigCaloRec.TrigCaloRecConfig import TrigCaloCellMaker_jet_fullcalo, TrigCaloClusterMaker_topo
  trigCaloCellMaker_fullcalo = TrigCaloCellMaker_jet_fullcalo("TrigCaloCellMaker_topo_Muon",
                                                              doNoise=0, AbsE=True, doPers=True)
  topologicalClusterMaker = TrigCaloClusterMaker_topo('TrigCaloClusterMaker_topo_Muon',
                                                      doMoments=True, doLC=False)
  # ----------------------------------------------------------------

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
    if "nscan" in self.chainName or "bTau" in self.chainName :
      self.doOvlpRm = False
    elif "ftkFS" in self.chainPart['FSinfo']:
      self.setup_muXX_noL1FTK()
    elif (self.mult > 1) & ('wOvlpRm' in self.ovlpRm):
      self.doOvlpRm = True
    elif "bJpsi" in self.chainName or "bDimu" in self.chainName or "bUpsi" in self.chainName :
      self.doOvlpRm = False
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
    elif "muL2" in self.chainPart['reccalibInfo']:
      self.setup_muXX_muL2()
    elif "JpsimumuL2" in self.chainPart['FSinfo']:
      self.setup_muXX_JPsiL2()
    elif "nscan" in self.chainPart['FSinfo']:
      self.setup_muXX_NS()
    elif "calotag" in self.chainPart['FSinfo']:
      self.setup_muXX_calotag_noL1()
    elif self.chainPart['extra'] or "JpsimumuFS" in self.chainPart['FSinfo']:
      self.setup_muXX_noL1()
    elif self.chainPart['reccalibInfo'] == "idperf":
      self.setup_muXX_idperf()
    elif self.chainPart['reccalibInfo'] == "l2msonly"  and "cosmicEF" not in self.chainPart['addInfo'] :
      self.setup_muXX_L2MSOnly()
    elif self.chainPart['reccalibInfo'] == "mgonly"  and "cosmicEF" not in self.chainPart['addInfo'] :
      self.setup_muXX_MGOnly()
    elif self.chainPart['reccalibInfo'] == "l2idonly"  and "cosmicEF" not in self.chainPart['addInfo'] :
      self.setup_muXX_l2idonly()
    elif self.chainPart['reccalibInfo'] == "nomucomb"  and "cosmicEF" not in self.chainPart['addInfo'] :
      self.setup_muXX_nomucomb()
    elif self.chainPart['reccalibInfo'] == "msonly"  and "cosmicEF" not in self.chainPart['addInfo'] :
      self.setup_muXX_MSOnly()
    elif self.chainPart['reccalibInfo'] == "muoncalib"  and "cosmicEF" not in self.chainPart['addInfo'] \
          and "ds3" not in self.chainPart['addInfo'] :
      self.setup_muXX_muoncalib()
    elif self.chainPart['reccalibInfo'] == "muoncalib"  and "cosmicEF" not in self.chainPart['addInfo'] \
          and "ds3" in self.chainPart['addInfo'] :
      self.setup_muXX_muoncalib_ds3()
    elif "cosmicEF" in self.chainPart['addInfo']:
      self.setup_muXX_cosmicEF()
    elif "mucombTag" in self.chainPart['reccalibInfo'] and "noEF" in self.chainPart['addInfo'] :
      self.setup_muXX_mucombTag()
    else:
      log.error('Chain %s could not be assembled' % (self.chainPartName))
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
      muFastThresh = str(self.chainPart['threshold'])+ "GeV" + "_v15a"
    elif "perf" in self.chainPart['addInfo']:
      muFastThresh = "passthrough"
    elif "mucombTag" in self.chainPart['reccalibInfo']:
      muFastThresh = "passthrough"
    elif "mgonly" in self.chainPart['reccalibInfo']:
      muFastThresh = "passthrough"
    elif "0eta105" in self.chainPart['etaRange']:
      muFastThresh = str(self.chainPart['threshold'])+ "GeV" + "_barrelOnly" + "_v15a"
    elif int(self.chainPart['threshold'])  == 4:
      muFastThresh = '4GeV_v15a'
    elif int(self.chainPart['threshold']) == 2:
      muFastThresh = '2GeV_v15a'
    else:
      muFastThresh = '6GeV_v15a'
    return muFastThresh
    

  def getL2AlgName(self):

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
    elif "llns" in self.chainPart['addInfo']:
      if int(self.chainPart['threshold']) == 20:
        muCombThresh = '6GeV_v15a'
      elif int(self.chainPart['threshold']) == 11:
        muCombThresh = '4GeV_v15a'
    elif "perf" in self.chainPart['addInfo']:
      muCombThresh = "passthrough"
    elif "mucombTag" in self.chainPart['reccalibInfo']:
      muCombThresh = str(self.chainPart['threshold'])+ "GeV_v15a"
    elif int(self.chainPart['threshold']) >= 24:
      muCombThresh = '22GeV_v15a'
    else:
      muCombThresh = str(self.chainPart['threshold'])+ "GeV_v15a"
    return muCombThresh


  def getEFExtrapolatorThresh(self):
    if "idperf" in self.chainPart['reccalibInfo']:
      EFExtrapolatorThresh = "passthrough"
    elif "0eta105" in self.chainPart['etaRange']:
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
      EFCombinerThresh = str(self.chainPart['threshold'])+ "GeV_v15a"
    return EFCombinerThresh
############################### HELPER FUNCTIONS - END ##############################




############################### DEFINE GROUPS OF CHAINS HERE ##############################

  def setup_muXX_ID(self):

    L2AlgName = self.getL2AlgName()
    muFastThresh = self.getMuFastThresh()
    EFCombinerThresh = self.getEFCombinerThresh()
		    
    #--- L2 algos ---
    if "l2muonSA" in self.chainPart['L2SAAlg']:
      theL2StandAloneAlg  = TrigL2MuonSAConfig(L2AlgName)
      theL2StandAloneHypo = MufastHypoConfig(L2AlgName, muFastThresh)
    else:
      log.error("Chain built with %s but so far only l2muonSA is supported." % (self.chainPart['L2SAAlg']))
      return False

    [trkfast, trkiso, trkprec] = TrigInDetSequence("Muon", "muon", "IDTrig", "2step").getSequence()

    
    #if "L2StarA" in self.chainPart['L2IDAlg']:                           # ---> this is Run1 tracking - keep it here
    #  from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_MuonA
    #  theTrigL2SiTrackFinder_MuonA = TrigL2SiTrackFinder_MuonA()
    #  from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_muonIsoA
    #  theTrigL2SiTrackFinder_muonIsoA = TrigL2SiTrackFinder_muonIsoA()
    #  id_alg_output = "STRATEGY_A" #depends on id_alg
    #else:
    #  log.error("Chain built with %s but so far only L2StarA,B and C are supported." % (self.chainPart['L2IDAlg']))
    #  return False    
    #id_alg_output = "STRATEGY_A"  

    id_alg_output = "TrigFastTrackFinder_Muon" 
    if "muComb" in self.chainPart['L2CBAlg']:
      muCombThresh = self.getMuCombThresh()

      if "r1extr" in self.chainPart['addInfo']:
        theL2CombinedAlg  = TrigmuCombConfig(L2AlgName, id_alg_output,"OLDextr")
      else:
        theL2CombinedAlg  = TrigmuCombConfig(L2AlgName, id_alg_output)

      theL2CombinedHypo = MucombHypoConfig(L2AlgName, muCombThresh)
    else:
      log.error("Chain built with %s but so far only muComb is supported." % (self.chainPart['L2CBAlg']))
      return False                                                           
        
    #--- EF algos ---
    if 'SuperEF' in self.chainPart['EFAlg']:
      theEFAlg = CfgGetter.getAlgorithm("TrigMuSuperEF")      
      EFRecoAlgName = "Muon"
      if ("ds2" in self.chainPart['addInfo']):
        theEFAlg.MuonContName = "HLT_MuonEFInfoDSOnly"
    else:
      log.error("Chain built with %s but so far only SuperEF is supported." % (self.chainPart['EFAlg']))
      return False
    
    theTrigMuonEFCombinerHypoConfig = TrigMuonEFCombinerHypoConfig(EFRecoAlgName,EFCombinerThresh)

    #----Sequence list---
    self.L2sequenceList += [[self.L2InputTE,
                             [theL2StandAloneAlg , theL2StandAloneHypo],
                             'L2_mu_step1']] 

    EFinputTE = ''

    if (self.doOvlpRm):
      self.L2sequenceList += [[['L2_mu_step1'], [self.theL2OvlpRmConfig_mufast ],'L2_step1a_wOvlpRm']]
      #self.L2sequenceList += [[['L2_muon_standalone_wOvlpRm'],           # ---> this is Run1 tracking - keep it here
      #                         [theTrigL2SiTrackFinder_MuonA, 
      #                          theL2CombinedAlg, theL2CombinedHypo],
      #                         'L2_step1a_wOvlpRm']]
      self.L2sequenceList += [[['L2_step1a_wOvlpRm'], 
                               trkfast+[theL2CombinedAlg, theL2CombinedHypo],
                               'L2_step1b_wOvlpRm']]
      if not "noMuCombOvlpRm" in self.chainPart['overlapRemoval']:
        self.L2sequenceList += [[['L2_step1b_wOvlpRm'], [ self.theL2OvlpRmConfig_mucomb ], 'L2_step2_wOvlpRm']]
        EFinputTE = 'L2_step2_wOvlpRm'
      else:
        EFinputTE = 'L2_step1b_wOvlpRm'

    else:
      #self.L2sequenceList += [[['L2_mu_step1'],                          # ---> this is Run1 tracking - keep it here
      #                           [theTrigL2SiTrackFinder_MuonA, 
      #                            theL2CombinedAlg, theL2CombinedHypo],
      #                            #],
      #                           'L2_mu_step2']]
      self.L2sequenceList += [[['L2_mu_step1'],
                               trkfast+
                               [theL2CombinedAlg,
                                theL2CombinedHypo],
                               'L2_mu_step2']]
      EFinputTE = 'L2_mu_step2'


    #self.EFsequenceList += [[[EFinputTE],                                # ---> this is Run1 tracking - keep it here
    #                          theTrigEFIDInsideOut_Muon.getSequence(),
    #                          'EF_mu_step1']]
    self.EFsequenceList += [[[EFinputTE],
                              trkprec,
                              'EF_mu_step1']]

    self.EFsequenceList += [[['EF_mu_step1'],
    	  		       [theEFAlg, theTrigMuonEFCombinerHypoConfig],
    	  		        'EF_mu_step2']]
       
    if '10invm30' in self.chainPart['addInfo'] and 'pt2' in self.chainPart['addInfo'] and 'z10' in self.chainPart['addInfo']:
       theTrigMuonEFCombinerDiMuonMassPtImpactsHypoConfig = TrigMuonEFCombinerDiMuonMassPtImpactsHypoConfig("DiMuon","0")
       self.EFsequenceList += [[['EF_mu_step2'],
                                [theTrigMuonEFCombinerDiMuonMassPtImpactsHypoConfig],
    	  		        'EF_mu_step3']]
    if self.chainPart['isoInfo']:
      if self.chainPart['isoInfo'] == "iloose":
        theTrigMuonEFTrackIsolationHypoConfig = TrigMuonEFTrackIsolationHypoConfig("Muon","RelEFOnlyMedium")
      elif self.chainPart['isoInfo'] == "imedium":
        theTrigMuonEFTrackIsolationHypoConfig = TrigMuonEFTrackIsolationHypoConfig("Muon","RelEFOnlyTightWide")
      elif self.chainPart['isoInfo'] == "ivarloose":
        theTrigMuonEFTrackIsolationHypoConfig = TrigMuonEFTrackIsolationHypoConfig("Muon","RelEFOnlyVarMedium")
      elif self.chainPart['isoInfo'] == "ivarmedium":
        theTrigMuonEFTrackIsolationHypoConfig = TrigMuonEFTrackIsolationHypoConfig("Muon","RelEFOnlyVarTightWide")
      elif self.chainPart['isoInfo'] == "icalo":
        theTrigMuonEFCaloIsolationHypoConfig = TrigMuonEFCaloIsolationHypoConfig("Muon", "Rel")
      elif self.chainPart['isoInfo'] == "iloosecalo":
        theTrigMuonEFTrackIsolationHypoConfig = TrigMuonEFTrackIsolationHypoConfig("Muon","RelEFOnlyMedium")
        theTrigMuonEFCaloIsolationHypoConfig = TrigMuonEFCaloIsolationHypoConfig("Muon", "Rel")
      elif self.chainPart['isoInfo'] == "imediumcalo":
        theTrigMuonEFTrackIsolationHypoConfig = TrigMuonEFTrackIsolationHypoConfig("Muon","RelEFOnlyTightWide")
        theTrigMuonEFCaloIsolationHypoConfig = TrigMuonEFCaloIsolationHypoConfig("Muon", "Rel")
      else:
        log.error("Isolation %s not yet supported." % (self.chainPart['isoInfo']))
        return False

      if self.chainPart['isoInfo'] != "icalo":
        self.EFsequenceList += [[['EF_mu_step2'],
                                 trkiso+trkprec,
                                 'EF_mu_step3']]

      if self.chainPart['isoInfo'] == "iloose" or self.chainPart['isoInfo'] == "imedium":
      	self.EFsequenceList += [[['EF_mu_step3'],
        	                 [TrigMuonEFTrackIsolationConfig("TrigMuonEFTrackIsolation"),theTrigMuonEFTrackIsolationHypoConfig],
                	         'EF_mu_step4']]
      elif self.chainPart['isoInfo'] == "ivarloose" or self.chainPart['isoInfo'] == "ivarmedium":
      	self.EFsequenceList += [[['EF_mu_step3'],
        	                 [TrigMuonEFTrackIsolationVarConfig("TrigMuonEFTrackIsolationVar"),theTrigMuonEFTrackIsolationHypoConfig],
                	         'EF_mu_step4']]
      elif self.chainPart['isoInfo'] == "icalo":
        self.EFsequenceList += [[['EF_mu_step2'],
                                 [self.trigCaloCellMaker_fullcalo,self.topologicalClusterMaker],
                                 'EF_mu_step3']]

        self.EFsequenceList += [[['EF_mu_step3'],
                                 [ TrigMuonEFCaloIsolationConfig("TrigMuonEFCaloIsolation"),theTrigMuonEFCaloIsolationHypoConfig ],
                                 'EF_mu_step4']]
      elif self.chainPart['isoInfo'] == "iloosecalo" or self.chainPart['isoInfo'] == "imediumcalo":
        self.EFsequenceList += [[['EF_mu_step3'],
                                 [self.trigCaloCellMaker_fullcalo,self.topologicalClusterMaker],
                                 'EF_mu_step4']]

        self.EFsequenceList += [[['EF_mu_step4'],
                           [TrigMuonEFTrackIsolationVarConfig("TrigMuonEFTrackIsolationVar"),theTrigMuonEFTrackIsolationHypoConfig],
                           'EF_mu_step5']]

        self.EFsequenceList += [[['EF_mu_step5'],
                                 [ TrigMuonEFCaloIsolationConfig("TrigMuonEFCaloIsolation"),theTrigMuonEFCaloIsolationHypoConfig ],
                                 'EF_mu_step6']]


    #--- adding signatures ----
    self.L2signatureList += [ [['L2_mu_step1']*self.mult] ]
    if (self.doOvlpRm):
      self.L2signatureList += [ [['L2_step1a_wOvlpRm']*self.mult] ]
      self.L2signatureList += [ [['L2_step1b_wOvlpRm']*self.mult] ]
      if not "noMuCombOvlpRm" in self.chainPart['overlapRemoval']:
        self.L2signatureList += [ [['L2_step2_wOvlpRm']*self.mult] ]
    else:
      self.L2signatureList += [ [['L2_mu_step2']*self.mult] ]
      
    self.EFsignatureList += [ [['EF_mu_step1']*self.mult] ]
    self.EFsignatureList += [ [['EF_mu_step2']*self.mult] ]

    if '10invm30' in self.chainPart['addInfo'] and 'pt2' in self.chainPart['addInfo'] and 'z10' in self.chainPart['addInfo']:
      self.EFsignatureList += [ [['EF_mu_step3']] ]

    if (self.chainPart['isoInfo']):# == "iloose" or self.chainPart['isoInfo'] == "imedium":
      self.EFsignatureList += [ [['EF_mu_step3']*self.mult] ]
      self.EFsignatureList += [ [['EF_mu_step4']*self.mult] ]
      if self.chainPart['isoInfo'] == 'iloosecalo' or self.chainPart['isoInfo'] == 'imediumcalo':
        self.EFsignatureList += [ [['EF_mu_step5']*self.mult] ]
        self.EFsignatureList += [ [['EF_mu_step6']*self.mult] ]

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

    if 'llns' in self.chainPart['addInfo']:
      self.TErenamingDict.update({'L2_mu_step2'   : mergeRemovingOverlap('L2_mucomb_',  L2AlgName+muCombThresh+'_'+self.L2InputTE+'_llns')}) 
    if '10invm30' in self.chainPart['addInfo'] and 'pt2' in self.chainPart['addInfo'] and 'z10' in self.chainPart['addInfo']:
      self.TErenamingDict.update({'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', chainPartNameNoMultNoDS.replace('_'+self.chainPart['isoInfo'],'')),
                                  'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   chainPartNameNoMultNoDS.replace('_'+self.chainPart['isoInfo'],'')),
                                  'EF_mu_step3': mergeRemovingOverlap('EF_invm_',    chainPartNameNoMultNoDS)})

    if (self.chainPart['isoInfo']) and ( self.chainPart['isoInfo'] not in ["icalo","iloosecalo","imediumcalo"] ):
      self.TErenamingDict.update({'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', chainPartNameNoMultNoDS.replace('_'+self.chainPart['isoInfo'],'')),
                                  'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   chainPartNameNoMultNoDS.replace('_'+self.chainPart['isoInfo'],'')),
                                  'EF_mu_step3': mergeRemovingOverlap('EF_muI_efid_',    chainPartNameNoMultNoDS),
                                  'EF_mu_step4': mergeRemovingOverlap('EF_trkIso_',       chainPartNameNoMultNoDS)})
    if (self.chainPart['isoInfo']) and ( self.chainPart['isoInfo'] == "icalo" ):
      self.TErenamingDict.update({'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', self.chainPartNameNoMult.replace('_'+self.chainPart['isoInfo'],'')),
                                  'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   self.chainPartNameNoMult.replace('_'+self.chainPart['isoInfo'],'')),
                                  'EF_mu_step3': mergeRemovingOverlap('EF_fullcalo_',    self.chainPartNameNoMult),
                                  'EF_mu_step4': mergeRemovingOverlap('EF_caloIso_',       self.chainPartNameNoMult)})
    if (self.chainPart['isoInfo']) and ( self.chainPart['isoInfo'] == "iloosecalo" or self.chainPart['isoInfo'] == "imediumcalo"):
      self.TErenamingDict.update({'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', self.chainPartNameNoMult.replace('_'+self.chainPart['isoInfo'],'')),
                                  'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   self.chainPartNameNoMult.replace('_'+self.chainPart['isoInfo'],'')),
                                  'EF_mu_step3': mergeRemovingOverlap('EF_muI_efid_',    chainPartNameNoMultNoDS),
                                  'EF_mu_step4': mergeRemovingOverlap('EF_fullcalo_',    self.chainPartNameNoMult),
                                  'EF_mu_step5': mergeRemovingOverlap('EF_trkIso_',       self.chainPartNameNoMult),
                                  'EF_mu_step6': mergeRemovingOverlap('EF_caloIso_',       self.chainPartNameNoMult)})

    if self.doOvlpRm:
      if not "noMuCombOvlpRm" in self.chainPart['overlapRemoval']:
        self.TErenamingDict.update({'L2_step1a_wOvlpRm'  : mergeRemovingOverlap('L2_mu_SAOvlpRm_',    L2AlgName+muFastThresh+'_'+self.L2InputTE+'_wOvlpRm' ),
                                    'L2_step1b_wOvlpRm'  : mergeRemovingOverlap('L2_muon_comb',       L2AlgName+muCombThresh+'_'+self.L2InputTE+'_wOvlpRm' ),
                                    'L2_step2_wOvlpRm'   : mergeRemovingOverlap('L2_mu_combOvlpRm_',  L2AlgName+muCombThresh+'_'+self.L2InputTE+'_wOvlpRm'),
                                    'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', chainPartNameNoMultNoDS+'_wOvlpRm'),
                                    'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   chainPartNameNoMultNoDS+'_wOvlpRm')})
      else:
        self.TErenamingDict.update({'L2_step1a_wOvlpRm'  : mergeRemovingOverlap('L2_mu_SAOvlpRm_',    L2AlgName+muFastThresh+'_'+self.L2InputTE+'_wOvlpRm' ),
                                    'L2_step1b_wOvlpRm'  : mergeRemovingOverlap('L2_muon_comb',       L2AlgName+muCombThresh+'_'+self.L2InputTE+'_wOvlpRm' ),
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
    if self.doOvlpRm and '10invm30' in self.chainPart['addInfo'] and 'pt2' in self.chainPart['addInfo'] and 'z10' in self.chainPart['addInfo']:
      self.TErenamingDict.update({'L2_step1a_wOvlpRm'  : mergeRemovingOverlap('L2_mu_SAOvlpRm_',    L2AlgName+muFastThresh+'_'+self.L2InputTE+'_wOvlpRm' ),
                                  'L2_step1b_wOvlpRm'  : mergeRemovingOverlap('L2_muon_comb',       L2AlgName+muCombThresh+'_'+self.L2InputTE+'_wOvlpRm' ),
                                  'L2_step2_wOvlpRm'   : mergeRemovingOverlap('L2_mu_combOvlpRm_',  L2AlgName+muCombThresh+'_'+self.L2InputTE+'_wOvlpRm'),
                                  'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', chainPartNameNoMultNoDS+'_wOvlpRm'),
                                  'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   chainPartNameNoMultNoDS+'_wOvlpRm'),
                                  'EF_mu_step3': mergeRemovingOverlap('EF_invm_',    chainPartNameNoMultNoDS+'_wOvlpRm')}) 
     


                                      
  #################################################################################################
  #################################################################################################

  def setup_muXX_muL2(self):

    L2AlgName = self.getL2AlgName()
    muFastThresh = self.getMuFastThresh()
		    
    #--- L2 algos ---
    if "l2muonSA" in self.chainPart['L2SAAlg']:
      theL2StandAloneAlg  = TrigL2MuonSAConfig(L2AlgName)
      theL2StandAloneHypo = MufastHypoConfig(L2AlgName, muFastThresh)
    else:
      log.error("Chain built with %s but so far only l2muonSA is supported." % (self.chainPart['L2SAAlg']))
      return False

    [trkfast, trkprec, trkiso] = TrigInDetSequence("Muon", "muon", "IDTrig", "2step").getSequence()
    
    id_alg_output = "TrigFastTrackFinder_Muon" 
    if "muComb" in self.chainPart['L2CBAlg']:
      muCombThresh = self.getMuCombThresh()
      theL2CombinedAlg  = TrigmuCombConfig(L2AlgName, id_alg_output)
      theL2CombinedHypo = MucombHypoConfig(L2AlgName, muCombThresh)
    else:
      log.error("Chain built with %s but so far only muComb is supported." % (self.chainPart['L2CBAlg']))
      return False
                   
    #----Sequence list---
    self.L2sequenceList += [[self.L2InputTE,
                             [theL2StandAloneAlg , theL2StandAloneHypo],
                             'L2_mu_step1']] 

    EFinputTE = ''

    self.L2sequenceList += [[['L2_mu_step1'],
    			      trkfast+
    			     [theL2CombinedAlg,
    			      theL2CombinedHypo],
    			     'L2_mu_step2']]
			     
    #--- adding signatures ----
    self.L2signatureList += [ [['L2_mu_step1']*self.mult] ]
    self.L2signatureList += [ [['L2_mu_step2']*self.mult] ]
      
    #--- renaming TEs ---
    self.TErenamingDict = {
      'L2_mu_step1': mergeRemovingOverlap('L2_mu_SA_', L2AlgName+muFastThresh+'_'+self.L2InputTE),
      'L2_mu_step2': mergeRemovingOverlap('L2_mucomb_',   self.chainPartNameNoMult.replace('_'+self.chainPart['isoInfo'], '')+'_'+self.L2InputTE),
      }    

                                      
  #################################################################################################
  #################################################################################################

  def setup_muXX_JPsiL2(self):

    L2AlgName = self.getL2AlgName()
    muFastThresh = self.getMuFastThresh()
		    
    #--- L2 algos ---
    if "l2muonSA" in self.chainPart['L2SAAlg']:
      theL2StandAloneAlg  = TrigL2MuonSAConfig(L2AlgName)
      theL2StandAloneHypo = MufastHypoConfig(L2AlgName, muFastThresh)
    else:
      log.error("Chain built with %s but so far only l2muonSA is supported." % (self.chainPart['L2SAAlg']))
      return False

    [trkfast, trkprec] = TrigInDetSequence("Muon", "muon", "IDTrig").getSequence()
    
    id_alg_output = "TrigFastTrackFinder_Muon" 
    if "muComb" in self.chainPart['L2CBAlg']:
      muCombThresh = self.getMuCombThresh()
      theL2CombinedAlg  = TrigmuCombConfig(L2AlgName, id_alg_output)
      theL2CombinedHypo = MucombHypoConfig(L2AlgName, muCombThresh)
    else:
      log.error("Chain built with %s but so far only muComb is supported." % (self.chainPart['L2CBAlg']))
      return False
                   
    
    from TrigBphysHypo.TrigL2BMuMuFexConfig import L2BMuMuFex_Jpsi_noSameMu
    theL2JpsimumuAlgo = L2BMuMuFex_Jpsi_noSameMu()
    from TrigBphysHypo.TrigL2BMuMuHypoConfig import L2BMuMuHypo_Jpsi_noSameMu
    theL2JpsimumuHypo = L2BMuMuHypo_Jpsi_noSameMu()

    #----Sequence list---
    self.L2sequenceList += [[self.L2InputTE,
                             [theL2StandAloneAlg , theL2StandAloneHypo],
                             'L2_mu_step1']] 

    EFinputTE = ''

    self.L2sequenceList += [[['L2_mu_step1'],
    			      trkfast+
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
      theL2StandAloneAlg  = TrigL2MuonSAConfig(L2AlgName)
      theL2StandAloneHypo = MufastHypoConfig(L2AlgName, muFastThresh)
    else:
      log.error("Chain built with %s but so far only l2muonSA is supported." % (self.chainPart['L2SAAlg']))
      return False

    from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_MuonA     # ---> this is Run1 tracking - keep it here
    theTrigL2SiTrackFinder_MuonA = TrigL2SiTrackFinder_MuonA()
    from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_MuonB
    theTrigL2SiTrackFinder_MuonB = TrigL2SiTrackFinder_MuonB()
    from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_MuonC
    theTrigL2SiTrackFinder_MuonC = TrigL2SiTrackFinder_MuonC()

    from TrigInDetConf.TrigInDetFTKSequence import TrigInDetFTKSequence
    if "FTKRefit" in self.chainPart['L2IDAlg']:
      [trkfast, trkprec] = TrigInDetFTKSequence("Muon","muon",sequenceFlavour=["refit","PT"]).getSequence()
    elif "FTK" in self.chainPart['L2IDAlg']:
      [trkfast, trkprec] = TrigInDetFTKSequence("Muon","muon",sequenceFlavour=["PT"]).getSequence()
    else:
      [trkfast, trkprec] = TrigInDetSequence("Muon", "muon", "IDTrig").getSequence()



    
    #id_alg_output = "STRATEGY_A"
    id_alg_output = "TrigFastTrackFinder_Muon" 

    if "muComb" in self.chainPart['L2CBAlg']:
      muCombThresh = self.getMuCombThresh()
      theL2CombinedAlg  = TrigmuCombConfig(L2AlgName, id_alg_output)
      theL2CombinedHypo = MucombHypoConfig(L2AlgName, muCombThresh)
    else:
      log.error("Chain built with %s but so far only muComb is supported." % (self.chainPart['L2CBAlg']))
      return False
        
    ########### EF algos  #################

    if 'SuperEF' in self.chainPart['EFAlg']:
      theTrigMuSuperEF = CfgGetter.getAlgorithm("TrigMuSuperEF_SAonly")
      theEFAlg = theTrigMuSuperEF 
      EFRecoAlgName = "Muon"
    else:
      log.error("Chain built with %s but so far only SuperEF is supported." % (self.chainPart['EFAlg']))
      return False

    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFCombinerHypoConfig
    theTrigMuonEFCombinerHypoConfig = TrigMuonEFCombinerHypoConfig(EFRecoAlgName,EFCombinerThresh)
   
    theTrigEFIDInsideOut_Muon = TrigEFIDSequence("Muon","muon")          # ---> this is Run1 tracking - keep it here

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
                               trkfast+
                               [theL2CombinedAlg,theL2CombinedHypo],
                                'L2_mu_step2']]

    EFinputTE = 'L2_mu_step2'

      
    if "L2Star" in self.chainPart['L2IDAlg']:                             # ---> this is Run1 tracking - keep it here
      self.EFsequenceList += [[[EFinputTE],
                                theTrigEFIDInsideOut_Muon.getSequence(),
                               'EF_mu_step1']]
    else:
      self.EFsequenceList += [[[EFinputTE],
                               trkprec,
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
      'EF_mu_step3': mergeRemovingOverlap('EF_SuperEF_SAonly_',   self.chainPartNameNoMult),
      'EF_mu_step4': mergeRemovingOverlap('EF_Comb_',   self.chainPartNameNoMult),
     }
    
  #################################################################################################
  #################################################################################################

  def setup_muXX_L2MSOnly(self):

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
      log.error("Chain built with %s but so far only l2muonSA is supported." % (self.chainPart['L2SAAlg']))
      return False

    from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
    [trkfast, trkprec] = TrigInDetSequence("Muon", "muon", "IDTrig").getSequence()

    #--- EF algos ---
    if 'SuperEF' in self.chainPart['EFAlg']:
      theTrigMuSuperEF = CfgGetter.getAlgorithm("TrigMuSuperEF")
      EFRecoAlgName = "Muon"
      theEFAlg = theTrigMuSuperEF 
    else:
      log.error("Chain built with %s but so far only SuperEF is supported." % (self.chainPart['EFAlg']))
      return False
    
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFCombinerHypoConfig
    theTrigMuonEFCombinerHypoConfig = TrigMuonEFCombinerHypoConfig(EFRecoAlgName,EFCombinerThresh)

    #----Sequence list---
    self.L2sequenceList += [[self.L2InputTE,
                             [theL2StandAloneAlg , theL2StandAloneHypo],
                             'L2_mu_step1']] 

    EFinputTE = ''

    if (self.doOvlpRm):
      self.L2sequenceList += [[['L2_mu_step1'], [self.theL2OvlpRmConfig_mufast ],'L2_mu_step1_wOvlpRm']]
      EFinputTE = 'L2_mu_step1_wOvlpRm'
    else:
      EFinputTE = 'L2_mu_step1'

    self.EFsequenceList += [[[EFinputTE],
                              trkfast+trkprec,
                              'EF_mu_step1']]

    self.EFsequenceList += [[['EF_mu_step1'],
    	  		       [theEFAlg, theTrigMuonEFCombinerHypoConfig],
    	  		        'EF_mu_step2']]
       
    #--- adding signatures ----
    self.L2signatureList += [ [['L2_mu_step1']*self.mult] ]
    if (self.doOvlpRm):
      self.L2signatureList += [ [['L2_mu_step1_wOvlpRm']*self.mult] ]
      
    self.EFsignatureList += [ [['EF_mu_step1']*self.mult] ]
    self.EFsignatureList += [ [['EF_mu_step2']*self.mult] ]

    #--- renaming TEs ---
    self.TErenamingDict = {
      'L2_mu_step1': mergeRemovingOverlap('L2_mu_SA_', L2AlgName+muFastThresh+'_'+self.L2InputTE),
      'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', self.chainPartNameNoMult+'_'+self.L2InputTE),
      'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   self.chainPartNameNoMult+'_'+self.L2InputTE),
      }    

    if self.doOvlpRm:
      self.TErenamingDict.update({'L2_mu_step1_wOvlpRm'  : mergeRemovingOverlap('L2_mu_SAOvlpRm_',    L2AlgName+muFastThresh+'_'+self.L2InputTE+'_wOvlpRm' ),
                                  'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', self.chainPartNameNoMult+'_wOvlpRm'),
                                  'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   self.chainPartNameNoMult+'_wOvlpRm')})                                
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
      log.error("Chain built with %s but so far only l2muonSA is supported." % (self.chainPart['L2SAAlg']))
      return False

    ########### EF algos  #################

    if 'SuperEF' in self.chainPart['EFAlg']:
      theTrigMuSuperEF = CfgGetter.getAlgorithm("TrigMuSuperEF_SAonly")
      theEFAlg = theTrigMuSuperEF 
      EFRecoAlgName = "Muon"
    else:
      log.error("Chain built with %s but so far only SuperEF is supported." % (self.chainPart['EFAlg']))
      return False

    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFExtrapolatorHypoConfig
    theTrigMuonEFExtrapolatorHypoConfig = TrigMuonEFExtrapolatorHypoConfig(EFRecoAlgName, EFExtrapolatorThresh)

    if "wOvlpRm" in self.chainPart['overlapRemoval']:
      from TrigMuonHypo.TrigEFMuonOverlapRemoverConfig import TrigEFMuonOverlapRemoverConfig
      theEFOvlpRmConfig = TrigEFMuonOverlapRemoverConfig('MuExtr','loose')

    ########### Sequence List ##############

    self.L2sequenceList += [[self.L2InputTE,
                             [theL2StandAloneAlg, theL2StandAloneHypo],
                             'L2_mu_step1']] 

    EFinputTE = ''

    if (self.doOvlpRm):
      self.L2sequenceList += [[['L2_mu_step1'], [self.theL2OvlpRmConfig_mufast ],'L2_mu_step1_wOvlpRm']]
      EFinputTE = 'L2_mu_step1_wOvlpRm'
    else:
      EFinputTE = 'L2_mu_step1'

    self.EFsequenceList += [[[EFinputTE],
    	  		       [theEFAlg, theTrigMuonEFExtrapolatorHypoConfig],
    	  		      'EF_mu_step1']]

    if (self.doOvlpRm):
      self.EFsequenceList += [[['EF_mu_step1'], [theEFOvlpRmConfig ],'EF_mu_step1_wOvlpRm']]
       
    ########### Signatures ###########
      
    self.L2signatureList += [ [['L2_mu_step1']*self.mult] ]
    if (self.doOvlpRm):
      self.L2signatureList += [ [['L2_mu_step1_wOvlpRm']*self.mult] ]
    self.EFsignatureList += [ [['EF_mu_step1']*self.mult] ]
    if (self.doOvlpRm):
      self.EFsignatureList += [ [['EF_mu_step1_wOvlpRm']*self.mult] ]

    ########### TE renaming ##########

    self.TErenamingDict = {
      'L2_mu_step1': mergeRemovingOverlap('L2_mu_SA_', L2AlgName+muFastThresh+'_'+self.L2InputTE),
      'EF_mu_step1': mergeRemovingOverlap('EF_SuperEF_',   self.chainPartNameNoMult),
      }

    if self.doOvlpRm:
      self.TErenamingDict.update({'L2_mu_step1_wOvlpRm'  : mergeRemovingOverlap('L2_mu_SAOvlpRm_',    L2AlgName+muFastThresh+'_'+self.L2InputTE+'_wOvlpRm' ),
                                  'EF_mu_step1': mergeRemovingOverlap('EF_SuperEF_', self.chainPartNameNoMult+'_wOvlpRm'),
				  'EF_mu_step1_wOvlpRm': mergeRemovingOverlap('EF_SuperEFOvlpRm_', self.chainPartNameNoMult+'_wEFOvlpRm')})
                                          
  #################################################################################################
  def setup_muXX_MGOnly(self):

    log.debug("MuonDef.py define MGonly")

    L2AlgName = self.getL2AlgName()
    muFastThresh = self.getMuFastThresh()
    EFExtrapolatorThresh = self.getEFExtrapolatorThresh()
    EFCombinerThresh = self.getEFCombinerThresh()

    log.debug('L2AlgName %s', L2AlgName)
    log.debug('muFastThresh %s', muFastThresh)
    log.debug('EFExtrapolatorThresh %s', EFExtrapolatorThresh)
    log.debug('EFCombinerThresh %s %s', EFCombinerThresh)
	
    ########### L2 algos  #################

    from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
    [trkfast, trkprec] = TrigInDetSequence("Muon", "muon", "IDTrig").getSequence()

    ########### EF algos  #################

    if 'SuperEF' in self.chainPart['EFAlg']:
      theTrigMuSuperEF = CfgGetter.getAlgorithm("TrigMuSuperEF_MGonly")
      EFRecoAlgName = "Muon"
    else:
      log.error("Chain built with %s but so far only SuperEF is supported." % (self.chainPart['EFAlg']))
      return False

    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFCombinerHypoConfig
    theTrigMuonEFCombinerHypoConfig = TrigMuonEFCombinerHypoConfig(EFRecoAlgName,EFCombinerThresh)   

  #################################################################################################
  #################################################################################################
  def setup_muXX_noL1(self):

    ########### EF algos  #################

    from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo

    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFExtrapolatorMultiHypoConfig, TrigMuonEFExtrapolatorHypoConfig

    ##Use list of muon threshold in the chain to correctly configure the FS hypos
    
    if "JpsimumuFS" in self.chainPart['FSinfo']:
      theTrigMuonEFSA_FS_Hypo = TrigMuonEFExtrapolatorMultiHypoConfig('Muon','0GeV','0GeV')
      hypocut='0GeV_0GeV'    
    else:
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

    [trkfast, trkprec] = TrigInDetSequence("Muon", "muon", "IDTrig").getSequence()

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
    if 'msonly' in self.chainPart['reccalibInfo']:

      self.EFsequenceList += [['',
                               [PESA__DummyUnseededAllTEAlgo("EFDummyAlgo")]+
                               [CfgGetter.getAlgorithm("TrigMuSuperEF_FSSA"),
                                theTrigMuonEFSA_FS_Hypo],
                               'EF_SA_FS']]
    else:
      self.EFsequenceList += [['',
                               [PESA__DummyUnseededAllTEAlgo("EFDummyAlgo")]+
                               [CfgGetter.getAlgorithm("TrigMuSuperEF_FSSA"),
                                theTrigMuonEFSA_FS_Hypo],
                               'EF_SA_FS']]
 
      self.EFsequenceList += [['EF_SA_FS',
                               [CfgGetter.getAlgorithm("TrigMuonEFFSRoiMaker")],
                               'EF_SAR_FS']]
      self.EFsequenceList += [['EF_SAR_FS',
                               trkfast+trkprec,                 #theTrigEFIDInsideOut_Muon,     #a fallback - it should be replaced by the previous line if it works
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
    if 'msonly' in self.chainPart['reccalibInfo']:
      self.EFsignatureList += [ [['EF_SA_FS']] ]
    else:
      self.EFsignatureList += [ [['EF_SA_FS']] ]
      self.EFsignatureList += [ [['EF_SAR_FS']] ]
      self.EFsignatureList += [ [['EF_FStracksMuon']] ]
      self.EFsignatureList += [ [['EF_CB_FS_single']] ]
      self.EFsignatureList += [ [['EF_CB_FS']] ]

    ########### TE renaming ##########

    if 'msonly' in self.chainPart['reccalibInfo']:
      self.TErenamingDict = {
        'EF_SA_FS': mergeRemovingOverlap('EF_SA_FS_msonly_','SAFSHypo'+hypocut),  
         }
    else:
      self.TErenamingDict = {
        'EF_SA_FS': mergeRemovingOverlap('EF_SA_FS_','SAFSHypo'+hypocut),
        'EF_SAR_FS': mergeRemovingOverlap('EF_SAR_FS_','SAFSHypo'+hypocut),
        'EF_FStracksMuon': mergeRemovingOverlap('EF_FStracksMuon_', 'SAFSHypo'+hypocut),
        'EF_CB_FS_single': mergeRemovingOverlap('EF_CB_FS_single_','SAFSHypo'+hypocut), 
        'EF_CB_FS': mergeRemovingOverlap('EF_CB_FS_', 'SAFSHypo'+hypocut+'_'+hypocutEF),

      }


 #################################################################################################
  #################################################################################################
  def setup_muXX_noL1FTK(self):

    from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo
    
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFExtrapolatorMultiHypoConfig, TrigMuonEFExtrapolatorHypoConfig
    
    ########### EF algos  #################
    

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
      
    from TrigInDetConf.TrigInDetFTKSequence import TrigInDetFTKSequence
    [trkfast, trkprec] = TrigInDetSequence("Muon", "muon", "IDTrig").getSequence()
    [trkfastftk, trkprecftk] = TrigInDetFTKSequence("Muon","muon",sequenceFlavour=["refit","PT"]).getSequence()

    from TrigMuonEF.TrigMuonEFConfig import TrigMuonEFFSRoiMaker
    FSroimaker = TrigMuonEFFSRoiMaker("TrigMuonEFFSRoiMakerFTK",CreateFSRoI=True,RoILabel="forID")
    from TrigMuonEF.TrigMuonEFConfig import InDetTrkRoiMaker_Muon
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFCombinerMultiHypoConfig

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
                            [PESA__DummyUnseededAllTEAlgo("EFDummyAlgo",createRoIDescriptors=True)],
                             'EF_dummyFTK']]
    self.EFsequenceList += [['EF_dummyFTK',
                             [FSroimaker],
                             'EF_dummyRoIFTK']]
    self.EFsequenceList += [['EF_dummyRoIFTK',
                             trkfastftk,
                             'EF_trk_trkFTK']]
    self.EFsequenceList += [['EF_trk_trkFTK',
                             [CfgGetter.getAlgorithm("InDetTrkRoiMaker_Muon")],#[InDetTrkRoiMaker_Muon("EFInDetTrkRoiMaker")],
                             'EF_trk_ROIFTK']]
    self.EFsequenceList += [['EF_trk_ROIFTK',
                            [CfgGetter.getAlgorithm("TrigMuSuperEF_FSSA"),
                             theTrigMuonEFSA_FS_Hypo],
                             'EF_SA_FSFTK']]
    self.EFsequenceList += [['EF_SA_FSFTK',
                            [CfgGetter.getAlgorithm("TrigMuonEFFSRoiMaker")],
                             'EF_SAR_FSFTK']]
    self.EFsequenceList += [['EF_SAR_FSFTK',
                             trkfast+trkprec,                 #theTrigEFIDInsideOut_Muon,     #a fallback - it should be replaced by the previous line if it works
                             'EF_FStracksMuonFTK']]
    self.EFsequenceList += [['EF_FStracksMuonFTK',
                             [CfgGetter.getAlgorithm("TrigMuSuperEF_TMEFCombinerOnly")],
                             'EF_CB_FS_singleFTK']]
    self.EFsequenceList += [['EF_CB_FS_singleFTK',
                            [TrigMuonEFRoiAggregatorConfig('TrigMuonEFFSRoiAggregator'),
                             theTrigMuonEFCombinerMultiHypoConfig],
                             'EF_CB_FSFTK']]

    ########### Signatures ###########
      
    self.EFsignatureList += [ [['EF_dummyFTK']] ]
    self.EFsignatureList += [ [['EF_dummyRoIFTK']] ]
    self.EFsignatureList += [ [['EF_trk_trkFTK']] ]
    self.EFsignatureList += [ [['EF_trk_ROIFTK']] ]
    self.EFsignatureList += [ [['EF_SA_FSFTK']] ]
    self.EFsignatureList += [ [['EF_SAR_FSFTK']] ]
    self.EFsignatureList += [ [['EF_FStracksMuonFTK']] ]
    self.EFsignatureList += [ [['EF_CB_FS_singleFTK']] ]
    self.EFsignatureList += [ [['EF_CB_FSFTK']] ]

    ########### TE renaming ##########

    self.TErenamingDict = {
      'EF_trk_ROIFTK': mergeRemovingOverlap('EF_trk_trkFTK_','SAFSHypo'+hypocut),
      'EF_trk_ROIbFTK': mergeRemovingOverlap('EF_trk_ROIFTK_','SAFSHypo'+hypocut),
      'EF_SA_FSFTK': mergeRemovingOverlap('EF_SA_FSFTK_','SAFSHypo'+hypocut),
      'EF_SAR_FSFTK': mergeRemovingOverlap('EF_SAR_FSFTK_','SAFSHypo'+hypocut),
      'EF_FStracksMuonFTK': mergeRemovingOverlap('EF_FStracksMuonFTK_', 'SAFSHypo'+hypocut),
      'EF_CB_FS_singleFTK': mergeRemovingOverlap('EF_CB_FSFTK_single_','SAFSHypo'+hypocut), 
      'EF_CB_FSFTK': mergeRemovingOverlap('EF_CB_FSFTK_', 'SAFSHypo'+hypocut+'_'+hypocutEF),

      }






  #################################################################################################
  #################################################################################################
  def setup_muXX_calotag_noL1(self):

    ########### EF algos  #################

    ##Use list of muon threshold in the chain to correctly configure the FS hypos

    if len(self.allMuThrs) == 0:
      log.error("The list of allMuonThreshold is empty for a noL1 chain! It should never happen")
      return
    
    threshold = str(self.chainPart['threshold'])+'GeV'
    multiplicity = str(self.mult)
    hypocut = 'opposite'
    hypocutEF = multiplicity+"_"+threshold
    seed = self.allMuThrs[0]

    theCTAlg = CfgGetter.getAlgorithm("TrigMuSuperEF_CTonly")

    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonCaloTagHypoConfig
    theTrigMuonCT_FS_Hypo = TrigMuonCaloTagHypoConfig('MuonCT', threshold, int(multiplicity) )

    from TrigCaloRec.TrigCaloRecConfig import TrigCaloClusterMaker_topo, TrigCaloCellMaker_super
    
    from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
    [trkfast, trkprec] = TrigInDetSequence("Muon", "muon", "IDTrig").getSequence()
    
    ########### Sequence List ##############
    if "0eta010" in self.chainPart['etaRange'] or "0eta500" in self.chainPart["etaRange"]:
      seed = '0eta0'
      from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo
      from TrigMuonEF.TrigMuonEFConfig import TrigMuonEFFSRoiMakerUnseededConfig
      if "0eta010" in self.chainPart['etaRange']:
        theEFRoIMaker = TrigMuonEFFSRoiMakerUnseededConfig("TrigMuonEFFSRoiMakerUnseeded_0eta010", RoISizeEta=0.1)
        hypocut = "0eta010"
      elif "0eta500" in self.chainPart['etaRange']:
        theEFRoIMaker = TrigMuonEFFSRoiMakerUnseededConfig("TrigMuonEFFSRoiMakerUnseeded_0eta500", RoISizeEta=5.0)
        hypocut = "0eta500"
      else:
        log.error("Cannot setup unseeded muon calotag chain which is not 0eta500 or 0eta010")
        return
      
      self.EFsequenceList += [["",
                              [PESA__DummyUnseededAllTEAlgo("EFDummyAlgo")],
                               'EF_CT_seed']]
      self.EFsequenceList += [['EF_CT_seed',
                              [theEFRoIMaker],
                               'EF_CT_roi']]
      self.EFsequenceList += [['EF_CT_roi',
                              [TrigCaloCellMaker_super(),
                              TrigCaloClusterMaker_topo()],
                               'EF_CT_calo']]
      self.EFsequenceList += [['EF_CT_calo',
                                trkfast+trkprec,
                               'EF_CT_tracks']]
      self.EFsequenceList += [['EF_CT_tracks',
                               [theCTAlg,
                               theTrigMuonCT_FS_Hypo],
                               'EF_CT_hypo']]
    
    else:
      from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFExtrapolatorHypoConfig
      theTrigMuSuperEF = CfgGetter.getAlgorithm("TrigMuSuperEF")
      theTrigMuonEFHypo = TrigMuonEFExtrapolatorHypoConfig('Muon', self.allMuThrs[0])
      
      from TrigMuonEF.TrigMuonEFConfig import TrigMuonEFRoiAggregatorConfig
      from TrigMuonEF.TrigMuonEFConfig import TrigMuonEFFSRoiMakerCaloTagConfig
      theEFRoIMaker = TrigMuonEFFSRoiMakerCaloTagConfig()
      theEFRoiAggregator = TrigMuonEFRoiAggregatorConfig("TrigMuonCaloTagRoiAggregator", CopyTracks=True)
    
      inputTEfromL2 = "placeHolderTE"

      self.EFsequenceList += [[inputTEfromL2,
                               [theTrigMuSuperEF,
                               theTrigMuonEFHypo],
                               'EF_CT_seed']]
      self.EFsequenceList += [['EF_CT_seed',
                              [theEFRoIMaker],
                               'EF_CT_roi']]
      self.EFsequenceList += [['EF_CT_roi',
                              [TrigCaloCellMaker_super(),
                              TrigCaloClusterMaker_topo()],
                               'EF_CT_calo']]
      self.EFsequenceList += [['EF_CT_calo',
                                trkfast+trkprec,
                               'EF_CT_tracks']]
      self.EFsequenceList += [['EF_CT_tracks',
                               [theCTAlg],
                               'EF_CT_fex']]
      self.EFsequenceList += [['EF_CT_fex',
                               [theEFRoiAggregator,
                               theTrigMuonCT_FS_Hypo],
                               'EF_CT_hypo']]

    
    ########### Signatures ###########
    
    self.EFsignatureList += [ [['EF_CT_seed']] ]
    self.EFsignatureList += [ [['EF_CT_roi']] ]
    self.EFsignatureList += [ [['EF_CT_calo']] ]
    self.EFsignatureList += [ [['EF_CT_tracks']] ]

    if "0eta01" not in self.chainPart['etaRange'] and "0eta500" not in self.chainPart['etaRange']:
      self.EFsignatureList += [ [['EF_CT_fex']] ]
    
    self.EFsignatureList += [ [['EF_CT_hypo']] ]

    ########### TE renaming ##########

    self.TErenamingDict = {
      'EF_CT_seed': mergeRemovingOverlap('EF_CT_seed_',seed),
      'EF_CT_roi': mergeRemovingOverlap('EF_CT_roi_',seed+"_"+hypocut),
      'EF_CT_calo': mergeRemovingOverlap('EF_CT_calo_', seed+"_"+hypocut),
      'EF_CT_tracks': mergeRemovingOverlap('EF_CT_tracks_',seed+"_"+hypocut),
      'EF_CT_fex': mergeRemovingOverlap('EF_CT_fex_', seed+"_"+hypocut),
      'EF_CT_hypo': mergeRemovingOverlap('EF_CT_hypo_',seed+"_"+ hypocut+'_CTHypo_'+hypocutEF),
      }
  #################################################################################################
  #################################################################################################
  def setup_muXX_NS(self):

    ########### EF algos  #################

    from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo

    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFExtrapolatorMultiHypoConfig, TrigMuonEFExtrapolatorHypoConfig

    ##Use list of muon threshold in the chain to correctly configure the NS hypos
    
    if len(self.allMuThrs) == 0:
      log.error("The list of allMuonThreshold is empty for a noL1 chain! It should never happen")

    if len(self.allMuThrs) == 1:
        theTrigMuonEFSA_NS_Hypo = TrigMuonEFExtrapolatorHypoConfig('Muon', '0GeV')
        hypocut = '0'

    elif len(self.allMuThrs) == 2:
        theTrigMuonEFSA_NS_Hypo = TrigMuonEFExtrapolatorMultiHypoConfig('Muon', '0GeV','0GeV')
        hypocut = '0_0'

    elif len(self.allMuThrs) == 3:
        theTrigMuonEFSA_NS_Hypo = TrigMuonEFExtrapolatorMultiHypoConfig('Muon', '0GeV','0GeV','0GeV')
        hypocut = '0_0_0'
    else:
      log.error("No MuonEFExtrapolatorHypo config yet for events with more than 3 muons")


    from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
    [trkfast, trkprec] = TrigInDetSequence("Muon", "muon", "IDTrig").getSequence()


    from TrigMuonEF.TrigMuonEFConfig import TrigMuonEFRoiAggregatorConfig
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFCombinerMultiHypoConfig, TrigMuonEFCombinerHypoConfig

    name = 'Muon'
    if "noComb" in self.chainPart['addInfo']:
      name = 'Muon_noComb'
    
    if len(self.allMuThrs) == 1:
      theTrigMuonEFCombinerMultiHypoConfig = TrigMuonEFCombinerHypoConfig(name, self.allMuThrs[0])
      theTrigMuonEFExtrapolatorMultiHypoConfig = TrigMuonEFExtrapolatorHypoConfig('Muon', self.allMuThrs[0])      
      try: hypocut = '%s' %(self.allMuThrs[0].split("GeV")[0])
      except:  hypocut = '%s' %(self.allMuThrs[0])

    elif len(self.allMuThrs) == 2:
      theTrigMuonEFCombinerMultiHypoConfig = TrigMuonEFCombinerMultiHypoConfig(name,self.allMuThrs[0], self.allMuThrs[1]) 
      theTrigMuonEFExtrapolatorMultiHypoConfig = TrigMuonEFExtrapolatorMultiHypoConfig('Muon',self.allMuThrs[0], self.allMuThrs[1])       
      try: hypocut = '%s_%s' %(self.allMuThrs[0].split("GeV")[0], self.allMuThrs[1].split("GeV")[0])                  
      except:hypocut = '%s_%s' %(self.allMuThrs[0], self.allMuThrs[1])                  

    elif len(self.allMuThrs) == 3:
      theTrigMuonEFCombinerMultiHypoConfig = TrigMuonEFCombinerMultiHypoConfig(name,self.allMuThrs[0],self.allMuThrs[1],self.allMuThrs[2])
      theTrigMuonEFExtrapolatorMultiHypoConfig = TrigMuonEFExtrapolatorMultiHypoConfig('Muon',self.allMuThrs[0],self.allMuThrs[1],self.allMuThrs[2])
      try: hypocut = '%s_%s_%s' %(self.allMuThrs[0].split("GeV")[0], self.allMuThrs[1].split("GeV")[0],self.allMuThrs[2].split("GeV")[0])      
      except: hypocut = '%s_%s_%s' %(self.allMuThrs[0], self.allMuThrs[1],self.allMuThrs[2])                                  
    else:
        log.error("No TrigMuonEFCombinerHypo config yet for events with more than 3 muons")
 

    if "noComb" in self.chainPart['addInfo']:
      theTrigMuonEFCombinerMultiHypoConfig.RejectCBmuons=True
    hypocutEF="MultiComb"     
    for i in range(0,len(self.allMuThrs)):        
      hypocutEF +=  "_%s" %(self.allMuThrs[i])

    cone = ""
    ########### Sequence List ##############
    inputTEfromL2 = "placeHolderTE"
    if "nscan03" in self.chainPart['FSinfo']:
      cone = "_cone03"
      #      self.EFsequenceList += [[self.chainPart['L1item'].replace("L1_",""),
      self.EFsequenceList += [[inputTEfromL2,
                               [CfgGetter.getAlgorithm("TrigMuSuperEF_WideCone"),
                                theTrigMuonEFSA_NS_Hypo],
                               'EF_SA_NS']]
      self.EFsequenceList += [['EF_SA_NS',
                              [CfgGetter.getAlgorithm("TrigMuonEFFSRoiMaker")],
                               'EF_SAR_NS']]
      self.EFsequenceList += [['EF_SAR_NS',
                               trkfast+trkprec,                             
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
      #      self.EFsequenceList += [[self.chainPart['L1item'].replace("L1_",""),
      self.EFsequenceList += [[inputTEfromL2,
                              [CfgGetter.getAlgorithm("TrigMuSuperEF_WideCone05"),
                               theTrigMuonEFSA_NS_Hypo],
                               'EF_SA_NS']]
      self.EFsequenceList += [['EF_SA_NS',
                              [TrigMuonEFRoiAggregatorConfig('TrigMuonEFFSRoiAggregator'),
#                               theTrigMuonEFCombinerMultiHypoConfig],
                               theTrigMuonEFExtrapolatorMultiHypoConfig],
                               'EF_NS']]
      if "noComb" in self.chainPart['addInfo']:
        self.EFsequenceList += [['EF_NS',
                                 [CfgGetter.getAlgorithm("TrigMuonEFFSRoiMaker")],
                                 'EF_SAR_NS']]
        self.EFsequenceList += [['EF_SAR_NS',
                                 trkfast+trkprec,                 #theTrigEFIDInsideOut_Muon,     #a fallback - it should be replaced by the previous line if it works
                                 'EF_NStracksMuon']]
        self.EFsequenceList += [['EF_NStracksMuon',
                                 [CfgGetter.getAlgorithm("TrigMuSuperEF_TMEFCombinerOnly")],
                                 'EF_CB_NS_single']]
        self.EFsequenceList += [['EF_CB_NS_single',
                                 [TrigMuonEFRoiAggregatorConfig('TrigMuonEFFSRoiAggregator'),
                                  theTrigMuonEFCombinerMultiHypoConfig],
                                 'EF_CB_NS']]

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
      if "noComb" in self.chainPart['addInfo']:
        self.EFsignatureList += [ [['EF_SAR_NS']] ]
        self.EFsignatureList += [ [['EF_NStracksMuon']] ]
        self.EFsignatureList += [ [['EF_CB_NS_single']] ]
        self.EFsignatureList += [ [['EF_CB_NS']] ]

    nscanseed = self.chainPart['L1item']
    l1seed = self.chainL1Item

    if "nscan03" in self.chainPart['FSinfo']:
      self.TErenamingDict = {
        'EF_SA_NS': mergeRemovingOverlap('EF_SA_NS_','SANShyp'+hypocut+cone+'_'+nscanseed+'_'+l1seed),
        'EF_SAR_NS': mergeRemovingOverlap('EF_SAR_NS_','SANShyp'+hypocut+cone+'_'+nscanseed+'_'+l1seed),
        'EF_NStracksMuon': mergeRemovingOverlap('EF_NStrkMu_', 'SANShyp'+hypocut+cone+'_'+nscanseed+'_'+l1seed),
        'EF_CB_NS_single': mergeRemovingOverlap('EF_CB_NS_sngl_','SANShyp'+hypocut+cone+'_'+nscanseed+'_'+l1seed), 
        'EF_CB_NS': mergeRemovingOverlap('EF_CB_NS_', 'SANShyp'+hypocut+'_'+hypocutEF+cone+'_'+nscanseed+'_'+l1seed),
      }
    if "nscan05" in self.chainPart['FSinfo']:
      if "noComb" in self.chainPart['addInfo']:
        print "hi nils "+ self.chainPartL1Item +' ' + self.chainPartName
        self.TErenamingDict = {
          'EF_SA_NS': mergeRemovingOverlap('EF_SA_NS_','SANShyp'+hypocut+cone+'_noComb_'+nscanseed+'_'+l1seed),
          'EF_NS': mergeRemovingOverlap('EF_NS_', 'SANShyp'+hypocut+cone+'_noComb_'+nscanseed+'_'+l1seed), 
          'EF_SAR_NS': mergeRemovingOverlap('EF_SAR_NS_','SANSHypo'+hypocut+cone+'_noComb_'+nscanseed+'_'+l1seed), 
          'EF_NStracksMuon': mergeRemovingOverlap('EF_NStracksMuon_', 'SANSHypo'+hypocut+cone+'_noComb_'+nscanseed+'_'+l1seed), 
          'EF_CB_NS_single': mergeRemovingOverlap('EF_CB_NS_single_','SANSHypo'+hypocut+cone+'_noComb_'+nscanseed+'_'+l1seed), 
          'EF_CB_NS': mergeRemovingOverlap('EF_CB_NS_', 'SANSHypo'+hypocut+'_'+hypocutEF+cone+'_noComb_'+nscanseed+'_'+l1seed),
          }
      else:
        self.TErenamingDict = {
          'EF_SA_NS': mergeRemovingOverlap('EF_SA_NS_','SANShyp'+hypocut+cone+'_'+nscanseed+'_'+l1seed),
          'EF_NS': mergeRemovingOverlap('EF_NS_','SANShyp'+hypocut+'_'+hypocutEF+cone+'_'+nscanseed+'_'+l1seed),
          }
      

  #################################################################################################
  #################################################################################################

  def setup_muXX_mucombTag(self):                                          

    L2AlgName = self.getL2AlgName()
    muFastThresh = self.getMuFastThresh()
		    
    #--- L2 algos ---
    if "l2muonSA" in self.chainPart['L2SAAlg']:
      from TrigL2MuonSA.TrigL2MuonSAConfig import TrigL2MuonSAConfig
      theL2StandAloneAlg  = TrigL2MuonSAConfig(L2AlgName)
      from TrigMuonHypo.TrigMuonHypoConfig import MufastHypoConfig
      theL2StandAloneHypo = MufastHypoConfig(L2AlgName, muFastThresh)
    else:
      log.error("Chain built with %s but so far only l2muonSA is supported." % (self.chainPart['L2SAAlg']))
      return False

    from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
    [trkfast, trkprec] = TrigInDetSequence("Muon", "muon", "IDTrig").getSequence()

    
    id_alg_output = "TrigFastTrackFinder_Muon" 
    if "muComb" in self.chainPart['L2CBAlg']:
      muCombThresh = self.getMuCombThresh()
      theL2CombinedAlg  = TrigmuCombConfig(L2AlgName, id_alg_output)
      theL2CombinedHypo = MucombHypoConfig(L2AlgName, muCombThresh)
    else:
      log.error("Chain built with %s but so far only muComb is supported." % (self.chainPart['L2CBAlg']))
      return False

                   
    #----Sequence list---
    self.L2sequenceList += [[self.L2InputTE,
                             [theL2StandAloneAlg , theL2StandAloneHypo],
                             'L2_mu_step1']] 

    self.L2sequenceList += [[['L2_mu_step1'],
                             trkfast+
                             [theL2CombinedAlg,
                              theL2CombinedHypo],
                             'L2_mu_step2']]

    #--- adding signatures ----
    self.L2signatureList += [ [['L2_mu_step1']*self.mult] ]
    self.L2signatureList += [ [['L2_mu_step2']*self.mult] ]
      
    #--- renaming TEs ---
    self.TErenamingDict = {
      'L2_mu_step1': mergeRemovingOverlap('L2_mu_SA_', L2AlgName+"_"+muFastThresh+'_'+self.L2InputTE),
      'L2_mu_step2': mergeRemovingOverlap('L2_mucomb_',   self.chainPartNameNoMult.replace('_'+self.chainPart['isoInfo'], '')+'_'+self.L2InputTE),
      }    


  #################################################################################################
  #################################################################################################
  def setup_muXX_cosmicEF(self):

    if 'SuperEF' in self.chainPart['EFAlg']:
      theTrigMuSuperEF = CfgGetter.getAlgorithm("TrigMuSuperEF")
      theEFAlg = theTrigMuSuperEF 
      EFRecoAlgName = "Muon"
    else:
      log.error("Chain built with %s but so far only SuperEF is supported." % (self.chainPart['EFAlg']))
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
    

  #################################################################################################
  #################################################################################################

  def setup_muXX_muoncalib_ds3(self):
    
    L2AlgName = self.getL2AlgName()

    from TrigL2MuonSA.TrigL2MuonSAConfig import TrigL2MuonSAConfig
    theL2StandAloneAlg  = TrigL2MuonSAConfig(L2AlgName)

    from TrigDetCalib.TrigDetCalibConf import ScoutingStreamWriter
    dsAlg = ScoutingStreamWriter("MuonCalibDataScouting") 
    dsAlg.CollectionTypeName = ['xAOD::TrigCompositeContainer#MuonCalibrationStream','xAOD::TrigCompositeAuxContainer#MuonCalibrationStreamAux']

    self.L2sequenceList += [[self.L2InputTE,
                             [theL2StandAloneAlg,dsAlg], 
                             'L2_mu_cal']]

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
      dsAlg.CollectionTypeName = ['xAOD::TrigCompositeContainer#MuonCalibrationStream','xAOD::TrigCompositeAuxContainer#MuonCalibrationStreamAux']
    else:
      log.error("Datascouting configuration not defined for chain %s." % (self.chainName))
      
    
    #inputTE = self.EFsequenceList[-1][-1] # should be "EF_mu_step2" or "EF_mu_step4"
    
    try: 
      inputTE = self.EFsequenceList[-1][-1] # should be "EF_mu_step2" or "EF_mu_step4" 
    except: 
      inputTE = self.L2sequenceList[-1][-1] # should be "EF_mu_step2" or "EF_mu_step4" 
      log.debug("Use L2 algorithm as input to muon data scouting ") 

    outputTE = "EF_mu_ds"
      
    self.EFsequenceList += [[ [inputTE], [dsAlg], outputTE ] ]
    
    self.TErenamingDict.update({'EF_mu_ds'  : mergeRemovingOverlap('EF_',    self.chainPartNameNoMult ),})




###########################################
## Adding new l2idonly featrue
###########################################
  def setup_muXX_l2idonly(self):

    L2AlgName = '900GeV'
    muFastThresh = '4GeV_v15a'
    EFCombinerThresh = self.getEFCombinerThresh()

    from TrigGenericAlgs.TrigGenericAlgsConf import PESA__TrigRoiUpdater
    RoiUpdater = PESA__TrigRoiUpdater("TrigRoiUpdater_Muon_l2idonly");
    if int(self.chainPart['threshold']) <= 4:
      RoiUpdater.EtaHalfWidth = 0.20
      RoiUpdater.PhiHalfWidth = 0.20
    elif int(self.chainPart['threshold']) <= 6:
      RoiUpdater.EtaHalfWidth = 0.15
      RoiUpdater.PhiHalfWidth = 0.15
    else:
      RoiUpdater.EtaHalfWidth = 0.10
      RoiUpdater.PhiHalfWidth = 0.10
    
    from TrigL2MuonSA.TrigL2MuonSAConfig import TrigL2MuonSAConfig
    theL2StandAloneAlg  = TrigL2MuonSAConfig(L2AlgName)
    from TrigMuonHypo.TrigMuonHypoConfig import MufastHypoConfig
    theL2StandAloneHypo = MufastHypoConfig(L2AlgName, muFastThresh)

    from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
    [trkfast, trkiso, trkprec] = TrigInDetSequence("Muon", "muon", "IDTrig", "2step").getSequence()

    if 'SuperEF' in self.chainPart['EFAlg']:
      theTrigMuSuperEF = CfgGetter.getAlgorithm("TrigMuSuperEF")
      EFRecoAlgName = "Muon"
      theEFAlg = theTrigMuSuperEF 
    else:
      log.error("Chain built with %s but so far only SuperEF is supported." % (self.chainPart['EFAlg']))
      return False
    
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFCombinerHypoConfig
    theTrigMuonEFCombinerHypoConfig = TrigMuonEFCombinerHypoConfig(EFRecoAlgName,EFCombinerThresh)
    
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonIDTrackMultiHypoConfig
    idmulti = "%ipt%i" % ( int(self.mult), int(self.chainPart['threshold']))
    theTrigMuonIDTrackMultiHypoConfig = TrigMuonIDTrackMultiHypoConfig( idmulti, "FTF" )
    theTrigMuonIDTrackMultiHypoConfig_Muon = TrigMuonIDTrackMultiHypoConfig( idmulti, "Muon" )

    #----Sequence list---
    use_muFast = 0
    if use_muFast == 1:
      self.L2sequenceList += [[self.L2InputTE,
                               [theL2StandAloneAlg , theL2StandAloneHypo],
                               'L2_mu_step1']] 
      self.L2sequenceList += [[['L2_mu_step1'],
                               [self.theL2OvlpRmConfig_mufast ],
                               'L2_mu_step2']]
      self.L2sequenceList += [['L2_mu_step2',
                               trkfast,
                               'L2_mu_step3']] 
      self.L2sequenceList += [['L2_mu_step3',
                               [theTrigMuonIDTrackMultiHypoConfig],
                               'L2_mu_step4']] 
      EFinputTE = 'L2_mu_step4'
    else:
      self.L2sequenceList += [[self.L2InputTE,
                               [RoiUpdater]+trkfast,
                               'L2_mu_step1']] 
      self.L2sequenceList += [['L2_mu_step1',
                               [theTrigMuonIDTrackMultiHypoConfig],
                               'L2_mu_step2']] 
      EFinputTE = 'L2_mu_step2'

    self.EFsequenceList += [[[EFinputTE],
                             trkprec,
                             'EF_mu_step1']]
    self.EFsequenceList += [[['EF_mu_step1'],
                             [theEFAlg, theTrigMuonEFCombinerHypoConfig],
                             'EF_mu_step2']]

    self.EFsequenceList += [['EF_mu_step2',
                             [theTrigMuonIDTrackMultiHypoConfig_Muon],
                             'EF_mu_step3']] 
    
    #--- adding signatures ----    
    if use_muFast == 1:
      self.L2signatureList += [ [['L2_mu_step1']*self.mult] ]
      self.L2signatureList += [ [['L2_mu_step2']*self.mult] ]
      self.L2signatureList += [ [['L2_mu_step3']*self.mult] ]
      self.L2signatureList += [ [['L2_mu_step4']] ]
    else:
      self.L2signatureList += [ [['L2_mu_step1']*self.mult] ]
      self.L2signatureList += [ [['L2_mu_step2']] ]
      

    self.EFsignatureList += [ [['EF_mu_step1']*self.mult] ]
    self.EFsignatureList += [ [['EF_mu_step2']*self.mult] ]
    self.EFsignatureList += [ [['EF_mu_step3']] ]

    #--- renaming TEs ---
    if use_muFast == 1:
      self.TErenamingDict = {
        'L2_mu_step1': mergeRemovingOverlap('L2_muFast_', idmulti+'_'+self.chainPartNameNoMult+'_'+self.L2InputTE),
        'L2_mu_step2': mergeRemovingOverlap('L2_muFastOvRm_', idmulti+'_'+self.chainPartNameNoMult+'_'+self.L2InputTE),
        'L2_mu_step3': mergeRemovingOverlap('L2_idtrking_', idmulti+'_'+self.chainPartNameNoMult+'_'+self.L2InputTE),
        'L2_mu_step4': mergeRemovingOverlap('L2_idtrkmulti_', idmulti+'_'+self.chainPartNameNoMult+'_'+self.L2InputTE),
        'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', idmulti+'_'+self.chainPartNameNoMult+'_'+self.L2InputTE),
        'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   idmulti+'_'+self.chainPartNameNoMult+'_'+self.L2InputTE),
        'EF_mu_step3': mergeRemovingOverlap('EF_mutrkmulti_', idmulti+'_'+self.chainPartNameNoMult+'_'+self.L2InputTE),
        }
    else:
      self.TErenamingDict = {
        'L2_mu_step1': mergeRemovingOverlap('L2_idtrking_', idmulti+'_'+self.chainPartNameNoMult+'_'+self.L2InputTE),
        'L2_mu_step2': mergeRemovingOverlap('L2_idtrkmulti_', idmulti+'_'+self.chainPartNameNoMult+'_'+self.L2InputTE),
        'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', idmulti+'_'+self.chainPartNameNoMult+'_'+self.L2InputTE),
        'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   idmulti+'_'+self.chainPartNameNoMult+'_'+self.L2InputTE),
        'EF_mu_step3': mergeRemovingOverlap('EF_mutrkmulti_', idmulti+'_'+self.chainPartNameNoMult+'_'+self.L2InputTE),
        }
      

###########################################
## Adding new nomucomb featrue
###########################################
  def setup_muXX_nomucomb(self):
    
    L2AlgName = self.getL2AlgName()
    muFastThresh = self.getMuFastThresh()
    EFCombinerThresh = self.getEFCombinerThresh()
    
    from TrigL2MuonSA.TrigL2MuonSAConfig import TrigL2MuonSAConfig
    theL2StandAloneAlg  = TrigL2MuonSAConfig(L2AlgName)
    from TrigMuonHypo.TrigMuonHypoConfig import MufastHypoConfig
    theL2StandAloneHypo = MufastHypoConfig(L2AlgName, muFastThresh)
    
    from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
    [trkfast, trkiso, trkprec] = TrigInDetSequence("Muon", "muon", "IDTrig", "2step").getSequence()
    
    theTrigMuSuperEF = CfgGetter.getAlgorithm("TrigMuSuperEF")
    EFRecoAlgName = "Muon"
    
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFCombinerHypoConfig
    theTrigMuonEFCombinerHypoConfig = TrigMuonEFCombinerHypoConfig(EFRecoAlgName,EFCombinerThresh)
    
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonIDTrackMultiHypoConfig
    idmulti = "%ipt%i" % ( int(self.mult), int(self.chainPart['threshold']))
    theTrigMuonIDTrackMultiHypoConfig_FTF = TrigMuonIDTrackMultiHypoConfig( idmulti, "FTF" )
    theTrigMuonIDTrackMultiHypoConfig_Muon = TrigMuonIDTrackMultiHypoConfig( idmulti, "Muon" )
    
    self.L2sequenceList += [[self.L2InputTE,
                             [theL2StandAloneAlg , theL2StandAloneHypo],
                             'L2_mu_step1']] 
    
    self.L2sequenceList += [['L2_mu_step1',
                             trkfast,
                             'L2_mu_step2']] 
    
    self.L2sequenceList += [['L2_mu_step2',
                             [theTrigMuonIDTrackMultiHypoConfig_FTF],
                             'L2_mu_step3']] 
    
    EFinputTE = 'L2_mu_step3'
    
    self.EFsequenceList += [[[EFinputTE],
                             trkprec,
                             'EF_mu_step1']]
    
    self.EFsequenceList += [[['EF_mu_step1'],
                             [theTrigMuSuperEF, theTrigMuonEFCombinerHypoConfig],
                             'EF_mu_step2']]
    
    self.EFsequenceList += [[['EF_mu_step2'],
                             [theTrigMuonIDTrackMultiHypoConfig_Muon],
                             'EF_mu_step3']]
    
    
    self.L2signatureList += [ [['L2_mu_step1']*self.mult] ]
    self.L2signatureList += [ [['L2_mu_step2']*self.mult] ]
    self.L2signatureList += [ [['L2_mu_step3']          ] ]
    self.EFsignatureList += [ [['EF_mu_step1']*self.mult] ]
    self.EFsignatureList += [ [['EF_mu_step2']*self.mult] ]
    self.EFsignatureList += [ [['EF_mu_step3']          ] ]
    
    self.TErenamingDict = {
      'L2_mu_step1': mergeRemovingOverlap('L2_mu_SA_',  L2AlgName+muFastThresh+'_'+self.L2InputTE),
      'L2_mu_step2': mergeRemovingOverlap('L2_idtrking_', L2AlgName+muFastThresh+'_'+self.L2InputTE),
      'L2_mu_step3': mergeRemovingOverlap('L2_idtrkmulti_', idmulti+'_'+L2AlgName+muFastThresh+'_'+self.L2InputTE),
      'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', idmulti+'_'+self.chainPartNameNoMult+'_'+self.L2InputTE),
      'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   idmulti+'_'+self.chainPartNameNoMult+'_'+self.L2InputTE),
      'EF_mu_step3': mergeRemovingOverlap('EF_mutrkmulti_', idmulti+'_'+self.chainPartNameNoMult+'_'+self.L2InputTE),
      }
    
