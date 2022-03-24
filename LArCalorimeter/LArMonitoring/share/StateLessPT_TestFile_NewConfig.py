#!/usr/bin/env python
#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

if __name__=='__main__':

   import os,sys,getopt

   if len(sys.argv)>1 and (sys.argv[1]=="-h" or sys.argv[1]=="--help"):
        print("Usage:")
        print(" ")
        print("StateLessPT_TestFile_NewConfig.py {--input=<infile>} {--config=XXX} {--stream=YYY} {--noPost} {--postFreq=ZZ}")
        print("                         default input: /det/dqm/GlobalMonitoring/SMW_test/testpart_sample/data21_900GeV.00405543.physics_MinBias.daq.RAW._lb1977._SFO-6._0001.data")
        print("                         default XXX: LArMon")
        print("                         default YYY: ''")
        print("                         --noPost is switching off postProcessing")
        print("                         default ZZ: 10") 

   #some defaults
   #INPUT = '/det/dqm/GlobalMonitoring/SMW_test/testpart_sample/data21_900GeV.00405543.physics_MinBias.daq.RAW._lb1977._SFO-6._0001.data'
   INPUT = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/data18_13TeV/data18_13TeV.00357750.physics_Main.daq.RAW/data18_13TeV.00357750.physics_Main.daq.RAW._lb0083._SFO-1._0001.data'
   CONFIG = 'LArMon'
   STREAM = "NONE"
   DOPOSTPROC = True
   POSTFREQ = 10
   if len(sys.argv)>1:
      for ii in range(1,len(sys.argv)):
         print(ii," ",sys.argv[ii])
      try:
         opts,args=getopt.getopt(sys.argv[1:],"t:",["config=","stream=","postFreq=","noPost"])
      except getopt.GetoptError as e:
         print("Failed to interpret arguments")
         print(e)
         sys.exit(-1)
         pass

      for o,a in opts:
          if o=="--config": CONFIG=a
          if o=="--stream": STREAM=a
          if o=="--input":  INPUT=a
          if o=="--noPost": DOPOSTPROC=False
          if o=="--postFreq": POSTFREQ=int(a)

   # #####################################################
   #  Read jobOpt configuration options
   # #####################################################

   ## Select the monitoring tool
   if 'CONFIG' not in dir():
       CONFIG = 'LArMon'
    
   ## Select events based on stream name
   if "STREAM" not in dir():
       STREAM = "NONE"
    
   from AthenaConfiguration.Enums import Format

   # #####################################################
   # Get environment variables
   # #####################################################

   partition = os.environ.get("TDAQ_PARTITION")
   
   from ispy import *

   # ################################
   # To read run parameters from IS
   # ################################
   
   if partition is not None:
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
              beamType='cosmics'
          else:
              y.checkout()
              beamtype = y.beam_type
              beamenergy = y.beam_energy
              runnumber = y.run_number
              project = y.T0_project_tag
              print("RUN CONFIGURATION: beam type %i, beam energy %i, run number %i, project tag %s"%(beamtype,beamenergy,runnumber,project))    
              # define beam type based on project tag name
              if project[7:10]=="cos":
                  beamType='cosmics'
              else:
                  beamType='collisions'
                  
      ### LAr TEST partition
      else:
          beamType='collisions'
      
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
   else: # defaults when reading from file
      ReadDigits = False
      FirstSample = 3
      NSamples = 4
      LArFormat = 1
      ReadDigits = False
      beamType='collisions'

   print("RUN CONFIGURATION: ReadDigits =", ReadDigits)
   
   ## And now CA
   from AthenaConfiguration.AllConfigFlags import ConfigFlags
   from AthenaCommon.Configurable import Configurable
   Configurable.configurableRun3Behavior = 1

   from AthenaMonitoring.DQConfigFlags import allSteeringFlagsOff
   allSteeringFlagsOff()

   ### Set Beam Type
   from AthenaConfiguration.Enums import BeamType
   if beamType=='collisions':
      ConfigFlags.Beam.Type=BeamType.Collisions
   elif beamType=='cosmics':
      ConfigFlags.Beam.Type=BeamType.Cosmics   
   else:   
      print('Setting default collisions beam type')
      ConfigFlags.Beam.Type=BeamType.Collisions
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
   # for P1
   ConfigFlags.DQ.FileKey=''
   
   ConfigFlags.LAr.doAlign=False
   ConfigFlags.LAr.doHVCorr=False

   ConfigFlags.Input.Files=[INPUT]

   if 'CaloMon' in CONFIG: # needs Lumi access
      ConfigFlags.DQ.enableLumiAccess=True

   ConfigFlags.lock()

   # taken from future EmonByteStreamConfig
   from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
   from AthenaConfiguration.ComponentFactory import CompFactory
   from SGComps.SGInputLoaderConfig import SGInputLoaderCfg

   #acc = ComponentAccumulator()

   from AthenaConfiguration.MainServicesConfig import MainServicesCfg
   acc = MainServicesCfg(ConfigFlags)
 
   from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
   acc.merge(ByteStreamReadCfg(ConfigFlags,type_names=['TileDigitsContainer/TileDigitsCnt',
                                                       'TileRawChannelContainer/TileRawChannelCnt',
                                                       'TileMuonReceiverContainer/TileMuRcvCnt']))

   #from RecoPT_NewConfig import LArMonitoringConfig
   # include("RecoPT_NewConfig.py")
   from LArMonitoring.RecoPT_NewConfig import LArMonitoringConfig
   acc.merge(LArMonitoringConfig(ConfigFlags,CONFIG,STREAM))
   
   # somehow needs to add postprocessing
   if DOPOSTPROC:
      from DataQualityUtils.DQPostProcessingAlg import DQPostProcessingAlg
      ppa = DQPostProcessingAlg("DQPostProcessingAlg")
      ppa.ExtraInputs = [( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' )]
      ppa.Interval = POSTFREQ
      if ConfigFlags.Common.isOnline:
         ppa.FileKey = ((ConfigFlags.DQ.FileKey + '/') if not ConfigFlags.DQ.FileKey.endswith('/') 
                        else ConfigFlags.DQ.FileKey) 

      acc.addEventAlgo(ppa, sequenceName='AthEndSeq')

   acc.run()
