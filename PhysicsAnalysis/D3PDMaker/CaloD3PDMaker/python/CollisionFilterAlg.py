# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file CaloD3PDMaker/python/CollisionFilterAlg.py
# @author scott snyder <snyder@bnl.gov>
# @date Mar, 2010
# @brief An algorithm to filter based on collcand.
#


from CaloD3PDMaker.CollisionDecisionGetter import CollisionDecisionGetter
from AthenaPython                          import PyAthena
from AthenaPython.PyAthena                 import StatusCode
from AthenaCommon.AlgSequence              import AlgSequence
from D3PDMakerConfig.D3PDMakerFlags        import D3PDMakerFlags


class CollisionFilterAlg (PyAthena.Alg):
    def __init__ (self,
                  name = 'D3PDCollisionFilterAlg',
                  **kw):
        super (CollisionFilterAlg, self).__init__ (name = name, **kw)
        preseq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName())
        CollisionDecisionGetter (preseq)
        self.SkimDecisionsContainerName = 'DESD_COLLCAND_SkimDecisionsContainer'
        self.SkimDecisionsFilterNames = [
            'Accept_CaloTimeFilterInCollisionStream',
            'Accept_MBTSTimeFilterInCollisionStream',
            'Accept_TriggerFilterInCollissionStream',
            ]
                                         
        return


    def initialize (self):
        self.sg = PyAthena.py_svc( "StoreGateSvc" )
        return StatusCode.Success


    def execute (self):
        sdc = self.sg.retrieve( "SkimDecisionCollection",
                                self.SkimDecisionsContainerName )
        passed = False
        if sdc == None:
            self.msg.error ("Can't find SkimDecisionCollection " +
                            self.SkimDecisionsContainerName)
        else:
            for sd in sdc:
                if (sd.getName() in self.SkimDecisionsFilterNames and
                    sd.isAccepted()):
                    passed = True
                    break

        self.setFilterPassed (passed)

        return StatusCode.Success
