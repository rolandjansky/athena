# default parameters for tilecal standalone reconstruction
# by default all Optimal filtering algorithms are switched off

if ('doSim' in dir()) and doSim:
    # don't need to run any optimal filtering on simulated data
    # this is already done at digitization step
    doTileFlat = False
    doTileOpt  = False
    doTileOF1 = False
    doTileOpt2 = False
    doTileQIE = False
    doTileOptATLAS = False
    doTileMF = False
    doTileWiener = False
    doTileFit  = False
    doTileFitCool = False
    TileCisRun = False
    TilePedRun = False
    TileLasRun = False
    TileMonoRun = False
    TilePhysRun = True
    TileRunType = 1
    if not 'useTMDB' in dir():
        useTMDB = False


# set options for TileRawChannelMaker
from TileRecUtils.TileRecFlags import jobproperties

if not 'doTileFlat' in dir():
    doTileFlat = jobproperties.TileRecFlags.doTileFlat()
else:
    jobproperties.TileRecFlags.doTileFlat = doTileFlat

if not 'doTileOpt' in dir():
    doTileOpt = jobproperties.TileRecFlags.doTileOpt()
else:
    jobproperties.TileRecFlags.doTileOpt = doTileOpt

if not 'doTileOF1' in dir():
    doTileOF1 = jobproperties.TileRecFlags.doTileOF1()
else:
    jobproperties.TileRecFlags.doTileOF1 = doTileOF1

if not 'doTileOpt2' in dir():
    doTileOpt2 = jobproperties.TileRecFlags.doTileOpt2()
else:
    jobproperties.TileRecFlags.doTileOpt2 = doTileOpt2
    
if not 'doTileQIE' in dir():
    doTileQIE = jobproperties.TileRecFlags.doTileQIE()
else:
    jobproperties.TileRecFlags.doTileQIE = doTileQIE

if not 'doTileOptATLAS' in dir():
    doTileOptATLAS = jobproperties.TileRecFlags.doTileOptATLAS()
else:
    jobproperties.TileRecFlags.doTileOptATLAS = doTileOptATLAS

if not 'doTileMF' in dir():
    doTileMF = jobproperties.TileRecFlags.doTileMF()
else:
    jobproperties.TileRecFlags.doTileMF = doTileMF

if not 'doTileWiener' in dir():
    doTileWiener = jobproperties.TileRecFlags.doTileWiener()
else:
    jobproperties.TileRecFlags.doTileWiener = doTileWiener

if not 'doTileFit' in dir():
    doTileFit = jobproperties.TileRecFlags.doTileFit()
else:
    jobproperties.TileRecFlags.doTileFit = doTileFit

if not 'doTileFitCool' in dir():
    doTileFitCool = jobproperties.TileRecFlags.doTileFitCool()
else:
    jobproperties.TileRecFlags.doTileFitCool = doTileFitCool
    
if not 'doTileManyAmps' in dir():
    doTileManyAmps = jobproperties.TileRecFlags.doTileManyAmps()
else:
    jobproperties.TileRecFlags.doTileManyAmps = doTileManyAmps

# prepare to reconstruct physics run
if not 'TileCisRun' in dir():
    TileCisRun = False

if not 'TilePedRun' in dir():
    TilePedRun = False

if not 'TileLasRun' in dir():
    TileLasRun = False

if not 'TileMonoRun' in dir():
    TileMonoRun = False

if not 'useTMDB' in dir():
    useTMDB = True

if not 'TileRunType' in dir():
    TileRunType = jobproperties.TileRecFlags.TileRunType()
else:
    jobproperties.TileRecFlags.TileRunType = TileRunType

if not 'TileCalibrateEnergy' in dir():
    TileCalibrateEnergy = jobproperties.TileRecFlags.calibrateEnergy()
else:
    jobproperties.TileRecFlags.calibrateEnergy = TileCalibrateEnergy

# take OFCs from COOL, no on the fly calculations
if not 'OfcFromCOOL' in dir():
    OfcFromCOOL = jobproperties.TileRecFlags.OfcFromCOOL()
else:
    jobproperties.TileRecFlags.OfcFromCOOL = OfcFromCOOL
    
if not 'TileCorrectAmplitude' in dir():
    TileCorrectAmplitude = jobproperties.TileRecFlags.correctAmplitude()
else:
    jobproperties.TileRecFlags.correctAmplitude = TileCorrectAmplitude

if not 'TileCorrectTime' in dir():
    TileCorrectTime = jobproperties.TileRecFlags.correctTime()
else:
    jobproperties.TileRecFlags.correctTime = TileCorrectTime


