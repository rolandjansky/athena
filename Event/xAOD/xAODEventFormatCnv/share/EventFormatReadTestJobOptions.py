# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""Test reading xAOD::EventFormat

Read in POOL files created by the write test job options. Uses the metadata
tool to transcribe the EventFormat metadata to the MetaDataStore. Then an
algorithm reads the EventFormat from the MetaDataStore and prints the content.

    Typical usage example
        1. run: athena xAODEventFormatCnv/EventFormatWriteTestJobOptions.py
        2. run: athena xAODEventFormatCnv/EventFormatReadTestJobOptions.py
"""
from AthenaCommon import CfgMgr
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ToolSvc
import AthenaPoolCnvSvc.ReadAthenaPool


# grab input files
svcMgr.EventSelector.InputCollections = [
    "testStream0.pool.root",
    "testStream1.pool.root",
    "testStream2.pool.root",
    "testStream3.pool.root",
    "testStream4.pool.root",
]

# propogate xAOD::EventFormat from InputMetaDataStore to MetaDataStore
ToolSvc += CfgMgr.xAODMaker__EventFormatMetaDataTool(
    "EventFormatMetaDataTool",
    OutputLevel=1,
)
svcMgr.MetaDataSvc.MetaDataTools += [ToolSvc.EventFormatMetaDataTool]


# Add the xAOD::EventFormat printer algorithm to main algorithm sequence
algSeq = AlgSequence()
algSeq += CfgMgr.xAODMakerTest__EventFormatPrinterAlg()

# read the last event of the first file and the first event of the second file
# to test metadata access across file boundary
theApp.EvtMax = -1
