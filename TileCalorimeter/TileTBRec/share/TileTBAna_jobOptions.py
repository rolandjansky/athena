#----------------------------------------
# Create TileCal testbeam analysis ntuple
#----------------------------------------
theApp.Dlls += [ "TileTBRec" ]
theApp.TopAlg += ["TileTBAna"]
TileTBAna = Algorithm( "TileTBAna" )

TileTBAna.NTupleLoc = "/FILE1/TileRec"
TileTBAna.NTupleID = "h1010"
TileTBAna.CaloCellContainer = "AllCalo"

TileTBAna.CompleteNtuple = TRUE
TileTBAna.CommitNtuple = TRUE

