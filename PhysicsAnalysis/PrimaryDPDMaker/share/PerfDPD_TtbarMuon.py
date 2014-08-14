##-----------------------------------------------------------------------------
## Name: PrimaryDPD_TtbarMuonStream.py
##       (was PrimaryDPD_TightMuonStream.py)
##
## Author: David Lopez Mateos (Columbia University/Caltech)
## Email:  David.Lopez@cern.ch
##
## Description: This defines the content of the Tight Muon DPD output stream.
##
##-----------------------------------------------------------------------------

# If this file is already included, don't include it again
include.block("PrimaryDPDMaker/PerfDPD_TtbarMuon.py")


## for messaging
from AthenaCommon.Logging import logging
muonDPDStream_msg = logging.getLogger( 'PrimaryDPD_TtbarMuonStream' )

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from RecExConfig.RecFlags import rec
## Include the job property flags for this package
from PrimaryDPDMaker.PrimaryDPDFlags_TtbarMuonStream import primDPDttbarMu

## This handels multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

###
#from JetCalibTools.JetCalibToolsConf import *
#from JetCalibTools.MakeCalibSequences import *
#from JetCalibTools.JetRecalibrator import makeJetRecalibrator
#from ApplyJetCalibration.SetupAthenaCalibration import doApplyJES
#makeJetRecalibrator("AntiKt4LCTopoJets", "LC:ApplyAreaOffsetJES")
###

##====================================================================
## Write the used options to the log file
##====================================================================
if rec.OutputLevel() <= INFO:
    muonDPDStream_msg.info('Values of all PerfDPD_TtbarMuon flags:')
    print primDPDttbarMu
    pass



# ============================================================================
# Check which muon collection are available
# ============================================================================
from RecExConfig.ObjKeyStore import objKeyStore
hasMuons = False
hasStacoMuons = False
hasMuidMuons = False
if objKeyStore.isInInput("Analysis::MuonContainer", "Muons") \
       or objKeyStore['transient'].has_item("Analysis::MuonContainer#Muons"):
    hasMuons = True
    pass
if objKeyStore.isInInput("Analysis::MuonContainer", "StacoMuonCollection") \
       or objKeyStore['transient'].has_item("Analysis::MuonContainer#StacoMuonCollection"):
    hasStacoMuons = True
    pass
if objKeyStore.isInInput("Analysis::MuonContainer", "MuidMuonCollection") \
       or objKeyStore['transient'].has_item("Analysis::MuonContainer#MuidMuonCollection"):
    hasMuidMuons = True
    pass


# ============================================================================
# Create a sub-sequence where all needed algorithms will run in
# ============================================================================
import AthenaCommon.CfgMgr as CfgMgr
desdTtbarMuonSequence = CfgMgr.AthSequencer( 'DESD_SLTTMU_Sequence',
                                              StopOverride = True )
topSequence += desdTtbarMuonSequence


##============================================================================
## Define the skimming (event selection) for the DESD_SLTTMU output stream
##============================================================================
include( "MuonIsolationTools/MuonIsolationTool_jobOptions.py" )

from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
from PrimaryDPDMaker.PrimaryDPDMakerConf   import PrimaryDPDPrescaler
from D2PDMaker.D2PDMakerConf import D2PDElectronSelector
from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
from D2PDMaker.D2PDMakerConf import D2PDTauSelector
from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
from D2PDMaker.D2PDMakerConf import D2PDMissingEtSelector
from D2PDMaker.D2PDMakerConf import D2PDJetSelector

        
# Define the some lists
commandstring = ""
algsToBookkeep = []



from RecExConfig.ObjKeyStore import objKeyStore
muonDPDStream_msg.debug("Items in objKeyStore[transient]: %s" %  objKeyStore['transient'].list() )
#print "Items in objKeyStore[transient]:", objKeyStore['transient'].list()


# ------------------------
# Create the modified ttbar filter
# ------------------------
if primDPDttbarMu.ModifiedTTBarFilter2():
    # Create a list of accept algs for the tau filters
    ttbarAcceptAlgsList = []
    
    # Create a Staco muon filter
    if hasStacoMuons:
        desdTtbarMuonSequence += D2PDMuonSelector( "TtbarMuonStream_ModifiedTTBarStacoMuonFilter",
                                                    OutputLevel                       = INFO,
                                                    inputCollection                   = 'StacoMuonCollection',
                                                    minNumberPassed                   = 1,
                                                    ptMin                             = primDPDttbarMu.ModifiedTTBarFilter2.MuonEt,
                                                    muonParametersList                = [ 17 ],
                                                    muonParametersMinList             = [ -100000000.0 ],
                                                    muonParametersMaxList             = [ primDPDttbarMu.ModifiedTTBarFilter2.AbsoluteMuonIso ],
                                                    muonParametersUseRelativeCutsList = [ False ]
                                                    )
        ttbarAcceptAlgsList.append("TtbarMuonStream_ModifiedTTBarStacoMuonFilter")
        pass
    
    # Check if we have the Muons chain collection available or not
    
    # Combine the three muon algorithms
    desdTtbarMuonSequence += PrimaryDPDPrescaler( "TtbarMuonStream_ModifiedTTBarCombinedMuonPrescaleFilter",
                                                   AcceptAlgs  = ttbarAcceptAlgsList,
                                                   Prescale = 1
                                                   )
    desdTtbarMuonSequence += D2PDJetSelector( "TtbarMuonStream_ModifiedTTBarJetFilter",
                                               OutputLevel          = INFO,
                                               inputCollection      = primDPDttbarMu.ModifiedTTBarFilter2.JetCollection,
                                               #outputLinkCollection = 'DESD_SLTTMUStream_ModifiedTTBarJetLinkCollection',
                                               minNumberPassed      = 1,
                                               ptMin                = primDPDttbarMu.ModifiedTTBarFilter2.JetPt,
                                               absEtaMax            = primDPDttbarMu.ModifiedTTBarFilter2.JetEta,
                                               jetFlavourTagWeightMin = 0.7892,
                                               jetFlavourTagName = "MV1"
                                               )

    # Define the prescaled filters; configurable with command line argument
    desdTtbarMuonSequence += PrimaryDPDPrescaler( "TtbarMuonStream_ModifiedTTBarMuonPrescaleFilter",
                                                   RequireAlgs = [ "TtbarMuonStream_ModifiedTTBarCombinedMuonPrescaleFilter",
                                                                   "TtbarMuonStream_ModifiedTTBarJetFilter" ],
                                                   Prescale    = primDPDttbarMu.ModifiedTTBarFilter2.Prescale )
    
    # Add the per-event bookkeeping of this filer
    algsToBookkeep.append("TtbarMuonStream_ModifiedTTBarMuonPrescaleFilter")
    if commandstring=="":
        commandstring = "TtbarMuonStream_ModifiedTTBarMuonPrescaleFilter"
        pass
    else:
        commandstring = commandstring + " or TtbarMuonStream_ModifiedTTBarMuonPrescaleFilter"
        pass
    pass


# Create the combined decision
desdTtbarMuonSequence += LogicalFilterCombiner( "StreamDESD_DESD_SLTTMU_AcceptEvent",
                                                 cmdstring = commandstring )
from PrimaryDPDMaker.PrimaryDPDFlags import desdEventSkimmingFilterNamesList
desdEventSkimmingFilterNamesList.append( "StreamDESD_DESD_SLTTMU_AcceptEvent" )



##====================================================================
## Define the Single Muon DPD output stream
##====================================================================
streamName = primDPD.WriteTtbarMuonStream.StreamName
fileName   = buildFileName( primDPD.WriteTtbarMuonStream )
if primDPD.WriteTtbarMuonStream.isVirtual or primDPD.isVirtual() :
    TtbarMuonStream = MSMgr.NewVirtualStream( streamName, fileName )
    pass
else:
    TtbarMuonStream = MSMgr.NewPoolStream( streamName, fileName )
    # Add the per-event bookkeeping 
    TtbarMuonStream.AddOtherAlgsToBookkeep( algsToBookkeep )
    pass

if primDPD.ApplySkimming() and primDPDttbarMu.ApplySkimming() :
    TtbarMuonStream.AcceptAlgs( [ "StreamDESD_DESD_SLTTMU_AcceptEvent"] )
    pass



#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
#old way
#TtbarMuonStream.Stream.TakeItemsFromInput = True
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take everything from the input
ExcludeList=[]

dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,ExcludeList)

#TtbarMuonStream.AddItem( ["egammaContainer#ElectronAODCollection"] )


