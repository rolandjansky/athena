# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#==============================================================
# Job options file for the AFP_GlobReco package
#==============================================================

from AthenaConfiguration.ComponentFactory import CompFactory
from TrigEDMConfig.TriggerEDMRun3 import recordable

def AFP_GlobReco_Cfg(kwargs={}):
	# side A = 0, side C = 1
	afpProtonRecoToolA = CompFactory.AFP_ProtonRecoAnalytical("AFP_ProtonRecoAnalyticalA", parametrizationFileName="param_mad_b1_def.txt", side=0, **kwargs)
	afpProtonRecoToolC = CompFactory.AFP_ProtonRecoAnalytical("AFP_ProtonRecoAnalyticalC", parametrizationFileName="param_mad_b2_def.txt", side=1, **kwargs)

	protonsToolsList=[afpProtonRecoToolA, afpProtonRecoToolC]

	# collect all output names and make a list with unique names for write handle keys; if this goes wrong AFP_GlobRecoTool::initialize() will complain
	outputProtonList=[]
	for protonTool in protonsToolsList:
		try:
			# in case the "protonsContainerName" is set
			contName=getattr(protonTool, "protonsContainerName")
		except AttributeError:
			# in case the "protonsContainerName" is not set
			contName=protonTool.getDefaultProperty("protonsContainerName")
		if contName not in outputProtonList:
			outputProtonList.append(contName)

	# prepare proton reco tool
	protonRecoTool = CompFactory.AFP_GlobRecoTool("AFP_GlobRecoTool", RecoToolsList=protonsToolsList, AFPProtonContainerList=outputProtonList )

	# actually setup the track reco
	return CompFactory.AFP_GlobReco("AFP_GlobReco", recoTool = protonRecoTool)


def AFP_GlobReco_HLT():
	
	AFP_Pr = AFP_GlobReco_Cfg({"AFPTrackContainerKey": "HLT_AFPTrackContainer", "protonsContainerName": recordable("HLT_AFPProtonContainer")})
	
	return AFP_Pr


