#======================================================================
# TOP Job options file for Combined TB2004 reconstruction 
# of real or simulated data
# updated for rel. 15.X.Y
#======================================================================

# -------------------------------
# Set global flags
# -------------------------------
# Input
if not 'RunNumber' in dir():
   RunNumber = 2102103

# Set the FilePrefix from the RunNumber
include("RecExTB/RecExTB_SetFlags_jobOptions.py")

if not 'FilePrefix' in dir():
   FilePrefix = "daq_SFI-51_combined"
if not 'InputDirectory' in dir():   
   #InputDirectory = "/castor/cern.ch/atlas/testbeam/combined/2004"
   #InputDirectory = "/afs/cern.ch/user/i/idctb/scratch0/Data/TB04"
   InputDirectory = "/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-ctb/testbeam/combined/2004"
# EvtMax
if not 'EvtMax' in dir():
   EvtMax=10

# Job type
if not 'doSim' in dir():
   doSim=False

if doSim:
   #-------------------------------------------------------------
   # For Monte Carlo only:
   # if doSim=false, the ntuple will be filled with the right beam information
   # anyway; if doSim=true, the ntuple will be filled with the figures below;
   # Variables in the ntuple are: RunEta, RunEnergy, RunPartType
   #----------------------------------------------------------
   if not 'InputFileNames' in dir():
      InputFileNames = ["ctb_CaloDigit.pool.root"] 
   if not 'BeamEnergy' in dir():
      BeamEnergy = 180
   if not 'BeamType' in dir():   
      BeamType = 3
   if not 'ComputeBeamEnergy' in dir():   
      ComputeBeamEnergy = True

# Reco type
# Subdetectors      
if not 'doInDet' in dir():
   doInDet=False
if not 'doMuons' in dir():
   doMuons=False
if not 'doTile' in dir():
   doTile=True
if not 'doLAr' in dir():
   doLAr=True
if not 'doBeamDetectors' in dir():
   doBeamDetectors=True
if not 'doTrigger' in dir():
   doTrigger = False

# Physics objects
if not 'doCaloMuId' in dir():
   doCaloMuId=True
if not 'doEgamma' in dir():
   doEgamma=True
if not 'doTrackToCalo' in dir():
   doTrackToCalo=False

# --------------------------------------------
# Cluster types in Calorimeters:
# --------------------------------------------
#Combined Sliding Window
if not 'doCaloCluster' in dir():
   doCaloCluster=True
if not 'doEMCaloCluster' in dir():
   doEMCaloCluster=False
if not 'doCaloCellRescaler' in dir():
   doCaloCellRescaler = True
#Topological Clustering on LAr and Tile
if not 'doCaloTopoCluster' in dir():
   doCaloTopoCluster=True
if not 'doCellWeightCalib' in dir():
   doCellWeightCalib=False
if not 'doCaloTopoClusterEM' in dir():
   doCaloTopoClusterEM=False
if not 'doCaloTopoLocalCalib' in dir():
   doCaloTopoLocalCalib=True
if not 'doCaloTopoLocalCalibDb' in dir():
   doCaloTopoLocalCalibDb=True

# Output   
if not 'doCBNT' in dir():
   doCBNT=True
if doCBNT:
   if not 'OutputCBNTFile' in dir():
      OutputCBNTFile = "ntuple.root"
if not 'doWriteESD' in dir():
   doWriteESD=True
if doWriteESD:
   if not 'OutputESDFile' in dir():
      OutputESDFile = "reconstruction.pool.root"
if not 'doCaloCalibHit' in dir():
   doCaloCalibHit = True
if not 'doJiveXML' in dir():
   doJiveXML = False
if not 'AtlantisGeometry' in dir():
   AtlantisGeometry = False
if not 'OutputLevel' in dir():
   OutputLevel = 2

# --------------------------------------------
# For simulation, do not run BeamDetectors
# --------------------------------------------
if doSim:
    doBeamDetectors = False

#LAr SW and LAr 3x3 cluster
if doLAr:
    if not 'doLArOFC' in dir():
       doLArOFC          = True # if 'False' use Parabola reconstruction
    if not 'doLArTimingNtuple' in dir():   
       doLArTimingNtuple = True    
    if not 'doEmCluster' in dir():   
       doEmCluster=False
    if not 'doEMTBCluster' in dir():   
       doEMTBCluster=True

# -----------------------------------------------
# For simulation, do not create LArTimingNtuple
# ----------------------------------------------
if doSim:
    doLArTimingNtuple = False

# --------------------------------------------
# For detector studies only :
# meaningless for simulation and far too large for production !
# --------------------------------------------
if doCBNT:    
   if not 'doDetailedNtuple' in dir():
      doDetailedNtuple=True
   if doTile:
      doTileNtuple=True
   else:
      doTileNtuple=False

   CBNTAthenaAware = True

# ------------------------------------
# import GlobalFlags and globalflags, the first will be soon deprecated
# ------------------------------------
from AthenaCommon.GlobalFlags import GlobalFlags
from AthenaCommon.GlobalFlags import globalflags


GlobalFlags.DetGeo.set_ctbh8()
globalflags.DetGeo='ctbh8'
# zero luminosity (obviously...)
GlobalFlags.Luminosity.set_zero()
globalflags.Luminosity='zero'
# DDV
globalflags.DetDescrVersion='CTB'
GlobalFlags.DetDescrVersion='CTB'
from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "CTB"

if doSim:
    # Reading G4 simulated data from Pool
    GlobalFlags.DataSource.set_geant4()
    GlobalFlags.InputFormat.set_pool()
    globalflags.DataSource='geant4'
    globalflags.InputFormat='pool'
else:
    # Reading ByteStream real data
    GlobalFlags.DataSource.set_data()
    GlobalFlags.InputFormat.set_bytestream()
    globalflags.DataSource='data'
    globalflags.InputFormat='bytestream'


#-----------------------------------------
# Set internal flags for each subdetector
# ----------------------------------------


# Change InDet flags
if doInDet or doTrigger:
    # InDet setup flags with default values
    include( "InDetTBRecExample/InDetTBFlags_jobOptions.py" )
    InDetTBFlags.doSim = doSim
    InDetTBFlags.RunNumber = RunNumber
    InDetTBFlags.Pixels=True
    InDetTBFlags.SCT=True
    InDetTBFlags.TRT=True
    InDetTBFlags.Magnet=False
    InDetTBFlags.Current = -850.
    InDetTBFlags.ConstBfield= False
    InDetTBFlags.OracleTag = "InnerDetector-CTB-04"
    InDetTBFlags.SelectRDO=False
    if doSim:
        InDetTBFlags.doTruth=True
    else:
        InDetTBFlags.doTruth=False
    InDetTBFlags.TRT_LUT=False    
    InDetTBFlags.xKalman=False
    InDetTBFlags.iPatRec=False
    InDetTBFlags.CTBTracking=True
    InDetTBFlags.Refit=False
    InDetTBFlags.ParticleCreation=True
    InDetTBFlags.CBNT=True
    InDetTBFlags.makeAlignNtp = False
    InDetTBFlags.JiveXML=False
    InDetTBFlags.AtlantisGeometry=False
    # hotfix for iPatRec flags using private RecExcommon settings
    if InDetTBFlags.iPatRec:
        doTruth = InDetTBFlags.doTruth
    # --- The selection of RDOs only works for real data for the moment
    if InDetTBFlags.doSim:
        InDetTBFlags.SelectRDO = False;
    
    InDetTBFlags.doTrackSegments = True
    if InDetTBFlags.doTrackSegments:
        InDetTBFlags.TRT_LUT=False
        InDetTBFlags.xKalman=False
        InDetTBFlags.iPatRec=False
        InDetTBFlags.IDSCAN=False
        InDetTBFlags.Refit = False

    # Set magnet, Current and OracleTag flags
    include("InDetTBRecExample/InDetTBSetFlags_jobOptions.py")
    if not InDetTBFlags.Magnet:
        InDetTBFlags.doMaterialInteractions=False
    #else:
    #    InDetTBFlags.Magnet      = False # load old field suitable for muons
    #    InDetTBFlags.Current     = 0.
    #    InDetTBFlags.ConstBfield = True


# change Muon flags
if doMuons:
    # Muons setup flags with default values
    include( "MuonTBRecExample/MuonTBFlags_jobOptions.py" )
    # Muons setup flags with default values
    include( "MuonTBRecExample/MuonTBFlags_jobOptions.py" )
    MuonLayout="CTB2004"   
    # Choose detectors to run reconstruction
    MuonTBFlags.MDT=True
    MuonTBFlags.RPC=True
    MuonTBFlags.TGC=True
    MuonTBFlags.MUCTPI=False
    # Choose which ntuple blocks have to be filled
    if doSim:
        MuonTBFlags.G4_Simulation = True
        MuonTBNtuples.MdtRaw   = False
        MuonTBNtuples.RpcRaw   = False
    else:
        MuonTBFlags.G4_Simulation = False
        if (RunNumber > 2100020):
            MuonTBFlags.Combined = True
        else:    
            MuonTBFlags.MDT_RotatingBIL = True
            MuonTBFlags.MDT_BOS = True
        MuonTBNtuples.MdtRaw   = False
        MuonTBNtuples.RpcRaw   = False                
    MuonTBNtuples.MdtDigit = True
    MuonTBNtuples.RpcDigit = True
    MuonTBNtuples.TgcDigit = False
    MuonTBNtuples.MUCTPI   = False
    MuonTBNtuples.MooreSegments = True
    MuonTBNtuples.RpcAnalysis   = False
    MuonTBNtuples.TgcAnalysis   = False
    # choose which reconstruction algs to run 
    MuonTBRecoAlgs.MOORE = True
    MuonTBRecoAlgs.Muonboy = True
    MuonTBNtuples.MuonboyTBNT = True
    
# LAr setup flags with default values
if doLAr:
   include( "LArTBRec/LArTBFlags_jobOptions.py" )

# Combined calorimeters reconstruction setup flags with default values
if doLAr or doTile:
   include( "CaloTBRec/CaloTBFlags_jobOptions.py" )


# ----------------------------------
# import DetFlags from AthenaCommon
# ----------------------------------
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.Resilience import treatException

if doInDet or doTrigger:
    DetFlags.detdescr.ID_setOn()
    if doSim:
        DetFlags.readRDOPool.ID_setOn()
    else:
        DetFlags.readRDOBS.ID_setOn()
    DetFlags.makeRIO.ID_setOn()        
if doLAr:
    DetFlags.detdescr.LAr_setOn()
    if doSim:
        DetFlags.readRDOPool.LAr_setOn()
    else:
        DetFlags.readRDOBS.LAr_setOn()
    DetFlags.makeRIO.LAr_setOn()  
if doTile:
    DetFlags.detdescr.Tile_setOn()
    if doSim:
        DetFlags.readRDOPool.Tile_setOn()
    else:
        DetFlags.readRDOBS.Tile_setOn()
    DetFlags.makeRIO.Calo_setOn()      
if doMuons:
    DetFlags.detdescr.Muon_setOn()
    if doSim:
        DetFlags.readRDOPool.Muon_setOn()
    else:
        DetFlags.readRDOBS.Muon_setOn()
    DetFlags.makeRIO.Muon_setOn()  

DetFlags.Print()

#-----------------------
# ByteStream Input 
#-----------------------    
if (globalflags.InputFormat == 'bytestream'):
    include( "ByteStreamCnvSvc/TBEventSelector_jobOptions.py" )
    include( "ByteStreamCnvSvcBase/BSAddProvSvc_RIO_jobOptions.py" )
    include( "ByteStreamCnvSvcBase/BSAddProvSvc_RDO_jobOptions.py" )

    # Specify input file
    theByteStreamInputSvc=svcMgr.ByteStreamInputSvc
    theByteStreamInputSvc.FullFileName = [ InputDirectory + "/" + FilePrefix + "_" + str(RunNumber) + "_file01.data" ]
    
    #First file to process
    #ByteStreamInputSvc.FirstFile      += [ 2 ]
    # Number of files to process
    #ByteStreamInputSvc.NumFile      += [ 1 ]
    # Number of events to skip at the beginning of the job
    #EventSelector.SkipEvents      = 50000
    # Reader type "Castor" or "Simple" 
    #ByteStreamEventStorageInputSvc = Service( "ByteStreamEventStorageInputSvc" )
    #ByteStreamEventStorageInputSvc.ReaderType = "Castor";

if doSim:
    from RecExConfig.ObjKeyStore import objKeyStore
    objKeyStore.readInputFile('RecExPers/OKS_streamRDO.py')

# ---------------------
# setup GeoModel
# ---------------------

from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion

if doTile:
   include ("TileTBRec/TileTBDefaults_jobOptions.py")
#------------------------
# Read from POOL
#------------------------
if (globalflags.InputFormat == 'pool'):
    include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
    if DetFlags.readRDOPool.ID_on():
        include( "InDetEventAthenaPool/InDetEventAthenaPool_joboptions.py" )
    if DetFlags.readRDOPool.LAr_on():
        include( "LArAthenaPool/LArAthenaPool_joboptions.py" )
    if DetFlags.readRDOPool.Tile_on():
        include( "TileEventAthenaPool/TileEventAthenaPool_joboptions.py" )
    if DetFlags.readRDOPool.Muon_on():
        include( "MuonEventAthenaPool/MuonEventAthenaPool_joboptions.py" )
    include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
    include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )

    svcMgr.EventSelector.InputCollections = InputFileNames


# Block the trigger tower mapping, doesn't exist for TB
include.block ( "CaloConditions/LArTTCellMap_ATLAS_jobOptions.py" )
include.block ( "CaloConditions/CaloTTIdMap_ATLAS_jobOptions.py" )


#-------------------------------------------------------------
# Detector description, conditions database and BS converters
#-------------------------------------------------------------

# Calo
if doLAr or doTile:
    # Need this to run only LAr
    if ( not doTile ):
        include( "TileIdCnv/TileIdCnv_jobOptions.py" )    
    #LAr
    include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_H8_joboptions.py" )
    # load Tile conditions data
    include( "TileConditions/TileTBConditions_jobOptions.py" )
    # frag to ROD mapping in 2004
    include( "TileTBRec/TileTBMap2004_jobOptions.py" )
    # conversion between local and ctb coordinate systems
    include ("TBCaloGeometry/TBCaloGeometry_H8_joboptions.py")

# Beam Detectors
if doBeamDetectors:
    include ("LArIdCnv/LArIdCnv_joboptions.py")
    include ("TBRec/H8BeamRec_jobOptions.py")
else:    
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    
if doLAr :
    if not doSim:
        # read ByteStream and run RawChannelBuilder
        if doLArOFC:
            # force BeamDetectors, they are needed for OFC's
            if not doBeamDetectors:
                include ("LArIdCnv/LArIdCnv_joboptions.py")
                include ("TBRec/H8BeamRec_jobOptions.py")
            # use this options to select the OFC method...
            useTCMOFC = False
            useRTMOFC = True
            include( "LArTBRec/LArTBRec_H8_OFC_jobOptions.py" )
        else:
            #include( "LArTBRec/LArTBRec_H8_Simple_jobOptions.py" )
            include( "LArTBRec/LArTBRec_H8_HalfSimple_jobOptions.py" )
            #include( "LArTBRec/LArTBRec_H8_jobOptions.py" )

    if doSim:
#        include("LArCondCnv/LArCondCnv_G4_jobOptions.py")
        from LArConditionsCommon.LArCondFlags import larCondFlags
        include("LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py")

    
if doTile:
    if not doSim :
        # read ByteStream and run RawChannelBuilder
        include( "TileTBRec/TileTBRec_jobOptions.py" )

# Inner Detector
if doInDet:
   if InDetTBFlags.xKalman or InDetTBFlags.iPatRec or InDetTBFlags.Refit or InDetTBFlags.Magnet or MuonTBRecoAlgs.MOORE or MuonTBRecoAlgs.Muonboy or InDetTBFlags.CTBTracking or InDetTBFlags.doTrackSegments:
    # get field service
     include ( "InDetTBRecExample/InDetTBMagneticFieldSetup.py" )
     InDetField = ConfiguredInDetMagneticField(InDetTBFlags.Current,
                                              InDetTBFlags.ConstBfield)
     InDetField.loadFieldMap()
#   InDetField.loadUserFieldMap("MyMapFile.data", IdSymmetricMap=False)
if doInDet or doTrigger:
    # Detector description
    include ("InDetTBRecExample/InDetTB2004DetDescr_jobOptions.py")
        
    # BS converters 
    if not doSim:
        include( "InDetTBRecExample/InDetTBBSCnv_jobOptions.py" )
    
# Muons
if doMuons:
    if not (doSim or doInDet or doTrigger):
        # Specify the Converters
        ByteStreamCnvSvc = Service( "ByteStreamCnvSvc" )
        theApp.CreateSvc += [ "ByteStreamCnvSvc" ]    
    # Initialize MuonGeoModel
    MuonDetectorTool.Id_helpers_from_converters =0
    if MuonTBFlags.BISon:
        MuonDetectorTool.IncludeCtbBis = 1

    # Initialize bytestream converters
    include( "MuonTestBeamCnv/jobOptions_MuonTestBeam.py" )
    if (RunNumber > 2101500):
        RPCcablingSvc.ConfFileName = "TB_25ns_LVL1conf.data"
        RPCcablingSvc.CorrFileName = "TB_25ns_LVL1conf.corr"
    # Load calibrations
    include( "MuonTBRecExample/MuonTBLoadCal_jobOptions.py" )


if doSim:
    GeoModelSvc.InDetVersionOverride = "InnerDetector-CTB-04-01"
    GeoModelSvc.TileVersionOverride  = "TileTB-3B-00"
    GeoModelSvc.IgnoreTagDifference = True


# -------------------------------------------------------------------
# Configure further tools needed by Trigger, iPatRec, xKalman, ...
# -------------------------------------------------------------------
if doInDet:
   if InDetTBFlags.CTBTracking or InDetTBFlags.xKalman or InDetTBFlags.iPatRec or InDetTBFlags.TRT_LUT:    
      # particle property service
      include( "PartPropSvc/PartPropSvc.py" )
   if InDetTBFlags.iPatRec and not doTrigger:
      # prepare LUT for Region Selector used in TRT_Rec
      include("InDetRegionSelector/TRT_RegionSelectorTable_jobOptions.py" )
if doTrigger:
    # prepare LUT for Region Selector
    include("InDetRegionSelector/InDetRegionSelectorTable_jobOptions.py" )

# -----------------------------
# Reconstruction
# -----------------------------           
# Calorimeter reconstruction :
if doLAr or doTile:
    from CaloRec.CaloTopoClusterFlags import jobproperties
    from CaloRec.CaloRecFlags import jobproperties
    jobproperties.CaloTopoClusterFlags.doCellWeightCalib.set_Value_and_Lock(False)
    if not doCaloTopoLocalCalib:
        jobproperties.CaloTopoClusterFlags.doTopoClusterLocalCalib.set_Value_and_Lock(False)
    if doCaloTopoCluster:
        jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock(True)
        if doCaloTopoLocalCalib:   
           jobproperties.CaloTopoClusterFlags.doTopoClusterLocalCalib.set_Value_and_Lock(True)
    else:
        jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock(False)
    if doCaloTopoClusterEM:
        jobproperties.CaloRecFlags.doCaloEMTopoCluster.set_Value_and_Lock(True)
    else:
        jobproperties.CaloRecFlags.doCaloEMTopoCluster.set_Value_and_Lock(False)
    if doCaloCluster:
        jobproperties.CaloRecFlags.doCaloCluster.set_Value_and_Lock(True)
    else:
        jobproperties.CaloRecFlags.doCaloCluster.set_Value_and_Lock(False)
    if doEMCaloCluster:
        jobproperties.CaloRecFlags.doEmCluster.set_Value_and_Lock(True)
    else:
        jobproperties.CaloRecFlags.doEmCluster.set_Value_and_Lock(False)

    from CaloRec.CaloCellFlags import jobproperties
    jobproperties.CaloCellFlags.doLArNoiseMasking = False
    jobproperties.CaloCellFlags.doLArHVCorr = False
    jobproperties.CaloCellFlags.doLArSporadicMasking = False
    jobproperties.CaloCellFlags.doLArCellEmMisCalib = False
    jobproperties.CaloCellFlags.doPedestalCorr = False
    jobproperties.CaloCellFlags.doLArRecalibration = False
    jobproperties.CaloCellFlags.doDeadCellCorr = False

    include( "CaloTBRec/CaloTBRec_H8_Cell_jobOptions.py" )
    #Cell correction
    if doCaloCellRescaler and not doSim:
       from CaloRec.CaloRecConf import CaloCellContainerCorrectorTool
       CellRescalerTool = CaloCellContainerCorrectorTool("CellRescalerTool")
       ToolSvc += CellRescalerTool
       from CaloCellCorrection.CaloCellCorrectionConf import CaloCellRescaler
       CaloCellRescaler = CaloCellRescaler()
       ToolSvc += CaloCellRescaler
       ToolSvc.CellRescalerTool.CellCorrectionToolNames+=[ToolSvc.CaloCellRescaler.getFullName()]
       ToolSvc.CellRescalerTool.CaloNums=[0,3] #LAr EM and Tile
       if doSim:
            factor=1
            ToolSvc.CaloCellRescaler.FactorToCellsEMB0 = factor; # 0.96 due to non-well-modeled E-Field 
            ToolSvc.CaloCellRescaler.FactorToCellsEMB1 = factor
            ToolSvc.CaloCellRescaler.FactorToCellsEMB2 = factor
            ToolSvc.CaloCellRescaler.FactorToCellsEMB3 = factor
            ToolSvc.CaloCellRescaler.FactorToCellsTILEB0 = factor
            ToolSvc.CaloCellRescaler.FactorToCellsTILEB1 = factor
            ToolSvc.CaloCellRescaler.FactorToCellsTILEB2 = factor
       else:
            factorlar = 1.038
            factortile=1.0
            ToolSvc.CaloCellRescaler.FactorToCellsEMB0 = 1.0;
            ToolSvc.CaloCellRescaler.FactorToCellsEMB1 = factorlar*0.9907 #calibration resistors
            ToolSvc.CaloCellRescaler.FactorToCellsEMB2 = factorlar
            ToolSvc.CaloCellRescaler.FactorToCellsEMB3 = factorlar
            ToolSvc.CaloCellRescaler.FactorToCellsTILEB0 = factortile
            ToolSvc.CaloCellRescaler.FactorToCellsTILEB1 = factortile/0.977 #BC-factor from Cs/muon
            ToolSvc.CaloCellRescaler.FactorToCellsTILEB2 = factortile/0.919 #A-factor from Cs/muon

       topSequence.CaloCellMaker.CaloCellMakerToolNames+=[ToolSvc.CellRescalerTool.getFullName()]

    if doCaloTopoCluster and doCaloTopoLocalCalib:   
        #  local had calibration for a moment from local files or redirect to different DB instance
        iovdbsvc = svcMgr.IOVDbSvc
        myIOVDbFolders = [] 
        for x in iovdbsvc.Folders: 
             if x.find("/CALO/HadCalibration/CaloEMFrac") == -1 and \
                x.find("/CALO/HadCalibration/H1ClusterCellWeights") == -1 and \
                x.find("/CALO/HadCalibration/CaloOutOfCluster") == -1 and \
                x.find("/CALO/HadCalibration/CaloOutOfClusterPi0") == -1 and \
                x.find("/CALO/HadCalibration/CaloDMCorr2") == -1:
                    print("adding ",x," to IOVDbSvc.Folders")
                    myIOVDbFolders += [x]
             else:
                    print("removing ",x," from IOVDbSvc.Folders")

        iovdbsvc.Folders = myIOVDbFolders
        if not doCaloTopoLocalCalibDb or doSim:
             # read from local files, default are here, could be changed to some other
             iovdbsvc.Folders+=["/CALO/HadCalibration/CaloEMFrac"+"<dbConnection>sqlite://X;schema=myEMFrac05BERT200.db;dbname=OFLP200</dbConnection>"+"<tag>CaloEMFrac-CSC05-BERT</tag>"]
             iovdbsvc.Folders+=["/CALO/HadCalibration/H1ClusterCellWeights"+"<dbConnection>sqlite://X;schema=myInvWeight05BERT200.db;dbname=OFLP200</dbConnection>"+"<tag>CaloH1CellWeights-CSC05-BERT</tag>"]
             iovdbsvc.Folders+=["/CALO/HadCalibration/CaloOutOfCluster"+"<dbConnection>sqlite://X;schema=myOOC05BERT200.db;dbname=OFLP200</dbConnection>"+"<tag>CaloHadOOCCorr-CSC05-BERT</tag>"]
             iovdbsvc.Folders+=["/CALO/HadCalibration/CaloOutOfClusterPi0"+"<dbConnection>sqlite://X;schema=myOOCPi005BERT200.db;dbname=OFLP200</dbConnection>"+"<tag>CaloHadOOCCorrPi0-CSC05-BERT</tag>"]
             iovdbsvc.Folders+=["/CALO/HadCalibration/CaloDMCorr2"+"<dbConnection>sqlite://X;schema=myDBHadDMCoeff200.db;dbname=OFLP200</dbConnection>"+"<tag>CaloHadDMCorr-002-00</tag>"]
             print(iovdbsvc.Folders)
             if not os.access('myEMFrac05BERT200.db',os.R_OK):
                os.system('cp -f /afs/cern.ch/user/m/menke/public/AtlasOffline-14.2.21/myEMFrac05BERT200.db .')
             if not os.access('myInvWeight05BERT200.db',os.R_OK):
                os.system('cp -f /afs/cern.ch/user/m/menke/public/AtlasOffline-14.2.21/myInvWeight05BERT200.db .')
             if not os.access('myOOC05BERT200.db',os.R_OK):
                os.system('cp -f /afs/cern.ch/user/m/menke/public/AtlasOffline-14.2.21/myOOC05BERT200.db .')
             if not os.access('myOOCPi005BERT200.db',os.R_OK):
                os.system('cp -f /afs/cern.ch/user/m/menke/public/AtlasOffline-14.2.21/myOOCPi005BERT200.db .')
             if not os.access('myDBHadDMCoeff200.db',os.R_OK):
                os.system('cp -f /afs/ipp-garching.mpg.de/home/g/gdp/public/HadDMCoeff2/myDBHadDMCoeff200.db .')

             from AthenaCommon.Utils.unixtools import FindFile
             localCondFile='PoolCat_oflcond.xml'
             import os
             dataPathList = os.environ['DATAPATH'].split(':')
             for j in range(0,len(dataPathList)): 
                dataPathList[j] += '/poolcond/'
             condFile=FindFile(localCondFile,dataPathList,os.R_OK)
             if condFile:
                 os.system('mkdir poolcond; cp '+condFile+' poolcond/'+localCondFile)
                 for x in ['mc08.10741_0-1.singlepart_singlepiplusminus_logE.recon.ESD.e342_s439_14.2.21_outofcluster_athena.pool.root',
                           'mc08.10741_0-1.singlepart_singlepiplusminus_logE.recon.ESD.e342_s439_14.2.21_weights_from_inv_weights_athena.pool.root', 
                           'mc08.10741_0-2.singlepart_singlepiplusminuszero_logE.recon.ESD.e342_s439_14.2.21_classify_noefrac_athena.pool.root',
                           'mc08.107412.singlepart_singlepizero_logE.recon.ESD.e342_s439_14.2.21_outofcluster_athena.pool.root']:
                      if not os.access(x,os.R_OK):
                           os.system('cp -f /afs/cern.ch/user/m/menke/public/AtlasOffline-14.2.21/'+x+' .')
                      os.system('pool_insertFileToCatalog -u xmlcatalog_file:poolcond/'+localCondFile+' '+x)
                 if not os.access('HadDMCoeff2_v21_1a.pool.root',os.R_OK):
                      os.system('cp -f /afs/ipp-garching.mpg.de/home/g/gdp/public/HadDMCoeff2/HadDMCoeff2_v21_1a.pool.root .')
                 os.system('pool_insertFileToCatalog -u xmlcatalog_file:poolcond/'+localCondFile+' HadDMCoeff2_v21_1a.pool.root')

        else:         
             # change the DB for weights 
             iovdbsvc.Folders+=["/CALO/HadCalibration/CaloEMFrac"+"<dbConnection>COOLONL_CALO/OFLP200</dbConnection>"+"<tag>CaloEMFrac-CSC05-ECut-BERT</tag>"]
             iovdbsvc.Folders+=["/CALO/HadCalibration/H1ClusterCellWeights"+"<dbConnection>COOLONL_CALO/OFLP200</dbConnection>"+"<tag>CaloH1CellWeights-CSC05-ECut-BERT</tag>"]
             iovdbsvc.Folders+=["/CALO/HadCalibration/CaloOutOfCluster"+"<dbConnection>COOLONL_CALO/OFLP200</dbConnection>"+"<tag>CaloHadOOCCorr-CSC05-ECut-BERT</tag>"]
             iovdbsvc.Folders+=["/CALO/HadCalibration/CaloOutOfClusterPi0"+"<dbConnection>COOLONL_CALO/OFLP200</dbConnection>"+"<tag>CaloHadOOCCorrPi0-CSC05-ECut-BERT</tag>"]
             iovdbsvc.Folders+=["/CALO/HadCalibration/CaloDMCorr2"+"<dbConnection>COOLONL_CALO/OFLP200</dbConnection>"+"<tag>CaloHadDMCorr-CSC05-ECut-BERT</tag>"]
    
    if doCaloCluster:
       include( "CaloTBRec/CaloTBRec_H8_Cluster_jobOptions.py" )

# InnerDetector
if doInDet:
    include( "InDetTBRecExample/InDetTBRec_jobOptions.py" )
else:
    if doTrigger:
        include( "InDetTBRecExample/InDetTBPRDSpacePoints_jobOptions.py" )

# Muons
if doMuons:
    include( "MuonTBRecExample/MuonTBRecoAlgs_jobOptions.py" )
    #MboyConfig = Algorithm( "MboyConfig" )
    MboySvc.LevelOfRecons              = 3
    MboySvc.MinRCHAMI = 3100.
    MboyRec = Algorithm( "MboyRec" )
    MboyRec.ShortSummaryTrack = 0
    MboyRec.ShortSummarySegment = 0
    MboyRec.TDCcnv=1
    if not doSim:
        MboySvc.AlignmentCorr = 2
        MboySvc.MDTTubeResolution      = 0.05

# TrackToCalo
if doTrackToCalo and doInDet and doLAr:
    include( "TBRec/TBTrackToCaloAlg_jobOptions.py" )

    # if TrackParticles are not built, use tracks : 
    if not InDetTBFlags.ParticleCreation:
        TrackToCaloAlg.TrackInputType = "Tracks"

    # for photon runs analysis, change default input :
    if InDetTBFlags.doTrackSegments and not InDetTBFlags.CTBTracking and not InDetTBFlags.RtfTracking:
        TrackToCaloAlg.TrackInputType = "Tracks"
        TrackToCaloAlg.TrackName="ConvertedXKalmanTracks"

# egamma
if doEgamma and doLAr and doInDet and InDetTBFlags.ParticleCreation:
    include ( "egammaRec/egammaRec_H8_jobOptions.py")
    #ToolSvc.emtrackmatch.newExtrapol        = True
    ToolSvc.emtrackmatch.useCandidateMatch  = False
    include ( "egammaRec/egammaSofteRec_H8_jobOptions.py")

# Calo muon id
# ( note that TileMuid is run as well, but fragment does CBNT as well
#  -> is below )
if doCaloMuId and doMuons and doLAr :
    include( "TBRec/MuToCaloAlg_jobOptions.py" )

# Calorimeter calibration hits :
if doCaloCalibHit and doSim:
   include( "CaloSimEvent/CaloSimEventDict_joboptions.py" )
    
#------------------------------
# CBNT general
#-----------------------------
if doCBNT:
   #  common part
   include ("CBNT_Athena/CBNT_AthenaAware_jobOptions.py")
   CBNT_AthenaAware.StreamName="FILE1"
   CBNT_AthenaAware.TreeName="tree"
   include( "CBNT_Athena/CBNT_EventInfo_jobOptions.py" )

   from AnalysisTools.AnalysisToolsConf import AANTupleStream
   topSequence += AANTupleStream()
   topSequence.AANTupleStream.ExtraRefNames = [ "" ]
   topSequence.AANTupleStream.TreeName = "tree"
   topSequence.AANTupleStream.StreamName = "FILE1"
   topSequence.AANTupleStream.ExistDataHeader = False

   include("TBRec/CBNT_TBInfo.py")
   if doSim:
      CBNT_TBInfo.BeamEnergy = BeamEnergy
      CBNT_TBInfo.BeamType = BeamType

   # Fill Truth information
   if doSim:
      include( "RecExCommon/CBNT_Truth_jobOptions.py" )

   # non-ATLAS detectors
   if doBeamDetectors:
      if not doSim:
         include( "TBRec/H8CBNTBeam_jobOptions.py" )

   # Calo :
   if doLAr or doTile :
      include( "CaloTBRec/CaloTBRec_H8_ntuple_jobOptions.py" )
      if doTileNtuple:
         include( "TileTBRec/TileTBNtuple_jobOptions.py" )
         TileTBNtuple.StreamName="FILE1"
      if doLArTimingNtuple:
         include ( "LArTBRec/LArTBRec_H8_TimingNtuple_jobOptions.py" )

      if doDetailedNtuple and not doSim:
         include( "LArTBRec/LArTBRec_H8_DetailedNtuple_jobOptions.py" ) 
         #CBNT_LArDigits.OutputLevel = 1

   # Inner Detector
   if doInDet:
      if InDetTBFlags.CBNT:
         include( "InDetTBRecExample/InDetTBNtuple_jobOptions.py" )
         include ("CBNT_Particle/CBNT_Particle_jobOptions.py")
        
   # Muons
   if doMuons:
      # include ntuple algs for LVL1, RDO, DIGITS
      include ( "MuonTBRecExample/MuonTBNtuples_jobOptions.py"  )
      # include MuonBoy variables in the ntuple
      include ("MuonboyAth/CBNT_Muon_jobOptions.py")
      MuonTBNtuples.MuonboyTBNT = True

   # TrackToCalo
   if doTrackToCalo and doInDet and doLAr:
      include( "TrackToCalo/CBNT_TrackToCalo_jobOptions.py" )
      CBNT_TrackToCalo.NTupleVariablePrefix="IIC_InDet"

   #egamma
   if doEgamma and doLAr and doInDet and InDetTBFlags.ParticleCreation:
      include ( "egammaRec/egamma_CBNT_H8_jobOptions.py")

   # calo mu id
   if doCaloMuId and doTile:
      include ( "TileMuId/CBNT_TileMu_jobOptions.py" )
      include ( "TileMuId/TileMuId_jobOptions.py" )
   if doCaloMuId and doMuons:
      include( "TrackToCalo/CBNT_MuToCalo_jobOptions.py" )
   if doInDet and doCaloMuId and doLAr:
      include( "LArMuId/LArMuId_jobOptions.py")

   # Calorimeter calibration hits :
   if doCaloCalibHit and doSim:
      include( "CaloCalibHitRec/CalibHit_CBNT_jobOptions.py" )
      include( "CaloCalibHitRec/CalibrationInfo_CBNT_jobOptions.py" )
    
   # --------------------------
   # Trigger
   # --------------------------
   if doTrigger:
      include( "TriggerJobOpts/jobOfragment_forRecExTB.py" )
      include( "TriggerJobOpts/jobOfragment_TriggerCBNT.py" )

   # output stream for CBNT
   from AthenaCommon.AppMgr import ServiceMgr
   if not hasattr(ServiceMgr, 'THistSvc'):
      from GaudiSvc.GaudiSvcConf import THistSvc
      ServiceMgr += THistSvc()

   ServiceMgr.THistSvc.Output = [ "FILE1 DATAFILE='" + OutputCBNTFile + "' OPT='RECREATE'" ]


# -----------------------------------------------
# JiveXML
# -----------------------------------------------

if doJiveXML:
    include ( "JiveXML/JiveXML_jobOptionBase.py" )
    if doSim:
        #include ( "JiveXML/DataTypes_Truth.py" )
        ToolSvc.EventData2XML.DataTypes += ["TruthTrack"]
        ToolSvc.EventData2XML.McEventCollection="TruthEvent"     
    if doLAr or doTile:
        include ( "JiveXML/DataTypes_Calos.py" )
    if doInDet:
        include ( "JiveXML/DataTypes_InDet.py" )
	#ToolSvc.EventData2XML.PixelSpacePoints         = "PixelSpacePoints"
        #ToolSvc.EventData2XML.SCTSpacePoints           = "SCT_SpacePoints"
        #ToolSvc.EventData2XML.TRTClusters              = "TRT_DriftCircles";   
        #ToolSvc.EventData2XML.iPatTracks               = "iPatTracks" 
        #ToolSvc.EventData2XML.xKalmanTracks            = "Tracks"
        #ToolSvc.EventData2XML.DataTypes += ["SiSpacePoint","SiCluster","TRT",
        #                    "xKalmanTrack","iPatTrack"]
        
    if doMuons:    
        #include ( "JiveXML/DataTypes_Muons.py" )
        #Moore polyline stuff 
        MooMakeTracks = Algorithm( "MooMakeTracks" )
        MooMakeTracks.atlantis = TRUE
        #==============================================================
        # Tell the algorithm where to find various objects in storegate
        #==============================================================
        ToolSvc.EventData2XML.MDTDigits                = "MDT_DIGITS" 
        #ToolSvc.EventData2XML.TGCDigits                = "TGC_DIGITS" 
        #ToolSvc.EventData2XML.CSCDigits                = "CSC_DIGITS" 
        #ToolSvc.EventData2XML.CSCClusters              = "cscClusters"
        ToolSvc.EventData2XML.RPCDigits                = "RPC_DIGITS" 
        #ToolSvc.EventData2XML.muid                    = "MuidStandAlonenoSeed";
        # adding a name to the list will convert the objects to XML
        # removing a name from the list will stop the conversion
        ToolSvc.EventData2XML.DataTypes += ["Moore","MDTDigit","RPCDigit"]
    if AtlantisGeometry:
        ToolSvc.EventData2XML.WriteGeometry = True   
    ToolSvc.EventData2XML.TestBeam=True
    
#-------------------------------------------------
# Write out Transient Event Store content in POOL
#-------------------------------------------------
if doWriteESD:
#    if not doSim:
    include ("AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py")
    from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
    Stream1 = AthenaPoolOutputStream( "Stream1" )

    # Define the output file name
#    if not doSim:
    Stream1.OutputFile    = OutputESDFile
#    else: 
#       ToolSvc.Stream1.OutputFile = "reconstruction.pool.root"

    # list of output objects
    # Converters:
    include ( "EventAthenaPool/EventAthenaPool_joboptions.py")
    include ( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py")

    # Needed for TBEvent ESD writing
    include ( "TBEventAthenaPool/TBEventAthenaPool_joboptions.py")
    
    # for LAr or Tile since Calo Cell converter is there
    include ( "LArAthenaPool/LArAthenaPool_joboptions.py")
    include( "InDetEventAthenaPool/InDetEventAthenaPool_joboptions.py" )
    include( "TrkEventAthenaPool/TrkEventAthenaPool_joboptions.py" )
    include ( "RecAthenaPool/RecAthenaPool_joboptions.py")
    include ("TileEventAthenaPool/TileEventAthenaPool_joboptions.py")
    #include ("InDetEventCnvTools/InDetEventCnvTools_jobOptions.py")
    
    #  EventInfo
    try:
        fullItemList += ["EventInfo#*"]
    except:
        fullItemList = []
        fullItemList += ["EventInfo#*"]

    #HepMC record (be careful it has to be read by something to be written out)
    #Stream1.ItemList+=["McEventCollection#*"]
    include ( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPoolItemList_joboptions.py")
    include ("CaloAthenaPool/CaloAthenaPool_joboptions.py")
    #Calo/LAr

    if doLAr:
       # Add class LArClusterContainer to the fullItemList list
       fullItemList += [ "LArClusterContainer#*" ]
    if doLAr or doTile:   
       # Add class CaloCellContainer to the fullItemList list
       fullItemList += [ "CaloCellContainer#*" ]
       # Add class CaloClusterContainer to the fullItemList list
       fullItemList += [ "CaloClusterContainer#*" ]

    if doCaloMuId:
       fullItemList += [ "TileMuContainer#*" ]   
    #full tracking
    if doInDet or doMuons:
       fullItemList +=["TrackCollection#Tracks"]
       fullItemList +=["TrackCollection#ConvertedMBoyTracks"]
       fullItemList += [ "Rec::TrackParticleContainer#*" ]

    # Inner Detector
    if doInDet:
       fullItemList +=["InDet::SCT_ClusterContainer#*"]
       fullItemList +=["InDet::PixelClusterContainer#*"]
       fullItemList +=["InDet::TRT_DriftCircleContainer#*"]
   
    #Reconstruction
    include ( "RecAthenaPool/RecAthenaPoolItemList_joboptions.py")

    #TestBeam
    fullItemList += [ "TBBPCCont#*" ]
    fullItemList += [ "TBBPCRawCont#*" ]
    fullItemList += [ "TBEventInfo#*" ]
    fullItemList += [ "TBHitCont#*" ]
    fullItemList += [ "TBHitPlaneCont#*" ]
    fullItemList += [ "TBIdentifiedParticle#*" ]
    fullItemList += [ "TBLArCalibDigitContainer#*" ]
    fullItemList += [ "TBLArDigitContainer#*" ]
    fullItemList += [ "TBPhase#*" ]
    fullItemList += [ "TBScintillatorCont#*" ]
    fullItemList += [ "TBScintillatorRawCont#*" ]
    fullItemList += [ "TBTDC#*" ]
    fullItemList += [ "TBTDCRawCont#*" ]
    fullItemList += [ "TBTrack#*" ]
    fullItemList += [ "TBTriggerPatternUnit#*" ]
    fullItemList += [ "TBADCRawCont#*" ]
    if doTile:
       fullItemList += [ "TileBeamElemContainer#*" ]

    #adding digits for the moment
    fullItemList += [ "LArDigitContainer#*" ]
    fullItemList += [ "TileDigitsContainer#*" ] 


#    if not doSim:
    Stream1.ItemList = fullItemList
#    else:   
#       ToolSvc.Stream1.ItemList = fullItemList


#-----
# Tricks for conditions
#-----    
#conddb.iovdbsvc.dbConnection="impl=cool;techno=sqlite;schema=sqlite200/ALLP200.db;X:TBDP200"
#conddb.iovdbsvc.dbConnection=""
conddb.addFolder("LAR","/LAR/BadChannels/BadChannels<tag>LARBadChannelsBadChannels-MC-empty</tag>")
if not doSim:
   conddb.addFolder("LAR","/LAR/BadChannels/MissingFEBs<tag>LARBadChannelsMissingFEBs-empty</tag>")
else:
   conddb.iovdbsvc.Folders += ["<db>impl=cool;techno=logical;schema=COOLONL_LAR;X:TBDP200</db> /LAR/BadChannels/MissingFEBs<tag>LARBadChannelsMissingFEBs-empty</tag>"]
#conddb.addFolder("CALO","/CALO/Identifier/CaloTTOnOffIdMapAtlas")
#conddb.addFolder("CALO","/CALO/Identifier/CaloTTOnAttrIdMapAtlas")
conddb.iovdbsvc.Folders += ["<db>COOLONL_LAR/OFLP200</db> /LAR/Align<tag>LARAlign_CSC_00</tag>"]
conddb.iovdbsvc.Folders += ["<db>COOLONL_LAR/OFLP200</db> /LAR/Identifier/LArTTCellMapAtlas<tag>LARIdentifierLArTTCellMapAtlas-DC3-05</tag>"]
conddb.iovdbsvc.Folders += ["<db>COOLONL_CALO/OFLP200</db> /CALO/Identifier/CaloTTOnAttrIdMapAtlas <tag>CALOIdentifierCaloTTOnAttrIdMapAtlas-0001</tag>"]
conddb.iovdbsvc.Folders += ["<db>COOLONL_CALO/OFLP200</db> /CALO/Identifier/CaloTTOnOffIdMapAtlas <tag>CALOIdentifierCaloTTOnOffIdMapAtlas-0001</tag>"]
conddb.iovdbsvc.Folders += ["<db>COOLONL_CALO/OFLP200</db> /CALO/Identifier/CaloTTPpmRxIdMapAtlas <tag>CALOIdentifierCaloTTPpmRxIdMapAtlas-0000</tag>"]
conddb.blockFolder("/CALO/CaloSwClusterCorrections/rfac")
conddb.blockFolder("/CALO/CaloSwClusterCorrections/etaoff")
conddb.blockFolder("/CALO/CaloSwClusterCorrections/phioff")
conddb.blockFolder("/CALO/CaloSwClusterCorrections/gap")
conddb.blockFolder("/CALO/CaloSwClusterCorrections/calhits")
conddb.blockFolder("/CALO/CaloSwClusterCorrections/trcorr")
conddb.blockFolder("/CALO/CaloSwClusterCorrections/etamod")
conddb.blockFolder("/CALO/CaloSwClusterCorrections/phimod")
if not doCaloTopoCluster or doSim:   
   conddb.blockFolder("/CALO/EMTopoClusterCorrections/topophioff")
if doSim:
   conddb.blockFolder("/LAR/BadChannels/MissingFEBs")
conddb.setGlobalTag("OFLCOND-CSC-00-00-00")

#---------------------------
# Chrono svc
#---------------------------
svcMgr.AuditorSvc.Auditors = [ "ChronoAuditor" ]

# # Dump all objects available
StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.Dump = True

if not doSim:
   ToolSvc.LArADC2MeVToolDefault.UseHVScaleCorr = False
ToolSvc.TileCellBuilder.maskBadChannels = False
GeoModelSvc.DetectorTools[ "LArDetectorToolNV" ].ApplyAlignments = True
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
# Output level & events
MessageSvc.OutputLevel      = OutputLevel
#IOVDbSvc.OutputLevel      = 1
#IOVSvc.OutputLevel      = 1
#svcMgr.ClassIDSvc.OutputLevel = 2
theApp.EvtMax = EvtMax
#EventSelector.SkipEvents = 160;
AthenaEventLoopMgr = Service("AthenaEventLoopMgr")
AthenaEventLoopMgr.FailureMode=2
#AthenaEventLoopMgr._handle.FailureMode = 2
#ToolSvc.EventData2XML.OutputLevel =1
MessageSvc.defaultLimit=1000000;
theApp.AuditAlgorithms=True
#theApp.AuditServices=True
#theApp.AuditTools=True # FIXME crash on finalisation 
MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"
