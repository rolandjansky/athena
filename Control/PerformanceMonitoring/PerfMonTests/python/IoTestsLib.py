# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## @file PerfMonTests.IoTestsLib
## @date April 2009

from __future__ import print_function

__author__ = "Sebastien Binet <binet@cern.ch>"
__version__ = "$Revision: 1.1 $"
__doc__ = """
a set of simple minded functions to test ROOT I/O (from python)
"""

from array import array as carray
from builtins import range

import random
# set some dummy seed, for reproducibility
random.seed(20080910) # first LHC startup :)

from os import sysconf
_pagesz = sysconf('SC_PAGE_SIZE') // 1024 # in kb

_py_dtype_to_root = {
    'i' : 'I',
    'f' : 'F',
    }
"""translates the usual python 'dtype' codes to the ROOT/CINT ones
"""

from PyUtils.Decorators import forking

def pymon():
    """returns (cpu[ms], vmem[kb], rss[kb])
    """
    from resource import getrusage, RUSAGE_SELF
    from string import split as ssplit
    cpu = getrusage(RUSAGE_SELF)
    mem = open('/proc/self/statm','r')
    cpu = (cpu.ru_utime+cpu.ru_stime) * 1e3 # in milliseconds
    mem = ssplit(mem.readlines()[0])
    vmem = int(mem[0])*_pagesz
    rss  = int(mem[1])*_pagesz
    return cpu,vmem,rss

def comp_delta(d, verbose=False):
    assert 'start' in d
    assert 'stop'  in d
    assert len(d['start']) == 3
    assert len(d['stop'])  == 3
    if verbose:
        print (repr(d))
    delta = { 'cpu' : d['stop'][0] - d['start'][0],
              'vmem': d['stop'][1] - d['start'][1],
              'rss' : d['stop'][2] - d['start'][2],
              'nbytes': -1
              }
    if 'nbytes' in d:
        delta['nbytes'] = d['nbytes']
    print ("==> cpu: %(cpu)8.3f ms  vmem: %(vmem)i kB  rss: %(rss)i kB  nbytes: %(nbytes)i kB" % delta)
    return delta

def import_ROOT():
    import sys
    # for ROOT...
    if '-b' not in sys.argv:
        sys.argv.insert(1, '-b')
    import ROOT
    return ROOT

ROOT = import_ROOT()

@forking
def io_test1_write(fname, nevts=1000, sz=1000, dtype='i'):
    """testing writing 1000 evts with arrays of 1000- integers
    """
    f = ROOT.TFile.Open(fname, 'RECREATE')
    t = ROOT.TTree('t', 't')

    nevts= nevts
    imax = sz
    data = carray(dtype, imax*[ 0 ] )
    #t.Branch( 'mynum', n, 'mynum/I' )
    t.Branch( 'i', data, 'data[%d]/%s'%(imax, _py_dtype_to_root[dtype]) )

    from random import randint
    
    fill = t.Fill
    for i in range(nevts):
        for j in range(sz):
            data[j] = randint(0, sz)
        fill()

    f.Write()
    f.Close()
    return

@forking
def io_test1_read(fname, verbose=False):
    f = ROOT.TFile.Open(fname, 'READ')
    t = f.Get('t')
    assert t, "could not find tree 't'"
    nevts = t.GetEntries()
    if verbose:
        print ("::: reading [%s] (%i events) [sz=%s kB]" % (fname, nevts,
                                                            f.GetSize()//1024))
    tot_bytes = 0
    get_entry = t.GetEntry
    start = pymon()
    for ievt in range(nevts):
        # copy next entry into memory and verify
        nb = get_entry(ievt)
        if nb <= 0:
            continue
        tot_bytes += nb
        # use the values directly from the tree
        data = getattr(t, 'data')
        sz = len(data)
        assert sz > 0
        #print ("::: ievt [%3i] : #data = %s" % (ievt, sz))
    stop = pymon()

    del t
    f.Close()
    
    return {'start' : start,
            'stop'  : stop,
            'nbytes': tot_bytes//1024}

@forking
def io_test2_write(fname, nevts=1000, sz=1000, dtype='i'):
    """testing writing 1000 evts with arrays of (variable length) 1000- ints
    """
    f = ROOT.TFile.Open(fname, 'RECREATE')
    t = ROOT.TTree('t', 't')

    nevts= nevts
    imax = sz

    n    = carray( 'i', [ 0 ] )
    data = carray( dtype, imax*[ 0 ] )
    t.Branch( 'sz',   n,    'sz/I' )
    t.Branch( 'data', data, 'data[sz]/%s'%_py_dtype_to_root[dtype])

    from random import randint
    
    fill = t.Fill
    for i in range(nevts):
        jmax = randint(1, sz)
        n[0] = jmax
        for j in range(jmax):
            data[j] = randint(0, sz)
        fill()

    f.Write()
    f.Close()
    return

@forking
def io_test2_read(fname, verbose=False):
    f = ROOT.TFile.Open(fname, 'READ')
    t = f.Get('t')
    assert t, "could not find tree 't'"
    nevts = t.GetEntries()
    if verbose:
        print ("::: reading [%s] (%i events) [sz=%s kB]" % (fname, nevts,
                                                            f.GetSize()//1024))
    tot_bytes = 0
    get_entry = t.GetEntry
    start = pymon()
    for ievt in range(nevts):
        # copy next entry into memory and verify
        nb = get_entry(ievt)
        if nb <= 0:
            continue
        tot_bytes += nb
        # use the values directly from the tree
        data = getattr(t, 'data')
        sz = len(data)
        assert sz > 0
        #print ("::: ievt [%3i] : #data = %s" % (ievt, sz))
    stop = pymon()

    del t
    f.Close()
    
    return {'start' : start,
            'stop'  : stop,
            'nbytes': tot_bytes//1024}


### tests ---------------------------------------------------------------------
if __name__ == "__main__":
    # FIXME: use 'nose' instead... for automatical test discovery
    print ("::: running all tests...")

    nreads   = 10 # nbr of times to repeat each 'read' test
    mon_data = {}
    
    # -----
    # io_test1
    # -----
    
    # io_test1 - ints
    fname = '/tmp/out_test1_ints.root'
    w = io_test1_write(fname=fname,
                       nevts=100000, sz=1000,
                       dtype='i')
    mon_data['io_test1-ints'] = []
    for _ in range(nreads):
        mon_data['io_test1-ints'].append(comp_delta(io_test1_read(fname=fname)))

    # io_test1 - floats
    fname = '/tmp/out_test1_flts.root'
    w = io_test1_write(fname=fname,
                       nevts=100000, sz=1000,
                       dtype='f')
    mon_data['io_test1-flts'] = []
    for _ in range(nreads):
        mon_data['io_test1-flts'].append(comp_delta(io_test1_read(fname=fname)))

    # -----
    # io_test2
    # -----
    
    # io_test2 - ints
    fname = '/tmp/out_test2_ints.root'
    w = io_test2_write(fname=fname,
                       nevts=100000, sz=1000,
                       dtype='i')
    mon_data['io_test2-ints'] = []
    for _ in range(nreads):
        mon_data['io_test2-ints'].append(comp_delta(io_test2_read(fname=fname)))

    # io_test2 - floats
    fname = '/tmp/out_test2_floats.root'
    w = io_test2_write(fname=fname,
                       nevts=100000, sz=1000,
                       dtype='f')
    mon_data['io_test2-flts'] = []
    for _ in range(nreads):
        mon_data['io_test2-flts'].append(comp_delta(io_test2_read(fname=fname)))


    print (mon_data)
