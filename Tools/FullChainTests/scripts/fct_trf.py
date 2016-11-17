#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# A command line utility to executr job transformations from a configuration file 


#author : Serhat Istin
#mail	: istin@cern.ch


#This code builds a set of job transformation(s) from a configuration file and executes it. Supported trf's are : 'generate_tf.py','AtlasG4_trf.py','Digi_trf.py','Reco_trf.py' 
#For an example configuration see : example.cfg

#There are several command line arguments. However only 2 of them are mandatory. (--config and trf type(s) --runDigi --runReco etc ... are mandatories )

# Trfs in a cfg file can be run individually or all of them can be run consecutively. This is a nice feature to implement a custom chain in a single cfg file

#IMPORTANT : Please note that no matter in what order trf-type sections in the cfg file are , they will be executed in the order evgen->simu->digi->reco if more than one type of trf s are supplied from commandline


from FullChainTests.TrfBuilder import TrfBuilder


#from TrfBuilder import TrfBuilder

import optparse
import os
import subprocess

myoptparser = optparse.OptionParser()

myoptparser.add_option('-c', '--config',help="trf configuration file (Mandatory)",dest="c",metavar='FILE')
myoptparser.add_option('-e','--runEvgen',help='Run event generation ',dest='e',metavar='flag',action='store_true')
myoptparser.add_option('-s','--runSimu',help='Run G4 simulation ',dest='s',metavar='flag',action='store_true')
myoptparser.add_option('-d','--runDigi',help='Run digitisation ',dest='d',metavar='flag',action='store_true')
myoptparser.add_option('-r','--runReco',help='Run reconstruction',dest='r',metavar='flag',action='store_true')
myoptparser.add_option('-w','--writeCommand',help='write trf command(s) in a file',dest='w',metavar='FILE')

#parse the command line
(options, args) = myoptparser.parse_args()


#if no input config file is supplied print help and exit
if options.c is None:
  print "fct_trf: Command line Error .... No config File is supplied"
  myoptparser.print_help()
  os.sys.exit(-1)

command=''#trf command to execute

#get cfg file from commandline
cfgfile=options.c

#parse the file
myconfig=TrfBuilder(cfgfile)

#handle command line arguments here 
if options.e:
  print 'fct_trf: INFO:: Will run evgen...'
  command=myconfig.buildCommand("generate_tf.py")
if options.s:
  print 'fct_trf: INFO:: Will run simu...'
  command=myconfig.buildCommand("AtlasG4_trf.py")
if options.d:
  print 'fct_trf: INFO:: Will run digi...'
  command=myconfig.buildCommand("Digi_tf.py")
if options.r:
  print 'fct_trf: INFO:: Will run reco...'
  command=myconfig.buildCommand("Reco_tf.py")
  
if not (options.s or options.e or options.r or options.d):
  print "fct_trf: INFO:: Command Line Error... No trf type is supplied"
  myoptparser.print_help()
  os.sys.exit(-1)

if options.w:
  print 'fct_trf: INFO:: Trf Command will be writen to file: '+options.w
  ofile=open(options.w,'w')
  for i in range(0,len(myconfig.commandList)):
    ofile.write(myconfig.commandList[i]+'\n')
  ofile.close()

#execute all commands in commandList. Take care of process exit codes

summarycode=0
for command in myconfig.commandList:
  print 'fct_trf: INFO:: Executing Command:'+command
  statcode=subprocess.call(command,shell=True)
  summarycode=summarycode+statcode
  if statcode!=0:
    print "fct_trf: ERROR : command failed."
  print 'fct_trf: INFO:: Finished executing command:'+command

if summarycode!=0:
  print "fct_trf :SUMMARY: fct_trf failed: Exiting with status code: "+str(summarycode)
  os.sys.exit(summarycode)
else:
  print "fct_trf :SUMMARY: All went OK. Exiting with status code: "+str(statcode)
  os.sys.exit(summarycode)
