#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file:    diff-jobo-cfg.py
# @purpose: check that 2 jobosvc.ascii files (produced with find_cfg_dups.py)
#           have same content (both in configurables and properties)
# @author:  Sebastien Binet <binet@cern.ch>
#           Adrien Renaud <renaud@lal.in2p3.fr>
# @date:    May 2010
#
# @example:
#
# diff-jobo-cfg ref.josvc.ascii chk.josvc.ascii
#

from __future__ import print_function

__author__  = "Sebastien Binet, Adrien Renaud"

import sys
import os

from optparse import OptionParser

def dump_seq(seq):
    for i in seq:
        print (i)
    pass

def cxx_sort(dpp):
    from collections import defaultdict
    cxx_dpp = defaultdict(list)          

    for k,v in dpp.iteritems():
        #print (v['cxx_type'])
        cxx_dpp[v['cxx_type']].append({k:v})

    for k,v in cxx_dpp.iteritems():
        print ('---',k)
        for vv in v: 
            print ('------',vv.keys())
            print ('---------',vv.values())#['comp_type']
    return cxx_dpp

def load_cfg_file(fname):
    """return the dictionary of components and their properties
    """
    comps_db = {}
    try:
        import shelve
        comps_db = shelve.open(fname, 'r')
        return comps_db['all-cfgs']
    except Exception:
        from past.builtins import execfile
        execfile(fname, comps_db)
        return comps_db['d']

def dict_diff(ref, chk):
    """ Return a dict of keys that differ with another config object.  If a value is
        not found in one fo the configs, it will be represented by KEYNOTFOUND.
        @param ref:   First dictionary to diff.
        @param chk:   Second dicationary to diff.
        @return diff:   Dict of Key => (ref.val, chk.val)
    """
    diff = {}
    # Check all keys in ref dict
    for k in ref.iterkeys():
        if not (k in chk):
            diff[k] = (ref[k], '<KEYNOTFOUND>')
        elif (ref[k] != chk[k]):
            diff[k] = (ref[k], chk[k])
    # Check all keys in chk dict to find missing
    for k in chk.iterkeys():
        if not (k in ref):
            diff[k] = ('<KEYNOTFOUND>', chk[k])
    return diff

def cmp_component_db(ref, chk, verbose=True):
    """ compare 2 dicts of components
    dicts are of the form:
     { 'comp_type' : <name of component>,
       'cxx_type'  : <C++ type of the component>,
       'props' : { 'property-name' : 'property-value', }
       }
    """
    common_keys = []
    ref_keys = set(ref.keys())
    chk_keys = set(chk.keys())

    common_keys = ref_keys & chk_keys
    ref_only_keys = ref_keys - chk_keys
    chk_only_keys = chk_keys - ref_keys

    print ("::: components in both files: [%5s]" % (len(common_keys),))
    print ("::: components in ref only:   [%5s]" % (len(ref_only_keys),))
    if len(ref_only_keys)>0:
        dump_seq(ref_only_keys)
        print ("="*80)
    print ("::: components in chk only:   [%5s]" % (len(chk_only_keys),))
    if len(chk_only_keys)>0:
        dump_seq(chk_only_keys)
        print ("="*80)

    diff = []
    for comp_name in common_keys:
        comp_ref = ref[comp_name]
        comp_chk = chk[comp_name]

        ref_props = sorted([(k,v) for k,v in comp_ref['props'].iteritems()])
        chk_props = sorted([(k,v) for k,v in comp_chk['props'].iteritems()])
        if ref_props != chk_props:
            diff.append((comp_name, ref_props, chk_props,
                         dict_diff(ref=comp_ref['props'],
                                   chk=comp_chk['props'])))
        pass

    print ("::: components with different properties: [%5s]" % (len(diff),))
    for name, ref_props, chk_props, diff_props in diff:
        print (":::  - component: [%s]" % (name,))
        for prop_name, prop_value in diff_props.iteritems():
            ref_value = prop_value[0]
            chk_value = prop_value[1]
            if isinstance(ref_value, list):
                ref_value = sorted(ref_value)
            if isinstance(chk_value, list):
                chk_value = sorted(chk_value)

            if isinstance(ref_value, list) and isinstance(chk_value, list):
                dref_value = set(ref_value) - set(chk_value)
                dchk_value = set(chk_value) - set(ref_value)
                ref_value = sorted(list(dref_value))
                chk_value = sorted(list(dchk_value))
            print ("-%s: %r" %(prop_name, ref_value,))
            print ("+%s: %r" %(prop_name, chk_value,))
    

    if (len(ref_only_keys) > 0 or
        len(chk_only_keys) > 0 or
        len(diff) > 0):
        return 1
    return 0

if __name__ == "__main__":

    parser = OptionParser(
        usage="usage: %prog [options] [-r] ref.josvc.ascii [-f] chk.josvc.ascii"
        )
    _add = parser.add_option
    
    _add( "-r",
          "--ref",
          dest = "ref_fname",
          help = "The path to the first josvc.ascii file to analyze" )
    
    _add( "-f",
          "--file",
          dest = "chk_fname",
          help = "The path to the second josvc.ascii file to analyze" )

#    _add("-o", "--output",
#         dest = "o_fname",
#         default = "cfg.diff",
#         help = "file where to store the output of `which diff` run on the input files given to %PROG")
    
    _add( "-v",
          "--verbose",
          action  = "store_true",
          dest = "verbose",
          default = False,
          help = "Switch to activate verbose printout" )


    (options, args) = parser.parse_args()

    if len(args) > 0 and args[0][0] != "-":
        options.ref_fname = args[0]
        pass
    if len(args) > 1 and args[1][0] != "-":
        options.chk_fname = args[1]
        pass

    if (options.chk_fname is None or options.ref_fname is None) :
        str(parser.print_help() or "")
        sys.exit(1)
        pass

    chk_fname = os.path.expandvars(os.path.expanduser(options.chk_fname))
    ref_fname = os.path.expandvars(os.path.expanduser(options.ref_fname))

    print (":"*80)
    print ("::: comparing configurations")
    print (":::  ref: %s" % ref_fname)
    ref_db = load_cfg_file(ref_fname)
    print (":::    -> [%d] components" % (len(ref_db.keys()),))
    print (":::  chk: %s" % chk_fname)
    chk_db = load_cfg_file(chk_fname)
    print (":::    -> [%d] components" % (len(chk_db.keys()),))

    sc = cmp_component_db(ref_db, chk_db, options.verbose)
    
    if sc==0:
        print ("::: all good")
    else:
        print ("::: configurations differ !")
    print ("::: bye.")
    print (":"*80)
    sys.exit(sc)
