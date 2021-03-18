#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
def CpmMonitoringConfig(inputFlags):
    '''Function to configure LVL1 Cpm algorithm in the monitoring system.'''

    import math 
    # get the component factory - used for getting the algorithms
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    # make the athena monitoring helper
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'CpmMonitoringCfg')

    # get any algorithms
    CpmMonAlg = helper.addAlgorithm(CompFactory.CpmMonitorAlgorithm,'CpmMonAlg')

    # add any steering
    groupName = 'CpmMonitor' # the monitoring group name is also used for the package name
    CpmMonAlg.PackageName = groupName
    crates = 4
    CpmMonAlg.s_crates = crates
    maxSlices = 5
    CpmMonAlg.s_maxSlices = maxSlices
    isolBits = 5
    CpmMonAlg.s_isolBits = isolBits
    tobsPerCPM = 5
    CpmMonAlg.s_tobsPerCPM = tobsPerCPM
    maxTobsPerCmx = 70
    CpmMonAlg.MaxTOBsPerCMX = maxTobsPerCmx

    # set up the directory structure
    mainDir = 'L1Calo'
    trigPath = 'CPM' # replaces m_rootDir
    errorPath=trigPath+"/Errors/Hardware"
    monShiftPath=errorPath
    monExpertPath=errorPath
    monDetailPath=errorPath+"/Detail/"
    monCPMinputPath=trigPath+"/Input/"
    monRoIPath=trigPath+"/Output/"
    monCMXPath=trigPath+"_CMX/Errors/Hardware/"
    monCMXinPath=trigPath+"_CMX/Input/"
    monCMXoutPath=trigPath+"_CMX/Output/"

    #MonGroup monEvents( this, errorDir + "/Detail", run, attr, "", "eventSample" );
    monEventsPath=errorPath+"/Detail/"

    # add monitoring algorithm to group, with group name and main directory 
    myGroup = helper.addGroup(CpmMonAlg, groupName , mainDir)

    #
    #   CPM Towers - monCPMinputPath
    #
    # Trigger Tower plots - for binning see TrigT1CaloLWHistogramTool::bookPPMEmEtaVsPhi
    etabins_2d=66
    etamin_2d=-3.3
    etamax_2d=3.3
    phibins_2d=64
    phimin_2d=0.0
    phimax_2d=64.0    
    # for 2D histograms x,y;histogram alias
    myGroup.defineHistogram('etaTT,phiTT;ppm_em_2d_etaPhi_tt_Hitmap',title='PPM Trigger Tower EM eta/phi;Tower #eta; Tower #phi',type='TH2F',
                            cutmask='mask_em_TT',path=monCPMinputPath,
                            xbins=etabins_2d,xmin=etamin_2d,xmax=etamax_2d,ybins=phibins_2d,ymin=phimin_2d,ymax=phimax_2d)
    myGroup.defineHistogram('etaTT,phiTT;ppm_had_2d_etaPhi_tt_Hitmap',title='PPM Trigger Tower HAD eta/phi;Tower #eta; Tower #phi',type='TH2F',
                            cutmask='mask_had_TT',path=monCPMinputPath,
                            xbins=etabins_2d,xmin=etamin_2d,xmax=etamax_2d,ybins=phibins_2d,ymin=phimin_2d,ymax=phimax_2d)
    
    # CPMTower plots
    maxEnergyRange = 256 # Maximum energy plotted
    # EM 1d
    myGroup.defineHistogram('etCpmTT_em;cpm_em_1d_tt_Et', title='CPM Tower EM Et;CPM Tower EM Energy;',
                            cutmask='',path=monCPMinputPath,xbins=maxEnergyRange,xmin=0,xmax=maxEnergyRange)
    myGroup.defineHistogram('etaCpmTT_em;cpm_em_1d_tt_Eta', title='CPM Tower EM eta;CPM Tower EM #eta;',
                            cutmask='',path=monCPMinputPath,xbins=50,xmin=-2.5,xmax=2.5)
    myGroup.defineHistogram('phiCpmTT_em;cpm_em_1d_tt_Phi', title='CPM Tower EM phi;CPM Tower EM #phi;',
                            cutmask='',path=monCPMinputPath,xbins=64,xmin=0,xmax=2*math.pi)
    # EM 2d
    myGroup.defineHistogram('etaCpmTT_em,phiScaledCpmTT_em;cpm_em_2d_etaPhi_tt_Hitmap',
                            title='CPM Tower EM eta/phi;Tower #eta; Tower #phi',type='TH2F',
                            cutmask='',path=monCPMinputPath,
                            xbins=etabins_2d,xmin=etamin_2d,xmax=etamax_2d,ybins=phibins_2d,ymin=phimin_2d,ymax=phimax_2d)
    myGroup.defineHistogram('etaCpmTT_em,phiScaledCpmTT_em;cpm_em_2d_etaPhi_tt_EtWeighted',
                            title='CPM Tower EM eta/phi weighted;Tower #eta; Tower #phi',type='TH2F',
                            cutmask='',path=monCPMinputPath,
                            xbins=etabins_2d,xmin=etamin_2d,xmax=etamax_2d,ybins=phibins_2d,ymin=phimin_2d,ymax=phimax_2d, weight="etCpmTT_em")
    # 2d errors monDetailPath
    myGroup.defineHistogram('GLinkParityError,cpmLoc;cpm_2d_Status',
                            title='CPM Sub-status bits;;',type='TH2F',
                            cutmask='',path=monDetailPath,
                            xbins=8,xmin=0.,xmax=8.0,ybins=56,ymin=0.,ymax=56.0)


    # HAD 1d
    myGroup.defineHistogram('etCpmTT_had;cpm_had_1d_tt_Et', title='CPM Tower HAD Et;CPM Tower HAD Energy;',
                            cutmask='',path=monCPMinputPath,xbins=maxEnergyRange,xmin=0,xmax=maxEnergyRange)
    myGroup.defineHistogram('etaCpmTT_had;cpm_had_1d_tt_Eta', title='CPM Tower HAD eta;CPM Tower HAD #eta;',
                            cutmask='',path=monCPMinputPath,xbins=50,xmin=-2.5,xmax=2.5)
    myGroup.defineHistogram('phiCpmTT_had;cpm_had_1d_tt_Phi', title='CPM Tower HAD phi;CPM Tower HAD #phi;',
                            cutmask='',path=monCPMinputPath,xbins=64,xmin=0,xmax=2*math.pi)
    # HAD 2d
    myGroup.defineHistogram('etaCpmTT_had,phiScaledCpmTT_had;cpm_had_2d_etaPhi_tt_Hitmap',
                            title='CPM Tower HAD eta/phi;Tower #eta; Tower #phi',type='TH2F',
                            cutmask='',path=monCPMinputPath,
                            xbins=etabins_2d,xmin=etamin_2d,xmax=etamax_2d,ybins=phibins_2d,ymin=phimin_2d,ymax=phimax_2d)
    myGroup.defineHistogram('etaCpmTT_had,phiScaledCpmTT_had;cpm_had_2d_etaPhi_tt_EtWeighted'
                            ,title='CPM Tower HAD eta/phi weighted;Tower #eta; Tower #phi',type='TH2F',
                            cutmask='',path=monCPMinputPath,
                            xbins=etabins_2d,xmin=etamin_2d,xmax=etamax_2d,ybins=phibins_2d,ymin=phimin_2d,ymax=phimax_2d, weight="etCpmTT_had")

    xbinshist = int(crates * maxSlices)
    myGroup.defineHistogram('sliceCpmTT_tot,peakCpmTT_tot;cpm_2d_tt_Slices'
                            ,title='CPM Slices and Triggered Slice;Crate/Number of Slices;Triggered Slice',type='TH2F',
                            cutmask='',path=monCPMinputPath,
                            xbins=xbinshist,xmin=0,xmax=xbinshist,ybins=maxSlices,ymin=0,ymax=maxSlices)


    #
    # Errors - monDetailPath
    #
    # em - tot means addition of CPM and Overlap containers
    myGroup.defineHistogram('etaCpmTT_em_tot,phiScaledCpmTT_em_tot;cpm_em_2d_etaPhi_tt_Parity'
                            ,title='CPM Tower EM Parity Errors;Tower #eta; Tower #phi',type='TH2F',
                            cutmask='parityErrorCpmTT_em',path=monDetailPath,
                            xbins=etabins_2d,xmin=etamin_2d,xmax=etamax_2d,ybins=phibins_2d,ymin=phimin_2d,ymax=phimax_2d)
    myGroup.defineHistogram('etaCpmTT_em_tot,phiScaledCpmTT_em_tot;cpm_em_2d_etaPhi_tt_LinkDown',
                            title='CPM Tower EM Link Down Errors;Tower #eta; Tower #phi',type='TH2F',
                            cutmask='linkDownErrorCpmTT_em',path=monDetailPath,
                            xbins=etabins_2d,xmin=etamin_2d,xmax=etamax_2d,ybins=phibins_2d,ymin=phimin_2d,ymax=phimax_2d)

    # had
    myGroup.defineHistogram('etaCpmTT_had_tot,phiScaledCpmTT_had_tot;cpm_had_2d_etaPhi_tt_Parity',
                            title='CPM Tower HAD Parity Errors;Tower #eta; Tower #phi',type='TH2F',
                            cutmask='parityErrorCpmTT_had',path=monDetailPath,
                            xbins=etabins_2d,xmin=etamin_2d,xmax=etamax_2d,ybins=phibins_2d,ymin=phimin_2d,ymax=phimax_2d)
    myGroup.defineHistogram('etaCpmTT_had_tot,phiScaledCpmTT_had_tot;cpm_had_2d_etaPhi_tt_LinkDown',
                            title='CPM Tower HAD Link Down Errors;Tower #eta; Tower #phi',type='TH2F',
                            cutmask='linkDownErrorCpmTT_had',path=monDetailPath,
                            xbins=etabins_2d,xmin=etamin_2d,xmax=etamax_2d,ybins=phibins_2d,ymin=phimin_2d,ymax=phimax_2d)


    #
    #  CPM TOB RoIs - monRoIPath
    #
    isolRange=32 # Maximum range for encoded isolation
    myGroup.defineHistogram('energyTobRoIsEner;cpm_1d_roi_EnergyEm', title='CPM TOB RoI Cluster Energy EM;Cluster Energy;',
                            cutmask='mask_tobroi_ener_em',path=monRoIPath,
                            xbins=maxEnergyRange,xmin=0,xmax=maxEnergyRange)
    myGroup.defineHistogram('energyTobRoIsEner;cpm_1d_roi_EnergyTau', title='CPM TOB RoI Cluster Energy Tau;Cluster Energy;',
                            cutmask='mask_tobroi_ener_tau',path=monRoIPath,
                            xbins=maxEnergyRange,xmin=0,xmax=maxEnergyRange)

    myGroup.defineHistogram('energyTobRoIsIsol;cpm_1d_roi_IsolationEm', title='CPM TOB RoI Encoded Isolation Value EM;;',
                            cutmask='mask_tobroi_isol_em',path=monRoIPath,
                            xbins=isolRange,xmin=0,xmax=isolRange)
    myGroup.defineHistogram('energyTobRoIsIsol;cpm_1d_roi_IsolationTau', title='CPM TOB RoI Encoded Isolation Value Tau;;',
                            cutmask='mask_tobroi_isol_tau',path=monRoIPath,
                            xbins=isolRange,xmin=0,xmax=isolRange)


    # bit masks
    myGroup.defineHistogram('bitsTobRoIsIsolEm;cpm_1d_roi_IsolationBitsEm', title='CPM TOB RoI Encoded Isolation Bits EM;Bit;',
                            cutmask='',path=monRoIPath,
                            xbins=isolBits,xmin=0,xmax=isolBits, weight="bitsTobRoIsIsolEmWeight")

    #
    myGroup.defineHistogram('bitsTobRoIsIsolTau;cpm_1d_roi_IsolationBitsTau', title='CPM TOB RoI Encoded Isolation Bits Tau;Bit;',
                            cutmask='',path=monRoIPath,
                            xbins=isolBits,xmin=0,xmax=isolBits, weight="bitsTobRoIsIsolTauWeight")

    # 2D
    # isolation
    myGroup.defineHistogram('etaTobRoIsIsol,phiTobRoIsIsol;cpm_2d_etaPhi_roi_HitmapIsolEm',
                            title='CPM TOB RoIs EM Non-zero Isolation Hit Map;Tower #eta; Tower #phi',type='TH2F',
                            cutmask='mask_tobroi_isol_em',path=monRoIPath,
                            xbins=etabins_2d,xmin=etamin_2d,xmax=etamax_2d,ybins=phibins_2d,ymin=phimin_2d,ymax=phimax_2d)
    myGroup.defineHistogram('etaTobRoIsIsol,phiTobRoIsIsol;cpm_2d_etaPhi_roi_HitmapIsolTau',
                            title='CPM TOB RoIs Tau Non-zero Isolation Hit Map;Tower #eta; Tower #phi',type='TH2F',
                            cutmask='mask_tobroi_isol_tau',path=monRoIPath,
                            xbins=etabins_2d,xmin=etamin_2d,xmax=etamax_2d,ybins=phibins_2d,ymin=phimin_2d,ymax=phimax_2d)

    # energy
    myGroup.defineHistogram('etaTobRoIsEner,phiTobRoIsEner;cpm_2d_etaPhi_roi_HitmapEm',
                            title='CPM TOB RoIs EM Hit Map;Tower #eta; Tower #phi',type='TH2F',
                            cutmask='mask_tobroi_ener_em',path=monRoIPath,
                            xbins=etabins_2d,xmin=etamin_2d,xmax=etamax_2d,ybins=phibins_2d,ymin=phimin_2d,ymax=phimax_2d)
    myGroup.defineHistogram('etaTobRoIsEner,phiTobRoIsEner;cpm_2d_etaPhi_roi_EtWeightedEm',
                            title='CPM TOB RoIs EM Weighted by Energy;Tower #eta; Tower #phi',type='TH2F',
                            cutmask='mask_tobroi_ener_em',path=monRoIPath,
                            xbins=etabins_2d,xmin=etamin_2d,xmax=etamax_2d,ybins=phibins_2d,ymin=phimin_2d,ymax=phimax_2d, weight="energyTobRoIsEner")

    myGroup.defineHistogram('etaTobRoIsEner,phiTobRoIsEner;cpm_2d_etaPhi_roi_HitmapTau',
                            title='CPM TOB RoIs Tau Hit Map;Tower #eta; Tower #phi',type='TH2F',
                            cutmask='mask_tobroi_ener_tau',path=monRoIPath,
                            xbins=etabins_2d,xmin=etamin_2d,xmax=etamax_2d,ybins=phibins_2d,ymin=phimin_2d,ymax=phimax_2d)
    myGroup.defineHistogram('etaTobRoIsEner,phiTobRoIsEner;cpm_2d_etaPhi_roi_EtWeightedTau',
                            title='CPM TOB RoIs Tau Weighted by Energy;Tower #eta; Tower #phi',type='TH2F',
                            cutmask='mask_tobroi_ener_tau',path=monRoIPath,
                            xbins=etabins_2d,xmin=etamin_2d,xmax=etamax_2d,ybins=phibins_2d,ymin=phimin_2d,ymax=phimax_2d, weight="energyTobRoIsEner")

    # TOBs per CPM 
    myGroup.defineHistogram('tobPerCPMEm;cpm_1d_roi_TOBsPerCPMEm', title='CPM TOB RoI TOBs per CPM EM;Number of TOBs;',
                            cutmask='',path=monRoIPath,
                            xbins=tobsPerCPM+1,xmin=1,xmax=tobsPerCPM+2)
    myGroup.defineHistogram('tobPerCPMTau;cpm_1d_roi_TOBsPerCPMTau', title='CPM TOB RoI TOBs per CPM Tau;Number of TOBs;',
                            cutmask='',path=monRoIPath,
                            xbins=tobsPerCPM+1,xmin=1,xmax=tobsPerCPM+2)

    #
    #  CMX-CP TOBs - monCMXinPath
    #
    myGroup.defineHistogram('cmxCpmTobsEnerLeft;cmx_1d_tob_EnergyLeft', title='CMX-CP TOBs Cluster Energy Left CMX',
                            cutmask='',path=monCMXinPath,
                            xbins=maxEnergyRange,xmin=0,xmax=maxEnergyRange)
    myGroup.defineHistogram('cmxCpmTobsEnerRight;cmx_1d_tob_EnergyRight', title='CMX-CP TOBs Cluster Energy Right CMX',
                            cutmask='',path=monCMXinPath,
                            xbins=maxEnergyRange,xmin=0,xmax=maxEnergyRange)
    myGroup.defineHistogram('cmxCpmTobsLeft;cmx_1d_tob_TOBsPerCPMLeft', title='CMX-CP TOBs per CPM Left;Number of TOBs;',
                            cutmask='',path=monCMXinPath,
                            xbins=tobsPerCPM+1,xmin=1,xmax=tobsPerCPM+2)
    myGroup.defineHistogram('cmxCpmTobsRight;cmx_1d_tob_TOBsPerCPMRight', title='CMX-CP TOBs per CPM Right;Number of TOBs;',
                            cutmask='',path=monCMXinPath,
                            xbins=tobsPerCPM+1,xmin=1,xmax=tobsPerCPM+2)

    #
    myGroup.defineHistogram('cmxCpmTobsIsolLeft;cmx_1d_tob_IsolationLeft', title='CMX-CP TOBs Encoded Isolation Left;;',
                            cutmask='', path=monCMXinPath,
                            xbins=isolRange,xmin=0,xmax=isolRange)
    myGroup.defineHistogram('cmxCpmTobsIsolRight;cmx_1d_tob_IsolationRight', title='CMX-CP TOBs Encoded Isolation Right;;',
                            cutmask='', path=monCMXinPath,
                            xbins=isolRange,xmin=0,xmax=isolRange)
    # isolation Bits
    myGroup.defineHistogram('cmxCpmTobsIsolBitsLeft;cmx_1d_tob_IsolationBitsLeft', 
                            title='CMX-CP TOBs Encoded Isolation Bits Left CMX;Bit;',
                            cutmask='', path=monCMXinPath, xbins=isolBits,xmin=0,xmax=isolBits,weight='cmxCpmTobsIsolBitsLeftWeight')
    myGroup.defineHistogram('cmxCpmTobsIsolBitsRight;cmx_1d_tob_IsolationBitsRight', 
                            title='CMX-CP TOBs Encoded Isolation Bits Right CMX;Bit;',
                            cutmask='', path=monCMXinPath, xbins=isolBits,xmin=0,xmax=isolBits,weight='cmxCpmTobsIsolBitsRightWeight')

    # Energy
    myGroup.defineHistogram('cmxCpmTobsEnerXLeft,cmxCpmTobsEnerYLeft;cmx_2d_tob_HitmapLeft',
                            title='CMX-CP TOBs Left CMX Hit Map',type='TH2F',
                            cutmask='',path=monCMXinPath,
                            xbins=56,xmin=0.,xmax=56.0,ybins=64,ymin=0.,ymax=64.)
    myGroup.defineHistogram('cmxCpmTobsEnerXRight,cmxCpmTobsEnerYRight;cmx_2d_tob_HitmapRight',
                            title='CMX-CP TOBs Right CMX Hit Map',type='TH2F',
                            cutmask='',path=monCMXinPath,
                            xbins=56,xmin=0.,xmax=56.0,ybins=64,ymin=0.,ymax=64.)

    #
    myGroup.defineHistogram('cmxCpmTobsIsolXLeft,cmxCpmTobsIsolYLeft;cmx_2d_tob_HitmapIsolLeft',
                            title='CMX-CP TOBs Left CMX Non-zero Isolation Hit Map',type='TH2F',
                            cutmask='',path=monCMXinPath,
                            xbins=56,xmin=0.,xmax=56.0,ybins=64,ymin=0.,ymax=64.)
    myGroup.defineHistogram('cmxCpmTobsIsolXRight,cmxCpmTobsIsolYRight;cmx_2d_tob_HitmapIsolRight',
                            title='CMX-CP TOBs Right CMX Non-zero Isolation Hit Map',type='TH2F',
                            cutmask='',path=monCMXinPath,
                            xbins=56,xmin=0.,xmax=56.0,ybins=64,ymin=0.,ymax=64.)

    # error overflow
    myGroup.defineHistogram('cmxCpmTobsErrorX,cmxCpmTobsErrorCmx;cmx_2d_tob_Overflow',
                            title='CMX-CP TOBs Overflow',type='TH2F',
                            cutmask='',path=monCMXinPath,
                            xbins=56,xmin=0.,xmax=56.0,ybins=2,ymin=0.,ymax=2.)


    # 
    myGroup.defineHistogram('cmxTobsCmxLeft;cmx_1d_tob_TOBsPerCMXLeft', title='CMX-CP TOBs per CMX Left;Number of TOBs;',
                            cutmask='',path=monCMXinPath,
                            xbins=maxTobsPerCmx,xmin=0,xmax=maxTobsPerCmx)
    myGroup.defineHistogram('cmxTobsCmxRight;cmx_1d_tob_TOBsPerCMXRight', title='CMX-CP TOBs per CMX Right;Number of TOBs;',
                            cutmask='',path=monCMXinPath,
                            xbins=maxTobsPerCmx,xmin=0,xmax=maxTobsPerCmx)


    #  CMX error bits
    myGroup.defineHistogram('cmxCpmTobsErrorX,cmxCpmTobsErrorYbase;cmx_2d_tob_Parity',
                            title='CMX-CP TOB Parity Errors;;CMX/Phase',type='TH2F',
                            cutmask='cmxCpmTobsErrorParity',path=monCMXPath,
                            xbins=56,xmin=0.,xmax=56.0,ybins=10,ymin=0.,ymax=10.)

    #
    #  CMX-CP Hits
    #
    xbinsThresh = crates * maxSlices
    myGroup.defineHistogram('cmxCpHitsCrateSlices,cmxCpHitsPeak;cmx_2d_thresh_Slices',
                            title='CMX Slices and Triggered Slice;Crate/Number of Slices;Triggered Slice',type='TH2F',
                            cutmask='',path=monCMXoutPath,
                            xbins=xbinsThresh,xmin=0.,xmax=xbinsThresh,ybins=maxSlices,ymin=0.,ymax=maxSlices)

    #
    myGroup.defineHistogram('cmxCpHitsCrateCmx;cmx_1d_topo_OutputChecksum', title='CMX-CP Topo Output Checksum Non-zero',
                            cutmask='cmxCpHits0TopoCheckSum',path=monCMXoutPath,
                            xbins=8,xmin=0,xmax=8)
    #
    myGroup.defineHistogram('cmxCpMapX,cmxCpMapY;cmx_2d_topo_CPMOccupancyMap',
                            title="CMX-CP Topo CPM Occupancy Maps;;",type='TH2F',
                            cutmask='',path=monCMXoutPath,
                            xbins=14,xmin=1.,xmax=15.0,ybins=8,ymin=0.,ymax=8.0,weight='cmxCpMapHit')

    myGroup.defineHistogram('cmxCpCountsX,cmxCpCountsY;cmx_2d_topo_CPMOccupancyCounts',
                            title="CMX-CP Topo CPM Occupancy Counts Weighted;;",type='TH2F',
                            cutmask='',path=monCMXoutPath,
                            xbins=14,xmin=1.,xmax=15.0,ybins=8,ymin=0.,ymax=8.0,weight='cmxCpCountsHit')

    #
    myGroup.defineHistogram('cmxTopoTobsCpmRight;cmx_1d_topo_TOBsPerCPMRight', title='CMX-CP Topo TOBs per CPM Right CMX;Number of TOBs',
                            cutmask='',path=monCMXoutPath,
                            xbins=7,xmin=1,xmax=8)
    myGroup.defineHistogram('cmxTopoTobsCpmLeft;cmx_1d_topo_TOBsPerCPMLeft', title='CMX-CP Topo TOBs per CPM Left CMX;Number of TOBs',
                            cutmask='',path=monCMXoutPath,
                            xbins=7,xmin=1,xmax=8)

    #
    myGroup.defineHistogram('cmxCpThresBinLeftX,cmxCpThresBinLeftY;cmx_2d_thresh_SumsWeightedLeft',
                            title="CMX-CP Hit Sums Thresholds Weighted Left CMX;;",type='TH2F',
                            cutmask='',path=monCMXoutPath,
                            xbins=8,xmin=0.,xmax=8.0,ybins=16,ymin=0.,ymax=16.0,weight='cmxCpThresBinLeftHit')
    myGroup.defineHistogram('cmxCpThresBinRightX,cmxCpThresBinRightY;cmx_2d_thresh_SumsWeightedRight',
                            title="CMX-CP Hit Sums Thresholds Weighted Right CMX;;",type='TH2F',
                            cutmask='',path=monCMXoutPath,
                            xbins=8,xmin=0.,xmax=8.0,ybins=16,ymin=0.,ymax=16.0,weight='cmxCpThresBinRightHit')

    # 
    myGroup.defineHistogram('cmxCpTopoTobsCmxLeft;cmx_1d_topo_TOBsPerCMXLeft', title='CMX-CP Topo TOBs per CMX Left;Number of TOBs;',
                            cutmask='',path=monCMXoutPath,
                            xbins=maxTobsPerCmx,xmin=0,xmax=maxTobsPerCmx)
    myGroup.defineHistogram('cmxCpTopoTobsCmxRight;cmx_1d_topo_TOBsPerCMXRight', title='CMX-CP Topo TOBs per CMX Right;Number of TOBs;',
                            cutmask='',path=monCMXoutPath,
                            xbins=maxTobsPerCmx,xmin=0,xmax=maxTobsPerCmx)


    #
    #  Error Overview and Summary
    #
    NumberOfSummaryBins=8

    # 2d overview to expert path
    myGroup.defineHistogram('cpmErrorX,cpmErrorY;cpm_2d_ErrorOverview',
                            title="CP Error Overview;;",type='TH2F',
                            cutmask='',path=monExpertPath,
                            xbins=64,xmin=0.,xmax=64.0,ybins=NumberOfSummaryBins,ymin=0.,ymax=NumberOfSummaryBins,weight='')

    # 1d summary to shiftpath
    myGroup.defineHistogram('cpmErrorSummary;cpm_1d_ErrorSummary', title='CP Error Summary;;Events',
                            cutmask='',path=monShiftPath,
                            xbins=NumberOfSummaryBins,xmin=0,xmax=NumberOfSummaryBins)


    EventSamples=10  # Number of Error Event Number Samples
    myGroup.defineHistogram('GLinkParityError,cpmLoc;cpm_2d_ErrorEventNumbers',
                            title='CP Error Event Numbers;Events with Error/Mismatch;ytit',type='TH2I',
                            cutmask='',path=monDetailPath,
                            xbins=EventSamples,xmin=0,xmax=EventSamples,ybins=NumberOfSummaryBins,ymin=0,ymax=NumberOfSummaryBins)

    #
    myGroup.defineHistogram('cpmErrorX,cpmErrorY;cpm_2d_ErrorEventNumbers',
                            title="CP Error Event Numbers;Events with Error/Mismatch;",type='TH2I',
                            cutmask='',path=monEventsPath,
                            xbins=EventSamples,xmin=0,xmax=EventSamples,ybins=NumberOfSummaryBins,ymin=0,ymax=NumberOfSummaryBins)

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

    inputs = glob.glob('/eos/atlas/atlastier0/rucio/data18_13TeV/physics_Main/00357750/data18_13TeV.00357750.physics_Main.recon.ESD.f1073/data18_13TeV.00357750.physics_Main.recon.ESD.f1073._lb0124._SFO-3._0001.1')

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

    CpmMonitorCfg = CpmMonitoringConfig(ConfigFlags)
    cfg.merge(CpmMonitorCfg)

    # message level for algorithm
    CpmMonitorCfg.getEventAlgo('CpmMonAlg').OutputLevel = 2 # 1/2 INFO/DEBUG
    # options - print all details of algorithms, very short summary 
    cfg.printConfig(withDetails=False, summariseProps = True)

    nevents=-1
    cfg.run(nevents)




