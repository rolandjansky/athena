#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

"""
@brief configuration for the min bias monitoring
"""


def TrigSPTRK(configFlags):

    from AthenaMonitoring import AthMonitorCfgHelper

    monConfig = AthMonitorCfgHelper(configFlags, "HLTMBSPTRKMonAlg")

    from AthenaConfiguration.ComponentFactory import CompFactory

    alg = monConfig.addAlgorithm(CompFactory.HLTMinBiasTrkMonAlg, "HLTMBSPTRKMonAlg")
    trkSel = CompFactory.InDet.InDetTrackSelectionTool(
        "InDetTrackSelectionTool_TightPrimary", CutLevel="TightPrimary"
    )

    alg.TrackSelectionTool = trkSel

    alg.triggerList = ["HLT_mb_sptrk_L1RD0_FILLED", "HLT_mb_sp_L1RD0_FILLED"]

    mbEffAllGroup = monConfig.addGroup(alg, "EffAll", topPath="HLT/MinBiasMon/")
    length = len(alg.triggerList)
    mbEffAllGroup.defineHistogram( "PurityPassed,whichTrigger", type="TH2D", title=";Purity;trigger passed", xbins=2, xmin=0, xmax=2, xlabels=["0 track", ">0 tracks"], 
                                    ybins=length, ymin=0, ymax=length, ylabels=list(alg.triggerList) )
    mbEffAllGroup.defineHistogram( "whichTrigger", title="count of triggers;HLT", xbins=length, xmin=0, xmax=length, xlabels=list(alg.triggerList) )

    for chain in alg.triggerList:

        mbEffGroup = monConfig.addGroup(
            alg, chain + "_Tracking", topPath="HLT/MinBiasMon/Tracking/" + chain + "/"
        )

        mbEffGroup.defineHistogram( "decision,nTrkOffline;efficiencyAnyMult", type="TEfficiency", title="Efficiency;Offline Good nTrk", xbins=100, xmin=0, xmax=400 )
        mbEffGroup.defineHistogram( "decision,nTrkOffline;efficiencyLowMult", type="TEfficiency", title="Efficiency;Offline Good nTrk", xbins=50, xmin=0, xmax=50 )
        mbEffGroup.defineHistogram( "nTrkOffline;nTrkOfflineLowMult", title="Number of tracks reconstructed offline;track counts", xbins=50, xmin=-1, xmax=50 )

        mbEffGroup.defineHistogram( "nTrkOffline", title="Number of tracks reconstructed offline;track counts", xbins=100, xmin=-1, xmax=400 )
        mbEffGroup.defineHistogram( "nTrkOnline;nTrkOnlineLowMult", title="Number of tracks reconstructed online;track counts", xbins=50, xmin=-1, xmax=50 )
        mbEffGroup.defineHistogram( "nTrkOnline", title="Number of tracks reconstructed online;track counts", xbins=100, xmin=-1, xmax=400 )
        mbEffGroup.defineHistogram( "nTrkRatio", title="Number of tracks reconstructed online/offline;track counts online/offline", xbins=100, xmin=-1, xmax=4 ) 
        mbEffGroup.defineHistogram( "decision,nTrkOnline", type="TEfficiency", title="Efficiency (step curve);Online nTrk", xbins=1000, xmin=0, xmax=1000 ) # expert plots
        mbEffGroup.defineHistogram( "trkSelOfflineRatio", path="Expert", title="Number of tracks reconstructed offline(selected)/offline; N sel/all", xbins=100, xmin=0.1, xmax=1.9 ) 
        mbEffGroup.defineHistogram( "nTrkOnline,nTrkOffline", type="TH2F", path="Expert", title=";N online tracks;N offline tracks", xbins=20, xmin=0, xmax=400, ybins=20, ymin=0, ymax=400 )

        mbSpGroup = monConfig.addGroup(
            alg,
            chain + "_SpacePoints",
            topPath="HLT/MinBiasMon/SpacePoints/" + chain + "/",
        )
        mbSpGroup.defineHistogram( "PixelCL;PixelCLNarrowRange", title="Number of SP in whole Pixels detector for all events", xbins=100, xmin=0, xmax=100 )
        mbSpGroup.defineHistogram( "PixelCL;PixelCLWideRange", title="Number of SP in whole Pixels detector for all events", xbins=100, xmin=0, xmax=30000 )
        mbSpGroup.defineHistogram( "PixBarr_SP", title="Number of SP for all events in Barrel", xbins=100, xmin=0, xmax=30000 )
        mbSpGroup.defineHistogram( "PixECA_SP", title="Number of SP for all events in ECA", xbins=100, xmin=0, xmax=30000 )
        mbSpGroup.defineHistogram( "PixECC_SP", title="Number of SP for all events in ECC", xbins=100, xmin=0, xmax=30000 )
        mbSpGroup.defineHistogram( "SctTot", title="Number of SP in whole SCT detector for all events", xbins=100, xmin=0, xmax=120000 )
        mbSpGroup.defineHistogram( "SctBarr_SP", title="Number of SCT_SP for all events in Barrel", xbins=100, xmin=0, xmax=50000 )
        mbSpGroup.defineHistogram( "SctECA_SP", title="Number of SCT_SP for all events in ECA", xbins=100, xmin=0, xmax=50000 )
        mbSpGroup.defineHistogram( "SctECC_SP", title="Number of SCT_SP for all events in ECC", xbins=100, xmin=0, xmax=50000 )
        # expert plots
        mbSpGroup.defineHistogram( "SctECA_SP,SctECC_SP", type="TH2F", title="SctECA_SP;SctECC_SP", xbins=100, xmin=0, xmax=100, ybins=100, ymin=0, ymax=100 )
        mbSpGroup.defineHistogram( "PixECA_SP,PixECC_SP", type="TH2F", title="PixECA_SP;PixECC_SP", xbins=100, xmin=0, xmax=100, ybins=100, ymin=0, ymax=100 )
        mbSpGroup.defineHistogram( "SctBarr_SP,PixBarr_SP", type="TH2F", title="SctBarr_SP;PixBarr_SP", xbins=100, xmin=0, xmax=100, ybins=100, ymin=0, ymax=100 )

    return monConfig.result()


if __name__ == "__main__":
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable

    Configurable.configurableRun3Behavior = 1


    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    ConfigFlags.DQ.Environment = "AOD"
    ConfigFlags.Concurrency.NumConcurrentEvents = 1
    # ConfigFlags.Input.Files = ['/afs/cern.ch/user/s/somadutt/public/testUPC2.AOD.pool.root'] #Local HI-UPC file

    # data AOD file
#    ConfigFlags.Input.Files = ["myAOD.pool.root"]
    # ConfigFlags.Input.isMC = True  #un-Comment this line for MC AOD files, comment for data-AOD files
    ConfigFlags.Output.HISTFileName = "TestMonitorOutput.root"

    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg

    cfg = MainServicesCfg(ConfigFlags)

    cfg.merge(PoolReadCfg(ConfigFlags))
    cfg.merge(TrigSPTRK(ConfigFlags))

    # If you want to turn on more detailed messages ...
    from AthenaCommon.Constants import DEBUG
    cfg.getEventAlgo("HLTMBSPTRKMonAlg").OutputLevel = DEBUG 
    cfg.printConfig(withDetails=True)  # set True for exhaustive info
    with open("cfg.pkl", "wb") as f:
        cfg.store(f)

    cfg.run()  # use cfg.run(20) to only run on first 20 events
    # to run:
    # python -m TrigMinBiasMonitoring.TrigMinBiasMonitoringMT
