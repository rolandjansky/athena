# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from egammaTools import egammaToolsConf
from egammaRec.Factories import ToolFactory
from egammaRec import egammaKeys

from CaloTools.CaloToolsConf import CaloAffectedTool

egammaOQFlagsBuilder = ToolFactory(
    egammaToolsConf.egammaOQFlagsBuilder,
    QCellCut=4000.,
    QCellHECCut=60000.,
    QCellSporCut=4000.,
    LArQCut=0.8,
    TCut=10.,
    TCutVsE=2.,
    RcellCut=0.8,
    CellsName=egammaKeys.caloCellKey(),
    affectedTool=CaloAffectedTool())
