#!/usr/bin/env python
#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

# This script parses outputs of trigger nightly test post-processing steps and creates a JSON file with extra data
# other than result codes (which are handled by ART).

import json
import re
import sys
import logging
import os.path
from collections import OrderedDict


class LastUpdatedOrderedDict(OrderedDict):
    'Store items in the order the keys were last added'

    def __setitem__(self, key, value):
        if key in self:
            del self[key]
        OrderedDict.__setitem__(self, key, value)


def find_line_in_file(pattern, filename):
    if not os.path.isfile(filename):
        logging.warning("Cannot open file {}".format(filename))
        return None
    with open(filename) as logfile:
        lines = re.findall("{}.*$".format(pattern), logfile.read(), re.MULTILINE)
        if len(lines) == 0:
            logging.warning("Could not find pattern \"{}\" in file {}".format(pattern, filename))
            return None
        return lines[0]


def get_num_from_checklog(filename):
    line = find_line_in_file('Found messages in', filename)
    if line is None:
        logging.warning("Cannot extract number of messages from {}".format(filename))
        return None
    logging.debug("line: {}".format(line))
    m = re.search('\((.+?)\):', line)
    return m.group(1)


def get_num_histos(filename):
    line = find_line_in_file('Total histograms:', filename)
    if line is None:
        logging.warning("Cannot extract number of histograms from {}".format(filename))
        return None
    logging.debug("line: {}".format(line))
    return line.split()[-1]


def convert_to_megabytes(number, unit):
    multipliers = {
        'B': 1.0/(1024**2),
        'kB': 1.0/1024,
        'MB': 1,
        "GB": 1024,
        'TB': 1024**2
    }
    for unit_name, mult in multipliers.iteritems():
        if unit_name == unit:
            return float(number)*mult
    logging.error("Unit conversion failed from {} to MB".format(unit))
    return None


def extract_mem(line):
    words = line[0].split()
    mem_end = words[5:7]
    logging.debug("mem_end = {}".format(mem_end))
    mem_delta = words[8:10]
    logging.debug("mem_delta = {}".format(mem_delta))
    mem_mb = convert_to_megabytes(mem_end[0], mem_end[1])
    logging.debug("mem_mb = {}".format(mem_mb))
    delta_mb = convert_to_megabytes(mem_delta[0], mem_delta[1])
    logging.debug("delta_mb = {}".format(delta_mb))
    return mem_mb, delta_mb


def analyse_perfmon(filename):
    if not os.path.isfile(filename):
        logging.warning("Cannot open file {}".format(filename))
        return None
    with open(filename) as logfile:
        first_line = -1
        last_line = -1
        all_lines = logfile.readlines()
        for i, line in enumerate(all_lines):
            if first_line >= 0 and last_line >= 0:
                break
            if "=== [evt - slice] ===" in line:
                first_line = i
            elif "=== [fin - slice] ===" in line:
                last_line = i
        if first_line < 0 or last_line < 0:
            logging.warning("Cannot extract memory usage information from {}".format(filename))
            return None
        evt_mon_lines = all_lines[first_line:last_line]
        vmem_line = re.findall("^VMem.*$", '\n'.join(evt_mon_lines), re.MULTILINE)
        rss_line = re.findall("^RSS.*$", '\n'.join(evt_mon_lines), re.MULTILINE)
        logging.debug("vmem_line = {}".format(vmem_line))
        logging.debug("rss_line = {}".format(rss_line))
        if len(vmem_line) == 0:
            logging.warning("Cannot extract VMem information from {}".format(filename))
        if len(rss_line) == 0:
            logging.warning("Cannot extract RSS information from {}".format(filename))
        vmem, dvmem = extract_mem(vmem_line)
        rss, drss = extract_mem(rss_line)
        data = LastUpdatedOrderedDict()
        data['vmem'] = "{0:.3f}".format(vmem)
        data['delta-vmem'] = "{0:.3f}".format(dvmem)
        data['rss'] = "{0:.3f}".format(rss)
        data['delta-rss'] = "{0:.3f}".format(drss)
        return data


def main():
    logging.basicConfig(stream=sys.stdout,
                        format='%(levelname)-8s %(message)s',
                        level=logging.INFO)

    data = LastUpdatedOrderedDict()

    # Get number of errors
    ne = get_num_from_checklog('checklog.log')
    logging.debug("ne: {}".format(ne))
    if ne is None:
        logging.warning("Failed to read number of errors from the log")
        data['num-errors'] = 'n/a'
    else:
        data['num-errors'] = ne

    # Get number of warnings
    nw = get_num_from_checklog('warnings.log')
    logging.debug("nw: {}".format(nw))
    if nw is None:
        logging.warning("Failed to read number of warnings from the log")
        data['num-warnings'] = 'n/a'
    else:
        data['num-warnings'] = nw

    # Get number of histograms
    nh = get_num_histos('histSizes.log')
    logging.debug("nh: {}".format(nh))
    if nh is None:
        logging.warning("Failed to read number of histograms from the log")
        data['num-histograms'] = 'n/a'
    else:
        data['num-histograms'] = nh

    # Get memory usage information
    perfmon_data = analyse_perfmon("ntuple.perfmon.summary.txt")
    if perfmon_data is None:
        logging.warning("Failed to read memory usage information from the log")
        data['memory-usage'] = 'n/a'
    else:
        data['memory-usage'] = perfmon_data

    # Save data to JSON file
    with open('extra-results.json', 'w') as outfile:
        json.dump(data, outfile, indent=4)


if "__main__" in __name__:
    sys.exit(main())
