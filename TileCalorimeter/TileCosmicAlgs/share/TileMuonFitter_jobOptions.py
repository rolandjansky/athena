#==============================================================
#
# Job options file : TileMuonFitter
# Read CaloCells and create TileCosmicMuon
# Fits cosmic muon track
#
#==============================================================
#----------
# Set various options
#----------
from CaloRec.CaloRecFlags import jobproperties
from TileRecUtils.TileRecFlags import jobproperties

if jobproperties.Beam.beamType == 'collisions':
    tmf_beamType = "collisions";
if jobproperties.Beam.beamType == 'singlebeam':
    tmf_beamType = "singlebeam";
if jobproperties.Beam.beamType == 'cosmics': 
    tmf_beamType = "cosmics";

if not 'ComTimeKeyTMF' in dir():
    ComTimeKeyTMF="ComTimeTileMuon"

if not 'doTMFMethod' in dir():
    doTMFMethod = 'Hough'
	
	
#==============================================================
#
# Set up properties common to both methods
#
#==============================================================
#
from TileCosmicAlgs.TileCosmicMuonGetter import *
theTileCosmicMuonGetter = TileCosmicMuonGetter()

if theTileCosmicMuonGetter and theTileCosmicMuonGetter.configured() :
    theTileCosmicMuonGetter.TileMuonFitter().ComTimeKey = ComTimeKeyTMF
    theTileCosmicMuonGetter.TileMuonFitter().EThreshold = 250.0
    theTileCosmicMuonGetter.TileMuonFitter().BeamType   = tmf_beamType
    theTileCosmicMuonGetter.TileMuonFitter().CaloCellContainer = 'AllCalo'
        
    theTileCosmicMuonGetter.TileMuonFitter().OutputLevel= INFO
    
    #==============================================================
    #
    # By default, always use Hough Transform unlesse specified otherwise
    #
    #==============================================================
    #
    if (doTMFMethod == 'Minuit'):
        theTileCosmicMuonGetter.TileMuonFitter().DoHoughTransform = False
        theTileCosmicMuonGetter.TileMuonFitter().TileCosmicMuonKey = "TileCosmicMuonMF"
    else:
        theTileCosmicMuonGetter.TileMuonFitter().DoHoughTransform = True
        theTileCosmicMuonGetter.TileMuonFitter().TileCosmicMuonKey = "TileCosmicMuonHT"
    print theTileCosmicMuonGetter.TileMuonFitter()
    
    
    #==============================================================
    #
    # Use Minuit as second method if both are requested
    #
    #==============================================================
    #
    if (doTMFMethod == 'Both'):
        from TileCosmicAlgs.TileCosmicAlgsConf import TileMuonFitter
        theTileMuonFitter2 = TileMuonFitter("TileMuonFitterMF",TileCosmicMuonKey = "TileCosmicMuonMF")
        theTileMuonFitter2.DoHoughTransform = False
        theTileMuonFitter2.ComTimeKey = "ComTimeTileMuonMF"
        theTileMuonFitter2.EThreshold = 250.0
        theTileMuonFitter2.BeamType   = tmf_beamType
        theTileMuonFitter2.CaloCellContainer = 'AllCalo'
        theTileMuonFitter2.OutputLevel= INFO
        topSequence += theTileMuonFitter2
        print theTileMuonFitter2
