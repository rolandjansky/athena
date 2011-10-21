# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from math import log10
from PyAthenaTests.Lib import _patch
from AthenaPython import PyAthena
StatusCode = PyAthena.StatusCode

def new_execute(self):
    """a new execute method, making use of new data members"""
    ### the old part #########################################
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
    ######################## NEW PART ########################
        if ene < self._ene_cut:
            # skip that cluster
            continue
        _clusE(ene, self._ene_weight)
    ######################## go one with OLD part ############
        _clusEta(cluster.eta())

        if i%100 != 0:
            continue

        _info('cluster #%3i has energy %8.3f',i, ene)
        for m in _patch.CaloClusterMoments():
            moment = RDouble(0.)
            if cluster.retrieveMoment(m, moment, useLink=False):
                _info('cluster #%3i has moment #%3i with value %8.3f',
                      i, m, moment)
        pass # loop over clusters

    return StatusCode.Success
        
