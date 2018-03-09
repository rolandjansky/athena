TileTB=True

if not 'Geo' in dir():
    # TileCal standalone setup with 2 barrels and 1 ext.barrel on top
    #Geo = '2B1EB'
    # TileCal standalone setup with 2 barrels and 2 ext.barrels on top
    #Geo = '2B2EB'
    # TileCal standalone setup with 3 barrels
    #Geo = '3B'
    # TileCal standalone setup with 5 barrels - use for sampling fraction calculation
    Geo = '5B'

if not 'doHitNtuple' in dir():
    doHitNtuple = True

if not 'doTileNtuple' in dir():
    doTileNtuple = True

if not 'doD3PD' in dir():
    doD3PD = False

if not 'doRDO' in dir():
    doRDO = False

if not 'TileUseCOOL' in dir():
    TileUseCOOL=False

include("TileSimEx/jobOptions_Tile_Dig.py")

