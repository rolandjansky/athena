#!/usr/bin/env physh

from optparse import OptionParser

parser = OptionParser(usage="usage: %prog lib1 [lib2...]", version="%prog v0.1 $Id: LArG4ShowerLibSvcTest.py 489535 2012-03-20 01:19:04Z gsedov $")

(options, args) = parser.parse_args()

args.pop(0)

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
