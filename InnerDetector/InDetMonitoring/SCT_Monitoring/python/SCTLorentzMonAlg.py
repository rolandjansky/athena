#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file SCTLorentzMonAlg.py
@author Susumu Oda
@date 2019-04-02
@brief Based on AthenaMonitoring/ExampleMonitorAlgorithm.py
'''

def SCTLorentzMonAlgConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

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

    ### STEP 4 ###
    # Add some tools. N.B. Do not use your own trigger decion tool. Use the
    # standard one that is included with AthMonitorAlgorithm.

    # # First, add a tool that's set up by a different configuration function. 
    # # In this case, CaloNoiseToolCfg returns its own component accumulator, 
    # # which must be merged with the one from this function.
    # from CaloTools.CaloNoiseToolConfig import CaloNoiseToolCfg
    # caloNoiseAcc, caloNoiseTool = CaloNoiseToolCfg(inputFlags)
    # result.merge(caloNoiseAcc)
    # myMonAlg.CaloNoiseTool = caloNoiseTool

    # set up geometry / conditions
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
    nProfileBins = 360
    N_BARRELS = 4
    nSidesInclBoth = 3 # 0: Side 0, 1: Side 1, 2: both sides
    bothSides = 2
    nSurfaces = 3 # 0: 100, 1: 111, 2: all surfaces
    surfaceNames = ["_100",   "_111",   ""]
    surfaceNames2 = ["_100_",   "_111_",   ""]
    surfaceTitles = ["100 - ", "111 - ", ""]
    sideNames = ["_0", "_1", ""]
    for l in range(N_BARRELS):
        for iSurface in range(nSurfaces):
            for side in range(nSidesInclBoth):
                xVar = "phiToWafer_"+str(l)+surfaceNames[iSurface]+sideNames[side]
                yVar = "nStrip_"+str(l)+surfaceNames[iSurface]+sideNames[side]
                if side==bothSides:
                    histTitle = surfaceTitles[iSurface]+"Inc. Angle vs nStrips for Layer"+str(l)
                    histName = "h_phiVsNstrips"+surfaceNames2[iSurface]+str(l)
                else:
                    histTitle = surfaceTitles[iSurface]+"Inc. Angle vs nStrips for Layer Side"+str(l)+str(side)
                    histName = "h_phiVsNstrips"+surfaceNames2[iSurface]+str(l)+"Side"+str(side)
                myMonGroup.defineHistogram(varname=xVar+","+yVar+";"+histName, # ; means alias
                                           type="TProfile",
                                           title=histTitle+";#phi to Wafer;Num of Strips",
                                           path="lorentz", # path cannot be "".
                                           xbins=nProfileBins, xmin=-90., xmax=90.)

    ### STEP 6 ###
    # Finalize. The return value should be a tuple of the ComponentAccumulator
    # and the sequence containing the created algorithms. If we haven't called
    # any configuration other than the AthMonitorCfgHelper here, then we can 
    # just return directly (and not create "result" above)
    #return helper.result()
    
    # # Otherwise, merge with result object and return
    result.merge(helper.result())
    return result
