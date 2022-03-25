# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#==============================================================
# Job options file for the AFP_VertexReco package
#==============================================================

from AthenaConfiguration.ComponentFactory import CompFactory
from TrigEDMConfig.TriggerEDMRun3 import recordable

# Prepare Vertex reconstruction algorithm tools
def AFP_VertexReco_Cfg(kwargs={}):

	TimeOffsetA = [64,45,25,11]
	TimeSlopeA = [-7.3,-5.0,-4.0,-4.]
	TimeOffsetC = [2,9,14,12]     
	TimeSlopeC = [4,5,3,6]
	TrainEdgesA = [-18.7,-13.2,-8.1,-4.0,-2.5]
	TrainEdgesC = [-18.7,-13.2,-8.1,-4.0,-2.5]
	TimeGlobalOffset = 5

	afpVertexRecoTool = CompFactory.AFP_VertexRecoBasic("AFP_VertexRecoBasic",TrackDistance=0.5,TimeOffsetA = TimeOffsetA, TimeSlopeA = TimeSlopeA, TimeOffsetC = TimeOffsetC, TimeSlopeC = TimeSlopeC, TimeGlobalOffset = TimeGlobalOffset, TrainEdgesA = TrainEdgesA, TrainEdgesC = TrainEdgesC, **kwargs)

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
	vertexRecoTool = CompFactory.AFP_VertexRecoTool("AFP_VertexRecoTool", RecoToolsList=verticesToolsList, AFPVertexContainerList=outputVertexList )

	# actually setup the vertex reco
	return CompFactory.AFP_VertexReco("AFP_VertexReco", recoTool = vertexRecoTool)


def AFP_VertexReco_HLT():
	
	AFP_Vtx = AFP_VertexReco_Cfg({"AFPToFTrackContainerKey": "HLT_AFPToFTrackContainer", "AFPProtonContainerKey": "HLT_AFPProtonContainer", "verticesContainerName": recordable("HLT_AFPVertexContainer")})
	
	return AFP_Vtx
