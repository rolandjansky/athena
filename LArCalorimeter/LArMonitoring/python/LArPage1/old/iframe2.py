#!/usr/bin/env python2.5

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
print 'Content-type: text/html;charset=utf-8\n'

########################################################################
import os,re,time,sys,cgi,string
import cgitb; cgitb.enable()
import sys,os,getopt,fileinput
import subprocess as sp
import test
import xmlrpclib
global server
#passfile = open("/afs/cern.ch/user/l/larmon/public/WebDisplayExtractor/passfile.txt")
#passwd = passfile.read().strip(); passfile.close()
#server = xmlrpclib.ServerProxy('https://%s@atlasdqm.cern.ch'%(passwd))
server = xmlrpclib.ServerProxy('https://trocme:ac1f0e5c-9f21-4b58-aa4b-eb2d69d57d1d@atlasdqm.cern.ch')

from threading import Thread
from time import *
from threading import Thread
import os
#import GetRunInfo
#from GetRunInfo import *

########################################################################
f = cgi.FieldStorage()


########################################################################
def TestImport():
    print 'hello'

########################################################################
def GetRunNumber():
    #f = cgi.FieldStorage()
    run = 0
    for i in f.keys(): run = f[i].value

    return run

def ReturnCleanRunInfo(output_string):
  tmp = output_string
  tmp = tmp.replace("{","")
  tmp = tmp.replace("}","")
  #print tmp
  NINFO = 12
  separators={}
  for i in range(0,NINFO):
      separators[i]=", "+str(NINFO-1-i)+": "

  separators[NINFO-1]="0: "
  #print 'separators',separators
      
##   separators[11]="RUN: "
##   separators[10]="start of run "
##   separators[9]="end of run "
##   separators[8]="end of calib loop "
##   separators[7]="number of LBs: "
##   separators[6]="nLBReady: "
##   separators[5]="from: "
##   separators[4]="to: "
##   separators[3]="number of colliding bunches:"
##   separators[2]="Peak lumi: "
##   separators[1]="Delivered lumi (approx.) "
##   separators[0]="CosmicCalo events: "

  info={}
  
  for i in range(0,NINFO):
    tmp2 = tmp.partition(separators[i])
    tmp = tmp2[0]
    #print tmp2[2]
    info[NINFO-1-i]=tmp2[2].replace("'","")

  #for i in range(0,len(info)):
  #    print info[i]

  return info

  #print "info: ",info
  #for i in range(0,NINFO-1):
  #    print info[i]
  #final = str(info)
  #final = final.replace("{","")
  #final = final.replace("}","")
  #final = final.replace("\n","")
  #final = final.replace("'","")
  #final = final.replace('''"''',"")
  #final = final.replace('''1 :''',"")
  
  #return final
  #final.replace(

## ########################################################################
## def CollectInfo():
##     info={}
##     info[0]=GetRunNumber()
##     tmp_info = GetRunInfo(info[0])

##     for i in range(0,len(tmp_info)-1): info[i+1]=tmp_info[i]
##     print info

########################################################################
print '''<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<title>LADIeS Iframe</title>
<meta name="author" content="Olivier Davignon [davignon@cern.ch]"/>
<link rel="icon" type="image/vnd.microsoft.icon" href="../favicon.ico" />
<link rel="Stylesheet" href="styles.css" type="text/css" />

<base target="_blank">'''
print '''</head>'''
print '''<body onload="">
'''
#for i in f.keys(): print "run:",f[i].value
## print '''<br />
##    - <b>END OF CALIB LOOP: Sun Aug 7 03:47:00 2011:</b><br />
##    - Run start: Fri Aug 5 05:14:27 2011</br />
##    - Run stop: Fri Aug 5 15:47:00 2011<br />
##    - Number of LB: 647<br />
##    - LB (ATLAS ready): 510 ( 138 - 647 )<br />
##    - Number of colliding bunches: 1317<br />
##    - Peak Lumi: 1987.25744629 e30 cm-2.s-1<br />
##    - Approx delivered luminosity: 52.6160678369 pb-1<br />
##    '''
cmd="export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup;"
cmd+="source $AtlasSetup/scripts/asetup.sh 16.6.5,32,here,builds;"
cmd+="python /afs/cern.ch/user/l/larmon/public/LArPage1/scripts/GetRunInfo.py "+str(GetRunNumber())
#print GetRunNumber()
TestNumber = sp.Popen(cmd,shell=True,stdout=sp.PIPE,stderr=sp.PIPE)
output =  TestNumber.communicate()[0]
#output = output.replace("['ATLF', 'ATLAS_COOLPROD', 'atlas_dd'] __Warning: use COOL_DISABLE_CORALCONNECTIONPOOLCLEANUP__ ['ATLF', 'ATLAS_COOLPROD', 'atlas_dd']","")
output = output.replace("['ATLF', 'ATLAS_COOLPROD', 'atlas_dd']","")
output = output.replace("__Warning: use COOL_DISABLE_CORALCONNECTIONPOOLCLEANUP__","")
print '''<b>More information about run: '''+GetRunNumber()+'''</b>'''
#print 'before '+str(output)
displays = {}
displays[0]="Run number:"
displays[1]="<br />Run start:"
displays[2]="<br />Run stop:"
displays[3]="<br />Run end of calib loop:"
displays[4]="<br />Number of LB:"
displays[5]="<br />LB (ATLAS ready):"
displays[6]="("
displays[7]="-"
displays[8]=")"
displays[9]="<br />Number of colliding bunches:"
displays[10]="<br />Peak luminosity:"
displays[11]="<br />Approx. integrated luminosity:"

infos = ReturnCleanRunInfo(output.replace("\n",""))
print displays[1],infos[1],""
print displays[2],infos[2],""
print displays[3],infos[3],""
print displays[4],infos[4],""
print displays[5],infos[5],displays[6]+infos[6]+displays[7]+infos[7]+displays[8]
print displays[9],infos[8],""
if infos[9]!="": print displays[10],round(float(infos[9]),0)," e30 cm-2.s-1"
else: print displays[10]," no peak lumi"
#if infos[10]!="": print displays[11],str(round(float(infos[10])/(100.*100.*1000.),2))," pb-1"
#else: print displays[11]," no lumi<br />"
print'''</body></html>'''
