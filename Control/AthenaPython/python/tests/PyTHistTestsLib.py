# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file    AthenaPython/python/tests/PyTHistTestsLib.py
# @purpose Test read and write histograms and trees via the @c ITHistSvc
# @author  Sebastien Binet <binet@cern.ch>

__doc__ = """Test read and write histograms and trees via ITHistSvc"""
__version__ = "$Revision: 1.4 $"
__author__  = "Sebastien Binet <binet@cern.ch>"

from AthenaPython import PyAthena
StatusCode = PyAthena.StatusCode

class PyHistReader(PyAthena.Alg):
    """Simple algorithm to read data from Root hist. svc
    """
    def __init__(self, name='PyHistReader', **kw):
        # init base class
        kw['name'] = name
        super(PyHistReader,self).__init__(**kw)

        # handle to the root hist svc
        self.hsvc = None
        return

    def initialize(self):
        _info = self.msg.info
        _info("==> initialize...")
        self.hsvc = PyAthena.py_svc('THistSvc/THistSvc')
        if not self.hsvc:
            self.msg.error('Could not retrieve THistSvc')
            return StatusCode.Failure

        def print_properties(h):
            _info(' -%-20s: %i <mean>=%8.3f rms=%8.3f',
                  h.GetName(), h.GetEntries(), h.GetMean(), h.GetRMS())
        
        # load histos from stream and print some of their properties
        o = self.hsvc.load('/read1/xxx/gauss1d', oid_type='hist')
        print_properties(o)
        
        o = self.hsvc.load('/read2/gauss2d', oid_type='hist')
        print_properties(o)

        o = self.hsvc.load('/read2/gauss3d', oid_type='hist')
        print_properties(o)

        o = self.hsvc.load('/read2/profile', oid_type='hist')
        print_properties(o)

        ## FIXME: 'THistSvc::getTrees' (always) segfaults
        ## https://savannah.cern.ch/bugs/index.php?36379
        try:
            o = self.hsvc.load('/read2/trees/stuff/tree1', oid_type='tree')
            _info(' -%-20s: %i', o.GetName(), o.GetEntries())
        except KeyError,err:
            self.msg.error(err)
            self.msg.error('bug #36379 still not fixed...')
        return StatusCode.Success

    def execute(self):
        _info = self.msg.info
        _info('==> execute...')
        return StatusCode.Success

    def finalize(self):
        self.msg.info('==> finalize...')
        return StatusCode.Success

    pass # class PyHistReader

from math import sin as math_sin
from random import gauss
from array import array
class PyHistWriter(PyAthena.Alg):
    """A simple python algorithm to write stuff into a ROOT file via the
    @c ITHistSvc (translated example from GaudiExamples/src/THist)
    """
    def __init__(self, name='PyHistWriter', **kw):
        # init base class
        kw['name']=name
        super(PyHistWriter,self).__init__(**kw)

        # handle to the root hist svc
        self.hsvc = None
        return

    def initialize(self):
        _info = self.msg.info
        _info('==> initialize...')
        self.hsvc = PyAthena.py_svc('THistSvc/THistSvc')
        if not self.hsvc:
            self.msg.error('could not retrieve THistSvc/THistSvc')
            return StatusCode.Failure

        from ROOT import TH1F, TH2F, TH3F, TProfile, TTree
        # helpers
        def th1(n,t): return TH1F(n,t,100,0.,100.)
        def th2(n,t): return TH2F(n,t,100,-50.,50.,100,-50.,50.)
        def th3(n,t): return TH3F(n,t,100,-50.,50.,100,-50.,50.,100,-50.,50.)
        def tp (n,t): return TProfile(n,t,100,-50.,50.)
        
        # temporary trees
        self.hsvc['/temp/h1']        = th1('h1', 'Temporary hist 1')
        self.hsvc['/temp/other/h1a'] = th1('h1a', 'Temporary hist 1a')

        # write to stream 'new'
        self.hsvc['/new/hists/h1'] = th1('h1', 'Persistent hist 1')

        # update to stream 'upd', dir '/xxx'
        self.hsvc['/upd/xxx/gauss1d'] = TH1F('gauss1d', '1D gaussian',
                                             100,-50.,50.)

        # recreate stuff in '/'
        self.hsvc['/rec/gauss2d'] = th2('gauss2d','2D gaussian')
        self.hsvc['/rec/gauss3d'] = th3('gauss3d','3D gaussian')
        self.hsvc['/rec/prof']    = tp ('profile','profile')

        # tree with branches in '/trees/stuff'
        self.hsvc['/rec/trees/stuff/tree1'] = TTree('tree1','tree title')

        self._n = -1
        return StatusCode.Success

    def execute(self):
        _info = self.msg.info
        _info('==> execute...')
        self._n += 1
        n = self._n
        x = math_sin(float(n)) * 52. + 50.
        hsvc = self.hsvc
        hsvc['/temp/h1'].Fill(x)
        for _ in xrange(2): hsvc['/temp/other/h1a'].Fill(x)
        for _ in xrange(3): hsvc['/new/hists/h1'].Fill(x)
        _fill = hsvc['/upd/xxx/gauss1d'].Fill
        for _ in xrange(1000): _fill(gauss(mu=0.,sigma=15.),1.)
        _fill = hsvc['/rec/gauss2d'].Fill
        for _ in xrange(1000): _fill(gauss(mu=0.,sigma=15.),
                                     gauss(mu=0.,sigma=15.), 1.)
        _fill = hsvc['/rec/gauss3d'].Fill
        for _ in xrange(1000): _fill(gauss(mu=0.,sigma=15.),
                                     gauss(mu=0.,sigma=15.),
                                     gauss(mu=0.,sigma=15.), 1.)

        tr = hsvc['/rec/trees/stuff/tree1']
        if n == 0:
            p1 = array('i',[0])
            p2 = array('i',[0])
            p3 = array('i',[0])
            tr.Branch('branch1', p1, 'point1/I')
            tr.Branch('branch2', p2, 'point2/I')
            tr.Branch('branch3', p3, 'point3/I')
            _tr_fill = tr.Fill
            for i in xrange(1000):
                p1[0] = i
                p2[0] = i%10
                p3[0] = i%7
                _tr_fill()
        return StatusCode.Success

    def finalize(self):
        _info = self.msg.info
        _info('==> finalize...')
        for n in ('/temp/h1', '/temp/other/h1a',
                  '/new/hists/h1'):
            _info('='*20)
            _info(' - histo [%r]', n)
            h = self.hsvc[n]
            _info(' entries = %i',    h.GetEntries())
            _info('    mean = %8.3f', h.GetMean())
            _info('    rms  = %8.3f', h.GetRMS())
        _info('='*20)
        return StatusCode.Success

    pass # class PyHistWriter
