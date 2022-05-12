# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""Configure jobs to propogate and/or create xAOD::EventFormat

Configure the EventFormatMetaDataTool to propogate EventFormat into the
MetaDataStore if it exists in the input. In addition configure the
EventFormatStreamHelperTool to create a new xAOD::EventFormat for the
output stream. The object created for the output stream is written to the
output file.

    Typical usage example

        from xAODEventFormatCnv.EventFormatConfig import EventFormatCfg
        ca = ComponentAccumulator()
        ca.merge(
            EventFormatCfg(
                flags=configFlags, stream=outputStream, streamName=streamName
            )
        )
"""
from AthenaConfiguration import ComponentFactory, ComponentAccumulator
from AthenaServices import MetaDataSvcConfig


def EventFormatCfg(flags, stream, streamName=None, key=None):
    """Add tools creating and propogating xAOD::FileMetaData
    
    Creates the IAthenaOutputStreamTool to create a xAOD::EventFormat for the
    outputstream. It also adds a MetaDataSvc configuration with the
    MetaDataTool that copies the xAOD::EventFormat from the InputMetaDataStore
    to the MetaDataStore. The output stream is configured to write out only the
    object created by the IAthenaOutputStreamTool.
    
    Args:
        flags: The AthConfigFlags for this job
        stream: An AthenaOutputStream to generate the xAOD::EventFormat for
        streamName: A name of the output stream, default: stream.name
        key: A key for the new xAOD::EventFormat created for the output
            stream, default "EventFormat{streamName}"

    Returns:
        A ComponentAccumulator with the EventFormatStreamHelperTool as well as
        as the MetaDataSvc using a EventFormatMetaDataTool.
    """
    if not streamName:
        streamName = stream.name
    if not key:
        key = f"EventFormat{streamName}"
    result = ComponentAccumulator.ComponentAccumulator()
    components = ComponentFactory.CompFactory

    # attach the FileMetaData creator to the output stream
    creator = components.xAODMaker.EventFormatStreamHelperTool(
        f'{streamName}_EventFormatStreamHelperTool',
        Key=key,
        DataHeaderKey=streamName,
    )
    stream.HelperTools.append(creator)
    stream.MetadataItemList += [
        f"xAOD::EventFormat#{key}",
    ]

    # Set up the tool propogating the FileMetaData
    result.merge(
        MetaDataSvcConfig.MetaDataSvcCfg(
            flags,
            ["xAODMaker::EventFormatMetaDataTool"]
        )
    )

    return result
