#
# Job options to run on AOD and run EF muon -> xAOD conversion algorithm
#
import glob
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from InDetRecExample.InDetJobProperties import InDetFlags

from AthenaCommon.AppMgr import theApp
theApp.ReflexPluginDebugLevel=1

#set input file. Note that this triggers autoconfiguration automatically (see RecExCommonAutoConfiguration wiki)
# here a MC RDO is specified, but this can also be a data BS, an ESD AOD or TAG
#if athenaCommonFlags.FilesInput.isDefault(): # check if not already set upstream
#    athenaCommonFlags.FilesInput=["LFN:top_GEO-02-01-00_RDO_extract.pool"] 

#athenaCommonFlags.FilesInput=["/misc/manio/markowen/AOD/data12_8TeV.00200863.physics_Muons.recon.DAOD_ZMUMU.f431_m716_f431/data12_8TeV.00200863.physics_Muons.recon.DAOD_ZMUMU.f431_m716_f431._0001.1"]
athenaCommonFlags.FilesInput=glob.glob('/misc/manio/markowen/AOD/data12_8TeV.00200863.physics_Muons.recon.DAOD_ZMUMU.f431_m716_f431/*')

athenaCommonFlags.EvtMax=-1   # number of events to process run on all file



# include my own algorithm(s). These jobo will be included at the end of topSequence configuration
rec.UserAlgs=[ "xAODTrigMuonCnv/addEFMuonxAODCnv_jobOptions.py" ]

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

#svcMgr.MessageSvc.OutputLevel=VERBOSE
#topSequence.CaloCluster2xAOD.OutputLevel=DEBUG


#include("RecExCommon/CleanItemList_Rel19.py")
