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

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'TgcRawDataMonitorCfg')

    from TgcRawDataMonitoring.TgcRawDataMonitoringConf import TgcRawDataMonitorAlgorithm
    tgcRawDataMonAlg = helper.addAlgorithm(TgcRawDataMonitorAlgorithm,'TgcRawDataMonAlg')

    tgcRawDataMonAlg.TagAndProbe = False

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
    pi = 3.14159265359

    trigPath = 'Trig/'

    myGroup = helper.addGroup(tgcRawDataMonAlg,'TgcRawDataMonitor',mainDir)

    myGroup.defineHistogram('thrNumber_barrel',title='thrNumber_barrel;thrNumber;Events',
                            path=trigPath,xbins=16,xmin=0.0,xmax=16.0)
    myGroup.defineHistogram('thrNumber_endcap',title='thrNumber_endcap;thrNumber;Events',
                            path=trigPath,xbins=16,xmin=0.0,xmax=16.0)
    myGroup.defineHistogram('thrNumber_forward',title='thrNumber_forward;thrNumber;Events',
                            path=trigPath,xbins=16,xmin=0.0,xmax=16.0)

    array = helper.addArray([16],tgcRawDataMonAlg,'TgcRawDataMonitor')

    array.defineHistogram('roiEta,roiPhi',title='roiEta2Phi;roiEta;roiPhi',type='TH2F',path=mainDir+trigPath,
                          xbins=100,xmin=-2.5,xmax=2.5,
                          ybins=48,ymin=-pi,ymax=pi)
    
    array.defineHistogram('roiEta',title='roiEta;roiEta;Events',type='TH1F',path=mainDir+trigPath,
                          xbins=100,xmin=-2.5,xmax=2.5)

    array.defineHistogram('roiPhi_barrel_A',title='roiPhi_barrel_A;roiPhi;Events',type='TH1F',path=mainDir+trigPath,
                          xbins=16,xmin=-pi,xmax=pi)
    array.defineHistogram('roiPhi_endcap_A',title='roiPhi_endcap_A;roiPhi;Events',type='TH1F',path=mainDir+trigPath,
                          xbins=48,xmin=-pi,xmax=pi)
    array.defineHistogram('roiPhi_forward_A',title='roiPhi_forward_A;roiPhi;Events',type='TH1F',path=mainDir+trigPath,
                          xbins=24,xmin=-pi,xmax=pi)

    array.defineHistogram('roiPhi_barrel_C',title='roiPhi_barrel_C;roiPhi;Events',type='TH1F',path=mainDir+trigPath,
                          xbins=16,xmin=-pi,xmax=pi)
    array.defineHistogram('roiPhi_endcap_C',title='roiPhi_endcap_C;roiPhi;Events',type='TH1F',path=mainDir+trigPath,
                          xbins=48,xmin=-pi,xmax=pi)
    array.defineHistogram('roiPhi_forward_C',title='roiPhi_forward_C;roiPhi;Events',type='TH1F',path=mainDir+trigPath,
                          xbins=24,xmin=-pi,xmax=pi)

    array.defineHistogram('bcID_barrel',title='trig2bcid_barrel;BCID;Events',type='TH1F',path=mainDir+trigPath,
                          xbins=4000,xmin=0,xmax=4000)
    array.defineHistogram('bcID_endcap',title='trig2bcid_endcap;BCID;Events',type='TH1F',path=mainDir+trigPath,
                          xbins=4000,xmin=0,xmax=4000)
    array.defineHistogram('bcID_forward',title='trig2bcid_forward;BCID;Events',type='TH1F',path=mainDir+trigPath,
                          xbins=4000,xmin=0,xmax=4000)
    
    array.defineHistogram('muEta,muPhi',title='muEta2Phi;muEta;muPhi',type='TH2F',path=mainDir+trigPath,
                          xbins=100,xmin=-2.5,xmax=2.5,
                          ybins=48,ymin=-pi,ymax=pi)

    array.defineHistogram('muEta',title='muEta;muEta;Events',type='TH1F',path=mainDir+trigPath,
                          xbins=100,xmin=-2.5,xmax=2.5)

    array.defineHistogram('muPt_barrel',title='muPt_barrel;muPt;Events',type='TH1F',path=mainDir+trigPath,
                          xbins=40,xmin=0,xmax=40)
    array.defineHistogram('muPt_endcap',title='muPt_endcap;muPt;Events',type='TH1F',path=mainDir+trigPath,
                          xbins=40,xmin=0,xmax=40)
    array.defineHistogram('muPt_forward',title='muPt_forward;muPt;Events',type='TH1F',path=mainDir+trigPath,
                          xbins=40,xmin=0,xmax=40)

    array.defineHistogram('muPhi_barrel_A',title='muPhi_barrel_A;muPhi;Events',type='TH1F',path=mainDir+trigPath,
                          xbins=16,xmin=-pi,xmax=pi)
    array.defineHistogram('muPhi_endcap_A',title='muPhi_endcap_A;muPhi;Events',type='TH1F',path=mainDir+trigPath,
                          xbins=48,xmin=-pi,xmax=pi)
    array.defineHistogram('muPhi_forward_A',title='muPhi_forward_A;muPhi;Events',type='TH1F',path=mainDir+trigPath,
                          xbins=24,xmin=-pi,xmax=pi)

    array.defineHistogram('muPhi_barrel_C',title='muPhi_barrel_C;muPhi;Events',type='TH1F',path=mainDir+trigPath,
                          xbins=16,xmin=-pi,xmax=pi)
    array.defineHistogram('muPhi_endcap_C',title='muPhi_endcap_C;muPhi;Events',type='TH1F',path=mainDir+trigPath,
                          xbins=48,xmin=-pi,xmax=pi)
    array.defineHistogram('muPhi_forward_C',title='muPhi_forward_C;muPhi;Events',type='TH1F',path=mainDir+trigPath,
                          xbins=24,xmin=-pi,xmax=pi)


    return helper.result()
    
if __name__=='__main__':
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    nightly = '/afs/cern.ch/user/e/ebergeas/work/public/triggermonitoring/nigtly_2019-08-24T2130/'
    file = 'AOD.pool.root'
    ConfigFlags.Input.Files = [nightly+file]
    ConfigFlags.Input.isMC = True
    ConfigFlags.Output.HISTFileName = 'ExampleMonitorOutput.root'
    
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesSerialCfg()
    cfg.merge(PoolReadCfg(ConfigFlags))

    tgcRawDataMonitorAcc = TgcRawDataMonitoringConfig(ConfigFlags)
    cfg.merge(tgcRawDataMonitorAcc)

    cfg.printConfig(withDetails=False)

    cfg.run()
