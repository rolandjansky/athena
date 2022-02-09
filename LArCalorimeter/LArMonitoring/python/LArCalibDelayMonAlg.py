#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

def LArCalibDelayMonConfig(inputFlags,gain="",doAccDigit=False,doCalibDigit=False,doAccCalibDigit=False):

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'LArCalibDelayMonCfg')

    from AthenaConfiguration.ComponentFactory import CompFactory
    LArCalibDelayMonConfigCore(helper,CompFactory.LArCalibDelayMonAlg,inputFlags,gain,doAccDigit,doCalibDigit,doAccCalibDigit)
 
    rv = ComponentAccumulator()

    # adding LAr*Mapping algos
    from LArCabling.LArCablingConfig import LArFebRodMappingCfg, LArCalibIdMappingCfg
    rv.merge(LArFebRodMappingCfg(inputFlags))
    rv.merge(LArCalibIdMappingCfg(inputFlags))
    
    rv.merge(helper.result())

    return rv

def LArCalibDelayMonConfigCore(helper,algoinstance,inputFlags,gain="",doAccDigit=False,doCalibDigit=False,doAccCalibDigit=False):

    from LArMonitoring.GlobalVariables import lArDQGlobals
    
    larDelayMonAlgTest = helper.addAlgorithm(algoinstance,'larCalibDelayMonAlgTest')
    if gain != "":
       if doAccDigit:
          larDelayMonAlgTest.LArAccumulatedDigitContainerKey=gain
       elif doAccCalibDigit:
          larDelayMonAlgTest.LArAccumulatedCalibDigitContainerKey=gain
       elif doCalibDigit:
          larDelayMonAlgTest.LArCalibDigitContainerKey=gain
          
    GroupName="DelayMonGroup"

    nslots=[]
    for i in range(0,len(lArDQGlobals.FEB_Slot)): 
       nslots.append(lArDQGlobals.FEB_Slot[lArDQGlobals.Partitions[i]][1])

    larDelayMonAlgTest.LArDelayGroupName=GroupName
    larDelayMonAlgTest.PartitionNames=lArDQGlobals.Partitions
    larDelayMonAlgTest.SubDetNames=lArDQGlobals.SubDet    
    
    Group = helper.addGroup(larDelayMonAlgTest,GroupName,'/LAr/'+GroupName+'/')


    #Summary histos
    summary_hist_path='Summary/'
    
    FEBchan_n = lArDQGlobals.FEB_N_channels
    FEBchan_low = lArDQGlobals.FEB_channels_Min
    FEBchan_up = lArDQGlobals.FEB_channels_Max
        
    Group.defineHistogram('febchid,sample_max;FEBvsMaxSamples', 
                                  title='Max samples per FEB;FEB_N_channels;MaxSample',
                                  type='TProfile',
                                  path=summary_hist_path,
                                  xbins=FEBchan_n, xmin=FEBchan_low, xmax=FEBchan_up)
    Group.defineHistogram('febchid,sample_min;FEBvsMinSamples', 
                                  title='Min samples per FEB;FEB_N_channels;MinSample',
                                  type='TProfile',
                                  path=summary_hist_path,
                                  xbins=FEBchan_n, xmin=FEBchan_low, xmax=FEBchan_up)
    Group.defineHistogram('sample_max;NbOfMaxSamples', 
                                  title='Max samples;ADC;Events',
                                  type='TH1F',
                                  path=summary_hist_path,
                                  xbins=lArDQGlobals.Samples_Bins*lArDQGlobals.FEB_N_channels-2, 
                                  xmin=-0.5, xmax=lArDQGlobals.Samples_Bins*lArDQGlobals.FEB_N_channels-2.5)
    Group.defineHistogram('sample_min;NbOfMinSamples', 
                                  title='Min samples;ADC;Events',
                                  type='TH1F',
                                  path=summary_hist_path,
                                  xbins=lArDQGlobals.Samples_Bins*lArDQGlobals.FEB_N_channels-2, 
                                  xmin=-0.5, xmax=lArDQGlobals.Samples_Bins*lArDQGlobals.FEB_N_channels-2.5)
    Group.defineHistogram('dac;ValueOfDAC', 
                                  title='Value of dac;DAC;Events',
                                  type='TH1F',
                                  path=summary_hist_path,
                                  xbins=60, 
                                  xmin=0.-0.5, xmax=6000.+0.5)
    Group.defineHistogram('delay;ValueOfDelay', 
                                  title='Value of delay;Delay;Events',
                                  type='TH1F',
                                  path=summary_hist_path,
                                  xbins=200, 
                                  xmin=-0.5, xmax=200.+0.5)
    Group.defineHistogram('febchid,dac;FEBvsDAC', 
                                  title='DAC per FEB;FEB_N_channels;DAC',
                                  type='TProfile',
                                  path=summary_hist_path,
                                  xbins=FEBchan_n, xmin=FEBchan_low, xmax=FEBchan_up)
    Group.defineHistogram('febchid,delay;FEBvsDelay', 
                                  title='Delay per FEB;FEB_N_channels;Delay',
                                  type='TProfile',
                                  path=summary_hist_path,
                                  xbins=FEBchan_n, xmin=FEBchan_low, xmax=FEBchan_up)
    
    # Now per partition histograms
    for subdet in range(0,lArDQGlobals.N_SubDet):
       hist_path='/LAr/'+GroupName+'/'+lArDQGlobals.SubDet[subdet]+'/'
       slot_low = lArDQGlobals.FEB_Slot[lArDQGlobals.Partitions[subdet*2]][0] - 0.5
       slot_up  = lArDQGlobals.FEB_Slot[lArDQGlobals.Partitions[subdet*2]][1] + 0.5
       slot_n = int(slot_up - slot_low)
       ft_low = lArDQGlobals.FEB_Feedthrough[lArDQGlobals.Partitions[subdet*2]][0] - 0.5
       ft_up  = lArDQGlobals.FEB_Feedthrough[lArDQGlobals.Partitions[subdet*2]][1] + 0.5
       ft_n = int(ft_up - ft_low)
       
       darray = helper.addArray([lArDQGlobals.Partitions[2*subdet:2*subdet+2]],larDelayMonAlgTest,lArDQGlobals.SubDet[subdet])
       
       darray.defineHistogram('slot,FT;NbOfEvents2D',
                              title='Number of Events;Slot;FT',
                              type='TH2F',
                              path=hist_path,
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)
       darray.defineHistogram('slot,FT,sample_max;SlotvsFTvsMaxSamples',
                              title='Average of MaxSamples;Slot;FT',
                              type='TProfile2D',
                              path=hist_path,
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)
       darray.defineHistogram('slot,FT,sample_min;SlotvsFTvsMinSamples',
                              title='Average of MinSamples;Slot;FT',
                              type='TProfile2D',
                              path=hist_path,
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)
       darray.defineHistogram('slot30,FT30,sample_max30;SlotvsFTvsMaxSamples30',
                              title='Average of MaxSamples>3000;Slot;FT',
                              type='TProfile2D',
                              path=hist_path,
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)
    
if __name__=='__main__':

   from AthenaConfiguration.AllConfigFlags import ConfigFlags
   from AthenaCommon.Logging import log
   from AthenaCommon.Constants import DEBUG
   from AthenaCommon.Configurable import Configurable
   Configurable.configurableRun3Behavior=1
   log.setLevel(DEBUG)


   from LArMonitoring.LArMonConfigFlags import createLArMonConfigFlags
   createLArMonConfigFlags()
   
   type_run="Delay"
   run="00404654"
   part="HecFcal"
   ConfigFlags.Input.Files = [
       "/eos/atlas/atlastier0/rucio/data21_calib/calibration_LArElec-Delay-32s-High-HecFcal/00404654/data21_calib.00404654.calibration_LArElec-Delay-32s-High-HecFcal.daq.RAW/data21_calib.00404654.calibration_LArElec-Delay-32s-High-HecFcal.daq.RAW._lb0000._SFO-1._0001.data",
       
       "/eos/atlas/atlastier0/rucio/data21_calib/calibration_LArElec-Delay-32s-High-HecFcal/00404654/data21_calib.00404654.calibration_LArElec-Delay-32s-High-HecFcal.daq.RAW/data21_calib.00404654.calibration_LArElec-Delay-32s-High-HecFcal.daq.RAW._lb0000._SFO-2._0001.data",
       
       "/eos/atlas/atlastier0/rucio/data21_calib/calibration_LArElec-Delay-32s-High-HecFcal/00404654/data21_calib.00404654.calibration_LArElec-Delay-32s-High-HecFcal.daq.RAW/data21_calib.00404654.calibration_LArElec-Delay-32s-High-HecFcal.daq.RAW._lb0000._SFO-3._0001.data",
       
       "/eos/atlas/atlastier0/rucio/data21_calib/calibration_LArElec-Delay-32s-High-HecFcal/00404654/data21_calib.00404654.calibration_LArElec-Delay-32s-High-HecFcal.daq.RAW/data21_calib.00404654.calibration_LArElec-Delay-32s-High-HecFcal.daq.RAW._lb0000._SFO-4._0001.data"]
   ConfigFlags.Output.HISTFileName = 'LArCalib'+type_run+'MonOutput_'+run+'-'+part+'.root'
   ConfigFlags.DQ.enableLumiAccess = False
   ConfigFlags.Input.isMC = False
   ConfigFlags.DQ.useTrigger = False
   ConfigFlags.LAr.doAlign=False
   ConfigFlags.Beam.Type = 'collisions'
   ConfigFlags.DQ.DataType = 'collisions'
   ConfigFlags.GeoModel.AtlasVersion = 'ATLAS-R2-2016-01-00-01'
   ConfigFlags.Detector.GeometryCSC=False
   ConfigFlags.Detector.GeometrysTGC=False
   ConfigFlags.Detector.GeometryMM=False
   #ConfigFlags.Exec.OutputLevel=DEBUG
   ConfigFlags.lock()

# Initialize configuration object, add accumulator, merge, and run.
   from AthenaConfiguration.MainServicesConfig import MainServicesCfg
   cfg = MainServicesCfg(ConfigFlags)

   from LArByteStream.LArRawCalibDataReadingConfig import LArRawCalibDataReadingCfg
   cfg.merge(LArRawCalibDataReadingCfg(ConfigFlags,gain="HIGH",doAccCalibDigit=True))

   cfg.merge(LArCalibDelayMonConfig(ConfigFlags, gain="HIGH",doAccCalibDigit=True))
   
   cfg.printConfig(withDetails=False) #set True for exhaustive info

   ConfigFlags.dump()
   f=open("LArCalibDelayMon_"+run+".pkl","wb")
   cfg.store(f)
   f.close()

   cfg.run(500,OutputLevel=DEBUG) #to only run on first 500 events
