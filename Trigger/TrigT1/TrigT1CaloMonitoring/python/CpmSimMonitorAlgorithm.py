#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
def CpmSimMonitoringConfig(inputFlags):
    '''Function to configure LVL1 CpmSim algorithm in the monitoring system.'''

    #import math 
    # get the component factory - used for getting the algorithms
    from AthenaConfiguration.AutoConfigFlags import GetFileMD
    from AthenaConfiguration.Enums import Format
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    # any things that need setting up for job e.g.
    #from AtlasGeoModel.AtlasGeoModelConfig import AtlasGeometryCfg
    #result.merge(AtlasGeometryCfg(inputFlags))

    # make the athena monitoring helper
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'CpmSimMonitoringCfg')

    # Use metadata to check Run3 compatible trigger info is available  
    md = GetFileMD(inputFlags.Input.Files)
    inputContainsRun3FormatConfigMetadata = ("metadata_items" in md and any(('TriggerMenuJson' in key) for key in md["metadata_items"].keys()))
    if inputFlags.Input.Format is Format.POOL and not inputContainsRun3FormatConfigMetadata:
        # No L1 menu available in the POOL file.
        return helper.result()

    # get any algorithms
    CpmSimMonAlg = helper.addAlgorithm(CompFactory.CpmSimMonitorAlgorithm,'CpmSimMonAlg')

    # configure the L1Menu depending on input type
    if inputFlags.Trigger.Online.isPartition or inputFlags.Input.Format is Format.BS:
        # For standalone monitoring use from RAW
        CpmSimMonAlg.TrigConfigSvc = ""
        from TrigConfigSvc.TrigConfigSvcCfg import L1ConfigSvcCfg
        helper.result().merge(L1ConfigSvcCfg(inputFlags))
    else:
        # For monitoring ESD (or MC) 
        from TrigConfxAOD.TrigConfxAODConfig import getxAODConfigSvc
        ca = getxAODConfigSvc(inputFlags)
        CpmSimMonAlg.TrigConfigSvc = ca.getPrimary()
        helper.result().merge(ca)


    # add any steering
    groupName = 'CpmSimMonitor' # the monitoring group name is also used for the package name
    CpmSimMonAlg.PackageName = groupName

    # mainDir is where the group starts from
    mainDir = 'L1Calo'
    # subdirectories
    trigPath = 'CPM'
    simPath = trigPath+'/Errors/Transmission_Simulation/'
    monCPMinPath = simPath + 'PPM2CPMTowers/'
    monRoiPath = simPath + 'Towers2RoIs/'
    simPathCmx = trigPath+'_CMX/Errors/Transmission_Simulation/'
    monCMXinPath = simPathCmx + 'RoIs2TOBs/'
    monCMXsumsPath = simPathCmx + '/TOBs2HitSums/'
    monCMXTopoPath = simPathCmx + '/TOBs2Topo'
    monExpertPath = simPath
    monShiftPath = simPath
    monEvent1Path = simPath + '/MismatchEventNumbers/'
    monEvent2Path = simPathCmx + '/MismatchEventNumbers/'

    # add monitoring algorithm to group, with group name and main directory 
    myGroup = helper.addGroup(CpmSimMonAlg, groupName , mainDir)

    # CPMTowers
    # em
    myGroup.defineHistogram('eta_em_PpmEqCor,phi_em_PpmEqCor;cpm_em_2d_etaPhi_tt_PpmEqCore',
                            title="EM Core CPM Tower/PPM Tower Non-zero Matches;;",type='TH2F',
                            cutmask='',path=monCPMinPath,
                            xbins=66,xmin=-3.3,xmax=3.3,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('eta_em_PpmNeCor,phi_em_PpmNeCor;cpm_em_2d_etaPhi_tt_PpmNeCore',
                            title="EM Core CPM Tower/PPM Tower Non-zero Mismatches;;",type='TH2F',
                            cutmask='',path=monCPMinPath,
                            xbins=66,xmin=-3.3,xmax=3.3,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('eta_em_PpmNoCor,phi_em_PpmNoCor;cpm_em_2d_etaPhi_tt_PpmNoCore',
                            title="EM PPM Towers but no Core CPM Towers;;",type='TH2F',
                            cutmask='',path=monCPMinPath,
                            xbins=66,xmin=-3.3,xmax=3.3,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('eta_em_CoreNoPpm,phi_em_CoreNoPpm;cpm_em_2d_etaPhi_tt_CoreNoPpm',
                            title="EM Core CPM Towers but no PPM Towers;;",type='TH2F',
                            cutmask='',path=monCPMinPath,
                            xbins=66,xmin=-3.3,xmax=3.3,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('eta_em_PpmEqOverlap,phi_em_PpmEqOverlap;cpm_em_2d_etaPhi_tt_PpmEqOverlap',
                            title="EM Overlap CPM Tower/PPM Tower Non-zero Matches;;",type='TH2F',
                            cutmask='',path=monCPMinPath,
                            xbins=66,xmin=-3.3,xmax=3.3,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('eta_em_PpmNeOverlap,phi_em_PpmNeOverlap;cpm_em_2d_etaPhi_tt_PpmNeOverlap',
                            title="EM Overlap CPM Tower/PPM Tower Non-zero Mismatches;;",type='TH2F',
                            cutmask='',path=monCPMinPath,
                            xbins=66,xmin=-3.3,xmax=3.3,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('eta_em_PpmNoOverlap,phi_em_PpmNoOverlap;cpm_em_2d_etaPhi_tt_PpmNoOverlap',
                            title="EM PPM Towers but no Overlap CPM Towers;;",type='TH2F',
                            cutmask='',path=monCPMinPath,
                            xbins=66,xmin=-3.3,xmax=3.3,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('eta_em_OverlapeNoPpm,phi_em_OverlapeNoPpm;cpm_em_2d_etaPhi_tt_OverlapNoPpm',
                            title="EM Overlap CPM Towers but no PPM Towers;;",type='TH2F',
                            cutmask='',path=monCPMinPath,
                            xbins=66,xmin=-3.3,xmax=3.3,ybins=64,ymin=0.,ymax=64.0)


    # hadronic
    myGroup.defineHistogram('eta_had_PpmEqCor,phi_had_PpmEqCor;cpm_had_2d_etaPhi_tt_PpmEqCore',
                            title="HAD Core CPM Tower/PPM Tower Non-zero Matches;;",type='TH2F',
                            cutmask='',path=monCPMinPath,
                            xbins=66,xmin=-3.3,xmax=3.3,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('eta_had_PpmNeCor,phi_had_PpmNeCor;cpm_had_2d_etaPhi_tt_PpmNeCore',
                            title="HAD Core CPM Tower/PPM Tower Non-zero Mismatches;;",type='TH2F',
                            cutmask='',path=monCPMinPath,
                            xbins=66,xmin=-3.3,xmax=3.3,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('eta_had_PpmNoCor,phi_had_PpmNoCor;cpm_had_2d_etaPhi_tt_PpmNoCore',
                            title="HAD PPM Towers but no Core CPM Towers;;",type='TH2F',
                            cutmask='',path=monCPMinPath,
                            xbins=66,xmin=-3.3,xmax=3.3,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('eta_had_CoreNoPpm,phi_had_CoreNoPpm;cpm_had_2d_etaPhi_tt_CoreNoPpm',
                            title="HAD Core CPM Towers but no PPM Towers;;",type='TH2F',
                            cutmask='',path=monCPMinPath,
                            xbins=66,xmin=-3.3,xmax=3.3,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('eta_had_PpmEqOverlap,phi_had_PpmEqOverlap;cpm_had_2d_etaPhi_tt_PpmEqOverlap',
                            title="HAD Overlap CPM Tower/PPM Tower Non-zero Matches;;",type='TH2F',
                            cutmask='',path=monCPMinPath,
                            xbins=66,xmin=-3.3,xmax=3.3,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('eta_had_PpmNeOverlap,phi_had_PpmNeOverlap;cpm_had_2d_etaPhi_tt_PpmNeOverlap',
                            title="HAD Overlap CPM Tower/PPM Tower Non-zero Mismatches;;",type='TH2F',
                            cutmask='',path=monCPMinPath,
                            xbins=66,xmin=-3.3,xmax=3.3,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('eta_had_PpmNoOverlap,phi_had_PpmNoOverlap;cpm_had_2d_etaPhi_tt_PpmNoOverlap',
                            title="HAD PPM Towers but no Overlap CPM Towers;;",type='TH2F',
                            cutmask='',path=monCPMinPath,
                            xbins=66,xmin=-3.3,xmax=3.3,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('eta_had_OverlapeNoPpm,phi_had_OverlapeNoPpm;cpm_had_2d_etaPhi_tt_OverlapNoPpm',
                            title="HAD Overlap CPM Towers but no PPM Towers;;",type='TH2F',
                            cutmask='',path=monCPMinPath,
                            xbins=66,xmin=-3.3,xmax=3.3,ybins=64,ymin=0.,ymax=64.0)


    # FPGA
    myGroup.defineHistogram('loc_PpmEqCpmFpga,loc_fpga_PpmEqCpmFpga;cpm_2d_tt_PpmEqCpmFpga',
                            title="CPM Tower/PPM Tower Non-zero Matches by FPGA;;",type='TH2F',
                            cutmask='',path=monCPMinPath,
                            xbins=56,xmin=0.0,xmax=56.0,ybins=20,ymin=0.,ymax=20.0)

    myGroup.defineHistogram('loc_PpmNeCpmFpga,loc_fpga_PpmNeCpmFpga;cpm_2d_tt_PpmNeCpmFpga',
                            title="CPM Tower/PPM Tower Non-zero Mismatches by FPGA;;",type='TH2F',
                            cutmask='',path=monCPMinPath,
                            xbins=56,xmin=0.0,xmax=56.0,ybins=20,ymin=0.,ymax=20.0)

    myGroup.defineHistogram('loc_PpmNoCpmFpga,loc_fpga_PpmNoCpmFpga;cpm_2d_tt_PpmNoCpmFpga',
                            title="PPM Towers but no CPM Towers by FPGA;;",type='TH2F',
                            cutmask='',path=monCPMinPath,
                            xbins=56,xmin=0.0,xmax=56.0,ybins=20,ymin=0.,ymax=20.0)

    myGroup.defineHistogram('loc_CpmNoPpmFpga,loc_fpga_CpmNoPpmFpga;cpm_2d_tt_CpmNoPpmFpga',
                            title="CPM Towers but no PPM Towers by FPGA;;",type='TH2F',
                            cutmask='',path=monCPMinPath,
                            xbins=56,xmin=0.0,xmax=56.0,ybins=20,ymin=0.,ymax=20.0)


    # RoIs
    myGroup.defineHistogram('emEnerSimEqDataLocX,emEnerSimEqDataLocY;cpm_2d_roi_EmEnergySimEqData',
                            title="CPM RoI EM Energy Data/Simulation Non-zero Matches;;",type='TH2F',
                            cutmask='',path=monRoiPath,
                            xbins=56,xmin=0.,xmax=56.0,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('emEnerSimNeDataLocX,emEnerSimNeDataLocY;cpm_2d_roi_EmEnergySimNeData',
                            title="CPM RoI EM Energy Data/Simulation Non-zero mismatches;;",type='TH2F',
                            cutmask='',path=monRoiPath,
                            xbins=56,xmin=0.,xmax=56.0,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('emEnerSimNoDataLocX,emEnerSimNoDataLocY;cpm_2d_roi_EmEnergySimNoData',
                            title="CPM RoI EM Energy Simulation but no Data;;",type='TH2F',
                            cutmask='',path=monRoiPath,
                            xbins=56,xmin=0.,xmax=56.0,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('emEnerDataNoSimLocX,emEnerDataNoSimLocY;cpm_2d_roi_EmEnergyDataNoSim',
                            title="CPM RoI EM Energy Data but no Simulation;;",type='TH2F',
                            cutmask='',path=monRoiPath,
                            xbins=56,xmin=0.,xmax=56.0,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('tauEnerSimEqDataLocX,tauEnerSimEqDataLocY;cpm_2d_roi_TauEnergySimEqData',
                            title="CPM RoI Tau Energy Data/Simulation Non-zero Matches;;",type='TH2F',
                            cutmask='',path=monRoiPath,
                            xbins=56,xmin=0.,xmax=56.0,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('tauEnerSimNeDataLocX,tauEnerSimNeDataLocY;cpm_2d_roi_TauEnergySimNeData',
                            title="CPM RoI Tau Energy Data/Simulation Non-zero mismatches;;",type='TH2F',
                            cutmask='',path=monRoiPath,
                            xbins=56,xmin=0.,xmax=56.0,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('tauEnerSimNoDataLocX,tauEnerSimNoDataLocY;cpm_2d_roi_TauEnergySimNoData',
                            title="CPM RoI Tau Energy Simulation but no Data;;",type='TH2F',
                            cutmask='',path=monRoiPath,
                            xbins=56,xmin=0.,xmax=56.0,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('tauEnerDataNoSimLocX,tauEnerDataNoSimLocY;cpm_2d_roi_TauEnergyDataNoSim',
                            title="CPM RoI Tau Energy Data but no Simulation;;",type='TH2F',
                            cutmask='',path=monRoiPath,
                            xbins=56,xmin=0.,xmax=56.0,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('roiEtaSimEqData,roiPhiSimEqData;cpm_2d_etaPhi_roi_SimEqData',
                            title="CPM RoI Data/Simulation Non-zero Matches;;",type='TH2F',
                            cutmask='',path=monRoiPath,
                            xbins=66,xmin=-3.3,xmax=3.3,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('roiEtaSimNeData,roiPhiSimNeData;cpm_2d_etaPhi_roi_SimNeData',
                            title="CPM RoI Data/Simulation Non-zero Mismatches;;",type='TH2F',
                            cutmask='',path=monRoiPath,
                            xbins=66,xmin=-3.3,xmax=3.3,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('roiEtaSimNoData,roiPhiSimNoData;cpm_2d_etaPhi_roi_SimNoData',
                            title="CPM RoI Simulation but no Data;;",type='TH2F',
                            cutmask='',path=monRoiPath,
                            xbins=66,xmin=-3.3,xmax=3.3,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('roiEtaDataNoSim,roiPhiDataNoSim;cpm_2d_etaPhi_roi_DataNoSim',
                            title="CPM RoI Data but no Simulation;;",type='TH2F',
                            cutmask='',path=monRoiPath,
                            xbins=66,xmin=-3.3,xmax=3.3,ybins=64,ymin=0.,ymax=64.0)

    # CMX-CP TOBs
    myGroup.defineHistogram('cmxLeftEnerSimEqDataLocX,cmxLeftEnerSimEqDataLocY;cmx_2d_tob_LeftEnergySimEqData',
                            title="CMX TOB Left Energy Data/Simulation Non-zero Matches;;",type='TH2F',
                            cutmask='',path=monCMXinPath,
                            xbins=56,xmin=0.,xmax=56.0,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('cmxRightEnerSimEqDataLocX,cmxRightEnerSimEqDataLocY;cmx_2d_tob_RightEnergySimEqData',
                            title="CMX TOB Right Energy Data/Simulation Non-zero Matches;;",type='TH2F',
                            cutmask='',path=monCMXinPath,
                            xbins=56,xmin=0.,xmax=56.0,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('cmxLeftEnerSimNeDataLocX,cmxLeftEnerSimNeDataLocY;cmx_2d_tob_LeftEnergySimNeData',
                            title="CMX TOB Left Energy Data/Simulation Non-zero Mismatches;;",type='TH2F',
                            cutmask='',path=monCMXinPath,
                            xbins=56,xmin=0.,xmax=56.0,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('cmxRightEnerSimNeDataLocX,cmxRightEnerSimNeDataLocY;cmx_2d_tob_RightEnergySimNeData',
                            title="CMX TOB Right Energy Data/Simulation Non-zero Mismatches;;",type='TH2F',
                            cutmask='',path=monCMXinPath,
                            xbins=56,xmin=0.,xmax=56.0,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('cmxLeftEnerSimNoDataLocX,cmxLeftEnerSimNoDataLocY;cmx_2d_tob_LeftEnergySimNoData',
                            title="CMX TOB Left Energy Simulation but no Data;;",type='TH2F',
                            cutmask='',path=monCMXinPath,
                            xbins=56,xmin=0.,xmax=56.0,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('cmxRightEnerSimNoDataLocX,cmxRightEnerSimNoDataLocY;cmx_2d_tob_RightEnergySimNoData',
                            title="CMX TOB Right Energy Simulation but no Data;;",type='TH2F',
                            cutmask='',path=monCMXinPath,
                            xbins=56,xmin=0.,xmax=56.0,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('cmxLeftEnerDataNoSimLocX,cmxLeftEnerDataNoSimLocY;cmx_2d_tob_LeftEnergyDataNoSim',
                            title="CMX TOB Left Energy Data but no Simulation;;",type='TH2F',
                            cutmask='',path=monCMXinPath,
                            xbins=56,xmin=0.,xmax=56.0,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('cmxRightEnerDataNoSimLocX,cmxRightEnerDataNoSimLocY;cmx_2d_tob_RightEnergyDataNoSim',
                            title="CMX TOB Right Energy Data but no Simulation;;",type='TH2F',
                            cutmask='',path=monCMXinPath,
                            xbins=56,xmin=0.,xmax=56.0,ybins=64,ymin=0.,ymax=64.0)

    #
    myGroup.defineHistogram('cmxLeftIsolSimEqDataLocX,cmxLeftIsolSimEqDataLocY;cmx_2d_tob_LeftIsolSimEqData',
                            title="CMX TOB Left Isolation Data/Simulation Non-zero Matches;;",type='TH2F',
                            cutmask='',path=monCMXinPath,
                            xbins=56,xmin=0.,xmax=56.0,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('cmxRightIsolSimEqDataLocX,cmxRightIsolSimEqDataLocY;cmx_2d_tob_RightIsolSimEqData',
                            title="CMX TOB Right Isolation Data/Simulation Non-zero Matches;;",type='TH2F',
                            cutmask='',path=monCMXinPath,
                            xbins=56,xmin=0.,xmax=56.0,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('cmxLeftIsolSimNeDataLocX,cmxLeftIsolSimNeDataLocY;cmx_2d_tob_LeftIsolSimNeData',
                            title="CMX TOB Left Isolation Data/Simulation Non-zero Mismatches;;",type='TH2F',
                            cutmask='',path=monCMXinPath,
                            xbins=56,xmin=0.,xmax=56.0,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('cmxRightIsolSimNeDataLocX,cmxRightIsolSimNeDataLocY;cmx_2d_tob_RightIsolSimNeData',
                            title="CMX TOB Right Isolation Data/Simulation Non-zero Mismatches;;",type='TH2F',
                            cutmask='',path=monCMXinPath,
                            xbins=56,xmin=0.,xmax=56.0,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('cmxLeftIsolSimNoDataLocX,cmxLeftIsolSimNoDataLocY;cmx_2d_tob_LeftIsolSimNoData',
                            title="CMX TOB Left Isolation Simulation but no Data;;",type='TH2F',
                            cutmask='',path=monCMXinPath,
                            xbins=56,xmin=0.,xmax=56.0,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('cmxRightIsolSimNoDataLocX,cmxRightIsolSimNoDataLocY;cmx_2d_tob_RightIsolSimNoData',
                            title="CMX TOB Right Isolation Simulation but no Data;;",type='TH2F',
                            cutmask='',path=monCMXinPath,
                            xbins=56,xmin=0.,xmax=56.0,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('cmxLeftIsolDataNoSimLocX,cmxLeftIsolDataNoSimLocY;cmx_2d_tob_LeftIsolDataNoSim',
                            title="CMX TOB Left Isolation Data but no Simulation;;",type='TH2F',
                            cutmask='',path=monCMXinPath,
                            xbins=56,xmin=0.,xmax=56.0,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('cmxRightIsolDataNoSimLocX,cmxRightIsolDataNoSimLocY;cmx_2d_tob_RightIsolDataNoSim',
                            title="CMX TOB Right Isolation Data but no Simulation;;",type='TH2F',
                            cutmask='',path=monCMXinPath,
                            xbins=56,xmin=0.,xmax=56.0,ybins=64,ymin=0.,ymax=64.0)

    #
    myGroup.defineHistogram('cmxOverLocXSimEqData,cmxOverCmxSimEqData;cmx_2d_tob_OverflowSimEqData',
                            title="CMX TOB RoI Overflow Bit Data/Simulation Matches;;",type='TH2F',
                            cutmask='',path=monCMXinPath,
                            xbins=56,xmin=0.0,xmax=56.0,ybins=2,ymin=0.,ymax=2.0)

    myGroup.defineHistogram('cmxOverLocXSimNeData,cmxOverCmxSimNeData;cmx_2d_tob_OverflowSimNeData',
                            title="CMX TOB RoI Overflow Bit Data/Simulation Mismatches;;",type='TH2F',
                            cutmask='',path=monCMXinPath,
                            xbins=56,xmin=0.0,xmax=56.0,ybins=2,ymin=0.,ymax=2.0)

    #
    myGroup.defineHistogram('cmxEtaSimEqData,cmxPhiSimEqData;cmx_2d_etaPhi_tob_SimEqData',
                            title="CMX TOB Data/Simulation Non-zero Matches;;",type='TH2F',
                            cutmask='',path=monCMXinPath,
                            xbins=66,xmin=-3.3,xmax=3.3,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('cmxEtaSimNeData,cmxPhiSimNeData;cmx_2d_etaPhi_tob_SimNeData',
                            title="CMX TOB Data/Simulation Non-zero Mismatches;;",type='TH2F',
                            cutmask='',path=monCMXinPath,
                            xbins=66,xmin=-3.3,xmax=3.3,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('cmxEtaSimNoData,cmxPhiSimNoData;cmx_2d_etaPhi_tob_SimNoData',
                            title="CMX TOB Simulation but no Data;;",type='TH2F',
                            cutmask='',path=monCMXinPath,
                            xbins=66,xmin=-3.3,xmax=3.3,ybins=64,ymin=0.,ymax=64.0)

    myGroup.defineHistogram('cmxEtaDataNoSim,cmxPhiDataNoSim;cmx_2d_etaPhi_tob_DataNoSim',
                            title="CMX TOB Data but no Simulation;;",type='TH2F',
                            cutmask='',path=monCMXinPath,
                            xbins=66,xmin=-3.3,xmax=3.3,ybins=64,ymin=0.,ymax=64.0)


    # Local/Remote/Total sums

    myGroup.defineHistogram('cmx_sum_loc_SimEqData;cmx_1d_thresh_SumsSimEqData', 
                            title='CMX Hit Sums Data/Simulation Non-zero Matches',
                            cutmask='',path=monCMXsumsPath,xbins=16,xmin=0.0,xmax=16.0)

    myGroup.defineHistogram('cmx_sum_loc_SimNeData;cmx_1d_thresh_SumsSimNeData', 
                            title='CMX Hit Sums Data/Simulation Non-zero Mismatches',
                            cutmask='',path=monCMXsumsPath,xbins=16,xmin=0.0,xmax=16.0)

    myGroup.defineHistogram('cmx_sum_loc_SimNoData;cmx_1d_thresh_SumsSimNoData', 
                            title='CMX Hit Sums Simulation but no Data',
                            cutmask='',path=monCMXsumsPath,xbins=16,xmin=0.0,xmax=16.0)

    myGroup.defineHistogram('cmx_sum_loc_DataNoSim;cmx_1d_thresh_SumsDataNoSim', 
                            title='CMX Hit Sums Data but no Simulation',
                            cutmask='',path=monCMXsumsPath,xbins=16,xmin=0.0,xmax=16.0)

    #
    myGroup.defineHistogram('cmx_x_leftsums_SimEqData,cmx_y_leftsums_SimEqData;cmx_2d_thresh_LeftSumsSimEqData', 
                            title='CMX Hit Sums Left Data/Simulation Threshold Non-zero Matches',type='TH2F',
                            cutmask='',path=monCMXsumsPath,xbins=8,xmin=0.0,xmax=8.0,ybins=16,ymin=0.0,ymax=16.0,
                            weight='cmx_w_leftsums_SimEqData')

    myGroup.defineHistogram('cmx_x_leftsums_SimNeData,cmx_y_leftsums_SimNeData;cmx_2d_thresh_LeftSumsSimNeData', 
                            title='CMX Hit Sums Left Data/Simulation Threshold Non-zero Mismatches',type='TH2F',
                            cutmask='',path=monCMXsumsPath,xbins=8,xmin=0.0,xmax=8.0,ybins=16,ymin=0.0,ymax=16.0,
                            weight='cmx_w_leftsums_SimNeData')

    myGroup.defineHistogram('cmx_x_rightsums_SimEqData,cmx_y_rightsums_SimEqData;cmx_2d_thresh_RightSumsSimEqData', 
                            title='CMX Hit Sums Right Data/Simulation Threshold Non-zero Matches',type='TH2F',
                            cutmask='',path=monCMXsumsPath,xbins=8,xmin=0.0,xmax=8.0,ybins=16,ymin=0.0,ymax=16.0,
                            weight='cmx_w_rightsums_SimEqData')

    myGroup.defineHistogram('cmx_x_rightsums_SimNeData,cmx_y_rightsums_SimNeData;cmx_2d_thresh_RightSumsSimNeData', 
                            title='CMX Hit Sums Right Data/Simulation Threshold Non-zero Mismatches',type='TH2F',
                            cutmask='',path=monCMXsumsPath,xbins=8,xmin=0.0,xmax=8.0,ybins=16,ymin=0.0,ymax=16.0,
                            weight='cmx_w_rightsums_SimNeData')


    # Topo output information
    """
    m_histTool->setMonGroup(&monCMXTopo);

    m_h_cmx_2d_topo_SimNoData = m_histTool->book2F(
        "cmx_2d_topo_SimNoData", "CMX Topo Output Simulation but no Data", 8, 0,
        8, 3, 0, 3);
    setLabelsTopo(m_h_cmx_2d_topo_SimNoData);
    m_h_cmx_2d_topo_DataNoSim = m_histTool->book2F(
        "cmx_2d_topo_DataNoSim", "CMX Topo Output Data but no Simulation", 8, 0,
        8, 3, 0, 3);
    setLabelsTopo(m_h_cmx_2d_topo_DataNoSim);
    """
    myGroup.defineHistogram('cmxTopoLocXSimNeData,cmxTopoLocYSimNeData;cmx_2d_topo_SimNeData',
                            title="CMX Topo Output Data/Simulation Non-zero Mismatches;;",type='TH2F',
                            cutmask='',path=monCMXTopoPath,
                            xbins=8,xmin=0.0,xmax=8.0,ybins=3,ymin=0.0,ymax=3.0)

    myGroup.defineHistogram('cmxTopoLocXSimEqData,cmxTopoLocYSimEqData;cmx_2d_topo_SimEqData',
                            title="CMX Topo Output Data/Simulation Non-zero Matches;;",type='TH2F',
                            cutmask='',path=monCMXTopoPath,
                            xbins=8,xmin=0.0,xmax=8.0,ybins=3,ymin=0.0,ymax=3.0)

    myGroup.defineHistogram('cmxTopoLocXSimNoData,cmxTopoLocYSimNoData;cmx_2d_topo_SimNoData',
                            title="CMX Topo Output Simulation but no Data;;",type='TH2F',
                            cutmask='',path=monCMXTopoPath,
                            xbins=8,xmin=0.0,xmax=8.0,ybins=3,ymin=0.0,ymax=3.0)

    myGroup.defineHistogram('cmxTopoLocXDataNoSim,cmxTopoLocYDataNoSim;cmx_2d_topo_DataNoSim',
                            title="CMX Topo Output Data but no Simulation;;",type='TH2F',
                            cutmask='',path=monCMXTopoPath,
                            xbins=8,xmin=0.0,xmax=8.0,ybins=3,ymin=0.0,ymax=3.0)


    # Summary
    NumberOfSummaryBins=10
    summary_labels=["EM tt","Had tt","#color[2]{EM RoIs}","#color[2]{Tau RoIs}","#splitline{Left}{TOBs}",
                    "#splitline{Right}{TOBs}","#color[2]{#splitline{Local}{Sums}}","#splitline{Remote}{Sums}","#splitline{Total}{Sums}","#splitline{Topo}{Info}"]
    #
    myGroup.defineHistogram('cpmErrorLoc,cpmError;cpm_2d_SimEqDataOverview',
                            title="CP Transmission/Comparison with Simulation Overview - Events with Matches;;",
                            type='TH2F',
                            cutmask='',path=monExpertPath,
                            xbins=64,xmin=0.,xmax=64.0,
                            ybins=NumberOfSummaryBins,ymin=0.,ymax=NumberOfSummaryBins)

    myGroup.defineHistogram('cpmErrorLoc_SimNeData,cpmError_SimNeData;cpm_2d_SimNeDataOverview',
                            title="CP Transmission/Comparison with Simulation Overview - Events with Mismatches;;",
                            type='TH2F',
                            cutmask='',path=monExpertPath,
                            xbins=64,xmin=0.,xmax=64.0,
                            ybins=NumberOfSummaryBins,ymin=0.,ymax=NumberOfSummaryBins)

    # 1d summary to shiftpath
    myGroup.defineHistogram('cpmErrorSummary;cpm_1d_SimNeDataSummary', 
                            title='CP Transmission/Comparison with Simulation Mismatch Summary;;Events',
                            cutmask='',path=monShiftPath,
                            xbins=NumberOfSummaryBins,xmin=0,xmax=NumberOfSummaryBins,xlabels=summary_labels)




    # Mismatch Event Number Samples        
    myGroup.defineHistogram('em_tt_evtstr,em_tt_y;cpm_em_2d_tt_MismatchEvents',
                            title='CPM Towers EM Mismatch Event Numbers;Events with Error/Mismatch;',type='TH2I',
                            path=monEvent1Path,merge='merge',
                            xbins=1,ybins=56,ymin=0,ymax=56.0)

    myGroup.defineHistogram('had_tt_evtstr,had_tt_y;cpm_had_2d_tt_MismatchEvents',
                            title='CPM Towers Had Mismatch Event Numbers;Events with Error/Mismatch;',type='TH2I',
                            path=monEvent1Path,merge='merge',
                            xbins=1,ybins=56,ymin=0,ymax=56.0)

    myGroup.defineHistogram('em_roi_evtstr,em_roi_y;cpm_2d_roi_EmMismatchEvents',
                            title='CPM RoIs EM Mismatch Event Numbers;Events with Error/Mismatch;',type='TH2I',
                            path=monEvent1Path,merge='merge',
                            xbins=1,ybins=56,ymin=0,ymax=56.0)

    myGroup.defineHistogram('tau_roi_evtstr,tau_roi_y;cpm_2d_roi_TauMismatchEvents',
                            title='CPM RoIs Tau Mismatch Event Numbers;Events with Error/Mismatch;',type='TH2I',
                            path=monEvent1Path,merge='merge',
                            xbins=1,ybins=56,ymin=0,ymax=56.0)

    # monEvent2Path
    myGroup.defineHistogram('cmx_tob_left_evtstr,cmx_tob_left_y;cmx_2d_tob_LeftMismatchEvents',
                            title='CMX TOBs Left Mismatch Event Numbers;Events with Error/Mismatch;',type='TH2I',
                            path=monEvent2Path,merge='merge',
                            xbins=1,ybins=56,ymin=0,ymax=56.0)

    myGroup.defineHistogram('cmx_tob_right_evstr,cmx_tob_right_y;cmx_2d_tob_RightMismatchEvents',
                            title='CMX TOBs Right Mismatch Event Numbers;Events with Error/Mismatch;',type='TH2I',
                            path=monEvent2Path,merge='merge',
                            xbins=1,ybins=56,ymin=0,ymax=56.0)

    thresh_ylabels=["Local 0/0","","","","","","","",
                    "Remote 0/0","","","","","","Total 3/0","",
                    "Topo 0/0","","","","","","",""]
    myGroup.defineHistogram('cmx_thresh_evtstr,cmx_thresh_y;cmx_2d_thresh_SumsMismatchEvents',
                            title='CMX Hit Sums Mismatch Event Numbers;Events with Error/Mismatch;', type='TH2I',
                            path=monEvent2Path,merge='merge',
                            xbins=1,ybins=24,ymin=0,ymax=24.0,ylabels=thresh_ylabels)

    
    acc = helper.result()
    result.merge(acc)
    return result


if __name__=='__main__':
    # For direct tests
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # set debug level for whole job
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO #DEBUG
    log.setLevel(INFO)

    # set input file and config options
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    import glob

    inputs = glob.glob('/eos/atlas/atlascerngroupdisk/data-art/build-output/master/Athena/x86_64-centos7-gcc8-opt/2020-04-06T2139/TrigP1Test/test_trigP1_v1PhysP1_T0Mon_build/ESD.pool.root')

    ConfigFlags.Input.Files = inputs
    ConfigFlags.Output.HISTFileName = 'ExampleMonitorOutput_LVL1.root'

    ConfigFlags.lock()
    ConfigFlags.dump() # print all the configs

    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr.Dump = False

    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesSerialCfg()
    cfg.merge(PoolReadCfg(ConfigFlags))

    CpmSimMonitorCfg = CpmSimMonitoringConfig(ConfigFlags)
    cfg.merge(CpmSimMonitorCfg)

    # message level for algorithm
    CpmSimMonitorCfg.getEventAlgo('CpmSimMonAlg').OutputLevel = 2 # 1/2 INFO/DEBUG
    # options - print all details of algorithms, very short summary 
    cfg.printConfig(withDetails=False, summariseProps = True)

    nevents=-1
    cfg.run(nevents)
