#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
from AthenaMonitoring import AthMonitorCfgHelperOld as AthMonitorCfgHelper
helper = AthMonitorCfgHelper(DQMonFlags, 'TRTMonitoringCfg')

from AthenaConfiguration.ComponentFactory import CompFactory
algTRTMonitoringRun3RAW = helper.addAlgorithm(CompFactory.TRTMonitoringRun3RAW_Alg,
                                           'AlgTRTMonitoringRun3RAW')

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

algTRTMonitoringRun3RAW.TrackSummaryTool = InDetTrackSummaryTool

maxLumiBlock         = 200
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

        rdoStackGroup.defineHistogram('HitWMapS_passed,HitWMapS;hHitWMapS',type='TEfficiency',title='Leading Edge in Time Window: Straws;Straw Number in Stack;Probability per Event',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe])
        rdoStackGroup.defineHistogram('HitTrWMapS_x,HitTrWMapS_y;hHitTrWMapS',type='TProfile',title='Mean Trailing Edge in Time Window: Straws;Straw Number in Stack;Time (ns)',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe])
        rdoStackGroup.defineHistogram('HitTrMapS_x,HitTrMapS_y;hHitTrMapS',type='TProfile',title='Mean Trailing Edge: Straws;Straw Number in Stack;Time (ns)',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe])
        rdoStackGroup.defineHistogram('HitAWMapS_passed,HitAWMapS;hHitAWMapS',type='TEfficiency',title='LL in Time Window: Straws;Straw Number in Stack;Probability',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe])
        rdoStackGroup.defineHistogram('HitAMapS_passed,HitAMapS;hHitAMapS',type='TEfficiency',title='Any LL Bit: Straws;Straw Number in Stack;Probability',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe])
        rdoStackGroup.defineHistogram('StrawOcc_passed,StrawOcc;hOccupancyS',type='TEfficiency',title='Straw Occupancy Distribution: Straws;Occupancy;Number of Straws',path=oss,xbins=201,xmin=0,xmax=1.005)
        rdoStackGroup.defineHistogram('HitToTMapS_x,HitToTMapS_y;hHitToTMapS',type='TProfile',title='Mean ToT: Straws;Straw Number in Stack;Time (ns)',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe])
        rdoStackGroup.defineHistogram('HitToTLongMapS_x,HitToTLongMapS_y;hHitToTLongMapS',type='TProfile',title='Mean ToT for Straws with ToT > LongToTCut: Straws;Straw Number in Stack;Time (ns)',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe])
        rdoStackGroup.defineHistogram('HitToTLongTrMapS_x,HitToTLongTrMapS_y;hHitToTLongTrMapS',type='TProfile',title='Mean Trailing Edge for Straws with ToT > LongToTCut: Straws;Straw Number in Stack;Probability',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe])
        rdoStackGroup.defineHistogram('HitHMapS_passed,HitHMapS;hHitHMapS',type='TEfficiency',title='Any HL Bit: Straws;Straw Number in Stack;Probability',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe])
        rdoStackGroup.defineHistogram('HitHWMapS_passed,HitHWMapS;hHitHWMapS',type='TEfficiency',title='HL in Time Window: Straws;Straw Number in Stack;Probability',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe])
        rdoStackGroup.defineHistogram('HtoLMapS_passed,HtoLMapS;hHtoLMapS',type='TEfficiency',title='HL/LL Ratio: Straws;Straw Number in Stack;Probability',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe])

        rdoStackGroup.defineHistogram('HitWMapC_passed,HitWMapC;hHitWMapC',type='TEfficiency',title='Leading Edge in Time Window: Chips;Chip Number in Stack;Probability',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe])
        rdoStackGroup.defineHistogram('HitTrMapC_x,HitTrMapC_y;hHitTrMapC',type='TProfile',title='Mean Trailing Edge: Chips;Chip Number in Stack;Time (ns)',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe])
        rdoStackGroup.defineHistogram('HitTrWMapC_x,HitTrWMapC_y;hHitTrWMapC',type='TProfile',title='Mean Trailing Edge in Time Window: Chips;Chip Number in Stack;Time (ns)',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe])
        rdoStackGroup.defineHistogram('HitAWMapC_passed,HitAWMapC;hHitAWMapC',type='TEfficiency',title='LL in Time Window: Chips;Chip Number in Stack;Probability',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe])
        rdoStackGroup.defineHistogram('HitAMapC_passed,HitAMapC;hHitAMapC',type='TEfficiency',title='Any LL Bit: Chips;Chip Number in Stack;Probability',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe])
        rdoStackGroup.defineHistogram('OccupancyC_passed,OccupancyC;hChipOcc',type='TEfficiency',title='Chip Occupancy Distribution;Occupancy;Number of Chips',path=oss,xbins=201,xmin=0,xmax=1.005)
        rdoStackGroup.defineHistogram('HitToTMapC_x,HitToTMapC_y;hHitToTMapC',type='TProfile',title='Mean ToT: Chips;Chip Number in Stack;Time (ns)',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe])
        rdoStackGroup.defineHistogram('HitHMapC_passed,HitHMapC;hHitHMapC',type='TEfficiency',title='Any HL Bit: Chips;Chip Number in Stack;Probability',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe])
        rdoStackGroup.defineHistogram('HitHWMapC_passed,HitHWMapC;hHitHWMapC',type='TEfficiency',title='HL in Time Window: Chips;Chip Number in Stack;Probability',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe])
        rdoStackGroup.defineHistogram('HtoLMapC_passed,HtoLMapC;hHtoLMapC',type='TEfficiency',title='HL/LL Ratio: Chips;Chip Number in Stack;Probability',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe])
        rdoStackGroup.defineHistogram('HtoBCMapC_x,HtoBCMapC_y;hHtoBCMapC',type='TH2F',title='HL in BC: Chips;Bunch Crossing ID;Chip Number in Stack',path=oss,xbins=3,xmin=0,xmax=3,ybins=iChipMax[ibe],ymin=0,ymax=iChipMax[ibe])

        if   ibe == 0: rdoStackGroup.defineHistogram('HtoBCMapB_x,HtoBCMapB_y;hHtoBCMapB',type='TH2F',title='HL in BC: Boards;Bunch Crossing ID;Board Number in Stack',path=oss,xbins=3,xmin=0,xmax=3,ybins=9,ymin=0,ymax=9)
        elif ibe == 1: rdoStackGroup.defineHistogram('HtoBCMapB_x,HtoBCMapB_y;hHtoBCMapB',type='TH2F',title='HL in BC: Boards;Bunch Crossing ID;Board Number in Stack',path=oss,xbins=3,xmin=0,xmax=3,ybins=20,ymin=-0.5,ymax=19.5)

### Registering Collisions Histograms ###
rdoShiftSmryGroup = helper.addGroup(algTRTMonitoringRun3RAW,'RDOShiftSmryHistograms')

sumLabels = ['Events', 'Tracks Total', 'Tracks BA', 'Tracks BA', 'Tracks BC', 'Tracks EA', 'Tracks EC', 'Transition Side A', 'Transition Side C']
rdoShiftSmryGroup.defineHistogram('Summary;hSummary',weight='SummaryWeight',type='TH1F',title='Run Summary;;Entries',path='TRT/Shift/Summary',xbins=8,xmin=0,xmax=8,xlabels=sumLabels) 

# Luminosity summary histograms
rdoShiftSmryGroup.defineHistogram('IntLum_passed,IntLum;IntLum',type='TEfficiency',title='Luminosity;;Luminosity [#mub^{1}]',path='TRT/Shift/Summary',xbins=1,xmin=0.,xmax=1.)
rdoShiftSmryGroup.defineHistogram('LBvsLum_passed,LBvsLum;LBvsLum',type='TEfficiency',title='Luminosity;Luminosity Bin;Luminosity [#mub^{1}]',path='TRT/Shift/Summary',xbins=2000,xmin=0.,xmax=2000.)
rdoShiftSmryGroup.defineHistogram('LBvsTime_passed,LBvsTime;LBvsTime',type='TEfficiency',title='Time;Luminosity Bin;Time [s]',path='TRT/Shift/Summary',xbins=2000,xmin=0.,xmax=2000.)

for ibe in range(2):
    for iside in range(2):
        regionTag = ' (' + beId[ibe] + sideId[iside] + ')'
        rdoShiftSmryRebinnedGroup = helper.addGroup(algTRTMonitoringRun3RAW,'RDOShiftSmryRebinnedHistograms{0}{1}'.format(ibe,iside))
        rdoShiftSmryRebinnedGroup.defineHistogram('ChipBSErrorsVsLB_x,ChipBSErrorsVsLB_y;hChipBSErrorsVsLB{0}{1}'.format(beId[ibe],sideId[iside]),type='TProfile',title='Chip Bytestream Errors vs LB{0};Luminosity Block;Fraction of Chips with Errors'.format(regionTag),path='TRT/Shift/Summary',xbins=maxLumiBlock + 1,xmin=-0.5,xmax=maxLumiBlock + 0.5)
        #CAN_REBIN(m_hChipBSErrorsVsLB[ibe][iside]);
        rdoShiftSmryRebinnedGroup.defineHistogram('RobBSErrorsVsLB_x,RobBSErrorsVsLB_y;hRobBSErrorsVsLB{0}{1}'.format(beId[ibe],sideId[iside]),type='TProfile',title='Rob Bytestream Errors vs LB{0};Luminosity Block;Fraction of RODs with Errors'.format(regionTag),path='TRT/Shift/Summary',xbins=maxLumiBlock + 1,xmin=-0.5,xmax=maxLumiBlock + 0.5)
        #CAN_REBIN(m_hRobBSErrorsVsLB[ibe][iside]);

# Barrel/Endcap Histograms
for ibe in range(2):
    regionTag = ' (' + barrelOrEndcap[ibe] + ')'
    rdoGroup = helper.addGroup(algTRTMonitoringRun3RAW,'RDOHistograms{0}'.format(ibe))
    rdoGroup.defineHistogram('BCIDvsOcc_x,BCIDvsOcc_y;hBCIDvsOcc',type='TProfile',title='Avg. Occupancy vs BCID{0};Bunch Crossing ID;Occupancy'.format(regionTag),path='TRT/{0}/Expert'.format(barrelOrEndcap[ibe]),xbins=3564,xmin=0,xmax=3564)
    if ibe == 0:
        rdoGroup.defineHistogram('HitWMap_passed,HitWMap;hHitWMap',type='TEfficiency',title='Leading Edge in Time Window: Xenon Straws (Barrel);Straw Number in Stack;Probability',path='TRT/Shift/{0}'.format(str(barrelOrEndcap[ibe])),xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe])
        rdoGroup.defineHistogram('HitWMap_Ar_passed,HitWMap_Ar;hHitWMap_Ar',type='TEfficiency',title='Leading Edge in Time Window: Argon Straws (Barrel);Straw Number in Stack;Probability',path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe])
        rdoGroup.defineHistogram('OccAll;hOccAll',type='TH1F',title='Occupancy per Event;Occupancy;Events',path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=400,xmin=0.0,xmax=1.0)
    elif ibe == 1:
        for iside in range(2):
            side = ('A', 'C')
            rdoEndcapGroup = helper.addGroup(algTRTMonitoringRun3RAW,'RDOHistograms1{0}'.format(iside))
            rdoEndcapGroup.defineHistogram('HitWMap_passed,HitWMap;hHitWMap_{0}'.format(side[iside]),type='TEfficiency',title='Leading Edge in Time Window: Xenon Straws (E{0});Straw Number in Stack;Probability'.format(side[iside]),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe])
            rdoEndcapGroup.defineHistogram('HitWMap_Ar_passed,HitWMap_Ar;hHitWMap_Ar_{0}'.format(side[iside]),type='TEfficiency',title='Leading Edge in Time Window: Argon Straws (E{0});Straw Number in Stack;Probability'.format(side[iside]),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe])   
    for iside in range(2):
        regionTag = ' (' + beId[ibe] + sideId[iside] + ')'
        regionMarker = (beId[ibe] + sideId[iside]) if athenaCommonFlags.isOnline is True else (sideId[iside])
        rdoLLHLOccGroup = helper.addGroup(algTRTMonitoringRun3RAW,'RDOLLHLOccHistograms{0}{1}'.format(ibe,iside))
        rdoLLHLOccGroup.defineHistogram('AvgHLOcc_side_x,AvgHLOcc_side_y;hAvgHLOcc_{0}'.format(regionMarker),type='TProfile',title='Avg. HL Occupancy{0};{1};Occupancy'.format(regionTag,stackOrSector[ibe]),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=32,xmin=1,xmax=33)
        rdoLLHLOccGroup.defineHistogram('AvgLLOcc_side_x,AvgLLOcc_side_y;hAvgLLOcc_{0}'.format(regionMarker),type='TProfile',title='Avg. LL Occupancy{0};{1};Occupancy'.format(regionTag,stackOrSector[ibe]),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=32,xmin=1,xmax=33)
        rdoLLHLOccGroup.defineHistogram('AvgLLOccMod_side_x,AvgLLOccMod_side_y;hAvgLLOccMod_{0}'.format(regionMarker),type='TProfile',title='Avg. LL Occupancy: {0}s{1};{2};Occupancy'.format(moduleOrWheel[ibe],regionTag,moduleNumAssign[ibe]),path='TRT/{0}/Expert'.format(barrelOrEndcap[ibe]),xbins=moduleNum[ibe],xmin=0,xmax=moduleNum[ibe])
        rdoLLHLOccGroup.defineHistogram('AvgHLOccMod_side_x,AvgHLOccMod_side_y;hAvgHLOccMod_{0}'.format(regionMarker),type='TProfile',title='Avg. HL Occupancy: {0}s{1};{2};Occupancy'.format(moduleOrWheel[ibe],regionTag,moduleNumAssign[ibe]),path='TRT/{0}/Expert'.format(barrelOrEndcap[ibe]),xbins=moduleNum[ibe],xmin=0,xmax=moduleNum[ibe])

effGroup = helper.addGroup(algTRTMonitoringRun3RAW,'TRTEfficiencyHistograms')
effGroup.defineHistogram('Efficiency_eta_passed,Efficiency_eta;hefficiency_eta',type='TEfficiency',title='Efficiency vs #eta;#eta;Efficiency',path='TRT/Efficiency',xbins=50,xmin=-2.8,xmax=2.8)
effGroup.defineHistogram('Efficiency_phi_passed,Efficiency_phi;hefficiency_phi',type='TEfficiency',title='Efficiency vs #phi;#phi (deg);Efficiency',path='TRT/Efficiency',xbins=50,xmin=-3.2,xmax=3.2)
effGroup.defineHistogram('Efficiency_pt_passed,Efficiency_pt;hefficiency_pt',type='TEfficiency',title='Efficiency vs pT;pT (GeV);Efficiency',path='TRT/Efficiency',xbins=50,xmin=0,xmax=10)
effGroup.defineHistogram('Efficiency_z0_passed,Efficiency_z0;hefficiency_z0',type='TEfficiency',title='Efficiency vs z0;z0;Efficiency',path='TRT/Efficiency',xbins=50,xmin=-200,xmax=200)
effGroup.defineHistogram('EfficiencyBarrel_locR_passed,EfficiencyBarrel_locR;hefficiencyBarrel_locR',type='TEfficiency',title='Efficiency vs Track-to-Wire Distance for Xenon Straws (Barrel);Track-to-Wire Distance (mm);Efficiency',path='TRT/Efficiency',xbins=50,xmin=-2.5,xmax=2.5)
effGroup.defineHistogram('EfficiencyBarrel_locR_Ar_passed,EfficiencyBarrel_locR_Ar;hefficiencyBarrel_locR_Ar',type='TEfficiency',title='Efficiency vs Track-to-Wire Distance for Argon Straws (Barrel);Track-to-Wire Distance (mm);Efficiency',path='TRT/Efficiency',xbins=50,xmin=-2.5,xmax=2.5)
effBarrelGroup = helper.addGroup(algTRTMonitoringRun3RAW,'TRTEfficiencyHistogramsBarrel')
effBarrelGroup.defineHistogram('EfficiencyBarrelMap_passed,EfficiencyBarrelMap;hEfficiencyBarrelMap',type='TEfficiency',title='Straw Efficiency Map (Barrel);Straw Number;Efficiency',path='TRT/Efficiency',xbins=strawMax[0],xmin=0,xmax=strawMax[0])
effEndcapGroup = helper.addGroup(algTRTMonitoringRun3RAW,'TRTEfficiencyHistogramsEndCap')
effEndcapGroup.defineHistogram('EfficiencyEndCapMap_passed,EfficiencyEndCapMap;hEfficiencyEndCapMap',type='TEfficiency',title='Straw Efficiency Map (Endcap);Straw Number;Efficiency',path='TRT/Efficiency',xbins=strawMax[1],xmin=0,xmax=strawMax[1])

for iside in range(2):
    regionTag = ' (' + beId[1]   + sideId[iside] + ')'
    effTRTEndcapACGroup = helper.addGroup(algTRTMonitoringRun3RAW,'TRTEfficiencyHistogramsEndCap{0}'.format(iside))
    effTRTEndcapACGroup.defineHistogram('EfficiencyEndCap_locR_passed,EfficiencyEndCap_locR;hEfficiencyEndCap{0}_locR'.format(sideId[iside]),type='TEfficiency',title='Efficiency vs Track-to-Wire Distance for Xenon Straws{0};Track-to-Wire Distance (mm);Efficiency'.format(regionTag),path='TRT/Efficiency',xbins=50,xmin=-2.5,xmax=2.5)
    effTRTEndcapACGroup.defineHistogram('EfficiencyEndCap_locR_Ar_passed,EfficiencyEndCap_locR_Ar;hEfficiencyEndCap{0}_locR_Ar'.format(sideId[iside]),type='TEfficiency',title='Efficiency vs Track-to-Wire Distance for Argon Straws{0};Track-to-Wire Distance (mm);Efficiency'.format(regionTag),path='TRT/Efficiency',xbins=50,xmin=-2.5,xmax=2.5)

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
            effStrawChipGroup.defineHistogram('EfficiencyS_passed,EfficiencyS;hHitEfficiencyS',type='TEfficiency',title='Straw Efficiency Map;Straw Number;Efficiency',path=oss,xbins=strawMax[ibe],xmin=0.5,xmax=strawMax[ibe])
            effStrawChipGroup.defineHistogram('EfficiencyC_passed,EfficiencyC;hHitEfficiencyC',type='TEfficiency',title='Chip Efficiency Map;Chip Number;Efficiency',  path=oss,xbins=iChipMax[ibe],xmin=0.5,xmax=iChipMax[ibe])
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

            trackGroup.defineHistogram('HitWonTMapS_passed,HitWonTMapS;hHitWonTMapS',type='TEfficiency',title='Leading Edge on Track in Time Window: Straws;Straw Number in Stack;Probability',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe])
            trackGroup.defineHistogram('HitAonTMapS_passed,HitAonTMapS;hHitAonTMapS',type='TEfficiency',title='Any LL Bit on Track: Straws;Straw Number in Stack;Probability',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe])
            trackGroup.defineHistogram('EfficiencyS_passed,EfficiencyS;hStrawsEff',type='TEfficiency',title='Straw Efficiency with {0} mm Cut;Straw Number in Stack;Efficiency'.format(distance),path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe])
            trackGroup.defineHistogram('HitHonTMapS_passed,HitHonTMapS;hHitHonTMapS',type='TEfficiency',title='HL Hit on Track: Straws;Straw Number in Stack;Probability',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe])
            trackGroup.defineHistogram('HitHWonTMapS_passed,HitHWonTMapS;hHitHWonTMapS',type='TEfficiency',title='HL Hit(In Time Window) on Track: Straws;Straw Number in Stack;Probability',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe])
            trackGroup.defineHistogram('HitAWonTMapS_passed,HitAWonTMapS;hHitAWonTMapS',type='TEfficiency',title='Any LL Bit on Track in Time Window: Straws;Straw Number in Stack;Probability',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe])
            trackGroup.defineHistogram('HitWonTMapC_passed,HitWonTMapC;hHitWonTMapC',type='TEfficiency',title='Leading Edge on Track in Time Window: Chips;Chip Number in Stack;Probability',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe])
            trackGroup.defineHistogram('HitTronTMapC_x,HitTronTMapC_y;hHitTronTMapC',type='TProfile',title='Mean Trailing Edge on Track: Chips;Chip Number in Stack;Time (ns)',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe])
            trackGroup.defineHistogram('HitAonTMapC_passed,HitAonTMapC;hHitAonTMapC',type='TEfficiency',title='Any LL Bit on Track: Chips;Chip Number in Stack;Probability',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe])
            trackGroup.defineHistogram('EfficiencyC_passed,EfficiencyC;hChipsEff',type='TEfficiency',title='Chip Efficiency with {0} mm Cut;Chip Number in Stack;Efficiency'.format(distance),path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe])
            trackGroup.defineHistogram('HitAWonTMapC_passed,HitAWonTMapC;hHitAWonTMapC',type='TEfficiency',title='Any LL Bit on Track in Time Window: Chips;Chip Number in Stack;Probability',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe])
            trackGroup.defineHistogram('HitHonTMapC_passed,HitHonTMapC;hHitHonTMapC',type='TEfficiency',title='HL Hit on Track: Chips;Chip Number in Stack;Probability',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe])
            trackGroup.defineHistogram('HitHWonTMapC_passed,HitHWonTMapC;hHitHWonTMapC',type='TEfficiency',title='HL Hit(In time Window) on Track: Chips;Chip Number in Stack;Probability',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe])

for ibe in range(2):
    if ibe == 0:
        regionTag = ' (' + barrelOrEndcap[ibe] + ')'
        shiftTrackGroup = helper.addGroup(algTRTMonitoringRun3RAW,'ShiftTRTTrackHistograms0')
        shiftTrackGroup.defineHistogram('HtoLRatioOnTrack_B_Ar;hHtoLRatioOnTrack_Ar',type='TH1F',title='HL/LL Ratio per Reconstructed Track for Argon{0};HL/LL Ratio;Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=50,xmin=0,xmax=1)
        shiftTrackGroup.defineHistogram('HtoLRatioOnTrack_B_Xe;hHtoLRatioOnTrack_Xe',type='TH1F',title='HL/LL Ratio per Reconstructed Track for Xenon{0};HL/LL Ratio;Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=50,xmin=0,xmax=1)
        shiftTrackGroup.defineHistogram('NumSwLLWoT_B;hNumSwLLWoT',type='TH1F',title='Number of Straws with Hits on Track in Time Window{0};Number of LL Hits per Track;Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=150,xmin=0,xmax=150)
        shiftTrackGroup.defineHistogram('HLhitOnTrack_B;hHLhitOnTrack',type='TH1F',title='Number of HL Hits per Reconstructed Track{0};Number of HL Hits per Track;Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=50,xmin=0,xmax=50)
        shiftTrackGroup.defineHistogram('HtoLRatioOnTrack_B;hHtoLRatioOnTrack',type='TH1F',title='HL/LL Ratio per Reconstructed Track for All{0};HL/LL Ratio;Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=50,xmin=0,xmax=1)
        shiftTrackGroup.defineHistogram('HitWonTMap_B;hHitWonTMap',type='TH1F',title='Leading Edge in Time Window per Reconstructed Track{0};Straw Number;Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=strawMax[0],xmin=0,xmax=strawMax[0])
        shiftTrackGroup.defineHistogram('StrawEffDetPhi_B_passed,StrawEffDetPhi_B;hStrawEffDetPhi',type='TEfficiency',title='Straw Efficiency on Track with {0} mm Cut vs #phi(2D){1};Stack;Avg. Straw Efficiency'.format(distance,regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=32,xmin=0,xmax=32)
    elif ibe == 1:
        for iside in range(2):
            regionTag = ' (' + beId[ibe] + sideId[iside] + ')'
            shiftTrackEndcapGroup = helper.addGroup(algTRTMonitoringRun3RAW,'ShiftTRTTrackHistograms1{0}'.format(iside))
            shiftTrackEndcapGroup.defineHistogram('HtoLRatioOnTrack_E_Ar;hHtoLRatioOnTrack_Ar_{0}'.format(sideId[iside]),type='TH1F',title='HL/LL Ratio per Reconstructed Track for Argon{0};HL/LL Ratio;Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=50,xmin=0,xmax=1)
            shiftTrackEndcapGroup.defineHistogram('HtoLRatioOnTrack_E_Xe;hHtoLRatioOnTrack_Xe_{0}'.format(sideId[iside]),type='TH1F',title='HL/LL Ratio per Reconstructed Track for Xenon{0};HL/LL Ratio;Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=50,xmin=0,xmax=1)
            shiftTrackEndcapGroup.defineHistogram('NumSwLLWoT_E;hNumSwLLWoT_{0}'.format(sideId[iside]),type='TH1F',title='Number of Straws with Hits on Track in Time Window{0};Number of LL Hits per Track;Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=150,xmin=0,xmax=150)
            shiftTrackEndcapGroup.defineHistogram('HLhitOnTrack_E;hHLhitOnTrack_{0}'.format(sideId[iside]),type='TH1F',title='Number of HL Hits per Reconstructed Track{0};Number of HL Hits per Track;Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=50,xmin=0,xmax=50)
            shiftTrackEndcapGroup.defineHistogram('HtoLRatioOnTrack_E;hHtoLRatioOnTrack_{0}'.format(sideId[iside]),type='TH1F',title='HL/LL Ratio per Reconstructed Track for All{0};HL/LL Ratio;Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=50,xmin=0,xmax=1)
            shiftTrackEndcapGroup.defineHistogram('HitWonTMap_E;hHitWonTMap_{0}'.format(sideId[iside]),type='TH1F',title='Leading Edge in Time Window per Reconstructed Track{0};Straw Number;Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=strawMax[1],xmin=0,xmax=strawMax[1])
            shiftTrackEndcapGroup.defineHistogram('StrawEffDetPhi_E_passed,StrawEffDetPhi_E;hStrawEffDetPhi_{0}'.format(sideId[iside]),type='TEfficiency',title='Straw Efficiency on Track with {0} mm Cut vs #phi(2D){1};Stack;Avg. Straw Efficiency'.format(distance,regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=32,xmin=0,xmax=32)
### Finished Booking TRT Hits Histograms ###

topSequence += helper.result()
