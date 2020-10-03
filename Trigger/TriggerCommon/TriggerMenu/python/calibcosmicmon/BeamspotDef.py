# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

###########################################################################
# SliceDef file for Beamspot chains
###########################################################################

__author__  = 'M.Backes, C.Bernius'
__version__=""
__doc__="Implementation of beamspot chains "
from TriggerMenu.menu.HltConfig import L2EFChainDef, mergeRemovingOverlap

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
      self.L2InputTE = self.L2InputTE.replace("L1_","")

      if ('trkFS' in self.chainPart['addInfo']) \
            or ('activeTE' in self.chainPart['addInfo'] )\
            or ('allTE' in self.chainPart['addInfo'] )\
            or ('idperf' in self.chainPart['addInfo'] ):
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

     if 'peb'== self.eventBuildType:
       from TrigDetCalib.TrigDetCalibConfig import TrigSubDetListWriter
       robListWriter = TrigSubDetListWriter('IDSubDetListWriter')
       robListWriter.SubdetId = [ "SiOnly","TDAQ_CTP" ]
       robListWriter.addL1Calo = False                        
       theAlg = robListWriter       
     elif 'pebTRT'== self.eventBuildType:
       from TrigDetCalib.TrigDetCalibConfig import TrigSubDetListWriter
       robListWriter = TrigSubDetListWriter('IDTRTSubDetListWriter')
#       robListWriter.SubdetId = [ "SiOnly","TDAQ_CTP","TRT" ]
       robListWriter.SubdetId = [ "InnerDetector","TDAQ_CTP" ]
       robListWriter.addL1Calo = False                        
       theAlg = robListWriter              
     else: 
       from TrigGenericAlgs.TrigGenericAlgsConf import PrescaleAlgo
       theAlg = PrescaleAlgo("terminateAlgo")   

     if 'trkFS' in self.chainPart['addInfo'] :
        from TrigT2BeamSpot.T2VertexBeamSpotConfig import T2VertexBeamSpot_FTF
        theFex = T2VertexBeamSpot_FTF()
     elif 'activeTE' in self.chainPart['addInfo']:
        from TrigT2BeamSpot.T2VertexBeamSpotConfig import T2VertexBeamSpot_activeTE_FTF
        theFex = T2VertexBeamSpot_activeTE_FTF()
     elif 'allTE' in self.chainPart['addInfo']:
        from TrigT2BeamSpot.T2VertexBeamSpotConfig import T2VertexBeamSpot_activeAllTE_FTF
        theFex =  T2VertexBeamSpot_activeAllTE_FTF()
     else:
        mlog.error('Cannot assemble chain %s - only configured for trkFS,allTE and activeTE' % (self.chainPartName))

     from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
     [trk_alg] = TrigInDetSequence("BeamSpot", "beamSpot", "IDTrig", sequenceFlavour=["FTF"]).getSequence()
        
     from TrigGenericAlgs.GenericDummyUnseededAllTEAlgoConfig import GenericDummyUnseededAllTEAlgo 

     dummyAllTEAlgo = GenericDummyUnseededAllTEAlgo("L2DummyAlgo")
     
#     from RegionSelector.RegSelToolConfig import makeRegSelTool_Pixel
#     from RegionSelector.RegSelToolConfig import makeRegSelTool_SCT
#     dummyAllTEAlgo.RegSelTool_Pixel = makeRegSelTool_Pixel()
#     dummyAttTEAlgo.RegSelTool_SCT  = makeRegSelTool_SCT()

     self.L2sequenceList += [ [[""], [dummyAllTEAlgo]+trk_alg, 'L2_BeamSpottracks']]

     self.L2sequenceList +=[[['L2_BeamSpottracks'], [theFex], 'L2_fex']]
     self.L2sequenceList +=[[['L2_fex'], [theAlg], 'L2_']]  

     self.L2signatureList += [ [['L2_BeamSpottracks']] ]     
     self.L2signatureList += [ [['L2_fex']] ]
     self.L2signatureList += [ [['L2_']] ]

     self.TErenamingDict = {
       'L2_fex'           : mergeRemovingOverlap('L2_', self.chainName+'_fex'),        
       'L2_'              : mergeRemovingOverlap('L2_', self.chainName),        
       'L2_BeamSpottracks': mergeRemovingOverlap('L2_', self.chainName+'_BeamSpottracks'),        
       }    

     



  
