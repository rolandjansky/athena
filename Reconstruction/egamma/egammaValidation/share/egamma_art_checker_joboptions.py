#
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
#

from AthenaCommon.AppMgr                    import theApp
from AthenaCommon.AppMgr                    import ServiceMgr as svcMgr
from AthenaCommon.AlgSequence               import AlgSequence
from GaudiSvc.GaudiSvcConf                  import THistSvc
import AthenaRootComps.ReadAthenaxAODHybrid


# ART File
testFile = 'ART_Rec_'+particleType+'.pool.root'
svcMgr.EventSelector.InputCollections = [testFile]


job = CfgMgr.AthSequencer("AthAlgSeq")
job += CfgMgr.EgammaMonitoring('MonitoringAlg', sampleType = particleType)


theApp.EvtMax = 2000


outputFile = 'ART-monitoring_'+particleType+'.root'
svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output = ["MONITORING DATAFILE='"+outputFile+"' OPT='RECREATE'"]


print ( "\n\nALL OK\n\n" )
