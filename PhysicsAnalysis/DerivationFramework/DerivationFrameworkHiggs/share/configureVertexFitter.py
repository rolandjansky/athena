from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
AnExtrapolator = AtlasExtrapolator()
ToolSvc += AnExtrapolator

from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
TrkVKalVrtFitter = Trk__TrkVKalVrtFitter(name = "TrkVKalVrtFitter",
                                         Extrapolator = AnExtrapolator,
                                         FirstMeasuredPoint = False)
ToolSvc += TrkVKalVrtFitter
