# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TrigEFBjetD3PDObject.py 364219 2011-05-06 13:45:02Z dferreir $
#
# This D3PDObject can be used to save information about the EF B-jet objects
#

from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation
import TrigBJetD3PDMaker
import EventCommonD3PDMaker

TrigEFBjetContainer_sgkey = 'HLT_EFBjetFex'

TrigEFBjetD3PDObject = make_SGDataVector_D3PDObject( 'TrigEFBjetContainer',
                                                     TrigEFBjetContainer_sgkey,
                                                     'trig_EF_bjet_',
                                                     'TrigEFbjetD3PDObject' )

TrigEFBjetD3PDObject.defineBlock( 0, 'Basics',
                                  TrigBJetD3PDMaker.TrigEFBjetFillerTool,
                                  WriteBasics = True )
TrigEFBjetD3PDObject.defineBlock( 0, 'FourMom',
                                  EventCommonD3PDMaker.FourMomFillerTool,
                                  WriteM = False )

TrigEFBjetD3PDObject.defineBlock( 1, 'HypoVars',
                                  TrigBJetD3PDMaker.TrigEFBjetFillerTool,
                                  WriteHypoVars = True )

TrigEFBjetD3PDObject.defineBlock( 1, 'Details',
                                  TrigBJetD3PDMaker.TrigEFBjetFillerTool,
                                  WriteDetails = True )

#
# I wanted to keep the ability to do the index association. But since this is not
# needed by default, it's put at a ridiculously high detail level.
#
EFTrackAssocObject = IndexMultiAssociation( TrigEFBjetD3PDObject,
                                            TrigBJetD3PDMaker.EFBjetToTrackAssociationTool,
                                            "trig_EF_bjetid_", prefix = "trk_", level = 11,
                                            blockname = "TrackAssoc",
                                            allowMissing = True );

from TriggerD3PDMaker.defineTriggerBits                import defineTriggerBits

defineTriggerBits(TrigEFBjetD3PDObject, 0, ["EF_b.*", "EF_2b.*", "EF_3b.*",
                                            "EF_mu4_j10_a4tc_EFFS", "EF_mu4_L1J10_matched",
                                            "EF_mu4_L1J20_matched",
                                            "EF_mu4_L1J30_matched", "EF_mu4_L1J50_matched",
                                            "EF_mu4_L1J75_matched", "EF_4j30_a4tc_EFFS",
                                            "EF_mu4_j100_a4tc_EFFS_L1matched"])

defineTriggerBits(TrigEFBjetD3PDObject, 0, ["L2_b.*", "L2_2b.*", "L2_3b.*",
                                            "L2_mu4_j10_a4tc_EFFS", "L2_mu4_L1J10_matched",
                                            "L2_mu4_L1J20_matched",
                                            "L2_mu4_L1J30_matched", "L2_mu4_L1J50_matched",
                                            "L2_mu4_L1J75_matched", "L2_4j30_a4tc_EFFS",
                                            "L2_mu4_j100_a4tc_EFFS_L1matched"])

