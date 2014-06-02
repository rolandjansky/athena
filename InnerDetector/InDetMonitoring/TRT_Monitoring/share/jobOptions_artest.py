OutputLevel     = INFO
doJiveXML       = False
doVP1           = False
doWriteESD      = False
doWriteAOD      = False
doReadBS        = True
doAuditors      = True

import os
if os.environ['CMTCONFIG'].endswith('-dbg'):
  doEdmMonitor    = True 
  doNameAuditor   = True
else:
  doEdmMonitor    = False
  doNameAuditor   = False
DetDescrVersion = "ATLAS-GEO-18-01-01"
import AthenaCommon.SystemOfUnits as Units
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'data'
if 'doReadBS' in dir() and doReadBS:
  globalflags.InputFormat   = 'bytestream'
  globalflags.ConditionsTag = 'COMCOND-BLKPST-005-01'
else:
  globalflags.InputFormat = 'pool'
globalflags.DetDescrVersion = DetDescrVersion
globalflags.print_JobProperties()
if len(globalflags.ConditionsTag())!=0:
   from IOVDbSvc.CondDB import conddb
   conddb.setGlobalTag(globalflags.ConditionsTag()) 
    
from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.energy             = 7.*Units.TeV
jobproperties.Beam.beamType           = 'collisions'
if 'doReadBS' in dir() and doReadBS:
  jobproperties.Beam.bunchSpacing       = 75
from RecExConfig.RecFlags import rec
rec.Commissioning=False

from AthenaCommon.DetFlags import DetFlags 
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

DetFlags.Print()
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth       = (globalflags.DataSource == 'geant4' and globalflags.InputFormat() == 'pool')
InDetFlags.doxKalman              = False
InDetFlags.doiPatRec              = True
InDetFlags.doMonitoringGlobal    = True
InDetFlags.doMonitoringPixel     = False
InDetFlags.doMonitoringSCT       = False
InDetFlags.doMonitoringTRT       = True
InDetFlags.doMonitoringAlignment = False
InDetFlags.doTrkNtuple      = True
InDetFlags.doPixelTrkNtuple = True
InDetFlags.doTrtTrkNtuple   = True
InDetFlags.doPrintConfigurables = True

from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.TRT_BuildStrawLayers = True

include("InDetRecExample/InDetRec_all.py")
from TRT_Monitoring.TRT_MonitoringConf import TRT_Monitoring_Tool
#TRT_Monitoring_Tool.m_useConditionsHTStatus = True
#TRT_Monitoring_Tool.m_useArgonStraws = True
TRT_Monitoring_Tool.doArgonXenonSeperation = True
TRT_Monitoring_Tool.useHoleFinder = False 
from IOVDbSvc.CondDB import conddb
conddb.addOverride("/TRT/Cond/StatusHT","TrtStrawStatusHT-ArTest-00-00")
theApp.EvtMax = 50
if not doReadBS:
  ServiceMgr.PoolSvc.AttemptCatalogPatch=True
  ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/maxidisk/d158/CSC.005200.T1_McAtNlo_Jimmy.RDO.pool.root" ]
if doReadBS:
#  ServiceMgr.ByteStreamInputSvc.FullFileName = [ "/afs/cern.ch/user/e/eyazici/rawdata/data12_8TeV.00205010.physics_ZeroBiasOverlay.merge.RAW/data12_8TeV.00205010.physics_ZeroBiasOverlay.merge.RAW._lb0137._SFO-ALL._0001.1"]
#AB  ServiceMgr.ByteStreamInputSvc.FullFileName = [ "/afs/cern.ch/user/e/eyazici/rawdata/data12_8TeV.00201113.physics_ZeroBiasOverlay.merge.RAW/data12_8TeV.00201113.physics_ZeroBiasOverlay.merge.RAW._lb0423._SFO-ALL._0001.1"]
  ServiceMgr.ByteStreamInputSvc.FullFileName = [ "/afs/cern.ch/user/e/eyazici/public/data12_8TeV.00201113.physics_ZeroBiasOverlay.merge.RAW._lb0423._SFO-ALL._0001.1"]

#  ServiceMgr.ByteStreamInputSvc.FullFileName = [ "/tmp/rjungst/testinput"]
