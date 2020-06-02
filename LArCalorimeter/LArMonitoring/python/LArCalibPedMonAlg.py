#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

def LArCalibPedMonConfig(inputFlags,gain="",doAccDigit=False,doCalibDigit=False,doAccCalibDigit=False):

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'LArCalibPedMonCfg')

    from LArMonitoring.GlobalVariables import lArDQGlobals

    from AthenaConfiguration.ComponentFactory import CompFactory
    larPedMonAlg = helper.addAlgorithm(CompFactory.LArCalibPedMonAlg,'larCalibPedMonAlg')
    if gain != "":
       if doAccDigit:
          larPedMonAlg.LArAccumulatedDigitContainerKey=gain
       elif doAccCalibDigit:
          larPedMonAlg.LArAccumulatedCalibDigitContainerKey=gain
       elif doCalibDigit:
          larPedMonAlg.LArCalibDigitContainerKey=gain

    GroupName="PedMonGroup"

    larPedMonAlg.LArPedGroupName=GroupName

    Group = helper.addGroup(
        larPedMonAlg,
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

   ConfigFlags.Input.Files = ["/eos/atlas/atlastier0/rucio/data20_calib/calibration_LArElec-Pedestal-32s-High-All/00374735/data20_calib.00374735.calibration_LArElec-Pedestal-32s-High-All.daq.RAW/data20_calib.00374735.calibration_LArElec-Pedestal-32s-High-All.daq.RAW._lb0000._SFO-3._0001.data"]
   ConfigFlags.Output.HISTFileName = 'LArCalibPedMonOutput.root'
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
   cfg.merge(LArRawCalibDataReadingCfg(ConfigFlags,gain="HIGH",doAccDigit=True))

   cfg.merge(LArCalibPedMonConfig(ConfigFlags, gain="HIGH",doAccDigit=True))

   cfg.printConfig()

   ConfigFlags.dump()
   f=open("LArCalibPedMon.pkl","w")
   cfg.store(f)
   f.close()

   cfg.run(500,OutputLevel=DEBUG)
