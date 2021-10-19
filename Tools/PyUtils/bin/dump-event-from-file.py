#!/usr/bin/env python3

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

""" Dump a pythonized version of the event content using RootFileDumper.
    The main logic is similar to what's being done in diff-root.
    Therefore, this can be used in conjunction w/ diff-root for further
    debugging. """
def dumpEvent(fname, idx):
    # import ROOT via RootUtils
    import PyUtils.RootUtils as ru
    ru.import_root()
    
    # file dumper: file, tree
    data = ru.RootFileDumper(fname, "CollectionTree")
    
    # iterator: tree, idx
    values = data.dump("CollectionTree", [int(idx)])
    
    # loop over the iterator and print the values
    try:
        while True:
            val = next(values)
            print(val)
    except StopIteration:
        pass

if __name__ == "__main__":
    import sys
    if len(sys.argv)!=3:
        print("dump-event-from-file.py: A script to dump event information from a file")
        print("Usage:")
        print(f"{sys.argv[0]} <file> <TTree index of the event>")
    else:
        dumpEvent(sys.argv[1], sys.argv[2])
