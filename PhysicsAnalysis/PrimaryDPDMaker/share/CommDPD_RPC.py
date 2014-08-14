# Creation: David Cote, November 2008

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD


# Define the skimming
from PrimaryDPDMaker.PrimaryDPDMakerConf import MuonFilterAlg
topSequence += MuonFilterAlg("RPCCommStream_MuonFilterMuonboy")
topSequence.RPCCommStream_MuonFilterMuonboy.cutPtMinMu       = 0.0*Units.GeV
topSequence.RPCCommStream_MuonFilterMuonboy.minRPCHits       = 3
topSequence.RPCCommStream_MuonFilterMuonboy.minMDTHits       = 15
topSequence.RPCCommStream_MuonFilterMuonboy.cutEtaMax        = 10
topSequence.RPCCommStream_MuonFilterMuonboy.UseTrackContainer = True
topSequence.RPCCommStream_MuonFilterMuonboy.muonContainer = "MuonboyTrackParticles"
topSequence += MuonFilterAlg("RPCCommStream_MuonFilterMoore")
topSequence.RPCCommStream_MuonFilterMoore.cutPtMinMu       = 0.0*Units.GeV
topSequence.RPCCommStream_MuonFilterMoore.minRPCHits       = 3
topSequence.RPCCommStream_MuonFilterMoore.minMDTHits       = 15
topSequence.RPCCommStream_MuonFilterMoore.cutEtaMax        = 10
topSequence.RPCCommStream_MuonFilterMoore.UseTrackContainer = True
topSequence.RPCCommStream_MuonFilterMoore.muonContainer = "MooreTrackParticles"
topSequence += MuonFilterAlg("RPCCommStream_MuonFilterMuTagIMO")
topSequence.RPCCommStream_MuonFilterMuTagIMO.cutPtMinMu       = 0.0*Units.GeV
topSequence.RPCCommStream_MuonFilterMuTagIMO.minRPCHits       = 3
topSequence.RPCCommStream_MuonFilterMuTagIMO.minMDTHits       = 15
topSequence.RPCCommStream_MuonFilterMuTagIMO.cutEtaMax        = 10
topSequence.RPCCommStream_MuonFilterMuTagIMO.UseTrackContainer = True
topSequence.RPCCommStream_MuonFilterMuTagIMO.muonContainer = "MuTagIMOTrackParticles"
    
# Combine filters logically
from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner

topSequence += LogicalFilterCombiner("RPCCommStream_FilterLogic")
topSequence.RPCCommStream_FilterLogic.prefix = "RPCCommStream"
topSequence.RPCCommStream_FilterLogic.cmdstring = "MuonFilterMuonboy or MuonFilterMoore or MuonFilterMuTagIMO"


# Define the output stream
streamName = primDPD.WriteRPCCommStream.StreamName
fileName   = buildFileName( primDPD.WriteRPCCommStream )
if primDPD.isVirtual() == False:
    dpd=MSMgr.NewPoolStream( streamName, fileName ) 
    pass
if primDPD.isVirtual() == True:
    dpd=MSMgr.NewVirtualStream( streamName, fileName ) 
    pass    

from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
excludeList=["CaloCellContainer#AllCalo"]
dpdOutput.addAllItemsFromInputExceptExcludeList( streamName,excludeList )

dpd.AddAcceptAlgs( "RPCCommStream_FilterLogic" )

