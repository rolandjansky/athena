# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: EmTauROID3PDObject.py 500205 2012-05-11 10:17:25Z krasznaa $
#
# D3PD object saving the LVL1 Em/Tau RoI information into the D3PD
#

from D3PDMakerCoreComps.D3PDObject  import make_SGDataVector_D3PDObject
import TriggerD3PDMaker
import EventCommonD3PDMaker

EmTauROID3PDObject = \
     make_SGDataVector_D3PDObject( "LVL1_ROI", "LVL1_ROI",
                                   "trig_L1_emtau_", "EmTauROID3PDObject",
                                   default_getterClass = \
                                   TriggerD3PDMaker.EmTau_ROIGetterTool )

EmTauROID3PDObject.defineBlock( 0, "Kinematics",
                                EventCommonD3PDMaker.FourMomFillerTool,
                                WriteM = False, WritePt = False )
EmTauROID3PDObject.defineBlock( 0, "ThrInfo",
                                TriggerD3PDMaker.EmTau_ROIFillerTool,
                                WriteThrInfo = True )

EmTauROID3PDObject.defineBlock( 1, "DecodedInfo",
                                TriggerD3PDMaker.EmTau_ROIFillerTool,
                                WriteDecodedInfo = True )

EmTauROID3PDObject.defineBlock( 2, "RoIWord",
                                TriggerD3PDMaker.EmTau_ROIFillerTool,
                                WriteRoIWord = True )
