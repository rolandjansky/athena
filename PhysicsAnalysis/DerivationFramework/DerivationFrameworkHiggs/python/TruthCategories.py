# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkIsMonteCarlo,DerivationFrameworkJob
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon import CfgMgr

if DerivationFrameworkIsMonteCarlo:

    from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__TruthCategoriesDecorator
    DFHTXSdecorator = DerivationFramework__TruthCategoriesDecorator(name = "DFHTXSdecorator")

    ToolSvc += DFHTXSdecorator
    DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("TruthCategoriesCommonKernel",
                                                                             AugmentationTools = [DFHTXSdecorator]
                                                                             )
