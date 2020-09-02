#!/usr/bin/env python
#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''Script to dump trigger counts to a text file'''

import sys
import argparse
import logging
import json
import ROOT
from collections import OrderedDict

total_events_key = 'TotalEventsProcessed'
column_width = 10  # width of the count columns for print out
name_width = 50  # width of the item name column for print out


def get_parser():
    parser = argparse.ArgumentParser(usage='%(prog)s [options]',
                                     description=__doc__)
    parser.add_argument('-f', '--inputFile',
                        metavar='PATH',
                        default='expert-monitoring.root',
                        help='Name of input root file')
    parser.add_argument('-r', '--referenceFile',
                        metavar='PATH',
                        help='Name of reference root file')
    parser.add_argument('-v', '--verbose',
                        action='store_true',
                        help='Increase output verbosity')
    parser.add_argument('-p', '--printOnly',
                        action='store_true',
                        default=False,
                        help='Print counts instead of saving to file')
    parser.add_argument('-d', '--diffOnly',
                        action='store_true',
                        default=False,
                        help='Only store out of tolerance results (does not change JSON)')
    parser.add_argument('--json',
                        metavar='PATH',
                        nargs='?',
                        const='chainDump.json',
                        help='Save outputs also to a json file with the given name or %(const)s if no name is given')
    parser.add_argument('--fracTolerance',
                        metavar='FRAC',
                        type=float,
                        default=0.001,
                        help='Tolerance as a fraction, default = %(default)s. '
                             'Flagged diffs must exceed all tolerances')
    parser.add_argument('--intTolerance',
                        metavar='NUM',
                        type=int,
                        default=2,
                        help='Tolerance as a number of counts, default = %(default)s. '
                             'Flagged diffs must exceed all tolerances')
    parser.add_argument('--countHists',
                        metavar='HISTS',
                        nargs='+',
                        default=[
                            'HLTFramework/TrigSignatureMoniMT/SignatureAcceptance',
                            'TrigSteer_HLT/ChainAcceptance',
                            'TrigSteer_HLT/NumberOfActiveTEs',
                            'HLTFramework/TrigSignatureMoniMT/DecisionCount',
                            'CTPSimulation/L1ItemsAV',
                            'L1/CTPSimulation/output/tavByName'],
                        help='Histograms to use for counts dump. All existing '
                             'histograms from the list are used, default = %(default)s')
    parser.add_argument('--totalHists',
                        metavar='HISTS',
                        nargs='+',
                        default=[
                            'TrigSteer_HLT/NInitialRoIsPerEvent',
                            'HLTFramework/L1Decoder/RoIsEM/count'],
                        help='Histograms to use for total events. First existing '
                             'histogram from the list is used, default = %(default)s')
    parser.add_argument('--histDict',
                        metavar='DICT',
                        nargs='+',
                        default=[
                            'HLTFramework/TrigSignatureMoniMT/SignatureAcceptance:HLTChain',
                            'TrigSteer_HLT/ChainAcceptance:HLTChain',
                            'TrigSteer_HLT/NumberOfActiveTEs:HLTTE',
                            'HLTFramework/TrigSignatureMoniMT/DecisionCount:HLTDecision',
                            'CTPSimulation/L1ItemsAV:L1AV',
                            'L1/CTPSimulation/output/tavByName:L1AV'],
                        help='Dictionary defining names of output text files for each '
                             'histogram, default = %(default)s')
    return parser


def open_root_file(file_path):
    f = ROOT.TFile(file_path)
    if f.IsOpen() and not f.IsZombie():
        return f
    else:
        return None


def load_histograms(root_file, hist_paths):
    hist_dict = {}
    for hist_path in hist_paths:
        h = root_file.Get(hist_path)
        if not isinstance(h, ROOT.TH1):
            logging.debug('Cannot open histogram %s, skipping', hist_path)
            continue
        logging.debug('Loaded histogram %s', hist_path)
        hist_dict[hist_path] = h
    return hist_dict


def get_counts(hist):
    '''
    Extract {xlabel, value} dictionary from a histogram. Values are stored as integers.
    If histogram is 2D, the last y-bin is used to extract the value.
    '''

    nbinsx = hist.GetNbinsX()
    nbinsy = hist.GetNbinsY()
    counts = {}
    for b in range(1, nbinsx+1):
        label = hist.GetXaxis().GetBinLabel(b)
        if not label:
            logging.debug('Bin %d in histogram %s has no label, skipping', b, hist.GetName())
            continue

        value = hist.GetBinContent(b) if hist.GetDimension() == 1 else hist.GetBinContent(b, nbinsy)
        counts[label] = int(value)

    return counts

def get_2D_counts(hist):
    '''
    Extract {xlabel_ylabel, value} dictionary from a histogram. Values are stored as integers.
    '''
    nbinsx = hist.GetNbinsX()
    nbinsy = hist.GetNbinsY()
    counts = {}
    for x in range(1, nbinsx+1):
        label = hist.GetXaxis().GetBinLabel(x)
        if not label:
            logging.debug('Bin %d in histogram %s has no label, skipping', x, hist.GetName())
            continue

        for y in range(3, nbinsy): #get only steps
            name = label + '_' + hist.GetYaxis().GetBinLabel(y)
            name = name.replace(' ', '')
            value = hist.GetBinContent(x, y)
            counts[name] = int(value)

    return counts

def make_counts_json_dict(in_counts, ref_counts):
    counts = OrderedDict()
    all_keys = set(in_counts.keys())
    all_keys.update(ref_counts.keys())
    keys_sorted = sorted(all_keys)
    for k in keys_sorted:
        v = in_counts[k] if k in in_counts else 'n/a'
        ref_v = ref_counts[k] if k in ref_counts else 'n/a'
        counts[k] = {
            'count': v,
            'ref_count': ref_v,
            'ref_diff': 'n/a'  # Filled in compare_ref
        }
    return counts


def parse_name_dict(name_dict_as_list):
    name_dict = {}
    for kv in name_dict_as_list:
        kv_split = kv.split(':')
        if len(kv_split) < 2:
            continue
        name_dict[kv_split[0]] = kv_split[1]
    return name_dict


def get_text_name(hist_name, name_dict):
    if hist_name in name_dict.keys():
        return name_dict[hist_name]
    else:
        return hist_name.replace('/', '_')


def count_diff(count_in, count_ref, total_in, total_ref, thr_frac, thr_num):
    # normalise input counts to total events in reference
    count_in_norm = (count_in / float(total_in)) * total_ref
    frac = count_in_norm / float(count_ref) if count_ref != 0 else None

    num_diff = abs(count_in_norm - count_ref) > thr_num
    frac_diff = abs(frac - 1.0) > thr_frac if frac else True

    return num_diff and frac_diff


def compare_ref(json_dict, thr_frac, thr_num):
    results = []
    in_total = json_dict[total_events_key]['count']
    ref_total = json_dict[total_events_key]['ref_count']
    for text_name in sorted(json_dict.keys()):
        if text_name == total_events_key:
            continue
        diff_val = []  # different counts in input and reference
        missing_ref = []  # input count exists but reference is n/a
        missing_val = []  # input count is n/a but reference exists
        for item_name, item_counts in json_dict[text_name]['counts'].items():
            v = item_counts['count']
            ref_v = item_counts['ref_count']
            if v == 'n/a':
                missing_val.append([item_name, v, ref_v])
                item_counts['ref_diff'] = True
            elif ref_v == 'n/a':
                missing_ref.append([item_name, v, ref_v])
                item_counts['ref_diff'] = True
            elif count_diff(v, ref_v, in_total, ref_total, thr_frac, thr_num):
                diff_val.append([item_name, v, ref_v])
                item_counts['ref_diff'] = True
            else:
                item_counts['ref_diff'] = False
        good = True
        if len(diff_val) > 0:
            good = False
            dump = '\n'.join(
                ['  {e[0]:{nw}s} {e[1]:>{w}d} {e[2]:>{w}d}'.format(
                    e=element, nw=name_width, w=column_width) for element in diff_val])
            logging.info('%s has %d item(s) out of tolerance:\n%s',
                         text_name, len(diff_val), dump)
        if (len(missing_ref)) > 0:
            good = False
            dump = '\n'.join(['  {e[0]:s}'.format(e=element) for element in missing_ref])
            logging.info('%s has %d item(s) missing in the reference:\n%s',
                         text_name, len(missing_ref), dump)
        if (len(missing_val)) > 0:
            good = False
            dump = '\n'.join(['  {e[0]:s}'.format(e=element) for element in missing_val])
            logging.info('%s has %d item(s) missing with respect to the reference:\n%s',
                         text_name, len(missing_val), dump)
        if good:
            logging.info('%s is matching the reference', text_name)
            results.append(0)
        else:
            results.append(1)
    return max(results)


def print_counts(json_dict):
    for text_name in json_dict.keys():
        if text_name == total_events_key:
            logging.info('%s: %d', text_name, json_dict[text_name]['count'])
            continue
        hist_name = json_dict[text_name]['hist_name']
        counts = json_dict[text_name]['counts']
        no_ref = True
        for item_counts in counts.values():
            if item_counts['ref_count'] != 'n/a':
                no_ref = False
                break
        dump_lines = []
        for item_name, item_counts in counts.items():
            v = item_counts['count']
            line = '  {name:{nw}s} {val:>{w}s}'.format(name=item_name, val=str(v), nw=name_width, w=column_width)
            if not no_ref:
                ref_v = item_counts['ref_count']
                diff = item_counts['ref_diff']
                line += ' {val:>{w}s}'.format(val=str(ref_v), w=column_width)
                if diff:
                    line += ' <<<<<<<<<<'
            dump_lines.append(line)
        logging.info('Writing %s counts from histogram %s:\n%s', text_name, hist_name, '\n'.join(dump_lines))


def format_txt_count(count):
    if type(count) is int:
        return '{val:>{w}d}'.format(val=count, w=column_width)
    elif type(count) is not str:
        logging.error('Unexpected count type %s', type(count))
        count = 'ERROR'
    if count == 'n/a':
        count = '-'
    return '{val:>{w}s}'.format(val=count, w=column_width)


def write_txt_output(json_dict, diff_only=False):
    for text_name in sorted(json_dict.keys()):
        if text_name == total_events_key:
            logging.info('Writing total event count to file %s.txt', text_name)
            with open('{:s}.txt'.format(text_name), 'w') as outfile:
                outfile.write('{:d}\n'.format(json_dict[text_name]['count']))
            continue
        hist_name = json_dict[text_name]['hist_name']
        logging.info('Writing counts from histogram %s to file %s.txt', hist_name, text_name)
        counts = json_dict[text_name]['counts']
        no_ref = True
        for item_counts in counts.values():
            if item_counts['ref_count'] != 'n/a':
                no_ref = False
                break
        with open('{:s}.txt'.format(text_name), 'w') as outfile:
            for item_name, item_counts in counts.items():
                v = item_counts['count']
                line = '{name:{nw}s} '.format(name=item_name, nw=name_width) + format_txt_count(v)
                if not no_ref:
                    ref_v = item_counts['ref_count']
                    diff = item_counts['ref_diff']
                    line += ' ' + format_txt_count(ref_v)
                    if diff:
                        line += ' <<<<<<<<<<'
                    elif diff_only:
                        line = None
                if line:
                    outfile.write(line+'\n')


def main():
    args = get_parser().parse_args()
    logging.basicConfig(stream=sys.stdout,
                        format='%(levelname)-8s %(message)s',
                        level=logging.DEBUG if args.verbose else logging.INFO)

    name_dict = parse_name_dict(args.histDict)

    ##################################################
    # Open input files
    ##################################################

    in_file = open_root_file(args.inputFile)
    if not in_file:
        logging.error('Failed to open input file %s', args.inputFile)
        return 1
    logging.debug('Opened input file %s', args.inputFile)

    if args.referenceFile:
        ref_file = open_root_file(args.referenceFile)
        if not ref_file:
            logging.error('Failed to open input file %s', args.referenceFile)
            return 1
        logging.debug('Opened input file %s', args.referenceFile)

    ##################################################
    # Load histograms and total number of events
    ##################################################

    in_hists = load_histograms(in_file, args.countHists)
    if len(in_hists) == 0:
        logging.error('No count histograms could be loaded.')
        return 1
    logging.info('Loaded count histograms: %s', sorted(in_hists.keys()))

    in_total_hists = load_histograms(in_file, args.totalHists)
    if len(in_total_hists) == 0:
        logging.error('No total-events histogram could be loaded')
        return 1
    items = list(in_total_hists.items())
    in_total = items[0][1].GetEntries()
    logging.info('Loaded total-events histogram %s, number of events: %d',
                 items[0][0], in_total)

    ref_hists = None
    ref_total_hists = None
    ref_total = None
    if args.referenceFile:
        ref_hists = load_histograms(ref_file, args.countHists)
        logging.info('Loaded reference count histograms: %s', sorted(ref_hists.keys()))
        missing_refs = [k for k in in_hists.keys() if k not in ref_hists.keys()]
        if len(missing_refs) > 0:
            logging.error('Count histogram(s) %s missing in the reference', missing_refs)
            return 1
        ref_total_hists = load_histograms(ref_file, args.totalHists)
        if len(ref_total_hists) == 0:
            logging.error('No total-events reference histogram could be loaded')
            return 1
        ref_total = list(ref_total_hists.values())[0].GetEntries()
        logging.info('Loaded total-events reference histogram %s, number of events: %d',
                     list(ref_total_hists.keys())[0], ref_total)

    ##################################################
    # Extract counts from histograms
    ##################################################

    json_dict = OrderedDict()
    json_dict[total_events_key] = OrderedDict()
    json_dict[total_events_key]['hist_name'] = list(in_total_hists.keys())[0]
    json_dict[total_events_key]['count'] = int(in_total)
    json_dict[total_events_key]['ref_count'] = int(ref_total) if ref_total else 'n/a'

    for hist_name, hist in in_hists.items():
        text_name = get_text_name(hist_name, name_dict)
        if text_name in json_dict.keys():
            logging.error(
                'Name "%s" assigned to more than one histogram, ', text_name,
                'results would be overwritten. Use --countHists and ',
                '--histDict options to avoid duplicates. Exiting.')

        counts = get_2D_counts(hist) if text_name == 'HLTDecision' else get_counts(hist)
        ref_counts = {}
        if ref_hists:
            ref_hist = ref_hists[hist_name]
            ref_counts = get_2D_counts(ref_hist) if text_name == 'HLTDecision' else get_counts(ref_hist)
        d = make_counts_json_dict(counts, ref_counts)

        json_dict[text_name] = OrderedDict()
        json_dict[text_name]['hist_name'] = hist_name
        json_dict[text_name]['counts'] = d

    ##################################################
    # Compare to reference and produce output files
    ##################################################

    retcode = 0
    if args.referenceFile:
        logging.info('Comparing counts to reference')
        retcode = compare_ref(json_dict, args.fracTolerance, args.intTolerance)

    if args.printOnly and not args.diffOnly:
        logging.info('Printing counts instead of dumping to files because of --printOnly option')
        print_counts(json_dict)

    if not args.printOnly:
        write_txt_output(json_dict, args.diffOnly)

    if args.json:
        logging.info('Writing results to %s', args.json)
        with open(args.json, 'w') as outfile:
            json.dump(json_dict, outfile, sort_keys=True)

    return retcode


if '__main__' in __name__:
    sys.exit(main())
