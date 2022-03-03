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
        
try:
    from Digitization.DigitizationFlags import jobproperties
    physicsList = jobproperties.Digitization.physicsList()

    G4Ver = jobproperties.Digitization.SimG4VersionUsed()
    G4Ve = G4Ver.split(".")
    G4V = int(G4Ve[1])+int(G4Ve[2])/100.
    
    #default value corresponds to new multiple scatering
    #EmScaleA = 34.3

    #default value since May-2011
    EmScaleA = 34.0

    #default value for G4 9.6 since Nov-2013 (need to check G4 version as well)
    if physicsList == 'FTFP_BERT' or (physicsList == 'QGSP_BERT' and G4V > 9.05999) :
        EmScaleA = 33.9

    #default value for G4 10.0 since June-2016 
    # see https://indico.cern.ch/event/489520/contributions/2193913/attachments/1285565/1914309/sampling_fractions.pdf
    if G4V >= 10.0 :
        EmScaleA = 33.7

    if G4V >= 10.05 :
        EmScaleA = 32.9

    # old value
    if physicsList == 'QGSP_EMV' or physicsList == 'QGSP_BERT_EMV' or physicsList == '' :
        EmScaleA = 35.9

    tileInfoConfigurator.EmScaleA = EmScaleA;  # 1/s.f. value for all normal cells
    msg.info("Using 1/s.f. = %s for %s physics list and G4version %s (%s)" , EmScaleA, physicsList, G4V, G4Ver )

    from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as geoFlags
    # new sampling fraction for gap/crack scintillators in RUN3
    # see https://indico.cern.ch/event/1084901/contributions/4616550/attachments/2349654/4007529/TileGap3_SamplingFraction.pdf
    # RUN2 values:         125 107 97 75
    # RUN3 EM scale:       109  89 74 61
    # RUN3 EM+nonEM scale: 123  85 69 62
    run = geoFlags.Run()
    if run in ['RUN3', 'RUN4'] and G4V >= 10.05:
        tileInfoConfigurator.EmScaleE1 = 109.0
        tileInfoConfigurator.EmScaleE2 = 89.0
        tileInfoConfigurator.EmScaleE3 = 74.0
        tileInfoConfigurator.EmScaleE4 = 61.0
        msg.info("Using 1/s.f. = %s %s %s %s for E-cells in %s",
                 tileInfoConfigurator.EmScaleE1, tileInfoConfigurator.EmScaleE2,
                 tileInfoConfigurator.EmScaleE3, tileInfoConfigurator.EmScaleE4, run )

    tileInfoConfigurator.TileNoise = jobproperties.Digitization.doCaloNoise()
    if tileInfoConfigurator.TileNoise:
        msg.info("Switching ON noise in Tile Digitization" )
    else:
        msg.info("Switching OFF noise in Tile Digitization" )

    mlog.info (tileInfoConfigurator)

except:
    msg.info("Physics list not found, using default sampling fraction value")
    msg.info("doCaloNoise flag not found, keeping previous value for TileNoise")
