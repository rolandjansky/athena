#!/usr/bin/env python
#
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

if __name__=='__main__':

   import os,sys
   import argparse

   # now process the CL options and assign defaults
   parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
   #parser.add_argument('-r','--runlist', dest='runlist', default=RunNumberList, nargs='+', help='Run numbers string list', type=str)
   parser.add_argument('-r','--run', dest='run', default='00408918', help='Run number string as in input filename', type=str)
   parser.add_argument('-g','--gain', dest='gain', default="MEDIUM", help='Gain string', type=str)
   parser.add_argument('-p','--partition', dest='partition', default="Em", help='Data taking partition string', type=str)
   parser.add_argument('-f','--fileprefix', dest='fprefix', default="data22_calib", help='File prefix string', type=str)
   parser.add_argument('-i','--indirprefix', dest='dprefix', default="/eos/atlas/atlastier0/rucio/", help='Input directory prefix string', type=str)
   parser.add_argument('-d','--indir', dest='indir', default="", help='Full input dir string', type=str)
   parser.add_argument('-t','--trigger', dest='trig', default='calibration_', help='Trigger string in filename', type=str)
   parser.add_argument('-o','--outrwaveprefix', dest='outrwaveprefix', default="LArCaliWave", help='Prefix of CaliWave output root filename', type=str)
   parser.add_argument('-l','--outppwaverefix', dest='outpwaveprefix', default="LArCaliWave", help='Prefix of CaliWave output pool filename', type=str)
   parser.add_argument('-a','--outrofcprefix', dest='outrofcprefix', default="LArOFCCali", help='Prefix of output Cali OFC root filename', type=str)
   parser.add_argument('-b','--outpofcprefix', dest='outpofcprefix', default="LArOFCCali", help='Prefix of output Cali OFC pool filename', type=str)
   parser.add_argument('-e','--outrdir', dest='outrdir', default="/eos/atlas/atlascerngroupdisk/det-larg/Temp/Weekly/ntuples", help='Output root file directory', type=str)
   parser.add_argument('-k','--outpdir', dest='outpdir', default="/eos/atlas/atlascerngroupdisk/det-larg/Temp/Weekly/poolFiles", help='Output pool file directory', type=str)
   parser.add_argument('-u','--insqlite', dest='insql', default="mysql.db", help='Input sqlite file with pedestals, in pool output dir.', type=str)
   parser.add_argument('-n','--outsqlite', dest='outsql', default="mysql_delay.db", help='Output sqlite file, in pool output dir.', type=str)
   parser.add_argument('-m','--subdet', dest='subdet', default="EMB", help='Subdetector, EMB, EMEC, HEC or FCAL', type=str)
   parser.add_argument('-s','--side', dest='side', default="C", help='Detector side empty (means both), C or A', type=str)
   parser.add_argument('-c','--isSC', dest='supercells', default=False, help='is SC data ?', type=bool)

   args = parser.parse_args()
   if help in args and args.help is not None and args.help:
      parser.print_help()
      sys.exit(0)

   for _, value in args._get_kwargs():
    if value is not None:
        print(value)

   # now set flags according parsed options
   if args.indir != "":
      InputDir = args.indir
   else:
      gain=args.gain.lower().capitalize()
      InputDir = args.dprefix+args.fprefix+"/calibration_LArElec-Delay-32s-"+gain+"-"+args.partition+"/"+args.run+"/"+args.fprefix+"."+args.run+".calibration_LArElec-Delay-32s-"+gain+"-"+args.partition+".daq.RAW/"

   # move start IOVC slightly back
   #IOVBegin = int(args.run)-200
   IOVBegin = int(args.run)
   
   #Import the configution-method we want to use (here: Pedestal and AutoCorr)
   from LArCalibProcessing.LArCalib_Delay_OFCCaliConfig import LArDelay_OFCCaliCfg
   
   #Import the MainServices (boilerplate)
   from AthenaConfiguration.MainServicesConfig import MainServicesCfg
   
   #Import the flag-container that is the arguemnt to the configuration methods
   from AthenaConfiguration.AllConfigFlags import ConfigFlags
   from LArCalibProcessing.LArCalibConfigFlags import addLArCalibFlags
   addLArCalibFlags(ConfigFlags)
   
   #This allows set flags from the command-line (not strictly required for the AP) 
   ConfigFlags.fillFromArgs()
   
   #Now we set the flags as required for this particular job:
   #The following flags help finding the input bytestream files: 
   ConfigFlags.LArCalib.Input.Dir = InputDir
   ConfigFlags.LArCalib.Input.Type = args.trig
   ConfigFlags.LArCalib.Input.RunNumbers = [int(args.run),]
   ConfigFlags.LArCalib.Input.Database = args.outpdir + "/" +args.insql

   # Input files
   ConfigFlags.Input.Files=ConfigFlags.LArCalib.Input.Files
   #Print the input files we found 
   print ("Input files to be processed:")
   for f in ConfigFlags.Input.Files:
       print (f)
   
   # others flags settings
   ConfigFlags.LArCalib.isSC = args.supercells

   #Some configs depend on the sub-calo in question
   #(sets also the preselection of LArRawCalibDataReadingAlg)
   if not ConfigFlags.LArCalib.isSC:
      if args.subdet == 'EMB' or args.subdet == 'EMEC':
         ConfigFlags.LArCalib.Input.SubDet="EM"
      else:   
         ConfigFlags.LArCalib.Input.SubDet=args.subdet
 
      if args.side == "":   
         ConfigFlags.LArCalib.Preselection.Side = [0,1]
      elif args.side == "C":
         ConfigFlags.LArCalib.Preselection.Side = [0]
      elif args.side == "A":   
         ConfigFlags.LArCalib.Preselection.Side = [1]
      else:   
         print("unknown side ",args.side)
         sys.exit(-1)
 
      if args.subdet == 'EMB':
         ConfigFlags.LArCalib.Preselection.BEC = [0]
      else:   
         ConfigFlags.LArCalib.Preselection.BEC = [1]
 
      if args.subdet == 'FCAL':
         ConfigFlags.LArCalib.Preselection.FT = [6]
      elif args.subdet == 'HEC':
         ConfigFlags.LArCalib.Preselection.FT = [3,10,16,22]
   
   #Configure the Bad-Channel database we are reading 
   #(the AP typically uses a snapshot in an sqlite file
   ConfigFlags.LArCalib.BadChannelDB = "LAR_OFL"
   ConfigFlags.LArCalib.BadChannelTag = "-RUN2-UPD3-00"
   
   #Output of this job:
   OutputCaliWaveRootFileName = args.outrwaveprefix + "_" + args.run
   OutputCaliWavePoolFileName = args.outpwaveprefix + "_" + args.run
   OutputOFCCaliRootFileName = args.outrofcprefix + "_" + args.run
   OutputOFCCaliPoolFileName = args.outpofcprefix + "_" + args.run
   OutputCaliWaveRootFileName += "_"+args.subdet
   OutputCaliWavePoolFileName += "_"+args.subdet
   OutputOFCCaliRootFileName += "_"+args.subdet
   OutputOFCCaliPoolFileName += "_"+args.subdet

   if ConfigFlags.LArCalib.Input.SubDet=="EM":
      OutputCaliWaveRootFileName +=  args.side
      OutputCaliWavePoolFileName +=  args.side
      OutputOFCCaliRootFileName += args.side
      OutputOFCCaliPoolFileName += args.side

   OutputCaliWaveRootFileName += ".root"
   OutputCaliWavePoolFileName += ".pool.root"
   OutputOFCCaliRootFileName +=  ".root"
   OutputOFCCaliPoolFileName += ".pool.root"

   ConfigFlags.LArCalib.Output.ROOTFile = args.outrdir + "/" + OutputCaliWaveRootFileName
   ConfigFlags.LArCalib.Output.POOLFile = args.outpdir + "/" + OutputCaliWavePoolFileName
   ConfigFlags.LArCalib.Output.ROOTFile2 = args.outrdir + "/" + OutputOFCCaliRootFileName
   ConfigFlags.LArCalib.Output.POOLFile2 = args.outpdir + "/" + OutputOFCCaliPoolFileName
   ConfigFlags.IOVDb.DBConnection="sqlite://;schema="+args.outpdir + "/" + args.outsql +";dbname=CONDBR2"

   #The global tag we are working with
   ConfigFlags.IOVDb.GlobalTag = "LARCALIB-RUN2-00"
   

   #Other potentially useful flags-settings:
   
   #Define the global output Level:
   from AthenaCommon.Constants import INFO 
   ConfigFlags.Exec.OutputLevel = INFO
   
   ConfigFlags.lock()
   
   cfg=MainServicesCfg(ConfigFlags)
   
   cfg.merge(LArDelay_OFCCaliCfg(ConfigFlags))

   cfg.getEventAlgo("OutputConditionsAlg").Run1= IOVBegin
   if 'IOVEnd' in dir() and IOVEnd>0:
      cfg.getEventAlgo("OutputConditionsAlg").Run2=IOVEnd


   #run the application
   cfg.run() 

   #build tag hierarchy in output sqlite file
   import subprocess
   cmdline = (['/afs/cern.ch/user/l/larcalib/LArDBTools/python/BuildTagHierarchy.py',args.outpdir + "/" + args.outsql , ConfigFlags.IOVDb.GlobalTag])
   print(cmdline)
   try:
      subprocess.run(cmdline, check=True)
   except Exception as e:
      print('Could not create tag hierarchy in output sqlite file !!!!')
      sys.exit(-1)

