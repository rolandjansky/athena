#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''
@file RPCMonitoringConfig.py
@brief Python configuration of RPC Monitoring for the Run III
'''

def RpcMonitoringConfig(inputFlags):

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'RpcMonitoringConfig')
    from AthenaConfiguration.ComponentFactory import CompFactory
    RPCMonitorAlgorithm=CompFactory.RPCMonitorAlgorithm

    rpcMonitorAlg = helper.addAlgorithm(RPCMonitorAlgorithm, "RPCMonitorAlgorithmAlg")
    # set properties of algorithm RPCMonitorAlgorithm
    rpcMonitorAlg.MinRoIDR  = 0.3
    rpcMonitorAlg.MinPt     = 2000
    rpcMonitorAlg.MuQuality = 1
    
    myGroup = helper.addGroup(rpcMonitorAlg, 'RPCMonitorAlgorithm', 'Muon/MuonRawDataMonitoring/RPC/')

    myGroup.defineHistogram('run;Run',
                            title='Run Number;run;Events',
                            type='TH1I', 
                            path='PlotCand',
                            xbins=1000000,xmin=-0.5,xmax=999999.5)
    myGroup.defineHistogram('nMu;NMuon',
                            title='Number of Muons;nMuons;Events',
                            type='TH1I',
                            path='PlotCand',
                            xbins=10,xmin=-0.5,xmax=9.5)
    myGroup.defineHistogram('nMuBarrel;NMuonBarrel',
                            title='Number of Barrel Muons;nMuons;Events',
                            type='TH1I',
                            path='PlotCand',
                            xbins=5,xmin=-0.5,xmax=4.5)

    myGroup.defineHistogram('muPtCollection;MuonPt',
                            title='muon Pt;Pt[MeV];Muons',
                            type='TH1D',
                            path='PlotCand',
                            xbins=200,xmin=0,xmax=1000e3)
    myGroup.defineHistogram('roiEtaCollection;roiEta',
                            title='roi eta;roi #eta;rois',
                            type='TH1D',
                            path='PlotCand',
                            xbins=50,xmin=-2.5,xmax=2.5)
    myGroup.defineHistogram('roiBarrelEtaCollection;roiBarrelEta',
                            title='Barrel roi eta;roi #eta;rois',
                            type='TH1D',
                            path='PlotCand',
                            xbins=50,xmin=-2.5,xmax=2.5)
    myGroup.defineHistogram('roiBarrelThrCollection;roiBarrelThrs',
                            title='Barrel roi threshold;roi threshold;rois',
                            type='TH1I',
                            path='PlotCand',
                            xbins=6,xmin=0.5,xmax=6.5)

    myGroup.defineHistogram('ptDen;Pt', title='muon Pt;Pt[MeV];Muons', type='TH1D', path='TriggerEff/Denominator',
                            xbins=150,xmin=0.0,xmax=150.0e3)
    myGroup.defineHistogram('ptDen;Pt_bins', title='muon Pt;Pt[MeV];Muons', type='TH1D', path='TriggerEff/Denominator',
                            xbins=[0,3.0e3,4.0e3,5.0e3,6.0e3,7.0e3,8.0e3,10.0e3,12.0e3,15.0e3,20.0e3,25.0e3,30.0e3,40.0e3,60.0e3,80.0e3,81.0e3])
    myGroup.defineHistogram('ptNumThr1;Pt', title='muon Pt;Pt[MeV];Muons', type='TH1D', path='TriggerEff/Thr1',
                            xbins=150,xmin=0.0,xmax=150.0e3)
    myGroup.defineHistogram('ptNumThr1;Pt_bins', title='muon Pt;Pt[MeV];Muons', type='TH1D', path='TriggerEff/Thr1',
                            xbins=[0,3.0e3,4.0e3,5.0e3,6.0e3,7.0e3,8.0e3,10.0e3,12.0e3,15.0e3,20.0e3,25.0e3,30.0e3,40.0e3,60.0e3,80.0e3,81.0e3])
    myGroup.defineHistogram('etaDen,phiDen;barrel_eta_phi', type='TH2D', title='muons;#eta;#phi',path='TriggerEff/Denominator',
                            xbins=42,xmin=-1.05,     xmax=1.05,
                            ybins=32,ymin=-3.1415926,ymax=3.1415926)
    myGroup.defineHistogram('etaNumThr1,phiNumThr1;barrel_eta_phi', type='TH2D', title='muons;#eta;#phi',path='TriggerEff/Thr1',
                            xbins=42,xmin=-1.05,     xmax=1.05,
                            ybins=32,ymin=-3.1415926,ymax=3.1415926)

    myGroup.defineHistogram('isPassed,ptDen;eff_vs_Pt',
                            title='Trigger Efficiency;Pt[MeV];#epsilon',
                            type='TEfficiency',
                            path='TriggerEff',
                            #xbins=[0,3.0e3,4.0e3,5.0e3,6.0e3,7.0e3,8.0e3,10.0e3,12.0e3,15.0e3,20.0e3,25.0e3,30.0e3,40.0e3,60.0e3,80.0e3,81.0e3])
                            xbins=10, xmin=0.0, xmax=80.0e3)

    myGroup.defineHistogram('isPassed,etaDen,phiDen;eff_vs_eta_phi',
                            title='Trigger Efficiency;#eta;#phi;#epsilon',
                            type='TEfficiency',
                            path='TriggerEff',
                            xbins=42,xmin=-1.05,     xmax=1.05,
                            ybins=32,ymin=-3.1415926,ymax=3.1415926)

    return helper.result()

if __name__=="__main__":
    # Setup the RunIII behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1
    
    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    TestFiles = ['/eos/atlas/atlascerngroupdisk/det-rpc/data/DESDM_MCP/data18_13TeV.00358615.physics_Main.merge.DESDM_MCP.f961_m2024/data18_13TeV.00358615.physics_Main.merge.DESDM_MCP.f961_m2024._0084.1']
    ConfigFlags.Input.Files = TestFiles
    ConfigFlags.Output.HISTFileName = 'RPCMonitoringOutput.root'

    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    acc = RpcMonitoringConfig(ConfigFlags)
    cfg.merge(acc)

    # If you want to turn on more detailed messages ...
    #acc.getEventAlgo('RPCMonitorAlgorithmAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=False)

    cfg.run(20)
