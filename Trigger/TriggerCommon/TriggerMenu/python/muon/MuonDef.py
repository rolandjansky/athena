# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

########################################################################
#
# SliceDef file for muon chains/signatures
#
#########################################################################
from AthenaCommon import CfgGetter
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)

log = logging.getLogger("TriggerMenu.muon.MuonDef")

from TriggerMenu.menu.HltConfig import mergeRemovingOverlap,L2EFChainDef

from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
from InDetTrigRecExample.EFInDetConfig import TrigEFIDSequence

from TrigmuComb.TrigmuCombConfig import TrigmuCombConfig
from TrigL2MuonSA.TrigL2MuonSAConfig import TrigL2MuonSAConfig
from TrigMuonHypo.TrigMuonHypoConfig import MucombHypoConfig, MufastHypoConfig

from TrigMuonEF.TrigMuonEFConfig import (TrigMuonEFTrackIsolationConfig,
                                         TrigMuonEFMSTrackIsolationConfig,
                                         TrigMuonEFTrackIsolationVarConfig,
                                         TrigMuonEFCaloIsolationConfig,
                                         TrigMuonEFRoiAggregatorConfig,
                                         TrigMuonEFIDTrackRoiMakerConfig)

from TrigMuonHypo.TrigMuonHypoConfig import (TrigMuonEFTrackIsolationHypoConfig,
                                             TrigMuonEFTrackIsolationMultiHypoConfig,
                                             TrigMuonEFCombinerHypoConfig,
                                             TrigMuonEFCombinerDiMuonMassPtImpactsHypoConfig,
                                             TrigMuonEFCaloIsolationHypoConfig)

from TrigHIHypo.HFMuonHypos import hiHFMuonHypos
from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyCopyAllTEAlgo
from TriggerMenu.commonUtils.makeCaloSequences import getFullScanCaloSequences

from TrigMuSuperEF.TrigMuonEFTagandProbeConfig import TrigMuonEFTagandProbeConfig 

#-----------------------------------
class L2EFChain_mu(L2EFChainDef):
#-----------------------------------

  # Define frequently used instances here as class variables
  from TrigMuonHypo.TrigL2MuonOverlapRemoverConfig import TrigL2MuonOverlapRemoverConfig
  theL2OvlpRmConfig_mufast = TrigL2MuonOverlapRemoverConfig('Mufast','nominal')
  theL2OvlpRmConfig_mucomb = TrigL2MuonOverlapRemoverConfig('Mucomb','nominal') 

  # ---- This is for Calo isolation --------------------------------
  DummyMergerAlgo = PESA__DummyCopyAllTEAlgo("DummyMergerAlgo")
  fullScanSeqMap = getFullScanCaloSequences()
  # ----------------------------------------------------------------

  def __init__(self, chainDict, asymDiMuonChain = False, asymMultiMuonChain = False, AllMuons = [], thisIsBphysChain=False):
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
    if 'AFP' in self.L2InputTE:
      self.L2InputTE = self.L2InputTE.replace("AFP_C_","")
    if self.L2InputTE:      # cut of L1_, _EMPTY,..., & multiplicity
      if "dRl1" in self.chainPart['addInfo']:
        #the dRl1 acts on a list of L1 RoIs, so we need to treat each item in the list separately
        #then recreate the list with proper thresholds
        #would probably be easier to just use ['MU6','MU6'] in the menu, but this way it's consistent 
        tmpL2InputTE = []
        for l1Thr in self.L2InputTE:
          l1Thr = l1Thr.replace("L1_","")
          l1Thr = l1Thr.split("_")[0]
          l1Thr = l1Thr[1:] if l1Thr.isdigit() else l1Thr 
          tmpL2InputTE.append(l1Thr)
        self.L2InputTE = tmpL2InputTE
      else:
        self.L2InputTE = self.L2InputTE.replace("L1_","")
        self.L2InputTE = self.L2InputTE.split("_")[0]
        self.L2InputTE = self.L2InputTE[1:] if self.L2InputTE[0].isdigit() else self.L2InputTE
      
    # --- used to configure hypos for FS muon chains
    self.allMuThrs=AllMuons

    # --- used to set default for Bphys chains
    self.thisIsBphysChain = thisIsBphysChain

    # --- when to run with ovlp removal ---
    self.ovlpRm = self.chainPart['overlapRemoval']
    self.asymDiMuonChain = asymDiMuonChain
    self.asymMultiMuonChain = asymMultiMuonChain

    self.doOvlpRm = False
    if "nscan" in self.chainName or "bTau" in self.chainName :
      self.doOvlpRm = False
    elif "FTKFS" in self.chainPart['FSinfo']:
      self.setup_muXX_noL1FTK()
    elif (self.mult > 1) & ('wOvlpRm' in self.ovlpRm):
      self.doOvlpRm = True
    elif "bJpsi" in self.chainName or "bDimu" in self.chainName or "bUpsi" in self.chainName or self.thisIsBphysChain :
      self.doOvlpRm = False
    elif self.asymMultiMuonChain and not self.chainPart['extra'] and not self.chainPart['reccalibInfo'] and not self.thisIsBphysChain:
      self.doOvlpRm = True
    elif (self.asymDiMuonChain) and (self.mult > 1) and not self.chainPart['extra'] and not self.chainPart['reccalibInfo'] :
      self.doOvlpRm = True
    else: self.doOvlpRm = False
 
      
    # --- call corresponding functions to set up chains ---
    if not self.chainPart['extra'] \
          and not self.chainPart['FSinfo'] \
          and not self.chainPart['hypoInfo'] \
          and not self.chainPart['reccalibInfo'] \
          and "cosmicEF" not in self.chainPart['addInfo'] \
          and not "dRl1" in self.chainPart['addInfo'] \
          and not self.thisIsBphysChain :
      self.setup_muXX_ID()
    elif "muL2" in self.chainPart['reccalibInfo']:
      self.setup_muXX_muL2()
    elif "JpsimumuL2" in self.chainPart['FSinfo']:
      self.setup_muXX_JPsiL2()
    elif "nscan" in self.chainPart['FSinfo']:
      self.setup_muXX_NS()
    elif "calotag" in self.chainPart['FSinfo']:
      self.setup_muXX_calotag_noL1()
    elif "noL1" in self.chainPart['extra'] or "JpsimumuFS" in self.chainPart['FSinfo']:
      self.setup_muXX_noL1()
    elif "dRl1" in self.chainPart['addInfo']:
      self.setup_muXX_dR()
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
    elif self.thisIsBphysChain  and "cosmicEF" not in self.chainPart['addInfo'] :  ## Keep this  at the end, in case non-default chain is requested
      if  self.chainPart['reccalibInfo'] == "btrk"  or "btrk" in self.chainPart['extra']  :
        self.setup_muXX_noL1btrk()
      else :
        self.setup_muXX_nomucomb()
    elif not self.thisIsBphysChain  and  "btrk" in self.chainPart['extra']  :
      log.error('btrk option exists only for bPhys chains, please check the chain %s ' % (self.chainPartName))      
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
    elif int(self.chainPart['threshold'])  == 3:
      muFastThresh = '3GeV_v15a'
    elif int(self.chainPart['threshold']) == 2:
      muFastThresh = '2GeV_v15a'
    elif "3layersEC" in self.chainPart['extra']:
      muFastThresh = str(self.chainPart['threshold'])+ "GeV" + "_v15a"
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
    elif "3layersEC" in self.chainName:
      EFExtrapolatorThresh = str(self.chainPart['threshold'])+ "GeV" + "_msonlyCut"
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
    
    [trkfast, trkiso, trkprec] = TrigInDetSequence("Muon", "muon", "IDTrig", sequenceFlavour=["2step"]).getSequence()
    
    

    id_alg_output = "TrigFastTrackFinder_Muon" 
    if "muComb" in self.chainPart['L2CBAlg']:
      muCombThresh = self.getMuCombThresh()
      
      if "r1extr" in self.chainPart['addInfo']:
        theL2CombinedAlgr1  = TrigmuCombConfig(L2AlgName, id_alg_output,"OLDextr")
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
                             [theL2StandAloneAlg],
                             'L2_mu_step1']]
    self.L2sequenceList += [['L2_mu_step1',
                             [theL2StandAloneHypo],
                             'L2_mu_hypo1']] 

    EFinputTE = ''

    if (self.doOvlpRm):
      self.L2sequenceList += [[['L2_mu_hypo1'], [self.theL2OvlpRmConfig_mufast ],'L2_step1a_wOvlpRm']]
      if "r1extr" in self.chainPart['addInfo']:
        self.L2sequenceList += [[['L2_step1a_wOvlpRm'], 
                                 trkfast+[theL2CombinedAlgr1],
                                 'L2_step1b_wOvlpRm']]
      else:
        self.L2sequenceList += [[['L2_step1a_wOvlpRm'], 
                                 trkfast+[theL2CombinedAlg],
                                 'L2_step1b_wOvlpRm']]

      self.L2sequenceList += [['L2_step1b_wOvlpRm',
                               [theL2CombinedHypo],
                               'L2_step1c_wOvlpRm']]
      if not "noMuCombOvlpRm" in self.chainPart['overlapRemoval']:
        self.L2sequenceList += [[['L2_step1c_wOvlpRm'], [ self.theL2OvlpRmConfig_mucomb ], 'L2_step2_wOvlpRm']]
        EFinputTE = 'L2_step2_wOvlpRm'
      else:
        EFinputTE = 'L2_step1c_wOvlpRm'

    else:
      if "r1extr" in self.chainPart['addInfo']:
        self.L2sequenceList += [[['L2_mu_hypo1'],
                                 trkfast+
                                 [theL2CombinedAlgr1],
                                 'L2_mu_step2']]
      else:
        self.L2sequenceList += [[['L2_mu_hypo1'],
                                 trkfast+
                                 [theL2CombinedAlg],
                                 'L2_mu_step2']]

      self.L2sequenceList += [['L2_mu_step2',
                               [theL2CombinedHypo],
                               'L2_mu_hypo2']]
      EFinputTE = 'L2_mu_hypo2'

      # Run also FTK tracking
    if "FTK" in self.chainPart['L2IDAlg']:
      from TrigInDetConf.TrigInDetFTKSequence import TrigInDetFTKSequence
      
      [ftktrkfast, ftktrkprec] = TrigInDetFTKSequence("Muon","muonIso",sequenceFlavour=["PT"]).getSequence()    
      
      self.L2sequenceList += [[['L2_mu_hypo2'],
                               ftktrkfast+ftktrkprec,
                               'L2_mu_step3']]
      from TrigMuonHypo.TrigMuonHypoConfig import MuisoHypoConfig
      
      theMuonFTKIsolationAlgo = TrigMuonEFTrackIsolationVarConfig("TrigMuonFTKTrackIsolationVar")
      
      theMuonFTKIsolationHypo = MuisoHypoConfig("TrigMuonFTKIsolationHypo")
      
      self.L2sequenceList += [[['L2_mu_step3'],
                               [theMuonFTKIsolationAlgo],
                               'L2_mu_step4']]
      self.L2sequenceList += [[['L2_mu_step4'],
                               [theMuonFTKIsolationHypo],
                               'L2_mu_hypo3']]


    self.EFsequenceList += [[[EFinputTE],
                             trkprec,
                             'EF_mu_step1']]

    self.EFsequenceList += [[['EF_mu_step1'],
                             [theEFAlg,theTrigMuonEFCombinerHypoConfig],
                             'EF_mu_step2']]

    if self.chainPart['flavour'] != '':
      self.EFsequenceList += [[['EF_mu_step2'],
                               [ hiHFMuonHypos[self.chainPart['flavour']] ],
                               'EF_mu_step_flavour']]
       
    if '10invm30' in self.chainPart['addInfo'] and 'pt2' in self.chainPart['addInfo'] and 'z10' in self.chainPart['addInfo']:
      theTrigMuonEFCombinerDiMuonMassPtImpactsHypoConfig = TrigMuonEFCombinerDiMuonMassPtImpactsHypoConfig("DiMuon","0")
      self.EFsequenceList += [[['EF_mu_step2'],
                               [theTrigMuonEFCombinerDiMuonMassPtImpactsHypoConfig],
                               'EF_mu_step3']]
    if 'invm1' in self.chainPart['addInfo']:
      theTrigMuonEFCombinerDiMuonMassPtImpactsHiggsinoHypoConfig = TrigMuonEFCombinerDiMuonMassPtImpactsHypoConfig("HiggsinoDiMu","0")
      theTrigMuonEFCombinerDiMuonMassPtImpactsHiggsinoHypoConfig.massThresLow = 1.0
      theTrigMuonEFCombinerDiMuonMassPtImpactsHiggsinoHypoConfig.massThresHigh = -1
      theTrigMuonEFCombinerDiMuonMassPtImpactsHiggsinoHypoConfig.pairptThresLow = -1
      theTrigMuonEFCombinerDiMuonMassPtImpactsHiggsinoHypoConfig.pairptThresHigh = -1
      theTrigMuonEFCombinerDiMuonMassPtImpactsHiggsinoHypoConfig.deltaZThres = -1
      theTrigMuonEFCombinerDiMuonMassPtImpactsHiggsinoHypoConfig.deltaPhiThresLow = -1
      theTrigMuonEFCombinerDiMuonMassPtImpactsHiggsinoHypoConfig.deltaPhiThresHigh = -1
      theTrigMuonEFCombinerDiMuonMassPtImpactsHiggsinoHypoConfig.AcceptAll = False
      self.EFsequenceList += [[['EF_mu_step2'],
                               [theTrigMuonEFCombinerDiMuonMassPtImpactsHiggsinoHypoConfig],
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
      elif self.chainPart['isoInfo'] == "ivartight":
        theTrigMuonEFTrackIsolationHypoConfig = TrigMuonEFTrackIsolationHypoConfig("Muon","RelEFOnlyVarTighterWide")
      elif self.chainPart['isoInfo'] == "ivarverytight":
        theTrigMuonEFTrackIsolationHypoConfig = TrigMuonEFTrackIsolationHypoConfig("Muon","RelEFOnlyVarVeryTightWide")
      elif self.chainPart['isoInfo'] == "icalo":
        theTrigMuonEFCaloIsolationHypoConfig = TrigMuonEFCaloIsolationHypoConfig("Muon", "Rel")
      elif self.chainPart['isoInfo'] == "iloosecalo":
        theTrigMuonEFTrackIsolationHypoConfig = TrigMuonEFTrackIsolationHypoConfig("Muon","RelEFOnlyMedium")
        theTrigMuonEFCaloIsolationHypoConfig = TrigMuonEFCaloIsolationHypoConfig("Muon", "Rel")
      elif self.chainPart['isoInfo'] == "imediumcalo":
        theTrigMuonEFTrackIsolationHypoConfig = TrigMuonEFTrackIsolationHypoConfig("Muon","RelEFOnlyTightWide")
        theTrigMuonEFCaloIsolationHypoConfig = TrigMuonEFCaloIsolationHypoConfig("Muon", "Rel")
      elif self.chainPart['isoInfo'] == "ivarloosecalo":
        theTrigMuonEFTrackIsolationHypoConfig = TrigMuonEFTrackIsolationHypoConfig("Muon","RelEFOnlyVarMedium")
        theTrigMuonEFCaloIsolationHypoConfig = TrigMuonEFCaloIsolationHypoConfig("Muon", "Rel")
      elif self.chainPart['isoInfo'] == "ivarmediumcalo":
        theTrigMuonEFTrackIsolationHypoConfig = TrigMuonEFTrackIsolationHypoConfig("Muon","RelEFOnlyVarTightWide")
        theTrigMuonEFCaloIsolationHypoConfig = TrigMuonEFCaloIsolationHypoConfig("Muon", "Rel")
      else:
        log.error("Isolation %s not yet supported." % (self.chainPart['isoInfo']))
        return False

      inputTrk='EF_mu_step2'
      if self.chainPart['isoInfo'] != "icalo":
        isoIDseqList = trkiso+trkprec
        if "zROItest" in self.chainPart['addInfo']:
          # before running the tracking, we update the RoI based on the muon with TrigMuonEFIDTrackRoiMaker
          theTrigMuonEFIDTrackRoiMaker = TrigMuonEFIDTrackRoiMakerConfig()
          self.EFsequenceList += [[['EF_mu_step2'],
                                   [theTrigMuonEFIDTrackRoiMaker],
                                   'EF_mu_roi']]
          inputTrk = 'EF_mu_roi'
        self.EFsequenceList += [[[inputTrk],
                                 isoIDseqList,
                                 'EF_mu_step3']]

      if self.chainPart['isoInfo'] == "iloose" or self.chainPart['isoInfo'] == "imedium" or self.chainPart['isoInfo'] == "iloosecalo" or self.chainPart['isoInfo'] == "imediumcalo":
        self.EFsequenceList += [[['EF_mu_step3'],
                                 [TrigMuonEFTrackIsolationConfig("TrigMuonEFTrackIsolation")],
                                 'EF_mu_step4']]
        self.EFsequenceList += [[['EF_mu_step4'],
                                 [theTrigMuonEFTrackIsolationHypoConfig],
                                 'EF_mu_hypo3']]
      elif self.chainPart['isoInfo'] == "ivarloose" or self.chainPart['isoInfo'] == "ivarmedium" or self.chainPart['isoInfo'] == "ivartight" or self.chainPart['isoInfo'] == "ivarverytight" or self.chainPart['isoInfo'] == "ivarloosecalo" or self.chainPart['isoInfo'] == "ivarmediumcalo":
        # we activate the track selection by the trigger name
        isoAlgoName = "TrigMuonEFTrackIsolationVar"
        if 'tsloose' in self.chainPart['addInfo']:
          isoAlgoName = isoAlgoName + 'LooseTSel'
        elif 'tstight' in self.chainPart['addInfo']:
          isoAlgoName = isoAlgoName + 'TightTSel'        
        self.EFsequenceList += [[['EF_mu_step3'],
                                 [TrigMuonEFTrackIsolationVarConfig(isoAlgoName)],
                                 'EF_mu_step4']]
        self.EFsequenceList += [[['EF_mu_step4'],
                                 [theTrigMuonEFTrackIsolationHypoConfig],
                                 'EF_mu_hypo3']]
      if 'calo' in self.chainPart['isoInfo']:

        # full scan topo and merging w/ RoI TEs
        te_in=''
        for step in self.fullScanSeqMap:
          self.EFsequenceList += [[[te_in],self.fullScanSeqMap[step],step]]
          te_in=step
        self.EFsequenceList += [[ ['EF_mu_step2',te_in],[self.DummyMergerAlgo],'EF_mu_step5']]
          

        self.EFsequenceList += [[['EF_mu_step5'],
                                 [ TrigMuonEFCaloIsolationConfig("TrigMuonEFCaloIsolation")],
                                 'EF_mu_step6']]
        self.EFsequenceList += [[['EF_mu_step6'],
                                 [theTrigMuonEFCaloIsolationHypoConfig ],
                                 'EF_mu_hypo4']]



    #--- adding signatures ----
    self.L2signatureList += [ [['L2_mu_step1']*self.mult] ]
    self.L2signatureList += [ [['L2_mu_hypo1']*self.mult] ]
    if (self.doOvlpRm):
      self.L2signatureList += [ [['L2_step1a_wOvlpRm']*self.mult] ]
      self.L2signatureList += [ [['L2_step1b_wOvlpRm']*self.mult] ]
      self.L2signatureList += [ [['L2_step1c_wOvlpRm']*self.mult] ]
      if not "noMuCombOvlpRm" in self.chainPart['overlapRemoval']:
        self.L2signatureList += [ [['L2_step2_wOvlpRm']*self.mult] ]
    else:
      self.L2signatureList += [ [['L2_mu_step2']*self.mult] ]
      self.L2signatureList += [ [['L2_mu_hypo2']*self.mult] ]
     
    if "FTK" in self.chainPart['L2IDAlg']:
      self.L2signatureList += [ [['L2_mu_step3']*self.mult] ]
      self.L2signatureList += [ [['L2_mu_step4']*self.mult] ] 
      self.L2signatureList += [ [['L2_mu_hypo3']*self.mult] ] 

    self.EFsignatureList += [ [['EF_mu_step1']*self.mult] ]
    self.EFsignatureList += [ [['EF_mu_step2']*self.mult] ]

    if '10invm30' in self.chainPart['addInfo'] and 'pt2' in self.chainPart['addInfo'] and 'z10' in self.chainPart['addInfo']:
      self.EFsignatureList += [ [['EF_mu_step3']] ]

    if 'invm1' in self.chainPart['addInfo']:
      self.EFsignatureList += [ [['EF_mu_step3']] ]

    if self.chainPart['isoInfo']:# == "iloose" or self.chainPart['isoInfo'] == "imedium":
      if self.chainPart['isoInfo'] != "icalo":
        if "zROItest" in self.chainPart['addInfo']:
          self.EFsignatureList += [ [['EF_mu_roi']*self.mult] ]
        self.EFsignatureList += [ [['EF_mu_step3']*self.mult] ]
        self.EFsignatureList += [ [['EF_mu_step4']*self.mult] ]
        self.EFsignatureList += [ [['EF_mu_hypo3']*self.mult] ]
      if 'calo' in self.chainPart['isoInfo']:
        for step in self.fullScanSeqMap:
          self.EFsignatureList += [ [[step]] ]
        self.EFsignatureList += [ [['EF_mu_step5']*self.mult] ]
        self.EFsignatureList += [ [['EF_mu_step6']*self.mult] ]
        self.EFsignatureList += [ [['EF_mu_hypo4']*self.mult] ]


    #--- renaming TEs ---
    self.TErenamingDict = {
      'L2_mu_step1': mergeRemovingOverlap('L2_mu_SA_', L2AlgName+muFastThresh+'_'+self.L2InputTE),
      'L2_mu_hypo1': mergeRemovingOverlap('L2_mu_SAhyp_', L2AlgName+muFastThresh+'_'+self.L2InputTE),
      'L2_mu_step2': mergeRemovingOverlap('L2_mucomb_',  self.chainPartNameNoMult.replace('_'+self.chainPart['isoInfo'], '').replace(self.chainPart['specialStream'], '')+'_'+self.L2InputTE),
      'L2_mu_hypo2': mergeRemovingOverlap('L2_mucombhyp_',  self.chainPartNameNoMult.replace('_'+self.chainPart['isoInfo'], '').replace(self.chainPart['specialStream'], '')+'_'+self.L2InputTE),
      'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', self.chainPartNameNoMult+'_'+self.L2InputTE),
      'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   self.chainPartNameNoMult+'_'+self.L2InputTE),
      }    
    if "FTK" in self.chainPart['L2IDAlg']:
      self.TErenamingDict.update({'L2_mu_step3': mergeRemovingOverlap('EF_ftkfex_',self.chainPartNameNoMult+'_'+self.L2InputTE),
                                  'L2_mu_step4': mergeRemovingOverlap('EF_ftkiso_',self.chainPartNameNoMult+'_'+self.L2InputTE),
                                  'L2_mu_hypo3': mergeRemovingOverlap('EF_ftkhypo_',self.chainPartNameNoMult+'_'+self.L2InputTE),
                                  })
      
    if (("ds1" in self.chainPart['addInfo'])):
      chainPartNameNoMultNoDS = self.chainPartNameNoMult.replace('_ds1', '')
    elif (("ds2" in self.chainPart['addInfo'])):
      chainPartNameNoMultNoDS = self.chainPartNameNoMult.replace('_ds2', '')
    elif ("ds3" in self.chainPart['addInfo']): 
      chainPartNameNoMultNoDS = self.chainPartNameNoMult.replace('_ds3', '') 
    else:
      chainPartNameNoMultNoDS = self.chainPartNameNoMult

    if 'llns' in self.chainPart['addInfo']:
      self.TErenamingDict.update({'L2_mu_step2'   : mergeRemovingOverlap('L2_mucomb_',  L2AlgName+muCombThresh+'_'+self.L2InputTE+'_llns'),
                                  'L2_mu_hypo2'   : mergeRemovingOverlap('L2_mucombhyp_',  L2AlgName+muCombThresh+'_'+self.L2InputTE+'_llns')}) 
    if '10invm30' in self.chainPart['addInfo'] and 'pt2' in self.chainPart['addInfo'] and 'z10' in self.chainPart['addInfo']:
      self.TErenamingDict.update({'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', chainPartNameNoMultNoDS.replace('_'+self.chainPart['isoInfo'],'')),
                                  'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   chainPartNameNoMultNoDS.replace('_'+self.chainPart['isoInfo'],'')),
                                  'EF_mu_step3': mergeRemovingOverlap('EF_invm_',    chainPartNameNoMultNoDS)})

    if 'invm1' in self.chainPart['addInfo']:
      self.TErenamingDict.update({'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', chainPartNameNoMultNoDS.replace('_'+self.chainPart['isoInfo'],'')),
                                  'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   chainPartNameNoMultNoDS.replace('_'+self.chainPart['isoInfo'],'')),
                                  'EF_mu_step3': mergeRemovingOverlap('EF_invm_',    chainPartNameNoMultNoDS)})

    if (self.chainPart['isoInfo']) and ( self.chainPart['isoInfo'] not in ["icalo","iloosecalo","imediumcalo","ivarmediumcalo","ivarloosecalo"] ):

      self.TErenamingDict.update({'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', chainPartNameNoMultNoDS.replace('_'+self.chainPart['isoInfo'],'')+'_'+self.L2InputTE),
                                  'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   chainPartNameNoMultNoDS.replace('_'+self.chainPart['isoInfo'],'')+'_'+self.L2InputTE),
                                  'EF_mu_roi': mergeRemovingOverlap('EF_SuperEFRoI_',   chainPartNameNoMultNoDS+'_'+self.L2InputTE),
                                  'EF_mu_step3': mergeRemovingOverlap('EF_muI_efid_',    chainPartNameNoMultNoDS+'_'+self.L2InputTE),
                                  'EF_mu_step4': mergeRemovingOverlap('EF_trkIso_',       chainPartNameNoMultNoDS+'_'+self.L2InputTE),
                                  'EF_mu_hypo3': mergeRemovingOverlap('EF_trkIsohyp_',       chainPartNameNoMultNoDS+'_'+self.L2InputTE)})
    if (self.chainPart['isoInfo']) and ( self.chainPart['isoInfo'] == "icalo" ):

      self.TErenamingDict.update({'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', self.chainPartNameNoMult.replace('_'+self.chainPart['isoInfo'],'')),
                                  'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   self.chainPartNameNoMult.replace('_'+self.chainPart['isoInfo'],'')),
                                  'EF_full':mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_fs'),
                                  'EF_full_cell':mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_fscalocell'),
                                  'EF_FSTopoClusters':mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_fscalotopo'),
                                  'EF_FSTopoClustersED':mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_fscalotopoed'),
                                  'EF_mu_step5': mergeRemovingOverlap('EF_fullcalo_',    self.chainPartNameNoMult),
                                  'EF_mu_step6': mergeRemovingOverlap('EF_caloIso_',       self.chainPartNameNoMult),
                                  'EF_mu_hypo4': mergeRemovingOverlap('EF_caloIsohyp_',       self.chainPartNameNoMult)})
    if (self.chainPart['isoInfo']) and ( self.chainPart['isoInfo'] == "iloosecalo" or self.chainPart['isoInfo'] == "imediumcalo" or self.chainPart['isoInfo'] == "ivarloosecalo" or self.chainPart['isoInfo'] == "ivarmediumcalo"):
 
      self.TErenamingDict.update({'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', self.chainPartNameNoMult.replace('_'+self.chainPart['isoInfo'],'')),
                                  'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   self.chainPartNameNoMult.replace('_'+self.chainPart['isoInfo'],'')),
                                  'EF_mu_roi': mergeRemovingOverlap('EF_SuperEFRoI_',   self.chainPartNameNoMult),
                                  'EF_mu_step3': mergeRemovingOverlap('EF_muI_efid_',    chainPartNameNoMultNoDS),
                                  'EF_mu_step4': mergeRemovingOverlap('EF_trkIso_',       chainPartNameNoMultNoDS),
                                  'EF_mu_hypo3': mergeRemovingOverlap('EF_trkIsohyp_',       chainPartNameNoMultNoDS),
                                  'EF_full':mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_fs'),
                                  'EF_full_cell':mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_fscalocell'),
                                  'EF_FSTopoClusters':mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_fscalotopo'),
                                  'EF_FSTopoClustersED':mergeRemovingOverlap('EF_', self.chainPartNameNoMult+'_fscalotopoed'),
                                  'EF_mu_step5': mergeRemovingOverlap('EF_fullcalo_',    self.chainPartNameNoMult),
                                  'EF_mu_step6': mergeRemovingOverlap('EF_caloIso_',       self.chainPartNameNoMult),
                                  'EF_mu_hypo4': mergeRemovingOverlap('EF_caloIsohyp_',       self.chainPartNameNoMult)})


    if self.doOvlpRm:
      if not "noMuCombOvlpRm" in self.chainPart['overlapRemoval']:
        self.TErenamingDict.update({'L2_step1a_wOvlpRm'  : mergeRemovingOverlap('L2_mu_SAOvlpRm_',    L2AlgName+muFastThresh+'_'+self.L2InputTE+'_wOvlpRm' ),
                                    'L2_step1b_wOvlpRm'  : mergeRemovingOverlap('L2_muon_comb',       L2AlgName+muCombThresh+'_'+self.L2InputTE+'_wOvlpRm' ),
                                    'L2_step1c_wOvlpRm'  : mergeRemovingOverlap('L2_muon_combhyp',       L2AlgName+muCombThresh+'_'+self.L2InputTE+'_wOvlpRm' ),
                                    'L2_step2_wOvlpRm'   : mergeRemovingOverlap('L2_mu_combOvlpRm_',  L2AlgName+muCombThresh+'_'+self.L2InputTE+'_wOvlpRm'),
                                    'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', chainPartNameNoMultNoDS+'_wOvlpRm'),
                                    'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   chainPartNameNoMultNoDS+'_wOvlpRm')})

      else:
        self.TErenamingDict.update({'L2_step1a_wOvlpRm'  : mergeRemovingOverlap('L2_mu_SAOvlpRm_',    L2AlgName+muFastThresh+'_'+self.L2InputTE+'_wOvlpRm' ),
                                    'L2_step1b_wOvlpRm'  : mergeRemovingOverlap('L2_muon_comb',       L2AlgName+muCombThresh+'_'+self.L2InputTE+'_wOvlpRm' ),
                                    'L2_step1c_wOvlpRm'  : mergeRemovingOverlap('L2_muon_combhyp',       L2AlgName+muCombThresh+'_'+self.L2InputTE+'_wOvlpRm' ),
                                    'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', chainPartNameNoMultNoDS+'_wOvlpRm'),
                                    'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   chainPartNameNoMultNoDS+'_wOvlpRm')})

    if self.doOvlpRm and self.chainPart['isoInfo']:
      self.TErenamingDict.update({'L2_step1a_wOvlpRm'  : mergeRemovingOverlap('L2_mu_SAOvlpRm_',    L2AlgName+muFastThresh+'_'+self.L2InputTE+'_wOvlpRm' ),
                                  'L2_step1b_wOvlpRm'  : mergeRemovingOverlap('L2_muon_comb',       L2AlgName+muCombThresh+'_'+self.L2InputTE+'_wOvlpRm' ),
                                  'L2_step1c_wOvlpRm'  : mergeRemovingOverlap('L2_muon_combhyp',       L2AlgName+muCombThresh+'_'+self.L2InputTE+'_wOvlpRm' ),
                                  'L2_step2_wOvlpRm'   : mergeRemovingOverlap('L2_mu_combOvlpRm_',  L2AlgName+muCombThresh+'_'+self.L2InputTE+'_wOvlpRm'),
                                  'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', chainPartNameNoMultNoDS+'_wOvlpRm'),
                                  'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   chainPartNameNoMultNoDS+'_wOvlpRm'),
                                  'EF_mu_roi': mergeRemovingOverlap('EF_SuperEFRoI_',   chainPartNameNoMultNoDS+'_wOvlpRm'),
                                  'EF_mu_step3': mergeRemovingOverlap('EF_muI_efid_',    chainPartNameNoMultNoDS+'_wOvlpRm'),
                                  'EF_mu_step4': mergeRemovingOverlap('EF_trkIso_',       chainPartNameNoMultNoDS+'_wOvlpRm'),
                                  'EF_mu_hypo3': mergeRemovingOverlap('EF_trkIsohyp_',       chainPartNameNoMultNoDS+'_wOvlpRm')}) 

    if self.doOvlpRm and '10invm30' in self.chainPart['addInfo'] and 'pt2' in self.chainPart['addInfo'] and 'z10' in self.chainPart['addInfo']:
      self.TErenamingDict.update({'L2_step1a_wOvlpRm'  : mergeRemovingOverlap('L2_mu_SAOvlpRm_',    L2AlgName+muFastThresh+'_'+self.L2InputTE+'_wOvlpRm' ),
                                  'L2_step1b_wOvlpRm'  : mergeRemovingOverlap('L2_muon_comb',       L2AlgName+muCombThresh+'_'+self.L2InputTE+'_wOvlpRm' ),
                                  'L2_step1c_wOvlpRm'  : mergeRemovingOverlap('L2_muon_combhyp',       L2AlgName+muCombThresh+'_'+self.L2InputTE+'_wOvlpRm' ),
                                  'L2_step2_wOvlpRm'   : mergeRemovingOverlap('L2_mu_combOvlpRm_',  L2AlgName+muCombThresh+'_'+self.L2InputTE+'_wOvlpRm'),
                                  'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', chainPartNameNoMultNoDS+'_wOvlpRm'),
                                  'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   chainPartNameNoMultNoDS+'_wOvlpRm'),
                                  'EF_mu_step3': mergeRemovingOverlap('EF_invm_',    chainPartNameNoMultNoDS+'_wOvlpRm')}) 
    if self.doOvlpRm and 'invm1' in self.chainPart['addInfo']:
      self.TErenamingDict.update({'L2_step1a_wOvlpRm'  : mergeRemovingOverlap('L2_mu_SAOvlpRm_',    L2AlgName+muFastThresh+'_'+self.L2InputTE+'_wOvlpRm' ),
                                  'L2_step1b_wOvlpRm'  : mergeRemovingOverlap('L2_muon_comb',       L2AlgName+muCombThresh+'_'+self.L2InputTE+'_wOvlpRm' ),
                                  'L2_step1c_wOvlpRm'  : mergeRemovingOverlap('L2_muon_combhyp',       L2AlgName+muCombThresh+'_'+self.L2InputTE+'_wOvlpRm' ),
                                  'L2_step2_wOvlpRm'   : mergeRemovingOverlap('L2_mu_combOvlpRm_',  L2AlgName+muCombThresh+'_'+self.L2InputTE+'_wOvlpRm'),
                                  'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', chainPartNameNoMultNoDS+'_wOvlpRm'),
                                  'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   chainPartNameNoMultNoDS+'_wOvlpRm'),
                                  'EF_mu_step3': mergeRemovingOverlap('EF_invm_',    chainPartNameNoMultNoDS+'_wOvlpRm')}) 
     
    if self.chainPart['flavour']:
      self.TErenamingDict.update({'EF_mu_step_flavour': mergeRemovingOverlap('EF_',    self.chainPartNameNoMult)})


                                      
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

    [trkfast, trkprec, trkiso] = TrigInDetSequence("Muon", "muon", "IDTrig", sequenceFlavour=["2step"]).getSequence()
    
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
                             [theL2StandAloneAlg],
                             'L2_mu_step1']]
    self.L2sequenceList += [['L2_mu_step1',
                             [theL2StandAloneHypo],
                             'L2_mu_hypo1']] 


    self.L2sequenceList += [[['L2_mu_step1'],
    			      trkfast+
    			     [theL2CombinedAlg],
                             'L2_mu_step2']]
    self.L2sequenceList += [['L2_mu_step2',
                             [theL2CombinedHypo],
    			     'L2_mu_hypo2']]
			     
    #--- adding signatures ----
    self.L2signatureList += [ [['L2_mu_step1']*self.mult] ]
    self.L2signatureList += [ [['L2_mu_hypo1']*self.mult] ]
    self.L2signatureList += [ [['L2_mu_step2']*self.mult] ]
    self.L2signatureList += [ [['L2_mu_hypo2']*self.mult] ]
      
    #--- renaming TEs ---
    self.TErenamingDict = {
      'L2_mu_step1': mergeRemovingOverlap('L2_mu_SA_', L2AlgName+muFastThresh+'_'+self.L2InputTE),
      'L2_mu_hypo1': mergeRemovingOverlap('L2_mu_SAhyp_', L2AlgName+muFastThresh+'_'+self.L2InputTE),
      'L2_mu_step2': mergeRemovingOverlap('L2_mucomb_',   self.chainPartNameNoMult.replace('_'+self.chainPart['isoInfo'], '').replace(self.chainPart['specialStream'], '')+'_'+self.L2InputTE),
      'L2_mu_hypo2': mergeRemovingOverlap('L2_mucombhyp_',   self.chainPartNameNoMult.replace('_'+self.chainPart['isoInfo'], '').replace(self.chainPart['specialStream'], '')+'_'+self.L2InputTE),
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
                             [theL2StandAloneAlg],
                             'L2_mu_step1']]
    self.L2sequenceList += [['L2_mu_step1',
                             [theL2StandAloneHypo],
                             'L2_mu_hypo1']] 


    self.L2sequenceList += [[['L2_mu_step1'],
    			      trkfast+
    			     [theL2CombinedAlg],
                             'L2_mu_step2']]
    self.L2sequenceList += [['L2_mu_step2',
    			      [theL2CombinedHypo],
    			     'L2_mu_hypo2']]
			     
    self.L2sequenceList += [[['L2_mu_step2']*self.mult,
                             [theL2JpsimumuAlgo,theL2JpsimumuHypo],
                             'L2_mu_step3']]

    #--- adding signatures ----
    self.L2signatureList += [ [['L2_mu_step1']*self.mult] ]
    self.L2signatureList += [ [['L2_mu_hypo1']*self.mult] ]
    self.L2signatureList += [ [['L2_mu_step2']*self.mult] ]
    self.L2signatureList += [ [['L2_mu_hypo2']*self.mult] ]
    self.L2signatureList += [ [['L2_mu_step3']] ]
      
    #--- renaming TEs ---
    self.TErenamingDict = {
      'L2_mu_step1': mergeRemovingOverlap('L2_mu_SA_', L2AlgName+muFastThresh+'_'+self.L2InputTE),
      'L2_mu_hypo1': mergeRemovingOverlap('L2_mu_SAhyp_', L2AlgName+muFastThresh+'_'+self.L2InputTE),
      'L2_mu_step2': mergeRemovingOverlap('L2_mucomb_',   self.chainPartNameNoMult.replace('_'+self.chainPart['isoInfo'], '').replace(self.chainPart['specialStream'], '')+'_'+self.L2InputTE),
      'L2_mu_hypo2': mergeRemovingOverlap('L2_mucombhyp_',   self.chainPartNameNoMult.replace('_'+self.chainPart['isoInfo'], '').replace(self.chainPart['specialStream'], '')+'_'+self.L2InputTE),
      'L2_mu_step3': mergeRemovingOverlap('L2_JPsimumu_',   self.chainPartNameNoMult.replace('_'+self.chainPart['isoInfo'], '')+'_'+self.L2InputTE),
      }    

                                      
  #################################################################################################
  #################################################################################################

  def setup_muXX_idperf(self):

    L2AlgName = self.getL2AlgName()
    muFastThresh = self.getMuFastThresh()
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


    ########### Sequence List ##############

    self.L2sequenceList += [[self.L2InputTE,
                             [theL2StandAloneAlg],
                             'L2_mu_step1']]
    self.L2sequenceList += [['L2_mu_step1',
                             [theL2StandAloneHypo],
                             'L2_mu_hypo1']] 

    EFinputTE = ''

    if "L2Star" in self.chainPart['L2IDAlg']:                             # ---> this is Run1 tracking - keep it here
      self.L2sequenceList += [[['L2_mu_step1'],
                               [theTrigL2SiTrackFinder_MuonA, 
                                theTrigL2SiTrackFinder_MuonB,
                                theTrigL2SiTrackFinder_MuonC, 
                                theL2CombinedAlg],
                               'L2_mu_step2']]
      self.L2sequenceList += [[['L2_mu_step2'],
                               [theL2CombinedHypo],
                               'L2_mu_hypo2']]
    else:
      self.L2sequenceList += [[['L2_mu_step1'],
                               trkfast+
                               [theL2CombinedAlg],
                               'L2_mu_step2']]
      self.L2sequenceList += [[['L2_mu_step2'],
                               [theL2CombinedHypo],
                                'L2_mu_hypo2']]

    EFinputTE = 'L2_mu_step2'

      
    if "L2Star" in self.chainPart['L2IDAlg']:                             # ---> this is Run1 tracking - keep it here
      self.EFsequenceList += [[[EFinputTE],
                                theTrigEFIDInsideOut_Muon.getSequence(),
                               'EF_mu_step1']]
    else:
      self.EFsequenceList += [[[EFinputTE],
                               trkprec,
                               'EF_mu_step1']]


    self.EFsequenceList += [[['EF_mu_step1'],
                             [theEFAlg],
                             'EF_mu_step2']]
    self.EFsequenceList += [[['EF_mu_step2'],
    	  		     [theTrigMuonEFCombinerHypoConfig],
    	  		      'EF_mu_hypo1']]

    ########### Signatures ###########
      
    self.L2signatureList += [ [['L2_mu_step1']*self.mult] ]
    self.L2signatureList += [ [['L2_mu_hypo1']*self.mult] ]
    self.L2signatureList += [ [['L2_mu_step2']*self.mult] ]
    self.L2signatureList += [ [['L2_mu_hypo2']*self.mult] ]
      
    self.EFsignatureList += [ [['EF_mu_step1']*self.mult] ]
    self.EFsignatureList += [ [['EF_mu_step2']*self.mult] ]
    self.EFsignatureList += [ [['EF_mu_hypo1']*self.mult] ]

    ########### TE renaming ##########

    self.TErenamingDict = {
      'L2_mu_step1': mergeRemovingOverlap('L2_mu_SA_',  L2AlgName+muFastThresh+'_'+self.L2InputTE),
      'L2_mu_hypo1': mergeRemovingOverlap('L2_mu_SAhyp_',  L2AlgName+muFastThresh+'_'+self.L2InputTE),
      'L2_mu_step2': mergeRemovingOverlap('L2_mucomb_', self.chainPartNameNoMult.replace('_'+self.chainPart['isoInfo'], '')+'_'+self.L2InputTE),
      'L2_mu_hypo2': mergeRemovingOverlap('L2_mucombhyp_', self.chainPartNameNoMult.replace('_'+self.chainPart['isoInfo'], '')+'_'+self.L2InputTE),
      'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOutMerged_', self.chainPartNameNoMult),
      'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   self.chainPartNameNoMult),
      'EF_mu_hypo1': mergeRemovingOverlap('EF_SuperEFhyp_',   self.chainPartNameNoMult),
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
                             [theL2StandAloneAlg],
                             'L2_mu_step1']]
    self.L2sequenceList += [['L2_mu_step1',
                             [theL2StandAloneHypo],
                             'L2_mu_hypo1']] 

    EFinputTE = ''

    if (self.doOvlpRm):
      self.L2sequenceList += [[['L2_mu_hypo1'], [self.theL2OvlpRmConfig_mufast ],'L2_mu_step1_wOvlpRm']]
      EFinputTE = 'L2_mu_step1_wOvlpRm'
    else:
      EFinputTE = 'L2_mu_step1'

    self.EFsequenceList += [[[EFinputTE],
                              trkfast+trkprec,
                              'EF_mu_step1']]

    self.EFsequenceList += [[['EF_mu_step1'],
    	  		       [theEFAlg],
                             'EF_mu_step2']]
    self.EFsequenceList += [[['EF_mu_step2'],
                             [theTrigMuonEFCombinerHypoConfig],
                             'EF_mu_hypo1']]
       
    #--- adding signatures ----
    self.L2signatureList += [ [['L2_mu_step1']*self.mult] ]
    self.L2signatureList += [ [['L2_mu_hypo1']*self.mult] ]
    if (self.doOvlpRm):
      self.L2signatureList += [ [['L2_mu_step1_wOvlpRm']*self.mult] ]
      
    self.EFsignatureList += [ [['EF_mu_step1']*self.mult] ]
    self.EFsignatureList += [ [['EF_mu_step2']*self.mult] ]
    self.EFsignatureList += [ [['EF_mu_hypo1']*self.mult] ]

    #--- renaming TEs ---
    self.TErenamingDict = {
      'L2_mu_step1': mergeRemovingOverlap('L2_mu_SA_', L2AlgName+muFastThresh+'_'+self.L2InputTE),
      'L2_mu_hypo1': mergeRemovingOverlap('L2_mu_SAhyp_', L2AlgName+muFastThresh+'_'+self.L2InputTE),
      'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', self.chainPartNameNoMult+'_'+self.L2InputTE),
      'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   self.chainPartNameNoMult+'_'+self.L2InputTE),
      'EF_mu_hypo1': mergeRemovingOverlap('EF_SuperEFhyp_',   self.chainPartNameNoMult+'_'+self.L2InputTE),
      }    

    if self.doOvlpRm:
      self.TErenamingDict.update({'L2_mu_step1_wOvlpRm'  : mergeRemovingOverlap('L2_mu_SAOvlpRm_',    L2AlgName+muFastThresh+'_'+self.L2InputTE+'_wOvlpRm' ),
                                  'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', self.chainPartNameNoMult+'_wOvlpRm'),
                                  'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   self.chainPartNameNoMult+'_wOvlpRm'),
                                  'EF_mu_hypo1': mergeRemovingOverlap('EF_SuperEFhyp_',   self.chainPartNameNoMult+'_wOvlpRm')})                                
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
    [trkfast, trkiso, trkprec] = TrigInDetSequence("Muon", "muon", "IDTrig", sequenceFlavour=["2step"]).getSequence()
    if "wOvlpRm" in self.chainPart['overlapRemoval']:
      from TrigMuonHypo.TrigEFMuonOverlapRemoverConfig import TrigEFMuonOverlapRemoverConfig
      theEFOvlpRmConfig = TrigEFMuonOverlapRemoverConfig('MuExtr','loose')
    if self.chainPart['isoInfo'] == "iloosems":
      theTrigMuonEFTrackIsolationHypoConfig = TrigMuonEFTrackIsolationHypoConfig("Muon","MSEFOnlyLooseWide")
    ########### Sequence List ##############
    self.L2sequenceList += [[self.L2InputTE,
                             [theL2StandAloneAlg],
                             'L2_mu_step1']]
    self.L2sequenceList += [['L2_mu_step1',
                             [theL2StandAloneHypo],
                             'L2_mu_hypo1']]
    EFinputTE = ''
    if (self.doOvlpRm):
      self.L2sequenceList += [[['L2_mu_step1'], [self.theL2OvlpRmConfig_mufast ],'L2_mu_step1_wOvlpRm']]
      EFinputTE = 'L2_mu_step1_wOvlpRm'
    else:
      EFinputTE = 'L2_mu_step1'
    self.EFsequenceList += [[[EFinputTE],
    	  		       [theEFAlg],
                             'EF_mu_step1']]
    self.EFsequenceList += [[['EF_mu_step1'],
                             [theTrigMuonEFExtrapolatorHypoConfig],
    	  		      'EF_mu_hypo1']]
    if (self.doOvlpRm):
      self.EFsequenceList += [[['EF_mu_hypo1'], [theEFOvlpRmConfig ],'EF_mu_step1_wOvlpRm']]
    if self.chainPart['isoInfo'] == "iloosems":
        self.EFsequenceList += [[['EF_mu_step1'],
                                 trkfast+trkiso+trkprec,
                                 'EF_mu_step2']]
        self.EFsequenceList += [[['EF_mu_step2'],
                                 [TrigMuonEFMSTrackIsolationConfig("TrigMuonEFMSTrackIsolation"),theTrigMuonEFTrackIsolationHypoConfig],
                	         'EF_mu_step3']]
    ########### Signatures ###########
      
    self.L2signatureList += [ [['L2_mu_step1']*self.mult] ]
    self.L2signatureList += [ [['L2_mu_hypo1']*self.mult] ]
    if (self.doOvlpRm):
      self.L2signatureList += [ [['L2_mu_step1_wOvlpRm']*self.mult] ]
    self.EFsignatureList += [ [['EF_mu_step1']*self.mult] ]
    self.EFsignatureList += [ [['EF_mu_hypo1']*self.mult] ]
    if (self.doOvlpRm):
      self.EFsignatureList += [ [['EF_mu_step1_wOvlpRm']*self.mult] ]
    if self.chainPart['isoInfo'] == "iloosems":
      self.EFsignatureList += [ [['EF_mu_step2']*self.mult] ]
      self.EFsignatureList += [ [['EF_mu_step3']*self.mult] ]

    ########### TE renaming ##########

    self.TErenamingDict = {
      'L2_mu_step1': mergeRemovingOverlap('L2_mu_SA_', L2AlgName+muFastThresh+'_'+self.L2InputTE),
      'L2_mu_hypo1': mergeRemovingOverlap('L2_mu_SAhyp_', L2AlgName+muFastThresh+'_'+self.L2InputTE),
      'EF_mu_step1': mergeRemovingOverlap('EF_SuperEF_',   self.chainPartNameNoMult+'_'+self.L2InputTE),
      'EF_mu_hypo1': mergeRemovingOverlap('EF_SuperEFhyp_',   self.chainPartNameNoMult+'_'+self.L2InputTE),
      }
    if (self.chainPart['isoInfo']):
      self.TErenamingDict.update({'EF_mu_step2': mergeRemovingOverlap('EF_EFIDInsideOut_', self.chainPartNameNoMult.replace('_'+self.chainPart['isoInfo'],'')+'_'+self.L2InputTE),
                                  'EF_mu_step3': mergeRemovingOverlap('EF_trkIso_',       self.chainPartNameNoMult+'_'+self.L2InputTE)})


    if self.doOvlpRm:
      self.TErenamingDict.update({'L2_mu_step1_wOvlpRm'  : mergeRemovingOverlap('L2_mu_SAOvlpRm_',    L2AlgName+muFastThresh+'_'+self.L2InputTE+'_wOvlpRm'+'_'+self.L2InputTE ),
                                  'EF_mu_step1': mergeRemovingOverlap('EF_SuperEF_', self.chainPartNameNoMult+'_wOvlpRm'+'_'+self.L2InputTE),
                                  'EF_mu_hypo1': mergeRemovingOverlap('EF_SuperEFhyp_', self.chainPartNameNoMult+'_wOvlpRm'+'_'+self.L2InputTE),
				  'EF_mu_step1_wOvlpRm': mergeRemovingOverlap('EF_SuperEFOvlpRm_', self.chainPartNameNoMult+'_wEFOvlpRm'+'_'+self.L2InputTE)})
                                          
  #################################################################################################
  #################################################################################################
  def setup_muXX_noL1(self):

    ########### EF algos  #################

    from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo

    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFExtrapolatorMultiHypoConfig, TrigMuonEFExtrapolatorHypoConfig

    if "dRl1" in self.chainPart['addInfo']:
      Nmuons=len(self.allMuThrs)-1
    else:
      Nmuons=len(self.allMuThrs)
    ##Use list of muon threshold in the chain to correctly configure the FS hypos
    
    if "JpsimumuFS" in self.chainPart['FSinfo']:
      theTrigMuonEFSA_FS_Hypo = TrigMuonEFExtrapolatorMultiHypoConfig('Muon','0GeV','0GeV')
      hypocut='0GeV_0GeV'    
    else:
      if Nmuons == 0:
        log.error("The list of allMuonThreshold is empty for a noL1 chain! It should never happen")

      if Nmuons == 1:
    	  theTrigMuonEFSA_FS_Hypo = TrigMuonEFExtrapolatorHypoConfig('Muon', '0GeV')
    	  hypocut = '0GeV'

      elif Nmuons == 2:
    	  theTrigMuonEFSA_FS_Hypo = TrigMuonEFExtrapolatorMultiHypoConfig('Muon', '0GeV','0GeV')
    	  hypocut = '0GeV_0GeV'

      elif Nmuons == 3:
    	  theTrigMuonEFSA_FS_Hypo = TrigMuonEFExtrapolatorMultiHypoConfig('Muon', '0GeV','0GeV','0GeV')
    	  hypocut = '0GeV_0GeV_0GeV'
      elif Nmuons == 4:
         theTrigMuonEFSA_FS_Hypo = TrigMuonEFExtrapolatorMultiHypoConfig('Muon', '0GeV','0GeV','0GeV','0GeV')
         hypocut = '0GeV_0GeV_0GeV_0GeV'
      else:
        log.error("No MuonEFExtrapolatorHypo config yet for events with more than 4 muons")

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
      elif len(self.allMuThrs) == 4:
        theTrigMuonEFCombinerMultiHypoConfig = TrigMuonEFCombinerMultiHypoConfig('Muon',self.allMuThrs[0],self.allMuThrs[1],self.allMuThrs[2],self.allMuThrs[3])                                                                                                           
      else:
        log.error("No TrigMuonEFCombinerHypo config yet for events with more than 4 muons")
 
      hypocutEF="MultiComb"     
      for i in range(0,len(self.allMuThrs)):        
        hypocutEF +=  "_%s" %(self.allMuThrs[i])
    run_isolation = False
    if self.chainPart['isoInfo'] == "ivarloose" or self.chainPart['isoInfo'] == "ivarmedium" or self.chainPart['isoInfo'] == "ivartight" or self.chainPart['isoInfo'] == "ivarverytight" or self.chainPart['isoInfo'] == "ivarloosecalo" or self.chainPart['isoInfo'] == "ivarmediumcalo":
      run_isolation = True
      [_, trkiso, _] = TrigInDetSequence("Muon", "muon", "IDTrig", sequenceFlavour=["2step"]).getSequence()
      if self.chainPart['isoInfo'] == "iloose":
        theTrigMuonEFTrackIsolationMultiHypoConfig = TrigMuonEFTrackIsolationMultiHypoConfig("Muon","RelEFOnlyMedium", self.allMuThrs)
      elif self.chainPart['isoInfo'] == "imedium":
        theTrigMuonEFTrackIsolationMultiHypoConfig = TrigMuonEFTrackIsolationMultiHypoConfig("Muon","RelEFOnlyTightWide")
      elif self.chainPart['isoInfo'] == "ivarloose":
        theTrigMuonEFTrackIsolationMultiHypoConfig = TrigMuonEFTrackIsolationMultiHypoConfig("Muon","RelEFOnlyVarMedium")
      elif self.chainPart['isoInfo'] == "ivarmedium":
        theTrigMuonEFTrackIsolationMultiHypoConfig = TrigMuonEFTrackIsolationMultiHypoConfig("Muon","RelEFOnlyVarTightWide", self.allMuThrs)
      elif self.chainPart['isoInfo'] == "ivartight":
        theTrigMuonEFTrackIsolationMultiHypoConfig = TrigMuonEFTrackIsolationMultiHypoConfig("Muon","RelEFOnlyVarTighterWide")
      elif self.chainPart['isoInfo'] == "ivarverytight":
        theTrigMuonEFTrackIsolationMultiHypoConfig = TrigMuonEFTrackIsolationMultiHypoConfig("Muon","RelEFOnlyVarVeryTightWide")
      else:
        log.error("Isolation %s not yet supported." % (self.chainPart['isoInfo']))
        return False
      if (("ds1" in self.chainPart['addInfo'])):
        chainPartNameNoMultNoDS = self.chainPartNameNoMult.replace('_ds1', '')
      elif (("ds2" in self.chainPart['addInfo'])):
        chainPartNameNoMultNoDS = self.chainPartNameNoMult.replace('_ds2', '')
      elif ("ds3" in self.chainPart['addInfo']): 
        chainPartNameNoMultNoDS = self.chainPartNameNoMult.replace('_ds3', '') 
      else:
        chainPartNameNoMultNoDS = self.chainPartNameNoMult

    ########### Sequence List ##############
    if 'msonly' in self.chainPart['reccalibInfo']:

      self.EFsequenceList += [['',
                               [PESA__DummyUnseededAllTEAlgo("EFDummyAlgo")],
                              'EF_dummy']]
      self.EFsequenceList += [['EF_dummy',
                               [CfgGetter.getAlgorithm("TrigMuSuperEF_FSSA")],
                               'EF_SA_FS']]
      self.EFsequenceList += [['EF_SA_FS',
                                [theTrigMuonEFSA_FS_Hypo],
                               'EF_SA_FS2']]
    else:
      self.EFsequenceList += [['',
                               [PESA__DummyUnseededAllTEAlgo("EFDummyAlgo")],
                               'EF_dummy']]
      self.EFsequenceList += [['EF_dummy',
                               [CfgGetter.getAlgorithm("TrigMuSuperEF_FSSA")],
                               'EF_SA_FS']]
 
      self.EFsequenceList += [['EF_SA_FS',
                               [theTrigMuonEFSA_FS_Hypo],
                               'EF_SA_FS2']]
 
      self.EFsequenceList += [['EF_SA_FS',
                               [CfgGetter.getAlgorithm("TrigMuonEFFSRoiMaker")],
                               'EF_SAR_FS']]
      self.EFsequenceList += [['EF_SAR_FS',
                               trkfast+trkprec,         
                               'EF_FStracksMuon']]
      self.EFsequenceList += [['EF_FStracksMuon',
                               [CfgGetter.getAlgorithm("TrigMuSuperEF_TMEFCombinerOnly")],
                               'EF_CB_FS_single']]
      self.EFsequenceList += [['EF_CB_FS_single',
                               [TrigMuonEFRoiAggregatorConfig('TrigMuonEFFSRoiAggregator')],
                               'EF_CB_ROI']]
      self.EFsequenceList += [['EF_CB_ROI',
                               [theTrigMuonEFCombinerMultiHypoConfig],
                               'EF_CB_FS']]

      if '20invm60' in self.chainPart['addInfo'] or '20invm60' in self.chainPart['extra']:
        theTrigMuonEFCombinerDiMuonMassPtImpactsHypoConfig = TrigMuonEFCombinerDiMuonMassPtImpactsHypoConfig("DiMuon","0")
        theTrigMuonEFCombinerDiMuonMassPtImpactsHypoConfig.massThresLow = 20.0
        theTrigMuonEFCombinerDiMuonMassPtImpactsHypoConfig.massThresHigh = 60.0
        self.EFsequenceList += [[['EF_mu_step2'],
           [theTrigMuonEFCombinerDiMuonMassPtImpactsHypoConfig],
           'EF_mu_step3']]

      if 'TagandProbe' in self.chainPart['FSinfo']:
        TrigMuonEFTagandProbeInstance = TrigMuonEFTagandProbeConfig()
        self.EFsequenceList += [['EF_CB_FS',
                                 [TrigMuonEFTagandProbeInstance],
                                 'EF_CB_FSTaP']]      

      if run_isolation:
        self.EFsequenceList += [['EF_CB_FS_single',
                                 trkiso,
                                 'EF_ID_FS']]
        self.EFsequenceList += [['EF_ID_FS',
                                 [TrigMuonEFRoiAggregatorConfig('TrigMuonEFFSRoiAggregator')],
                                 'EF_ID_FS_single']]
        isoAlgoName = "TrigMuonEFTrackIsolationVar"
        self.EFsequenceList += [[['EF_ID_FS_single'],
                                 [TrigMuonEFTrackIsolationVarConfig(isoAlgoName)],
                                 'EF_ISO_FS']]
        self.EFsequenceList += [[['EF_ISO_FS'],
                                 [theTrigMuonEFTrackIsolationMultiHypoConfig],
                                 'EF_ISO_HYPO']]
                                

    ########### Signatures ###########
      
    self.EFsignatureList += [ [['EF_dummy']] ]
    self.EFsignatureList += [ [['EF_SA_FS']] ]
    self.EFsignatureList += [ [['EF_SA_FS2']] ]
    if 'msonly' not in self.chainPart['reccalibInfo']:
      self.EFsignatureList += [ [['EF_SAR_FS']] ]
      self.EFsignatureList += [ [['EF_FStracksMuon']] ]
      self.EFsignatureList += [ [['EF_CB_FS_single']] ]
      self.EFsignatureList += [ [['EF_CB_ROI']] ]
      self.EFsignatureList += [ [['EF_CB_FS','EF_SA_FS2']] ]
      if 'TagandProbe' in self.chainPart['FSinfo']:
        self.EFsignatureList += [ [['EF_CB_FSTaP']] ]

      if run_isolation:
        self.EFsignatureList += [ [['EF_ID_FS']] ]
        self.EFsignatureList += [ [['EF_ID_FS_single']] ]
        self.EFsignatureList += [ [['EF_ISO_FS']] ]
        self.EFsignatureList += [ [['EF_ISO_HYPO']] ]


    ########### TE renaming ##########

    if 'msonly' in self.chainPart['reccalibInfo']:
      self.TErenamingDict = {
        'EF_SA_FS': mergeRemovingOverlap('EF_SA_FS_msonly_','SAFSHypo'),  
        'EF_SA_FS2': mergeRemovingOverlap('EF_SA_FS2_msonly_','SAFSHypo'+hypocut),  
         }
    else:
      self.TErenamingDict = {
        'EF_SA_FS': mergeRemovingOverlap('EF_SA_FS_','SAFSHypo'),
        'EF_SA_FS2': mergeRemovingOverlap('EF_SA_FS2_','SAFSHypo'+hypocut),
        'EF_SAR_FS': mergeRemovingOverlap('EF_SAR_FS_','SAFSHypo'),
        'EF_FStracksMuon': mergeRemovingOverlap('EF_FStracksMuon_', 'SAFSHypo'),
        'EF_CB_FS_single': mergeRemovingOverlap('EF_CB_FS_single_','SAFSHypo'), 
      }
      self.TErenamingDict['EF_CB_ROI'] = mergeRemovingOverlap('EF_CB_ROI_','SAFSRoi')
      self.TErenamingDict['EF_CB_FS'] = mergeRemovingOverlap('EF_CB_FS_', 'SAFSHypo'+hypocut+'_'+hypocutEF)

      if 'TagandProbe' in self.chainPart['FSinfo'] : self.TErenamingDict['EF_CB_FSTaP'] = mergeRemovingOverlap('EF_CB_FSTaP_', 'SAFSHypo'+hypocut+'_'+hypocutEF)

      if run_isolation:
        self.TErenamingDict['EF_ID_FS_single'] = mergeRemovingOverlap('EF_trkIso_', chainPartNameNoMultNoDS+'EFFSID')
        self.TErenamingDict['EF_ID_FS'] = mergeRemovingOverlap('EF_ID_FS_', 'SAFSHypo'+hypocut+'_'+hypocutEF + "_ID")
        self.TErenamingDict['EF_ISO_FS'] = mergeRemovingOverlap('EF_ISO_FS_', chainPartNameNoMultNoDS+'EFFSISO')
        self.TErenamingDict['EF_ISO_HYPO'] = mergeRemovingOverlap('EF_ISO_FS_', chainPartNameNoMultNoDS+'EFFSISOHypo')

      
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
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFCombinerMultiHypoConfig, TrigMuonEFCombinerDiMuonMassHypoConfig

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
                            [CfgGetter.getAlgorithm("TrigMuSuperEF_FSSA")],
                             'EF_SA_FSFTK']]
    self.EFsequenceList += [['EF_SA_FSFTK',
                             [theTrigMuonEFSA_FS_Hypo],
                             'EF_SA_FSFTK2']]
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
                            [TrigMuonEFRoiAggregatorConfig('TrigMuonEFFSRoiAggregator')],
                             'EF_CB_FSFTK']]
    self.EFsequenceList += [['EF_CB_FSFTK',
                             [theTrigMuonEFCombinerMultiHypoConfig],
                             'EF_CB_FSFTK2']]

    ########### Signatures ###########
      
    self.EFsignatureList += [ [['EF_dummyFTK']] ]
    self.EFsignatureList += [ [['EF_dummyRoIFTK']] ]
    self.EFsignatureList += [ [['EF_trk_trkFTK']] ]
    self.EFsignatureList += [ [['EF_trk_ROIFTK']] ]
    self.EFsignatureList += [ [['EF_SA_FSFTK']] ]
    self.EFsignatureList += [ [['EF_SA_FSFTK2']] ]
    self.EFsignatureList += [ [['EF_SAR_FSFTK']] ]
    self.EFsignatureList += [ [['EF_FStracksMuonFTK']] ]
    self.EFsignatureList += [ [['EF_CB_FS_singleFTK']] ]
    self.EFsignatureList += [ [['EF_CB_FSFTK']] ]
    self.EFsignatureList += [ [['EF_CB_FSFTK2']] ]

    ########### TE renaming ##########

    self.TErenamingDict = {
      'EF_trk_ROIFTK': mergeRemovingOverlap('EF_trk_trkFTK_','SAFSHypo'+hypocut),
      'EF_trk_ROIbFTK': mergeRemovingOverlap('EF_trk_ROIFTK_','SAFSHypo'+hypocut),
      'EF_SA_FSFTK': mergeRemovingOverlap('EF_SA_FSFTK_','SAFSHypo'+hypocut),
      'EF_SA_FSFTK2': mergeRemovingOverlap('EF_SA_FSFTK2_','SAFSHypo'+hypocut),
      'EF_SAR_FSFTK': mergeRemovingOverlap('EF_SAR_FSFTK_','SAFSHypo'+hypocut),
      'EF_FStracksMuonFTK': mergeRemovingOverlap('EF_FStracksMuonFTK_', 'SAFSHypo'+hypocut),
      'EF_CB_FS_singleFTK': mergeRemovingOverlap('EF_CB_FSFTK_single_','SAFSHypo'+hypocut), 
      'EF_CB_FSFTK': mergeRemovingOverlap('EF_CB_FSFTK_', 'SAFSHypo'+hypocut+'_'+hypocutEF),
      'EF_CB_FSFTK2': mergeRemovingOverlap('EF_CB_FSFTK2_', 'SAFSHypo'+hypocut+'_'+hypocutEF),

      }

  #################################################################################################
  #################################################################################################

  def setup_muXX_dR(self):
    
    from TrigGenericAlgs.TrigGenericAlgsConfig import OverlapRemovalConfig
    OverlapRemoval_algo = OverlapRemovalConfig('OvlRem', MinCentDist = 1)
    
    self.L2sequenceList += [[self.L2InputTE,
                             [OverlapRemoval_algo],
                             'L2_OvlRem_dRl1']]

    self.L2signatureList += [ [['L2_OvlRem_dRl1']] ]

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
    #seed = self.allMuThrs[0]
    theCTAlg = CfgGetter.getAlgorithm("TrigMuSuperEF_CTonly")
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonCaloTagHypoConfig
    theTrigMuonCT_FS_Hypo = TrigMuonCaloTagHypoConfig('MuonCT', threshold, int(multiplicity),  self.allMuThrs[0])

    theTrigMuonEFCombinerHypoConfigCT = TrigMuonEFCombinerHypoConfig('MuonCT',self.allMuThrs[0])
    from TrigCaloRec.TrigCaloRecConfig import TrigCaloClusterMaker_topo, TrigCaloCellMaker_super
    from TrigMuonEF.TrigMuonEFConfig import TrigMuonEFRoiAggregatorConfig

    from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
    [trkfast, trkprec] = TrigInDetSequence("Muon", "muon", "IDTrig").getSequence()
    

    inputTEfromL2 = "placeHolderTE"
    ########### Sequence List ##############
    if "0eta010" in self.chainPart['etaRange'] or "0eta500" in self.chainPart["etaRange"]:
      #seed = '0eta0'
      from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo
      from TrigMuonEF.TrigMuonEFConfig import TrigMuonEFFSRoiMakerUnseededConfig, TrigMuonEFFSRoiMakerConfig
      if "0eta010" in self.chainPart['etaRange']:
        theEFRoIMaker = TrigMuonEFFSRoiMakerUnseededConfig("TrigMuonEFFSRoiMakerUnseeded_0eta010", RoISizeEta=0.1)
        hypocut = "0eta010"
      elif "0eta500" in self.chainPart['etaRange']:
        theEFRoIMaker = TrigMuonEFFSRoiMakerUnseededConfig("TrigMuonEFFSRoiMakerUnseeded_0eta500", RoISizeEta=5.0)
        hypocut = "0eta500"
      else:
        log.error("Cannot setup unseeded muon calotag chain which is not 0eta500 or 0eta010")
        return
      
      theEFRoIMakerCT = TrigMuonEFFSRoiMakerConfig("TrigMuonEFFSRoiMakerCT", RoISizeEta=0.1)

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
                               [theCTAlg],
                               'EF_CT_alg']]
      self.EFsequenceList += [[inputTEfromL2,
                               [theTrigMuonEFCombinerHypoConfigCT],
                               'EF_CT_hypo1']]
      self.EFsequenceList += [[['EF_CT_alg','EF_CT_hypo1'],
                               [theEFRoIMakerCT],
                               'EF_CT_roi2']]
      self.EFsequenceList += [['EF_CT_roi2',
                               [TrigMuonEFRoiAggregatorConfig('TrigMuonEFFSRoiAggregatorCT')],
                               'EF_CT_roiagg']]

      self.EFsequenceList += [['EF_CT_roiagg',
                               [theTrigMuonCT_FS_Hypo],
                               'EF_CT_hypo']]
    
    else:
        log.error("full scan calo tagging not supported")

    
    ########### Signatures ###########
    
    self.EFsignatureList += [ [['EF_CT_seed']] ]
    self.EFsignatureList += [ [['EF_CT_roi']] ]
    self.EFsignatureList += [ [['EF_CT_calo']] ]
    self.EFsignatureList += [ [['EF_CT_tracks']] ]
    self.EFsignatureList += [ [['EF_CT_alg']] ]    
    self.EFsignatureList += [ [['EF_CT_hypo1']] ]
    self.EFsignatureList += [ [['EF_CT_roi2']] ]    
    self.EFsignatureList += [ [['EF_CT_roiagg']] ]    
    self.EFsignatureList += [ [['EF_CT_hypo']] ]

    ########### TE renaming ##########

    self.TErenamingDict = {
      'EF_CT_seed': mergeRemovingOverlap('EF_CT_seed_',hypocut),
      'EF_CT_roi': mergeRemovingOverlap('EF_CT_roi_',hypocut),
      'EF_CT_calo': mergeRemovingOverlap('EF_CT_calo_', hypocut),
      'EF_CT_tracks': mergeRemovingOverlap('EF_CT_tracks_',hypocut),
      'EF_CT_hypo1': mergeRemovingOverlap('EF_CT_hyp1_',hypocut+'_CTHypo1_'+hypocutEF+'_'+self.allMuThrs[0]),
      'EF_CT_alg': mergeRemovingOverlap('EF_CT_alg_', hypocut+'_'+hypocutEF),
      'EF_CT_roi2': mergeRemovingOverlap('EF_CT_roi2_', hypocut+'_'+hypocutEF+'_'+self.allMuThrs[0]),
      'EF_CT_roiagg': mergeRemovingOverlap('EF_CT_roiagg_', hypocut+'_'+hypocutEF+'_'+self.allMuThrs[0]),
      'EF_CT_hypo': mergeRemovingOverlap('EF_CT_hyp_',hypocut+'_CTHypo_'+hypocutEF+'_'+self.allMuThrs[0]),
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
        theTrigMuonEFSA_NS_Hypo = TrigMuonEFExtrapolatorHypoConfig('MuonNS', '0GeV')
        hypocut = '0'

    elif len(self.allMuThrs) == 2:
        theTrigMuonEFSA_NS_Hypo = TrigMuonEFExtrapolatorMultiHypoConfig('MuonNS', '0GeV','0GeV')
        hypocut = '0_0'

    elif len(self.allMuThrs) == 3:
        theTrigMuonEFSA_NS_Hypo = TrigMuonEFExtrapolatorMultiHypoConfig('MuonNS', '0GeV','0GeV','0GeV')
        hypocut = '0_0_0'
    else:
      log.error("No MuonEFExtrapolatorHypo config yet for events with more than 3 muons")


    from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
    if self.chainPart['isoInfo'] == "iloosems":
      [trkfast, trkiso, trkprec] = TrigInDetSequence("Muon", "muon", "IDTrig", sequenceFlavour=["2step"]).getSequence()
    else:
      [trkfast, trkprec] = TrigInDetSequence("Muon", "muon", "IDTrig").getSequence()

    from TrigMuonEF.TrigMuonEFConfig import TrigMuonEFRoiAggregatorConfig
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFCombinerMultiHypoConfig, TrigMuonEFCombinerHypoConfig

    if self.chainPart['isoInfo'] == "iloosems":
      theTrigMuonEFTrackIsolationHypoConfig = TrigMuonEFTrackIsolationHypoConfig("Muon","MSEFOnlyLooseWide")

    name = 'MuonNS'
    if "noComb" in self.chainPart['addInfo']:
      name = 'Muon_noComb'
    if "nscan05" in self.chainPart['FSinfo']:
      name = 'Muon05'
      if "noComb" in self.chainPart['addInfo']:
        name = 'Muon05_noComb'

    if len(self.allMuThrs) == 1:
      theTrigMuonEFCombinerMultiHypoConfigNS = TrigMuonEFCombinerHypoConfig(name, self.allMuThrs[0])
      theTrigMuonEFExtrapolatorMultiHypoConfigNS = TrigMuonEFExtrapolatorHypoConfig('MuonNS', self.allMuThrs[0])      
      try: hypocut = '%s' %(self.allMuThrs[0].split("GeV")[0])
      except:  hypocut = '%s' %(self.allMuThrs[0])

    elif len(self.allMuThrs) == 2:
      theTrigMuonEFCombinerMultiHypoConfigNS = TrigMuonEFCombinerMultiHypoConfig(name,self.allMuThrs[0], self.allMuThrs[1]) 
      theTrigMuonEFExtrapolatorMultiHypoConfigNS = TrigMuonEFExtrapolatorMultiHypoConfig('MuonNS',self.allMuThrs[0], self.allMuThrs[1])       
      try: hypocut = '%s_%s' %(self.allMuThrs[0].split("GeV")[0], self.allMuThrs[1].split("GeV")[0])                  
      except:hypocut = '%s_%s' %(self.allMuThrs[0], self.allMuThrs[1])                  

    elif len(self.allMuThrs) == 3:
      theTrigMuonEFCombinerMultiHypoConfigNS = TrigMuonEFCombinerMultiHypoConfig(name,self.allMuThrs[0],self.allMuThrs[1],self.allMuThrs[2])
      theTrigMuonEFExtrapolatorMultiHypoConfigNS = TrigMuonEFExtrapolatorMultiHypoConfig('MuonNS',self.allMuThrs[0],self.allMuThrs[1],self.allMuThrs[2])
      try: hypocut = '%s_%s_%s' %(self.allMuThrs[0].split("GeV")[0], self.allMuThrs[1].split("GeV")[0],self.allMuThrs[2].split("GeV")[0])      
      except: hypocut = '%s_%s_%s' %(self.allMuThrs[0], self.allMuThrs[1],self.allMuThrs[2])                                  
    else:
        log.error("No TrigMuonEFCombinerHypo config yet for events with more than 3 muons")
 

    if "noComb" in self.chainPart['addInfo']:
      theTrigMuonEFCombinerMultiHypoConfigNS.RejectCBmuons=True
    hypocutEF="MultiComb"     
    for i in range(0,len(self.allMuThrs)):        
      hypocutEF +=  "_%s" %(self.allMuThrs[i])

    theTrigMuonEFSA_NS_Hypo.DoNscan=True
    theTrigMuonEFExtrapolatorMultiHypoConfigNS.DoNscan=True
    theTrigMuonEFCombinerMultiHypoConfigNS.DoNscan=True

    cone = ""
    ########### Sequence List ##############

    self.EFsequenceList += [['',
                             [PESA__DummyUnseededAllTEAlgo("EFDummyAlgo",createRoIDescriptors=True)],
                             'EF_dummy']]
    if "nscan03" in self.chainPart['FSinfo']:
      cone = "_cone03"
      theTrigMuonEFSA_NS_Hypo.ConeSize=0.3
      theTrigMuonEFCombinerMultiHypoConfigNS.ConeSize=0.3
      if "msonly" in self.chainPart['reccalibInfo']:
              #      self.EFsequenceList += [[self.chainPart['L1item'].replace("L1_",""),
        self.EFsequenceList += [['EF_dummy',
                                 [CfgGetter.getAlgorithm("TrigMuSuperEF_FSSA")],
                                 'EF_SA_NS']]
        self.EFsequenceList += [['EF_SA_NS',
                                 [theTrigMuonEFSA_NS_Hypo],
                                 'EF_SA_NS2']]
        if self.chainPart['isoInfo'] == "iloosems":
          self.EFsequenceList += [['EF_SA_NS',
                                   [CfgGetter.getAlgorithm("TrigMuonEFFSRoiMaker")],
                                   'EF_SAR_NS']]
          self.EFsequenceList += [[['EF_SAR_NS'],
                                   trkfast+trkiso+trkprec,
                                   'EF_NStracksMuonIso']]
          self.EFsequenceList += [[['EF_NStracksMuonIso'],
                                   [TrigMuonEFMSTrackIsolationConfig("TrigMuonEFMSTrackIsolation"),theTrigMuonEFTrackIsolationHypoConfig],
                                   'EF_msIso']]
        if "noComb" in self.chainPart['addInfo']:
          self.EFsequenceList += [['EF_SA_NS',
                                   [CfgGetter.getAlgorithm("TrigMuonEFFSRoiMaker")],
                                   'EF_SAR_NS']]
          if self.chainPart['isoInfo'] == "iloosems":
            self.EFsequenceList += [['EF_NStracksMuonIso',
                                     [CfgGetter.getAlgorithm("TrigMuSuperEF_TMEFCombinerOnly")],
                                     'EF_CB_NS_singleIso']]
            self.EFsequenceList += [['EF_CB_NS_singleIso',
                                     [TrigMuonEFRoiAggregatorConfig('TrigMuonEFFSRoiAggregator'),
                                      theTrigMuonEFCombinerMultiHypoConfigNS],
                                     'EF_CB_NSIso']]
            
          else:
            self.EFsequenceList += [['EF_SAR_NS',
                                     trkfast+trkprec,		  #theTrigEFIDInsideOut_Muon,	  #a fallback - it should be replaced by the previous line if it works
                                     'EF_NStracksMuon']]
            self.EFsequenceList += [['EF_NStracksMuon',
                                     [CfgGetter.getAlgorithm("TrigMuSuperEF_TMEFCombinerOnly")],
                                     'EF_CB_NS_single']]
            self.EFsequenceList += [['EF_CB_NS_single',
                                     [TrigMuonEFRoiAggregatorConfig('TrigMuonEFFSRoiAggregator'),
                                      theTrigMuonEFCombinerMultiHypoConfigNS],
                                     'EF_CB_NS']]
      
      else:
          #      self.EFsequenceList += [[self.chainPart['L1item'].replace("L1_",""),
          self.EFsequenceList += [['EF_dummy',
                                   [CfgGetter.getAlgorithm("TrigMuSuperEF_FSSA")],
                                   'EF_SA_NS']]
          self.EFsequenceList += [['EF_SA_NS',
                                   [theTrigMuonEFSA_NS_Hypo],
                                   'EF_SA_NS2']]
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
                                    theTrigMuonEFCombinerMultiHypoConfigNS],
                                   'EF_CB_NS']]
    elif "nscan05" in self.chainPart['FSinfo']:
      cone = "_cone05"
      theTrigMuonEFExtrapolatorMultiHypoConfigNS.ConeSize=0.3
      theTrigMuonEFCombinerMultiHypoConfigNS.ConeSize=0.3
      #      self.EFsequenceList += [[self.chainPart['L1item'].replace("L1_",""),
      self.EFsequenceList += [['EF_dummy',
                               [CfgGetter.getAlgorithm("TrigMuSuperEF_FSSA")],
                               'EF_SA_NS']]
      self.EFsequenceList += [['EF_SA_NS',
                               [theTrigMuonEFSA_NS_Hypo],
                               'EF_SA_NS2']]
      if "noComb" in self.chainPart['addInfo']:
        self.EFsequenceList += [['EF_SA_NS',
                                 [CfgGetter.getAlgorithm("TrigMuonEFFSRoiMaker")],
                                 'EF_SAR_NS']]
        if self.chainPart['isoInfo'] == "iloosems":
          self.EFsequenceList += [[['EF_SAR_NS'],
                                   trkfast+trkiso+trkprec,
                                   'EF_NStracksMuonIso']]
          self.EFsequenceList += [[['EF_NStracksMuonIso'],
                                   [TrigMuonEFMSTrackIsolationConfig("TrigMuonEFMSTrackIsolation"),theTrigMuonEFTrackIsolationHypoConfig],
                                   'EF_msIso']]
          self.EFsequenceList += [['EF_NStracksMuonIso',
                                   [CfgGetter.getAlgorithm("TrigMuSuperEF_TMEFCombinerOnly")],
                                   'EF_CB_NS_singleIso']]
          self.EFsequenceList += [['EF_CB_NS_singleIso',
                                   [TrigMuonEFRoiAggregatorConfig('TrigMuonEFFSRoiAggregator'),
                                    theTrigMuonEFCombinerMultiHypoConfigNS],
                                   'EF_CB_NSIso']]

        else:
          self.EFsequenceList += [['EF_SAR_NS',
                                   trkfast+trkprec,                 #theTrigEFIDInsideOut_Muon,     #a fallback - it should be replaced by the previous line if it works
                                   'EF_NStracksMuon']]
          self.EFsequenceList += [['EF_NStracksMuon',
                                   [CfgGetter.getAlgorithm("TrigMuSuperEF_TMEFCombinerOnly")],
                                   'EF_CB_NS_single']]
          self.EFsequenceList += [['EF_CB_NS_single',
                                   [TrigMuonEFRoiAggregatorConfig('TrigMuonEFFSRoiAggregator'),
                                    theTrigMuonEFCombinerMultiHypoConfigNS],
                                   'EF_CB_NS']]

    else:
      log.error("No other cone than 05 or 03 was implemented")


    ########### Signatures ###########
    self.EFsignatureList += [ [['EF_dummy']] ]
    self.EFsignatureList += [ [['EF_SA_NS']] ]
    self.EFsignatureList += [ [['EF_SA_NS2']] ]
    if "nscan03" in self.chainPart['FSinfo']:
      if "msonly" in self.chainPart['reccalibInfo']:
        if "noComb" in self.chainPart['addInfo']:
          self.EFsignatureList += [ [['EF_SAR_NS']] ]
          if self.chainPart['isoInfo'] == "iloosems":
            self.EFsignatureList += [ [['EF_NStracksMuonIso']] ]
            self.EFsignatureList += [ [['EF_msIso']] ]
            self.EFsignatureList += [ [['EF_CB_NS_singleIso']] ]
            self.EFsignatureList += [ [['EF_CB_NSIso','EF_SA_NS2']] ]
          else:
            self.EFsignatureList += [ [['EF_NStracksMuon']] ]
            self.EFsignatureList += [ [['EF_CB_NS_single']] ]
            self.EFsignatureList += [ [['EF_CB_NS','EF_SA_NS2']] ]
        else:
          if self.chainPart['isoInfo'] == "iloosems":
            self.EFsignatureList += [ [['EF_NStracksMuonIso']] ]
            self.EFsignatureList += [ [['EF_msIso']] ]


      else:
        self.EFsignatureList += [ [['EF_SAR_NS']] ]
        self.EFsignatureList += [ [['EF_NStracksMuon']] ]
        self.EFsignatureList += [ [['EF_CB_NS_single']] ]
        self.EFsignatureList += [ [['EF_CB_NS','EF_SA_NS2']] ]
    if "nscan05" in self.chainPart['FSinfo']:
      if "noComb" in self.chainPart['addInfo']:
        self.EFsignatureList += [ [['EF_SAR_NS']] ]
        if self.chainPart['isoInfo'] == "iloosems":
          self.EFsignatureList += [ [['EF_NStracksMuonIso']] ]
          self.EFsignatureList += [ [['EF_msIso']] ]
          self.EFsignatureList += [ [['EF_CB_NS_singleIso']] ]
          self.EFsignatureList += [ [['EF_CB_NSIso','EF_SA_NS2']] ]
        else:
          self.EFsignatureList += [ [['EF_NStracksMuon']] ]
          self.EFsignatureList += [ [['EF_CB_NS_single']] ]
          self.EFsignatureList += [ [['EF_CB_NS','EF_SA_NS2']] ]
          
    nscanseed = self.chainPart['L1item']
    l1seed = self.chainL1Item

    if "nscan03" in self.chainPart['FSinfo']:
      if "msonly" in self.chainPart['reccalibInfo']:
        if "noComb" in self.chainPart['addInfo']:
          if (self.chainPart['isoInfo']):
            self.TErenamingDict = {
              'EF_SA_NS': mergeRemovingOverlap('EF_SA_NS_','SANShyp'),
              'EF_SA_NS2': mergeRemovingOverlap('EF_SA_NS_','SANShyp2'+hypocut+cone+'_noComb_'+nscanseed+'_'+l1seed),
              'EF_SAR_NS': mergeRemovingOverlap('EF_SAR_NS_','SANSHypo'), 
              'EF_NStracksMuonIso': mergeRemovingOverlap('EF_NStracksMuonIso_', 'SANSHypo'), 
              'EF_CB_NS_singleIso': mergeRemovingOverlap('EF_CB_NS_singleIso_','SANSHypo'), 
              'EF_CB_NSIso': mergeRemovingOverlap('EF_CB_NSIso_', 'SANSHypo'+hypocut+'_'+hypocutEF+cone+'_noComb_'+nscanseed+'_'+l1seed),
              'EF_msIso' : mergeRemovingOverlap('EF_msIso_', 'SANSHypo'),
              }
          else:
            self.TErenamingDict = {
              'EF_SA_NS': mergeRemovingOverlap('EF_SA_NS_','SANShyp'),
              'EF_SA_NS2': mergeRemovingOverlap('EF_SA_NS_','SANShyp2'+hypocut+cone+'_noComb_'+nscanseed+'_'+l1seed),
              'EF_SAR_NS': mergeRemovingOverlap('EF_SAR_NS_','SANSHypo'), 
              'EF_NStracksMuon': mergeRemovingOverlap('EF_NStracksMuon_', 'SANSHypo'), 
              'EF_CB_NS_single': mergeRemovingOverlap('EF_CB_NS_single_','SANSHypo'), 
              'EF_CB_NS': mergeRemovingOverlap('EF_CB_NS_', 'SANSHypo'+hypocut+'_'+hypocutEF+cone+'_noComb_'+nscanseed+'_'+l1seed),
              }
        else:
          if (self.chainPart['isoInfo']):
            self.TErenamingDict = {
              'EF_SA_NS': mergeRemovingOverlap('EF_SA_NS_','SANShyp'),
              'EF_NStracksMuonIso': mergeRemovingOverlap('EF_NStracksMuonIso_', 'SANSHypo'), 
              'EF_msIso' : mergeRemovingOverlap('EF_msIso_', 'SANSHypo'),
              }
          else:
            self.TErenamingDict = {
              'EF_SA_NS': mergeRemovingOverlap('EF_SA_NS_','SANShyp'),
              }
      else:
        self.TErenamingDict = {
          'EF_SA_NS': mergeRemovingOverlap('EF_SA_NS_','SANShyp'),
          'EF_SA_NS2': mergeRemovingOverlap('EF_SA_NS_','SANShyp'+hypocut+cone+'_'+nscanseed+'_'+l1seed),
          'EF_SAR_NS': mergeRemovingOverlap('EF_SAR_NS_','SANShyp'),
          'EF_NStracksMuon': mergeRemovingOverlap('EF_NStrkMu_', 'SANShyp'),
          'EF_CB_NS_single': mergeRemovingOverlap('EF_CB_NS_sngl_','SANShyp'), 
          'EF_CB_NS': mergeRemovingOverlap('EF_CB_NS_', 'SANShyp'+hypocut+'_'+hypocutEF+cone+'_'+nscanseed+'_'+l1seed),
          }
    if "nscan05" in self.chainPart['FSinfo']:
      if "noComb" in self.chainPart['addInfo']:
        if (self.chainPart['isoInfo']):
          self.TErenamingDict = {
            'EF_SA_NS': mergeRemovingOverlap('EF_SA_NS_','SANShyp'),
            'EF_SA_NS2': mergeRemovingOverlap('EF_SA_NS_','SANShyp'+hypocut+cone+'_'+nscanseed+'_'+l1seed),
            'EF_SAR_NS': mergeRemovingOverlap('EF_SAR_NS_','SANSHypo'), 
            'EF_NStracksMuonIso': mergeRemovingOverlap('EF_NStracksMuonIso_', 'SANSHypo'), 
            'EF_CB_NS_singleIso': mergeRemovingOverlap('EF_CB_NS_singleIso_','SANSHypo'), 
            'EF_CB_NSIso': mergeRemovingOverlap('EF_CB_NSIso_', 'SANSHypo'+hypocut+'_'+hypocutEF+cone+'_noComb_'+nscanseed+'_'+l1seed),
            'EF_msIso' : mergeRemovingOverlap('EF_msIso_', 'SANSHypo'),
            }
        else:
          self.TErenamingDict = {
            'EF_SA_NS': mergeRemovingOverlap('EF_SA_NS_','SANShyp'),
            'EF_SA_NS2': mergeRemovingOverlap('EF_SA_NS_','SANShyp'+hypocut+cone+'_'+nscanseed+'_'+l1seed),
            'EF_SAR_NS': mergeRemovingOverlap('EF_SAR_NS_','SANSHypo'), 
            'EF_NStracksMuon': mergeRemovingOverlap('EF_NStracksMuon_', 'SANSHypo'), 
            'EF_CB_NS_single': mergeRemovingOverlap('EF_CB_NS_single_','SANSHypo'), 
            'EF_CB_NS': mergeRemovingOverlap('EF_CB_NS_', 'SANSHypo'+hypocut+'_'+hypocutEF+cone+'_noComb_'+nscanseed+'_'+l1seed),
            }
      else:
        if (self.chainPart['isoInfo']):
          self.TErenamingDict = {
            'EF_SA_NS': mergeRemovingOverlap('EF_SA_NS_','SANShyp'),
            'EF_NStracksMuonIso': mergeRemovingOverlap('EF_NStracksMuonIso_', 'SANSHypo'), 
            'EF_msIso' : mergeRemovingOverlap('EF_msIso_', 'SANSHypo'),
            }
        else:
          self.TErenamingDict = {
            'EF_SA_NS': mergeRemovingOverlap('EF_SA_NS_','SANShyp'),
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
                             [theL2StandAloneAlg],
                             'L2_mu_step1']]
    self.L2sequenceList += [['L2_mu_step1',
                             [theL2StandAloneHypo],
                             'L2_mu_hypo1']] 

    self.L2sequenceList += [[['L2_mu_step1'],
                             trkfast+
                             [theL2CombinedAlg],
                             'L2_mu_step2']]
    self.L2sequenceList += [[['L2_mu_step2'],
                              [theL2CombinedHypo],
                             'L2_mu_hypo2']]

    #--- adding signatures ----
    self.L2signatureList += [ [['L2_mu_step1']*self.mult] ]
    self.L2signatureList += [ [['L2_mu_hypo1']*self.mult] ]
    self.L2signatureList += [ [['L2_mu_step2']*self.mult] ]
    self.L2signatureList += [ [['L2_mu_hypo2']*self.mult] ]
      
    #--- renaming TEs ---
    self.TErenamingDict = {
      'L2_mu_step1': mergeRemovingOverlap('L2_mu_SA_', L2AlgName+muFastThresh+"_"+'_'+self.L2InputTE),
      'L2_mu_hypo1': mergeRemovingOverlap('L2_mu_SAhyp_', L2AlgName+"_"+muFastThresh+'_'+self.L2InputTE),
      'L2_mu_step2': mergeRemovingOverlap('L2_mucomb_',   self.chainPartNameNoMult.replace(self.chainPart['specialStream'], '')+'_'+self.L2InputTE),
      'L2_mu_hypo2': mergeRemovingOverlap('L2_mucombhyp_',   self.chainPartNameNoMult.replace(self.chainPart['specialStream'], '')+'_'+self.L2InputTE),
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
    	  		       [theEFAlg],
                               'EF_mu_step1']]
      self.EFsequenceList += [[['EF_mu_step1'],
                               [theTrigMuonEFExtrapolatorHypoConfig],
                               'EF_mu_hypo1']]
    else:
      EFCombinerThresh = self.getEFCombinerThresh()
      
      ########### EF algos  #################
      from InDetTrigRecExample.EFInDetConfig import TrigEFIDInsideOut_CosmicsN
      theEFIDTracking=TrigEFIDInsideOut_CosmicsN()
      
      from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFCombinerHypoConfig
      theTrigMuonEFCombinerHypoConfig = TrigMuonEFCombinerHypoConfig(EFRecoAlgName,EFCombinerThresh)
                  
      
      ########### Sequence List ##############
      self.EFsequenceList += [[[self.L2InputTE],
                               theEFIDTracking.getSequence(),
                               'EF_mu_step1']]
      
      self.EFsequenceList += [[['EF_mu_step1'],
    	  		       [theEFAlg],
                               'EF_mu_step2']]
      self.EFsequenceList += [[['EF_mu_step2'],
                               [theTrigMuonEFCombinerHypoConfig],
                               'EF_mu_hypo1']]



    ########### Signatures ###########
    self.EFsignatureList += [ [['EF_mu_step1']*self.mult] ]
    self.EFsignatureList += [ [['EF_mu_hypo1']*self.mult] ]
    if not (self.chainPart['reccalibInfo'] == "msonly"):
      self.EFsignatureList += [ [['EF_mu_step2']*self.mult] ]
      self.EFsignatureList += [ [['EF_mu_hypo1']*self.mult] ]

    ########### TE renaming ##########
    if (self.chainPart['reccalibInfo'] == "msonly"):
      self.TErenamingDict = {
        'EF_mu_step1': mergeRemovingOverlap('EF_SuperEF_Extrapolator_', self.chainPartNameNoMult),
        'EF_mu_hypo1': mergeRemovingOverlap('EF_SuperEF_Extrapolatorhyp_', self.chainPartNameNoMult),
        }    
    else:
      self.TErenamingDict = {
        'EF_mu_step1': mergeRemovingOverlap('EF_CosmicsN_', self.chainPartNameNoMult),
        'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   self.chainPartNameNoMult),
        'EF_mu_hypo1': mergeRemovingOverlap('EF_SuperEFhyp_',   self.chainPartNameNoMult),
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
    [trkfast, trkiso, trkprec] = TrigInDetSequence("Muon", "muon", "IDTrig", sequenceFlavour=["2step"]).getSequence()

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
                               [theL2StandAloneAlg],
                               'L2_mu_step1']]
      self.L2sequenceList += [['L2_mu_step1',
                               [theL2StandAloneHypo],
                               'L2_mu_hypo1']] 
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
                             [theEFAlg],
                             'EF_mu_step2']]
    self.EFsequenceList += [[['EF_mu_step2'],
                             [theTrigMuonEFCombinerHypoConfig],
                             'EF_mu_hypo1']]

    self.EFsequenceList += [['EF_mu_hypo1',
                             [theTrigMuonIDTrackMultiHypoConfig_Muon],
                             'EF_mu_step3']] 
    
    #--- adding signatures ----    
    if use_muFast == 1:
      self.L2signatureList += [ [['L2_mu_step1']*self.mult] ]
      self.L2signatureList += [ [['L2_mu_hypo1']*self.mult] ]
      self.L2signatureList += [ [['L2_mu_step2']*self.mult] ]
      self.L2signatureList += [ [['L2_mu_step3']*self.mult] ]
      self.L2signatureList += [ [['L2_mu_step4']] ]
    else:
      self.L2signatureList += [ [['L2_mu_step1']*self.mult] ]
      self.L2signatureList += [ [['L2_mu_step2']] ]
      

    self.EFsignatureList += [ [['EF_mu_step1']*self.mult] ]
    self.EFsignatureList += [ [['EF_mu_step2']*self.mult] ]
    self.EFsignatureList += [ [['EF_mu_hypo1']*self.mult] ]
    self.EFsignatureList += [ [['EF_mu_step3']] ]

    #--- renaming TEs ---
    if use_muFast == 1:
      self.TErenamingDict = {
        'L2_mu_step1': mergeRemovingOverlap('L2_muFast_', idmulti+'_'+self.chainPartNameNoMult+'_'+self.L2InputTE),
        'L2_mu_hypo1': mergeRemovingOverlap('L2_muFasthyp_', idmulti+'_'+self.chainPartNameNoMult+'_'+self.L2InputTE),
        'L2_mu_step2': mergeRemovingOverlap('L2_muFastOvRm_', idmulti+'_'+self.chainPartNameNoMult+'_'+self.L2InputTE),
        'L2_mu_step3': mergeRemovingOverlap('L2_idtrking_', idmulti+'_'+self.chainPartNameNoMult+'_'+self.L2InputTE),
        'L2_mu_step4': mergeRemovingOverlap('L2_idtrkmulti_', idmulti+'_'+self.chainPartNameNoMult+'_'+self.L2InputTE),
        'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', idmulti+'_'+self.chainPartNameNoMult+'_'+self.L2InputTE),
        'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   idmulti+'_'+self.chainPartNameNoMult+'_'+self.L2InputTE),
        'EF_mu_hypo1': mergeRemovingOverlap('EF_SuperEFhyp_',   idmulti+'_'+self.chainPartNameNoMult+'_'+self.L2InputTE),
        'EF_mu_step3': mergeRemovingOverlap('EF_mutrkmulti_', idmulti+'_'+self.chainPartNameNoMult+'_'+self.L2InputTE),
        }
    else:
      self.TErenamingDict = {
        'L2_mu_step1': mergeRemovingOverlap('L2_idtrking_', idmulti+'_'+self.chainPartNameNoMult+'_'+self.L2InputTE),
        'L2_mu_step2': mergeRemovingOverlap('L2_idtrkmulti_', idmulti+'_'+self.chainPartNameNoMult+'_'+self.L2InputTE),
        'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', idmulti+'_'+self.chainPartNameNoMult+'_'+self.L2InputTE),
        'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   idmulti+'_'+self.chainPartNameNoMult+'_'+self.L2InputTE),
        'EF_mu_hypo1': mergeRemovingOverlap('EF_SuperEFhyp_',   idmulti+'_'+self.chainPartNameNoMult+'_'+self.L2InputTE),
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
    [trkfast, trkiso, trkprec] = TrigInDetSequence("Muon", "muon", "IDTrig", sequenceFlavour=["2step"]).getSequence()
    
    theTrigMuSuperEF = CfgGetter.getAlgorithm("TrigMuSuperEF")
    EFRecoAlgName = "Muon"
    
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFCombinerHypoConfig
    theTrigMuonEFCombinerHypoConfig = TrigMuonEFCombinerHypoConfig(EFRecoAlgName,EFCombinerThresh)
    
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonIDTrackMultiHypoConfig
    idmulti = "%ipt%i" % ( int(self.mult), int(self.chainPart['threshold']))
    theTrigMuonIDTrackMultiHypoConfig_FTF = TrigMuonIDTrackMultiHypoConfig( idmulti, "FTF" )
    theTrigMuonIDTrackMultiHypoConfig_Muon = TrigMuonIDTrackMultiHypoConfig( idmulti, "Muon" )
    
    self.L2sequenceList += [[self.L2InputTE,
                             [theL2StandAloneAlg],
                             'L2_mu_step1']]
    self.L2sequenceList += [['L2_mu_step1',
                             [theL2StandAloneHypo],
                             'L2_mu_hypo1']] 
    
    self.L2sequenceList += [['L2_mu_hypo1',
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
                             [theTrigMuSuperEF],
                             'EF_mu_step2']]
    self.EFsequenceList += [[['EF_mu_step2'],
                             [theTrigMuonEFCombinerHypoConfig],
                             'EF_mu_hypo1']]
  
    
    self.L2signatureList += [ [['L2_mu_step1']*self.mult] ]
    self.L2signatureList += [ [['L2_mu_hypo1']*self.mult] ]
    self.L2signatureList += [ [['L2_mu_step2']*self.mult] ]
    self.L2signatureList += [ [['L2_mu_step3']          ] ]
    self.EFsignatureList += [ [['EF_mu_step1']*self.mult] ]
    self.EFsignatureList += [ [['EF_mu_step2']*self.mult] ]
    self.EFsignatureList += [ [['EF_mu_hypo1']*self.mult] ]
    
    self.TErenamingDict = {
      'L2_mu_step1': mergeRemovingOverlap('L2_mu_SA_',  L2AlgName+muFastThresh+'_'+self.L2InputTE),
      'L2_mu_hypo1': mergeRemovingOverlap('L2_mu_SAhyp_',  L2AlgName+muFastThresh+'_'+self.L2InputTE),
      'L2_mu_step2': mergeRemovingOverlap('L2_idtrking_', L2AlgName+muFastThresh+'_'+self.L2InputTE),
      'L2_mu_step3': mergeRemovingOverlap('L2_idtrkmulti_', idmulti+'_'+L2AlgName+muFastThresh+'_'+self.L2InputTE),
      'EF_mu_step1': mergeRemovingOverlap('EF_EFIDInsideOut_', idmulti+'_'+self.chainPartNameNoMult.replace(self.chainPart['specialStream'], '')+'_'+self.L2InputTE).replace('__', '_'),
      'EF_mu_step2': mergeRemovingOverlap('EF_SuperEF_',   idmulti+'_'+self.chainPartNameNoMult.replace(self.chainPart['specialStream'], '')+'_'+self.L2InputTE).replace('__', '_'),
      'EF_mu_hypo1': mergeRemovingOverlap('EF_SuperEFhyp_',   idmulti+'_'+self.chainPartNameNoMult.replace(self.chainPart['specialStream'], '')+'_'+self.L2InputTE).replace('__', '_'),
      }

    # OI this part gives problem to EF 2TE algos; keep it for v6 only.
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    if "_v6" in TriggerFlags.triggerMenuSetup():
      self.EFsequenceList += [[['EF_mu_hypo1'],
                              [theTrigMuonIDTrackMultiHypoConfig_Muon],
                              'EF_mu_step3']]
      self.EFsignatureList += [ [['EF_mu_step3']          ] ]
      self.TErenamingDict['EF_mu_step3'] = mergeRemovingOverlap('EF_mutrkmulti_', idmulti+'_'+self.chainPartNameNoMult.replace(self.chainPart['specialStream'], '')+'_'+self.L2InputTE).replace('__', '_')
###########################################
## Adding new noL1btrk feature for muXnoL1btrk triggers, where EF is seeded by 
#################################################################################################
  def setup_muXX_noL1btrk(self):

    
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFExtrapolatorMultiHypoConfig, TrigMuonEFExtrapolatorHypoConfig
    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFCombinerMultiHypoConfig
    
    ########### EF algos  #################
    
    ##Use list of muon threshold in the chain to correctly configure the FS hypos
    
    if len(self.allMuThrs) == 0:
      log.error("The list of allMuonThreshold is empty for a btrk chain! It should never happen")

    if len(self.allMuThrs) == 1:
      theTrigMuonEFSA_FS_Hypo = TrigMuonEFExtrapolatorHypoConfig('Muon', '0GeV')
      hypocut = '0GeV'
      theTrigMuonEFCombinerMultiHypoConfig = TrigMuonEFCombinerHypoConfig('Muon', self.allMuThrs[0])

    elif len(self.allMuThrs) == 2:
      theTrigMuonEFCombinerMultiHypoConfig = TrigMuonEFCombinerMultiHypoConfig('Muon',self.allMuThrs[0], self.allMuThrs[1]) 
      theTrigMuonEFSA_FS_Hypo = TrigMuonEFExtrapolatorMultiHypoConfig('Muon', '0GeV','0GeV')
      hypocut = '0GeV_0GeV'

    elif len(self.allMuThrs) == 3:
      theTrigMuonEFCombinerMultiHypoConfig = TrigMuonEFCombinerMultiHypoConfig('Muon',self.allMuThrs[0],self.allMuThrs[1],self.allMuThrs[2])
      theTrigMuonEFSA_FS_Hypo = TrigMuonEFExtrapolatorMultiHypoConfig('Muon', '0GeV','0GeV','0GeV')
      hypocut = '0GeV_0GeV_0GeV'
    else:
      log.error("No MuonEFExtrapolatorHypo config yet for events with more than 3 muons (btrk chain)")
      log.error("No TrigMuonEFCombinerHypo config yet for events with more than 3 muons")
      

    hypocutEF="MultiComb"     
    for i in range(0,len(self.allMuThrs)):        
        hypocutEF +=  "_%s" %(self.allMuThrs[i])

    from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
    [trkfast, trkiso, trkprec] = TrigInDetSequence("Muon", "muon", "IDTrig", sequenceFlavour=["2step"]).getSequence()
    #[trkfast, trkprec] = TrigInDetSequence("Muon", "muonBtrk", "IDTrig").getSequence()


    from TrigBphysHypo.TrigBphysHypoConf import TrigBphysTrackRoiMaker
    theTrigBphysTrackRoiMaker = TrigBphysTrackRoiMaker()
    
    ########### Sequence List ##############

    # first we would like to get extra tracks in slightly wider cone
    
    # This is a tricky place.  So Here we need input from L2 bphys FEX that fills tracks.
    # but this part of the code is called before bPhys FEX is attached.
    # so we put a dummy TE here, which is corrected in generateBPhysicsChainDef.py

#    inputTEs = theChainDef.signatureList[-1]['listOfTriggerElements']

#    self.EFsequenceList += [['',
#                            [PESA__DummyUnseededAllTEAlgo("EFDummyAlgo",createRoIDescriptors=True)],
#                             'EF_dummyFTK']]
#    self.EFsequenceList += [['EF_dummyFTK',
#                             [FSroimaker],
#                             'EF_dummyRoIFTK']]
#    self.EFsequenceList += [[inputTEs,
#                             trkfast,
#                             'L2_trk_trkBTRK']]
    self.EFsequenceList += [['L2BPHYS_TOBEFILLED',  # the following algorithm should just find all available tracks
                             [theTrigBphysTrackRoiMaker],
                             'EF_trk_ROIBTRK']]
    self.EFsequenceList += [['EF_trk_ROIBTRK',
#                            [CfgGetter.getAlgorithm("TrigMuSuperEF_SAonly")],
                            [CfgGetter.getAlgorithm("TrigMuSuperEF_FSSA")],
                             'EF_SA_FSBTRK']]
    self.EFsequenceList += [['EF_SA_FSBTRK',
                             [theTrigMuonEFSA_FS_Hypo],
                             'EF_SA_FSBTRK2']]
    self.EFsequenceList += [['EF_SA_FSBTRK',
                            [CfgGetter.getAlgorithm("TrigMuonEFFSRoiMaker")],
                             'EF_SAR_FSBTRK']]
    self.EFsequenceList += [['EF_SAR_FSBTRK',
                             trkfast+trkprec,       #theTrigEFIDInsideOut_Muon,     #a fallback - it should be replaced by the previous line if it works
                             'EF_FStracksMuonBTRK']]
    self.EFsequenceList += [['EF_FStracksMuonBTRK',
                             [CfgGetter.getAlgorithm("TrigMuSuperEF_TMEFCombinerOnly")],
                             'EF_CB_FS_singleBTRK']]
    self.EFsequenceList += [['EF_CB_FS_singleBTRK',
                            [TrigMuonEFRoiAggregatorConfig('TrigMuonEFFSRoiAggregator')],
                             'EF_CB_FSBTRK']]
    self.EFsequenceList += [['EF_CB_FSBTRK',
                             [theTrigMuonEFCombinerMultiHypoConfig],
                             'EF_CB_FSBTRK2']]

    ########### Signatures ###########
      
#    self.EFsignatureList += [ [['EF_dummyBTRK']] ]
#    self.EFsignatureList += [ [['EF_dummyRoIBTRK']] ]
#    self.EFsignatureList += [ [['EF_trk_trkBTRK']] ]
    self.EFsignatureList += [ [['EF_trk_ROIBTRK']] ]
    self.EFsignatureList += [ [['EF_SA_FSBTRK']] ]
    self.EFsignatureList += [ [['EF_SA_FSBTRK2']] ]
    self.EFsignatureList += [ [['EF_SAR_FSBTRK']] ]
    self.EFsignatureList += [ [['EF_FStracksMuonBTRK']] ]
    self.EFsignatureList += [ [['EF_CB_FS_singleBTRK']] ]
    self.EFsignatureList += [ [['EF_CB_FSBTRK']] ]
    self.EFsignatureList += [ [['EF_CB_FSBTRK2']] ]

    ########### TE renaming ##########

    self.TErenamingDict = {
        #'EF_trk_ROIBTRK': mergeRemovingOverlap('EF_trk_trkBTRK_','SAFSHypo'+hypocut),
      'EF_trk_ROIBTRK': mergeRemovingOverlap('EF_trk_ROIBTRK_','SAFSHypo'+hypocutEF),
      'EF_SA_FSBTRK': mergeRemovingOverlap('EF_SA_FSBTRK_','SAFSHypo'+hypocutEF),
      'EF_SA_FSBTRK2': mergeRemovingOverlap('EF_SA_FSBTRK2_','SAFSHypo'+hypocutEF),
      'EF_SAR_FSBTRK': mergeRemovingOverlap('EF_SAR_FSBTRK_','SAFSHypo'+hypocutEF),
      'EF_FStracksMuonBTRK': mergeRemovingOverlap('EF_FStracksMuonBTRK_', 'SAFSHypo'+hypocutEF),
      'EF_CB_FS_singleBTRK': mergeRemovingOverlap('EF_CB_FSBTRK_single_','SAFSHypo'+hypocutEF), 
      'EF_CB_FSBTRK': mergeRemovingOverlap('EF_CB_FSBTRK_', 'SAFSHypo'+hypocut+'_'+hypocutEF),
      'EF_CB_FSBTRK2': mergeRemovingOverlap('EF_CB_FSBTRK2_', 'SAFSHypo'+hypocut+'_'+hypocutEF),

      }
 
###########################################
## Adding new late-muon chain
###########################################

  def setup_muXX_MGOnly(self):
    ########### L2 algos  #################
    from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
    [trkfast, trkprec] = TrigInDetSequence("Muon", "muon", "IDTrig").getSequence()

    L2AlgName = self.getL2AlgName()
    muFastThresh = self.getMuFastThresh()

    #--- L2 algos ---
    if "l2muonSA" in self.chainPart['L2SAAlg']:
       theL2StandAloneAlg  = TrigL2MuonSAConfig(L2AlgName)
       theL2StandAloneHypo = MufastHypoConfig(L2AlgName, muFastThresh)
    else:
       log.error("Chain built with %s but so far only l2muonSA is supported." % (self.chainPart['L2SAAlg']))
       return False

    ########### EF algos  #################
    print self.chainPart['EFAlg']
    if 'SuperEF' in self.chainPart['EFAlg']:
      from AthenaCommon import CfgGetter
      theTrigMuSuperEF = CfgGetter.getAlgorithm("TrigMuSuperEF_MGonly")
      EFRecoAlgName = "Muon"
      EFCombinerThresh = self.getEFCombinerThresh()
      theEFAlg = theTrigMuSuperEF
    else:
      log.error("Chain built with %s but so far only SuperEF is supported." % (self.chainPart['EFAlg']))
      return False

    from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFCombinerHypoConfig
    theTrigMuonEFCombinerHypoConfig = TrigMuonEFCombinerHypoConfig(EFRecoAlgName,EFCombinerThresh)

    # L1 seeded from RoiMaker (seed from L1Topo) or Muon L1 Trigger + HLT chains configuration
    if "inTimeRoI" not in self.chainPart['addInfo']:
      from TrigmuRoI.TrigmuRoIConfig import TrigmuRoIConfig
      Roimaker = TrigmuRoIConfig("TrigMuRoIMGonly")
      self.L2sequenceList += [[ '', [Roimaker], 'L2_mu_step0']]
      self.L2sequenceList += [[ 'L2_mu_step0', [theL2StandAloneAlg], 'L2_mu_step1']]
    else:
      self.L2sequenceList += [[ self.L2InputTE, [theL2StandAloneAlg], 'L2_mu_step1']]

    self.L2sequenceList += [[ 'L2_mu_step1' , [theL2StandAloneHypo], 'L2_mu_hypo1']]
    self.EFsequenceList += [[ 'L2_mu_hypo1' , trkfast+trkprec, 'EF_mu_step1']]
    self.EFsequenceList += [[ 'EF_mu_step1' , [theEFAlg], 'EF_mu_step2']]
    self.EFsequenceList += [[ 'EF_mu_step2' , [theTrigMuonEFCombinerHypoConfig], 'EF_mu_step3']]

    if "inTimeRoI" not in self.chainPart['addInfo']:
      self.L2signatureList += [ [['L2_mu_step0']] ]

    self.L2signatureList += [ [['L2_mu_step1']] ]
    self.L2signatureList += [ [['L2_mu_hypo1']] ]
    self.EFsignatureList += [ [['EF_mu_step1']] ]
    self.EFsignatureList += [ [['EF_mu_step2']] ]
    self.EFsignatureList += [ [['EF_mu_step3']] ]

    if "inTimeRoI" not in self.chainPart['addInfo']:
      self.TErenamingDict = {
        'L2_mu_step0':  mergeRemovingOverlap('L2_mu_SA_MGOnly_',       "TrigMuRoIMGonly_L1x"+self.L2InputTE ),
        'L2_mu_step1':  mergeRemovingOverlap('L2_mu_SA_MGOnly_',       L2AlgName+"_L1x"+self.L2InputTE ),
        'L2_mu_hypo1':  mergeRemovingOverlap('L2_mu_SA_MGOnly_',       L2AlgName+"_"+muFastThresh+"_L1x"+self.L2InputTE ),
        'EF_mu_step1':  mergeRemovingOverlap('EF_SuperEF_MGOnly_L1x',  self.L2InputTE ),
        'EF_mu_step2':  mergeRemovingOverlap('EF_SuperEF_MGOnly_L1x',  self.chainPartNameNoMult),
        'EF_mu_step3':  mergeRemovingOverlap('EF_SuperEFHypo_MGOnly',  self.chainPartNameNoMult)
      }
    else:
      self.TErenamingDict = {
        'L2_mu_step1':  mergeRemovingOverlap('EF_SuperEF_MGOnly_inTimeRoI_',     L2AlgName+"_L1x"+self.L2InputTE ),
        'L2_mu_hypo1':  mergeRemovingOverlap('EF_SuperEF_MGOnly_inTimeRoI_',     L2AlgName+"_"+muFastThresh+"_L1x"+self.L2InputTE ),
        'EF_mu_step1':  mergeRemovingOverlap('EF_SuperEF_MGOnly_inTimeRoI_',     self.L2InputTE ),
        'EF_mu_step2':  mergeRemovingOverlap('EF_SuperEF_MGOnly_inTimeRoI_',     self.chainPartNameNoMult),
        'EF_mu_step3':  mergeRemovingOverlap('EF_SuperEFHypo_MGOnly_inTimeRoI',  self.chainPartNameNoMult)
      }

