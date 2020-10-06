# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""Test writing event format information

Open an arbitrary number of streams to write to. An algorithm creates dummy
electrons. The EventFormatStreamHelperTool creates the corresponding
EventFormat metadata in the output stream.

    Typical usage example:

        athena xAODEventFormatCnv/EventFormatWriteTestJobOptions.py
"""
from AthenaCommon import CfgMgr
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from xAODEventFormatCnv.xAODEventFormatCnvConf import xAODMaker__EventFormatStreamHelperTool


DEBUG = 2
INFO = 3
EVENTS_PER_FILE = 5
NUMBER_OF_STREAMS = 5


def makeStream(name='TestStream'):
    """create and return an output stream"""
    test_stream = MSMgr.NewStream(
        StreamName=name,
        FileName="{}.pool.root".format(name),
        noTag=True,
    )
    # Add dummy items for testing
    test_stream.AddItem("xAODMakerTest::AVec#TestAVec")
    test_stream.AddItem("xAODMakerTest::AAuxContainer#TestAAuxContainer.")
    for tool in test_stream.GetEventStream().HelperTools:
        if isinstance(tool, xAODMaker__EventFormatStreamHelperTool):
            tool.TypeNames += ['.*xAODMakerTest::.*']
            break
    return test_stream


algSeq = AlgSequence()

# Add (an) xAOD creator algorithm(s) to the job.
algSeq += CfgMgr.xAODMakerTest__ACreatorAlg(
    "ACreator", OutputKey="TestObject"
)

# Write some output streams to file with this container.
streams = [
    makeStream('testStream{}'.format(i)) for i in range(NUMBER_OF_STREAMS)
]

# Make sure event info is available
algSeq += xAODMaker__EventInfoCnvAlg()

# Some extra job parameters.
theApp.EvtMax = EVENTS_PER_FILE

svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MetaDataSvc.OutputLevel = DEBUG
