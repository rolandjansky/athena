
#--------------------------------------------------------------------
# Perfmon configuration
#--------------------------------------------------------------------
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True
jobproperties.PerfMonFlags.doMallocMonitoring = True
jobproperties.PerfMonFlags.doPostProcessing = True

#--------------------------------------------------------------
# AthenaCommon configuration
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.EvtMax=-1
#athenaCommonFlags.PoolHitsInput=["/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005122.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005207.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005259.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005319.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005366.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005435.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005453.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005555.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005595.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005607.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005619.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005656.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005667.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005717.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005745.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005758.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005838.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005902.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005921.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005976.pool.root.1"]
#athenaCommonFlags.PoolHitsInput=["root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.simul.HITS.e1193_s1469/HITS.782686._001790.pool.root.1"]
#athenaCommonFlags.PoolHitsInput=["/afs/cern.ch/user/n/ncalace/public/HITS.01138936._005122.pool.root.1"]
#athenaCommonFlags.PoolHitsInput=["/afs/cern.ch/user/t/tsfiligo/public/mc12_8TeV.107199.ParticleGenerator_mu_Pt1.merge.HITS.e1717_s1609_s1611_tid01138923_00/HITS.01138923._000001.pool.root.1"] #   1 GeV
#athenaCommonFlags.PoolHitsInput=["/afs/cern.ch/user/t/tsfiligo/public/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1399981501/HITS.01138936._005122.pool.root.1"] #   5 GeV
#athenaCommonFlags.PoolHitsInput=["/afs/cern.ch/user/t/tsfiligo/public/user.tsfiligo.mc12_8TeV.107211.ParticleGenerator_mu_Pt10.merge.HITS.e1717_s1609_s1611_tid01138939_00_der1399974302/HITS.01138939._000080.pool.root.1"] #  10 GeV
#athenaCommonFlags.PoolHitsInput=["/afs/cern.ch/user/t/tsfiligo/public/mc12_8TeV.107233.ParticleGenerator_mu_Pt100.simul.HITS.e1717_s1581_tid01239219_00/HITS.01239219._000001.pool.root.1"] # 100 GeV
athenaCommonFlags.PoolHitsInput=["Hits.pool.root"]
athenaCommonFlags.PoolRDOOutput="DigitizationOutput.pool.root"

# the magnetic field
from MagFieldServices import SetupField
from IOVDbSvc.CondDB import conddb
conddb.addOverride('/GLOBAL/BField/Map','BFieldMap-FullAsym-09-solTil')

DetDescrVersion = 'ATLAS-R1-2012-02-01-00'
#from AtlasGeoModel import SetGeometryVersion 
#from AtlasGeoModel import GeoModelInit 

#from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
#GeoModelSvc = GeoModelSvc()
#ServiceMgr += GeoModelSvc
#GeoModelSvc.AtlasVersion = DetDescrVersion

#from IOVDbSvc.CondDB import conddb
#conddb.setGlobalTag('OFLCOND-SIM-00-00-00')

from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.ISF_FatrasCustomGeometry = True 
# TrkDetFlags.ConfigurationOutputLevel = INFO
TrkDetFlags.MaterialSource           = 'COOL'
TrkDetFlags.MaterialVersion          = 17
TrkDetFlags.ConfigurationOutputLevel = VERBOSE
TrkDetFlags.MaterialDatabaseLocal    = True
if TrkDetFlags.MaterialDatabaseLocal() is True :
    # prepare the magic tag
    splitGeo = DetDescrVersion.split('-')
    MaterialMagicTag = splitGeo[0] + '-' + splitGeo[1] + '-' + splitGeo[2]
    # now say where the file is
    TrkDetFlags.MaterialStoreGateKey        = '/GLOBAL/TrackingGeo/BinnedLayerMaterial'
    TrkDetFlags.MaterialDatabaseLocalPath    = '' 
    TrkDetFlags.MaterialDatabaseLocalName    = 'AtlasLayerMaterial-'+DetDescrVersion+'.db'
    TrkDetFlags.MaterialMagicTag             = MaterialMagicTag

#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import digitizationFlags
#digitizationFlags.IOVDbGlobalTag='OFLCOND-MC12-SDR-06'
digitizationFlags.IOVDbGlobalTag='OFLCOND-SIM-00-00-00'

#--------------------------------------------------------------
# Global flags. Like eg the DD version:
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
#globalflags.DetDescrVersion='ATLAS-GEO-20-00-01'
globalflags.DetDescrVersion=DetDescrVersion

#--------------------------------------------------------------------
# DetFlags. Use to turn on/off individual subdetector or LVL1 trigger
#--------------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.LVL1_setOff()
DetFlags.Truth_setOn()

include("FastSiDigitization/SiSmearing_jobOptions.py")

DetFlags.pixel_setOff()
DetFlags.detdescr.pixel_setOn()

DetFlags.SCT_setOff() # setOn to use the standard digi
DetFlags.detdescr.SCT_setOn()

DetFlags.TRT_setOn() # setOn to use the standard digi

include("Digitization/Digitization.py")

StoreGateSvc = Service("StoreGateSvc")
StoreGateSvc.Dump = True

from InDetRecExample.InDetKeys import InDetKeys
InDetKeys.PixelManager = "Pixel"

from PixelConditionsTools.PixelConditionsToolsConf import PixelRecoDbTool
ToolSvc += PixelRecoDbTool()
ToolSvc.PixelRecoDbTool.InputSource = 1

from PixelConditionsTools.PixelConditionsToolsConf import PixelCalibDbTool
ToolSvc += PixelCalibDbTool()

from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.enableSuppression = False
ServiceMgr.MessageSvc.Format       = "% F%50W%S%7W%R%T %0W%M"
OutputNameString = 'CheckSmearing'

if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
 
#ServiceMgr.THistSvc.Output  = ["val DATAFILE='"+OutputNameString+".root' TYPE = 'ROOT' OPT='RECREATE'"];


from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.SystemOfUnits import *
import math

#from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
#TrkDetFlags.ISF_FatrasCustomGeometry = True 
#TrkDetFlags.MaterialSource           = 'None'

from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc

topSequence = AlgSequence()

topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.CheckSmear=True
topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.OutputLevel=VERBOSE
topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.pitch_X=10.0*micrometer
#topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.pitch_X=0*micrometer
topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.pitch_Y=55.0*micrometer
#topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.pitch_Y=0*micrometer
topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.TrackingGeometrySvc = AtlasTrackingGeometrySvc
topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.UseCustomGeometry = True
topSequence.PixelSmearedDigitization.SiSmearedDigitizationTool.Nsigma = 1.0

topSequence.SCTSmearedDigitization.SiSmearedDigitizationTool.CheckSmear=True
topSequence.SCTSmearedDigitization.SiSmearedDigitizationTool.SmearPixel = False
topSequence.SCTSmearedDigitization.SiSmearedDigitizationTool.OutputLevel=VERBOSE
topSequence.SCTSmearedDigitization.SiSmearedDigitizationTool.pitch_X= 70.0*micrometer
#topSequence.SCTSmearedDigitization.SiSmearedDigitizationTool.pitch_X= 0*micrometer


topSequence.StreamRDO.ItemList += ["iFatras::PlanarClusterContainer#PlanarClusters"]
#topSequence.StreamRDO.ItemList += ["InDet::SiClusterContainer#PixelClusters"]
topSequence.StreamRDO.ItemList += ["InDet::SiClusterContainer#SCT_Clusters"]

topSequence.StreamRDO.ItemList += ["PRD_MultiTruthCollection#PRD_MultiTruthPlanar"]
#topSequence.StreamRDO.ItemList += ["PRD_MultiTruthCollection#PRD_MultiTruthPixel"]
topSequence.StreamRDO.ItemList += ["PRD_MultiTruthCollection#PRD_MultiTruthSCT"]

#from VP1Algs.VP1AlgsConf import VP1Alg 
#topSequence += VP1Alg()

