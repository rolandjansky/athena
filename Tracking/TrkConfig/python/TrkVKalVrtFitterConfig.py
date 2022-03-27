# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of TrkVKalVrtFitter package

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def TrkVKalVrtFitterCfg(flags, name="TrkVKalVrtFitter"):
    acc = ComponentAccumulator()
    kwargs = {}
    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    kwargs["Extrapolator"] = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
    kwargs["FirstMeasuredPoint"] = flags.InDet.SecVertex.Fitter.FirstMeasuredPoint
    kwargs["FirstMeasuredPointLimit"] = flags.InDet.SecVertex.Fitter.FirstMeasuredPointLimit
    kwargs["InputParticleMasses"] = flags.InDet.SecVertex.Fitter.InputParticleMasses
    kwargs["IterationNumber"] = flags.InDet.SecVertex.Fitter.IterationNumber
    kwargs["MakeExtendedVertex"] = flags.InDet.SecVertex.Fitter.MakeExtendedVertex
    kwargs["Robustness"] = flags.InDet.SecVertex.Fitter.Robustness
    kwargs["usePhiCnst"] = flags.InDet.SecVertex.Fitter.usePhiCnst
    kwargs["useThetaCnst"] = flags.InDet.SecVertex.Fitter.useThetaCnst
    kwargs["CovVrtForConstraint"] = flags.InDet.SecVertex.Fitter.CovVrtForConstraint
    kwargs["VertexForConstraint"] = flags.InDet.SecVertex.Fitter.VertexForConstraint

    tool = CompFactory.Trk.TrkVKalVrtFitter(name, **kwargs)
    acc.setPrivateTools(tool)
    return acc
