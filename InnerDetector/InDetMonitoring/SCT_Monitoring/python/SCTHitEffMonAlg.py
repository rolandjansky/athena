#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
'''@file SCTHitEffMonAlg.py
@author Ken Kreul
@date 2019-10-30
@brief Based on AthenaMonitoring/ExampleMonitorAlgorithm.py
'''

def dedicatedTitle(i, isub):
    m_element = i
    m_layerStr = i / 2
    m_region = isub
    if m_region == 1 :
        return "Layer " + str(m_layerStr) + " Side " + str((m_element % 2 + 1) % 2)
    else:
        return "Disk " + str(m_layerStr) + " Side " + str((m_element % 2 + 1) % 2)

def SCTHitEffMonAlgConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''
    ### STEP 1 ###
    # Define one top-level monitoring algorithm. The new configuration 
    # framework uses a component accumulator.
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags, 'SCTHitEffMonCfg')


    ### STEP 2 ###
    # Adding an algorithm to the helper. Here, we will use the example 
    # algorithm in the AthenaMonitoring package. Just pass the type to the 
    # helper. Then, the helper will instantiate an instance and set up the 
    # base class configuration following the inputFlags. The returned object 
    # is the algorithm.
    from AthenaConfiguration.ComponentFactory import CompFactory
    myMonAlg = helper.addAlgorithm(CompFactory.SCTHitEffMonAlg, 'SCTHitEffMonAlg')
    # # If for some really obscure reason you need to instantiate an algorithm
    # # yourself, the AddAlgorithm method will still configure the base 
    # # properties and add the algorithm to the monitoring sequence.
    # helper.AddAlgorithm(myExistingAlg)


    ### STEP 3 ###
    # Edit properties of a algorithm
    myMonAlg.TriggerChain = ''

    from LumiBlockComps.BunchCrossingCondAlgConfig import BunchCrossingCondAlgCfg
    result.merge(BunchCrossingCondAlgCfg(inputFlags))

    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    result.merge(MagneticFieldSvcCfg(inputFlags))

    ### STEP 4 ###
    # Add some tools. N.B. Do not use your own trigger decion tool. Use the
    # standard one that is included with AthMonitorAlgorithm.

    # set up geometry / conditions
    from AtlasGeoModel.InDetGMConfig import InDetGeometryCfg
    result.merge(InDetGeometryCfg(inputFlags))

    # Add a generic monitoring tool (a "group" in old language). The returned 
    # object here is the standard GenericMonitoringTool.

    from ROOT import SCT_Monitoring as sctMon

    myMonGroup = [
        helper.addGroup(
            myMonAlg,
            "SCTHitEffMonitorEC",
            "SCT/SCTEC/"
        ),
        helper.addGroup(
            myMonAlg,
            "SCTHitEffMonitorB",
            "SCT/SCTB/"
        ),
        helper.addGroup(
            myMonAlg,
            "SCTHitEffMonitorEA",
            "SCT/SCTEA/"
        ),
        helper.addGroup(
            myMonAlg,
            "SCTHitEffMonitor",
            "SCT/GENERAL/"
        )
    ]

    ### STEP 5 ###
    # Configure histograms
    
    # Conversion of ROOT.vector of ROOT.TString to list of str
    subDetName = []
    for i in range(len(sctMon.subDetName)):
        subDetName.append(sctMon.subDetName[i].Data())

    mapName = ["m_eff_", "eff_", "p_eff_"] 
    ineffmapName = ["ineffm_", "ineff_", "ineffp_"] 

    sumEff = ["summaryeffm", "summaryeff", "summaryeffp"]
    sumEffTitle = ["Summary Module Efficiency in Endcap C",
                   "Summary Module Efficiency in Barrel",
                   "Summary Module Efficiency in Endcap A"]

    limit = [sctMon.N_DISKS*2, sctMon.N_BARRELS*2, sctMon.N_DISKS*2]

    # GENERAL
    myMonGroup[sctMon.GENERAL_INDEX].defineHistogram(varname= "isub, eff;" + "SctTotalEff",
                                                     type= "TProfile",
                                                     title= "SCT Total Efficiency",
                                                     path="eff",
                                                     xbins=sctMon.N_REGIONS,
                                                     xmin=0.,
                                                     xmax=sctMon.N_REGIONS,
                                                     xlabels=subDetName)

    myMonGroup[sctMon.GENERAL_INDEX].defineHistogram(varname= "isub, eff;" + "SctTotalEffBCID",
                                                     type= "TProfile",
                                                     title= "SCT Total Efficiency for First BCID",
                                                     path="eff",
                                                     xbins=sctMon.N_REGIONS,
                                                     xmin=0.,
                                                     xmax=sctMon.N_REGIONS,
                                                     xlabels=subDetName,
                                                     cutmask="isFirstBCID")

    myMonGroup[sctMon.GENERAL_INDEX].defineHistogram(varname= "sideHash, eff;" + "effHashCode",
                                                     type= "TProfile",
                                                     title= "Efficiency vs module Hash code" + ";Module Hash Code;Efficiency",
                                                     path="eff",
                                                     xbins=sctMon.n_mod[sctMon.GENERAL_INDEX] * 2,
                                                     xmin=-0.5,
                                                     xmax=sctMon.n_mod[sctMon.GENERAL_INDEX] * 2 -0.5)
                
    myMonGroup[sctMon.GENERAL_INDEX].defineHistogram(varname= "LumiBlock, eff;" + "effLumiBlock",
                                                     type= "TProfile",
                                                     title= "Efficiency vs Luminosity block"+";;Efficiency",
                                                     path="eff",
                                                     xbins=sctMon.NBINS_LBs,
                                                     xmin=0.5,
                                                     xmax=sctMon.NBINS_LBs + 0.5)

    ### This histogram should be filled by post processing ###
    # myMonGroup[sctMon.GENERAL_INDEX].defineHistogram(varname= "eff;" + "SctEffDistribution",
    #                                                  type= "TH1F",
    #                                                  title= "SCT Efficiency Distribution"+";Efficiency;Links",
    #                                                  path="eff",
    #                                                  xbins= 500,
    #                                                  xmin=0.,
    #                                                  xmax=1.)

    # SCTEC, SCTB, SCTEA
    for isub in range(sctMon.N_REGIONS):
        profileLabels = list(range(limit[isub]))
        for k in range(limit[isub]):
            profileLabels[k] = dedicatedTitle(k, isub)
        # Efficiency
        myMonGroup[isub].defineHistogram(varname= "layerPlusHalfSide, eff;" + sumEff[isub],
                                         type= "TProfile",
                                         title= sumEffTitle[isub]+ ";;Efficiency",
                                         path="eff",
                                         xbins=2*sctMon.n_layers[isub],
                                         xmin=0.,
                                         xmax=sctMon.n_layers[isub],
                                         xlabels=profileLabels)
        # Efficiency for first BCIDs
        myMonGroup[isub].defineHistogram(varname= "layerPlusHalfSide, eff;" + sumEff[isub] + "BCID",
                                         type= "TProfile",
                                         title= sumEffTitle[isub]+" for First BC" + ";;Efficiency", 
                                         path="eff",
                                         xbins=2*sctMon.n_layers[isub],
                                         xmin=0.,
                                         xmax=sctMon.n_layers[isub],
                                         xlabels=profileLabels,
                                         cutmask="isFirstBCID")
        # Efficiency as a function of LB
        myMonGroup[isub].defineHistogram(varname= "LumiBlock, eff;"+"effLumiBlock", #different names for fill
                                         type= "TProfile",
                                         title= "Efficiency vs Luminosity block in "+subDetName[isub]+";Luminosity block"+";Efficiency",
                                         path="eff",
                                         xbins=sctMon.NBINS_LBs,
                                         xmin=0.5,
                                         xmax=sctMon.NBINS_LBs + 0.5)

        # Disks for SCTEC and SCTEA and layers for SCTB
        for layer_disk in range(sctMon.n_layers[isub]):
            for side in range(2):
                etaPhiSuffix = "_" + str(layer_disk) + "_" + str(side)
                effName = mapName[isub] + str(layer_disk) + "_" + str(side)
                ineffName = ineffmapName[isub] + str(layer_disk) + "_" + str(side)
                # Efficiency
                myMonGroup[isub].defineHistogram(varname="ieta" + etaPhiSuffix + ",iphi" + etaPhiSuffix + ",eff;" + effName,
                                                 type= "TProfile2D",
                                                 title="Hit efficiency of" + sctMon.layerName[isub].Data() + str(layer_disk) + " / side " + str(side) + " in " + subDetName[isub] + ";Index in the direction of #eta;Index in the direction of #phi",
                                                 path="eff",
                                                 xbins=sctMon.n_etabins[isub], xmin=sctMon.f_etabin[isub] - .5, xmax=sctMon.l_etabin[isub] + .5,
                                                 ybins=sctMon.n_phibins[isub], ymin=sctMon.f_phibin[isub] - .5, ymax=sctMon.l_phibin[isub] + .5)
                # Efficiency for first BCIDs
                myMonGroup[isub].defineHistogram(varname="ieta" + etaPhiSuffix + ",iphi" + etaPhiSuffix + ",eff;" + effName + "_bcid",
                                                 type= "TProfile2D",
                                                 title="Hit efficiency of" + sctMon.layerName[isub].Data() + str(layer_disk) + " / side " + str(side) + " in " + subDetName[isub] +  " for first BCID" + ";Index in the direction of #eta;Index in the direction of #phi",
                                                 path="eff",
                                                 xbins=sctMon.n_etabins[isub], xmin=sctMon.f_etabin[isub] - .5, xmax=sctMon.l_etabin[isub] + .5,
                                                 ybins=sctMon.n_phibins[isub], ymin=sctMon.f_phibin[isub] - .5, ymax=sctMon.l_phibin[isub] + .5,
                                                 cutmask="isFirstBCID")
                # Inefficiency
                myMonGroup[isub].defineHistogram(varname="ieta" + etaPhiSuffix + ",iphi" + etaPhiSuffix  + ",ineff;" + ineffName,
                                                 type= "TProfile2D",
                                                 title="Hit inefficiency of" + sctMon.layerName[isub].Data() + str(layer_disk) + " / side " + str(side) + " in " + subDetName[isub] + ";Index in the direction of #eta;Index in the direction of #phi",
                                                 path="eff",
                                                 xbins=sctMon.n_etabins[isub], xmin=sctMon.f_etabin[isub] - .5, xmax=sctMon.l_etabin[isub] + .5,
                                                 ybins=sctMon.n_phibins[isub], ymin=sctMon.f_phibin[isub] - .5, ymax=sctMon.l_phibin[isub] + .5)

    # Merge with result object and return
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
    ConfigFlags.Input.isMC = True
    ConfigFlags.Output.HISTFileName = 'SCTHitEffMonOutput.root'
    ConfigFlags.GeoModel.Align.Dynamic = False
    ConfigFlags.Detector.GeometryID = True
    ConfigFlags.Detector.GeometryPixel = True
    ConfigFlags.Detector.GeometrySCT = True
    ConfigFlags.Detector.GeometryTRT = True
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    from AtlasGeoModel.AtlasGeoModelConfig import AtlasGeometryCfg
    geoCfg=AtlasGeometryCfg(ConfigFlags)
    cfg.merge(geoCfg)

    sctHitEffMonAcc = SCTHitEffMonAlgConfig(ConfigFlags)
    cfg.merge(sctHitEffMonAcc)

    cfg.run()
