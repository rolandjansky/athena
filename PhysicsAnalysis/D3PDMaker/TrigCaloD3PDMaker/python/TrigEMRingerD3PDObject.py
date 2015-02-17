# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from TrigCaloD3PDMaker.TrigCaloD3PDMakerConf import D3PD__TrigEMRingerFillerTool

TrigEMClusterContainer_sgkey = 'HLT_TrigT2CaloEgamma'
TrigEMRingerD3PDObject = make_SGDataVector_D3PDObject('TrigEMClusterContainer',
                                                       TrigEMClusterContainer_sgkey,
                                                       'trig_L2_emcl_ringer_',
                                                       'TrigEMRingerD3PDObject')

# Level 0
TrigEMRingerD3PDObject.defineBlock(0, 'Basics',
                                    D3PD__TrigEMRingerFillerTool,
                                    WriteBasics=True)

# Level 1
TrigEMRingerD3PDObject.defineBlock(1, 'HypoVars', 
                                    D3PD__TrigEMRingerFillerTool,
                                    WriteHypoVars=True)

# Level 2
TrigEMRingerD3PDObject.defineBlock(2, 'Details', 
                                    D3PD__TrigEMRingerFillerTool,
                                    WriteDetails=True)
