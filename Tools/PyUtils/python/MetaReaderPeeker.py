#!/usr/bin/env python

# info

__all__ = ['metadata', 'metadata_all_files',
           'convert_itemList', 'convert_metadata_items']

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

    inFiles = InputFileNames()
    if len(inFiles) < 1:
        msg.warning("No input files specified yet! Cannot do anything.")
        return

    metadata_all_files = read_metadata(inFiles, mode='peeker', promote=True)

    first_filename = inFiles[0]

    metadata = metadata_all_files[first_filename]
    metadata['file_name'] = first_filename


# convert_itemList and convert_metadata_items have the same implementation as the one in MetaReaderPeekerFull.
# If there are changes, these must be modified in both files.

def convert_itemList(layout=None):
    # Find the itemsList:
    item_list = None

    if 'itemList' in metadata:
        item_list = metadata['itemList']
    else:

        current_key = None

        for key in metadata:
            if key in metadata['metadata_items'] and metadata['metadata_items'][key] == 'EventStreamInfo_p3':
                current_key = key
                break
        if current_key is not None:
            item_list = metadata[current_key]['itemList']

    if item_list is not None:

        if layout is None:
            return item_list

        elif layout == '#join':
            return [k + '#' + v for k, v in item_list if k]


        elif layout == 'dict':
            from collections import defaultdict
            dic = defaultdict(list)

            for k, v in item_list:
                dic[k].append(v)

            return dict(dic)


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
