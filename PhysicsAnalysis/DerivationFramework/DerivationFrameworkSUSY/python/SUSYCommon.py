# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from DerivationFrameworkCore.DerivationFrameworkMaster import *

from AthenaCommon import Logging
susycommonlog = Logging.logging.getLogger('SUSYCommon')

SUSYCommonAugmentationTools = []

susycommonlog.info( "Scheduling decoration of tau seed jet width for FakeTauTaskForce tool." )

from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__TauDecorator
SUSYCommonTauDecorator = DerivationFramework__TauDecorator( name = "SUSYCommonTauDecorator" )
ToolSvc += SUSYCommonTauDecorator
SUSYCommonAugmentationTools.append(SUSYCommonTauDecorator)

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob

DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("SUSYCommonKernel",
                                                                         AugmentationTools = SUSYCommonAugmentationTools)
