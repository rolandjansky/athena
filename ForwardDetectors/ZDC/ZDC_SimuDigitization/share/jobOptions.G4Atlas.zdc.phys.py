
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

ServiceMgr.MessageSvc.OutputLevel = DEBUG

from AthenaCommon.DetFlags import DetFlags
DetFlags.bpipe_setOn()
DetFlags.ZDC_setOn()
DetFlags.Truth_setOn()

from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.ConditionsTag = "OFLCOND-SDR-BS7T-05-14"

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput = ['/afs/cern.ch/user/a/asbrizzi/links/reldis_offline/v0.0/test/inter/evgen/MC12.Reldis.evgen_17.1.4.2_status1_MeV.pool.root']
athenaCommonFlags.PoolHitsOutput = "atlasG4.hits.pool.root"
athenaCommonFlags.EvtMax = 1

from G4AtlasApps.SimFlags import SimFlags
SimFlags.load_atlas_flags()
SimFlags.SimLayout='ATLAS-GEO-18-01-00'
SimFlags.EventFilter.set_Off()
SimFlags.MagneticField.set_Off()
SimFlags.ForwardDetectors.set_On()
SimFlags.ForwardDetectors=2

from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
topSeq += PyG4AtlasAlg()

