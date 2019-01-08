# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# import pdb
# pdb.set_trace()

from TrigFTK_RecAlgs.TrigFTK_RecAlgs_Config import TrigFTK_VxPrimary_EF
from TrigInDetConf.TrigInDetSequence import TrigInDetSequence,TrigInDetSequenceBase

from AthenaCommon.Logging import logging 
log = logging.getLogger("TrigInDetFTKSequence.py")

class TrigInDetFTKSequence(TrigInDetSequence):

  def __init__(self,
               signatureName="Electron",
               signature="electron", 
               sequenceFlavour=[""]):

    TrigInDetSequenceBase.__init__(self)
    self.__signatureName__ = signatureName
    self.__signature__     = signature
    self.__sequenceType__  = "FTK"
    self.__sequenceFlavour__  = sequenceFlavour
    self.__step__ = [signature]

    if type(sequenceFlavour)!=type(list()):
      log.error("TrigInDetFTKSequence invoked with a non-list sequenceFlavour argument %s" )
      

    if "2step" in self.__sequenceFlavour__:
      if self.__signature__ == "tau":
        self.__step__ = ["tauCore","tauIso","tau"]; 
      elif self.__signature__ == "muon":
        self.__step__ = ["muonCore","muonIso","muon"]; 
      elif self.__signature__ == "bjet":  
        self.__step__ = ["bjetVtx","bjet","bjet"]; 
      self.__step__.reverse()

    fullseq = list()

    log.info("TrigInDetFTKSequence  sigName=%s seqFlav=%s sig=%s" % (signatureName, sequenceFlavour, signature))

    dataprep = [
      ("PixelClustering", "PixelClustering_IDTrig"),
      ("SCTClustering",   "SCTClustering_IDTrig"),
      ("SiTrigSpacePointFinder","SiTrigSpacePointFinder_IDTrig"),
      ]

    suffix = "FTK"
    if "refit" in sequenceFlavour:
        suffix = "FTKRefit"
    if "mon" in sequenceFlavour:
        suffix = "FTKMon"


    ftfname = ""
    roiupdater = ""

    ftfname = "TrigFastTrackFinder_"+suffix;
    cnvname = "InDetTrigTrackingxAODCnv_%s_"+suffix;

    #start filling sequences

    algos = [("IDTrigRoiUpdater", roiupdater)]
    algos += [("FTK_TrackMaker", "")]
    algos += [("TrigFTKAvailable", "")]

    if "FTKVtx" in sequenceFlavour:
      algos += [("TrigFTK_VxPrimary","")]
      fullseq.append(algos)
      algos = list()


    algos += [("TrigFastTrackFinder",ftfname),
              ("InDetTrigTrackingxAODCnv",cnvname),
              ]
    fullseq.append(algos)


    if "PT" in sequenceFlavour:
      algos = list()

      if "dev" in sequenceFlavour:
        algos += dataprep

      ptsuffix = suffix+"_IDTrig"
      algos += [("TrigAmbiguitySolver","TrigAmbiguitySolver_"+suffix),
                ("TRTDriftCircleMaker",""),
                ("InDetTrigPRD_MultiTruthMaker",""), 
                ("TRTTrackExtAlg",""),
                ("TrigExtProcessor",""),
                ("InDetTrigTrackSlimmer",""),
                ("InDetTrigTrackingxAODCnv","InDetTrigTrackingxAODCnv_%s_"+ptsuffix),
                #("InDetTrigDetailedTrackTruthMaker",""),
                ]
      fullseq.append(algos)

   

    log.info("Full sequence has %d items" % len(fullseq) )
    #log.info("Full sequence has ", fullseq )

    for i in fullseq:
      #print i
      #print 'next item ', i
      if self.__step__:
        self.__signature__ = self.__step__.pop()
      self.generatePyCode(i)
      #print self.__sequence__
