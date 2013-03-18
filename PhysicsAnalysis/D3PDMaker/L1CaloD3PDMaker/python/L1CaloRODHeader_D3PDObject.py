# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# D3PD object saving L1Calo ROD Headers
# @author John Morris <john.morris@cern.ch>
#

from D3PDMakerCoreComps.D3PDObject  import make_SGDataVector_D3PDObject
import EventCommonD3PDMaker
import L1CaloD3PDMaker

L1CaloRODHeader_D3PDObject = make_SGDataVector_D3PDObject('DataVector<LVL1::RODHeader>',
                                                         'RODHeaders',
                                                         'L1CaloROD_RODHeader_',
                                                         'L1CaloRODHeaderD3PDObject')

#Level 0
L1CaloRODHeader_D3PDObject.defineBlock(0,'RODHeader',
                                      L1CaloD3PDMaker.L1CaloRODHeaderFillerTool)
