# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#==============================================================
# Job options file for the AFP_VertexReco package
#==============================================================

from AthenaConfiguration.ComponentFactory import CompFactory


# Prepare Vertex reconstruction algorithm tools 


TimeOffsetA = [64,57,30,52]
TimeSlopeA = [-8.6,-10.4,-9.7,-17.]
TimeOffsetC = [-4,1,17,7]     
TimeSlopeC = [9.6,11,11,11]
TrainEdgesA = [-18.7,-13.2,-8.1,-4.0,-2.5]
TrainEdgesC = [-18.7,-13.2,-8.1,-4.0,-2.5]
TimeGlobalOffset = 0

afpVertexRecoTool = CompFactory.getComp("AFP_VertexRecoBasic")("AFP_VertexRecoBasic",TrackDistance=0.5,TimeOffsetA = TimeOffsetA, TimeSlopeA = TimeSlopeA, TimeOffsetC = TimeOffsetC, TimeSlopeC = TimeSlopeC, TimeGlobalOffset = TimeGlobalOffset, TrainEdgesA = TrainEdgesA, TrainEdgesC = TrainEdgesC)

verticesToolsList=[afpVertexRecoTool]

# collect all output names and make a list with unique names for write handle keys; if this goes wrong AFP_VertexRecoTool::initialize() will complain
outputVertexList=[]
for vertexTool in verticesToolsList:
        try:
                # in case the "verticesContainerName" is set
                contName=getattr(vertexTool, "verticesContainerName")
        except AttributeError:
                # in case the "verticesContainerName" is not set
                contName=vertexTool.getDefaultProperty("verticesContainerName")
        if contName not in outputVertexList:
                outputVertexList.append(contName)

# prepare vertex reco tool
vertexRecoTool = CompFactory.getComp("AFP_VertexRecoTool")("AFP_VertexRecoTool", RecoToolsList=verticesToolsList, AFPVertexContainerList=outputVertexList )

# actually setup the vertex reco
from AFP_VertexReco.AFP_VertexRecoConf import AFP_VertexReco
topSequence += AFP_VertexReco("AFP_VertexReco", recoTool = vertexRecoTool)


