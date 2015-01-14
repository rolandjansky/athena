#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import sys, getopt
from PyCool import cool
from CoolConvUtilities.AtlCoolLib import indirectOpen
import MyCOOLlib
from MyCOOLlib import *
import os
from os import open
import string
from ROOT import *
from ROOT import gStyle, TCanvas, TString, TPad
import time
import urllib2,sys
import xmlrpclib
import subprocess as sp
from DQDefects import DefectsDB

import os,re,time,sys,cgi,string
import cgitb; cgitb.enable()
import os,fileinput
## import test
import xmlrpclib
global server
from threading import Thread
## from time import *
## import os



server = xmlrpclib.ServerProxy('http://atlasdqm.cern.ch') 


############################################################
def CheckDefects(run):
    outputstring = ""
    db = DefectsDB()
    lar_defects = [d for d in (db.defect_names | db.virtual_defect_names) if d.startswith("LAR")]
    defects = db.retrieve((run, 1), (run+1, 0), lar_defects)
    here_ES = 0
    here_BULK = 0
    here_SIGNOFF = 0
    never_here_ES = 1
    never_here_BULK = 1
    never_here_SIGNOFF = 1
    uploaded = 0
    
    for defect in defects:
        print defect.since.run,defect.since.lumi,defect.until.lumi,"defect channel:",defect.channel
        print "defect present: ",defect.present
        if "LAR_UNCHECKED" in defect.channel and not here_ES and defect.present!=False:
            here_ES = 1
        if "LAR_UNCHECKED" not in defect.channel:
            never_here_ES += 1
        if "LAR_BULK_UNCHECKED" in defect.channel and not here_BULK and defect.present!=False:
            here_BULK = 1
        if "LAR_BULK_UNCHECKED" not in defect.channel:
            never_here_BULK += 1
        if "LAR_UNCHECKED_FINAL" in defect.channel and not here_SIGNOFF and defect.present!=False:
            here_SIGNOFF = 1
        if "LAR_UNCHECKED_FINAL" not in defect.channel:
            never_here_SIGNOFF += 1

    #number of default defects
    if len(defects)==0 or len(defects)<5: uploaded = 0
    else: uploaded = 1

    if len(defects)==0:
        outputstring+="UP:0"
        outputstring+=",ES:0"
        outputstring+=",BULK:0"
        outputstring+=",SIGNOFF:0"

    if uploaded==0: outputstring+="UP:0"
    else: outputstring+="UP:1"
    
    if here_ES==1 and never_here_ES!=len(defects): outputstring+=",ES:0"
    else: outputstring+=",ES:1"

    if here_BULK==1 or never_here_BULK==len(defects): outputstring+=",BULK:0"
    else: outputstring+=",BULK:1"    

    if here_SIGNOFF==1 or never_here_SIGNOFF==len(defects): outputstring+=",SIGNOFF:0"
    else: outputstring+=",SIGNOFF:1"
    
    return outputstring

############################################################
def NumberOfProcEvents(runnumber,processing):#1: ES, 2:bulk
  source = 'tier0'
  stream = 'physics_CosmicCalo'
  proc = processing

  multicall = xmlrpclib.MultiCall(server)
  run_spec = {'stream': 'physics_CosmicCalo', 'source': 'tier0', 'low_run': runnumber, 'high_run' : runnumber, 'proc_ver' : processing, }
  dic = server.get_hist_nentries(run_spec, 'LAr/LAR_GLOBAL/Run_Parameters/NbOfEventsVsLB')

  if str(runnumber) in dic: return int(dic[str(runnumber)])
  else:
    run_spec = {'stream': 'tmp_physics_CosmicCalo', 'source': 'tier0', 'low_run': str(runnumber), 'high_run' : str(runnumber), 'proc_ver' : processing, }
    dic = server.get_hist_nentries(run_spec, 'LAr/LAR_GLOBAL/Run_Parameters/NbOfEventsVsLB')
    if str(runnumber) in dic: return int(dic[str(runnumber)])
    else: return 0

############################################################
def GetSFO_LBNEvents( cursor, runno, streamname ):
    """returns nfiles, fsize, nevents"""
    stype, sep, sname = streamname.partition('_')
    cursor.execute( "SELECT SUM(NREVENTS),LUMIBLOCKNR FROM SFO_TZ_File WHERE RUNNR=:arg_1 and STREAMTYPE=:arg_2 and STREAM=:arg_3 GROUP BY LUMIBLOCKNR ORDER BY LUMIBLOCKNR",
                    arg_1=runno, arg_2=stype, arg_3=sname )
    return cursor.fetchall()

############################################################
def GetSFO_LBs( cursor, runno ):
    cursor.execute( "SELECT MIN(LUMIBLOCKNR), MAX(LUMIBLOCKNR) FROM SFO_TZ_Lumiblock WHERE RUNNR=:arg_1", arg_1=runno )
    return cursor.fetchone()[0:2]

############################################################
def OpenSFOConnection():
    import cx_Oracle
    return cx_Oracle.connect("ATLAS_SFO_T0_R/readmesfotz2008@atlr")

############################################################
def GetEvents(runnumber):

    events = 0

    connection = OpenSFOConnection()
    cursor     = connection.cursor()

    lblist = GetSFO_LBNEvents( cursor, runnumber, "physics_CosmicCalo" )
    if len(lblist)==0:
        return
    else:
        for i in range(0,len(lblist)):
            events += lblist[i][0]
    
    cursor.close()
    connection.close()

    return events

###############
def CheckCAFprocessing(run):

    dicCAF={}

##     cmd_ES = "export  CAFMGRPATH=/afs/cern.ch/user/l/larmon/public/LArPage1/scripts/CAFManager;"
##     cmd_ES += "export PATH=$PATH:$CAFMGRPATH; "
##     cmd_ES += "source statusCAF.sh "+str(run)+" | grep runCleaning_00"+str(run)
##     cout_CAF_cells_ES = sp.Popen(cmd_ES,shell=True,stdout=sp.PIPE,stderr=sp.PIPE)
##     output_CAF_cells_ES = cout_CAF_cells_ES.communicate()[0]
##     if "SUCCESS" in output_CAF_cells_ES: dicCAF["ESCleaning"]='SUCCESS'
##     elif "RUNNING" in output_CAF_cells_ES: dicCAF["ESCleaning"]='RUNNING'
##     elif "SCHEDULED" in output_CAF_cells_ES: dicCAF["ESCleaning"]='SCHEDULED'
##     elif "FAILED" in output_CAF_cells_ES: dicCAF["ESCleaning"]='FAILED'
##     else: dicCAF["ESCleaning"]='NOT LAUNCHED YET:'+output_CAF_cells_ES

    cmd_LCE = "export  CAFMGRPATH=/afs/cern.ch/user/l/larmon/public/LArPage1/scripts/CAFManager;"
    cmd_LCE += "export PATH=$PATH:$CAFMGRPATH; "
    cmd_LCE += "source statusCAF.sh "+str(run)+" | grep larCellsEmpty2011_00"+str(run)
    cout_LCE = sp.Popen(cmd_LCE,shell=True,stdout=sp.PIPE,stderr=sp.PIPE)
    output_LCE = cout_LCE.communicate()[0]
    if output_LCE.count("SUCCESS")>=14: dicCAF["LCE"]='SUCCESS'
    elif output_LCE.count("SUCCESS")>13 and output_LCE.count("SUCCESS")<14 and output_LCE.count("FAILED")<2: dicCAF["LCE"]='SUCCESS'
    elif output_LCE.count("FAILED")>5: dicCAF["LCE"]='FAILED'
    elif output_LCE.count("SCHEDULED")>5: dicCAF["LCE"]='SCHEDULED'
    elif output_LCE.count("RUNNING")>0: dicCAF["LCE"]='RUNNING'
    else: dicCAF["LCE"]='NOT LAUNCHED YET'#+output_LCE

##     if "runCleaning          SUCCESS" in output_LCE: dicCAF["ESCleaning"]='SUCCESS'
##     else: dicCAF["ESCleaning"]='NOT LAUNCHED YET'
##     dicCAF["BULKCleaning"]='NOT LAUNCHED YET'


    cmd_ES = "export  CAFMGRPATH=/afs/cern.ch/user/l/larmon/public/LArPage1/scripts/CAFManager;"
    cmd_ES += "export PATH=$PATH:$CAFMGRPATH; "
    cmd_ES += "source statusCAF.sh "+str(run)+" | grep runCleaning_00"+str(run)
    cout_ES = sp.Popen(cmd_ES,shell=True,stdout=sp.PIPE,stderr=sp.PIPE)
    output_ES = cout_ES.communicate()[0]
    if "SUCCESS" in output_ES: dicCAF["ESCleaning"]='SUCCESS'
    elif "RUNNING" in output_ES: dicCAF["ESCleaning"]='RUNNING'
    elif "SCHEDULED" in output_ES: dicCAF["ESCleaning"]='SCHEDULED'
    elif "FAILED" in output_ES: dicCAF["ESCleaning"]='FAILED'
    else: dicCAF["ESCleaning"]='NOT LAUNCHED YET'+output_ES

    cmd_BULK = "export  CAFMGRPATH=/afs/cern.ch/user/l/larmon/public/LArPage1/scripts/CAFManager;"
    cmd_BULK += "export PATH=$PATH:$CAFMGRPATH; "
    cmd_BULK += "source statusCAF.sh "+str(run)+" | grep runCleaningBulk"
    cout_BULK = sp.Popen(cmd_BULK,shell=True,stdout=sp.PIPE,stderr=sp.PIPE)
    output_BULK = cout_BULK.communicate()[0]
    if "SUCCESS" in output_BULK: dicCAF["BULKCleaning"]='SUCCESS'
    elif "RUNNING" in output_BULK: dicCAF["BULKCleaning"]='RUNNING'
    elif "SCHEDULED" in output_BULK: dicCAF["BULKCleaning"]='SCHEDULED'
    elif "FAILED" in output_BULK: dicCAF["BULKCleaning"]='FAILED'
    else: dicCAF["BULKCleaning"]='NOT LAUNCHED YET'+output_BULK
    
##     cmd_BULK = "export  CAFMGRPATH=/afs/cern.ch/user/l/larmon/public/LArPage1/scripts/CAFManager;"
##     cmd_BULK += "export PATH=$PATH:$CAFMGRPATH; "
##     cmd_BULK += "source statusCAF.sh "+str(run)+" | grep runCleaningBulk_00"+str(run)
##     cout_CAF_cells_BULK = sp.Popen(cmd_BULK,shell=True,stdout=sp.PIPE,stderr=sp.PIPE)
##     output_CAF_cells_BULK = cout_CAF_cells_BULK.communicate()[0]
##     if "SUCCESS" in output_CAF_cells_BULK: dicCAF["BULKCleaning"]='SUCCESS'
##     elif "RUNNING" in output_CAF_cells_BULK: dicCAF["BULKCleaning"]='RUNNING'
##     elif "SCHEDULED" in output_CAF_cells_BULK: dicCAF["BULKCleaning"]='SCHEDULED'
##     elif "FAILED" in output_CAF_cells_BULK: dicCAF["BULKCleaning"]='FAILED'
##     else: dicCAF["BULKCleaning"]='NOT LAUNCHED YET'

    return dicCAF


############################################################
def BadCellListProduced(run):
    #filename = "/afs/cern.ch/user/l/larmon/public/WebDisplayExtractor/LCE/BadCellList_run"
    #filename += str(run)
    #filename += ".txt"
    
    cmd_BCL = "ls -ltr /afs/cern.ch/user/l/larmon/public/WebDisplayExtractor/LCE/BadCellList_run"
    cmd_BCL += str(run)
    cmd_BCL += ".txt"
    cout_BCL = sp.Popen(cmd_BCL,shell=True,stdout=sp.PIPE,stderr=sp.PIPE)
    output_BCL = cout_BCL.communicate()[0]

    #print "HERE",output_BCL

    #fichier = open("/afs/cern.ch/user/l/larmon/public/WebDisplayExtractor/LCE/BadCellList_run187815.txt", "r")
    #fichier = open("test.txt","r")

    cmd_BCL_content = "more /afs/cern.ch/user/l/larmon/public/WebDisplayExtractor/LCE/BadCellList_run"
    cmd_BCL_content += str(run)
    cmd_BCL_content += ".txt"
    output_BCL_content = ""
    
    if "No such file or directory" not in output_BCL_content:
        cout_BCL_content = sp.Popen(cmd_BCL_content,shell=True,stdout=sp.PIPE,stderr=sp.PIPE)
        output_BCL_content = cout_BCL_content.communicate()[0]
    
    present = ""

    if "No such file or directory" in output_BCL or output_BCL_content=="":
        present = "NO"
    else:
        present = "YES"
    
    return present
    

############################################################
def BasicInfo(run):

    run = int(run)
    LBTS = MyCOOLlib.GetLBTimeStamps(run)
    atlasready=MyCOOLlib.GetReadyFlag(run,LBTS)
    readylist=[lb+1 for lb in xrange(len(atlasready)) if atlasready[lb]>0 ]
    LBs = [lb for lb in xrange(len(LBTS))]
    lumi = float(GetTotalLuminosity(run,LBs,readylist))/(10000.*1000.)
    EndCalibLoop=time.asctime(time.gmtime(LBTS[len(LBTS)-1]+2.*3600.+48.*3600.))#end of calib loop
    CAFprocessing_dic = CheckCAFprocessing(run)

    endstring = "ListOfCells:"+BadCellListProduced(run)+", EndCalibLoop:"+str(EndCalibLoop)+", IntegratedLumi:"+str(round(lumi,2))+", ESProcEvents:"+str(NumberOfProcEvents(run,1))+", BULKProcEvents:"+str(NumberOfProcEvents(run,2))+", TotalEvents:"+str(GetEvents(run))+", ESCleaning:"+str(CAFprocessing_dic["ESCleaning"])+", BULKCleaning:"+str(CAFprocessing_dic["BULKCleaning"])+", LCE:"+str(CAFprocessing_dic["LCE"])+", Defects:"+CheckDefects(run)

    return endstring
  
if __name__ == "__main__":

    run = sys.argv[1]
##     dbSvc = cool.DatabaseSvcFactory.databaseService()
##     dbstring="COOLONL_TDAQ/COMP200"
##     db=indirectOpen('COOLONL_TDAQ/COMP200',oracle=True)
    
##     if (db is None):
##         print "ERROR: Cannot connect to COOLONL_TDAQ/COMP200"
##         sys.exit(-1)
    
 ##   folderLB_Params=db.getFolder('/TDAQ/RunCtrl/SOR_Params')
    ThisRunInfo = BasicInfo(run)
    print ThisRunInfo
