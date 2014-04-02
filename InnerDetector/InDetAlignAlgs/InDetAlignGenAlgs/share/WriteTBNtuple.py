# job option fragment to enable InDetAlignNt for combined testbeam geometry
# in framework of RecExTB
# enables alignment ntuple production for InDetTB_Tracks (produced by
# straightline fitter)
#
theApp.Dlls   += [ "InDetAlignGenAlgs","InDetAlignGenTools" ]
theApp.TopAlg += [ "InDetAlignNt" ]
InDetAlignNt = Algorithm("InDetAlignNt")
InDetAlignNt.ReadAl=TRUE
InDetAlignNt.TRT=1
InDetAlignNt.layout=11
InDetAlignNt.truth=FALSE
InDetAlignNt.newtrk=True
InDetAlignNt.newtrkcol="InDetTB_Tracks"

# setup extrapolator/propagator instance
from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as Propagator
InDetAlignNtPropag=Propagator(name='InDetAlignNtPropag')
ToolSvc+=InDetAlignNtPropag
from TrkExTools.TrkExToolsConf import Trk__Extrapolator
InDetAlignNtExtrap=Trk__Extrapolator(name='InDetAlignNtExtrap',
                                     Propagators=[InDetAlignNtPropag.getType()],
                                     PropagatorInstances=[InDetAlignNtPropag.getName()])
ToolSvc+=InDetAlignNtExtrap
InDetAlignNt.Extrapolator=InDetAlignNtExtrap
