#!/usr/bin/env python
#
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

if __name__=='__main__':

   import os,sys
   import argparse
   import subprocess
   from AthenaCommon import Logging
   log = Logging.logging.getLogger( 'LArPedestalAutoCorr' )

   MinSample      = -1 
   MaxSample      = -1

   # now process the CL options and assign defaults
   parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
   #parser.add_argument('-r','--runlist', dest='runlist', default=RunNumberList, nargs='+', help='Run numbers string list', type=str)
   parser.add_argument('-r','--run', dest='run', default='00408913', help='Run number string as in input filename', type=str)
   parser.add_argument('-g','--gain', dest='gain', default="MEDIUM", help='Gain string', type=str)
   parser.add_argument('-p','--partition', dest='partition', default="All", help='Partition string', type=str)
   parser.add_argument('-f','--fileprefix', dest='fprefix', default="data22_calib", help='File prefix string', type=str)
   parser.add_argument('-i','--indirprefix', dest='dprefix', default="/eos/atlas/atlastier0/rucio/", help='Input directory prefix string', type=str)
   parser.add_argument('-d','--indir', dest='indir', default="", help='Full input dir string', type=str)
   parser.add_argument('-t','--trigger', dest='trig', default='calibration_', help='Trigger string in filename', type=str)
   parser.add_argument('-o','--outrprefix', dest='outrprefix', default="LArPedAutoCorr", help='Prefix of output root filename', type=str)
   parser.add_argument('-l','--outpprefix', dest='outpprefix', default="LArPedAutoCorr", help='Prefix of output pool filename', type=str)
   parser.add_argument('-e','--outrdir', dest='outrdir', default="/eos/atlas/atlascerngroupdisk/det-larg/Temp/Weekly/ntuples", help='Output root file directory', type=str)
   parser.add_argument('-k','--outpdir', dest='outpdir', default="/eos/atlas/atlascerngroupdisk/det-larg/Temp/Weekly/poolFiles", help='Output pool file directory', type=str)
   parser.add_argument('-n','--outsqlite', dest='outsql', default="mysql.db", help='Output sqlite file, in pool output dir.', type=str)
   parser.add_argument('-m','--subdet', dest='subdet', default="EMB", help='Subdetector, EMB, EMEC, HEC or FCAL', type=str)
   parser.add_argument('-s','--side', dest='side', default="C", help='Detector side empty (means both), C or A', type=str)
   parser.add_argument('-c','--isSC', dest='supercells', default=False, help='is SC data ?', type=bool)
   parser.add_argument('-a','--isRawdata', dest='rawdata', default=False, help='is raw data ?', type=bool)
   parser.add_argument('-b','--badchansqlite', dest='badsql', default="SnapshotBadChannel.db", help='Output sqlite file, in pool output dir.', type=str)

   args = parser.parse_args()
   if help in args and args.help is not None and args.help:
      parser.print_help()
      sys.exit(0)

   for _, value in args._get_kwargs():
    if value is not None:
        log.debug(value)

   # now set flags according parsed options
   if args.indir != "":
      InputDir = args.indir
   else:
      gain=args.gain.lower().capitalize()
      if not args.supercells:
         InputDir = args.dprefix+args.fprefix+"/calibration_LArElec-Pedestal-32s-"+gain+"-"+args.partition+"-DT-RawData/"+args.run+"/"+args.fprefix+"."+args.run+".calibration_LArElec-Pedestal-32s-"+gain+"-"+args.partition+"-DT-RawData.daq.RAW/"
      else:   
         InputDir = args.dprefix+args.fprefix+"/calibration_LArElec-Pedestal-32s-"+gain+"-"+args.partition+"-DT-RawData/"+args.run+"/"+args.fprefix+"."+args.run+".calibration_LArElec-Pedestal-32s-"+gain+"-"+args.partition+"-DT-RawData.daq.RAW/"

   
   # move start IOVC slightly back
   #IOVBegin = int(args.run)-200
   IOVBegin = int(args.run)
   
   #Import the configution-method we want to use (here: Pedestal and AutoCorr)
   from LArCalibProcessing.LArCalib_PedestalAutoCorrConfig import LArPedestalAutoCorrCfg
   
   #Import the MainServices (boilerplate)
   from AthenaConfiguration.MainServicesConfig import MainServicesCfg
   
   #Import the flag-container that is the arguemnt to the configuration methods
   from AthenaConfiguration.AllConfigFlags import ConfigFlags
   from LArCalibProcessing.LArCalibConfigFlags import addLArCalibFlags
   
   #first needs to define, if we are with SC or not
   addLArCalibFlags(ConfigFlags)
   ConfigFlags.LArCalib.isSC = args.supercells
   
   #Now we set the flags as required for this particular job:
   #The following flags help finding the input bytestream files: 
   ConfigFlags.LArCalib.Input.Dir = InputDir
   ConfigFlags.LArCalib.Input.Type = args.trig
   ConfigFlags.LArCalib.Input.RunNumbers = [int(args.run),]
   ConfigFlags.LArCalib.Input.isRawData = args.rawdata

   # Input files
   ConfigFlags.Input.Files=ConfigFlags.LArCalib.Input.Files
   #Print the input files we found 
   log.info("Input files to be processed:")
   for f in ConfigFlags.Input.Files:
       log.info(f)
   

   #Some configs depend on the sub-calo in question
   #(sets also the preselection of LArRawCalibDataReadingAlg)
   if not ConfigFlags.LArCalib.isSC:
      if args.subdet == 'EMB' or args.subdet == 'EMEC':
         ConfigFlags.LArCalib.Input.SubDet="EM"
      else:   
         ConfigFlags.LArCalib.Input.SubDet=args.subdet
 
      if args.side !="":
         if args.side == "C":
            ConfigFlags.LArCalib.Preselection.Side = [0]
         elif args.side == "A":   
            ConfigFlags.LArCalib.Preselection.Side = [1]
         else:   
            log.warning("Bad side argument: ",args.side," using both!!") 
 
      if args.subdet == 'EMB':
         ConfigFlags.LArCalib.Preselection.BEC = [0]
      else:   
         ConfigFlags.LArCalib.Preselection.BEC = [1]
 
      if args.subdet == 'FCAL':
         ConfigFlags.LArCalib.Preselection.FT = [6]
      elif args.subdet == 'HEC':
         ConfigFlags.LArCalib.Preselection.FT = [3,10,16,22]

   #Output of this job:
   OutputPedAutoCorrRootFileName = args.outrprefix + "_" + args.run
   OutputPedAutoCorrPoolFileName = args.outpprefix + "_" + args.run
      
   OutputPedAutoCorrRootFileName = OutputPedAutoCorrRootFileName + "_"+args.subdet
   OutputPedAutoCorrPoolFileName = OutputPedAutoCorrPoolFileName + "_"+args.subdet
   if ConfigFlags.LArCalib.Input.SubDet=="EM":
      OutputPedAutoCorrRootFileName = OutputPedAutoCorrRootFileName + args.side
      OutputPedAutoCorrPoolFileName = OutputPedAutoCorrPoolFileName + args.side
   OutputPedAutoCorrRootFileName = OutputPedAutoCorrRootFileName + ".root"
   OutputPedAutoCorrPoolFileName = OutputPedAutoCorrPoolFileName + ".pool.root"

   ConfigFlags.LArCalib.Output.ROOTFile = args.outrdir + "/" + OutputPedAutoCorrRootFileName
   ConfigFlags.LArCalib.Output.POOLFile = args.outpdir + "/" + OutputPedAutoCorrPoolFileName
   ConfigFlags.IOVDb.DBConnection="sqlite://;schema="+args.outpdir + "/" + args.outsql +";dbname=CONDBR2"

   #The global tag we are working with
   ConfigFlags.IOVDb.GlobalTag = "LARCALIB-RUN2-00"
   
   #BadChannels sqlite file to be created 
   ConfigFlags.LArCalib.BadChannelDB = args.outpdir + "/" + args.badsql

   #Other potentially useful flags-settings:
   
   #Define the global output Level:
   from AthenaCommon.Constants import INFO 
   ConfigFlags.Exec.OutputLevel = INFO
   
   ConfigFlags.lock()
   
   # create bad chan sqlite file
   if not ConfigFlags.LArCalib.isSC:
      cmdline = (['AtlCoolCopy', 'COOLOFL_LAR/CONDBR2', 'sqlite://;schema='+ConfigFlags.LArCalib.BadChannelDB+';dbname=CONDBR2', '-f', '/LAR/BadChannelsOfl/BadChannels',  '-f', '/LAR/BadChannelsOfl/MissingFEBs', '-t', ConfigFlags.IOVDb.GlobalTag, '-c', '-a',  '-hitag'])
   else:   
      cmdline = (['AtlCoolCopy', 'COOLOFL_LAR/CONDBR2', 'sqlite://;schema='+ConfigFlags.LArCalib.BadChannelDB+';dbname=CONDBR2', '-f', '/LAR/BadChannelsOfl/BadChannels',  '-of', '/LAR/BadChannelsOfl/BadChannelsSC', '-t', 'LARBadChannelsOflBadChannels-RUN2-empty', '-ot', 'LARBadChannelsOflBadChannelsSC-RUN2-UPD3-00', '-c', '-a',  '-hitag', '-ch', '0'])
      cmdline1 = (['AtlCoolCopy', 'COOLOFL_LAR/CONDBR2', 'sqlite://;schema='+ConfigFlags.LArCalib.BadChannelDB+';dbname=CONDBR2', '-f', '/LAR/BadChannelsOfl/MissingFEBs', '-of', '/LAR/BadChannelsOfl/MissingFEBsSC', '-t', ConfigFlags.IOVDb.GlobalTag, '-ot', 'LARBadChannelsOflMissingFEBsSC-RUN2-UPD3-01', '-a',  '-hitag'])

   try:
      cp = subprocess.run(cmdline, check=True, capture_output=True )
   except Exception as e:
      log.error('Could not create BadChan sqlite file !!!!')
      sys.exit(-1)
 
   if ConfigFlags.LArCalib.isSC:
      try:
         cp = subprocess.run(cmdline1, check=True, capture_output=True )
      except Exception as e:
         log.error('Could not create BadChan sqlite file !!!!')
         sys.exit(-1)
 
   cfg=MainServicesCfg(ConfigFlags)
   
   cfg.merge(LArPedestalAutoCorrCfg(ConfigFlags))

   cfg.getEventAlgo("OutputConditionsAlg").Run1= IOVBegin
   if 'IOVEnd' in dir() and IOVEnd>0:
      cfg.getEventAlgo("OutputConditionsAlg").Run2=IOVEnd


   #run the application
   cfg.run() 

   #build tag hierarchy in output sqlite file
   cmdline = (['/afs/cern.ch/user/l/larcalib/LArDBTools/python/BuildTagHierarchy.py',args.outpdir + "/" + args.outsql , ConfigFlags.IOVDb.GlobalTag])
   log.debug(cmdline)
   try:
      subprocess.run(cmdline, check=True)
   except Exception as e:
      log.error('Could not create tag hierarchy in output sqlite file !!!!')
      sys.exit(-1)

