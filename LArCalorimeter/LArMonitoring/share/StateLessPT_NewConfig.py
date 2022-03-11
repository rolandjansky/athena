#!/usr/bin/env python
#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

if __name__=='__main__':
   import os,sys,getopt

   if len(sys.argv)>1 and (sys.argv[1]=="-h" or sys.argv[1]=="--help"):
        print("Usage:")
        print(" ")
        print("StateLessPT_NewConfig.py {--config XXX} {--stream YYY}")
        print("                         default XXX: LArMon")
        print("                         default YYY: ''")

   # #####################################################
   #  Read jobOpt configuration options
   # #####################################################

   #some defaults
   CONFIG = 'LArMon'
   STREAM = "NONE"
   if len(sys.argv)>1:
      for ii in range(1,len(sys.argv)):
         print(ii," ",sys.argv[ii])
      try:
         opts,args=getopt.getopt(sys.argv[1:],"t:",["config=","stream="])
      except getopt.GetoptError as e:
         print("Failed to interpret arguments")
         print(e)
         sys.exit(-1)
         pass

      for o,a in opts:
          if o=="--config": 
             print("Got option --config: ",a)
             CONFIG=a
          if o=="--stream": STREAM=a

   from AthenaConfiguration.Enums import Format

   # #####################################################
   # Get environment variables
   # #####################################################

   partition = os.environ.get("TDAQ_PARTITION")
   
   from ispy import *

   # ################################
   # To read run parameters from IS
   # ################################
   p = IPCPartition(partition)
   if not p.isValid():
      print("Partition:",p.name(),"is not valid")
      sys.exit(1)
   
   ### ATLAS partition: Read Global Run Parameters to configure the jobs
   if partition == "ATLAS":
       try:
           y = ISObject(p, 'RunParams.SOR_RunParams', 'RunParams')
       except:
           print("Could not find Run Parameters in IS - Set default beam type to 'cosmics'")
           BeamType='cosmics'
       else:
           y.checkout()
           beamtype = y.beam_type
           beamenergy = y.beam_energy
           runnumber = y.run_number
           project = y.T0_project_tag
           print("RUN CONFIGURATION: beam type %i, beam energy %i, run number %i, project tag %s"%(beamtype,beamenergy,runnumber,project))    
           # define beam type based on project tag name
           if project[7:10]=="cos":
               BeamType='cosmics'
           else:
               BeamType='collisions'
               
   ### LAr TEST partition
   else:
       BeamType='collisions'
   
   ### Read LAr Parameters
   try:
      x = ISObject(p, 'LArParams.LAr.RunLogger.GlobalParams', 'GlobalParamsInfo')
   except:
      print("Couldn not find IS Parameters - Set default flag")
      ReadDigits = False
      FirstSample = 3
      NSamples = 4
      LArFormat = 1
   else:
      try:
         x.checkout()
      except:
         print("Couldn not find IS Parameters - Set default flag")
         ReadDigits = False
         FirstSample = 3
         NSamples = 4
         LArFormat = 1
      else:
         RunType = x.runType
         LArFormat = x.format
         FirstSample = x.firstSample
         NSamples = x.nbOfSamples   
         print("RUN CONFIGURATION: format %i,run type %i"%(RunType,LArFormat))
         # Decide how to get cell energy: DSP or digits
         if LArFormat==0:
             ReadDigits = True
         else:
             ReadDigits = False
   
   print("RUN CONFIGURATION: ReadDigits =", ReadDigits)
   
   ## And now CA
   from AthenaConfiguration.AllConfigFlags import ConfigFlags
   from AthenaCommon.Configurable import Configurable
   Configurable.configurableRun3Behavior = 1

   from AthenaMonitoring.DQConfigFlags import allSteeringFlagsOff
   allSteeringFlagsOff()

   ### Set Beam Type
   ConfigFlags.Beam.Type=BeamType 
   ConfigFlags.Beam.BunchSpacing=25
   print("RUN CONFIGURATION: Beamtype =",ConfigFlags.Beam.Type)
   
   ConfigFlags.Common.isOnline=True
   ConfigFlags.Input.Format=Format.BS
   ConfigFlags.Input.isMC=False

   ConfigFlags.IOVDb.DatabaseInstance="CONDBR2"
   ConfigFlags.IOVDb.GlobalTag="CONDBR2-ES1PA-2016-03"

   ConfigFlags.GeoModel.Layout="alas"
   ConfigFlags.GeoModel.AtlasVersion="ATLAS-R2-2015-04-00-00"

   ConfigFlags.Exec.MaxEvents=-1

   from AthenaConfiguration.AutoConfigOnlineRecoFlags import setDefaultOnlineRecoFlags
   setDefaultOnlineRecoFlags(ConfigFlags)

   from AthenaConfiguration.DetectorConfigFlags import setupDetectorsFromList
   setupDetectorsFromList(ConfigFlags, ['LAr'], toggle_geometry=True)

   ConfigFlags.Trigger.doID=False
   ConfigFlags.Trigger.doMuon=False
   ConfigFlags.Trigger.L1.doMuon=False
   ConfigFlags.Trigger.L1.doTopo=False
   ConfigFlags.Trigger.triggerConfig='COOL'

   ConfigFlags.DQ.doMonitoring=True
   ConfigFlags.DQ.disableAtlasReadyFilter=True
   ConfigFlags.DQ.enableLumiAccess=False
   ConfigFlags.DQ.useTrigger=True
   ConfigFlags.DQ.FileKey=''
   
   ConfigFlags.LAr.doAlign=False
   ConfigFlags.LAr.doHVCorr=False

   if 'CaloMon' in CONFIG: # needs Lumi access
      ConfigFlags.DQ.enableLumiAccess=True

   ConfigFlags.lock()

   # taken from future EmonByteStreamConfig
   from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
   from AthenaConfiguration.ComponentFactory import CompFactory
   from SGComps.SGInputLoaderConfig import SGInputLoaderCfg

   from AthenaConfiguration.MainServicesConfig import MainServicesCfg
   acc = MainServicesCfg(ConfigFlags)
 
   bytestream_conversion = CompFactory.ByteStreamCnvSvc()
   acc.addService(bytestream_conversion, primary=True)

   bytestream_input = CompFactory.ByteStreamEmonInputSvc("ByteStreamInputSvc")
   # ############################################################
   # The name of the partition you want to connect to is taken
   # from 'TDAQ_PARTITION' if it exists, otherwise from here.
   # ############################################################

   bytestream_input.Partition = partition
   
   # #########################################
   # The source of events, dcm for full events.
   # #########################################
   
   if partition == "ATLAS" or partition[:3] == "LAr":
      bytestream_input.Key = "dcm"
   else:
      bytestream_input.Key = "CompleteEvent"
   
   bytestream_input.KeyCount = 15
   bytestream_input.KeyCount = 150
   bytestream_input.UpdatePeriod = 200
   #
   # #######################################
   # Set this to the IS server where you want
   # to publish histograms, too. If unset, no histograms 
   # are published.
   # #######################################
   
   bytestream_input.ISServer = 'LArHistogramming'
   
   # ########################################
   # The provider name under which your histograms
   #  appear in OH.
   # ########################################
   
   if STREAM!='NONE':
     bytestream_input.PublishName = '%s-%s'%(CONFIG,STREAM)
   else:
     bytestream_input.PublishName = '%s-%s'%(CONFIG,"NoTrigSel")
   
   # ###################################################
   # Should histograms be cleared at new run ? default: yes
   # Moved to False 27th Jan 2010 - Reset for each event in HLT 15.5.5
   # ###################################################
   
   bytestream_input.ClearHistograms = True
   
   # #########################
   # To get all Beam Splashes this needs uncomment
   # #########################
   
   #bytestream_input.Dispersion=False
   
   # ###############################################
   # Frequency of updates (in number of events, not secs...)
   # ###############################################
   
   bytestream_input.Frequency = 5
   
   # #######################################################
   # TimeOut (in milliseconds) - Prevent job with low rates 
   # to reconnect too often to SFIs
   # ######################################################
   
   bytestream_input.Timeout = 3600000
   
   # ###################################################
   #          Stream/trigger Selection
   # --> For LAr partitions, no stream available
   # --> For ATLAS, can create streams from L1 items
   # ##################################################

   if bytestream_input.Partition[:3] == "LAr":
       bytestream_input.StreamType = ""
   else:
       bytestream_input.LVL1Logic   = "Or"
       bytestream_input.LVL1Origin  = "TAV"
       bytestream_input.StreamType = "physics"
       bytestream_input.StreamLogic = "And"
   if  STREAM!="NONE":
       if  STREAM=="LArCellsEmpty":
           bytestream_input.StreamType = "calibration"
           bytestream_input.StreamNames = ['LArCellsEmpty']
       if STREAM=="Main":
           bytestream_input.StreamNames = ['Main']
       if STREAM=="CosmicCalo":
           bytestream_input.StreamNames = ['CosmicCalo']
       if STREAM=="L1Topo":
           bytestream_input.StreamNames = ['L1Topo']
       if STREAM=="express":
           bytestream_input.StreamNames = ['express']
           bytestream_input.StreamType = "express"
       if  STREAM=="LArNoiseBurst":
           bytestream_input.StreamType = "calibration"
           bytestream_input.StreamNames = ['LArNoiseBurst']
       if STREAM=="Standby":
           bytestream_input.StreamNames = ['Standby']
       if STREAM=="ZeroBias":
           bytestream_input.StreamNames = ['ZeroBias']
       if STREAM=="Background":
           bytestream_input.StreamNames = ['Background']
       if  STREAM=="05_Jets":
           bytestream_input.StreamType = "calibration"
           bytestream_input.StreamNames = ['DataScouting_05_Jets']
       if STREAM=="random":
           bytestream_input.StreamNames = ['monitoring_random']
           bytestream_input.StreamType = "monitoring"
       if  STREAM=="calibration":
           bytestream_input.StreamType = "calibration"

           
       print("DEBUG: bytestream_input.StreamNames:",bytestream_input.StreamNames)

   else: # select any trigger, but from physics events only !
       bytestream_input.StreamType = "physics"
       bytestream_input.StreamLogic = "Or"
    
            
   # #################################################
   # Shall athena exit if the partition is shutdown ?
   # For offline athena tasks ONLY.
   # Set this parameter to FALSE online.
   # #################################################
   bytestream_input.ExitOnPartitionShutdown = False
   
   # #################################################
   # From Sergei Kolos, Nov. 2009:
   # Parameter default overwritten to drastically
   # decrease the load on the network
   # #################################################
   
   bytestream_input.BufferSize = 10
   
   # #################################################
   # Private Application Configuration options, replace with
   # your configuration.
   # #################################################
   
   acc.addService(bytestream_input)

   event_selector = CompFactory.EventSelectorByteStream(
        name='EventSelector',
        ByteStreamInputSvc=bytestream_input.name,
        FileBased=False)
   acc.addService(event_selector)
   acc.setAppProperty("EvtSel", event_selector.name)

   event_persistency = CompFactory.EvtPersistencySvc(
        name="EventPersistencySvc",
        CnvServices=[bytestream_conversion.name])
   acc.addService(event_persistency)

   address_provider = CompFactory.ByteStreamAddressProviderSvc()
   acc.addService(address_provider)

   proxy = CompFactory.ProxyProviderSvc()
   proxy.ProviderNames += [address_provider.name]
   acc.addService(proxy)

   acc.merge(SGInputLoaderCfg(ConfigFlags, FailIfNoProxy=True))

   from RecoPT_NewConfig import LArMonitoringConfig
   print('CONFIG ',CONFIG)
   print('STREAM ',STREAM)
   acc.merge(LArMonitoringConfig(ConfigFlags,CONFIG,STREAM))
   
   # somehow needs to add postprocessing
   from DataQualityUtils.DQPostProcessingAlg import DQPostProcessingAlg
   ppa = DQPostProcessingAlg("DQPostProcessingAlg")
   ppa.ExtraInputs = [( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' )]
   ppa.Interval = 100
   if ConfigFlags.Common.isOnline:
      ppa.FileKey = ((ConfigFlags.DQ.FileKey + '/') if not ConfigFlags.DQ.FileKey.endswith('/') 
                     else ConfigFlags.DQ.FileKey) 

   acc.addEventAlgo(ppa, sequenceName='AthEndSeq')

   acc.run()
