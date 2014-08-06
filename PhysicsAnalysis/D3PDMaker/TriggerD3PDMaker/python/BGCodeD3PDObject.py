# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: BGCodeD3PDObject.py 338058 2010-12-20 13:20:43Z krasznaa $
#
# D3PD object saving the BunchGroup code from the TDT into the D3PD
#

from D3PDMakerCoreComps.D3PDObject import make_Void_D3PDObject
import TriggerD3PDMaker

BGCodeD3PDObject = make_Void_D3PDObject( "trig_", "BGCodeD3PDObject",
                                         default_name = "BGCodeFiller" )

BGCodeD3PDObject.defineBlock( 0, "BGCode",
                              TriggerD3PDMaker.TrigDecisionFillerTool,
                              SaveBGCode = True )
