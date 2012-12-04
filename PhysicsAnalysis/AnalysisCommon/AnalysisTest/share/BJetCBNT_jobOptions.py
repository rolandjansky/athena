#--------------------------------------------------
# CBNT_BJet
#-------------------------------------------------
theApp.Dlls += [ "AnalysisTest" ]
CBNT_Athena = Algorithm( "CBNT_Athena" )

OutputLevel = 3

# general BTag CBNT variables
CBNT_Athena.Members += [ "CBNT_BJetGeneral/CBNT_BJetGeneral" ]
CBNT_BJetGeneral = Algorithm( "CBNT_BJetGeneral" )
CBNT_BJetGeneral.BCandidatesName = "BCandidates"
CBNT_BJetGeneral.OutputLevel = OutputLevel

# lifetime BTag CBNT variables
if BTaggingAlgsFlags.lifetime1D:
  CBNT_Athena.Members += [ "CBNT_BJetLifetimeTag/CBNT_BJetLifetimeTag1D" ]
  CBNT_BJetLifetimeTag1D = Algorithm( "CBNT_BJetLifetimeTag1D" )
  CBNT_BJetLifetimeTag1D.BCandidatesName = "BCandidates"
  CBNT_BJetLifetimeTag1D.tagMode = "1D"
  CBNT_BJetLifetimeTag1D.OutputLevel = OutputLevel
if BTaggingAlgsFlags.lifetime2D:
  CBNT_Athena.Members += [ "CBNT_BJetLifetimeTag/CBNT_BJetLifetimeTag2D" ]
  CBNT_BJetLifetimeTag2D = Algorithm( "CBNT_BJetLifetimeTag2D" )
  CBNT_BJetLifetimeTag2D.BCandidatesName = "BCandidates"
  CBNT_BJetLifetimeTag2D.tagMode = "2D"
  CBNT_BJetLifetimeTag2D.OutputLevel = OutputLevel
if BTaggingAlgsFlags.lifetime3D:
  CBNT_Athena.Members += [ "CBNT_BJetLifetimeTag/CBNT_BJetLifetimeTag3D" ]
  CBNT_BJetLifetimeTag3D = Algorithm( "CBNT_BJetLifetimeTag3D" )
  CBNT_BJetLifetimeTag3D.BCandidatesName = "BCandidates"
  CBNT_BJetLifetimeTag3D.tagMode = "3D"
  CBNT_BJetLifetimeTag3D.OutputLevel = OutputLevel

# sec vtx BTag variables
if BTaggingAlgsFlags.secVtxFitBU:  
  CBNT_Athena.Members += [ "CBNT_BJetSecVtxTag/CBNT_BJetSecVtxTagBU" ]
  CBNT_BJetSecVtxTagBU = Algorithm( "CBNT_BJetSecVtxTagBU" )
  CBNT_BJetSecVtxTagBU.BCandidatesName = "BCandidates"
  CBNT_BJetSecVtxTagBU.tagMode = "BU"
  CBNT_BJetSecVtxTagBU.OutputLevel = OutputLevel
if BTaggingAlgsFlags.secVtxFitTD:  
  CBNT_Athena.Members += [ "CBNT_BJetSecVtxTag/CBNT_BJetSecVtxTagTD" ]
  CBNT_BJetSecVtxTagTD = Algorithm( "CBNT_BJetSecVtxTagTD" )
  CBNT_BJetSecVtxTagTD.BCandidatesName = "BCandidates"
  CBNT_BJetSecVtxTagTD.tagMode = "TD"
  CBNT_BJetSecVtxTagTD.OutputLevel = OutputLevel


