#--------------------------------------------------------------
# jobOptions for writing RDO to POOL
#--------------------------------------------------------------
# Load POOL support
include( "AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py" )

# Define the output Db parameters (the default value are shown)
Stream1.OutputFile = "MuonPool.root"

# Converters:
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
include( "MuonEventAthenaPool/MuonEventAthenaPool_joboptions.py" )

# list of output objects key
# EventInfo
Stream1 = Algorithm( "Stream1" )
Stream1.ItemList+=["EventInfo#*"]
# CSC
Stream1.ItemList+=["CscDigitContainer#*"]
# MDT
Stream1.ItemList+=["MdtDigitContainer#*"]
# RPC
Stream1.ItemList+=["RpcDigitContainer#*"]
# TGC
Stream1.ItemList+=["TgcDigitContainer#*"]
