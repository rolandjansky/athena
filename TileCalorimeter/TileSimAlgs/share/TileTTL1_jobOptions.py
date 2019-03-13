from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from TileSimAlgs.TileTTL1FromHitsGetter import TileTTL1FromHitsGetter
theTileTTL1FromHitsGetter = TileTTL1FromHitsGetter()

from Digitization.DigitizationFlags import jobproperties

from AthenaCommon.BeamFlags import jobproperties
if jobproperties.Beam.beamType == 'cosmics': 

    theTileTTL1Cosmics=TileHitToTTL1("TileHitToTTL1_Cosmics",
                                     TileTTL1Type = "Cosmics",
                                     TileHitContainer = "TileHitCnt",
                                     TileTTL1Container = "TileTTL1CosmicsCnt",
				     TileMBTSTTL1Container = "TileMBTSTTL1CosmicsContainer"
                                     )
    topSequence += theTileTTL1Cosmics
