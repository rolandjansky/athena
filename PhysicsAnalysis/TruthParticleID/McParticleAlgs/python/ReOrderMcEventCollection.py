# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file:    McParticleAlgs/python/FixMcEventCollection.py
# @purpose: <put some purpose here>
# @author:  P-A Delsart 

__doc__     = '''Re-order the McEventCollection so the first GenEvent is the signal event.
IMPORTANT : this alg must be run before any other algs invoking TruthParticleCnvToolContainer.
'''

__version__ = '$Revision: 1.5 $'
__author__  = 'P-A Delsart'

import AthenaCommon.SystemOfUnits as Units
import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

import ROOT

class FixMcEventCollection (PyAthena.Alg):
    def __init__(self, name='FixMcEventCollection', **kw):
        ## init base class
        kw['name'] = name
        super(FixMcEventCollection, self).__init__(**kw)

        ## properties and data members
        #self.foo = kw.get('foo', 10) # default value
        return

    def initialize(self):
        self.msg.info('==> initialize...')        
        self.sg = PyAthena.py_svc ('StoreGateSvc')        
        return StatusCode.Success


    def execute(self):
        #self.msg.info('==> execute...')
        sg = self.sg
        mcevt0 = sg.retrieve('McEventCollection',"GEN_AOD")

        # if 1st event is populated, the collection is correct
        genEvt = mcevt0[0]
        if genEvt.particles_size()>1:        
            return StatusCode.Success

        sigEvt = None
        puEvts = []
        for evt in mcevt0:
            if evt.particles_size()>1:
                sigEvt = evt
            else:
                puEvts.append(evt)

        etmp = ROOT.HepMC.GenEvent() # temporary only used for swapping
        correctOrder = [sigEvt] + puEvts
        for i, e in enumerate(correctOrder):
            mcevt0.swapElement(i,e,etmp)

        return StatusCode.Success            

    def finalize(self):
        self.msg.info('==> finalize...')
        return StatusCode.Success

