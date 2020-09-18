#!/usr/bin/env python
#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

# This script parses outputs of trigger nightly test post-processing steps
# and creates a JSON file with extra data other than result codes
# (which are handled by ART).

import json
import re
import sys
import logging
import os
from collections import OrderedDict
from TrigValTools.TrigARTUtils import first_existing_file, newest_file

logging.basicConfig(stream=sys.stdout,
                    format='%(levelname)-8s %(message)s',
                    level=logging.INFO)
do_prmon = True
mod_name = ''
try:
    mod_name = 'numpy'
    import numpy as np
    mod_name = 'pandas'
    from pandas import read_csv
    mod_name = 'scipy.optimize'
    from scipy.optimize import curve_fit
    mod_name = 'matplotlib'
    import matplotlib
    matplotlib.use('PDF')
    import matplotlib.pyplot as plt
except Exception as e:
    do_prmon = False
    logging.warning('Failed to import module %s, prmon output analysis will be skipped! %s', mod_name, e)

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
    m = re.search(r'\((.+?)\):', line)
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
    for unit_name, mult in multipliers.items():
        if unit_name == unit:
            return float(number)*mult
    logging.error("Unit conversion failed from {} to MB".format(unit))
    return None


def extract_mem_perfmon(line):
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
        vmem, dvmem = extract_mem_perfmon(vmem_line)
        rss, drss = extract_mem_perfmon(rss_line)
        data = LastUpdatedOrderedDict()
        data['vmem'] = "{0:.3f}".format(vmem)
        data['delta-vmem'] = "{0:.3f}".format(dvmem)
        data['rss'] = "{0:.3f}".format(rss)
        data['delta-rss'] = "{0:.3f}".format(drss)
        return data


def mem_func(x_arr, x_trans, init_slope, exec_slope):
    retval = []
    for x in x_arr:
        if x < x_trans:
            retval.append(init_slope * x)
        else:
            retval.append(exec_slope * x + (init_slope - exec_slope) * x_trans)
    return retval


def find_dmem_prmon(xdata, ydata, label, filename):
    popt, pcov = curve_fit(mem_func, xdata, ydata, bounds=(0, [0.9*max(xdata), np.inf, np.inf]))
    logging.debug("Fit result: %s", str(popt))
    plot_prmon(xdata, ydata, label, filename, popt)
    x_trans = popt[0]
    x_last = xdata.iloc[-1]
    dmem_v = mem_func([x_last, x_trans], popt[0], popt[1], popt[2])
    return dmem_v[0] - dmem_v[1]


def plot_prmon(xdata, ydata, name, filename, params=[]):
    plt.plot(xdata, ydata, 'b-', label=name)
    if len(params)>0:
        plt.plot(xdata, mem_func(xdata, *params), 'r-', label='{:s} fit, exec slope={:.2f} kB/s'.format(name, params[2]))
    plt.xlabel('wtime [s]')
    plt.ylabel(name+' [kB]')
    plt.legend()
    plt.title('{:s} from {:s}'.format(name, filename))
    plt.savefig('prmon_{:s}.pdf'.format(name), bbox_inches='tight')
    plt.clf()


def analyse_prmon(filename):
    try:
        prmon_data = read_csv(filename, sep='\t')
    except IOError:
        logging.warning("Cannot open file {}".format(filename))
        return None
    time_v = prmon_data['wtime']
    pss_v = prmon_data['pss']
    rss_v = prmon_data['rss']
    vmem_v = prmon_data['vmem']
    data = LastUpdatedOrderedDict()
    data['vmem'] = "{0:.3f}".format(convert_to_megabytes(max(vmem_v), 'kB'))
    data['rss'] = "{0:.3f}".format(convert_to_megabytes(max(rss_v), 'kB'))
    data['pss'] = "{0:.3f}".format(convert_to_megabytes(max(pss_v), 'kB'))
    if len(time_v) < 80:
        logging.info('Not enough prmon data points, skipping memory slope fitting')
        # Save plots without fitting and return
        plot_prmon(time_v, pss_v, 'pss', filename)
        plot_prmon(time_v, rss_v, 'rss', filename)
        plot_prmon(time_v, vmem_v, 'vmem', filename)
        return data
    d_pss = find_dmem_prmon(time_v, pss_v, 'pss', filename)
    d_rss = find_dmem_prmon(time_v, rss_v, 'rss', filename)
    d_vmem = find_dmem_prmon(time_v, vmem_v, 'vmem', filename)
    data['delta-vmem'] = "{0:.3f}".format(convert_to_megabytes(d_vmem, 'kB'))
    data['delta-rss'] = "{0:.3f}".format(convert_to_megabytes(d_rss, 'kB'))
    data['delta-pss'] = "{0:.3f}".format(convert_to_megabytes(d_pss, 'kB'))
    return data


def main():
    data = LastUpdatedOrderedDict()

    # Get number of errors
    checklog_log = first_existing_file(['checklog.log', 'CheckLog.log'])
    ne = get_num_from_checklog(checklog_log) if checklog_log else None
    logging.debug("ne: {}".format(ne))
    if ne is None:
        logging.warning("Failed to read number of errors from the log")
        data['num-errors'] = 'n/a'
    else:
        data['num-errors'] = ne

    # Get number of warnings
    warnings_log = first_existing_file(['warnings.log', 'Warnings.log'])
    nw = get_num_from_checklog(warnings_log) if warnings_log else None
    logging.debug("nw: {}".format(nw))
    if nw is None:
        logging.warning("Failed to read number of warnings from the log")
        data['num-warnings'] = 'n/a'
    else:
        data['num-warnings'] = nw

    # Get number of histograms
    histcount_log = first_existing_file(['histSizes.log', 'HistCount.log'])
    nh = get_num_histos(histcount_log) if histcount_log else None
    logging.debug("nh: {}".format(nh))
    if nh is None:
        logging.warning("Failed to read number of histograms from the log")
        data['num-histograms'] = 'n/a'
    else:
        data['num-histograms'] = nh

    # Get memory usage information from prmon
    if do_prmon:
        prmon_log = newest_file(r'prmon\..*\.txt')
        if not prmon_log:
            prmon_log = first_existing_file(['prmon.full.RDOtoRDOTrigger', 'prmon.full.RAWtoESD', 'prmon.full.ESDtoAOD'])
        if not prmon_log:
            logging.info("No prmon output found, the result will be empty")
            data['prmon'] = 'n/a'
        else:
            logging.info("Analysing prmon output from %s", prmon_log)
            prmon_data = analyse_prmon(prmon_log)
            if prmon_data is None:
                logging.warning("Could not analyse prmon output, the result will be empty")
                data['prmon'] = 'n/a'
            else:
                data['prmon'] = prmon_data

    # Get memory usage information from PerfMon
    perfmon_log = newest_file(r'.*perfmon\.summary\.txt')
    if not perfmon_log:
        logging.info("No PerfMon output found, the result will be empty")
        data['memory-usage'] = 'n/a'
    else:
        logging.info("Analysing PerfMon output from %s", perfmon_log)
        perfmon_data = analyse_perfmon(perfmon_log)
        if perfmon_data is None:
            logging.warning("Could not analyse PerfMon output, the result will be empty")
            data['memory-usage'] = 'n/a'
        else:
            data['memory-usage'] = perfmon_data

    # Save data to JSON file
    with open('extra-results.json', 'w') as outfile:
        json.dump(data, outfile, indent=4)


if "__main__" in __name__:
    sys.exit(main())
