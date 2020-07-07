#**************************************************************
#
# jopOptions file for TileCal testbeam analysis
#
#==============================================================
#-----------------------
# ByteStream Input 
#-----------------------
include( "ByteStreamCnvSvc/TBEventSelector_jobOptions.py" )

include( "ByteStreamCnvSvcBase/BSAddProvSvc_RDO_jobOptions.py" )

theApp.ExtSvc += [ "ByteStreamCnvSvc" ] 

ByteStreamInputSvc = Service( "ByteStreamInputSvc" )

# After using ~tilebeam/scripts/get_run <run_number> script
# we have this link in current directory
#
ByteStreamInputSvc.InputDirectory += ["." ]
ByteStreamInputSvc.FilePrefix     += ["daq_SFI-51_Tile"] 
ByteStreamInputSvc.RunNumber      += [ 0000000 ] 

#--------------------------------
# TileCal testbeam reconstruction
#--------------------------------
# special options
# which algorithms to run 
# and output from which algorithm to use as input for TileCellBuilder

doTileFlat = True
doTileFit = True

TileRunType = 8
TileCalibMode = True
TileFrameLength = 9
TileCalibrateEnergy = False

# set defaults, init DetDescr and load conditions data
DetDescrVersion = "CTB"
include( "TileGeoModel/TileGeoModel_jobOptions.py" )
include( "TileTBRec/TileTBDefaults_jobOptions.py" )
include( "TileConditions/TileTBConditions_jobOptions.py" )

# read ByteStream and reconstruct data
include( "TileTBRec/TileTBRec_jobOptions.py" )

# frag to ROD mapping in 2004
include( "TileTBRec/TileTBMap2004_jobOptions.py" )

# create TileCal standalone ntuple
include( "TileTBRec/TileTBNtuple_jobOptions.py" )

#-----------------------
# And some final options
#-----------------------

# Ntuple service output
#
# theApp.Dlls += [ "HbookCnv" ];  # not needed, already loaded
# theApp.HistogramPersistency="HBOOK"
# HbookHistSvc = Service( "HbookHistSvc" )
# HbookHistSvc.NPAWC = 1500000 
# HbookHistSvc.RecordLength = 1024
# HbookHistSvc.IQUEST10 = 65000 
# HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
# HistogramPersistencySvc.OutputFile  = "histo.hbook"
# NTupleSvc = Service( "NTupleSvc" )
# NTupleSvc.Output = [ "FILE1 DATAFILE='tiletb.ntup' OPT='NEW'" ]
# it is not possible to set record length for file with ntuple
# so, the size of the hbook ntuple will be limited to 32000 * 4 * 1024 = 128 MB
#
# TileTBNtuple.NTupleID = 1000

# Root Ntuple output (uncomment these lines, comment previous Hbook lines)
#
theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
HistogramPersistencySvc.OutputFile  = "histo.root"
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE1 DATAFILE='tiletb.root' OPT='NEW'" ]

theApp.Dlls += [ "GaudiAud" ] 
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors =  [ "ChronoAuditor" ] 

theApp.serviceMgr().EventSelector.MaxBadEvents = 10000
# ByteStreamInputSvc.DumpFlag = TRUE
# TileROD_Decoder = Algorithm( "TileROD_Decoder" )
# TileROD_Decoder.VerboseOutput = TRUE

TileTBNtuple.drawerList = [ -1 ]

# don't use cabling for channel->pmt conversion
TileTBNtuple.drawerType = [ 0, 0, 0, 0, 0, 0 ]

# keep only fragment with CIS parameters
TileTBNtuple.beamFragList = ["0x0ff"]

# don't store some extra stuff
TileTBNtuple.CompleteNtuple = FALSE

MessageSvc = Service( "MessageSvc" )
MessageSvc.defaultLimit=1000000
MessageSvc.OutputLevel = 4
theApp.EvtMax = 500000

