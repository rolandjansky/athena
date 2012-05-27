# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: EFJetD3PDObject.py 497739 2012-04-25 18:29:21Z lbeemste $
#
# D3PD object that saves the information about EF jets.
#

from RecExConfig.RecFlags import rec
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags,_string_prop
import TrigJetD3PDMaker
import EventCommonD3PDMaker
import JetD3PDMaker

EFJetD3PDObject = make_SGDataVector_D3PDObject( 'JetCollection',
                                                'HLT_TrigJetRec', 
                                                'trig_EF_jet_',
                                                'EFJetD3PDObject' )

EFJetD3PDObject.defineBlock( 0, 'Kinematics',
                             EventCommonD3PDMaker.FourMomFillerTool,
                             WriteE  = True )

EFJetD3PDObject.defineBlock( 0, 'EMScale',
                             JetD3PDMaker.JetSignalStateFillerTool,
                             WriteE = True,
                             SignalStateNumber = 0,
                             SignalStatePrefix = 'emscale' )

EFJetD3PDObject.defineBlock( 0, 'ConstituentScale',
                             JetD3PDMaker.JetSignalStateFillerTool,
                             WriteE = True,
                             SignalStateNumber = 2,
                             SignalStatePrefix = 'constscale' )

EFJetD3PDObject.defineBlock( 0, 'JetAuthor', JetD3PDMaker.JetAuthorFillerTool, WriteAuthor = True, WriteCalibTags = True)

momlist = []

#DQ based moments
momlist += ['n90', 'Timing', 'LArQuality', 'HECQuality', 'NegativeE', 'fracSamplingMax', 'SamplingMax',
            'n90constituents', 'TileQuality', 'hecF','EMFraction','presamplerFraction','AverageLArQF']

#Calib based moments
momlist += ['EMJES', 'EMJES_EtaCorr'] 

#HI base moments
momlist += ['Discriminant', 'MeanTowerEtUnsubtr']


EFJetD3PDObject.defineBlock( 0, 'JetMoments', JetD3PDMaker.JetMomentFillerTool, Moments = momlist + ['TrigJetFeaturesUnpacker_Status'])


# Set up the object sources in a separate list
# V4 Menu naming convention
# cannot rely on this when chain name contains more than one algorithm tag
# use _calibtag branch (std::string) instead
EFJetObjectSourcesV4 = [ "EF_.*_a2hiem.*",     "a2hiem",
                       "EF_.*_a4hiem.*",     "a4hiem",
                       "EF_.*_a2hihad.*",    "a2hihad",
                       "EF_.*_a4hihad.*",    "a4hihad",
                       "EF_.*_a2tchad.*",    "a2tchad",
                       "EF_.*_a2tcem.*",     "a2tcem", 
                       "EF_.*_a4tcem.*",     "a4tcem",
                       "EF_.*_a4tchad.*",    "a4tchad",
                       "EF_.*_a4ttem.*",     "a4ttem",
                       "EF_.*_a4tthad.*",    "a4tthad",
                       "EF_.*_a10tcem.*",    "a10tcem",
                       "EF_.*_a2tclcw.*",    "a2tclcw",
                       "EF_.*_a4tclcw.*",    "a4tclcw",
                       "EF_.*_a10tclcw.*",   "a10tclcw",
                      ]

## This is for backwards compatibility, only meaningfull for v3 menus.
EFJetObjectSources = [ "EF_.*_a4tc_EFFS.*",   "a4tc",
                       "EF_.*_a10tc_EFFS.*",  "a10tc", 
                       "EF_.*_a2hi_EFFS.*",   "a2hi",
                     ]

# This block flags the EF jet objects based on which algorithm was
# used to reconstruct them. One has to find a set of unprescaled trigger chains
# that use one particular reconstruction algorithm, and pair them up with a
# flag name. Unfortunately this does make the configuration a bit trigger
# menu dependent...
EFJetD3PDObject.defineBlock( 2, "Source",
                             TrigJetD3PDMaker.JetObjectSourceFillerTool,
                             Sources = EFJetObjectSources )

EFJetD3PDObject.defineBlock( 2, 'JetRoIword',
                             JetD3PDMaker.JetROIWordFillerTool )


if not hasattr( D3PDMakerFlags, "JetEFTrigPattern" ):
    _string_prop( "JetEFTrigPattern", "EF_[2-9]?f?j[0-9]*.*" )
EFJetD3PDObject.defineBlock( 99, 'Triggers',
                             TrigJetD3PDMaker.JetObjectDecisionFillerTool,
                             AllowExtend = D3PDMakerFlags.AllowTrigExtend(),
                             Chains = [ D3PDMakerFlags.JetEFTrigPattern() ] )
