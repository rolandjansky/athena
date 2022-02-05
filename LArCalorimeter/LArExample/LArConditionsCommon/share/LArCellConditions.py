#!/bin/env python
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

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

from AthenaConfiguration.AllConfigFlags import ConfigFlags 
ConfigFlags.Input.Files = []
ConfigFlags.Input.TimeStamp = 1000
ConfigFlags.Input.isMC=False
ConfigFlags.Input.RunNumber=run
ConfigFlags.IOVDb.DatabaseInstance="CONDBR2" if run>222222 else "COMP200"
ConfigFlags.IOVDb.GlobalTag=tag
ConfigFlags.LAr.doAlign=False
ConfigFlags.Exec.OutputLevel=8
ConfigFlags.lock()

from RootUtils import PyROOTFixes  # noqa F401
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
cfg=MainServicesCfg(ConfigFlags)

from McEventSelector.McEventSelectorConfig import McEventSelectorCfg
cfg.merge (McEventSelectorCfg (ConfigFlags))

if geo:
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    cfg.merge(LArGMCfg(ConfigFlags))
else:
    from DetDescrCnvSvc.DetDescrCnvSvcConfig import DetDescrCnvSvcCfg
    cfg.merge(DetDescrCnvSvcCfg(ConfigFlags))    

from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg 
cfg.merge(LArOnOffIdMappingCfg(ConfigFlags))

from LArConfiguration.LArElecCalibDBConfig import LArElecCalibDbCfg
requiredConditions=["Pedestal","Ramp","DAC2uA","uA2MeV","MphysOverMcal","HVScaleCorr"]
cfg.merge(LArElecCalibDbCfg(ConfigFlags,requiredConditions))

from LArBadChannelTool.LArBadChannelConfig import LArBadChannelCfg
cfg.merge(LArBadChannelCfg(ConfigFlags))

from LArConditionsCommon.LArCellConditionsAlg import LArCellConditionsAlg
theLArCellConditionsAlg=LArCellConditionsAlg("LArCellConditions",
                                             printConditions=printCond,
                                             printLocation=geo)
cfg.addEventAlgo(theLArCellConditionsAlg)

if os.path.exists( fhistory ):
    readline.read_history_file( fhistory )
readline.set_history_length( 128 )
cfg.run(2,OutputLevel=7) #First event is dummy to close DB connections, second has the user-loop

readline.write_history_file(fhistory)
