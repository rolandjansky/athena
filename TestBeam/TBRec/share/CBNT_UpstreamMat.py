include.block("TBRec/CBNT_UpstreamMat.py")

theApp.Dlls += [ "TBRec" ]

include("G4SimAthenaPOOL/G4SimAthenaPOOL_joboptions.py")

CBNT_Athena.Members += [ "CBNT_UpstreamMat" ]
CBNT_UpstreamMat = Algorithm( "CBNT_UpstreamMat" )

CBNT_UpstreamMat.CollectionName="LArFarUpstreamMaterialExitLayer"

