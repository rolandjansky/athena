#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import sys, getopt
from PyCool import cool,coral
from CoolConvUtilities.AtlCoolLib import indirectOpen
import MyCOOLlib
from MyCOOLlib import *
import os
import string
from ROOT import *
from ROOT import gStyle, TCanvas, TString, TPad
import time
import urllib2,sys

def CalibLoop(run):

    run = int(run)
    LBTS = MyCOOLlib.GetLBTimeStamps(run)
    info=time.asctime(time.gmtime(LBTS[len(LBTS)-1]+2.*3600.+36.*3600.))#end of calib loop

    return str(info)
  
if __name__ == "__main__":

    run = sys.argv[1]
##     dbSvc = cool.DatabaseSvcFactory.databaseService()
##     dbstring="COOLONL_TDAQ/COMP200"
##     db=indirectOpen('COOLONL_TDAQ/COMP200',oracle=True)
    
##     if (db is None):
##         print "ERROR: Cannot connect to COOLONL_TDAQ/COMP200"
##         sys.exit(-1)
    
 ##   folderLB_Params=db.getFolder('/TDAQ/RunCtrl/SOR_Params')
    ThisRunInfo = CalibLoop(run)
    print ThisRunInfo
