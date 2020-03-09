#!/usr/bin/env python
#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

# This script parses outputs of HLTMPPU ROS hit simulation, converts them
# to JSON and appends to extra-results.json

import logging
import re
import sys
import os.path
import json
from collections import OrderedDict
from enum import IntEnum


class LastUpdatedOrderedDict(OrderedDict):
    'Store items in the order the keys were last added'

    def __setitem__(self, key, value):
        if key in self:
            del self[key]
        OrderedDict.__setitem__(self, key, value)


class LookUpDict(IntEnum):
    'Defines columns in hitstats file'
    ROS_NAME = 0
    NUM_ROBS = 1
    TOTAL_HITS_PER_EVT = 2
    TOTAL_ROB_FRAC_PER_EVT = 3
    TOTAL_BYTES_PER_EVT = 4
    NOBLD_HITS_PER_EVT = 5
    NOBLD_ROB_FRAC_PER_EVT = 6
    NOBLD_BYTES_PER_EVT = 7
    EVBLD_HITS_PER_EVT = 8
    EVBLD_ROB_FRAC_PER_EVT = 9
    EVBLD_BYTES_PER_EVT = 10


def get_lines(pattern, filename):
    if not os.path.isfile(filename):
        logging.warning("Cannot open file {}".format(filename))
        return None
    with open(filename) as logfile:
        lines = re.findall("{}.*$".format(pattern),
                           logfile.read(), re.MULTILINE)
        if len(lines) == 0:
            logging.warning("Could not find pattern \"{}\" in file {}".format(pattern, filename))
            return None
        return lines


def split_lines(pattern, lines):
    result = []
    for line in lines:
        split_result = []
        for token in re.split(pattern, line):
            token = token.replace(' ', '')
            if not token == '':
                split_result.append(token)
        result.append(split_result)
    return result


def main():
    logging.basicConfig(stream=sys.stdout,
                        format='%(levelname)-8s %(message)s',
                        level=logging.INFO)

    ros_stats = LastUpdatedOrderedDict()

    lines = get_lines(r'^\s*ROS-', 'ros_hitstats_reject.txt')
    table = split_lines(r'\||,', lines)

    for row in table:
        name = row[LookUpDict.ROS_NAME]
        hit_rate = row[LookUpDict.TOTAL_ROB_FRAC_PER_EVT]
        data_rate = row[LookUpDict.TOTAL_BYTES_PER_EVT]
        ros_stats[name] = LastUpdatedOrderedDict({
            'hits-per-evt': hit_rate,
            'kbytes-per-evt': float(data_rate)/1000
        })

    data = LastUpdatedOrderedDict()
    output_file = 'extra-results.json'
    if os.path.isfile(output_file):
        with open(output_file) as f:
            data.update(json.load(f, object_pairs_hook=LastUpdatedOrderedDict))

    data.update({'ros-stats': ros_stats})

    with open(output_file, 'w') as f:
        json.dump(data, f, indent=4)


if "__main__" in __name__:
    sys.exit(main())
