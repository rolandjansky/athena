# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import cppyy
from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from egammaTools.egammaOQFlagsBuilderConfig import egammaOQFlagsBuilderCfg
EMFourMomBuilder=CompFactory.EMFourMomBuilder
from ElectronPhotonSelectorTools.AsgForwardElectronIsEMSelectorsConfig import AsgForwardElectronIsEMSelectorCfg
egammaForwardBuilder=CompFactory.egammaForwardBuilder
from ROOT import egammaPID

cppyy.load_library('libElectronPhotonSelectorToolsDict')


def egammaForwardBuilderCfg(flags, name='egammaForwardElectron', **kwargs):

    mlog = logging.getLogger(name)
    mlog.info('Starting configuration')

    acc = ComponentAccumulator()

    if "forwardelectronIsEMselectors" not in kwargs:
        LooseFwdElectronSelector = AsgForwardElectronIsEMSelectorCfg(flags, "LooseForwardElectronSelector", egammaPID.ForwardElectronIDLoose)
        MediumFwdElectronSelector = AsgForwardElectronIsEMSelectorCfg(flags, "MediumForwardElectronSelector", egammaPID.ForwardElectronIDMedium)
        TightFwdElectronSelector = AsgForwardElectronIsEMSelectorCfg(flags, "TightForwardElectronSelector", egammaPID.ForwardElectronIDTight)

        kwargs.setdefault("forwardelectronIsEMselectors", [LooseFwdElectronSelector.popPrivateTools(),
                                                           MediumFwdElectronSelector.popPrivateTools(),
                                                           TightFwdElectronSelector.popPrivateTools()])
        kwargs.setdefault("forwardelectronIsEMselectorResultNames", ["Loose", "Medium", "Tight"])

        acc.merge(LooseFwdElectronSelector)
        acc.merge(MediumFwdElectronSelector)
        acc.merge(TightFwdElectronSelector)

    if "ObjectQualityTool" not in kwargs:
        egOQ = egammaOQFlagsBuilderCfg(flags)
        kwargs["ObjectQualityTool"] = egOQ.popPrivateTools()
        acc.merge(egOQ)

    kwargs.setdefault("ElectronOutputName",   flags.Egamma.Keys.Output.ForwardElectrons)
    kwargs.setdefault("TopoClusterName",      flags.Egamma.Keys.Input.ForwardTopoClusters)
    kwargs.setdefault("ClusterContainerName", flags.Egamma.Keys.Output.ForwardClusters)
    kwargs.setdefault("FourMomBuilderTool",   EMFourMomBuilder())

    fwdAlg = egammaForwardBuilder(name, **kwargs)

    acc.addEventAlgo(fwdAlg)
    return acc
