# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
from plot_times import times
import pylab as pl
import sys
import glob
from collections import defaultdict
import itertools

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

def get_tot(fn):
    print('processing ', fn)
    t = times(fn)
    # return sum(t)/float(len(t))
    return sum(t)


def get_nbkgd(fn):
    print(fn)
    x = fn.split('.')[0] # remove .log
    toks = x.split('_')
    x = [t for t in toks if t.startswith('b')][0]
    x = x[1:]  # remove 'b'
    print(x)
    return float(x)

def plot_by_fn_type(fn_list, marker, label):
    print('plotting', fn_list)

    tot_times = [get_tot(fn) for fn in fn_list]
    n_bkgd = [get_nbkgd(fn) for fn in fn_list]
    # pl.plot(n_bkgd, tot_times, marker = marker, label=label)
    pl.plot(n_bkgd, tot_times, marker, label=label)

fn_types = defaultdict(list)

for fn in fns:
    key = fn.split('_')
    key = [k for k in key if k.startswith('j')]
    key = '_'.join(key)
    fn_types[key].append(fn)

markers = itertools.cycle(('ro','bo'))
print()
print('fn_types:')
for k, v in fn_types.items():
    print(k)
    print('  ', v)

print()
for k, v in fn_types.items():
    print(k)
    print(' ' + str(v))
    plot_by_fn_type(v, next(markers), label=k)


pl.suptitle(outname[:-4])
pl.xlabel('n background')
pl.ylabel('tot exec time 1000 events (ns)')
pl.legend()
pl.savefig(outname)
pl.show()
