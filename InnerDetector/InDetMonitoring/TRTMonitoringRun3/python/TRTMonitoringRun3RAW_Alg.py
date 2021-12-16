#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

def TRTHoleSearchCfg(flags, **kwargs) :
    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory

    acc=ComponentAccumulator()
    kwargs.setdefault('name','TRTTrackHoleSearchTool')
    if 'Extrapolator' not in kwargs :
        extrapolator = acc.popToolsAndMerge( AtlasExtrapolatorCfg(flags) )
        acc.addPublicTool(extrapolator)
        kwargs.setdefault('extrapolator',extrapolator)
    acc.setPrivateTools( CompFactory.TRTTrackHoleSearchTool(**kwargs) )
    return acc

def TRTHoleSearch(name='TRTTrackHoleSearchTool', **kwargs) :
    kwargs.setdefault('name',name)
    if 'Extrapolator' not in kwargs :
        from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
        kwargs.setDefaults(kwargs,Extrapolator = AtlasExtrapolator())

    from TRT_TrackHoleSearch.TRT_TrackHoleSearchConf import TRTTrackHoleSearchTool
    return TRTTrackHoleSearchTool(**kwargs)


def TRTMonitoringRun3RAW_AlgConfig(inputFlags):
    from AthenaConfiguration.ComponentFactory import isRun3Cfg
    if isRun3Cfg():
        from AthenaMonitoring import AthMonitorCfgHelper
        isOnline = inputFlags.Common.isOnline
        from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
        rv = ComponentAccumulator()
    else:
        from AthenaMonitoring import AthMonitorCfgHelperOld as AthMonitorCfgHelper
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        isOnline = athenaCommonFlags.isOnline
    helper = AthMonitorCfgHelper(inputFlags, 'TRTMonitoringCfg')

    from AthenaConfiguration.ComponentFactory import CompFactory
    algTRTMonitoringRun3RAW = helper.addAlgorithm(CompFactory.TRTMonitoringRun3RAW_Alg,
                                                  'AlgTRTMonitoringRun3RAW',
                                                  ByteStreamErrors= "" if inputFlags.Input.isMC else "TRT_ByteStreamErrs",
                                                  TrackSummaryTool= "InDetTrackSummaryTool"
                                              )

    # @TODO really run the TRT hole search ? Hole search still seems to use a condition service
    if isRun3Cfg():
        from InDetConfig.TrackingCommonConfig import TrackSummaryToolCfg
        algTRTMonitoringRun3RAW.TrackSummaryTool = rv.getPrimaryAndMerge(TrackSummaryToolCfg(inputFlags))
        algTRTMonitoringRun3RAW.trt_hole_search= rv.popToolsAndMerge( TRTHoleSearchCfg(inputFlags) )
    else :
        algTRTMonitoringRun3RAW.trt_hole_search=TRTHoleSearch()


    maxLumiBlockSummary  = 3000
    maxLumiBlockShift    = 3000
    numberOfBarrelStacks = 32
    distToStraw          = 0.4
    numberOfStacks       = (32, 32)
    moduleNum            = (96, 64)
    iChipMax             = (104, 240)
    strawMax             = (1642, 3840)
    moduleOrWheel        = ('Module', 'Wheel')
    barrelOrEndcap       = ('Barrel', 'EndCap')
    beId                 = ('B', 'E')
    sideId               = ('A', 'C')
    stackOrSector        = ('Barrel Stack', 'Endcap Sector')
    moduleNumAssign      = ( 'Modules Type 1 (1-32), Type 2 (33-64), Type 3 (65-96)', 'Wheels A (1-32), B (33-64)')

    algTRTMonitoringRun3RAW.strawMax = strawMax
    algTRTMonitoringRun3RAW.iChipMax = iChipMax

    ### Booking TRT RDO Histograms ###
    # ibe = 0 (Barrel), ibe = 1 (Endcap)
    for ibe in range(2):
        for i in range(numberOfStacks[ibe]*2):
            rdoStackGroup = helper.addGroup(algTRTMonitoringRun3RAW,'RDOStackHistograms{0}{1}'.format(ibe,i))
            if ibe == 0:
                if i < numberOfStacks[ibe]:
                    oss = 'TRT/Barrel/Stack' + str(i + 1) + 'A'
                elif i >= numberOfStacks[ibe]:
                    oss = 'TRT/Barrel/Stack' + str(i + 1 - 32) + 'C'
            elif ibe == 1:
                if i < numberOfStacks[ibe]:
                    oss = 'TRT/EndcapA/Sector' + str(i + 1)
                elif i >= numberOfStacks[ibe]:
                    oss = 'TRT/EndcapC/Sector' + str(i + 1 - 32)

            rdoStackGroup.defineHistogram('strawNumber,HitWMapS_passed;hHitWMapS',cutmask='HitWMapS_cut',type='TProfile',title='Leading Edge in Time Window: Straws;Straw Number in Stack;Probability per Event',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe],opt='kAlwaysCreate')
            rdoStackGroup.defineHistogram('strawNumber,HitTrWMapS_y;hHitTrWMapS',cutmask='HitTrWMapS_cut',type='TProfile',title='Mean Trailing Edge in Time Window: Straws;Straw Number in Stack;Time (ns)',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe],opt='kAlwaysCreate')
            rdoStackGroup.defineHistogram('strawNumber,HitTrMapS_y;hHitTrMapS',type='TProfile',title='Mean Trailing Edge: Straws;Straw Number in Stack;Time (ns)',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe],opt='kAlwaysCreate')
            rdoStackGroup.defineHistogram('strawNumber,HitAWMapS_passed;hHitAWMapS',cutmask='HitAWMapS_cut',type='TProfile',title='LL in Time Window: Straws;Straw Number in Stack;Probability',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe],opt='kAlwaysCreate')
            rdoStackGroup.defineHistogram('strawNumber,HitAMapS_passed;hHitAMapS',cutmask='HitAMapS_cut',type='TProfile',title='Any LL Bit: Straws;Straw Number in Stack;Probability',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe],opt='kAlwaysCreate')
            rdoStackGroup.defineHistogram('strawNumber;unscaled_hHitAMapS',cutmask='HitAMapS_cut',type='TH1F',title='Any LL Bit: Straws;Straw Number in Stack;Probability',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe],opt='kAlwaysCreate')
            rdoStackGroup.defineHistogram('strawNumber,HitToTMapS_y;hHitToTMapS',type='TProfile',title='Mean ToT: Straws;Straw Number in Stack;Time (ns)',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe],opt='kAlwaysCreate')
            rdoStackGroup.defineHistogram('strawNumber,HitToTMapS_y;hHitToTLongMapS',cutmask='HitToTLong_cut',type='TProfile',title='Mean ToT for Straws with ToT > LongToTCut: Straws;Straw Number in Stack;Time (ns)',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe],opt='kAlwaysCreate')
            rdoStackGroup.defineHistogram('strawNumber,HitTrMapS_y;hHitToTLongTrMapS',cutmask='HitToTLong_cut',type='TProfile',title='Mean Trailing Edge for Straws with ToT > LongToTCut: Straws;Straw Number in Stack;Time (ns)',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe],opt='kAlwaysCreate')
            rdoStackGroup.defineHistogram('strawNumber,HitHMapS_passed;hHitHMapS',cutmask='HitHMapS_cut',type='TProfile',title='Any HL Bit: Straws;Straw Number in Stack;Probability',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe],opt='kAlwaysCreate')
            rdoStackGroup.defineHistogram('strawNumber,HitHWMapS_passed;hHitHWMapS',cutmask='HitHWMapS_cut',type='TProfile',title='HL in Time Window: Straws;Straw Number in Stack;Probability',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe],opt='kAlwaysCreate')
            rdoStackGroup.defineHistogram('HtoLMapS,HtoLMapS_passed;hHtoLMapS',type='TProfile',title='HL/LL Ratio: Straws;Straw Number in Stack;Probability',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe],opt='kAlwaysCreate')

            rdoStackGroup.defineHistogram('chipNumber,HitWMapC_passed;hHitWMapC',cutmask='HitWMapC_cut',type='TProfile',title='Leading Edge in Time Window: Chips;Chip Number in Stack;Probability',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe],opt='kAlwaysCreate')
            rdoStackGroup.defineHistogram('chipNumber,HitTrMapC_y;hHitTrMapC',type='TProfile',title='Mean Trailing Edge: Chips;Chip Number in Stack;Time (ns)',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe],opt='kAlwaysCreate')
            rdoStackGroup.defineHistogram('chipNumber,HitTrMapC_y;hHitTrWMapC',cutmask='HitTrWMapC_cut',type='TProfile',title='Mean Trailing Edge in Time Window: Chips;Chip Number in Stack;Time (ns)',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe],opt='kAlwaysCreate')
            rdoStackGroup.defineHistogram('chipNumber,HitAWMapC_passed;hHitAWMapC',cutmask='HitAWMapC_cut',type='TProfile',title='LL in Time Window: Chips;Chip Number in Stack;Probability',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe],opt='kAlwaysCreate')
            rdoStackGroup.defineHistogram('chipNumber,HitAMapC_passed;hHitAMapC',cutmask='HitAMapC_cut',type='TProfile',title='Any LL Bit: Chips;Chip Number in Stack;Probability',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe],opt='kAlwaysCreate')
            rdoStackGroup.defineHistogram('chipNumber;unscaled_hHitAMapC',cutmask='HitAMapC_cut',type='TH1F',title='Any LL Bit: Chips;Chip Number in Stack;Probability',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe],opt='kAlwaysCreate')
            rdoStackGroup.defineHistogram('chipNumber,HitToTMapC_y;hHitToTMapC',type='TProfile',title='Mean ToT: Chips;Chip Number in Stack;Time (ns)',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe],opt='kAlwaysCreate')
            rdoStackGroup.defineHistogram('chipNumber,HitHMapC_passed;hHitHMapC',cutmask='HitHMapC_cut',type='TProfile',title='Any HL Bit: Chips;Chip Number in Stack;Probability',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe],opt='kAlwaysCreate')
            rdoStackGroup.defineHistogram('chipNumber,HitHWMapC_passed;hHitHWMapC',cutmask='HitHWMapC_cut',type='TProfile',title='HL in Time Window: Chips;Chip Number in Stack;Probability',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe],opt='kAlwaysCreate')
            rdoStackGroup.defineHistogram('HtoLMapC,HtoLMapC_passed;hHtoLMapC',type='TProfile',title='HL/LL Ratio: Chips;Chip Number in Stack;Probability',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe],opt='kAlwaysCreate')
            rdoStackGroup.defineHistogram('HtoBCMapC_x,chipNumber;hHtoBCMapC',cutmask='HtoBCMap_cut',type='TH2F',title='HL in BC: Chips;Bunch Crossing ID;Chip Number in Stack',path=oss,xbins=3,xmin=0,xmax=3,ybins=iChipMax[ibe],ymin=0,ymax=iChipMax[ibe],opt='kAlwaysCreate')

            if   ibe == 0: rdoStackGroup.defineHistogram('HtoBCMapB_x,HtoBCMapB_y;hHtoBCMapB',type='TH2F',title='HL in BC: Boards;Bunch Crossing ID;Board Number in Stack',path=oss,xbins=3,xmin=0,xmax=3,ybins=9,ymin=0,ymax=9,opt='kAlwaysCreate')
            elif ibe == 1: rdoStackGroup.defineHistogram('HtoBCMapB_x,HtoBCMapB_y;hHtoBCMapB',type='TH2F',title='HL in BC: Boards;Bunch Crossing ID;Board Number in Stack',path=oss,xbins=3,xmin=0,xmax=3,ybins=20,ymin=-0.5,ymax=19.5,opt='kAlwaysCreate')

    ### Registering Collisions Histograms ###
    for ibe in range(2):
        for iside in range(2):
            regionTag = ' (' + beId[ibe] + sideId[iside] + ')'
            rdoShiftSmryRebinnedGroup = helper.addGroup(algTRTMonitoringRun3RAW,'RDOShiftSmryRebinnedHistograms{0}{1}'.format(ibe,iside))
            rdoShiftSmryRebinnedGroup.defineHistogram('ChipBSErrorsVsLB_x,ChipBSErrorsVsLB_y;hChipBSErrorsVsLB_{0}{1}'.format(beId[ibe],sideId[iside]),type='TProfile',title='Chip Bytestream Errors vs LB{0};Luminosity Block;Fraction of Chips with Errors'.format(regionTag),path='TRT/Shift/Summary',xbins=maxLumiBlockSummary + 1,xmin=-0.5,xmax=maxLumiBlockSummary + 0.5,opt='kAlwaysCreate')
            rdoShiftSmryRebinnedGroup.defineHistogram('RobBSErrorsVsLB_x,RobBSErrorsVsLB_y;hRobBSErrorsVsLB_{0}{1}'.format(beId[ibe],sideId[iside]),type='TProfile',title='Rob Bytestream Errors vs LB{0};Luminosity Block;Fraction of RODs with Errors'.format(regionTag),path='TRT/Shift/Summary',xbins=maxLumiBlockSummary + 1,xmin=-0.5,xmax=maxLumiBlockSummary + 0.5,opt='kAlwaysCreate')
            
    # Barrel/Endcap Histograms
    for ibe in range(2):
        regionTag = ' (' + barrelOrEndcap[ibe] + ')'
        rdoGroup = helper.addGroup(algTRTMonitoringRun3RAW,'RDOHistograms{0}'.format(ibe))
        rdoGroup.defineHistogram('BCIDvsOcc_x,BCIDvsOcc_y;hBCIDvsOcc',type='TProfile',title='Avg. Occupancy vs BCID{0};Bunch Crossing ID;Occupancy'.format(regionTag),path='TRT/{0}/Expert'.format(barrelOrEndcap[ibe]),xbins=3564,xmin=0,xmax=3564,opt='kAlwaysCreate')
        if ibe == 0:
            rdoGroup.defineHistogram('strawNumber,HitWMap_passed;hHitWMap',cutmask='isNotAr',type='TProfile',title='Leading Edge in Time Window: Xenon Straws (Barrel);Straw Number in Stack;Probability',path='TRT/Shift/{0}'.format(str(barrelOrEndcap[ibe])),xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe],opt='kAlwaysCreate')
            rdoGroup.defineHistogram('strawNumber,HitWMap_Ar_passed;hHitWMap_Ar',cutmask='isAr',type='TProfile',title='Leading Edge in Time Window: Argon Straws (Barrel);Straw Number in Stack;Probability',path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe],opt='kAlwaysCreate')
            rdoGroup.defineHistogram('OccAll;hOccAll',type='TH1F',title='Occupancy per Event;Occupancy;Events',path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=400,xmin=0.0,xmax=1.0,opt='kAlwaysCreate')
        elif ibe == 1:
            for iside in range(2):
                side = ('A', 'C')
                rdoEndcapGroup = helper.addGroup(algTRTMonitoringRun3RAW,'RDOHistograms1{0}'.format(iside))
                rdoEndcapGroup.defineHistogram('strawNumber,HitWMap_passed;hHitWMap_{0}'.format(side[iside]),cutmask='isNotAr',type='TProfile',title='Leading Edge in Time Window: Xenon Straws (E{0});Straw Number in Stack;Probability'.format(side[iside]),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe],opt='kAlwaysCreate')
                rdoEndcapGroup.defineHistogram('strawNumber,HitWMap_Ar_passed;hHitWMap_Ar_{0}'.format(side[iside]),cutmask='isAr',type='TProfile',title='Leading Edge in Time Window: Argon Straws (E{0});Straw Number in Stack;Probability'.format(side[iside]),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe],opt='kAlwaysCreate')   
        for iside in range(2):
            regionTag = ' (' + beId[ibe] + sideId[iside] + ')'
            regionMarker = (beId[ibe] + sideId[iside]) if isOnline is True else (sideId[iside])
            rdoLLHLOccGroup = helper.addGroup(algTRTMonitoringRun3RAW,'RDOLLHLOccHistograms{0}{1}'.format(ibe,iside))
            rdoLLHLOccGroup.defineHistogram('AvgHLOcc_side_x,AvgHLOcc_side_y;hAvgHLOcc_{0}'.format(regionMarker),type='TProfile',title='Avg. HL Occupancy{0};{1};Occupancy'.format(regionTag,stackOrSector[ibe]),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=32,xmin=1,xmax=33,opt='kAlwaysCreate')
            rdoLLHLOccGroup.defineHistogram('AvgLLOcc_side_x,AvgLLOcc_side_y;hAvgLLOcc_{0}'.format(regionMarker),type='TProfile',title='Avg. LL Occupancy{0};{1};Occupancy'.format(regionTag,stackOrSector[ibe]),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=32,xmin=1,xmax=33,opt='kAlwaysCreate')
            rdoLLHLOccGroup.defineHistogram('AvgLLOccMod_side_x,AvgLLOccMod_side_y;hAvgLLOccMod_{0}'.format(regionMarker),type='TProfile',title='Avg. LL Occupancy: {0}s{1};{2};Occupancy'.format(moduleOrWheel[ibe],regionTag,moduleNumAssign[ibe]),path='TRT/{0}/Expert'.format(barrelOrEndcap[ibe]),xbins=moduleNum[ibe],xmin=0,xmax=moduleNum[ibe],opt='kAlwaysCreate')
            rdoLLHLOccGroup.defineHistogram('AvgHLOccMod_side_x,AvgHLOccMod_side_y;hAvgHLOccMod_{0}'.format(regionMarker),type='TProfile',title='Avg. HL Occupancy: {0}s{1};{2};Occupancy'.format(moduleOrWheel[ibe],regionTag,moduleNumAssign[ibe]),path='TRT/{0}/Expert'.format(barrelOrEndcap[ibe]),xbins=moduleNum[ibe],xmin=0,xmax=moduleNum[ibe],opt='kAlwaysCreate')

    effGroup = helper.addGroup(algTRTMonitoringRun3RAW,'TRTEfficiencyHistograms')
    effGroup.defineHistogram('Efficiency_eta_passed,Efficiency_eta;hEfficiency_eta',type='TProfile',title='Efficiency vs #eta;#eta;Efficiency',path='TRT/Efficiency',xbins=50,xmin=-2.8,xmax=2.8,opt='kAlwaysCreate')
    effGroup.defineHistogram('Efficiency_phi_passed,Efficiency_phi;hEfficiency_phi',type='TProfile',title='Efficiency vs #phi;#phi (deg);Efficiency',path='TRT/Efficiency',xbins=50,xmin=-3.2,xmax=3.2,opt='kAlwaysCreate')
    effGroup.defineHistogram('Efficiency_pt_passed,Efficiency_pt;hEfficiency_pt',type='TProfile',title='Efficiency vs pT;pT (GeV);Efficiency',path='TRT/Efficiency',xbins=250,xmin=0,xmax=50,opt='kAlwaysCreate')
    effGroup.defineHistogram('Efficiency_z0_passed,Efficiency_z0;hEfficiency_z0',type='TProfile',title='Efficiency vs z0;z0;Efficiency',path='TRT/Efficiency',xbins=50,xmin=-200,xmax=200,opt='kAlwaysCreate')
    effGroup.defineHistogram('EfficiencyBarrel_locR,EfficiencyBarrel_locR_passed;hEfficiencyBarrel_locR',type='TProfile',title='Efficiency vs Track-to-Wire Distance for Xenon Straws (Barrel);Track-to-Wire Distance (mm);Efficiency',path='TRT/Efficiency',xbins=50,xmin=-2.5,xmax=2.5,opt='kAlwaysCreate')
    effGroup.defineHistogram('EfficiencyBarrel_locR_Ar,EfficiencyBarrel_locR_Ar_passed;hEfficiencyBarrel_locR_Ar',type='TProfile',title='Efficiency vs Track-to-Wire Distance for Argon Straws (Barrel);Track-to-Wire Distance (mm);Efficiency',path='TRT/Efficiency',xbins=50,xmin=-2.5,xmax=2.5,opt='kAlwaysCreate')
    effBarrelGroup = helper.addGroup(algTRTMonitoringRun3RAW,'TRTEfficiencyHistogramsBarrel')
    effBarrelGroup.defineHistogram('EfficiencyBarrelMap,EfficiencyBarrelMap_passed;hEfficiencyBarrelMap',type='TProfile',title='Straw Efficiency Map (Barrel);Straw Number;Efficiency',path='TRT/Efficiency',xbins=strawMax[0],xmin=0,xmax=strawMax[0],opt='kAlwaysCreate')
    effEndcapGroup = helper.addGroup(algTRTMonitoringRun3RAW,'TRTEfficiencyHistogramsEndCap')
    effEndcapGroup.defineHistogram('EfficiencyEndCapMap,EfficiencyEndCapMap_passed;hEfficiencyEndCapMap',type='TProfile',title='Straw Efficiency Map (Endcap);Straw Number;Efficiency',path='TRT/Efficiency',xbins=strawMax[1],xmin=0,xmax=strawMax[1],opt='kAlwaysCreate')

    for iside in range(2):
        regionTag = ' (' + beId[1]   + sideId[iside] + ')'
        effTRTEndcapACGroup = helper.addGroup(algTRTMonitoringRun3RAW,'TRTEfficiencyHistogramsEndCap{0}'.format(iside))
        effTRTEndcapACGroup.defineHistogram('EfficiencyEndCap_locR,EfficiencyEndCap_locR_passed;hEfficiencyEndCap{0}_locR'.format(sideId[iside]),type='TProfile',title='Efficiency vs Track-to-Wire Distance for Xenon Straws{0};Track-to-Wire Distance (mm);Efficiency'.format(regionTag),path='TRT/Efficiency',xbins=50,xmin=-2.5,xmax=2.5,opt='kAlwaysCreate')
        effTRTEndcapACGroup.defineHistogram('EfficiencyEndCap_locR_Ar,EfficiencyEndCap_locR_Ar_passed;hEfficiencyEndCap{0}_locR_Ar'.format(sideId[iside]),type='TProfile',title='Efficiency vs Track-to-Wire Distance for Argon Straws{0};Track-to-Wire Distance (mm);Efficiency'.format(regionTag),path='TRT/Efficiency',xbins=50,xmin=-2.5,xmax=2.5,opt='kAlwaysCreate')

    for ibe in range(2):
        for iside in range(2):
            regionTag = ' (' + beId[ibe] + sideId[iside] + ')'
            imintmp = numberOfBarrelStacks*iside
            imaxtmp = numberOfBarrelStacks*(iside + 1)
            for i in range(imintmp,imaxtmp):
                if ibe == 0: 
                    oss = 'TRT/Barrel/Stack' + str(i + 1 - 32*iside) + str(sideId[iside])
                elif ibe == 1:
                    oss = 'TRT/Endcap' + str(sideId[iside]) + '/Sector' + str(i + 1 - 32*iside) 
                effStrawChipGroup = helper.addGroup(algTRTMonitoringRun3RAW,'TRTEfficiencyHistograms{0}{1}'.format(ibe,i))
                effStrawChipGroup.defineHistogram('EfficiencyS,EfficiencyS_passed;hHitEfficiencyS',type='TProfile',title='Straw Efficiency Map;Straw Number;Efficiency',path=oss,xbins=strawMax[ibe],xmin=0.5,xmax=strawMax[ibe],opt='kAlwaysCreate')
                effStrawChipGroup.defineHistogram('EfficiencyC,EfficiencyC_passed;hHitEfficiencyC',type='TProfile',title='Chip Efficiency Map;Chip Number;Efficiency',  path=oss,xbins=iChipMax[ibe],xmin=0.5,xmax=iChipMax[ibe],opt='kAlwaysCreate')
    ## Finished Booking TRT RDO Histograms ###

    ### Booking TRT Hits Histograms ###
    for ibe in range(2):
            oss_distance = distToStraw
            distance     = str(oss_distance)
            regionTag    = ' (' + barrelOrEndcap[ibe] + ')'
            for i in range(numberOfStacks[ibe]*2):
                trackGroup = helper.addGroup(algTRTMonitoringRun3RAW,'TRTTrackHistograms{0}{1}'.format(ibe,i))
                nBarrelModules = 0
                if ibe == 0:
                    if i < numberOfStacks[ibe]:
                        nBarrelModules = i + 1
                        oss = 'TRT/Barrel/Stack{0}A'.format(nBarrelModules)
                    elif i >= numberOfStacks[ibe]:
                        nBarrelModules = i + 1 - 32
                        oss = 'TRT/Barrel/Stack{0}C'.format(nBarrelModules)
                elif ibe == 1:
                    if i < numberOfStacks[ibe]:
                        nBarrelModules = i + 1
                        oss = 'TRT/EndcapA/Sector{0}'.format(nBarrelModules)
                    elif i >= numberOfStacks[ibe]:
                        nBarrelModules = i + 1 - 32
                        oss = 'TRT/EndcapC/Sector{0}'.format(nBarrelModules)      

                trackGroup.defineHistogram('HitWonTMapS;unscaled_hHitWonTMapS',type='TH1F',title='Leading Edge on Track in Time Window: Straws;Straw Number in Stack;Probability',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe],opt='kAlwaysCreate')
                trackGroup.defineHistogram('HitAonTMapS;unscaled_hHitAonTMapS',type='TH1F',title='Any LL Bit on Track: Straws;Straw Number in Stack;Probability',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe],opt='kAlwaysCreate')
                trackGroup.defineHistogram('HitHonTMapS;unscaled_hHitHonTMapS',type='TH1F',title='HL Hit on Track: Straws;Straw Number in Stack;Probability',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe],opt='kAlwaysCreate')
                trackGroup.defineHistogram('HitHWonTMapS;unscaled_hHitHWonTMapS',type='TH1F',title='HL Hit(In Time Window) on Track: Straws;Straw Number in Stack;Probability',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe],opt='kAlwaysCreate')
                trackGroup.defineHistogram('HitAWonTMapS;unscaled_hHitAWonTMapS',type='TH1F',title='Any LL Bit on Track in Time Window: Straws;Straw Number in Stack;Probability',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe],opt='kAlwaysCreate')
                trackGroup.defineHistogram('EfficiencyS,EfficiencyS_passed;hEfficiencyS',type='TProfile',title='Straw Efficiency with {0} mm Cut;Straw Number in Stack;Efficiency'.format(distance),path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe],opt='kAlwaysCreate')
                trackGroup.defineHistogram('EfficiencyC,EfficiencyC_passed;hEfficiencyC',type='TProfile',title='Chip Efficiency with {0} mm Cut;Chip Number in Stack;Efficiency'.format(distance),path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe],opt='kAlwaysCreate')
                trackGroup.defineHistogram('HitWonTMapC;unscaled_hHitWonTMapC',type='TH1F',title='Leading Edge on Track in Time Window: Chips;Chip Number in Stack;Probability',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe],opt='kAlwaysCreate')
                trackGroup.defineHistogram('HitAonTMapC;unscaled_hHitAonTMapC',type='TH1F',title='Any LL Bit on Track: Chips;Chip Number in Stack;Probability',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe],opt='kAlwaysCreate')
                trackGroup.defineHistogram('HitAWonTMapC;unscaled_hHitAWonTMapC',type='TH1F',title='Any LL Bit on Track in Time Window: Chips;Chip Number in Stack;Probability',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe],opt='kAlwaysCreate')
                trackGroup.defineHistogram('HitHonTMapC;unscaled_hHitHonTMapC',type='TH1F',title='HL Hit on Track: Chips;Chip Number in Stack;Probability',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe],opt='kAlwaysCreate')
                trackGroup.defineHistogram('HitHWonTMapC;unscaled_hHitHWonTMapC',type='TH1F',title='HL Hit(In time Window) on Track: Chips;Chip Number in Stack;Probability',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe],opt='kAlwaysCreate')
                trackGroup.defineHistogram('HtoLonTMapS,HtoLonTMapS_passed;hHtoLonTMapS',type='TProfile',title='HL/LL Ratio on Track: Straws;Straw Number in Stack;Probability',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe],opt='kAlwaysCreate')
                trackGroup.defineHistogram('HtoLWonTMapS,HtoLWonTMapS_passed;hHtoLWonTMapS',type='TProfile',title='HL/LL (In Time Window) Ratio on Track: Straws;Straw Number in Stack;Probability',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe],opt='kAlwaysCreate')
                trackGroup.defineHistogram('HtoLWonTMapC,HtoLWonTMapC_passed;hHtoLWonTMapC',type='TProfile',title='HL/LL(In Time Window) Ratio on Track: Chips;Chip Number in Stack;Probability',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe],opt='kAlwaysCreate')
                trackGroup.defineHistogram('HtoLonTMapC,HtoLonTMapC_passed;hHtoLonTMapC',type='TProfile',title='HL/LL Ratio on Track: Chips;Chip Number in Stack;Probability',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe],opt='kAlwaysCreate')

    for ibe in range(2):
        if ibe == 0:
            regionTag = ' (' + barrelOrEndcap[ibe] + ')'
            shiftTrackGroup = helper.addGroup(algTRTMonitoringRun3RAW,'ShiftTRTTrackHistograms0')
            shiftTrackGroup.defineHistogram('HtoLRatioOnTrack_B_Ar;hHtoLRatioOnTrack_Ar',type='TH1F',title='HL/LL Ratio per Reconstructed Track for Argon{0};HL/LL Ratio;Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=50,xmin=0,xmax=1,opt='kAlwaysCreate')
            shiftTrackGroup.defineHistogram('HtoLRatioOnTrack_B_Xe;hHtoLRatioOnTrack_Xe',type='TH1F',title='HL/LL Ratio per Reconstructed Track for Xenon{0};HL/LL Ratio;Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=50,xmin=0,xmax=1,opt='kAlwaysCreate')
            shiftTrackGroup.defineHistogram('NumSwLLWoT_B;hNumSwLLWoT',type='TH1F',title='Number of Straws with Hits on Track in Time Window{0};Number of LL Hits per Track;Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=150,xmin=0,xmax=150,opt='kAlwaysCreate')
            shiftTrackGroup.defineHistogram('HLhitOnTrack_B;hHLhitOnTrack',type='TH1F',title='Number of HL Hits per Reconstructed Track{0};Number of HL Hits per Track;Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=50,xmin=0,xmax=50,opt='kAlwaysCreate')
            shiftTrackGroup.defineHistogram('HtoLRatioOnTrack_B;hHtoLRatioOnTrack',type='TH1F',title='HL/LL Ratio per Reconstructed Track for All{0};HL/LL Ratio;Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=50,xmin=0,xmax=1,opt='kAlwaysCreate')
            shiftTrackGroup.defineHistogram('strawNumber,HitWonTMap_B_y;hHitWonTMap',type='TProfile',title='Leading Edge in Time Window per Reconstructed Track{0};Straw Number;Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=strawMax[0],xmin=0,xmax=strawMax[0],opt='kAlwaysCreate')
            shiftTrackGroup.defineHistogram('StrawEffDetPhi_B_passed,StrawEffDetPhi_B;hStrawEffDetPhi',type='TProfile',title='Straw Efficiency on Track with {0} mm Cut vs #phi(2D){1};Stack;Avg. Straw Efficiency'.format(distance,regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=32,xmin=0,xmax=32,opt='kAlwaysCreate')
        elif ibe == 1:
            for iside in range(2):
                regionTag = ' (' + beId[ibe] + sideId[iside] + ')'
                shiftTrackEndcapGroup = helper.addGroup(algTRTMonitoringRun3RAW,'ShiftTRTTrackHistograms1{0}'.format(iside))
                shiftTrackEndcapGroup.defineHistogram('HtoLRatioOnTrack_E_Ar;hHtoLRatioOnTrack_Ar_{0}'.format(sideId[iside]),type='TH1F',title='HL/LL Ratio per Reconstructed Track for Argon{0};HL/LL Ratio;Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=50,xmin=0,xmax=1,opt='kAlwaysCreate')
                shiftTrackEndcapGroup.defineHistogram('HtoLRatioOnTrack_E_Xe;hHtoLRatioOnTrack_Xe_{0}'.format(sideId[iside]),type='TH1F',title='HL/LL Ratio per Reconstructed Track for Xenon{0};HL/LL Ratio;Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=50,xmin=0,xmax=1,opt='kAlwaysCreate')
                shiftTrackEndcapGroup.defineHistogram('NumSwLLWoT_E;hNumSwLLWoT_{0}'.format(sideId[iside]),type='TH1F',title='Number of Straws with Hits on Track in Time Window{0};Number of LL Hits per Track;Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=150,xmin=0,xmax=150,opt='kAlwaysCreate')
                shiftTrackEndcapGroup.defineHistogram('HLhitOnTrack_E;hHLhitOnTrack_{0}'.format(sideId[iside]),type='TH1F',title='Number of HL Hits per Reconstructed Track{0};Number of HL Hits per Track;Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=50,xmin=0,xmax=50,opt='kAlwaysCreate')
                shiftTrackEndcapGroup.defineHistogram('HtoLRatioOnTrack_E;hHtoLRatioOnTrack_{0}'.format(sideId[iside]),type='TH1F',title='HL/LL Ratio per Reconstructed Track for All{0};HL/LL Ratio;Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=50,xmin=0,xmax=1,opt='kAlwaysCreate')
                shiftTrackEndcapGroup.defineHistogram('strawNumber,HitWonTMap_E_y;hHitWonTMap_{0}'.format(sideId[iside]),type='TProfile',title='Leading Edge in Time Window per Reconstructed Track{0};Straw Number;Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=strawMax[1],xmin=0,xmax=strawMax[1],opt='kAlwaysCreate')
                shiftTrackEndcapGroup.defineHistogram('StrawEffDetPhi_E_passed,StrawEffDetPhi_E;hStrawEffDetPhi_{0}'.format(sideId[iside]),type='TProfile',title='Straw Efficiency on Track with {0} mm Cut vs #phi(2D){1};Stack;Avg. Straw Efficiency'.format(distance,regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=32,xmin=0,xmax=32,opt='kAlwaysCreate')
    ### Finished Booking TRT Hits Histograms ###

    for ibe in range(2):
        regionTag = ' (' + barrelOrEndcap[ibe] + ')'
        if ibe == 0:
            rdoShiftRebinnedBarrelGroup = helper.addGroup(algTRTMonitoringRun3RAW,'RDOShiftRebinnedBarrelHistograms0')
            rdoShiftRebinnedBarrelGroup.defineHistogram('NHitsperLB_x,NHitsperLB_y;hNHitsperLB',type='TProfile',title='Avg. Occupancy{0};Luminosity Block;Occupancy'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=maxLumiBlockShift+1,xmin=-0.5,xmax=maxLumiBlockShift+0.5,duration='run',opt='kAlwaysCreate') #CAN_REBIN(m_hNHitsperLB_B);
            rdoShiftRebinnedBarrelGroup.defineHistogram('NHLHitsperLB_x,NHLHitsperLB_y;hNHLHitsperLB',type='TProfile',title='Avg. HL Occupancy{0};Luminosity Block;Occupancy'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=maxLumiBlockShift+1,xmin=-0.5,xmax=maxLumiBlockShift+0.5,duration='run',opt='kAlwaysCreate') #CAN_REBIN(m_hNHLHitsperLB_B);
        elif ibe == 1:
            for iside in range(2):
                regionTag = ' (' + beId[ibe] + sideId[iside] + ')'
                rdoShiftRebinnedEndcapGroup = helper.addGroup(algTRTMonitoringRun3RAW,'RDOShiftRebinnedEndcapHistograms1{0}'.format(iside))
                rdoShiftRebinnedEndcapGroup.defineHistogram('NHitsperLB_x,NHitsperLB_y;hNHitsperLB_{0}'.format(sideId[iside]),type='TProfile',title='Avg. Occupancy{0};Luminosity Block;Occupancy'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=maxLumiBlockShift+1,xmin=-0.5,xmax=maxLumiBlockShift+0.5,duration='run',opt='kAlwaysCreate') #CAN_REBIN(m_hNHitsperLB_E[iside]);
                rdoShiftRebinnedEndcapGroup.defineHistogram('NHLHitsperLB_x,NHLHitsperLB_y;hNHLHitsperLB_{0}'.format(sideId[iside]),type='TProfile',title='Avg. HL Occupancy{0};Luminosity Block;Occupancy'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=maxLumiBlockShift+1,xmin=-0.5,xmax=maxLumiBlockShift+0.5,duration='run',opt='kAlwaysCreate') #CAN_REBIN(m_hNHLHitsperLB_E[iside]);


    if isRun3Cfg():
        rv.merge(helper.result())
        return rv
    else:
        return helper.result()
