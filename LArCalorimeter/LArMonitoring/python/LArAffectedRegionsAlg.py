#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file ExampleLArMonitorAlgorithm.py
@author C. D. Burton
@author P. Onyisi
@date 2018-01-11
@brief Example python configuration for the Run III AthenaMonitoring package
'''

def LArAffectedRegionsConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    ### STEP 1 ###
    # Define one top-level monitoring algorithm. The new configuration 
    # framework uses a component accumulator.
    #from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    #result = ComponentAccumulator()

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'LArAffectedRegionsCfg')


    ### STEP 2 ###
    # Adding an algorithm to the helper. Just pass the type to the 
    # helper. Then, the helper will instantiate an instance and set up the 
    # base class configuration following the inputFlags. The returned object 
    # is the algorithm.
    from LArMonitoring.LArMonitoringConf import LArAffectedRegionsAlg
    larAffectedRegAlg = helper.addAlgorithm(LArAffectedRegionsAlg,'larAffectedRegAlg')

    #define the group names here, as you'll use them multiple times
    affectedRegGroupName="LArAffectedRegionsMonGroup"


    ### STEP 3 ###
    # Edit properties of a algorithm
    larAffectedRegAlg.AffectedRegionsGroupName=affectedRegGroupName
    larAffectedRegAlg.IsOnline = isOnline  #assuming isOnline is defined in upper level JO

    ### STEP 4 ###
    # Add some tools. N.B. Do not use your own trigger decion tool. Use the
    # standard one that is included with AthMonitorAlgorithm.

    # # First, add a tool that's set up by a different configuration function. 
    # # In this case, CaloNoiseToolCfg returns its own component accumulator, 
    # # which must be merged with the one from this function.
    # from CaloTools.CaloNoiseToolConfig import CaloNoiseToolCfg
    # caloNoiseAcc, caloNoiseTool = CaloNoiseToolCfg(inputFlags)
    # result.merge(caloNoiseAcc)
    # exampleMonAlg.CaloNoiseTool = caloNoiseTool

    # # Then, add a tool that doesn't have its own configuration function. In
    # # this example, no accumulator is returned, so no merge is necessary.
    # from MyDomainPackage.MyDomainPackageConf import MyDomainTool
    # exampleMonAlg.MyDomainTool = MyDomainTool()



    # Add a generic monitoring tool (a "group" in old language). The returned 
    # object here is the standard GenericMonitoringTool.
    #for each group, then add two histograms

    from LArMonitoring import GlobalVariables #to define the ranges
    larAffReg_hist_path='AffectedRegions/' #histogram path
    
    #EMBPS
    group_name_ending="EMBPS" 
    larAffectedRegAlg.EMBPSname=group_name_ending
    affectedRegGroupEMBPS = helper.addGroup(
        larAffectedRegAlg,
        affectedRegGroupName+group_name_ending,
        '/LAr/'
    )

    affectedRegGroupEMBPS.defineHistogram('etaPOS,phi;LArAffectedRegionsEMBAPS',
                                          title='HV Affected Regions - EMBA - Presampler;#eta;#phi',
                                          type='TH2F',
                                          path=larAffReg_hist_path,
                                          weight='problem',
                                          xbins=GlobalVariables.HVeta_EMB["EMBAPS"][0],xmin=GlobalVariables.HVeta_EMB["EMBAPS"][1],xmax=GlobalVariables.HVeta_EMB["EMBAPS"][2],
                                          ybins=GlobalVariables.HVphi_EMB["EMBAPS"][0],ymin=GlobalVariables.HVphi_EMB["EMBAPS"][1],ymax=GlobalVariables.HVphi_EMB["EMBAPS"][2]
    )
    affectedRegGroupEMBPS.defineHistogram('etaNEG,phi;LArAffectedRegionsEMBCPS',
                                          title='HV Affected Regions - EMBC - Presampler;#eta;#phi',
                                          type='TH2F',
                                          path=larAffReg_hist_path,
                                          weight='problem',
                                          xbins=GlobalVariables.HVeta_EMB["EMBCPS"][0],xmin=GlobalVariables.HVeta_EMB["EMBCPS"][1],xmax=GlobalVariables.HVeta_EMB["EMBCPS"][2],
                                          ybins=GlobalVariables.HVphi_EMB["EMBCPS"][0],ymin=GlobalVariables.HVphi_EMB["EMBCPS"][1],ymax=GlobalVariables.HVphi_EMB["EMBCPS"][2]

    )

    #EMB
    group_name_ending="EMB"
    larAffectedRegAlg.EMBname=group_name_ending
    affectedRegGroupEMB = helper.addGroup(
        larAffectedRegAlg,
        affectedRegGroupName+group_name_ending,
        '/LAr/'
    )

    affectedRegGroupEMB.defineHistogram('etaPOS,phi;LArAffectedRegionsEMBA',
                                        title='HV Affected Regions - EMBA - Samplings 1-3;#eta;#phi',
                                        type='TH2F',
                                        path=larAffReg_hist_path,
                                        weight='problem',
                                        xbins=GlobalVariables.HVeta_EMB["EMBA"][0],xmin=GlobalVariables.HVeta_EMB["EMBA"][1],xmax=GlobalVariables.HVeta_EMB["EMBA"][2],
                                        ybins=GlobalVariables.HVphi_EMB["EMBA"][0],ymin=GlobalVariables.HVphi_EMB["EMBA"][1],ymax=GlobalVariables.HVphi_EMB["EMBA"][2]
         
    )
    affectedRegGroupEMB.defineHistogram('etaNEG,phi;LArAffectedRegionsEMBC',
                                        title='HV Affected Regions - EMBC - Samplings 1-3;#eta;#phi',
                                        type='TH2F',
                                        path=larAffReg_hist_path,
                                        weight='problem',
                                        xbins=GlobalVariables.HVeta_EMB["EMBC"][0],xmin=GlobalVariables.HVeta_EMB["EMBC"][1],xmax=GlobalVariables.HVeta_EMB["EMBC"][2],
                                        ybins=GlobalVariables.HVphi_EMB["EMBC"][0],ymin=GlobalVariables.HVphi_EMB["EMBC"][1],ymax=GlobalVariables.HVphi_EMB["EMBC"][2]
    )
        

    #EMECPS
    group_name_ending="EMECPS"
    larAffectedRegAlg.EMECPSname=group_name_ending
    affectedRegGroupEMECPS = helper.addGroup(
        larAffectedRegAlg,
        affectedRegGroupName+group_name_ending,
        '/LAr/'
    )
    
    affectedRegGroupEMECPS.defineHistogram('etaPOS,phi;LArAffectedRegionsEMECAPS',
                                           title='HV Affected Regions - EMECA - Presampler;#eta;#phi',
                                           type='TH2F',
                                           path=larAffReg_hist_path,
                                           weight='problem',
                                           xbins=GlobalVariables.HVeta_EMEC["EMECAPS"],
                                           ybins=GlobalVariables.HVphi_EMEC["EMECAPS"]
    )
    affectedRegGroupEMECPS.defineHistogram('etaNEG,phi;LArAffectedRegionsEMECCPS',
                                           title='HV Affected Regions - EMECC - Presampler;#eta;#phi',
                                           type='TH2F',
                                           path=larAffReg_hist_path,
                                           weight='problem',
                                           xbins=GlobalVariables.HVeta_EMEC["EMECCPS"],
                                           ybins=GlobalVariables.HVphi_EMEC["EMECCPS"]
    )


    #EMEC
    group_name_ending="EMEC"
    larAffectedRegAlg.EMECname=group_name_ending 
    affectedRegGroupEMEC = helper.addGroup(
        larAffectedRegAlg,
        affectedRegGroupName+group_name_ending,
        '/LAr/'
    )
    
    affectedRegGroupEMEC.defineHistogram('etaPOS,phi;LArAffectedRegionsEMECA',
                                         title='HV Affected Regions - EMECA - Samplings 1-3;#eta;#phi',
                                         type='TH2F',
                                         path=larAffReg_hist_path,
                                         weight='problem',
                                         xbins=GlobalVariables.HVeta_EMEC["EMECA"],
                                         ybins=GlobalVariables.HVphi_EMEC["EMECA"]
    )
    affectedRegGroupEMEC.defineHistogram('etaNEG,phi;LArAffectedRegionsEMECC',
                                         title='HV Affected Regions - EMECC - Samplings 1-3;#eta;#phi',
                                         type='TH2F',
                                         path=larAffReg_hist_path,
                                         weight='problem',
                                         xbins=GlobalVariables.HVeta_EMEC["EMECC"],
                                         ybins=GlobalVariables.HVphi_EMEC["EMECC"]
    )


    #HEC0
    group_name_ending="HEC0"
    larAffectedRegAlg.HEC0name=group_name_ending
    affectedRegGroupHEC0 = helper.addGroup(
        larAffectedRegAlg,
        affectedRegGroupName+group_name_ending,
        '/LAr/'
    )
        
    affectedRegGroupHEC0.defineHistogram('etaPOS,phi;LArAffectedRegionsHECA0',
                                         title='HV Affected Regions - HECA - Layer 1;#eta;#phi',
                                         type='TH2F',
                                         path=larAffReg_hist_path,
                                         weight='problem',
                                         xbins=GlobalVariables.HVeta_HECFcal["HECA"][0],xmin=GlobalVariables.HVeta_HECFcal["HECA"][1],xmax=GlobalVariables.HVeta_HECFcal["HECA"][2],
                                         ybins=GlobalVariables.HVphi_HECFcal["HECA"][0],ymin=GlobalVariables.HVphi_HECFcal["HECA"][1],ymax=GlobalVariables.HVphi_HECFcal["HECA"][2]
    )
    affectedRegGroupHEC0.defineHistogram('etaNEG,phi;LArAffectedRegionsHECC0',
                                         title='HV Affected Regions - HECC - Layer 1;#eta;#phi',
                                         type='TH2F',
                                         path=larAffReg_hist_path,
                                         weight='problem',
                                         xbins=GlobalVariables.HVeta_HECFcal["HECC"][0],xmin=GlobalVariables.HVeta_HECFcal["HECC"][1],xmax=GlobalVariables.HVeta_HECFcal["HECC"][2],
                                         ybins=GlobalVariables.HVphi_HECFcal["HECC"][0],ymin=GlobalVariables.HVphi_HECFcal["HECC"][1],ymax=GlobalVariables.HVphi_HECFcal["HECC"][2]
    )

    #HEC1
    group_name_ending="HEC1"
    larAffectedRegAlg.HEC1name=group_name_ending
    affectedRegGroupHEC1 = helper.addGroup(
        larAffectedRegAlg,
        affectedRegGroupName+group_name_ending,
        '/LAr/'
    )
    
    affectedRegGroupHEC1.defineHistogram('etaPOS,phi;LArAffectedRegionsHECA1',
                                         title='HV Affected Regions - HECA - Layer 2;#eta;#phi',
                                         type='TH2F',
                                         path=larAffReg_hist_path,
                                         weight='problem',
                                         xbins=GlobalVariables.HVeta_HECFcal["HECA"][0],xmin=GlobalVariables.HVeta_HECFcal["HECA"][1],xmax=GlobalVariables.HVeta_HECFcal["HECA"][2],
                                         ybins=GlobalVariables.HVphi_HECFcal["HECA"][0],ymin=GlobalVariables.HVphi_HECFcal["HECA"][1],ymax=GlobalVariables.HVphi_HECFcal["HECA"][2]
    )
    affectedRegGroupHEC1.defineHistogram('etaNEG,phi;LArAffectedRegionsHECC1',
                                         title='HV Affected Regions - HECC - Layer 2;#eta;#phi',
                                         type='TH2F',
                                         path=larAffReg_hist_path,
                                         weight='problem',
                                         xbins=GlobalVariables.HVeta_HECFcal["HECC"][0],xmin=GlobalVariables.HVeta_HECFcal["HECC"][1],xmax=GlobalVariables.HVeta_HECFcal["HECC"][2],
                                         ybins=GlobalVariables.HVphi_HECFcal["HECC"][0],ymin=GlobalVariables.HVphi_HECFcal["HECC"][1],ymax=GlobalVariables.HVphi_HECFcal["HECC"][2]
    )
    
    #HEC2
    group_name_ending="HEC2"
    larAffectedRegAlg.HEC2name=group_name_ending
    affectedRegGroupHEC2 = helper.addGroup(
        larAffectedRegAlg,
        affectedRegGroupName+group_name_ending,
        '/LAr/'
    )
    
    affectedRegGroupHEC2.defineHistogram('etaPOS,phi;LArAffectedRegionsHECA2',
                                         title='HV Affected Regions - HECA - Layer 3;#eta;#phi',
                                         type='TH2F',
                                         path=larAffReg_hist_path,
                                         weight='problem',
                                         xbins=GlobalVariables.HVeta_HECFcal["HECA"][0],xmin=GlobalVariables.HVeta_HECFcal["HECA"][1],xmax=GlobalVariables.HVeta_HECFcal["HECA"][2],
                                         ybins=GlobalVariables.HVphi_HECFcal["HECA"][0],ymin=GlobalVariables.HVphi_HECFcal["HECA"][1],ymax=GlobalVariables.HVphi_HECFcal["HECA"][2]
    )
    affectedRegGroupHEC2.defineHistogram('etaNEG,phi;LArAffectedRegionsHECC2',
                                         title='HV Affected Regions - HECC - Layer 3;#eta;#phi',
                                         type='TH2F',
                                         path=larAffReg_hist_path,
                                         weight='problem',
                                         xbins=GlobalVariables.HVeta_HECFcal["HECC"][0],xmin=GlobalVariables.HVeta_HECFcal["HECC"][1],xmax=GlobalVariables.HVeta_HECFcal["HECC"][2],
                                         ybins=GlobalVariables.HVphi_HECFcal["HECC"][0],ymin=GlobalVariables.HVphi_HECFcal["HECC"][1],ymax=GlobalVariables.HVphi_HECFcal["HECC"][2]
    )
        
    #HEC3
    group_name_ending="HEC3"
    larAffectedRegAlg.HEC3name=group_name_ending
    affectedRegGroupHEC3 = helper.addGroup(
        larAffectedRegAlg,
        affectedRegGroupName+group_name_ending,
        '/LAr/'
    )
    
    affectedRegGroupHEC3.defineHistogram('etaPOS,phi;LArAffectedRegionsHECA3',
                                         title='HV Affected Regions - HECA - Layer 4;#eta;#phi',
                                         type='TH2F',
                                         path=larAffReg_hist_path,
                                         weight='problem',
                                         xbins=GlobalVariables.HVeta_HECFcal["HECA"][0],xmin=GlobalVariables.HVeta_HECFcal["HECA"][1],xmax=GlobalVariables.HVeta_HECFcal["HECA"][2],
                                         ybins=GlobalVariables.HVphi_HECFcal["HECA"][0],ymin=GlobalVariables.HVphi_HECFcal["HECA"][1],ymax=GlobalVariables.HVphi_HECFcal["HECA"][2]
    )
    affectedRegGroupHEC3.defineHistogram('etaNEG,phi;LArAffectedRegionsHECC3',
                                         title='HV Affected Regions - HECC - Layer 4;#eta;#phi',
                                         type='TH2F',
                                         path=larAffReg_hist_path,
                                         weight='problem',
                                         xbins=GlobalVariables.HVeta_HECFcal["HECC"][0],xmin=GlobalVariables.HVeta_HECFcal["HECC"][1],xmax=GlobalVariables.HVeta_HECFcal["HECC"][2],
                                         ybins=GlobalVariables.HVphi_HECFcal["HECC"][0],ymin=GlobalVariables.HVphi_HECFcal["HECC"][1],ymax=GlobalVariables.HVphi_HECFcal["HECC"][2]
    )
    
    #FCAL0
    group_name_ending="FCAL0"
    larAffectedRegAlg.FCAL0name=group_name_ending
    affectedRegGroupFCAL0 = helper.addGroup(
        larAffectedRegAlg,
        affectedRegGroupName+group_name_ending,
        '/LAr/'
    )
    
    affectedRegGroupFCAL0.defineHistogram('etaPOS,phi;LArAffectedRegionsFCALA0',
                                          title='HV Affected Regions - FCALA - Layer 1;#eta;#phi',
                                          type='TH2F',
                                          path=larAffReg_hist_path,
                                          weight='problem',
                                          xbins=GlobalVariables.HVeta_HECFcal["FCalA"][0],xmin=GlobalVariables.HVeta_HECFcal["FCalA"][1],xmax=GlobalVariables.HVeta_HECFcal["FCalA"][2],
                                          ybins=GlobalVariables.HVphi_HECFcal["FCalA"][0],ymin=GlobalVariables.HVphi_HECFcal["FCalA"][1],ymax=GlobalVariables.HVphi_HECFcal["FCalA"][2]
    )
    affectedRegGroupFCAL0.defineHistogram('etaNEG,phi;LArAffectedRegionsFCALC0',
                                          title='HV Affected Regions - FCALC - Layer 1;#eta;#phi',
                                          type='TH2F',
                                          path=larAffReg_hist_path,
                                          weight='problem',
                                          xbins=GlobalVariables.HVeta_HECFcal["FCalC"][0],xmin=GlobalVariables.HVeta_HECFcal["FCalC"][1],xmax=GlobalVariables.HVeta_HECFcal["FCalC"][2],
                                          ybins=GlobalVariables.HVphi_HECFcal["FCalC"][0],ymin=GlobalVariables.HVphi_HECFcal["FCalC"][1],ymax=GlobalVariables.HVphi_HECFcal["FCalC"][2]
    )

    #FCAL1
    group_name_ending="FCAL1"
    larAffectedRegAlg.FCAL1name=group_name_ending
    affectedRegGroupFCAL1 = helper.addGroup(
        larAffectedRegAlg,
        affectedRegGroupName+group_name_ending,
        '/LAr/'
    )
    
    affectedRegGroupFCAL1.defineHistogram('etaPOS,phi;LArAffectedRegionsFCALA1',
                                          title='HV Affected Regions - FCALA - Layer 2;#eta;#phi',
                                          type='TH2F',
                                          path=larAffReg_hist_path,
                                          weight='problem',
                                          xbins=GlobalVariables.HVeta_HECFcal["FCalA"][0],xmin=GlobalVariables.HVeta_HECFcal["FCalA"][1],xmax=GlobalVariables.HVeta_HECFcal["FCalA"][2],
                                          ybins=GlobalVariables.HVphi_HECFcal["FCalA"][0],ymin=GlobalVariables.HVphi_HECFcal["FCalA"][1],ymax=GlobalVariables.HVphi_HECFcal["FCalA"][2]
    )
    affectedRegGroupFCAL1.defineHistogram('etaNEG,phi;LArAffectedRegionsFCALC1',
                                          title='HV Affected Regions - FCALC - Layer 2;#eta;#phi',
                                          type='TH2F',
                                          path=larAffReg_hist_path,
                                          weight='problem',
                                          xbins=GlobalVariables.HVeta_HECFcal["FCalC"][0],xmin=GlobalVariables.HVeta_HECFcal["FCalC"][1],xmax=GlobalVariables.HVeta_HECFcal["FCalC"][2],
                                          ybins=GlobalVariables.HVphi_HECFcal["FCalC"][0],ymin=GlobalVariables.HVphi_HECFcal["FCalC"][1],ymax=GlobalVariables.HVphi_HECFcal["FCalC"][2]

    )

    #FCAL2
    group_name_ending="FCAL2"
    larAffectedRegAlg.FCAL2name=group_name_ending
    affectedRegGroupFCAL2 = helper.addGroup(
        larAffectedRegAlg,
        affectedRegGroupName+group_name_ending,
        '/LAr/'
    )
        
    affectedRegGroupFCAL2.defineHistogram('etaPOS,phi;LArAffectedRegionsFCALA2',
                                          title='HV Affected Regions - FCALA - Layer 3;#eta;#phi',
                                          type='TH2F',
                                          path=larAffReg_hist_path,
                                          weight='problem',
                                          xbins=GlobalVariables.HVeta_HECFcal["FCalA"][0],xmin=GlobalVariables.HVeta_HECFcal["FCalA"][1],xmax=GlobalVariables.HVeta_HECFcal["FCalA"][2],
                                          ybins=GlobalVariables.HVphi_HECFcal["FCalA"][0],ymin=GlobalVariables.HVphi_HECFcal["FCalA"][1],ymax=GlobalVariables.HVphi_HECFcal["FCalA"][2]
    )
    affectedRegGroupFCAL2.defineHistogram('etaNEG,phi;LArAffectedRegionsFCALC2',
                                          title='HV Affected Regions - FCALC - Layer 3;#eta;#phi',
                                          type='TH2F',
                                          path=larAffReg_hist_path,
                                          weight='problem',
                                          xbins=GlobalVariables.HVeta_HECFcal["FCalC"][0],xmin=GlobalVariables.HVeta_HECFcal["FCalC"][1],xmax=GlobalVariables.HVeta_HECFcal["FCalC"][2],
                                          ybins=GlobalVariables.HVphi_HECFcal["FCalC"][0],ymin=GlobalVariables.HVphi_HECFcal["FCalC"][1],ymax=GlobalVariables.HVphi_HECFcal["FCalC"][2]
    )

    #    pass #end of isOnline
    


    ### STEP 6 ###
    # Finalize. The return value should be a tuple of the ComponentAccumulator
    # and the sequence containing the created algorithms. If we haven't called
    # any configuration other than the AthMonitorCfgHelper here, then we can 
    # just return directly (and not create "result" above)
    
    return helper.result()
    
    # # Otherwise, merge with result object and return
    # acc, seq = helper.result()
    # result.merge(acc)
    # return result, seq


if __name__=='__main__':

    isOnline=False

    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    nightly = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/'
    file = 'data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1'
    ConfigFlags.Input.Files = [nightly+file]
    ConfigFlags.Input.isMC = False
    ConfigFlags.Output.HISTFileName = 'LArAffectedRegionsOutput.root'
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesSerialCfg()
    cfg.merge(PoolReadCfg(ConfigFlags))

#    larAffectedRegionsAcc, larAffectedRegionsAlg = LArAffectedRegionsConfig(ConfigFlags)
    larAffectedRegionsAcc = LArAffectedRegionsConfig(ConfigFlags)
    
    cfg.merge(larAffectedRegionsAcc)
    Nevents=10
    cfg.run(Nevents) #use cfg.run() to run on all events
