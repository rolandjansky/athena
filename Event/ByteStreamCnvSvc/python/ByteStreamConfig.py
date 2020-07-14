#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""module docstring
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaCommon.Configurable import Configurable
from AthenaServices.MetaDataSvcConfig import MetaDataSvcCfg


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


def ByteStreamWriteCfg(flags, type_names=[]):
    result = ComponentAccumulator("AthOutSeq")
    outputSvc = CompFactory.ByteStreamEventStorageOutputSvc()
    outputSvc.MaxFileMB = 15000
    # event (beyond which it creates a new file)
    outputSvc.MaxFileNE = 15000000
    outputSvc.OutputDirectory = "./"
    outputSvc.AppName = "Athena"
    # release variable depends the way the env is configured
    # FileTag = release
    allRuns = set(flags.Input.RunNumber)
    assert (
        len(allRuns) == 1
    ), "The input is from multiple runs, do not know which one to use {}".format(
        allRuns
    )
    outputSvc.RunNumber = allRuns.pop()

    bytestream_conversion = CompFactory.ByteStreamCnvSvc("ByteStreamCnvSvc")

    bytestream_conversion.ByteStreamOutputSvcList = [outputSvc.getName()]
    streamAlg = CompFactory.AthenaOutputStream(
        "BSOutputStreamAlg",
        EvtConversionSvc=bytestream_conversion.getName(),
        OutputFile="ByteStreamEventStorageOutputSvc",
        ItemList=type_names,
    )

    result.addService(outputSvc)
    result.addService(bytestream_conversion)
    result.addEventAlgo(streamAlg, primary=True)
    return result


if __name__ == "__main__":
    Configurable.configurableRun3Behavior = True

    ConfigFlags.Input.Files = defaultTestFiles.RAW

    acc = ByteStreamReadCfg(ConfigFlags)
    acc.store(open("test.pkl", "wb"))
    print("All OK")
