# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TrigDBKeysD3PDObject.py 338058 2010-12-20 13:20:43Z krasznaa $
#
# D3PD object saving the trigger configuration DB keys that were used to record
# the event. To be used in conjunction with saving the trigger configuration metadata.
#

from D3PDMakerCoreComps.D3PDObject import make_Void_D3PDObject
import TriggerD3PDMaker

TrigDBKeysD3PDObject = make_Void_D3PDObject( "trig_DB_", "TrigDBKeysD3PDObject",
                                             default_name = "TrigDBKeysFiller" )

TrigDBKeysD3PDObject.defineBlock( 0, "DBKeys",
                                  TriggerD3PDMaker.TrigDBKeysFillerTool )
