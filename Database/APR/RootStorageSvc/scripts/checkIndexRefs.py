#!/usr/bin/env python3

# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

import ROOT
import sys
from AthenaCommon.Logging import logging

def main(infile, intree="CollectionTree"):
    """
        This script simply takes a ROOT file as input,
        reads the index_ref for all events in the file
        and makes sure each event has a unique value.
        Otherwise, it raises an error...
    """

    logging.info(f"== Using input file {infile} ==")

    f = ROOT.TFile(infile)
    t = f.Get(intree)
    t.SetBranchStatus("*", 0)
    t.SetBranchStatus("index_ref", 1)

    indices = {}
    nentries = t.GetEntries()

    # Read index_ref for all the events and build a dictionary that
    # holds index_ref => # of instances
    for idx in range(nentries):
        t.GetEntry(idx)
        if hasattr(t, 'index_ref'):
            iref = getattr(t, 'index_ref')
            if iref not in indices:
                indices[iref] = 1
            else:
                indices[iref] += 1
        else:
            logging.error("Cannot read index_ref!")
            return sys.exit(1)

    nindices = len(indices)

    # The number of unique indices should equal the number of events
    if nindices == nentries:
        logging.info(f"== Total number of keys is {nindices} vs"
                     f" total number of events is {nentries} ==")
        logging.info("All good!")
        return sys.exit(0)
    else:
        # Print some additional information
        for key in indices:
            pid = key >> 32
            offset = key - ( pid << 32 )
            logging.warning(f"  >> {key} (pid : {pid}, offset : {offset})"
                            f" is repeated {indices[key]} times...")
        logging.error(f"== Total number of keys is {nindices} vs"
                      f" total number of events is {nentries} ==")
        logging.error("The test FAILED!")
        return sys.exit(1)

if __name__ == '__main__':
    # Check user input
    if len(sys.argv) != 2:
        logging.info(f"Help: Use as checkIndexRefs.py DAOD.pool.root")
        sys.exit(1)
    # Run the test
    main(infile=sys.argv[1])
