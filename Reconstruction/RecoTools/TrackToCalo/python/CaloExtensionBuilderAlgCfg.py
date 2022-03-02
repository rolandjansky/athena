# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentFactory import CompFactory

def CaloExtensionBuilderAlgCfg(flags, name="CaloExtensionBuilderAlg", **kwargs):
    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg    
    result = AtlasExtrapolatorCfg(flags)
    kwargs.setdefault("LastCaloExtentionTool", CompFactory.Trk.ParticleCaloExtensionTool(Extrapolator=result.popPrivateTools()))

    # P->T conversion extra dependencies
    if flags.Detector.GeometryITk:
        kwargs.setdefault("ExtraInputs", [
            ("InDetDD::SiDetectorElementCollection", "ConditionStore+ITkPixelDetectorElementCollection"),
            ("InDetDD::SiDetectorElementCollection", "ConditionStore+ITkStripDetectorElementCollection"),
        ])
    else:
        kwargs.setdefault("ExtraInputs", [
            ("InDetDD::SiDetectorElementCollection", "ConditionStore+PixelDetectorElementCollection"),
            ("InDetDD::SiDetectorElementCollection", "ConditionStore+SCT_DetectorElementCollection"),
        ])

    result.addEventAlgo(CompFactory.Trk.CaloExtensionBuilderAlg(name, **kwargs))
    return result


def CaloExtensionBuilderAlgLRTCfg(flags, name="CaloExtensionBuilderAlg_LRT", **kwargs):
    kwargs.setdefault("TrkPartContainerName", "InDetLargeD0TrackParticles")
    kwargs.setdefault("ParticleCache", "ParticleCaloExtension_LRT")
    return CaloExtensionBuilderAlgCfg(flags, name, **kwargs)


def CaloExtensionBuilderCfg(flags):
    result = CaloExtensionBuilderAlgCfg(flags)
    if flags.Detector.GeometryID and flags.InDet.Tracking.doR3LargeD0 and flags.InDet.Tracking.storeSeparateLargeD0Container:
        result.merge(CaloExtensionBuilderAlgLRTCfg(flags))
    return result
