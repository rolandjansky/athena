################################################################
#
#  jopOptions file for reading test beam ByteStream 
#
################################################################

include = Include( 1 )  # trace all includes

#MessageSvc.useColors   = FALSE
MessageSvc.OutputLevel  = DEBUG
#ROBDataProviderSvc.stripModuleType = TRUE
ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )
ByteStreamAddressProviderSvc.TypeNames += [
    "RpcPadContainer/RPCPAD",
    "MdtCsmContainer/MDTCSM",
    "ROIB::RoIBResult/RoIBResult",
    "L2Result/L2Result"
    ]

# Specify the Converters
#-----------------------
#ByteStreamCnvSvc.InitCnvs +=  [ "MdtCsm" ]
#ByteStreamCnvSvc.InitCnvs += [ "RpcPad" ]

ToolSvc = Service ( "ToolSvc" )


#Define the Muon Layout on where run
#MuonLayout = "Q02_initial"   # set the Muon Layout Type
                         # Available types:
			 # P03-DC1, P03-DC2, Q02_initial, Q02, CTB2004

DetDescrVersion = "Rome-Initial"  # set the Muon Layout Type
                         # Available types:
			 # DC1, DC2, Rome-Initial
			 
# setup the L2 environment and the Data Monitor algor
#----------------------------------------------------
include ( "TrigmuFast/jobOptions_MTmuFast.py" )

# OutputLevels
#----------------------------------------------------
L2Result.OutputLevel = DEBUG
ResultBuilder_L2.OutputLevel = DEBUG
muFast.OutputLevel = DEBUG
muFast.EvtToShow = 10
MessageSvc.OutputLevel  = DEBUG
StoreGate.OutputLevel = DEBUG
StoreGate.Dump = True

# dump properties
#----------------
#print " ----> DataMonitor.properties() ",DataMonitor.properties()
#print " ----> FakeLvl1ConversionTestBeam_L2.properties() ",FakeLvl1ConversionTestBeam_L2.properties()
