#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

def LArHVCorrMonConfigOld(inputFlags):

    from AthenaMonitoring.AthMonitorCfgHelper import AthMonitorCfgHelperOld
    from LArMonitoring.LArMonitoringConf import  LArHVCorrectionMonAlg

    helper = AthMonitorCfgHelperOld(inputFlags, 'LArHVCorrMonAlgOldCfg')
    LArHVCorrMonConfigCore(helper, LArHVCorrectionMonAlg, inputFlags)

    from LArConditionsCommon import LArHVDB # noqa: F401

    return helper.result()
    
def LArHVCorrMonConfig(inputFlags):

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'LArHVCorrMonAlgCfg')

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    acc = LArGMCfg(inputFlags)
    from TileGeoModel.TileGMConfig import TileGMCfg
    acc.merge(TileGMCfg(inputFlags))
    from LArCalibUtils.LArHVScaleConfig import LArHVScaleCfg
    acc.merge(LArHVScaleCfg(inputFlags))

    from AthenaConfiguration.ComponentFactory import CompFactory
    LArHVCorrMonConfigCore(helper, CompFactory.LArHVCorrectionMonAlg, inputFlags)

    acc.merge(helper.result())
    return acc

def LArHVCorrMonConfigCore(helper, algoinstance,inputFlags):

    larHVCorrAlg = helper.addAlgorithm(algoinstance,'larHVCorrMonAlg')

    #define the group names here, as you'll use them multiple times
    hvCorrGroupName="LArHVCorrMonGroup"


    # Edit properties of a algorithm
    larHVCorrAlg.HVCorrMonGroupName=hvCorrGroupName
    delta_eta=0.01
    larHVCorrAlg.EtaGranularity=delta_eta
    delta_phi=0.01
    larHVCorrAlg.EtaGranularity=delta_phi
    num_LB=3000
    larHVCorrAlg.NumberOfLBs=num_LB
    threshold=0.02
    larHVCorrAlg.ErrorThreshold=threshold


    from LArMonitoring.GlobalVariables import lArDQGlobals #to define the ranges
    larHVCorr_hist_path='HVCorrection/' #histogram path
    

    hvCorrGroup = helper.addGroup( larHVCorrAlg,
        hvCorrGroupName, '/LAr/'
    )

    #EMB
    hvCorrGroup.defineHistogram('etaEMBA,phiEMBA;LArHVCorrectionEMBA',
                                          title='HV deviation (nominal - corrected) - EMBA;#eta;#phi',
                                          type='TH2F',
                                          path=larHVCorr_hist_path,
                                          weight='hvcorrEMBA',
                                          xbins=int(lArDQGlobals.HVeta_EMB["EMBAPS"][2]/delta_eta),xmin=lArDQGlobals.HVeta_EMB["EMBAPS"][1],xmax=lArDQGlobals.HVeta_EMB["EMBAPS"][2],
                                          ybins=lArDQGlobals.HVphi_EMB["EMBAPS"][0],ymin=lArDQGlobals.HVphi_EMB["EMBAPS"][1],ymax=lArDQGlobals.HVphi_EMB["EMBAPS"][2]
    )

    hvCorrGroup.defineHistogram('etaEMBC,phiEMBC;LArHVCorrectionEMBC',
                                          title='HV deviation (nominal - corrected) - EMBA;#eta;#phi',
                                          type='TH2F',
                                          path=larHVCorr_hist_path,
                                          weight='hvcorrEMBC',
                                          xbins=int(lArDQGlobals.HVeta_EMB["EMBAPS"][2]/delta_eta),xmax=lArDQGlobals.HVeta_EMB["EMBAPS"][1]-0.01,xmin=-lArDQGlobals.HVeta_EMB["EMBAPS"][2]-0.01,
                                          ybins=lArDQGlobals.HVphi_EMB["EMBAPS"][0],ymin=lArDQGlobals.HVphi_EMB["EMBAPS"][1],ymax=lArDQGlobals.HVphi_EMB["EMBAPS"][2]
    )

    hvCorrGroup.defineHistogram('LB,nonnominalEMBA;NDeviatingChannelsEMBA',
                                          title='Number of channels per LB with HV corr>'+str(100*threshold)+' - EMBA;LumiBlock',
                                          type='TProfile',
                                          path=larHVCorr_hist_path,
                                          xbins=num_LB,xmin=0.5,xmax=num_LB+0.5
    )

    hvCorrGroup.defineHistogram('LB,nonnominalEMBC;NDeviatingChannelsEMBC',
                                          title='Number of channels per LB with HV corr>'+str(100*threshold)+' - EMBC;LumiBlock',
                                          type='TProfile',
                                          path=larHVCorr_hist_path,
                                          xbins=num_LB,xmin=0.5,xmax=num_LB+0.5
    )

    #EMEC
    hvCorrGroup.defineHistogram('etaEMECA,phiEMECA;LArHVCorrectionEMECA',
                                          title='HV deviation (nominal - corrected) - EMECA;#eta;#phi',
                                          type='TH2F',
                                          path=larHVCorr_hist_path,
                                          weight='hvcorrEMECA',
                                          xbins=lArDQGlobals.HVeta_EMEC["EMECA"],
                                          ybins=lArDQGlobals.HVphi_EMEC["EMECA"]
    )

    hvCorrGroup.defineHistogram('etaEMECC,phiEMECC;LArHVCorrectionEMECC',
                                          title='HV deviation (nominal - corrected) - EMECA;#eta;#phi',
                                          type='TH2F',
                                          path=larHVCorr_hist_path,
                                          weight='hvcorrEMECC',
                                          xbins=lArDQGlobals.HVeta_EMEC["EMECC"],
                                          ybins=lArDQGlobals.HVphi_EMEC["EMECC"]
    )

    hvCorrGroup.defineHistogram('LB,nonnominalEMECA;NDeviatingChannelsEMECA',
                                          title='Number of channels per LB with HV corr>'+str(100*threshold)+' - EMECA;LumiBlock',
                                          type='TProfile',
                                          path=larHVCorr_hist_path,
                                          xbins=num_LB,xmin=0.5,xmax=num_LB+0.5
    )

    hvCorrGroup.defineHistogram('LB,nonnominalEMECC;NDeviatingChannelsEMECC',
                                          title='Number of channels per LB with HV corr>'+str(100*threshold)+' - EMECC;LumiBlock',
                                          type='TProfile',
                                          path=larHVCorr_hist_path,
                                          xbins=num_LB,xmin=0.5,xmax=num_LB+0.5
    )
    
    #HEC
    hvCorrGroup.defineHistogram('etaHECA,phiHECA;LArHVCorrectionHECA',
                                          title='HV deviation (nominal - corrected) - HECA;#eta;#phi',
                                          type='TH2F',
                                          path=larHVCorr_hist_path,
                                          weight='hvcorrHECA',
                                          xbins=lArDQGlobals.HVeta_HECFcal["HECA"][0],xmin=lArDQGlobals.HVeta_HECFcal["HECA"][1], xmax=lArDQGlobals.HVeta_HECFcal["HECA"][2],
                                          ybins=lArDQGlobals.HVphi_HECFcal["HECA"][0], ymin=lArDQGlobals.HVphi_HECFcal["HECA"][1], ymax=lArDQGlobals.HVphi_HECFcal["HECA"][2]
    )

    hvCorrGroup.defineHistogram('etaHECC,phiHECC;LArHVCorrectionHECC',
                                          title='HV deviation (nominal - corrected) - HECA;#eta;#phi',
                                          type='TH2F',
                                          path=larHVCorr_hist_path,
                                          weight='hvcorrHECC',
                                          xbins=lArDQGlobals.HVeta_HECFcal["HECC"][0],xmin=lArDQGlobals.HVeta_HECFcal["HECC"][1], xmax=lArDQGlobals.HVeta_HECFcal["HECC"][2],
                                          ybins=lArDQGlobals.HVphi_HECFcal["HECC"][0], ymin=lArDQGlobals.HVphi_HECFcal["HECC"][1], ymax=lArDQGlobals.HVphi_HECFcal["HECC"][2]
    )

    hvCorrGroup.defineHistogram('LB,nonnominalHECA;NDeviatingChannelsHECA',
                                          title='Number of channels per LB with HV corr>'+str(100*threshold)+' - HECA;LumiBlock',
                                          type='TProfile',
                                          path=larHVCorr_hist_path,
                                          xbins=num_LB,xmin=0.5,xmax=num_LB+0.5
    )

    hvCorrGroup.defineHistogram('LB,nonnominalHECC;NDeviatingChannelsHECC',
                                          title='Number of channels per LB with HV corr>'+str(100*threshold)+' - HECC;LumiBlock',
                                          type='TProfile',
                                          path=larHVCorr_hist_path,
                                          xbins=num_LB,xmin=0.5,xmax=num_LB+0.5
    )
    
    #FCAL
    hvCorrGroup.defineHistogram('etaFCALA,phiFCALA;LArHVCorrectionFCALA',
                                          title='HV deviation (nominal - corrected) - FCALA;#eta;#phi',
                                          type='TH2F',
                                          path=larHVCorr_hist_path,
                                          weight='hvcorrFCALA',
                                          xbins=lArDQGlobals.HVeta_HECFcal["FCalA"][0],xmin=lArDQGlobals.HVeta_HECFcal["FCalA"][1], xmax=lArDQGlobals.HVeta_HECFcal["FCalA"][2],
                                          ybins=lArDQGlobals.HVphi_HECFcal["FCalA"][0], ymin=lArDQGlobals.HVphi_HECFcal["FCalA"][1], ymax=lArDQGlobals.HVphi_HECFcal["FCalA"][2]
    )

    hvCorrGroup.defineHistogram('etaFCALC,phiFCALC;LArHVCorrectionFCALC',
                                          title='HV deviation (nominal - corrected) - FCALA;#eta;#phi',
                                          type='TH2F',
                                          path=larHVCorr_hist_path,
                                          weight='hvcorrFCALC',
                                          xbins=lArDQGlobals.HVeta_HECFcal["FCalC"][0],xmin=lArDQGlobals.HVeta_HECFcal["FCalC"][1], xmax=lArDQGlobals.HVeta_HECFcal["FCalC"][2],
                                          ybins=lArDQGlobals.HVphi_HECFcal["FCalC"][0], ymin=lArDQGlobals.HVphi_HECFcal["FCalC"][1], ymax=lArDQGlobals.HVphi_HECFcal["FCalC"][2]
    )

    hvCorrGroup.defineHistogram('LB,nonnominalFCALA;NDeviatingChannelsFCALA',
                                          title='Number of channels per LB with HV corr>'+str(100*threshold)+' - FCALA;LumiBlock',
                                          type='TProfile',
                                          path=larHVCorr_hist_path,
                                          xbins=num_LB,xmin=0.5,xmax=num_LB+0.5
    )

    hvCorrGroup.defineHistogram('LB,nonnominalFCALC;NDeviatingChannelsFCALC',
                                          title='Number of channels per LB with HV corr>'+str(100*threshold)+' - FCALC;LumiBlock',
                                          type='TProfile',
                                          path=larHVCorr_hist_path,
                                          xbins=num_LB,xmin=0.5,xmax=num_LB+0.5
    )
    

if __name__=='__main__':

    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    nightly = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/'
    file = 'data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1'
    ConfigFlags.Input.Files = [nightly+file]
    ConfigFlags.Input.isMC = False
    ConfigFlags.Output.HISTFileName = 'LArHVCorrMonOutput.root'
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    cfg.merge(LArHVCorrMonConfig(ConfigFlags))

    Nevents=10
    cfg.run(Nevents) #use cfg.run() to run on all events
