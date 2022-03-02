#!/usr/bin/env python
#
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

'''
Dump content of the HLT navigation collections found in HLT ByteStream data
'''

import argparse
import eformat
from TrigByteStreamTools import hltResultMT

from AthenaCommon.Logging import logging
log = logging.getLogger('dumpHLTNav')


def get_parser():
    parser = argparse.ArgumentParser(usage='%(prog)s [options] FILE [FILE]',
                                     description=__doc__)
    parser.add_argument('files',
                        metavar='FILE [FILE]', nargs='+',
                        help='RAW files to inspect')
    parser.add_argument('-n', '--events',
                        metavar='N', action='store', type=int,
                        help='Process N events')
    parser.add_argument('-s', '--skip',
                        metavar='N', action='store', type=int,
                        help='Skip N events')
    parser.add_argument('-m', '--module',
                        metavar='N', action='store', type=int, default=0,
                        help='HLT ROB module ID to print; negative number means all modules, default is %(default)s')
    return parser


def dump_nav(collections):

    xaod_map = {}

    # First step: loop over all collections to match interface + aux + decorations
    for col in collections:
        if 'HLTNav_' not in col.name():
            continue

        if col.is_xAOD_interface_container():
            xaod_map[col.name_key] = {
                'interface': col,
                'aux_cont': None,
                'decorations': []
            }
        elif col.is_xAOD_aux_container():
            strip_name = col.name_key[0:-4]  # Remove 'Aux.'
            if strip_name not in xaod_map:
                log.warning('%s not in xaod_map', strip_name)
                continue
            xaod_map[strip_name]['aux_cont'] = col
        elif col.is_xAOD_decoration():
            if not col.parent:
                log.warning('{:s} has no parent', col.name())
            strip_name = col.parent.name_key[0:-4]  # Remove 'Aux.'
            if strip_name not in xaod_map:
                log.warning('%s not in xaod_map', strip_name)
                continue
            xaod_map[strip_name]['decorations'].append(col)

    # Second step: loop over the matched collections and decode and print
    for key, col_dict in xaod_map.items():
        if not col_dict['interface']:
            log.warning('%s interface collection missing', key)
            continue
        if not col_dict['aux_cont']:
            log.warning('%s aux_cont collection missing', key)
            continue
        cont_if = col_dict['interface'].deserialise()
        cont_aux = col_dict['aux_cont'].deserialise()
        cont_deco = [c.deserialise() for c in col_dict['decorations']]

        if not cont_if:
            # Don't know why this happens, silence the warning for now
            # log.warning('%s interface deserialisation failed', key)
            continue
        if not cont_aux:
            log.warning('%s aux_cont deserialisation failed', key)
            continue

        cont_if.setStore(cont_aux)

        print('  - %s' % key, flush=True)
        for i in range(cont_if.size()):
            obj = cont_if.at(i)
            print('    - Element #%d' % i)
            print('      - name: %s' % obj.name())
            print('      - decisions: %s' % obj.decisions())
            if cont_deco:
                print('      - decorations:')
                for i_deco, deco_vec in enumerate(cont_deco):
                    try:
                        print('        - %s = %s' % (col_dict['decorations'][i_deco].name_key, deco_vec.at(i)))
                    except Exception as ex:
                        log.warning(ex)
                        log.warning('i: %d', i)
                        log.warning('len(deco_vec): %d', len(deco_vec))
                        log.warning('i_deco: %d', i_deco)
                        log.warning('len(col_dict["decorations"]): %d', len(col_dict['decorations']))


def dump_info(bsfile, args):
    log.info('Opening %s', bsfile)
    input = eformat.istream(bsfile)
    offset = args.skip if args.skip else 0
    max_events = min(args.events, len(input)) if args.events else len(input)
    event_count = 0

    # Loop over events
    for event in input:
        event_count += 1
        if event_count <= offset:
            continue
        if event_count > offset+max_events:
            break

        # Print event header info
        print('{sep:s} Event: {:d}, Run: {:d}, LB: {:d}, Global_ID: {:d} {sep:s}'.format(
              event_count,
              event.run_no(),
              event.lumi_block(),
              event.global_id(),
              sep='='*20))

        for rob in event.children():
            if rob.source_id().subdetector_id() != eformat.helper.SubDetector.TDAQ_HLT:
                continue
            if args.module>=0 and rob.source_id().module_id() != args.module:
                continue

            # Print ROB header info
            print('- ROB SourceID: {:s}'.format(rob.source_id().human()))

            # Parse the raw data
            collections = hltResultMT.get_collections(rob)
            dump_nav(collections)


def load_streamerinfos():
    import ROOT
    import os
    for p in os.environ['DATAPATH'].split (':'):
        fname = os.path.join (p, 'bs-streamerinfos.root')
        if os.path.exists (fname):
            ROOT.TFile.Open (fname)
            break
    else:
        log.warning('Cannot find bs-streamerinfos.root file in DATAPATH')
    return


if '__main__' in __name__:
    args = get_parser().parse_args()
    load_streamerinfos()
    for f in args.files:
        dump_info(f, args)
