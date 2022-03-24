###############################################################
#
# Job options file : TileSamplingFraction
# Choose sampling fraction according to physics list
#
#==============================================================

from AthenaCommon.Logging import logging
msg = logging.getLogger( 'TileSamplingFraction_jobOptions.py' )

from TileConditions.TileInfoConfigurator import TileInfoConfigurator
tileInfoConfigurator = TileInfoConfigurator()
tileInfoConfigurator.setupCOOLSFR()
# Temporary solution (this folder is only here)
conddb.blockFolder('/TILE/OFL02/CALIB/SFR')
conddb.addFolderWithTag('TILE_OFL','/TILE/OFL02/CALIB/SFR','TileOfl02CalibSfr-SIM-02',force=True,forceMC=True,className='CondAttrListCollection')

try:
    from Digitization.DigitizationFlags import jobproperties

    tileInfoConfigurator.TileNoise = jobproperties.Digitization.doCaloNoise()
    if tileInfoConfigurator.TileNoise:
        msg.info("Switching ON noise in Tile Digitization" )
    else:
        msg.info("Switching OFF noise in Tile Digitization" )

    mlog.info (tileInfoConfigurator)

except:
    msg.info("Physics list not found, using default sampling fraction value")
    msg.info("doCaloNoise flag not found, keeping previous value for TileNoise")
