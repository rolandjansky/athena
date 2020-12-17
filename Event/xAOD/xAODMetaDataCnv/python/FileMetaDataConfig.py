# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""Configure jobs to propogate and/or create xAOD::FileMetaData

Configure the FileMetaDataTool to propogate FileMetaData into the MetaDataStore
if it exists in the input. In addition configure the FileMetaDataCreatorTool to
create xAOD::FileMetaData from the output stream. The object created for the
output stream is written to the output file.

    Typical usage example

        from xAODFileMetaDataCnv import FileMetaDataConfig
        ca = ComponentAccumulator()
        ca.merge(
            FileMetaDataConfig.FileMetaDataCfg(
                flags=ConfigFlags,
                stream=outputStreamObject,
            )
        )
"""
from AthenaConfiguration import ComponentFactory, ComponentAccumulator
from AthenaCommon import Logging
from AthenaServices import MetaDataSvcConfig


def FileMetaDataCfg(flags, stream, streamName=None, key="FileMetaData"):
    """Add tools creating and propogating xAOD::FileMetaData"""
    if not streamName:
        streamName = stream.name
    result = ComponentAccumulator.ComponentAccumulator()
    components = ComponentFactory.CompFactory

    # attach the FileMetaData creator to the output stream
    creator = components.xAODMaker.FileMetaDataCreatorTool(
        '{}_FileMetaDataCreatorTool'.format(streamName),
        OutputKey=key,
        StreamName=streamName,
    )
    stream.HelperTools.append(creator)
    stream.MetadataItemList += [
        "xAOD::FileMetaData#{}".format(key),
        "xAOD::FileMetaDataAuxInfo#{}Aux.".format(key),
    ]

    # Set up the tool propogating the FileMetaData
    result.merge(
        MetaDataSvcConfig.MetaDataSvcCfg(
            flags,
            ["xAODMaker::FileMetaDataTool"]
        )
    )

    return result


def main():
    """Run a job writing a file with FileMetaData"""
    msg = Logging.logging.getLogger("FileMetaDataConfig")
    msg.info("FileMetaData config OK")


if __name__ == "__main__":
    main()
