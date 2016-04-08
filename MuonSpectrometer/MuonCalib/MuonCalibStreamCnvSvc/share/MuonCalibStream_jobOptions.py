#
# jobO fragment for the calib stream converter MuonCalibStreamCnvSvc
#
# to use it :
# switch off the standard bytestrem converter 
# include this file
# set the input file:
# 
# svcMgr.MuonCalibStreamFileInputSvc.InputFiles = [ "xxxxx.dat" ]
# 

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.Logging import *
 
# use a private Event Loop
theApp.EvtSel = "NONE"

# the services
theApp.ExtSvc += [ "MuonCalibStreamCnvSvc" ]
theApp.ExtSvc += [ "MuonCalibStreamFileInputSvc" ]
theApp.ExtSvc += [ "EventSelectorMuonCalibStream/EventSelector" ]
from MuonCalibStreamCnvSvc.MuonCalibStreamCnvSvcConf import EventSelectorMuonCalibStream
svcMgr+=EventSelectorMuonCalibStream("EventSelector")
# set the private event loop
theApp.EvtSel =  "EventSelector" 
# set the converter
#svcMgr.EventPersistencySvc = Service( "EventPersistencySvc" )
svcMgr.EventPersistencySvc.CnvServices      += [ "MuonCalibStreamCnvSvc" ]
# Load ProxyProviderSvc
from AthenaCommon.ConfigurableDb import getConfigurable
svcMgr += getConfigurable( "ProxyProviderSvc" )()
# Make "MuonCalibStreamAddressProviderSvc" an AddressProvider
svcMgr.ProxyProviderSvc.ProviderNames += [
  "MuonCalibStreamAddressProviderSvc" 
]
# configure it
svcMgr += getConfigurable( "MuonCalibStreamAddressProviderSvc" )()
#
svcMgr += getConfigurable("MuonCalibStreamFileInputSvc")()
#svcMgr.MuonCalibStreamFileInputSvc.OutputLevel=VERBOSE
#
svcMgr += getConfigurable("MuonCalibStreamCnvSvc")()
#svcMgr.MuonCalibStreamCnvSvc.OutputLevel=VERBOSE
#
svcMgr += getConfigurable( "MuonCalibStreamDataProviderSvc" )()

svcMgr += getConfigurable( "MuonCalibRunLumiBlockCoolSvc" )(OutputLevel=INFO)

svcMgr.MuonCalibStreamDataProviderSvc.LumiBlockNumberFromCool=True
svcMgr.MuonCalibStreamDataProviderSvc.RunNumberFromCool=True
