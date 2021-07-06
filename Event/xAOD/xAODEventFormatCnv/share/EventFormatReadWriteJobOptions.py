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
from xAODEventFormatCnv.xAODEventFormatCnvConf import xAODMaker__EventFormatStreamHelperTool
from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

VERBOSE = 1
DEBUG = 2
INFO = 3
NUMBER_OF_STREAMS = 1


def makeStream(name='TestStream'):
    """create and return an output stream"""
    test_stream = MSMgr.NewStream(
        StreamName=name,
        FileName="{}.pool.root".format(name),
        noTag=True,
    )
    # Add dummy items for testing
    test_stream.AddItem("xAODMakerTest::AVec#TestObject")
    test_stream.AddItem("xAODMakerTest::AAuxContainer#TestObjectAux.")
    test_stream.AddItem("xAODMakerTest::AVec#TestObject2")
    test_stream.AddItem("xAODMakerTest::AAuxContainer#TestObject2Aux.")
    for tool in test_stream.GetMetaDataStream().HelperTools:
        if isinstance(tool, xAODMaker__EventFormatStreamHelperTool):
            tool.TypeNames += ['.*xAODMakerTest::.*']
            tool.OutputLevel = VERBOSE
            break
    return test_stream


# grab input files
svcMgr.EventSelector.InputCollections = [
    "testStream0.pool.root",
    "testStream1.pool.root",
]

# propogate xAOD::EventFormat from InputMetaDataStore to MetaDataStore
ToolSvc += CfgMgr.xAODMaker__EventFormatMetaDataTool(
    "EventFormatMetaDataTool",
    OutputLevel=VERBOSE,
)
svcMgr.MetaDataSvc.MetaDataTools += [ToolSvc.EventFormatMetaDataTool]


# Add the xAOD::EventFormat printer algorithm to main algorithm sequence
algSeq = AlgSequence()
algSeq += CfgMgr.xAODMakerTest__EventFormatPrinterAlg()


# Add (an) xAOD creator algorithm(s) to the job.
algSeq += CfgMgr.xAODMakerTest__ACreatorAlg(
    "ACreator", OutputKey="TestObject2"
)

# Write some output streams to file with this container.
streams = [
    makeStream('readWriteTestStream{}'.format(i)) for i in range(NUMBER_OF_STREAMS)
]

# Make sure event info is available
algSeq += xAODMaker__EventInfoCnvAlg()

svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MetaDataSvc.OutputLevel = DEBUG

# read the last event of the first file and the first event of the second file
# to test metadata access across file boundary
theApp.EvtMax = -1
