#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# script for retrieving ami tag and converting it into recotrf arguments
# MB 20090226
# original : /afs/cern.ch/atlas/www/cgi-bin/t0AMIupdater.cgi 

from datetime import datetime
import cgi, commands, pickle, sys, string, re, os
sys.path.insert(0,'/afs/cern.ch/atlas/project/tzero/prod1/code')
import Eowyn.luxml as luxml


def makePickle(argdict,outputpickle):
  file = open(outputpickle,'w')
  pickle.dump(argdict,file)
  file.close()
  return


def getAMITag(amitag):
  amioutput = amitag+".gpickle"
  amicmd  = 'source /afs/cern.ch/user/a/albrand/public/pyAMI/pyAMI_setup.sh ; '
  amicmd += 'python /afs/cern.ch/user/a/albrand/public/pyAMI/python/commands/nomenclature/amiListConfigurationTag.py -configTag=%s -output=%s' % (amitag, amioutput)
  s,o = commands.getstatusoutput(amicmd)
  if s:
    print "error: %s\nOutput message:\n%s" %(str(s),o)
    sys.exit(1)
  found = os.path.exists( amioutput )
  return found,amitag,amioutput


def getLatestAMITag(idx=90):
  tagfound = True
  while tagfound:
    amitag='f%d' % idx
    tagfound,amitag,amioutput = getAMITag(amitag)
    if tagfound: 
      s,o = commands.getstatusoutput("rm -f %s" % amioutput)
      idx+=1
      continue;
    else: idx-=1
  return getAMITag('f%d'%idx)


def amiSummary(amitag,amimap) :
  sfile=open(amitag+'.txt','w')
  keys=amimap.keys()
  keys.sort()
  rl=['tag','productionStep','readStatus','writeStatus','description']
  for r in rl : 
    keys.remove(r)  
  summaryString  = '%s: %s\n' % ('tag',str(amimap['tag']))
  summaryString += '%s: %s\n' % ('productionStep',str(amimap['productionStep']))
  for key in keys : 
    summaryString += '%s: %s\n' % (key,str(amimap[key])) 
  summaryString += '%s: %s\n' % ('description',str(amimap['description']).replace('\n',' '))
  sfile.write(summaryString)
  sfile.close()
  return summaryString


def checkConfigMap(amimap):
  msg=''
  moreInfoDict=luxml.toPy(amimap['moreInfo'])
  if moreInfoDict['phconfig'].has_key('conditionsTag') :
    if moreInfoDict['phconfig']['conditionsTag'] != str(amimap['ConditionsTag']):
      msg+='Conditions tag does not match\n'
  #swRelCache=moreInfoDict['tasktransinfo']['trfsetupcmd'].split(' ')[3]
  #if swRelCache != str(amimap['SWReleaseCache']):
  #  msg+='SW release cache does not match\n'
  if moreInfoDict['tasktransinfo']['trfpath'] != (str(amimap['transformation'])).split(',')[0]:
    msg+='Trf path does not match\n'
  return msg


## main
if __name__ == "__main__":

  if len(sys.argv) < 2:
    print "usage: ", sys.argv[0], "<amitag> [<runargsfile>] [<outputpickle>]"
    sys.exit(1)

  recoargsfile='recotrf.cmdargs'
  if len(sys.argv) >= 3:
    recoargsfile = sys.argv[2]    

  outputpickle='recoargdict.pickle'
  if len(sys.argv) >= 4:
    outputpickle = sys.argv[3]

  from PATJobTransforms.ArgDicTools import GetInfoFromAMI
  configmap=GetInfoFromAMI(sys.argv[1])['amiPhysDic']
  
  stringToWrite=''
  for key in configmap: 
    if type(configmap[key]) == str:
      if 'preExec' in key or 'postExec' in key:
        stringToWrite += "%s='%s' " % (key,configmap[key])
      else:
        stringToWrite += "%s=%s " % (key,configmap[key])
        pass
    elif type(configmap[key]) == int:
      stringToWrite += "%s=%d " % (key,configmap[key])
    else: 
      stringToWrite += "%s" % key
      pass
  try:
    fH=open(recoargsfile,'w')
  except:
    print "Cannot open recoargs file. Exit."
    sys.exit(2)
  fH.write(stringToWrite)
  fH.close()
  makePickle(configmap,outputpickle)

  ## print summary of configuration
  print "\nSummary of amitag <%s>" % sys.argv[1]
  print "------------------------"
  print ">> Recotrf arguments for amitag <%s> stored in <%s> and <%s>\n" % (sys.argv[1],recoargsfile,outputpickle)
  print "ErrorCode=0 (OK)"

