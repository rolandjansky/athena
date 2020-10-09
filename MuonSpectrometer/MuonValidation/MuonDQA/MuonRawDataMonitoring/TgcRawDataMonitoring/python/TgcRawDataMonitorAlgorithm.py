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
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu6_idperf'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu14'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu20_mu8noL1;HLT_mu20'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu24_mu8noL1;HLT_mu24'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu50_L1MU20'
    tgcRawDataMonAlg.TagTrigList += ',HLT_mu60_0eta105_msonly_L1MU20'

    tgcRawDataMonAlg.TagAndProbe = False
    tgcRawDataMonAlg.TagAndProbeZmumu = False

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

    for side in ['A', 'C']:# side-A or side-C
        for station in range(1,5):# M1,2,3,4
            for s_or_w in ['S','W']:# strip or wire
                name = "%sM%02i%s" % (side,station,s_or_w)
                x_name = "lb_for_%s" % (name)
                y_name = name
                nbins = 10
                if station==1:
                    nbins = 648
                elif station==2 or station==3:
                    nbins = 528
                else: # station==4
                    nbins = 90

                myGroup.defineHistogram(x_name+','+y_name+';'+y_name+'_vs_lb',
                                        title=y_name+'_vs_lb;Luminosity block;Chamber index',type='TH2F',
                                        path=hitPath,xbins=100,xmin=-0.5,xmax=99.5,
                                        ybins=nbins,ymin=0.5,ymax=nbins+0.5,opt='kAddBinsDynamically')
                x_name = name
                y_name = "timing_for_%s" % (name)
                myGroup.defineHistogram(x_name+','+y_name+';'+y_name,
                                        title='Timing_for_'+name+';Chamber index;Timing',type='TH2F',
                                        path=hitPath,xbins=nbins,xmin=0.5,xmax=nbins+0.5,
                                        ybins=3,ymin=-1.5,ymax=1.5,ylabels=['Previous','Current','Next'])

                x_name = "x_%s" % (name)
                y_name = "y_%s" % (name)
                nbinsx = 10
                nbinsy = 10
                if station==1:
                    nbinsx = 15
                    nbinsy = 48
                elif station==2 or station==3:
                    nbinsx = 12
                    nbinsy = 48
                else: # station==4
                    nbinsx = 4
                    nbinsy = 24

                myGroup.defineHistogram(x_name+','+y_name+';'+name+'_vs_iEta',
                                        title=name+'_vs_iEta;iEta;Chamber index',type='TH2F',path=hitPath,
                                        xbins=nbinsx,xmin=0.5,xmax=nbinsx+0.5,
                                        ybins=nbinsy,ymin=0.5,ymax=nbinsy+0.5)


    coinPath = 'Coin/'
    myGroup.defineHistogram('coin_n;TgcCoin_nCoins',title='TgcPrd_nCoins;Number of coincidences;Number of events',
                            path=coinPath,xbins=100,xmin=0,xmax=1000,opt='kAddBinsDynamically')
    myGroup.defineHistogram('coin_bunch;TgcCoin_Timing',title='TgcCoin_Timing;Timing;Number of events',
                            path=coinPath,xbins=4,xmin=-1.5,xmax=1.5,xlabels=['Previous','Current','Next'])
    

    hitDetailsPath = 'HitDetails/'
    for side in ['A', 'C']:# side-A or side-C
        for sector in range(1,13):# Sector 01,,,12
            for station in range(1,5):# M1,2,3,4
                for phi in range(4):# internal phi 0,1,2,3
                    for eta in range(6):# eta index 1,,,5 for Endcap, and 0 for Forward
                        for lay in range(1,4):# sub-layer 1,2,3 (triplet) or 1,2 (doublet)
                            chamber_name = "%s%02dM%02df%02d%s%02dL%02d" % (side,sector,station,phi,'F' if eta==0 else 'E',eta,lay)
                            for s_or_w in ['S','W']:# strip or wire
                                nbins = 100
                                if s_or_w=="S":
                                    nbins = 32
                                else:
                                    if station==1:
                                        if eta==1:     nbins = 24
                                        elif eta==2:   nbins = 23
                                        elif eta==3:
                                            if lay==1: nbins = 61
                                            else:      nbins = 62
                                        elif eta==4:
                                            if lay==1: nbins = 92
                                            else:      nbins = 91
                                        else: # forward
                                            if lay==2: nbins = 104
                                            else:      nbins = 105
                                    elif station==2:
                                        if eta==1 or eta==2 or eta==3: nbins = 32
                                        elif eta==4:                   nbins = 103
                                        elif eta==5:                   nbins = 110
                                        else:                          nbins = 125 # forward
                                    elif station==3:
                                        if eta==1:   nbins = 31
                                        elif eta==2: nbins = 30
                                        elif eta==3: nbins = 32
                                        elif eta==4: nbins = 106
                                        elif eta==5: nbins = 96
                                        else:        nbins = 122
                                    else: # EI/FI
                                        if eta==1:   nbins = 24 # EI
                                        else:        nbins = 32 # FI

                                myGroup.defineHistogram('hits_on_'+chamber_name+s_or_w,
                                                        title='Hits_on_'+chamber_name+s_or_w+";Channel ID;Number of events",
                                                        path=hitDetailsPath,xbins=nbins,xmin=0.5,xmax=nbins+0.5)


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
