#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# This file defines the default input files for trigger validation tests
# and keywords to retrieve them in test configuration

'''
Common way to configure input samples for Trigger ART tests
'''

import os
import json

from TrigValTools.TrigValSteering.Common import get_logger, find_file_in_path
from functools import lru_cache

input_json = 'TrigValTools/TrigValInputs.json'


class TrigValInput(object):
    '''
    Input object representing a data sample of a given format. The sample can
    consist of multiple files.
    '''

    def __init__(self, keyword, source, format, paths):
        self.log = get_logger()
        self.keyword = keyword

        allowed_sources = ['data', 'mc']
        if source not in allowed_sources:
            self.log.error('source has to be one of %s', allowed_sources)
            self.source = None
        else:
            self.source = source

        allowed_formats = ['BS', 'HITS', 'RDO', 'ESD', 'AOD']
        if format not in allowed_formats:
            self.log.error('format has to be one of %s', allowed_formats)
            self.format = None
        else:
            self.format = format

        if not isinstance(paths, list):
            self.log.error('paths have to be provided as a list')
            self.paths = None
        else:
            self.paths = []
            for path in paths:
                if not os.path.isfile(path):
                    self.log.error('Cannot access file: %s', path)
                else:
                    self.paths.append(path)
            if len(self.paths) == 0:
                self.log.error('Failed to parse paths')
                self.paths = None

    def __str__(self):
        str = 'TrigValInput object:\n'
        str += '---- keyword: {}\n'.format(self.keyword)
        str += '---- source: {}\n'.format(self.source)
        str += '---- format: {}\n'.format(self.format)
        str += '---- paths: {}\n'.format(self.paths)
        str += '---- is_valid: {}'.format(self.is_valid())
        return str

    def is_valid(self):
        for param in [self.keyword, self.source, self.format, self.paths]:
            if param is None:
                return False
        return True


@lru_cache
def load_input_json():
    '''Reads the json file with input definitions and returns the data as dictionary'''

    log = get_logger()

    input_json_fullpath = find_file_in_path(input_json, 'DATAPATH')
    if not input_json_fullpath:
        log.error('Failed to determine full path for input JSON %s', input_json)
        return None

    log.debug('Reading %s', input_json_fullpath)
    with open(input_json_fullpath) as data_file:
        return json.load(data_file)


def is_input_defined(keyword):
    '''Checks if the keyword exists in the input json file'''
    data = load_input_json()
    return keyword in data


def get_input(keyword):
    '''Common getter function to retrieve inputs by keyword'''

    log = get_logger()

    data = load_input_json()
    if keyword not in data.keys():
        log.error('Failed to find keyword "%s" in input JSON %s',
                  keyword, input_json)
        return None

    data_object = data[keyword]

    result = TrigValInput(
        keyword,
        data_object["source"],
        data_object["format"],
        data_object["paths"]
    )

    if result.is_valid():
        return result
    else:
        log.error('Failed to create a valid input object')
        return None
