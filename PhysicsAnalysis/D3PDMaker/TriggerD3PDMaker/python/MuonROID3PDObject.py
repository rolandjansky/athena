# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: MuonROID3PDObject.py 620244 2014-10-06 19:02:48Z ssnyder $
#
# D3PD object saving the LVL1 muon RoI information into the D3PD
#

from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
import TriggerD3PDMaker
import EventCommonD3PDMaker
import D3PDMakerCoreComps

MuonROID3PDObject = \
     make_SGDataVector_D3PDObject( "xAOD::MuonRoIContainer_v1",
                                   "LVL1MuonRoIs",
                                   "trig_L1_mu_", "MuonROID3PDObject")

MuonROID3PDObject.defineBlock (
    0, 'Kinematics',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['eta', 'phi'])
MuonROID3PDObject.defineBlock (
    0, 'ThrInfo',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['thrName', 'thrValue'])
MuonROID3PDObject.defineBlock (
    2, 'RoIWord',
    D3PDMakerCoreComps.AuxDataFillerTool,
    Vars = ['RoIWord = roiWord'])

MuonROID3PDObject.defineBlock( 1, "DecodedInfo",
                               TriggerD3PDMaker.Muon_ROIFillerTool )

