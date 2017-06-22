#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# tct_finishedMail.py: utilities for sending notification emails
# about the results from the tests in the Tier0ChainTests
#

import sys, string, os, smtplib
from get_release_2017 import (fn2dt, _get_release)

def sendSms(msg,toAdd='0041762725254@sms.switch.ch'):
  yourCernMailAdd = 'rtt@mail.cern.ch'
  cernSMTPsrv='cernmx.cern.ch'
  s=smtplib.SMTP(cernSMTPsrv)
  toList=[toAdd]
  s.sendmail(yourCernMailAdd,toList,msg)
  print "<br>%s<br>A notification has been sent to the admin<br>" % msg
  s.quit()
  return

def sendMail(recipient, message):
  s = smtplib.SMTP('cernmx.cern.ch')
  s.sendmail('rtt@mail.cern.ch', recipient, message)
  s.quit()
  return

## main
if __name__ == "__main__":

  usageline = "usage: " + sys.argv[0] + " <emailaddress1,emailaddress2,emailaddress3,etc> summaryESDTAGcomp.txt Run00184169_Egamma_Collisions_Comparison.txt,Run00183021_MinBias0_Collisions_Comparison.txt" 
  if (len(sys.argv)<2):
    print usageline
    sys.exit(1)

  project   = os.environ["AtlasProject"]
  nightly   = os.environ["AtlasBuildStamp"]
  branch    = os.environ["AtlasBuildBranch"]
  cmtconfig = os.environ['CMTCONFIG']
  relNumber = _get_release(fn2dt(nightly))
  release = "%s %s %s" % (nightly, branch, project)

  emailAddresses = sys.argv[1]

  resultsurl       = "https://atlas-rtt.cern.ch/index.php?q=%%28release=%s;packagename=Tier0ChainTests;branch=%s;cmtconfig=%s;project=%s;verbosity=vvv;%%29" % ( relNumber, branch, cmtconfig, project )
  comparisonlogurl = "https://atlas-rtt.cern.ch/prod/rtt/%s/%s/build/%s/%s/Tier0ChainTests/ESDTAGCOMM_comparison/ESDTAGCOMM_comparison_log" % ( relNumber, branch, cmtconfig, project )

  msgbody = """<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML//EN">
<html>
<body>
<h1></h1>
<p style=\"font-family: monospace;\">
The results from the Tier0ChainTests are available <a href=\"%s\">here</a>
<br><br>
(Note that results may be published on the web with a slight delay)
</p>
""" % resultsurl

  if len(sys.argv) >= 3:
    filesNames = sys.argv[2]
    filesList = filesNames.split(',')
    for fileName in filesList:
      if not os.path.exists(fileName): continue
      if 'ESDTAGcomp' in fileName:
          fSummary = open(fileName,'r')
          testSummaryString = """
<p style=\"font-family: monospace;\">
==========================================<br>
= Summary of comparisons (ESD, AOD, TAG) =<br>
==========================================<br>
<br>
(Full log available <a href=\"%s\">here</a>)
</p>
<pre>""" % comparisonlogurl

          lines = [line for line in fSummary]
          lines.sort()
          for line in lines:
              testSummaryString += line
              pass
          testSummaryString += "</pre>"
          continue
      # the rest of the files, if any
      fSummary = open(fileName,'r')
      lSummary = fSummary.readlines()
      isSuccess = False
      for line in lSummary:
        if 'TestCode=SUCCESS' in line: isSuccess = True
        pass
      fileNameList = fileName.split('/')
      shortFileName = fileNameList[len(fileNameList)-1]
      if isSuccess:
        testSummaryString += ("%s = OK\n" % (shortFileName.split('.'))[0])
      else:
        testSummaryString += ("%s = NOT OK\n" % (shortFileName.split('.'))[0])
        pass
      pass
    pass

  msgbody += testSummaryString

  overwriteCheckString = """
<pre>

===================================================================
= Summary of comparisons of TAGs made with and without monitoring =
===================================================================

"""
  if len(sys.argv) >= 4:
    filesNames = sys.argv[3]
    filesList = filesNames.split(',')
    for fileName in filesList:
      if not os.path.exists(fileName): continue
      fOverwrite = open(fileName, 'r')
      for line in fOverwrite: 
        overwriteCheckString += line

  overwriteCheckString += "</pre>\n"
  msgbody += overwriteCheckString

  tctStatusString = "<pre><br>"
  os.system('tct_getstatus.py --release=%s --day=%s --dq --dump' % ( branch, relNumber ) )
  fstatus = open('tctstatus.txt','r')
  for line in fstatus: 

    tctStatusString += line

  tctStatusString += "</pre>"
  msgbody += tctStatusString
  msgbody += "</html>"

  ## send emails
  print "Sending email(s) to: " + emailAddresses
  msg = "From: rtt@mail.cern.ch\nSubject: Finished TCT for nightly: %s\nTo: %s\nContent-Type: text/html\n\n%s\n" % (release, emailAddresses, msgbody)

  recipients = emailAddresses.split(',')
  for recipient in recipients:
    print "Will now send notification email to %s with the following contents:" % recipient
    print msg
    sendMail(recipient, msg)
 
  ## send text messages
  #sendSms("From: rtt@mail.cern.ch\nSubject: test\n")

  print "(the line below is to trick the RTT framework that this \"Athena\" job finished successfully)\n"
  print "ErrorCode=0 (OK)"
