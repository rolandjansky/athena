#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

def LArCalibPedMonConfig_REN(inputFlags,gain="",doAccDigit=False,doCalibDigit=False,doAccCalibDigit=False):

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'LArCalibPedMonCfg')

    from LArMonitoring.GlobalVariables import lArDQGlobals

    from AthenaConfiguration.ComponentFactory import CompFactory
    larPedMonAlgTest = helper.addAlgorithm(CompFactory.LArCalibPedMonAlgREN,'larCalibPedMonAlgTest')
    if gain != "":
       if doAccDigit:
          larPedMonAlgTest.LArAccumulatedDigitContainerKey=gain
       elif doAccCalibDigit:
          larPedMonAlgTest.LArAccumulatedCalibDigitContainerKey=gain
       elif doCalibDigit:
          larPedMonAlgTest.LArCalibDigitContainerKey=gain

# this creates a "Group" called "PedMonGroup" which will put its histograms into the subdirectory "'/LAr/'+GroupName+'/'"
    GroupName="PedMonGroup"

    nslots=[]
    for i in range(0,len(lArDQGlobals.FEB_Slot)): 
       nslots.append(lArDQGlobals.FEB_Slot[lArDQGlobals.Partitions[i]][1])
       
    larPedMonAlgTest.LArPedGroupName=GroupName
    larPedMonAlgTest.PartitionNames=lArDQGlobals.Partitions
    larPedMonAlgTest.SubDetNames=lArDQGlobals.SubDet

    Group = helper.addGroup(larPedMonAlgTest,GroupName,'/LAr/'+GroupName+'/')


    #Summary histos
    summary_hist_path='Summary/'
    
# this defines a 1I histogram named "NbOfReadoutChannelsGlobal" monitoring "nbChan".
# the title will be "# of readout channels"; the x axis label is "Total channels for all FEB"; and the y axis label is "Events",
# which will be put into the subdirectory "summary_hist_path"
    Group.defineHistogram('nbChan;NbOfReadoutChannelsGlobal', 
                                  title='# of readout channels;Total channels for all FEB;Events',
                                  type='TH1I',
                                  path=summary_hist_path,
                                  xbins=lArDQGlobals.N_FEB*lArDQGlobals.FEB_N_channels+5, 
                                  xmin=-0.5, xmax=lArDQGlobals.N_FEB*lArDQGlobals.FEB_N_channels+4.5)
    Group.defineHistogram('nbFEB;NbOfReadoutFEBGlobal', 
                                  title='# of readout FEB/DSP header;N_FEB;Events',
                                  type='TH1I',
                                  path=summary_hist_path,
                                  xbins=lArDQGlobals.N_FEB+11, xmin=-0.5, xmax=lArDQGlobals.N_FEB+10+0.5)
    Group.defineHistogram('nbFEBpart,part;NbOfEvts2d', 
                                  title='# of readout FEB/DSP header;N_FEB;Partition',
                                  type='TH2I',
                                  path=summary_hist_path,
                                  xbins=lArDQGlobals.N_FEB_Parttions_Max, xmin=-0.5, xmax=lArDQGlobals.N_FEB_Parttions_Max-0.5,
                                  ybins=lArDQGlobals.N_Partitions, ymin=-0.5, ymax=lArDQGlobals.N_Partitions-0.5,
                                  ylabels=lArDQGlobals.Partitions
                                  )
    # Now per partition histograms
    for subdet in range(0,lArDQGlobals.N_SubDet):
       hist_path='/LAr/'+GroupName+'NewAlg/'+lArDQGlobals.SubDet[subdet]+'/'
       slot_low = lArDQGlobals.FEB_Slot[lArDQGlobals.Partitions[subdet*2]][0] - 0.5
       slot_up  = lArDQGlobals.FEB_Slot[lArDQGlobals.Partitions[subdet*2]][1] + 0.5
       slot_n = int(slot_up - slot_low)
       
       darray = helper.addArray([lArDQGlobals.Partitions[2*subdet:2*subdet+2]],larPedMonAlgTest,lArDQGlobals.SubDet[subdet])
       
       darray.defineHistogram('nbFEBpart;nbOfFebBlocks',
                              title='# of readout FEBs (DSP header check only):Slot:FT;N_FEB;Events',
                              type='TH1I',
                              path=hist_path,
                              xbins=lArDQGlobals.N_FEB_Parttions_Max, xmin=-0.5, xmax=lArDQGlobals.N_FEB_Parttions_Max-0.5)
       
       
       
    return helper.result()

    

if __name__=='__main__':

   from AthenaConfiguration.AllConfigFlags import ConfigFlags
   from AthenaCommon.Logging import log
   from AthenaCommon.Constants import DEBUG
   from AthenaCommon.Configurable import Configurable
   Configurable.configurableRun3Behavior=1
   log.setLevel(DEBUG)


   from LArMonitoring.LArMonConfigFlags import createLArMonConfigFlags
   createLArMonConfigFlags()

   run="00395761"
   ConfigFlags.Input.Files = ["/eos/atlas/atlastier0/rucio/data21_calib/calibration_LArElec-Pedestal-32s-High-All/00395761/data21_calib.00395761.calibration_LArElec-Pedestal-32s-High-All.daq.RAW/data21_calib.00395761.calibration_LArElec-Pedestal-32s-High-All.daq.RAW._lb0000._SFO-3._0001.data"]
   ConfigFlags.Output.HISTFileName = 'LArCalibPedMonOutput_'+run+'.root'
   ConfigFlags.DQ.enableLumiAccess = False
   ConfigFlags.Input.isMC = False
   ConfigFlags.DQ.useTrigger = False
   ConfigFlags.Beam.Type = 'collisions'
   ConfigFlags.DQ.DataType = 'collisions'
   ConfigFlags.GeoModel.AtlasVersion = 'ATLAS-R2-2016-01-00-01'
   ConfigFlags.Detector.GeometryCSC=False
   ConfigFlags.Detector.GeometrysTGC=False
   ConfigFlags.Detector.GeometryMM=False
   ConfigFlags.Exec.OutputLevel=DEBUG
   ConfigFlags.lock()

# Initialize configuration object, add accumulator, merge, and run.
   from AthenaConfiguration.MainServicesConfig import MainServicesCfg
   cfg = MainServicesCfg(ConfigFlags)

   from LArByteStream.LArRawCalibDataReadingConfig import LArRawCalibDataReadingCfg
   cfg.merge(LArRawCalibDataReadingCfg(ConfigFlags,gain="HIGH",doAccCalibDigit=True))

   cfg.merge(LArCalibPedMonConfig_REN(ConfigFlags, gain="HIGH",doAccCalibDigit=True))

# If you want to turn on more detailed messages ...
# LArCalibPedMonConfig_REN.getEventAlgo('larPedMonAlgTest').OutputLevel = 2 # DEBUG
   cfg.printConfig(withDetails=False) #set True for exhaustive info

   ConfigFlags.dump()
   f=open("LArCalibPedMon_"+run+".pkl","wb")
   cfg.store(f)
   f.close()

#   cfg.run(500,OutputLevel=DEBUG) #to only run on first 500 events
   cfg.run(OutputLevel=DEBUG)
