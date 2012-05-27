# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TrigJetD3PDObject.py 488065 2012-03-12 17:10:31Z chapleau $
#
# D3PD object saving information about the LVL2 TrigJet objects
#

from D3PDMakerCoreComps.D3PDObject  import make_SGDataVector_D3PDObject
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags,_string_prop
import EventCommonD3PDMaker
import TrigJetD3PDMaker

TrigJetContainer_sgkey = 'HLT_TrigT2CaloJet'
TrigJetD3PDObject = make_SGDataVector_D3PDObject( 'TrigT2JetContainer',
                                                  TrigJetContainer_sgkey,
                                                  'trig_L2_jet_',
                                                  'TrigJetD3PDObject' )

# Level 0
TrigJetD3PDObject.defineBlock( 0, 'Kinematics',
                               EventCommonD3PDMaker.FourMomFillerTool,
                               WriteE  = True,
                               WritePt = False,
                               WriteM  = False )
TrigJetD3PDObject.defineBlock( 0, 'Basics',
                               TrigJetD3PDMaker.TrigT2JetFillerTool,
                               WriteBasics = True )

# Set up the object sources in a separate list
# V4 Menu naming convention. Regular Cone jets only
L2JetObjectSources = [ "L2_.*_c4cchad.*",    "c4cchad",
                       "L2_.*_c4ccem.*",     "c4ccem",
                       "L2_.*_c4uchad.*",    "c4uchad",
                      ]


# This block flags the L2 jet objects based on which algorithm was
# used to reconstruct them. One has to find a set of unprescaled trigger chains
# that use one particular reconstruction algorithm, and pair them up with a
# flag name. Unfortunately this does make the configuration a bit trigger
# menu dependent...
TrigJetD3PDObject.defineBlock( 0, 'Source',
                              TrigJetD3PDMaker.TrigT2JetObjectSourceFillerTool,
                              Sources = L2JetObjectSources )


# Level 1
TrigJetD3PDObject.defineBlock( 1, 'Details',
                               TrigJetD3PDMaker.TrigT2JetFillerTool,
                               WriteDetails = True )

# Level 2
TrigJetD3PDObject.defineBlock( 1, 'Cleaning',
                               TrigJetD3PDMaker.TrigT2JetFillerTool,
                               WriteCleaning = True )


if not hasattr( D3PDMakerFlags, "JetL2TrigPattern" ):
    _string_prop( "JetL2TrigPattern", "L2_[2-9]?f?j[0-9]*.*" )
TrigJetD3PDObject.defineBlock( 99, "Decision",
                               TrigJetD3PDMaker.TrigT2JetObjectDecisionFillerTool,
                               AllowExtend = D3PDMakerFlags.AllowTrigExtend(),
                               Chains = [ D3PDMakerFlags.JetL2TrigPattern() ] )
