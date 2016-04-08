#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# usage: tct_finishedMail.py

import cgi, commands, pickle, sys, string, re, os, smtplib
from datetime import datetime
from OnlineRecoTests.ort_interpretTimestamp import *


def sendSms(msg,toAdd='0041762725254@sms.switch.ch'):
  yourCernMailAdd = 'fct@cern.ch'
  cernSMTPsrv='cernmx.cern.ch'
  s=smtplib.SMTP(cernSMTPsrv)
  toList=[toAdd]
  s.sendmail(yourCernMailAdd,toList,msg)
  print "<br>%s<br>A notification has been sent to the admin<br>" % msg
  s.quit()
  return


def sendMail(msg,toList):
  yourCernMailAdd = 'fct@cern.ch'
  cernSMTPsrv='cernmx.cern.ch'
  s=smtplib.SMTP(cernSMTPsrv)
  s.sendmail(yourCernMailAdd,toList,msg)
  s.quit()
  return


## main
if __name__ == "__main__":

  usageline = "usage: " + sys.argv[0] + " <timestampfile> <emailaddress1,emailaddress2,emailaddress3,etc> [<body>]"
  if (len(sys.argv)<3):
    print usageline
    sys.exit(1)

  release = os.environ["AtlasArea"]
  timestampfile = sys.argv[1]
  eList = sys.argv[2]
  body = ""
  if len(sys.argv)>3:
    body = sys.argv[3]

  newkitInstalled,pacmankey = interpretNewTimestamp(timestampfile)  

  if newkitInstalled:
    ## send emails
    print "Sending emails to: " + eList
    msg = "From: fct@mail.cern.ch\nSubject: Finished ORT: %s\nTo: %s\n\n%s\n" % (release,eList,body)

    toList = eList.split(",")
    for address in toList:
      sendMail(msg,address)

  print "ErrorCode=0 (OK)"

