from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
InDetExtrapolator = AtlasExtrapolator()
ToolSvc += InDetExtrapolator
print InDetExtrapolator

from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
TrkVKalVrtFitter = Trk__TrkVKalVrtFitter(name = "TrkVKalVrtFitter",
                                         Extrapolator = InDetExtrapolator,
                                         FirstMeasuredPoint = False)
ToolSvc += TrkVKalVrtFitter
print TrkVKalVrtFitter
