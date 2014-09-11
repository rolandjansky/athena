# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file:    JetMomentTools/python/JetCopyMomentsAlg.py.py
# @purpose: <Copy some moments from a collection to another >
# @author:  P-A Delsart <delsart in2p3 fr>

__doc__     = 'some documentation here'
__version__ = '$Revision: 1.5 $'
__author__  = 'Sebastien Binet <binet@cern.ch>'

import AthenaCommon.SystemOfUnits as Units
import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode
import ROOT

class JetCopyMomentsAlg (PyAthena.Alg):
    'put some documentation here'
    def __init__(self, name='JetCopyMomentsAlg', **kw):
        ## init base class
        kw['name'] = name
        super(JetCopyMomentsAlg, self).__init__(**kw)

        ## properties and data members
        self.Moments = kw.get('Moments', []) # default value
        self.JetCollectionSrc = kw.get('JetCollectionSrc', "") # default value
        self.JetCollectionDest = kw.get('JetCollectionDest', "") # default value
        return

    def initialize(self):
        self.msg.info('==> initialize...')
        self.sg = PyAthena.py_svc ('StoreGateSvc')
        return StatusCode.Success

    def execute(self):
        jcoll_src = list(self.sg.retrieve('JetCollection', self.JetCollectionSrc))
        jcoll_dest = list(self.sg.retrieve('JetCollection', self.JetCollectionDest))

        # sort on constit scale to help with ordering.
        jcoll_dest.sort(key= lambda x: x.pt(2) )
        jcoll_src.sort(key= lambda x: x.pt(2) ) 
        getMoment = ROOT.Jet.getMoment
        moments = self.Moments
        
        dr = ROOT.JetDistances.deltaR
        needUnorderedMatch = False
        for i, (jS, jD) in enumerate(zip(jcoll_src, jcoll_dest)):
            if dr(jS,jD)>0.05:
                needUnorderedMatch = True
                break 
            for m in moments:
                jD.setMoment(m,jS.getMoment(m))

        if needUnorderedMatch :
            # this may happen because of final Pt cut and a calibration changing order w.r.t lcscale.
            # fall back to a double loop with deltaR check on remaining jets
            jcoll_src = jcoll_src[i:]
            jcoll_dest = jcoll_dest[i:]
            for jS in jcoll_src:
                for jD in jcoll_dest:
                    if dr(jS,jD)<0.05:
                        for m in moments:
                            jD.setMoment(m,jS.getMoment(m))
            

        
        return StatusCode.Success

    def finalize(self):
        self.msg.info('==> finalize...')
        return StatusCode.Success

    # class JetCopyMomentsAlg
