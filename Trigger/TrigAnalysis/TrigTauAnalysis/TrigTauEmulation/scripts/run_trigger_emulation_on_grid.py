#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Simple script to run L1 emulation on the grid. 

TODO: needs detection of whether the dataset exists on the grid site you're trying to send your job to
"""

import argparse
import os
import subprocess
import sys

def which(program):
    import os
    def is_exe(fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    fpath, fname = os.path.split(program)
    if fpath:
        if is_exe(program):
            return program
    else:
        for path in os.environ["PATH"].split(os.pathsep):
            path = path.strip('"')
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file

    return None

if which("prun") is None:
    print "Cannot file 'prun' executable. Did you run localSetupPandaClient?"
    sys.exit()

parser = argparse.ArgumentParser()
parser.add_argument("--bin", type=str, default='emulate_level1.cxx')
parser.add_argument("--inDS", metavar="NAME", type=str, default="", help="name of dataset to use", required=True)
parser.add_argument("--outDS", metavar="NAME", type=str, default="", help="name of output dataset", required=True)
parser.add_argument("--nFiles", metavar="N", type=int, default=0,
                   help="number of files to run over (if 0, use all)")
parser.add_argument("--site", metavar="NAME", type=str, help="grid site to use", default="")
parser.add_argument("--filename", metavar="FILE", type=str, help="name of file in DS (default output.root)", default="output.root")

args = parser.parse_args()

cmd = 'prun --exec "\$ROOTCOREBIN/bin/x86_64-slc6-gcc48-opt/%s %%IN" --cmtConfig=x86_64-slc6-gcc48-opt --useRootCore --outDS=%s --inDS=%s --outputs %s' % (args.bin, args.outDS, args.inDS, args.filename)

if args.nFiles > 0:
    cmd += ' --nFiles=%d' % args.nFiles

if args.site != "":
    cmd += ' --site=%s' % args.site

print cmd
subprocess.call(cmd, shell=True)
