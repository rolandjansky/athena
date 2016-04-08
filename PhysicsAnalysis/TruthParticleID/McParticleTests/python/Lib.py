# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file:    McParticleTests/python/Lib.py
# @purpose: <put some purpose here>
# @author:  Sebastien Binet <binet@cern.ch>

__doc__     = 'some documentation here'
__version__ = '$Revision: 1.1 $'
__author__  = 'Sebastien Binet <binet@cern.ch>'

import AthenaCommon.SystemOfUnits as Units
import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

class PyMcAodSymLinkTests (PyAthena.Alg):
    """Simple algorithm to test symlinks from python"""
    def __init__(self, name='PyMcAodSymLinkTests', **kw):
        ## init base class
        kw['name'] = name
        super(PyMcAodSymLinkTests, self).__init__(**kw)

        ## properties and data members
        self.mcpartsName = kw.get('TruthParticles', 'SpclMC') # default value

        # handle to event store
        self.sg = None

        # the types we want to retrieve with the above sg-key
        self._types = [
            'INavigable4MomentumCollection',
            'DataVector<IParticle>',
            'TruthParticleContainer',
            ]
        return

    def initialize(self):
        self.msg.info('==> initialize...')
        self.sg = PyAthena.py_svc('StoreGateSvc')
        if not self.sg:
            self.msg.error('could not retrieve StoreGateSvc')
            return StatusCode.Failure
        
        return StatusCode.Success

    def execute(self):
        _info = self.msg.info
        _error= self.msg.error
        _retrieve = self.sg.retrieve
        mcparts = []
        for cont_type in self._types:
            cont = _retrieve(cont_type, self.mcpartsName)
            if cont:
                mcparts += [cont]
            else:
                _info('could not retrieve [%s/%s] !',
                      cont_type, self.mcpartsName)
                pass
            pass
        if not len(mcparts) == len(self._types):
            _error('could not retrieve all containers !')
            return StatusCode.Failure
        
        if not all([len(c)==len(mcparts[0]) for c in mcparts]):
            _error('Symlinked containers do not have the same size !!')
            for i,cont_type in enumerate(self._types):
                _error(' %-20s: %r', cont_type, len(mcparts[i]))
            return StatusCode.Failure

        eps = 2.22045e-16
        allgood = True
        for i in xrange(len(mcparts[0])):
            in_ene = mcparts[0][i].e()
            ip_ene = mcparts[1][i].e()
            tp_ene = mcparts[2][i].e()
            if not( (tp_ene - ip_ene) < eps and \
                    (tp_ene - in_ene) < eps and \
                    (ip_ene - in_ene) < eps ):
                _error('symlink FAILS at index [%i]',i)
                _error(' TruthParticle::e(): %r', tp_ene)
                _error(' IParticle::e():     %r', tp_ene)
                _error(' INav4Mom::e():      %r', tp_ene)
                _error(' epsilon:            %r', eps)
                allgood=False
            pass
        if not allgood:
            return StatusCode.Failure
        _info('McAodSymLink tests OK')
        return StatusCode.Success

    def finalize(self):
        self.msg.info('==> finalize...')
        return StatusCode.Success

    # class PyMcAodSymLinkTests
