# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TrigL2BjetD3PDObject.py 348188 2011-02-28 13:06:21Z krasznaa $
#
# This D3PDObject can be used to save information about the LVL2 B-jet objects
#

from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation
import TrigBJetD3PDMaker
import EventCommonD3PDMaker

TrigL2BjetContainer_sgkey = 'HLT_L2BjetFex'

TrigL2BjetD3PDObject = make_SGDataVector_D3PDObject( 'TrigL2BjetContainer',
                                                     TrigL2BjetContainer_sgkey,
                                                     'trig_L2_bjet_',
                                                     'TrigL2BjetD3PDObject' )

TrigL2BjetD3PDObject.defineBlock( 0, 'Basics',
                                  TrigBJetD3PDMaker.TrigL2BjetFillerTool,
                                  WriteBasics = True )
TrigL2BjetD3PDObject.defineBlock( 0, 'FourMom',
                                  EventCommonD3PDMaker.FourMomFillerTool,
                                  WriteM = False )

TrigL2BjetD3PDObject.defineBlock( 1, 'HypoVars',
                                  TrigBJetD3PDMaker.TrigL2BjetFillerTool,
                                  WriteHypoVars = True )

TrigL2BjetD3PDObject.defineBlock( 1, 'Details',
                                  TrigBJetD3PDMaker.TrigL2BjetFillerTool,
                                  WriteDetails = True )

#
# I wanted to keep the ability to do the index association. But since this is not
# needed by default, it's put at a ridiculously high detail level.
#
L2TrackAssocSiObject = IndexMultiAssociation( TrigL2BjetD3PDObject,
                                              TrigBJetD3PDMaker.L2BjetToTrackAssociationTool,
                                              "trig_L2_sitrk_jet_", prefix = "trksi_",
                                              level = 11,
                                              blockname = "SiTrackAssoc",
                                              allowMissing = True )
L2TrackAssocIdObject = IndexMultiAssociation( TrigL2BjetD3PDObject,
                                              TrigBJetD3PDMaker.L2BjetToTrackAssociationTool,
                                              "trig_L2_idtrk_jet_", prefix = "trkid_",
                                              level = 11,
                                              blockname = "IDTrackAssoc",
                                              allowMissing = True )
