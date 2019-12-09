# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
###############################################################
#
# Minimal athena options to execute the CutFlowSvc
#
## @file TestCutFlowSvcDumpBookkkeepers.py
##
## @brief Test basic CutFlowSvc functionality and dump existing cutbookkeepers
##
## @author Tadej Novak <tadej.novak@cern.ch>
#
#==============================================================

# basic job configuration
import AthenaCommon.AtlasUnixStandardJob

# setup the input
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
if 'inputFiles' in dir():
    athenaCommonFlags.FilesInput = inputFiles.split(',')
    del inputFiles
else:
    athenaCommonFlags.FilesInput = [
        '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/mc16_13TeV.410501.PowhegPythia8EvtGen_A14_ttbar_hdamp258p75_nonallhad.merge.AOD.e5458_s3126_r9364_r9315/AOD.11182705._000001.pool.root.1'
    ]

# get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# get a handle to the ServiceManager and the Tool service
from AthenaCommon.AppMgr import ServiceMgr, ToolSvc

# get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

# load POOL support
import AthenaPoolCnvSvc.ReadAthenaPool

# setup the CutFlowSvc and corresponding tools
from EventBookkeeperTools.CutFlowHelpers import CreateCutFlowSvc, CreateBookkeeperDumperTool
CreateCutFlowSvc()

# setup the bookkeeper dumper tool
CreateBookkeeperDumperTool()

# set debug logging
ServiceMgr.MessageSvc.defaultLimit = 9999999
ServiceMgr.CutFlowSvc.OutputLevel = DEBUG
ToolSvc.CutBookkeepersTool.OutputLevel = VERBOSE
ToolSvc.PDFSumOfWeights.OutputLevel = DEBUG

# run on 10 events
theApp.EvtMax = 1
