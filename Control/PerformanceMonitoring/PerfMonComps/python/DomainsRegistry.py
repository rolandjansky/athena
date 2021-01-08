# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file PerfMonComps/python/DomainsRegistry.py
# @purpose hold a registry of alg names and their association w/ domain
# @date September 2009
# @author Sebastien Binet <binet@cern.ch>
from __future__ import print_function
import six

__version__ = "$Revision$"
__author__  = "Sebastien Binet <binet@cern.ch>"
__doc__ = """hold a registry of alg names and their association w/ domain"""

## KNOWN limitations:
#
#   - will break if somebody adds a new algorithm somewhere in the top-sequence
#     in a place which is not the end (ie: topseq.insert(0, myalg))
#     the main assumption here is that everybody does: topseq += myalg
#

__all__ = [
    'flag_domain',
    'domain_registry',
    'domain_algs',
    'has_domain',
    'domain_names',
    'a2d_db',        # the dict of alg->domain
    'd2a_db',        # the dict of domain->[algs]
    ]

## imports --------------------------------------------------------------------
try:
    from AthenaCommon.AlgSequence import iter_algseq
except ImportError:
    def iter_algseq(seq):
        def _iter_algseq(seq):
            yield seq
            for c in seq.getChildren():
                for sub in _iter_algseq(c):
                    yield sub
        return _iter_algseq(seq)


## classes --------------------------------------------------------------------
class Registry(object):
    """
    """
    def __init__(self):
        self.verbose = False
        self._registry = []
        """list of pairs (domain-name, alg-name-of-precedent-domain)
        """

        self._usr_fixups = []
        """list of pairs (domain-name, alg-name) which are 'after the facts'
        fix-ups provided by the user (to fine tune the registry of domain-alg
        associations)
        """

        self._a2d_db = {}
        """dict of alg-name -> domain-name this is the final db of
        alg->domain associations.
        """

        self._d2a_db = {}
        """dict of domain-name -> alg-name this is the final db of
        alg<-domain associations.
        """

        self._dirty_db = True
        """flag to detect when to regenerate the a2d_db dict
        """

        self.top_stack = '__top_stack__'

    def flag_domain(self, name):
        """function to declare that the upcoming algorithms which will be added
        to the job sequence will be part of the domain `name`
        ex:
        - alg1
        - alg2 <-+- flag_domain('core')
        - alg3   | 
        - alg4   |
        - alg5 <-+- flag_domain('id')
        """
        self._dirty_db = True
    
        if self.verbose:
            print(":::flag_domain(%s)..." % (name,))
        
        ## if has_domain(name):
        ##     raise RuntimeError(
        ##         'declaring nested domains or non-contiguous domains is not '+
        ##         'supported (yet?)'
        ##         )
    
        # get the current alg name
        from AthenaCommon.AlgSequence import AlgSequence
        job = AlgSequence()

        alg_names = [c.getName() for c in job.getChildren()]
        n_algs = len(alg_names)

        if len(self._registry) > 0:
            last_entry = self._registry[-1]
            if n_algs == last_entry[2]:
                # no new alg. remove last entry.
                self._registry = self._registry[:-1]
                pass
        
        if alg_names:
            self._registry.append((name, alg_names[-1], n_algs))
        else:
            self._registry.append((name, self.top_stack, n_algs))

        if self.verbose:
            print(":::flag_domain(%s)... => registry: %r (#algs: %s)" % (
                name, self._registry, len(alg_names)))

        return

    def domain_registry(self):
        """ returns the association between a domain and the before-first
        algorithm of that domain
        """
        import copy as cp
        return cp.deepcopy(self._registry)

    def has_domain(self, name, registry=None):
        """ returns whether we know about a certain domain `name`
        """
        if registry is None:
            registry = self._registry
        # FIXME: linear search
        for i in registry:
            domain = i[0]
            if domain == name:
                return True
        return False

    def domain_names(self, registry=None):
        """ returns the list of domain names currently registered
        """
        if registry is None:
            registry = self._registry
        names = []
        for domain in registry:
            domain_name = domain[0]
            if domain_name in names:
                continue
            if len(self.domain_algs(domain_name)):
                names.append(domain_name)
        return names

    def domain_start(self, name, registry=None):
        """ returns the (idx,name) of the algorithm flagging the start of the
        domain `name`
        """
        if registry is None:
            registry=self._registry
        for ielmt, elmt in enumerate(registry):
            if elmt[0] == name:
                return ielmt, elmt[1]
        return None, None

    def _domain_algs(self, name, registry, alg_names=None):
        """ returns the list of algorithms which are part of a given
        domain `name`
        """
        from AthenaCommon.AlgSequence import AlgSequence
        job = AlgSequence()

        if alg_names is None:
            alg_names = [c.getName() for c in job.getChildren()]

        domain_idx,start_alg = self.domain_start(name,registry=registry)

        try:
            if start_alg is self.top_stack:
                start_idx = 0
            ## elif start_alg is None:

            else:
                start_idx = alg_names.index(start_alg) + 1
        except ValueError:
            raise ValueError('no algorithm [%s] in list of algorithms' %
                             (start_alg,))

        # get the start of the domain *after* the current one (to get the end
        # of the domain slice)
        next_domain_idx = domain_idx + 1
        if next_domain_idx >= len(registry):
            # there is no next domain
            stop_idx = None
            stop_alg = None
        else:
            next_domain_name = registry[next_domain_idx]
            _,stop_alg = self.domain_start(next_domain_name[0],
                                           registry=registry[next_domain_idx:])
            #print("----->",stop_alg,domain_idx,next_domain_idx)
            if stop_alg is None: # bottom of the stack: take rest of the slice
                stop_idx = None
            else:
                try:
                    stop_idx = alg_names.index(stop_alg)+1
                except ValueError:
                    raise ValueError(
                        'no algorithm [%s] in list of algorithms' % stop_alg
                        )
                pass
            pass

        # we know the whole slice information
        return start_idx,stop_idx,registry[domain_idx+1:]

    def domain_algs(self, name):
        """ returns the list of algorithms which are part of a given
        domain `name`
        """
        if not self.has_domain(name):
            raise RuntimeError("no such domain [%s]" % name)

        #print(":"*40)
        #print("::: domain_algs(%s)..." % (name,))

        from AthenaCommon.AlgSequence import AlgSequence
        job = AlgSequence()

        alg_names = [c.getName() for c in job.getChildren()]

        # the list of slices to pick-up from the alg_names
        slices = []

        # sliding windows over the alg-names, collecting slices as we go
        start_idx = 0
        stop_idx = None
        w_start = 0
        w_stop  = None
        w_alg_names = alg_names[:]
        w_registry = self._registry[:]
        itr = 0
        while len(w_alg_names)>0 and w_start is not None:
            #print("== %s: itr [%i]==\nw_names: %s \nreg: %s" % (
            #    name, itr, w_alg_names, w_registry))

            w_slice = slice(w_start, w_stop)
            w_alg_names = alg_names[w_slice][:]
            start_idx,stop_idx,w_registry = self._domain_algs(
                name,
                registry=w_registry,
                alg_names=w_alg_names)
            #print("slice: [%s:%s]" % (start_idx,stop_idx))
            slices.extend(w_alg_names[slice(start_idx,stop_idx)])
            w_start = stop_idx
            itr +=1
            if not self.has_domain(name, registry=w_registry):
                # this means we exhausted all possible 'sparse blocks'
                break

        # we know the whole slice information
        # return a copy of the slice
        return slices[:]

    def set_domain(self, alg_name, domain_name):
        """declare that component ``alg_name`` is in fact part
        of domain ``domain_name``
        """
        self._dirty_db = True
        self._usr_fixups.append((alg_name, domain_name))

    def a2d_db(self):
        """return a dict of alg-name -> domain-name"""
        if not self._dirty_db:
            return dict(self._a2d_db)

        a2d = {}
        
        from AthenaCommon.AlgSequence import AlgSequence
        # FIXME: oblivious to AthFilterSeq and others...
        job = AlgSequence()

        # first pass: fill-in the domains for the 'root' algs
        # (ie: the ones which are not nested)
        domain_names = self.domain_names()
        for domain in domain_names:
            alg_names = self.domain_algs(domain)
            for alg_name in alg_names:
                a2d[alg_name] = domain
        
        algs = list(iter_algseq(job))
        if algs[0] is job:
            algs = algs[1:]

        # then, process the nested algorithms...
        for i,alg in enumerate(algs):
            alg_name = alg.getName()
            if alg_name in a2d:
                # already done
                continue
            domain_name = None
            # assume the domain of the nested alg is the domain from the
            # previous algorithm...
            if i >= 1:
                domain_name = a2d[algs[i-1].getName()]
            a2d[alg_name] = domain_name
            # print("-->",alg_name,a2d[alg_name])

        # now, apply user fix-ups...
        for domain,alg_name in self._usr_fixups:
            a2d[alg_name] = domain

        self._a2d_db = a2d
        self._dirty_db = False

        # build reverse dict too
        from collections import defaultdict
        d2a = defaultdict(list)
        for alg_name, domain_name in six.iteritems (a2d):
            d2a[domain_name].append(alg_name)
        self._d2a_db = dict(d2a)
        
        return dict(self._a2d_db)

    def d2a_db(self):
        """return the dict domain->alg(s)"""
        if not self._dirty_db:
            return dict(self._d2a_db)
        # side-effect of calling self.algs: will build  self._d2a_db
        a2d = self.algs # noqa: F841
        return dict(self._d2a_db)

    @property
    def algs(self):
        return self.a2d_db()

    @property
    def domains(self):
        return self.d2a_db()

    pass # class Registry

## globals --------------------------------------------------------------------
reg = Registry()
del Registry

## functions ------------------------------------------------------------------
flag_domain = reg.flag_domain
domain_registry = reg.domain_registry    
has_domain = reg.has_domain
domain_names = reg.domain_names
domain_start = reg.domain_start
domain_algs  = reg.domain_algs
set_domain = reg.set_domain
a2d_db = reg.a2d_db
d2a_db = reg.d2a_db

### tests ---------------------------------------------------------------------
def _test_main():
    import AthenaCommon.CfgMgr as CfgMgr
    from AthenaCommon.AlgSequence import AlgSequence
    job = AlgSequence()

    import PerfMonComps.DomainsRegistry as pdr
    pdr.flag_domain('core')
    job += CfgMgr.AthSequencer('alg1')
    job += CfgMgr.AthSequencer('alg2')
    job += CfgMgr.AthSequencer('alg3')
    pdr.flag_domain('id')
    job += CfgMgr.AthSequencer('alg4')
    job += CfgMgr.AthSequencer('alg5')

    pdr.flag_domain('empty')

    pdr.flag_domain('muons')
    job += CfgMgr.AthSequencer('alg6')
    job += CfgMgr.AthSequencer('alg7')

    pdr.flag_domain('empty')
    pdr.flag_domain('id')
    job += CfgMgr.AthSequencer('alg4_0')
    job += CfgMgr.AthSequencer('alg5_0')

    pdr.flag_domain('empty')
    pdr.flag_domain('jets')
    job += CfgMgr.AthSequencer('alg8')
    job += CfgMgr.AthSequencer('alg9')

    pdr.flag_domain('empty1')
    pdr.flag_domain('empty2')

    pdr.flag_domain('sub-algs-1')
    job += CfgMgr.AthSequencer('subseq_1')
    job.subseq_1 += CfgMgr.AthSequencer('subseq1_alg1')
    
    pdr.flag_domain('empty3')
    pdr.flag_domain('sub-algs-1')
    job.subseq_1 += CfgMgr.AthSequencer('subseq1_alg2')

    pdr.flag_domain('sub-algs-2')
    job += CfgMgr.AthSequencer('subseq_2')

    pdr.flag_domain('empty3')
    pdr.flag_domain('sub-algs-2')
    job.subseq_2 += CfgMgr.AthSequencer('subseq2_alg1')
    pdr.flag_domain('sub-algs-2')
    job.subseq_2 += CfgMgr.AthSequencer('subseq2_alg2')

    pdr.flag_domain('sub-algs-1')
    job.subseq_1 += CfgMgr.AthSequencer('subseq1_alg3')

    # dummy:
    pdr.set_domain(domain_name='sub-algs-2',
                   alg_name='subseq2_alg2')
    
    ref = {
        'core' : ['alg1','alg2','alg3'],
        'id':    ['alg4','alg5','alg4_0','alg5_0'],
        'muons': ['alg6','alg7'],
        'empty': [],
        'empty1': [],
        'empty2': [],
        'jets': ['alg8','alg9'],
        'sub-algs-1': ['subseq_1'],
        'sub-algs-2': ['subseq_2'],
        }

    print("=== algs:",[c.getName() for c in job.getChildren()])
    print("=== domains:",pdr.domain_names())
    assert sorted(pdr.domain_names()) == \
           sorted(['core', 'id', 'muons',
                   #'empty', 'empty1', 'empty2',
                   'jets',
                   'sub-algs-1',
                   'sub-algs-2',
                   ])
    for d in pdr.domain_names():
        print("::: domain [%s]..." % d)
        #print(" ->",pdr.domain_start(d))
        algs = pdr.domain_algs(d)
        print("    algs:",algs)
        print("    ref: ",ref[d])
        assert algs == ref[d]

    print("OK")
    return 0

if __name__ == "__main__":
    import sys
    sys.exit(_test_main())
