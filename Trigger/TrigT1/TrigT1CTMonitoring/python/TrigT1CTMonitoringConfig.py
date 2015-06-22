# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp
## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
#---------------------------------------------------------------
# CTP / MuCTPI bytestream conversion
#---------------------------------------------------------------
from TrigT1ResultByteStream.TrigT1ResultByteStreamConf import MuCTPIByteStreamTool,RecMuCTPIByteStreamTool
from TrigT1ResultByteStream.TrigT1ResultByteStreamConf import CTPByteStreamTool,RecCTPByteStreamTool

if not hasattr( svcMgr, "THistSvc" ):
  from GaudiSvc.GaudiSvcConf import THistSvc
  svcMgr += THistSvc()
#svcMgr.THistSvc.Output = ["file1 DATAFILE='HistFile.root' OPT='RECREATE'"];

if not hasattr( theApp.Dlls, "RootHistCnv" ):
  theApp.Dlls += [ "RootHistCnv" ]
  theApp.HistogramPersistency = "ROOT"

if not hasattr( svcMgr, "THistSvc" ):
  from GaudiSvc.GaudiSvcConf import THistSvc
  svcMgr += THistSvc()
#svcMgr.THistSvc.Output = ["file1 DATAFILE='HistFile.root' OPT='RECREATE'"];

if not hasattr( theApp.Dlls, "RootHistCnv" ):
  theApp.Dlls += [ "RootHistCnv" ]
  theApp.HistogramPersistency = "ROOT"

# if not hasattr( topSequence, "TrigT1CTMonitoring__BSMonitoring" ):
#   from TrigT1CTMonitoring.TrigT1CTMonitoringConf import TrigT1CTMonitoring__BSMonitoring
#   CTMonAlg = TrigT1CTMonitoring__BSMonitoring()
#   topSequence += CTMonAlg
