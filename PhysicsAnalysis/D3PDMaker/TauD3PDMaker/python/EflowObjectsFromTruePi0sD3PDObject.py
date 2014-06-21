# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# -*- coding: utf-8 -*-
# $Id: EflowObjectsFromTruePi0sD3PDObject.py 529155 2012-12-06 12:15:53Z mtm $
#
# @file  TauD3PDMaker/python/EflowObjectsD3PDObject.py
# $author  Christian Limbach
# @date    Sep, 2012
# @brief   dump 4 vector of eFlow objects matched to true pions


import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import make_SGDataVector_D3PDObject
import EventCommonD3PDMaker
import TauD3PDMaker


EflowObjectsFromTruePi0sD3PDObject = make_SGDataVector_D3PDObject( "eflowObjectContainer",
                                                   "EFOsMatchedToNeutralPions",
                                                   "efoFromTruePi0s_",
                                                   "EflowObjectsD3PDObject" )

#-----------------------------------------------------------------------------
# Blocks
#-----------------------------------------------------------------------------

EflowObjectsFromTruePi0sD3PDObject.defineBlock (0, 'Kinematics',
                           EventCommonD3PDMaker.FourMomFillerTool,
                           WriteEt = True, # ?
                           WritePt = True )


