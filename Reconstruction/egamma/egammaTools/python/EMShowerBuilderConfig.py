# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

__doc__ = "ToolFactory to instantiate all EMShowerBuilder with default configuration"

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

from AthenaCommon.Logging import logging
from egammaTools.egammaToolsConf import EMShowerBuilder
from egammaCaloTools.egammaCaloToolsConf import egammaIso, egammaShowerShape
from CaloIdentifier import SUBCALO


def EMShowerBuilderCfg(flags, name='EMShowerBuilder', **kwargs):

    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    acc = ComponentAccumulator()

    kwargs.setdefault("CellsName", flags.Egamma.Keys.Input.CaloCells)
    kwargs.setdefault("CaloNums", [SUBCALO.LAREM, SUBCALO.LARHEC, SUBCALO.TILE])
    kwargs.setdefault("ShowerShapeTool", egammaShowerShape())
    kwargs.setdefault("HadronicLeakageTool", egammaIso())

    tool = EMShowerBuilder(name, **kwargs)

    acc.setPrivateTools(tool)
    return acc
