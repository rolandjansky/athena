#guard against multiple inclusion
include.block( "CaloRec/CaloCellMaker_config.py" )

include( "CaloRec/CaloCellMakerFlags_jobOptions.py" )


# in addition the following flag may be checked
# make compactified/decompactified cell container *in addition* to existing one
if not 'doCompactCellInAddition' in dir(): 
    doCompactCellInAddition=False
    
# make compactified/decompactified cell container instead of existing one
if not 'doCompactCellInstead' in dir():
    doCompactCellInstead=False

# doLArCellDirect flag is used if defined
if doCompactCellInstead and doCompactCellInAddition:
    raise RuntimeError, 'only one of doCompactCellInstead  doCompactCellInAddition should be set'

CaloCellMakerFlags.doCompactCellInstead=doCompactCellInstead
CaloCellMakerFlags.doCompactCellInAddition=doCompactCellInAddition

# set sensible defaults according to global tags

CaloCellMakerFlags.makeCellFromRawChannel=False
CaloCellMakerFlags.makeCellFromDigit=False
CaloCellMakerFlags.doLArHitToCellDirect=False
CaloCellMakerFlags.doTileHitToRawChannelDirect=False

from AthenaCommon.GlobalFlags  import globalflags

if globalflags.DataSource() == 'data':
    if globalflags.DetGeo() == 'ctb':
        CaloCellMakerFlags.makeCellFromDigit=True
if globalflags.DataSource() == 'geant3' and globalflags.InputFormat() == 'zebra':
    # for geant3 digitisation of all calo is needed
    DetFlags.digitize.Calo_setOn()
    # usually use fast digitisation for lar
    CaloCellMakerFlags.doLArHitToCellDirect=True
    # usually use slow digitisation for tile
    CaloCellMakerFlags.doTileHitToRawChannelDirect=False
    CaloCellMakerFlags.makeCellFromDigit=True
elif globalflags.DataSource() == 'geant3' or globalflags.DataSource()=='geant4':
    # on geant4 usually run on raw channel file
    CaloCellMakerFlags.makeCellFromRawChannel=True

if not DetFlags.digitize.LAr_on():
    CaloCellMakerFlags.doLArHitToCellDirect=False
if not DetFlags.digitize.Tile_on():
    CaloCellMakerFlags.doTileHitToRawChannelDirect=False

    
