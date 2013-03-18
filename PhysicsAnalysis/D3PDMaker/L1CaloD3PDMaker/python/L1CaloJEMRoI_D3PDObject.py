# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# D3PD object saving L1Calo JEM RoI
# @author John Morris <john.morris@cern.ch>
#

from D3PDMakerCoreComps.D3PDObject  import make_SGDataVector_D3PDObject
import EventCommonD3PDMaker
import L1CaloD3PDMaker

L1CaloJEMRoI_D3PDObject = make_SGDataVector_D3PDObject('DataVector<LVL1::JEMRoI>',
                                                       'JEMRoIs',
                                                       'L1CaloJEM_RoI_',
                                                       'L1CaloJEMRoI_D3PDObject')

#Level 0
L1CaloJEMRoI_D3PDObject.defineBlock(0,'JEMRoI',
                                     L1CaloD3PDMaker.L1CaloJEMRoIFillerTool)
