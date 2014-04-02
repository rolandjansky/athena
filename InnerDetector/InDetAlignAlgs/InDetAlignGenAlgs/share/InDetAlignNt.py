#-------------------------------------------------------
# options for InDetAlignNt
# running in standard InDetRecExample reconstruction
# makes ntuple from standard Track collection
# other options can be set by editing the file below
#-------------------------------------------------------

theApp.Dlls   += [ "InDetAlignGenAlgs","InDetAlignGenTools" ]
from InDetAlignGenAlgs.InDetAlignGenAlgsConf import InDetAlignNt
AlignNt=InDetAlignNt(Truth=True,
                     Overlap=False,NumDeriv=0,
                     AlignNt=True,BeamPos=True,ReadAl=True,
                     OutputLevel=3)
topSequence+=AlignNt


# setup extrapolator/propagator instance specially for InDetAlignNt
# InDetAlignNt expects to find one with name InDetAlignNtExtrap
from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as Propagator
InDetAlignNtPropag=Propagator(name='InDetAlignNtPropag')
ToolSvc+=InDetAlignNtPropag
from TrkExTools.TrkExToolsConf import Trk__Extrapolator
InDetAlignNtExtrap=Trk__Extrapolator(name='InDetAlignNtExtrap',
                                     Propagators=[InDetAlignNtPropag])
ToolSvc+=InDetAlignNtExtrap
print "Extrapolator for InDetAlignNt is",InDetAlignNtExtrap

# options to use different track collections (default "Tracks")
#AlignNt.NewTrkCol="ConvertedXKalmanTracks"
#AlignNt.NewTrkCol="ConvertedIPatTracks"

# this to make truth for xkalman tracks available
#InDetTrackTruthMaker = Algorithm( "InDetTrackTruthMaker" )
#InDetTrackTruthMaker.TrackName = "ConvertedXKalmanTracks"
#InDetTrackTruthMaker.TrackName = "ConvertedIPatTracks"
#InDetTrackTruthMaker.TrackName = "Tracks"

# include this for refitting tracks before generating alignment constants
# uses preconfigured Trk::KalmanFitter/InDetTrackFitter to do fitting
#AlignNt.Refit=True
#AlignNt.RefitFromRIO=True

# needed to use InDetAlignTrkSel tool 
#InDetAlignNt.TrkSel=True
#from InDetAlignGenTools.InDetAlignGenToolsConf import InDetAlignTrackSelTool
#AlignTrackSelTool=InDetAlignTrackSelTool(MinPt=5000.)
#InDetAlignNt.TrkSelTool=AlignTrackSelTool

# print algorithm configuration
print "InDetAlignNt configuration:", AlignNt

# include this to write alignment constants (use with care and edit WriteDBS.py
# beforehand to set your server/IOV/tag options
# include("InDetAlignGenAlgs/WriteDBS.py")

# produce ROOT ntuple using THistSvc
from AthenaCommon.AppMgr import ServiceMgr
if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output+=[ "IDALIGNFILE DATAFILE='idalign.root' OPT='RECREATE'"]
