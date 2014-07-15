# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
#  provisionally a copy of InDetRecExample
#

from AthenaCommon.Include import include

include.block("InDetTrigRecExample/EFInDetConfig.py")

include("InDetTrigRecExample/InDetTrigRec_jobOptions.py") # this is needed to get InDetTrigFlags

from AthenaCommon.Logging import logging 
log = logging.getLogger("EFInDetConfig.py")

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

from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags


class TrigInDetSequenceBase:
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

  def _item_line(self, algName, instance, seqName="Tau", type="tau", seqType="InsideOut"):
    if instance=="":
      if "Fast" in seqType:
        _inst = algName+'_%s_FTF'
      elif "L2Star" in seqType:
        _inst = algName+'_%s_L2ID'
      else:
        _inst = algName+'_%s_EFID'
    else:
      _inst = instance

    seqinstance = _inst % seqName

    #this should be not be done here
    #_line = algName+"_EF(\""+seqinstance+"\","+type")"
    #the TRTOnly sequence needs to instantiate a dedaicated set of cuts
    if seqType is "TRTOnly" and (algName.startswith("TRT_TrigTrackSegmentsFinder") or algName.startswith("TRT_TrigStandaloneTrackFinder")):
      _line = "%s_EF(\"%s\",\"%s\",\"%s\")" % (algName,seqinstance,type,seqType)
    else:  
      _line = "%s_EF(\"%s\",\"%s\")" % (algName,seqinstance,type)
      
    return _line

  def __init__(self, seqName="Tau", type="tau", seqType="InsideOut"):
    """
    @param[in] seqName  which enters into the sequence name inbetween
    algorithmName_seqName_EFID can be freely modified
    
    @param[in] type     which is a slice name and is used to set up slice
    specific variables
    
    @param[in] seqType  the type of sequence - it can be one of InsideOut,
    OutsideIn, TRTOnly, Combined,....
    """
    #print "TrigEFIDSequence  seqName=%s type=%s seqType=%s" % (seqName,type,seqType)
    algos = []
             
    if seqType=="Fast":
      pass
    elif seqType=="FastxAOD":
      algos = [("InDetTrigTrackingxAODCnv",""),
               ]
    elif seqType=="L2StarxAOD":
      algos = [("InDetTrigTrackingxAODCnv",""),
               ]
      

    #remove truth algorithms
    if not InDetTrigFlags.doTruth():
      import re
      for i in algos:
        if re.search('Truth', i[0]):
          algos.remove((i[0],i[1]))

    #create python code to be executed
    alglist = "algseq = ["
    for i in algos:
      #self.__algos__.append(i)
      algline = self._item_line(i[0], i[1], seqName, type, seqType)
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
      log.error("Cannot create EFID sequence %s, leaving empty" % alglist)
      import traceback
      print traceback.format_exc()

      #raise Exception

    self.__sequence__ = algseq
      
