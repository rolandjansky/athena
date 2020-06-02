#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
Calculate FCal Sampling Fractions
=================================

Calculate the FCal sampling fractions from Geant4 simulation.
"""

from __future__ import absolute_import, division, print_function


import argparse
import glob
import math
import sys

import ROOT as root

# To stop ROOT from hijacking '--help'
root.PyConfig.IgnoreCommandLineOptions = True


def _docstring(docstring):
    """Return summary of docstring
    """
    return docstring.split('\n')[4] if docstring else ""


def parse_args():
    """Parse command-line arguments
    """
    parser = argparse.ArgumentParser(description=_docstring(__doc__))
    parser.add_argument("--version", action="version", version="%(prog)s 0.1")
    parser.add_argument("-v", "--verbose", action="count", default=0,
                        help="print verbose messages; multiple -v result in more verbose messages")
    parser.add_argument("infile", default="LArFCalSamplingFraction.aan.root",
                        help="Path to input file; wildcards are supported (default: %(default)s)")
    parser.add_argument("-m", "--module", choices=["FCal1", "FCal2", "FCal3"],
                        help="Calculate the sampling fraction for this FCal module only; "
                             "if unspecified, it will try to parse the module from the file name")
    parser.add_argument("-o", "--outfile", default="out.txt",
                        help="Path to output file where sampling fraction results are written "
                             "(default: %(default)s)")

    args = parser.parse_args()

    return args


def calculate_samp_frac(args):
    """Read ntuple from the Geant4 simulation and calculate FCal sampling
    fractions.

    Returns (E_init, samp_frac, samp_frac_err)
    """
    tree_name = "tree_AS"
    print("Reading tree '{}' from file '{}'...".format(tree_name, args.infile))

    aan_chain = root.TChain(tree_name)
    aan_chain.Add(args.infile)

    n_event = aan_chain.GetEntries()

    # Get initial electron energy [GeV]
    for event in aan_chain:
        E_init = round(event.E, 2) / 1000
        break

    samp_frac = 0
    samp_frac_sq = 0

    if args.verbose:
        print("Event  Active E [MeV]  Total E [MeV]")

    # Loop over events and sum energy values
    for event in aan_chain:
        if args.module.lower() == "fcal1":
            totalE = event.Calib_FCal1Active + event.Calib_FCal1Inactive
            activeE = event.FCal1_E
        elif args.module.lower() == "fcal2":
            totalE = event.Calib_FCal2Active + event.Calib_FCal2Inactive
            activeE = event.FCal2_E
        elif args.module.lower() == "fcal3":
            totalE = event.Calib_FCal3Active + event.Calib_FCal3Inactive
            activeE = event.FCal3_E

        samp_frac += activeE / totalE
        samp_frac_sq += (activeE / totalE)**2

        if args.verbose:
            print("{:<6} {:<15g} {:<15g}".format(event.Event, activeE, totalE))

    if args.verbose:
        print("")

    # Mean sampling fractions
    samp_frac /= n_event
    samp_frac_sq /= n_event

    # Sampling fraction error = sqrt(<x^2> - <x>^2) / sqrt(N)
    samp_frac_err = math.sqrt(samp_frac_sq - samp_frac**2) / math.sqrt(n_event)

    print("{} sampling fraction (E = {:g} GeV): {:g} +/- {:g}".format(args.module, E_init, samp_frac, samp_frac_err))

    return E_init, samp_frac, samp_frac_err


def write_results(results, args):
    with open(args.outfile, "w") as outfile:
        outfile.write("[{}]\n".format(args.module))

        for key in sorted(results.keys()):
            outfile.write("{:g} GeV: {} +/- {}\n".format(key, results[key][0], results[key][1]))

def main():
    try:
        args = parse_args()

        if args.module is None:
            if "fcal1" in args.infile.lower():
                args.module = "fcal1"
            elif "fcal2" in args.infile.lower():
                args.module = "fcal2"
            elif "fcal3" in args.infile.lower():
                args.module = "fcal3"
            else:
                raise Exception("unknown FCal module")

        infiles = glob.glob(args.infile)
        infiles.sort()

        results = {}

        for infile in infiles:
            args.infile = infile
            E_init, samp_frac, samp_frac_err = calculate_samp_frac(args)

            results[E_init] = (samp_frac, samp_frac_err)

        print("Writing results to '{}'".format(args.outfile))
        write_results(results, args)

    except KeyboardInterrupt:
        return 1

    except Exception as err:
        print("error: {}".format(err))
        return 1


if __name__ == '__main__':
    sys.exit(main())
