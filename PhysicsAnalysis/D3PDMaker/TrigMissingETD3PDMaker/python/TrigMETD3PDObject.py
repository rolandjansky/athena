# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TrigMETD3PDObject.py 338101 2010-12-20 14:57:28Z krasznaa $
#
# D3PD object saving information about the HLT missing-Et reconstruction.
# Picks up the TrigMissingET objects directly from StoreGate. Default
# parameters only save the information about the default LVL2 MET
# reconstruction. The user should configure it specifically in the job
# itself.
#

from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerCoreComps.D3PDObject import make_SG_D3PDObject
import TrigMissingETD3PDMaker

TrigMETD3PDObject = make_SG_D3PDObject( "TrigMissingETContainer",
                                        "HLT_T2MissingET",
                                        "trig_L2_met_",
                                        "TrigMETD3PDObject" )

TrigMETD3PDObject.defineBlock( 0, "BasicInfo",
                               TrigMissingETD3PDMaker.TrigMissingETBaseFillerTool )

TrigMETD3PDObject.defineBlock( 0, "ComponentInfo",
                               TrigMissingETD3PDMaker.TrigMissingETComponentFillerTool )
