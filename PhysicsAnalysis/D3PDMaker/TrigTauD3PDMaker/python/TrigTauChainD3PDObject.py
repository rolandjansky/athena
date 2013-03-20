# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# D3PD object saving the tau trigger decision in the D3PD per RoI.
# Saves std::map< std::string, <std::vector<unsigned int> > chainsToRoIwords
# Per passed physics chain in the event roiwords saved
#

from D3PDMakerCoreComps.D3PDObject import make_Void_D3PDObject
import TrigTauD3PDMaker

TrigTauChainD3PDObject = make_Void_D3PDObject( "trigEvent_",
                                               "TrigTauChainFiller" )

TrigTauChainD3PDObject.defineBlock( 0, "mapChainsToRoIWords",
                                    TrigTauD3PDMaker.TrigTauChainFiller )
