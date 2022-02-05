#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__all__ = ['metadata', 'convert_itemList', 'convert_metadata_items']

metadata = {}


def _setup():
    from PyUtils.MetaReader import read_metadata

    from AthenaCommon.Logging import logging
    msg = logging.getLogger('MetaReaderPeeker')

    global metadata

    # get input file name
    # Start by checking ConfigFlags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    inFiles = ConfigFlags.Input.Files
    if inFiles == ['_ATHENA_GENERIC_INPUTFILE_NAME_']:
        # Input files have not been set in ConfigFlags
        # Fall back to the legacy configuration mechanisms
        try:
            msg.warning("Input files not set ConfigFlags.Input.Files. Falling back to the legacy way of obtaining input file names")
            from RecExConfig.RecoFunctions import InputFileNames
            inFiles = InputFileNames()
        except ModuleNotFoundError:
            try:
                msg.warning("In a release without RecExConfig, trying AthenaCommonFlags")
                from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
                inFiles = athenaCommonFlags.FilesInput()
            except ModuleNotFoundError:
                msg.warning("Failed to load any module containing list of input files")
                pass

    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    if athenaCommonFlags.isOnline() and (not inFiles or all([f.strip() == '' for f in inFiles])):
        # set minimal items of inputFileSummary
        metadata = {
          'file_type':'BS',
          'eventTypes':['IS_DATA','IS_ATLAS','IS_PHYSICS'],
          'TagStreamsRef':''
        }
    else:
        if len(inFiles) < 1:
            msg.warning("No input files specified yet! Cannot do anything.")
            return
            
            
        for inFile in inFiles:

            metadatas = read_metadata(inFile, mode='peeker', promote=True, ignoreNonExistingLocalFiles=True)
            
            for foundFile, metadata in metadatas.items():
                
                metadata['file_name'] = inFile
                if metadata.get("nentries"):
                    break
            else:
                continue # This make the previous break to exit from both for loops
            break
            
            # if no nentries > 0 metadata is found, it will keep the last
            


# convert_itemList and convert_metadata_items have the same implementation as the one in MetaReaderPeekerFull.
# If there are changes, these must be modified in both files.

def convert_itemList(layout=None):
    global metadata
    from PyUtils.MetaReader import convert_itemList as convert_itemList_MetaReader
    return convert_itemList_MetaReader(metadata, layout)


def convert_metadata_items(layout=None):
    metadata_items = None

    if 'metadata_items' in metadata:
        metadata_items = metadata['metadata_items']

    # add key to match the athfile output
    if metadata_items is not None:
        metadata_items[';00;MetaDataSvc'] = 'DataHeader'

    if layout is None:
        if metadata_items is not None:
            return [(class_name, name) for name, class_name in metadata_items.items()]
        else:
            return []

    elif layout == '#join':
        if metadata_items is not None:
            return [class_name + '#' + name for name, class_name in metadata_items.items()]
        else:
            return []

    elif layout == 'dict':
        if metadata_items is not None:

            from collections import defaultdict
            result = defaultdict(list)

            for name, class_name in metadata_items.items():
                result[class_name].append(name)

            return dict(result)

        else:
            return {}

    else:
        raise ValueError('"{}" is not a valid option for layout.'
                         'Please use one of the 3 options: None, "#join"or "dict" for the layout parameter'.format(layout))


_setup()
