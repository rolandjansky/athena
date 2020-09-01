#!/usr/bin/env python
#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''
Write a copy of a ByteStream file with updated metadata
'''

import sys
import argparse
import logging
import eformat
import libpyevent_storage as EventStorage
import six


# This mapping was found in a comment in PyUtils.MetaReader
beam_type_dict = {
    'none': 0,
    'protons': 1,
    'ions': 2
}


def get_parser():
    parser = argparse.ArgumentParser(usage='%(prog)s [options] FILE',
                                     description=__doc__,
                                     formatter_class=lambda prog: argparse.HelpFormatter(prog, max_help_position=36, width=100))
    parser.add_argument('file',
                        metavar='FILE',
                        help='input ByteStream file')
    parser.add_argument('--outputName', '-o',
                        metavar='NAME', action='store',
                        help='Output file name base. If not specified, created automatically from metadata')
    parser.add_argument('--copyFrom', '-c',
                        metavar='FILE', action='store',
                        help='Copy metadata from other ByteStream file')
    parser.add_argument('--numEvents', '-n',
                        metavar='NUM', action='store', type=int,
                        help='Copy only first NUM events from input to output')
    parser.add_argument('--runNumber', '-r',
                        metavar='NUM', action='store', type=int,
                        help='Change run number')
    parser.add_argument('--stream', '-s',
                        metavar='NAME', action='store',
                        help='Change stream name')
    parser.add_argument('--projectTag', '-p',
                        metavar='TAG', action='store',
                        help='Change project tag')
    parser.add_argument('--beamType', '-b',
                        metavar='TYPE', action='store',
                        choices=beam_type_dict.keys(),
                        help='Change beam type, possible values are: %(choices)s')
    parser.add_argument('--beamEnergy', '-e',
                        metavar='ENERGY', action='store', type=int,
                        help='Change beam energy')
    parser.add_argument('--lumiBlock', '-l',
                        metavar='LB', action='store', type=int,
                        help='Change lumi block number')
    parser.add_argument('--triggerType', '-t',
                        metavar='TT', action='store', type=int,
                        help='Change trigger type')
    parser.add_argument('--detectorMask', '-m',
                        metavar='MASK', action='store', type=int,
                        help='Change detector mask')
    parser.add_argument('--verbose', '-v',
                        action='store_true',
                        help='Increase output verbosity')
    return parser


def main():
    args = get_parser().parse_args()
    logging.basicConfig(stream=sys.stdout,
                        format='%(levelname)-8s %(message)s',
                        level=logging.DEBUG if args.verbose else logging.INFO)

    if args.copyFrom:
        logging.info('Reading events from %s and metadata from %s', args.file, args.copyFrom)
    else:
        logging.info('Reading events and metadata from %s', args.file)
    meta_input = args.copyFrom if args.copyFrom else args.file
    reader = EventStorage.pickDataReader(meta_input)
    input_stream = eformat.istream(args.file)

    # Read metadata from input file
    metadata_basic = {}  # arguments for eformat.ostream
    metadata_extra = {}  # metadata passed as dictionary
    metadata_basic['runNumber'] = reader.runNumber()
    metadata_basic['triggerType'] = reader.triggerType()
    metadata_basic['detectorMask'] = reader.detectorMask()
    metadata_basic['beamType'] = reader.beamType()
    metadata_basic['beamEnergy'] = reader.beamEnergy()
    metadata_extra['Stream'] = reader.stream()
    metadata_extra['Project'] = reader.projectTag()
    metadata_extra['LumiBlock'] = reader.lumiblockNumber()

    logging.debug('Input metadata_basic = %s', metadata_basic)
    logging.debug('Input metadata_extra = %s', metadata_extra)

    # Change metadata
    if args.runNumber:
        metadata_basic['runNumber'] = args.runNumber
    if args.triggerType:
        metadata_basic['triggerType'] = args.triggerType
    if args.detectorMask:
        metadata_basic['detectorMask'] = args.detectorMask
    if args.beamType:
        metadata_basic['beamType'] = beam_type_dict[args.beamType]
    if args.beamEnergy:
        metadata_basic['beamEnergy'] = args.beamEnergy
    if args.stream:
        metadata_extra['Stream'] = args.stream
    if args.projectTag:
        metadata_extra['Project'] = args.projectTag
    if args.lumiBlock:
        metadata_extra['LumiBlock'] = args.lumiBlock

    logging.debug('Updated metadata_basic = %s', metadata_basic)
    logging.debug('Updated metadata_extra = %s', metadata_extra)

    # Create new file name
    file_name_base = args.outputName
    if not file_name_base:
        # Get the name elements
        ptag = metadata_extra['Project']
        runno = metadata_basic['runNumber']
        stream = metadata_extra['Stream']
        lbn = metadata_extra['LumiBlock']
        # Build the name
        file_name_list = []
        file_name_list.append(ptag if ptag else 'data')
        file_name_list.append('{:08d}'.format(runno if runno else 0))
        file_name_list.append(stream if stream else 'unknown_stream')
        file_name_list.append('lb{:04d}'.format(lbn if lbn else 0))
        file_name_base = '.'.join(file_name_list)

    # Write the new file
    metadata_extra_strings = ['{:s}={:s}'.format(k, str(v)) for k, v in six.iteritems(metadata_extra)]
    output_stream = eformat.ostream(
        core_name         = file_name_base,
        run_number        = metadata_basic['runNumber'],
        trigger_type      = metadata_basic['triggerType'],
        detector_mask     = metadata_basic['detectorMask'],
        beam_type         = metadata_basic['beamType'],
        beam_energy       = metadata_basic['beamEnergy'],
        meta_data_strings = metadata_extra_strings)

    logging.info('Writing file %s', output_stream.current_filename().replace('.writing', '.data'))

    ievt = 0
    nmax = args.numEvents or -1
    for event in input_stream:
        ievt+=1
        if nmax >= 0 and ievt > nmax:
            break
        logging.debug('Writing event %d', ievt)
        output_stream.write(event)


if '__main__' in __name__:
    sys.exit(main())
