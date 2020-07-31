#======================================================================
# TOP Job options file for Combined TB2004 reconstruction 
# of real or simulated data
#======================================================================

# -------------------------------
# Set global flags
# -------------------------------
#RunNumber = 2100963
#RunNumber = 2100100
RunNumber = 2102103
#RunNumber = 2102549
#RunNumber = 2102728
#RunNumber = 2000362
#RunNumber = 2100103
#RunNumber = 2100180
FilePrefix = "daq_SFI-51_combined"
InputDirectory = "/castor/cern.ch/atlas/testbeam/combined/2004"
#InputDirectory = "/afs/cern.ch/user/i/idctb/scratch0/Data/TB04"
# Set the FilePrefix from the RunNumber
include("RecExTB/RecExTB_SetFlags_jobOptions.py")

#-------------------------------------------------------------
# For Monte Carlo only:
# if doSim=false, the ntuple will be filled with the right beam information
# anyway; if doSim=true, the ntuple will be filled with the figures below;
# Variables in the ntuple are: RunEta, RunEnergy, RunPartType
#----------------------------------------------------------
BeamEnergy = 180
BeamType = 3
#----------------------------------------------------------

doSim=False
doInDet=False
doMuons=False
doTile=True
doLAr=True
doBeamDetectors=False
doEgamma=False
doCaloMuId=False
doTrackToCalo=False
doTrigger = False
doCaloCalibHit = False
doWriteESD=False
doJiveXML = False
AtlantisGeometry = False

# TrackToCalo flags
# default for TestBeam up to now :
doStraightToCalo=False

# --------------------------------------------
# For simulation, do not run BeamDetectors
# --------------------------------------------
if doSim:
    doBeamDetectors = False

# --------------------------------------------
# Cluster types in Calorimeters:
# --------------------------------------------
#Combined Sliding Window
doCaloCluster=False

#Topological Clustering on LAr and Tile
doCaloTopoCluster=True;

#LAr SW and LAr 3x3 cluster
if doLAr:
    doLArOFC          = True # if 'False' use Parabola reconstruction
    doLArTimingNtuple = True    
    doEmCluster=True
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
doDetailedNtuple=False
if doTile:
    doTileNtuple=True
else:
    doTileNtuple=False

# ------------------------------------
# import GlobalFlags from RecExCommon
# ------------------------------------
from AthenaCommon.GlobalFlags import GlobalFlags

# GlobalFlags : Author Tadashi Maeno, David Rousseau
# This is class for main jobs configuration flags
#
# Category : DetGeo has states : atlas ctbh6 ctbh8    (ctbh6 and ctbh8 grouped in ctb)
# Category : DataSource has states : data geant3 geant4
# Category : InputFormat has states : zebra pool bytestream
#
#Each category can be set once and only once by the main topOptions.
# states are mutually exclusive
#
# from RecExCommon.GlobalFlags import GlobalFlags
# GlobalFlags.DetGeo.set_atlas()  # set DetGeo to be atlas
#  (trying to set again the same category will throw an error)
#
# state can be interrogated as follows (by construction, one category
#   is only in one state. Testing a category that has not been set
#          throws an error):
# if GlobalFlags.DetGeo.is_atlas():
#
#Groupings canbe used to interrogate (no set methods available):
# if GlobalFlags.DetGeo.is_ctb():  (or of is_ctbh6() and is_ctbh8())

GlobalFlags.DetGeo.set_ctbh8()
# zero luminosity (obviously...)
GlobalFlags.Luminosity.set_zero()

if doSim:
    # Reading G4 simulated data from Pool
    GlobalFlags.DataSource.set_geant4()
    GlobalFlags.InputFormat.set_pool()
else:
    # Reading ByteStream real data
    GlobalFlags.DataSource.set_data()
    GlobalFlags.InputFormat.set_bytestream()    


#-----------------------------------------
# Set internal flags for each subdetector
# ----------------------------------------

# InDet setup flags with default values
include( "InDetTBRecExample/InDetTBFlags_jobOptions.py" )

# Change InDet flags
if doInDet or doTrigger:
    InDetTBFlags.doSim = doSim
    InDetTBFlags.RunNumber = RunNumber
    InDetTBFlags.Pixels=True
    InDetTBFlags.SCT=True
    InDetTBFlags.TRT=True
    InDetTBFlags.Magnet=False
    InDetTBFlags.Current = -850.
    InDetTBFlags.ConstBfield = True
    InDetTBFlags.OracleTag = "InnerDetector-CTB-04"
    InDetTBFlags.SelectRDO=False
    if doSim:
        InDetTBFlags.doTruth=True
    else:
        InDetTBFlags.doTruth=False
    InDetTBFlags.TRT_LUT=False    
    InDetTBFlags.xKalman=False
    InDetTBFlags.iPatRec=False
    InDetTBFlags.SiTBLineFitter=True
    InDetTBFlags.Refit=False
    InDetTBFlags.ParticleCreation=True
    InDetTBFlags.CBNT=True
    InDetTBFlags.makeAlignNtp = True
    InDetTBFlags.JiveXML=False
    InDetTBFlags.AtlantisGeometry=False
    # hotfix for iPatRec flags using private RecExcommon settings
    if InDetTBFlags.iPatRec:
        doTruth = InDetTBFlags.doTruth
    # --- The selection of RDOs only works for real data for the moment
    if InDetTBFlags.doSim:
        InDetTBFlags.SelectRDO = False;
    
    InDetTBFlags.doTrackSegments = False
    if InDetTBFlags.doTrackSegments:
        InDetTBFlags.TRT_LUT=False
        InDetTBFlags.xKalman=False
        InDetTBFlags.iPatRec=False
        InDetTBFlags.IDSCAN=False
        InDetTBFlags.Refit = False

    if not InDetTBFlags.doSim:
        # Set magnet, Current and OracleTag flags
        include("InDetTBRecExample/InDetTBSetFlags_jobOptions.py")
        
    # Compute magnetic field from the current
    if InDetTBFlags.Magnet:
        Bfield = (-1)*InDetTBFlags.Current*(1.4)/850.
        if InDetTBFlags.ConstBfield:
            ScaledFactor = Bfield/1.4
        else:
            ScaledFactor = -850.*Bfield/(800.*1.4)        
        InDetTBFlags.xmin = -1000.
        InDetTBFlags.xmax = 1000.
       
# Muons setup flags with default values
include( "MuonTBRecExample/MuonTBFlags_jobOptions.py" )

# change Muon flags
if doMuons:
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
include( "LArTBRec/LArTBFlags_jobOptions.py" )

# Combined calorimeters reconstruction setup flags with default values
include( "CaloTBRec/CaloTBFlags_jobOptions.py" )
if doLAr and doSim:
    LArCondCnvDbServer = "atlasdev1.cern.ch"

# ----------------------------------
# import DetFlags from AthenaCommon
# ----------------------------------
from AthenaCommon.DetFlags import DetFlags

# DetFlags : Author Tadashi Maeno
#
# detectors : ID = pixel SCT TRT
#             LAr = em HEC FCal 
#             Calo = em HEC FCal Tile
#             Muon = MDT CSC TGC RPC 
# tasks:
#   geometry : setup the geometry
#   digitize : hit -> RDO (raw data objects)
#   makeRIO  : RDO -> RIO (Reconstruction Input Objects)
#   writeBS  : write RDO byte stream
#   readRDOBS : read RDO from byte stream
#   readRDOPool : read RDO from pool
#   readRIOBS : read RIO directly from BS
#   writeRDOPool : write RDO in pool
#
# subdetectors and tasks from a matrix of flags which are all
#    off by default
# changing the flags:
# tasks can be switched on/off for all detectors:
#    DetFlags.detdescr.all_setOn() (or all_setOff()
# a task for a given subdetector can be switched on/off
#    DetFlags.readRDOPool.Pixel_setOn() (or setOff() )
# setting off/on a group of subdetectors (e.g. LAr or Muon) switch off/on
#     all the corresponding subdetectors
# a subdetector can be switched on/off for all tasks:
#       DetFlags.Muon_setOn() (or setOff() )
#
# Flags can be tested individually:
#if DetFlags.detdescr.pixel_on():
#    print "pix Geo On"
#else:
#    print "pix Geo Off"    
# there is no off() method, just use not
# Group of subdetectors can also be tested:
#if DetFlags.detdescr.any_on() : true if any subdet is true
#if DetFlags.detdescr.allOn() : true if all subdet is true
#if DetFlags.detdescr.LAr_on() : true if any LAr det is true
#if DetFlags.detdescr.LAr_allOn() : true if all LAr det is true

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
if GlobalFlags.InputFormat.is_bytestream():
    include( "ByteStreamCnvSvc/TBEventSelector_jobOptions.py" )
    include( "ByteStreamCnvSvcBase/BSAddProvSvc_RIO_jobOptions.py" )
    include( "ByteStreamCnvSvcBase/BSAddProvSvc_RDO_jobOptions.py" )

    # Specify input file
    ByteStreamInputSvc = Service( "ByteStreamInputSvc" )
    
    ByteStreamInputSvc.InputDirectory = [ InputDirectory ]
    ByteStreamInputSvc.FilePrefix     = [ FilePrefix ]
    ByteStreamInputSvc.RunNumber      = [ RunNumber ]
    
    #First file to process
    #ByteStreamInputSvc.FirstFile      += [ 2 ]
    # Number of files to process
    #ByteStreamInputSvc.NumFile      += [ 1 ]
    # Number of events to skip at the beginning of the job
    #EventSelector.SkipEvents      = 50000
    # Reader type "Castor" or "Simple" 
    ByteStreamEventStorageInputSvc = Service( "ByteStreamEventStorageInputSvc" )
    ByteStreamEventStorageInputSvc.ReaderType = "Castor";

# ---------------------
# setup GeoModel
# ---------------------

include ("AtlasGeoModel/SetGeometryVersion.py")
include ("TileTBRec/TileTBDefaults_jobOptions.py")
include ("AtlasGeoModel/GeoModelInit.py")

#------------------------
# Read from POOL
#------------------------
if GlobalFlags.InputFormat.is_pool():
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
    EventSelector = Service( "EventSelector" )

    EventSelector.InputCollections = ["rfio:/castor/cern.ch/atlas/ctb/test/monte_carlo/digitization/photon/ctb.2857.G4Ctb_DigitProd.photon_eta_0.540_Mag_1.4.1040.v1.00001.root"]


# ------------------------------------------------------------
# Specify IOVDbSvc and access conditions database
#-------------------------------------------------------------
include( "RecExTB/RecExTB_CondDB.py" )

#-------------------------------------------------------------
# Detector description, conditions database and BS converters
#-------------------------------------------------------------

# Calo
if doLAr or doTile:
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
        include("LArCondCnv/LArCondCnv_G4_jobOptions.py")

    
if doTile:
    if not doSim :
        # read ByteStream and run RawChannelBuilder
        include( "TileTBRec/TileTBRec_jobOptions.py" )

# Inner Detector
if doInDet or doTrigger:
    # Detector description
    include ("InDetTBRecExample/InDetTB2004DetDescr_jobOptions.py")
        
    # BS converters 
    if not doSim:
        include( "InDetTBRecExample/InDetTBBSCnv_jobOptions.py" )
    
# Muons
if doMuons:
    # Initialize MuonGeoModel
    GeoModelSvc.MuonDetectorTool.Id_helpers_from_converters =0
    # Initialize bytestream converters 
    include( "MuonTestBeamCnv/jobOptions_MuonTestBeam.py" )
    if (RunNumber > 2101500):
        RPCcablingSvc.ConfFileName = "TB_25ns_LVL1conf.data"
        RPCcablingSvc.CorrFileName = "TB_25ns_LVL1conf.corr"
    # Load calibrations
    include( "MuonTBRecExample/MuonTBLoadCal_jobOptions.py" )
    MuonTBCalibrationSvc = Service ("MuonTBCalibrationSvc")
    MuonTBCalibrationSvc.T0_InputFiles = ["/afs/cern.ch/atlas/testbeam/muontbh8/scratch05/calibration_data_2004/MdtCalibRomData/forCTBprod_march05/t0_BIL1_40_2102541_2102734.dat.new", "/afs/cern.ch/atlas/testbeam/muontbh8/scratch05/calibration_data_2004/MdtCalibRomData/forCTBprod_march05/t0_BIL2_40_2102541_2102734.dat.new", "/afs/cern.ch/atlas/testbeam/muontbh8/scratch05/calibration_data_2004/MdtCalibRomData/forCTBprod_march05/t0_BILr_40_2102541_2102734.dat.new", "/afs/cern.ch/atlas/testbeam/muontbh8/scratch05/calibration_data_2004/MdtCalibRomData/forCTBprod_march05/t0_BML1_40_2102541_2102734.dat.new", "/afs/cern.ch/atlas/testbeam/muontbh8/scratch05/calibration_data_2004/MdtCalibRomData/forCTBprod_march05/t0_BML2_40_2102541_2102734.dat.new", "/afs/cern.ch/atlas/testbeam/muontbh8/scratch05/calibration_data_2004/MdtCalibRomData/forCTBprod_march05/t0_BOL1_40_2102552_2102615.dat.new", "/afs/cern.ch/atlas/testbeam/muontbh8/scratch05/calibration_data_2004/MdtCalibRomData/forCTBprod_march05/t0_BOL2_40_2102541_2102734.dat.new", "/afs/cern.ch/atlas/testbeam/muontbh8/scratch05/calibration_data_2004/MdtCalibRomData/forCTBprod_march05/t0_EIL1_40_2102541_2102734.dat.new", "/afs/cern.ch/atlas/testbeam/muontbh8/scratch05/calibration_data_2004/MdtCalibRomData/forCTBprod_march05/t0_EIS1_40_2102541_2102734.dat.new", "/afs/cern.ch/atlas/testbeam/muontbh8/scratch05/calibration_data_2004/MdtCalibRomData/forCTBprod_march05/newt0_EMS_EML_40.dat", "/afs/cern.ch/atlas/testbeam/muontbh8/scratch05/calibration_data_2004/MdtCalibRomData/forCTBprod_march05/newt0_EOS_EOL_40.dat"]
    
    # Calib files with RT relations
    MuonTBCalibrationSvc.RT_InputFiles = ["/afs/cern.ch/atlas/testbeam/muontbh8/scratch05/calibration_data_2004/MdtCalibRomData/forCTBprod_march05/rt_full_average_2690_2692.dat"]

# -------------------------------------------------------------------
# Configure further tools needed by Trigger, iPatRec, xKalman, ...
# -------------------------------------------------------------------
if InDetTBFlags.xKalman or InDetTBFlags.iPatRec or InDetTBFlags.Refit or InDetTBFlags.Magnet or MuonTBRecoAlgs.MOORE or MuonTBRecoAlgs.Muonboy or InDetTBFlags.SiTBLineFitter or InDetTBFlags.doTrackSegments:
    # get field service
    include( "BFieldAth/BFieldAth_jobOptions.py" )
    MagFieldAthenaSvc.NameOfTheBfieldFile = \
      "mbps1-all-id-800-mbps2-muons-800.data" ;
    MagFieldAthenaSvc.itf = 2
    MagFieldAthenaSvc.H8dx1 = 47.5
    MagFieldAthenaSvc.H8dx3 = 47.5
    MagFieldAthenaSvc.NameOfTheSource="BYPYTH"
if InDetTBFlags.SiTBLineFitter or InDetTBFlags.xKalman or InDetTBFlags.iPatRec or InDetTBFlags.TRT_LUT:    
    # particle property service
    include( "PartPropSvc/PartPropSvc.py" )
if doTrigger:
    # prepare LUT for Region Selector
    include("InDetRegionSelector/InDetRegionSelectorTable_jobOptions.py" )
if InDetTBFlags.iPatRec and not doTrigger:
    # prepare LUT for Region Selector used in TRT_Rec
    include("InDetRegionSelector/TRT_RegionSelectorTable_jobOptions.py" )

# -----------------------------
# Reconstruction
# -----------------------------           
# Calorimeter reconstruction :
if doLAr or doTile:
    include( "CaloTBRec/CaloTBRec_H8_Cell_jobOptions.py" )
    include( "CaloTBRec/CaloTBRec_H8_Cluster_jobOptions.py" )
    #include( "CaloTBRec/CaloTBRec_H8_ClusterOnly_jobOptions.py" )

# InnerDetector
if doInDet:
    include( "InDetTBRecExample/InDetTBRec_jobOptions.py" )
else:
    if doTrigger:
        include( "InDetTBRecExample/InDetTBPRDSpacePoints_jobOptions.py" )

# Muons
if doMuons:
    include( "MuonTBRecExample/MuonTBRecoAlgs_jobOptions.py" )
    MboyConfig = Algorithm( "MboyConfig" )
    MboyConfig.LevelOfRecons              = 3
    MboyConfig.MinRCHAMI = 3100.
    MboyRec = Algorithm( "MboyRec" )
    MboyRec.ShortSummaryTrack = 0
    MboyRec.ShortSummarySegment = 0

# TrackToCalo
if doTrackToCalo and doInDet and doLAr:
    include( "TrackToCalo/TrackToCaloAlg_jobOptions.py" )

    # if TrackParticles are not built, use tracks : 
    if not InDetTBFlags.ParticleCreation:
        TrackToCaloAlg.TrackInputType = "Tracks"

    # for photon runs analysis, change default input :
    if InDetTBFlags.doTrackSegments and not InDetTBFlags.SiTBLineFitter and not InDetTBFlags.RtfTracking:
        TrackToCaloAlg.TrackInputType = "Tracks"
        TrackToCaloAlg.TrackName="ConvertedXKalmanTracks"

# egamma
if doEgamma and doLAr:
    include ( "egammaRec/egammaRec_H8_jobOptions.py")
    ToolSvc.emtrackmatch.newExtrapol        = True
    ToolSvc.emtrackmatch.useCandidateMatch  = False
    include ( "egammaRec/egammaSofteRec_H8_jobOptions.py")

# Calo muon id
# ( note that TileMuid is run as well, but fragment does CBNT as well
#  -> is below )
if doCaloMuId and doMuons and doLAr :
    include( "TrackToCalo/MuToCaloAlg_jobOptions.py" )

# Calorimeter calibration hits :
if doCaloCalibHit and doSim:
   include( "CaloSimEvent/CaloSimEventDict_joboptions.py" )
    
#------------------------------
# CBNT general
#-----------------------------

CBNT_Athena = Algorithm( "CBNT_Athena" )
# Fill the CBNT_Athena stuff
CBNT_Athena.Members += [ "CBNT_EventInfo" ]

include("TBRec/CBNT_TBInfo.py")
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
    if not doDetailedNtuple:
        include( "CaloTBRec/CaloTBRec_H8_ntuple_jobOptions.py" )
        if doTileNtuple:
            include( "TileTBRec/TileTBNtuple_jobOptions.py" )
	if doLArTimingNtuple:
           include ( "LArTBRec/LArTBRec_H8_TimingNtuple_jobOptions.py" )
    else:
        if doLAr:
            include( "LArTBRec/LArTBRec_H8_DetailedNtuple_jobOptions.py" ) 
        if doTile:
            include( "TileTBRec/TileTBNtuple_jobOptions.py" )

# Inner Detector
if doInDet:
    if InDetTBFlags.CBNT:
        include( "InDetTBRecExample/InDetTBNtuple_jobOptions.py" )
    
if doMuons:
    # Muons
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
if doEgamma and doLAr:
    include ( "egammaRec/egamma_CBNT_H8_jobOptions.py")

# calo mu id
if doCaloMuId and doTile:
    CBNT_Athena.Members += ["CBNT_TileMu"]
    include ( "TileMuId/TileMuId_jobOptions.py" )
if doCaloMuId and doMuons:
    include( "TrackToCalo/CBNT_MuToCalo_jobOptions.py" )
if doCaloMuId and doLAr:
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

# ROOT Ntuple output (uncomment these lines, comment the three previouslines)
theApp.Dlls += ["CBNT_Utils", "CBNT_Athena" ]

theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE1 DATAFILE='ntuple.root' OPT='NEW'" ]
CBNT_Athena = Algorithm( "CBNT_Athena" )
CBNT_Athena.NtupleLocID="/FILE1/TB/tree"
theApp.TopAlg += [ "CBNT_Athena" ]


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
        ToolSvc.EventData2XML.PixelSpacePoints         = "PixelSpacePoints"
        ToolSvc.EventData2XML.SCTSpacePoints           = "SCT_SpacePoints"
        ToolSvc.EventData2XML.TRTClusters              = "TRT_RIOs";   
        ToolSvc.EventData2XML.iPatTracks               = "iPatTracks" 
        ToolSvc.EventData2XML.xKalmanTracks            = "Tracks"
        ToolSvc.EventData2XML.DataTypes += ["SiSpacePoint","SiCluster","TRT",
                            "xKalmanTrack","iPatTrack"]       
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
    if not doSim:
        include ("AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py")
    else:
        # WriteAthenaPool is inconsistent with ReadAthenaPool
        # there should be a specific file for that
        theApp.OutStream += [ "Stream1" ]
        theApp.OutStreamType = "AthenaOutputStream"
        Stream1 = Algorithm( "Stream1" )
        Stream1.EvtConversionSvc = "AthenaPoolCnvSvc"
        Stream1.OutputFile = "reconstruction.root"

    # Define the output file name
    Stream1.OutputFile    = "athena.root"

    # list of output objects
    # Converters:
    include ( "EventAthenaPool/EventAthenaPool_joboptions.py")
    include ( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py")

    # Needed for TBEvent ESD writing
    include ( "TBEventAthenaPool/TBEventAthenaPool_joboptions.py")
    
    # for LAr or Tile since Calo Cell converter is there
    # this DLL contain CaloCompactCellTool
    theApp.Dlls += [ "CaloTools" ]
    include ( "LArAthenaPool/LArAthenaPool_joboptions.py")
    include( "InDetEventAthenaPool/InDetEventAthenaPool_joboptions.py" )
    include( "TrkEventAthenaPool/TrkEventAthenaPool_joboptions.py" )
    include ( "RecAthenaPool/RecAthenaPool_joboptions.py")
    include ("TileEventAthenaPool/TileEventAthenaPool_joboptions.py")
    include ("InDetEventCnvTools/InDetEventCnvTools_jobOptions.py")
    
    #  EventInfo
    try:
        fullItemList += ["EventInfo#*"]
    except:
        fullItemList = []
        fullItemList += ["EventInfo#*"]

    #HepMC record (be careful it has to be read by something to be written out)
    #Stream1.ItemList+=["McEventCollection#*"]
    include ( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPoolItemList_joboptions.py")
    #Calo/LAr
    # include ( "LArAthenaPool/LArAthenaPoolItemList_joboptions.py")
    # cannot use it directly :also has hit and rawchannel

    # Add class LArClusterContainer to the fullItemList list
    fullItemList += [ "LArClusterContainer#*" ]
    # Add class CaloCellContainer to the fullItemList list
    fullItemList += [ "CaloCellContainer#*" ]
    # Add class CaloClusterContainer to the fullItemList list
    fullItemList += [ "CaloClusterContainer#*" ]

    #full tracking
    fullItemList +=["TrackCollection#Tracks"]
    fullItemList +=["TrackCollection#ConvertedMBoyTracks"]
    fullItemList += [ "Rec::TrackParticleContainer#*" ]

    # Inner Detector
    fullItemList +=["InDet::SCT_ClusterContainer#*"]
    fullItemList +=["InDet::PixelClusterContainer#*"]
    fullItemList +=["InDet::TRT_DriftCircleContainer#*"]
   
    #as long as not in AOD
    # it cannot be yet with the actual muon reconstruction
    #fullItemList += ["MuonContainer#*"]

    #Reconstruction
    include ( "RecAthenaPool/RecAthenaPoolItemList_joboptions.py")

    #TestBeam
    fullItemList += [ "TBBPCCont#*" ]
    fullItemList += [ "TBEventInfo#*" ]
    fullItemList += [ "TBHitCont#*" ]
    fullItemList += [ "TBHitPlaneCont#*" ]
    fullItemList += [ "TBIdentifiedParticle#*" ]
    fullItemList += [ "TBLArCalibDigitContainer#*" ]
    fullItemList += [ "TBLArDigitContainer#*" ]
    fullItemList += [ "TBPhase#*" ]
    fullItemList += [ "TBScintillatorCont#*" ]
    fullItemList += [ "TBTDC#*" ]
    fullItemList += [ "TBTrack#*" ]
    fullItemList += [ "TBTriggerPatternUnit#*" ]


    Stream1.ItemList = fullItemList


    
#---------------------------
# Chrono svc
#---------------------------
theApp.Dlls += [ "GaudiAud" ] 
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors =  [ "ChronoAuditor" ]

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
# Output level & events
MessageSvc.OutputLevel      = 3
theApp.EvtMax = 5
#EventSelector.SkipEvents = 160;
AthenaEventLoopMgr = Service("AthenaEventLoopMgr")
AthenaEventLoopMgr.FailureMode=2
#ToolSvc.EventData2XML.OutputLevel =1
MessageSvc.defaultLimit=1000000;
theApp.AuditAlgorithms=True
#theApp.AuditServices=True
#theApp.AuditTools=True # FIXME crash on finalisation 
MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"
