# jobOptions file for testing LArCell
# ApplicationMgr.Dlls += {  "LArCellRec" };
theApp.TopAlg += [
"LArCellTest/LArCellTestEM",
"LArCellTest/LArCellTestHEC",
"LArCellTest/LArCellTestFCAL"
]
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
LArCellTestEM = Algorithm( "LArCellTestEM" )
LArCellTestEM.CellColl="LArEM"
LArCellTestEM.NtupleID=101; 
LArCellTestHEC = Algorithm( "LArCellTestHEC" )
LArCellTestHEC.CellColl="LArHEC"
LArCellTestHEC.NtupleID=102; 
LArCellTestFCAL = Algorithm( "LArCellTestFCAL" )
LArCellTestFCAL.CellColl="LArFCal"
LArCellTestFCAL.NtupleID=103; 
