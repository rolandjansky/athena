#!/usr/bin/env python

# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

'''
Merges events from multiple raw files from one or more directories.
This is mainly for use on small samples
'''

import eformat
import libpyevent_storage as EventStorage
import sys
import os
import argparse
import logging


def get_parser():
    parser = argparse.ArgumentParser(usage='%(prog)s [options] outBaseName dir1 [dir2 ...]',
                                     description=__doc__,
                                     formatter_class=lambda prog: argparse.HelpFormatter(prog, max_help_position=36, width=100))
    parser.add_argument('outBaseName',
                        help='Output file name base')
    parser.add_argument('dirs',
                        metavar='dir',
                        nargs='+',
                        help='Input directory (can be multiple)')
    parser.add_argument('--copyFrom', '-c',
                        metavar='FILE', action='store',
                        help='Copy metadata from other ByteStream file')
    parser.add_argument('--nepfOutput', '-n',
                        metavar='NUM', action='store', type=int, default=500,
                        help='Number of events per output file (default=%(default)s)')
    parser.add_argument('--nepfInput', '-N',
                        metavar='NUM', action='store', type=int, default=0,
                        help='Number of events per input file, 0 means all (default=%(default)s)')
    parser.add_argument('--renumber', '-r',
                        action='store_true',
                        help='Renumber event IDs from 1')
    parser.add_argument('--verbose', '-v',
                        action='store_true',
                        help='Increase output verbosity')
    return parser


def get_filenames(dirs):
    filenames = []
    for dirname in sorted(dirs):
        files = os.listdir(dirname)
        logging.debug('Found %d files in %s', len(files), dirname)
        for f in sorted(files):
            filenames.append(dirname + '/' + f)
    return filenames


def main():
    args = get_parser().parse_args()
    logging.basicConfig(stream=sys.stdout,
                        format='%(levelname)-8s %(message)s',
                        level=logging.DEBUG if args.verbose else logging.INFO)

    filenames = get_filenames(args.dirs)
    logging.info('Merging %d files', len(filenames))
    logging.info('filenames: %s', filenames)

    ievt = 1
    dr = EventStorage.pickDataReader(filenames[0])
    output = eformat.ostream(core_name=args.outBaseName,
                             run_number=dr.runNumber(),
                             trigger_type=dr.triggerType(),
                             detector_mask=dr.detectorMask(),
                             beam_type=dr.beamType(),
                             beam_energy=dr.beamEnergy())
    output.max_events_per_file(args.nepfOutput)

    for ifile, input_file in enumerate(filenames):
        logging.info("Opening file %d: %s", ifile+1, input_file)
        inputFile = eformat.istream([input_file])

        for ievt_in_file, event in enumerate(inputFile):
            if args.nepfInput > 0 and ievt_in_file >= args.nepfInput:
                break
            logging.debug("Reading event with Lvl1 Id = %ld", event.lvl1_id())
            new_event = eformat.write.FullEventFragment(event)
            if args.renumber:
                logging.debug("Rewriting the event with Lvl1 Id = %ld", ievt)
                new_event.lvl1_id(ievt)
                new_event.global_id(ievt)
                for rob in new_event.children():
                    rob.rod_lvl1_id(ievt)
                    rob.checksum_type(0)  # remove checksum
                    rob.rod_lvl1_trigger_type(event.lvl1_trigger_type())
            else:
                logging.debug("Rewriting the event with unchanged Lvl1 Id")

            output.write(new_event)
            ievt += 1

    logging.info('Wrote %d events to %d files', ievt-1, 1+((ievt-2)//args.nepfOutput))


if "__main__" in __name__:
    sys.exit(main())
