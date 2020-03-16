#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''
@file TgcRawDataMonitorAlgorithm.py
@author M.Aoki
@date 2019-10-03
@brief Python configuration for the Run III AthenaMonitoring package for TGC 
'''

def TgcRawDataMonitoringConfig(inputFlags):
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()
    # from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    # result.merge(MuonGeoModelCfg(inputFlags))
    # result.addPublicTool( CompFactory.Muon__MuonIdHelperTool() )
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    result.merge(MagneticFieldSvcCfg(inputFlags))

    from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    trackGeomCfg = TrackingGeometrySvcCfg(inputFlags)
    geom_svc = trackGeomCfg.getPrimary() 
    geom_svc.GeometryBuilder.Compactify = False ######## To avoid crash ########
    result.merge(trackGeomCfg)
    
    # from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    # result.merge(MagneticFieldSvcCfg(inputFlags))

    # from TrkConfig.TrackCollectionReadConfig import TrackCollectionReadCfg
    # result.merge( TrackCollectionReadCfg (inputFlags, 'CombinedInDetTracks') )

    # from TrkConfig.TrackCollectionReadConfig import TrackCollectionReadCfg
    # result.merge (TrackCollectionReadCfg (inputFlags, 'CombinedInDetTracks'))
    # result.merge (TrackCollectionReadCfg (inputFlags, 'Tracks'))
    # from PixelConditionsAlgorithms.PixelConditionsConfig import PixelConfigCondAlgCfg
    # result.merge(PixelConfigCondAlgCfg(inputFlags,
    #                                 UseCalibConditions=True,
    #                                 UseDeadmapConditions=True,
    #                                 UseDCSStateConditions=False,
    #                                 UseDCSStatusConditions=False,
    #                                 UseDCSHVConditions=True,
    #                                 UseDCSTemperatureConditions=True,
    #                                 UseTDAQConditions=False))
    # from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg
    # InDetPixelConditionsSummaryTool = result.popToolsAndMerge(PixelConditionsSummaryCfg(inputFlags))

    # from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    # trackGeomCfg = TrackingGeometrySvcCfg(inputFlags)
    # geom_svc = trackGeomCfg.getPrimary()
    # geom_svc.GeometryBuilder.Compactify = False ######## To avoid crash ########
    # result.merge(trackGeomCfg)
    # from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    # result.merge(MagneticFieldSvcCfg(inputFlags))

    # from TrkExTools.TrkExToolsConf import Trk__Navigator
    # AtlasNavigator = Trk__Navigator(name = 'AtlasNavigator')
    # AtlasNavigator.TrackingGeometrySvc = geom_svc
    # result.addPublicTool(AtlasNavigator)


    
    # from TrkExTools.TrkExToolsConf import Trk__Navigator
    # AtlasNavigator = Trk__Navigator(name = 'AtlasNavigator')
    # AtlasNavigator.TrackingGeometrySvc = geom_svc
    # result.addPublicTool(AtlasNavigator)
    # from TrigInDetConfig.InDetConfig import TrigInDetCondConfig
    # result.merge(TrigInDetCondConfig(inputFlags))

    # from PixelConditionsAlgorithms.PixelConditionsConfig import PixelConfigCondAlgCfg,PixelDetectorElementCondAlgCfg
    # result.merge( PixelConfigCondAlgCfg(inputFlags,name="PixelConfigCondAlg") )
    # result.merge( PixelDetectorElementCondAlgCfg(inputFlags,name="PixelDetectorElementCondAlg") )

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'TgcRawDataMonitorCfg')

    from AthenaConfiguration.ComponentFactory import CompFactory
    tgcRawDataMonAlg = helper.addAlgorithm(CompFactory.TgcRawDataMonitorAlgorithm,'TgcRawDataMonAlg')

    tgcRawDataMonAlg.TagAndProbe = True

    tgcRawDataMonAlg.TagTrigList = 'HLT_mu26_ivarmedium'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu26_ivarmedium'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu26_ivarmedium_L1MU20'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu6'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu6_L1MU6'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu20_mu8noL1;HLT_mu20'

    # Run 2 containers?
    if 'HLT_xAOD__MuonContainer_MuonEFInfo' in inputFlags.Input.Collections:
        tgcRawDataMonAlg.MuonEFContainerName='HLT_xAOD__MuonContainer_MuonEFInfo'
    
    mainDir = 'Muon/MuonRawDataMonitoring/TGC/'
    import math

    trigPath = 'Trig/'

    myGroup = helper.addGroup(tgcRawDataMonAlg,'TgcRawDataMonitor',mainDir)

    myGroup.defineHistogram('roi_thr;MuonRoI_Thresholds_RPC',title='MuonRoI Thresholds RPC;MuonRoI Threshold number;Number of events',
                            cutmask='roi_rpc',path=trigPath,xbins=20,xmin=-0.5,xmax=19.5)
    myGroup.defineHistogram('roi_thr;MuonRoI_Thresholds_TGC',title='MuonRoI Thresholds TGC;MuonRoI Threshold number;Number of events',
                            cutmask='roi_tgc',path=trigPath,xbins=20,xmin=-0.5,xmax=19.5)

    for n in range(16):
        myGroup.defineHistogram('roi_eta;MuonRoI_Eta_Thr'+str(n),title='MuonRoI Eta Thr'+str(n)+';MuonRoI Eta;Number of events',
                                cutmask='thrmask'+str(n),path=trigPath,xbins=60,xmin=-3,xmax=3)
        myGroup.defineHistogram('roi_phi_rpc;MuonRoI_Phi_RPC_Thr'+str(n),title='MuonRoI Phi RPC Thr'+str(n)+';MuonRoI Phi;Number of events',
                                cutmask='thrmask'+str(n),path=trigPath,xbins=32,xmin=-math.pi,xmax=math.pi)
        myGroup.defineHistogram('roi_phi_tgc;MuonRoI_Phi_TGC_Thr'+str(n),title='MuonRoI Phi TGC Thr'+str(n)+';MuonRoI Phi;Number of events',
                                cutmask='thrmask'+str(n),path=trigPath,xbins=48,xmin=-math.pi,xmax=math.pi)
        myGroup.defineHistogram('roi_eta,roi_phi;MuonRoI_EtaVsPhi_Thr'+str(n),type='TH2F',
                                title='MuonRoI Eta vs Phi Thr'+str(n)+';MuonRoI Eta;MuonRoI Phi',cutmask='thrmask'+str(n),path=trigPath,
                                xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
        myGroup.defineHistogram('muon_l1passThr'+str(n)+',muon_pt_rpc;MuonRoI_Eff_Pt_RPC_Thr'+str(n),title='MuonRoI_Eff_Pt_RPC_Thr'+str(n)+';Offline muon pT [GeV];Efficiency',
                                type='TEfficiency',path=trigPath,xbins=100,xmin=0,xmax=50)
        myGroup.defineHistogram('muon_l1passThr'+str(n)+',muon_pt_tgc;MuonRoI_Eff_Pt_TGC_Thr'+str(n),title='MuonRoI_Eff_Pt_TGC_Thr'+str(n)+';Offline muon pT [GeV];Efficiency',
                                type='TEfficiency',path=trigPath,xbins=100,xmin=0,xmax=50)
        
    # myGroup.defineHistogram('pT_passed,pT', type='TEfficiency', title='Test TEfficiency;x;Eff',
    #                         path='AndInTheDarkness', xbins=100, xmin=0.0, xmax=50.0)
    
    # myGroup.defineHistogram('eta;Test1',title='Hoge1;hoge2;hoge3',
    #                         path=trigPath,xbins=60,xmin=-3,xmax=3)

    # # myGroup.defineHistogram('eta,phi;Test2',title='Hoge1;hoge2;hoge3',type='TH2F',path=trigPath,
    # #                         xbins=100,xmin=-2.5,xmax=2.5,
    # #                         ybins=48,ymin=-math.pi,ymax=math.pi)

    # myGroup.defineHistogram('eta,pt;Test3',title='Hoge1;hoge2;hoge3',type='TH2F',path=trigPath,
    #                         xbins=100,xmin=-2.5,xmax=2.5,
    #                         ybins=10,ymin=0,ymax=100)

    # myGroup.defineHistogram('eta,phi;Test2',title='Hoge1;hoge2;hoge3',type='TH2F',path=trigPath,
    #                         xbins=100,xmin=-2.5,xmax=2.5,
    #                         ybins=10,ymin=0,ymax=100)


    # myGroup.defineHistogram('thrNumber_barrel',title='thrNumber_barrel;thrNumber;Events',
    #                         path=trigPath,xbins=16,xmin=0.0,xmax=16.0)
    # myGroup.defineHistogram('thrNumber_endcap',title='thrNumber_endcap;thrNumber;Events',
    #                         path=trigPath,xbins=16,xmin=0.0,xmax=16.0)
    # myGroup.defineHistogram('thrNumber_forward',title='thrNumber_forward;thrNumber;Events',
    #                         path=trigPath,xbins=16,xmin=0.0,xmax=16.0)

    # array = helper.addArray([16],tgcRawDataMonAlg,'TgcRawDataMonitor')
    # array.defineHistogram('roi_eta,roi_phi;MuonRoI_EtaVsPhi_Thr',title='MuonRoI_EtaVsPhi_Thr;roiEta;roiPhi',type='TH2F',path=mainDir+trigPath,
    #                       xbins=100,xmin=-2.5,xmax=2.5,
    #                       ybins=48,ymin=-math.pi,ymax=math.pi)
    
    # array.defineHistogram('roiEta',title='roiEta;roiEta;Events',type='TH1F',path=mainDir+trigPath,
    #                       xbins=100,xmin=-2.5,xmax=2.5)

    # array.defineHistogram('roiPhi_barrel_A',title='roiPhi_barrel_A;roiPhi;Events',type='TH1F',path=mainDir+trigPath,
    #                       xbins=16,xmin=-math.pi,xmax=math.pi)
    # array.defineHistogram('roiPhi_endcap_A',title='roiPhi_endcap_A;roiPhi;Events',type='TH1F',path=mainDir+trigPath,
    #                       xbins=48,xmin=-math.pi,xmax=math.pi)
    # array.defineHistogram('roiPhi_forward_A',title='roiPhi_forward_A;roiPhi;Events',type='TH1F',path=mainDir+trigPath,
    #                       xbins=24,xmin=-math.pi,xmax=math.pi)

    # array.defineHistogram('roiPhi_barrel_C',title='roiPhi_barrel_C;roiPhi;Events',type='TH1F',path=mainDir+trigPath,
    #                       xbins=16,xmin=-math.pi,xmax=math.pi)
    # array.defineHistogram('roiPhi_endcap_C',title='roiPhi_endcap_C;roiPhi;Events',type='TH1F',path=mainDir+trigPath,
    #                       xbins=48,xmin=-math.pi,xmax=math.pi)
    # array.defineHistogram('roiPhi_forward_C',title='roiPhi_forward_C;roiPhi;Events',type='TH1F',path=mainDir+trigPath,
    #                       xbins=24,xmin=-math.pi,xmax=math.pi)

    # array.defineHistogram('bcID_barrel',title='trig2bcid_barrel;BCID;Events',type='TH1F',path=mainDir+trigPath,
    #                       xbins=4000,xmin=0,xmax=4000)
    # array.defineHistogram('bcID_endcap',title='trig2bcid_endcap;BCID;Events',type='TH1F',path=mainDir+trigPath,
    #                       xbins=4000,xmin=0,xmax=4000)
    # array.defineHistogram('bcID_forward',title='trig2bcid_forward;BCID;Events',type='TH1F',path=mainDir+trigPath,
    #                       xbins=4000,xmin=0,xmax=4000)
    
    # array.defineHistogram('muEta,muPhi',title='muEta2Phi;muEta;muPhi',type='TH2F',path=mainDir+trigPath,
    #                       xbins=100,xmin=-2.5,xmax=2.5,
    #                       ybins=48,ymin=-math.pi,ymax=math.pi)

    # array.defineHistogram('muEta',title='muEta;muEta;Events',type='TH1F',path=mainDir+trigPath,
    #                       xbins=100,xmin=-2.5,xmax=2.5)

    # array.defineHistogram('muPt_barrel',title='muPt_barrel;muPt;Events',type='TH1F',path=mainDir+trigPath,
    #                       xbins=40,xmin=0,xmax=40)
    # array.defineHistogram('muPt_endcap',title='muPt_endcap;muPt;Events',type='TH1F',path=mainDir+trigPath,
    #                       xbins=40,xmin=0,xmax=40)
    # array.defineHistogram('muPt_forward',title='muPt_forward;muPt;Events',type='TH1F',path=mainDir+trigPath,
    #                       xbins=40,xmin=0,xmax=40)

    # array.defineHistogram('muPhi_barrel_A',title='muPhi_barrel_A;muPhi;Events',type='TH1F',path=mainDir+trigPath,
    #                       xbins=16,xmin=-math.pi,xmax=math.pi)
    # array.defineHistogram('muPhi_endcap_A',title='muPhi_endcap_A;muPhi;Events',type='TH1F',path=mainDir+trigPath,
    #                       xbins=48,xmin=-math.pi,xmax=math.pi)
    # array.defineHistogram('muPhi_forward_A',title='muPhi_forward_A;muPhi;Events',type='TH1F',path=mainDir+trigPath,
    #                       xbins=24,xmin=-math.pi,xmax=math.pi)

    # array.defineHistogram('muPhi_barrel_C',title='muPhi_barrel_C;muPhi;Events',type='TH1F',path=mainDir+trigPath,
    #                       xbins=16,xmin=-math.pi,xmax=math.pi)
    # array.defineHistogram('muPhi_endcap_C',title='muPhi_endcap_C;muPhi;Events',type='TH1F',path=mainDir+trigPath,
    #                       xbins=48,xmin=-math.pi,xmax=math.pi)
    # array.defineHistogram('muPhi_forward_C',title='muPhi_forward_C;muPhi;Events',type='TH1F',path=mainDir+trigPath,
    #                       xbins=24,xmin=-math.pi,xmax=math.pi)

    #acc, seq = helper.result()
    acc = helper.result()
    result.merge(acc)
    return result
    # return helper.result()
    
if __name__=='__main__':
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    # nightly = '/afs/cern.ch/user/e/ebergeas/work/public/triggermonitoring/nigtly_2019-08-24T2130/'
    # file = 'AOD.pool.root'
    # ConfigFlags.Input.Files = [nightly+file]
    import glob
    # inputs = glob.glob('/scratch/masato/L1MuonDevRun3/21.3-rerunLVL1/run_data_eb_nocos/*/AOD.pool.root')
    # inputs = glob.glob('/data03/data/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.AOD.e4993_s3214_d1540_r11773_tid20245298_00//*')
    # inputs = glob.glob('/data02/data/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.recon.AOD.e4993_s3214_d1546_r11767/AOD.20152803._000173.pool.root.1')
    # inputs = glob.glob('/data01/masato/L1MuonDevRun3/athenaMT/run_mc_zmumu_normal/*/tmp.ESD')
    inputs = glob.glob('/data01/masato/L1MuonDevRun3/athenaMT/run_mc_zmumu_normal/*/AOD*')
    # inputs = glob.glob('/data01/masato/L1MuonDevRun3/athenaMT/mc_scan_normal/*/tmp.ESD')
    ConfigFlags.Input.Files = inputs
    ConfigFlags.Input.isMC = True
    ConfigFlags.Output.HISTFileName = 'ExampleMonitorOutput.root'
    # ConfigFlags.Output.HISTFileName = 'ExampleMonitorOutput_e4993_s3214_d1540_r11773.root'
    # ConfigFlags.Output.HISTFileName = 'ExampleMonitorOutput_e4993_s3214_d1546_r11767.root'

    #ConfigFlags.IOVDb.GlobalTag = "'CONDBR2-BLKPA-2017-08"
    ConfigFlags.GeoModel.AtlasVersion = "ATLAS-R2-2016-01-00-01"

    # from AthenaCommon.Include import Include, IncludeError, include
    # include("RecExCommon/RecExCommon_topOptions.py")


    ConfigFlags.lock()
    ConfigFlags.dump()

    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr.Dump = False

    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesSerialCfg()
    cfg.merge(PoolReadCfg(ConfigFlags))

    # Configuration of the ATLAS Geo Model
    from AtlasGeoModel.AtlasGeoModelConfig import AtlasGeometryCfg
    geoCfg = AtlasGeometryCfg(ConfigFlags)
    cfg.merge(geoCfg)
    
    tgcRawDataMonitorAcc = TgcRawDataMonitoringConfig(ConfigFlags)
    tgcRawDataMonitorAcc.OutputLevel = DEBUG
    cfg.merge(tgcRawDataMonitorAcc)

    # cfg.printConfig(withDetails=False)
    cfg.printConfig(withDetails=True, summariseProps = True)

    cfg.run()
