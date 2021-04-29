####################################################
#
# last update Fabien.Tarrade AT cern.ch July 17th
#
####################################################

if Type == 'Cosmic':
   if not 'UseOFC' in dir():
      UseOFC=True
      
   if UseOFC:
       # This is a minimal version to do produce LArRawChannel
       include( "CaloCosEx/LArCosmicFlags_jobOptions.py" )
       
       ## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
       from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

       ## define the DB Gobal Tag :
       svcMgr.IOVDbSvc.GlobalTag="COMCOND-ES1S-000-00"

       include("LArConditionsCommon/LArIdMap_comm_jobOptions.py")
       include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")
       include("CaloCosEx/LArSignalReco_Cosmics_jobOptions.py")

       theLArRawChannelBuilderPedestalDataBase.LArPedestalKey='LArPedestal'
                 
   else:
      # Use a simple Tool
      ###############################################
      # this is totaly obsolete and not working
      # not sure if it make sense to have this part
      ###############################################

      from LArRecUtils.LArADC2MeVCondAlgDefault import LArADC2MeVCondAlgDefault
      LArADC2MeVCondAlgDefault()
      
      from LArROD.LArRODConf import LArRawChannelSimpleBuilder
      theLArRawChannelBuilder = LArRawChannelSimpleBuilder()
      theLArRawChannelBuilder.DataLocation                               = LArMonFlags.LArDigitKey()
      theLArRawChannelBuilder.LArRawChannelContainerName = LArRawChannelKey
      theLArRawChannelBuilder.maxSamp                                    = 2
      topSequence += theLArRawChannelBuilder
      
elif Type == 'Pedestal':
   # This is a minimal version to do produce LArRawChannel
   include( "CaloCosEx/LArCosmicFlags_jobOptions.py" )

   ## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
   from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

   ## define the DB Gobal Tag :
   svcMgr.IOVDbSvc.GlobalTag="COMCOND-ES1S-000-00"

   include("LArConditionsCommon/LArIdMap_comm_jobOptions.py")
   include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")
   include("CaloCosEx/LArSignalReco_Cosmics_jobOptions.py")
   from AthenaCommon.BeamFlags import jobproperties
   if jobproperties.Global.DataSource.get_Value() == 'data':
      theLArRawChannelBuilderPedestalDataBase.LArPedestalKey='Pedestal'
   else:
      theLArRawChannelBuilderPedestalDataBase.LArPedestalKey='LArPedestal'
     
elif Type == 'Ramp':
   ## define the DB Gobal Tag :
   svcMgr.IOVDbSvc.GlobalTag="COMCOND-ES1S-000-00"

   from LArCalibUtils.LArCalibUtilsConf import LArRampBuilder
   theLArRampBuilder = LArRampBuilder()
   theLArRampBuilder.KeyList = [ "HIGH", "MEDIUM", "LOW" ] #   = [ LArMonFlags.LArDigitKey() ]
   theLArRampBuilder.SubtractDac0 = False
   if PeakOF: # doesn't work for now
      theLArRampBuilder.RecoType = "OF"
      from LArRecUtils.LArRecUtilsConf import LArOFPeakRecoTool
      theLArOFPeakRecoTool = LArOFPeakRecoTool()
      #theLArOFPeakRecoTool.KeyShape = "LArShape"
      theLArOFPeakRecoTool.UseShape = False
      ToolSvc += theLArOFPeakRecoTool
   else :
      theLArRampBuilder.RecoType = "Parabola"
      from LArRecUtils.LArRecUtilsConf import LArParabolaPeakRecoTool
      theLArParabolaPeakRecoTool = LArParabolaPeakRecoTool()
      theLArParabolaPeakRecoTool.correctBias = False
      ToolSvc += theLArParabolaPeakRecoTool
   theLArRampBuilder.DAC0 = -1           # As the first ramp point no longer is taken at DAC=0, this point should not have a special place.
   theLArRampBuilder.StoreRawRamp = True
   theLArRampBuilder.StoreRecRamp = True
   theLArRampBuilder.Polynom = 1
   theLArRampBuilder.RampRange = 4000;  # The first DAC is not substracted, hence saturation occurs above 4000
   theLArRampBuilder.correctBias = False
   theLArRampBuilder.ConsecutiveADCs = 0; # As the number of ramppoints was increased to 6, the demand that two points are 50 DAC counts apart no longer works
   theLArRampBuilder.minDAC = 10      # minimum DAC value to use in fit
   theLArRampBuilder.KeyOutput = "LArRamp"
   theLArRampBuilder.DeadChannelCut = -9999
   theLArRampBuilder.GroupingType = "ExtendedFeedThrough"
   theLArRampBuilder.OutputLevel = 4
   topSequence += theLArRampBuilder
    
elif Type == 'Delay' or Type == 'Cabling':
   ## define the DB Gobal Tag :
   svcMgr.IOVDbSvc.GlobalTag="COMCOND-ES1S-000-00"
   from LArCalibUtils.LArCalibUtilsConf import LArCaliWaveBuilder
   topSequence += LArCaliWaveBuilder( "LArCaliWaveBuilder" )
   LArCaliWaveBuilder                          = topSequence.LArCaliWaveBuilder
   LArCaliWaveBuilder.KeyList             = [LArMonFlags.LArDigitKey()]
   LArCaliWaveBuilder.ADCsaturation  = 4090
   LArCaliWaveBuilder.SubtractPed     = False
   LArCaliWaveBuilder.UseDacAndIsPulsedIndex  = True  # added for HEC, suitable to any subdet ?
  
   from LArCalibTools.LArCalibToolsConf import LArWaves2Ntuple
   topSequence += LArWaves2Ntuple( "LArCaliWaves2Ntuple" )
   LArCaliWaves2Ntuple = topSequence.LArCaliWaves2Ntuple
   LArCaliWaves2Ntuple.NtupleName = "CALIWAVE"
   LArCaliWaves2Ntuple.KeyList         = ["LArCaliWave"]
   
elif Type == 'DelayXtalk':
   ## define the DB Gobal Tag :
   svcMgr.IOVDbSvc.GlobalTag="COMCOND-ES1S-000-00"
   from LArCalibUtils.LArCalibUtilsConf import LArCaliWaveBuilderXtalk
   topSequence += LArCaliWaveBuilderXtalk("LArCaliWaveBuilderXtalk")
   LArCaliWaveBuilderXtalk = topSequence.LArCaliWaveBuilderXtalk
   LArCaliWaveBuilderXtalk.KeyList              = [LArMonFlags.LArDigitKey()]
   LArCaliWaveBuilderXtalk.ADCsaturation   = 4090
   LArCaliWaveBuilderXtalk.CalibPattern       = "StripSingle1"
   LArCaliWaveBuilderXtalk.PartitionPulsed   = "BarrelA"
   LArCaliWaveBuilderXtalk.FeedthroughPul = 10
