#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from collections import OrderedDict
from timeit import default_timer as timer
import ROOT
import argparse
import logging
import operator
import sys

# Order entries in the CollectionTree based on the event number
# The lightest weight way to do this is to read the EventInfo
def order_tree(input_file, output_file, reverse_order = False):

    # Open input file 
    logging.debug('Opening the input file...')
    logging.debug('')
    fin  = ROOT.TFile.Open('{}'.format(input_file),'READ')
    tin  = fin.Get('CollectionTree')

    # Build the {index:event_number} dictionary for the input file
    dict_in = {} 
    nevts = tin.GetEntriesFast()
    for idx in range(0, nevts):
        if idx % 100 == 0: logging.info('Read {} events from the input so far'.format(idx))
        tin.GetEntry(idx)
        if hasattr(tin,'xAOD::EventAuxInfo_v1_EventInfoAux.'):
            event_info = getattr(tin,'xAOD::EventAuxInfo_v1_EventInfoAux.')
            event_number = event_info.eventNumber
        elif hasattr(tin,'EventInfoAux.'):
            event_info = getattr(tin,'EventInfoAux.')
            event_number = event_info.eventNumber
        elif hasattr(tin,'EventInfo_p4_McEventInfo'):
            event_info = getattr(tin,'EventInfo_p4_McEventInfo')
            event_number = event_info.m_event_ID.m_event_number
        elif hasattr(tin,'EventInfo_p4_ByteStreamEventInfo'):
            event_info = getattr(tin,'EventInfo_p4_ByteStreamEventInfo')
            event_number = event_info.m_event_ID.m_event_number
        elif hasattr(tin,'ByteStreamEventInfo'):
            event_info = getattr(tin,'ByteStreamEventInfo')
            event_number = event_info.m_event_ID.m_event_number
        else:
            logging.error('Cannot read event info, will bail out.')
            break
        logging.debug('Idx : EvtNum {:10d} : {}'.format(idx,event_number))
        dict_in[idx] = event_number 

    # Sort the dictionary by event numbers
    dict_out = OrderedDict(sorted(dict_in.items(), key=operator.itemgetter(1), reverse = reverse_order))

    # Open output file
    logging.info('')
    logging.debug('Opening the output file...')
    logging.debug('')
    fout = ROOT.TFile.Open('{}'.format(output_file),'RECREATE')
    tout = tin.CloneTree(0)

    # Write out the ordered file 
    for idx in dict_out:
        if idx % 100 == 0: logging.info('Wrote {} events to the output so far'.format(idx))
        logging.debug('Idx : EvtNum {:10d} : {}'.format(idx,dict_out[idx]))
        tin.GetEntry(idx)
        tout.Fill()

    # Write and close
    tout.Write()
    fout.Close()
    fin.Close()

    return nevts

# Maing function
if __name__ == '__main__':

    # Parser
    parser = argparse.ArgumentParser(description = 'Configurable CollectionTree orderer (either ascending or descending in event number)',
                                     add_help=True)

    parser.add_argument('-i', '--input', action = 'store', dest = 'input_file', 
                        help = 'name of the input ROOT file')
    parser.add_argument('-o', '--output', action = 'store', dest = 'output_file', 
                        help = 'name of the output ROOT file')
    parser.add_argument('-r', '--reverse', action = 'store_true', dest = 'reverse_order', 
                        help = 'order the tree in descending order (default = ascending)')
    parser.add_argument('-l', '--log-level', action = 'store', dest = 'logging', default = 'INFO', 
                        choices = ['INFO' ,'WARNING', 'ERROR', 'DEBUG'], 
                        help = 'logging level')
    parser.add_argument('-v', '--version', action = 'version', version = '%(prog)s 0.1')

    args = parser.parse_args()

    # Logging
    logging.basicConfig(filename = './tree-orderer.log', level = args.logging,
                        format='%(filename)s \t %(asctime)s \t %(levelname)-8s %(message)s',
                        datefmt='%m-%d %H:%M', filemode='w')
    console = logging.StreamHandler()
    console.setLevel(args.logging)
    formatter = logging.Formatter('%(filename)s \t %(levelname)-8s %(message)s')
    console.setFormatter(formatter)
    logging.getLogger('').addHandler(console)

    # Check if an input file name is specified
    if not args.input_file:
        logging.error('')
        logging.error('Please provide an input file! See below for usage:')
        logging.error('')
        parser.print_help()
        sys.exit(-1)

    # Configure the output file name if needed
    if not args.output_file: args.output_file = args.input_file.replace('.root','.ordered.root')
    if args.input_file == args.output_file: args.output_file += '.ordered'

    # Print the configurables
    logging.info('')
    logging.info('Running tree-orderer.py')
    logging.info('Input file    is {}'.format(args.input_file))
    logging.info('Output file   is {}'.format(args.output_file))
    logging.info('Order format  is {}'.format('descending' if args.reverse_order else 'ascending'))
    logging.info('Logging level is {}'.format(args.logging))
    logging.info('')

    # Run the ordering code
    start = timer() 
    nevts = order_tree(args.input_file, args.output_file, args.reverse_order)
    end   = timer()

    # Exit
    logging.info('')
    logging.info('DONE! Processed a total of {} events in {:.2f} seconds'.format(nevts, end-start))
    logging.info('')
