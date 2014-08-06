# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: JetROID3PDObject.py 500205 2012-05-11 10:17:25Z krasznaa $
#
# D3PD object saving the LVL1 jet RoI information into the D3PD
#

from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
import TriggerD3PDMaker
import EventCommonD3PDMaker

JetROID3PDObject = \
     make_SGDataVector_D3PDObject( "LVL1_ROI", "LVL1_ROI",
                                   "trig_L1_jet_", "JetROID3PDObject",
                                   default_getterClass = \
                                   TriggerD3PDMaker.Jet_ROIGetterTool )

JetROID3PDObject.defineBlock( 0, "Kinematics",
                              EventCommonD3PDMaker.FourMomFillerTool,
                              WriteM = False, WritePt = False )
JetROID3PDObject.defineBlock( 0, "ThrInfo",
                              TriggerD3PDMaker.Jet_ROIFillerTool,
                              WriteThrInfo = True )

JetROID3PDObject.defineBlock( 1, "DecodedInfo",
                              TriggerD3PDMaker.Jet_ROIFillerTool,
                              WriteDecodedInfo = True,
                              WriteRoIWord = True )
