# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: MuonROID3PDObject.py 500205 2012-05-11 10:17:25Z krasznaa $
#
# D3PD object saving the LVL1 muon RoI information into the D3PD
#

from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
import TriggerD3PDMaker
import EventCommonD3PDMaker

MuonROID3PDObject = \
     make_SGDataVector_D3PDObject( "LVL1_ROI", "LVL1_ROI",
                                   "trig_L1_mu_", "MuonROID3PDObject",
                                   default_getterClass = \
                                   TriggerD3PDMaker.Muon_ROIGetterTool )

MuonROID3PDObject.defineBlock( 0, "Kinematics",
                               EventCommonD3PDMaker.FourMomFillerTool,
                               WriteM = False )
MuonROID3PDObject.defineBlock( 0, "ThrInfo",
                               TriggerD3PDMaker.Muon_ROIFillerTool,
                               WriteThrInfo = True )

MuonROID3PDObject.defineBlock( 1, "DecodedInfo",
                               TriggerD3PDMaker.Muon_ROIFillerTool,
                               WriteDecodedInfo = True )

MuonROID3PDObject.defineBlock( 2, "RoIWord",
                               TriggerD3PDMaker.Muon_ROIFillerTool,
                               WriteRoIWord = True )
