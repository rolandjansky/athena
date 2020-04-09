#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file TRTMonitoringRun3_Alg.py
@author N. Belyaev
@date 20.09.2019
@brief MT-compatible TRT Monitoring Tool for Run III based on the
AthenaMonitoring package
'''


def TRTMonitoringRun3_AlgConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    # === STEP 1 === #
    # Define one top-level monitoring algorithm. The new configuration
    # framework uses a component accumulator.
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    from TrkConfig.TrackCollectionReadConfig import TrackCollectionReadCfg
    result.merge (TrackCollectionReadCfg (inputFlags, 'CombinedInDetTracks'))
    result.merge (TrackCollectionReadCfg (inputFlags, 'Tracks'))

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags, 'TRTMonitoringCfg')

    # === STEP 2 === #
    # Adding an algorithm to the helper. Here, we will use the example
    # algorithm in the AthenaMonitoring package. Just pass the type to the
    # helper. Then, the helper will instantiate an instance and set up the
    # base class configuration following the inputFlags. The returned object
    # is the algorithm.
    from AthenaConfiguration.ComponentFactory import CompFactory
    algTRTMonitoringRun3 = helper.addAlgorithm(CompFactory.TRTMonitoringRun3_Alg,
                                               'AlgTRTMonitoringRun3')

    # You can actually make multiple instances of the same algorithm and give
    # them different configurations

    # # If for some really obscure reason you need to instantiate an algorithm
    # # yourself, the AddAlgorithm method will still configure the base
    # # properties and add the algorithm to the monitoring sequence.
    # helper.AddAlgorithm(myExistingAlg)

    # === STEP 3 === #
    # Edit properties of a algorithm
    # some generic property
    # AlgTRTMonitoringRun3.RandomHist = True
    # to enable a trigger filter, for example:
    algTRTMonitoringRun3.TriggerChain = ''

    # === STEP 4 === #
    # Add some tools. N.B. Do not use your own trigger decion tool. Use the
    # standard one that is included with AthMonitorAlgorithm.

    # # First, add a tool that's set up by a different configuration function.
    # # In this case, CaloNoiseToolCfg returns its own component accumulator,
    # # which must be merged with the one from this function.
    # from CaloTools.CaloNoiseToolConfig import CaloNoiseToolCfg
    # caloNoiseAcc, caloNoiseTool = CaloNoiseToolCfg(inputFlags)
    # result.merge(caloNoiseAcc)
    # AlgTRTMonitoringRun3.CaloNoiseTool = caloNoiseTool
    from AtlasGeoModel.AtlasGeoModelConfig import AtlasGeometryCfg
    result.merge(AtlasGeometryCfg(inputFlags))

    from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
    if InDetTrigFlags.useConditionsClasses():
        _useDCS = False
        _onlineMode = False
        _prefix = ''
        #_prefix = ''
    from InDetTrigRecExample.InDetTrigConfigConditions import TRT_ConditionsSetup
    TRT_ConditionsSetup.config(useDCS=_useDCS, onlineMode=_onlineMode,prefix=_prefix)
    TRT_ConditionsSetup.lock()
    TRT_ConditionsSetup.createSvc()
    from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
    result.merge(addFoldersSplitOnline(inputFlags, "TRT","/TRT/Onl/Calib/RT","/TRT/Calib/RT",className="TRTCond::RtRelationMultChanContainer"))
    result.merge(addFoldersSplitOnline(inputFlags, "TRT","/TRT/Onl/Calib/T0","/TRT/Calib/T0",className="TRTCond::StrawT0MultChanContainer"))

    # # Then, add a tool that doesn't have its own configuration function. In
    # # this example, no accumulator is returned, so no merge is necessary.
    # from MyDomainPackage.MyDomainPackageConf import MyDomainTool
    # AlgTRTMonitoringRun3.MyDomainTool = MyDomainTool()

    ############################## WORKAROUND (START) ##########################
    ############################ TO RUN TRACKSUMMARYTOOL #######################

    # Taken from InnerDetector/InDetDigitization/PixelDigitization/python/PixelDigitizationConfigNew.py
    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelConfigCondAlgCfg
    result.merge(PixelConfigCondAlgCfg(inputFlags,
                                    UseCalibConditions=True,
                                    UseDeadmapConditions=True,
                                    UseDCSStateConditions=False,
                                    UseDCSStatusConditions=False,
                                    UseDCSHVConditions=True,
                                    UseDCSTemperatureConditions=True,
                                    UseTDAQConditions=False))

    from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg
    InDetPixelConditionsSummaryTool = result.popToolsAndMerge(PixelConditionsSummaryCfg(inputFlags))

    # Taken from Tracking/TrkExtrapolation/TrkExTools/python/AtlasExtrapolatorConfig.py
    # AtlasExtrapolatorConfig can give only private extrapolator. We need public extrapolator.
    #from TrkDetDescrSvc.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    trackGeomCfg = TrackingGeometrySvcCfg(inputFlags)
    geom_svc = trackGeomCfg.getPrimary() 
    geom_svc.GeometryBuilder.Compactify = False ######## To avoid crash ########
    result.merge(trackGeomCfg)
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    result.merge(MagneticFieldSvcCfg(inputFlags))
    AtlasNavigator = CompFactory.Trk.Navigator(name = 'AtlasNavigator')
    AtlasNavigator.TrackingGeometrySvc = geom_svc
    result.addPublicTool(AtlasNavigator) 

    # Taken from InnerDetector/InDetExample/InDetRecExample/share/InDetRecLoadTools.py
    Propagator=CompFactory.Trk.RungeKuttaPropagator
    InDetPropagator = Propagator(name = 'InDetPropagator')
    InDetPropagator.AccuracyParameter = 0.0001
    InDetPropagator.MaxStraightLineStep = .004
    result.addPublicTool(InDetPropagator)
    InDetMaterialUpdator = CompFactory.Trk.MaterialEffectsUpdator(name = "InDetMaterialEffectsUpdator")
    result.addPublicTool(InDetMaterialUpdator)
    InDetSubPropagators = []
    InDetSubUpdators    = []
    # -------------------- set it depending on the geometry ----------------------------------------------------
    # default for ID is (Rk,Mat)
    InDetSubPropagators += [ InDetPropagator.name ]
    InDetSubUpdators    += [ InDetMaterialUpdator.name ]
    # default for Calo is (Rk,MatLandau)
    InDetSubPropagators += [ InDetPropagator.name ]
    InDetSubUpdators    += [ InDetMaterialUpdator.name ]
    # default for MS is (STEP,Mat)
    #InDetSubPropagators += [ InDetStepPropagator.name ]
    InDetSubUpdators    += [ InDetMaterialUpdator.name ]

    InDetExtrapolator = CompFactory.Trk.Extrapolator(name                    = 'InDetExtrapolator',
                                                     Propagators             = [ InDetPropagator ],
                                                     MaterialEffectsUpdators = [ InDetMaterialUpdator ],
                                                     Navigator               = AtlasNavigator,
                                                     SubPropagators          = InDetSubPropagators,
                                                     SubMEUpdators           = InDetSubUpdators)
    result.addPublicTool(InDetExtrapolator)
    InDetTestPixelLayerTool = CompFactory.InDet.InDetTestPixelLayerTool(name = "InDetTestPixelLayerTool",
                                                                         PixelSummaryTool = InDetPixelConditionsSummaryTool,
                                                                         CheckActiveAreas=True,
                                                                         CheckDeadRegions=True,
                                                                         CheckDisabledFEs=True)
    result.addPublicTool(InDetTestPixelLayerTool)
    InDetHoleSearchTool = CompFactory.InDet.InDetTrackHoleSearchTool(name = "InDetHoleSearchTool",
                                                                     Extrapolator = InDetExtrapolator,
                                                                     PixelSummaryTool = InDetPixelConditionsSummaryTool,
                                                                     usePixel      = inputFlags.Detector.GeometryPixel,
                                                                     useSCT        = inputFlags.Detector.GeometrySCT,
                                                                     CountDeadModulesAfterLastHit = True,
                                                                     PixelLayerTool = InDetTestPixelLayerTool)
    result.addPublicTool(InDetHoleSearchTool)
    InDetPrdAssociationTool = CompFactory.InDet.InDetPRD_AssociationToolGangedPixels(name                           = "InDetPrdAssociationTool",
                                                                                     PixelClusterAmbiguitiesMapName = "PixelClusterAmbiguitiesMap",
                                                                                     SetupCorrect                   = True,
                                                                                     addTRToutliers                 = True)
    result.addPublicTool(InDetPrdAssociationTool)
    InDetTrackSummaryHelperTool = CompFactory.InDet.InDetTrackSummaryHelperTool(name            = "InDetSummaryHelper",
                                                                                AssoTool        = InDetPrdAssociationTool,
                                                                                PixelToTPIDTool = None,
                                                                                TestBLayerTool  = None,
                                                                                RunningTIDE_Ambi = True,
                                                                                DoSharedHits    = False,
                                                                                HoleSearch      = InDetHoleSearchTool,
                                                                                usePixel        = inputFlags.Detector.GeometryPixel,
                                                                                useSCT          = inputFlags.Detector.GeometrySCT,
                                                                                useTRT          = inputFlags.Detector.GeometryTRT)

    InDetTrackSummaryTool = CompFactory.Trk.TrackSummaryTool(name = "InDetTrackSummaryTool",
                                                              InDetSummaryHelperTool = InDetTrackSummaryHelperTool,
                                                              doSharedHits           = False,
                                                              doHolesInDet           = True,
                                                              TRT_ElectronPidTool    = None,
                                                              TRT_ToT_dEdxTool       = None,
                                                              PixelToTPIDTool        = None)
    ############################## WORKAROUND (END) ############################

    # Set InDetTrackSummaryTool to TrackSummaryTool of SCTLorentzMonAlg
    algTRTMonitoringRun3.TrackSummaryTool = InDetTrackSummaryTool

    # === STEP 5 === #
    # Configure histograms

    barrel_or_endcap       = ('Barrel', 'EndCap')
    be_id                  = ('B', 'E')
    side_id                = ('A', 'C')
    s_Straw_max            = (1642, 3840)
    s_iChip_max            = (104, 240)
    s_numberOfStacks       = (32, 32)
    newRun                 = True
    m_doShift              = True
    m_doExpert             = True
    m_doStraws             = True
    m_doChips              = True
    newLumiBlock           = True
    m_DistToStraw          = 0.4
    s_numberOfBarrelStacks = 32
    m_nphi_bins            = 360
    m_minTRThits           = 10

    barrel_or_endcap = ('Barrel', 'EndCap')

    for ibe in range(2):
        oss_distance = m_DistToStraw
        distance     = str(oss_distance)
        regionTag    = ' (' + barrel_or_endcap[ibe] + ')'

        for i in range(s_numberOfStacks[ibe]*2):
            myGroup = helper.addGroup(algTRTMonitoringRun3,'TRTTrackHistograms{0}{1}'.format(ibe,i))

            nBarrelModules = 0

            if ibe == 0:
                if i < s_numberOfStacks[ibe]:
                    nBarrelModules = i + 1
                    oss = 'TRT/Barrel/Stack{0}A'.format(nBarrelModules)
                elif i >= s_numberOfStacks[ibe]:
                    nBarrelModules = i + 1 - 32
                    oss = 'TRT/Barrel/Stack{0}C'.format(nBarrelModules)
            elif ibe == 1:
                if i < s_numberOfStacks[ibe]:
                    nBarrelModules = i + 1
                    oss = 'TRT/EndcapA/Sector{0}'.format(nBarrelModules)
                elif i >= s_numberOfStacks[ibe]:
                    nBarrelModules = i + 1 - 32
                    oss = 'TRT/EndcapC/Sector{0}'.format(nBarrelModules)      

            if newRun and m_doExpert and m_doStraws:
                myGroup.defineHistogram('HitWonTMapS_passed,HitWonTMapS;hHitWonTMapS{0}{1}'.format(ibe,i),type='TEfficiency',title='Leading Edge on Track in Time Window: Straws;Straw Number in Stack;Probability',path=oss,xbins=s_Straw_max[ibe],xmin=0,xmax=s_Straw_max[ibe])
                myGroup.defineHistogram('HitTronTMapS_x,HitTronTMapS_y;hHitTronTMapS{0}{1}'.format(ibe,i),type='TProfile',title='Mean Trailing Edge on Track: Straws;Straw Number in Stack;Time (ns)',path=oss,xbins=s_Straw_max[ibe],xmin=0,xmax=s_Straw_max[ibe])  #TProfile
                myGroup.defineHistogram('HitAonTMapS_passed,HitAonTMapS;hHitAonTMapS{0}{1}'.format(ibe,i),type='TEfficiency',title='Any LL Bit on Track: Straws;Straw Number in Stack;Probability',path=oss,xbins=s_Straw_max[ibe],xmin=0,xmax=s_Straw_max[ibe])
                myGroup.defineHistogram('EfficiencyS_passed,EfficiencyS;hStrawsEff{0}{1}'.format(ibe,i),type='TEfficiency',title='Straw Efficiency with {0} mm Cut;Straw Number in Stack;Efficiency'.format(distance),path=oss,xbins=s_Straw_max[ibe],xmin=0,xmax=s_Straw_max[ibe])  #TProfile
                myGroup.defineHistogram('HitHonTMapS_passed,HitHonTMapS;hHitHonTMapS{0}{1}'.format(ibe,i),type='TEfficiency',title='HL Hit on Track: Straws;Straw Number in Stack;Probability',path=oss,xbins=s_Straw_max[ibe],xmin=0,xmax=s_Straw_max[ibe])
                myGroup.defineHistogram('HitHWonTMapS_passed,HitHWonTMapS;hHitHWonTMapS{0}{1}'.format(ibe,i),type='TEfficiency',title='HL Hit(In Time Window) on Track: Straws;Straw Number in Stack;Probability',path=oss,xbins=s_Straw_max[ibe],xmin=0,xmax=s_Straw_max[ibe])
                myGroup.defineHistogram('HitAWonTMapS_passed,HitAWonTMapS;hHitAWonTMapS{0}{1}'.format(ibe,i),type='TEfficiency',title='Any LL Bit on Track in Time Window: Straws;Straw Number in Stack;Probability',path=oss,xbins=s_Straw_max[ibe],xmin=0,xmax=s_Straw_max[ibe])
                myGroup.defineHistogram('HitToTonTMapS_x,HitToTonTMapS_y;hHitToTonTMapS{0}{1}'.format(ibe,i),type='TProfile',title='Mean ToT on Track: Straws;Straw Number in Stack;Time (ns)',path=oss,xbins=s_Straw_max[ibe],xmin=0,xmax=s_Straw_max[ibe])  #TProfile
                myGroup.defineHistogram('ValidRawDriftTimeonTrkS_x,ValidRawDriftTimeonTrkS_y;hValidRawDriftTimeonTrk{0}{1}'.format(ibe,i),type='TProfile',title='Valid Raw Drift Time on Track: Straws;Straw Number in Stack;Time (ns)',path=oss,xbins=s_Straw_max[ibe],xmin=0,xmax=s_Straw_max[ibe])  #TProfile
                myGroup.defineHistogram('HitTronTwEPCMapS_x,HitTronTwEPCMapS_y;hHitTronTwEPCMapS{0}{1}'.format(ibe,i),type='TProfile',title='Mean Trailing Edge on Track (with Event Phase Correction): Straws;Straw Number in Stack;Time (ns)',path=oss,xbins=s_Straw_max[ibe],xmin=0,xmax=s_Straw_max[ibe])  #TProfile

            if newRun and m_doExpert and m_doChips:
                myGroup.defineHistogram('HitWonTMapC_passed,HitWonTMapC;hHitWonTMapC{0}{1}'.format(ibe,i),type='TEfficiency',title='Leading Edge on Track in Time Window: Chips;Chip Number in Stack;Probability',path=oss,xbins=s_iChip_max[ibe],xmin=0,xmax=s_iChip_max[ibe])
                myGroup.defineHistogram('HitTronTMapC_x,HitTronTMapC_y;hHitTronTMapC{0}{1}'.format(ibe,i),type='TProfile',title='Mean Trailing Edge on Track: Chips;Chip Number in Stack;Time (ns)',path=oss,xbins=s_iChip_max[ibe],xmin=0,xmax=s_iChip_max[ibe])  #TProfile
                myGroup.defineHistogram('HitAonTMapC_passed,HitAonTMapC;hHitAonTMapC{0}{1}'.format(ibe,i),type='TEfficiency',title='Any LL Bit on Track: Chips;Chip Number in Stack;Probability',path=oss,xbins=s_iChip_max[ibe],xmin=0,xmax=s_iChip_max[ibe])
                myGroup.defineHistogram('EfficiencyC_passed,EfficiencyC;hChipsEff{0}{1}'.format(ibe,i),type='TEfficiency',title='Chip Efficiency with {0} mm Cut;Chip Number in Stack;'.format(distance),path=oss,xbins=s_iChip_max[ibe],xmin=0,xmax=s_iChip_max[ibe])  #TProfile
                myGroup.defineHistogram('HitAWonTMapC_passed,HitAWonTMapC;hHitAWonTMapC{0}{1}'.format(ibe,i),type='TEfficiency',title='Any LL Bit on Track in Time Window: Chips;Chip Number in Stack;Probability',path=oss,xbins=s_iChip_max[ibe],xmin=0,xmax=s_iChip_max[ibe])
                myGroup.defineHistogram('HitHonTMapC_passed,HitHonTMapC;hHitHonTMapC{0}{1}'.format(ibe,i),type='TEfficiency',title='HL Hit on Track: Chips;Chip Number in Stack;Probability',path=oss,xbins=s_iChip_max[ibe],xmin=0,xmax=s_iChip_max[ibe])
                myGroup.defineHistogram('HitHWonTMapC_passed,HitHWonTMapC;hHitHWonTMapC{0}{1}'.format(ibe,i),type='TEfficiency',title='HL Hit(In time Window) on Track: Chips;Chip Number in Stack;Probability',path=oss,xbins=s_iChip_max[ibe],xmin=0,xmax=s_iChip_max[ibe])
                myGroup.defineHistogram('HitToTonTMapC_x,HitToTonTMapC_y;hHitToTonTMapC{0}{1}'.format(ibe,i),type='TProfile',title='Chip Number in Stack;Time (ns);Chip Number in Stack;Time (ns)',path=oss,xbins=s_iChip_max[ibe],xmin=0,xmax=s_iChip_max[ibe])  #TProfile
                myGroup.defineHistogram('ValidRawDriftTimeonTrkC_x,ValidRawDriftTimeonTrkC_y;hValidRawDriftTimeonTrkC{0}{1}'.format(ibe,i),type='TProfile',title='Valid Raw Drift Time on Track: Chips;Chip Number in Stack;Time (ns)',path=oss,xbins=s_iChip_max[ibe],xmin=0,xmax=s_iChip_max[ibe])  #TProfile
                myGroup.defineHistogram('HitTronTwEPCMapC_x,HitTronTwEPCMapC_y;hHitTronTwEPCMapC{0}{1}'.format(ibe,i),type='TProfile',title='Mean Trailing Edge on Track (with Event Phase Correction): Chips;Chip Number in Stack;Time (ns)',path=oss,xbins=s_iChip_max[ibe],xmin=0,xmax=s_iChip_max[ibe])  #TProfile

    if newRun:
        myGroup = helper.addGroup(algTRTMonitoringRun3,'TRTEfficiencyHistograms')
        myGroup.defineHistogram('Efficiency_eta_passed,Efficiency_eta;hefficiency_eta',type='TEfficiency',title='Efficiency vs #eta;#eta;Efficiency',path='TRT/Efficiency',xbins=50,xmin=-2.8,xmax=2.8)  #TProfile
        myGroup.defineHistogram('Efficiency_phi_passed,Efficiency_phi;hefficiency_phi',type='TEfficiency',title='Efficiency vs #phi;#phi (deg);Efficiency',path='TRT/Efficiency',xbins=50,xmin=-3.2,xmax=3.2)  #TProfile
        myGroup.defineHistogram('Efficiency_pt_passed,Efficiency_pt;hefficiency_pt',type='TEfficiency',title='Efficiency vs pT;pT (GeV);Efficiency',path='TRT/Efficiency',xbins=50,xmin=0,xmax=10)  #TProfile
        myGroup.defineHistogram('Efficiency_z0_passed,Efficiency_z0;hefficiency_z0',type='TEfficiency',title='Efficiency vs z0;z0;Efficiency',path='TRT/Efficiency',xbins=50,xmin=-200,xmax=200)  #TProfile
        myGroup.defineHistogram('EfficiencyBarrel_locR_passed,EfficiencyBarrel_locR;hefficiencyBarrel_locR',type='TEfficiency',title='Efficiency vs Track-to-Wire Distance for Xenon Straws (Barrel);Track-to-Wire Distance (mm);Efficiency',path='TRT/Efficiency',xbins=50,xmin=-2.5,xmax=2.5)  #TProfile
        myGroup.defineHistogram('EfficiencyBarrel_locR_Ar_passed,EfficiencyBarrel_locR_Ar;hefficiencyBarrel_locR_Ar',type='TEfficiency',title='Efficiency vs Track-to-Wire Distance for Argon Straws' ' (Barrel);Track-to-Wire Distance (mm);Efficiency',path='TRT/Efficiency',xbins=50,xmin=-2.5,xmax=2.5)  #TProfile
        myGroup.defineHistogram('EfficiencyBarrelMap_passed,EfficiencyBarrelMap;hefficiencyMap0',type='TEfficiency',title='Straw Efficiency Map' ' (Barrel);Straw Number;Efficiency',path='TRT/Efficiency',xbins=s_Straw_max[0],xmin=0,xmax=s_Straw_max[0])  #TProfile
        myGroup.defineHistogram('EfficiencyEndCapMap_passed,EfficiencyEndCapMap;hefficiencyMap1',type='TEfficiency',title='Straw Efficiency Map' ' (Endcap);Straw Number;Efficiency',path='TRT/Efficiency',xbins=s_Straw_max[1],xmin=0,xmax=s_Straw_max[1])  #TProfile

        for iside in range(2):
            myGroup = helper.addGroup(algTRTMonitoringRun3,'TRTEfficiencyHistograms{0}'.format(iside))
            regionTag = ' (' + be_id[1]   + side_id[iside] + ')'
            myGroup.defineHistogram('EfficiencyEndCap_locR_passed,EfficiencyEndCap_locR;hEfficiencyEndCap{0}_locR'.format(side_id[iside]),type='TEfficiency',title='Efficiency vs Track-to-Wire Distance for Xenon Straws{0};Track-to-Wire Distance (mm);Efficiency'.format(regionTag),path='TRT/Efficiency',xbins=50,xmin=-2.5,xmax=2.5)  #TProfile
            myGroup.defineHistogram('EfficiencyEndCap_locR_Ar_passed,EfficiencyEndCap_locR_Ar;hEfficiencyEndCap{0}_locR_Ar'.format(side_id[iside]),type='TEfficiency',title='Efficiency vs Track-to-Wire Distance for Argon Straws{0};Track-to-Wire Distance (mm);Efficiency'.format(regionTag),path='TRT/Efficiency',xbins=50,xmin=-2.5,xmax=2.5)  #TProfile

        for ibe in range(2):
            for iside in range(2):
                regionTag = ' (' + be_id[ibe] + side_id[iside] + ')'

                if m_doExpert:
                    imintmp = s_numberOfBarrelStacks*iside
                    imaxtmp = s_numberOfBarrelStacks*(iside + 1)

                    for i in range(imintmp,imaxtmp):
                        if ibe == 0: 
                            oss = 'TRT/Barrel/Stack' + str(i + 1 - 32*iside) + str(side_id[iside])
                        elif ibe == 1:
                            oss = 'TRT/Endcap' + str(side_id[iside]) + '/Sector' + str(i + 1 - 32*iside)
                            
                        myGroup = helper.addGroup(algTRTMonitoringRun3,'TRTEfficiencyHistograms{0}{1}'.format(ibe,i))
                        myGroup.defineHistogram('EfficiencyS_passed,EfficiencyS;hHitEfficiencyS{0}{1}'.format(ibe,i),type='TEfficiency',title='Straw Efficiency Map;Straw Number;Efficiency',path=oss,xbins=s_Straw_max[ibe],xmin=0.5,xmax=s_Straw_max[ibe])  #TProfile
                        myGroup.defineHistogram('EfficiencyC_passed,EfficiencyC;hHitEfficiencyC{0}{1}'.format(ibe,i),type='TEfficiency',title='Chip Efficiency Map;Chip Number;Efficiency',  path=oss,xbins=s_iChip_max[ibe],xmin=0.5,xmax=s_iChip_max[ibe])  #TProfile

    # Arrays for Aging
    gas = ('in_A', 'in_B', 'out_A', 'out_B')
    Mod = ('1', '2', '3', 'shortP', 'shortN')

    for ibe in range(2):
        myGroup = helper.addGroup(algTRTMonitoringRun3,'ShiftTRTTrackHistograms{0}'.format(ibe))
        regionTag = ' (' + barrel_or_endcap[ibe] + ')'

        if newRun and m_doShift:
            if ibe == 0:
                myGroup.defineHistogram('EvtPhase_passed,EvtPhase;hEvtPhase',type='TEfficiency',title='Event Phase Correction Factor;Event Phase (ns);Entries',path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=200,xmin=-50,xmax=50)
                myGroup.defineHistogram('EvtPhaseVsTrig_x,EvtPhaseVsTrig_y;hEvtPhaseVsTrig',type='TH2F',title='Event Phase vs L1 Trigger Item;Event Phase (ns);L1 Trigger Item',path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=300,xmin=-200,xmax=100,ybins=256,ymin=-0.5,ymax=255.5)  #TH2F
                myGroup.defineHistogram('EvtPhaseDetPhi_B_x,EvtPhaseDetPhi_B_y;hEvtPhaseDetPhi',type='TProfile',title='Event Phase vs #phi (2D){0};#phi (deg);Event Phase from Tracks per Event'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=m_nphi_bins,xmin=0.,xmax=360.)  #TProfile
                myGroup.defineHistogram('RtRelation_B_x,RtRelation_B_y;hrtRelation',type='TH2F',title='R(t) Relation for Xenon Straws{0};Measured Leading Edge (ns);Track-to-Wire Distance (mm)'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=30,xmin=-12.5,xmax=81.25,ybins=50,ymin=0.,ymax=2.5)  #TH2F
                myGroup.defineHistogram('NumHoTDetPhi_B_x,NumHoTDetPhi_B_y;hNumHoTDetPhi',type='TProfile',title='Number of Hits per Track with {0} mm Cut vs #phi{1};#phi (deg);Hits per Track, TRT Hits >= {2}'.format(distance,regionTag,m_minTRThits),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=m_nphi_bins,xmin=0.,xmax=360.)  #TProfile
                myGroup.defineHistogram('TronTDist_B_passed,TronTDist_B;hTronTDist',type='TEfficiency',title='Trailing Edge Distribution on Track for Xenon Straws{0};Trailing Edge (ns);Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=26,xmin=-0.5,xmax=80.75)
                myGroup.defineHistogram('DriftTimeonTrkDist_B_passed,DriftTimeonTrkDist_B;hDriftTimeonTrkDist',type='TEfficiency',title='Drift Time Distribution on Track for Xenon Straws{0};Drift Time (ns);Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=32,xmin=0,xmax=100)
                myGroup.defineHistogram('NumTrksDetPhi_B_passed,NumTrksDetPhi_B;hNumTrksDetPhi_B',type='TEfficiency',title='Number of Reconstructed Tracks vs #phi (2D){0};#phi (deg);Number of Tracks'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=60,xmin=0,xmax=360)

                myGroup.defineHistogram('DriftTimeonTrkDist_B_Ar_passed,DriftTimeonTrkDist_B_Ar;hDriftTimeonTrkDist_Ar',type='TEfficiency',title='Drift Time Distribution on Track for Argon Straws{0};Drift Time (ns);Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=32,xmin=0,xmax=100)
                myGroup.defineHistogram('TronTDist_B_Ar_passed,TronTDist_B_Ar;hTronTDist_Ar',type='TEfficiency',title='Trailing Edge Distribution on Track for Argon Straws{0};Trailing Edge (ns);Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=26,xmin=-0.5,xmax=80.75)
                myGroup.defineHistogram('RtRelation_B_Ar_x,RtRelation_B_Ar_y;hrtRelation_Ar',type='TH2F',title='R(t) Relation for Argon Straws{0};Measured Leading Edge (ns);Track-to-Wire Distance (mm)'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=30,xmin=-12.5,xmax=81.25,ybins=50,ymin=0,ymax=2.5)  #TH2F
                myGroup.defineHistogram('Pull_Biased_Barrel_passed,Pull_Biased_Barrel;hPull_Biased_Barrel',type='TEfficiency',title='Biased Track Pulls for Barrel Hits;Pulls;Entries',path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=200,xmin=-2.5,xmax=2.5)
                myGroup.defineHistogram('Residual_B_Ar_passed,Residual_B_Ar;hResidual_Ar',type='TEfficiency',title='Residuals for Argon Straws{0};Hit-to-Track Distance (mm);Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=200,xmin=-2.5,xmax=2.5)
                myGroup.defineHistogram('Residual_B_Ar_20GeV_passed,Residual_B_Ar_20GeV;hResidual_Ar_20GeV',type='TEfficiency',title='Residuals for Argon Straws{0} (After 20GeV pT cut);Hit-to-Track Distance (mm);Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=200,xmin=-2.5,xmax=2.5)
                myGroup.defineHistogram('AvgTroTDetPhi_B_Ar_x,AvgTroTDetPhi_B_Ar_y;hAvgTroTDetPhi_Ar',type='TProfile',title='Avg. Trailing Edge on Track vs #phi (2D) for Argon{0};#phi (deg);Trailing Edge (ns)'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=m_nphi_bins,xmin=0,xmax=360)  #TProfile
                myGroup.defineHistogram('TimeResidual_B_Ar_passed,TimeResidual_B_Ar;hTimeResidual_Ar',type='TEfficiency',title='Time Residuals for Argon Straws{0};Time Residual (ns);Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=200,xmin=-20,xmax=20)
                myGroup.defineHistogram('WireToTrkPosition_B_Ar_passed,WireToTrkPosition_B_Ar;hWireToTrkPosition_Ar',type='TEfficiency',title='Track-to-Wire Distance for Argon{0};Track-to-Wire Distance (mm);Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=100,xmin=-5,xmax=5)
                myGroup.defineHistogram('HtoLRatioOnTrack_B_Ar_passed,HtoLRatioOnTrack_B_Ar;hHtoLRatioOnTrack_Ar',type='TEfficiency',title='HL/LL Ratio per Reconstructed Track for Argon{0};HL/LL Ratio;Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=50,xmin=0,xmax=1)  #for argon

                myGroup.defineHistogram('HtoLRatioOnTrack_B_Xe_passed,HtoLRatioOnTrack_B_Xe;hHtoLRatioOnTrack_Xe',type='TEfficiency',title='HL/LL Ratio per Reconstructed Track for Xenon{0};HL/LL Ratio;Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=50,xmin=0,xmax=1)  #for xenon
                myGroup.defineHistogram('Residual_B_passed,Residual_B;hResidual',type='TEfficiency',title='Residuals for Xenon Straws{0};Hit-to-Track Distance (mm);Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=200,xmin=-2.5,xmax=2.5)
                myGroup.defineHistogram('Residual_B_20GeV_passed,Residual_B_20GeV;hResidual_20GeV',type='TEfficiency',title='Residuals for Xenon Straws{0} (After 20GeV pT cut);Hit-to-Track Distance (mm);Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=200,xmin=-2.5,xmax=2.5)
                myGroup.defineHistogram('TimeResidual_B_passed,TimeResidual_B;hTimeResidual',type='TEfficiency',title='Time Residuals for Xenon Straws{0};Time Residual (ns);Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=200,xmin=-20,xmax=20)
                myGroup.defineHistogram('WireToTrkPosition_B_passed,WireToTrkPosition_B;hWireToTrkPosition',type='TEfficiency',title='Track-to-Wire Distance for Xenon{0};Track-to-Wire Distance (mm);Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=100,xmin=-5,xmax=5)
                myGroup.defineHistogram('NumSwLLWoT_B_passed,NumSwLLWoT_B;hNumSwLLWoT',type='TEfficiency',title='Number of Straws with Hits on Track in Time Window{0};Number of LL Hits per Track;Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=150,xmin=0,xmax=150)
                myGroup.defineHistogram('AvgTroTDetPhi_B_x,AvgTroTDetPhi_B_y;hAvgTroTDetPhi',type='TProfile',title='Avg. Trailing Edge on Track vs #phi (2D) for Xenon{0};#phi (deg);Trailing Edge (ns)'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=m_nphi_bins,xmin=0,xmax=360)  #TProfile
                myGroup.defineHistogram('HLhitOnTrack_B_passed,HLhitOnTrack_B;hHLhitOnTrack',type='TEfficiency',title='Number of HL Hits per Reconstructed Track{0};Number of HL Hits per Track;Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=50,xmin=0,xmax=50)
                myGroup.defineHistogram('HtoLRatioOnTrack_B_passed,HtoLRatioOnTrack_B;hHtoLRatioOnTrack',type='TEfficiency',title='HL/LL Ratio per Reconstructed Track for All{0};HL/LL Ratio;Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=50,xmin=0,xmax=1)
                myGroup.defineHistogram('HitWonTMap_B_passed,HitWonTMap_B;hHitWonTMap',type='TEfficiency',title='Leading Edge in Time Window per Reconstructed Track{0};Straw Number;Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=s_Straw_max[0],xmin=0,xmax=s_Straw_max[0])
                myGroup.defineHistogram('StrawEffDetPhi_B_passed,StrawEffDetPhi_B;hStrawEffDetPhi',type='TEfficiency',title='Straw Efficiency on Track with {0} mm Cut vs #phi(2D){1};Stack;Avg. Straw Efficiency'.format(distance,regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=32,xmin=0,xmax=32)  #TProfile
            elif ibe == 1:
                myGroup.defineHistogram('Pull_Biased_EndCap_passed,Pull_Biased_EndCap;hPull_Biased_EndCap',type='TEfficiency',title='Biased Track Pulls for EndCap Hits;Pulls;Entries',path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=200,xmin=-2.5,xmax=2.5)

                for iside in range(2):
                    myGroup = helper.addGroup(algTRTMonitoringRun3,'ShiftTRTTrackHistograms{0}{1}'.format(ibe,iside))

                    regionTag = ' (' + be_id[ibe] + side_id[iside] + ')'
                    myGroup.defineHistogram('EvtPhaseDetPhi_E_x,EvtPhaseDetPhi_E_y;hEvtPhaseDetPhi_{0}'.format(side_id[iside]),type='TProfile',title='Event Phase vs #phi (2D){0};#phi (deg);Event Phase from Tracks per Event'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=m_nphi_bins,xmin=0,xmax=360)  #TProfile
                    myGroup.defineHistogram('RtRelation_E_x,RtRelation_E_y;hrtRelation_{0}'.format(side_id[iside]),type='TH2F',title='R(t) Relation for Xenon Straws{0};Measured Leading Edge (ns);Track-to-Wire Distance (mm)'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=30,xmin=-12.5,xmax=81.25,ybins=50,ymin=0,ymax=2.5)  #TH2F
                    myGroup.defineHistogram('NumHoTDetPhi_E_x,NumHoTDetPhi_E_y;hNumHoTDetPhi_{0}'.format(side_id[iside]),type='TProfile',title='Number of Hits per Track with {0} mm Cut vs #phi{1};#phi (deg);Hits per Track, TRT Hits> = {2}'.format(distance,regionTag,m_minTRThits),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=m_nphi_bins,xmin=0,xmax=360)  #TProfile
                    myGroup.defineHistogram('TronTDist_E_passed,TronTDist_E;hTronTDist_{0}'.format(side_id[iside]),type='TEfficiency',title='Trailing Edge Distribution on Track for Xenon Straws{0};Trailing Edge (ns);Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=26,xmin=-0.5,xmax=80.75)
                    myGroup.defineHistogram('DriftTimeonTrkDist_E_passed,DriftTimeonTrkDist_E;hDriftTimeonTrkDist_{0}'.format(side_id[iside]),type='TEfficiency',title='Drift Time Distribution on Track for Xenon Straws{0};Drift Time (ns);Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=32,xmin=0,xmax=100)
                    myGroup.defineHistogram('NumTrksDetPhi_E_passed,NumTrksDetPhi_E;hNumTrksDetPhi_{0}'.format(side_id[iside]),type='TEfficiency',title='Number of Reconstructed Tracks vs #phi (2D){0};#phi (deg);Number of Tracks'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=60,xmin=0,xmax=360)
                    myGroup.defineHistogram('Residual_E_passed,Residual_E;hResidual_{0}'.format(side_id[iside]),type='TEfficiency',title='Residuals for Xenon Straws{0};Hit-to-Track Distance (mm);Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=200,xmin=-2.5,xmax=2.5)
                    myGroup.defineHistogram('Residual_E_20GeV_passed,Residual_E_20GeV;hResidual_{0}_20GeV'.format(side_id[iside]),type='TEfficiency',title='Residuals for Xenon Straws{0} (After 20GeV pT cut);Hit-to-Track Distance (mm);Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=200,xmin=-2.5,xmax=2.5)
                    myGroup.defineHistogram('TimeResidual_E_passed,TimeResidual_E;hTimeResidual_{0}'.format(side_id[iside]),type='TEfficiency',title='Time Residuals for Xenon Straws{0};Time Residual (ns);Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=200,xmin=-20,xmax=20)

                    myGroup.defineHistogram('TronTDist_E_Ar_passed,TronTDist_E_Ar;hTronTDist_Ar_{0}'.format(side_id[iside]),type='TEfficiency',title='Trailing Edge Distribution on Track for Argon Straws{0};Trailing Edge (ns);Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=26,xmin=-0.5,xmax=80.75)
                    myGroup.defineHistogram('AvgTroTDetPhi_E_Ar_x,AvgTroTDetPhi_E_Ar_y;hAvgTroTDetPhi_Ar_{0}'.format(side_id[iside]),type='TProfile',title='Avg. Trailing Edge on Track vs #phi (2D) for Argon{0};#phi (deg);Trailing Edge (ns)'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=m_nphi_bins,xmin=0,xmax=360)  #TProfile
                    myGroup.defineHistogram('RtRelation_E_Ar_x,RtRelation_E_Ar_y;hrtRelation_Ar_{0}'.format(side_id[iside]),type='TH2F',title='R(t) Relation for Argon Straws{0};Measured Leading Edge (ns);Track-to-Wire Distance (mm)'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=30,xmin=-12.5,xmax=81.25,ybins=50,ymin=0,ymax=2.5)  #TH2F
                    myGroup.defineHistogram('DriftTimeonTrkDist_E_Ar_passed,DriftTimeonTrkDist_E_Ar;hDriftTimeonTrkDist_Ar_{0}'.format(side_id[iside]),type='TEfficiency',title='Drift Time Distribution on Track for Argon Straws{0};Drift Time (ns);Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=32,xmin=0,xmax=100)
                    myGroup.defineHistogram('Residual_E_Ar_passed,Residual_E_Ar;hResidual_Ar_{0}'.format(side_id[iside]),type='TEfficiency',title='Residuals for Argon Straws{0};Hit-to-Track Distance (mm);Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=200,xmin=-2.5,xmax=2.5)
                    myGroup.defineHistogram('Residual_E_Ar_20GeV_passed,Residual_E_Ar_20GeV;hResidual_Ar_{0}_20GeV'.format(side_id[iside]),type='TEfficiency',title='Residuals for Argon Straws{0} (After 20GeV pT cut);Hit-to-Track Distance (mm);Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=200,xmin=-2.5,xmax=2.5)
                    myGroup.defineHistogram('TimeResidual_E_Ar_passed,TimeResidual_E_Ar;hTimeResidual_Ar_{0}'.format(side_id[iside]),type='TEfficiency',title='Time Residuals for Argon Straws{0};Time Residual (ns);Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=200,xmin=-20,xmax=20)
                    myGroup.defineHistogram('WireToTrkPosition_E_Ar_passed,WireToTrkPosition_E_Ar;hWireToTrkPosition_Ar_{0}'.format(side_id[iside]),type='TEfficiency',title='Track-to-Wire Distance for Argon{0};Track-to-Wire Distance (mm);Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=100,xmin=-5,xmax=5)
                    myGroup.defineHistogram('HtoLRatioOnTrack_E_Ar_passed,HtoLRatioOnTrack_E_Ar;hHtoLRatioOnTrack_Ar_{0}'.format(side_id[iside]),type='TEfficiency',title='HL/LL Ratio per Reconstructed Track for Argon{0};HL/LL Ratio;Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=50,xmin=0,xmax=1)

                    myGroup.defineHistogram('HtoLRatioOnTrack_E_Xe_passed,HtoLRatioOnTrack_E_Xe;hHtoLRatioOnTrack_Xe_{0}'.format(side_id[iside]),type='TEfficiency',title='HL/LL Ratio per Reconstructed Track for Xenon{0};HL/LL Ratio;Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=50,xmin=0,xmax=1)
                    myGroup.defineHistogram('WireToTrkPosition_E_passed,WireToTrkPosition_E;hWireToTrkPosition_{0}'.format(side_id[iside]),type='TEfficiency',title='Track-to-Wire Distance for Xenon{0};Track-to-Wire Distance (mm);Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=100,xmin=-5,xmax=5)
                    myGroup.defineHistogram('NumSwLLWoT_E_passed,NumSwLLWoT_E;hNumSwLLWoT_{0}'.format(side_id[iside]),type='TEfficiency',title='Number of Straws with Hits on Track in Time Window{0};Number of LL Hits per Track;Norm. Entries'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=150,xmin=0,xmax=150)
                    myGroup.defineHistogram('AvgTroTDetPhi_E_x,AvgTroTDetPhi_E_y;hAvgTroTDetPhi_{0}'.format(side_id[iside]),type='TProfile',title='Avg. Trailing Edge on Track vs #phi (2D) for Xenon{0};#phi (deg);Trailing Edge (ns)'.format(regionTag),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=m_nphi_bins,xmin=0,xmax=360)  #TProfile

                    myGroup.defineHistogram('HLhitOnTrack_E_passed,HLhitOnTrack_E;hHLhitOnTrack_{0}'.format(side_id[iside]),type='TEfficiency',title='Number of HL Hits per Reconstructed Track{0};Number of HL Hits per Track;Norm. Entries'.format(regionTag).format(side_id[iside]),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=50,xmin=0,xmax=50)
                    myGroup.defineHistogram('HtoLRatioOnTrack_E_passed,HtoLRatioOnTrack_E;hHtoLRatioOnTrack_{0}'.format(side_id[iside]),type='TEfficiency',title='HL/LL Ratio per Reconstructed Track for All{0};HL/LL Ratio;Norm. Entries'.format(regionTag).format(side_id[iside]),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=50,xmin=0,xmax=1)
                    myGroup.defineHistogram('HitWonTMap_E_passed,HitWonTMap_E;hHitWonTMap_{0}'.format(side_id[iside]),type='TEfficiency',title='Leading Edge in Time Window per Reconstructed Track{0};Straw Number;Norm. Entries'.format(regionTag).format(side_id[iside]),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=s_Straw_max[1],xmin=0,xmax=s_Straw_max[1])
                    myGroup.defineHistogram('StrawEffDetPhi_E_passed,StrawEffDetPhi_E;hStrawEffDetPhi_{0}'.format(side_id[iside]),type='TEfficiency',title='Straw Efficiency on Track with {0} mm Cut vs #phi(2D){1};Stack;Avg. Straw Efficiency'.format(distance,regionTag).format(side_id[iside]),path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),xbins=32,xmin=0,xmax=32)  #TProfile

        #Initialize Aging plots
        if newLumiBlock and m_doShift:
            for iL in range(5):
                for iSide in range(2): 
                    if ibe == 0: 
                        if iL < 3:
                            myGroup = helper.addGroup(algTRTMonitoringRun3,'TRTAgingHistograms0{0}{1}'.format(iL,iSide))
                            myGroup.defineHistogram('Trackz_All_passed,Trackz_All;trackz_m{0}_{1}_All'.format(Mod[iL],side_id[iSide]),type='TEfficiency',title='Number All Hits side {0} Layer {1};z [mm];Number of Hits'.format(side_id[iSide],Mod[iL]),path='TRT/Aging/{0}'.format(barrel_or_endcap[ibe]),xbins=30,xmin=-750.,xmax=750.)
                            myGroup.defineHistogram('Trackz_HT_passed,Trackz_HT;trackz_m{0}_{1}_HT'.format(Mod[iL],side_id[iSide]),type='TEfficiency',title='Number HT Hits side {0} Layer {1};z [mm];Number of HT Hits'.format(side_id[iSide],Mod[iL]),path='TRT/Aging/{0}'.format(barrel_or_endcap[ibe]),xbins=30,xmin=-750.,xmax=750.)
                        if iL == 3:
                            myGroup = helper.addGroup(algTRTMonitoringRun3,'TRTAgingHistograms03{0}'.format(iSide))
                            myGroup.defineHistogram('Trackz_All_passed,Trackz_All;trackz_m1_{0}_All_{1}'.format(side_id[iSide],Mod[iL]),type='TEfficiency',title='Number All Hits side {0} Layer 1 {1};z [mm];Number of Hits'.format(side_id[iSide],Mod[iL]),path='TRT/Aging/{0}'.format(barrel_or_endcap[ibe]),xbins=30,xmin=0.,xmax=725.)
                            myGroup.defineHistogram('Trackz_HT_passed,Trackz_HT;trackz_m1_{0}_HT_{1}'.format(side_id[iSide],Mod[iL]),type='TEfficiency',title='Number HT Hits side {0} Layer 1 {1};z [mm];Number of HT Hits'.format(side_id[iSide],Mod[iL]),path='TRT/Aging/{0}'.format(barrel_or_endcap[ibe]),xbins=30,xmin=0.,xmax=725.)
                        if iL == 4:
                            myGroup = helper.addGroup(algTRTMonitoringRun3,'TRTAgingHistograms04{0}'.format(iSide))
                            myGroup.defineHistogram('Trackz_All_passed,Trackz_All;trackz_m1_{0}_All_{1}'.format(side_id[iSide],Mod[iL]),type='TEfficiency',title='Number All Hits side {0} Layer 1 {1};z [mm];Number of Hits'.format(side_id[iSide],Mod[iL]),path='TRT/Aging/{0}'.format(barrel_or_endcap[ibe]),xbins=30,xmin=-725.,xmax=0.)
                            myGroup.defineHistogram('Trackz_HT_passed,Trackz_HT;trackz_m1_{0}_HT_{1}'.format(side_id[iSide],Mod[iL]),type='TEfficiency',title='Number HT Hits side {0} Layer 1 {1};z [mm];Number of HT Hits'.format(side_id[iSide],Mod[iL]),path='TRT/Aging/{0}'.format(barrel_or_endcap[ibe]),xbins=30,xmin=-725.,xmax=0.)
                    elif ibe == 1:
                        # prevent double booking of histograms here
                        if iL < 4: 
                            myGroup = helper.addGroup(algTRTMonitoringRun3,'TRTAgingHistograms1{0}{1}'.format(iL,iSide))
                            myGroup.defineHistogram('Trackr_All_passed,Trackr_All;trackr_E{0}_{1}_All'.format(side_id[iSide],gas[iL]),type='TEfficiency',title='Number All Hits E{0} {1};r [mm];Number of Hits'.format(side_id[iSide],gas[iL]),path='TRT/Aging/{0}'.format(barrel_or_endcap[ibe]),xbins=30,xmin=644.,xmax=1004.)
                            myGroup.defineHistogram('Trackr_HT_passed,Trackr_HT;trackr_E{0}_{1}_HT'.format(side_id[iSide],gas[iL]),type='TEfficiency',title='Number HT Hits E{0} {1};r [mm];Number of HT Hits'.format(side_id[iSide],gas[iL]),path='TRT/Aging/{0}'.format(barrel_or_endcap[ibe]),xbins=30,xmin=644.,xmax=1004.)

    # === STEP 6 === #
    # Finalize. The return value should be a tuple of the ComponentAccumulator
    # and the sequence containing the created algorithms. If we haven't called
    # any configuration other than the AthMonitorCfgHelper here, then we can
    # just return directly (and not create 'result' above)
    # return helper.result()

    # # Otherwise, merge with result object and return
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
    file = 'data16_13TeV.00358031.physics_Main.recon.AOD.Athena.21.0.78.f961/myESD.pool.root'
    ConfigFlags.Input.Files = [nightly+file]
    ConfigFlags.Input.isMC = False
    ConfigFlags.Output.HISTFileName = 'TRTMonitoringRun3_ToolOutput.root'
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    from AthenaCommon.AppMgr import ServiceMgr
    cfg = MainServicesSerialCfg()
    cfg.merge(PoolReadCfg(ConfigFlags))

    # Configuration of the ATLAS Geo Model
    from AtlasGeoModel.AtlasGeoModelConfig import AtlasGeometryCfg
    geoCfg = AtlasGeometryCfg(ConfigFlags)
    cfg.merge(geoCfg)

    TRTMonitoringRun3Acc = TRTMonitoringRun3_AlgConfig(ConfigFlags)
    ServiceMgr.Dump = False

    cfg.merge(TRTMonitoringRun3Acc)
    cfg.run()
