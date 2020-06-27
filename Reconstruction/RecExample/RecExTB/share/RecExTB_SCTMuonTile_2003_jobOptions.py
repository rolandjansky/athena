#=======================================================================
# jopOptions file for reading SCT+Tile+Muons test beam 2003 ByteStream    
#=======================================================================
#-------------------------------------------------------------------
# Input 
# ByteStreamCnvSvc options
#-------------------------------------------------------------------
include( "ByteStreamCnvSvc/TBEventSelector_jobOptions.py" )

include( "ByteStreamCnvSvcBase/BSAddProvSvc_RIO_jobOptions.py" )

include( "ByteStreamCnvSvcBase/BSAddProvSvc_RDO_jobOptions.py" )

# Specify input file
# Directory 
ByteStreamInputSvc = Service( "ByteStreamInputSvc" )
ByteStreamInputSvc.InputDirectory = ["/castor/cern.ch/atlas/testbeam/combined/2003"] ; 
# Prefix 
ByteStreamInputSvc.FilePrefix = ["daq_SFO-1_combined"] ; 
# Run number 
ByteStreamInputSvc.RunNumber = [1222] ; 
# Reader type "Castor" or "Simple" 
ByteStreamEventStorageInputSvc = Service( "ByteStreamEventStorageInputSvc" )
ByteStreamEventStorageInputSvc.ReaderType = "Castor";

theApp.serviceMgr().EventSelector.MaxBadEvents = 200

# This does not work for some unknown reason. That means that the SCT
# reconstruction needs to be commented (even if what we try to say here
# is to ignore the SCT ROS
ROBDataProviderSvc = Service( "ROBDataProviderSvc" )
ROBDataProviderSvc.ignoreROS = [ 155652 ]

#-------------------------------------------------------------------
# Detector description, calibrations and BS converters
#-------------------------------------------------------------------

# SCT 
include( "InDetTBRecExample/SCT_TB2003DetDescr_jobOptions.py" )
include( "InDetTBRecExample/SCT_TB2003BSCnv_jobOptions.py" )

# Muons
include( "MuonTestBeamCnv/jobOptions_MuonTestBeam.py" )

MuonTBMdtCablingSvc = Service( "MuonTBMdtCablingSvc" )
MuonTBMdtCablingSvc.InputFile = "H8MdtEleMap_combined.data"

# Tile

# init DetDescr and load conditions data
include( "TileTBRec/TileTBConditions_jobOptions.py" )
# read ByteStream and reconstruct data
include( "TileTBRec/TileTBRec_jobOptions.py" )


#-------------------------------------------------------------------
# Reconstruction and filling CBNT ntuple
#-------------------------------------------------------------------
# SCT
include( "InDetTBRecExample/SCT_TB2003Rec_jobOptions.py" )

# Muons
##include "MuonTestBeamCnv/jobOptions_MooreAlgs.txt"
CBNT_Athena = Algorithm( "CBNT_Athena" )
CBNT_Athena.Members += [ "CBNT_EventInfo" ]
CBNT_Athena.Members += [ "MdtDigitNtuple" ]
#CBNT_Athena.Members += { "RpcDigitNtuple" };
#CBNT_Athena.Members += { "MuonTBAliNtuple" };

# Tile

# create TileCal standalone ntuple
include( "TileTBRec/TileTBNtuple_jobOptions.py" )

# ROOT Ntuple output (uncomment these lines, comment the three previouslines)
theApp.Dlls += ["CBNT_Utils", "CBNT_Athena" ]
#theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE1 DATAFILE='ntuple.root' OPT='NEW'" ]
CBNT_Athena.NtupleLocID="/FILE1/MUTB/tree"
HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
HistogramPersistencySvc.OutputFile  = "tracks.root"
theApp.TopAlg += [ "CBNT_Athena" ]
# Chrono svc
theApp.Dlls += [ "GaudiAud" ] 
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors =  [ "ChronoAuditor" ] 
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
# Output level & events
MessageSvc.OutputLevel      = 5
theApp.EvtMax = 100
