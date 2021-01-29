# 
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

    # Add a generic monitoring tool (a "group" in old language). The returned 
    # object here is the standard GenericMonitoringTool.
    AFPSiGroup = helper.addGroup(afpSiLayerAlgorithm, 'AFPSiLayerTool', 'AFP/') 
    AFPToFGroup = helper.addGroup(afpToFAlgorithm, 'AFPToFTool', 'AFP/')
    
    xLabelsStations = ['farAside', 'nearAside', 'nearCside', 'farCside']
    xLabelsStationsPlanes = ['fA0','fA1','fA2','fA3','nA0','nA1','nA2','nA3','nC0','nC1','nC2','nC3','fC0','fC1','fC2','fC3']

    AFPSiGroup.defineHistogram('lb,nSiHits', title='Total number of hits divided by number of events;lumiblock;total number of hits', type='TProfile', path='SiT/', xbins=1000, xmin=-0.5, xmax=999.5)
    AFPSiGroup.defineHistogram('eventsPerStation', title='Number of events per stations; station; events', type='TH1I', path='SiT/Aux/', xbins=16, xmin=-0.5, xmax=15.5 )
    AFPSiGroup.defineHistogram('clustersInPlanes', title='Number of clusters per planes; plane; clusters', type='TH1I', path='SiT/Aux/', xbins=16, xmin=-0.5, xmax=15.5, xlabels=xLabelsStationsPlanes )
    AFPSiGroup.defineHistogram('lb,muPerBX', title='<mu>;lumiBlock;<mu>', type='TProfile', path='SiT/', xbins=1000, xmin=-0.5, xmax=999.5)
    AFPSiGroup.defineHistogram('planeHitsAll', title='Number of hits per plane;plane; hits', type='TH1I', path='SiT/HitsPerPlanes/', xbins=16, xmin=-0.5, xmax=15.5, xlabels=xLabelsStationsPlanes )
    AFPSiGroup.defineHistogram('planeHitsAllMU', title='Number of hits per plane divided by <mu>;plane; hits/<mu>', type='TH1F', path='SiT/HitsPerPlanes/', xbins=16, xmin=-0.5, xmax=15.5, weight = 'weightAllPlanes', xlabels=xLabelsStationsPlanes )
    AFPSiGroup.defineHistogram('numberOfHitsPerStation', title='Number of hits per station; station; hits', type='TH1I', path='SiT/Aux/', xbins=4, xmin=-0.5, xmax=3.5, xlabels=xLabelsStations)
    AFPSiGroup.defineHistogram('lbEvents;NumberOfEventsPerLumiblock',title='Number of events per lumiblock; lumiblock; events', type='TH1I', path='SiT/Aux/', xbins=1000, xmin=-0.5, xmax=999.5)
    AFPSiGroup.defineHistogram('lbHits;NumberOfHitsPerLumiblock',title='Number of hits per lumiblock; lumiblock; hits', type='TH1I', path='SiT/Aux/', xbins=1000, xmin=-0.5, xmax=999.5)
    
    AFPSiGroup.defineHistogram('bcidAll', title='(All) Paired bunches; BX; entries', type='TH1I', path='SiT/BCID_Mask', xbins=4000, xmin=-0.5, xmax=3999.5)
    AFPSiGroup.defineHistogram('bcidFront', title='(Front) Paired bunches; BX; entries', type='TH1I', path='SiT/BCID_Mask', xbins=4000, xmin=-0.5, xmax=3999.5)
    AFPSiGroup.defineHistogram('bcidMiddle', title='(Middle) Paired bunches; BX; entries', type='TH1I', path='SiT/BCID_Mask', xbins=4000, xmin=-0.5, xmax=3999.5)
    AFPSiGroup.defineHistogram('bcidEnd', title='(End) Paired bunches; BX; entries', type='TH1I', path='SiT/BCID_Mask', xbins=4000, xmin=-0.5, xmax=3999.5)
    AFPSiGroup.defineHistogram('lbEventsStationsAll', title='Number of events per lumiblock for all stations;lumiblock; events', type='TH1I', path='SiT/StationEvents/', xbins=1000, xmin=-0.5, xmax=999.5)
    AFPSiGroup.defineHistogram('numberOfEventsPerLumiblockFront', title='(Front) Number of events per lumiblock for all stations;lumiblock; events', type='TH1I', path='SiT/Aux/', xbins=1000, xmin=-0.5, xmax=999.5)
    AFPSiGroup.defineHistogram('numberOfEventsPerLumiblockMiddle', title='(Middle) Number of events per lumiblock for all stations;lumiblock; events', type='TH1I', path='SiT/Aux/', xbins=1000, xmin=-0.5, xmax=999.5)
    AFPSiGroup.defineHistogram('numberOfEventsPerLumiblockEnd', title='(End) Number of events per lumiblock for all stations;lumiblock; events', type='TH1I', path='SiT/Aux/', xbins=1000, xmin=-0.5, xmax=999.5)


    AFPToFGroup.defineHistogram('lb,nTofHits', title='Multiplicity;lumiblock;total number of Hits', type='TProfile', path='ToF/', xbins=1000, xmin=-0.5, xmax=999.5) 
    AFPToFGroup.defineHistogram('numberOfHit_S0', title='Number of hit per bar station 0;bar', path='ToF/', xbins=4, xmin=-0.5, xmax=3.5)
    AFPToFGroup.defineHistogram('numberOfHit_S3', title='Number of hit per bar station 3;bar', path='ToF/', xbins=4, xmin=-0.5, xmax=3.5)
    AFPToFGroup.defineHistogram('barInTrainAllA', title='Number of hits in bar per stationA; barInTrain;hits',type='TH1I', path='ToF/HitsPerBarsInTrain/', xbins=16, xmin=-0.5, xmax=15.5)
    AFPToFGroup.defineHistogram('barInTrainAllC', title='Number of hits in bar per stationC; barInTrain;hits',type='TH1I', path='ToF/HitsPerBarsInTrain/', xbins=16, xmin=-0.5, xmax=15.5)

    # Using a map of groups
    layerList = ['P0','P1', 'P2', 'P3'] ## TODO XXX adapt to the enum/xAOD namespace names
    stationList = ['farAside', 'nearAside', 'nearCside', 'farCside']
    trainList = ['train0', 'train1', 'train2', 'train3']

    array = helper.addArray([stationList,layerList], afpSiLayerAlgorithm, 'AFPSiLayerTool', topPath = 'AFP/SiT/')

    array.defineHistogram('pixelColIDChip', title='Hits per column for station {0}, layer {1};ColID; entries', path='PixelColIDChip/{0}', xbins=80, xmin=0.5, xmax=80.5)
    array.defineHistogram('pixelRowIDChip', title='Hits per row for station {0}, layer {1};RowID; entries', path='PixelRowIDChip/{0}', xbins=336, xmin=0.5, xmax=336.5)
    array.defineHistogram('pixelRowIDChip,pixelColIDChip', title='Hitmap for station {0}, layer {1};RowID;ColID', type='TH2I', path='pixelColRow2D/{0}', xbins=336, xmin=0.5, xmax=336.5, ybins=80, ymin=0.5, ymax=80.5)
    array.defineHistogram('timeOverThreshold', type='TH1F', title='Time over threshold for station {0}, layer {1};timeOverThreshold; entries', path='SiTimeOverThreshold/{0}', xbins=16, xmin=0.5, xmax=16.5)
    
    array.defineHistogram('clusterY,clusterX', title='Cluster position in station {0} Layer {1};x [mm];y [mm]', type='TH2I', path='Cluster/{0}', xbins=336, xmin=0.0, xmax=17.0, ybins=80, ymin=0.0, ymax=20.0)
    array.defineHistogram('lb,clustersPerPlane', title='Number of clusters per event divided by <mu> in station {0}, layer {1};lumiblock; clusters per event / <mu>', type='TH2F', path='clustersPerPlane/{0}', xbins=1000, xmin=-0.5, xmax=999.5, ybins=1000, ymin=-0.2, ymax=1.0)
    array.defineHistogram('lb,clustersPerPlaneFront', title='(Front BCID) Number of clusters per event divided by <mu> in station {0}, layer {1};lumiblock; clusters per event / <mu>', type='TH2F', path='clustersPerPlaneFront/{0}', xbins=1000, xmin=-0.5, xmax=999.5, ybins=1000, ymin=-0.2, ymax=1.0)
    array.defineHistogram('lb,clustersPerPlaneEnd', title='(End BCID) Number of clusters per event divided by <mu> in station {0}, layer {1};lumiblock; clusters per event / <mu>', type='TH2F', path='clustersPerPlaneEnd/{0}', xbins=1000, xmin=-0.5, xmax=999.5, ybins=1000, ymin=-0.2, ymax=1.0)
    array.defineHistogram('lb,clustersPerPlaneMiddle', title='(Middle BCID) Number of clusters per event divided by <mu> in station {0}, layer {1};lumiblock; clusters per event / <mu>', type='TH2F', path='clustersPerPlaneMiddle/{0}', xbins=1000, xmin=-0.5, xmax=999.5, ybins=1000, ymin=-0.2, ymax=1.0)
    
    array.defineHistogram('lb,hitsCounterPlanesTProfile', title='Number of hits divided by number of events per lumiblock, station {0} plane {1};lumiblock; hits/events', type='TProfile', path='HitsCounterPlanes/', xbins=1000, xmin=-0.5, xmax=999.5)
    
    array.defineHistogram('lbClustersPerPlanes', title='(All) Number of clusters in station {0}, plane {1} per lumiblock divided by <mu>;lumiblock;clusters/<mu> per event', type='TH1F', path='ClustersPerPlanesPP/', xbins=1000, xmin=-0.5, xmax=999.5, weight = 'weightClustersByMU')
    array.defineHistogram('clustersPerPlaneFrontPP', title='(Front) Number of clusters in station {0}, plane {1} per lumiblock divided by <mu>;lumiblock; clusters/<mu> per event', type='TH1F', path='ClustersPerPlanesPP/Front/', xbins=1000, xmin=-0.5, xmax=999.5, weight = 'weightClustersPerPlaneFrontPP')
    array.defineHistogram('clustersPerPlaneMiddlePP', title='(Middle) Number of clusters in station {0}, plane {1} per lumiblock divided by <mu>; lumiblock;clusters/<mu> per event', type='TH1F', path='ClustersPerPlanesPP/Middle/', xbins=1000, xmin=-0.5, xmax=999.5, weight = 'weightClustersPerPlaneMiddlePP')
    array.defineHistogram('clustersPerPlaneEndPP', title='(End) Number of clusters in station {0}, plane {1} per lumiblock divided by <mu>; lumiblock;clusters/<mu> per event', type='TH1F', path='ClustersPerPlanesPP/End/', xbins=1000, xmin=-0.5, xmax=999.5, weight = 'weightClustersPerPlaneEndPP')
    
    array.defineHistogram('lbHitsPerPlanes', title='Number of hits in station {0}, plane {1} per lumiblock divided by <mu>;lumiblock; hits/<mu> per event', type='TH1F', path='HitsPerPlanesVsLb/', xbins=1000, xmin=-0.5, xmax=999.5, weight = 'weightHitsByMU')


    array = helper.addArray([stationList], afpSiLayerAlgorithm, 'AFPSiLayerTool', topPath='AFP/SiT/')
    
    array.defineHistogram('planeHits', type='TH1I', title='Number of hits per plane, station {0};plane; hits', path='HitsPerPlanes', xbins=4, xmin=-0.5, xmax=3.5)
    array.defineHistogram('trackY,trackX', title='Number of tracks in AFP station {0};x [mm];y [mm]', type='TH2I', path='Track', xbins=336, xmin=0.0, xmax=17.0, ybins=80, ymin=0.0, ymax=20.0)
    array.defineHistogram('lb,clustersPerStation', title ='Number of clusters per event divided by <mu> in station {0};lumiblock; clustersPerStation', type='TH2F', path='clustersPerStation', xbins=1000, xmin=-0.5, xmax=999.5, ybins=1000, ymin=-0.2, ymax=1.0)
    array.defineHistogram('lb,clustersPerStationFront', title ='(Front) Number of clusters per event divided by <mu> in station {0};lumiblock; clustersPerStation', type='TH2F', path='clustersPerStation', xbins=1000, xmin=-0.5, xmax=999.5, ybins=1000, ymin=-0.2, ymax=1.0)
    array.defineHistogram('lb,clustersPerStationEnd', title ='(End) Number of clusters per event divided by <mu> in station {0};lumiblock; clustersPerStation', type='TH2F', path='clustersPerStation', xbins=1000, xmin=-0.5, xmax=999.5, ybins=1000, ymin=-0.2, ymax=1.0)
    array.defineHistogram('lb,clustersPerStationMiddle', title ='(Middle) Number of clusters per event divided by <mu> in station {0};lumiblock; clustersPerStation', type='TH2F', path='clustersPerStation', xbins=1000, xmin=-0.5, xmax=999.5, ybins=1000, ymin=-0.2, ymax=1.0)
    
    array.defineHistogram('lb,hitsCounterStationsTProfile', title='Number of hits divided by number of events per lumiblock, station {0};lumiblock; hits/events', type='TProfile', path='HitsCounterPlanes/', xbins=1000, xmin=-0.5, xmax=999.5)
    array.defineHistogram('lbEventsStations', title='Number of events per lumiblock, station {0};lumiblock; events', type='TH1I', path='StationEvents/', xbins=1000, xmin=-0.5, xmax=999.5)
    
    array.defineHistogram('lbTracksAll', title = '(All) Tracks vs lumiblock divided by <mu>, station {0};lumiblock;tracks/<mu> per event', type='TH1F', path='TracksVsLb', xbins=1000, xmin=-0.5, xmax=999.5, weight = 'weightTracksAll')
    array.defineHistogram('lbTracksFront', title = '(Front) Tracks vs lumiblock divided by <mu>, station {0}; lumiblock;tracks/<mu> per event', type='TH1F', path='TracksVsLb/Front', xbins=1000, xmin=-0.5, xmax=999.5, weight = 'weightTracksFront')
    array.defineHistogram('lbTracksMiddle', title = '(Middle) Tracks vs lumiblock divided by <mu>, station {0}; lumiblock;tracks/<mu> per event', type='TH1F', path='TracksVsLb/Middle', xbins=1000, xmin=-0.5, xmax=999.5, weight = 'weightTracksMiddle')
    array.defineHistogram('lbTracksEnd', title = '(End) Tracks vs lumiblock divided by <mu>, station {0}; lumiblock;tracks/<mu> per event', type='TH1F', path='TracksVsLb/End', xbins=1000, xmin=-0.5, xmax=999.5, weight = 'weightTracksEnd')


    arrayToF = helper.addArray([stationList], afpToFAlgorithm, 'AFPToFTool', topPath='AFP/ToF/')
    
    arrayToF.defineHistogram('barInTrainID,trainID', title='ToF hit bar vs train {0};barInTrainID;trainID', type='TH2I', path='HitBarvsTrain/',xbins=4,xmin=-0.5,xmax=3.5,ybins=4,ymin=-0.5,ymax=3.5)
    arrayToF = helper.addArray([trainList], afpToFAlgorithm, 'AFPToFTool', topPath='AFP/ToF/')
    arrayToF.defineHistogram('barInTrainIDA', title='Total hits per bars in {}; barID; hits', type='TH1I', path='HitsPerBarsInTrain/farAside', xbins=4,xmin=-0.5,xmax=3.5)
    arrayToF.defineHistogram('barInTrainIDC', title='Total hits per bars in {}; barID; hits', type='TH1I', path='HitsPerBarsInTrain/farCside', xbins=4,xmin=-0.5,xmax=3.5)
    
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
    ConfigFlags.Input.Files = ['/afs/cern.ch/work/n/ndikic/backup_raw_337176/user.ladamczy.21473705.EXT0._000002.xAOD.root']
    ConfigFlags.Input.isMC = False
    ConfigFlags.Output.HISTFileName = 'AFPOutput124-337176-test.root'
    
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

    cfg.run(20000)



