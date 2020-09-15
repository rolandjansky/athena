# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# @file: AthExThinning/python/Lib.py
# @purpose: a set of Py-components to tests py-thinning
# @author: Sebastien Binet <binet@cern.ch>

import AthenaCommon.SystemOfUnits as Units
from AthenaPython import PyAthena
StatusCode = PyAthena.StatusCode
import ROOT


def keep (dec, l, op = 'set'):
    if op == 'and':
        opflag = 1
    elif op == 'or':
        opflag = 2
    else:
        opflag = 0
    if isinstance (l, list):
        for i, x in enumerate(l):
            dec.keep (i, x, opflag)
    else:
        dec.keep (l, opflag)
    return
    

class PyWriteThinnedData( PyAthena.Alg ):
    """A simple python algorithm to thin data
    """
    def __init__(self, name = "PyWriteThinnedData", **kw):
        ## init base class
        kw['name'] = name
        super(PyWriteThinnedData,self).__init__(**kw)

        self.Particles   = kw.get('Particles',   "Particles")
        self.Decay       = kw.get('Decay',       "TwoBodyDecay")
        self.Elephantino = kw.get('Elephantino', "PinkElephantino")
        self.Filter      = kw.get('Filter',      [False]*10)

    def initialize(self):
        self.msg.info( "Initializing %s", self.name )
        ## storegate
        self.sg = PyAthena.StoreGate.pointer("StoreGateSvc")
        # or: PyAthena.py_svc("StoreGateSvc",createIf=True)
        
        ## Datavector stuff
        import cppyy
        cppyy.load_library("libAthExThinningEventDict")
        from RootUtils import PyROOTFixes  # noqa: 401
        self.filter = self.Filter
        return StatusCode.Success

    def execute(self):
        self.msg.debug( "Executing %s...", self.name )
        allGood = True
        for t in ("test1","test2","test3",):
            if self.test(t) != StatusCode.Success:
                self.msg.warning( "Could not perform 'thinning %s'",t )
                allGood = False
        if allGood: return StatusCode.Success
        return StatusCode.Failure

    def test(self, testName):
        self.msg.info( "Performing thinning test [%s]...", testName )

        # fetch particles
        particlesName = "%s_%s" % (self.Particles,testName)
        particles = self.sg.retrieve( "AthExParticles", particlesName )
        if particles is None:
            self.msg.warning( "Could not fetch particles at [%s] !!",
                              self.Particles )
            return StatusCode.Failure
        self.msg.info( "particles: %i", particles.size() )

        # fetch iparticles
        iparticles = self.sg.retrieve( "AthExIParticles", particlesName )
        if iparticles is None:
            self.msg.warning( "Could not fetch iparticles at [%s] !!",
                              self.Particles )
            return StatusCode.Failure
        self.msg.info( "iparticles: %i", iparticles.size() )
        
        if ( iparticles.size()     != particles.size() or
             iparticles.at(0).px() != particles.at(0).px() ):
            self.msg.error( "symlinked containers are corrupted: " )
            self.msg.error( " #iparticles: %i", iparticles.size() )
            self.msg.error( " # particles: %i",  particles.size() )
            self.msg.error( "  ipx[0] = %r", iparticles.at(0).px() )
            self.msg.error( "   px[0] = %r",  particles.at(0).px() )
            return StatusCode.Failure
        
        # fetch decay
        decay = self.sg.retrieve( "AthExDecay",
                                  "%s_%s" % (self.Decay, testName) )
        if decay is None:
            self.msg.warning( "Could not fetch Decay at [%s] !!",
                              self.Decay )
            return StatusCode.Failure

        # fetch elephantino
        elephantino = self.sg.retrieve( "AthExElephantino",
                                        "%s_%s" % (self.Elephantino, testName) )
        if elephantino is None:
            self.msg.warning( "Could not fetch Elephantino at [%s] !!",
                              self.Elephantino )
            return StatusCode.Failure

        self.msg.debug( "IN particles: %i", particles.size() )
        self.msg.debug( "IN decay:" )
        self.msg.debug( " p1: px= %r", decay.p1().px() / Units.GeV )
        self.msg.debug( " p2: px= %r", decay.p2().px() / Units.GeV )
        self.msg.debug( " l1: px= %r", decay.l1().px() / Units.GeV )
        self.msg.debug( " l2: px= %r", decay.l2().px() / Units.GeV )

        self.msg.debug( "IN elephantino:" )
        self.msg.debug( " leg1: px= %r", elephantino.leg1().px() / Units.GeV )
        self.msg.debug( " leg2: px= %r", elephantino.leg2().px() / Units.GeV )
        self.msg.debug( " leg3: px= %r", elephantino.leg3().px() / Units.GeV )
        self.msg.debug( " leg4: px= %r", elephantino.leg4().px() / Units.GeV )
        self.msg.debug( " ear1: px= %r", elephantino.ear1().px() / Units.GeV )
        self.msg.debug( " ear2: px= %r", elephantino.ear2().px() / Units.GeV )

        ## thinning
        dec = ROOT.SG.ThinningDecision (particlesName)
        if   testName == "test1":
            if self.doThinningTest1( particles, dec) != StatusCode.Success:
                self.msg.warning( "Could not exercize Thinning !!" )
        elif testName == "test2":
            if self.doThinningTest2( particles, dec) != StatusCode.Success:
                self.msg.warning( "Could not exercize Thinning !!" )
        elif testName == "test3":
            if self.doThinningTest3(iparticles, dec) != StatusCode.Success:
                self.msg.warning( "Could not exercize Thinning !!" )
        else:
            self.msg.error( "Unknown test [%s]", testName )
            return StatusCode.Failure
        dec.lock()
        if not self.sg.record (dec, particlesName + '_THINNED_StreamUSR_0'):
            return StatusCode.Failure
        ROOT.SetOwnership (dec, False)

        self.msg.info( "Decay is now:" )
        self.msg.info( " p1: px= %r", decay.p1().px() / Units.GeV )
        self.msg.info( " p2: px= %r", decay.p2().px() / Units.GeV )
        self.msg.info( " l1: px= %r", decay.l1().px() / Units.GeV )
        self.msg.info( " l2: px= %r", decay.l2().px() / Units.GeV )

        self.msg.info( "Elephantino is now: " )
        self.msg.info( " leg1: px= %r", elephantino.leg1().px() / Units.GeV )
        self.msg.info( " leg2: px= %r", elephantino.leg2().px() / Units.GeV )
        self.msg.info( " leg3: px= %r", elephantino.leg3().px() / Units.GeV )
        self.msg.info( " leg4: px= %r", elephantino.leg4().px() / Units.GeV )
        self.msg.info( " ear1: px= %r", elephantino.ear1().px() / Units.GeV )
        self.msg.info( " ear2: px= %r", elephantino.ear2().px() / Units.GeV )

        self.msg.info( "[%s] has been performed.", testName )
        return StatusCode.Success

    def doThinningTest1(self, particles, dec):
        filter = self.filter[:]
        RemovedIdx = ROOT.SG.ThinningDecisionBase.RemovedIdx
        self.msg.info( "Particles | filter :" )
        for i in range(particles.size()):
            if filter[i]: kr = "keep"
            else:         kr = "remove"
            self.msg.info( "%9s | %s", (i+1)*10, kr )
        self.msg.info( "="*19 )

        filter[len(filter)//2:] = [True]*(len(filter)//2)
        self.msg.info( "Filter %r", filter )

        self.msg.info( "... Processing [pre-thinning] ..." )
        keep (dec, filter)

        self.msg.info( "======== Index table =========" )
        tmp = ROOT.SG.ThinningDecisionBase (dec)
        tmp.buildIndexMap()
        import os
        os.sys.stdout.flush()
        for i in range(particles.size()):
            newIdx = tmp.index(i)
            if newIdx == RemovedIdx: newIdx = "-"
            self.msg.info( " idx %i -> %s", i, newIdx )

        filter = self.filter[:]
        filter[:len(filter)//2] = [True]*(len(filter)//2)
        self.msg.info( "Filter %r", filter )
        
        self.msg.info( "... Processing [thinning] ..." )
        keep (dec, filter, 'and')
        
        self.msg.info( "======== Index table =========" )
        tmp = ROOT.SG.ThinningDecisionBase (dec)
        tmp.buildIndexMap()
        for i in range(particles.size()):
            newIdx = tmp.index(i)
            if newIdx == RemovedIdx: newIdx = "-"
            self.msg.info( " idx %i -> %s", i, newIdx )

        return StatusCode.Success

    def doThinningTest2(self, particles, dec):
        filter = self.filter[:]
        RemovedIdx = ROOT.SG.ThinningDecisionBase.RemovedIdx
        self.msg.info( "Particles | filter :" )
        for i in range(particles.size()):
            if filter[i]: kr = "keep"
            else:         kr = "remove"
            self.msg.info( "%9s | %s", (i+1)*10, kr )
        self.msg.info( "="*19 )

        filter[len(filter)//2:] = [False]*(len(filter)//2)
        self.msg.info( "Filter %s", filter )

        self.msg.info( "... Processing [pre-thinning] ..." )
        keep (dec, filter)

        self.msg.info( "======== Index table =========" )
        tmp = ROOT.SG.ThinningDecisionBase (dec)
        tmp.buildIndexMap()
        for i in range(particles.size()):
            newIdx = tmp.index(i)
            if newIdx == RemovedIdx: newIdx = "-"
            self.msg.info( " idx %i -> %s", i, newIdx )

        filter = self.filter[:]
        filter[:len(filter)//2] = [False]*(len(filter)//2)
        self.msg.info( "Filter %s", filter )
        
        self.msg.info( "... Processing [thinning] ..." )
        keep (dec, filter, 'or')
        
        self.msg.info( "======== Index table =========" )
        tmp = ROOT.SG.ThinningDecisionBase (dec)
        tmp.buildIndexMap()
        for i in range(particles.size()):
            newIdx = tmp.index(i)
            if newIdx == RemovedIdx: newIdx = "-"
            self.msg.info( " idx %i -> %s", i, newIdx )

        return StatusCode.Success

    def doThinningTest3(self, iparticles, dec):
        filter = self.filter[:]
        RemovedIdx = ROOT.SG.ThinningDecisionBase.RemovedIdx
        self.msg.info( "IParticles | filter :" )
        for i in range(iparticles.size()):
            if filter[i]: kr = "keep"
            else:         kr = "remove"
            self.msg.info( "%9s | %s", (i+1)*10, kr )
        self.msg.info( "="*19 )

        filter[len(filter)//2:] = [True]*(len(filter)//2)
        self.msg.info( "Filter %r", filter )

        self.msg.info( "... Processing [pre-thinning] ..." )
        keep (dec, filter)

        self.msg.info( "======== Index table =========" )
        tmp = ROOT.SG.ThinningDecisionBase (dec)
        tmp.buildIndexMap()
        for i in range(iparticles.size()):
            newIdx = tmp.index(i)
            if newIdx == RemovedIdx: newIdx = "-"
            self.msg.info( " idx %i -> %s", i, newIdx )

        filter = self.filter[:]
        filter[:len(filter)//2] = [True]*(len(filter)//2)
        self.msg.info( "Filter %r", filter )
        
        self.msg.info( "... Processing [thinning] ..." )
        keep (dec, filter, 'and')
        
        self.msg.info( "======== Index table =========" )
        tmp = ROOT.SG.ThinningDecisionBase (dec)
        tmp.buildIndexMap()
        for i in range(iparticles.size()):
            newIdx = tmp.index(i)
            if newIdx == RemovedIdx: newIdx = "-"
            self.msg.info( " idx %i -> %s", i, newIdx )

        return StatusCode.Success

    def finalize(self):
        self.msg.info( "Finalizing %s...", self.name )
        return StatusCode.Success

    pass # PyWriteThinnedData


class PyReadNonThinnedData( PyAthena.Alg ):
    """A simple python algorithm to read non-thinned data
    """
    def __init__(self, name = "PyReadNonThinnedData", **kw):
        ## init base class
        kw['name'] = name
        super(PyReadNonThinnedData,self).__init__(**kw)

        self.Particles   = kw.get('Particles',   "Particles")
        self.Decay       = kw.get('Decay',       "TwoBodyDecay")
        self.Elephantino = kw.get('Elephantino', "PinkElephantino")

    def initialize(self):
        self.msg.info( "Initializing %s", self.name )
        ## storegate
        self.sg = PyAthena.StoreGate.pointer("StoreGateSvc")
        # or: PyAthena.py_svc("StoreGateSvc",createIf=True)
        
        ## Datavector stuff
        import cppyy
        cppyy.load_library("libAthExThinningEventDict")
        from RootUtils import PyROOTFixes  # noqa: F401
        return StatusCode.Success

    def execute(self):
        sg = self.sg
        _warning = self.msg.warning
        _info    = self.msg.info
        _info( "Executing %s...", self.name )
        for test in ("test1", "test2", "test3"):
            key = "%s_%s"%(self.Particles,test)
            particles = sg.retrieve("AthExParticles", key)
            if particles is None:
                _warning("Could not fetch particles at [%s] !!", key)
                return StatusCode.Failure

            iparticles = sg.retrieve("AthExIParticles", key)
            if iparticles is None:
                _warning("Could not fetch iparticles at [%s] !!", key)
                return StatusCode.Failure

            key = "%s_%s" % (self.Decay, test)
            decay = sg.retrieve("AthExDecay", key)
            if decay is None:
                _warning("Could not fetch Decay at [%s] !!", key)
                return StatusCode.Failure

            key = "%s_%s" % (self.Elephantino, test)
            elephantino = self.sg.retrieve("AthExElephantino", key)
            if elephantino is None:
                _warning("Could not fetch Elephantino at [%s] !!", key)
                return StatusCode.Failure

            _info( "Test: %s", test)
            _info( "IN  particles: %i", particles.size() )
            _info( "IN iparticles: %i", iparticles.size() )
            _info( "IN decay:" )
            _info( " p1: px= %r", decay.p1().px() / Units.GeV )
            _info( " p2: px= %r", decay.p2().px() / Units.GeV )
            _info( " l1: px= %r", decay.l1().px() / Units.GeV )
            _info( " l2: px= %r", decay.l2().px() / Units.GeV )

            _info( "IN elephantino:" )
            _info( " leg1: px= %r", elephantino.leg1().px() / Units.GeV )
            _info( " leg2: px= %r", elephantino.leg2().px() / Units.GeV )
            _info( " leg3: px= %r", elephantino.leg3().px() / Units.GeV )
            _info( " leg4: px= %r", elephantino.leg4().px() / Units.GeV )
            _info( " ear1: px= %r", elephantino.ear1().px() / Units.GeV )
            _info( " ear2: px= %r", elephantino.ear2().px() / Units.GeV )
        return StatusCode.Success

    def finalize(self):
        self.msg.info( "Finalizing %s...", self.name )
        return StatusCode.Success

    pass # PyReadNonThinnedData

### ---------------------------------------------------------------------------
class PyReadFatObject (PyAthena.Alg):
    """a simple algorithm to read back AthExFatObject"""
    def __init__ (self, name="PyReadFatObject", **kw):
        ## init base class
        kw['name'] = name
        super (PyReadFatObject, self).__init__ (**kw)

        ## properties and data members
        self.particles = kw.get ("particles", "AthExParticles")
        self.fatobject = kw.get ("fatobject", "AthExFatObject")

        return

    def initialize(self):
        _info = self.msg.info
        _info('==> initialize...')
        # retrieve storegate
        self.sg = PyAthena.py_svc ("StoreGateSvc")
        if not self.sg:
            self.msg.error ("could not retrieve the event store !")
            return StatusCode.Failure

        _info ("input particles: [%s]", self.particles)
        _info ("input fatobject: [%s]", self.fatobject)
        
        return StatusCode.Success

    def execute(self):
        _info = self.msg.info
        particles = self.sg[self.particles]
        if not particles:
            self.msg.error ("could not retrieve particles at [%s] !",
                            self.particles)
            return StatusCode.Failure
        _info ("particles' size: %i", particles.size())

        fatobject = self.sg[self.fatobject]
        if not fatobject:
            self.msg.error ("could not retrieve fatobject at [%s] !",
                            self.fatobject)
            return StatusCode.Failure

        raw = fatobject.particle()
        lnk = fatobject.particleLink()
        _info ("fat.raw.px: %s",
               "slimmed" if not raw           else (raw.px()/Units.GeV))
        _info ("fat.lnk.e : %s",
               "slimmed" if not lnk.isValid() else (lnk.e() /Units.GeV))
        
        return StatusCode.Success

    def finalize(self):
        self.msg.info('==> finalize...')
        return StatusCode.Success

    # class PyReadFatObject
