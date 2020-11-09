# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from DerivationFrameworkCore.DerivationFrameworkMaster import *

if DerivationFrameworkHasTruth:

    from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__TruthCategoriesDecorator
    DFHTXSdecorator = DerivationFramework__TruthCategoriesDecorator(name = "DFHTXSdecorator")

    ToolSvc += DFHTXSdecorator
    DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("TruthCategoriesCommonKernel",
                                                                             AugmentationTools = [DFHTXSdecorator]
                                                                             )
