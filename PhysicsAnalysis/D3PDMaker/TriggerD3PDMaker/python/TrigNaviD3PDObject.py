# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerConfig.D3PDMakerFlags           import D3PDMakerFlags
from TriggerD3PDMaker.TriggerD3PDMakerConf import D3PD__ChainEntryFillerTool,\
     D3PD__CombLinksFillerTool
from AthenaCommon.AlgSequence import AlgSequence

"""
These D3PDObjects are based on ChainEntryContainer and CombLinksContainer which
do not exist in AOD nor ESD. They must be recorded to StoreGate by running
the FlatHltNaviMaker. This algorithm requires a list of chains the user is
interested in and also needs to know the RoI types associated to each chain.
One needs to setup the following before adding the D3PDMaker to the TopAlg.

from AthenaCommon.AlgSequence import AlgSequence
from TriggerMenuAnalysis.TriggerMenuAnalysisConf import FlatHltNaviMaker
job = AlgSequence()
job += FlatHltNaviMaker()
include('TriggerMenuAnalysis/Setup_RoILinksCnvTool.py') # <--- Modify this

The above Setup_RoILinksCnvTool.py shows an example of how to provide the
information necessary to create the flat navigation structure.
"""

# Hook setting up the navigation slimming:
def __navSlimming( c, **kw ):
    from TriggerD3PDMaker.TrigNavSlimming import TrigNavSlimming
    preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())
    TrigNavSlimming (sequence = preseq)
    return

# ChainEntry
ChainEntryD3PDObject = make_SGDataVector_D3PDObject('ChainEntryContainer',
                                                    'ChainEntryContainer',
                                                    'trig_Nav_',
                                                    'ChainEntryD3PDObject')

ChainEntryD3PDObject.defineHook( __navSlimming )

ChainEntryD3PDObject.defineBlock(0, 'ChainEntry',
                                 D3PD__ChainEntryFillerTool,
                                 prefix='chain_')

# CombLinks
roi_types = { # (enums defined in ChainEntry::RoIType)
    # L2 RoI types
    1001: 'L2_mu', 
    1002: 'L2_e',
    1003: 'L2_tau',
    1004: 'L2_j',
    1005: 'L2_b',
    1006: 'L2_xe',
    1007: 'L2_DiMu',
    1010: 'L2_TileMu', 

    # EF RoI types
    2001: 'EF_mu',
    2002: 'EF_e',
    2003: 'EF_tau',
    2004: 'EF_j',
    2005: 'EF_b',
    2006: 'EF_xe',
    2007: 'EF_DiMu',
    2010: 'EF_TileMu', 
    }

CombLinksD3PDObjects = {}
for (type, name) in roi_types.iteritems():
    obj = make_SGDataVector_D3PDObject('CombLinksContainer', name, 'trig_RoI_'+name+'_',
                                       'CombLinksD3PDObject'+name)
    obj.defineBlock(1, 'CombLinksFillerTool_'+name,
                    D3PD__CombLinksFillerTool, 
                    prefix='',
                    RoIType=type)
    CombLinksD3PDObjects[name] = obj

