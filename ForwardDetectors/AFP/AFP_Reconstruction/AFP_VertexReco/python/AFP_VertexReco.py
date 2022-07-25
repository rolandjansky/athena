# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#==============================================================
# Job options file for the AFP_VertexReco package
#==============================================================

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFolders
from TrigEDMConfig.TriggerEDMRun3 import recordable

# Prepare Vertex reconstruction algorithm tools
def AFP_VertexReco_Cfg(flags, kwargs={}):
	
	acc = ComponentAccumulator()
	
	if flags.Input.isMC:
		acc.merge(addFolders(flags, "/FWD/AFP/ToFParameters/Vertex<tag>AFPMCToFVtx-ideal-01</tag>", 'FWD_OFL', className='CondAttrListCollection', db='OFLP200'))
		tofVtxParamTool  = CompFactory.getComp("AFP::ToFVtxParamDBTool")("ToFVtxParamDBTool", vtx_param_key="/FWD/AFP/ToFParameters/Vertex")
	else:
		acc.merge(addFolders(flags, "/FWD/Onl/AFP/ToFParameters/Vertex<tag>AFPToFVtx-01</tag>", 'FWD_ONL', className='CondAttrListCollection', db='CONDBR2'))
		tofVtxParamTool  = CompFactory.getComp("AFP::ToFVtxParamDBTool")("ToFVtxParamDBTool", vtx_param_key="/FWD/Onl/AFP/ToFParameters/Vertex")
	
	afpVertexRecoTool = CompFactory.AFP_VertexRecoBasic("AFP_VertexRecoBasic", TrackDistance=0.5, tofVtxParamDBTool=tofVtxParamTool, **kwargs)
	
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
	acc.addEventAlgo(CompFactory.AFP_VertexReco("AFP_VertexReco", recoTool = vertexRecoTool))
	
	return acc


def AFP_VertexReco_HLT(flags):
	from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
	acc = ComponentAccumulator()
	
	from IOVDbSvc.CondDB import conddb
	if flags.Input.isMC:
		if not conddb.folderRequested('/FWD/AFP/ToFParameters/Vertex'):
			conddb.addFolder("FWD_OFL","/FWD/AFP/ToFParameters/Vertex<tag>AFPMCToFVtx-ideal-01</tag>", className="CondAttrListCollection")
	else:
		if not conddb.folderRequested('/FWD/Onl/AFP/ToFParameters/Vertex'):
			conddb.addFolder("FWD_ONL","/FWD/Onl/AFP/ToFParameters/Vertex<tag>AFPToFVtx-03</tag>", className="CondAttrListCollection")
	
	acc.merge(AFP_VertexReco_Cfg(flags, {"AFPToFTrackContainerKey": "HLT_AFPToFTrackContainer", "AFPProtonContainerKey": "HLT_AFPProtonContainer", "verticesContainerName": recordable("HLT_AFPVertexContainer")}))
	
	return acc

