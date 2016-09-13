# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import ServiceMgr

from AthenaCommon.AppMgr import theApp


#--------------------------------------------------------------
# Load Geometry
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = "ATLAS-R1-2010-02-00-00"
globalflags.DetGeo="atlas"
globalflags.InputFormat="pool"
globalflags.DataSource="geant4"
print globalflags



import AtlasGeoModel.SetGeometryVersion
import AtlasGeoModel.GeoModelInit


#--------------------------------------------------------------
# Load ReadCalibData Alg and Service
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from InDetPhysValMonitoring.InDetPhysValMonitoringConf import HistogramDefinitionSvc
ToolSvc = ServiceMgr.ToolSvc
ServiceMgr+=HistogramDefinitionSvc()
ServiceMgr.HistogramDefinitionSvc.DefinitionSource="../share/testHDef.xml"
ServiceMgr.HistogramDefinitionSvc.DefinitionFormat="text/xml"

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
monMan = AthenaMonManager( "PhysValMonManager" )
monMan.ManualDataTypeSetup = True
monMan.DataType            = "monteCarlo"
monMan.Environment         = "altprod"
monMan.ManualRunLBSetup    = True
monMan.Run                 = 1
monMan.LumiBlock           = 1
monMan.FileKey = "TestPlots"
topSequence += monMan



from InDetPhysValMonitoring.InDetPhysValMonitoringConf import ToolTestMonitoringPlots
tool1 = ToolTestMonitoringPlots()

ToolSvc += tool1
monMan.AthenaMonTools += [tool1]


from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
svcMgr.THistSvc.Output += ["TestPlots DATAFILE='TestPlots.root' OPT='RECREATE'"]


#--------------------------------------------------------------
# Event selector settings. Use McEventSelector
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob

theApp.EvtMax                    = 1


#--------------------------------------------------------------
# Set output lvl (VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.HistogramDefinitionSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.infoLimit        = 100000

#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
#IOVDbSvc.GlobalTag='OFLCOND-RUN12-SDR-25'
#IOVDbSvc.OutputLevel = DEBUG

