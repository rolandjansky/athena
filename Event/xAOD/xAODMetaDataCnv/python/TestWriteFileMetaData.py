# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""Test multi-threaded xAOD::FileMetaData writing

Read default test AOD and configure to write output stream with
xAOD::FileMetaData object. Set up to process 4 concurrent events. Inspect the
produced file for xAOD::FileMetaData metadata items.

    Typical usage example
        python -m xAODMetaDataCnv.TestWriteFileMetaData |tee log
"""
from AthenaConfiguration import AllConfigFlags, TestDefaults, MainServicesConfig
from AthenaCommon import Configurable, Logging
from OutputStreamAthenaPool import OutputStreamConfig
from PyUtils import MetaReader
from xAODEventInfoCnv import xAODEventInfoCnvConfig


def writeFileMetaData(flags):
    """set up an output stream and xAOD::EventInfo"""
    accumulator = OutputStreamConfig.OutputStreamCfg(
        configFlags=flags, streamName="AOD"
    )

    accumulator.merge(
        xAODEventInfoCnvConfig.EventInfoCnvAlgCfg(
            flags=flags, inputKey="", disableBeamSpot=flags.Common.Project!="Athena"
        )
    )

    return accumulator


def testMetaData(file_name):
    """Check that file metadata is in output"""
    meta = MetaReader.read_metadata([file_name])[file_name]["metadata_items"]
    file_info_items = [
        v for _, v in meta.items() if "FileMetaData" in v
    ]
    print("found file metadata objects:", file_info_items)
    return bool(file_info_items)


def main():
    """Run a job writing a file with FileMetaData"""
    Configurable.Configurable.configurableRun3Behavior = True
    msg = Logging.logging.getLogger("TestFileMetaData")

    config_flags = AllConfigFlags.ConfigFlags
    config_flags.Input.Files = TestDefaults.defaultTestFiles.AOD
    config_flags.Output.AODFileName = "test.pool.root"
    config_flags.Output.doWriteAOD = True
    config_flags.Concurrency.NumThreads = 4
    config_flags.Concurrency.NumConcurrentEvents = 4
    config_flags.lock()

    write = MainServicesConfig.MainServicesCfg(config_flags)
    write.merge(writeFileMetaData(config_flags))
    write.run(100)

    try:
        if testMetaData(config_flags.Output.AODFileName):
            msg.info("File contains xAOD::FileMetaData")
            return 0
        msg.error("File does not contain xAOD::FileMetaData")
    except ReferenceError:
        msg.error("Failed to produce output file")
    except KeyError:
        msg.error("Failed to get metadata item list from file")
    return 1


if __name__ == "__main__":
    main()
