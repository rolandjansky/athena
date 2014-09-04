# Test LArCellIDC object in StoreGate, using Algorithm LArCellIDC_Test 
# DLLs 
theApp.Dlls += [ "LArRawUtils", "LArCellRec" ]
# Alg
theApp.TopAlg += [ "LArCellIDC_Test" ]
# Properties
LArCellIDC_Test = Algorithm( "LArCellIDC_Test" )
LArCellIDC_Test.CellColl="LArCellIDC"
LArCellIDC_Test.NtupleID=101
