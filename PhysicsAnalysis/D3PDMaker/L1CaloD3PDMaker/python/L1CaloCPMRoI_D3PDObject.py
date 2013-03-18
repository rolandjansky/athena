# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# D3PD object saving L1Calo CPM RoI
# @author John Morris <john.morris@cern.ch>
#

from AthenaCommon.AppMgr import ToolSvc
from TrigT1CaloTools.TrigT1CaloToolsConf import LVL1__L1EmTauTools
L1EmTauTools_d3pd = LVL1__L1EmTauTools("LVL1EmTauTools_d3pd")
L1EmTauTools_d3pd.LVL1ConfigSvc="TrigConf::TrigConfigSvc/TrigConfigSvc"
ToolSvc += L1EmTauTools_d3pd

from D3PDMakerCoreComps.D3PDObject  import make_SGDataVector_D3PDObject
import EventCommonD3PDMaker
import L1CaloD3PDMaker

L1CaloCPMRoI_D3PDObject = make_SGDataVector_D3PDObject('DataVector<LVL1::CPMRoI>',
                                                       'CPMRoIs',
                                                       'L1CaloCPM_RoI_',
                                                       'L1CaloCPMRoI_D3PDObject')

#Level 0
L1CaloCPMRoI_D3PDObject.defineBlock(0,'CPMRoI',
                                     L1CaloD3PDMaker.L1CaloCPMRoIFillerTool,
                                     L1EmTauTool = L1EmTauTools_d3pd)
