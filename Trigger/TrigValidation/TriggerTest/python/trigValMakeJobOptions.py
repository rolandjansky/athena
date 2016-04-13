#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

import getopt, sys
from TrigValJobOptionBuilder import TrigValJobOptionBuilder

def makeSlice(slice, executable="", config="", iinput="RDO", output = "") :
    TrigValJobOptionBuilder("",slice,"",executable,config,iinput,output).makeJobOption()

def makeAllSlicesOldConfig( executable = "", iinput="RDO",output = "") :
    makeSlice("Electron", executable, "OldConfig", iinput, output)
    makeSlice("Photon"  , executable, "OldConfig", iinput, output)
    makeSlice("Tau"     , executable, "OldConfig", iinput, output)
    makeSlice("Muon"    , executable, "OldConfig", iinput, output)
    makeSlice("Jet"     , executable, "OldConfig", iinput, output)
    makeSlice("MET"     , executable, "OldConfig", iinput, output)
    makeSlice("Bjet"    , executable, "OldConfig", iinput, output)
#    makeSlice("Bphysics", executable, "OldConfig", iinput, output)

def makeAllSlices( executable = "", iinput="RDO",output = "") :
    makeSlice("Electron", executable, "Modern", iinput, output)
    makeSlice("Photon"  , executable, "Modern", iinput, output)
    makeSlice("Tau"     , executable, "Modern", iinput, output)
    makeSlice("Muon"    , executable, "Modern", iinput, output)
    makeSlice("Jet"     , executable, "Modern", iinput, output)
    makeSlice("MET"     , executable, "Modern", iinput, output)
    makeSlice("Bjet"    , executable, "Modern", iinput, output)
#    makeSlice("Bphysics", executable, "Modern", iinput, output)


def makeDetector(detector) :
    TrigValJobOptionBuilder(detector,"","","","Modern").makeJobOption()
    TrigValJobOptionBuilder(detector,"","","","OldConfig").makeJobOption()    


def makeAll() :
    # TrigValJobOptionBuilder (detector = "All", slice = "All", menu = "Default", 
    #                          executable = "Athena", input = "RDO", output = "",
    #                          specialFlags = "", validation = "") :
    # absolute default
    TrigValJobOptionBuilder().makeJobOption()
    TrigValJobOptionBuilder("","","","","OldConfig").makeJobOption()
    # AllAlgs=True
    TrigValJobOptionBuilder("","","","AthenaAllAlgsTrue").makeJobOption()
    TrigValJobOptionBuilder("","","","AthenaAllAlgsTrue","OldConfig").makeJobOption()

    # memleak checks (old config)
    TrigValJobOptionBuilder("","","","","OldConfig","","","memTrack","NODEBUG").makeJobOption()
    TrigValJobOptionBuilder("","","","","OldConfig","","ESDAOD","memTrack","NODEBUG").makeJobOption()
    TrigValJobOptionBuilder("","","","AthenaAllAlgsTrue","OldConfig","","","memTrack","NODEBUG").makeJobOption()
    TrigValJobOptionBuilder("","Electron","","","OldConfig","","","memTrack","NODEBUG").makeJobOption()
    TrigValJobOptionBuilder("","","","AthenaStandalone","OldConfig","","","","").makeJobOption()
    # memleak checks (new config)
    TrigValJobOptionBuilder("","","","","Modern","","","memTrack","NODEBUG").makeJobOption()
    TrigValJobOptionBuilder("","","","","Modern","","ESDAOD","memTrack","NODEBUG").makeJobOption()
    TrigValJobOptionBuilder("","","","AthenaAllAlgsTrue","Modern","","","memTrack","NODEBUG").makeJobOption()
    TrigValJobOptionBuilder("","Electron","","","Modern","","","memTrack","NODEBUG").makeJobOption()
    TrigValJobOptionBuilder("","","","AthenaStandalone","Modern","","","","").makeJobOption()

    # Detector specific
    makeDetector("Calo")
    makeDetector("ID")
    makeDetector("Muon")

    # Slice specific
    makeAllSlices()
    makeAllSlicesOldConfig()    
    makeSlice("Cosmics", "", "OldConfig", "RDO")
    makeSlice("Cosmics", "", "Modern", "RDO")

    # moving Bphys here temporarily because it needs special flags (RPC hack) - RG 23/8
    TrigValJobOptionBuilder("","Bphysics","","","OldConfig","","","hackRPC","").makeJobOption()
    TrigValJobOptionBuilder("","Bphysics","","","Modern","","","hackRPC","").makeJobOption()    
    
    # write ESD
    TrigValJobOptionBuilder("","","","","OldConfig","RDO","ESDAOD","","").makeJobOption()
    TrigValJobOptionBuilder("","","","","Modern","RDO","ESDAOD","","").makeJobOption()    
    makeAllSlices("","RDO","ESDAOD")
    makeAllSlicesOldConfig("","RDO","ESDAOD")    

    #read ESD
    TrigValJobOptionBuilder("","","","","OldConfig","ESD","AOD","","").makeJobOption()
    TrigValJobOptionBuilder("","","","","Modern","ESD","AOD","","").makeJobOption()
    #read ESD
    TrigValJobOptionBuilder("","","","","OldConfig","ESD","","rerunHYPO","").makeJobOption()
    TrigValJobOptionBuilder("","","","","OldConfig","AOD","","rerunHYPO","").makeJobOption()
    TrigValJobOptionBuilder("","","","","Modern","ESD","","rerunHYPO","").makeJobOption()
    TrigValJobOptionBuilder("","","","","Modern","AOD","","rerunHYPO","").makeJobOption()    

    #write BS
    TrigValJobOptionBuilder("","","","","OldConfig","RDO","BS","","").makeJobOption()
    TrigValJobOptionBuilder("","","","","Modern","RDO","BS","","").makeJobOption()
    #read BS (standalone)
    TrigValJobOptionBuilder("","","","AthenaStandalone","OldConfig","BS","","","").makeJobOption()
    TrigValJobOptionBuilder("","","","AthenaStandalone","OldConfig","BStmp","","","").makeJobOption()
    TrigValJobOptionBuilder("","","","AthenaStandalone","Modern","BS","","","").makeJobOption()
    TrigValJobOptionBuilder("","","","AthenaStandalone","Modern","BStmp","","","").makeJobOption()    
    makeAllSlices("AthenaStandalone","BStmp")
    makeAllSlicesOldConfig("AthenaStandalone","BStmp")    

    #standalone tests
    TrigValJobOptionBuilder("","Electron","","","OldConfig","","","memTrack","NODEBUG").makeJobOption()
    TrigValJobOptionBuilder("","Electron","","","Modern","","","memTrack","NODEBUG").makeJobOption()
    


###################################################################################    
def usage(): 
   print """To run the script do :
./TrigValMakeJobOption <options> 

   where  <options> are
        -h , --help            - print this message
        -o , --options         - print all options of TrigValMakeJobBuilder
        -a , --all             - make all jobOptions
        -d , --detector <Det>  - make jobO for a particular detector Calo, ID, Muon 
        -s , --slice <slice>   - make jobO for a particular slice : Electron, Photon, Muon,
                                                                    Tau, Jet, MET, 
                                                                    Bjet, Bphysics, Cosmics
"""
def lastStep():
    print "to copy it is to relase do : "
    print "  mv TriggerTest/* ../share/"
    print "  mv TrigAnalysisTest/* ../../TrigAnalysisTest/share/"
    
def warning() :
   answer = raw_input("This scripts overwrites files in TriggerTest directory. Are you sure you want to proceed (y/n)?")
   if( answer.lower() != "y" and answer.lower() !=  "yes" ) :
      import sys
      sys.exit("Nothing is touched")

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "ds:hoa", ["help", "options", "all=","slice=","detector="])
    except getopt.GetoptError:
        # print help information and exit:
        usage()
        sys.exit(2)
    output = None
    verbose = False
    if( opts == [] ) :
       #usage()
       #sys.exit(2)
       makeAll()
       lastStep()
       sys.exit()
    for o, a in opts:
        if o in ("-h", "--help"):
            usage()
            sys.exit()
        if o in ("--options", "-o"):
            TrigValJobOptionBuilder().help()
            sys.exit()
        if o in ("-s", "--slice"):
            #warning()
            makeSlice(a)
            lastStep()
        if o in ("-d", "--detector"):
            #warning()
            makeDetector(a)
            lastStep()
        if o in ("-a", "--all"):
            #warning()
            makeAll()
            lastStep()
            sys.exit()
    # ...
    
if __name__ == "__main__":
    main()
