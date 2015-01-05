#!/usr/bin/env python

import os,commands,sys,logging
import traceback,imp

########################################################
import HistoComparison
import Truth
import extractRec
import checkAODESDcontent
import memleak
import addHTML
########################################################

logLevel=logging.INFO
logger = logging.getLogger('checkT')
hdlr = logging.FileHandler('checkT.log',"w")
formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
hdlr.setFormatter(formatter)
logger.addHandler(hdlr)
logger.setLevel(logLevel)

com = "cat env.log|grep AtlasVersion | cut -d= -f2| tail -n1"
res,release=commands.getstatusoutput(com)


com = "cat env.log|grep AtlasArea | cut -d= -f2| tail -n1"
res,branch=commands.getstatusoutput(com)



com="grep RTTINFO MYRTTINFOS.txt | cut -d= -f2 | tail -n1 | awk '{print $1}'"
res,testName=commands.getstatusoutput(com)
if res!=0 :
    self.logger.error(testName.strip())
    sys.exit(1)



name=testName+'Checks.py';
mod = imp.load_source("tests",name)
import tests


file = open('./checks.html','w')
txt='<html><head><title>RTT results for '+testName.strip()+'</title><link rel=\"StyleSheet\" href=\"rtt.css\" type=\"text/css\" /><meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\"><meta http-equiv=\"Content-Language\" content=\"en\"></head><body><center><h1>RTT results for '+testName.strip()+'</h1></center>'
txt+='<br><br><br><br>'
txt+='<center>'+'Branch : '+branch+'<br>'
txt+='Release : '+release+'<br><br><br>'

if tests.doHistoComparison==True:
    txt+='<a href="DrawHistos.html">Histograms</a>'

if tests.doTruth==True:
    txt+='<br><a href="Truth.html">Truth plots</a>'

if tests.docheckAODESDcontent==True:
    txt+='<br><a href="ESDAOD.log">ESD and AOD dump (in some cases)</a>'

if tests.doaddPlotElectronicNoise==True:
    txt+='<br><a href="ElectronicNoise.html">Electronic noise</a>'

txt+='</center></body></html>'
file.write(txt)
file.close()

logger.info('============= CHECKS BEGIN =================')

 
# Comparison plots
if tests.doHistoComparison:
    logger.info('---- Histograms comparison ----')
    try: 
       a=HistoComparison.HistosComparison(logger,logging.info)
       res=a.run()
    except:
       traceback.print_exc()
       logger.error('execution of HistoComparison failed')
       res=10
    if res==0:
        logger.info('Histograms comparison done successfully')
    else:
        logger.info('Histograms comparison failed')

# Truth
if tests.doTruth:
    logger.info('---- Truth ----')
    try:
       a=Truth.Truth(logger,logging.INFO)
       res=a.run()
    except:
       traceback.print_exc()
       logger.error('execution of Truth failed')
       res=10

    if res==0:
        logger.info('Truth done successfully')
    else:
        logger.info('Truth failed')

# CPU time
if tests.doextractRec:
    logger.info('---- CPU time ----')
    try: 
       a=extractRec.extractRec(logger,logging.INFO)
       res=a.run()
    except:
       traceback.print_exc()
       logger.error('execution of extractRec failed')
       res=10

    if res==0:
        logger.info('extractRec done successfully')
    else:
        logger.info('extractRec failed')

# Memory leaks
if tests.doMemLeak:
    logger.info('---- Memory leak ----')
    try:
       a=memleak.memleak(logger,logging.INFO)
       res=a.run()
    except:
       traceback.print_exc()
       logger.error('execution of memleak failed')
       res=10

    if res==0:
        logger.info('MemLeak done successfully')
    else:
        logger.info('MemLeak failed')

# AOD, ESD size
if tests.docheckAODESDcontent:
    logger.info('---- checks of AOD, ESD ----')
    try:
       a=checkAODESDcontent.checkAODESDcontent(logger,logging.INFO)
       res=a.run()
    except:
      traceback.print_exc()
      logger.error('execution of checkAODESDcontent failed')
      res=10

    if res==0:
        logger.info('checkAODESDcontent done successfully')
    else:
        logger.info('checkAODESDcontent failed')

# add extra plots
if tests.doaddPlotElectronicNoise==True:
    logger.info('---- add plot of Electronic Noise ----')
    try:
       a=addHTML.addHTML(logger,logging.INFO,'plotAllCaloNoise13','ElectronicNoise')
       res=a.run()
    except:
      traceback.print_exc()
      logger.error('execution of addPlotElectronicNoise failed')
      res=10

    if res==0:
        logger.info('addPlotElectronicNoise done successfully')
    else:
        logger.info('addPlotElectronicNoise failed')        


logger.info('============= CHECKS END =================')
