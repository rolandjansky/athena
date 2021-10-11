# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob
from AthenaCommon.GlobalFlags import globalflags

if globalflags.DataSource()=='geant4': 
    from AthenaCommon.AppMgr import ToolSvc
    import AthenaCommon.CfgMgr as CfgMgr

    from DerivationFrameworkHiggs.DerivationFrameworkHiggsConf import DerivationFramework__TruthCategoriesDecorator
    DFHTXSdecorator = DerivationFramework__TruthCategoriesDecorator(name = "DFHTXSdecorator")
    ToolSvc += DFHTXSdecorator

    ToolSvc += DFHTXSdecorator
    DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("TruthCategoriesCommonKernel",
                                                                             AugmentationTools = [DFHTXSdecorator]
                                                                             )
