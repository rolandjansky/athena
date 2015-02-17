# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
import D3PDMakerCoreComps

TrigEMClusterContainer_sgkey = 'HLT_TrigT2CaloEgamma,HLT_xAOD__TrigEMClusterContainer_TrigT2CaloEgamma'
TrigEMClusterD3PDObject = make_SGDataVector_D3PDObject('DataVector<xAOD::TrigEMCluster_v1>',
                                                       TrigEMClusterContainer_sgkey,
                                                       'trig_L2_emcl_',
                                                       'TrigEMClusterD3PDObject')

# Level 0
TrigEMClusterD3PDObject.defineBlock (
    0, 'Basics',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['quality = clusterQuality < unsigned int: 0'])
TrigEMClusterD3PDObject.defineBlock (
    0, 'Basics1',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['E = energy < float: 0',
            'Et = et < float:0',
            'eta < float:0',
            'phi < float:0'])

# Level 1
TrigEMClusterD3PDObject.defineBlock (
    1, 'HypoVars1',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['E237 = e237 < float:0',
            'E277 = e277 < float:0',
            'fracs1 < float:0',
            'weta2 < float:0',
            'Ehad1 = ehad1 < float:0',
            'eta1 < float:0',
            'emaxs1 < float:0',
            'e2tsts1 < float:0',
            'E233 = e233 < float:0',
            'wstot < float:0'])

# Level 2
TrigEMClusterD3PDObject.defineBlock (
    2, 'Details',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['rawEnergy < float:0',
            'rawEta < float:0',
            'rawPhi < float:0',
            'RoIWord = RoIword < long: 0',
            'nCells < int: 0'])
#TrigEMClusterD3PDObject.defineBlock (
#    2, 'Details1',
#    D3PDMakerCoreComps.AuxDataFillerTool,
#    Vars = ['energyInSample = energySample'])
