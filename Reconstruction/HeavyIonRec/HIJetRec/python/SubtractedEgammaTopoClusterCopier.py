# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = """
ToolFactory to instantiate egammaTopoClusterCopier with default configuration"""
__author__ = "Jovan Mitrevski"

from egammaAlgs import egammaAlgsConf
from egammaRec.Factories import AlgFactory
from egammaRec.egammaRecFlags import jobproperties

SubtractedEgammaTopoClusterCopier = AlgFactory(
    egammaAlgsConf.egammaTopoClusterCopier,
    name='SubtractedEgammaTopoClusterCopier',
    InputTopoCollection=jobproperties.egammaRecFlags.inputTopoClusterCollection(),
    OutputTopoCollection=jobproperties.egammaRecFlags.egammaTopoClusterCollection(),
    OutputTopoCollectionShallow="tmp_" +
    jobproperties.egammaRecFlags.egammaTopoClusterCollection()
)