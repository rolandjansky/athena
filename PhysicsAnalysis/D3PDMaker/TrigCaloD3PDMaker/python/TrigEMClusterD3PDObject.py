# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from TrigCaloD3PDMaker.TrigCaloD3PDMakerConf \
     import D3PD__TrigEMClusterFillerTool, D3PD__TrigCaloClusterFillerTool

TrigEMClusterContainer_sgkey = 'HLT_TrigT2CaloEgamma'
TrigEMClusterD3PDObject = make_SGDataVector_D3PDObject('TrigEMClusterContainer',
                                                       TrigEMClusterContainer_sgkey,
                                                       'trig_L2_emcl_',
                                                       'TrigEMClusterD3PDObject')

# Level 0
TrigEMClusterD3PDObject.defineBlock(0, 'Basics',
                                    D3PD__TrigCaloClusterFillerTool,
                                    WriteBasics=True)
TrigEMClusterD3PDObject.defineBlock(0, 'Basics1',
                                    D3PD__TrigEMClusterFillerTool,
                                    WriteBasics=True)

# Level 1
TrigEMClusterD3PDObject.defineBlock(1, 'HypoVars', 
                                    D3PD__TrigCaloClusterFillerTool,
                                    WriteHypoVars=True)

TrigEMClusterD3PDObject.defineBlock(1, 'HypoVars1', 
                                    D3PD__TrigEMClusterFillerTool,
                                    WriteHypoVars=True)

# Level 2
TrigEMClusterD3PDObject.defineBlock(2, 'Details', 
                                    D3PD__TrigCaloClusterFillerTool,
                                    WriteDetails=True)

TrigEMClusterD3PDObject.defineBlock(2, 'Details1', 
                                    D3PD__TrigEMClusterFillerTool,
                                    WriteDetails=True)
