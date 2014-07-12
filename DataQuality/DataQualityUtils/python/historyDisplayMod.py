# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import string, os
import re, commands 
## Needed to correct ROOT behavior; see below
CWD = os.getcwd()

import sys
import pdb

import ROOT
## Importing gSystem may change the current directory to one of the
## command-line arguments; chdir to original directory to have
## predictable behavior

from ROOT import gSystem, gROOT
os.chdir(CWD)

## Import the ROOT library for reading han results
gSystem.Load('libDataQualityUtils')
from ROOT import dqutils

from PyCool import cool, coral
##sys.path.append('../DataQuality/DataQualityUtils/python')
from DataQualityUtils.DBInfo_Histo import *

connectString  = "sqlite://" +  getWriteServer()
connectString += ";schema=" +   getSchema()
connectString += ";dbname=" +   getDbName()

folderName = getFolderH()
metadata = getChannelDictH()

def runlist( xml ):

#initial and final run 
    global runsince, rununtil
# time stamps
    global timesince, timeuntil
    timesince = 0
    timeuntil=(2 << 31)-1
    
    g=open(xml,'r')
    f=open('runxml','w')
    mydict={}
    while 1:
        line = g.readline()
        if not line:
            break
        if line.find('<html>') > -1:
            gg = line.split('/run_')[1]
            nn = gg.split('</html>')[0]            
            try:
                n = int(nn)                
                mydict[nn]=n
            except: 
                n = 0
# find last run on the list
    rununtil = 0
    for elem in mydict.keys():
        if (int(elem)>rununtil):
            rununtil=int(elem)
# find first run on the list
    runsince = 9999999
    for elem in mydict.keys():
        if (int(elem)<runsince):
            runsince=int(elem)

    for elem in mydict.keys():
        f.write(elem+"\n")
    f.close()
    g.close()
    return runsince, timesince, rununtil, timeuntil

import resource
def cpu_time():
    return resource.getrusage(resource.RUSAGE_SELF)[0]
    
def historyDisplay( xml, htmlDirectory, dbConnectionHisto="", dqmfOflHisto="", dbTagName="", configuration=""):

    if (len(dbConnectionHisto)<1):
        dbConnectionHisto = "sqlite://;schema=/afs/cern.ch/user/a/atlasdqm/w0/users/onofrio/DQMF_08112008/14.2.10/WorkArea/python/MyCOOL_histo_FDR2c.db;dbname=OFLP200"
    if (len(dqmfOflHisto)<1):      dqmfOflHisto      = "/GLOBAL/DETSTATUS/DQMFOFLH"
    if (len(dbTagName)<1):         dbTagName      = "DetStatusDQMFOFLH-FDR2-02"

    runlist( xml )
    f=open('runxml','r')        
    histodir=htmlDirectory+"history_plot"
    if( not os.access(histodir,os.F_OK) ):
      try:
        os.makedirs(histodir)
      except os.error:
        print 'Cannot create directory "' + histodir + '"; exiting.'
        sys.exit(-1)
    print histodir

    g=open(histodir+'/index.html','w')
    g.write('<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">\n')
    g.write('<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">\n')
    g.write('<head>\n')
    g.write('<meta http-equiv="Content-Type" content="text/html;charset=utf-8" />\n')
    g.write('<font=+1> <font COLOR="green"><b> History plots </font></b></font> \n')
    g.write('</head>')    
    g.write('<body>')    
    g.write('<ul>\n')

# dump all info from COOL to a root file
    comm = 'AtlCoolCopy.exe "%s" dqmfoflh_%s.root -root -folder %s -tag %s -rls %i %i -rlu %i %i ' % (dbConnectionHisto,configuration,dqmfOflHisto,dbTagName, runsince,timesince, rununtil+1,timeuntil)
    print comm
    commands.getstatusoutput(comm)

    of = dqutils.HistogramDataCOOL( dbConnectionHisto,dqmfOflHisto,0,0,0,0 )
    for key in metadata:
        channel = metadata[key]        
        root_file = 'dqmfoflh_'+configuration+'.root'
        if(os.access(root_file,os.F_OK) ):            
            of.ntupleDB(channel,key,configuration, runsince, rununtil)
        else:
            print ' I did not find the root file, accessing DB one by one'
            of.historyDB(channel,key,dbTagName)
            
        sep = key.split('/')
        riu = '-'.join(sep)
        if (riu.find('-->')>-1):
            hh = riu.split('-->')
            gg = '-'.join(hh)
            riu = gg
           
        movetoweb = 'mv c1.gif '+histodir+'/h'+str(riu)+'.gif'
        makeroot = 'mv c1.root '+histodir+'/h'+str(riu)+'.root'

        commands.getstatusoutput(movetoweb)
        commands.getstatusoutput(makeroot)

        g.write("<li> <a href="+"\"" +"h"+str(riu)+".gif"+"\""+ "> "+ str(key) + "</a> \n </li>")

    g.write('</ul>\n')       
    g.write('</body>\n</html>')
    g.close()

