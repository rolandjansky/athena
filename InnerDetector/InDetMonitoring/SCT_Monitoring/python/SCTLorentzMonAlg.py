#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file SCTLorentzMonAlg.py
@author Susumu Oda
@date 2019-04-02
@brief Based on AthenaMonitoring/ExampleMonitorAlgorithm.py
'''

def SCTLorentzMonAlgConfig(inputFlags):
    
    ### STEP 1 ###
    # Define one top-level monitoring algorithm. The new configuration 
    # framework uses a component accumulator.
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags, 'SCTLorentzMonCfg')


    ### STEP 2 ###
    # Adding an algorithm to the helper. Here, we will use the example 
    # algorithm in the AthenaMonitoring package. Just pass the type to the 
    # helper. Then, the helper will instantiate an instance and set up the 
    # base class configuration following the inputFlags. The returned object 
    # is the algorithm.
    from SCT_Monitoring.SCT_MonitoringConf import SCTLorentzMonAlg
    myMonAlg = helper.addAlgorithm(SCTLorentzMonAlg, 'SCTLorentzMonAlg')

    # # If for some really obscure reason you need to instantiate an algorithm
    # # yourself, the AddAlgorithm method will still configure the base 
    # # properties and add the algorithm to the monitoring sequence.
    # helper.AddAlgorithm(myExistingAlg)


    ### STEP 3 ###
    # Edit properties of a algorithm
    myMonAlg.TriggerChain = ''
    # myMonAlg.RandomHist = True

    ############################## WORKAROUND (START) ##########################
    ############################## TO RUN TRACKSUMMARYTOOL #####################

    # Taken from InnerDetector/InDetDigitization/PixelDigitization/python/PixelDigitizationConfigNew.py
    from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg
    InDetPixelConditionsSummaryTool = result.popToolsAndMerge(PixelConditionsSummaryCfg(inputFlags))

    # Taken from Tracking/TrkConfig/python/AtlasExtrapolatorConfig.py
    # AtlasExtrapolatorConfig can give only private extrapolator. We need public extrapolator.
    from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    trackGeomCfg = TrackingGeometrySvcCfg(inputFlags)
    geom_svc = trackGeomCfg.getPrimary() 
    geom_svc.GeometryBuilder.Compactify = False ######## To avoid crash ########
    result.merge(trackGeomCfg)
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    result.merge(MagneticFieldSvcCfg(inputFlags))
    from TrkExTools.TrkExToolsConf import Trk__Navigator
    AtlasNavigator = Trk__Navigator(name = 'AtlasNavigator')
    AtlasNavigator.TrackingGeometrySvc = geom_svc
    result.addPublicTool(AtlasNavigator) 

    # Taken from InnerDetector/InDetExample/InDetRecExample/share/InDetRecLoadTools.py
    from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as Propagator
    InDetPropagator = Propagator(name = 'InDetPropagator')
    InDetPropagator.AccuracyParameter = 0.0001
    InDetPropagator.MaxStraightLineStep = .004
    result.addPublicTool(InDetPropagator)
    from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator
    InDetMaterialUpdator = Trk__MaterialEffectsUpdator(name = "InDetMaterialEffectsUpdator")
    result.addPublicTool(InDetMaterialUpdator)
    InDetSubPropagators = []
    InDetSubUpdators    = []
    # -------------------- set it depending on the geometry ----------------------------------------------------
    # default for ID is (Rk,Mat)
    InDetSubPropagators += [ InDetPropagator.name() ]
    InDetSubUpdators    += [ InDetMaterialUpdator.name() ]
    # default for Calo is (Rk,MatLandau)
    InDetSubPropagators += [ InDetPropagator.name() ]
    InDetSubUpdators    += [ InDetMaterialUpdator.name() ]
    # default for MS is (STEP,Mat)
    #InDetSubPropagators += [ InDetStepPropagator.name() ]
    InDetSubUpdators    += [ InDetMaterialUpdator.name() ]
    from TrkExTools.TrkExToolsConf import Trk__Extrapolator
    InDetExtrapolator = Trk__Extrapolator(name                    = 'InDetExtrapolator',
                                          Propagators             = [ InDetPropagator ],
                                          MaterialEffectsUpdators = [ InDetMaterialUpdator ],
                                          Navigator               = AtlasNavigator,
                                          SubPropagators          = InDetSubPropagators,
                                          SubMEUpdators           = InDetSubUpdators)
    result.addPublicTool(InDetExtrapolator)
    from InDetTestPixelLayer.InDetTestPixelLayerConf import InDet__InDetTestPixelLayerTool
    InDetTestPixelLayerTool = InDet__InDetTestPixelLayerTool(name = "InDetTestPixelLayerTool",
                                                             PixelSummaryTool = InDetPixelConditionsSummaryTool,
                                                             CheckActiveAreas=True,
                                                             CheckDeadRegions=True,
                                                             CheckDisabledFEs=True)
    result.addPublicTool(InDetTestPixelLayerTool)
    from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
    InDetHoleSearchTool = InDet__InDetTrackHoleSearchTool(name = "InDetHoleSearchTool",
                                                          Extrapolator = InDetExtrapolator,
                                                          PixelSummaryTool = InDetPixelConditionsSummaryTool,
                                                          usePixel      = inputFlags.Detector.GeometryPixel,
                                                          useSCT        = inputFlags.Detector.GeometrySCT,
                                                          CountDeadModulesAfterLastHit = True,
                                                          PixelLayerTool = InDetTestPixelLayerTool)
    result.addPublicTool(InDetHoleSearchTool)
    from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
    InDetPrdAssociationTool = InDet__InDetPRD_AssociationToolGangedPixels(name                           = "InDetPrdAssociationTool",
                                                                          PixelClusterAmbiguitiesMapName = "PixelClusterAmbiguitiesMap",
                                                                          SetupCorrect                   = True,
                                                                          addTRToutliers                 = True)
    result.addPublicTool(InDetPrdAssociationTool)
    from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
    InDetTrackSummaryHelperTool = InDet__InDetTrackSummaryHelperTool(name            = "InDetSummaryHelper",
                                                                     AssoTool        = InDetPrdAssociationTool,
                                                                     PixelToTPIDTool = None,
                                                                     TestBLayerTool  = None,
                                                                     RunningTIDE_Ambi = True,
                                                                     DoSharedHits    = False,
                                                                     HoleSearch      = InDetHoleSearchTool,
                                                                     usePixel        = inputFlags.Detector.GeometryPixel,
                                                                     useSCT          = inputFlags.Detector.GeometrySCT,
                                                                     useTRT          = inputFlags.Detector.GeometryTRT)
    from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
    InDetTrackSummaryTool = Trk__TrackSummaryTool(name = "InDetTrackSummaryTool",
                                                  InDetSummaryHelperTool = InDetTrackSummaryHelperTool,
                                                  doSharedHits           = False,
                                                  doHolesInDet           = True,
                                                  TRT_ElectronPidTool    = None,
                                                  TRT_ToT_dEdxTool       = None,
                                                  PixelToTPIDTool        = None)
    ############################## WORKAROUND (END) ############################

    # Set InDetTrackSummaryTool to TrackSummaryTool of SCTLorentzMonAlg
    myMonAlg.TrackSummaryTool = InDetTrackSummaryTool

    ### STEP 4 ###
    # Add some tools. N.B. Do not use your own trigger decion tool. Use the
    # standard one that is included with AthMonitorAlgorithm.


    # set up geometry / conditions
    from AtlasGeoModel.AtlasGeoModelConfig import AtlasGeometryCfg
    result.merge(AtlasGeometryCfg(inputFlags))

    # # Then, add a tool that doesn't have its own configuration function. In
    # # this example, no accumulator is returned, so no merge is necessary.
    # from MyDomainPackage.MyDomainPackageConf import MyDomainTool
    # myMonAlg.MyDomainTool = MyDomainTool()

    # Add a generic monitoring tool (a "group" in old language). The returned 
    # object here is the standard GenericMonitoringTool.
    myMonGroup = helper.addGroup(
        myMonAlg,
        "SCTLorentzMonitor",
        "SCT/GENERAL/"
    )

    ### STEP 5 ###
    # Configure histograms
    N_BARRELS = 4
    nSidesInclBoth = 2 # 0: Side 0, 1: Side 1
    #bothSides = 2
    nSurfaces = 2 # 0: 100, 1: 111
    surfaceNames = ["_100",   "_111"]
    surfaceNames2 = ["_100_",   "_111_"]
    surfaceTitles = ["100 - ", "111 - "]
    sideNames = ["_0", "_1"]
    for l in range(N_BARRELS):
        for iSurface in range(nSurfaces):
            for side in range(nSidesInclBoth):
                xVar = "phiToWafer_"+str(l)+surfaceNames[iSurface]+sideNames[side]
                yVar = "nStrip_"+str(l)+surfaceNames[iSurface]+sideNames[side]
                histTitle = surfaceTitles[iSurface]+"Inc. Angle vs nStrips for Layer Side"+str(l)+str(side)
                histName = "h_phiVsNstrips"+surfaceNames2[iSurface]+str(l)+"Side"+str(side)
                myMonGroup.defineHistogram(varname=xVar+","+yVar+";"+histName, # ; means alias
                                           type="TProfile",
                                           title=histTitle+";#phi to Wafer;Num of Strips",
                                           path="lorentz", # path cannot be "".
                                           xbins=360, xmin=-90., xmax=90.)

    ### STEP 6 ###
    # Finalize. The return value should be a tuple of the ComponentAccumulator
    # and the sequence containing the created algorithms. If we haven't called
    # any configuration other than the AthMonitorCfgHelper here, then we can 
    # just return directly (and not create "result" above)
    #return helper.result()
    
    # # Otherwise, merge with result object and return
    result.merge(helper.result())
    return result
    
if __name__ == "__main__": 
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc16_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.recon.ESD.e3668_s3170_r10572_homeMade.pool.root"]
    #ConfigFlags.Input.Files = [" /afs/cern.ch/user/e/ebergeas/work/public/triggermonitoring/nigtly_2019-08-24T2130/AOD.pool.root "]
    ConfigFlags.Input.isMC = True
    ConfigFlags.Output.HISTFileName = 'SCTLorentzMonOutput.root'
    ConfigFlags.GeoModel.Align.Dynamic = False
    ConfigFlags.Detector.GeometryID = True
    ConfigFlags.Detector.GeometryPixel = True
    ConfigFlags.Detector.GeometrySCT = True
    ConfigFlags.Detector.GeometryTRT = True
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesSerialCfg()
    cfg.merge(PoolReadCfg(ConfigFlags))

    from AtlasGeoModel.AtlasGeoModelConfig import AtlasGeometryCfg
    geoCfg=AtlasGeometryCfg(ConfigFlags)
    cfg.merge(geoCfg)

    sctLorentzMonAcc = SCTLorentzMonAlgConfig(ConfigFlags)
    cfg.merge(sctLorentzMonAcc)

    cfg.run()
