# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

athenaCommonFlags.EvtMax=1
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 1

from AthenaCommon.AppMgr import ServiceMgr
from LArG4ShowerLibSvc.LArG4ShowerLibSvcConf import LArG4ShowerLibSvc
ServiceMgr += LArG4ShowerLibSvc()

ServiceMgr.LArG4ShowerLibSvc.FileNameList = args

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = DEBUG

from LArG4ShowerLibSvc.LArG4ShowerLibSvcConf import LArG4ShowerLibSvcTest
topSequence += LArG4ShowerLibSvcTest()
