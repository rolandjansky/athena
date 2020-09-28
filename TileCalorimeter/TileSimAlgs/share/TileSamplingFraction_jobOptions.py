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
    msg.info("Using 1/s.f. = %s for %s physics list and G4version %s (%s)" % (EmScaleA,physicsList,G4V,G4Ver) )
    #tileInfoConfigurator.EmScaleE = 75.0; # keep the same EmScale for gap scintillators for the moment

    tileInfoConfigurator.TileNoise = jobproperties.Digitization.doCaloNoise()
    if tileInfoConfigurator.TileNoise:
        msg.info("Switching ON noise in Tile Digitization" )
    else:
        msg.info("Switching OFF noise in Tile Digitization" )

    mlog.info (tileInfoConfigurator)

except:
    msg.info("Physics list not found, using default sampling fraction value")
    msg.info("doCaloNoise flag not found, keeping previous value for TileNoise")
