#########################################################################
#                                                                       #
#        Standalone jobO for testing the HLT Config service             #
#                                                                       #


MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel  = INFO
MessageSvc.defaultLimit = 10000000

#from TrigConfigSvc.TrigConfigSvcConfig import HLTConfigSvc
from TrigConfigSvc.TrigConfigSvcConf import TrigConf__HLTConfigSvc
from AthenaCommon.AlgSequence import AlgSequence

job = AlgSequence()

#job += HLTConfigSvc( "HLTConfigSvc-Test" )
ServiceMgr += TrigConf__HLTConfigSvc( "HLTConfigSvc" )


#theApp.Dlls += [ "TrigConfigSvc" ]

#theApp.CreateSvc += [ "TrigConf::HLTConfigSvc/HLTConfigSvc" ]

#HLTConfigSvc = Service( "HLTConfigSvc" )
#HLTConfigSvc.ConfigSource        = "XML"
# HLTConfigSvc.DBServer = "devdb"
#HLTConfigSvc.XMLSignatureFile    = "siglistnew_newconfig.xml"
#HLTConfigSvc.XMLSequenceFile     = "seqlistnew_newconfig.xml"

#job.HLTConfigSvc.XMLMenuFile     = "hlt_menu.xml"
#HLTConfigSvc.OutputLevel         = VERBOSE

theApp.EvtMax = 1

#                                                                       #
#      End of standalone jobO for testing the HLTConfig service        #
#                                                                       #
#########################################################################
