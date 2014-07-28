# use Combined testbeam geometry
DetDescrVersion = "CTB"

include ("AtlasGeoModel/SetGeometryVersion.py")

DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.IdDictFromRDB = True

# TagInfo related include
include( "EventInfoMgt/EventInfoMgt_jobOptions.py" )

# Load RDBAcessSvc (Access to parameter Database)
include ("RDBAccessSvc/RDBAccessSvcPdb_jobOptions.py")
    
# Load NOVA objects into the Detector store
# Still some NOVA dependence in test beam.
include( "NovaCnvSvc/NovaCnvSvc_jobOptions.py" )
include( "NovaConverters/NovaConverters_jobOptions.py" )

# Load GeoModelSvc
theApp.Dlls += [ "GeoModelSvc" ]
theApp.CreateSvc += [ "GeoModelSvc"]

# Load the detectors. These job option fragments look at DetFlags if it is
# defined. 

#include ("AtlasGeoModel/InDetGeoModelCTB.py")
include ("AtlasGeoModel/MuonGeoModelCTB.py")
#include ("AtlasGeoModel/TileGeoModelCTB.py")
#include ("AtlasGeoModel/LArGeoModel.py")

GeoModelSvc.MuonDetectorTool.IncludeCtbBis = 1


# *******************  OLD Muon Detector Description  ***********************
# Initialize old detector description
#include ( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )
#include ( "MuonDetMgrDetDescrCnv/MuonDetMgrDetDescrCnv_joboptions.py" )

#theApp.DLLs += [ "MuonDetMgrDetDescrCnv" ]
#DetDescrCnvSvc.DecodeIdDict = TRUE
#DetDescrCnvSvc.MuonIDFileName  = "IdDictMuonSpectrometer_H8_2004.xml"
#DetDescrCnvSvc.ReadFromROOT = TRUE
# *******************  OLD Muon Detector Description  ***********************



# Initialize Muon Test Beam Cabling and Calibration Services
theApp.DLLs   += [ "MuonTBConditions" ]
theApp.ExtSvc += [ "MuonTBMdtCablingSvc" ]
theApp.ExtSvc += [ "MuonTBCalibrationSvc" ]

#initialise the calibration Service if used
if ( muFastUseCalib ):
	MuonTBCalibrationSvc = Service( "MuonTBCalibrationSvc" )
	MuonTBCalibrationSvc.T0_InputFiles = [ 
        "/afs/cern.ch/atlas/testbeam/muontbh8/scratch05/calibration_data_new/tzero/t0_barrel_aug.dat",
        "/afs/cern.ch/atlas/testbeam/muontbh8/scratch05/calibration_data_new/tzero/t0_ec_hodo_1198_1217.dat"]

	MuonTBCalibrationSvc.RT_InputFiles = [
        "/afs/cern.ch/atlas/testbeam/muontbh8/scratch05/calibration_data_new/rt_rels/rt_030001_030018_bil11.dat",
        "/afs/cern.ch/atlas/testbeam/muontbh8/scratch05/calibration_data_new/rt_rels/rt_030001_030018_bil12.dat",
        "/afs/cern.ch/atlas/testbeam/muontbh8/scratch05/calibration_data_new/rt_rels/rt_030001_030018_bil21.dat",
        "/afs/cern.ch/atlas/testbeam/muontbh8/scratch05/calibration_data_new/rt_rels/rt_030001_030018_bil22.dat",
        "/afs/cern.ch/atlas/testbeam/muontbh8/scratch05/calibration_data_new/rt_rels/rt_030001_030018_bml31.dat",
        "/afs/cern.ch/atlas/testbeam/muontbh8/scratch05/calibration_data_new/rt_rels/rt_030001_030018_bml32.dat",
        "/afs/cern.ch/atlas/testbeam/muontbh8/scratch05/calibration_data_new/rt_rels/rt_030001_030018_bml41.dat",
        "/afs/cern.ch/atlas/testbeam/muontbh8/scratch05/calibration_data_new/rt_rels/rt_030001_030018_bml42.dat",
        "/afs/cern.ch/atlas/testbeam/muontbh8/scratch05/calibration_data_new/rt_rels/rt_030001_030018_bol51.dat",
        "/afs/cern.ch/atlas/testbeam/muontbh8/scratch05/calibration_data_new/rt_rels/rt_030001_030018_bol52.dat",
        "/afs/cern.ch/atlas/testbeam/muontbh8/scratch05/calibration_data_new/rt_rels/rt_030001_030018_bol61.dat",
        "/afs/cern.ch/atlas/testbeam/muontbh8/scratch05/calibration_data_new/rt_rels/rt_030001_030018_bol62.dat"
        ]	

if ( muFastUseCalib and (not Hodo) ):
	MuonTBCalibrationSvc.T0_shift = 60


MuonTBMdtCablingSvc = Service ( "MuonTBMdtCablingSvc" )
MuonTBRpcCablingSvc = Service ( "MuonTBRpcCablingSvc" )

# Default file name for the MDT cabling map
if ( CombinedRun ):
	MuonTBMdtCablingSvc.InputFile = "H8MdtEleMap_TB04_Combined.data"
else:
	MuonTBMdtCablingSvc.InputFile = "H8MdtEleMap_TB04.data"

theApp.ExtSvc += [ "MuonTBRpcCablingSvc" ]

# Default file name for the RPC cabling map
MuonTBRpcCablingSvc.InputFile = "H8RpcEleMap_TB04.data"


#//MuonTBMdtCablingSvc.ReadFromAscii = TRUE
MuonTBMdtCablingSvc.ReadFromConditions = FALSE

# Muon Test Beam libraries
theApp.DLLs += [ "MuonTestBeamCnv" ]


RpcROD_Decoder = iProperty( 'RpcROD_Decoder' )
RpcROD_Decoder.DecodeTDC = FALSE
#RpcROD_Decoder = Tool ( "RpcROD_Decoder" )

#RpcROD_Decoder.DecodeTDC = FALSE

