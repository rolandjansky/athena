# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# Configuration classes for EF InDet Algs
#
# Classes:
# uses internal classes:
# TrigEFIDInsideOut_Slice, where Slice=Electron,Muon,Tau,Bjet,Bphysics
# TrigEFIDOutsideIn_Slice, where Slice=Photon
#
# Constructor arguments:
# level, type, threshold, isIsolated
# 
# e.g. level=L2, type=muon, threshold=30, isIsolated=None
#      level=EF, type=egamma, threshold=20, isIsolated=isolated
#
# Methods:
# instanceName() : returns name of algorithm instance
# classAndInstanceName() : returns a string to be entered in the sequence file. This string
#              defines the class and instance name
#

from __future__ import print_function

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
from TrigInDetConf.TrigInDetSequence import TrigInDetSequenceBase


class TrigEFIDSequence(TrigInDetSequenceBase):
  """

  Create EFID sequences from a textual input rather than one class per
  sequence. It also allows users to create multiple instances of a
  sequence without modifying this file The class has three input
  parameters
  
  """

  def _item_line(self, algName, instance, seqName="Tau", type="tau", seqType="InsideOut"):
    if instance=="":
      _inst = algName+'_%s_EFID'
    else:
      _inst = instance

    seqinstance = _inst % seqName

    #_line = algName+"_EF(\""+seqinstance+"\","+type")"
    #the TRTOnly sequence needs to instantiate a dedaicated set of cuts
    if seqType == "TRTOnly" and (algName.startswith("TRT_TrigTrackSegmentsFinder") or algName.startswith("TRT_TrigStandaloneTrackFinder")):
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
    #print ("TrigEFIDSequence  seqName=%s type=%s seqType=%s" % (seqName,type,seqType))
    algos = []

    from TrigInDetConf.TrigInDetSequence import vertexXAODCnvNeeded
    runvtxxaodcnv = vertexXAODCnvNeeded()
             
    if seqType=="InsideOut":
      algos = [("PixelClustering", ""),
               ("SCTClustering", ""),
               ("TRTDriftCircleMaker",""),
               ("InDetTrigPRD_MultiTruthMaker",""), 
               ("SiTrigSpacePointFinder",""),
               ("SiTrigTrackFinder",""),
               ("TrigAmbiguitySolver",""),
               ("TRTTrackExtAlg",""),
               ("TrigExtProcessor",""),
               ("InDetTrigTrackSlimmer",""),
               ("InDetTrigTrackingxAODCnv",""),
#               ("InDetTrigDetailedTrackTruthMaker",""),
               ("TrigVxPrimary",""),
#               ("InDetTrigParticleCreation",""),
#               ("InDetTrigTrackParticleTruthMaker",""),
               ]
      
      if runvtxxaodcnv:
        algos += [("InDetTrigVertexxAODCnv","")]

      if type=="minBias" or type=="minBias400":
        algos = [("PixelClustering",""),
                 ("SCTClustering",""), 
                 ("InDetTrigPRD_MultiTruthMaker",""), 
                 ("SiTrigSpacePointFinder",""),
                 ("SiTrigTrackFinder",""),
                 ("TrigAmbiguitySolver",""),
                 ("InDetTrigTrackSlimmer",""),
                 ("InDetTrigTrackingxAODCnv",""),
                 ("TrigVxPrimary",""),
#                 ("InDetTrigParticleCreation",""),
                 ]
        if runvtxxaodcnv:
          algos += [("InDetTrigVertexxAODCnv","")]

      elif type=="bjet":
        #from InDetTrigVxSecondary.InDetTrigVxSecondary_LoadTools import TrigVxSecondary_EF
        #algos += [("TrigVxSecondary","")]
        algos = [("PixelClustering", ""),
                 ("SCTClustering", ""),
                 ("TRTDriftCircleMaker",""),
                 ("InDetTrigPRD_MultiTruthMaker",""), 
                 ("SiTrigSpacePointFinder",""),
                 ("SiTrigTrackFinder",""),
                 ("TrigAmbiguitySolver",""),
                 ("TRTTrackExtAlg",""),
                 ("TrigExtProcessor",""),
                 ("InDetTrigTrackSlimmer",""),
                 ("InDetTrigTrackingxAODCnv",""),
                 ("InDetTrigDetailedTrackTruthMaker",""),
                 ("TrigVxPrimary",""),
#                 ("InDetTrigParticleCreation",""),
                 ("InDetTrigTrackParticleTruthMaker","")
                 ]
        if runvtxxaodcnv:
          algos += [("InDetTrigVertexxAODCnv","")]


    elif seqType=="InsideOutTrk":
      algos = [("PixelClustering", ""),
               ("SCTClustering", ""),
               ("TRTDriftCircleMaker",""),
               ("InDetTrigPRD_MultiTruthMaker",""), 
               ("SiTrigSpacePointFinder",""),
               ("SiTrigTrackFinder",""),
               ("TrigAmbiguitySolver",""),
               ("TRTTrackExtAlg",""),
               ("TrigExtProcessor",""),
               ("InDetTrigTrackSlimmer",""),
               ("InDetTrigTrackingxAODCnv",""),
               ("InDetTrigDetailedTrackTruthMaker","")]

    elif seqType=="Vtx":
      algos = [("TrigVxPrimary","")]
      if type=="bjet":
        algos = [("TrigVxPrimaryAllTE","")]
        if runvtxxaodcnv:
          algos += [("InDetTrigVertexxAODCnv","")]


    elif seqType=="InsideOutPost":
      algos = [
        #("InDetTrigParticleCreation",""),
        ("InDetTrigTrackingxAODCnv",""),
        ("InDetTrigTrackParticleTruthMaker","")]

    elif seqType=="InsideOutNoTRT":
      a1 = [("PixelClustering", ""),
            ("SCTClustering", ""),
            ("InDetTrigPRD_MultiTruthMaker",""), 
            ("SiTrigSpacePointFinder",""),
            ("SiTrigTrackFinder",""),
            ("TrigAmbiguitySolver",""),
            ("InDetTrigTrackSlimmer",""),
            ("InDetTrigTrackingxAODCnv",""),
            ("InDetTrigDetailedTrackTruthMaker",""),]
      av = [("TrigVxPrimary","")]
      if runvtxxaodcnv:
        av += [("InDetTrigVertexxAODCnv","")]

      a2 = [
        #("InDetTrigParticleCreation",""),
        ("InDetTrigTrackParticleTruthMaker","")
            ]
      if type=="heavyIon" or type=="heavyIonFS":
        algos = a1 + a2
      else:
        algos = a1 + av + a2

      
    elif seqType=="InsideOutLowPt":
      algos = [("PixelClustering",""),
               ("SCTClustering",""), 
               ("InDetTrigPRD_MultiTruthMaker",""), 
               ("SiTrigSpacePointFinder",""),
               ("SiTrigTrackFinder",""),
               ("TrigAmbiguitySolver",""),
               ("InDetTrigTrackPRD_Association",""),
               ("SiTrigSpacePointFinderLowPt",""),
               ("SiTrigTrackFinderLowPt",""),
               ("TrigAmbiguitySolverLowPt",""),
               ("SimpleTrigTrackCollMerger",  ""),                
               ("InDetTrigTrackSlimmer",""),
               ("InDetTrigTrackingxAODCnv",""),
               ("TrigVxPrimary",""),
               #("InDetTrigParticleCreation",""),
               ]
      if runvtxxaodcnv:
        algos += [("InDetTrigVertexxAODCnv","")]

      
    elif seqType=="OutsideIn":
      algos = [("PixelClustering",""),
               ("SCTClustering",""),
               ("TRTDriftCircleMaker",""),
               ("InDetTrigPRD_MultiTruthMaker",""), 
               ("SiTrigSpacePointFinder",""),
               ("TRT_TrigTrackSegmentsFinder",    "InDetTrigTRT_TrackSegmentsFinder_%s_EFID"),
               ("TRT_TrigSeededTrackFinder",      "InDetTrigTRT_SeededTrackFinder_%s_EFID"),
               ("TRTSeededTrigAmbiguitySolver", ""),
               ("InDetTrigTrackSlimmer",""),
               ("InDetTrigTrackingxAODCnv",""),
               ("InDetTrigDetailedTrackTruthMaker",""),
               #("InDetTrigParticleCreation",""),
               ("InDetTrigTrackParticleTruthMaker","")
               ]

    elif seqType=="TRTOnly":
      algos = [("TRTDriftCircleMaker",""),
               ("InDetTrigPRD_MultiTruthMaker",""), 
               ("TRT_TrigTrackSegmentsFinder",      "InDetTrigTRT_TrackSegmentsFinder_%s_EFID"),
               ("InDetTrigTrackPRD_Association",    "InDetTrigTRTStandalonePRD_Association_%s_EFID"),
               ("TRT_TrigStandaloneTrackFinder",    "InDetTrigTRT_StandaloneTrackFinder_%s_EFID"), 
               ("InDetTrigTrackSlimmer",            "InDetTrigTrackSlimmerTRTOnly_%s_EFID"),
               ("InDetTrigTrackingxAODCnv",         "InDetTrigTrackingxAODCnvTRTOnly_%s_EFID"),
               ("InDetTrigDetailedTrackTruthMaker", "InDetTrigDetailedTrackTruthMakerTRTOnly_%s_EFID"),
               ("TrigVxPrimary",                    "TrigVxPrimaryTRTOnly_%s_EFID"),
               #("InDetTrigParticleCreation",        "InDetTrigParticleCreationTRTOnly_%s_EFID"),
               ("InDetTrigTrackParticleTruthMaker", "InDetTrigTrackParticleTruthMakerTRTOnly_%s_EFID")
               ]
      if runvtxxaodcnv:
        algos += [("InDetTrigVertexxAODCnv","InDetTrigVertexxAODCnvTRTOnly_%s_EFID")]

    elif seqType=="Combined":
      algos = [("PixelClustering",""),
               ("SCTClustering",""),
               ("TRTDriftCircleMaker",""),
               ("InDetTrigPRD_MultiTruthMaker",""), 
               ("SiTrigSpacePointFinder",""),
               ("SiTrigTrackFinder",""),
               ("TrigAmbiguitySolver",""),
               ("TRTTrackExtAlg",""),
               ("TrigExtProcessor",""),
               ("InDetTrigTrackPRD_Association", "InDetTrigSegmentPRD_Association_%s_EFID"), 
               ("TRT_TrigTrackSegmentsFinder",   "InDetTrigTRT_TrackSegmentsFinderCombined_%s_EFID"),
               ("TRT_TrigSeededTrackFinder",     "InDetTrigTRT_SeededTrackFinder_%s_EFID"),
               ("TRTSeededTrigAmbiguitySolver",  "TRTSeededTrigAmbiguitySolver_%s_EFID"),
               ("InDetTrigTrackPRD_Association", "InDetTrigTRTOnlyPRD_Association_%s_EFID"), 
               ("TRT_TrigStandaloneTrackFinder", "InDetTrigTRT_StandaloneTrackFinderCombined_%s_EFID"),
               ("TrkTrigTrackCollectionMerger",  "TrigTrackCollectionMerger_%s_EFID"),                
               ("InDetTrigTrackSlimmer"       ,  "InDetTrigTrackSlimmerCombined_%s_EFID"),
               ("InDetTrigTrackingxAODCnv",      "InDetTrigTrackingxAODCnvCombined_%s_EFID"),
               ("InDetTrigDetailedTrackTruthMaker", "InDetTrigDetailedTrackTruthMakerCombined_%s_EFID"),
               #("InDetTrigParticleCreation",      "InDetTrigParticleCreationCombined_%s_EFID"),
               ("InDetTrigTrackParticleTruthMaker", "InDetTrigTrackParticleTruthMakerCombined_%s_EFID")
               ]

    elif seqType=="CombinedWithConversions":
      algos = [("PixelClustering",""),
               ("SCTClustering",""),
               ("TRTDriftCircleMaker",""),
               ("InDetTrigPRD_MultiTruthMaker",""), 
               ("SiTrigSpacePointFinder",""),
               ("SiTrigTrackFinder",""),
               ("TrigAmbiguitySolver",""),
               ("TRTTrackExtAlg",""),
               ("TrigExtProcessor",""),
               ("InDetTrigTrackPRD_Association", "InDetTrigSegmentPRD_Association_%s_EFID"), 
               ("TRT_TrigTrackSegmentsFinder",   "InDetTrigTRT_TrackSegmentsFinderCombined_%s_EFID"),
               ("TRT_TrigSeededTrackFinder",     "InDetTrigTRT_SeededTrackFinder_%s_EFID"),
               ("TRTSeededTrigAmbiguitySolver",  "TRTSeededTrigAmbiguitySolver_%s_EFID"),
               ("InDetTrigTrackPRD_Association", "InDetTrigTRTOnlyPRD_Association_%s_EFID"), 
               ("TRT_TrigStandaloneTrackFinder", "InDetTrigTRT_StandaloneTrackFinderCombined_%s_EFID"),
               ("TrkTrigTrackCollectionMerger",  "TrigTrackCollectionMerger_%s_EFID"),                
               ("InDetTrigTrackSlimmer"       ,  "InDetTrigTrackSlimmerCombined_%s_EFID"),
               ("InDetTrigTrackingxAODCnv",      "InDetTrigTrackingxAODCnvCombined_%s_EFID"),
               ("InDetTrigDetailedTrackTruthMaker", "InDetTrigDetailedTrackTruthMakerCombined_%s_EFID"),
               #("InDetTrigParticleCreation",      "InDetTrigParticleCreationCombined_%s_EFID"),
               ("InDetTrigTrackParticleTruthMaker", "InDetTrigTrackParticleTruthMakerCombined_%s_EFID"),
               ("InDetTrigConversionFinder",      "InDetTrigConversionFinderCombined_%s_EFID")
               ]

    elif seqType=="InsideOutAndTRTOnly":
      algos = [("PixelClustering", ""),
               ("SCTClustering", ""),
               ("TRTDriftCircleMaker",""),
               ("InDetTrigPRD_MultiTruthMaker",""), 
               ("SiTrigSpacePointFinder",""),
               ("SiTrigTrackFinder",""),
               ("TrigAmbiguitySolver",""),
               ("TRTTrackExtAlg",""),
               ("TrigExtProcessor",""),
               ("InDetTrigTrackParticleTruthMaker",""),
               ("TRT_TrigTrackSegmentsFinder",      "InDetTrigTRT_TrackSegmentsFinder_%s_EFID"),
               ("TRT_TrigStandaloneTrackFinder",    "InDetTrigTRT_StandaloneTrackFinder_%s_EFID"), 
               ("TrkTrigTrackCollectionMerger",     "TrigTrackCollectionMergerIOTRT_%s_EFID"),         
               ("InDetTrigTrackSlimmer",            "InDetTrigTrackSlimmerIOTRT_%s_EFID"),
               ("InDetTrigTrackingxAODCnv",         "InDetTrigTrackingxAODCnvIOTRT_%s_EFID"),
               ("InDetTrigDetailedTrackTruthMaker", "InDetTrigDetailedTrackTruthMakerIOTRT_%s_EFID"),
               #("InDetTrigParticleCreation",        "InDetTrigParticleCreationIOTRT_%s_EFID"),
               ("InDetTrigTrackParticleTruthMaker", "InDetTrigTrackParticleTruthMakerIOTRT_%s_EFID"),
               ("TrigVxPrimary",""),
               ]
      if runvtxxaodcnv:
        algos += [("InDetTrigVertexxAODCnv","InDetTrigVertexxAODCnvIOTRT_%s_EFID")]

    elif seqType=="DataPrep":
      algos = [("PixelClustering", "PixelClustering_%s_IDTrig"),
               ("SCTClustering", "SCTClustering_%s_IDTrig"),
               #("TRTDriftCircleMaker",""),
               ("SiTrigSpacePointFinder","SiTrigSpacePointFinder_%s_IDTrig"),
             ]
    elif seqType=="SimpleTrackFinder":
      algos = [
        #("SiTrigTrackFinder",""),
        ("SiTrigSimpleTrackFinder",""),
        ]
    elif seqType=="InsideOutMerged":
      algos = [
               #("SiTrigTrackSeededFinder",""),
               ("TrigAmbiguitySolver",""),
               ("TRTDriftCircleMaker",""),
               ("InDetTrigPRD_MultiTruthMaker",""), 
               ("TRTTrackExtAlg",""),
               ("TrigExtProcessor",""),
               ("InDetTrigTrackSlimmer",""),
               ("InDetTrigTrackingxAODCnv",""),
               ("InDetTrigDetailedTrackTruthMaker",""),
               ("TrigVxPrimary",""),
               #("InDetTrigParticleCreation",""),
               ("InDetTrigTrackParticleTruthMaker",""),
               ]
      if runvtxxaodcnv:
        algos += [("InDetTrigVertexxAODCnv","")]

    elif seqType=="StandAloneVtxAllTE":
      algos = [("TrigVxPrimaryAllTESG", ""),
               ]
      if runvtxxaodcnv:
        algos += [("InDetTrigVertexxAODCnv","")]

      
    #remove truth algorithms
    if not InDetTrigFlags.doTruth():
      import re
      for i in algos:
        if re.search('Truth', i[0]):
          algos.remove((i[0],i[1]))

    #create python code to be executed
    alglist = "["
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
      alglist = '[dummyAlgEFID]'

    algseq = []
    try:
      algseq = eval (alglist)
    except:
      from sys import exc_info
      (a,reason,c) = exc_info()
      print (reason)
      log.error("Cannot create EFID sequence %s, leaving empty" % alglist)
      import traceback
      traceback.print_exc()

      #raise Exception

    self.__sequence__ = algseq
      


#define classes using TrigEFIDInsideOut for backwards compatibility
class TrigEFIDInsideOut(TrigEFIDSequence):
  def __init__(self, seqName="Tau",type="tau"):
    #print ("TrigEFIDSequence  seqName=%s type=%s" % (seqName,type))
    TrigEFIDSequence.__init__(self,seqName,type,seqType="InsideOut")

class TrigEFIDInsideOut_Tau(TrigEFIDInsideOut):
  def __init__(self, name="dummy", seqName="Tau", type="tau"):
    TrigEFIDInsideOut.__init__(self,seqName,type)

class TrigEFIDInsideOut_BeamGas(TrigEFIDInsideOut):
  def __init__(self,  seqName="BeamGas", type="beamgas"):
    TrigEFIDInsideOut.__init__(self,seqName,type)

class TrigEFIDInsideOut_CosmicsN(TrigEFIDInsideOut):
  def __init__(self, seqName="CosmicsN", type="cosmicsN"):
    TrigEFIDSequence.__init__(self,seqName,type,seqType="InsideOutAndTRTOnly")

class TrigEFIDInsideOut_Bjet(TrigEFIDInsideOut):
  def __init__(self, seqName="Bjet", type="bjet"):
    TrigEFIDInsideOut.__init__(self,seqName,type)

class TrigEFIDInsideOut_Bphysics(TrigEFIDInsideOut):
  def __init__(self, seqName="Bphysics", type="bphysics"):
    TrigEFIDInsideOut.__init__(self,seqName,type)

class TrigEFIDInsideOut_Electron(TrigEFIDInsideOut):
  def __init__(self, seqName="Electron", type="electron"):
    TrigEFIDInsideOut.__init__(self,seqName,type)

class TrigEFIDInsideOut_Muon(TrigEFIDInsideOut):
  def __init__(self, seqName="Muon", type="muon"):
    TrigEFIDInsideOut.__init__(self,seqName,type)

class TrigEFIDInsideOut_FullScan(TrigEFIDInsideOut):
  def __init__(self, seqName="FullScan", type="fullScan"):
    TrigEFIDInsideOut.__init__(self,seqName,type)

# class TrigEFIDInsideOut_MinBias(TrigEFIDInsideOut):
#   def __init__(self, seqName="MinBias", type="minBias"):
#     TrigEFIDInsideOut.__init__(self,seqName,type)

class TrigEFIDInsideOut_MinBias(TrigEFIDSequence):
  def __init__(self, 
               seqName="MinBias",type="minBias",seqType="InsideOutLowPt"):
    TrigEFIDSequence.__init__(self,seqName,type,seqType)


# classes which will be obsolete soon
class TrigEFIDOutsideIn_Photon(TrigEFIDSequence):
  def __init__(self, 
               seqName="Photon",type="photon",seqType="OutsideIn"):
    TrigEFIDSequence.__init__(self,seqName,type,seqType)

class TrigEFIDOutsideIn_Egamma(TrigEFIDSequence):
  def __init__(self,
               seqName="Egamma", type="photon",seqType="OutsideIn"):
    TrigEFIDSequence.__init__(self,seqName,type,seqType)
### end of obsolete classes

## TRT standalone sequence
class TrigEFIDOutsideInTRTOnly_Electron(TrigEFIDSequence):
  def __init__(self,
               seqName="Electron",type="electron",seqType="TRTOnly"):
    TrigEFIDSequence.__init__(self,seqName,type,seqType)

class TrigEFIDOutsideInTRTOnly_Tau(TrigEFIDSequence):
  def __init__(self,
               seqName="Tau",type="tau",seqType="TRTOnly"):
    TrigEFIDSequence.__init__(self,seqName,type,seqType)

class TrigEFIDOutsideInTRTOnly_Muon(TrigEFIDSequence):
  def __init__(self,
               seqName="Muon",type="muon",seqType="TRTOnly"):
    TrigEFIDSequence.__init__(self,seqName,type,seqType)

class TrigEFIDOutsideInTRTOnly_MinBias(TrigEFIDSequence):
  def __init__(self,
               seqName="MinBias",type="minBias",seqType="TRTOnly"):
    TrigEFIDSequence.__init__(self,seqName,type,seqType)



## Combined sequences
class TrigEFIDCombined_Photon(TrigEFIDSequence):
  def __init__(self,
               seqName="Electron",type="photon",seqType="CombinedWithConversions"):
    TrigEFIDSequence.__init__(self,seqName,type,seqType)

class TrigEFIDCombined_Electron(TrigEFIDSequence):
  def __init__(self,
               seqName="Electron",type="photon",seqType="Combined"):
    TrigEFIDSequence.__init__(self,seqName,type,seqType)
  

## not used
class TrigEFIDInsideOut_old(TrigInDetSequenceBase):
  """ Factorize common code for EFID Inside Out seqences
  the class can be called directly with two parameters - (sequenceName,type)
  sequenceName can be used to construct a unique sequence name
  type determines the sequence settings (i.e. slice specific settings)
  """
  def __init__(self, seqName="Tau", type="tau"):
    self.__sequence__ = \
    [PixelClustering_EF("PixelClustering_%s_EFID" % seqName, type),
     SCTClustering_EF("SCTClustering_%s_EFID" % seqName, type),
     TRTDriftCircleMaker_EF("TRTDriftCircleMaker_%s_EFID" % seqName, type),
     InDetTrigPRD_MultiTruthMaker_EF("InDetTrigPRD_MultiTruthMaker_%s_EFID" % seqName, type),
     #
     SiTrigSpacePointFinder_EF("SiTrigSpacePointFinder_%s_EFID" % seqName, type),
     SiTrigTrackFinder_EF("SiTrigTrackFinder_%s_EFID" % seqName, type),
     TrigAmbiguitySolver_EF("TrigAmbiguitySolver_%s_EFID" % seqName, type),
     #
     TRTTrackExtAlg_EF("TRTTrackExtAlg_%s_EFID" % seqName, type),
     TrigExtProcessor_EF("TrigExtProcessor_%s_EFID" % seqName, type),
     #
     InDetTrigTrackSlimmer_EF("InDetTrigTrackSlimmer_%s_EFID" % seqName, type),
     InDetTrigDetailedTrackTruthMaker_EF("InDetTrigDetailedTrackTruthMaker_%s_EFID" % seqName, type),
     #
     TrigVxPrimary_EF("TrigVxPrimary_%s_EFID" % seqName, type),
     InDetTrigParticleCreation_EF("InDetTrigParticleCreation_%s_EFID" % seqName, type),
     InDetTrigTrackParticleTruthMaker_EF("InDetTrigTrackParticleTruthMaker_%s_EFID" % seqName,type) ]
    if type=='minBias':
      self.__sequence__ = \
          [PixelClustering_EF("PixelClustering_%s_EFID" % seqName, type),
           SCTClustering_EF("SCTClustering_%s_EFID" % seqName, type),
           InDetTrigPRD_MultiTruthMaker_EF("InDetTrigPRD_MultiTruthMaker_%s_EFID" % seqName, type),
           SiTrigSpacePointFinder_EF("SiTrigSpacePointFinder_%s_EFID" % seqName, type),
           SiTrigTrackFinder_EF("SiTrigTrackFinder_%s_EFID" % seqName, type),
           TrigAmbiguitySolver_EF("TrigAmbiguitySolver_%s_EFID" % seqName, type),
           InDetTrigTrackSlimmer_EF("InDetTrigTrackSlimmer_%s_EFID" % seqName, type),
           #
           TrigVxPrimary_EF("TrigVxPrimary_%s_EFID" % seqName, type)]

