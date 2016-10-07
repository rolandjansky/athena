# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import egammaToolsConf
from egammaRec.Factories import FcnWrapper, ToolFactory
from egammaRec import egammaKeys

from LArBadChannelTool import LArBadChannelToolConf
LArBadChanTool = ToolFactory( LArBadChannelToolConf.LArBadChanTool )

from CaloTools import CaloAffectedToolDefault as CATD
CaloAffectedToolDefault = ToolFactory( CATD.CaloAffectedToolDefault )

egammaOQFlagsBuilder = ToolFactory( egammaToolsConf.egammaOQFlagsBuilder,
  QCellCut = 4000.,
  QCellHECCut = 60000.,
  QCellSporCut = 4000.,
  LArQCut = 0.8,
  TCut = 10.,
  TCutVsE = 2.,
  RcellCut = 0.8,
  CellsName = egammaKeys.caloCellKey(),
  LArBadChannelTool = LArBadChanTool,
  affectedTool = CaloAffectedToolDefault)
