# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# @file AthenaCommon/python/CfgMergerLib
# @purpose a set of python tools to analyze configurables and find candidates
#          to be lumped together
# @author Sebastien Binet <binet@cern.ch>

"""
a set of python tools to analyze configurables and find candidates to be lumped
together.
"""
from __future__ import with_statement

import os, sys

from AthenaCommon.Logging import logging
msg = logging.getLogger ("CfgMerger")
msg.setLevel (logging.INFO)

_dups_suppress_list = (
    'AthSequencer', # FIXME: this shouldn't be suppressed
    'StoreGateSvc',
    )
def dup_candidates():
    from collections import defaultdict
    dup_dict = defaultdict(list)
    from AthenaCommon.Configurable import Configurable as _cc
    _cfgs = _cc.allConfigurables
    alive_cfgs = [c for c in _cfgs]

    all_cfgs = {}
    
    # get all configurables, group by C++ type
    for cfg_name in alive_cfgs:
        cfg = _cfgs[cfg_name]
        n = cfg.getJobOptName()
        all_cfgs[n] = {
            'props' : {},
            'cxx_type': cfg.getType(),
            'comp_type': 'N/A',
            }
        for k,v in cfg.properties().iteritems():
            if v is cfg.propertyNoValue:
                v = cfg.getDefaultProperty(k)
            value = None
            if isinstance(v, basestring):
                value = v
            else:
                vstr = str(v)
                if hasattr(v, 'toStringProperty'):
                    value = v.toStringProperty()
                else:
                    value = eval(vstr)
            all_cfgs[n]['props'][k] = value
            pass
        import AthenaCommon.Configurable as acc
        import AthenaCommon.AppMgr as acam
        comp_type = 'N/A'
        if isinstance(cfg, acc.ConfigurableAlgorithm):
            comp_type = 'alg'
        elif isinstance(cfg, acc.ConfigurableAlgTool):
            comp_type = 'tool'
        elif isinstance(cfg, (acc.ConfigurableService,
                              acam.AthServiceManager)):
            comp_type = 'svc'
        elif isinstance(cfg, acc.ConfigurableAuditor):
            comp_type = 'aud'
        else:
            print "** type of configurable [%s] is unknown ! (%s)" % (
                n, type(cfg)
                )
        all_cfgs[n]['comp_type'] = comp_type
        # fully qualified class name
        cls_name = cfg.getFullJobOptName()
        cls_name = cls_name[:cls_name.find('/')]
        # apply veto on some well-known configurables
        if cls_name in _dups_suppress_list:
            continue
        dup_dict[cls_name].append (cfg)
        
    # only keep the ones with more than one instance
    cfg_types = dup_dict.keys()[:]
    for cfg_cls in cfg_types:
        if len(dup_dict[cfg_cls])<=1:
            del dup_dict[cfg_cls]

    return dup_dict, all_cfgs

def combinations(iterable, r):
    # combinations('ABCD', 2) --> AB AC AD BC BD CD
    # combinations(range(4), 3) --> 012 013 023 123
    pool = tuple(iterable)
    n = len(pool)
    if r > n:
        return
    indices = range(r)
    yield tuple(pool[i] for i in indices)
    while 1:
        for i in reversed(range(r)):
            if indices[i] != i + n - r:
                break
        else:
            return
        indices[i] += 1
        for j in range(i+1, r):
            indices[j] = indices[j-1] + 1
        yield tuple(pool[i] for i in indices)

_types = set()
import GaudiKernel.GaudiHandles as _GKGH
_handled_types = (
    bool,str,int,float,list,dict,
    _GKGH.GaudiHandle,
    _GKGH.GaudiHandleArray,
    )
def compare_cfg (cfg1, cfg2):
    global _types
    global _handled_types
    diffs = []
    properties = cfg1.properties()
    def _prop_value (c, k):
        from AthenaCommon.Configurable import Configurable as C
        v = c.properties()[k]
        if v == C.propertyNoValue:
            v = c.getDefaultProperty(k)
        if isinstance (v, C):
            v = v.getFullJobOptName()
        return v
    msg.debug ("%s (cfg1:%s <=> cfg2:%s)",
               ">"*20, cfg1.getName(), cfg2.getName())
    
    for k in properties.iterkeys():
        v1 = _prop_value (cfg1, k)
        v2 = _prop_value (cfg2, k)
        map (_types.add, (type(v1), type(v2)))
        def _check_value (k,vv,cfg):
            if not isinstance(vv, _handled_types):
                msg.error ("-ERR-"*10)
                msg.error ("(%s:%s) (%s) !!!",
                           k, type(vv), cfg.__class__.mro())
                #msg.error (cfg)
                msg.error ("-ERR-"*10)
                
        _check_value (k,v1,cfg1)
        _check_value (k,v2,cfg2)

        ## FIXME: rely on someobject.__cmp__ instead of __repr__ ?
        v1=str(v1)
        v2=str(v2)
        if v1 != v2:
            msg.debug (" cfg1.%s: %s", k, v1)
            msg.debug (" cfg2.%s: %s", " "*len(k), v2)
            diffs.append (dict(key=k,
                               cfg_val1=str(v1),
                               cfg_val2=str(v2)))
    ndiffs = len(diffs)
    msg.debug ("%s (diffs=%d)", "<"*20, ndiffs)
    return diffs

def cfg_diff(cfg1, cfg2):
    return

def dump_cfgs(cfgs):
    for c in cfgs:
        print c

def dump_josvc_content(fname='josvc.ascii'):
    """dump the content of the joboptionsvc into `fname` and return that content
    
    the representation of the content will be a dict of dict:
     { 'component-name' : { 'property-name' : 'property-value', ... } }
    """
    f = open(fname, 'w')
    import collections as _c
    cfg = _c.defaultdict(dict)
    
    import AthenaPython.PyAthena as PyAthena
    josvc = PyAthena.py_svc('JobOptionsSvc', iface='IJobOptionsSvc')
    assert josvc is not None, "Could not retrieve the JobOptionsSvc"

    clients = list(josvc.getClients())
    for client in clients:
        props = josvc.getProperties(client)
        for prop in props:
            n = prop.name()
            v = prop.toString()
            cfg[client][n] = v

    cfg = dict(cfg)
    print >> f, "# content of the joboptions-svc"
    print >> f, "josvc_catalog = \\ "

    from pprint import pprint
    pprint(cfg, stream=f)
    print >> f, "# EOF"
    
    return cfg

def select_dups_by(dups, ndiffs=0):
    selected = dict()
    msg.info (":"*80)
    msg.info ("configurables with 'ndiffs==%d':",ndiffs)
    nselected = 0
    for k,values in dups.iteritems():
        dups_for_k = []
        for v in values:
            if len(v['diffs'])==ndiffs:
                dups_for_k.append((v['cfg1'], v['cfg2']))
        if len(dups_for_k):
            selected[k] = list()
            dup_names = [set(v for v in dups_for_k[0])]
            for v in dups_for_k[1:]:
                for n in dup_names:
                    if v[0] in n or v[1] in n:
                        n.add(v[0])
                        n.add(v[1])
                        break
                # the pair could not be found in what we already clustered
                # create a new entry, then
                else:
                    dup_names.append(set(v))
            for v in dup_names:
                selected[k].append(list(v))
                nselected += len(v)
    dump_selected (selected)
    msg.info ("configurables with 'ndiffs==%d': %d",ndiffs,nselected)
    return nselected,selected

def dump_selected(selected, ascii=False):
    if ascii:
        for k,dups in selected.iteritems():
            print " -- [component type: %s] -- "%k
            for v in dups:
                print "   +--"
                for i in v:
                    print "   | %s" % i
                print "   +--"
    else:
        for k,dups in selected.iteritems():
            msg.info (" -- [component type: %s] -- ", k)
            for v in dups:
                msg.info ("   +--")
                for i in v:
                    msg.info ("   | %s", i)
                msg.info ("   +--")
        
    return

def save_selected(dct, oname):
    import shelve, contextlib
    with contextlib.closing(shelve.open(oname)) as db:
        db['selected'] = dct['selected']
        db['all-cfgs'] = dct['all-cfgs']
    return

def analyze(oname="merge_candidates.pkl"):
    from collections import defaultdict
    histo=defaultdict(int)
    cands,all_cfgs = dup_candidates()
    dups = defaultdict(list)
    for k,v in cands.iteritems():
        combs = list(combinations(v, r=2))
        msg.debug ("=== %s #instances=[%d]\t#combinations=[%d]",
                   k,len(v),len(combs))
        for comb in combs:
            c1,c2=comb
            diffs = compare_cfg (*comb)
            ndiffs = len(diffs)
            histo[ndiffs]+=1
            if ndiffs==0:
                msg.info (" **MERGE** ==> [%s] and [%s] (#diffs: %d)",
                          c1.getName(), c2.getName(), ndiffs)
            dups[k].append (dict(cfg1=c1.getFullJobOptName(),
                                 cfg2=c2.getFullJobOptName(),
                                 diffs=diffs))

    nsels,selected=select_dups_by(dups=dups, ndiffs=0)
    msg.info ("stats of number of diffs b/w 'dups':")
    histo = dict(histo)
    for k,v in histo.iteritems():
        msg.info ('#diffs: %3s  => #cfgs: %3s', k, v)

    if os.path.splitext(oname)[1] == ".ascii":
        msg.info ("dumping report in ASCII [%s]", oname)
        with open(oname, "w") as ascii:
            sys.stdout = ascii
            dump_selected(selected, ascii=True)
            sys.stdout = sys.__stdout__
            pass
    else:
        msg.info ("saving report in shelve [%s]", oname)
        save_selected ({'selected':selected,
                        'all-cfgs':all_cfgs}, oname=oname)
    return dict(dups)
