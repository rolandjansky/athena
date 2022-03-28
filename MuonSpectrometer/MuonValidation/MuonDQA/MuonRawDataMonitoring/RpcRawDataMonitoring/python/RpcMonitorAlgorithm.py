#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

'''
@file RPCMonitoringConfig.py
@brief Python configuration of RPC Monitoring for the Run III
'''

def RpcMonitoringConfig(inputFlags):

    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    from AtlasGeoModel.AtlasGeoModelConfig import AtlasGeometryCfg
    result.merge(MagneticFieldSvcCfg(inputFlags))
    result.merge(AtlasGeometryCfg(inputFlags))
    
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'RpcMonitoringCfg')

    ######################################################################################################
    ## RpcTrackAnaAlgAlg
    ######################################################################################################
    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    extrapolator = result.popToolsAndMerge(AtlasExtrapolatorCfg(inputFlags))

    rpcTrackAnaAlg = helper.addAlgorithm(CompFactory.RpcTrackAnaAlg, "RpcTrackAnaAlgAlg", TrackExtrapolator = extrapolator)

    from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import TrackingGeometryCondAlgCfg
    result.merge( TrackingGeometryCondAlgCfg(inputFlags ) )

    rpcTrackAnaAlg.plotMuonEff = True
    rpcTrackAnaAlg.plotPRD     = True

    # rpcTrackAnaAlg.TagTrigList = 'HLT_mu26_ivarmedium'
    rpcTrackAnaAlg.TagAndProbe         = False
    # rpcTrackAnaAlg.TagAndProbeZmumu    = False

    if not inputFlags.DQ.triggerDataAvailable:
        rpcTrackAnaAlg.MuonRoIContainerName = ''

    ######################################################################################################
    ## Occupancy histograms
    ######################################################################################################
    myGroup_track = helper.addGroup(rpcTrackAnaAlg, 'RpcTrackAnaAlg', 'Muon/MuonRawDataMonitoring/RPC/')

    myGroup_track.defineHistogram('run;Run',
                            title='Run Number;run;Events',
                            type='TH1I', 
                            path='RpcOccupancy',
                            xbins=800000,xmin=200000.5,xmax=1000000.5)

    myGroup_track.defineHistogram('evtLB',
                        title='Number of Event;Luminosity Block;N Event',
                        type='TH1I', 
                        path='RpcOccupancy',
                        xbins=1200, xmin=0.5, xmax=1200.5)

    myGroup_track.defineHistogram('prdTime', 
                        title="Number of RPC Prepare Data;Time;N RPC Prepare Data",
                        type='TH1D', 
                        path='RpcOccupancy',
                        xbins=67, xmin=-104.6875, xmax=104.6875)

    myGroup_track.defineHistogram('prd_sec,prd_layer;NPRDHit_sectorVSlayer', 
                        title="NPRDHit_sectorVSlayer;Sector;layer((dbR-1)*2+gasGap);NHit",
                        type='TH2I', 
                        path='RpcOccupancy',
                        xbins=33, xmin=-16.5, xmax=16.5, 
                        ybins=8, ymin=0.5, ymax=8.5)
    myGroup_track.defineHistogram('prd_sec_1214,prd_layer_1214;NPRDHit_sectorVSlayer_Sector1214', 
                        title="NPRDHit_sectorVSlayer_Sector1214;Sector;layer((dbR-1)*2+gasGap);NHit",
                        type='TH2I', 
                        path='RpcOccupancy',
                        xbins=[-14.5,-13.5,-12.5,-11.5, 11.5, 12.5, 13.5, 14.5],
                        ybins=8, ymin=0.5, ymax=8.5)
    myGroup_track.defineHistogram('prd_sec_eta,prd_layer_eta;NPRDHit_sectorVSlayer_Eta', 
                        title="NPRDHit_sectorVSlayer_eta;Sector;layer((dbR-1)*2+gasGap);NHit",
                        type='TH2I', 
                        path='RpcOccupancy',
                        xbins=33, xmin=-16.5, xmax=16.5, 
                        ybins=8, ymin=0.5, ymax=8.5)
    myGroup_track.defineHistogram('prd_sec_phi,prd_layer_phi;NPRDHit_sectorVSlayer_Phi', 
                        title="NPRDHit_sectorVSlayer_phi;Sector;layer((dbR-1)*2+gasGap);NHit",
                        type='TH2I', 
                        path='RpcOccupancy',
                        xbins=33, xmin=-16.5, xmax=16.5, 
                        ybins=8, ymin=0.5, ymax=8.5)

    myGroup_track.defineHistogram('StationName,panelInd;NPRDHit_stationName_vs_panelIndex', 
                title='StationName_vs_panelIndex;StationName;Panel Index;NHit',
                type='TH2I', 
                path='RpcOccupancy',
                xbins=53, xmin=0.5, xmax=53.5, ybins=8592, ymin=-0.5, ymax=8591.5)
    myGroup_track.defineHistogram('StationEta,panelInd;NPRDHit_stationEta_vs_panelIndex', 
                title='StationEta_vs_panelIndex;StationEta;Panel Index;NHit',
                type='TH2I', 
                path='RpcOccupancy',
                xbins=17, xmin=-8.5, xmax=8.5, ybins=8592, ymin=-0.5, ymax=8591.5)
    myGroup_track.defineHistogram('StationPhi,panelInd;NPRDHit_stationPhi_vs_panelIndex', 
                title='StationPhi_vs_panelIndex;StationPhi;Panel Index;NHit',
                type='TH2I', 
                path='RpcOccupancy',
                xbins=8, xmin=0.5, xmax=8.5, ybins=8592, ymin=-0.5, ymax=8591.5)
    myGroup_track.defineHistogram('DoubletR,panelInd;NPRDHit_doubletR_vs_panelIndex', 
                title='DoubletR_vs_panelIndex;DoubletR;Panel Index;NHit',
                type='TH2I', 
                path='RpcOccupancy',
                xbins=2, xmin=0.5, xmax=2.5, ybins=8592, ymin=-0.5, ymax=8591.5)
    myGroup_track.defineHistogram('DoubletZ,panelInd;NPRDHit_doubletZ_vs_panelIndex', 
                title='DoubletZ_vs_panelIndex;DoubletZ;Panel Index;NHit',
                type='TH2I', 
                path='RpcOccupancy',
                xbins=3, xmin=0.5, xmax=3.5, ybins=8592, ymin=-0.5, ymax=8591.5)
    myGroup_track.defineHistogram('DoubletPhi,panelInd;NPRDHit_doubletPhi_vs_panelIndex', 
                title='DoubletPhi_vs_panelIndex;DoubletPhi;Panel Index;NHit',
                type='TH2I', 
                path='RpcOccupancy',
                xbins=2, xmin=0.5, xmax=2.5, ybins=8592, ymin=-0.5, ymax=8591.5)
    myGroup_track.defineHistogram('GasGap,panelInd;NPRDHit_gasgap_vs_panelIndex', 
                title='GasGap_vs_panelIndex;GasGap;Panel Index;NHit',
                type='TH2I', 
                path='RpcOccupancy',
                xbins=2, xmin=0.5, xmax=2.5, ybins=8592, ymin=-0.5, ymax=8591.5)
    myGroup_track.defineHistogram('MeasPhi,panelInd;NPRDHit_measPhi_vs_panelIndex', 
                title='MeasPhi_vs_panelIndex;MeasPhi;Panel Index;NHit',
                type='TH2I', 
                path='RpcOccupancy',
                xbins=2, xmin=-0.5, xmax=1.5, ybins=8592, ymin=-0.5, ymax=8591.5)

    myGroup_track.defineHistogram('LB,panelInd;NPRDHit_Panels_All', 
                title='Number of RPC Prepare Data;Luminosity Block;Panel Index;NHit',
                type='TH2I', 
                path='RpcOccupancy',
                xbins=1200, xmin=0.5, xmax=1200.5, ybins=8592, ymin=-0.5, ymax=8591.5)
    myGroup_track.defineHistogram('LB;NPRDHitVSLB_All', 
                title="Number of RPC Prepare Data;Luminosity Block;NHit",
                type='TH1I', 
                path='RpcOccupancy',
                xbins=1200, xmin=0.5, xmax=1200.5)

    ######################################################################################################
    ## Rpc Track Analysis
    ######################################################################################################
    trackPath = 'TrackMatch'
    myGroup_track.defineHistogram('hitMultiplicity_eta;HitMultiplicity_eta', 
                            type='TH1I', 
                            title='Hit Multiplicity_eta All Panel;#eta strip hit Multiplicity;muon entries',
                            path=trackPath,
                            xbins=11,xmin=-0.5,   xmax=10.5)

    myGroup_track.defineHistogram('hitMultiplicity_phi;HitMultiplicity_phi', 
                            type='TH1I', 
                            title='Hit Multiplicity_phi All Panel;#phi strip hit Multiplicity;muon entries',
                            path=trackPath,
                            xbins=11,xmin=-0.5,   xmax=10.5)

    myGroup_track.defineHistogram('hitMultiplicity,panelInd_hM;HitMultiplicity_Panels', 
                            title='Hit Multiplicity;Hit Multiplicity;Panel Index;NMuon',
                            type='TH2I', 
                            path=trackPath,
                            xbins=11, xmin=-0.5, xmax=10.5, ybins=8592, ymin=-0.5, ymax=8591.5)

    myGroup_track.defineHistogram('clustMultiplicity_eta;ClusterSize_etaView', 
                            type='TH1I', 
                            title='Cluster size(#eta view);Cluster size;NCluster',
                            path=trackPath,
                            xbins=11,xmin=-0.5,   xmax=10.5)

    myGroup_track.defineHistogram('clustMultiplicity_phi;ClusterSize_phiView', 
                            type='TH1I', 
                            title='Cluster size(#phi view);Cluster size;NCluster',
                            path=trackPath,
                            xbins=11,xmin=-0.5,   xmax=10.5)

    myGroup_track.defineHistogram('clustMultiplicity,panelInd_clust;ClusterSize_Panels', 
                            title='Cluste Multiplicity;Cluste Multiplicity;Panel Index;NCluster',
                            type='TH2I', 
                            path=trackPath,
                            xbins=11, xmin=-0.5, xmax=10.5, ybins=8592, ymin=-0.5, ymax=8591.5)

    myGroup_track.defineHistogram('muon_passExtrap,panelInd_hM;Panel_Efficiency', 
                            title='Panels Detection Efficiency;Panel Index;Efficiency',
                            type='TEfficiency',
                            path=trackPath,
                            xbins=8592, xmin=-0.5, xmax=8591.5)

    myGroup_track.defineHistogram('detPar_localY_allPanel,panelInd_detpar;DetPar_localY_allPanel', 
                            type='TH2F', 
                            title='Detector Parameter localY;Local Y;panelID;N matched Gasgap',
                            path=trackPath,
                            xbins=270,xmin=-2700., xmax=+2700., ybins=8592, ymin=-0.5, ymax=8591.5)

    myGroup_track.defineHistogram('detPar_localZ_allPanel,panelInd_detpar;DetPar_localZ_allPanel', 
                            type='TH2F', 
                            title='Detector Parameter localZ;Local Z;panelID;N matched Gasgap',
                            path=trackPath,
                            xbins=140,xmin=-700., xmax=700., ybins=8592, ymin=-0.5, ymax=8591.5)

    myGroup_track.defineHistogram('detPar_globalX_allPanel,panelInd_detpar;DetPar_globalX_allPanel', 
                            type='TH2F', 
                            title='Detector Parameter globalX;Global X;panelID;N matched Gasgap',
                            path=trackPath,
                            xbins=300,xmin=-15000., xmax=15000., ybins=8592, ymin=-0.5, ymax=8591.5)

    myGroup_track.defineHistogram('detPar_globalY_allPanel,panelInd_detpar;DetPar_globalY_allPanel', 
                            type='TH2F', 
                            title='Detector Parameter globalY;Global Y;panelID;N matched Gasgap',
                            path=trackPath,
                            xbins=300,xmin=-15000., xmax=15000., ybins=8592, ymin=-0.5, ymax=8591.5)

    myGroup_track.defineHistogram('detPar_globalR_allPanel,panelInd_detpar;DetPar_globalR_allPanel', 
                            type='TH2F', 
                            title='Detector Parameter globalR;Global R;panelID;N matched Gasgap',
                            path=trackPath,
                            xbins=220,xmin=-22000., xmax=22000., ybins=8592, ymin=-0.5, ymax=8591.5)

    myGroup_track.defineHistogram('detPar_globalZ_allPanel,panelInd_detpar;DetPar_globalZ_allPanel', 
                            type='TH2F', 
                            title='Detector Parameter globalZ;Global Z;panelID;N matched Gasgap',
                            path=trackPath,
                            xbins=300,xmin=-15000., xmax=15000., ybins=8592, ymin=-0.5, ymax=8591.5)

    myGroup_track.defineHistogram('dR_TrackGasGap_allPanel,panelInd_detpar;DR_TrackGasGap_allPanel', 
                            type='TH2F', 
                            title='DR between track and gasgap on panel;#Delta R_trackAndgasgap;panelID;N matched Gasgap',
                            path=trackPath,
                            xbins=50,xmin=0., xmax=1., ybins=8592, ymin=-0.5, ymax=8591.5)

    myGroup_track.defineHistogram('isOutTime_prd,panelInd_prd;OuttimeHitFraction_PRDHit', 
                            title='Outtime Hit Fraction of PRD Hit;Panel Index;Outtime Hit Fraction',
                            type='TEfficiency',
                            path=trackPath,
                            xbins=8592, xmin=-0.5, xmax=8591.5) 

    myGroup_track.defineHistogram('isOutTime_prd_onTrack,panelInd_prd_onTrack;OuttimeHitFraction_PRDHit_onTrack',
                            title='Outtime Hit Fraction of PRD Hit on Muon Track;Panel Index;Outtime Hit Fraction',
                            type='TEfficiency',
                            path=trackPath,
                            xbins=8592, xmin=-0.5, xmax=8591.5)

    # myGroup_track.defineHistogram('prdHit_time;PrdHit_time', 
    #                         title='PRD Hit Time;Hit Time;NHit',
    #                         type='TH1F',
    #                         path=trackPath,
    #                         xbins=64, xmin=-100., xmax=100.)

    # myGroup_track.defineHistogram('prdHit_time,panelInd_prd;PrdHit_time_perPanel', 
    #                         type='TH2F',
    #                         title='PRD Hit Time;PRD Hit Time;panelID;NHit',
    #                         path=trackPath,
    #                         xbins=64, xmin=-100., xmax=100., ybins=8592, ymin=-0.5, ymax=8591.5)


    ######################################################################################################
    ## Rpc lv1 Analysis
    ######################################################################################################
    RPCLv1AnaAlg    = CompFactory.RPCLv1AnaAlg

    Lv1AnaAlg  = helper.addAlgorithm(RPCLv1AnaAlg, "RPCLv1AnaAlgAlg")
    # Lv1AnaAlg.TriggerChain  = 'HLT_mu26_ivarmedium'
    
    if not inputFlags.DQ.triggerDataAvailable:
        Lv1AnaAlg.MuonRoIContainerName = ''

    myGroup_lv1Trigger = helper.addGroup(Lv1AnaAlg, 'RPCLv1AnaAlg', 'Muon/MuonRawDataMonitoring/RPC/')

    myGroup_lv1Trigger.defineHistogram('nMu;NMuon',
                            title='Number of Muons;nMuons;Events',
                            type='TH1I',
                            path='PlotCand',
                            xbins=10,xmin=-0.5,xmax=9.5)
    myGroup_lv1Trigger.defineHistogram('nMuBarrel;NMuonBarrel',
                            title='Number of Barrel Muons;nMuons;Events',
                            type='TH1I',
                            path='PlotCand',
                            xbins=5,xmin=-0.5,xmax=4.5)

    myGroup_lv1Trigger.defineHistogram('muPt_full;MuonPt_full',
                            title='barrel and endcap muon Pt;Pt[MeV];NMuon',
                            type='TH1D',
                            path='PlotCand',
                            xbins=200,xmin=0,xmax=1000e3)

    myGroup_lv1Trigger.defineHistogram('roiEta;roiEta',
                            title='roi eta;roi #eta;rois',
                            type='TH1D',
                            path='PlotCand',
                            xbins=50,xmin=-2.5,xmax=2.5)

    myGroup_lv1Trigger.defineHistogram('roiBarrelEta;roiBarrelEta',
                            title='Barrel roi eta;roi #eta;rois',
                            type='TH1D',
                            path='PlotCand',
                            xbins=50,xmin=-2.5,xmax=2.5)

    myGroup_lv1Trigger.defineHistogram('roiBarrelThr;roiBarrelThrs',
                            title='Barrel roi threshold;roi threshold;rois',
                            type='TH1I',
                            path='PlotCand',
                            xbins=6,xmin=0.5,xmax=6.5)
    
    myGroup_lv1Trigger.defineHistogram('nMuBarrel_medium;NMuonBarrel_medium',
                            title='Number of Barrel Medium Muons;nMuons;Events',
                            type='TH1I',
                            path='L1Trigger',
                            xbins=5,xmin=-0.5,xmax=4.5)

    myGroup_lv1Trigger.defineHistogram('muPtDen;MuonPt',
                            title='Barrel Muon Pt;Pt[MeV];NMuon',
                            type='TH1D',
                            path='L1Trigger',
                            xbins=200,xmin=0,xmax=1000e3)

    myGroup_lv1Trigger.defineHistogram('muEtaDen,muPhiDen;L1TriggerEffDen', 
                            type='TH2D', 
                            title='L1 Trigger Efficiency Denominator;#eta;#phi;NMuon',
                            path='L1Trigger',
                            xbins=42,xmin=-1.05,     xmax=1.05,
                            ybins=32,ymin=-3.1415926,ymax=3.1415926)

    lv1Triggers = [str(k) for k in range(1, 6+1)]
    array_triggerThr = helper.addArray([lv1Triggers], Lv1AnaAlg, 'RPCLv1AnaAlg', 'Muon/MuonRawDataMonitoring/RPC')

    array_triggerThr.defineHistogram('passTrigger,muPt;L1TriggerEff_muPt',
                title='L1 Trigger Threshold{0} Efficiency;Pt[MeV];#epsilon Thr{0}',
                type='TEfficiency',
                path='L1Trigger',
                xbins=10, xmin=0.0, xmax=80.0e3)

    array_triggerThr.defineHistogram('passTrigger,muEta;L1TriggerEff_muEta',
                title='L1 Trigger Threshold{0} Efficiency;#eta;#epsilon Thr{0}',
                type='TEfficiency',
                path='L1Trigger',
                xbins=42,xmin=-1.05, xmax=1.05)

    array_triggerThr.defineHistogram('passTrigger,muPhi;L1TriggerEff_muPhi',
                title='L1 Trigger Threshold{0} Efficiency;#phi;#epsilon Thr{0}',
                type='TEfficiency',
                path='L1Trigger',
                xbins=32,xmin=-3.1415926,xmax=3.1415926)

    array_triggerThr.defineHistogram('muEta,muPhi;L1TriggerEffNum', 
                type='TH2D', 
                title='L1 Trigger Efficiency numerator;#eta;#phi;NMuon Thr{0}',
                path='L1Trigger',
                xbins=42,xmin=-1.05,     xmax=1.05,
                ybins=32,ymin=-3.1415926,ymax=3.1415926)

    array_triggerThr.defineHistogram('passTrigger,muEta,muPhi;L1TriggerEff_eta_phi',
                title='L1 Trigger Threshold{0} Efficiency;#eta;#phi;#epsilon Thr{0}',
                type='TEfficiency',
                path='L1Trigger',
                xbins=42,xmin=-1.05,     xmax=1.05,
                ybins=32,ymin=-3.1415926,ymax=3.1415926)

    result.merge(helper.result())
    print(" RpcMonitorAlgorithm END !")

    return result


if __name__=="__main__":
    print(" In RpcMonitorAlgorithm !")
    # Setup the RunIII behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1
    
    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    # Config Input/Output 
    import os

    file_list = []
    if os.path.exists('input.txt'):
        infile = open('input.txt', "r")
        file_list = infile.readlines()
        file_list = [ filename.strip() for filename in file_list ]
        print ("read files path from input.txt .")
        print ("files paths: \n", file_list)
    else:
        file_list = ['/eos/atlas/atlascerngroupdisk/det-rpc/data/DESDM_MCP/data18_13TeV.00358615.physics_Main.merge.DESDM_MCP.f961_m2024/data18_13TeV.00358615.physics_Main.merge.DESDM_MCP.f961_m2024._0005.1']

        print ("file input.txt does not exist")
        print ("WIll use files: \n", file_list)

    ConfigFlags.Input.Files = file_list

    ConfigFlags.Output.HISTFileName = 'RPCMonitoringOutput.root'

    ConfigFlags.GeoModel.AtlasVersion = "ATLAS-R2-2016-01-00-01"

    ConfigFlags.lock()
    ConfigFlags.dump()

    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr.Dump = False

    # Initialize configuration object, add accumulator, merge and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    acc = RpcMonitoringConfig(ConfigFlags)
    acc.OutputLevel = INFO
    cfg.merge(acc)

    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    from AtlasGeoModel.AtlasGeoModelConfig import AtlasGeometryCfg
    cfg.merge(MagneticFieldSvcCfg(ConfigFlags))
    cfg.merge(AtlasGeometryCfg(ConfigFlags))

    if ConfigFlags.DQ.Steering.Muon.doTrackMon:
        # do not run in RAW->ESD
        if ConfigFlags.DQ.Environment not in ('tier0Raw',):
            from MuonTrackMonitoring.MuonTrackMonitorAlgorithm import MuonTrackConfig
            cfg.merge(MuonTrackConfig(ConfigFlags))

    cfg.printConfig(withDetails=True, summariseProps = True)

    cfg.run()
