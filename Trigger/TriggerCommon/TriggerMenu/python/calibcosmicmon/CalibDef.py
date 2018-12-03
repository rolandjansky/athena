# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###########################################################################
# SliceDef file for TrkCalib chains
###########################################################################

__author__  = 'M.Backes, C.Bernius, O.Igonkina, P.Bell'
__version__=""
__doc__="Implementation of calib trigger sequence "

from AthenaCommon.SystemOfUnits import GeV
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

from TriggerMenu.menu.HltConfig import L2EFChainDef, mergeRemovingOverlap
from TriggerJobOpts.TriggerFlags import TriggerFlags

from TrigDetCalib.TrigDetCalibConfig import (LArL2ROBListWriter, 
                                             TileROBSelector,
                                             CSCSubDetListWriter,
                                             L2ROBListWriter)
from TriggerMenu.commonUtils.makeCaloSequences import (getFullScanCaloSequences) 
###########################################################################
# Helper classes
###########################################################################
class L1CaloTileHackL2ROBListWriter(L2ROBListWriter):
  __slots__ = []
  def __init__(self, name = "L1CaloTileHackL2ROBListWriter"):
    super( L1CaloTileHackL2ROBListWriter, self ).__init__( name )
    self.MaxRoIsPerEvent = 5
    self.RoIDescriptorLabels = [ "TrigT2CaloEgamma", "initialRoI" ]
    self.extraROBs += [
      7602188, 7602204, 7602220, 7602236, 7602189, 7602205, 7602221, 7602237,
      7471112, 7471144, 7471113, 7471145, 7471114, 7471146, 7471115, 7471147,
      7405574, 7405590, 7405606, 7405622, 7405575, 7405591, 7405607, 7405623,
      7405571, 7405587, 7405603, 7405619, 7405572, 7405588, 7405604, 7405620,
      7405573, 7405589, 7405605, 7405621, 7405568, 7405584, 7405600, 7405616,
      7405569, 7405585, 7405601, 7405617, 7405570, 7405586, 7405602, 7405618,
      ]
  
  def setDefaults(self, handle):
    if hasattr( handle, 'name' ) :
      handle.ROBSelector = TileROBSelector("TileROBSelector_L2")

	
###################################################################################
def getInputTEfromL1Item(item):
  L1Map = {'L1_CALREQ2':        ['NIM30'],
           #'L1_RD0_EMPTY':      [''],
           'L1_TAU8_EMPTY':      ['HA8'],
           'L1_TAU12_EMPTY':      ['HA12'],
           'L1_FJ30_EMPTY':      ['JF30'],
           }
	
  if item in L1Map:
    return L1Map[item]
  else: 
    TE = item.split(",")[0]
    TE = TE.replace("L1_","")
    TE = TE.split("_")[0]
#    print "and now I have TE:"
#    print TE
    TE = TE[1:] if TE[0].isdigit() else TE
#    print "returning: " + TE
    return TE


###########################################################################
class L2EFChain_CalibTemplate(L2EFChainDef):

   def __init__(self, chainDict):
      self.L2sequenceList   = []
      self.EFsequenceList   = []
      self.L2signatureList  = []
      self.EFsignatureList  = []
      self.TErenamingDict   = []
      
      self.chainDict = chainDict
      self.chainPart = chainDict['chainParts']	
      self.chainL1Item = chainDict['L1item']       
      self.chainPartL1Item = self.chainPart['L1item']	
      self.chainCounter = chainDict['chainCounter']       
      self.L2Name = 'L2_'+self.chainPart['chainPartName']
      self.EFName = 'EF_'+self.chainPart['chainPartName']
      self.chainName = chainDict['chainName']
      self.chainPartName = self.chainPart['chainPartName']	       
      self.L2InputL1Item = self.chainPartL1Item or self.chainL1Item
      self.hypo = self.chainPart['hypo']
      self.location = self.chainPart['location']
      self.mult = int(self.chainPart['multiplicity'])   
      self.chainPartNameNoMult = self.chainPartName[1:] if self.mult > 1 else self.chainPartName 
      self.signatureCounterOffset = 0
      
      self.L2InputTE = self.chainPartL1Item or self.chainL1Item
      if self.L2InputL1Item:
        self.L2InputTE = getInputTEfromL1Item(self.L2InputL1Item)
      else:
        self.L2InputTE = ''


	

      #if len(self.L2InputTE)>0:
      #   self.L2InputTE = self.L2InputTE.replace("L1_","")
      #   self.L2InputTE = self.L2InputTE.split("_")[0]
      #   self.L2InputTE = self.L2InputTE[1:] if self.L2InputTE[0].isdigit() else self.L2InputTE
      #roi1 = 'HA8'
      if 'idcalib' in self.chainPart['purpose']:         
        if not 'HI_' in TriggerFlags.triggerMenuSetup():
          roi1 = 'HA8'
          #log.info('Using '+roi1+' as ROI for calibtrk chains, triggered by non-HI menuname)
          self.L2InputTE = roi1
        self.setupTrkCalibChains()
      elif 'ibllumi' in self.chainPart['purpose']:
        self.setupIBLLumiChains()
      elif ('larcalib' in self.chainPart['purpose']) or ('tilelarcalib' in self.chainPart['purpose']):
         self.setupLArROBListWWriterLvl1Chain()
      elif 'l1calocalib' in self.chainPart['purpose']:
         self.L2InputTE = self.L2InputL1Item
         self.setupL1CaloCalibrationChains()
      elif 'alfacalib' in self.chainPart['purpose']:
        self.setupL1ALFACalibrationChains()
      elif 'larnoiseburst' in self.chainPart['purpose']:
        self.setupLarNoiseBurstChains()
      elif 'lumipeb' in self.chainPart['purpose']:
        self.setupLumiPEBChains()
      elif 'lhcfpeb' in self.chainPart['purpose']:
        self.setupLHCfChains()
      elif 'alfaidpeb' in self.chainPart['purpose']:
        self.setupALFAIDChains()
      elif 'larpebj' in self.chainPart['purpose']:
        self.setupLarPEBChains()
      elif 'l1satmon' in self.chainPart['purpose']:
        self.setupL1SaturatedMon()
      elif 'zdcpeb' in self.chainPart['purpose']:
        self.setupZDCPEBChains()
      elif 'calibAFPALFA' in self.chainPart['purpose']:
        self.setupAFPALFACalibrationChains()
      elif 'calibAFP' in self.chainPart['purpose']:
        self.setupAFPCalibrationChains()
      elif 'rpcpebsecondaryreadout' in self.chainPart['purpose']:
        self.setupRPCSecondaryReadoutChains()
      elif 'rpcpeb' in self.chainPart['purpose']:
        self.setupRPCCalibrationChains()
      elif 'idpsl1' in self.chainPart['purpose']:
        self.setupIDPSCalibrationChains()
      elif 'larpebcalib' in self.chainPart['purpose']:
        self.setupLArPEBCalibCalibrationChains()
      else:
         log.error('Chain %s could not be assembled' % (self.chainPartName))
         return False      

      L2EFChainDef.__init__(self, self.chainName, self.L2Name, self.chainCounter,
                            self.chainL1Item, self.EFName, self.chainCounter, self.L2InputTE, signatureCounterOffset=self.signatureCounterOffset)

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
      self.TErenamingMap=self.TErenamingDict


   ###########################################################################
   # IBL luminosity chains
   ###########################################################################
   def setupIBLLumiChains(self):
      
      from TrigDetCalib.TrigDetCalibConfig import IBLSubDetListWriter
      l2_iblSubDetListWriter = IBLSubDetListWriter("IBLSubDetListWriter")
      l2_iblSubDetListWriter.Subdetectors = "IBL"
      l2_iblSubDetListWriter.MaxRoIsPerEvent = 1
      self.iblWriter = [l2_iblSubDetListWriter]                

      self.L2sequenceList += [[ '', self.iblWriter, 'L2_' ]]
      self.L2signatureList += [[['L2_']]]
      self.TErenamingDict = {
         'L2_':     'L2_ibllumi',
         }

   ###########################################################################
   # L1CaloCalibration chains
   ###########################################################################
   def setupL1CaloCalibrationChains(self):
      
      from TrigDetCalib.TrigDetCalibConfig import L1CaloSubDetListWriter
      l2_l1caloSubDetListWriter = L1CaloSubDetListWriter("L1CaloSubDetListWriter")
      l2_l1caloSubDetListWriter.Subdetectors = "L1Calo"
      l2_l1caloSubDetListWriter.MaxRoIsPerEvent = 1
      l2_l1caloSubDetListWriter.addL1Calo = True        
      self.l1caloWriter = [l2_l1caloSubDetListWriter]                

      self.L2sequenceList += [[ '', self.l1caloWriter, 'L2_']]
      #self.L2sequenceList += [[self.L2InputTE, self.l1caloWriter, 'L2_']]
      self.L2signatureList += [[['L2_']]]
      self.TErenamingDict = {
         'L2_':     'L2_l1calocalib',
         }

   ###########################################################################
   # TrkCalibChains
   ###########################################################################
   def setupTrkCalibChains(self):
      #self.AlgoName = self.hypo+'_'+self.location
      self.AlgoName = self.chainName
      self.AlgList = []
      self.signatureCounterOffset = 14

      from TrigDetCalib.TrigDetCalibConfig import (CheckForTracks_Trk9_Central,
                                                   CheckForTracks_Trk16_Central,
                                                   CheckForTracks_Trk29_Central,
                                                   CheckForTracks_Trk9_Fwd,
                                                   CheckForTracks_Trk16_Fwd,
                                                   CheckForTracks_Trk29_Fwd,
                                                   CheckForTracks_Trk9_Central_Beamspot,
                                                   CheckForTracks_Trk9_Fwd_Beamspot) 

      trkAlgDict = {
         'idcalib_trk9_central'  : CheckForTracks_Trk9_Central('CheckForTracks_Trk9_Central'),
         'idcalib_trk16_central' : CheckForTracks_Trk16_Central('CheckForTracks_Trk16_Central'),
         'idcalib_trk29_central' : CheckForTracks_Trk29_Central('CheckForTracks_Trk29_Central'),
         'idcalib_trk9_fwd'      : CheckForTracks_Trk9_Fwd('CheckForTracks_Trk9_Fwd'),
         'idcalib_trk16_fwd'     : CheckForTracks_Trk16_Fwd('CheckForTracks_Trk16_Fwd'),
         'idcalib_trk29_fwd'     : CheckForTracks_Trk29_Fwd('CheckForTracks_Trk29_Fwd'),
         'idcalib_trk9_central_L1J12_VTE100'  : CheckForTracks_Trk9_Central_Beamspot('CheckForTracks_Trk9_Central_Beamspot_1'),
         'idcalib_trk9_fwd_L1J12_VTE100'  : CheckForTracks_Trk9_Fwd_Beamspot('CheckForTracks_Trk9_Fwd_Beamspot_1'),
         'idcalib_trk9_central_L1J12_VTE200'  : CheckForTracks_Trk9_Central_Beamspot('CheckForTracks_Trk9_Central_Beamspot_2'),
         'idcalib_trk9_fwd_L1J12_VTE200'  : CheckForTracks_Trk9_Fwd_Beamspot('CheckForTracks_Trk9_Fwd_Beamspot_2'),
         'idcalib_trk9_central_L1J10_VTE100'  : CheckForTracks_Trk9_Central_Beamspot('CheckForTracks_Trk9_Central_Beamspot_1'),
         'idcalib_trk9_fwd_L1J10_VTE100'  : CheckForTracks_Trk9_Fwd_Beamspot('CheckForTracks_Trk9_Fwd_Beamspot_1'),
         'idcalib_trk9_central_L1J10_VTE200'  : CheckForTracks_Trk9_Central_Beamspot('CheckForTracks_Trk9_Central_Beamspot_2'),
         'idcalib_trk9_fwd_L1J10_VTE200'  : CheckForTracks_Trk9_Fwd_Beamspot('CheckForTracks_Trk9_Fwd_Beamspot_2'),
         'idcalib_trk9_central_bs'  : CheckForTracks_Trk9_Central_Beamspot('CheckForTracks_Trk9_Central_Beamspot_1'),
         'idcalib_trk9_fwd_bs'  : CheckForTracks_Trk9_Fwd_Beamspot('CheckForTracks_Trk9_Fwd_Beamspot_1'),
         'idcalib_trk9_central_L1MBTS_1_VTE70'  : CheckForTracks_Trk9_Central_Beamspot('CheckForTracks_Trk9_Central_Beamspot_1'),
         'idcalib_trk9_fwd_L1MBTS_1_VTE70'  : CheckForTracks_Trk9_Fwd_Beamspot('CheckForTracks_Trk9_Fwd_Beamspot_1'),

         }
      for name, alg in trkAlgDict.items():
         if self.AlgoName == name:
            self.AlgList = [alg]
        
      if len(self.AlgList) == 0:
         log.error('Chain %s could not be assembled' % (self.chainPartName))
         return False      

      self.L2sequenceList += [[self.L2InputTE, self.AlgList, 'L2_']]
      self.L2signatureList += [[['L2_']]]
      self.TErenamingDict = {
         'L2_':     'L2_' + self.chainName,
         }

   ###########################################################################
   # LArCalibChains
   ###########################################################################
   def setupLArROBListWWriterLvl1Chain(self):
      self.L2sequenceList += [[ self.L2InputTE, 
                                [LArL2ROBListWriter('LArL2ROBListWriter_' + self.chainName, addCTPResult = True, addL2Result = True),
                                 L1CaloTileHackL2ROBListWriter('L1CaloTileHackL2ROBListWriter_' + self.chainName)],
                                'L2_step1']]

      if ('larcalib' in self.chainName):
        self.L2sequenceList += [[ ['L2_step1'], [CSCSubDetListWriter('CSCSubDetListWriter_' + self.chainName)], 'L2_step2']]
        Tespecifier='CSCsubdetlistwriter'
      elif ('tilelarcalib' in self.chainName):
        from TrigDetCalib.TrigDetCalibConfig import TileSubDetListWriter
        self.l2_tileSubDetListWriter = TileSubDetListWriter("L2_Cosmic_"+self.chainName+"_TileSubDetListWriter")
        self.l2_tileSubDetListWriter.Subdetectors = "Tile"
        self.l2_tileSubDetListWriter.MaxRoIsPerEvent = 1
        self.l2_tileSubDetListWriter.addL1Calo = True
        self.L2sequenceList += [[ ['L2_step1'], [self.l2_tileSubDetListWriter], 'L2_step2']]
        Tespecifier='Tilesubdetlistwriter'
      else:
        log.error('Cannot find the right sequence for this chain.')
        Tespecifier=''

      self.L2signatureList += [ [['L2_step1']] ]
      self.L2signatureList += [ [['L2_step2']] ]

      self.TErenamingDict = {
        'L2_step1': mergeRemovingOverlap('L2_', 'Cosmic_'+self.chainName),
        'L2_step2': mergeRemovingOverlap('L2_', 'Cosmic_'+self.chainName+Tespecifier),        
        }



   ###########################################################################
   # ALFA Calibration chains
   ###########################################################################
   def setupL1ALFACalibrationChains(self):
     
     from TrigDetCalib.TrigDetCalibConfig import TrigSubDetListWriter
     
     l2_ALFASubDetListWriter = TrigSubDetListWriter("ALFASubDetListWriter")
     l2_ALFASubDetListWriter.SubdetId = ['TDAQ_CTP','FORWARD_ALPHA']
     l2_ALFASubDetListWriter.MaxRoIsPerEvent=1
     
     self.robWriter = [l2_ALFASubDetListWriter]            
     self.L2sequenceList += [['', self.robWriter, 'L2_']]
     
     self.L2signatureList += [[['L2_']]]
     self.TErenamingDict = {
       'L2_':     'L2_l1ALFAcalib',
       }


   ###########################################################################
   # RPC Calibration chains
   ###########################################################################
   def setupRPCCalibrationChains(self):
     
     from TrigDetCalib.TrigDetCalibConfig import TrigSubDetListWriter
     
     l2_RPCSubDetListWriter = TrigSubDetListWriter("RPCSubDetListWriter")
     l2_RPCSubDetListWriter.SubdetId = ['TDAQ_MUON', 'TDAQ_CTP', 'TDAQ_HLT', 'RPC']
     l2_RPCSubDetListWriter.extraROBs += [0x610080, 0x620080]

     l2_RPCSubDetListWriter.MaxRoIsPerEvent=1
     
     self.robWriter = [l2_RPCSubDetListWriter]            
     self.L2sequenceList += [['', self.robWriter, 'L2_']]
     
     self.L2signatureList += [[['L2_']]]
     self.TErenamingDict = {
       'L2_':     'L2_l1RPCcalib',
       }

   def setupRPCSecondaryReadoutChains(self):
     
     from TrigDetCalib.TrigDetCalibConfig import TrigSubDetListWriter
     
     l2_RPCSubDetListWriter = TrigSubDetListWriter("RPCSecondaryListWriter")
     l2_RPCSubDetListWriter.extraROBs = [0x610080, 0x620080]

     l2_RPCSubDetListWriter.MaxRoIsPerEvent=1
     
     self.robWriter = [l2_RPCSubDetListWriter]            
     self.L2sequenceList += [['', self.robWriter, 'L2_']]
     
     self.L2signatureList += [[['L2_']]]
     self.TErenamingDict = {
       'L2_':     'L2_l1RPCsecondarycalib',
       }
  
   ###########################################################################
   # IDprescaledL1 Calibration chains
   ###########################################################################
   def setupIDPSCalibrationChains(self):

     from TrigDetCalib.TrigDetCalibConfig import TrigSubDetListWriter

     l2_IDPSSubDetListWriter = TrigSubDetListWriter("IDPSSubDetListWriter")
     l2_IDPSSubDetListWriter.SubdetId = ['TDAQ_CTP','TDAQ_HLT','InnerDetector']

     l2_IDPSSubDetListWriter.MaxRoIsPerEvent=1

     self.robWriter = [l2_IDPSSubDetListWriter]
     self.L2sequenceList += [['', self.robWriter, 'L2_']]

     self.L2signatureList += [[['L2_']]]
     self.TErenamingDict = {
       'L2_':     'L2_l1IDPScalib',
       }


   ###########################################################################
   # AFP Calibration chains
   ###########################################################################
   def setupAFPCalibrationChains(self):
     
     from TrigDetCalib.TrigDetCalibConfig import TrigSubDetListWriter
     
     l2_AFPSubDetListWriter = TrigSubDetListWriter("AFPSubDetListWriter")
     l2_AFPSubDetListWriter.SubdetId = ['TDAQ_CTP','FORWARD_AFP']
     l2_AFPSubDetListWriter.MaxRoIsPerEvent=1
     
     self.robWriter = [l2_AFPSubDetListWriter]            
     self.L2sequenceList += [['', self.robWriter, 'L2_']]
     
     self.L2signatureList += [[['L2_']]]
     self.TErenamingDict = {
       'L2_':     'L2_l1AFPcalib',
       }
     
   ###########################################################################
   # AFP+ALFA Calibration chains
   ###########################################################################
   def setupAFPALFACalibrationChains(self):
     
     from TrigDetCalib.TrigDetCalibConfig import TrigSubDetListWriter
     
     l2_AFPALFASubDetListWriter = TrigSubDetListWriter("AFPALFASubDetListWriter")
     l2_AFPALFASubDetListWriter.SubdetId = ['TDAQ_CTP','FORWARD_AFP','FORWARD_ALPHA']
     l2_AFPALFASubDetListWriter.MaxRoIsPerEvent=1
     
     self.robWriter = [l2_AFPALFASubDetListWriter]            
     self.L2sequenceList += [['', self.robWriter, 'L2_AFPALFAcalib']]
     self.L2signatureList += [[['L2_AFPALFAcalib']]]
     
   ###########################################################################
   # LarNoiseBurst chains
   ###########################################################################
   def setupLarNoiseBurstChains(self):
     
     fullScanSeqMap = getFullScanCaloSequences()
     from TrigCaloHypo.TrigCaloHypoConfig import EFCaloHypoNoiseConfig

     if "loose" in self.chainPart['addInfo']:
       theCaloHypo = EFCaloHypoNoiseConfig("EFCaloHypoNoiseConfigLoose")
       theCaloHypo.NoiseTool.BadChanPerFEB=1
       theCaloHypo.NoiseTool.CellQualityCut=100
     else:
       theCaloHypo = EFCaloHypoNoiseConfig()


     self.EFsequenceList += [['',fullScanSeqMap['EF_full'],'EF_full']]
     self.EFsequenceList += [[['EF_full'],fullScanSeqMap['EF_full_cell'],'EF_full_cell']]
     self.EFsequenceList += [[['EF_full_cell'],[theCaloHypo], 'jet_hypo']]

     self.L2signatureList += [ [['EF_full']] ]
     self.EFsignatureList += [ [['EF_full_cell']] ]
     self.EFsignatureList += [ [['jet_hypo']] ]

     antiktsize = 0
     
     suffix = "_loose" if "loose" in self.chainPart['addInfo'] else ""

     self.TErenamingDict = { 
       'EF_full_cell' : mergeRemovingOverlap('HLT_full__cluster__', 'jr_antikt'+str(antiktsize)+'tc_had' ), 
       'jet_hypo' : mergeRemovingOverlap('HLT_full__cluster__', 'jr_antikt'+str(antiktsize)+'tc_had_noiseHypo'+suffix ), 
       }

   ###########################################################################
   # LarPEB chains
   ###########################################################################
   def setupLarPEBChains(self):

     threshold=self.chainName.split("_")[0].replace("conej","")
     from TrigT2CaloJet.TrigT2CaloJetConfig import T2CaloJet_Jet_noCalib_noiseCut
     from TrigCaloHypo.TrigCaloHypoConfig import L2JetHypo
     from TrigDetCalib.TrigDetCalibConfig import EtaHypo_HEC
     from TrigDetCalib.TrigDetCalibConfig import EtaHypo_FCAL
     from TrigDetCalib.TrigDetCalibConfig import LArL2ROBListWriter
     theL2JetFex = T2CaloJet_Jet_noCalib_noiseCut()
     theL2JetHypo = L2JetHypo('L2JetHypo_j'+threshold,l2_thr=int(threshold)*GeV)
     theEtaHypo = EtaHypo_FCAL('EtaHypo_FCAL_fj'+threshold+'_larcalib') if "31ETA49" in self.L2InputTE else EtaHypo_HEC('EtaHypo_HEC_fj'+threshold+'_larcalib')
     theLArL2ROB = LArL2ROBListWriter('LArL2ROBListWriter_larcalib_j'+threshold)
     self.L2sequenceList += [[self.L2InputTE,
                              [theL2JetFex],
                              'L2_larpeb_step1']]
     self.L2sequenceList += [[['L2_larpeb_step1'],[theL2JetHypo,theEtaHypo],'L2_larpeb_step2']]
     self.L2sequenceList += [[['L2_larpeb_step2'],[theLArL2ROB],'L2_larpeb_step3']]
     self.L2signatureList += [ [['L2_larpeb_step1']*self.mult] ]
     self.L2signatureList += [ [['L2_larpeb_step2']*self.mult] ]
     self.L2signatureList += [ [['L2_larpeb_step3']*self.mult] ]
     self.TErenamingDict = { 'L2_larpeb_step1': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'_'+self.L2InputTE+'_jetCalo'),
                             'L2_larpeb_step2': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'_'+self.L2InputTE+'_jetCaloHypo'),
                             'L2_larpeb_step3': mergeRemovingOverlap('L2_', self.chainPartNameNoMult+'_'+self.L2InputTE+'_jetEtaHypo'), 
                             }
     
   ###########################################################################
   # L1Saturated Monitored chains
   ###########################################################################
   def setupL1SaturatedMon(self):

     from TrigCaloRec.TrigCaloRecConf import TrigL1BSTowerMaker
     from TrigCaloRec.TrigCaloRecConfig import TrigL1BSTowerHypoConfig
     theL1BS = TrigL1BSTowerMaker()
     theL1BSHypo = TrigL1BSTowerHypoConfig('TrigL1BSTowerHypoConfig_'+self.chainName.replace(".",""))
     self.L2sequenceList += [[self.L2InputTE,
                              [theL1BS],
                              'L2_l1bs_step1']]
     self.L2sequenceList += [[['L2_l1bs_step1'],[theL1BSHypo],'L2_l1bs_step2']]
     self.L2signatureList += [ [['L2_l1bs_step1']*self.mult] ]
     self.L2signatureList += [ [['L2_l1bs_step2']*self.mult] ]

     self.TErenamingDict = { 'L2_l1bs_step1':  mergeRemovingOverlap('L2_', self.chainName.replace(".","")+'_l1bs'),
                             'L2_l1bs_step2':  mergeRemovingOverlap('L2_', self.chainName.replace(".","")+'_l1bsHypo'),
     }


   ###########################################################################
   # VdM chains
   ###########################################################################
   def setupLumiPEBChains(self):
          
     from TrigDetCalib.TrigDetCalibConfig import TrigSubDetListWriter
     
     vdmSubDetListWriter = TrigSubDetListWriter("VdMSubDetListWriter")
     vdmSubDetListWriter.SubdetId = ['TDAQ_HLT','TDAQ_CTP','SiOnly','DBM']
     vdmSubDetListWriter.MaxRoIsPerEvent=1

     self.robWriter = [vdmSubDetListWriter]            
     self.L2sequenceList += [['', self.robWriter, 'L2_vdm']]     
     self.L2signatureList += [[['L2_vdm']]]



   ###########################################################################
   # LHCf chains
   ###########################################################################
   def setupLHCfChains(self):
          
     from TrigDetCalib.TrigDetCalibConfig import TrigSubDetListWriter
     
     lhcfSubDetListWriter = TrigSubDetListWriter("LHCfSubDetListWriter")
     lhcfSubDetListWriter.SubdetId = ['TDAQ_HLT','TDAQ_CTP','InnerDetector','DBM','FORWARD_ALPHA','FORWARD_LUCID','FORWARD_ZDC','FORWARD_BCM']
     lhcfSubDetListWriter.MaxRoIsPerEvent=1

     self.robWriter = [lhcfSubDetListWriter]            
     self.L2sequenceList += [['', self.robWriter, 'L2_lhcf']]     
     self.L2signatureList += [[['L2_lhcf']]]



   ###########################################################################
   # ALFA+ID chains
   ###########################################################################
   def setupALFAIDChains(self):
          
     from TrigDetCalib.TrigDetCalibConfig import TrigSubDetListWriter
     
     alfaidSubDetListWriter = TrigSubDetListWriter("ALFAIDSubDetListWriter")
     alfaidSubDetListWriter.SubdetId = ['TDAQ_CTP','InnerDetector','FORWARD_ALPHA']
     alfaidSubDetListWriter.MaxRoIsPerEvent=1

     self.robWriter = [alfaidSubDetListWriter]            
     self.L2sequenceList += [['', self.robWriter, 'L2_alfaid']]     
     self.L2signatureList += [[['L2_alfaid']]]

#####################################################################
   def setupZDCPEBChains(self):
     from TrigDetCalib.TrigDetCalibConfig import TrigSubDetListWriter
     zdcSubDetListWriter = TrigSubDetListWriter("ZDCSubDetListWriter")
     zdcSubDetListWriter.SubdetId = ['TDAQ_CTP','FORWARD_ZDC'] 
     zdcSubDetListWriter.MaxRoIsPerEvent=1
 
     self.robWriter = [zdcSubDetListWriter]            
     self.L2sequenceList += [['', self.robWriter, 'L2_zdc']]     
     self.L2signatureList += [[['L2_zdc']]]

######################################################################
   def setupLArPEBCalibCalibrationChains(self):
     from TrigDetCalib.TrigDetCalibConfig import TrigSubDetListWriter
     larpebSubDetListWriter = TrigSubDetListWriter("LArPEBSubDetListWriter")
     larpebSubDetListWriter.SubdetId = ['TDAQ_CTP','LAr']

     larpebSubDetListWriter.MaxRoIsPerEvent=1

     self.robWriter = [larpebSubDetListWriter]
     self.L2sequenceList += [['', self.robWriter, 'L2_larpeb']]
     self.L2signatureList += [[['L2_larpeb']]]
 
