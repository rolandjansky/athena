#!/usr/bin/env python
# -*- coding: utf-8 -*
# Filename: HitMapBuilder.py
# Author: Pixel DQ
# Author: Yosuke Takubo (Yosuke.Takubo at cern.ch)
# Project: Hit Map Tool
# Description: ATHENA Jop Options for HitMapBuilder.py
# Date: Jul. 2019
# Input: inputfilelist (A list of byte stream files or pool files)
#           or athena.py arguments --filesInput
#           or RAW files in current directory
# Output: A ROOT file
###############################################################

#==============================================================
# Job options file
#==============================================================
doClusterization = False
doMonitoring     = False
doValidate       = False
filelist         = 'inputfilelist'
nevents          = -1

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

#------------------------------------------
# Input files
#------------------------------------------
import glob
collection = []
if len(athenaCommonFlags.FilesInput.get_Value()) != 0: # athena.py argument --filesInput ...
    collection = athenaCommonFlags.FilesInput.get_Value()
elif os.path.isfile(filelist): # inputfilelist
    for line in open(filelist):
        if line[0] != '#':
            collection.append(line.strip())
    athenaCommonFlags.FilesInput = collection
elif len(glob.glob("data*TeV*RAW*")) != 0:
    collection = glob.glob("data*TeV*RAW*") # Find data file from current directory for pathena
    athenaCommonFlags.FilesInput = collection
else:
    errmess="### Unable to open input filelist: '%s'! or --FilesInput not set! or cannot find data file on current directory!" % filelist
    raise RuntimeError(errmess)

#------------------------------------------
# DetFlags
#------------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
DetFlags.pixel_setOn()
DetFlags.Print()

#------------------------------------------
# PixelDist
#------------------------------------------
# Conditions sequence for Athena MT
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")

from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelConfigCondAlg
alg = PixelConfigCondAlg(name="PixelConfigCondAlg")
PixelConfigCondAlg.DistortionInputSource = 0 
alg.DistortionWriteToFile = False
condSeq += alg

print(alg)

from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDistortionAlg
condSeq += PixelDistortionAlg(name="PixelDistortionAlg")
#------------------------------------------
# GlobalFlags
#------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = "ATLAS-R2-2016-01-00-01" # [SGS] how to know exact version (e.g. AMI) ?
globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'data'

# set InputFormat
import PyUtils.AthFile as AthFile
inputfile = AthFile.fopen( collection[0] )

if inputfile.fileinfos['file_type'] == 'bs':
    globalflags.InputFormat = 'bytestream'
elif inputfile.fileinfos['file_type'] == 'pool':
    globalflags.InputFormat = 'pool'
else:
    raise RuntimeError("Unable to read input file (format not supported)")


# show debug info [SGS]
print('## globalflags.InputFormat = %s' % globalflags.InputFormat)

if globalflags.InputFormat() == 'pool':
    globalflags.DetDescrVersion = inputfile.fileinfos['geometry']
else:
    globalflags.ConditionsTag = 'CONDBR2-BLKPA-RUN2-07'
    globalflags.DatabaseInstance = 'CONDBR2'

globalflags.print_JobProperties()

#------------------------------------------
# GeoModel
#------------------------------------------
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

#------------------------------------------
# ByteStream
#------------------------------------------
include( "InDetRecExample/InDetReadBS_jobOptions.py" )
from ByteStreamCnvSvc import ReadByteStream
ServiceMgr.EventSelector.Input = collection

from PixelReadoutGeometry.PixelReadoutGeometryConf import InDetDD__PixelReadoutManager
PixelReadoutManager = InDetDD__PixelReadoutManager("PixelReadoutManager")
ServiceMgr += PixelReadoutManager

#------------------------------------------
# Offline Condition
#------------------------------------------
if globalflags.InputFormat() == 'bytestream':
    if len(globalflags.ConditionsTag())!=0:
        conddb.setGlobalTag(globalflags.ConditionsTag())


#------------------------------------------
# histo service
#------------------------------------------
from GaudiSvc.GaudiSvcConf import THistSvc
if not hasattr(ServiceMgr, 'THistSvc'):
    ServiceMgr += THistSvc()
THistSvc = ServiceMgr.THistSvc
THistSvc.Output += ["histfile DATAFILE='HitMap.root' OPT='RECREATE'"]

#------------------------------------------
# HitMapBuilder algorithm
#------------------------------------------
from PixelCalibAlgs.PixelCalibAlgsConf import HitMapBuilder
HitMapBuilder = HitMapBuilder()
HitMapBuilder.LBMin = 0
HitMapBuilder.LBMax = -1
print(HitMapBuilder)
topSequence += HitMapBuilder

#--------------------------------------------------------------
# Number of events
#--------------------------------------------------------------
#ServiceMgr.EventSelector.SkipEvents = 0
theApp.EvtMax = nevents

#--------------------------------------------------------------
# MessageSvc
#--------------------------------------------------------------
ServiceMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
ServiceMgr.MessageSvc.defaultLimit = 9999999 # all messages
ServiceMgr.MessageSvc.useColors = True

#--------------------------------------------------------------
# Message service output level threshold
# (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel  = INFO

#==============================================================
# End of job options file
#==============================================================

