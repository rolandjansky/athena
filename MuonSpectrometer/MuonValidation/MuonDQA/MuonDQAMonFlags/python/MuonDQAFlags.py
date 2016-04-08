# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# Basic class with all the flags to run on cosmics
# 
#
class MuonDQAFlags:
#-------------------------------------------------------------------        
  # number of events to process
  EvtMax = 10
#-------------------------------------------------------------------     
  # number of events to skip
  SkipEvents=0
#-------------------------------------------------------------------     
  # data type (if simulation doSim = True)
  doSim = False
  doSimpleFile = False
#-------------------------------------------------------------------     
  # ---- make truth
  doTruth = False 
#-------------------------------------------------------------------   
# real data input file 
#-------------------------------------------------------------------  
  RunNumber = 12460
  InputDir = "/castor/cern.ch/atlas/P1commisioning/phaseIII"
  run = "m3"
  FilePrefix = "daq_SFO-1_"+run+"_combined"
## example of M5 runs ##
  FullFileName = ["/castor/cern.ch/grid/atlas/t0/perm/DAQ/M5/daq.m5_combined.0029576.Default.L1TT-b00000001.LB0000.SFO-4._0014.data"]
  NumFile = []
   
#Reminder: if doM3 or doSector13Data-->doSimpleFile = False
###----> FLAGS FOR DIFFERENT DATA TAKING PERIODS 
  doM8 = False 
  doM7 = False 
  doM6 = False 
  doM5 = False 
  doM4 = False
  doM3 = False 
  doP5 = False
  doP4 = False
  doP3 = False 
  doP2 = False
  doP1 = False
        
  #LSFTimeKeeper
  doTimeLimit = True
  doFinalCPU = True
  doDumpFile = True
  doMuonDQAPerfMonitoring = True
  doMuonDQAHephTool = False
  doCpuMuonMon = True
  # --- timing of algorithms
  doAuditor = False

  # --- Callgrind Tool
  doCallgrind = True
  CallgrindHitAlgs = ["MDTRawDataESDValAlg","MDTvsRPCRawDataValAlg",
                      "RPCRawDataValAlg","RPCLV1RawDataValAlg",
		      "TGCRawDataValAlg","TGCRawDataESDValAlg","TGCLV1RawDataValAlg","TGCLV1RawDataESDValAlg",
		      "MDTvsTGCLV1RawDataValAlg","MDTvsTGCRawDataESDValAlg",
		      "CSCRdoValAlg","CSCPrdValAlg","CSCClusterTree","CSCClusterValAlg",
		      "MuonMonManager","AthenaMonManager"] #list of algs to check
  CallgrindTrkAlgs = ["MuonGenericTracksMon","MuonSelectedTracksMon","MuonMonManager","AthenaMonManager"] #list of algs to check
  CallgrindTrkAlignAlgs = ["MuonAlignValidation","MuonMonManager","AthenaMonManager"] #list of algs to check
  CallgrindSegmAlgs = ["MuonSegmValidation","MuonMonManager","AthenaMonManager"] #list of algs to check  
  CallgrindSkip = 2 # specify number of skipped events

  # --- VP1 event display
  doVP1 = True
    
  # --- Graphics 
  doGraphics = False
  JiveXML = False
  OnlineJiveXML = False
  doAtlantis = False
  doPersint = False
  doNoReco = False
  # --- write Atlantis geometry xml file (JiveXML needs to be set True)
  AtlantisGeometry = False
