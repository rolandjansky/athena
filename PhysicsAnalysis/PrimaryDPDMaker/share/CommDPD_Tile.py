# Creation: Jose Maneira, November 2008

# Skimming
from PrimaryDPDMaker.TileTrackFilter import TileTrackFilter
topSequence += TileTrackFilter("TileCommStream_TileTrackFilter")
topSequence.TileCommStream_TileTrackFilter.doEnergyCut         = True
topSequence.TileCommStream_TileTrackFilter.cutFullEnergyMin    = 1.*Units.GeV
topSequence.TileCommStream_TileTrackFilter.doNTracksCut        = True
topSequence.TileCommStream_TileTrackFilter.cutNTracksMin       = 1

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD

streamName = primDPD.WriteTileCommStream.StreamName
fileName   = buildFileName( primDPD.WriteTileCommStream )
if primDPD.isVirtual() == False:
    dpd=MSMgr.NewPoolStream( streamName, fileName ) 
    pass
if primDPD.isVirtual() == True:
    dpd=MSMgr.NewVirtualStream( streamName, fileName ) 
    pass    

from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
excludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList( streamName,excludeList )

dpd.AddAcceptAlgs( "TileCommStream_TileTrackFilter" )


