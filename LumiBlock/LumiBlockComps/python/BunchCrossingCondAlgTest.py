# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from future import standard_library
from AthenaConfiguration.ComponentFactory import CompFactory
standard_library.install_aliases()

import sys,os
from LumiBlockComps.BunchCrossingCondAlgConfig import BunchCrossingCondAlgCfg

from LumiBlockComps.dummyLHCFillDB import createSqlite,fillFolder


def createBCMask1():
    mask=[]
    #pre-fill with zero
    for i in range(0,3564):
        mask.append(0x0)    

    #A train across the wrap-around:
    for i in range (0,25):
        mask[i]=0x3
    
    for i in range (3550,3564):
        mask[i]=0x3

    #A short sequence of bunches that doesn't qualify as train
    for i in range (1000,1030):

        mask[i]=0x3
    return mask
         

def createBCMask2():
    mask=[]
    #pre-fill with zero
    for i in range(0,3564):
        mask.append(0x0)    

    t8b4e=[0x3,0x3,0x3,0x3, 0x3,0x3,0x3,0x3, 0x0,0x0,0x0,0x0]
    
    for i in range(0,20):
        #create a train of 20 8be4 groups start
        mask[100+i*12:100+(i+1)*12]=t8b4e

    return mask



    
#First, create a dummy database to work with: 

#Delete any previous instance, if there is any:
try:
    os.remove("test.db")
except OSError:
    pass

#Copy the standard db content for one run (330470, LB301) to IOV 1 - 2 
copycmd='AtlCoolCopy "COOLONL_TDAQ/CONDBR2" "sqlite://;schema=test.db;dbname=CONDBR2" -c -f /TDAQ/OLC/LHC/FILLPARAMS -ts 1500867637 -tu 1500867638 -a -nts 0 -ntu 2'

from subprocess import getstatusoutput
stat,out=getstatusoutput(copycmd)

if (stat):
    print(out)
    sys.exit(-1)
   

db,folder=createSqlite("test.db")
    
d1=createBCMask1()
d2=createBCMask2()

onesec=1000000000

#Add two dummy masks with iov 2-3 and 3-4 
fillFolder(folder,d1,iovMin=2*onesec,iovMax=3*onesec)

fillFolder(folder,d2,3*onesec,4*onesec)

db.closeDatabase()
    
   

from AthenaCommon.Configurable import Configurable
Configurable.configurableRun3Behavior=1

from AthenaConfiguration.AllConfigFlags import ConfigFlags


from AthenaConfiguration.MainServicesConfig import MainServicesCfg

ConfigFlags.IOVDb.DatabaseInstance="CONDBR2"
ConfigFlags.Input.isMC=False
ConfigFlags.IOVDb.GlobalTag="CONDBR2-BLKPA-2017-05"
ConfigFlags.GeoModel.AtlasVersion="ATLAS-R2-2016-01-00-01"
ConfigFlags.lock()

result=MainServicesCfg(ConfigFlags)


McEventSelector=CompFactory.McEventSelector
McCnvSvc=CompFactory.McCnvSvc
EvtPersistencySvc=CompFactory.EvtPersistencySvc

mcevtsel=McEventSelector(RunNumber=330470,
                         EventsPerRun=1,
                         FirstEvent=1183722158,
                         FirstLB=310,
                         EventsPerLB=1,
                         #InitialTimeStamp=1500867637,
                         InitialTimeStamp=1,
                         TimeStampInterval=1
                         )

result.addService(mcevtsel)
result.setAppProperty("EvtSel",mcevtsel.getFullJobOptName())

mccnvsvc=McCnvSvc()
result.addService(mccnvsvc)


result.addService(EvtPersistencySvc("EventPersistencySvc",CnvServices=[mccnvsvc.getFullJobOptName(),]))
    
result.merge(BunchCrossingCondAlgCfg(ConfigFlags))
    
result.getService("IOVDbSvc").Folders=["<db>sqlite://;schema=test.db;dbname=CONDBR2</db><tag>HEAD</tag>/TDAQ/OLC/LHC/FILLPARAMS"]
result.getCondAlgo("BunchCrossingCondAlg").OutputLevel=1
    
BunchCrossingCondTest=CompFactory.BunchCrossingCondTest
result.addEventAlgo(BunchCrossingCondTest(FileName="BCData.txt",compact=True))


#result.setDebugStage("exec")

result.run(3)
                       
