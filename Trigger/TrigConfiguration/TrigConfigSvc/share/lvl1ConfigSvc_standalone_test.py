#########################################################################
#                                                                       #
#        Standalone jobO for testing the LVL1Config service             #
#                                                                       #

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel  = INFO
MessageSvc.defaultLimit = 10000000

theApp.Dlls += [ "TrigConfigSvc" ]

theApp.CreateSvc += [ "TrigConf::LVL1ConfigSvc/LVL1ConfigSvc" ]
from TrigConfigSvc.TrigConfigSvcConfig import LVL1ConfigSvc
from AthenaCommon.AppMgr import ServiceMgr

theLVL1ConfigSvc = LVL1ConfigSvc('LVL1ConfigSvc')
ServiceMgr += theLVL1ConfigSvc

theLVL1ConfigSvc.ConfigSource = "XML"
#theLVL1ConfigSvc.DBServer = "devdb"
theLVL1ConfigSvc.XMLFile = "LVL1Config.xml"
theLVL1ConfigSvc.CreateLegacyObjects = TRUE
theLVL1ConfigSvc.DumpTTVmap = False
theLVL1ConfigSvc.OutputLevel = VERBOSE

#include( "TrigT1Muctpi/TrigT1Muctpi_jobOptions.py" )
#include( "TrigT1Calo/TrigT1CaloJobOptions.py" )
#include( "TrigT1TGC/TrigT1TGCJobOptions.py" )
#include( "TrigT1CTP/TrigT1CTP_jobOptions.py" )

theApp.EvtMax = 0

#                                                                       #
#      End of standalone jobO for testing the LVL1Config service        #
#                                                                       #
#########################################################################
