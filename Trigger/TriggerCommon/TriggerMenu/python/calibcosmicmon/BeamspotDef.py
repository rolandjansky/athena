# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###########################################################################
# SliceDef file for Beamspot chains
###########################################################################

__author__  = 'M.Backes, C.Bernius'
__version__=""
__doc__="Implementation of beamspot chains "
from TriggerMenu.menu.HltConfig import *
from AthenaCommon.Include import include
from AthenaCommon.SystemOfUnits import GeV

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
mlog = logging.getLogger("TriggerMenu.calibcosmic.BeamspotDef")



###########################################################################
class L2EFChain_Beamspot(L2EFChainDef):

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
      self.addInfo = self.chainPart['addInfo']
      self.l2IDAlg = self.chainPart['l2IDAlg']
      self.eventBuildType = self.chainPart['eventBuildType']

      self.L2InputTE = self.chainPartL1Item or self.chainL1Item

      if ('trkFS' in self.chainPart['addInfo']) \
            or ('activeTE' in self.chainPart['addInfo'] )\
            or ('allTE' in self.chainPart['addInfo'] ):
        self.setupBeamspotChains()
      else:
        mlog.error('Cannot assemble chain %s - not the right info provided!' % (self.chainPartName))


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
   # beamspot chains
   ###########################################################################
   def setupBeamspotChains(self):

     if ('peb' in self.eventBuildType):
       from TrigDetCalib.TrigDetCalibConfig import TrigSubDetListWriter
       robListWriter = TrigSubDetListWriter('IDSubDetListWriter')
       robListWriter.SubdetId = [ "SiOnly","TDAQ_CTP" ]
       robListWriter.addL1Calo = False                        
       theAlg = robListWriter       
     else: 
       from TrigGenericAlgs.TrigGenericAlgsConf import PrescaleAlgo
       theAlg = PrescaleAlgo("terminateAlgo")   

            
     if 'trkFS' in self.chainPart['addInfo'] :         
       from TrigT2BeamSpot.T2VertexBeamSpotConfig import T2VertexBeamSpot_L2StarB
       theFex = T2VertexBeamSpot_L2StarB()
     elif 'activeTE' in self.chainPart['addInfo']:         
       from TrigT2BeamSpot.T2VertexBeamSpotConfig import T2VertexBeamSpot_activeTE_L2StarB
       theFex = T2VertexBeamSpot_activeTE_L2StarB()
     elif 'allTE' in self.chainPart['addInfo']:         
       from TrigT2BeamSpot.T2VertexBeamSpotConfig import T2VertexBeamSpot_activeAllTE_L2StarB
       theFex =  T2VertexBeamSpot_activeAllTE_L2StarB()
     else:
       mlog.error('Cannot assemble chain %s - only configured for trkFS,allTE and activeTE' % (self.chainPartName))


     #commenting out because of ATR-8976 (removal of package)
     #if  ('L2StarB' in self.l2IDAlg):
     #  from TrigSiTrack.TrigSiTrack_Config import TrigSiTrack_BeamSpot
     #  TrigL2SiTrackFinder_Config = __import__('TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config', fromlist=[""])      
     #  trk_alg = getattr(TrigL2SiTrackFinder_Config, "TrigL2SiTrackFinder_BeamSpotB") 
     #  teaddition = 'L2StarB'
     #else:
     #  mlog.error('Cannot assemble chain %s - only configured for L2StarB' % (self.chainPartName))
         

     self.L2sequenceList +=[[ self.L2InputTE, [theFex], 'L2_fex']]
     self.L2sequenceList +=[[['L2_fex'], [theAlg], 'L2_']]  

     #from TrigGenericAlgs.TrigGenericAlgsConf import  PESA__DummyUnseededAllTEAlgo
     #self.L2sequenceList += [ [[""], [PESA__DummyUnseededAllTEAlgo("L2DummyAlgo"), trk_alg()], 'L2_BeamSpottracks']]
     
     self.L2signatureList += [ [['L2_fex']] ]
     self.L2signatureList += [ [['L2_']] ]
     #self.L2signatureList += [ [['L2_BeamSpottracks']] ]
     
     self.TErenamingDict = {
       'L2_fex'           : mergeRemovingOverlap('L2_', self.chainName+'_fex'),        
       'L2_'              : mergeRemovingOverlap('L2_', self.chainName),        
       #'L2_BeamSpottracks': mergeRemovingOverlap('L2_', 'BeamSpottracks'+teaddition),        
       }    

     



  
