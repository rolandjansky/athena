# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
###############################################################
#
# Minimal athena options to execute the CutFlowSvc with algorithms
#
## @file TestCutFlowSvcOutput.py
##
## @brief Test basic CutFlowSvc functionality with a few algorithms
##
## @author Tadej Novak <tadej.novak@cern.ch>
#
#==============================================================

# debug logging
from AthenaCommon.Logging import logging
logging.getLogger('CreateCutFlowSvc').setLevel(DEBUG)

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

# setup some configuration
from RecExConfig.RecFlags import rec
rec.mergingStreamName = 'Stream1'

# setup the CutFlowSvc and corresponding tools
from EventBookkeeperTools.CutFlowHelpers import CreateCutFlowSvc
CreateCutFlowSvc(seq=topSequence)

# add a test algorithm
from EventBookkeeperTools.EventBookkeeperToolsConf import CppFilterTester
alg = CppFilterTester('CppFilterTester')
alg.cut1 = 21
alg.cut2 = 12
topSequence += alg

# output options
from OutputStreamAthenaPool.CreateOutputStreams import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream('Stream1', 'OutputAOD.root', asAlg=True)
Stream1.ItemList = ['xAOD::EventInfo#*', 'xAOD::EventAuxInfo#*']
Stream1.MetadataItemList += ['xAOD::CutBookkeeperContainer#CutBookkeepers', 'xAOD::CutBookkeeperAuxContainer#CutBookkeepersAux.']
Stream1.MetadataItemList += ['xAOD::CutBookkeeperContainer#IncompleteCutBookkeepers', 'xAOD::CutBookkeeperAuxContainer#IncompleteCutBookkeepersAux.']

# set debug logging
ServiceMgr.MessageSvc.defaultLimit = 9999999
ServiceMgr.CutFlowSvc.OutputLevel = DEBUG
ToolSvc.CutBookkeepersTool.OutputLevel = DEBUG

# run on 10 events
theApp.EvtMax = 10
