
include ("RecExRecoTest/RecExRecoTest_RTT_common.py")

jp.AthenaCommonFlags.PoolESDOutput="id_ESD.pool.root"
rec.doTrigger=False
#rec.doWriteESD=False # uncomment if do not write ESD
rec.doAOD=False
rec.doWriteAOD=False # uncomment if do not write AOD
rec.doWriteTAG=False # uncomment if do not write TAG

from RecExConfig.RecFlags import rec
rec.doForwardDet=False
rec.doInDet=True
rec.doMuon=True # sw
rec.doCalo=True # sw
rec.doEgamma=True # sw
rec.doMuonCombined=True # sw2
rec.doJetMissingETTag=True # sw2
rec.doTau=False
rec.doJiveXML=True # sw



# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")
include ("RecExRecoTest/RecExRecoTest_RTT_common_postOptions.py")

include ("xAODJiveXML/xAODJiveXML_DataTypes.py")

theEventData2XML.DataTypes.remove("JiveXML::SiSpacePointRetriever/SiSpacePointRetriever")
theEventData2XML.DataTypes.remove("JiveXML::SiClusterRetriever/SiClusterRetriever")
theEventData2XML.DataTypes.remove("JiveXML::PixelClusterRetriever/PixelClusterRetriever")
theEventData2XML.DataTypes.remove("JiveXML::TRTRetriever/TRTRetriever")
theEventData2XML.DataTypes.remove("JiveXML::TrigSiSpacePointRetriever/TrigSiSpacePointRetriever")
theCaloLArRetriever.LArlCellThreshold = 200.
theCaloHECRetriever.HEClCellThreshold = 200.
####
theEventData2XML.DataTypes.remove("JiveXML::PixelRDORetriever/PixelRDORetriever")
theEventData2XML.DataTypes.remove("JiveXML::SCTRDORetriever/SCTRDORetriever")

