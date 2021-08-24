#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
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


    SCGroup.defineHistogram('Mmaxpos,Mpartition;h_maxposPart', 
                                  title='Partition vs. position of max sample',
                                  type='TH2F',
                                  path=sc_hist_path,
                                  xbins=32,xmin=0.5,xmax=32.5,
                                  ybins=lArDQGlobals.N_Partitions, ymin=-0.5, ymax=lArDQGlobals.N_Partitions-0.5,
                                  xlabels=["1","2","3","4","5"],ylabels=lArDQGlobals.Partitions)

    SCGroup.defineHistogram('Msampos,MADC;h_ADCsampos', 
                                  title='ADC vs sample position',
                                  type='TH2F',
                                  path=sc_hist_path,
                                  xbins=32,xmin=0.5,xmax=32.5,
                                  ybins=400, ymin=500, ymax=1300)#,

    SCGroup.defineHistogram('Msampos,MADC;h_ADCFullRangesampos', 
                                  title='ADC vs sample position',
                                  type='TH2F',
                                  path=sc_hist_path,
                                  xbins=32,xmin=0.5,xmax=32.5,
                                  ybins=4000, ymin=0, ymax=40000)#,


    SCGroup.defineHistogram('MlatomeSourceIdBIN,Mmaxpos;h_maxposLatID', 
                                  title='Position of max sample vs. LATOME',
                                  type='TH2F',
                                  path=sc_hist_path,
                                  xbins=NLatomeBins,xmin=0,xmax=NLatomeBins,
                                  ybins=5,ymin=0.5,ymax=5.5,
                                  xlabels=BinLabel,
                                  ylabels=["1","2","3","4","5"])


    SCGroup.defineHistogram('MlatomeSourceIdBIN,MADC;h_ADCLatID', 
                                  title='ADC vs LATOME',
                                  type='TH2F',
                                  path=sc_hist_path,
                                  xbins=NLatomeBins,xmin=0,xmax=NLatomeBins,
                                  ybins=400, ymin=500, ymax=1300,
                                  xlabels=BinLabel)

    SCGroup.defineHistogram('MlatomeSourceIdBIN,MADC;h_ADCFullRangeLatID', 
                                  title='ADC vs LATOME',
                                  type='TH2F',
                                  path=sc_hist_path,
                                  xbins=NLatomeBins,xmin=0,xmax=NLatomeBins,
                                  ybins=1000, ymin=0, ymax=40000,
                                  xlabels=BinLabel)


    SCGroup.defineHistogram('MlatomeSourceIdBIN,MSCeT;h_eTLatID', 
                                  title='SC eT [MeV] vs LATOME',
                                  type='TH2F',
                                  path=sc_hist_path,
                                  xbins=NLatomeBins,xmin=0,xmax=NLatomeBins,      
                                  ybins=400, ymin=0, ymax=400000,
                                  xlabels=BinLabel)

    SCGroup.defineHistogram('MlatomeSourceIdBIN,MSCsatur;h_saturLatID', 
                                  title='SC saturation vs LATOME',
                                  type='TH2F',
                                  path=sc_hist_path,
                                  xbins=NLatomeBins,xmin=0,xmax=NLatomeBins,      
                                  ybins=3, ymin=-0.5, ymax=2.5,
                                  xlabels=BinLabel,
                                  ylabels=["0","1","2"])

    SCGroup.defineHistogram('MSCsatur;h_satur', 
                                  title='SC saturation',
                                  type='TH1F',
                                  path=sc_hist_path,
                                  xbins=3, xmin=0, xmax=3)

    SCGroup.defineHistogram('MSCeT;h_eT', 
                                  title='SC eT [MeV]',
                                  type='TH1F',
                                  path=sc_hist_path,
                                  xbins=400, xmin=0, xmax=400000)


    SCGroup.defineHistogram('MNsamples;h_Nsamples', 
                                  title='Nsamples',
                                  type='TH1F',
                                  path=sc_hist_path,
                                  xbins=40,xmin=1,xmax=40)

    SCGroup.defineHistogram('MlatomeChannel;h_latomeChannel', 
                                  title='LATOME Channel',
                                  type='TH1F',
                                  path=sc_hist_path,                                  
                                  xbins=360,xmin=1,xmax=360)

    SCGroup.defineHistogram('MlatomeSourceId;h_latomeID', 
                                  title='LATOME sourceID',
                                  type='TH1F',
                                  path=sc_hist_path,                                  
                                  xbins=1000,xmin=4000000,xmax=6000000)



    return helper.result()


if __name__=='__main__':

   from AthenaConfiguration.AllConfigFlags import ConfigFlags
   from AthenaCommon.Logging import log
   from AthenaCommon.Constants import DEBUG ,WARNING
   from AthenaCommon.Configurable import Configurable
   Configurable.configurableRun3Behavior=1
   log.setLevel(DEBUG)


   from LArMonitoring.LArMonConfigFlags import createLArMonConfigFlags
   createLArMonConfigFlags()

   from AthenaConfiguration.TestDefaults import defaultTestFiles
   ConfigFlags.Input.Files = defaultTestFiles.RAW

   ConfigFlags.Output.HISTFileName = 'LArDigitalTriggMonOutput.root'
   ConfigFlags.DQ.enableLumiAccess = False
   ConfigFlags.DQ.useTrigger = False
   ConfigFlags.Beam.Type = 'collisions'
   ConfigFlags.lock()

   from CaloRec.CaloRecoConfig import CaloRecoCfg
   cfg=CaloRecoCfg(ConfigFlags)

   aff_acc = LArDigitalTriggMonConfig(ConfigFlags)
   cfg.merge(aff_acc)

   ConfigFlags.dump()
   f=open("LArDigitalTriggMon.pkl","wb")
   cfg.store(f)
   f.close()

   cfg.run(100,OutputLevel=WARNING)
