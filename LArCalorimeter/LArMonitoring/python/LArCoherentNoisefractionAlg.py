#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
'''@file LArCoherentNoisefractionAlg
@author P. Strizenec
@date 22-05-2021
@brief Adapted from LArNoiseCorrelationMonAlg by M. Spalla 
'''


def LArCoherentNoisefractionConfigOld(inputFlags, febsToMonitor=[], groupsToMonitor=[], isCalib=True):

    from AthenaMonitoring.AthMonitorCfgHelper import AthMonitorCfgHelperOld
    from LArMonitoring.LArMonitoringConf import LArCoherentNoisefractionMonAlg

    helper = AthMonitorCfgHelperOld(inputFlags, 'LArCoherentNoisefractionMonAlgCfg')
    LArCoherentNoisefractionConfigCore(helper, LArCoherentNoisefractionMonAlg,inputFlags,febsToMonitor,groupsToMonitor,isCalib)
    return helper.result()

def LArCoherentNoisefractionConfig(inputFlags,febsToMonitor=[], groupsToMonitor=[], isCalib=True):
    
    from AthenaMonitoring.AthMonitorCfgHelper import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'LArCoherentNoisefractionMonAlgCfg')

    from AthenaConfiguration.ComponentFactory import CompFactory
    return LArCoherentNoisefractionConfigCore(helper, CompFactory.LArCoherentNoisefractionMonAlg,inputFlags,febsToMonitor,groupsToMonitor,isCalib)

def LArCoherentNoisefractionConfigCore(helper, algoinstance, inputFlags, febsToMonitor, groupsToMonitor, isCalib):


    from LArMonitoring.GlobalVariables import lArDQGlobals

    larCoherentNoisefractionMonAlg = helper.addAlgorithm(algoinstance,'larCoherentNoisefractionMonAlg')

    #set all groups to monitor, if the empty list is passed
    allGroups=["tot","top","bot","left","right","q1","q2","q3","q4","qs11","qs22","qs3","qs4"]
    groupsNChan=[128,64,64,64,64,32,32,32,32,32,32,32,32]
    allMonitor=[True,True,True,True,True,True,True,True,True,True,True,True,True]

    #from AthenaCommon.Constants import DEBUG
    #larCoherentNoisefractionMonAlg.OutputLevel = DEBUG
    larCoherentNoisefractionMonAlg.IsCalibrationRun = isCalib
    larCoherentNoisefractionMonAlg.LArDigitContainerKey = "HIGH"
    larCoherentNoisefractionMonAlg.ListOfGroupNames = allGroups
    larCoherentNoisefractionMonAlg.GroupNchan = groupsNChan
    if len(groupsToMonitor) == 0:
       customGroupstoMonitor = allMonitor
    else:
       if len(customGroupstoMonitor) != len(allMonitor):
             from AthenaCommon.Logging import logging
             logging.getLogger().warning("Wrong list of groups to monitor, setting all !")
             customGroupstoMonitor = allMonitor
       else:      
             customGroupstoMonitor = groupsToMonitor   
    larCoherentNoisefractionMonAlg.GroupsToMonitor = customGroupstoMonitor   

    # if empty list passed,
    #set custom list of FEBs to be monitored (if you want one): each FEB should be passed as a string of the form "BarrelAft01slot10"
    FEBs_from_DQ_run_350440 = ["endcapAft19slot12","endcapAft19slot09","endcapAft20slot09"]

    if len(febsToMonitor) == 0:
       customFEBStoMonitor=FEBs_from_DQ_run_350440
    else:   
       customFEBStoMonitor=febsToMonitor

    #correct custom FEBs for upper-lower cases or single-digit ft and slot numbers (e.g. 3 instead of 03)
    from ROOT import LArStrHelper
    larStrHelp=LArStrHelper()
    customFEBStoMonitor=[larStrHelp.fixFEBname(nm) for nm in customFEBStoMonitor]


    # adding BadChan masker private tool
    from AthenaConfiguration.ComponentFactory import isRun3Cfg


    larCoherentNoisefractionMonAlg.TriggerChain = "HLT_noalg_zb_L1ZB, HLT_noalg_cosmiccalo_L1RD1_EMPTY" #turn off for calibration run 


    setCustomFEBS=set(customFEBStoMonitor)
    febsToMonitorBarrelA=list(setCustomFEBS.intersection(lArDQGlobals.febsBarrelA))
    febsToMonitorEndcapA=list(setCustomFEBS.intersection(lArDQGlobals.febsEndcapA))
    febsToMonitorBarrelC=list(setCustomFEBS.intersection(lArDQGlobals.febsBarrelC))
    febsToMonitorEndcapC=list(setCustomFEBS.intersection(lArDQGlobals.febsEndcapC))

    if len(febsToMonitorBarrelA)==0 and len(febsToMonitorEndcapA)==0 and len(febsToMonitorBarrelC)==0 and len(febsToMonitorEndcapC)==0:
        print("LArCoherentNoisefractionMonAlg:WARNING. None of the following FEBs were recognised, no plot will be produced")
        print(customFEBStoMonitor)
        larCoherentNoisefractionMonAlg.PlotsOFF=True #lets protect ourselves against poor writing
        larCoherentNoisefractionMonAlg.PlotCustomFEBSset=False
        larCoherentNoisefractionMonAlg.FEBlist=[]
    else:
        #pass to algorithm
        larCoherentNoisefractionMonAlg.PlotCustomFEBSset=True
        larCoherentNoisefractionMonAlg.FEBlist=febsToMonitorBarrelA+febsToMonitorBarrelC+febsToMonitorEndcapA+febsToMonitorEndcapC
        pass

    #prepare the monitoring groups
    for grp in range(0,len(allGroups)):
       if not customGroupstoMonitor[grp]: continue   


       cnfArray = helper.addArray([larCoherentNoisefractionMonAlg.FEBlist],larCoherentNoisefractionMonAlg,allGroups[grp]) 

       hist_path='/LAr/CNF/'
    
       tot_plot_name="cnf_tot"
       tot_var_and_name="SumDev;"+tot_plot_name
       cnfArray.defineHistogram(tot_var_and_name,
                                title=tot_plot_name,
                                type='TH1F',
                                path=hist_path+'BarrelA',
                                xbins=lArDQGlobals.CNFN_tot, xmin=lArDQGlobals.CNFXmin_tot, xmax=lArDQGlobals.CNFXmax_tot,
                                pattern=febsToMonitorBarrelA)

       cnfArray.defineHistogram(tot_var_and_name,
                                title=tot_plot_name,
                                type='TH1F',
                                path=hist_path+'BarrelC',
                                xbins=lArDQGlobals.CNFN_tot, xmin=lArDQGlobals.CNFXmin_tot, xmax=lArDQGlobals.CNFXmax_tot,
                                pattern=febsToMonitorBarrelC)

       print(lArDQGlobals.CNFN_tot)
       cnfArray.defineHistogram(tot_var_and_name,
                                title=tot_plot_name,
                                type='TH1F',
                                path=hist_path+'EndcapA',
                                xbins=lArDQGlobals.CNFN_tot, xmin=lArDQGlobals.CNFXmin_tot, xmax=lArDQGlobals.CNFXmax_tot,
                                pattern=febsToMonitorEndcapA)
                                

       cnfArray.defineHistogram(tot_var_and_name,
                                title=tot_plot_name,
                                type='TH1F',
                                path=hist_path+'EndcapC',
                                xbins=lArDQGlobals.CNFN_tot, xmin=lArDQGlobals.CNFXmin_tot, xmax=lArDQGlobals.CNFXmax_tot,
                                pattern=febsToMonitorEndcapC)


       noncoh_plot_name="cnf_noncoh"
       noncoh_var_and_name="Dev;"+noncoh_plot_name

       cnfArray.defineHistogram(noncoh_var_and_name,
                                title=noncoh_plot_name,
                                type='TH1F',
                                path=hist_path+'BarrelA',
                                xbins=lArDQGlobals.CNFN_ncoh, xmin=lArDQGlobals.CNFXmin_ncoh, xmax=lArDQGlobals.CNFXmax_ncoh,
                                pattern=febsToMonitorBarrelA)

       cnfArray.defineHistogram(noncoh_var_and_name,
                                title=noncoh_plot_name,
                                type='TH1F',
                                path=hist_path+'BarrelC',
                                xbins=lArDQGlobals.CNFN_ncoh, xmin=lArDQGlobals.CNFXmin_ncoh, xmax=lArDQGlobals.CNFXmax_ncoh,
                                pattern=febsToMonitorBarrelC)

       cnfArray.defineHistogram(noncoh_var_and_name,
                                title=noncoh_plot_name,
                                type='TH1F',
                                path=hist_path+'EndcapA',
                                xbins=lArDQGlobals.CNFN_ncoh, xmin=lArDQGlobals.CNFXmin_ncoh, xmax=lArDQGlobals.CNFXmax_ncoh,
                                pattern=febsToMonitorEndcapA)

       cnfArray.defineHistogram(noncoh_var_and_name,
                                title=noncoh_plot_name,
                                type='TH1F',
                                path=hist_path+'EndcapC',
                                xbins=lArDQGlobals.CNFN_ncoh, xmin=lArDQGlobals.CNFXmin_ncoh, xmax=lArDQGlobals.CNFXmax_ncoh,
                                pattern=febsToMonitorEndcapC)


    print(cnfArray.toolList())

    if isRun3Cfg():
        cfg.merge(helper.result())
        return cfg
    else:    
        return helper.result()
    

if __name__=='__main__':

   from AthenaConfiguration.AllConfigFlags import ConfigFlags
   from AthenaCommon.Logging import log
   from AthenaCommon.Constants import DEBUG #,WARNING
   from AthenaCommon.Configurable import Configurable
   Configurable.configurableRun3Behavior=1
   log.setLevel(DEBUG)


   from LArMonitoring.LArMonConfigFlags import createLArMonConfigFlags
   createLArMonConfigFlags()

   #from AthenaConfiguration.TestDefaults import defaultTestFiles
   ConfigFlags.Input.Files = ['/eos/atlas/atlastier0/rucio/data21_calib/calibration_LArElec-Pedestal-5s-High-Emec-A-RawData/00393063/data21_calib.00393063.calibration_LArElec-Pedestal-5s-High-Emec-A-RawData.daq.RAW/data21_calib.00393063.calibration_LArElec-Pedestal-5s-High-Emec-A-RawData.daq.RAW._lb0000._SFO-1._0001.data','/eos/atlas/atlastier0/rucio/data21_calib/calibration_LArElec-Pedestal-5s-High-Emec-A-RawData/00393063/data21_calib.00393063.calibration_LArElec-Pedestal-5s-High-Emec-A-RawData.daq.RAW/data21_calib.00393063.calibration_LArElec-Pedestal-5s-High-Emec-A-RawData.daq.RAW._lb0000._SFO-2._0001.data','/eos/atlas/atlastier0/rucio/data21_calib/calibration_LArElec-Pedestal-5s-High-Emec-A-RawData/00393063/data21_calib.00393063.calibration_LArElec-Pedestal-5s-High-Emec-A-RawData.daq.RAW/data21_calib.00393063.calibration_LArElec-Pedestal-5s-High-Emec-A-RawData.daq.RAW._lb0000._SFO-3._0001.data','/eos/atlas/atlastier0/rucio/data21_calib/calibration_LArElec-Pedestal-5s-High-Emec-A-RawData/00393063/data21_calib.00393063.calibration_LArElec-Pedestal-5s-High-Emec-A-RawData.daq.RAW/data21_calib.00393063.calibration_LArElec-Pedestal-5s-High-Emec-A-RawData.daq.RAW._lb0000._SFO-4._0001.data']

   ConfigFlags.Output.HISTFileName = 'LArCNFMonOutput.root'
   ConfigFlags.DQ.enableLumiAccess = False
   ConfigFlags.DQ.useTrigger = False
   from AthenaConfiguration.Enums import BeamType
   ConfigFlags.Beam.Type = BeamType.Collisions
   ConfigFlags.lock()

   from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
   cfg=ComponentAccumulator()
   from LArByteStream.LArRawDataReadingConfig import LArRawDataReadingCfg
   cfg.merge(LArRawDataReadingCfg(ConfigFlags,LArDigitKey="HIGH",LArRawChannelKey=""))
   # for calib digits:
   #from LArByteStream.LArRawCalibDataReadingConfig import LArRawCalibDataReadingCfg
   #cfg.merge(LArRawCalibDataReadingCfg(ConfigFlags,gain="HIGH",doCalibDigit=True))
   from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg
   cfg.merge(LArOnOffIdMappingCfg(ConfigFlags))
   from LArConfiguration.LArElecCalibDBConfig import LArElecCalibDbCfg
   cfg.merge(LArElecCalibDbCfg(ConfigFlags,["Pedestal"]))

   feblist=[]
   for ft in [11,12,23,24]:
      for slot in range(1,15):
         if slot < 10:
             feblist += ['EndcapAft'+str(ft)+'slot0'+str(slot)]
         else:
             feblist += ['EndcapAft'+str(ft)+'slot'+str(slot)]
   aff_acc = LArCoherentNoisefractionConfig(ConfigFlags,feblist)

   cfg.merge(aff_acc)

   cfg.printConfig()
   log.setLevel(DEBUG)
   ConfigFlags.dump()
   f=open("LArCNFMon.pkl","wb")
   cfg.store(f)
   f.close()

   #cfg.run(100)
