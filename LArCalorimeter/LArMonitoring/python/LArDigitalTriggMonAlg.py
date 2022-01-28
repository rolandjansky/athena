#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
from ROOT import TMath

def LArDigitalTriggMonConfigOld(inputFlags, topSequence):
    from AthenaMonitoring.AthMonitorCfgHelper import AthMonitorCfgHelperOld
    from LArMonitoring.LArMonitoringConf import LArDigitalTriggMonAlg

    helper = AthMonitorCfgHelperOld(inputFlags, 'LArDigitalTriggMonAlgCfg')
    LArDigitalTriggMonConfigCore(helper, LArDigitalTriggMonAlg,inputFlags)
    return helper.result()

def LArDigitalTriggMonConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''
    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    
    from AthenaMonitoring.AthMonitorCfgHelper import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'LArDigitalTriggMonAlgCfg')

    from AthenaConfiguration.ComponentFactory import CompFactory
    return LArDigitalTriggMonConfigCore(helper, CompFactory.LArDigitalTriggMonAlg,inputFlags)

def LArDigitalTriggMonConfigCore(helper, algoinstance,inputFlags):

    from LArMonitoring.GlobalVariables import lArDQGlobals
    
    from AthenaCommon.AlgSequence import AthSequencer
    #from IOVDbSvc.CondDB import conddb

    #get SC onl-offl mapping from DB    
    from AthenaConfiguration.ComponentFactory import isRun3Cfg
    folder="/LAR/Identifier/OnOffIdMap_SC"
    persClass="AthenaAttributeList"
    dbString="<db>COOLONL_LAR/CONDBR2</db>"
    if isRun3Cfg():
        from AthenaConfiguration.ComponentFactory import CompFactory
        try:
            condLoader=helper.resobj.getCondAlgo("CondInputLoader")
        except Exception:
            from IOVDbSvc.IOVDbSvcConfig import IOVDbSvcCfg
            helper.resobj.merge(IOVDbSvcCfg(inputFlags))
            condLoader=helper.resobj.getCondAlgo("CondInputLoader")
            iovDbSvc=helper.resobj.getService("IOVDbSvc")
            helper.resobj.addCondAlgo(CompFactory.LArOnOffMappingAlg("LArOnOffMappingAlgSC",ReadKey=folder, WriteKey="LArOnOffIdMapSC", isSuperCell=True)) 
    else:
        from LArRecUtils.LArRecUtilsConf import LArOnOffMappingAlg#, LArFebRodMappingAlg, LArCalibLineMappingAlg
        from AthenaCommon import CfgGetter
        iovDbSvc=CfgGetter.getService("IOVDbSvc")
        #from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")
        if hasattr(condSeq,"LArOnOffMappingAlgSC") :
            return #Already there....
        #folder="/LAR/IdentifierOfl/OnOffIdMap_SC"
        #conddb.addFolder("","<db>sqlite://;schema=/det/lar/project/databases/OnOffIdMap_SC.db;dbname=OFLP200</db>"+folder,className="AthenaAttributeList",forceMC=True)
        #conddb.addOverride(folder,"LARIdentifierOflOnOffIdMap_SC-000")
        condLoader=condSeq.CondInputLoader
        condSeq+=LArOnOffMappingAlg("LArOnOffMappingAlgSC",ReadKey=folder, WriteKey="LArOnOffIdMapSC", isSuperCell=True)
    iovDbSvc.Folders.append(folder+dbString)
    condLoader.Load.append((persClass,folder))

    larDigitalTriggMonAlg = helper.addAlgorithm(algoinstance,'larDigitalTriggMonAlg')

    SCGroupName="SC"
    larDigitalTriggMonAlg.SCMonGroup=SCGroupName
    # uncomment if needed:
    #larDigitalTriggMonAlg.FileKey="CombinedMonitoring"

    SCGroup = helper.addGroup(
        larDigitalTriggMonAlg,
        SCGroupName,
        '/LArDigitalTrigger/',
        'run'
    )

    sc_hist_path='/'

    LatomeDetBinMapping = dict([
        ("0x48",{"Subdet":"FCALC","Bin":1}),
        ("0x4c",{"Subdet":"EMEC/HECC","Bin":3}),
        ("0x44",{"Subdet":"EMECC","Bin":11}),
        ("0x4a",{"Subdet":"EMB/EMECC","Bin":27}),
        ("0x42",{"Subdet":"EMBC","Bin":43}),
        ("0x41",{"Subdet":"EMBA","Bin":59}),
        ("0x49",{"Subdet":"EMB/EMECA","Bin":75}),
        ("0x43",{"Subdet":"EMECA","Bin":91}),
        ("0x4b",{"Subdet":"EMEC/HECA","Bin":107}),
        ("0x47",{"Subdet":"FCALA","Bin":115})
    ])    
    NLatomeBins=117
    BinLabel=[]
    phi=0
    for bb in range (0,NLatomeBins):
        Label=""
        for detID in LatomeDetBinMapping:
            if bb==(LatomeDetBinMapping[detID]["Bin"]-1):
                Label=LatomeDetBinMapping[detID]["Subdet"]
                phi=1
                break
        BinLabel+=[Label+str(phi)]
        phi+=1


    SCGroup.defineHistogram('Mmaxpos,Mpartition;Partition_maxSamplePosition', 
                                  title='Partition vs. position of max sample',
                                  type='TH2F',
                                  path=sc_hist_path,
                                  xbins=32,xmin=0.5,xmax=32.5,
                                  ybins=lArDQGlobals.N_Partitions, ymin=-0.5, ymax=lArDQGlobals.N_Partitions-0.5,
                                  xlabels = [str(x) for x in range(1,33)],      
                                  ylabels=lArDQGlobals.Partitions)

    SCGroup.defineHistogram('Msampos,MADC;ADC_samplePosition',
                                  title='ADC (zoom) vs sample position',
                                  type='TH2F',
                                  path=sc_hist_path,
                                  xbins=32,xmin=0.5,xmax=32.5,
                                  xlabels = [str(x) for x in range(1,33)],
                                  ybins=750, ymin=0, ymax=1300) #start from 0 otherwise miss endcap pedestals

    SCGroup.defineHistogram('Msampos,MADC;ADCFullRange_samplePosition', 
                                  title='ADC vs sample position',
                                  type='TH2F',
                                  path=sc_hist_path,
                                  xbins=32,xmin=0.5,xmax=32.5,
                                  xlabels = [str(x) for x in range(1,33)],
                                  ybins=500, ymin=0, ymax=5000) #raw ADC is 12 bit


    SCGroup.defineHistogram('MlatomeSourceIdBIN,Mmaxpos;MaxSamplePosition_LATOME', 
                                  title='Position of max sample vs. LATOME name',
                                  type='TH2F',
                                  path=sc_hist_path,
                                  xbins=NLatomeBins,xmin=1,xmax=NLatomeBins+1,
                                  ybins=32,ymin=0.5,ymax=32.5,
                                  xlabels=BinLabel,
                                  ylabels = [str(x) for x in range(1,33)])      
                            

    SCGroup.defineHistogram('MlatomeSourceIdBIN,MADC;ADC_LATOME', 
                                  title='ADC (zoom) vs LATOME name',
                                  type='TH2F',
                                  path=sc_hist_path,
                                  xbins=NLatomeBins,xmin=1,xmax=NLatomeBins+1,
                                  ybins=750, ymin=0, ymax=1300,
                                  xlabels=BinLabel)

    SCGroup.defineHistogram('MlatomeSourceIdBIN,MADC;ADCFullRange_LATOME', 
                                  title='ADC vs LATOME name',
                                  type='TH2F',
                                  path=sc_hist_path,
                                  xbins=NLatomeBins,xmin=1,xmax=NLatomeBins+1,
                                  ybins=500, ymin=0, ymax=5000, #raw ADC is 12 bit
                                  xlabels=BinLabel)


    #have to make plots per sampling and per subdetector, as in LArCoverageAlg, also update GlobalVariables.py
    SCGroup.defineHistogram('MSCeta,MSCphi;Coverage_eta_phi', 
                                  title='SC coverage: #phi vs #eta;#eta;#phi',
                                  type='TH2F',
                                  path=sc_hist_path,
                                  xbins=400, xmin=-5, xmax=5, #smallest eta bin size 0.025
                                  ybins=64, ymin=-TMath.Pi(), ymax=TMath.Pi()) #at most 64 phi bins of 0.1 size

    #NEW    
    #for part in LArDigitalTriggMonAlg.LayerNames:
    for part in ["EMBPA", "EMBPC", "EMB1A", "EMB1C", "EMB2A", "EMB2C", "EMB3A", "EMB3C", "HEC0A", "HEC0C", "HEC1A", "HEC1C", "HEC2A", "HEC2C", "HEC3A", "HEC3C", "EMECPA", "EMECPC", "EMEC1A", "EMEC1C", "EMEC2A", "EMEC2C", "EMEC3A", "EMEC3C", "FCAL1A", "FCAL1C", "FCAL2A", "FCAL2C", "FCAL3A", "FCAL3C"]:
        Part = part[:-2]
        if Part == "FCAL": 
            Part = "FCal"
        Side = part[-1]
        Sampling = part[-2]
        if Sampling == "P": 
            Sampling = "0"
        
        
        SCGroup.defineHistogram('superCellEta_'+part+',superCellPhi_'+part+';CoverageEtaPhi_'+part,
                                  title='SC coverage '+part+': #phi vs #eta;#eta;#phi',
                                  type='TH2F',
                                  path=sc_hist_path+'CoveragePerPartition/NoCut',
                                  xbins=lArDQGlobals.Cell_Variables["etaRange"][Part][Side][Sampling],
                                  ybins=lArDQGlobals.Cell_Variables["phiRange"][Part][Side][Sampling])    
        
        SCGroup.defineHistogram('eta_digi_'+part+',phi_digi_'+part+';CoverageEtaPhi_CutADC_'+part,
                                  title='SC coverage '+part+': #phi vs #eta with adc[max]-adc[0]>15;#eta;#phi',
                                  type='TH2F',
                                  path=sc_hist_path+'CoveragePerPartition/CutADC',
                                  xbins=lArDQGlobals.Cell_Variables["etaRange"][Part][Side][Sampling],
                                  ybins=lArDQGlobals.Cell_Variables["phiRange"][Part][Side][Sampling])


                    
        # --> IT SEEMS TH3 IS NOT IMPLEMENTED YET...
        #SCGroup.defineHistogram('superCellEta_Et10_'+part+',superCellPhi_Et10_'+part+',superCellEt_Et10_'+part+';CoverageEtaPhi_Et10_'+part,
        #                          title='SC Energy $E_T>10$ coverage'+part+': #phi vs #eta;#eta;#phi;$E_T$',
        #                          type='TH3F',
        #                          path=sc_hist_path,
        #                          xbins=lArDQGlobals.Cell_Variables["etaRange"][Part][Side][Sampling],
        #                          ybins=lArDQGlobals.Cell_Variables["phiRange"][Part][Side][Sampling])
                        
        #SCGroup.defineHistogram('superCellEta_'+part+',superCellPhi_'+part+',superCellEt_'+part+';CoverageEtaPhi_'+part,
        #                          title='SC Energy coverage'+part+': #phi vs #eta;#eta;#phi;$E_T$',
        #                          type='TH3F',
        #                          path=sc_hist_path,
        #                          xbins=lArDQGlobals.Cell_Variables["etaRange"][Part][Side][Sampling],
        #                          ybins=lArDQGlobals.Cell_Variables["phiRange"][Part][Side][Sampling])

        SCGroup.defineHistogram('superCellEta_EtCut_'+part+',superCellPhi_EtCut_'+part+';CoverageEtaPhi_EtCut_'+part,
                                  title='SC coverage E_T>1GeV '+part+': #phi vs #eta;#eta;#phi',
                                  type='TH2F',
                                  path=sc_hist_path+'CoveragePerPartition/CutET',
                                  xbins=lArDQGlobals.Cell_Variables["etaRange"][Part][Side][Sampling],
                                  ybins=lArDQGlobals.Cell_Variables["phiRange"][Part][Side][Sampling])


    for layer in ["0","1","2","3"]: 
        SCGroup.defineHistogram('MlatomeSourceIdBIN_'+layer+',Mmaxpos_'+layer+';MaxSamplePosition_LATOME_Layer'+layer,
                                  title='Position of max sample vs. LATOME in layer '+layer,
                                  type='TH2F',
                                  path=sc_hist_path,
                                  xbins=NLatomeBins,xmin=1,xmax=NLatomeBins+1,
                                  ybins=32,ymin=0.5,ymax=32.5,
                                  xlabels=BinLabel,
                                  ylabels = [str(x) for x in range(1,33)])  
                                
        SCGroup.defineHistogram('MlatomeSourceIdBIN_'+layer+'_cut,Mmaxpos_'+layer+'_cut'+';MaxSamplePosition_LATOME_Layer'+layer+'_cut',
                                  title='Position of max sample vs. LATOME in layer '+layer+' with adc[max]-adc[0]>15',
                                  type='TH2F',
                                  path=sc_hist_path,
                                  xbins=NLatomeBins,xmin=1,xmax=NLatomeBins+1,
                                  ybins=32,ymin=0.5,ymax=32.5,
                                  xlabels=BinLabel,
                                  ylabels = [str(x) for x in range(1,33)])  
                                

    SCGroup.defineHistogram('MlatomeSourceIdBIN,MSCChannel;Coverage_SCchan_LATOME', 
                                  title='SC coverage: channel vs LATOME name',
                                  type='TH2F',
                                  path=sc_hist_path,
                                  xbins=NLatomeBins,xmin=1,xmax=NLatomeBins+1,      
                                  ybins=360,ymin=1,ymax=360,
                                  xlabels=BinLabel)


    SCGroup.defineHistogram('MlatomeSourceIdBIN,MSCeT;SCeT_LATOME', 
                                  title='SC eT [MeV] vs LATOME name',
                                  type='TH2F',
                                  path=sc_hist_path,
                                  xbins=NLatomeBins,xmin=1,xmax=NLatomeBins+1,      
                                  ybins=400, ymin=0, ymax=400000,
                                  xlabels=BinLabel)


    SCGroup.defineHistogram('MlatomeSourceIdBIN,MSCsatur;SCsaturation_LATOME', 
                                  title='SC saturation vs LATOME name',
                                  type='TH2F',
                                  path=sc_hist_path,
                                  xbins=NLatomeBins,xmin=1,xmax=NLatomeBins+1,      
                                  ybins=3, ymin=-0.5, ymax=2.5,
                                  xlabels=BinLabel,
                                  ylabels=["0","1","2"])

    SCGroup.defineHistogram('MSCsatur;SCsaturation', 
                                  title='SC saturation',
                                  type='TH1F',
                                  path=sc_hist_path,
                                  xbins=3, xmin=0, xmax=3)

    SCGroup.defineHistogram('MSCeT;SCeT', 
                                  title='SC eT [MeV]',
                                  type='TH1F',
                                  path=sc_hist_path,
                                  xbins=400, xmin=0, xmax=400000)


    SCGroup.defineHistogram('MNsamples;SCNsamples', 
                                  title='Nsamples',
                                  type='TH1F',
                                  path=sc_hist_path,
                                  xbins=40,xmin=1,xmax=40)

    SCGroup.defineHistogram('MSCChannel;SCchannel', 
                                  title='SC Channel number',
                                  type='TH1F',
                                  path=sc_hist_path,                                  
                                  xbins=360,xmin=1,xmax=360)

    SCGroup.defineHistogram('MlatomeSourceId;LATOMEsourceID', 
                                  title='LATOME sourceID',
                                  type='TH1F',
                                  path=sc_hist_path,                                  
                                  xbins=1000,xmin=4000000,xmax=6000000)



    return helper.result()


if __name__=='__main__':

   #import os
   from AthenaConfiguration.AllConfigFlags import ConfigFlags
   from AthenaCommon.Logging import log
   from AthenaCommon.Constants import DEBUG, ERROR #WARNING,
   from AthenaCommon.Configurable import Configurable
   Configurable.configurableRun3Behavior=1
   log.setLevel(DEBUG)
   
   #from LArMonitoring.LArMonitoringConf import  LArSuperCellMonAlg
   from LArMonitoring.LArMonConfigFlags import createLArMonConfigFlags
   createLArMonConfigFlags()

   ConfigFlags.Input.Files = ["/eos/atlas/atlastier0/rucio/data21_idcomm/physics_CosmicCalo/00401410/data21_idcomm.00401410.physics_CosmicCalo.merge.RAW/data21_idcomm.00401410.physics_CosmicCalo.merge.RAW._lb0722._SFO-ALL._0001.1"]
   
   ConfigFlags.Output.HISTFileName = 'LArDigitalTriggMonOutput.root'
   ConfigFlags.DQ.enableLumiAccess = False
   ConfigFlags.DQ.useTrigger = False
   ConfigFlags.Beam.Type = 'collisions'
   ConfigFlags.lock()

   # in case of tier0 workflow:
   from CaloRec.CaloRecoConfig import CaloRecoCfg
   cfg=CaloRecoCfg(ConfigFlags)

   #from AthenaCommon.AppMgr import (ServiceMgr as svcMgr,ToolSvc)
   from LArByteStream.LArRawSCDataReadingConfig import LArRawSCDataReadingCfg
   SCData_acc =  LArRawSCDataReadingCfg(ConfigFlags)
   SCData_acc.OutputLevel=DEBUG

   cfg.merge(SCData_acc)


   aff_acc = LArDigitalTriggMonConfig(ConfigFlags)
   cfg.merge(aff_acc)
   
   cfg.getCondAlgo("LArHVCondAlg").OutputLevel=ERROR
   #cfg.getEventAlgo("LArHVCondAlg").OutputLevel=ERROR

   ConfigFlags.dump()
   f=open("LArDigitalTriggMon.pkl","wb")
   cfg.store(f)
   f.close()


