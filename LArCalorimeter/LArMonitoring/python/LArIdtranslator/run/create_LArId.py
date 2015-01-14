#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import sys
sys.path.append('/afs/cern.ch/user/a/atlcond/utils/python/')
from AtlCoolBKLib import resolveAlias
from optparse import OptionParser
usage = 'usage: %prog [options] arg'
parser = OptionParser(usage)
parser.add_option('-r','--run',type='str',dest='run',default=0x7fffffff,help='input a run',action='store')
#parser.add_option('-t','--tag',type='str',dest='tag',default='COMCOND-ES1PA-006-03',help='input tag',action='store')
parser.add_option('-t','--tag',type='str',dest='tag',default='Head',help='input tag',action='store')
parser.add_option('-d','--debug',default=False,dest='debug',help='debug output',action='store_true')

(options, args) = parser.parse_args()
run = int(options.run)
tag = options.tag
if tag=='Head':
    resolver=resolveAlias()
    tag = resolver.getCurrentES().replace("*","ST")
    if not 'CONDBR' in tag:
       tag='CONDBR2-ES1PA-2014-01'

dbg = options.debug

# athena stuff
from AthenaCommon.Include import Include, IncludeError, include
include.setShowIncludes(0)
from AthenaCommon import CfgMgr
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ToolSvc, ServiceMgr

# protect against sloppy module authors
toolSvc=ToolSvc
from AthenaCommon.Logging import *
from AthenaCommon.Constants import *
log.setLevel(INFO)
theApp.setOutputLevel(INFO)

from AthenaCommon.Configurable import *
from AthenaCommon.OldStyleConfig import *
import AthenaCommon.AtlasUnixGeneratorJob
import AthenaPython.PyAthena as PyAthena

from LArConditionsCommon.LArCondFlags import larCondFlags
larCondFlags.SingleVersion.set_Value_and_Lock(True)
larCondFlags.LoadElecCalib.set_Value_and_Lock(True)


from AthenaCommon.GlobalFlags import globalflags
#globalflags.DataSource.set_Value_and_Lock('geant4')
globalflags.DetGeo.set_Value_and_Lock('atlas')       
globalflags.Luminosity.set_Value_and_Lock('zero')
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.InputFormat.set_Value_and_Lock("bytestream") 
globalflags.DatabaseInstance.set_Value_and_Lock("CONDBR2")

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-GEO-20-00-00"

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetupRecoGeometry

import time
ttuple=(2013, 02, 06, 16, 00, 0,-1,-1,-1)
itime = int(time.mktime(ttuple))
TimeStamp=itime

#ServiceMgr.EventSelector.InitialTimeStamp = int(time.time())-1
ServiceMgr.EventSelector.InitialTimeStamp = TimeStamp
ServiceMgr.IOVDbSvc.GlobalTag = tag
print "Tag used for LArId.db = ",tag
ServiceMgr.EventSelector.RunNumber = 999999#run
ServiceMgr.EventSelector.EventsPerRun      = 100
ServiceMgr.EventSelector.FirstEvent        = 0
ServiceMgr.EventSelector.EventsPerLB       = 100
ServiceMgr.EventSelector.FirstLB           = 1
ServiceMgr.EventSelector.TimeStampInterval = 5
ServiceMgr.EventSelector.OverrideRunNumber = True
#Get identifier mapping
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
include( "LArIdCnv/LArIdCnv_joboptions.py" )
include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
include( "AthenaSealSvc/AthenaSealSvc_joboptions.py" )
include( "LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py" )
include( "LArDetDescr/LArDetDescr_joboptions.py" )
include( "LArCondAthenaPool/LArCondAthenaPool_joboptions.py" )
#include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
include( "LArIdtranslator.py" )

conddb.addFolder("DCS_OFL","/LAR/DCS/HV/BARREl/I16")
conddb.addFolder("DCS_OFL","/LAR/DCS/HV/BARREL/I8")
conddb.addFolder("LAR_OFL","/LAR/IdentifierOfl/HVLineToElectrodeMap");
#conddb.blockFolder("/LAR/IdentifierOfl/OnOffIdMap_SC");
conddb.addFolder("","/LAR/IdentifierOfl/OnOffIdMap_SC<db>COOLOFL_LAR/OFLP200</db><tag>LARIdentifierOflOnOffIdMap_SC-000</tag>");
#conddb.addFolder("LAR","/LAR/Identifier/HVLineToElectrodeMap");
#  conddb.addOverride("/LAR/Identifier/HVLineToElectrodeMap","LARHVLineToElectrodeMap-001")
conddb.addFolder("LAR_ONL","/LAR/ElecCalibOnl/HVScaleCorr")

folders = ServiceMgr.IOVDbSvc.Folders
excluded_folders = ['Bad']
#excluded_folders = ['Corr','TT','DAC2uA','MphysOverMcal','Pedestal','Ramp','uA2MeV','phases']

for i in range(len(folders)-1,0,-1):
  for j in excluded_folders:
    if j in folders[i]: folders.pop(i); break

ServiceMgr.IOVDbSvc.Folders = folders
print ServiceMgr.IOVDbSvc.Folders
################################################3
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

## import my algorithm and add it to the list of algorithms to be run
theLArIdtranslator = LArIdtranslator("LArIdtranslator")

job += theLArIdtranslator

theApp.initialize()
theApp.nextEvent(2)
theApp.finalize()
