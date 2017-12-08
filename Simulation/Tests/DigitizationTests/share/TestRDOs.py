#import glob, os, re
import traceback

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#==============================================================
# Job definition parameters:
#==============================================================
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#Jobs should stop if an include fails.
athenaCommonFlags.AllowIgnoreConfigError = False
#athenaCommonFlags.FilesInput = [ 'root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/rtt/prod/rtt/rel_3/devval/x86_64-slc5-gcc43-opt/offline/DigitizationTests/mc11_ttbar_LVL1_on/mc11_ttbar.RDO.pool.root' ]
athenaCommonFlags.FilesInput = [ 'root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.simul.HITS.e1193_s1469/HITS.782686._001790.pool.root.1' ]

from AthenaCommon.AppMgr import theApp
theApp.EvtMax = -1

## Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.LAr_setOff()
DetFlags.Tile_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOn()

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion.set_Value_and_Lock( 'ATLAS-GEO-18-01-03' )
globalflags.ConditionsTag.set_Value_and_Lock( 'OFLCOND-SDR-BS7T-05-24' )
#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
from AthenaCommon.AppMgr import ServiceMgr
from AthenaPoolCnvSvc.AthenaPoolCnvSvcConf import AthenaPoolCnvSvc
ServiceMgr += AthenaPoolCnvSvc()

ServiceMgr.AthenaPoolCnvSvc.PoolAttributes = [ "DEFAULT_BUFFERSIZE = '2048'" ]

import AthenaPoolCnvSvc.ReadAthenaPool

from CLIDComps.CLIDCompsConf import ClassIDSvc
ServiceMgr += ClassIDSvc()
include( "PartPropSvc/PartPropSvc.py" )

# load all possible converters for EventCheck
GeoModelSvc = Service( "GeoModelSvc" )
GeoModelSvc.IgnoreTagDifference=True

# set up all detector description stuff
include( "RecExCond/AllDet_detDescr.py" )

## TGCcablingServerSvc Hack
import MuonCnvExample.MuonCablingConfig

#--------------------------------------------------------------
# Setup Input
#--------------------------------------------------------------
ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()
ServiceMgr.EventSelector.SkipEvents = 0

from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc("THistSvc")
#ServiceMgr.THistSvc.Output  = ["atlasTest DATAFILE='atlasTest.muons.histo.root' OPT='RECREATE'"];
ServiceMgr.THistSvc.Output  = ["truth DATAFILE='truth.root' OPT='RECREATE'"];

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
from AthenaCommon import CfgGetter
from DigitizationTests.DigitizationTestsConf import DigiTestAlg,McEventCollectionTestTool,PileUpEventInfoTestTool,PixelRDOsTestTool,SCT_RDOsTestTool,TRT_RDOsTestTool
job += DigiTestAlg()
if DetFlags.Truth_on():
  job.DigiTestAlg.DigiTestTools += [CfgGetter.getPublicTool("McEventCollectionTestTool", checkType=True)]
if DetFlags.Truth_on():
  job.DigiTestAlg.DigiTestTools += [CfgGetter.getPublicTool("PileUpEventInfoTestTool", checkType=True)]
if DetFlags.pixel_on():
  job.DigiTestAlg.DigiTestTools += [CfgGetter.getPublicTool("PixelRDOsTestTool", checkType=True)]
if DetFlags.SCT_on():
  job.DigiTestAlg.DigiTestTools += [CfgGetter.getPublicTool("SCT_RDOsTestTool", checkType=True)]
if DetFlags.TRT_on():
  job.DigiTestAlg.DigiTestTools += [CfgGetter.getPublicTool("TRT_RDOsTestTool", checkType=True)]
