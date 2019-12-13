#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file TRTMonitoringRun3_Tool.py
@author N. Belyaev
@date 20.09.2019
@brief MT-compatible TRT Monitoring Tool for Run III based on the
AthenaMonitoring package
'''


def TRTMonitoringRun3_ToolConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    # === STEP 1 === #
    # Define one top-level monitoring algorithm. The new configuration
    # framework uses a component accumulator.
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    from TrkEventAthenaPool.TrackCollectionReadConfig import TrackCollectionReadCfg
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
    from TRTMonitoringRun3.TRTMonitoringRun3Conf import TRTMonitoringRun3_Tool
    algTRTMonitoringRun3 = helper.addAlgorithm(TRTMonitoringRun3_Tool,
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
    from AtlasGeoModel.AtlasGeoModelConfig import AtlasGeometryCfg
    result.merge(AtlasGeometryCfg(inputFlags))
    

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

    # # Then, add a tool that doesn't have its own configuration function. In
    # # this example, no accumulator is returned, so no merge is necessary.
    # from MyDomainPackage.MyDomainPackageConf import MyDomainTool
    # AlgTRTMonitoringRun3.MyDomainTool = MyDomainTool()

    # Add a generic monitoring tool (a 'group' in old language). The returned
    # object here is the standard GenericMonitoringTool.

    # Add a GMT for the other example monitor algorithm
    # anotherGroup = helper.addGroup(anotherAlgTRTMonitoringRun3,
    #                                'TRTMonitoringRun3Monitor')

    # === STEP 5 === #
    # Configure histograms

    barrel_or_endcap = ('Barrel', 'EndCap')
    side_id = ('A', 'C')
    newRun = True
    m_doShift = True

    for ibe in range(2):
        myGroup = helper.addGroup(algTRTMonitoringRun3,
                                  'testTracksHistograms{0}'.format(ibe))
        regionTag = ' (' + barrel_or_endcap[ibe] + ')'

        if newRun and m_doShift:
            if ibe == 0:
                myGroup.defineHistogram('DriftTimeonTrkDist_B;hDriftTimeonTrkDist',
                                        type='TH1F',
                                        title='Drift Time Distribution on Track for Xenon Straws{0};'
                                        'Drift Time (ns);Norm. Entries'.format(regionTag),
                                        path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),
                                        xbins=32, xmin=0, xmax=100)
                myGroup.defineHistogram('WireToTrkPosition_B;hWireToTrkPosition',
                                        type='TH1F',
                                        title='Track-to-Wire Distance for Xenon{0};'
                                        'Track-to-Wire Distance (mm);Norm. Entries'.format(regionTag),
                                        path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),
                                        xbins=100, xmin=-5, xmax=5)
                myGroup.defineHistogram('TronTDist_B;hTronTDist',
                                        type='TH1F',
                                        title='Trailing Edge Distribution on Track for Xenon Straws{0};'
                                        'Trailing Edge (ns);Norm. Entries'.format(regionTag),
                                        path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),
                                        xbins=26, xmin=-0.5, xmax=80.75)
            elif ibe == 1:
                myGroup.defineHistogram('Pull_Biased_EndCap_passed,Pull_Biased_EndCap;hPull_Biased_EndCap',
                                        type='TProfile',
                                        title='Biased Track Pulls for EndCap Hits;Pulls;Entries',
                                        path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),
                                        xbins=200, xmin=-2.5, xmax=2.5)
                for iside in range(2):
                    myGroup = helper.addGroup(algTRTMonitoringRun3,
                                              'testTracksHistogramsIside{0}{1}'
                                              ''.format(ibe, iside))
                    myGroup.defineHistogram('WireToTrkPosition_E;hWireToTrkPosition_{0}'.format(side_id[iside]),
                                            type='TH1F',
                                            title='Track-to-Wire Distance for Xenon{0};'
                                            'Track-to-Wire Distance (mm);Norm. Entries'.format(regionTag),
                                            path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),
                                            xbins=100, xmin=-5, xmax=5)
                    myGroup.defineHistogram('TronTDist_E;hTronTDist_{0}'.format(side_id[iside]),
                                            type='TH1F',
                                            title='Trailing Edge Distribution on Track for Xenon Straws{0};'
                                            'Trailing Edge (ns);Norm. Entries'.format(regionTag),
                                            path='TRT/Shift/{0}'.format(barrel_or_endcap[ibe]),
                                            xbins=26, xmin=-0.5, xmax=80.75)

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
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    nightly = '/afs/cern.ch/work/n/nbelyaev/public/'
    file = 'data16_13TeV.00358031.physics_Main.recon.AOD.Athena.21.0.78.f961/TRT.ESD.11038520._000002.pool.root.1'
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

    TRTMonitoringRun3Acc = TRTMonitoringRun3_ToolConfig(ConfigFlags)
    ServiceMgr.Dump = False

    cfg.merge(TRTMonitoringRun3Acc)
    cfg.run()
