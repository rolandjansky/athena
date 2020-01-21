#
#Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration                                                                                           
#

from AthenaConfiguration.ComponentFactory import CompFactory
#from MdtRawDataMonitoring.MdtRawMonLabels import *
from MdtMonUtils import getMDTLabel
from MDTTubeMax import tubeMax
from MDTChambers import mdtBA,mdtBC,mdtEA,mdtEC

def MdtMonitoringConfigOld(inputFlags):
    from AthenaMonitoring import AthMonitorCfgHelperOld
    helper = AthMonitorCfgHelperOld(inputFlags,'MdtAthMonitorCfg')
    return helper.result()


def MdtMonitoringConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    ### STEP 1 ###
    # Define one top-level monitoring algorithm. The new configuration 
    # framework uses a component accumulator.
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    #mik
    # Make sure muon geometry is configured
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    result.merge(MuonGeoModelCfg(inputFlags))

    # Temporary, until we move to services/private tools-- from MuonSpectrometer/MuonConfig
    result.addPublicTool( CompFactory.Muon__MuonIdHelperTool() )

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'MdtAthMonitorCfg')
    
    #from AthenaCommon.CfgGetter import getAlgorithm

    ### STEP 2 ###
    # Adding an algorithm to the helper. Here, we will use the example 
    # algorithm in the AthenaMonitoring package. Just pass the type to the 
    # helper. Then, the helper will instantiate an instance and set up the 
    # base class configuration following the inputFlags. The returned object 
    # is the algorithm.
    #MdtRawDataMonAlg.DoMdtEsd = True
    mdtMonAlg = helper.addAlgorithm(CompFactory.MdtRawDataMonAlg,'MdtMonAlg')
    mdtMonAlg.DoMdtEsd = True

    # You can actually make multiple instances of the same algorithm and give 
    # them different configurations
    ######anotherExampleMonAlg = helper.addAlgorithm(ExampleMonitorAlgorithm,'AnotherExampleMonAlg')

    # # If for some really obscure reason you need to instantiate an algorithm
    # # yourself, the AddAlgorithm method will still configure the base 
    # # properties and add the algorithm to the monitoring sequence.
    # helper.AddAlgorithm(myExistingAlg)


    ### STEP 3 ###
    # Edit properties of a algorithm
    #####exampleMonAlg.TriggerChain = ''

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
    mdtGroup = helper.addGroup(mdtMonAlg,'MdtMonitor','Muon/MuonRawDataMonitoring/MDT/')

    # Add a GMT for the other example monitor algorithm
    ### STEP 5 ###
    # Configure histograms
    mdtGroup.defineHistogram('mdt_tube_z_barrel,mdt_tube_perp_barrel;Number_of_BarrelMDTHits_inRZView_Global_ADCCut', type='TH2F',
                            title='Number_of_BarrelMDTHits_inRZView_Global_ADCCut;MDT-GlobalZ(mm);MDT-GlobalR(mm)',
                            path='Overview/Expert/Geometry',  xbins=250,xmin=-25000.,xmax=25000., ybins=120, ymin=0., ymax=12000.   
                            )

    mdtGroup.defineHistogram('mdt_tube_x_barrel,mdt_tube_y_barrel;Number_of_BarrelMDTHits_inXYView_Global_ADCCut',   type='TH2F',
                            title='Number_of_BarrelMDTHits_inXYView_Global_ADCCut;MDT-GlobalX(mm);MDT-GlobalY(mm)',
                            path='Overview/Expert/Geometry',   xbins=150, xmin=-15000., xmax=15000., ybins=150, ymin=-15000., ymax=15000.
                            )

    mdtGroup.defineHistogram('mdt_tube_z_ovl,mdt_tube_perp_ovl;Number_of_OverlapMDTHits_inRZView_Global_ADCCut',  type='TH2F',
                            title='Number_of_OverlapMDTHits_inRZView_Global_ADCCut;MDT-GlobalZ(mm);MDT-GlobalR(mm)',
                            path='Overview/Expert/Geometry',  xbins=250,xmin=-25000.,xmax=25000., ybins=120, ymin=0., ymax=12000.   
                            )

    mdtGroup.defineHistogram('mdt_tube_x_ovl,mdt_tube_y_ovl;Number_of_OverlapMDTHits_inXYView_Global_ADCCut',   type='TH2F',
                            title='Number_of_OverlapMDTHits_inXYView_Global_ADCCut;MDT-GlobalX(mm);MDT-GlobalY(mm)',
                            path='Overview/Expert/Geometry',   xbins=150, xmin=-15000., xmax=15000., ybins=150, ymin=-15000., ymax=15000.
                            )

    mdtGroup.defineHistogram('mdt_tube_z_endcap,mdt_tube_perp_endcap;Number_of_EndcapMDTHits_inRZView_Global_ADCCut', type='TH2F',
                            title='Number_of_EndcapMDTHits_inRZView_Global_ADCCut;MDT-GlobalZ(mm);MDT-GlobalR(mm)',
                            path='Overview/Expert/Geometry',  xbins=250,xmin=-25000.,xmax=25000., ybins=120, ymin=0., ymax=12000.   
                            )

    mdtGroup.defineHistogram('mdt_tube_x_endcap,mdt_tube_y_endcap;Number_of_EndcapMDTHits_inXYView_Global_ADCCut',  type='TH2F',
                            title='Number_of_EndcapMDTHits_inXYView_Global_ADCCut;MDT-GlobalX(mm);MDT-GlobalY(mm)',
                            path='Overview/Expert/Geometry',   xbins=150, xmin=-15000., xmax=15000., ybins=150, ymin=-15000., ymax=15000.
                            )

    mdtGroup.defineHistogram('tdc_mon,adc_mon;Overall_TDCADC_spectrum',    type='TH2F',
                            title='Overall_TDCADC_spectrum;[nsec];[adc counts]',
                            path='Overview',   xbins=50, xmin=0., xmax=2000., ybins=20, ymin=0., ymax=400.
                            )

    mdtGroup.defineHistogram('adc_mon_nosel;Overall_ADC_spectrum',  type='TH1F',
                            title='Overall_ADC_spectrum;[adc counts];Number of Entries',
                            path='Overview',   xbins=100, xmin=0., xmax=400.
                            )

    mdtGroup.defineHistogram('adc_mon_nosel;Overall_ADC_spectrum_NoiseBursts',  type='TH1F',
                            cutmask='noiseBurst',
                            title='Overall_ADC_spectrum_NoiseBursts;[adc counts];Number of Entries',
                            path='Overview',   xbins=100, xmin=0., xmax=400.
                            )
    
    mdtGroup.defineHistogram('tdc_mon_nosel,adc_mon_nosel;Overall_TDCADC_spectrum_NoiseBursts',    type='TH2F',
                            cutmask='noiseBurst',
                            title='Overall_TDCADC_spectrum_NoiseBursts;[nsec];[adc counts]',
                            path='Overview',   xbins=50, xmin=0., xmax=2000., ybins=20, ymin=0., ymax=400.
                            )

    mdtGroup.defineHistogram('tdc_mon_nosel;Overall_TDC_spectrum_NoiseBursts',  type='TH1F',
                            cutmask='noiseBurst',
                            title='Overall_TDC_spectrum_NoiseBursts;[nsec];Number of Entries',
                            path='Overview',   xbins=120, xmin=0., xmax=2000.
                            )

    ##Maybe not needed
    mdtGroup.defineHistogram('tdc_mon_noiseBurst_adcCut;Overall_TDC_spectrum_NoiseBursts_ADCCut',  type='TH1F',
                            title='Overall_TDC_spectrum_NoiseBursts_ADCCut;[nsec];Number of Entries',
                            path='Overview',   xbins=120, xmin=0., xmax=2000.
                            )

    mdtGroup.defineHistogram('tdc_mon_adcCut;Overall_TDC_ADCCUT_spectrum',  type='TH1F',
                            title='Overall_TDC_spectrum_ADCCut;[nsec];Number of Entries',
                            path='Overview',   xbins=120, xmin=0., xmax=2000.
                            )



    bigRegions = ["Barrel","EndCap"]
    regions = ["BA","BC","EA","EC"]
    layers = ["Inner","Middle","Outer","Extra"]

    #mdtNumberOfHitPerChamber=helper.addGroup(mdtMonAlg, "NumberOfHitPerChamber", "Muon/MuonRawDataMonitoring/MDT/")
    for iregion in bigRegions:
        theTitle = "NumberOfHitsIn"+iregion+"PerChamber_ADCCut"
        theTitle_noise = "NumberOfHitsIn"+iregion+"PerChamber_ADCCutt_NoiseBurst"
        thisLabelx,thisLabely=getMDTLabel(theTitle+"_labelx",theTitle+"_labely")
        if iregion=="Barrel":
            etamin=-8.
            etamax=9.
            etabins=17
            phimin=0.
            phimax=58. #as defined in binMdtGlobal() c++ function
            phibins=58
            hits="x_mon_barrel,y_mon_barrel;"
            hits_noise="x_mon_barrel_noise,y_mon_barrel_noise;"
        if iregion=="EndCap":
            etamin=-6.
            etamax=6.
            etabins=12
            phimin=0.
            phimax=64. #as defined in binMdtGlobal() c++ function       
            phibins=64
            hits="x_mon_endcap,y_mon_endcap;"
            hits_noise="x_mon_endcap_noise,y_mon_endcap_noise;"
        mdtGroup.defineHistogram(hits+theTitle, type='TH2F', title=theTitle+";Eta;LayerPhi", path='Overview',
                                 xbins=etabins, xmin=etamin, xmax=etamax,  ybins=phibins, ymin=phimin, ymax=phimax, labels=thisLabelx+thisLabely)  

        mdtGroup.defineHistogram(hits_noise+theTitle_noise, type='TH2F', title=theTitle_noise+";Eta;LayerPhi", path='Overview',
                                 xbins=etabins, xmin=etamin, xmax=etamax,  ybins=phibins, ymin=phimin, ymax=phimax, labels=thisLabelx+thisLabely)  


    for ilayer in layers:
        if(ilayer=="Extra"):
            continue
        hits="x_mon_"+ilayer+",y_mon_"+ilayer+";"
        theTitle = "NumberOfHitsInMDT"+ilayer+"_ADCCut"
        thisLabelx,thisLabely=getMDTLabel(theTitle+"_labelx",theTitle+"_labely")
        if(ilayer=="Inner"):
            etabins=38
            phibins=36
        if(ilayer=="Middle"):
            etabins=24
            phibins=32
        if(ilayer=="Outer"):
            etabins=31
            phibins=32
        mdtGroup.defineHistogram(hits+theTitle, type='TH2F', title=theTitle+";Eta station;Phi station", path='Overview',
                                 xbins=etabins, xmin=0, xmax=etabins,  ybins=phibins, ymin=0, ymax=phibins, labels=thisLabelx+thisLabely)  
        


    phimax = 16
    for iregion in regions:
        MDT_regionGroup="MDT_regionGroup{0}".format(iregion)
        mdtRegionGroup=helper.addGroup(mdtMonAlg, MDT_regionGroup, "Muon/MuonRawDataMonitoring/MDT/MDT"+iregion)

        title_TDCADC_spectrum="Overall_TDCADC_spectrum_"+iregion
        title_TDCADC_spectrum_noiseBurst="Overall_TDCADC_spectrum_"+iregion+"_NoiseBurst"
        mdtRegionGroup.defineHistogram("tdc_mon,adc_mon;"+title_TDCADC_spectrum, title=title_TDCADC_spectrum+";[nsec];[adc counts]", type='TH2F',
                                             path='Overview', xbins=50, xmin=0., xmax=2000., ybins=20, ymin=0., ymax=400.)
        mdtRegionGroup.defineHistogram("tdc_mon_nb2,adc_mon_nb2;"+title_TDCADC_spectrum_noiseBurst, title=title_TDCADC_spectrum_noiseBurst+";[nsec];[adc counts]",
                                                       type='TH2F', path='Overview', xbins=50, xmin=0., xmax=2000., ybins=20, ymin=0., ymax=400.)
        title_ADCSummaryRegion="MDTADC_Summary_"+iregion
        mdtRegionGroup.defineHistogram("adc_mon;"+title_ADCSummaryRegion,  type='TH1F',
                                       title=title_ADCSummaryRegion+";[adc counts];Number of Entries",
                                       path='Overview',   xbins=100, xmin=0., xmax=400.)

        title_ADCSummaryRegion="MDTADC_Summary_ADCCUT_"+iregion
        mdtRegionGroup.defineHistogram("adc_mon_adccut;"+title_ADCSummaryRegion,  type='TH1F',
                                       title=title_ADCSummaryRegion+";[adc counts];Number of Entries",
                                       path='Overview',   xbins=100, xmin=0., xmax=400.)

        title_TDCSummaryRegion="MDTTDC_Summary_ADCCUT_"+iregion
        mdtRegionGroup.defineHistogram("tdc_mon_adccut;"+title_TDCSummaryRegion,  type='TH1F',
                                       title=title_TDCSummaryRegion+";[nsec];Number of Entries",
                                       path='Overview',   xbins=50, xmin=0., xmax=2000.)
        
        title_TDC_NoiseBursts = "MDTTDC_NoiseBursts_"+iregion
        mdtRegionGroup.defineHistogram("tdc_mon_nb1;"+title_TDC_NoiseBursts,  type='TH1F',
                                       title=title_TDC_NoiseBursts+";[nsec];Number of Entries",
                                       path='Overview',   xbins=50, xmin=0., xmax=2000.)

        title_TDC_NoiseBursts = "MDTTDC_NoiseBursts_ADCCUT_"+iregion
        mdtRegionGroup.defineHistogram("tdc_mon_nb3;"+title_TDC_NoiseBursts,  type='TH1F',
                                       title=title_TDC_NoiseBursts+";[nsec];Number of Entries",
                                       path='Overview',   xbins=50, xmin=0., xmax=2000.)

        title_ADC_NoiseBursts = "MDTADC_NoiseBursts_"+iregion
        mdtRegionGroup.defineHistogram("adc_mon_nb1;"+title_ADC_NoiseBursts,  type='TH1F',
                                       title=title_ADC_NoiseBursts+";[adc counts];Number of Entries",
                                       path='Overview',   xbins=20, xmin=0., xmax=400.)

        title_TDC_RPCtrig = "MDTTDC_Summary_ADCCut_"+iregion+"_RPC"
        mdtRegionGroup.defineHistogram("tdc_mon_rpc;"+title_TDC_RPCtrig,  type='TH1F',
                                       title=title_TDC_RPCtrig+";[nsec];Number of Entries",
                                       path='Overview',   xbins=50, xmin=0., xmax=2000.)

        title_TDC_TGCtrig = "MDTTDC_Summary_ADCCut_"+iregion+"_TGC"
        mdtRegionGroup.defineHistogram("tdc_mon_tgc;"+title_TDC_TGCtrig,  type='TH1F',
                                       title=title_TDC_TGCtrig+";[nsec];Number of Entries",
                                       path='Overview',   xbins=50, xmin=0., xmax=2000.)

        for ilayer in layers:
            title_MDTNumberOfHitsPerML="NumberOfHitsIn"+iregion+ilayer+"PerMultiLayer_ADCCut"
            thisLabelx,thisLabely=getMDTLabel(title_MDTNumberOfHitsPerML+"_labelx",title_MDTNumberOfHitsPerML+"_labely")
            phimaxML=phimax*2
            if (iregion=="BA" or iregion=="BC") and ilayer=="Extra": phimaxML=phimax/2
            if (iregion=="BA" or iregion=="BC") and ilayer=="Inner": phimaxML=phimax*2+4
            etamax=2 #extra layer
            if (iregion=="BA" or iregion=="BC") and ilayer=="Inner": etamax=8
            if (iregion=="BA" or iregion=="BC") and ilayer=="Middle": etamax=6
            if iregion=="BA" and  ilayer=="Outer": etamax=9
            if iregion=="BC" and  ilayer=="Outer": etamax=8
            if (iregion=="EA" or iregion=="EC") and ilayer=="Inner": etamax=5
            if (iregion=="EA" or iregion=="EC") and ilayer=="Middle": etamax=5
            if (iregion=="EA" or iregion=="EC") and ilayer=="Outer": etamax=6
            var="x_mon_"+iregion+"_"+ilayer+",y_mon_"+iregion+"_"+ilayer+";"
            mdtRegionGroup.defineHistogram(var+title_MDTNumberOfHitsPerML, title=title_MDTNumberOfHitsPerML+";[Eta];[Phi - Multilayer]", type='TH2F',
                                           path='Overview',  xbins=etamax, xmin=0, xmax=etamax, ybins=phimaxML, ymin=0., ymax=phimaxML, labels=thisLabelx+thisLabely)

            for phi in range(1, phimax+1):
                if not ((iregion=="BA" or iregion=="BC") and ilayer=="Extra" and (phi%2!=0)):
                    var="stEta_"+iregion+"_"+ilayer+"_phi"+str(phi)+";"
                    title_MDTHitSummary="MDTHits_ADCCut_"+iregion+"_"+ilayer+"_StPhi"+str(phi)
                    max=7
                    if ilayer=="Inner" and (iregion=="BA" or iregion=="BC"):
                        max=11

                    mdtRegionGroup.defineHistogram(var+title_MDTHitSummary, title=title_MDTHitSummary+";StationEta;Number of Entries", type='TH1F',
                                                   path='Overview/Hits',  xbins=max-1, xmin=1., xmax=max)


    #mdtoccvslb_summaryPerSector->GetYaxis()->SetBinLabel(1,"BA");
    #mdtoccvslb_summaryPerSector->GetYaxis()->SetBinLabel(17,"BC");
    #mdtoccvslb_summaryPerSector->GetYaxis()->SetBinLabel(33,"EA");
    #mdtoccvslb_summaryPerSector->GetYaxis()->SetBinLabel(49,"EC");

    mdtGroup.defineHistogram('lb_mon,sector;OccupancyPerSectorVsLB', type='TH2F',
                            title='OccupancyPerSectorVsLB;LB;[Phi]',
                            path='Overview',  xbins=834,xmin=1.,xmax=2502., ybins=64, ymin=0., ymax=64.
                            )
    

    #histograms per chambers
    mdtPerChamberBAGroup=helper.addGroup(mdtMonAlg,"MdtMonPerChamberBA", "Muon/MuonRawDataMonitoring/MDT/MDTBA/Chambers")    
    for ch in mdtBA:
        #print ch
        title_mdttdc= ch+"_MDT_Station_TDC"
        var="tdc_perch_"+ch+";"+title_mdttdc
        mdtPerChamberBAGroup.defineHistogram(var, title=title_mdttdc+";[nsec];Number of Entries", 
                                             type='TH1F', path=ch, xbins=100, xmin=0., xmax=2000.)
        title_mdttdc_ml1_adccut=ch+"_MDT_Station_TDC_ML1_ADCCut"
        var="tdc_perch_"+ch+";"+title_mdttdc_ml1_adccut
        mdtPerChamberBAGroup.defineHistogram(var, title=title_mdttdc_ml1_adccut+";[nsec];Number of Entries",
                                             cutmask='ml1_adccut',
                                             type='TH1F', path=ch, xbins=100, xmin=0., xmax=2000.)
        title_mdttdc_ml2_adccut=ch+"_MDT_Station_TDC_ML2_ADCCut"
        var="tdc_perch_"+ch+";"+title_mdttdc_ml2_adccut
        mdtPerChamberBAGroup.defineHistogram(var, title=title_mdttdc_ml2_adccut+";[nsec];Number of Entries",
                                             cutmask='ml2_adccut',
                                           type='TH1F', path=ch, xbins=100, xmin=0., xmax=2000.)
        title_mdtadc= ch+"_MDT_Station_ADC"
        var="adc_perch_"+ch+";"+title_mdtadc
        mdtPerChamberBAGroup.defineHistogram(var,  type='TH1F',
                                             title=title_mdtadc+";[adc counts];Number of Entries",
                                             path=ch,   xbins=100, xmin=0., xmax=400.)
        
        title_mdtdcadc= ch+"_MDT_Station_TDCADC"
        var="tdc_perch_"+ch+",adc_perch_"+ch+";"+title_mdtdcadc
        mdtPerChamberBAGroup.defineHistogram(var, type='TH2F',
                                             title=title_mdtdcadc+';[nsec];[adc counts]',
                                             path=ch,   xbins=50, xmin=0., xmax=2000., ybins=20, ymin=0., ymax=400.)
        

        title_mdtlayer= ch+"_MDT_Station_LAYER_ADCCut"
        var="layer_perch_"+ch+";"+title_mdtlayer
        mdtPerChamberBAGroup.defineHistogram(var,  type='TH1F',
                                             cutmask='adccut_nonoise',
                                             title=title_mdtlayer+";layerID;Number of Entries",
                                             path=ch,   xbins=10, xmin=0., xmax=10.)
        
        title_mdttube= ch+"_MDT_Station_TUBE_ADCCut"
        var="tube_perch_"+ch+";"+title_mdttube
        binmax=tubeMax[ch]
        mdtPerChamberBAGroup.defineHistogram(var,  type='TH1F',
                                             cutmask='adccut',
                                             title=title_mdttube+";tubeID;Number of Entries",
                                             path=ch,   xbins=binmax, xmin=0., xmax=binmax)
        #to do
        title_mdtmezz= ch+"_MDT_Station_MEZZ_ADCCut"
        mdtPerChamberBAGroup.defineHistogram("mezz_perch;"+title_mdtmezz,  type='TH1F',
                                             title=title_mdtmezz+";mezzID;Number of Entries",
                                             path=ch,   xbins=20, xmin=0., xmax=20.)

    mdtPerChamberBCGroup=helper.addGroup(mdtMonAlg, "MdtMonPerChamberBC", "Muon/MuonRawDataMonitoring/MDT/MDTBC/Chambers")    
    for ch in mdtBC:
        title_mdttdc=ch+"_MDT_Station_TDC"
        #print ch
        var="tdc_perch_"+ch+";"+title_mdttdc
        mdtPerChamberBCGroup.defineHistogram(var, title=title_mdttdc+";[nsec];Number of Entries",
                                             type='TH1F', path=ch, xbins=100, xmin=0., xmax=2000.)
        title_mdttdc_ml1_adccut=ch+"_MDT_Station_TDC_ML1_ADCCut"
        var="tdc_perch_"+ch+";"+title_mdttdc_ml1_adccut
        mdtPerChamberBCGroup.defineHistogram(var, title=title_mdttdc_ml1_adccut+";[nsec];Number of Entries",
                                             cutmask='ml1_adccut',
                                             type='TH1F', path=ch, xbins=100, xmin=0., xmax=2000.)
        title_mdttdc_ml2_adccut=ch+"_MDT_Station_TDC_ML2_ADCCut"
        var="tdc_perch_"+ch+";"+title_mdttdc_ml2_adccut
        mdtPerChamberBCGroup.defineHistogram(var, title=title_mdttdc_ml2_adccut+";[nsec];Number of Entries",
                                             cutmask='ml2_adccut',
                                             type='TH1F', path=ch, xbins=100, xmin=0., xmax=2000.)
        title_mdtadc= ch+"_MDT_Station_ADC"
        var="adc_perch_"+ch+";"+title_mdtadc
        mdtPerChamberBCGroup.defineHistogram(var, title=title_mdtadc+";[adc counts];Number of Entries",
                                             path=ch,   xbins=100, xmin=0., xmax=400.)
        title_mdtdcadc= ch+"_MDT_Station_TDCADC"
        var="tdc_perch_"+ch+",adc_perch_"+ch+";"+title_mdtdcadc
        mdtPerChamberBCGroup.defineHistogram(var,    type='TH2F',
                                             title=title_mdtdcadc+';[nsec];[adc counts]',
                                             path=ch,   xbins=50, xmin=0., xmax=2000., ybins=20, ymin=0., ymax=400.)
        title_mdtlayer= ch+"_MDT_Station_LAYER_DCCut"
        var="layer_perch_"+ch+";"+title_mdtlayer
        mdtPerChamberBCGroup.defineHistogram(var,  type='TH1F',
                                             cutmask='adccut_nonoise',
                                             title=title_mdtlayer+";layerID;Number of Entries",
                                             path=ch,   xbins=10, xmin=0., xmax=10.)
        title_mdttube= ch+"_MDT_Station_TUBE_ADCCut"
        var="tube_perch_"+ch+";"+title_mdttube
        binmax=tubeMax[ch]
        mdtPerChamberBCGroup.defineHistogram(var,  type='TH1F',
                                             cutmask='adccut',
                                             title=title_mdttube+";tubeID;Number of Entries",
                                             path=ch,   xbins=binmax, xmin=0., xmax=binmax)

        #to-do
        title_mdtmezz= ch+"_MDT_Station_MEZZ_ADCCut"
        mdtPerChamberBCGroup.defineHistogram("mezz_perch;"+title_mdtmezz,  type='TH1F',
                                             title=title_mdtmezz+";mezzID;Number of Entries",
                                             path=ch,   xbins=20, xmin=0., xmax=20.)

    mdtPerChamberEAGroup=helper.addGroup(mdtMonAlg, "MdtMonPerChamberEA", "Muon/MuonRawDataMonitoring/MDT/MDTEA/Chambers")           
    for ch in mdtEA:
        title_mdttdc=ch+"_MDT_Station_TDC"
        #print ch
        var="tdc_perch_"+ch+";"+title_mdttdc
        mdtPerChamberEAGroup.defineHistogram(var, title=title_mdttdc+";[nsec];Number of Entries", 
                                             type='TH1F', path=ch, xbins=100, xmin=0., xmax=2000.)
        title_mdttdc_ml1_adccut=ch+"_MDT_Station_TDC_ML1_ADCCut"
        var="tdc_perch_"+ch+";"+title_mdttdc_ml1_adccut
        mdtPerChamberEAGroup.defineHistogram(var, title=title_mdttdc_ml1_adccut+";[nsec];Number of Entries",
                                             cutmask='ml1_adccut',
                                            type='TH1F', path=ch, xbins=100, xmin=0., xmax=2000.)
        title_mdttdc_ml2_adccut=ch+"_MDT_Station_TDC_ML2_ADCCut"
        var="tdc_perch_"+ch+";"+title_mdttdc_ml2_adccut
        mdtPerChamberEAGroup.defineHistogram(var, title=title_mdttdc_ml2_adccut+";[nsec];Number of Entries",
                                             cutmask='ml2_adccut',
                                             type='TH1F', path=ch, xbins=100, xmin=0., xmax=2000.)
        title_mdtadc= ch+"_MDT_Station_ADC"
        var="adc_perch_"+ch+";"+title_mdtadc
        mdtPerChamberEAGroup.defineHistogram(var,  type='TH1F',
                                             title=title_mdtadc+";[adc counts];Number of Entries",
                                             path=ch,   xbins=100, xmin=0., xmax=400.)
        title_mdtdcadc= ch+"_MDT_Station_TDCADC"
        var="tdc_perch_"+ch+",adc_perch_"+ch+";"+title_mdtdcadc
        mdtPerChamberEAGroup.defineHistogram(var,    type='TH2F',
                                             title=title_mdtdcadc+";[nsec];[adc counts]",
                                             path=ch,   xbins=50, xmin=0., xmax=2000., ybins=20, ymin=0., ymax=400.)
        
        title_mdtlayer= ch+"_MDT_Station_LAYER_DCCut"
        var="layer_perch_"+ch+";"+title_mdtlayer
        mdtPerChamberEAGroup.defineHistogram(var,  type='TH1F',
                                             cutmask='adccut_nonoise',
                                             title=title_mdtlayer+";layerID;Number of Entries",
                                             path=ch,   xbins=10, xmin=0., xmax=10.)

        title_mdttube= ch+"_MDT_Station_TUBE_ADCCut"
        var="tube_perch_"+ch+";"+title_mdttube
        binmax=tubeMax[ch]
        mdtPerChamberEAGroup.defineHistogram(var,  type='TH1F',
                                             cutmask='adccut',
                                             title=title_mdttube+";tubeID;Number of Entries",
                                             path=ch,   xbins=binmax, xmin=0., xmax=binmax)


        #to-do
        title_mdtmezz= ch+"_MDT_Station_MEZZ_ADCCut"
        mdtPerChamberEAGroup.defineHistogram("mezz_perch;"+title_mdtmezz,  type='TH1F',
                                             title=title_mdtmezz+";mezzID;Number of Entries",
                                             path=ch,   xbins=20, xmin=0., xmax=20.)
    
    mdtPerChamberECGroup=helper.addGroup(mdtMonAlg, "MdtMonPerChamberEC", "Muon/MuonRawDataMonitoring/MDT/MDTEC/Chambers")    
    for ch in mdtEC:
        #print ch
        title_mdttdc=ch+"_MDT_Station_TDC"
        var="tdc_perch_"+ch+";"+title_mdttdc
        mdtPerChamberECGroup.defineHistogram(var, title=title_mdttdc+";[nsec];Number of Entries", 
                                             type='TH1F', path=ch, xbins=100, xmin=0., xmax=2000.)
        title_mdttdc_ml1_adccut=ch+"_MDT_Station_TDC_ML1_ADCCut"
        var="tdc_perch_"+ch+";"+title_mdttdc_ml1_adccut
        mdtPerChamberECGroup.defineHistogram(var, title=title_mdttdc_ml1_adccut+";[nsec];Number of Entries",
                                             cutmask='ml1_adccut',
                                             type='TH1F', path=ch, xbins=100, xmin=0., xmax=2000.)
        title_mdttdc_ml2_adccut=ch+"_MDT_Station_TDC_ML2_ADCCut"
        var="tdc_perch_"+ch+";"+title_mdttdc_ml2_adccut
        mdtPerChamberECGroup.defineHistogram(var, title=title_mdttdc_ml2_adccut+";[nsec];Number of Entries",
                                             cutmask='ml2_adccut',
                                             type='TH1F', path=ch, xbins=100, xmin=0., xmax=2000.)
        title_mdtadc= ch+"_MDT_Station_ADC"
        var="adc_perch_"+ch+";"+title_mdtadc
        mdtPerChamberECGroup.defineHistogram(var,  type='TH1F',
                                             title=title_mdtadc+";[adc counts];Number of Entries",
                                             path=ch,   xbins=100, xmin=0., xmax=400.)
        title_mdtdcadc= ch+"_MDT_Station_TDCADC"
        var="tdc_perch_"+ch+",adc_perch_"+ch+";"+title_mdtdcadc
        mdtPerChamberECGroup.defineHistogram(var,    type='TH2F',
                                             title=title_mdtdcadc+';[nsec];[adc counts]',
                                             path=ch,   xbins=50, xmin=0., xmax=2000., ybins=20, ymin=0., ymax=400.)
        #to do
        title_mdtlayer= ch+"_MDT_Station_LAYER_ADCCut"
        var="layer_perch_"+ch+";"+title_mdtlayer
        mdtPerChamberECGroup.defineHistogram(var,  type='TH1F',
                                             cutmask='adccut_nonoise',
                                             title=title_mdtlayer+";layerID;Number of Entries",
                                             path=ch,   xbins=10, xmin=0., xmax=10.)
        title_mdttube= ch+"_MDT_Station_TUBE_ADCCut"
        var="tube_perch_"+ch+";"+title_mdttube
        binmax=tubeMax[ch]
        mdtPerChamberECGroup.defineHistogram(var,  type='TH1F',
                                             cutmask='adccut',
                                             title=title_mdttube+";tubeID;Number of Entries",
                                             path=ch,   xbins=binmax, xmin=0., xmax=binmax)
        #to-do
        title_mdtmezz= ch+"_MDT_Station_MEZZ_ADCCut"
        mdtPerChamberECGroup.defineHistogram("mezz_perch;"+title_mdtmezz,  type='TH1F',
                                             title=title_mdtmezz+";mezzID;Number of Entries",
                                             path=ch,   xbins=20, xmin=0., xmax=20.)



    
        
    ### STEP 6 ###
    # Finalize. The return value should be a tuple of the ComponentAccumulator
    # and the sequence containing the created algorithms. If we haven't called
    # any configuration other than the AthMonitorCfgHelper here, then we can 
    # just return directly (and not create "result" above)
    #####return helper.result()


    
    # # Otherwise, merge with result object and return
    #acc, seq = helper.result()
    acc = helper.result()
    result.merge(acc)
    return result


if __name__=='__main__':
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    #from AthenaCommon.Logging import log
    #from AthenaCommon.Constants import DEBUG
    #log.setLevel(DEBUG)    

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    
    #nightly = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/'
    #file = 'data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1'
    #file = 'data16_13TeV.00297447.physics_Main.daq.RAW._lb0555._SFO-1._0001.data'
    #ConfigFlags.Input.Files = [nightly+file]
    #ConfigFlags.Input.Files = ['/afs/cern.ch/user/b/bigliett/work/DQ/group.det-muon.DiMuon10_100GeV.ESD.rel21_3_8.v001_EXT1/group.det-muon.16399844.EXT1._000118.ESD.pool.root',
    #                           '/afs/cern.ch/user/b/bigliett/work/DQ/group.det-muon.DiMuon10_100GeV.ESD.rel21_3_8.v001_EXT1/group.det-muon.16399844.EXT1._000120.ESD.pool.root',
    #                           '/afs/cern.ch/user/b/bigliett/work/DQ/group.det-muon.DiMuon10_100GeV.ESD.rel21_3_8.v001_EXT1/group.det-muon.16399844.EXT1._000098.ESD.pool.root',
    #                           '/afs/cern.ch/user/b/bigliett/work/DQ/group.det-muon.DiMuon10_100GeV.ESD.rel21_3_8.v001_EXT1/group.det-muon.16399844.EXT1._000143.ESD.pool.root']
    #ConfigFlags.Input.Files = ['/afs/cern.ch/user/b/bigliett/work/DQ/data17_13TeV.00333192.physics_Main.daq.RAW._lb0424._SFO-1._0001.data']
    ConfigFlags.Input.Files = ['/afs/cern.ch/user/b/bigliett/work/DQ/esd.root']
    #ConfigFlags.Input.Files = ['/afs/cern.ch/user/e/ebergeas/work/public/triggermonitoring/nigtly_2019-08-24T2130/AOD.pool.root']

    #from AthenaConfiguration.TestDefaults import defaultTestFiles
    #ConfigFlags.Input.Files = defaultTestFiles.ESD


    #ConfigFlags.Input.isMC = False
    #ConfigFlags.Input.isMC = True
    ConfigFlags.Output.HISTFileName = 'MdtMonitorOutput.root'
    #ConfigFlags.Output.AODFileName='aod.root'

    #mik
    #ConfigFlags.IOVDb.GlobalTag = "'CONDBR2-BLKPA-2017-08"
    ConfigFlags.GeoModel.AtlasVersion = "ATLAS-R2-2016-01-00-01"
                                          
    #ConfigFlags.GeoModel.AtlasVersion = "ATLAS-R3-2021-00-00-00"

    #ConfigFlags.GeoModel.Run = "RUN3"
    ConfigFlags.Muon.doCSCs = False
    ConfigFlags.Muon.doMicromegas = False
    ConfigFlags.DQ.useTrigger=False


    ConfigFlags.lock()
    ConfigFlags.dump()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesSerialCfg()

    cfg.merge(PoolReadCfg(ConfigFlags))
    

    #mdtMonitorAcc,mdtMonitorAlg =  MdtMonitoringConfig(ConfigFlags)
    mdtMonitorAcc =  MdtMonitoringConfig(ConfigFlags)
    
    #mdtMonitorAcc.OutputLevel=DEBUG
    
    cfg.merge(mdtMonitorAcc)

    #cfg.printConfig(withDetails=True, summariseProps = True)
    
    cfg.run(20)

