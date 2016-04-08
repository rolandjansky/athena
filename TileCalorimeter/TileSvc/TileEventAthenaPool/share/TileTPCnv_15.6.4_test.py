infile = 'esd/ESD-15.6.4.pool.root'
keys = [
    #TileL2Container_p1
    'TileL2Cnt',

    #TileMuContainer_p1
    'TileMuObj',

    #TileTTL1Container_p1
    'TileTTL1MBTS',
         ]

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = 'ATLAS-GEO-10-00-00'

from AthenaCommon.DetFlags      import DetFlags
DetFlags.detdescr.Calo_setOn()
noMuon = True
noID = True

include ('AthenaPoolUtilities/TPCnvTest.py')

from TileConditions.TileInfoConfigurator import TileInfoConfigurator
tileInfoConfigurator = TileInfoConfigurator()

