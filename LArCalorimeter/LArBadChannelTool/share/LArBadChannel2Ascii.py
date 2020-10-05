# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

if 'OutputFile' not in dir():
    OutputFile="bc_output.txt"

if 'DBInstance' not in dir():
   DBInstance="CONDBR2"

if 'DBString' not in dir():
   DBString="<db>COOLOFL_LAR/"

if 'sqlite' in dir():
    dbStr="<db>sqlite://;schema="+sqlite+";dbname="+DBInstance+"</db>"
    if 'tag' not in dir():
        tag="LARBadChannelsOflBadChannels-UPD4-00"
else:
    dbStr=DBString+DBInstance+"</db>"

if 'tag' in dir():
    tagStr="<tag>"+tag+"</tag>"
else:
    tagStr=""

if 'IOVEndRun' not in dir():
   IOVEndRun = -1

if 'IOVEndLB' not in dir():
   IOVEndLB = -1

if 'folderStr' not in dir():
   folderStr="/LAR/BadChannelsOfl/BadChannels"

if 'ExecutiveSummaryFile' not in dir():
    ExecutiveSummaryFile=""


import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.GlobalFlags import  globalflags
globalflags.DataSource="data"
globalflags.InputFormat="bytestream"
if 'OFLP' not in DBInstance:
   globalflags.DatabaseInstance=DBInstance

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-R2-2015-04-00-00"

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

#Set up GeoModel (not really needed but crashes without)
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )

theApp.EvtMax = 1

if IOVEndRun <= 0:
   svcMgr.EventSelector.RunNumber         = 2147483647
else:   
   svcMgr.EventSelector.RunNumber         = IOVEndRun

if IOVEndLB <= 0:
   svcMgr.EventSelector.FirstEvent        = 1
else:   
   svcMgr.EventSelector.FirstEvent        = 0
   svcMgr.EventSelector.FirstLB           = IOVEndLB

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr)


from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")

from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
condSeq+=xAODMaker__EventInfoCnvAlg()

from IOVSvc.IOVSvcConf import CondInputLoader
theCLI=CondInputLoader( "CondInputLoader")
condSeq += theCLI 

import StoreGate.StoreGateConf as StoreGateConf
svcMgr += StoreGateConf.StoreGateSvc("ConditionStore")

from LArCabling.LArCablingAccess import LArOnOffIdMapping
LArOnOffIdMapping()

from LArBadChannelTool.LArBadChannelAccess import LArBadChannelAccess
LArBadChannelAccess(dbString="/LAR/BadChannelsOfl/BadChannels"+dbStr+tagStr)

if len(ExecutiveSummaryFile):
    from LArBadChannelTool.LArBadFebAccess import LArBadFebAccess
    LArBadFebAccess()

from LArBadChannelTool.LArBadChannelToolConf import LArBadChannel2Ascii
theLArBadChannels2Ascii=LArBadChannel2Ascii(SkipDisconnected=True)
theLArBadChannels2Ascii.FileName=OutputFile
theLArBadChannels2Ascii.WithMissing=False
theLArBadChannels2Ascii.ExecutiveSummaryFile=ExecutiveSummaryFile
topSequence+=theLArBadChannels2Ascii

svcMgr.IOVDbSvc.GlobalTag="CONDBR2-ES1PA-2014-01" 
