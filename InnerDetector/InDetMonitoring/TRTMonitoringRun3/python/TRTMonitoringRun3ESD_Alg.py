#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''@file TRTMonitoringRun3ESD_Alg.py
@author N. Belyaev
@date 20.09.2019
@brief MT-compatible TRT Monitoring Tool for Run III based on the
AthenaMonitoring package
'''


def TRTMonitoringRun3ESD_AlgConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    from TrkConfig.TrackCollectionReadConfig import TrackCollectionReadCfg
    result.merge (TrackCollectionReadCfg (inputFlags, 'CombinedInDetTracks'))
    result.merge (TrackCollectionReadCfg (inputFlags, 'Tracks'))

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags, 'TRTMonitoringCfg')

    from AthenaConfiguration.ComponentFactory import CompFactory
    algTRTMonitoringRun3ESD = helper.addAlgorithm(CompFactory.TRTMonitoringRun3ESD_Alg,
                                               'AlgTRTMonitoringRun3')

    from AtlasGeoModel.AtlasGeoModelConfig import AtlasGeometryCfg
    result.merge(AtlasGeometryCfg(inputFlags))

    from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
    result.merge(addFoldersSplitOnline(inputFlags, "TRT","/TRT/Onl/Calib/errors2d","/TRT/Calib/errors2d",className="TRTCond::RtRelationMultChanContainer"))
    result.merge(addFoldersSplitOnline(inputFlags, "TRT","/TRT/Onl/Calib/slopes","/TRT/Calib/slopes",className="TRTCond::RtRelationMultChanContainer"))
    result.merge(addFoldersSplitOnline(inputFlags, "TRT","/TRT/Onl/Calib/RT","/TRT/Calib/RT",className="TRTCond::RtRelationMultChanContainer"))
    result.merge(addFoldersSplitOnline(inputFlags, "TRT","/TRT/Onl/Calib/T0","/TRT/Calib/T0",className="TRTCond::StrawT0MultChanContainer"))

    ############################## WORKAROUND (START) ##########################
    from SCT_Monitoring.TrackSummaryToolWorkaround import TrackSummaryToolWorkaround
    algTRTMonitoringRun3ESD.TrackSummaryTool = result.popToolsAndMerge(TrackSummaryToolWorkaround(inputFlags))
    ############################## WORKAROUND (END) ############################

    barrelOrEndcap     = ('Barrel', 'EndCap')
    beId                 = ('B', 'E')
    sideId               = ('A', 'C')
    strawMax             = (1642, 3840)
    iChipMax             = (104, 240)
    numberOfStacks       = (32, 32)
    distToStraw          = 0.4
    nPhiBins             = 360
    minTRTHits           = 10

    for ibe in range(2):
        oss_distance = distToStraw
        distance     = str(oss_distance)
        regionTag    = ' (' + barrelOrEndcap[ibe] + ')'

        for i in range(numberOfStacks[ibe]*2):
            trackGroup = helper.addGroup(algTRTMonitoringRun3ESD,'TRTTrackHistograms{0}{1}'.format(ibe,i))

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

            trackGroup.defineHistogram('HitTronTMapS_x,HitTronTMapS_y;hHitTronTMapS',type='TProfile',title='Mean Trailing Edge on Track: Straws;Straw Number in Stack;Time (ns)',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe],duration='run')
            trackGroup.defineHistogram('HitToTonTMapS_x,HitToTonTMapS_y;hHitToTonTMapS',type='TProfile',title='Mean ToT on Track: Straws;Straw Number in Stack;Time (ns)',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe],duration='run')
            trackGroup.defineHistogram('ValidRawDriftTimeonTrkS_x,ValidRawDriftTimeonTrkS_y;hValidRawDriftTimeonTrk',type='TProfile',title='Valid Raw Drift Time on Track: Straws;Straw Number in Stack;Time (ns)',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe],duration='run')
            trackGroup.defineHistogram('HitTronTwEPCMapS_x,HitTronTwEPCMapS_y;hHitTronTwEPCMapS',type='TProfile',title='Mean Trailing Edge on Track (with Event Phase Correction): Straws;Straw Number in Stack;Time (ns)',path=oss,xbins=strawMax[ibe],xmin=0,xmax=strawMax[ibe],duration='run')

            trackGroup.defineHistogram('HitTronTMapC_x,HitTronTMapC_y;hHitTronTMapC',type='TProfile',title='Mean Trailing Edge on Track: Chips;Chip Number in Stack;Time (ns)',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe],duration='run')
            trackGroup.defineHistogram('HitToTonTMapC_x,HitToTonTMapC_y;hHitToTonTMapC',type='TProfile',title='Chip Number in Stack;Time (ns);Chip Number in Stack;Time (ns)',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe],duration='run')
            trackGroup.defineHistogram('ValidRawDriftTimeonTrkC_x,ValidRawDriftTimeonTrkC_y;hValidRawDriftTimeonTrkC',type='TProfile',title='Valid Raw Drift Time on Track: Chips;Chip Number in Stack;Time (ns)',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe],duration='run')
            trackGroup.defineHistogram('HitTronTwEPCMapC_x,HitTronTwEPCMapC_y;hHitTronTwEPCMapC',type='TProfile',title='Mean Trailing Edge on Track (with Event Phase Correction): Chips;Chip Number in Stack;Time (ns)',path=oss,xbins=iChipMax[ibe],xmin=0,xmax=iChipMax[ibe],duration='run')

    # Arrays for Aging
    gas = ('in_A', 'in_B', 'out_A', 'out_B')
    Mod = ('1', '2', '3', 'shortP', 'shortN')

    for ibe in range(2):
        shiftTrackGroup = helper.addGroup(algTRTMonitoringRun3ESD,'ShiftTRTTrackHistograms{0}'.format(ibe))
        regionTag = ' (' + barrelOrEndcap[ibe] + ')'

        if ibe == 0:
            shiftTrackGroup.defineHistogram('EvtPhase;hEvtPhase',type='TH1F',title='Event Phase Correction Factor;Event Phase (ns);Entries',path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=200,xmin=-50,xmax=50)
            shiftTrackGroup.defineHistogram('EvtPhaseVsTrig_x,EvtPhaseVsTrig_y;hEvtPhaseVsTrig',type='TH2F',title='Event Phase vs L1 Trigger Item;Event Phase (ns);L1 Trigger Item',path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=300,xmin=-200,xmax=100,ybins=256,ymin=-0.5,ymax=255.5,duration='run')
            shiftTrackGroup.defineHistogram('EvtPhaseDetPhi_B_x,EvtPhaseDetPhi_B_y;hEvtPhaseDetPhi',type='TProfile',title='Event Phase vs #phi (2D){0};#phi (deg);Event Phase from Tracks per Event'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=nPhiBins,xmin=0.,xmax=360.,duration='run')
            shiftTrackGroup.defineHistogram('RtRelation_B_x,RtRelation_B_y;hrtRelation',type='TH2F',title='R(t) Relation for Xenon Straws{0};Measured Leading Edge (ns);Track-to-Wire Distance (mm)'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=30,xmin=-12.5,xmax=81.25,ybins=50,ymin=0.,ymax=2.5,duration='run')
            shiftTrackGroup.defineHistogram('NumHoTDetPhi_B_x,NumHoTDetPhi_B_y;hNumHoTDetPhi',type='TProfile',title='Number of Hits per Track with {0} mm Cut vs #phi{1};#phi (deg);Hits per Track, TRT Hits >= {2}'.format(distance,regionTag,minTRTHits),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=nPhiBins,xmin=0.,xmax=360.,duration='run')
            shiftTrackGroup.defineHistogram('TronTDist_B;hTronTDist',type='TH1F',title='Trailing Edge Distribution on Track for Xenon Straws{0};Trailing Edge (ns);Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=26,xmin=-0.5,xmax=80.75,duration='run')
            shiftTrackGroup.defineHistogram('DriftTimeonTrkDist_B;hDriftTimeonTrkDist',type='TH1F',title='Drift Time Distribution on Track for Xenon Straws{0};Drift Time (ns);Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=32,xmin=0,xmax=100,duration='run')
            shiftTrackGroup.defineHistogram('NumTrksDetPhi_B;hNumTrksDetPhi_B',type='TH1F',title='Number of Reconstructed Tracks vs #phi (2D){0};#phi (deg);Number of Tracks'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=60,xmin=0,xmax=360,duration='run')

            shiftTrackGroup.defineHistogram('DriftTimeonTrkDist_B_Ar;hDriftTimeonTrkDist_Ar',type='TH1F',title='Drift Time Distribution on Track for Argon Straws{0};Drift Time (ns);Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=32,xmin=0,xmax=100,duration='run')
            shiftTrackGroup.defineHistogram('TronTDist_B_Ar;hTronTDist_Ar',type='TH1F',title='Trailing Edge Distribution on Track for Argon Straws{0};Trailing Edge (ns);Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=26,xmin=-0.5,xmax=80.75,duration='run')
            shiftTrackGroup.defineHistogram('RtRelation_B_Ar_x,RtRelation_B_Ar_y;hrtRelation_Ar',type='TH2F',title='R(t) Relation for Argon Straws{0};Measured Leading Edge (ns);Track-to-Wire Distance (mm)'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=30,xmin=-12.5,xmax=81.25,ybins=50,ymin=0,ymax=2.5,duration='run')
            shiftTrackGroup.defineHistogram('Pull_Biased_Barrel;hPull_Biased_Barrel',type='TH1F',title='Biased Track Pulls for Barrel Hits;Pulls;Entries',path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=200,xmin=-2.5,xmax=2.5,duration='run')
            shiftTrackGroup.defineHistogram('Residual_B_Ar;hResidual_Ar',type='TH1F',title='Residuals for Argon Straws{0};Hit-to-Track Distance (mm);Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=200,xmin=-2.5,xmax=2.5,duration='lowStat')
            shiftTrackGroup.defineHistogram('Residual_B_Ar;hResidual_Ar',type='TH1F',title='Residuals for Argon Straws{0};Hit-to-Track Distance (mm);Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=200,xmin=-2.5,xmax=2.5,duration='run')
            shiftTrackGroup.defineHistogram('Residual_B_Ar_20GeV;hResidual_Ar_20GeV',type='TH1F',title='Residuals for Argon Straws{0} (After 20GeV pT cut);Hit-to-Track Distance (mm);Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=200,xmin=-2.5,xmax=2.5,duration='run')
            shiftTrackGroup.defineHistogram('AvgTroTDetPhi_B_Ar_x,AvgTroTDetPhi_B_Ar_y;hAvgTroTDetPhi_Ar',type='TProfile',title='Avg. Trailing Edge on Track vs #phi (2D) for Argon{0};#phi (deg);Trailing Edge (ns)'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=nPhiBins,xmin=0,xmax=360,duration='run')
            shiftTrackGroup.defineHistogram('TimeResidual_B_Ar;hTimeResidual_Ar',type='TH1F',title='Time Residuals for Argon Straws{0};Time Residual (ns);Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=200,xmin=-20,xmax=20,duration='run')
            shiftTrackGroup.defineHistogram('WireToTrkPosition_B_Ar;hWireToTrkPosition_Ar',type='TH1F',title='Track-to-Wire Distance for Argon{0};Track-to-Wire Distance (mm);Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=100,xmin=-5,xmax=5,duration='run')
            
            shiftTrackGroup.defineHistogram('Residual_B;hResidual_Xe',type='TH1F',title='Residuals for Xenon Straws{0};Hit-to-Track Distance (mm);Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=200,xmin=-2.5,xmax=2.5,duration='lowStat')
            shiftTrackGroup.defineHistogram('Residual_B;hResidual_Xe',type='TH1F',title='Residuals for Xenon Straws{0};Hit-to-Track Distance (mm);Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=200,xmin=-2.5,xmax=2.5,duration='run')
            shiftTrackGroup.defineHistogram('Residual_B_20GeV;hResidual_Xe_20GeV',type='TH1F',title='Residuals for Xenon Straws{0} (After 20GeV pT cut);Hit-to-Track Distance (mm);Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=200,xmin=-2.5,xmax=2.5,duration='run')
            shiftTrackGroup.defineHistogram('TimeResidual_B;hTimeResidual',type='TH1F',title='Time Residuals for Xenon Straws{0};Time Residual (ns);Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=200,xmin=-20,xmax=20,duration='run')
            shiftTrackGroup.defineHistogram('WireToTrkPosition_B;hWireToTrkPosition',type='TH1F',title='Track-to-Wire Distance for Xenon{0};Track-to-Wire Distance (mm);Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=100,xmin=-5,xmax=5,duration='run')
            shiftTrackGroup.defineHistogram('AvgTroTDetPhi_B_x,AvgTroTDetPhi_B_y;hAvgTroTDetPhi',type='TProfile',title='Avg. Trailing Edge on Track vs #phi (2D) for Xenon{0};#phi (deg);Trailing Edge (ns)'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=nPhiBins,xmin=0,xmax=360,duration='run')
        elif ibe == 1:
            shiftTrackGroup.defineHistogram('Pull_Biased_EndCap;hPull_Biased_EndCap',type='TH1F',title='Biased Track Pulls for EndCap Hits;Pulls;Entries',path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=200,xmin=-2.5,xmax=2.5,duration='run')

            for iside in range(2):
                regionTag = ' (' + beId[ibe] + sideId[iside] + ')'
                shiftTrackEndcapGroup = helper.addGroup(algTRTMonitoringRun3ESD,'ShiftTRTTrackHistograms{0}{1}'.format(ibe,iside))
                shiftTrackEndcapGroup.defineHistogram('EvtPhaseDetPhi_E_x,EvtPhaseDetPhi_E_y;hEvtPhaseDetPhi_{0}'.format(sideId[iside]),type='TProfile',title='Event Phase vs #phi (2D){0};#phi (deg);Event Phase from Tracks per Event'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=nPhiBins,xmin=0,xmax=360,duration='run')
                shiftTrackEndcapGroup.defineHistogram('RtRelation_E_x,RtRelation_E_y;hrtRelation_{0}'.format(sideId[iside]),type='TH2F',title='R(t) Relation for Xenon Straws{0};Measured Leading Edge (ns);Track-to-Wire Distance (mm)'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=30,xmin=-12.5,xmax=81.25,ybins=50,ymin=0,ymax=2.5,duration='run')
                shiftTrackEndcapGroup.defineHistogram('NumHoTDetPhi_E_x,NumHoTDetPhi_E_y;hNumHoTDetPhi_{0}'.format(sideId[iside]),type='TProfile',title='Number of Hits per Track with {0} mm Cut vs #phi{1};#phi (deg);Hits per Track, TRT Hits> = {2}'.format(distance,regionTag,minTRTHits),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=nPhiBins,xmin=0,xmax=360,duration='run')
                shiftTrackEndcapGroup.defineHistogram('TronTDist_E;hTronTDist_{0}'.format(sideId[iside]),type='TH1F',title='Trailing Edge Distribution on Track for Xenon Straws{0};Trailing Edge (ns);Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=26,xmin=-0.5,xmax=80.75,duration='run')
                shiftTrackEndcapGroup.defineHistogram('DriftTimeonTrkDist_E;hDriftTimeonTrkDist_{0}'.format(sideId[iside]),type='TH1F',title='Drift Time Distribution on Track for Xenon Straws{0};Drift Time (ns);Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=32,xmin=0,xmax=100,duration='run')
                shiftTrackEndcapGroup.defineHistogram('NumTrksDetPhi_E;hNumTrksDetPhi_{0}'.format(sideId[iside]),type='TH1F',title='Number of Reconstructed Tracks vs #phi (2D){0};#phi (deg);Number of Tracks'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=60,xmin=0,xmax=360,duration='run')
                shiftTrackEndcapGroup.defineHistogram('Residual_E;hResidual_Xe_{0}'.format(sideId[iside]),type='TH1F',title='Residuals for Xenon Straws{0};Hit-to-Track Distance (mm);Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=200,xmin=-2.5,xmax=2.5,duration='lowStat')
                shiftTrackEndcapGroup.defineHistogram('Residual_E;hResidual_Xe_{0}'.format(sideId[iside]),type='TH1F',title='Residuals for Xenon Straws{0};Hit-to-Track Distance (mm);Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=200,xmin=-2.5,xmax=2.5,duration='run')
                shiftTrackEndcapGroup.defineHistogram('Residual_E_20GeV;hResidual_Xe_{0}_20GeV'.format(sideId[iside]),type='TH1F',title='Residuals for Xenon Straws{0} (After 20GeV pT cut);Hit-to-Track Distance (mm);Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=200,xmin=-2.5,xmax=2.5,duration='run')
                shiftTrackEndcapGroup.defineHistogram('TimeResidual_E;hTimeResidual_{0}'.format(sideId[iside]),type='TH1F',title='Time Residuals for Xenon Straws{0};Time Residual (ns);Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=200,xmin=-20,xmax=20,duration='run')

                shiftTrackEndcapGroup.defineHistogram('TronTDist_E_Ar;hTronTDist_Ar_{0}'.format(sideId[iside]),type='TH1F',title='Trailing Edge Distribution on Track for Argon Straws{0};Trailing Edge (ns);Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=26,xmin=-0.5,xmax=80.75,duration='run')
                shiftTrackEndcapGroup.defineHistogram('AvgTroTDetPhi_E_Ar_x,AvgTroTDetPhi_E_Ar_y;hAvgTroTDetPhi_Ar_{0}'.format(sideId[iside]),type='TProfile',title='Avg. Trailing Edge on Track vs #phi (2D) for Argon{0};#phi (deg);Trailing Edge (ns)'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=nPhiBins,xmin=0,xmax=360,duration='run')
                shiftTrackEndcapGroup.defineHistogram('RtRelation_E_Ar_x,RtRelation_E_Ar_y;hrtRelation_Ar_{0}'.format(sideId[iside]),type='TH2F',title='R(t) Relation for Argon Straws{0};Measured Leading Edge (ns);Track-to-Wire Distance (mm)'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=30,xmin=-12.5,xmax=81.25,ybins=50,ymin=0,ymax=2.5,duration='run')
                shiftTrackEndcapGroup.defineHistogram('DriftTimeonTrkDist_E_Ar;hDriftTimeonTrkDist_Ar_{0}'.format(sideId[iside]),type='TH1F',title='Drift Time Distribution on Track for Argon Straws{0};Drift Time (ns);Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=32,xmin=0,xmax=100,duration='run')
                shiftTrackEndcapGroup.defineHistogram('Residual_E_Ar;hResidual_Ar_{0}'.format(sideId[iside]),type='TH1F',title='Residuals for Argon Straws{0};Hit-to-Track Distance (mm);Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=200,xmin=-2.5,xmax=2.5,duration='lowStat')
                shiftTrackEndcapGroup.defineHistogram('Residual_E_Ar;hResidual_Ar_{0}'.format(sideId[iside]),type='TH1F',title='Residuals for Argon Straws{0};Hit-to-Track Distance (mm);Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=200,xmin=-2.5,xmax=2.5,duration='run')
                shiftTrackEndcapGroup.defineHistogram('Residual_E_Ar_20GeV;hResidual_Ar_{0}_20GeV'.format(sideId[iside]),type='TH1F',title='Residuals for Argon Straws{0} (After 20GeV pT cut);Hit-to-Track Distance (mm);Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=200,xmin=-2.5,xmax=2.5,duration='run')
                shiftTrackEndcapGroup.defineHistogram('TimeResidual_E_Ar;hTimeResidual_Ar_{0}'.format(sideId[iside]),type='TH1F',title='Time Residuals for Argon Straws{0};Time Residual (ns);Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=200,xmin=-20,xmax=20,duration='run')
                shiftTrackEndcapGroup.defineHistogram('WireToTrkPosition_E_Ar;hWireToTrkPosition_Ar_{0}'.format(sideId[iside]),type='TH1F',title='Track-to-Wire Distance for Argon{0};Track-to-Wire Distance (mm);Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=100,xmin=-5,xmax=5,duration='run')
                
                shiftTrackEndcapGroup.defineHistogram('WireToTrkPosition_E;hWireToTrkPosition_{0}'.format(sideId[iside]),type='TH1F',title='Track-to-Wire Distance for Xenon{0};Track-to-Wire Distance (mm);Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=100,xmin=-5,xmax=5,duration='run')
                shiftTrackEndcapGroup.defineHistogram('AvgTroTDetPhi_E_x,AvgTroTDetPhi_E_y;hAvgTroTDetPhi_{0}'.format(sideId[iside]),type='TProfile',title='Avg. Trailing Edge on Track vs #phi (2D) for Xenon{0};#phi (deg);Trailing Edge (ns)'.format(regionTag),path='TRT/Shift/{0}'.format(barrelOrEndcap[ibe]),xbins=nPhiBins,xmin=0,xmax=360,duration='run')

        #Initialize Aging plots
        for iL in range(5):
            for iSide in range(2): 
                if ibe == 0: 
                    if iL < 3:
                        agingGroup = helper.addGroup(algTRTMonitoringRun3ESD,'TRTAgingHistograms0{0}{1}'.format(iL,iSide))
                        agingGroup.defineHistogram('Trackz_All;trackz_m{0}_{1}_All'.format(Mod[iL],sideId[iSide]),type='TH1F',title='Number All Hits side {0} Layer {1};z [mm];Number of Hits'.format(sideId[iSide],Mod[iL]),path='TRT/Aging/{0}'.format(barrelOrEndcap[ibe]),xbins=30,xmin=-750.,xmax=750.,duration='run')
                        agingGroup.defineHistogram('Trackz_All;trackz_m{0}_{1}_All'.format(Mod[iL],sideId[iSide]),type='TH1F',title='Number All Hits side {0} Layer {1};z [mm];Number of Hits'.format(sideId[iSide],Mod[iL]),path='TRT/Aging/{0}'.format(barrelOrEndcap[ibe]),xbins=30,xmin=-750.,xmax=750.,duration='lowStat')
                        agingGroup.defineHistogram('Trackz_HT;trackz_m{0}_{1}_HT'.format(Mod[iL],sideId[iSide]),type='TH1F',title='Number HT Hits side {0} Layer {1};z [mm];Number of HT Hits'.format(sideId[iSide],Mod[iL]),path='TRT/Aging/{0}'.format(barrelOrEndcap[ibe]),xbins=30,xmin=-750.,xmax=750.,duration='run')
                        agingGroup.defineHistogram('Trackz_HT;trackz_m{0}_{1}_HT'.format(Mod[iL],sideId[iSide]),type='TH1F',title='Number HT Hits side {0} Layer {1};z [mm];Number of HT Hits'.format(sideId[iSide],Mod[iL]),path='TRT/Aging/{0}'.format(barrelOrEndcap[ibe]),xbins=30,xmin=-750.,xmax=750.,duration='lowStat')
                    if iL == 3:
                        agingGroup = helper.addGroup(algTRTMonitoringRun3ESD,'TRTAgingHistograms03{0}'.format(iSide))
                        agingGroup.defineHistogram('Trackz_All;trackz_m1_{0}_All_{1}'.format(sideId[iSide],Mod[iL]),type='TH1F',title='Number All Hits side {0} Layer 1 {1};z [mm];Number of Hits'.format(sideId[iSide],Mod[iL]),path='TRT/Aging/{0}'.format(barrelOrEndcap[ibe]),xbins=30,xmin=0.,xmax=725.,duration='run')
                        agingGroup.defineHistogram('Trackz_All;trackz_m1_{0}_All_{1}'.format(sideId[iSide],Mod[iL]),type='TH1F',title='Number All Hits side {0} Layer 1 {1};z [mm];Number of Hits'.format(sideId[iSide],Mod[iL]),path='TRT/Aging/{0}'.format(barrelOrEndcap[ibe]),xbins=30,xmin=0.,xmax=725.,duration='lowStat')
                        agingGroup.defineHistogram('Trackz_HT;trackz_m1_{0}_HT_{1}'.format(sideId[iSide],Mod[iL]),type='TH1F',title='Number HT Hits side {0} Layer 1 {1};z [mm];Number of HT Hits'.format(sideId[iSide],Mod[iL]),path='TRT/Aging/{0}'.format(barrelOrEndcap[ibe]),xbins=30,xmin=0.,xmax=725.,duration='run')
                        agingGroup.defineHistogram('Trackz_HT;trackz_m1_{0}_HT_{1}'.format(sideId[iSide],Mod[iL]),type='TH1F',title='Number HT Hits side {0} Layer 1 {1};z [mm];Number of HT Hits'.format(sideId[iSide],Mod[iL]),path='TRT/Aging/{0}'.format(barrelOrEndcap[ibe]),xbins=30,xmin=0.,xmax=725.,duration='lowStat')
                    if iL == 4:
                        agingGroup = helper.addGroup(algTRTMonitoringRun3ESD,'TRTAgingHistograms04{0}'.format(iSide))
                        agingGroup.defineHistogram('Trackz_All;trackz_m1_{0}_All_{1}'.format(sideId[iSide],Mod[iL]),type='TH1F',title='Number All Hits side {0} Layer 1 {1};z [mm];Number of Hits'.format(sideId[iSide],Mod[iL]),path='TRT/Aging/{0}'.format(barrelOrEndcap[ibe]),xbins=30,xmin=-725.,xmax=0.,duration='run')
                        agingGroup.defineHistogram('Trackz_All;trackz_m1_{0}_All_{1}'.format(sideId[iSide],Mod[iL]),type='TH1F',title='Number All Hits side {0} Layer 1 {1};z [mm];Number of Hits'.format(sideId[iSide],Mod[iL]),path='TRT/Aging/{0}'.format(barrelOrEndcap[ibe]),xbins=30,xmin=-725.,xmax=0.,duration='lowStat')
                        agingGroup.defineHistogram('Trackz_HT;trackz_m1_{0}_HT_{1}'.format(sideId[iSide],Mod[iL]),type='TH1F',title='Number HT Hits side {0} Layer 1 {1};z [mm];Number of HT Hits'.format(sideId[iSide],Mod[iL]),path='TRT/Aging/{0}'.format(barrelOrEndcap[ibe]),xbins=30,xmin=-725.,xmax=0.,duration='run')
                        agingGroup.defineHistogram('Trackz_HT;trackz_m1_{0}_HT_{1}'.format(sideId[iSide],Mod[iL]),type='TH1F',title='Number HT Hits side {0} Layer 1 {1};z [mm];Number of HT Hits'.format(sideId[iSide],Mod[iL]),path='TRT/Aging/{0}'.format(barrelOrEndcap[ibe]),xbins=30,xmin=-725.,xmax=0.,duration='lowStat')
                elif ibe == 1:
                    # prevent double booking of histograms here
                    if iL < 4: 
                        agingGroup = helper.addGroup(algTRTMonitoringRun3ESD,'TRTAgingHistograms1{0}{1}'.format(iL,iSide))
                        agingGroup.defineHistogram('Trackr_All;trackr_E{0}_{1}_All'.format(sideId[iSide],gas[iL]),type='TH1F',title='Number All Hits E{0} {1};r [mm];Number of Hits'.format(sideId[iSide],gas[iL]),path='TRT/Aging/{0}'.format(barrelOrEndcap[ibe]),xbins=30,xmin=644.,xmax=1004.,duration='run')
                        agingGroup.defineHistogram('Trackr_All;trackr_E{0}_{1}_All'.format(sideId[iSide],gas[iL]),type='TH1F',title='Number All Hits E{0} {1};r [mm];Number of Hits'.format(sideId[iSide],gas[iL]),path='TRT/Aging/{0}'.format(barrelOrEndcap[ibe]),xbins=30,xmin=644.,xmax=1004.,duration='lowStat')
                        agingGroup.defineHistogram('Trackr_HT;trackr_E{0}_{1}_HT'.format(sideId[iSide],gas[iL]),type='TH1F',title='Number HT Hits E{0} {1};r [mm];Number of HT Hits'.format(sideId[iSide],gas[iL]),path='TRT/Aging/{0}'.format(barrelOrEndcap[ibe]),xbins=30,xmin=644.,xmax=1004.,duration='run')
                        agingGroup.defineHistogram('Trackr_HT;trackr_E{0}_{1}_HT'.format(sideId[iSide],gas[iL]),type='TH1F',title='Number HT Hits E{0} {1};r [mm];Number of HT Hits'.format(sideId[iSide],gas[iL]),path='TRT/Aging/{0}'.format(barrelOrEndcap[ibe]),xbins=30,xmin=644.,xmax=1004.,duration='lowStat')

    acc = helper.result()
    result.merge(acc)
    return result

if __name__ == '__main__':
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    log.setLevel(DEBUG)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    nightly = '/afs/cern.ch/work/n/nbelyaev/public/'
    file = 'data16_13TeV.00358031.physics_Main.recon.AOD.Athena.21.0.78.f961/ESD.pool.root'
    ConfigFlags.Input.Files = [nightly+file]
    ConfigFlags.Input.isMC = False
    ConfigFlags.Output.HISTFileName = 'TRTMonitoringRun3_ToolOutput.root'
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    from AthenaCommon.AppMgr import ServiceMgr
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    # Configuration of the ATLAS Geo Model
    from AtlasGeoModel.AtlasGeoModelConfig import AtlasGeometryCfg
    geoCfg = AtlasGeometryCfg(ConfigFlags)
    cfg.merge(geoCfg)

    TRTMonitoringRun3Acc = TRTMonitoringRun3ESD_AlgConfig(ConfigFlags)
    ServiceMgr.Dump = False

    cfg.merge(TRTMonitoringRun3Acc)
    cfg.run(200)
