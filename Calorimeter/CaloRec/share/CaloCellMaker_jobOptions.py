#jobOption to make cells in any configuration
# only from G4 raw channel for time being
# one of the following variables need be set
# makeCellFromDigit
# makeCellFromRawChannel
# DetFlags can be used in addition to redigitise one or more calorimeter
# DetFlags.digitize.Tile_setOn()
# in that case fast digitisation can be requested:


#check input variable consistency and deduce which steps are necessary
if ( not CaloCellMakerFlags.makeCellFromDigit and not CaloCellMakerFlags.makeCellFromRawChannel and not DetFlags.digitize.Calo_allOn()  ):
    raise RunTimeError, 'only one of these variables must be set:DetFlags.Digitize.calo_allOn() or makeCellFromDigit or makeCellFromRawChannel '

if CaloCellMakerFlags.makeCellFromRawChannel and CaloCellMakerFlags.makeCellFromDigit :
    raise RunTimeError, 'only one of these variables must be set: makeCellFromDigit or makeCellFromRawChannel'

doLArHitToDigit=False
doLArDigitToRawChannel=False
doLArRawChannelToCell=False

doTileHitToDigit=False
doTileDigitToRawChannel=False
doTileRawChannelToCell=False

    
if DetFlags.digitize.LAr_on():
    if not CaloCellMakerFlags.doLArHitToCellDirect:
        doLArHitToDigit=True
        doLArDigitToRawChannel=True
        doLArRawChannelToCell=True

if DetFlags.digitize.Tile_on():
    if not CaloCellMakerFlags.doTileHitToRawChannelDirect:
        doTileHitToDigit=True
        doTileDigitToRawChannel=True
    doTileRawChannelToCell=True

        
if CaloCellMakerFlags.makeCellFromRawChannel :
    if DetFlags.makeRIO.LAr_on() and not CaloCellMakerFlags.doLArHitToCellDirect:
        doLArRawChannelToCell=True
    if DetFlags.makeRIO.Tile_on() and not CaloCellMakerFlags.doTileHitToRawChannelDirect:
        doTileRawChannelToCell=True

if CaloCellMakerFlags.makeCellFromDigit:
    if DetFlags.makeRIO.LAr_on() and not CaloCellMakerFlags.doLArHitToCellDirect:
        doLArDigitToRawChannel=True
        doLArRawChannelToCell=True
    if DetFlags.makeRIO.Tile_on() and  not CaloCellMakerFlags.doTileHitToRawChannelDirect:
        doTileDigitToRawChannel=True
        doTileRawChannelToCell=True


# SUBCALO enum LAREM LARHEC LARFCAL TILE NSUBCALO NOT_VALID
from CaloIdentifier import SUBCALO

#make the lar cells

# LArHit to LArDigit
if doLArHitToDigit:
    if not DetFlags.digitize.LAr_allOn():
        raise RunTimeError, 'full LAR partial digitisation not implemented'
    print "now doing digitization"
    include( "LArDigitization/LArDigitization_G4_jobOptions.py" )


#LArDigit to LArRawChannel
if doLArDigitToRawChannel:
    include( "LArROD/LArROD_jobOptions.py" )

# build Tile Raw Channel
doTileHitToRawChannelDirect=CaloCellMakerFlags.doTileHitToRawChannelDirect
if (doTileHitToDigit) or (doTileHitToRawChannelDirect) or (doTileDigitToRawChannel):
    include( "TileSimAlgs/TileDigitization_jobOptions.py" )



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
if CaloCellMakerFlags.doCompactCellInAddition:
    CaloCellMaker.CaloCompactCellsOutputName = "CompactAllCalo"

# compactified/decompactified CaloCellContainer replace the normal one
if CaloCellMakerFlags.doCompactCellInstead:
    CaloCellMaker.CaloCellsOutputName = "OriginalAllCalo"
    CaloCellMaker.CaloCompactCellsOutputName = "AllCalo"

#make the calo cells (LArEM, HEC and FCal)
# configured by doLArHitToCellDirect
if DetFlags.makeRIO.LAr_on():
    include ("LArCellRec/LArCellMakerTool_jobOptions.py")

ToolSvc = Service( "ToolSvc" )


from AthenaCommon.GlobalFlags  import globalflags

if (globalflags.DataSource()=='geant3' or globalflags.DataSource()=='data' )  and DetFlags.makeRIO.LAr_on():
    # apply correction to LAr Cells
    # only lar cell so far in CaloCellContainer. More efficient than checking on calo type.
    CaloCellMaker.CaloCellMakerToolNames += ["CaloCellContainerCorrectorTool/LArCellCorrectorTool"]

    # apply LArG3EScale correction only for geant3 data
    if globalflags.DataSource.() == 'geant3':
        ToolSvc.LArCellCorrectorTool.CaloNums = [ SUBCALO.NSUBCALO ] 
        ToolSvc.LArCellCorrectorTool.CellCorrectionToolNames= [ "LArG3Escale" ] 

    # apply HV correction only to real data
    if globalflags.DataSource() == 'data':
        ToolSvc.LArCellCorrectorTool.CaloNums = [ SUBCALO.NSUBCALO ] 
        ToolSvc.LArCellCorrectorTool.CellCorrectionToolNames= [ "LArCellHVCorrection" ] 



# make the tile cells
include( "TileRecUtils/TileCellBuilder_jobOptions.py" )





# now the CaloCellContainer is completely filled with LAr and Tile cells

#finalize container
CaloCellMaker.CaloCellMakerToolNames += ["CaloCellContainerFinalizerTool"]

#make lots of checks (should not be necessary eventually)
#to print the check add:
#ToolSvc.CaloCellContainerCheckerTool.OutputLevel=DEBUG
CaloCellMaker.CaloCellMakerToolNames += ["CaloCellContainerCheckerTool"] 


# if necessary finalize and check the compactified/decompactified tool
if CaloCellMakerFlags.doCompactCellInAddition or CaloCellMakerFlags.doCompactCellInstead:
    theApp.DLLs+= ["CaloTools"]
    CaloCellMaker.CaloCompactCellMakerToolNames += ["CaloCellContainerFinalizerTool"]
    CaloCellMaker.CaloCompactCellMakerToolNames += ["CaloCellContainerCheckerTool"] 

