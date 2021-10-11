# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""Test writing xAOD::EventFormat in job reading from input

Read in POOL files created by the write test job options. Uses the metadata
tool to transcribe the EventFormat metadata to the MetaDataStore. Then an
algorithm reads the EventFormat from the MetaDataStore and prints the content.

    Typical usage example
        1. run: athena xAODEventFormatCnv/EventFormatWriteTestJobOptions.py
        2. run: athena xAODEventFormatCnv/EventFormatReadWriteTestJobOptions.py
"""
from AthenaCommon import CfgMgr
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ToolSvc
import AthenaPoolCnvSvc.ReadAthenaPool
from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

VERBOSE = 1
DEBUG = 2
INFO = 3
WARNING = 4


def makeStream(name='TestStream'):
    """create and return an output stream"""
    test_stream = MSMgr.NewStream(
        StreamName=name,
        FileName="{}.pool.root".format(name),
    )
    return test_stream


# grab input files
svcMgr.EventSelector.InputCollections = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1"]

# propogate xAOD::EventFormat from InputMetaDataStore to MetaDataStore
ToolSvc += CfgMgr.xAODMaker__FileMetaDataTool(
    "FileMetaDataTool",
    OutputLevel=DEBUG,
)
svcMgr.MetaDataSvc.MetaDataTools += [ToolSvc.FileMetaDataTool]


# Add the xAOD::EventFormat printer algorithm to main algorithm sequence
algSeq = AlgSequence()

# Make sure event info is available
algSeq += xAODMaker__EventInfoCnvAlg()

# Write some output streams to file with this container.
streams = [
    makeStream('xAOD')
]

svcMgr.MessageSvc.OutputLevel = INFO
# svcMgr.MetaDataSvc.OutputLevel = INFO


# Set the event printout interval.
if not hasattr(svcMgr, theApp.EventLoop):
    svcMgr += getattr(CfgMgr, theApp.EventLoop)()
evtLoop = getattr(svcMgr, theApp.EventLoop)
evtLoop.EventPrintoutInterval = 1000

# read the last event of the first file and the first event of the second file
# to test metadata access across file boundary
theApp.EvtMax = 100
