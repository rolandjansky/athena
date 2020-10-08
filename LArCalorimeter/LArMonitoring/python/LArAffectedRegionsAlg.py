#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

def LArAffectedRegionsConfigOld(inputFlags):
    
    from AthenaMonitoring.AthMonitorCfgHelper import AthMonitorCfgHelperOld
    from LArMonitoring.LArMonitoringConf import LArAffectedRegionsAlg

    helper = AthMonitorCfgHelperOld(inputFlags,'LArAffectedRegionsAlgOldCfg')
    LArAffectedRegionsConfigCore(helper, LArAffectedRegionsAlg, inputFlags)

    return helper.result() 

def LArAffectedRegionsConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'LArAffectedRegionsAlgCfg')

    from AthenaConfiguration.ComponentFactory import CompFactory
    LArAffectedRegionsConfigCore(helper, CompFactory.LArAffectedRegionsAlg, inputFlags)

    return helper.result()


def LArAffectedRegionsConfigCore(helper, algoinstance, inputFlags):

    larAffectedRegAlg = helper.addAlgorithm(algoinstance,'larAffectedRegAlg')

    #define the group names here, as you'll use them multiple times
    affectedRegGroupName="LArAffectedRegionsMonGroup"


    # Edit properties of a algorithm
    larAffectedRegAlg.AffectedRegionsGroupName=affectedRegGroupName
    isOnline=False
    from AthenaConfiguration.ComponentFactory import isRun3Cfg
    if isRun3Cfg():
       if inputFlags.DQ.Environment == 'online':
          isOnline=True
    else:
       from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
       if athenaCommonFlags.isOnline:
          isOnline=True

    larAffectedRegAlg.IsOnline = isOnline


    from LArMonitoring.GlobalVariables import lArDQGlobals #to define the ranges
    larAffReg_hist_path='AffectedRegions/' #histogram path
    

    #EMBPS
    group_name_ending="EMBPS" 
    larAffectedRegAlg.EMBPSname=group_name_ending
    affectedRegGroupEMBPS = helper.addGroup(
        larAffectedRegAlg,
        affectedRegGroupName+group_name_ending,
        '/LAr/',
        'run'
    )


    affectedRegGroupEMBPS.defineHistogram('etaPOS,phi;RAW_LArAffectedRegionsEMBAPS',
                                          title='HV Affected Regions - EMBA - Presampler;#eta;#phi',
                                          type='TH2F',
                                          path=larAffReg_hist_path,
                                          weight='problem',
                                          xbins=lArDQGlobals.HVeta_EMB["EMBAPS"][0],xmin=lArDQGlobals.HVeta_EMB["EMBAPS"][1],xmax=lArDQGlobals.HVeta_EMB["EMBAPS"][2],
                                          ybins=lArDQGlobals.HVphi_EMB["EMBAPS"][0],ymin=lArDQGlobals.HVphi_EMB["EMBAPS"][1],ymax=lArDQGlobals.HVphi_EMB["EMBAPS"][2],
                                          merge='weightedAverage'
    )
    affectedRegGroupEMBPS.defineHistogram('etaNEG,phi;RAW_LArAffectedRegionsEMBCPS',
                                          title='HV Affected Regions - EMBC - Presampler;#eta;#phi',
                                          type='TH2F',
                                          path=larAffReg_hist_path,
                                          weight='problem',
                                          xbins=lArDQGlobals.HVeta_EMB["EMBCPS"][0],xmin=lArDQGlobals.HVeta_EMB["EMBCPS"][1],xmax=lArDQGlobals.HVeta_EMB["EMBCPS"][2],
                                          ybins=lArDQGlobals.HVphi_EMB["EMBCPS"][0],ymin=lArDQGlobals.HVphi_EMB["EMBCPS"][1],ymax=lArDQGlobals.HVphi_EMB["EMBCPS"][2],
                                          merge='weightedAverage'
    )


    #EMB
    group_name_ending="EMB"
    larAffectedRegAlg.EMBname=group_name_ending
    affectedRegGroupEMB = helper.addGroup(
        larAffectedRegAlg,
        affectedRegGroupName+group_name_ending,
        '/LAr/',
        'run'
    )

    affectedRegGroupEMB.defineHistogram('etaPOS,phi;RAW_LArAffectedRegionsEMBA',
                                        title='HV Affected Regions - EMBA - Samplings 1-3;#eta;#phi',
                                        type='TH2F',
                                        path=larAffReg_hist_path,
                                        weight='problem',
                                        xbins=lArDQGlobals.HVeta_EMB["EMBA"][0],xmin=lArDQGlobals.HVeta_EMB["EMBA"][1],xmax=lArDQGlobals.HVeta_EMB["EMBA"][2],
                                        ybins=lArDQGlobals.HVphi_EMB["EMBA"][0],ymin=lArDQGlobals.HVphi_EMB["EMBA"][1],ymax=lArDQGlobals.HVphi_EMB["EMBA"][2],
                                        merge='weightedAverage'
    )
    affectedRegGroupEMB.defineHistogram('etaNEG,phi;RAW_LArAffectedRegionsEMBC',
                                        title='HV Affected Regions - EMBC - Samplings 1-3;#eta;#phi',
                                        type='TH2F',
                                        path=larAffReg_hist_path,
                                        weight='problem',
                                        xbins=lArDQGlobals.HVeta_EMB["EMBC"][0],xmin=lArDQGlobals.HVeta_EMB["EMBC"][1],xmax=lArDQGlobals.HVeta_EMB["EMBC"][2],
                                        ybins=lArDQGlobals.HVphi_EMB["EMBC"][0],ymin=lArDQGlobals.HVphi_EMB["EMBC"][1],ymax=lArDQGlobals.HVphi_EMB["EMBC"][2],
                                        merge='weightedAverage'
    )
        

    #EMECPS
    group_name_ending="EMECPS"
    larAffectedRegAlg.EMECPSname=group_name_ending
    affectedRegGroupEMECPS = helper.addGroup(
        larAffectedRegAlg,
        affectedRegGroupName+group_name_ending,
        '/LAr/',
        'run'
    )
    
    affectedRegGroupEMECPS.defineHistogram('etaPOS,phi;RAW_LArAffectedRegionsEMECAPS',
                                           title='HV Affected Regions - EMECA - Presampler;#eta;#phi',
                                           type='TH2F',
                                           path=larAffReg_hist_path,
                                           weight='problem',
                                           xbins=lArDQGlobals.HVeta_EMEC["EMECAPS"],
                                           ybins=lArDQGlobals.HVphi_EMEC["EMECAPS"],
                                           merge='weightedAverage'
    )
    affectedRegGroupEMECPS.defineHistogram('etaNEG,phi;RAW_LArAffectedRegionsEMECCPS',
                                           title='HV Affected Regions - EMECC - Presampler;#eta;#phi',
                                           type='TH2F',
                                           path=larAffReg_hist_path,
                                           weight='problem',
                                           xbins=lArDQGlobals.HVeta_EMEC["EMECCPS"],
                                           ybins=lArDQGlobals.HVphi_EMEC["EMECCPS"],
                                           merge='weightedAverage'
    )


    #EMEC
    group_name_ending="EMEC"
    larAffectedRegAlg.EMECname=group_name_ending 
    affectedRegGroupEMEC = helper.addGroup(
        larAffectedRegAlg,
        affectedRegGroupName+group_name_ending,
        '/LAr/',
        'run'
    )
    
    affectedRegGroupEMEC.defineHistogram('etaPOS,phi;RAW_LArAffectedRegionsEMECA',
                                         title='HV Affected Regions - EMECA - Samplings 1-3;#eta;#phi',
                                         type='TH2F',
                                         path=larAffReg_hist_path,
                                         weight='problem',
                                         xbins=lArDQGlobals.HVeta_EMEC["EMECA"],
                                         ybins=lArDQGlobals.HVphi_EMEC["EMECA"],
                                         merge='weightedAverage'
    )
    affectedRegGroupEMEC.defineHistogram('etaNEG,phi;RAW_LArAffectedRegionsEMECC',
                                         title='HV Affected Regions - EMECC - Samplings 1-3;#eta;#phi',
                                         type='TH2F',
                                         path=larAffReg_hist_path,
                                         weight='problem',
                                         xbins=lArDQGlobals.HVeta_EMEC["EMECC"],
                                         ybins=lArDQGlobals.HVphi_EMEC["EMECC"],
                                         merge='weightedAverage'
    )


    #HEC0
    group_name_ending="HEC0"
    larAffectedRegAlg.HEC0name=group_name_ending
    affectedRegGroupHEC0 = helper.addGroup(
        larAffectedRegAlg,
        affectedRegGroupName+group_name_ending,
        '/LAr/',
        'run'
    )
        
    affectedRegGroupHEC0.defineHistogram('etaPOS,phi;RAW_LArAffectedRegionsHECA0',
                                         title='HV Affected Regions - HECA - Layer 1;#eta;#phi',
                                         type='TH2F',
                                         path=larAffReg_hist_path,
                                         weight='problem',
                                         xbins=lArDQGlobals.HVeta_HECFcal["HECA"][0],xmin=lArDQGlobals.HVeta_HECFcal["HECA"][1],xmax=lArDQGlobals.HVeta_HECFcal["HECA"][2],
                                         ybins=lArDQGlobals.HVphi_HECFcal["HECA"][0],ymin=lArDQGlobals.HVphi_HECFcal["HECA"][1],ymax=lArDQGlobals.HVphi_HECFcal["HECA"][2],
                                         merge='weightedAverage'
    )
    affectedRegGroupHEC0.defineHistogram('etaNEG,phi;RAW_LArAffectedRegionsHECC0',
                                         title='HV Affected Regions - HECC - Layer 1;#eta;#phi',
                                         type='TH2F',
                                         path=larAffReg_hist_path,
                                         weight='problem',
                                         xbins=lArDQGlobals.HVeta_HECFcal["HECC"][0],xmin=lArDQGlobals.HVeta_HECFcal["HECC"][1],xmax=lArDQGlobals.HVeta_HECFcal["HECC"][2],
                                         ybins=lArDQGlobals.HVphi_HECFcal["HECC"][0],ymin=lArDQGlobals.HVphi_HECFcal["HECC"][1],ymax=lArDQGlobals.HVphi_HECFcal["HECC"][2],
                                         merge='weightedAverage'
    )

    #HEC1
    group_name_ending="HEC1"
    larAffectedRegAlg.HEC1name=group_name_ending
    affectedRegGroupHEC1 = helper.addGroup(
        larAffectedRegAlg,
        affectedRegGroupName+group_name_ending,
        '/LAr/',
        'run'
    )
    
    affectedRegGroupHEC1.defineHistogram('etaPOS,phi;RAW_LArAffectedRegionsHECA1',
                                         title='HV Affected Regions - HECA - Layer 2;#eta;#phi',
                                         type='TH2F',
                                         path=larAffReg_hist_path,
                                         weight='problem',
                                         xbins=lArDQGlobals.HVeta_HECFcal["HECA"][0],xmin=lArDQGlobals.HVeta_HECFcal["HECA"][1],xmax=lArDQGlobals.HVeta_HECFcal["HECA"][2],
                                         ybins=lArDQGlobals.HVphi_HECFcal["HECA"][0],ymin=lArDQGlobals.HVphi_HECFcal["HECA"][1],ymax=lArDQGlobals.HVphi_HECFcal["HECA"][2],
                                         merge='weightedAverage'
    )
    affectedRegGroupHEC1.defineHistogram('etaNEG,phi;RAW_LArAffectedRegionsHECC1',
                                         title='HV Affected Regions - HECC - Layer 2;#eta;#phi',
                                         type='TH2F',
                                         path=larAffReg_hist_path,
                                         weight='problem',
                                         xbins=lArDQGlobals.HVeta_HECFcal["HECC"][0],xmin=lArDQGlobals.HVeta_HECFcal["HECC"][1],xmax=lArDQGlobals.HVeta_HECFcal["HECC"][2],
                                         ybins=lArDQGlobals.HVphi_HECFcal["HECC"][0],ymin=lArDQGlobals.HVphi_HECFcal["HECC"][1],ymax=lArDQGlobals.HVphi_HECFcal["HECC"][2],
                                         merge='weightedAverage'
    )
    
    #HEC2
    group_name_ending="HEC2"
    larAffectedRegAlg.HEC2name=group_name_ending
    affectedRegGroupHEC2 = helper.addGroup(
        larAffectedRegAlg,
        affectedRegGroupName+group_name_ending,
        '/LAr/',
        'run'
    )
    
    affectedRegGroupHEC2.defineHistogram('etaPOS,phi;RAW_LArAffectedRegionsHECA2',
                                         title='HV Affected Regions - HECA - Layer 3;#eta;#phi',
                                         type='TH2F',
                                         path=larAffReg_hist_path,
                                         weight='problem',
                                         xbins=lArDQGlobals.HVeta_HECFcal["HECA"][0],xmin=lArDQGlobals.HVeta_HECFcal["HECA"][1],xmax=lArDQGlobals.HVeta_HECFcal["HECA"][2],
                                         ybins=lArDQGlobals.HVphi_HECFcal["HECA"][0],ymin=lArDQGlobals.HVphi_HECFcal["HECA"][1],ymax=lArDQGlobals.HVphi_HECFcal["HECA"][2],
                                         merge='weightedAverage'
    )
    affectedRegGroupHEC2.defineHistogram('etaNEG,phi;RAW_LArAffectedRegionsHECC2',
                                         title='HV Affected Regions - HECC - Layer 3;#eta;#phi',
                                         type='TH2F',
                                         path=larAffReg_hist_path,
                                         weight='problem',
                                         xbins=lArDQGlobals.HVeta_HECFcal["HECC"][0],xmin=lArDQGlobals.HVeta_HECFcal["HECC"][1],xmax=lArDQGlobals.HVeta_HECFcal["HECC"][2],
                                         ybins=lArDQGlobals.HVphi_HECFcal["HECC"][0],ymin=lArDQGlobals.HVphi_HECFcal["HECC"][1],ymax=lArDQGlobals.HVphi_HECFcal["HECC"][2],
                                         merge='weightedAverage'
    )
        
    #HEC3
    group_name_ending="HEC3"
    larAffectedRegAlg.HEC3name=group_name_ending
    affectedRegGroupHEC3 = helper.addGroup(
        larAffectedRegAlg,
        affectedRegGroupName+group_name_ending,
        '/LAr/',
        'run'
    )
    
    affectedRegGroupHEC3.defineHistogram('etaPOS,phi;RAW_LArAffectedRegionsHECA3',
                                         title='HV Affected Regions - HECA - Layer 4;#eta;#phi',
                                         type='TH2F',
                                         path=larAffReg_hist_path,
                                         weight='problem',
                                         xbins=lArDQGlobals.HVeta_HECFcal["HECA"][0],xmin=lArDQGlobals.HVeta_HECFcal["HECA"][1],xmax=lArDQGlobals.HVeta_HECFcal["HECA"][2],
                                         ybins=lArDQGlobals.HVphi_HECFcal["HECA"][0],ymin=lArDQGlobals.HVphi_HECFcal["HECA"][1],ymax=lArDQGlobals.HVphi_HECFcal["HECA"][2],
                                         merge='weightedAverage'
    )
    affectedRegGroupHEC3.defineHistogram('etaNEG,phi;RAW_LArAffectedRegionsHECC3',
                                         title='HV Affected Regions - HECC - Layer 4;#eta;#phi',
                                         type='TH2F',
                                         path=larAffReg_hist_path,
                                         weight='problem',
                                         xbins=lArDQGlobals.HVeta_HECFcal["HECC"][0],xmin=lArDQGlobals.HVeta_HECFcal["HECC"][1],xmax=lArDQGlobals.HVeta_HECFcal["HECC"][2],
                                         ybins=lArDQGlobals.HVphi_HECFcal["HECC"][0],ymin=lArDQGlobals.HVphi_HECFcal["HECC"][1],ymax=lArDQGlobals.HVphi_HECFcal["HECC"][2],
                                         merge='weightedAverage'
    )
    
    #FCAL0
    group_name_ending="FCAL0"
    larAffectedRegAlg.FCAL0name=group_name_ending
    affectedRegGroupFCAL0 = helper.addGroup(
        larAffectedRegAlg,
        affectedRegGroupName+group_name_ending,
        '/LAr/',
        'run'
    )
    
    affectedRegGroupFCAL0.defineHistogram('etaPOS,phi;RAW_LArAffectedRegionsFCALA0',
                                          title='HV Affected Regions - FCALA - Layer 1;#eta;#phi',
                                          type='TH2F',
                                          path=larAffReg_hist_path,
                                          weight='problem',
                                          xbins=lArDQGlobals.HVeta_HECFcal["FCalA"][0],xmin=lArDQGlobals.HVeta_HECFcal["FCalA"][1],xmax=lArDQGlobals.HVeta_HECFcal["FCalA"][2],
                                          ybins=lArDQGlobals.HVphi_HECFcal["FCalA"][0],ymin=lArDQGlobals.HVphi_HECFcal["FCalA"][1],ymax=lArDQGlobals.HVphi_HECFcal["FCalA"][2],
                                          merge='weightedAverage'
    )
    affectedRegGroupFCAL0.defineHistogram('etaNEG,phi;RAW_LArAffectedRegionsFCALC0',
                                          title='HV Affected Regions - FCALC - Layer 1;#eta;#phi',
                                          type='TH2F',
                                          path=larAffReg_hist_path,
                                          weight='problem',
                                          xbins=lArDQGlobals.HVeta_HECFcal["FCalC"][0],xmin=lArDQGlobals.HVeta_HECFcal["FCalC"][1],xmax=lArDQGlobals.HVeta_HECFcal["FCalC"][2],
                                          ybins=lArDQGlobals.HVphi_HECFcal["FCalC"][0],ymin=lArDQGlobals.HVphi_HECFcal["FCalC"][1],ymax=lArDQGlobals.HVphi_HECFcal["FCalC"][2],
                                          merge='weightedAverage'
    )

    #FCAL1
    group_name_ending="FCAL1"
    larAffectedRegAlg.FCAL1name=group_name_ending
    affectedRegGroupFCAL1 = helper.addGroup(
        larAffectedRegAlg,
        affectedRegGroupName+group_name_ending,
        '/LAr/',
        'run'
    )
    
    affectedRegGroupFCAL1.defineHistogram('etaPOS,phi;RAW_LArAffectedRegionsFCALA1',
                                          title='HV Affected Regions - FCALA - Layer 2;#eta;#phi',
                                          type='TH2F',
                                          path=larAffReg_hist_path,
                                          weight='problem',
                                          xbins=lArDQGlobals.HVeta_HECFcal["FCalA"][0],xmin=lArDQGlobals.HVeta_HECFcal["FCalA"][1],xmax=lArDQGlobals.HVeta_HECFcal["FCalA"][2],
                                          ybins=lArDQGlobals.HVphi_HECFcal["FCalA"][0],ymin=lArDQGlobals.HVphi_HECFcal["FCalA"][1],ymax=lArDQGlobals.HVphi_HECFcal["FCalA"][2],
                                          merge='weightedAverage'
    )
    affectedRegGroupFCAL1.defineHistogram('etaNEG,phi;RAW_LArAffectedRegionsFCALC1',
                                          title='HV Affected Regions - FCALC - Layer 2;#eta;#phi',
                                          type='TH2F',
                                          path=larAffReg_hist_path,
                                          weight='problem',
                                          xbins=lArDQGlobals.HVeta_HECFcal["FCalC"][0],xmin=lArDQGlobals.HVeta_HECFcal["FCalC"][1],xmax=lArDQGlobals.HVeta_HECFcal["FCalC"][2],
                                          ybins=lArDQGlobals.HVphi_HECFcal["FCalC"][0],ymin=lArDQGlobals.HVphi_HECFcal["FCalC"][1],ymax=lArDQGlobals.HVphi_HECFcal["FCalC"][2],
                                          merge='weightedAverage'
    )

    #FCAL2
    group_name_ending="FCAL2"
    larAffectedRegAlg.FCAL2name=group_name_ending
    affectedRegGroupFCAL2 = helper.addGroup(
        larAffectedRegAlg,
        affectedRegGroupName+group_name_ending,
        '/LAr/',
        'run'
    )
        
    affectedRegGroupFCAL2.defineHistogram('etaPOS,phi;RAW_LArAffectedRegionsFCALA2',
                                          title='HV Affected Regions - FCALA - Layer 3;#eta;#phi',
                                          type='TH2F',
                                          path=larAffReg_hist_path,
                                          weight='problem',
                                          xbins=lArDQGlobals.HVeta_HECFcal["FCalA"][0],xmin=lArDQGlobals.HVeta_HECFcal["FCalA"][1],xmax=lArDQGlobals.HVeta_HECFcal["FCalA"][2],
                                          ybins=lArDQGlobals.HVphi_HECFcal["FCalA"][0],ymin=lArDQGlobals.HVphi_HECFcal["FCalA"][1],ymax=lArDQGlobals.HVphi_HECFcal["FCalA"][2],
                                          merge='weightedAverage'
    )
    affectedRegGroupFCAL2.defineHistogram('etaNEG,phi;RAW_LArAffectedRegionsFCALC2',
                                          title='HV Affected Regions - FCALC - Layer 3;#eta;#phi',
                                          type='TH2F',
                                          path=larAffReg_hist_path,
                                          weight='problem',
                                          xbins=lArDQGlobals.HVeta_HECFcal["FCalC"][0],xmin=lArDQGlobals.HVeta_HECFcal["FCalC"][1],xmax=lArDQGlobals.HVeta_HECFcal["FCalC"][2],
                                          ybins=lArDQGlobals.HVphi_HECFcal["FCalC"][0],ymin=lArDQGlobals.HVphi_HECFcal["FCalC"][1],ymax=lArDQGlobals.HVphi_HECFcal["FCalC"][2],
                                          merge='weightedAverage'
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
    from LArMonitoring.LArMonConfigFlags import createLArMonConfigFlags
    createLArMonConfigFlags()

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RAW

    ConfigFlags.Output.HISTFileName = 'LArAffectedRegionsOutput.root'
    ConfigFlags.DQ.enableLumiAccess = False
    ConfigFlags.DQ.useTrigger = False
    ConfigFlags.Beam.Type = 'collisions'
    ConfigFlags.lock()


    from CaloRec.CaloRecoConfig import CaloRecoCfg
    cfg=CaloRecoCfg(ConfigFlags)

    #add affected regions
    affregmon = LArAffectedRegionsConfig(ConfigFlags)
    cfg.merge(affregmon)

    ConfigFlags.dump()
    f=open("AffectedRegionsMonMaker.pkl","w")
    cfg.store(f)
    f.close()

