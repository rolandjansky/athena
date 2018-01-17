# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
#  provisionally a copy of InDetRecExample
#

from AthenaCommon.Include import include

include.block("InDetTrigRecExample/EFInDetConfig.py")

include("InDetTrigRecExample/InDetTrigRec_jobOptions.py") # this is needed to get InDetTrigFlags

from AthenaCommon.Logging import logging 
log = logging.getLogger("TrigInDetSequence.py")

from InDetTrigRecExample.InDetTrigConfigRecPreProcessing import *

from InDetTrigRecExample.InDetTrigConfigRecNewTracking import \
     SiTrigTrackFinder_EF, SiTrigTrackSeededFinder_EF, SiTrigSimpleTrackFinder_EF, \
     TrigAmbiguitySolver_EF,TRTTrackExtAlg_EF,\
     TrigExtProcessor_EF, InDetTrigDetailedTrackTruthMaker_EF

from InDetTrigRecExample.InDetTrigConfigRecNewTracking import SimpleTrigTrackCollMerger_EF
  
from InDetTrigRecExample.InDetTrigConfigRecBackTracking import *
from InDetTrigRecExample.InDetTrigConfigRecPostProcessing import *

from InDetTrigRecExample.InDetTrigRecLowPtTracking \
     import SiTrigSpacePointFinderLowPt_EF, SiTrigTrackFinderLowPt_EF, \
     TrigAmbiguitySolverLowPt_EF

try:
  from TrigFTK_RecAlgs.TrigFTK_RecAlgs_Config import TrigFTK_VxPrimary_EF
except:
  pass

try:
  from TrigInDetConf.HypoAlgCfgble import TrigFTKAvailable
except:
  log.info("Could not import TrigFTKAvailable")
  pass

try:
  from TrigInDetConf.TrackingAlgCfgble import FTK_TrackMaker
except:
  log.info("Could not import FTK_TrackMaker")
  pass


from TrigInDetConf.RoiManipulators import IDTrigRoiUpdater
from TrigInDetConf.TrackingAlgCfgble import TrigFastTrackFinder

from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags

def vertexXAODCnvNeeded():
  # temporary check whether the release have a new version of TrigVxPrimary capable of producing 
  # xAOD::Vertex natively

  from InDetTrigPriVxFinder.InDetTrigPriVxFinderConf import InDet__TrigVxPrimary
  a = InDet__TrigVxPrimary("dummy")
  return not hasattr(a, "BeamCondSvc")     


class TrigInDetSequenceBase:
  """
  """
  def __init__(self):
    self.__sequence__ = list()
  def getSequence(self):
    return self.__sequence__

class TrigInDetSequence(TrigInDetSequenceBase):
  """

  Create InDet reconstruction sequences from a textual input rather
  than one class per sequence. It also allows users to create multiple
  instances of a sequence without modifying this file The class has
  three input parameters
  
  """

  def _get_class_name(self, algname):
    efidclasses = [ "PixelClustering","SCTClustering", "TRTDriftCircleMaker",
                    "InDetTrigPRD_MultiTruthMaker", 
                    "SiTrigSpacePointFinder", "SiTrigTrackFinder", "TrigAmbiguitySolver",
                    "SiTrigSimpleTrackFinder",
                    "TRTTrackExtAlg", "TrigExtProcessor",
                    "InDetTrigTrackSlimmer",  "InDetTrigTrackingxAODCnv",
                    "InDetTrigDetailedTrackTruthMaker",
                    "TrigVxPrimary",
                    "InDetTrigParticleCreation",
                    "InDetTrigTrackParticleTruthMaker",
                    "InDetTrigVertexxAODCnv",
                    "TrigFTK_VxPrimary",
                    ]

    clname = algname
    if algname in efidclasses:
      clname =  "%s_EF" % algname

    return clname



  def _item_line(self, algName, instance):
    seqType = self.__sequenceType__
    seqName = self.__signatureName__
    if instance=="":
      if "Fast" in seqType:
        _inst = algName+'_%s_FTF'
      elif "L2Star" in seqType:
        _inst = algName+'_%s_L2ID'
      elif "IDTrig" in seqType:
        _inst = algName+'_%s_IDTrig'
      elif "FTK" in seqType:
        _inst = algName+'_%s_FTK'
      else:
        _inst = algName+'_%s_EFID'
    else:
      _inst = instance

    if '%s' in  _inst:
      seqinstance = _inst % seqName
    else:
      seqinstance = _inst

    pyline = _line = "%s(\"%s\",\"%s\")" % (self._get_class_name(algName),seqinstance,self.__signature__)
    return pyline



  def removeTruth(self):
    pass

  def generatePyCode(self, algos):
    #create python code to be executed
    alglist = "algseq = ["
    for i in algos:
      #self.__algos__.append(i)
      algline = self._item_line(i[0], i[1])
      alglist += algline+',' 
    alglist += "]"

    #this should be avoided by a higher level steering
    #    modify the sequence acoording to triggerflags
    from TriggerJobOpts.TriggerFlags  import TriggerFlags

    if not ( TriggerFlags.doEF() or TriggerFlags.doHLT() ) or not TriggerFlags.doFEX():
      from TrigSteeringTest.TrigSteeringTestConf import PESA__dummyAlgo as dummyAlgo_disabledByTriggerFlags_EFID
      dummyAlgEFID = dummyAlgo_disabledByTriggerFlags_EFID("doEF_or_doFEX_False_no_EFID")
      alglist = 'algseq = [dummyAlgEFID]'

    algseq = []
    try:
      exec alglist
    except:
      from sys import exc_info
      (a,reason,c) = exc_info()
      print reason
      log.error("Cannot create ID tracking sequence %s, leaving empty" % alglist)
      import traceback
      print traceback.format_exc()

      #raise Exception

    log.debug('%s ' % algseq)  

    self.__sequence__.append(algseq)
    pass


  def __init__(self,
               signatureName="Electron",
               signature="electron", 
               sequenceType="IDTrig",
               sequenceFlavour="Fast"):

    TrigInDetSequenceBase.__init__(self)
    self.__signatureName__ = signatureName
    self.__signature__     = signature
    self.__sequenceType__  = sequenceType
    self.__sequenceFlavour__  = sequenceFlavour
    self.__step__ = [signature]

    if self.__sequenceFlavour__ =="2step":
      if self.__signature__ == "tau":
        self.__step__ = ["tauCore","tauIso","tau"]; 
      elif self.__signature__ == "muon":
        self.__step__ = ["muonCore","muonIso","muon"]; 
      elif self.__signature__ == "bjet":  
        self.__step__ = ["bjetVtx","bjet","bjet"]; 
      self.__step__.reverse()

    fullseq = list()

    log.debug("TrigInDetSequence  sigName=%s seqFlav=%s sig=%s sequenceType=%s" % (signatureName, sequenceFlavour, signature, sequenceType))

    dataprep = [
      ("PixelClustering", "PixelClustering_IDTrig"),
      ("SCTClustering",   "SCTClustering_IDTrig"),
      ("SiTrigSpacePointFinder","SiTrigSpacePointFinder_IDTrig"),
      ]

    ftfname = ""
    roiupdater = ""
    cnvname = "InDetTrigTrackingxAODCnv_%s_FTF"
    cnvptname = ""

    if sequenceFlavour=="2step":
      ftfname = "TrigFastTrackFinder_%sCore";  ftf2name = "TrigFastTrackFinder_%sIso"; 
      cnvname = "InDetTrigTrackingxAODCnv_%sCore_FTF";  cnv2name = "InDetTrigTrackingxAODCnv_%sIso_FTF";  
      roiupdater = "IDTrigRoiUpdater_%sCore_IDTrig";  roi2updater="IDTrigRoiUpdater_%sIso_IDTrig"
      if self.__signature__=="bjet":
        ftfname = "TrigFastTrackFinder_%sVtx"; ftf2name = ""; 
        cnvname = "InDetTrigTrackingxAODCnv_%sPrmVtx_FTF";  cnv2name = "InDetTrigTrackingxAODCnv_%s_FTF"
        roiupdater = "IDTrigRoiUpdater_%sVtx_IDTrig"; roi2updater="";
      elif self.__signature__=="muon":
        if sequenceType=="IDTrig":
          ftfname = "TrigFastTrackFinder_%s_IDTrig";  ftf2name = "TrigFastTrackFinder_%sIso_IDTrig"; 
          cnvname = "InDetTrigTrackingxAODCnv_%s_FTF";  cnv2name = "InDetTrigTrackingxAODCnv_%sIso_FTF";  
          roiupdater = "IDTrigRoiUpdater_%s_IDTrig";  roi2updater="IDTrigRoiUpdater_%sIso_IDTrig"
    
    if sequenceType=="IDTrig":

      algos = list()

      if True:    #not (sequenceFlavour == "FTF" and self.__signature__=="beamSpot"):
        algos += [("IDTrigRoiUpdater", roiupdater)]

      algos += dataprep
      algos += [("TrigFastTrackFinder",ftfname),
                ("InDetTrigTrackingxAODCnv",cnvname),
                ]

      if self.__signature__=="fullScan":
        algos += [("TrigVxPrimary","")]
        if vertexXAODCnvNeeded(): 
          algos += [("InDetTrigVertexxAODCnv","")]

      if sequenceFlavour=="2step" and self.__signature__=="bjet":
        algos += [("TrigVxPrimary","")]
        if vertexXAODCnvNeeded(): 
           algos += [("InDetTrigVertexxAODCnv","")]

      fullseq.append(algos)
 

      if sequenceFlavour=="2step":
        algos = [("IDTrigRoiUpdater", roi2updater)]
        algos += dataprep
        algos += [("TrigFastTrackFinder",ftf2name),
                  ("InDetTrigTrackingxAODCnv",cnv2name),
                  ]
        fullseq.append(algos)


      if sequenceFlavour != "FTF":
        algos = [("TrigAmbiguitySolver",""),
                 ("TRTDriftCircleMaker","TRTDriftCircleMaker_IDTrig"),
                 ("InDetTrigPRD_MultiTruthMaker",""), 
                 ("TRTTrackExtAlg",""),
                 ("TrigExtProcessor",""),
                 #("InDetTrigTrackSlimmer",""),
                 ("InDetTrigTrackingxAODCnv",cnvptname),
                 ("InDetTrigDetailedTrackTruthMaker",""),
                 #("TrigVxPrimary",""),
                 #("InDetTrigParticleCreation",""),
                 #("InDetTrigTrackParticleTruthMaker",""),
                 #("InDetTrigVertexxAODCnv","")
                 ]
        if self.__signature__ != "bjet":
          algos += [("TrigVxPrimary","")]
          if vertexXAODCnvNeeded(): 
            algos += [("InDetTrigVertexxAODCnv","")]

        fullseq.append(algos)

      
    elif sequenceType=="FastxAOD":
      algos = [("InDetTrigTrackingxAODCnv",""),
               ]
      fullseq.append(algos)

    elif sequenceType=="L2StarxAOD":
      algos = [("InDetTrigTrackingxAODCnv",""),
               ]
      fullseq.append(algos)

    elif sequenceType=="TRTdata":
      algos = [("IDTrigRoiUpdater", "IDTrigRoiUpdater_HIP"),
               ("TRTDriftCircleMaker","TRTDriftCircleMaker_IDTrig"),]
      fullseq.append(algos)

    elif sequenceType=="FastEFID":
      algos = [("IDTrigRoiUpdater", roiupdater)]
      algos += dataprep
      algos += [("SiTrigSimpleTrackFinder",""),
                ("InDetTrigTrackingxAODCnv","InDetTrigTrackingxAODCnv_%s_EFID"),
                ]
      fullseq.append(algos)

    else:
      pass

    log.debug("Full sequence has %d items" % len(fullseq) )
    #print fullseq
    #log.info("generate python now")

    for i in fullseq:
      #print i
      #print 'next item ', i
      if self.__step__:
        self.__signature__ = self.__step__.pop()
      self.generatePyCode(i)
      #print self.__sequence__


