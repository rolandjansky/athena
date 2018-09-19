#
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
#

from AthenaCommon.AppMgr                    import theApp
from AthenaCommon.AppMgr                    import ServiceMgr as svcMgr
from AthenaCommon.AlgSequence               import AlgSequence
from GaudiSvc.GaudiSvcConf                  import THistSvc
import AthenaRootComps.ReadAthenaxAODHybrid


from MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier
MCClassifier = MCTruthClassifier( name                      = "MCTruthClassifier",
                                  ParticleCaloExtensionTool = "" )
ToolSvc += MCClassifier
# Ouput Message Level
svcMgr.MessageSvc.OutputLevel = INFO

# ART File
testFile = 'Nightly_AOD_'+particleType+'.pool.root'
svcMgr.EventSelector.InputCollections = [testFile]


job = CfgMgr.AthSequencer("AthAlgSeq")
job += CfgMgr.EgammaMonitoring('MonitoringAlg', sampleType = particleType)


theApp.EvtMax = 60000


outputFile = 'Nightly-monitoring_'+particleType+'.hist.root'
svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output = ["MONITORING DATAFILE='"+outputFile+"' OPT='RECREATE'"]


print ( "\n\nALL OK\n\n" )
