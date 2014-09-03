###################################################
# Calibration Hits --> CaloCells
###################################################

theApp.Dlls += [ "CaloCalibHitRec" ]
theApp.TopAlg += ["CalibHitToCaloCell"]

CalibHitToCaloCell = Algorithm( "CalibHitToCaloCell" )
CalibHitToCaloCell.StoreUnknownCells = False


#CaloCellContainer names 
#
#CalibHitToCaloCell.CellTotEne   = "TotalCalibCell"  #default
#CalibHitToCaloCell.CellVisEne   = "VisCalibCell"    #default
#CalibHitToCaloCell.CellEmEne    = "EmCalibCell"     #suggested
#CalibHitToCaloCell.CellNonEmEne = "NonEmCalibCell"  #suggested


