#!/usr/bin/env python3

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import argparse
import json
import numpy
import sys

# Helper for range parsing
def parse_range(string):
    result = string.split(',')
    if len(result) != 2 or not result[0].isnumeric() or not result[1].isnumeric() or int(result[0]) >= int(result[1]):
        print(f"Invalid input range {string} (must be N,M w/ N/M as numbers and M>N)...")
        sys.exit(1)
    else:
        return result

# Main function
if '__main__' in __name__:

    # Parse the user input
    parser = argparse.ArgumentParser(description = 'Script to re-fit PerfMonMTSvc measurements')

    parser.add_argument('-i', '--input', type = str, required = True,
                        help = 'PerfMonMTSvc JSON file')
    parser.add_argument('-r', '--range', type = parse_range, dest = 'range',
                        help = 'A comma comma-separated list that determines the '
                        'minimum and maximum event numbers over which one wants '
                        'to perform the fit (e.g. 10,100)')
    parser.add_argument('-f', '--slice', type = float, dest = 'slice',
                        help = 'A (python) slice defining the event-range over '
                        'to which one wants to perform the fit from the end '
                        '(default: 0.8, i.e. last 80 percent of the events)')
    parser.add_argument('-o', '--observable', dest = 'obs', nargs = '?', default = 'pss',
                        choices = ['vmem', 'rss', 'pss', 'swap', 'cpuTime', 'wallTime'],
                        help = 'Variable to fit')
    parser.add_argument('-m', '--minimal', dest = 'minimal', action = 'store_true',
                        help = 'Prints only the result')
    parser.add_argument('-d', '--debug', dest = 'debug', action = 'store_true',
                        help = 'Enables debug printouts')

    args = parser.parse_args()

    # Check the argument settings
    # User needs to pick either a range or a slice
    if args.range and args.slice:
        print('Please specify either range or slice fitting - not both, quitting...')
        sys.exit(1)
    # If neither a slice nor a range provided, set a slice
    if not args.range and not args.slice:
        args.slice = 0.8
    # The slice range should be >0. and <1.
    if args.slice and (float(args.slice) > 1. or float(args.slice) < 0.):
        print('Slice cannot be outside [0, 1], resetting to 1...')
        args.slice = 1.

    # Load the data, perform the fit, and print the requested information
    with(open(args.input)) as json_file:

        data = json.load(json_file)

        # Check if data exist
        if 'eventLevel' not in data or args.obs not in data['eventLevel']['1']:
            print('Data do not contain the necessary information, please check'
                  ' the input file or the provided observable...')
            sys.exit(1)

        # Fill the arrays
        x, y = [], []

        for entry in sorted(data['eventLevel'], key=float):
            x.append(entry)
            y.append(data['eventLevel'][entry][args.obs])

        # Find the units
        units = 'ms/event' if 'time' in args.obs.lower() else 'kb/event'

        # Find the indices over which the fit will be performed
        if args.range:
            if (args.range[0] not in x or args.range[1] not in x):
                print(f"Provided range parameters {args.range} are not in data, will use slicing w/ 0.8...")
                idx_min = round(0.2*len(x))
                idx_max = len(x)-1
            else:
                idx_min = x.index(args.range[0])
                idx_max = x.index(args.range[1])
        else:
            idx_min = round((1-args.slice)*len(x))
            idx_max = len(x)-1

        # Print some debug information if asked for
        if args.debug:
            print(f"DEBUG:: Inputs were {args.range} and {args.slice}")
            print(f"DEBUG:: Original data has {len(x)} points:")
            print(f"DEBUG:: >> x : {x}")
            print(f"DEBUG:: >> y : {y}")
            print(f"DEBUG:: Computed minimum and maximum indices are {idx_min} and {idx_max}, respectively")
            print(f"DEBUG:: Sliced data has {len(x[idx_min:idx_max+1])} points:")
            print(f"DEBUG:: >> x : {x[idx_min:idx_max+1]}")
            print(f"DEBUG:: >> y : {y[idx_min:idx_max+1]}")

        # Perform the fit
        n_tot = len(x[idx_min:idx_max+1]) # Total number of points used in the fit
        coeffs = numpy.lib.polyfit( numpy.array(x[idx_min:idx_max+1], numpy.float),
                                    numpy.array(y[idx_min:idx_max+1], numpy.float) , deg = 1 )

        # Print the final result
        if not args.minimal:
          print('='*70)
          print(f"  [ Begin -   End ] : Slope of {args.obs} in {units} using {n_tot} points")
          print('='*70)
          print(f"  [ {x[idx_min]:>5} - {x[idx_max]:>5} ] : {coeffs[0]:^.2f}")
          print('='*70)
        else:
          print(f"{x[idx_min]},{x[idx_max]},{coeffs[0]:^.2f}")
