#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

"""
@brief configuration for the min bias monitoring
"""


def TrigSPTRK(configFlags, highGranularity=False):

    from AthenaMonitoring import AthMonitorCfgHelper

    monConfig = AthMonitorCfgHelper(configFlags, "HLTMBSPTRKMonAlg")

    from AthenaConfiguration.ComponentFactory import CompFactory

    alg = monConfig.addAlgorithm(CompFactory.HLTMinBiasTrkMonAlg, "HLTMBSPTRKMonAlg")
    trkSel = CompFactory.InDet.InDetTrackSelectionTool(
        "InDetTrackSelectionTool_LoosePrimary", CutLevel="LoosePrimary"
    )

    alg.TrackSelectionTool = trkSel

    alg.triggerList = ["HLT_mb_sptrk_L1RD0_FILLED", "HLT_mb_sp_L1RD0_FILLED"]

    for chain in alg.triggerList:

        mbEffGroup = monConfig.addGroup(
            alg, chain + "_Tracking", topPath="HLT/MinBiasMon/Tracking/" + chain + "/"
        )

        mbEffGroup.defineHistogram( "decision,nTrkOffline;efficiencyHighMult", type="TEfficiency", title="Efficiency;Offline Good nTrk", xbins=200, xmin=0, xmax=400 )
        mbEffGroup.defineHistogram( "decision,nTrkOffline;efficiencyLowMult", type="TEfficiency", title="Efficiency;Offline Good nTrk", xbins=50, xmin=0, xmax=50 )
        mbEffGroup.defineHistogram( "nTrkOffline;nTrkOfflineLowMult", title="Number of tracks reconstructed offline;track counts", xbins=50, xmin=-1, xmax=50 )

        mbEffGroup.defineHistogram( "nTrkOffline", title="Number of tracks reconstructed offline;track counts", xbins=200, xmin=-1, xmax=400 )
        mbEffGroup.defineHistogram( "nTrkOnline;nTrkOnlineLowMult", title="Number of tracks reconstructed online;track counts", xbins=50, xmin=-1, xmax=50 )
        nbins = 400 if highGranularity else 100
        mbEffGroup.defineHistogram( "nTrkOnline,nTrkOffline", type="TH2F", title=";N online tracks;N offline tracks", xbins=nbins, xmin=0, xmax=400, ybins=nbins, ymin=0, ymax=400 )
        mbEffGroup.defineHistogram( "nTrkOnline", title="Number of tracks reconstructed online;track counts", xbins=200, xmin=-1, xmax=400 )
        mbEffGroup.defineHistogram( "nTrkRatio", title="Number of tracks reconstructed online/offline;track counts online/offline", xbins=200, xmin=-1, xmax=4 ) 
        mbEffGroup.defineHistogram( "decision,nTrkOnline", type="TEfficiency", title="Efficiency (step curve);Online nTrk", xbins=400, xmin=0, xmax=400 )
        mbEffGroup.defineHistogram( "trkSelOfflineRatio", title="Number of tracks reconstructed offline(selected)/offline; N sel/all", xbins=200, xmin=0.1, xmax=1.9 ) 

        if "HLT_mb_sptrk" in chain:
            nbins = 400 if highGranularity else 50
            mbEffGroup.defineHistogram( "SctTot,nTrkOffline", type="TH2F", title=";Number of SP in whole SCT detector for all events;N offline tracks", xbins=nbins, xmin=0, xmax=4000, ybins=nbins, ymin=0, ymax=400 )
            mbEffGroup.defineHistogram( "SctTot,nTrkOnline", type="TH2F",  title=";Number of SP in whole SCT detector for all events;N online tracks", xbins=nbins, xmin=0, xmax=4000, ybins=nbins, ymin=0, ymax=400 )
            mbEffGroup.defineHistogram( "PixelCL,nTrkOffline", type="TH2F",  title=";Number of SP in whole Pixels detector for all events;N offline tracks", xbins=100, xmin=0, xmax=4000, ybins=100, ymin=0, ymax=400 )
            mbEffGroup.defineHistogram( "PixelCL,nTrkOnline", type="TH2F",  title=";Number of SP in whole Pixels detector for all events;N online tracks", xbins=100, xmin=0, xmax=4000, ybins=100, ymin=0, ymax=400 )

            mbEffGroup.defineHistogram( "L1sumEt", title=";L1 Total ET [GeV] ;Events", xbins=52, xmin=-2, xmax=50 ) 
            mbEffGroup.defineHistogram( "nTrkOffline,L1sumEt", type="TH2F", title=";Offline Ntrk;L1 Total ET [GeV]", xbins=100, xmin=-1, xmax=200, ybins=52, ymin=-2, ymax=50   ) 
            mbEffGroup.defineHistogram( "nTrkOnline,L1sumEt", type="TH2F", title=";Online Ntrk;L1 Total ET [GeV]",   xbins=100, xmin=-1, xmax=200, ybins=52, ymin=-2, ymax=50   ) 
            mbEffGroup.defineHistogram( "SctTot,L1sumEt", type="TH2F", title=";Number of SP in whole SCT detector for all events;L1 Total ET [GeV]", xbins=100, xmin=0, xmax=4000, ybins=52, ymin=-2, ymax=50   ) 
            mbEffGroup.defineHistogram( "PixelCL,L1sumEt", type="TH2F", title=";Number of SP in whole Pixels detector for all events;L1 Total ET [GeV]", xbins=100, xmin=0, xmax=4000, ybins=52, ymin=-2, ymax=50   ) 

            mbEffGroup.defineHistogram( "trkPt", cutmask="trkMask", title="Offline selected tracks pt;p_{T} [GeV]", xbins=100, xmin=0, xmax=10)
            mbEffGroup.defineHistogram( "trkEta", cutmask="trkMask", title="Offline selected tracks eta;#eta", xbins=50, xmin=-2.5, xmax=2.5)
            mbEffGroup.defineHistogram( "trkD0", cutmask="trkMask", title="Offline selected tracks D0;d_{0} [mm]", xbins=40, xmin=-20, xmax=20)
            mbEffGroup.defineHistogram( "trkZ0", cutmask="trkMask", title="Offline selected tracks Z0;z_{0}[mm]", xbins=40, xmin=-20, xmax=20)
            mbEffGroup.defineHistogram( "trkZ0;trackZdWideRange", cutmask="trkMask", title="Offline selected tracks Z0;z_{0}[mm]", xbins=40, xmin=-200, xmax=200)

        mbSpGroup = monConfig.addGroup(
            alg,
            chain + "_SpacePoints",
            topPath="HLT/MinBiasMon/SpacePoints/" + chain + "/",
        )
        mbSpGroup.defineHistogram( "PixelCL;PixelCLNarrowRange", title="Number of SP in whole Pixels detector for all events", xbins=200, xmin=0, xmax=100 )
        mbSpGroup.defineHistogram( "PixelCL;PixelCLWideRange", title="Number of SP in whole Pixels detector for all events", xbins=250, xmin=0, xmax=5000 )
        mbSpGroup.defineHistogram( "PixBarr_SP", title="Number of SP for all events in Barrel", xbins=250, xmin=0, xmax=5000 )
        mbSpGroup.defineHistogram( "PixECA_SP", title="Number of SP for all events in ECA", xbins=250, xmin=0, xmax=500 )
        mbSpGroup.defineHistogram( "PixECC_SP", title="Number of SP for all events in ECC", xbins=250, xmin=0, xmax=500 )
        mbSpGroup.defineHistogram( "SctTot", title="Number of SP in whole SCT detector for all events", xbins=250, xmin=0, xmax=5000 )
        mbSpGroup.defineHistogram( "SctBarr_SP", title="Number of SCT_SP for all events in Barrel", xbins=250, xmin=0, xmax=5000 )
        mbSpGroup.defineHistogram( "SctECA_SP", title="Number of SCT_SP for all events in ECA", xbins=250, xmin=0, xmax=5000 )
        mbSpGroup.defineHistogram( "SctECC_SP", title="Number of SCT_SP for all events in ECC", xbins=250, xmin=0, xmax=5000 )
        # expert plots
        if "HLT_mb_sptrk" in chain:
            nbins = 400 if highGranularity else 50
            mbSpGroup.defineHistogram( "SctECA_SP,SctECC_SP", type="TH2F", title=";SctECA_SP;SctECC_SP", xbins=nbins, xmin=0, xmax=4000, ybins=nbins, ymin=0, ymax=4000 )
            mbSpGroup.defineHistogram( "PixECA_SP,PixECC_SP", type="TH2F", title=";PixECA_SP;PixECC_SP", xbins=nbins, xmin=0, xmax=1000, ybins=nbins, ymin=0, ymax=1000 )
            mbSpGroup.defineHistogram( "SctBarr_SP,PixBarr_SP", type="TH2F", title=";SctBarr_SP;PixBarr_SP", xbins=nbins, xmin=0, xmax=4000, ybins=nbins, ymin=0, ymax=4000 )
            mbSpGroup.defineHistogram( "SctECA_SP,PixECA_SP", type="TH2F", title=";SctECA_SP;PixECA_SP", xbins=nbins, xmin=0, xmax=4000, ybins=nbins, ymin=0, ymax=1000 )
            mbSpGroup.defineHistogram( "SctECC_SP,PixECC_SP", type="TH2F", title=";SctECC_SP;PixECC_SP", xbins=nbins, xmin=0, xmax=6000, ybins=nbins, ymin=0, ymax=4000 )
            mbSpGroup.defineHistogram( "SctTot,PixelCL", type="TH2F", title=";Number of SP in whole SCT detector for all events;Number of SP in whole Pixels detector for all events", xbins=nbins, xmin=0, xmax=4000, ybins=nbins, ymin=0, ymax=4000 )
        

    return monConfig.result()


if __name__ == "__main__":
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable

    Configurable.configurableRun3Behavior = 1


    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.DQ.Environment = "AOD"
    ConfigFlags.Concurrency.NumConcurrentEvents = 5
    # ConfigFlags.Input.Files = ['/scratch/somadutt/valid1.361238.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_low.recon.AOD.e4981_s3454_s3089_d1617_r12430_tid24359040_00/AOD.24359040._000041.pool.root.1'] #Local HI-UPC file
    import glob
    #ConfigFlags.Input.Files = glob.glob("/ATLAS/tbold/DATA/data18_13TeV.00341615.physics_EnhancedBias.merge.AOD.r12635_p4534_tid25577237_00/*")[:1]
    ConfigFlags.Input.Files = glob.glob("/ATLAS/tbold/athena/add-view-to-zfinder-output-config/*/*AOD._lb*")
    
    #ConfigFlags.Input.Files = glob.glob('/scratch/somadutt/valid1.361238.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_low.recon.AOD.e4981_s3454_s3089_d1617_r12430_tid24359040_00/*')
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
    cfg.merge(TrigSPTRK(ConfigFlags, highGranularity=True)) # for local testing have very granular histograms

    # If you want to turn on more detailed messages ...
    from AthenaCommon.Constants import DEBUG
    cfg.getEventAlgo("HLTMBSPTRKMonAlg").OutputLevel = DEBUG
    cfg.printConfig(withDetails=True)  # set True for exhaustive info
    with open("cfg.pkl", "wb") as f:
        cfg.store(f)

    cfg.run()  # use cfg.run(20) to only run on first 20 events
    # to run:
    # python -m TrigMinBiasMonitoring.TrigMinBiasMonitoringMT
