# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#==============================================================
# Job options file for the AFP_LocReco package
#==============================================================

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import Format
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from TrigEDMConfig.TriggerEDMRun3 import recordable

def AFP_LocReco_SiD_Cfg(kwargs={}):

	# prepare track reconstruction algorithm tools - one for each station
	kalmanTool0 = CompFactory.AFPSiDBasicKalmanTool("AFPSiDBasicKalmanTool0", stationID=0, **kwargs)

	# number of layers in each station, size of the vector determines number of stations
	# kalmanTool1.numberOfLayersInStation = 3

	# Maximal distance at which cluster can be joined to the track
	# kalmanTool1.maxAllowedDistance = 10

	# Minimal number of clusters in track. If there are less clusters track is rejected
	# kalmanTool1.minClustersNumber = 3

	kalmanTool1 = CompFactory.AFPSiDBasicKalmanTool("AFPSiDBasicKalmanTool1", stationID=1, **kwargs)
	kalmanTool2 = CompFactory.AFPSiDBasicKalmanTool("AFPSiDBasicKalmanTool2", stationID=2, **kwargs)
	kalmanTool3 = CompFactory.AFPSiDBasicKalmanTool("AFPSiDBasicKalmanTool3", stationID=3, **kwargs)
	
	# it's also possible to have the same track reco tool as in AFPAnalysisToolbox
	# linRegTool0 = CompFactory.AFPSiDLinRegTool("AFPSiDLinRegTool0", stationID=0, **kwargs)
	# linRegTool1 = CompFactory.AFPSiDLinRegTool("AFPSiDLinRegTool1", stationID=1, **kwargs)
	# linRegTool2 = CompFactory.AFPSiDLinRegTool("AFPSiDLinRegTool2", stationID=2, **kwargs)
	# linRegTool3 = CompFactory.AFPSiDLinRegTool("AFPSiDLinRegTool3", stationID=3, **kwargs)

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
	trackRecoTool = CompFactory.AFP_SIDLocRecoTool("AFP_SIDLocRecoTool", RecoToolsList=kalmanToolsList, AFPTrackContainerList=outputKalmanList )

	# actually setup the track reco
	return CompFactory.AFP_SIDLocReco("AFP_SIDLocReco", recoTool = trackRecoTool)


def AFP_LocReco_TD_Cfg(kwargs={}):

	# Prepare ToF reconstruction algorithm tools - one for each station
	BarWeight = [1.0, 1.0, 1.0, 1.0] 

	TimeOffset0 = [1494, 1500, 1500, 1500,
	1500, 1500, 1500, 1500,
	1500, 1500, 1500, 1500,
	1500, 1500, 1500, 1500]
	basicTool0 = CompFactory.getComp("AFPTDBasicTool")("AFPTDBasicTool0", stationID=0, maxAllowedLength=1500, TimeOffset=TimeOffset0, BarWeight=BarWeight, **kwargs)
	
	if ConfigFlags.Input.Format is Format.POOL and "AFPToFHitContainer" not in ConfigFlags.Input.Collections:
		basicTool0.AFPToFHitClusterContainerKey=""

	TimeOffset3 = [1500, 1500, 1500, 1500,
	1500, 1500, 1500, 1500,
	1500, 1500, 1500, 1500,
	1500, 1500, 1500, 1500]
	basicTool3 = CompFactory.getComp("AFPTDBasicTool")("AFPTDBasicTool3", stationID=3, maxAllowedLength=1500, TimeOffset=TimeOffset3, BarWeight=BarWeight, **kwargs)

	if ConfigFlags.Input.Format is Format.POOL and "AFPToFHitContainer" not in ConfigFlags.Input.Collections:
		basicTool3.AFPToFHitClusterContainerKey=""

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
	ToFtrackRecoTool = CompFactory.AFP_TDLocRecoTool("AFP_TDLocRecoTool", RecoToolsList=basicToolsList, AFPToFTrackContainerList=outputBasicList )

	# actually setup the ToF track reco
	return CompFactory.AFP_TDLocReco("AFP_TDLocReco", recoTool=ToFtrackRecoTool)


def AFP_LocReco_SiD_HLT():
	
	AFP_SID = AFP_LocReco_SiD_Cfg({"tracksContainerName": recordable("HLT_AFPTrackContainer"), "AFPSiHitsClusterContainerKey": "HLT_AFPSiHitsClusterContainer"})
	
	from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
	
	monTool_AFP_SIDLocRecoTool = GenericMonitoringTool("MonTool_AFP_SIDLocRecoTool")
	monTool_AFP_SIDLocRecoTool.defineHistogram( 'TrkSize', path='EXPERT', type='TH1F', title='AFP tracks size',xbins=50, xmin=0, xmax=100 )
	AFP_SID.recoTool.MonTool = monTool_AFP_SIDLocRecoTool
	
	for i, kalmanTool in enumerate(AFP_SID.recoTool.RecoToolsList):
		monTool_AFP_BasicKalman = GenericMonitoringTool("MonTool_AFP_"+kalmanTool.name())
	
		monTool_AFP_BasicKalman.defineHistogram( 'TrkStationID', path='EXPERT', type='TH1F', title='Track station ID',xbins=4, xmin=0, xmax=4 )
		monTool_AFP_BasicKalman.defineHistogram( 'TrkXLocal', path='EXPERT', type='TH1F', title='Track xLocal',xbins=100, xmin=-200, xmax=200 )
		monTool_AFP_BasicKalman.defineHistogram( 'TrkYLocal', path='EXPERT', type='TH1F', title='Track yLocal',xbins=100, xmin=-30, xmax=30 )
		monTool_AFP_BasicKalman.defineHistogram( 'TrkZLocal', path='EXPERT', type='TH1F', title='Track zLocal',xbins=100, xmin=-500000, xmax=500000 )
		monTool_AFP_BasicKalman.defineHistogram( 'TrkXSlope', path='EXPERT', type='TH1F', title='Track x slope',xbins=100, xmin=-0.5, xmax=0.5 )
		monTool_AFP_BasicKalman.defineHistogram( 'TrkYSlope', path='EXPERT', type='TH1F', title='Track y slope',xbins=100, xmin=-0.5, xmax=0.5 )
		monTool_AFP_BasicKalman.defineHistogram( 'TrkNClusters', path='EXPERT', type='TH1F', title='Track number of clusters',xbins=100, xmin=0, xmax=100 )
		monTool_AFP_BasicKalman.defineHistogram( 'TrkNHoles', path='EXPERT', type='TH1F', title='Track number of holes',xbins=100, xmin=0, xmax=100 )
		monTool_AFP_BasicKalman.defineHistogram( 'TrkChi2', path='EXPERT', type='TH1F', title='Track chi2',xbins=50, xmin=0, xmax=10 )

		AFP_SID.recoTool.RecoToolsList[i].MonTool = monTool_AFP_BasicKalman
			
	return AFP_SID

def AFP_LocReco_TD_HLT():
	
	AFP_TD = AFP_LocReco_TD_Cfg({"tracksContainerName": recordable("HLT_AFPToFTrackContainer")})
	
	return AFP_TD
	

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

