# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
from plot_times import times
import pylab as pl
import sys
import glob

if len(sys.argv) < 3:
    print('Please supply an input file name stub to glob on the command line, and an output file name')
    sys.exit(0)


stub = sys.argv[1]
outname = sys.argv[2]
if not outname.endswith('.pdf'): outname += '.pdf'

fns = glob.glob(stub)
print('glob found %d files: %s for stub %s' %  (len(fns), str(fns), stub))
if not fns:
    print('no files found for stub', stub)
    sys.exit()

def get_av(fn):
    t = times(fn)
    return sum(t)/float(len(t))

av_times = [get_av(fn) for fn in fns]

def get_nbkgd(fn):
    print(fn)
    x = fn.split('.')[0] # remove .log
    toks = x.split('_')
    x = [t for t in toks if t.startswith('b')][0]
    x = x[1:]  # remove 'b'
    print(x)
    return float(x)

n_bkgd = [get_nbkgd(fn) for fn in fns]    

# print x
pl.plot(n_bkgd, av_times, 'o')
pl.suptitle(outname[:-4])
pl.xlabel('n background')
pl.ylabel('av exec time (ns)')
pl.savefig(outname)
pl.show()
