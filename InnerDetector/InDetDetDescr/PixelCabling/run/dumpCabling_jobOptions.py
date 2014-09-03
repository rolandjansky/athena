# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#use McEventSelector
## basic job configuration (for generator)
import AthenaCommon.AtlasUnixGeneratorJob

# Set handler for traceback
#gbl.AthenaServices.SetFatalHandler(438)
from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource = 'data'

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topjob = AlgSequence()

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

svcMgr.MessageSvc.OutputLevel      = INFO
svcMgr.MessageSvc.debugLimit       = 100000
svcMgr.MessageSvc.infoLimit        = 100000
svcMgr.MessageSvc.Format           = "% F%30W%S%7W%R%T %0W%M"
#--------------------------------------------------------------
# Access to IOVSvc, IOVDbSvc and CondDBMySQLCnvSvc
#--------------------------------------------------------------

#--------------------------------------------------------------
# Options for IOVRegistrationSvc
import RegistrationServices.IOVRegistrationSvc

# Dont set the tag for ALL folders
# in COOL folder tags must be globally unique
regSvc = svcMgr.IOVRegistrationSvc
# regSvc.IOVDbTag   = "DC1"
regSvc.OutputLevel=2

# Select the following to delete and recreate the folders. Default is
# NOT to recreate
regSvc.RecreateFolders = False

#here follow original python

# Add Sequencer to topSequence
# Sequencer has to be named! 
from AthenaCommon.AlgSequence import AthSequencer
job = AthSequencer('dumpCabling_sequencer')
topjob += job

from AthenaCommon.AppMgr import ServiceMgr
#from AthenaCommon.AppMgr import ToolSvc

# get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

from AthenaCommon.GlobalFlags import GlobalFlags
# --- default is atlas geometry
GlobalFlags.DetGeo.set_atlas()

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('COMCOND-000-00')
#conddb.addFolder("PIXEL","/PIXEL/PixelCablingData")
conddb.addFolder("","<dbConnection>oracle://INTR;schema=ATLAS_MLIMPER;dbname=COMP200</dbConnection>/PIXEL/ReadoutSpeed")


include( "AthenaCommon/AthenaCommonFlags.py" )

## from AthenaCommon.DetFlags import DetFlags 
## DetFlags.ID_setOn()
## DetFlags.Calo_setOff()
## DetFlags.Muon_setOff()
## DetFlags.Truth_setOff()
## DetFlags.LVL1_setOff()
## DetFlags.SCT_setOff()
## DetFlags.TRT_setOff()

from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
DetFlags.pixel_setOn()

import AtlasGeoModel.SetGeometryVersion
import AtlasGeoModel.GeoModelInit

from PixelCabling.PixelCablingConf import PixelCablingSvc
PixelCablingSvc = PixelCablingSvc()
svcMgr += PixelCablingSvc

from PixelCabling.PixelCablingConf import DumpCabling
DumpCabling = DumpCabling()

DumpCabling.Write = True

import IOVDbSvc.IOVDb
#location of test DB
svcMgr.IOVDbSvc.dbConnection  = "oracle://INTR;schema=ATLAS_MLIMPER;dbname=COMP200"
#if no explicit dbConnection is given a local sqlite-file "mycool.db" is created in the run-directory

job += DumpCabling

#some reasonable run-number (=good run for 7 TeV minimum bias)
svcMgr.EventSelector.RunNumber         = 158856
svcMgr.EventSelector.InitialTimeStamp  = 1222819200
theApp.EvtMax = 1
