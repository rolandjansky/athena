# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys, os, os.path, time
from datetime import datetime

# *********************************************************************************
# Author  : B.Simmons
# Date    : 29th August 2007
# Purpose : Checks two aspects of the copier:
#              a) that the last copier report is recent (indicates the copier is running)
#              b) that the proxy time left is great enough (else sends mail)
#           Reads in the datacopier report to figure both these out.
# Usage   : python dataCopierWatcher.py
# *********************************************************************************

# --- Global variables

copierReport = '/afs/cern.ch/atlas/project/RTT/Results/DatasetManager.report'
copierReportMaxAge = 1 # copier report should be no more than 1 day old
proxyMinTimeLimit = 24*5 # 5 days advanced warning of proxy expiry

# ----------------------------------------------------------------

def sendEmail(emailTitle, emailContent):
    """A simple mailer"""
    if not emailContent.strip():
        print 'All OK, no email was sent.'
        return
    
    def at(): return '@'
    def dot(): return '.'
    
    to = 'brinick' + dot() + 'simm' + 'ons' + at() + 'ce' + 'rn' + dot() + 'c' + 'h'
    
    subject = emailTitle
    command = 'mail ' + str(to) + ' -s ' + '"' + str(subject) + '"'
    os.popen(command,'w').write(emailContent)
    print 'An email has been sent to %s' % to
    print '------------------'
    
# ----------------------------------------------------------------

def readCopierReport():
    try:
        handle = open(copierReport)
        conts = handle.readlines()
        handle.close()
    except Exception, e:
        title   = 'WARNING: unable to read in the data copier report!'
        message = str(e)
        sendEmail(title, message)
        print 'Unable to read in the data copier report.'
        print str(e)
        sys.exit(0)

    return conts

# ----------------------------------------------------------------

def grabProxyInfo(conts):
    date = grabDateOfReport(conts)
    timeLeftInHours, timeLeftInDays = grabProxyTimeLeft(conts)
    return (date, timeLeftInHours, timeLeftInDays)

# ----------------------------------------------------------------

def grabDateOfReport(conts):
    for line in conts:
        if line.find('DataSetManager Report')!=-1:
            date = line[line.find('DataSetManager Report')+len('DataSetManager Report '):].strip()
            return date
    return ''

# ----------------------------------------------------------------

def grabProxyTimeLeft(conts):
    for line in conts:
        if line.find('Proxy time left in Hours')!=-1:
            timeLeftInHours  = int(line.split(':')[2])
            timeLeftInDays = str(timeLeftInHours/24.0)
            timeLeftInDays = timeLeftInDays[:timeLeftInDays.find('.')+2]            
            return (timeLeftInHours, timeLeftInDays)
    return ('', '')

# ----------------------------------------------------------------

def isDateTooOld(theDate):
    months = {'Jan':1,'Feb':2,'Mar':3,'Apr':4,'May':5,'Jun':6,'Jul':7, 'Aug':8, 'Sep':9, 'Oct':10, 'Nov':11, 'Dec':12}
    dDayName, dMonth, dDay, dTime, dYear = theDate.split()
    ddMonth = months[dMonth]
    dateInst = datetime(int(dYear),ddMonth,int(dDay))

    nowTuple = time.gmtime()
    nYear  = nowTuple[0]
    nMonth = nowTuple[1]
    nDay   = nowTuple[2]
    nowInst = datetime(nYear,nMonth,nDay)

    timeDeltaInst = nowInst - dateInst
    return timeDeltaInst.days > copierReportMaxAge

# ----------------------------------------------------------------

def checkDataCopierReport():
    conts = readCopierReport()
    date = grabDateOfReport(conts)
    tooOld = isDateTooOld(date)
    if tooOld:
        title   = 'WARNING: data copier manager report out of date!'
        message = 'Data copier manager report seems out of date (last stamp: %s). Is copier working?' % date
        print 'Checking data copier report age.....FAILED (last report from: %s)' % date
        sendEmail(title, message)
    else:
        print 'Checking data copier report age.....PASSED OK' 
        
# ----------------------------------------------------------------

def checkDataCopierProxy():
    conts = readCopierReport()
    dateOfReport, proxyTimeHours, proxyTimeDays = grabProxyInfo(conts)
    if proxyTimeHours < proxyMinTimeLimit:
        title   = 'WARNING: data copier proxy expires soon!'
        message = 'At %s, proxy had only %s days left (%d hours)' % (dateOfReport, proxyTimeDays, proxyTimeHours)
        print 'Checking data copier proxy time left.....FAILED (only %d hours left)' % proxyTimeHours
        sendEmail(title, message)
    else:
        print 'Checking data copier proxy time left.....PASSED OK'

# ----------------------------------------------------------------
# ----------------------------------------------------------------
# ----------------------------------------------------------------

if __name__ == '__main__':
    checkDataCopierReport()
    checkDataCopierProxy()
