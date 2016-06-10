#
# Basic class with all the flags to run on cosmics
# 
#
include.block("MuonCommissioning/MuDetCosmicFlags_jobOptions.py")
class MuDetCosmicFlags:
  # ---- data type (if simulated doSim=True)
  EvtMax = 1000            ############## modificabile 
  SkipEvents = 00000          ############## modificabile 
  doSim = False

  # --- Flag for M4 runs
  doM4 = True
  # --- Flag for M3 runs
  doM3 = False
  # --- Flag for activating the special data format of sector13 2006 runs
  doSector13Data = False

  FullFileName = ["input.data"]############## modificabile 
  NumFile = []

  doSimpleFile = False
  if doM4:
    doSimpleFile = True
  # --- data specific flags (choose the directory and data file to run)
  doMDTRPC  = False 
  doMDTonly = False 
  doRPConly = False 
  dooldruns = False 

###----> FLAGS FOR CONVERTERS BS->RDO->PRD

  # --- ROB numbers for MDT/RPC/TGC 
  doMdtROBNumber = False
  doRpcROBNumber = False
  doTgcROBNumber = False
  MdtSpecialROBNumber = 0x610009
  RpcSpecialROBNumber = 0x000000
  TgcSpecialROBNumber = 0

  #RDO-->PREPDATA flags   
  doMDTRDOPRD = False
  doRPCRDOPRD = True
  doTGCRDOPRD = True
  if doRPCRDOPRD:
      doRPCPRDphi = True
  else:
      doRPCPRDphi = False 

  # --- digits to prepdata conversion, for 12.0.X
  doDigitsToPrepData = False

  #Turn on/off the filter of PRDs (for the moment, for simulation only )
  doSelectPRD = False
  #if doSim:
  #  doSelectPRD = True 

  # --- LVL1 Trigger flags
  doMUCTPI = False
  doCTP = False

  # --- Select data on sector/side of Muon Spectro 
  Sector = 13
  Side = 1
  doSelectSector = False

###---> FLAGS FOR DETECTOR DESCRIPTION

  BarrelnoBF  = True
  BarrelwithBF  = False

###---> FLAGS FOR RECONSTRUCTION 

  # ---- Reconstruction packages options 
  doMoore = False
  doMuonboy = False
  doMoMu = True
              
  # ----Conditions database options
  doCalibCOOL2 = False     ############## modificabile 
  doCalibCOOLprod= False   ############## modificabile 
  doCalibCOOL13 = False
  doAlignCOOLprod = False

  # ----Write ESD
  doESD = False 
  PoolOutputFile = "MuDetRec_cosmics.pool.root"
  
  # ----Produce calibration Ntuple 
  doCalibFillNt = True
  
  # ---- make truth
  if doSim:
      doTruth = True
  else:
      doTruth = False

###---> FLAGS FOR OUTPUT (NTUPLE, MONITORING,GRAPHICS)
  
  # --- write CBNT ntuple
  doNtuple = True 
  doCBNTAthenaAware = False #default for 13.0.X releases

  # --- Fill ntuple with specific blocks
  doMdtRawNt = False 
  doRpcRawNt = False 
  doCscRawNt = False
  doTgcRawNt = False
  if not doM4:
      doTgcRawNt = False 

  # --- Monitoring
  doMonitoring = False

  # --- Graphics 
  doGraphics = False
  JiveXML = False
  OnlineJiveXML = False
  if doGraphics:
      doAtlantis = True 
      doPersint = True
      if doMuonboy:
        doNoReco = False
      else:
        doNoReco = True
  else:
      doAtlantis = False
      doPersint = False
      doNoReco = False

  # --- write Atlantis geometry xml file (JiveXML needs to be set True)
  AtlantisGeometry = False
