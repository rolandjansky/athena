#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

__author__ = "Sebastien Binet <binet@cern.ch>"
__doc__    = "get the inclusive and exclusive vmem sizes of a library"

## std imports
import argparse
import ctypes
import os

## 3rd-party imports
from PyUtils.Decorators import forking as forking
from PerfMonComps.PyMonUtils import loaded_libs, pymon
import six

_veto_libs = [
    'resource.so', # from python std-lib 'resource'...
    ]

def lib_loader(libname):
    return ctypes.cdll.LoadLibrary(libname)
    
@forking
def load_lib (libname):
    _,vmem0,_ = pymon()
    lib_loader (libname)
    _,vmem1,_  = pymon()
    libs = [l for l in loaded_libs()
            if not os.path.basename(l) in _veto_libs and
            os.access(l, os.R_OK)]
    return (vmem0, vmem1, libs)

@forking
def dep_libs(libname):
    """retrieve the list of dependencies of a given library
    both direct and indirect dependencies
    """
    bkg_libs = [os.path.basename(l) for l in loaded_libs()]
    bkg_libs = [l for l in bkg_libs if l not in _veto_libs]

    _,_,libs = load_lib(libname)
    libs = [os.path.basename(l) for l in libs]
    return [l for l in libs if l not in bkg_libs and l!=libname]
    
@forking
def analyze (libname):
    bkg_libs = loaded_libs()
    #print ("::: bkg_libs:",map(os.path.basename, bkg_libs))
    vmem0,vmem1,libs = load_lib (libname)
    dVmemOffset = vmem1-vmem0
    #print ("vmem0=%s vmem1=%s libs=%s" % (vmem0, vmem1, len(libs)))
    linked_libs = [os.path.basename(lib)
                   for lib in libs
                   if (not (lib in bkg_libs) and
                       os.path.basename(lib) != os.path.basename(libname)
                       and os.access(lib, os.R_OK))]
    #print ("::: linked_libs:",linked_libs)

    # load all linked-in libs
    def load_linked_in_libs(linked_libs):
        all_good = True
        for l in linked_libs:
            try:
                #print (" - loading [%s]..." % l)
                lib_loader (l)
            except Exception as err:
                print ("** problem loading [%s]\n%s" % (l,err))
                all_good = False
                pass
        return all_good
    #print ("--> loading all linked-in libs...")
    NRETRY = 10
    for _ in range(NRETRY):
        ok = load_linked_in_libs(linked_libs)
        if ok:
            break
    else:
        print ("** err: could not reliably load all libs (after %s retries)"%NRETRY)
        
    #print ("--> loading all linked-in libs... [done]")

    vmem0,vmem1,libs = load_lib (libname)
    #print ("vmem0=%s vmem1=%s libs=%s" % (vmem0, vmem1, len(libs)))
    dVmemLib = vmem1-vmem0
    return (dVmemLib, dVmemOffset, len(linked_libs), len(bkg_libs))

@forking
def analyze_libraries (libnames, detailed=False):
    display = []
    _print = display.append
    lib_stats = {}
    for libname in libnames:
        _print(":"*80)
        bname = os.path.basename(libname)
        _print(":: inspecting library: %s" % bname)
        stats = analyze (libname)
        _print("::  nbr linked: %s" % stats[2])
        _print("::  dVmem-self: %8.3f Mb" % stats[0])
        _print("::  dVmem-all : %8.3f Mb" % stats[1])
        #print ("::  bkg-libs: %s" % stats[3])
        lib_stats[bname] = dict(
            vmem_self=  stats[0],
            vmem_all=   stats[1],
            nbr_linked= stats[2],
            )
        if detailed:
            import operator as _op
            deps = dep_libs(libname)
            dep_stats = dict((d, analyze(d)[:2]) for d in deps)
            dep_vmem = dep_stats.items()
            dep_vmem.sort(key=_op.itemgetter(1), reverse=True)
            _print("::  deps: lib-self | lib-all:")
            for k,v in dep_vmem:
                _print("::   --> [%8.3f |%8.3f Mb] (%s)" % (v[0], v[1], k))
            
    return display,lib_stats

def save_stats (lib_stats, fname=None):
    if fname is None:
        fname = "vmem-stats-sz.csv"
    print (":: saving vmem statistics in [%s]..."%fname)
    import csv, os
    if os.path.exists (fname):
        os.remove (fname)
    o = csv.writer (open(fname, "w"), delimiter=';')
    o.writerow (['nbr libraries', len(lib_stats)])
    o.writerow (['lib name', 'dvmem-self (Mb)', 'dvmem-all (Mb)', 'nbr linked-libs'])
    for k,v in six.iteritems(lib_stats):
        o.writerow ([os.path.basename(k), v['vmem_self'], v['vmem_all'], v['nbr_linked']])

    print (":: saving vmem statistics in [%s]... [done]"%fname)
    
def main():

    parser = argparse.ArgumentParser(
        description='get the inclusive and exclusive vmem sizes of a library'
        )
    _add = parser.add_argument
    _add('libnames',
         type=str,
         nargs='+',
         help='list of library names to be inspected')
    _add('--detailed',
         action='store_true',
         default=False,
         help='enable detailed output')
    args = parser.parse_args()

    libnames = args.libnames
    doDetailed = args.detailed
    
    print (":: inspecting libraries: %s" % libnames)
    display,lib_stats = analyze_libraries (libnames, doDetailed)

    for l in display:
        print (l)
    save_stats (lib_stats)
    
    return lib_stats

if __name__ == '__main__':
    lib_stats = main()
    
