# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# -*- coding: utf-8 -*-
# $Id: TruthTauFullD3PDObject.py 504929 2012-06-11 15:00:43Z mtm $
#
# @file  TauD3PDMaker/python/PanTauTruthD3PDObject.py
# $author  Sebastian Fleischmann, Christian Limbach
# @date    Sep, 2010 (modified/extended May 2012)
# @brief   dump TruthTaus. All Truthtaus, not only the hadronic ones. Also include more detailed decay mode information


import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
import EventCommonD3PDMaker
import TauD3PDMaker


TruthTauFullD3PDObject = make_SGDataVector_D3PDObject( "TruthParticleContainer",
                                                   "TruthTaus",
                                                   "truthTau_",
                                                   "TruthTauFullD3PDObject" )

#-----------------------------------------------------------------------------
# Blocks
#-----------------------------------------------------------------------------

TruthTauFullD3PDObject.defineBlock (0, 'Kinematics',
                           EventCommonD3PDMaker.FourMomFillerTool,
                           WriteEt = False, # ?
                           WritePt = True )

TruthTauFullD3PDObject.defineBlock (0, 'Info',
                           TauD3PDMaker.TruthTauFullDPDFillerTool)

