# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from PyJobTransformsCore.runargs import RunArguments
from Tools import getRunNumber, stage # needs a link to CAFManager/Tools.py
import os

def CafRunArgs(cafJobInputs) :

  runArgs = RunArguments()

  # Maximum number of events to process
  runArgs.maxEvents = -1

  # List of strings separated by commas
  runArgs.autoConfiguration = ['everything']

  # stage files before running
  inputs = stage(cafJobInputs, os.getenv('WORKDIR'))

  runArgs.inputBSFile = []
  if inputs[0] :
    for f in inputs[0] :
      runArgs.inputBSFile.append(f)
    print runArgs.inputBSFile

  runNum = getRunNumber(cafJobInputs[0][0])
  if runNum != None and runNum.isdigit() :
    runArgs.RunNumber = int(runNum)
    print 'Setting run number to', runArgs.RunNumber

  # Output file that contains ESD's
  runArgs.outputESDFile = '$WORKDIR/dummy_ESD.pool.root'

  runArgs.loglevel = 'INFO'
  runArgs.preInclude = []                                           
  runArgs.postInclude = [] 
  runArgs.preExec = [] 
  runArgs.postExec = []

  return runArgs

