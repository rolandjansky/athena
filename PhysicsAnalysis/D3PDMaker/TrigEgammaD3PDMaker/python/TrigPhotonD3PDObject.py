# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TrigPhotonD3PDObject.py 466844 2011-11-07 23:33:05Z ssnyder $
#
# D3PD object saving information about the LVL2 TrigPhoton objects
#

from D3PDMakerCoreComps.D3PDObject  import make_SGDataVector_D3PDObject
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags

import EventCommonD3PDMaker
import TrigEgammaD3PDMaker

TrigPhotonContainer_sgkey = 'HLT_L2PhotonFex'
TrigPhotonD3PDObject = make_SGDataVector_D3PDObject('TrigPhotonContainer',
                                                    TrigPhotonContainer_sgkey,
                                                    'trig_L2_ph_',
                                                    'TrigPhotonD3PDObject')

# Level 0
TrigPhotonD3PDObject.defineBlock(0, 'Kinematics',
                                 EventCommonD3PDMaker.FourMomFillerTool,
                                 WriteE  = True,
                                 WriteEt = True,
                                 WriteM  = False)
TrigPhotonD3PDObject.defineBlock(0, 'Basics',
                                 TrigEgammaD3PDMaker.TrigPhotonFillerTool,
                                 WriteBasics=True)

# Level 1
TrigPhotonD3PDObject.defineBlock(1, 'HypoVars',
                                 TrigEgammaD3PDMaker.TrigPhotonFillerTool,
                                 WriteHypoVars=True)

TrigPhotonD3PDObject.defineBlock \
       (1, 'Triggers',
        TrigEgammaD3PDMaker.TrigPhotonObjectDecisionFillerTool,
        AllowExtend = D3PDMakerFlags.AllowTrigExtend(),
        Chains = [ D3PDMakerFlags.PhotonL2TrigPattern() ])

# Level 2
TrigPhotonD3PDObject.defineBlock(2, 'Details',
                                 TrigEgammaD3PDMaker.TrigPhotonFillerTool,
                                 WriteDetails=True)

