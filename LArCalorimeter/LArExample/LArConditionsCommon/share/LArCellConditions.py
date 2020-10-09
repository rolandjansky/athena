#!/bin/env python

##=======================================================================================
## Name:        LArCellConditions.py
##
## Description: Python script using athena infrastructure to display database information 
##              about LAr Cells:
##                - Converts online <-> offline identifiers
##                - Displays bad channel status
##                - Optionally displays some basic calibration constants
##                - Can search for (bad) channels in a given detector region
##========================================================================================


import os,sys,getopt
fhistory = os.path.expanduser("~/.LArCellConditionsHist")


def usage():
    print(sys.argv[0]+": Convert and expand LAr Identifiers, print some database content")
    print("Options:")
    print("-c Print also (some) electronic calibration constants")
    print("-s Use SingleVersion folders (default)")
    print("-m Use MultiVersion folders (the opposite of -s)")
    print("-g Include geometrical position (true eta/phi)")
    print("-d Include DSP thresholds")
    print("-r <run> Specify a run number")
    print("-t <tag> Specify global tag")
    print("--detdescr <DetDescrVersion>")
    print("--sqlite <sqlitefile>")
    print("-h Print this help text and exit")
        
try:
    opts,args=getopt.getopt(sys.argv[1:],"csmgdhr:t:",["help","detdescr=","sqlite="])
except Exception as e:
    usage()
    print(e)
    sys.exit(-1)

    
printCond=False
run=None
tag=None
sv=True
geo=False
dspth=False
detdescrtag="ATLAS-R2-2016-01-00-01"
detdescrset=False
sqlite=""

for o,a in opts:
    if (o=="-c"): printCond=True
    if (o=="-t"): tag=a
    if (o=="-r" and a.isdigit()): run=int(a)
    if (o=="-s"): sv=True
    if (o=="-m"): sv=False
    if (o=="-g"): geo=True
    if (o=="-d"): dspth=True
    if (o=="-h" or o=="--help"):
        usage()
        sys.exit(0)
    if (o=="--detdescr"):
        detdescrtag=a
        detdescrset=True
    if (o=="--sqlite"): sqlite=a
    
import readline

try:
    if run is None:
        defRun=0x7fffffff
        prompt= "Enter run number [%i]:" % defRun
        runIn=input(prompt).strip()
        if runIn=="":
            run=defRun
        else:
            if runIn.isdigit():
                run=int(runIn)
            else:
                usage()
                print("Expect numerical parameter for run, got",runIn)
                sys.exit(0)
                pass
            pass
        pass

    if tag is None:
        if (run>222222):
            defTag="CONDBR2-BLKPA-2014-00"
        else:
            defTag="COMCOND-BLKPA-RUN1-06"
            pass
        prompt= "Enter conditions tag [%s]:" % defTag
        tagIn=input(prompt).strip()
        if tagIn=="":
            tag=defTag
        else:
            tag=tagIn


    if geo and not detdescrset:
        prompt="Enter DetectorDescripton tag [%s]:" % detdescrtag
        detdescrtagIn=input(prompt).strip()
        if detdescrtagIn != "":
            detdescrtag=detdescrtagIn
                
except:
    print("Failed to get run number and/or conditions tag")
    sys.exit(0)


#Don't let PyRoot open X-connections
sys.argv = sys.argv[:1] + ['-b'] 


#Now inport the athena stuff
from AthenaCommon.Include import Include, IncludeError, include
include.setShowIncludes(0)
from AthenaCommon import CfgMgr
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ToolSvc, ServiceMgr #, theAuditorSvc
#protect against sloppy module authors
svcMgr=ServiceMgr
toolSvc=ToolSvc
from AthenaCommon.Logging import *
from AthenaCommon.Constants import *
log.setLevel(ERROR)
theApp.setOutputLevel(ERROR)

from AthenaCommon.Configurable import *
from AthenaCommon.OldStyleConfig import *

import AthenaCommon.AtlasUnixGeneratorJob

import AthenaPython.PyAthena as PyAthena


from LArConditionsCommon.LArCondFlags import larCondFlags
larCondFlags.SingleVersion.set_Value_and_Lock(sv)
larCondFlags.LoadElecCalib.set_Value_and_Lock(printCond)
if len(sqlite):
    larCondFlags.LArElecCalibSqlite.set_Value(sqlite)


from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.Luminosity.set_Value_and_Lock('zero')
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.InputFormat.set_Value_and_Lock('bytestream')
try:
    if (run>222222): 
        globalflags.DatabaseInstance.set_Value_and_Lock('CONDBR2')
    else:
        globalflags.DatabaseInstance.set_Value_and_Lock('COMP200')
except: 
    #flag doesn't exist in older releases
    pass

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = detdescrtag #"ATLAS-Comm-00-00-00"


if geo:
    from AthenaCommon.DetFlags import DetFlags
    DetFlags.Calo_setOn()
    
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetupRecoGeometry

ServiceMgr.IOVDbSvc.GlobalTag=tag
ServiceMgr.IOVDbSvc.DBInstance=""
ServiceMgr.EventSelector.RunNumber = run
#ServiceMgr+=CfgMgr.PyAthenaEventLoopMgr(EventPrintoutInterval=0)

#Get identifier mapping
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
include( "LArIdCnv/LArIdCnv_joboptions.py" )

include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )

#include("CaloIdCnv/CaloIdCnv_joboptions.py")

include( "LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py" )

#if dspth:
#    conddb.addFolder("LAR_ONL","/LAR/Configuration/DSPThreshold/Templates<tag>LARConfigurationDSPThresholdTemplates-Qt5sigma-samp5sigma</tag>")



from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

## import my algorithm and add it to the list of algorithms to be run
from LArConditionsCommon.LArCellConditionsAlg import LArCellConditionsAlg
theLArCellConditionsAlg=LArCellConditionsAlg("LArCellConditions",
                                             printConditions=printCond,
                                             printLocation=geo,
                                             printDSPTh=dspth)
job+= theLArCellConditionsAlg


theApp.initialize()
import readline
if os.path.exists( fhistory ):
    readline.read_history_file( fhistory )
readline.set_history_length( 128 )
#readline.parse_and_bind( 'tab:')
theApp.nextEvent(2) #First event is dummy to close DB connections, second has the user-loop
theApp.finalize()
readline.write_history_file(fhistory)
theApp.exit()
#theApp.EvtMax = 1

## EOF ##
