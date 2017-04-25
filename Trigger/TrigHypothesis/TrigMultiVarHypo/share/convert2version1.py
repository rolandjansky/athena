#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RingerCore import Logger, LoggingLevel, retrieve_kw, checkForUnusedVars


def __retrieve_py_module(logger, pypath):
  #try:
  import imp
  obj__Module__ = imp.load_source('SignaturesMap', pypath)
  #except ImportError:
  #  self._logger.fatal('Can not import the discriminator %s',pypath)
  SignaturesMap = obj__Module__.SignaturesMap()

  try: # Remove junk file created by the python reader
    import os
    #os.remove( pypath+'c')
  except:
    logger.warning('%sc not found',pypath)
  return SignaturesMap



import argparse

mainLogger = Logger.getModuleLogger("convert to new model")

parser = argparse.ArgumentParser(description = '',
                                     add_help = False)
parser = argparse.ArgumentParser()

parser.add_argument('-py','--pypath', action='store', 
    dest='pypath', required = True,
    help = "The python discriminator path ")

parser.add_argument('--name', action='store', 
    dest='name', required = True,
    help = "The python discriminator path ")


import sys,os
if len(sys.argv)==1:
  parser.print_help()
  sys.exit(1)

args = parser.parse_args()

mainLogger = Logger.getModuleLogger( __name__, LoggingLevel.INFO )
name = args.name
c =  __retrieve_py_module(mainLogger, args.pypath)

outputDict    = {'version':1, 'type': ['Fex' ]  , 'date':0, 'metadata':dict(), 'tuning':dict(), 'name': [name]}
thresholdDict = {'version':1, 'type': ['Hypo']  , 'date':0, 'metadata':dict(), 'tuning':dict(), 'name': [name]}

metadata = {'UseLumiTool': False,
            'UseLumiVar' : False,
            'UseEtaVar'  : False,
            'LumiCut'    : 60,
            'UseNoActivationFunctionInTheLastLayer': False,
            'DoPileupCorrection': False,
            }

# Hold the metadata default configuration
outputDict['metadata']    = metadata
thresholdDict['metadata'] = metadata



for tuningName in sorted(c.keys()):
  dDict=dict()
  tDict=dict()
  for key in sorted(c[tuningName].keys()):

    discr     = {'discriminator':dict(),'configuration':dict()}
    threshold = {'configuration':dict()}
    etBin = c[tuningName][key]['configuration']['etBin']
    etaBin = c[tuningName][key]['configuration']['etaBin']
    discr['discriminator']['nodes']         = c[tuningName][key]['discriminator']['nodes']
    discr['discriminator']['weights']       = c[tuningName][key]['discriminator']['weights']
    discr['discriminator']['bias']          = c[tuningName][key]['discriminator']['bias']
    discr['configuration']['etBin']         = etBin
    discr['configuration']['etaBin']        = etaBin
    discr['configuration']['benchmarkName'] = c[tuningName][key]['configuration']['benchmarkName']

    threshold['configuration']['etBin']     = etBin
    threshold['configuration']['etaBin']    = etaBin
    myThr  = c[tuningName][key]['discriminator']['threshold']
    
    if not type(myThr) is list:
      threshold['threshold'] = (0, 0, myThr)
    else:
      threshold['threshold'] = myThr

    dDict[key] = discr
    tDict[key] = threshold

  outputDict['tuning'][tuningName] = dDict
  thresholdDict['tuning'][tuningName] = tDict

####################### Write Ringer Configuration #########################

pyfile = open('TrigL2CaloRingerConstants.py','w')
pyfile.write('def SignaturesMap():\n')
pyfile.write('  s=dict()\n')
for key in outputDict.keys():
  pyfile.write('  s["%s"]=%s\n' % (key, outputDict[key]))
pyfile.write('  return s\n')


pyfile = open('TrigL2CaloRingerThresholds.py','w')
pyfile.write('def ThresholdsMap():\n')
pyfile.write('  s=dict()\n')
for key in thresholdDict.keys():
  pyfile.write('  s["%s"]=%s\n' % (key, thresholdDict[key]))
pyfile.write('  return s\n')




#output = open('TrigL2CaloRingerConstants.py','w')
#output.write('def SignaturesMap():\n')
#output.write('  signatures=dict()\n')
#
#for key in tuningNameList:
#  output.write('  signatures["%s"]=%s\n' % (key, outputDict[key]))
#
#output.write('  return signatures\n')
#

###########################################################################







