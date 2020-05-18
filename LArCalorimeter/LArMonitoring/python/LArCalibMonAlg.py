#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

def LArCalibMonConfig(inputFlags,gain="",doAccDigit=False,doCalibDigit=False,doAccCalibDigit=False):

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'LArCalibMonCfg')

    from LArMonitoring.GlobalVariables import lArDQGlobals

    from AthenaConfiguration.ComponentFactory import CompFactory
    larCalibMonAlg = helper.addAlgorithm(CompFactory.LArCalibPedMonAlg,'larCalibMonAlg')
    if gain != "":
       if doAccDigit:
          larCalibMonAlg.LArAccumulatedDigitContainerKey=gain
       elif doAccCalibDigit:
          larCalibMonAlg.LArAccumulatedCalibDigitContainerKey=gain
       elif doCalibDigit:
          larCalibMonAlg.LArCalibDigitContainerKey=gain

    GroupName="CalibMonGroup"

    larCalibMonAlg.LArPedGroupName=GroupName

    Group = helper.addGroup(
        larCalibMonAlg,
        GroupName,
        '/LAr/'+GroupName+'/'
    )


    #Summary histos
    summary_hist_path='Summary/'
    

    Group.defineHistogram('nbChan;NbOfReadoutChannelsGlobal', 
                                  title='# of readout channels',
                                  type='TH1I',
                                  path=summary_hist_path,
                                  xbins=lArDQGlobals.N_FEB*lArDQGlobals.FEB_N_channels+5, 
                                  xmin=-0.5, xmax=lArDQGlobals.N_FEB*lArDQGlobals.FEB_N_channels+4.5)

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

   ConfigFlags.Input.Files = ["/eos/atlas/atlastier0/rucio/data20_calib/calibration_LArElec-Delay-32s-Medium-Em/00374740/data20_calib.00374740.calibration_LArElec-Delay-32s-Medium-Em.daq.RAW/data20_calib.00374740.calibration_LArElec-Delay-32s-Medium-Em.daq.RAW._lb0000._SFO-2._0001.data"]
   ConfigFlags.Output.HISTFileName = 'LArCalibMonOutput.root'
   ConfigFlags.DQ.enableLumiAccess = False
   ConfigFlags.DQ.useTrigger = False
   ConfigFlags.Beam.Type = 'collisions'
   ConfigFlags.DQ.DataType = 'collisions'
   ConfigFlags.AtlasVersion = 'ATLAS-R2-2016-01-00-01'
   ConfigFlags.Detector.GeometryCSC=False
   ConfigFlags.Detector.GeometrysTGC=False
   ConfigFlags.Detector.GeometryMM=False
   ConfigFlags.lock()

   from AthenaConfiguration.MainServicesConfig import MainServicesCfg
   cfg = MainServicesCfg(ConfigFlags)

   from LArByteStream.LArRawCalibDataReadingConfig import LArRawCalibDataReadingCfg
   cfg.merge(LArRawCalibDataReadingCfg(ConfigFlags,gain="MEDIUM",doAccCalibDigit=True))

   cfg.merge(LArCalibMonConfig(ConfigFlags, gain="MEDIUM",doAccCalibDigit=True))

   cfg.printConfig()

   ConfigFlags.dump()
   f=open("LArCalibPedMon.pkl","w")
   cfg.store(f)
   f.close()

   cfg.run(500,OutputLevel=DEBUG)
