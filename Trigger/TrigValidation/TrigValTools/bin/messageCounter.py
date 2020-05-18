#!/usr/bin/env python
#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''
Script listing and counting Athena messages between two points in a log file,
typically between start and end of event loop.
'''

from __future__ import print_function
import sys
import os
import re
import logging
import argparse
import json
from collections import OrderedDict
import six


default_ignore_patterns = [
    r'TrigCOOLUpd.*INFO.*Reload',
    r'TrigCOOLUpd.*INFO.*Invalidated',
    r'warn.*frontier.c.*Request',
    r'warn.*frontier.c.*Trying',
]


def get_parser():
    parser = argparse.ArgumentParser(usage='%(prog)s [options] LogFiles',
                                     description=__doc__)
    parser.add_argument('logfiles',
                        metavar='LogFiles',
                        nargs='+',
                        help='Log file(s) to analyse. Multiple space-separated file names may be given.')
    parser.add_argument('-s', '--startPattern',
                        metavar='PATTERN',
                        help='Pattern to match start point for log analysis. If empty, log files are analysed from first line.')
    parser.add_argument('-e', '--endPattern',
                        metavar='PATTERN',
                        help='Pattern to match end point for log analysis. If empty, log files are analysed until last line.')
    parser.add_argument('-i', '--ignore',
                        metavar='PATTERN',
                        action='append',
                        default=[],
                        help='Add an ignore pattern to the default ones.' +
                             'The option can be specified multiple times. Defaults are: {:s}'.format(str(default_ignore_patterns)))
    parser.add_argument('-p', '--printMessages',
                        action='store_true',
                        help='Print the messages found in analysed files')
    parser.add_argument('--saveAll',
                        action='store_true',
                        help='Store all the messages into the output JSON file')
    parser.add_argument('-v', '--verbose',
                        action='store_true',
                        help='Increase output verbosity')
    return parser


def extract_messages(lines, start, end, ignore):
    patterns = [re.compile(p) for p in ['FATAL', 'ERROR', 'WARNING', 'INFO', 'DEBUG', 'VERBOSE']]
    result = OrderedDict()
    for p in patterns:
        result[p.pattern] = []
    result['other'] = []
    result['ignored'] = []

    running = False if start else True
    for line in lines:
        if not running and start and start.search(line):
            running = True
            continue
        if running and end and end.search(line):
            running = False
            break
        if running:
            matched = False
            ignored = False
            for p in ignore:
                if p.search(line):
                    result['ignored'].append(line)
                    ignored = True
                    break
            if ignored:
                continue
            for p in patterns:
                if p.search(line):
                    result[p.pattern].append(line)
                    matched = True
                    break
            if not matched:
                result['other'].append(line)

    return result


def make_summary(result):
    summary = OrderedDict()
    for p in result.keys():
        summary[p] = len(result[p])
    total = sum(summary.values())
    summary['all'] = total
    return summary


def print_result(summary, full_result, print_messages=False):
    summary_str = 'Found the following number of messages:\n'
    for p, n in six.iteritems(summary):
        summary_str += '{:8d} {:s} messages\n'.format(n, p)
    logging.info(summary_str)
    if print_messages:
        for p, lines in six.iteritems(full_result):
            logging.info('##### The following %s messages were found #####', p)
            for line in lines:
                print(line, end='')  # noqa: ATL901


def save_summary_to_json(result, filename):
    logging.info('Saving results to %s', filename)
    with open(filename, 'w') as f:
        json.dump(result, f, indent=4)


def save_all_to_json(full_result, filename):
    logging.info('Saving results to %s', filename)
    with open(filename, 'w') as f:
        json.dump(full_result, f, indent=4)


def main():
    args = get_parser().parse_args()
    logging.basicConfig(stream=sys.stdout,
                        format='messageCounter %(levelname)-8s %(message)s',
                        level=logging.DEBUG if args.verbose else logging.INFO)

    start = re.compile(args.startPattern) if args.startPattern else None
    end = re.compile(args.endPattern) if args.endPattern else None
    args.ignore.extend(default_ignore_patterns)
    ignore = [re.compile(p) for p in args.ignore]

    for fname in args.logfiles:
        if not os.path.isfile(fname):
            logging.error('Cannot open file %s, skipping', fname)
            continue
        logging.info('Analysing file %s', fname)
        encargs = {} if six.PY2 else {'encoding' : 'utf-8'}
        with open(fname, **encargs) as f:
            messages = extract_messages(f, start, end, ignore)
        summary = make_summary(messages)
        print_result(summary, messages, args.printMessages)
        out_file_name = 'MessageCount.{:s}.json'.format(fname)
        save_summary_to_json(summary, out_file_name)
        if args.saveAll:
            all_out_file_name = 'Messages.{:s}.json'.format(fname)
            save_all_to_json(messages, all_out_file_name)


if '__main__' in __name__:
    sys.exit(main())
