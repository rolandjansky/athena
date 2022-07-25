# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#==============================================================
# Job options file for the AFP_LocReco package
#==============================================================

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import Format
from IOVDbSvc.IOVDbSvcConfig import addFolders
from TrigEDMConfig.TriggerEDMRun3 import recordable

def AFP_LocReco_SiD_Cfg(flags, kwargs={}):

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
        acc = ComponentAccumulator()
        acc.addEventAlgo(CompFactory.AFP_SIDLocReco("AFP_SIDLocReco", recoTool = trackRecoTool))
        
        return acc


def AFP_LocReco_TD_Cfg(flags, kwargs={}):

        acc = ComponentAccumulator()
        
        acc.addCondAlgo(CompFactory.CondInputLoader(Load=[('CondAttrListCollection', 'ConditionStore+/FWD/AFP/ToFParameters/Local')]))
        if flags.Input.isMC:
                # if other tags are desired, they can be set also in postExec, e.g.
                # --postExec 'from IOVDbSvc.CondDB import conddb; conddb.addOverride("/FWD/AFP/ToFParameters/Local","AFPMCToFLoc-329484-02");'

                acc.merge(addFolders(flags, "/FWD/AFP/ToFParameters/Local<tag>AFPMCToFLoc-ideal-01</tag>", 'FWD_OFL', className='CondAttrListCollection', db='OFLP200'))
                tofLocParamTool  = CompFactory.getComp("AFP::ToFLocParamDBTool")("ToFLocParamDBTool", loc_param_key="/FWD/AFP/ToFParameters/Local")
        else:
                acc.merge(addFolders(flags, "/FWD/Onl/AFP/ToFParameters/Local<tag>AFPToFLoc-01</tag>", 'FWD_ONL', className='CondAttrListCollection', db='CONDBR2'))
                tofLocParamTool  = CompFactory.getComp("AFP::ToFLocParamDBTool")("ToFLocParamDBTool", loc_param_key="/FWD/Onl/AFP/ToFParameters/Local")
        
        basicTool0 = CompFactory.getComp("AFPTDBasicTool")("AFPTDBasicTool0", stationID=0, tofLocParamDBTool=tofLocParamTool, **kwargs)
        basicTool3 = CompFactory.getComp("AFPTDBasicTool")("AFPTDBasicTool3", stationID=3, tofLocParamDBTool=tofLocParamTool, **kwargs)
        
        if flags.Input.Format is Format.POOL:
                if "AFPToFHitContainer" not in flags.Input.Collections:
                        basicTool0.AFPToFHitContainerKey=""
                        basicTool3.AFPToFHitContainerKey=""
                else:
                        from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
                        acc.merge(SGInputLoaderCfg(flags, Load=[('xAOD::AFPToFHitContainer' , 'StoreGateSvc+AFPToFHitContainer')]))
                        
        basicToolsList=[basicTool0, basicTool3]

        # collect all output names and make a list with unique names for write handle keys; if this goes wrong AFP_TDLocRecoTool::initialize() will complain
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
        acc.addEventAlgo(CompFactory.AFP_TDLocReco("AFP_TDLocReco", recoTool=ToFtrackRecoTool))
        
        return acc


def AFP_LocReco_SiD_HLT(flags):
        from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
        acc = ComponentAccumulator()
        acc.merge(AFP_LocReco_SiD_Cfg(flags, {"tracksContainerName": recordable("HLT_AFPTrackContainer"), "AFPSiHitsClusterContainerKey": "HLT_AFPSiHitsClusterContainer"}))
        AFP_SID = acc.getEventAlgo("AFP_SIDLocReco")

        from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

        monTool_AFP_SIDLocRecoTool = GenericMonitoringTool("MonTool_AFP_SIDLocRecoTool")
        monTool_AFP_SIDLocRecoTool.defineHistogram( 'TrkSize', path='EXPERT', type='TH1F', title='AFP tracks size',xbins=50, xmin=0, xmax=50 )
        AFP_SID.recoTool.MonTool = monTool_AFP_SIDLocRecoTool
                
        for i, kalmanTool in enumerate(AFP_SID.recoTool.RecoToolsList):
               monTool_AFP_BasicKalman = GenericMonitoringTool("MonTool_AFP_"+kalmanTool.getName())

               monTool_AFP_BasicKalman.defineHistogram( 'TrkStationID', path='EXPERT', type='TH1F', title='Track station ID',xbins=4, xmin=0, xmax=4, cutmask='TrkMask' )
               monTool_AFP_BasicKalman.defineHistogram( 'TrkXLocal', path='EXPERT', type='TH1F', title='Track xLocal',xbins=100, xmin=-200, xmax=200, cutmask='TrkMask' )
               monTool_AFP_BasicKalman.defineHistogram( 'TrkYLocal', path='EXPERT', type='TH1F', title='Track yLocal',xbins=100, xmin=-30, xmax=30, cutmask='TrkMask' )
               monTool_AFP_BasicKalman.defineHistogram( 'TrkZLocal', path='EXPERT', type='TH1F', title='Track zLocal',xbins=100, xmin=-500000, xmax=500000, cutmask='TrkMask' )
               monTool_AFP_BasicKalman.defineHistogram( 'TrkXSlope', path='EXPERT', type='TH1F', title='Track x slope',xbins=100, xmin=-0.5, xmax=0.5, cutmask='TrkMask' )
               monTool_AFP_BasicKalman.defineHistogram( 'TrkYSlope', path='EXPERT', type='TH1F', title='Track y slope',xbins=100, xmin=-0.5, xmax=0.5, cutmask='TrkMask' )
               monTool_AFP_BasicKalman.defineHistogram( 'TrkNClusters', path='EXPERT', type='TH1F', title='Track number of clusters',xbins=100, xmin=0, xmax=100, cutmask='TrkMask' )
               monTool_AFP_BasicKalman.defineHistogram( 'TrkNHoles', path='EXPERT', type='TH1F', title='Track number of holes',xbins=100, xmin=0, xmax=100, cutmask='TrkMask' )
               monTool_AFP_BasicKalman.defineHistogram( 'TrkChi2', path='EXPERT', type='TH1F', title='Track chi2',xbins=50, xmin=0, xmax=10, cutmask='TrkMask' )

               AFP_SID.recoTool.RecoToolsList[i].MonTool = monTool_AFP_BasicKalman

        return acc

def AFP_LocReco_TD_HLT(flags):
        from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
        acc = ComponentAccumulator()
        
        from IOVDbSvc.CondDB import conddb
        if flags.Input.isMC:
                if not conddb.folderRequested('/FWD/AFP/ToFParameters/Local'):
                        conddb.addFolder("FWD_OFL","/FWD/AFP/ToFParameters/Local<tag>AFPMCToFLoc-ideal-01</tag>", className="CondAttrListCollection")
        else:
                if not conddb.folderRequested('/FWD/Onl/AFP/ToFParameters/Local'):
                        conddb.addFolder("FWD_ONL","/FWD/Onl/AFP/ToFParameters/Local<tag>AFPToFLoc-03</tag>", className="CondAttrListCollection")


        acc.merge(AFP_LocReco_TD_Cfg(flags, {"tracksContainerName": recordable("HLT_AFPToFTrackContainer")}))

        return acc

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

