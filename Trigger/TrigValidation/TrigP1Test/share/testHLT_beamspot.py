# Minimal job options to test BeamSpot updates

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

athenaCommonFlags.isOnline=True

globalflags.InputFormat='bytestream'
globalflags.DataSource='data'
globalflags.DetDescrVersion='ATLAS-R2-2015-03-01-00' 
globalflags.ConditionsTag='CONDBR2-HLTP-2016-01'

from IOVDbSvc.CondDB import conddb
svcMgr.IOVDbSvc.GlobalTag=globalflags.ConditionsTag()

conddb.addFolderWithTag('beampos.db','/Indet/Onl/Beampos <key>/Indet/Beampos</key>','IndetBeamposOnl-HLT-UPD1-001-00')

from InDetBeamSpotService.InDetBeamSpotServiceConf import BeamCondSvc
svcMgr += BeamCondSvc()
theApp.CreateSvc += [ svcMgr.BeamCondSvc.getFullName() ]

svcMgr.HltEventLoopMgr.CoolUpdateTool.enable() 
