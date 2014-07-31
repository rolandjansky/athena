#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# usage: tct_finishedMail.py

import cgi, commands, pickle, sys, string, re, os, smtplib
from datetime import datetime

def sendSms(msg,toAdd='0041762725254@sms.switch.ch'):
  yourCernMailAdd = 'rtt@mail.cern.ch'
  cernSMTPsrv='cernmx.cern.ch'
  s=smtplib.SMTP(cernSMTPsrv)
  toList=[toAdd]
  s.sendmail(yourCernMailAdd,toList,msg)
  print "<br>%s<br>A notification has been sent to the admin<br>" % msg
  s.quit()
  return

def sendMail(msg,toList):
  yourCernMailAdd = 'rtt@mail.cern.ch'
  cernSMTPsrv='cernmx.cern.ch'
  s=smtplib.SMTP(cernSMTPsrv)
  s.sendmail(yourCernMailAdd,toList,msg)
  s.quit()
  return

## main
if __name__ == "__main__":

  usageline = "usage: " + sys.argv[0] + " <emailaddress1,emailaddress2,emailaddress3,etc> summaryESDTAGcomp.txt Run00184169_Egamma_Collisions_Comparison.txt,Run00183021_MinBias0_Collisions_Comparison.txt" 
  if (len(sys.argv)<2):
    print usageline
    sys.exit(1)

  release = os.environ["AtlasArea"]
  releaseArr = release.split("/")
  release = "%s %s %s" % (releaseArr[7], releaseArr[8], releaseArr[9])

  nightly = os.environ["AtlasVersion"]
  cmtconfig = os.environ['CMTCONFIG']
  project = releaseArr[7]

  eList = sys.argv[1]
  body = "https://atlas-rtt.cern.ch/index.php?q=%28release="+releaseArr[9]+";packagename=Tier0ChainTests;branch="+releaseArr[7]+";cmtconfig="+cmtconfig+";project=AtlasProduction;verbosity=vvv;%29"
  body += "\nPlease look at the link at least 30 min after mail reception\n"

  if len(sys.argv) >= 3:
    filesNames = sys.argv[2]
    filesList = filesNames.split(',')
    #body += "\nSummary of data quality results:\n"
    #body += "================================\n"
    for fileName in filesList:
      if not os.path.exists(fileName): continue
      if 'ESDTAGcomp' in fileName:
          fSummary = open(fileName,'r')
          body += "\nSummary of ESD/TAG comparison:\n"
          body += "==============================\n\n"
          body += "https://atlas-rtt.cern.ch/prod/tct/"+releaseArr[9]+"/"+releaseArr[7]+"/build/"+cmtconfig+"/offline/Tier0ChainTests/ESDTAGCOMM_comparison/ESDTAGCOMM_comparison_log \n"
          for line in fSummary:
              body += line
              pass          
          continue
      fSummary = open(fileName,'r')
      lSummary = fSummary.readlines()
      test = False
      for line in lSummary:
        if 'TestCode=SUCCESS' in line: test = True
        pass
      fileNameList = fileName.split('/')
      shortFileName = fileNameList[len(fileNameList)-1]
      if test:
        body += ("%s = OK\n" % (shortFileName.split('.'))[0])
      else:
        body += ("%s = NOT OK\n" % (shortFileName.split('.'))[0])
        pass
      pass
    pass

  if len(sys.argv) >= 4:
    # Summary of TAG comparisons for overwrite check
    body += "\nSummary of TAG comparisons for overwrite check\n"
    body += "==============================================\n"
    filesNames = sys.argv[3]
    filesList = filesNames.split(',')
    for fileName in filesList:
      if not os.path.exists(fileName): continue
      fOverwrite = open(fileName, 'r')
      for line in fOverwrite: body += line

  os.system('tct_getstatus.py --release=%s --day=%s --dq --dump' % (releaseArr[7],releaseArr[9]))
  fstatus = open('tctstatus.txt','r')
  for line in fstatus: body += line

  print body

  ## send emails
  print "Sending email(s) to: " + eList
  msg = "From: rtt@mail.cern.ch\nSubject: Finished TCT for nightly: %s\nTo: %s\n\n%s\n" % (release,eList,body)

  toList = eList.split(',')
  for eaddress in toList:
    sendMail(msg,eaddress)
 
  ## send text messages
  #sendSms("From: rtt@mail.cern.ch\nSubject: test\n")

  print "ErrorCode=0 (OK)"

