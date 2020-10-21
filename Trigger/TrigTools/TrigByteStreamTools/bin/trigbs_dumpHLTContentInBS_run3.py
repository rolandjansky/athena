#!/usr/bin/env python
#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''
Dump content of the HLT result and HLT related details from the event header
'''

import argparse
import eformat
from TrigByteStreamTools import hltResultMT

from AthenaCommon.Logging import logging
log = logging.getLogger('dumpHLTContentInBS')


def get_parser():
    parser = argparse.ArgumentParser(usage='%(prog)s [options] files',
                                     description=__doc__)
    parser.add_argument('files',
                        metavar='FILE', nargs='+',
                        help='RAW file to inspect')
    parser.add_argument('-n', '--events',
                        metavar='N', action='store', type=int,
                        help='Process N events')
    parser.add_argument('-s', '--skip',
                        metavar='N', action='store', type=int,
                        help='Skip N events')
    parser.add_argument('--l1',
                        action='store_true', default=False,
                        help='L1 trigger bits (from event header)')
    parser.add_argument('--l2',
                        action='store_true', default=False,
                        help='L2 trigger bits (from event header)')
    parser.add_argument('--ef', '--hlt',
                        action='store_true', default=False,
                        help='EF/HLT trigger bits (from event header)')
    parser.add_argument('--stag',
                        action='store_true', default=False,
                        help='stream tag')
    parser.add_argument('--efres', '--hltres',
                        action='store_true', default=False,
                        help='details of EF/HLT ROB payload')
    parser.add_argument('--sizes',
                        action='store_true', default=False,
                        help='dump info about EDM sizes per result')
    parser.add_argument('--sizeSummary',
                        action='store_true', default=False,
                        help='dump summary info about EDM sizes at the end')
    return parser


def decodeTriggerBits(words, base=32):
    bit_indices = []
    for iw in range(len(words)):
        bit_indices.extend([base*iw+i for i in range(base) if words[iw] & (1 << i)])
    return bit_indices


def header_info(event):
    '''Return a string with basic information about the event from the header'''

    'RunNumber: {:7d}, LB: {:4d}, LVL1_ID: {:10d}, Global_ID: {:10d}, BCID: {:4d}, TT: x{:2x}'
    info_str = 'RunNumber: {:6d}, '.format(event.run_no())
    info_str += 'LB: {:4d}, '.format(event.lumi_block())
    info_str += 'Global_ID: {:10d}, '.format(event.global_id())
    info_str += 'LVL1_ID: {:10d}, '.format(event.lvl1_id())
    info_str += 'BC_ID: {:4d}, '.format(event.bc_id())
    info_str += 'TT: x{:2x}'.format(event.lvl1_trigger_type())
    return info_str


def lvl1_bits(event):
    '''Return a string with information about LVL1 bits (IDs of items passed at TBP, TAP, TAV)'''

    info = event.lvl1_trigger_info()
    nwords = len(info)//3  # TBP, TAP, TAV
    lvl1_bits = [decodeTriggerBits(info[i*nwords:(i+1)*nwords]) for i in range(3)]
    info_str = 'L1 CTP IDs - TBP: {:s}\n'.format(str(lvl1_bits[0]))
    info_str += 'L1 CTP IDs - TAP: {:s}\n'.format(str(lvl1_bits[1]))
    info_str += 'L1 CTP IDs - TAV: {:s}'.format(str(lvl1_bits[2]))
    return info_str


def hlt_bits(event, l2=False):
    '''Return a string with information about passed chain IDs at L2/EF/HLT'''

    info = event.lvl2_trigger_info() if l2 else event.event_filter_info()
    hlt_bits = decodeTriggerBits(info)
    info_str = 'L2' if l2 else 'EF'
    info_str += ' passed chain IDs: {:s}'.format(str(hlt_bits))
    return info_str


def stream_tags(event):
    info_str = 'Stream Tags: '
    stags = [('{}_{}'.format(s.type, s.name)) for s in event.stream_tag()]
    info_str += '{:s}'.format(str(stags))
    return info_str


def hlt_result(event, print_sizes=False):
    num_hlt_robs = 0
    info_str = ""
    for rob in event.children():
        if rob.source_id().subdetector_id() != eformat.helper.SubDetector.TDAQ_HLT:
            continue
        num_hlt_robs += 1
        info_str += '\n-- {:s} SourceID: {:s}, Size: {:d} bytes'.format(
            rob.__class__.__name__,
            rob.source_id().human(),
            rob.fragment_size_word()*4
        )
        if print_sizes:
            raw_data = tuple(rob.rod_data())
            collections = hltResultMT.get_collections(raw_data)
            for coll in collections:
                indent = '----' if not coll.is_xAOD_decoration() else '------'
                info_str += '\n{:s} {:s}'.format(indent, str(coll))

    info_str = 'Found {:d} HLT ROBs'.format(num_hlt_robs) + info_str
    return info_str


def size_summary(events):
    data = {}
    # Format of the data dictionary:
    # {
    #   0: {
    #     'total': 123,
    #     'collections': {
    #       'collA': 22,
    #       'collB': 35
    #     }
    #   }
    # }
    for event in events:
        for rob in event.children():
            if rob.source_id().subdetector_id() != eformat.helper.SubDetector.TDAQ_HLT:
                continue
            module = rob.source_id().module_id()
            if module not in data.keys():
                data[module] = {'total': 0}
            data[module]['total'] += rob.fragment_size_word()*4
            if 'collections' not in data[module].keys():
                data[module]['collections'] = {}
            raw_data = tuple(rob.rod_data())
            for coll in hltResultMT.get_collections(raw_data):
                coll_name = coll.name()
                if coll_name in data[module]['collections'].keys():
                    data[module]['collections'][coll_name] += coll.size_bytes
                else:
                    data[module]['collections'][coll_name] = coll.size_bytes
    info_str = '='*20 + '\nSize summary:\n' + '='*20
    for module in data.keys():
        module_size = data[module]['total']
        module_size_per_evt = module_size / float(len(events))
        info_str += '\n-- TDAQ_HLT module {:d} total size {:d} bytes, {:.3f} bytes/event'.format(
            module, module_size, module_size_per_evt)
        sorted_colls = sorted(data[module]['collections'].items(),
                              key=lambda kv: kv[1],
                              reverse=True)
        max_name_len = len(max(data[module]['collections'].keys(), key=len))
        max_name_len = min(120, max_name_len)  # Make a reasonable limit to avoid line breaks
        for coll_name, coll_size in sorted_colls:
            coll_size_per_evt = coll_size / float(len(events))
            info_str += '\n---- {:{width}} {:12d} B {:12.3f} B/ev'.format(
                coll_name, coll_size, coll_size_per_evt, width=max_name_len)
    return info_str


def dump_info(bsfile, args):
    log.info('Opening %s', bsfile)
    input = eformat.istream(bsfile)
    offset = args.skip if args.skip else 0
    max_events = min(args.events, len(input)) if args.events else len(input)
    event_count = 0
    events = []

    # Loop over events
    for event in input:
        event_count += 1
        if event_count <= offset:
            continue
        if event_count > offset+max_events:
            break
        events.append(event)

        # Print header info
        print('{sep:s} Event: {:{width}d}, {:s} {sep:s}'.format(
              event_count, header_info(event),
              sep='='*20, width=len(str(max_events))))

        # Print L1/L2/HLT bits
        if args.l1:
            print(lvl1_bits(event))
        if args.l2:
            print(hlt_bits(event, l2=True))
        if args.ef:
            print(hlt_bits(event))

        # Print Stream Tags
        if args.stag:
            print(stream_tags(event))

        # HLT Result
        if args.efres or args.sizes:
            print(hlt_result(event, args.sizes))

    # Size summary (after the loop over events)
    if args.sizeSummary:
        print(size_summary(events))


if '__main__' in __name__:
    args = get_parser().parse_args()
    for f in args.files:
        dump_info(f, args)
