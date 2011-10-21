# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: PyAthenaTests/python/Lib.py
# @purpose: a set of Py-components to test pyathena
# @author: Sebastien Binet <binet@cern.ch>

__version__ = "$Revision: 1.10 $"

import AthenaCommon.SystemOfUnits as Units
from AthenaPython import PyAthena
StatusCode = PyAthena.StatusCode

from AthenaPython.Bindings import _clid_typename_aliases
_clid_typename_aliases['DataVector<AthExParticle>' ] = 'AthExParticles'
_clid_typename_aliases['DataVector<AthExIParticle>'] = 'AthExIParticles'

def _gen_dv_typedef(elem_type):
    if not hasattr(PyAthena, '%ss'%elem_type):
        dv = getattr(PyAthena, 'DataVector< %s >'%elem_type)
        setattr(PyAthena, '%ss'%elem_type, dv)
    return getattr(PyAthena, '%ss'%elem_type)

class PyRecord( PyAthena.Alg ):
    """A simple python algorithm to record data into storegate
    """
    def __init__(self, name = "PyRecord", **kw):
        ## init base class
        kw['name'] = name
        super(PyRecord,self).__init__(**kw)

        self.particles   = kw.get('particles',   "Particles")
        self.decay       = kw.get('decay',       "TwoBodyDecay")
        self.elephantino = kw.get('elephantino', "PinkElephantino")

        self.sg = None
        return
    
    def initialize(self):
        _info = self.msg.info
        
        _info( "initializing %s", self.name() )
        ## storegate
        self.sg = PyAthena.StoreGate.pointer("StoreGateSvc")
        if not self.sg:
            self.msg.error("could not retrieve storegatesvc !")
            return StatusCode.Failure

        ## hack to load clids...
        from AthenaCommon.AppMgr import theApp
        theApp.Dlls += ['AthExThinningAlgs',
                        'AthExThinningEvent',
                        'AthExThinningPoolCnv']

        ## Datavector stuff
##         assert _gen_dv_typedef('AthExParticle') is not None
##         assert _gen_dv_typedef('AthExIParticle') is not None

##         import ctypes
##         ctypes.cdll.LoadLibrary ('libAthExThinningAlgs.so')
        return StatusCode.Success

    def execute(self):
        _info = self.msg.info
        _err  = self.msg.error
        _record = self.sg.record
        _info( "executing %s...", self.name() )
        allGood = True
        particles = PyAthena.AthExParticles()
        ## fill-in particles:
        for i in xrange(10):
            particles.push_back( PyAthena.AthExParticle(i,i,i,i) )
            
        if _record(particles, self.particles) != StatusCode.Success:
            _err('could not record particles at [%s]', self.particles)
            allGood = False
            pass

        iparticles = PyAthena.AthExIParticles()
#        self.sg['I%s'%self.particles] = iparticles
        if _record(iparticles, 'I%s'%self.particles) != StatusCode.Success:
            _err('could not record iparticles at [I%s]', self.particles)
            allGood = False
            pass
        
        decay = PyAthena.AthExDecay()
        if _record(decay, self.decay) != StatusCode.Success:
            _err('could not record decay at [%s]', self.decay)
            allGood = False
            pass
        
        ele = PyAthena.AthExElephantino()
        if _record(ele, self.elephantino) != StatusCode.Success:
            _err('could not record elephantino at [%s]', self.elephantino)
            allGood = False
            pass

        _info('all good: %s', allGood)
        if allGood:
            return StatusCode.Success
        return StatusCode.Failure

    def finalize(self):
        self.msg.info( "finalizing %s...", self.name() )
        return StatusCode.Success

    pass # PyRecord


class PyRetrieve( PyAthena.Alg ):
    """A simple python algorithm to retrieve data from storegate
    """
    def __init__(self, name = "PyRetrieve", **kw):
        ## init base class
        kw['name'] = name
        super(PyRetrieve,self).__init__(**kw)

        self.particles   = kw.get('particles',   "Particles")
        self.decay       = kw.get('decay',       "TwoBodyDecay")
        self.elephantino = kw.get('elephantino', "PinkElephantino")

        ## handle to event store
        self.sg = None
        return
    
    def initialize(self):
        self.msg.info( "initializing %s", self.name() )
        ## storegate
        self.sg = PyAthena.StoreGate.pointer("StoreGateSvc")
        if not self.sg:
            self.msg.error('could not retrieve event store')
            return StatusCode.Failure
        
        ## hack to load clids...
        from AthenaCommon.AppMgr import theApp
        theApp.Dlls += ['AthExThinningAlgs',
                        'AthExThinningEvent',
                        'AthExThinningPoolCnv']
        ## Datavector stuff
##         assert _gen_dv_typedef('AthExParticle') is not None
##         assert _gen_dv_typedef('AthExIParticle') is not None

        return StatusCode.Success

    def execute(self):
        _retrieve = self.sg.retrieve
        _err  = self.msg.error
        _info = self.msg.info
        _info( "executing %s...", self.name() )
        
        allGood = True
        particles = _retrieve("AthExParticles", self.particles)
        if particles is None:
            _err("Could not fetch particles at [%s] !!", self.particles)
            allGood = False
            pass
        assert len(particles) == 10
        for idx,p in enumerate(particles):
            assert p.px() == float(idx)
            assert p.py() == float(idx)
            assert p.pz() == float(idx)
            assert p.e()  == float(idx)
            
        iparticles = _retrieve("AthExIParticles", 'I%s'%self.particles)
        if iparticles is None:
            _err("Could not fetch iparticles at [I%s] !!", self.particles)
            allGood = False
            pass
        assert len(iparticles) == 0
        
        decay = _retrieve("AthExDecay", self.decay)
        if decay is None:
            _err("Could not fetch Decay at [%s] !!", self.decay)
            allGood = False
            pass
        
        elephantino = _retrieve("AthExElephantino", self.elephantino)
        if elephantino is None:
            _err("Could not fetch Elephantino at [%s] !!", self.elephantino)
            allGood = False
            pass

        _info('all good: %s', allGood)
        if allGood: return StatusCode.Success
        return StatusCode.Failure

    def finalize(self):
        self.msg.info( "finalizing %s...", self.name() )
        return StatusCode.Success

    pass # PyRetrieve

### ---------------------------------------------------------------------------
import os
class TrigDecTestAlg (PyAthena.Alg):
    'Dummy algorithm testing accessing to HLT informations'
    def __init__(self, name='TrigDecTestAlg', **kw):
        ## init base class
        kw['name'] = name
        super(TrigDecTestAlg, self).__init__(**kw)

        ## properties and data members
        self.filename = kw.get('filename', 'trig_passed_evts.ascii')
        self.trigDec = None
        return

    def initialize(self):
        self.msg.info('==> initialize...')
        self.trigDec = PyAthena.py_tool('Trig::TrigDecisionTool')
        if not self.trigDec:
            self.msg.error('could not retrieve TrigDecisionTool !')
            return StatusCode.Failure

        ## fix-up HLT::Chain
        def _str_hlt_chain(c):
            """helper function to make a HLT::Chain printable"""
            return "Counter = %r success (raw) = %r pass-through = %r "\
                   "prescaled = %r lastActiveStep = %r \tname = %r" % \
                   (c.getChainCounter(),
                    bool(c.chainPassedRaw()),
                    bool(c.isPassedThrough()),
                    bool(c.isPrescaled()),
                    c.getChainStep(),
                    c.getChainName())
        PyAthena.HLT.Chain.__str__ = _str_hlt_chain
        del _str_hlt_chain
        
        self.sg = PyAthena.py_svc('StoreGateSvc')
        if not self.sg:
            self.msg.error('could not retrieve event store !')
            return StatusCode.Failure

        self._passed_evts = open(self.filename, 'w')
        return StatusCode.Success

    def execute(self):
        _info = self.msg.info
        _info('==> execute...')
        self.setFilterPassed(False)

        evid = self.sg.retrieve('EventInfo', 'McEventInfo').event_ID()
        
        ef_state = bool(self.trigDec.isPassed("EF_.*"))
        _info('Pass state EF = %r', ef_state)

        # test L2_e25i ?
        n = "L2_e25i"
        passed = bool(self.trigDec.isPassed(n))
        if not passed:
            _info("chain %s: passed: %s",n, passed)
#            _info('== FAILED ==')
        else:
            _info("chain %s: passed: %s",n, passed)
            _info('== SUCCESS ==')
            # this is mainly for regression test purposes
            evid = "[%s/%s]" % (evid.run_number(),
                                evid.event_number())
            self._passed_evts.write(evid+os.linesep)
            #_info('filter passed for %s',evid)
        self.setFilterPassed(passed)
        return StatusCode.Success

    def finalize(self):
        self.msg.info('==> finalize...')
        self._passed_evts.flush()
        self._passed_evts.close()
        return StatusCode.Success

    pass # class TrigDecTestAlg

### ---------------------------------------------------------------------------
class PyFilterAlg (PyAthena.Alg):
    'Dummy algorithm filtering out one event out of 2'
    def __init__(self, name='PyFilterAlg', **kw):
        ## init base class
        kw['name'] = name
        super(PyFilterAlg, self).__init__(**kw)

        ## properties and data members
        self.filename = kw.get('filename', 'evtlist.ascii')
        self.evtinfo  = kw.get('evtinfo',  'EventInfo')
        return

    def initialize(self):
        self.msg.info('==> initialize...')
        self.sg = PyAthena.py_svc('StoreGateSvc')
        if not self.sg:
            self.msg.error('could not retrieve event store !')
            return StatusCode.Failure

        self._evts = open(self.filename, 'w')
        self._evtcnt = 0
        return StatusCode.Success

    def execute(self):
        self.setFilterPassed(False)
        _info = self.msg.info
        _info('==> execute...')
        evid = self.sg.retrieve('EventInfo', self.evtinfo).event_ID()
        
        evid = "[%s/%s]" % (evid.run_number(), evid.event_number())
        _info('[run/evt]: %s',evid)
        if self._evtcnt%2 == 0:
            self._evts.write(evid+os.linesep)
            self.setFilterPassed(True)
        self._evtcnt += 1
        return StatusCode.Success

    def finalize(self):
        self.msg.info('==> finalize...')
        self.msg.info('analyzed [%i] events', self._evtcnt)
        self._evts.flush()
        self._evts.close()
        return StatusCode.Success

    pass # class PyFilterAlg

### ---------------------------------------------------------------------------
class EventInfoDumper (PyAthena.Alg):
    'Dummy algorithm logging out run_nbr/evt_nbr'
    def __init__(self, name='EventInfoDumper', **kw):
        ## init base class
        kw['name'] = name
        super(EventInfoDumper, self).__init__(**kw)

        ## properties and data members
        self.filename = kw.get('filename', 'evtlist.ascii')
        self.evtinfo  = kw.get('evtinfo',  'EventInfo')
        return

    def initialize(self):
        self.msg.info('==> initialize...')
        self.sg = PyAthena.py_svc('StoreGateSvc')
        if not self.sg:
            self.msg.error('could not retrieve event store !')
            return StatusCode.Failure

        self._evts = open(self.filename, 'w')
        return StatusCode.Success

    def execute(self):
        _info = self.msg.info
        _info('==> execute...')
        evid = self.sg.retrieve('EventInfo', self.evtinfo).event_ID()
        
        evid = "[%s/%s]" % (evid.run_number(), evid.event_number())
        _info('[run/evt]: %s',evid)
        self._evts.write(evid+os.linesep)
        return StatusCode.Success

    def finalize(self):
        self.msg.info('==> finalize...')
        self._evts.flush()
        self._evts.close()
        return StatusCode.Success

    pass # class EventInfoDumper

### ---------------------------------------------------------------------------
class McDump(PyAthena.Alg):
    """A python algorithm to inspect McEventCollections
    """
    def __init__(self, name="McDump", **kw):
        ## init the base class
        kw['name'] = name
        super(McDump,self).__init__(**kw)

        ## provide default values for properties
        self.mcName = kw.get('mcName', 'GEN_EVENT')
        return

    def initialize(self):
        self.msg.info('initializing...')
        self.sg = PyAthena.py_svc('StoreGateSvc')
        if not self.sg:
            self.msg.error('Could not retrieve StoreGateSvc !')
            return StatusCode.Failure

        self.msg.info('Dumping configuration:')
        self.msg.info('McEventCollection key: %s', self.mcName)
        return StatusCode.Success

    def execute(self):
        _info = self.msg.info
        _info('running execute...')
        _info('retrieve [%s/%s]', 'McEventCollection', self.mcName)
        mc = self.sg.retrieve('McEventCollection', self.mcName)
        if not mc:
            self.msg.warning(
                'Could not retrieve McEventCollection at [%s]',
                self.mcName
                )
            return StatusCode.Recoverable

        if len(mc) <= 0:
            self.msg.info('No GenEvent in "%s"!', self.mcName)
        _info('number of GenEvents: %i', len(mc))
        evt = mc[0]
        _info('number of GenParticles: %i', evt.particles_size())
        _info('number of GenVertices:  %i', evt.vertices_size())

        return StatusCode.Success

    def finalize(self):
        self.msg.info('finalizing...')
        return StatusCode.Success

    pass # class McDump

### ---------------------------------------------------------------------------
class Thinner(PyAthena.Alg):
    """A python algorithm to thin containers
    """
    def __init__(self, name="Thinner", **kw):
        ## init the base class
        kw['name'] = name
        super(Thinner,self).__init__(**kw)

        ## provide default values for properties
        self.coll_type = kw.get('coll_type',  'JetCollection')
        self.coll_name = kw.get('coll_name',  'AtlfastJetContainer')
        self.filter_fct= kw.get('filter_fct', lambda x: x.e() > 40.*Units.GeV)
        return

    def initialize(self):
        self.msg.info('initializing...')
        self.sg = PyAthena.py_svc('StoreGateSvc')
        if not self.sg:
            self.msg.error('Could not retrieve StoreGateSvc !')
            return StatusCode.Failure

        self.thinSvc = PyAthena.py_svc('ThinningSvc/MyThinningSvc')
        if not self.thinSvc:
            self.msg.error('Could not retrieve [ThinningSvc/MyThinningSvc] !')
            return StatusCode.Failure
        
        self.msg.info('Dumping configuration:')
        self.msg.info('Container type: %s', self.coll_type)
        self.msg.info('Container key:  %s', self.coll_name)
        self.msg.info('Filter: %r',         self.filter_fct.__name__)
        return StatusCode.Success

    def execute(self):
        _info = self.msg.info
        _info('running execute...')
        _info('retrieve [%s/%s]', self.coll_type, self.coll_name)
        cont = self.sg.retrieve(self.coll_type, self.coll_name)
        if not cont:
            self.msg.warning(
                'Could not retrieve %s at [%s]',
                self.coll_type,
                self.coll_name
                )
            return StatusCode.Recoverable

        if len(cont) <= 0:
            self.msg.info('Container "%s/%s" is empty',
                          self.coll_type, self.coll_name)
        _info('number of elements before thinning: %i', len(cont))

        predicate = self.filter_fct
        mask = [ predicate(element) for element in cont ]

        ## apply thinning:
        if not (self.thinSvc.filter(cont, mask) == StatusCode.Success):
            self.msg.error("Could not apply thinning !")
            return StatusCode.Failure
        
        _info('number of elements after  thinning: %i',
              len([m for m in mask if m]))
        
        return StatusCode.Success

    def finalize(self):
        self.msg.info('finalizing...')
        return StatusCode.Success

    pass # class Thinner

### ---------------------------------------------------------------------------
class BasicThinner(PyAthena.Alg):
    """A python algorithm to thin basic containers
    """
    def __init__(self, name="BasicThinner", **kw):
        ## init the base class
        kw['name'] = name
        super(BasicThinner,self).__init__(**kw)

        ## provide default values for properties
        self.coll_type = kw.get('coll_type',  'AthExParticles')
        self.coll_name = kw.get('coll_name',  'athexparticles')
        self.filter_fct= kw.get('filter_fct', lambda i,x: i%2==0)
        ## thinning-svc name
        self.thinSvc   = kw.get('thinSvc', 'ThinningSvc/MyThinningSvc')
        return

    def initialize(self):
        self.msg.info('initializing...')
        self.sg = PyAthena.py_svc('StoreGateSvc')
        if not self.sg:
            self.msg.error('Could not retrieve StoreGateSvc !')
            return StatusCode.Failure

        self.thinSvc = PyAthena.py_svc(self.thinSvc)
        if not self.thinSvc:
            self.msg.error('Could not retrieve thinning service!')
            return StatusCode.Failure
        
        self.msg.info('Dumping configuration:')
        self.msg.info('Container type: %s', self.coll_type)
        self.msg.info('Container key:  %s', self.coll_name)
        self.msg.info('Filter: %r',         self.filter_fct.__name__)
        return StatusCode.Success

    def execute(self):
        _info = self.msg.info
        _info('running execute...')

        _info('retrieve [%s/%s]', self.coll_type, self.coll_name)
        cont = self.sg.retrieve(self.coll_type, self.coll_name)
        if not cont:
            self.msg.warning(
                'Could not retrieve %s at [%s]',
                self.coll_type,
                self.coll_name
                )
            return StatusCode.Recoverable

        if len(cont) <= 0:
            self.msg.info('Container "%s/%s" is empty',
                          self.coll_type, self.coll_name)
        _info('number of elements before thinning: %i', len(cont))

        predicate = self.filter_fct
        mask = [ predicate(idx,element) for idx,element in enumerate(cont) ]

        ## apply thinning:
        if not (self.thinSvc.filter(cont, mask) == StatusCode.Success):
            self.msg.error("Could not apply thinning !")
            return StatusCode.Failure
        
        _info('number of elements after  thinning: %i',
              len([m for m in mask if m]))
        
        return StatusCode.Success

    def finalize(self):
        self.msg.info('finalizing...')
        return StatusCode.Success

    pass # class BasicThinner

### ---------------------------------------------------------------------------
class ThinChecker(PyAthena.Alg):
    """A python algorithm to check the output of maybe thinned basic containers
    """
    def __init__(self, name="ThinChecker", **kw):
        ## init the base class
        kw['name'] = name
        super(ThinChecker,self).__init__(**kw)

        ## provide default values for properties
        self.coll_type = kw.get('coll_type',  'AthExParticles')
        self.coll_name = kw.get('coll_name',  'athexparticles')

        ## storegate
        self.sg = None
        return

    def initialize(self):
        self.msg.info('initializing...')
        self.sg = PyAthena.py_svc('StoreGateSvc')
        if not self.sg:
            self.msg.error('Could not retrieve StoreGateSvc !')
            return StatusCode.Failure

        self.msg.info('Dumping configuration:')
        self.msg.info('Container type: %s', self.coll_type)
        self.msg.info('Container key:  %s', self.coll_name)
        return StatusCode.Success

    def execute(self):
        _info = self.msg.info
        _info('running execute...')

        _info('retrieve [%s/%s]', self.coll_type, self.coll_name)
        cont = self.sg.retrieve(self.coll_type, self.coll_name)
        if not cont:
            self.msg.warning(
                'Could not retrieve %s at [%s]',
                self.coll_type,
                self.coll_name
                )
            return StatusCode.Recoverable

        if len(cont) <= 0:
            self.msg.info('Container "%s/%s" is empty',
                          self.coll_type, self.coll_name)
        _info('number of elements : %i', len(cont))

        for idx,e in enumerate(cont):
            _info( '[%s][%i]: %f %f %f %f', self.coll_name, idx,
                   e.e(), e.px(), e.py(), e.pz() )
            pass
        return StatusCode.Success

    def finalize(self):
        self.msg.info('finalizing...')
        return StatusCode.Success

    pass # class ThinChecker

### ---------------------------------------------------------------------------
from math import log10

## helper function to turn a pair of (c++) iterators into
## a valid python iterator (stolen from Scott)
def toiter(beg,end):
    while beg != end:
        yield beg.__deref__()
        beg.__preinc__()
    return

## FIXME: 'temporary' patch to work around missing dict.
## for 'CaloCluster::MomentStoreIter'
class _patch:
    @staticmethod
    def CaloClusterMoments():
        import ROOT
        return [
            ROOT.CaloClusterMoment.FIRST_PHI,
            ROOT.CaloClusterMoment.FIRST_ETA,
            ROOT.CaloClusterMoment.SECOND_R,
            ROOT.CaloClusterMoment.SECOND_LAMBDA,
            ROOT.CaloClusterMoment.DELTA_PHI,
            ROOT.CaloClusterMoment.DELTA_THETA,
            ROOT.CaloClusterMoment.DELTA_ALPHA,
            ROOT.CaloClusterMoment.CENTER_X,
            ROOT.CaloClusterMoment.CENTER_Y,
            ROOT.CaloClusterMoment.CENTER_Z,
            ROOT.CaloClusterMoment.CENTER_LAMBDA,
            ROOT.CaloClusterMoment.LATERAL,
            ROOT.CaloClusterMoment.LONGITUDINAL,
            ROOT.CaloClusterMoment.ENG_FRAC_EM,
            ROOT.CaloClusterMoment.ENG_FRAC_MAX,
            ROOT.CaloClusterMoment.ENG_FRAC_CORE,
            ROOT.CaloClusterMoment.FIRST_ENG_DENS,
            ROOT.CaloClusterMoment.SECOND_ENG_DENS,
            ROOT.CaloClusterMoment.ISOLATION,
            ]
            
class ClusterExAlg(PyAthena.Alg):
    """example on how to use Calo-stuff in PyAthena
    """
    def __init__(self, name='ClusterExAlg', **kw):
        # base init
        kw['name'] = name
        super(ClusterExAlg, self).__init__(**kw)

        # sg key for clusters
        self.clustersName = kw.get('Clusters', 'CaloCalTopoCluster')

        self.hsvc = None # handle to histo svc
        self.sg   = None # handle to storegate
        return

    def initialize(self):
        _info = self.msg.info
        _info('==> initialize')
        self.hsvc = PyAthena.py_svc('THistSvc')
        if not self.hsvc:
            self.msg.error('could not retrieve THistSvc')
            return StatusCode.Failure

        self.sg = PyAthena.py_svc('StoreGateSvc')
        if not self.sg:
            self.msg.error('could not retrieve event store')
            return StatusCode.Failure

        _info('reading clusters from [%r]', self.clustersName)

        # registering histograms
        import ROOT
        hsvc = self.hsvc
        hsvc['/py/clusE']   = ROOT.TH1F('clusE',   'clusE',   100, -4.0, 6.0)
        hsvc['/py/clusEta'] = ROOT.TH1F('clusEta', 'clusEta', 220, -5.5, 5.5)

        self._evtNbr = -1
        return StatusCode.Success

    def execute(self):
        _info = self.msg.info
        _info('==> execute')
        self._evtNbr += 1
        # retrieve clusters
        clusters = self.sg.retrieve('CaloClusterContainer', self.clustersName)
        if not clusters:
            self.msg.warning('could not retrieve clusters at [%r]',
                             self.clustersName)
            return StatusCode.Recoverable

        _info('event #%i has %3i clusters', self._evtNbr, len(clusters))
        _clusE   = self.hsvc['/py/clusE'].Fill
        _clusEta = self.hsvc['/py/clusEta'].Fill

        from ROOT import CaloClusterMoment
        from ROOT import Double as RDouble
        
        for i,cluster in enumerate(clusters):
            ene = cluster.e()

            if   ene < -1.: ene = -log10(-ene)
            elif ene >  1.: ene =  log10( ene)
            else:           ene = 0.
            _clusE(ene)
            _clusEta(cluster.eta())

            if i%100 != 0:
                continue

            _info('cluster #%3i has energy %8.3f',i, ene)
            ## FIXME: missing dictionary for 'CaloCluster::MomentStoreIter'
##             for moment in toiter(cluster.beginMoment(useLink=False),
##                                  cluster.endMoment  (useLink=False)):
            
            for m in _patch.CaloClusterMoments():
                moment = RDouble(0.)
                if cluster.retrieveMoment(m, moment, useLink=False):
                    _info('cluster #%3i has moment #%3i with value %8.3f',
                          i, m, moment)
            pass # loop over clusters

        return StatusCode.Success

    def finalize(self):
        _info = self.msg.info
        _info('==> finalize')
        import AthenaCommon.Constants
        clusE   = self.hsvc['/py/clusE']
        clusEta = self.hsvc['/py/clusEta']

        _info('=== analyzed [%i] events ===', self._evtNbr)
        _info(' <cluster ene> = %8.3f MeV', clusE.GetMean())
        _info('          rms  = %8.3f MeV', clusE.GetRMS() )
        _info(' <cluster eta> = %8.3f', clusEta.GetMean())
        _info('          rms  = %8.3f', clusEta.GetRMS ())
        return StatusCode.Success

    pass # ClusterExAlg

### reading clusters, but with Ares -------------------------------------------
class AresClusterExAlg(PyAthena.Alg):
    """an example on how to do calo-stuff in PyAthena/Ares"""
    def __init__(self, name='AresClusterExAlg', **kw):
        # base init
        kw['name'] = name
        super(AresClusterExAlg, self).__init__(**kw)

        # sg key for clusters
        self.clustersName = kw.get('Clusters', 'CaloCalTopoCluster')

        self.hsvc = None # handle to histo svc
        self.sg   = None # handle to storegate
        self.ares = None # handle to the AresEvtSvc
        return

    def initialize(self):
        _info = self.msg.info
        _info('==> initialize')
        self.hsvc = PyAthena.py_svc('THistSvc')
        if not self.hsvc:
            self.msg.error('could not retrieve THistSvc')
            return StatusCode.Failure

        self.sg = PyAthena.py_svc('StoreGateSvc')
        if not self.sg:
            self.msg.error('could not retrieve event store')
            return StatusCode.Failure

        _info('reading clusters from [%r]', self.clustersName)

        # registering histograms
        import ROOT
        hsvc = self.hsvc
        hsvc['/py/clusE']   = ROOT.TH1F('clusE',   'clusE',   100, -4.0, 6.0)
        hsvc['/py/clusEta'] = ROOT.TH1F('clusEta', 'clusEta', 220, -5.5, 5.5)

        self.ares = self.hsvc['/temp/TTreeStream/CollectionTree_trans']
        
        self._evtNbr = -1
        return StatusCode.Success

    def execute(self):
        _info = self.msg.info
        _info('==> execute')
        self._evtNbr += 1
        # retrieve clusters
        clusters = getattr(self.ares, self.clustersName)
        if not clusters:
            self.msg.warning('could not retrieve clusters at [%r]',
                             self.clustersName)
            return StatusCode.Recoverable

        _info('event #%i has %3i clusters', self._evtNbr, len(clusters))
        _clusE   = self.hsvc['/py/clusE'].Fill
        _clusEta = self.hsvc['/py/clusEta'].Fill

        from ROOT import CaloClusterMoment
        from ROOT import Double as RDouble
        
        for i,cluster in enumerate(clusters):
            ene = cluster.e()

            if   ene < -1.: ene = -log10(-ene)
            elif ene >  1.: ene =  log10( ene)
            else:           ene = 0.
            _clusE(ene)
            _clusEta(cluster.eta())

            if i%100 != 0:
                continue

            _info('cluster #%3i has energy %8.3f',i, ene)
            ## FIXME: missing dictionary for 'CaloCluster::MomentStoreIter'
##             for moment in toiter(cluster.beginMoment(useLink=False),
##                                  cluster.endMoment  (useLink=False)):
            
            for m in _patch.CaloClusterMoments():
                moment = RDouble(0.)
                if cluster.retrieveMoment(m, moment, useLink=False):
                    _info('cluster #%3i has moment #%3i with value %8.3f',
                          i, m, moment)
            pass # loop over clusters

        return StatusCode.Success

    def finalize(self):
        _info = self.msg.info
        _info('==> finalize')
        import AthenaCommon.Constants
        clusE   = self.hsvc['/py/clusE']
        clusEta = self.hsvc['/py/clusEta']

        _info('=== analyzed [%i] events ===', self._evtNbr)
        _info(' <cluster ene> = %8.3f MeV', clusE.GetMean())
        _info('          rms  = %8.3f MeV', clusE.GetRMS() )
        _info(' <cluster eta> = %8.3f', clusEta.GetMean())
        _info('          rms  = %8.3f', clusEta.GetRMS ())
        return StatusCode.Success

    pass # AresClusterExAlg

