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
    Vars = ['quality = clusterQuality'])
TrigEMClusterD3PDObject.defineBlock (
    0, 'Basics1',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['E = energy', 'Et = et', 'eta', 'phi'])

# Level 1
TrigEMClusterD3PDObject.defineBlock (
    1, 'HypoVars1',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['E237 = e237',
            'E277 = e277',
            'fracs1',
            'weta2',
            'Ehad1 = ehad1',
            'eta1',
            'emaxs1',
            'e2tsts1',
            'E233 = e233',
            'wstot'])

# Level 2
TrigEMClusterD3PDObject.defineBlock (
    2, 'Details',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['rawEnergy', 'rawEta', 'rawPhi', 'RoIWord = RoIword', 'nCells'])
TrigEMClusterD3PDObject.defineBlock (
    2, 'Details1',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['energyInSample = energySample'])
