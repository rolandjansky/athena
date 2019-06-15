# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

__doc__ = "Instantiate the EMGSFCaloExtensionBuilder with default configuration"

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from TrkExTools.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
from TrackToCalo.TrackToCaloConfig import ParticleCaloExtensionToolCfg
from egammaAlgs.egammaAlgsConf import EMGSFCaloExtensionBuilder


def EMGSFCaloExtensionBuilderCfg(flags, name='EMGSFCaloExtensionBuilder', **kwargs):

    mlog = logging.getLogger(name)
    mlog.info('Starting configuration')

    acc = ComponentAccumulator()

    if "Extrapolator" not in kwargs:
        extrapAcc = AtlasExtrapolatorCfg(flags)
        kwargs["Extrapolator"] = extrapAcc.popPrivateTools()
        acc.merge(extrapAcc)

    if "PerigeeCaloExtensionTool" not in kwargs:
        perigeeCaloExtrapAcc = ParticleCaloExtensionToolCfg(flags,
                                                            name="PerigeeCaloExtensionTool",
                                                            Extrapolator=kwargs["Extrapolator"],
                                                            ParticleType="electron",
                                                            StartFromPerigee=True)
        kwargs["PerigeeCaloExtensionTool"] = perigeeCaloExtrapAcc.popPrivateTools()
        acc.merge(perigeeCaloExtrapAcc)

    if "LastCaloExtensionTool" not in kwargs:
        lastCaloExtrapAcc = ParticleCaloExtensionToolCfg(flags,
                                                         name="LastCaloExtensionTool",
                                                         ParticleType="electron",
                                                         Extrapolator=kwargs["Extrapolator"])
        kwargs["LastCaloExtensionTool"] = lastCaloExtrapAcc.popPrivateTools()
        acc.merge(lastCaloExtrapAcc)

    kwargs.setdefault("GSFPerigeeCache", 'GSFPerigeeCaloExtension')
    kwargs.setdefault("GSFLastCache",    'GSFLastCaloExtension')
    kwargs.setdefault("GFFTrkPartContainerName", flags.Egamma.Keys.Output.GSFTrackParticles)

    emgscaloextfAlg = EMGSFCaloExtensionBuilder(name, **kwargs)

    acc.addEventAlgo(emgscaloextfAlg)
    return acc
