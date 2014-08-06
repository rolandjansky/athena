# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TrigDecisionD3PDObject.py 451855 2011-08-03 09:28:07Z krasznaa $
#
# D3PD object saving the raw trigger decision in the D3PD.
# Note, that you need to know the trigger configuration to make sense
# of this information. Your best bet is to save the configuration as
# meta-data using TrigConfMetadataTool.
#

from D3PDMakerCoreComps.D3PDObject import make_Void_D3PDObject
import TriggerD3PDMaker

TrigDecisionD3PDObject = make_Void_D3PDObject( "trig_", "TrigDecisionD3PDObject",
                                               default_name = "TrigDecisionFiller" )

TrigDecisionD3PDObject.defineBlock( 0, "PhysicsBits",
                                    TriggerD3PDMaker.TrigDecisionFillerTool,
                                    SaveLVL1Physics = True, SaveHLTPhysics = True )

TrigDecisionD3PDObject.defineBlock( 1, "RawBits",
                                    TriggerD3PDMaker.TrigDecisionFillerTool,
                                    SaveLVL1Raw = True, SaveHLTRaw = True,
                                    SaveTruncated = True )

TrigDecisionD3PDObject.defineBlock( 2, "HLTDetails",
                                    TriggerD3PDMaker.TrigDecisionFillerTool,
                                    SaveHLTResurrected = True, SaveHLTPassThrough = True,
                                    SaveHLTChainState = True )
