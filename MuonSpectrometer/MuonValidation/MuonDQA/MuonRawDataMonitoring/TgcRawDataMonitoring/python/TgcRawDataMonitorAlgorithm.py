#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'TgcRawDataMonitorCfg')

    tgcRawDataMonAlg = helper.addAlgorithm(CompFactory.TgcRawDataMonitorAlgorithm,'TgcRawDataMonAlg')

    tgcRawDataMonAlg.TagTrigList = 'HLT_mu26_ivarmedium'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu26_ivarmedium'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu26_ivarmedium_L1MU20'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu6'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu6_L1MU6'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu20_mu8noL1;HLT_mu20'

    if not inputFlags.DQ.triggerDataAvailable:
        tgcRawDataMonAlg.MuonRoIContainerName = ''

    if 'HLT_xAOD__MuonContainer_MuonEFInfo' in inputFlags.Input.Collections:
        tgcRawDataMonAlg.MuonEFContainerName='HLT_xAOD__MuonContainer_MuonEFInfo'
    if 'TGC_MeasurementsAllBCs' in inputFlags.Input.Collections:
        tgcRawDataMonAlg.AnaTgcPrd=True
    
    mainDir = 'Muon/MuonRawDataMonitoring/TGC/'
    import math

    trigPath = 'Trig/'

    myGroup = helper.addGroup(tgcRawDataMonAlg,'TgcRawDataMonitor',mainDir)

    myGroup.defineHistogram('roi_thr;MuonRoI_Thresholds_RPC',title='MuonRoI Thresholds RPC;MuonRoI Threshold number;Number of events',
                            cutmask='roi_rpc',path=trigPath,xbins=20,xmin=-0.5,xmax=19.5)
    myGroup.defineHistogram('roi_thr;MuonRoI_Thresholds_TGC',title='MuonRoI Thresholds TGC;MuonRoI Threshold number;Number of events',
                            cutmask='roi_tgc',path=trigPath,xbins=20,xmin=-0.5,xmax=19.5)

    myGroup.defineHistogram('lb,roi_phi_barrel;MuonRoI_PhiVsLB_Barrel_sideA',title='MuonRoI PhiVsLB Barrel sideA;Luminosity block;Trigger sector',type='TH2F',
                            cutmask='roi_sideA',path=trigPath,xbins=100,xmin=-0.5,xmax=99.5,ybins=8,ymin=-math.pi,ymax=math.pi,opt='kAddBinsDynamically')
    myGroup.defineHistogram('lb,roi_phi_barrel;MuonRoI_PhiVsLB_Barrel_sideC',title='MuonRoI PhiVsLB Barrel sideC;Luminosity block;Trigger sector',type='TH2F',
                            cutmask='roi_sideC',path=trigPath,xbins=100,xmin=-0.5,xmax=99.5,ybins=8,ymin=-math.pi,ymax=math.pi,opt='kAddBinsDynamically')

    myGroup.defineHistogram('lb,roi_phi_endcap;MuonRoI_PhiVsLB_Endcap_sideA',title='MuonRoI PhiVsLB Endcap sideA;Luminosity block;Trigger sector',type='TH2F',
                            cutmask='roi_sideA',path=trigPath,xbins=100,xmin=-0.5,xmax=99.5,ybins=48,ymin=-math.pi,ymax=math.pi,opt='kAddBinsDynamically')
    myGroup.defineHistogram('lb,roi_phi_endcap;MuonRoI_PhiVsLB_Endcap_sideC',title='MuonRoI PhiVsLB Endcap sideC;Luminosity block;Trigger sector',type='TH2F',
                            cutmask='roi_sideC',path=trigPath,xbins=100,xmin=-0.5,xmax=99.5,ybins=48,ymin=-math.pi,ymax=math.pi,opt='kAddBinsDynamically')

    myGroup.defineHistogram('lb,roi_phi_forward;MuonRoI_PhiVsLB_Forward_sideA',title='MuonRoI PhiVsLB Forward sideA;Luminosity block;Trigger sector',type='TH2F',
                            cutmask='roi_sideA',path=trigPath,xbins=100,xmin=-0.5,xmax=99.5,ybins=24,ymin=-math.pi,ymax=math.pi,opt='kAddBinsDynamically')
    myGroup.defineHistogram('lb,roi_phi_forward;MuonRoI_PhiVsLB_Forward_sideC',title='MuonRoI PhiVsLB Forward sideC;Luminosity block;Trigger sector',type='TH2F',
                            cutmask='roi_sideC',path=trigPath,xbins=100,xmin=-0.5,xmax=99.5,ybins=24,ymin=-math.pi,ymax=math.pi,opt='kAddBinsDynamically')
    
    for n in range(1,16):
        myGroup.defineHistogram('roi_eta;MuonRoI_Eta_Thr'+str(n),title='MuonRoI Eta Thr'+str(n)+';MuonRoI Eta;Number of events',
                                cutmask='thrmask'+str(n),path=trigPath,xbins=100,xmin=-2.5,xmax=2.5)
        myGroup.defineHistogram('roi_phi_rpc;MuonRoI_Phi_RPC_Thr'+str(n),title='MuonRoI Phi RPC Thr'+str(n)+';MuonRoI Phi;Number of events',
                                cutmask='thrmask'+str(n),path=trigPath,xbins=32,xmin=-math.pi,xmax=math.pi)
        myGroup.defineHistogram('roi_phi_tgc;MuonRoI_Phi_TGC_Thr'+str(n),title='MuonRoI Phi TGC Thr'+str(n)+';MuonRoI Phi;Number of events',
                                cutmask='thrmask'+str(n),path=trigPath,xbins=48,xmin=-math.pi,xmax=math.pi)
        myGroup.defineHistogram('roi_eta,roi_phi;MuonRoI_EtaVsPhi_Thr'+str(n),type='TH2F',
                                title='MuonRoI Eta vs Phi Thr'+str(n)+';MuonRoI Eta;MuonRoI Phi',cutmask='thrmask'+str(n),path=trigPath,
                                xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
        myGroup.defineHistogram('muon_l1passThr'+str(n)+',muon_pt_rpc;MuonRoI_Eff_Pt_RPC_Thr'+str(n),title='MuonRoI_Eff_Pt_RPC_Thr'+str(n)+';Offline muon pT [GeV];Efficiency',
                                type='TEfficiency',path=trigPath,xbins=50,xmin=0,xmax=50)
        myGroup.defineHistogram('muon_l1passThr'+str(n)+',muon_pt_tgc;MuonRoI_Eff_Pt_TGC_Thr'+str(n),title='MuonRoI_Eff_Pt_TGC_Thr'+str(n)+';Offline muon pT [GeV];Efficiency',
                                type='TEfficiency',path=trigPath,xbins=50,xmin=0,xmax=50)
        myGroup.defineHistogram('muon_l1passThr'+str(n)+',muon_phi_rpc;MuonRoI_Eff_Phi_RPC_Thr'+str(n),title='MuonRoI_Eff_Phi_RPC_Thr'+str(n)+';Offline muon phi [rad.];Efficiency',
                                type='TEfficiency',path=trigPath,xbins=32,xmin=-math.pi,xmax=math.pi)
        myGroup.defineHistogram('muon_l1passThr'+str(n)+',muon_phi_tgc;MuonRoI_Eff_Phi_TGC_Thr'+str(n),title='MuonRoI_Eff_Phi_TGC_Thr'+str(n)+';Offline muon phi [rad.];Efficiency',
                                type='TEfficiency',path=trigPath,xbins=48,xmin=-math.pi,xmax=math.pi)
        myGroup.defineHistogram('muon_l1passThr'+str(n)+',muon_eta;MuonRoI_Eff_Eta_Thr'+str(n),title='MuonRoI_Eff_Eta_Thr'+str(n)+';Offline muon eta;Efficiency',
                                type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5)
        myGroup.defineHistogram('muon_l1passThr'+str(n)+',muon_eta,muon_phi;MuonRoI_Eff_EtaVsPhi_Thr'+str(n),title='MuonRoI_Eff_EtaVsPhi_Thr'+str(n)+';Offline muon eta; Offline muon phi',
                                type='TEfficiency',path=trigPath,xbins=100,xmin=-2.5,xmax=2.5,ybins=48,ymin=-math.pi,ymax=math.pi)
        

    hitPath = 'Hit/'
    myGroup.defineHistogram('hit_n;TgcPrd_nHits',title='TgcPrd_nHits;Number of hits;Number of events',
                            path=hitPath,xbins=100,xmin=0,xmax=1000,opt='kAddBinsDynamically')
    myGroup.defineHistogram('hit_bunch;TgcPrd_Timing',title='TgcPrd_Timing;Timing;Number of events',
                            path=hitPath,xbins=4,xmin=-1.5,xmax=1.5,xlabels=['Previous','Current','Next'])

    coinPath = 'Coin/'
    myGroup.defineHistogram('coin_n;TgcCoin_nCoins',title='TgcPrd_nCoins;Number of coincidences;Number of events',
                            path=coinPath,xbins=100,xmin=0,xmax=1000,opt='kAddBinsDynamically')
    myGroup.defineHistogram('coin_bunch;TgcCoin_Timing',title='TgcCoin_Timing;Timing;Number of events',
                            path=coinPath,xbins=4,xmin=-1.5,xmax=1.5,xlabels=['Previous','Current','Next'])
    
    acc = helper.result()
    result.merge(acc)
    return result
    
if __name__=='__main__':
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO,DEBUG
    log.setLevel(INFO)

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    import glob

    inputs = glob.glob('/data03/mySamples/run_mc_zmumu/*/tmp.ESD')

    ConfigFlags.Input.Files = inputs
    ConfigFlags.Input.isMC = True
    ConfigFlags.Output.HISTFileName = 'ExampleMonitorOutput.root'

    ConfigFlags.GeoModel.AtlasVersion = "ATLAS-R2-2016-01-00-01"

    ConfigFlags.lock()
    ConfigFlags.dump()

    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr.Dump = False

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    tgcRawDataMonitorAcc = TgcRawDataMonitoringConfig(ConfigFlags)
    tgcRawDataMonitorAcc.OutputLevel = DEBUG
    cfg.merge(tgcRawDataMonitorAcc)

    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    from AtlasGeoModel.AtlasGeoModelConfig import AtlasGeometryCfg
    from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    cfg.merge(MagneticFieldSvcCfg(ConfigFlags))
    cfg.merge(AtlasGeometryCfg(ConfigFlags))
    cfg.merge(TrackingGeometrySvcCfg(ConfigFlags))

    cfg.printConfig(withDetails=True, summariseProps = True)

    cfg.run()
