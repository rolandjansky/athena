infile = 'esd/ESD-20.1.7.2.pool.root'
keys = [
    #TileDigitsContainer_p1
    'TileDigitsFlt',

    #TileL2Container_p2
    'TileL2Cnt',

    #TileMuContainer_p1
    'TileMuObj',

    #TileTTL1Container_p1
    'TileTTL1MBTS',
         ]

from AthenaCommon.DetFlags      import DetFlags
DetFlags.detdescr.Calo_setOn()
noMuon = True
noID = True

include ('AthenaPoolUtilities/TPCnvTest.py')

from TileConditions.TileInfoConfigurator import TileInfoConfigurator
tileInfoConfigurator = TileInfoConfigurator()

