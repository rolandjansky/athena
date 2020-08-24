
# options for writing Cosmic Calibration constants (T0 etc)
# into CondDB using the TRT_TestWrite algorithm

# The input is a text file with this format:
#
# bec >> layer  >> sector >>  straw_layer >> straw
# >> t0 >> nh >> d0 >> a0 >> a1 >> a2 >> a3
# 
# bec = +-1 or +-2, layer= 0,1,2, sector = 1..32 etc
# t0 is in ns
# nh is number of calibration hits (not used)
# d0 is individual wire shift (mm in Rphi - not used)
# and R= a0 + (t-t0)*(a1+(t-t0)*(a2+(t-t0)*a3))


include( "InDetCosmicRecExample/InDetCosmicFlags_jobOptions.py" )
# ------------------------------------
# import GlobalFlags from RecExCommon
# ------------------------------------
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()
if InDetCosmicFlags.doSim:
    # Reading G4 simulated data from Pool
    GlobalFlags.DataSource.set_geant4()
    GlobalFlags.InputFormat.set_pool()
else:
    # Reading ByteStream real data
    GlobalFlags.DataSource.set_data()
    GlobalFlags.InputFormat.set_bytestream()

# ----------------------------------
# import DetFlags from AthenaCommon
# ----------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.pixel_setOn()
DetFlags.SCT_setOn()
DetFlags.TRT_setOn()

DetFlags.detdescr.pixel_setOn()
DetFlags.detdescr.SCT_setOn()
DetFlags.detdescr.TRT_setOn()
DetFlags.readRDOBS.pixel_setOff()
DetFlags.readRDOBS.SCT_setOff()
DetFlags.readRDOBS.TRT_setOff()
DetFlags.makeRIO.pixel_setOff()
DetFlags.makeRIO.SCT_setOff()
DetFlags.makeRIO.TRT_setOff()
DetFlags.Print()
#
# Access to IOVDbSvc
include ( "IOVDbSvc/IOVDbSvc_jobOptions.py" )
#
# Need this for the TRT Geomodel manager
include ("AtlasGeoModel/SetGeometryVersion.py")
GeoModelSvc.AtlasVersion = "ATLAS-DC2-00"
GeoModelSvc.InDetVersionOverride="InnerDetector-Cosmic-01"
DetDescrCnvSvc.InDetIDFileName="IdDictParser/IdDictInnerDetector_Cosmic.xml"
include ("AtlasGeoModel/GeoModelInit.py")

#load relevant libraries
theApp.Dlls    += [ "TRT_ConditionsAlgs" ]
theApp.Dlls    += [ "InDetConditionsAthenaPoolPoolCnv" ]
theApp.Dlls    += [ "DetDescrCondAthenaPoolPoolCnv" ]
#top algorithm
theApp.TopAlg += [ "TRT_TestWrite" ]

#debug level
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = 3
MessageSvc.debugLimit       = 2000
MessageSvc.infoLimit        = 2000

#configure IOVRegistrationSvc
include( "RegistrationServices/IOVRegistrationSvc_jobOptions.py" )
regSvc = Service( "IOVRegistrationSvc" )
regSvc.RecreateFolders = False

#configure IOVDbSvc
IOVDbSvc = Service( "IOVDbSvc" )
IOVDbSvc.OutputLevel       = 2
#
# Connect to testdatabase
#IOVDbSvc.dbConnection = "impl=cool;techno=oracle;devdb10:TESTCOOL:ATLAS_COOLTEST:pwd4cool"
#
# to sqlite standalone db
IOVDbSvc.dbConnection = "impl=cool;techno=sqlite;schema=CosmicCalibration.db:DUMMY:COOLTEST"
#
# or to testbeam production db
#IOVDbSvc.dbConnection = "impl=conddb;techno=mysql;atlmysql01.cern.ch:conditions_ctb_2004:condwriter:xxxxxx"
#
# or to testbeam COOL.1.3 production db
#IOVDbSvc.dbConnection = "impl=cool;techno=oracle;schema=ATLAS_COOL_TRT;ATLAS_COOLPROD:COMPROD:ATLAS_COOL_TRT:xxxxxxx"


# or do not connect any output db

# Load POOL support
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py" )
PoolSvc = Service( "PoolSvc" )
PoolSvc.DbOpen = "update"
PoolSvc.WriteCatalog="file:CosmicCalibration.xml"
PoolSvc.FileOpen = "update"
#
#  Use AthenaOutputStreamTool to write
#    Must "turn off" standard AthenaOutputStream
theApp.OutStream = []
ToolSvc = Service( "ToolSvc" )
ToolSvc.CondStream1.OutputFile = "CosmicCalibration.root"

#
# Configure the database tool
# output stream
from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTAlignDbTool
MyTRTAlignDbTool = TRTAlignDbTool (name='MyAlignDbTool')

# make COOL folders
ToolSvc.TRT_CondDbTool.useCOOL = True
# no IOVCallback
ToolSvc.TRT_CondDbTool.readFromTextFiles = True
# no reading in the tool initialize - we do it in the alg
ToolSvc.TRT_CondDbTool.calibTextFile = ""

#
theApp.EvtMax = 5
#

# TRT_Testwrite options
TRT_TestWrite = Algorithm( "TRT_TestWrite" )

# create new FloatArrayStores
TRT_TestWrite.Create=TRUE

# write them to ConDB/file
TRT_TestWrite.Write=TRUE

# text files to read from and write to

TRT_TestWrite.CalibInputFile="CosmicCalibrationConstants.txt"
TRT_TestWrite.AlignInputFile=""
TRT_TestWrite.PoolOutputFile= "CosmicCalibration.root"
TRT_TestWrite.WriteCalibTextFile="condout.txt"
TRT_TestWrite.WriteAlignTextFile=""

# ##################################################
# choose whether to register the data with IOV
TRT_TestWrite.RegisterIOV=True
####################################################

# validity range
TRT_TestWrite.ValidRun1=0
TRT_TestWrite.ValidRun2=10000
TRT_TestWrite.ValidEvent1=0
TRT_TestWrite.ValidEvent2=9999999

# DB tag (will be concatenated with a contraction of foldername)
TRT_TestWrite.VersionTag="01"


