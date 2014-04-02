#-------------------------------------------------------
# options for InDetAlignNt
# running in standard InDetCosmicRecExample cosmic recon
# makes ntuple from standard InDetCosmic_Tracks collection
# other options can be set by editing the file below
#-------------------------------------------------------

theApp.Dlls   += [ "InDetAlignGenAlgs","InDetAlignGenTools" ]
from InDetAlignGenAlgs.InDetAlignGenAlgsConf import InDetAlignNt
AlignNt=InDetAlignNt(Truth=False,
                     Overlap=False,NumDeriv=0,
                     AlignNt=True,BeamPos=False,ReadAl=True,
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
AlignNt.NewTrkCol="InDetCosmic_Tracks"

# include this to write alignment constants (use with care and edit WriteDBS.py
# beforehand to set your server/IOV/tag options
# include("InDetAlignGenAlgs/WriteDBS.py")

# produce ROOT ntuple using THistSvc
THistSvc=Service("THistSvc")
THistSvc.Output+=[ "IDALIGNFILE DATAFILE='idalign.root' OPT='RECREATE'"]
