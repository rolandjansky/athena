#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""module docstring
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from AthenaCommon.Configurable import Configurable
from AthenaCommon.Logging import logging
from AthenaServices.MetaDataSvcConfig import MetaDataSvcCfg

LOG = logging.getLogger('ByteStreamConfig')


def ByteStreamReadCfg(flags, type_names=None):
    """Creates resultumulator for BS reading
    """
    result = ComponentAccumulator()

    bytestream_conversion = CompFactory.ByteStreamCnvSvc()
    result.addService(bytestream_conversion)

    bytestream_input = CompFactory.ByteStreamEventStorageInputSvc(
        name="ByteStreamInputSvc",
        EventInfoKey="{}EventInfo".format(
            flags.Overlay.BkgPrefix if flags.Overlay.DataOverlay else ""
        ),
    )
    result.addService(bytestream_input)

    if flags.Input.SecondaryFiles:
        event_selector = CompFactory.EventSelectorByteStream(
            name="SecondaryEventSelector",
            IsSecondary=True,
            Input=flags.Input.SecondaryFiles,
            ByteStreamInputSvc=bytestream_input.name,
        )
        result.addService(event_selector)
    else:
        event_selector = CompFactory.EventSelectorByteStream(
            name="EventSelector",
            Input=flags.Input.Files,
            SkipEvents=flags.Exec.SkipEvents,
            ByteStreamInputSvc=bytestream_input.name,
        )
        event_selector.HelperTools += [
            CompFactory.xAODMaker.EventInfoSelectorTool()
        ]
        result.addService(event_selector)
        result.setAppProperty("EvtSel", event_selector.name)

    event_persistency = CompFactory.EvtPersistencySvc(
        name="EventPersistencySvc", CnvServices=[bytestream_conversion.name]
    )
    result.addService(event_persistency)

    result.addService(CompFactory.ROBDataProviderSvc())

    address_provider = CompFactory.ByteStreamAddressProviderSvc(
        TypeNames=type_names if type_names else list(),
    )
    result.addService(address_provider)

    result.merge(
        MetaDataSvcCfg(flags, ["IOVDbMetaDataTool", "ByteStreamMetadataTool"])
    )

    proxy = CompFactory.ProxyProviderSvc()
    proxy.ProviderNames += [address_provider.name]
    result.addService(proxy)

    return result


def ByteStreamWriteCfg(flags, type_names=None):
    """func level docstring goes here
    """
    all_runs = set(flags.Input.RunNumber)
    assert (
        len(all_runs) == 1
    ), "Input is from multiple runs, do not know which one to use {}".format(
        all_runs
    )

    result = ComponentAccumulator("AthOutSeq")

    event_storage_output = CompFactory.ByteStreamEventStorageOutputSvc(
        MaxFileMB=15000,
        MaxFileNE=15000000,  # event (beyond which it creates a new file)
        OutputDirectory="./",
        AppName="Athena",
        RunNumber=all_runs.pop(),
    )
    result.addService(event_storage_output)
    # release variable depends the way the env is configured
    # FileTag = release

    bytestream_conversion = CompFactory.ByteStreamCnvSvc(
        name="ByteStreamCnvSvc",
        ByteStreamOutputSvcList=[event_storage_output.getName()],
    )
    result.addService(bytestream_conversion)

    output_stream = CompFactory.AthenaOutputStream(
        name="BSOutputStreamAlg",
        EvtConversionSvc=bytestream_conversion.name,
        OutputFile="ByteStreamEventStorageOutputSvc",
        ItemList=type_names if type_names else list(),
    )
    result.addEventAlgo(output_stream, primary=True)

    return result


def main():
    """Implement running simple functional test"""
    Configurable.configurableRun3Behavior = True

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.Output.doWriteBS = True
    ConfigFlags.lock()

    read = ByteStreamReadCfg(ConfigFlags)
    read.store(open("test.pkl", "wb"))
    print("All OK")

    write = ByteStreamWriteCfg(ConfigFlags)
    write.printConfig()
    LOG.info("Write setup OK")

    acc = MainServicesCfg(ConfigFlags)
    acc.merge(read)
    acc.merge(write)
    acc.printConfig()
    LOG.info("Config OK")

    acc.run(10)


if __name__ == "__main__":
    main()
