# Creation: Claudio Gatti, May 2009

from AthenaCommon.Logging import logging
muonCommDPDStream_msg = logging.getLogger( 'CommDPD_MuonStream' )

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

from PrimaryDPDMaker.PrimaryDPDFlags_MUONCommStream import primMUONCommDPD

##====================================================================
## Write the used options to the log file
##====================================================================
if rec.OutputLevel() <= INFO:
    muonCommDPDStream_msg.info('Values of all CommDPD_Muon flags:')
    print primMUONCommDPD
    pass

logicString=""
FilterList = []
# Define the skimming

if primMUONCommDPD.ApplySkimming() and primMUONCommDPD.DoMuonBoy() :
    from PrimaryDPDMaker.PrimaryDPDMakerConf import MuonFilterAlg
    topSequence += MuonFilterAlg("MUONCommStream_MuonFilterMuonboy")
    topSequence.MUONCommStream_MuonFilterMuonboy.cutPtMinMu       = 0.0*Units.GeV
    topSequence.MUONCommStream_MuonFilterMuonboy.minRPCHits       = 0
    topSequence.MUONCommStream_MuonFilterMuonboy.minTotTrigHits   = 2
    topSequence.MUONCommStream_MuonFilterMuonboy.minMDTHits       = 0
    topSequence.MUONCommStream_MuonFilterMuonboy.cutEtaMax        = 10
    topSequence.MUONCommStream_MuonFilterMuonboy.doz0Cut          = True
    topSequence.MUONCommStream_MuonFilterMuonboy.z0MaxValue       = 300*Units.mm
    topSequence.MUONCommStream_MuonFilterMuonboy.dod0Cut          = True
    topSequence.MUONCommStream_MuonFilterMuonboy.d0MaxValue       = 300*Units.mm
    topSequence.MUONCommStream_MuonFilterMuonboy.UseTrackContainer = True
    topSequence.MUONCommStream_MuonFilterMuonboy.muonContainer = "MuonboyTrackParticles"
    FilterList.append("MUONCommStream_MuonFilterMuonboy")
    if logicString=="":
        logicString=logicString+"MuonFilterMuonboy"
    else:
        logicString=logicString+" or MuonFilterMuonboy"

        
if primMUONCommDPD.ApplySkimming() and primMUONCommDPD.DoMoore() :
    topSequence += MuonFilterAlg("MUONCommStream_MuonFilterMoore")
    topSequence.MUONCommStream_MuonFilterMoore.cutPtMinMu       = 0.0*Units.GeV
    topSequence.MUONCommStream_MuonFilterMoore.minRPCHits       = 0
    topSequence.MUONCommStream_MuonFilterMoore.minTotTrigHits   = 2
    topSequence.MUONCommStream_MuonFilterMoore.minMDTHits       = 0
    topSequence.MUONCommStream_MuonFilterMoore.cutEtaMax        = 10
    topSequence.MUONCommStream_MuonFilterMoore.doz0Cut          = True
    topSequence.MUONCommStream_MuonFilterMoore.z0MaxValue       = 300*Units.mm
    topSequence.MUONCommStream_MuonFilterMoore.dod0Cut          = True
    topSequence.MUONCommStream_MuonFilterMoore.d0MaxValue       = 300*Units.mm
    topSequence.MUONCommStream_MuonFilterMoore.UseTrackContainer = True
    topSequence.MUONCommStream_MuonFilterMoore.muonContainer = "MuidExtrTrackParticles"
    FilterList.append("MUONCommStream_MuonFilterMoore")
    if logicString=="":
        logicString=logicString+"MuonFilterMoore"
    else:
        logicString=logicString+" or MuonFilterMoore"

if primMUONCommDPD.ApplySkimming() and primMUONCommDPD.DoMuTagIMO() :
    topSequence += MuonFilterAlg("MUONCommStream_MuonFilterMuTagIMO")
    topSequence.MUONCommStream_MuonFilterMuTagIMO.cutPtMinMu       = 0.0*Units.GeV
    topSequence.MUONCommStream_MuonFilterMuTagIMO.minRPCHits       = 0
    topSequence.MUONCommStream_MuonFilterMuTagIMO.minTotTrigHits   = 2
    topSequence.MUONCommStream_MuonFilterMuTagIMO.minMDTHits       = 0
    topSequence.MUONCommStream_MuonFilterMuTagIMO.cutEtaMax        = 10
    topSequence.MUONCommStream_MuonFilterMuTagIMO.doz0Cut          = True
    topSequence.MUONCommStream_MuonFilterMuTagIMO.z0MaxValue       = 300*Units.mm
    topSequence.MUONCommStream_MuonFilterMuTagIMO.dod0Cut          = True
    topSequence.MUONCommStream_MuonFilterMuTagIMO.d0MaxValue       = 300*Units.mm
    topSequence.MUONCommStream_MuonFilterMuTagIMO.UseTrackContainer = True
    topSequence.MUONCommStream_MuonFilterMuTagIMO.muonContainer = "MuTagIMOTrackParticles"
    FilterList.append("MUONCommStream_MuonFilterMuTagIMO")
    if logicString=="":
        logicString=logicString+"MuonFilterMuTagIMO"
    else:
        logicString=logicString+" or MuonFilterMuTagIMO"


if primMUONCommDPD.ApplySkimming() and primMUONCommDPD.DoTrackID() :
    topSequence += MuonFilterAlg("MUONCommStream_MuonFilterTrackID")
    topSequence.MUONCommStream_MuonFilterTrackID.cutPtMinMu       = 0.0*Units.GeV
    topSequence.MUONCommStream_MuonFilterTrackID.minSCTHits       = 0
    topSequence.MUONCommStream_MuonFilterTrackID.minTRTHits       = 0
    topSequence.MUONCommStream_MuonFilterTrackID.minPIXELHits     = 0
    topSequence.MUONCommStream_MuonFilterTrackID.minTOTSIHits     = 1
    topSequence.MUONCommStream_MuonFilterTrackID.cutEtaMax        = 10
    topSequence.MUONCommStream_MuonFilterTrackID.doz0Cut          = True
    topSequence.MUONCommStream_MuonFilterTrackID.z0MaxValue       = 300*Units.mm
    topSequence.MUONCommStream_MuonFilterTrackID.dod0Cut          = True
    topSequence.MUONCommStream_MuonFilterTrackID.d0MaxValue       = 300*Units.mm
    topSequence.MUONCommStream_MuonFilterTrackID.UseTrackContainer = True
    topSequence.MUONCommStream_MuonFilterTrackID.muonContainer = "TrackParticleCandidate"
    FilterList.append("MUONCommStream_MuonFilterTrackID")
    if logicString=="":
        logicString=logicString+"MuonFilterTrackID"
    else:
        logicString=logicString+" or MuonFilterTrackID"

if primMUONCommDPD.ApplySkimming():
    # Combine filters logically
    from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner    
    topSequence += LogicalFilterCombiner("MUONCommStream_FilterLogic")
    topSequence.MUONCommStream_FilterLogic.prefix = "MUONCommStream"
    topSequence.MUONCommStream_FilterLogic.cmdstring = logicString
    #topSequence.MUONCommStream_FilterLogic.cmdstring = "MuonFilterMuonboy or MuonFilterMoore or MuonFilterMuTagIMO or MuonFilterTrackID"



# Define the output stream
streamName = primDPD.WriteMUONCommStream.StreamName
fileName   = buildFileName( primDPD.WriteMUONCommStream )
if primDPD.isVirtual() == False:
    dpd=MSMgr.NewPoolStream( streamName, fileName ) 
    pass
if primDPD.isVirtual() == True:
    dpd=MSMgr.NewVirtualStream( streamName, fileName ) 
    pass    

from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
#excludeList=["CaloCellContainer#AllCalo"]
excludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList( streamName,excludeList )

if primMUONCommDPD.ApplySkimming():
    dpd.AddAcceptAlgs( "MUONCommStream_FilterLogic" )
    dpd.AddOtherAlgsToBookkeep( FilterList )
    #dpd.AddOtherAlgsToBookkeep( ["MUONCommStream_MuonFilterMuonboy", "MUONCommStream_MuonFilterMoore","MUONCommStream_MuonFilterMuTagIMO","MUONCommStream_MuonFilterTrackID"] )

