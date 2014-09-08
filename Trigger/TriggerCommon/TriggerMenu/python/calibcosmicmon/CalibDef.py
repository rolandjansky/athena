# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###########################################################################
# SliceDef file for TrkCalib chains
###########################################################################

__author__  = 'M.Backes, C.Bernius, O.Igonkina, P.Bell'
__version__=""
__doc__="Implementation of calib trigger sequence "
from TriggerMenu.menu.HltConfig import *
from AthenaCommon.Include import include
from AthenaCommon.SystemOfUnits import GeV

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
mlog = logging.getLogger("TriggerMenu.calibcosmic.CalibDef")


###########################################################################
# !!!!HARDCODED ROI TO SEED TRKCALIB CHAINS OFF!!!!
###########################################################################

roi1  = 'HA8'    

from TrigDetCalib.TrigDetCalibConfig import *
#from TrigSteeringTest.TrigSteeringTestConf import PESA__dummyAlgo
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
           'L1_FJ30_EMPTY':      ['JF30'],
           }
	
  if item in L1Map:
    return L1Map[item]
  else: 
    TE = item.replace("L1_","")
    TE = TE.split("_")[0]
    TE = TE[1:] if TE[0].isdigit() else TE
    return TE


###########################################################################
class L2EFChain_CalibTemplate(L2EFChainDef):

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
      self.chainName = chainDict['chainName']
      self.chainPartName = self.chainPart['chainPartName']	       
      self.L2InputL1Item = self.chainPartL1Item or self.chainL1Item
      self.hypo = self.chainPart['hypo']
      self.location = self.chainPart['location']

      self.L2InputTE = self.chainPartL1Item or self.chainL1Item
      if self.L2InputL1Item:
        self.L2InputTE = getInputTEfromL1Item(self.L2InputL1Item)
      else:
        self.L2InputTE = ''
	

      #if len(self.L2InputTE)>0:
      #   self.L2InputTE = self.L2InputTE.replace("L1_","")
      #   self.L2InputTE = self.L2InputTE.split("_")[0]
      #   self.L2InputTE = self.L2InputTE[1:] if self.L2InputTE[0].isdigit() else self.L2InputTE
      

      if 'idcalib' in self.chainPart['purpose']:         
         self.L2InputTE = roi1
         self.setupTrkCalibChains()
      elif ('larcalib' in self.chainPart['purpose']) or ('tilelarcalib' in self.chainPart['purpose']):
         self.setupLArROBListWWriterLvl1Chain()
      elif 'l1calocalib' in self.chainPart['purpose']:
         self.L2InputTE = self.L2InputL1Item
         self.setupL1CaloCalibrationChains()
      #elif '
      else:
         mlog.error('Chain %s could not be assembled' % (self.chainPartName))
         return False      


      L2EFChainDef.__init__(self, self.chainName, self.L2Name, self.chainCounter,
                            self.chainL1Item, self.EFName, self.chainCounter, self.L2InputTE)

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
      self.AlgoName = self.hypo+'_'+self.location
      self.AlgList = []


      from TrigDetCalib.TrigDetCalibConfig import *
      trkAlgDict = {
         'trk9_central'  : CheckForTracks_Trk9_Central('CheckForTracks_Trk9_Central'),
         'trk16_central' : CheckForTracks_Trk16_Central('CheckForTracks_Trk16_Central'),
         'trk29_central' : CheckForTracks_Trk29_Central('CheckForTracks_Trk29_Central'),
         'trk9_fwd'      : CheckForTracks_Trk9_Fwd('CheckForTracks_Trk9_Fwd'),
         'trk16_fwd'     : CheckForTracks_Trk16_Fwd('CheckForTracks_Trk16_Fwd'),
         'trk29_fwd'     : CheckForTracks_Trk29_Fwd('CheckForTracks_Trk29_Fwd'),
         }
      for name, alg in trkAlgDict.items():
         if self.AlgoName == name:
            self.AlgList = [alg]
        
      if len(self.AlgList) == 0:
         mlog.error('Chain %s could not be assembled' % (self.chainPartName))
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
        mlog.error('Cannot find the right sequence for this chain.')
        Tespecifier=''

      self.L2signatureList += [ [['L2_step1']] ]
      self.L2signatureList += [ [['L2_step2']] ]

      self.TErenamingDict = {
        'L2_step1': mergeRemovingOverlap('L2_', 'Cosmic_'+self.chainName),
        'L2_step2': mergeRemovingOverlap('L2_', 'Cosmic_'+self.chainName+Tespecifier),        
        }


