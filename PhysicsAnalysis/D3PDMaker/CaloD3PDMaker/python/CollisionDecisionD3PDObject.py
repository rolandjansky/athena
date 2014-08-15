# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file CaloD3PDMaker/python/CollisionDecisionD3PDObject.py
# @author scott snyder <snyder@bnl.gov>
# @date Mar, 2010
# @brief D3PD object for collision decision flags.
#


import CaloD3PDMaker
import EventCommonD3PDMaker
from CaloD3PDMaker.CollisionDecisionGetter import CollisionDecisionGetter
from D3PDMakerConfig.D3PDMakerFlags        import D3PDMakerFlags
from D3PDMakerCoreComps.D3PDObject         import make_SG_D3PDObject
from AthenaCommon.AlgSequence              import AlgSequence


CollisionDecisionD3PDObject = \
           make_SG_D3PDObject ('SkimDecisionCollection',
                               'DESD_COLLCAND_SkimDecisionsContainer',
                               'collcand_', 'CollisionDecisionD3PDObject')

def _collDecisionAlgHook (c,
                          seq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName()),
                          *args, **kw):
    CollisionDecisionGetter (seq)
    return
CollisionDecisionD3PDObject.defineHook (_collDecisionAlgHook)


CollisionDecisionD3PDObject.defineBlock \
              (0, 'CollisionDecision',
               EventCommonD3PDMaker.SkimDecisionFillerTool,
               Flags = ["Accept_CaloTimeFilterInCollisionStream:passCaloTime:"
                        "True if the calorimeter time requirement has passed.",

                        "Accept_MBTSTimeFilterInCollisionStream:passMBTSTime:"
                        "True if the MBTS time requirement has passed.",

                        "Accept_TriggerFilterInCollissionStream:passTrigger:"
                        "True if the trigger requirement has passed.",
                        ],
               AnyPassedName = "pass",
                   )
