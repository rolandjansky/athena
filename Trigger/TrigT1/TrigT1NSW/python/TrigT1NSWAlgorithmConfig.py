# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp
## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr


if not hasattr( svcMgr, "THistSvc" ):
  from GaudiSvc.GaudiSvcConf import THistSvc
  svcMgr += THistSvc()
#svcMgr.THistSvc.Output = ["file1 DATAFILE='HistFile.root' OPT='RECREATE'"];

if not hasattr( theApp.Dlls, "RootHistCnv" ):
  theApp.Dlls += [ "RootHistCnv" ]
  theApp.HistogramPersistency = "ROOT"

if not hasattr( svcMgr, "NTupleSvc" ):
  from GaudiSvc.GaudiSvcConf import NTupleSvc
  svcMgr += NTupleSvc()
#svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='NtupleFile.root' OPT='NEW'" ]

#print topSequence
