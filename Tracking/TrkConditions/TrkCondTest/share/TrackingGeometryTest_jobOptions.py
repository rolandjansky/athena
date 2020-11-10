# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
###############################################################
#
# Job options 
#
#==============================================================

#--------------------------------------------------------------
# ATLAS default Application Configuration options
#--------------------------------------------------------------

# Use McEventSelector so we can run with AthenaMP

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = ["/afs/cern.ch/work/r/rlangenb/public/data18_13TeV.00363979.physics_Main.daq.ESD/100evts10lumiblocks.ESD.root"]

import AthenaPoolCnvSvc.ReadAthenaPool

# build GeoModel
import AthenaPython.ConfigLib as apcl
cfg = apcl.AutoCfg(name = 'TrackingGeometryTest', input_files=athenaCommonFlags.FilesInput())

cfg.configure_job()

from AthenaCommon.GlobalFlags import globalflags
if len(globalflags.ConditionsTag())!=0:
  from IOVDbSvc.CondDB import conddb
  conddb.setGlobalTag(globalflags.ConditionsTag())
from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags
InDetGeometryFlags.useDynamicAlignFolders=True

import MagFieldServices.SetupField

# the global detflags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
DetFlags.TRT_setOn()

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from AtlasGeoModel import SetGeometryVersion 
from AtlasGeoModel import GeoModelInit 

# switch the material loading off
from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.MaterialSource           = 'None'
TrkDetFlags.MaterialVersion          = 16
TrkDetFlags.ConfigurationOutputLevel = VERBOSE


TrkDetFlags.PixelBuildingOutputLevel     = VERBOSE
TrkDetFlags.SCT_BuildingOutputLevel      = VERBOSE
TrkDetFlags.TRT_BuildingOutputLevel      = VERBOSE
TrkDetFlags.InDetBuildingOutputLevel     = VERBOSE
TrkDetFlags.ConfigurationOutputLevel     = VERBOSE

from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc


# load the tracking geometry cond alg
from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlg import ConfiguredTrackingGeometryCondAlg
TrkGeoCondAlg = ConfiguredTrackingGeometryCondAlg('AtlasTrackingGeometryCondAlg')
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")
condSeq+= TrkGeoCondAlg


# ----------------------
from TrkDetDescrTestTools.TrkDetDescrTestToolsConf import Trk__GeometryAsciiDumper
GeometryAsciiDumper = Trk__GeometryAsciiDumper(name= 'GeometryAsciiDumper')
ToolSvc += GeometryAsciiDumper
GeometryProcessors = [ GeometryAsciiDumper ]

from TrkCondTest.TrackingGeometryCondAlgTestConf import Trk__TrackingGeometryCondAlgTest
TrkGeomCondAlgTest = Trk__TrackingGeometryCondAlgTest(name = "TrkGeomTest")
TrkGeomCondAlgTest.GeometryProcessors = GeometryProcessors
job += TrkGeomCondAlgTest

#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------

# Output level for HelloAlg only (note name: instance, not type)
#TrackingGeometryTest = INFO

# You can set the global output level on the message svc (not
# recommended) or by using the -l athena CLI parameter

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------


theApp.EvtMax = 2

from AthenaCommon.AppMgr import ServiceMgr
# output level
ServiceMgr.MessageSvc.OutputLevel  = INFO
# increase the number of letter reserved to the alg/tool name from 18 to 30
ServiceMgr.MessageSvc.Format       = "% F%50W%S%7W%R%T %0W%M"
# to change the default limit on number of message
ServiceMgr.MessageSvc.defaultLimit = 9999999  # all messages

#==============================================================
#
# End of job options file
#
###############################################################
