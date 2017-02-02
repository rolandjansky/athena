#
# Job options to run on AOD and run MuonFeature, MuonFeatureDetails -> xAOD conversion algorithm
#
import glob
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from InDetRecExample.InDetJobProperties import InDetFlags

from AthenaCommon.AppMgr import theApp
theApp.ReflexPluginDebugLevel=1

athenaCommonFlags.FilesInput=glob.glob('/afs/cern.ch/user/m/mishitsu/public/data12_8TeV.00200863.physics_Muons.recon.DAOD_ZMUMU.f431_m716_f431._0001.1')

athenaCommonFlags.EvtMax=-1   # number of events to process run on all file

rec.UserAlgs=[ "xAODTrigMuonCnv/addMuonFeaturexAODCnv_jobOptions.py" ]

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")
