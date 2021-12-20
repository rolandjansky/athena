# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#==============================================================
# Job options file for the AFP_LocReco package
#==============================================================

from AthenaCommon.GlobalFlags import globalflags

#-- SiD part ------------------------------------------------------------

# convert EventInfo to xAOD::EventInfo
if( not globalflags.InputFormat.is_bytestream() and ( not objKeyStore.isInInput( "xAOD::EventInfo") ) and ( not hasattr( topSequence, "xAODMaker::EventInfoCnvAlg" ) ) ):
        from xAODEventInfoCnv.xAODEventInfoCnvAlgDefault import xAODEventInfoCnvAlgDefault
        xAODEventInfoCnvAlgDefault (sequence = topSequence)

# get geometry tool
from AthenaConfiguration.ComponentFactory import CompFactory
afpGeometryTool = CompFactory.getComp("AFP_GeometryTool")("AFP_Geometry_tool")

# prepare clustering tool
from AthenaConfiguration.ComponentFactory import CompFactory
clusterNeighbour = CompFactory.getComp("AFPSiClusterBasicNearestNeighbour")("AFPSiClusterBasicNearestNeighbour")
# it's also possible to get the same clustering tool as in AFPAnalysisToolbox
# clusterNeighbour = CompFactory.getComp("AFPSiClusterAllNeighbours")("AFPSiClusterAllNeighbours", neighbourhoodType="X")

rowColToLocal = CompFactory.getComp("AFPSiRowColToLocalCSTool")("AFPSiRowColToLocalCSTool", AFP_Geometry=afpGeometryTool)

clusterTool = CompFactory.getComp("AFPSiClusterTool")("AFPSiClusterTool", clusterAlgTool=clusterNeighbour, rowColToLocalCSTool = rowColToLocal)

# actually setup the cluster reco
from AFP_SiClusterTools.AFP_SiClusterToolsConf import AFPSiCluster
topSequence += AFPSiCluster("AFPSiCluster", clusterRecoTool = clusterTool)


# prepare track reconstruction algorithm tools - one for each station
kalmanTool0 = CompFactory.getComp("AFPSiDBasicKalmanTool")("AFPSiDBasicKalmanTool0", stationID=0)

# number of layers in each station, size of the vector determines number of stations
# kalmanTool1.numberOfLayersInStation = 3

# Maximal distance at which cluster can be joined to the track
# kalmanTool1.maxAllowedDistance = 10

# Minimal number of clusters in track. If there are less clusters track is rejected
# kalmanTool1.minClustersNumber = 3

kalmanTool1 = CompFactory.getComp("AFPSiDBasicKalmanTool")("AFPSiDBasicKalmanTool1", stationID=1)
kalmanTool2 = CompFactory.getComp("AFPSiDBasicKalmanTool")("AFPSiDBasicKalmanTool2", stationID=2)
kalmanTool3 = CompFactory.getComp("AFPSiDBasicKalmanTool")("AFPSiDBasicKalmanTool3", stationID=3)

# it's also possible to have the same track reco tool as in AFPAnalysisToolbox
# linRegTool0 = CompFactory.getComp("AFPSiDLinRegTool")("AFPSiDLinRegTool0", stationID=0)
# linRegTool1 = CompFactory.getComp("AFPSiDLinRegTool")("AFPSiDLinRegTool1", stationID=1)
# linRegTool2 = CompFactory.getComp("AFPSiDLinRegTool")("AFPSiDLinRegTool2", stationID=2)
# linRegTool3 = CompFactory.getComp("AFPSiDLinRegTool")("AFPSiDLinRegTool3", stationID=3)

kalmanToolsList=[kalmanTool0, kalmanTool1, kalmanTool2, kalmanTool3]

# collect all output names and make a list with unique names for write handle keys; if this goes wrong AFP_SIDLocRecoTool::initialize() will complain
outputKalmanList=[]
for kalmanTool in kalmanToolsList:
        try:
                # in case the "tracksContainerName" is set
                contName=getattr(kalmanTool, "tracksContainerName")
        except AttributeError:
                # in case the "tracksContainerName" is not set
                contName=kalmanTool.getDefaultProperty("tracksContainerName")
        if contName not in outputKalmanList:
                outputKalmanList.append(contName)

# prepare track reco tool
trackRecoTool = CompFactory.getComp("AFP_SIDLocRecoTool")("AFP_SIDLocRecoTool", RecoToolsList=kalmanToolsList, AFPTrackContainerList=outputKalmanList )

# actually setup the track reco
from AFP_LocReco.AFP_LocRecoConf import AFP_SIDLocReco
topSequence += AFP_SIDLocReco("AFP_SIDLocReco", recoTool = trackRecoTool)


#-- TiD part ------------------------------------------------------------


# Prepare ToF reconstruction algorithm tools - one for each station
BarWeight = [1.0, 1.0, 1.0, 1.0] 
TimeOffset0 = [105707, 105670, 105675, 105638,
               105834, 105819, 105778, 105754,
               105900, 105892, 105870, 105820,
               105726, 105720, 105714, 105717]

basicTool0 = CompFactory.getComp("AFPTDBasicTool")("AFPTDBasicTool0", stationID=0, maxAllowedLength=100, TimeOffset=TimeOffset0, BarWeight=BarWeight)

TimeOffset3 = [105796, 105761, 105742, 105696,
               105890, 105871, 105839, 105816,
               105923, 105899, 105862, 105853,
       	       105707, 105711, 105714, 105713]

basicTool3 = CompFactory.getComp("AFPTDBasicTool")("AFPTDBasicTool3", stationID=3, maxAllowedLength=100, TimeOffset=TimeOffset3, BarWeight=BarWeight)
basicToolsList=[basicTool0, basicTool3]

# collect all output names and make a list with unique names for write handle keys; if this goes wrong AFP_SIDLocRecoTool::initialize() will complain
outputBasicList=[]
for basicTool in basicToolsList:
        try:
                # in case the "tracksContainerName" is set
                contName=getattr(basicTool, "tracksContainerName")
        except AttributeError:
                # in case the "tracksContainerName" is not set
                contName=basicTool.getDefaultProperty("tracksContainerName")
        if contName not in outputBasicList:
                outputBasicList.append(contName)

# prepare track reco tool
ToFtrackRecoTool = CompFactory.getComp("AFP_TDLocRecoTool")("AFP_TDLocRecoTool", RecoToolsList=basicToolsList, AFPToFTrackContainerList=outputBasicList )

# actually setup the ToF track reco
from AFP_LocReco.AFP_LocRecoConf import AFP_TDLocReco
topSequence += AFP_TDLocReco("AFP_TDLocReco", recoTool=ToFtrackRecoTool)


#-- SiAlignDBTester part ------------------------------------------------------------
# this is a setup for ReadCondHandle (see AFP_DBTools/SiAlignDBTester)

# from IOVDbSvc.CondDB import conddb

# conddb.addFolder("CALO","/CALO/HadCalibration2/CaloEMFrac", className='CaloLocalHadCoeff')
# conddb.addFolderSplitOnline("INDET","/Indet/Onl/Beampos", "/Indet/Beampos", className='AthenaAttributeList')

# schema = "<db>sqlite://;schema=Example.db;dbname=CONDBR2</db>"
# folder = "/FWD/AFP/LocalAlignment"
# locTag="<tag>AFPAlignLoc-00</tag>"

# conddb.addFolder("", schema+folder+locTag, className='CondAttrListCollection')

# from AFP_DBTools.AFP_DBToolsConf      import SiAlignDBTester
# TopLocRecSeq += SiAlignDBTester("SiAlignDBTester", OutputLevel=DEBUG)

