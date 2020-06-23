#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
def DQTDetSynchMonAlgConfig(flags):
    from AthenaMonitoring import AthMonitorCfgHelper
    from AthenaConfiguration.ComponentFactory import CompFactory
    helper = AthMonitorCfgHelper(flags,'DQTDetSynchMonAlgCfg')
    _DQTDetSynchMonAlgConfigCore(helper, CompFactory.DQTDetSynchMonAlg,
                                flags.Common.isOnline, False)
    acc = helper.result()
    # RPC currently crashes, switch off
    acc.getEventAlgo('DQTDetSynchMonAlg').doRPC = False
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    acc.merge(MagneticFieldSvcCfg(flags))
    from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
    acc.merge(TileCablingSvcCfg(flags))
    return acc

def DQTDetSynchMonAlgConfigOld(flags):
    from AthenaMonitoring import AthMonitorCfgHelperOld
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    from DataQualityTools.DataQualityToolsConf import DQTDetSynchMonAlg
    helper = AthMonitorCfgHelperOld(flags,'DQTDetSynchMonAlgCfg')
    _DQTDetSynchMonAlgConfigCore(helper, DQTDetSynchMonAlg, athenaCommonFlags.isOnline, True)
    return helper.result()

def _DQTDetSynchMonAlgConfigCore(helper, algConfObj, isOnline=False, run2Compat=False):
    monAlg = helper.addAlgorithm(algConfObj,'DQTDetSynchMonAlg')
    monAlg.run2Compat = run2Compat

    # arguments are: algorithm, name of group used to access it from the alg,
    # the 'top level path' to put outputs in, and the default duration of
    # associated histograms
    bcidg = helper.addGroup(monAlg, 'bcid', 'GLOBAL/DQTSynch/BCID')
    l1idg = helper.addGroup(monAlg, 'l1id', 'GLOBAL/DQTSynch/L1ID')
    bfieldg = helper.addGroup(monAlg, 'bfield', 'GLOBAL/DQTSynch/Bfield')

    bcidg.defineHistogram("diffx,diffy;m_BCID",
                          title="BCID subdetector summary",
                          type='TH2I',
                          xbins=7, xmin=-0.5, xmax=6.5,
                          ybins=7, ymin=-0.5, ymax=6.5,
                          xlabels = ["CTP", "SCT", "TRT", "LAr",
                                     "Tile", "RPC", "Pixel"],
                          ylabels = ["CTP", "SCT", "TRT", "LAr",
                                     "Tile", "RPC", "Pixel"]
    )
    bcidg.defineHistogram("bcidrates_idx,bcidrates;m_BCID_rate",
                          title="BCID subdetector rate summary",
                          type='TH2I',
                          xbins=6, xmin=0.5, xmax=6.5,
                          ybins=20, ymin=0.0, ymax=1.0,
                          xlabels = ["SCT", "TRT", "LAr",
                                     "Tile", "RPC", "Pixel"]
    )

    l1idg.defineHistogram("diffx,diffy;m_L1ID",
                          title="L1ID subdetector summary",
                          type='TH2I',
                          xbins=7, xmin=-0.5, xmax=6.5,
                          ybins=7, ymin=-0.5, ymax=6.5,
                          xlabels = ["CTP", "SCT", "TRT", "LAr",
                                     "Tile", "RPC", "Pixel"],
                          ylabels = ["CTP", "SCT", "TRT", "LAr",
                                     "Tile", "RPC", "Pixel"]
    )
    
    detlist = ['CTP', 'SCT', 'TRT', 'LAR', 'Tile', 'RPC', 'Pixel']
    for det in detlist:
        bcidg.defineHistogram("%sbcid;m_%s_BCID" % (det.lower(), det),
                              title=(("BCID of %s" % det) +
                                     (" detector" if det != 'CTP' else '')),
                              type='TH1I',
                              xbins=4096, xmin=-0.5, xmax=4095.5)
        l1idg.defineHistogram("%sl1id;m_%s_L1ID" % (det.lower(), det),
                              title=("L1ID of %s detector" %  det),
                              type='TH1I',
                              xbins=(10000 if det == 'CTP' else 1000),
                              xmin=-0.5, xmax=9999.5)
        
    # cross-detector correlations
    for i1, det1 in enumerate(detlist):
        for i2, det2 in enumerate(detlist[i1+1:]):
            didx1, didx2 = i1, i2+i1+1
            if run2Compat and det2 == 'Pixel':
                didx1, didx2 = didx2, didx1
                det1, det2 = det2, det1
            # BCID
            bcidg.defineHistogram('diff_%1d_%1d;m_diff_%s_%s_BCID' % (didx1, didx2, det1, det2),
                                  title='BCID difference between %s and %s detectors' % (det1, det2),
                                  type='TH1I',
                                  opt=('kRebinAxes' if not run2Compat and not isOnline else ''),
                                  xbins=51, xmin=-25.5, xmax=25.5)

            # L1ID
            l1idg.defineHistogram('diff_%1d_%1d;m_diff_%s_%s_L1ID' % (didx1, didx2, det1, det2),
                                  title='L1ID difference between %s and %s detectors' % (det1, det2),
                                  type='TH1I',
                                  opt=('kRebinAxes' if not run2Compat and not isOnline else ''),
                                  xbins=51, xmin=-25.5, xmax=25.5)
            
            if run2Compat and not isOnline:
                # BCID
                bcidg.defineHistogram('diff_%1d_%1d;m_diff_%s_%s_BCID_Rebin' % (didx1, didx2, det1, det2),
                                      title='BCID difference between %s and %s detectors. Full Range.' % (det1, det2),
                                      type='TH1I',
                                      opt='kRebinAxes',
                                      xbins=51, xmin=-25.5, xmax=25.5)
                
                # L1ID
                l1idg.defineHistogram('diff_%1d_%1d;m_diff_%s_%s_L1ID_Rebin' % (didx1, didx2, det1, det2),
                                      title='L1ID difference between %s and %s detectors. Full Range.' % (det1, det2),
                                      type='TH1I',
                                      opt='kRebinAxes',
                                      xbins=51, xmin=-25.5, xmax=25.5)
                
            if ((det1 == 'CTP'
                 or (det1 == 'Pixel' and det2 == 'CTP'))
                and not isOnline):
                if run2Compat and det1 == 'Pixel':
                    # reproducing old code ...
                    didx1, didx2, det1, det2 = didx2, didx1, det2, det1
                bcidg.defineHistogram('LB,diff_%1d_%1d;m_diff_%s_%s_BCID_lumi' % (didx1, didx2, det1, det2),
                                      title='BCID difference between %s and %s detectors as a function of the LumiBlock' % (det1, det2), 
                                      type='TProfile',
                                      opt='kAddBinsDynamically',
                                      xbins=30, xmin=0., xmax=30)
                
                l1idg.defineHistogram('LB,diff_%1d_%1d;m_diff_%s_%s_L1ID_lumi' % (didx1, didx2, det1, det2),
                                      title='L1ID difference between %s and %s detectors as a function of the LumiBlock' % (det1, det2), 
                                      type='TProfile',
                                      opt='kAddBinsDynamically',
                                      xbins=30, xmin=0., xmax=30)

    bfieldg.defineHistogram("solenoid_bz;m_Bfield_solenoid",
                            title="Bz of Solenoid",
                            type='TH1I', xbins=50, xmin=-1, xmax=4)
    bfieldg.defineHistogram("toroid_bx;m_Bfield_toroid",
                            title="Bx of Toroid",
                            type='TH1I', xbins=50, xmin=-1, xmax=4)
    bfieldg.defineHistogram("LB,solenoid_bz;m_Bfield_solenoid_vsLB",
                            title="Bz of Solenoid vs LB",
                            opt='kAddBinsDynamically',
                            type='TProfile', xbins=1500, xmin=-0.5, xmax=1499.5)
    bfieldg.defineHistogram("LB,toroid_bx;m_Bfield_toroid_vsLB",
                            title="Bx of Toroid vs LB",
                            opt='kAddBinsDynamically',
                            type='TProfile', xbins=1500, xmin=-0.5, xmax=1499.5)



if __name__ == '__main__':
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    nightly = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/'
    file = 'data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1'
    ConfigFlags.Input.Files = defaultTestFiles.RAW
    #ConfigFlags.Input.isMC = False
    ConfigFlags.Output.HISTFileName = 'ExampleMonitorOutput.root'
    
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    #from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(ByteStreamReadCfg(ConfigFlags))
    from TrigInDetConfig.InDetConfig import TrigInDetConfig
    cfg.merge(TrigInDetConfig(ConfigFlags))

    cfg.merge(DQTDetSynchMonAlgConfig(ConfigFlags))

    # If you want to turn on more detailed messages ...
    #exampleMonitorAcc.getEventAlgo('ExampleMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=False) # set True for exhaustive info

    cfg.run() #use cfg.run(20) to only run on first 20 events

