#jobOption file for CaloCellMaker to make CaloCell from RawChannel on G4

#
theApp.Dlls += [ 
"CaloRec"
]
theApp.TopAlg += [
"CaloCellMaker" 
]
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
CaloCellMaker = Algorithm( "CaloCellMaker" )
CaloCellMaker.CaloCellsOutputName = "AllCalo"

#make the calo cells (LArEM, HEC and FCal

if DetFlags.makeRIO.LAr_on():
    include ("LArCellRec/LArCellFromLArRawTool_MCG4_jobOptions.py")

#make the Tile cells 

if DetFlags.makeRIO.Tile_on():
    include( "TileRecUtils/TileCellBuilder_jobOptions.py" )

#finalize container
CaloCellMaker.CaloCellMakerToolNames += ["CaloCellContainerFinalizerTool"]

#make lots of checks (should not be necessary eventually)
CaloCellMaker.CaloCellMakerToolNames += ["CaloCellContainerCheckerTool"] 

