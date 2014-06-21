# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file  TauD3PDMaker/python/TruthTauD3PDObject.py
# $author  Ryan Reece <ryan.reece@cern.ch>
# @date    Dec, 2009
# @brief   dump true hadronic taus


import TauD3PDMaker
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
import EventCommonD3PDMaker


TruthTauD3PDObject = make_SGDataVector_D3PDObject( "TruthParticleContainer",
                                                   "TrueHadronicTaus",
                                                   "trueTau_",
                                                   "TruthTauD3PDObject" )


#-----------------------------------------------------------------------------
# Blocks
#-----------------------------------------------------------------------------
TruthTauD3PDObject.defineBlock (0, 'Kinematics',
                           EventCommonD3PDMaker.FourMomFillerTool,
                           WriteEt = False, # ?
                           WritePt = True )

TruthTauD3PDObject.defineBlock (0, 'Info',
                           TauD3PDMaker.TruthTauFillerTool)


