# 
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

'''
@file Run3AFPExampleMonitorAlgorithm.py
@author N. Dikic
@date 2020-08-12
'''

def Run3AFPExampleMonitoringConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    # don't run in RAWtoESD
    if inputFlags.DQ.Environment in ('tier0Raw',):
        return result

    from LumiBlockComps.BunchCrossingCondAlgConfig import BunchCrossingCondAlgCfg
    result.merge(BunchCrossingCondAlgCfg(inputFlags))
    
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'Run3AFPMonitorCfg')
    
    from AthenaConfiguration.ComponentFactory import CompFactory

    #from Run3AFPMonitoring.Run3AFPMonitoringConf import AFPSiLayerAlgorithm
    afpSiLayerAlgorithmFac = CompFactory.AFPSiLayerAlgorithm
    afpSiLayerAlgorithm = helper.addAlgorithm(afpSiLayerAlgorithmFac,'AFPSiLayerAlg')

    #from Run3AFPMonitoring.Run3AFPMonitoringConf import AFPToFAlgorithm
    afpToFAlgorithmFac = CompFactory.AFPToFAlgorithm
    afpToFAlgorithm = helper.addAlgorithm(afpToFAlgorithmFac,'AFPToFAlg')

    afpToFSiTAlgorithmFac = CompFactory.AFPToFSiTAlgorithm
    afpToFSiTAlgorithm = helper.addAlgorithm(afpToFSiTAlgorithmFac,'AFPToFSiTAlg')

    # Add a generic monitoring tool (a "group" in old language). The returned 
    # object here is the standard GenericMonitoringTool.
    AFPSiGroup = helper.addGroup(afpSiLayerAlgorithm, 'AFPSiLayerTool', 'AFP/') 
    AFPToFGroup = helper.addGroup(afpToFAlgorithm, 'AFPToFTool', 'AFP/')
    AFPToFSiTGroup = helper.addGroup(afpToFSiTAlgorithm, 'AFPToFSiTTool', 'AFP/')
    
    xLabelsStations = ['farAside', 'nearAside', 'nearCside', 'farCside']
    xLabelsStationsPlanes = ['fA3','fA2','fA1','fA0','nA3','nA2','nA1','nA0','nC0','nC1','nC2','nC3','fC0','fC1','fC2','fC3']
    xLabelsForEventsPerStation = [ 'fA', '-','-','-', 'nA', '-', '-', '-', 'nC', '-', '-', '-', 'fC', '-', '-', '-', ]
    xLabelsHitBarVsTrain = [ 'A', 'B', 'C', 'D']
    yLabelsHitBarVsTrain = [ '3', '2', '1', '0']
    #xLabelsStationsPlanesProposed = ['fC3', 'fC2', 'fC1', 'fC0', 'nC3', 'nC2', 'nC1', 'nC0', 'nA0', 'nA1', 'nA2', 'nA3', 'fA0', 'fA1' 'fA2','fA3']

    AFPSiGroup.defineHistogram('lb,nSiHits', title='Total number of hits divided by number of events;lumiblock;total number of hits', type='TProfile', path='SiT/', xbins=2000, xmin=0.5, xmax=2000.5)
    AFPSiGroup.defineHistogram('eventsPerStation', title='Number of events per stations; station; events', type='TH1I', path='SiT/Aux/', xbins=16, xmin=-0.5, xmax=15.5, xlabels=xLabelsForEventsPerStation )
    AFPSiGroup.defineHistogram('clustersInPlanes', title='Number of clusters per planes; plane; clusters', type='TH1I', path='SiT/Aux/', xbins=16, xmin=-0.5, xmax=15.5, xlabels=xLabelsStationsPlanes )
    AFPSiGroup.defineHistogram('lb,muPerBX', title='<mu>;lumiBlock;<mu>', type='TProfile', path='SiT/', xbins=2000, xmin=0.5, xmax=2000.5)
    AFPSiGroup.defineHistogram('planeHitsAll', title='Number of hits per plane;plane; hits', type='TH1I', path='SiT/HitsPerPlanes/', xbins=16, xmin=-0.5, xmax=15.5, xlabels=xLabelsStationsPlanes )
    AFPSiGroup.defineHistogram('planeHitsAllMU', title='Number of hits per plane divided by <mu>;plane; hits/<mu>', type='TH1F', path='SiT/HitsPerPlanes/', xbins=16, xmin=-0.5, xmax=15.5, weight = 'weightAllPlanes', xlabels=xLabelsStationsPlanes )
    AFPSiGroup.defineHistogram('numberOfHitsPerStation', title='Number of hits per station; station; hits', type='TH1I', path='SiT/Aux/', xbins=4, xmin=-0.5, xmax=3.5, xlabels=xLabelsStations)
    AFPSiGroup.defineHistogram('lbEvents;NumberOfEventsPerLumiblock',title='Number of events per lumiblock; lumiblock; events', type='TH1I', path='SiT/Aux/', xbins=2000, xmin=0.5, xmax=2000.5)
    AFPSiGroup.defineHistogram('lbHits;NumberOfHitsPerLumiblock',title='Number of hits per lumiblock; lumiblock; hits', type='TH1I', path='SiT/Aux/', xbins=2000, xmin=0.5, xmax=2000.5)
    
    #SIT: BCID histograms
    AFPSiGroup.defineHistogram('bcidAll', title='(All) Paired bunches - SiT; BX; entries', type='TH1I', path='SiT/BCID_Mask', xbins=4000, xmin=-0.5, xmax=3999.5)
    AFPSiGroup.defineHistogram('bcidFront', title='(Front) Paired bunches - SiT; BX; entries', type='TH1I', path='SiT/BCID_Mask', xbins=4000, xmin=-0.5, xmax=3999.5)
    AFPSiGroup.defineHistogram('bcidMiddle', title='(Middle) Paired bunches - SiT; BX; entries', type='TH1I', path='SiT/BCID_Mask', xbins=4000, xmin=-0.5, xmax=3999.5)
    AFPSiGroup.defineHistogram('bcidEnd', title='(End) Paired bunches - SiT; BX; entries', type='TH1I', path='SiT/BCID_Mask', xbins=4000, xmin=-0.5, xmax=3999.5)
    
    AFPSiGroup.defineHistogram('lbEventsStationsAll', title='Number of events per lumiblock for all stations;lumiblock; events', type='TH1I', path='SiT/StationEvents/', xbins=2000, xmin=0.5, xmax=2000.5)
    AFPSiGroup.defineHistogram('numberOfEventsPerLumiblockFront', title='(Front) Number of events per lumiblock for all stations;lumiblock; events', type='TH1I', path='SiT/Aux/', xbins=2000, xmin=0.5, xmax=2000.5)
    AFPSiGroup.defineHistogram('numberOfEventsPerLumiblockMiddle', title='(Middle) Number of events per lumiblock for all stations;lumiblock; events', type='TH1I', path='SiT/Aux/', xbins=2000, xmin=0.5, xmax=2000.5)
    AFPSiGroup.defineHistogram('numberOfEventsPerLumiblockEnd', title='(End) Number of events per lumiblock for all stations;lumiblock; events', type='TH1I', path='SiT/Aux/', xbins=2000, xmin=0.5, xmax=2000.5)


    AFPToFGroup.defineHistogram('lb,nTofHits', title='Multiplicity;lumiblock;total number of Hits', type='TProfile', path='ToF/', xbins=2000, xmin=0.5, xmax=2000.5) 
    AFPToFGroup.defineHistogram('numberOfHit_S0', title='Number of hit per bar station 0;bar', path='ToF/', xbins=4, xmin=-0.5, xmax=3.5)
    AFPToFGroup.defineHistogram('numberOfHit_S3', title='Number of hit per bar station 3;bar', path='ToF/', xbins=4, xmin=-0.5, xmax=3.5)
    AFPToFGroup.defineHistogram('barInTrainAllA', title='Number of hits in bar per stationA; barInTrain;hits',type='TH1I', path='ToF/HitsPerBarsInTrain/', xbins=16, xmin=-0.5, xmax=15.5)
    AFPToFGroup.defineHistogram('barInTrainAllC', title='Number of hits in bar per stationC; barInTrain;hits',type='TH1I', path='ToF/HitsPerBarsInTrain/', xbins=16, xmin=-0.5, xmax=15.5)
    
    AFPToFGroup.defineHistogram('ToFHits_sideA', title='ToF hits per lumiblock divided by <mu>, side A; lb; hits', path='ToF/StationHits', type='TH1F', xbins=2000, xmin=0.5, xmax=2000.5, weight = 'ToFHits_MU_Weight')
    AFPToFGroup.defineHistogram('ToFHits_sideC', title='ToF hits per lumiblock divided by <mu>, side C; lb; hits', path='ToF/StationHits', type='TH1F', xbins=2000, xmin=0.5, xmax=2000.5, weight = 'ToFHits_MU_Weight')
    AFPToFGroup.defineHistogram('lb,muPerBXToF', title='<mu>;lumiblock;<mu>', type='TProfile', path='ToF/', xbins=2000, xmin=0.5, xmax=2000.5)
    
    AFPToFGroup.defineHistogram('lbAandCToFEvents', title='Number of events in ToF stations A and C vs lb; lumiblock; events', type='TH1I', path='ToF/Events', xbins=2000, xmin=0.5, xmax=2000.5)
    AFPToFGroup.defineHistogram('lbAToFEvents', title='Number of events in ToF station A vs lb; lumiblock; events', type='TH1I', path='ToF/Events', xbins=2000, xmin=0.5, xmax=2000.5)
    AFPToFGroup.defineHistogram('lbCToFEvents', title='Number of events in ToF station C vs lb; lumiblock; events', type='TH1I', path='ToF/Events', xbins=2000, xmin=0.5, xmax=2000.5)
    
    # TOF: BCID histograms
    AFPToFGroup.defineHistogram('bcidAllToF', title='(All) Paired bunches - ToF; BX; entries', type='TH1I', path='ToF/BCID_Mask', xbins=4000, xmin=-0.5, xmax=3999.5)
    AFPToFGroup.defineHistogram('bcidFrontToF', title='(Front) Paired bunches - ToF; BX; entries', type='TH1I', path='ToF/BCID_Mask', xbins=4000, xmin=-0.5, xmax=3999.5)
    AFPToFGroup.defineHistogram('bcidMiddleToF', title='(Middle) Paired bunches - ToF; BX; entries', type='TH1I', path='ToF/BCID_Mask', xbins=4000, xmin=-0.5, xmax=3999.5)
    AFPToFGroup.defineHistogram('bcidEndToF', title='(End) Paired bunches - ToF; BX; entries', type='TH1I', path='ToF/BCID_Mask', xbins=4000, xmin=-0.5, xmax=3999.5)
    
    # TOF: Hits per trains histograms (side A):
    AFPToFGroup.defineHistogram('lbAToF_T0', title='Number of hits per train (side A, train0) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideA/All/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbAToF_TAll_Weight')
    AFPToFGroup.defineHistogram('lbAToF_T1', title='Number of hits per train (side A, train1) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideA/All/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbAToF_TAll_Weight')
    AFPToFGroup.defineHistogram('lbAToF_T2', title='Number of hits per train (side A, train2) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideA/All/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbAToF_TAll_Weight')
    AFPToFGroup.defineHistogram('lbAToF_T3', title='Number of hits per train (side A, train3) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideA/All/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbAToF_TAll_Weight')
    
    #Front:
    AFPToFGroup.defineHistogram('lbAToF_T0_Front', title='(Front) Number of hits per train (side A, train0) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideA/Front/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbAToF_TFront_Weight')
    AFPToFGroup.defineHistogram('lbAToF_T1_Front', title='(Front) Number of hits per train (side A, train1) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideA/Front/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbAToF_TFront_Weight')
    AFPToFGroup.defineHistogram('lbAToF_T2_Front', title='(Front) Number of hits per train (side A, train2) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideA/Front/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbAToF_TFront_Weight')
    AFPToFGroup.defineHistogram('lbAToF_T3_Front', title='(Front) Number of hits per train (side A, train3) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideA/Front/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbAToF_TFront_Weight')
    
    #Middle
    AFPToFGroup.defineHistogram('lbAToF_T0_Middle', title='(Middle) Number of hits per train (side A, train0) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideA/Middle/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbAToF_TMiddle_Weight')
    AFPToFGroup.defineHistogram('lbAToF_T1_Middle', title='(Middle) Number of hits per train (side A, train1) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideA/Middle/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbAToF_TMiddle_Weight')
    AFPToFGroup.defineHistogram('lbAToF_T2_Middle', title='(Middle) Number of hits per train (side A, train2) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideA/Middle/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbAToF_TMiddle_Weight')
    AFPToFGroup.defineHistogram('lbAToF_T3_Middle', title='(Middle) Number of hits per train (side A, train3) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideA/Middle/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbAToF_TMiddle_Weight')

    #End
    AFPToFGroup.defineHistogram('lbAToF_T0_End', title='(End) Number of hits per train (side A, train0) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideA/End/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbAToF_TEnd_Weight')
    AFPToFGroup.defineHistogram('lbAToF_T1_End', title='(End) Number of hits per train (side A, train1) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideA/End/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbAToF_TEnd_Weight')
    AFPToFGroup.defineHistogram('lbAToF_T2_End', title='(End) Number of hits per train (side A, train2) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideA/End/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbAToF_TEnd_Weight')
    AFPToFGroup.defineHistogram('lbAToF_T3_End', title='(End) Number of hits per train (side A, train3) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideA/End/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbAToF_TEnd_Weight')


    # Hits per trains histograms (side C):
    AFPToFGroup.defineHistogram('lbCToF_T0', title='Number of hits per train (side C, train0) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideC/All/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbCToF_TAll_Weight')
    AFPToFGroup.defineHistogram('lbCToF_T1', title='Number of hits per train (side C, train1) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideC/All/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbCToF_TAll_Weight')
    AFPToFGroup.defineHistogram('lbCToF_T2', title='Number of hits per train (side C, train2) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideC/All/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbCToF_TAll_Weight')
    AFPToFGroup.defineHistogram('lbCToF_T3', title='Number of hits per train (side C, train3) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideC/All/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbCToF_TAll_Weight')
    
    #Front
    AFPToFGroup.defineHistogram('lbCToF_T0_Front', title='(Front) Number of hits per train (side C, train0) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideC/Front/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbCToF_TFront_Weight')
    AFPToFGroup.defineHistogram('lbCToF_T1_Front', title='(Front) Number of hits per train (side C, train1) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideC/Front/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbCToF_TFront_Weight')
    AFPToFGroup.defineHistogram('lbCToF_T2_Front', title='(Front) Number of hits per train (side C, train2) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideC/Front/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbCToF_TFront_Weight')
    AFPToFGroup.defineHistogram('lbCToF_T3_Front', title='(Front) Number of hits per train (side C, train3) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideC/Front/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbCToF_TFront_Weight')
    
    #Middle
    AFPToFGroup.defineHistogram('lbCToF_T0_Middle', title='(Middle) Number of hits per train (side C, train0) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideC/Middle/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbCToF_TMiddle_Weight')
    AFPToFGroup.defineHistogram('lbCToF_T1_Middle', title='(Middle) Number of hits per train (side C, train1) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideC/Middle/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbCToF_TMiddle_Weight')
    AFPToFGroup.defineHistogram('lbCToF_T2_Middle', title='(Middle) Number of hits per train (side C, train2) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideC/Middle/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbCToF_TMiddle_Weight')
    AFPToFGroup.defineHistogram('lbCToF_T3_Middle', title='(Middle) Number of hits per train (side C, train3) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideC/Middle/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbCToF_TMiddle_Weight')

    #End
    AFPToFGroup.defineHistogram('lbCToF_T0_End', title='(End) Number of hits per train (side C, train0) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideC/End/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbCToF_TEnd_Weight')
    AFPToFGroup.defineHistogram('lbCToF_T1_End', title='(End) Number of hits per train (side C, train1) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideC/End/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbCToF_TEnd_Weight')
    AFPToFGroup.defineHistogram('lbCToF_T2_End', title='(End) Number of hits per train (side C, train2) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideC/End/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbCToF_TEnd_Weight')
    AFPToFGroup.defineHistogram('lbCToF_T3_End', title='(End) Number of hits per train (side C, train3) divided by <mu>; lb; hits/<mu>', type='TH1F', path='ToF/ToFHitsVsLb/sideC/End/', xbins=2000, xmin=-0.5, xmax=1999.5, weight='lbCToF_TEnd_Weight')

    AFPToFSiTGroup.defineHistogram('lqBar_A,fsp0_rows_A;ToFSiTCorrXA', title='LQBar vs FSP0 rows Side A;Train;FSP0 x-pix [50 um]', type='TH2F', path='ToFSiTCorr/', xbins=20, xmin=-1, xmax=4, ybins=346, ymin=-10.5, ymax=335.5)
    AFPToFSiTGroup.defineHistogram('lqBar_C,fsp0_rows_C;ToFSiTCorrXC', title='LQBar vs FSP0 rows Side C;Train;FSP0 x-pix [50 um]', type='TH2F', path='ToFSiTCorr/', xbins=20, xmin=-1, xmax=4, ybins=346, ymin=-10.5, ymax=335.5)
    AFPToFSiTGroup.defineHistogram('lqBar_A,fsp0_columns_A;ToFSiTCorrYA', title='LQBar vs FSP0 columns Side A;Train;FSP0 y-pix [250 um]', type='TH2F', path='ToFSiTCorr/', xbins=20, xmin=-1, xmax=4, ybins=85, ymin=-5.5, ymax=79.5)
    AFPToFSiTGroup.defineHistogram('lqBar_C,fsp0_columns_C;ToFSiTCorrYC', title='LQBar vs FSP0 columns Side C;Train;FSP0 y-pix [250 um]', type='TH2F', path='ToFSiTCorr/', xbins=20, xmin=-1, xmax=4, ybins=85, ymin=-5.5, ymax=79.5)

    AFPToFSiTGroup.defineHistogram('lqBar_tight_A,fsp0_rows_tight_A;ToFSiTCorrTightXA', title='LQBar vs FSP0 X dim with FSP2 Side A;Train;FSP0 x-pix [50 um]', type='TH2I', path='ToFSiTCorr/', xbins=20, xmin=-1, xmax=4, ybins=34, ymin=-0.05, ymax=335.5)
    AFPToFSiTGroup.defineHistogram('lqBar_tight_C,fsp0_rows_tight_C;ToFSiTCorrTightXC', title='LQBar vs FSP0 X dim with FSP2 Side C;Train;FSP0 x-pix [50 um]', type='TH2I', path='ToFSiTCorr/', xbins=20, xmin=-1, xmax=4, ybins=34, ymin=-0.05, ymax=335.5)
    AFPToFSiTGroup.defineHistogram('lqBar_tight_A,fsp0_columns_tight_A;ToFSiTCorrTightYA', title='LQBar vs FSP0 columns with FSP2 hit Side A;Train;FSP0 y-pix [250 um]', type='TH2I', path='ToFSiTCorr/', xbins=20, xmin=-1, xmax=4, ybins=80, ymin=-0.5, ymax=79.5)
    AFPToFSiTGroup.defineHistogram('lqBar_tight_C,fsp0_columns_tight_C;ToFSiTCorrTightYC', title='LQBar vs FSP0 columns with FSP2 hit Side C;Train;FSP0 y-pix [250 um]', type='TH2I', path='ToFSiTCorr/', xbins=20, xmin=-1, xmax=4, ybins=80, ymin=-0.5, ymax=79.5)

    AFPToFSiTGroup.defineHistogram('trainHits_A,fsp0_rows_tight_A;ToFSiTCorrTrainHitsXA', title='# hits in train vs FSP0 X dim with FSP2 Side A;Train hits (train+numHits/5);FSP0 x-pix [50 um]', type='TH2I', path='ToFSiTCorr/', xbins=25, xmin=-1, xmax=4, ybins=34, ymin=-0.05, ymax=335.5)
    AFPToFSiTGroup.defineHistogram('trainHits_C,fsp0_rows_tight_C;ToFSiTCorrTrainHitsXC', title='# hits in train vs FSP0 X dim with FSP2 Side C;Train hits (train+numHits/5);FSP0 x-pix [50 um]', type='TH2I', path='ToFSiTCorr/', xbins=25, xmin=-1, xmax=4, ybins=34, ymin=-0.05, ymax=335.5)
    AFPToFSiTGroup.defineHistogram('trainHits_A,fsp0_columns_tight_A;ToFSiTCorrTrainHitsYA', title='# hits in train vs FSP0 columns with FSP2 hit Side A;Train hits (train+numHits/5);FSP0 y-pix [250 um]', type='TH2I', path='ToFSiTCorr/', xbins=25, xmin=-1, xmax=4, ybins=80, ymin=-0.5, ymax=79.5)
    AFPToFSiTGroup.defineHistogram('trainHits_C,fsp0_columns_tight_C;ToFSiTCorrTrainHitsYC', title='# hits in train vs FSP0 columns with FSP2 hit Side C;Train hits (train+numHits/5);FSP0 y-pix [250 um]', type='TH2I', path='ToFSiTCorr/', xbins=25, xmin=-1, xmax=4, ybins=80, ymin=-0.5, ymax=79.5)

    AFPToFSiTGroup.defineHistogram('tofHits_A,fsp0Hits_A;ToFSiTNumHitsA', title='TOF vs FSP0 num. hits Side A;#Hit bars;FSP0 multiplicity', type='TH2F', path='ToFSiTCorr/', xbins=17, xmin=0, xmax=16, ybins=150, ymin=-0.5, ymax=149.5)
    AFPToFSiTGroup.defineHistogram('tofHits_C,fsp0Hits_C;ToFSiTNumHitsC', title='TOF vs FSP0 num. hits Side C;#Hit bars;FSP0 multiplicity', type='TH2F', path='ToFSiTCorr/', xbins=17, xmin=0, xmax=16, ybins=150, ymin=-0.5, ymax=149.5)

    # Using a map of groups
    layerList = ['P0','P1', 'P2', 'P3'] ## TODO XXX adapt to the enum/xAOD namespace names
    stationList = ['farAside', 'nearAside', 'nearCside', 'farCside']
    trainList = ['train0', 'train1', 'train2', 'train3']

    array = helper.addArray([stationList,layerList], afpSiLayerAlgorithm, 'AFPSiLayerTool', topPath = 'AFP/SiT/')

    array.defineHistogram('pixelColIDChip', title='Hits per column for station {0}, layer {1};ColID; entries', path='PixelColIDChip/{0}', xbins=80, xmin=0.5, xmax=80.5)
    array.defineHistogram('pixelRowIDChip', title='Hits per row for station {0}, layer {1};RowID; entries', path='PixelRowIDChip/{0}', xbins=336, xmin=0.5, xmax=336.5)
    array.defineHistogram('pixelRowIDChip,pixelColIDChip', title='Hitmap for station {0}, layer {1};RowID;ColID', type='TH2I', path='pixelColRow2D/{0}', xbins=336, xmin=0.5, xmax=336.5, ybins=80, ymin=0.5, ymax=80.5)
    array.defineHistogram('timeOverThreshold', type='TH1I', title='Time over threshold for station {0}, layer {1};timeOverThreshold; entries', path='SiTimeOverThreshold/{0}', xbins=16, xmin=0.5, xmax=16.5)
    
    array.defineHistogram('clusterY,clusterX', title='Cluster position in station {0} Layer {1};x [mm];y [mm]', type='TH2F', path='Cluster/{0}', xbins=336, xmin=0.0, xmax=17.0, ybins=80, ymin=0.0, ymax=20.0)
    array.defineHistogram('lb,clustersPerPlane', title='Number of clusters per event divided by <mu> in station {0}, layer {1};lumiblock; clusters per event / <mu>', type='TH2F', path='clustersPerPlane/{0}', xbins=2000, xmin=0.5, xmax=2000.5, ybins=1000, ymin=-0.2, ymax=1.0)
    array.defineHistogram('lb,clustersPerPlaneFront', title='(Front BCID) Number of clusters per event divided by <mu> in station {0}, layer {1};lumiblock; clusters per event / <mu>', type='TH2F', path='clustersPerPlaneFront/{0}', xbins=2000, xmin=0.5, xmax=2000.5, ybins=1000, ymin=-0.2, ymax=1.0)
    array.defineHistogram('lb,clustersPerPlaneEnd', title='(End BCID) Number of clusters per event divided by <mu> in station {0}, layer {1};lumiblock; clusters per event / <mu>', type='TH2F', path='clustersPerPlaneEnd/{0}', xbins=2000, xmin=0.5, xmax=2000.5, ybins=1000, ymin=-0.2, ymax=1.0)
    array.defineHistogram('lb,clustersPerPlaneMiddle', title='(Middle BCID) Number of clusters per event divided by <mu> in station {0}, layer {1};lumiblock; clusters per event / <mu>', type='TH2F', path='clustersPerPlaneMiddle/{0}', xbins=2000, xmin=0.5, xmax=2000.5, ybins=1000, ymin=-0.2, ymax=1.0)
    
    array.defineHistogram('lb,hitsCounterPlanesTProfile', title='Number of hits divided by number of events per lumiblock, station {0} plane {1};lumiblock; hits/events', type='TProfile', path='HitsCounterPlanes/', xbins=2000, xmin=0.5, xmax=2000.5)
    
    array.defineHistogram('lbClustersPerPlanes', title='(All) Number of clusters in station {0}, plane {1} per lumiblock divided by <mu>;lumiblock;clusters/<mu> per event', type='TH1F', path='ClustersPerPlanesPP/', xbins=2000, xmin=0.5, xmax=2000.5, weight = 'weightClustersByMU')
    array.defineHistogram('clustersPerPlaneFrontPP', title='(Front) Number of clusters in station {0}, plane {1} per lumiblock divided by <mu>;lumiblock; clusters/<mu> per event', type='TH1F', path='ClustersPerPlanesPP/Front/', xbins=2000, xmin=0.5, xmax=2000.5, weight = 'weightClustersPerPlaneFrontPP')
    array.defineHistogram('clustersPerPlaneMiddlePP', title='(Middle) Number of clusters in station {0}, plane {1} per lumiblock divided by <mu>; lumiblock;clusters/<mu> per event', type='TH1F', path='ClustersPerPlanesPP/Middle/', xbins=2000, xmin=0.5, xmax=2000.5, weight = 'weightClustersPerPlaneMiddlePP')
    array.defineHistogram('clustersPerPlaneEndPP', title='(End) Number of clusters in station {0}, plane {1} per lumiblock divided by <mu>; lumiblock;clusters/<mu> per event', type='TH1F', path='ClustersPerPlanesPP/End/', xbins=2000, xmin=0.5, xmax=2000.5, weight = 'weightClustersPerPlaneEndPP')
    
    array.defineHistogram('lbClustersPerPlanes_full', title='(All) Number of clusters in station {0}, plane {1} per lumiblock;lumiblock;clusters', type='TH1I', path='ClustersPerPlanesPP_full/', xbins=2000, xmin=0.5, xmax=2000.5)
    #array.defineHistogram('clustersPerPlaneFrontPP_full', title='(Front) Number of clusters in station {0}, plane {1} per lumiblock;lumiblock; clusters', type='TH1I', path='ClustersPerPlanesPP_full/Front/', xbins=2000, xmin=0.5, xmax=2000.5)
    #array.defineHistogram('clustersPerPlaneMiddlePP_full', title='(Middle) Number of clusters in station {0}, plane {1} per lumiblock; lumiblock;clusters', type='TH1I', path='ClustersPerPlanesPP_full/Middle/', xbins=2000, xmin=0.5, xmax=2000.5)
    #array.defineHistogram('clustersPerPlaneEndPP_full', title='(End) Number of clusters in station {0}, plane {1} per lumiblock; lumiblock;clusters', type='TH1I', path='ClustersPerPlanesPP_full/End/', xbins=2000, xmin=0.5, xmax=2000.5)
    
    array.defineHistogram('lbHitsPerPlanes', title='Number of hits in station {0}, plane {1} per lumiblock divided by <mu>;lumiblock; hits/<mu> per event', type='TH1F', path='HitsPerPlanesVsLb/', xbins=2000, xmin=0.5, xmax=2000.5, weight = 'weightHitsByMU')
    array.defineHistogram('lbHitsPerPlanes_full', title='Number of hits in station {0}, plane {1} per lumiblock;lumiblock; hits per event', type='TH1F', path='HitsPerPlanesVsLb_full/', xbins=2000, xmin=0.5, xmax=2000.5)
    
    array.defineHistogram('clusterToT', title='Sum of all hits\' ToT in each cluster, station {0}, layer {1};Cluster ToT; Counts', type='TH1I', path='ClusterToT/', xbins=18, xmin=0.5, xmax=18.5)


    array = helper.addArray([stationList], afpSiLayerAlgorithm, 'AFPSiLayerTool', topPath='AFP/SiT/')
    
    array.defineHistogram('planeHits', type='TH1I', title='Number of hits per plane, station {0};plane; hits', path='HitsPerPlanes', xbins=4, xmin=-0.5, xmax=3.5)
    array.defineHistogram('trackY,trackX', title='Number of tracks in AFP station {0};x [mm];y [mm]', type='TH2F', path='Track', xbins=336, xmin=0.0, xmax=17.0, ybins=80, ymin=0.0, ymax=20.0)
    array.defineHistogram('lb,clustersPerStation', title ='Number of clusters per event divided by <mu> in station {0};lumiblock; clustersPerStation', type='TH2F', path='clustersPerStation', xbins=2000, xmin=0.5, xmax=2000.5, ybins=1000, ymin=-0.2, ymax=1.0)
    array.defineHistogram('lb,clustersPerStationFront', title ='(Front) Number of clusters per event divided by <mu> in station {0};lumiblock; clustersPerStation', type='TH2F', path='clustersPerStation', xbins=2000, xmin=0.5, xmax=2000.5, ybins=1000, ymin=-0.2, ymax=1.0)
    array.defineHistogram('lb,clustersPerStationEnd', title ='(End) Number of clusters per event divided by <mu> in station {0};lumiblock; clustersPerStation', type='TH2F', path='clustersPerStation', xbins=2000, xmin=0.5, xmax=2000.5, ybins=1000, ymin=-0.2, ymax=1.0)
    array.defineHistogram('lb,clustersPerStationMiddle', title ='(Middle) Number of clusters per event divided by <mu> in station {0};lumiblock; clustersPerStation', type='TH2F', path='clustersPerStation', xbins=2000, xmin=0.5, xmax=2000.5, ybins=1000, ymin=-0.2, ymax=1.0)
    
    array.defineHistogram('lb,hitsCounterStationsTProfile', title='Number of hits divided by number of events per lumiblock, station {0};lumiblock; hits/events', type='TProfile', path='HitsCounterPlanes/', xbins=2000, xmin=0.5, xmax=2000.5)
    array.defineHistogram('lbEventsStations', title='Number of events per lumiblock, station {0};lumiblock; events', type='TH1I', path='StationEvents/', xbins=2000, xmin=0.5, xmax=2000.5)
    
    array.defineHistogram('lbTracksAll', title = '(All) Tracks vs lumiblock divided by <mu>, station {0};lumiblock;tracks/<mu> per event', type='TH1F', path='TracksVsLb', xbins=2000, xmin=0.5, xmax=2000.5, weight = 'weightTracksAll')
    array.defineHistogram('lbTracksFront', title = '(Front) Tracks vs lumiblock divided by <mu>, station {0}; lumiblock;tracks/<mu> per event', type='TH1F', path='TracksVsLb/Front', xbins=2000, xmin=0.5, xmax=2000.5, weight = 'weightTracksFront')
    array.defineHistogram('lbTracksMiddle', title = '(Middle) Tracks vs lumiblock divided by <mu>, station {0}; lumiblock;tracks/<mu> per event', type='TH1F', path='TracksVsLb/Middle', xbins=2000, xmin=0.5, xmax=2000.5, weight = 'weightTracksMiddle')
    array.defineHistogram('lbTracksEnd', title = '(End) Tracks vs lumiblock divided by <mu>, station {0}; lumiblock;tracks/<mu> per event', type='TH1F', path='TracksVsLb/End', xbins=2000, xmin=0.5, xmax=2000.5, weight = 'weightTracksEnd')
    
    array.defineHistogram('lbTracksAll_full', title = '(All) Tracks vs lumiblock, station {0};lumiblock;tracks', type='TH1I', path='TracksVsLb_full', xbins=2000, xmin=0.5, xmax=2000.5, weight = 'weightTracksAll_full')
    array.defineHistogram('lbTracksFront_full', title = '(Front) Tracks vs lumiblock, station {0}; lumiblock;tracks', type='TH1I', path='TracksVsLb_full/Front', xbins=2000, xmin=0.5, xmax=2000.5, weight = 'weightTracksFront_full')
    array.defineHistogram('lbTracksMiddle_full', title = '(Middle) Tracks vs lumiblock, station {0}; lumiblock;tracks', type='TH1I', path='TracksVsLb_full/Middle', xbins=2000, xmin=0.5, xmax=2000.5, weight = 'weightTracksMiddle_full')
    array.defineHistogram('lbTracksEnd_full', title = '(End) Tracks vs lumiblock, station {0}; lumiblock;tracks', type='TH1I', path='TracksVsLb_full/End', xbins=2000, xmin=0.5, xmax=2000.5, weight = 'weightTracksEnd_full')


    arrayToF = helper.addArray([stationList], afpToFAlgorithm, 'AFPToFTool', topPath='AFP/ToF/')
    
    arrayToF.defineHistogram('barInTrainID,trainID', title='ToF hit bar vs train {0};barInTrainID;trainID', type='TH2I', path='HitBarvsTrain/',xbins=4,xmin=-0.5,xmax=3.5,ybins=4,ymin=-0.5,ymax=3.5, xlabels = xLabelsHitBarVsTrain, ylabels = yLabelsHitBarVsTrain)
    arrayToF = helper.addArray([trainList], afpToFAlgorithm, 'AFPToFTool', topPath='AFP/ToF/')
    arrayToF.defineHistogram('barInTrainIDA', title='Total hits per bars in {}; barID; hits', type='TH1I', path='HitsPerBarsInTrain/farAside', xbins=4,xmin=-0.5,xmax=3.5)
    arrayToF.defineHistogram('barInTrainIDC', title='Total hits per bars in {}; barID; hits', type='TH1I', path='HitsPerBarsInTrain/farCside', xbins=4,xmin=-0.5,xmax=3.5)
    
    trainIDListToF = ['T0', 'T1', 'T2', 'T3']
    barInTrainIDListToF = ['A', 'B', 'C', 'D']
    arrayToF = helper.addArray([trainIDListToF, barInTrainIDListToF], afpToFAlgorithm, 'AFPToFTool', topPath = 'AFP/ToF/')
    arrayToF.defineHistogram('lbAToF', title='ToF hits vs lumiblock divided by <mu> (train {0}, bar {1}), side A; lb; hits/<mu>', type='TH1F', path='ToFHitsVsLb/sideA', xbins=2000, xmin=0.5, xmax=2000.5, weight = 'lbAToF_Weight')
    arrayToF.defineHistogram('lbCToF', title='ToF hits vs lumiblock divided by <mu> (train {0}, bar {1}), side C; lb; hits/<mu>', type='TH1F', path='ToFHitsVsLb/sideC', xbins=2000, xmin=0.5, xmax=2000.5, weight = 'lbCToF_Weight')
    
    
    # Finalize. The return value should be a tuple of the ComponentAccumulator
    result.merge(helper.result())
    return result
    

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
    #ConfigFlags.Input.Files = ['/dsk1/AFPFiles/datasets/355754/user.ladamczy.23432842.EXT0._000001.xAOD.root']
    ConfigFlags.Input.Files = ['/eos/atlas/atlascerngroupdisk/det-afp/nikola/afs/public/backup_raw_337176/user.ladamczy.21473705.EXT0._000002.xAOD.root',
                               '/eos/atlas/atlascerngroupdisk/det-afp/nikola/afs/public/backup_raw_337176/user.ladamczy.21473705.EXT0._000003.xAOD.root',
                               '/eos/atlas/atlascerngroupdisk/det-afp/nikola/afs/public/backup_raw_337176/user.ladamczy.21473705.EXT0._000004.xAOD.root']
    #ConfigFlags.Input.Files = ['/afs/cern.ch/user/p/pbalek/public/AFP/testxAOD.root']
    ConfigFlags.Input.isMC = False
    ConfigFlags.Output.HISTFileName = 'AFPTest-337176-10k-FMETrains-MU.root'
    

    ConfigFlags.Concurrency.NumThreads=10
    ConfigFlags.Concurrency.NumConcurrentEvents=10
    
    
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))
    
    exampleMonitorAcc = Run3AFPExampleMonitoringConfig(ConfigFlags)
    cfg.merge(exampleMonitorAcc)

    cfg.run(10000)



