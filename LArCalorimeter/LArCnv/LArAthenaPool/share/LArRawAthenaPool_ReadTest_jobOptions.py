#**************************************************************
#
# jopOptions file in LArAthenaPool package
#       Read in ByteStream events, and Access LArRawChannel
# 
#==============================================================
#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

# Read LArRawChannel 
theApp.Dlls += [ "LArRawUtils", "LArROD" ]
theApp.Dlls += [ "LArByteStream" ]
# Algs
theApp.TopAlg += [ "ReadLArRaw" ]
ReadLArRaw = Algorithm( "ReadLArRaw" )
ReadLArRaw.LArRawChannelContainerName = "LArRawChannels" 
ReadLArRaw.HistogramID = 168 
ReadLArRaw.NtupleLocID = "/FILE1/LARG" 
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )

include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_joboptions.py" )

# ToolSvc.LArROD_Decoder.Print=true; 
# Define input
theApp.Dlls   += [ "LArAthenaPoolPoolCnv" ]
EventSelector = Service( "EventSelector" )
EventSelector.InputCollection        = "LArRawTest.root"
PoolSvc = Service( "PoolSvc" )
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = 2
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 200
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
#---------------------------------------------------------------
# Ntuple service output
#---------------------------------------------------------------
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output    = [ "FILE1 DATAFILE='larrod_read_ntup.root' OPT='NEW'" ]
theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
