# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigInDetConf.TrigInDetSequence import TrigInDetSequence,TrigInDetSequenceBase

from AthenaCommon.Logging import logging 
log = logging.getLogger("TrigInDetFTKSequence.py")

class TrigInDetFTKSequence(TrigInDetSequence):

  def __init__(self,
               signatureName="Electron",
               signature="electron", 
               sequenceType="",
               sequenceFlavour=[""]):

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

    log.info("TrigInDetSequence  sigName=%s seqFlav=%s sig=%s sequenceType=%s" % (signatureName, sequenceFlavour, signature, sequenceType))

    dataprep = [
      ("PixelClustering", "PixelClustering_IDTrig"),
      ("SCTClustering",   "SCTClustering_IDTrig"),
      ("SiTrigSpacePointFinder","SiTrigSpacePointFinder_IDTrig"),
      ]

    suffix = "FTK"
    if "refit" in sequenceFlavour:
        suffix = "FTKRefit"

    ftfname = ""
    roiupdater = ""

    ftfname = "TrigFastTrackFinder_"+suffix;
    cnvname = "InDetTrigTrackingxAODCnv_%s_"+suffix;

      
    if "2step" in sequenceFlavour:
      ftfname = "TrigFastTrackFinder_"+suffix;  ftf2name = "TrigFastTrackFinder_"+suffix; 
      cnvname = "InDetTrigTrackingxAODCnv_%sCore_"+suffix;  cnv2name = "InDetTrigTrackingxAODCnv_%sIso_"+suffix;  
      roiupdater = "IDTrigRoiUpdater_%sCore_IDTrig";  roi2updater="IDTrigRoiUpdater_%sIso_IDTrig"
      if self.__signature__=="bjet":
        ftfname = "TrigFastTrackFinder_"+suffix; ftf2name = ftfname; 
        cnvname = "InDetTrigTrackingxAODCnv_%sPrmVtx_"+suffix;  cnv2name = "InDetTrigTrackingxAODCnv_%s_"+suffix;
        roiupdater = "IDTrigRoiUpdater_%sVtx_IDTrig"; roi2updater="";
      elif self.__signature__=="muon":
        ftfname = "TrigFastTrackFinder_"+suffix; ftf2name = ftfname;
        cnvname = "InDetTrigTrackingxAODCnv_%s_"+suffix; cnv2name = "InDetTrigTrackingxAODCnv_%sIso_"+suffix; 
        roiupdater = "IDTrigRoiUpdater_%s_IDTrig";  roi2updater="IDTrigRoiUpdater_%sIso_IDTrig"
        


    algos = list()

    if True:    #not (sequenceFlavour == "FTF" and self.__signature__=="beamSpot"):
      algos += [("IDTrigRoiUpdater", roiupdater)]

    algos += [("TrigFastTrackFinder",ftfname),
              ("InDetTrigTrackingxAODCnv",cnvname),
              ]
    if "2step" in sequenceFlavour and self.__signature__=="bjet":
      algos += [("TrigVxPrimary",""),
                ("InDetTrigVertexxAODCnv","")]

    fullseq.append(algos)


    if  "2step" in sequenceFlavour:
      algos = [("IDTrigRoiUpdater", roi2updater)]
      algos += dataprep
      algos += [("TrigFastTrackFinder",ftf2name),
                ("InDetTrigTrackingxAODCnv",cnv2name),
                ]
      fullseq.append(algos)


    if "FTF" not in  sequenceFlavour:
      algos = dataprep
      algos += [("TrigAmbiguitySolver",""),
                ("TRTDriftCircleMaker",""),
                ("InDetTrigPRD_MultiTruthMaker",""), 
                ("TRTTrackExtAlg",""),
                ("TrigExtProcessor",""),
                ("InDetTrigTrackSlimmer",""),
                ("InDetTrigTrackingxAODCnv",""),
                ("InDetTrigDetailedTrackTruthMaker",""),
                #("TrigVxPrimary",""),
                #("InDetTrigParticleCreation",""),
                #("InDetTrigTrackParticleTruthMaker",""),
                #("InDetTrigVertexxAODCnv","")
                ]

    if self.__signature__ != "bjet":
      algos += [("TrigVxPrimary",""),
                ("InDetTrigVertexxAODCnv","")]
      fullseq.append(algos)

   

    log.info("Full sequence has %d items" % len(fullseq) )
    #print fullseq
    #log.info("generate python now")

    for i in fullseq:
      #print i
      #print 'next item ', i
      if self.__step__:
        self.__signature__ = self.__step__.pop()
      self.generatePyCode(i)
      #print self.__sequence__
