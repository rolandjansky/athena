# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TrigElectronD3PDObject.py 466844 2011-11-07 23:33:05Z ssnyder $
#
# D3PD object saving information about the LVL2 TrigElectron objects
#

from D3PDMakerCoreComps.D3PDObject  import make_SGDataVector_D3PDObject
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
import EventCommonD3PDMaker
import TrigEgammaD3PDMaker

TrigElectronContainer_sgkey = 'HLT_L2ElectronFex,HLT_L2IDCaloFex'
TrigElectronD3PDObject = make_SGDataVector_D3PDObject('TrigElectronContainer',
                                                      TrigElectronContainer_sgkey,
                                                      'trig_L2_el_',
                                                      'TrigElectronD3PDObject')

# Level 0
TrigElectronD3PDObject.defineBlock(0, 'Kinematics',
                                   EventCommonD3PDMaker.FourMomFillerTool,
                                   WriteE  = True,
                                   WriteEt = True,
                                   WriteM  = False)
TrigElectronD3PDObject.defineBlock(0, 'Basics',
                                   TrigEgammaD3PDMaker.TrigElectronFillerTool,
                                   WriteBasics=True)

# Level 1
TrigElectronD3PDObject.defineBlock(1, 'HypoVars',
                                   TrigEgammaD3PDMaker.TrigElectronFillerTool,
                                   WriteHypoVars=True)

TrigElectronD3PDObject.defineBlock \
       (1, 'Triggers',
        TrigEgammaD3PDMaker.TrigElectronObjectDecisionFillerTool,
        AllowExtend = D3PDMakerFlags.AllowTrigExtend(),
        Chains = [ D3PDMakerFlags.ElectronL2TrigPattern() ])

# Level 2
TrigElectronD3PDObject.defineBlock(2, 'Details',
                                   TrigEgammaD3PDMaker.TrigElectronFillerTool,
                                   WriteDetails=True)

