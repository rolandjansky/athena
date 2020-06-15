#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__all__ = ['metadata', 'metadata_all_files', 'convert_itemList', 'convert_metadata_items']

metadata = {}
metadata_all_files = {}


def _setup():

    from PyUtils.MetaReader import read_metadata

    from AthenaCommon.Logging import logging
    msg = logging.getLogger('MetaReader')

    global metadata
    global metadata_all_files
    
    # get input file name
    from RecExConfig.RecoFunctions import InputFileNames

    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    if athenaCommonFlags.isOnline() and (not InputFileNames() or all([f.strip() == '' for f in InputFileNames()])):
        # set minimal items of inputFileSummary
        metadata = {
          'file_type':'BS',
          'eventTypes':['IS_DATA','IS_ATLAS','IS_PHYSICS'],
          'TagStreamsRef':''
        }
    else:
        inFiles = InputFileNames()
        if len(inFiles) < 1:
            msg.warning("No input files specified yet! Cannot do anything.")
            return

        metadata_all_files = read_metadata(inFiles, mode='full')

        first_filename = inFiles[-1]  # take only the last input file from the infiles

        metadata = metadata_all_files[first_filename]
        metadata['file_name'] = first_filename


# convert_itemList and convert_metadata_items have the same implementation as the one in MetaReaderPeeker.
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
