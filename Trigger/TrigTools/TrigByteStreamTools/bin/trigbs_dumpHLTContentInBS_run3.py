#!/usr/bin/env python
#
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
    parser.add_argument('--ctp', nargs="?", metavar="MODULE_ID", default=False, const=1,
                        help="CTP ROB details of ROB with MODULE_ID [default=%(const)s]")
    parser.add_argument('--l1',
                        action='store_true', default=False,
                        help='L1 trigger bits (from event header)')
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
                        help='dump info about EDM sizes per result; implies --hltres')
    parser.add_argument('--sizeSummary',
                        action='store_true', default=False,
                        help='dump summary info about EDM sizes at the end')
    parser.add_argument('--confKeys',
                        action='store_true', default=False,
                        help='dump TrigConfKeys stored in the events; implies --hltres')
    return parser


def decodeTriggerBits(words, num_sets, base=32):
    assert len(words) % num_sets == 0
    n_words_per_set = len(words) // num_sets
    result = []
    for iset in range(num_sets):
        words_in_set = words[iset*n_words_per_set:(iset+1)*n_words_per_set]
        bit_indices = []
        for iw in range(len(words_in_set)):
            bit_indices.extend([base*iw+i for i in range(base) if words_in_set[iw] & (1 << i)])
        result.append(bit_indices)
    return result


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


def ctp_rob(event, module_id=1):
    '''Return a string with information from the CTP ROB'''

    ctp_robs = [rob for rob in event \
                if rob.source_id().subdetector_id() == eformat.helper.SubDetector.TDAQ_CTP \
                and rob.source_id().module_id()==module_id]

    if len(ctp_robs)==0:
        log.warning('No CTP ROB found')
        return ''

    assert len(ctp_robs)==1
    rob = ctp_robs[0]

    from TrigByteStreamTools import CTPfragment
    x = CTPfragment.getExtraPayloadObject(rob)
    info_str = (f'ROB 0x{rob.source_id().code():x}, '
                f'L1ID {event.lvl1_id():10d}, '
                f'LB {event.lumi_block():4d}, '
                f'Version {CTPfragment.ctpFormatVersion(rob)}, '
                f'Bunch {CTPfragment.lvl1AcceptBunch(rob)}, '
                f'HLT counter: {CTPfragment.hltCounter(rob):3d}, '
                f'Payload #{CTPfragment.numberHltExtraPayloadWords(rob)} '
                f'{CTPfragment.hltExtraPayloadWords(rob)} '
                f'L1PSK {x.getL1PSK()} BGK {x.getBGK()}')
    folderUpdates = CTPfragment.getFolderUpdates(x)
    if len(folderUpdates)>0:
        info_str += ' COOLUPD '+''.join([f'[{f.second.folderIndex}, {f.second.lumiBlock}]' for f in folderUpdates])
    return info_str


def lvl1_bits(event):
    '''Return a string with information about LVL1 bits (IDs of items passed at TBP, TAP, TAV)'''

    info = event.lvl1_trigger_info()
    lvl1_bits = decodeTriggerBits(info, 3)  # TBP, TAP, TAV
    info_str = 'L1 CTP IDs - TBP: {:s}\n'.format(str(lvl1_bits[0]))
    info_str += 'L1 CTP IDs - TAP: {:s}\n'.format(str(lvl1_bits[1]))
    info_str += 'L1 CTP IDs - TAV: {:s}'.format(str(lvl1_bits[2]))
    return info_str


def hlt_bits(event, version=(1,1)):
    '''Return a string with information about passed chain IDs at HLT'''

    # Version 1.0 has {passed, prescaled, rerun}, 1.1 and later only {passed, prescaled}
    num_sets = 3 if version[0] < 1 or version==(1,0) else 2

    info = event.event_filter_info()
    hlt_bits = decodeTriggerBits(info, num_sets)
    info_str = 'HLT passed chain IDs: {:s}'.format(str(hlt_bits[0]))
    info_str += '\nHLT prescaled chain IDs: {:s}'.format(str(hlt_bits[1]))
    if num_sets==3:
        info_str += '\nHLT rerun chain IDs: {:s}'.format(str(hlt_bits[2]))
    return info_str


def stream_tags(event):
    def hex_list(nums):
        return '[' + ', '.join([hex(num) for num in nums]) + ']'
    info_str = 'Stream Tags:'
    for st in event.stream_tag():
        info_str += '\n-- {}_{}'.format(st.type, st.name)
        robs = list(st.robs)
        dets = list(st.dets)
        if len(robs) == 0 and len(dets) == 0:
            info_str += ' - Full Event Building'
        else:
            info_str += ' - Partial Event Building, robs={}, dets={}'.format(hex_list(robs), hex_list(dets))
    return info_str


def hlt_rod_minor_version(rob):
    version = rob.rod_version()
    minor_version = version.minor_version()
    minor_version_M = (minor_version & 0xff00) >> 8
    minor_version_L = minor_version & 0xff
    return (minor_version_M, minor_version_L)


def hlt_rod_minor_version_from_event(event):
    for rob in event.children():
        if rob.source_id().subdetector_id() == eformat.helper.SubDetector.TDAQ_HLT:
            return hlt_rod_minor_version(rob)


def hlt_result(event, print_sizes=False, conf_keys=False):
    num_hlt_robs = 0
    info_str = ""
    for rob in event.children():
        if rob.source_id().subdetector_id() != eformat.helper.SubDetector.TDAQ_HLT:
            continue
        num_hlt_robs += 1
        version = hlt_rod_minor_version(rob)
        info_str += '\n-- {:s} SourceID: {:s}, Version: {:s}, Size: {:d} bytes'.format(
            rob.__class__.__name__,
            rob.source_id().human(),
            f'{version[0]:d}.{version[1]:d}',
            rob.fragment_size_word()*4
        )
        if print_sizes or conf_keys:
            if version[0] < 1:
                raise RuntimeError('Cannot decode data from before Run 3, HLT ROD minor version needs to be >= 1.0')
            skip_payload = not conf_keys
            collections = hltResultMT.get_collections(rob, skip_payload=skip_payload)
            if conf_keys:
                conf_list = [c for c in collections if 'xAOD::TrigConfKeys_v' in c.name_persistent]
                conf_available = False
                for conf in conf_list:
                    conf_obj = conf.deserialise()
                    if not conf_obj:
                        continue
                    conf_available = True
                    info_str += '\n---- {:s}#{:s} SMK: {:d}, L1PSK: {:d}, HLTPSK: {:d}'.format(
                        conf.name_persistent, conf.name_key,
                        conf_obj.smk(), conf_obj.l1psk(), conf_obj.hltpsk())
                if not conf_available:
                    info_str += '\n---- TrigConfKeys unavailable in this ROB'
            if print_sizes:
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
            version = hlt_rod_minor_version(rob)
            if version[0] < 1:
                raise RuntimeError('Cannot decode data from before Run 3, HLT ROD minor version needs to be >= 1.0')
            module = rob.source_id().module_id()
            if module not in data.keys():
                data[module] = {'total': 0}
            data[module]['total'] += rob.fragment_size_word()*4
            if 'collections' not in data[module].keys():
                data[module]['collections'] = {}
            for coll in hltResultMT.get_collections(rob, skip_payload=True):
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
    version = None

    # Loop over events
    for event in input:
        event_count += 1
        if event_count <= offset:
            continue
        if event_count > offset+max_events:
            break
        events.append(event)

        if version is None:
            version = hlt_rod_minor_version_from_event(event)

        # Print header info
        print('{sep:s} Event: {:{width}d}, {:s} {sep:s}'.format(
              event_count, header_info(event),
              sep='='*20, width=len(str(max_events))))

        # Print CTP ROB
        if args.ctp is not False:
            print(ctp_rob(event, 1))

        # Print L1/L2/HLT bits
        if args.l1:
            print(lvl1_bits(event))
        if args.ef:
            print(hlt_bits(event, version))

        # Print Stream Tags
        if args.stag:
            print(stream_tags(event))

        # HLT Result
        if args.efres or args.sizes or args.confKeys:
            print(hlt_result(event, args.sizes, args.confKeys))

    # Size summary (after the loop over events)
    if args.sizeSummary:
        print(size_summary(events))


if '__main__' in __name__:
    args = get_parser().parse_args()
    for f in args.files:
        dump_info(f, args)
