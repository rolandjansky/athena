#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""Set up to read and/or write bytestream files.

This module configures the Athena components required to read from
RAW/bytestream input. Use either byteStreamReadCfg to set up for reading
and byteStreamWriteCfg to set up for writing. Merge the component accumulator
the functions return into your job configuration as needed.

Executing this module will run a short test over 10 events. The events are read
from the default bytestream input on CVMFS and written to a local bytestream
file.

    Typical usage examples:

        component_accumulator.merge(byteStreamReadCfg(ConfigFlags))
"""
import AthenaConfiguration.ComponentFactory
import AthenaConfiguration.AllConfigFlags
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from AthenaCommon.Configurable import Configurable
from AthenaCommon.Logging import logging
from AthenaServices.MetaDataSvcConfig import MetaDataSvcCfg
from IOVDbSvc.IOVDbSvcConfig import IOVDbSvcCfg


def ByteStreamReadCfg(flags, type_names=None):
    """Set up to read from a bytestream file

    The function adds the components required to read events and metadata from
    bytestream input. May be used to read events from a secondary input as well
    primary input file.

    Args:
        flags:      Job configuration, usually derived from ConfigFlags
        type_names: (optional) specific type names for address provider to find

    Returns:
        A component accumulator fragment containing the components required to
        read from bytestream. Should be merged into main job configuration.
    """
    result = ComponentAccumulator()
    comp_factory = AthenaConfiguration.ComponentFactory.CompFactory

    bytestream_conversion = comp_factory.ByteStreamCnvSvc()
    result.addService(bytestream_conversion)

    bytestream_input = comp_factory.ByteStreamEventStorageInputSvc(
        name="ByteStreamInputSvc",
        EventInfoKey="{}EventInfo".format(
            flags.Overlay.BkgPrefix if flags.Overlay.DataOverlay else ""
        ),
    )
    result.addService(bytestream_input)

    if flags.Input.SecondaryFiles:
        event_selector = comp_factory.EventSelectorByteStream(
            name="SecondaryEventSelector",
            IsSecondary=True,
            Input=flags.Input.SecondaryFiles,
            ByteStreamInputSvc=bytestream_input.name,
        )
        result.addService(event_selector)
    else:
        event_selector = comp_factory.EventSelectorByteStream(
            name="EventSelector",
            Input=flags.Input.Files,
            SkipEvents=flags.Exec.SkipEvents,
            ByteStreamInputSvc=bytestream_input.name,
        )
        event_selector.HelperTools += [
            comp_factory.xAODMaker.EventInfoSelectorTool()
        ]
        result.addService(event_selector)
        result.setAppProperty("EvtSel", event_selector.name)

    event_persistency = comp_factory.EvtPersistencySvc(
        name="EventPersistencySvc", CnvServices=[bytestream_conversion.name]
    )
    result.addService(event_persistency)

    result.addService(comp_factory.ROBDataProviderSvc())

    address_provider = comp_factory.ByteStreamAddressProviderSvc(
        TypeNames=type_names if type_names else list(),
    )
    result.addService(address_provider)

    result.merge(
        MetaDataSvcCfg(flags, ["IOVDbMetaDataTool", "ByteStreamMetadataTool"])
    )

    proxy = comp_factory.ProxyProviderSvc()
    proxy.ProviderNames += [address_provider.name]
    result.addService(proxy)

    return result


def ByteStreamWriteCfg(flags, type_names=None):
    """Set up output stream in RAW/bytestream format

    Configure components responsible for writing bytestream format. Write job
    results to bytestream file. ATLAS file naming conventions are enforced as
    determined from the given configuration flags.

    Args:
        flags:      Job configuration, usually derived from ConfigFlags
        type_names: (optional) Specify item list for output stream to write

    Returns:
        A component accumulator fragment containing the components required to
        write to bytestream. Should be merged into main job configuration.
    """
    all_runs = set(flags.Input.RunNumber)
    assert (
        len(all_runs) == 1
    ), "Input is from multiple runs, do not know which one to use {}".format(
        all_runs
    )

    result = ComponentAccumulator("AthOutSeq")
    comp_factory = AthenaConfiguration.ComponentFactory.CompFactory

    event_storage_output = comp_factory.ByteStreamEventStorageOutputSvc(
        MaxFileMB=15000,
        MaxFileNE=15000000,  # event (beyond which it creates a new file)
        OutputDirectory="./",
        AppName="Athena",
        RunNumber=all_runs.pop(),
        OutputLevel=2,
    )
    result.addService(event_storage_output)
    # release variable depends the way the env is configured
    # FileTag = release

    bytestream_conversion = comp_factory.ByteStreamCnvSvc(
        name="ByteStreamCnvSvc",
        ByteStreamOutputSvcList=[event_storage_output.getName()],
        OutputLevel=2,
    )
    result.addService(bytestream_conversion)

    output_stream = comp_factory.AthenaOutputStream(
        name="BSOutputStreamAlg",
        EvtConversionSvc=bytestream_conversion.name,
        OutputFile="ByteStreamEventStorageOutputSvc",
        ItemList=type_names if type_names else list(),
        OutputLevel=2,
    )
    result.addEventAlgo(output_stream, primary=True)

    result.merge(IOVDbSvcCfg(flags))

    result.merge(
        MetaDataSvcCfg(flags, ["IOVDbMetaDataTool", "ByteStreamMetadataTool"])
    )

    return result


def main():
    """Run a functional test if module is executed"""
    log = logging.getLogger('ByteStreamConfig')
    Configurable.configurableRun3Behavior = True

    config_flags = AthenaConfiguration.AllConfigFlags.ConfigFlags
    config_flags.Input.Files = defaultTestFiles.RAW
    config_flags.Output.doWriteBS = True
    config_flags.lock()

    read = ByteStreamReadCfg(config_flags)
    read.store(open("test.pkl", "wb"))
    print("All OK")

    write = ByteStreamWriteCfg(config_flags)
    write.printConfig()
    log.info("Write setup OK")

    acc = MainServicesCfg(config_flags)
    acc.merge(read)
    acc.merge(write)
    acc.printConfig()
    log.info("Config OK")

    with open('ByteStreamConfig.pkl', 'wb') as pkl:
        acc.store(pkl)

    acc.run(10)


if __name__ == "__main__":
    main()
