#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
    from AthenaConfiguration.ComponentFactory import CompFactory
    myMonAlg = helper.addAlgorithm(CompFactory.SCTLorentzMonAlg, 'SCTLorentzMonAlg')

    # # If for some really obscure reason you need to instantiate an algorithm
    # # yourself, the AddAlgorithm method will still configure the base 
    # # properties and add the algorithm to the monitoring sequence.
    # helper.AddAlgorithm(myExistingAlg)


    ### STEP 3 ###
    # Edit properties of a algorithm
    myMonAlg.TriggerChain = ''
    # myMonAlg.RandomHist = True

    # Set InDetTrackSummaryTool to TrackSummaryTool of SCTLorentzMonAlg
    from .TrackSummaryToolWorkaround import TrackSummaryToolWorkaround
    myMonAlg.TrackSummaryTool = result.popToolsAndMerge(TrackSummaryToolWorkaround(inputFlags))

    ### STEP 4 ###
    # Add some tools. N.B. Do not use your own trigger decion tool. Use the
    # standard one that is included with AthMonitorAlgorithm.


    # set up geometry / conditions
    from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
    result.merge(BeamPipeGeometryCfg(inputFlags))
    from AtlasGeoModel.InDetGMConfig import InDetGeometryCfg
    result.merge(InDetGeometryCfg(inputFlags))

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
    nSides = 2 # 0: Side 0, 1: Side 1
    nSurfaces = 2 # 0: 100, 1: 111
    surfaceNames = ["_100",   "_111"]
    surfaceNames2 = ["_100_",   "_111_"]
    surfaceTitles = ["100 - ", "111 - "]
    sideNames = ["_0", "_1"]
    for l in range(N_BARRELS):
        for iSurface in range(nSurfaces):
            for side in range(nSides):
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
    ConfigFlags.Detector.GeometryPixel = True
    ConfigFlags.Detector.GeometrySCT = True
    ConfigFlags.Detector.GeometryTRT = True
    ConfigFlags.Detector.GeometryMuon = False
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    sctLorentzMonAcc = SCTLorentzMonAlgConfig(ConfigFlags)
    cfg.merge(sctLorentzMonAcc)

    cfg.run()
