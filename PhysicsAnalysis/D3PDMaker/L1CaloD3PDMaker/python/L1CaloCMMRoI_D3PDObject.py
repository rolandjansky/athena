# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# D3PD object saving L1Calo CMM RoIs
# @author John Morris <john.morris@cern.ch>
#

from D3PDMakerCoreComps.D3PDObject  import make_SG_D3PDObject
import EventCommonD3PDMaker
import L1CaloD3PDMaker

L1CaloCMMRoI_D3PDObject = make_SG_D3PDObject('LVL1::CMMRoI',
                                             'CMMRoIs',
                                             'L1CaloCMM_RoIs_',
                                             'L1CaloCMMRoI_D3PDObject')

#Level 0
L1CaloCMMRoI_D3PDObject.defineBlock(0,'CMMRoI',
                                    L1CaloD3PDMaker.L1CaloCMMRoIFillerTool)
