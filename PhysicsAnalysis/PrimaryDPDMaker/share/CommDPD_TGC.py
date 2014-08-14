# Creation: David Cote, November 2008

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD


# Skim definitions
from PrimaryDPDMaker.PrimaryDPDMakerConf import MuonFilterAlg
topSequence += MuonFilterAlg("TGCCommStream_MuonFilterMuonboy")
topSequence.TGCCommStream_MuonFilterMuonboy.cutPtMinMu       = 0.0*Units.GeV
topSequence.TGCCommStream_MuonFilterMuonboy.minTGCHits       = 2
topSequence.TGCCommStream_MuonFilterMuonboy.cutEtaMax        = 10
topSequence.TGCCommStream_MuonFilterMuonboy.UseTrackContainer = True
topSequence.TGCCommStream_MuonFilterMuonboy.muonContainer = "MuonboyTrackParticles"
topSequence += MuonFilterAlg("TGCCommStream_MuonFilterMoore")
topSequence.TGCCommStream_MuonFilterMoore.cutPtMinMu       = 0.0*Units.GeV
topSequence.TGCCommStream_MuonFilterMoore.minTGCHits       = 2
topSequence.TGCCommStream_MuonFilterMoore.cutEtaMax        = 10
topSequence.TGCCommStream_MuonFilterMoore.UseTrackContainer = True
topSequence.TGCCommStream_MuonFilterMoore.muonContainer = "MooreTrackParticles"
topSequence += MuonFilterAlg("TGCCommStream_MuonFilterMuTagIMO")
topSequence.TGCCommStream_MuonFilterMuTagIMO.cutPtMinMu       = 0.0*Units.GeV
topSequence.TGCCommStream_MuonFilterMuTagIMO.minTGCHits       = 2
topSequence.TGCCommStream_MuonFilterMuTagIMO.cutEtaMax        = 10
topSequence.TGCCommStream_MuonFilterMuTagIMO.UseTrackContainer = True
topSequence.TGCCommStream_MuonFilterMuTagIMO.muonContainer = "MuTagIMOTrackParticles"

# Combine filters logically
from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner

topSequence += LogicalFilterCombiner("TGCCommStream_FilterLogic")
topSequence.TGCCommStream_FilterLogic.prefix = "TGCCommStream"
topSequence.TGCCommStream_FilterLogic.cmdstring = "MuonFilterMuonboy or MuonFilterMoore or MuonFilterMuTagIMO"


# Define the output stream
streamName = primDPD.WriteTGCCommStream.StreamName
fileName   = buildFileName( primDPD.WriteTGCCommStream )
if primDPD.isVirtual() == False:
    dpd=MSMgr.NewPoolStream( streamName, fileName) 
    pass
if primDPD.isVirtual() == True:
    dpd=MSMgr.NewVirtualStream( streamName, fileName) 
    pass


from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
excludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,excludeList)

dpd.AddAcceptAlgs( "TGCCommStream_FilterLogic" )

