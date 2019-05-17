#
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
#

from AthenaCommon.AppMgr                    import theApp
from AthenaCommon.AppMgr                    import ServiceMgr as svcMgr
from AthenaCommon.AlgSequence               import AlgSequence
from GaudiSvc.GaudiSvcConf                  import THistSvc
import AthenaPoolCnvSvc.ReadAthenaPool

from MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier
MCClassifier = MCTruthClassifier( name                      = "MCTruthClassifier",
                                  ParticleCaloExtensionTool = "" )
ToolSvc += MCClassifier
# Ouput Message Level
svcMgr.MessageSvc.OutputLevel = INFO

Geometry="ATLAS-R2-2016-01-00-01"
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOn()
DetFlags.Forward_setOff()
DetFlags.ID_setOff()

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion=Geometry

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
include( "LArDetDescr/LArDetDescr_joboptions.py" )


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
