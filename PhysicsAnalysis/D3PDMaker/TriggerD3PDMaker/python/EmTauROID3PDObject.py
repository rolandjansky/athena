# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# D3PD object saving the LVL1 Em/Tau RoI information into the D3PD
#

from D3PDMakerCoreComps.D3PDObject  import make_SGDataVector_D3PDObject
import TriggerD3PDMaker
import D3PDMakerCoreComps


EmTauROID3PDObject = \
     make_SGDataVector_D3PDObject( 'xAOD::EmTauRoIContainer',
                                   "HLT_xAOD__EmTauRoIContainer_L1TopoEM,LVL1EmTauRoIs",
                                   "trig_L1_emtau_", "EmTauROID3PDObject")

EmTauROID3PDObject.defineBlock (
    0, 'Kinematics',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['eta', 'phi'])

EmTauROID3PDObject.defineBlock (
    0, 'ThrInfo',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['thrNames', 'thrValues'])

EmTauROID3PDObject.defineBlock (
    1, 'DecodedInfo',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['core',
            'EMClus = emClus',
            'tauClus',
            'EMIsol = emIsol',
            'hadIsol',
            'hadCore',
            ])

EmTauROID3PDObject.defineBlock (
    1, 'DecodedPatternInfo',
    TriggerD3PDMaker.EmTau_ROIthrPatternFillerTool)

EmTauROID3PDObject.defineBlock (
    2, 'RoIWord',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['RoIWord = roiWord'])
