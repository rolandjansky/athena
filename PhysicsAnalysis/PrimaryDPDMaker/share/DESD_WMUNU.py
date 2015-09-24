##-----------------------------------------------------------------------------
## Name: DESD_WENU.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: This defines the selection of the ESD
##              containing W->mu nu events only.
##
##-----------------------------------------------------------------------------


## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package and from RecExCommon
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

## This handels multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr


# Import needed utils
from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
from PrimaryDPDMaker.PrimaryDPDMakerConf   import PrimaryDPDPrescaler


##====================================================================
## Define the skimming (event selection) for the  Wmunu DPD output stream
##====================================================================
if primDPD.ApplySkimming() :
    # The primary vertex filter
    from D2PDMaker.D2PDMakerConf import D2PDVertexSelector
    topSequence += D2PDVertexSelector( "DESD_WMUNUStream_VertexFilter",
                                       OutputLevel          = INFO,
                                       inputCollection      = 'VxPrimaryCandidate',
                                       minNumberVxTracks    = 3,
                                       vertexZMin           = -150.0*Units.mm,
                                       vertexZMax           = 150.0*Units.mm,
                                       )

    # The missing Et filter
    from PrimaryDPDMaker.MissingEtFilter import MissingEtFilter
    topSequence += MissingEtFilter( "DESD_WMUNUStream_MissingEtRefFinalFilter",
                                    metCollectionName = "MET_RefFinal",
                                    cutMetMin         = 20.0*Units.GeV
                                    )

    from PrimaryDPDMaker.CompositeMissingEtFilter import CompositeMissingEtFilter
    topSequence += CompositeMissingEtFilter( "DESD_WMUNUStream_MissingEtLocHadTopoFilter",
                                             metCollectionTypeList = [ "MissingET", "MissingET", "MissingET" ],
                                             metCollectionNameList = [ "MET_LocHadTopo", "MET_MuonBoy", "MET_RefMuon_Track" ],
                                             metCombinationMetric  = [ 1.0, 1.0, -1.0 ],
                                             cutMetMin             = 20.0*Units.GeV
                                             )

    # Combine the two different MET selections
    from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
    topSequence += LogicalFilterCombiner( "DESD_WMUNUStream_MissingEtFilter",
                                          cmdstring = "DESD_WMUNUStream_MissingEtRefFinalFilter or DESD_WMUNUStream_MissingEtLocHadTopoFilter"
                                          )


    # Create a Staco muon filter
    from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
    topSequence += D2PDMuonSelector( "DESD_WMUNUStream_StacoMuonFilter",
                                     OutputLevel              = INFO,
                                     inputCollection          = 'StacoMuonCollection',
                                     outputCollection         = 'DESD_WMUNUStream_StacoMuonCollection',
                                     minNumberPassed          = 1,
                                     ptMin                    = 20.0*Units.GeV,
                                     muonRequireIsCombined    = True
                                     )

    # Create a MuID muon filter
    from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
    topSequence += D2PDMuonSelector( "DESD_WMUNUStream_MuidMuonFilter",
                                     OutputLevel              = INFO,
                                     inputCollection          = 'MuidMuonCollection',
                                     outputCollection         = 'DESD_WMUNUStream_MuidMuonCollection',
                                     minNumberPassed          = 1,
                                     ptMin                    = 20.0*Units.GeV,
                                     muonRequireIsCombined    = True
                                     )


    # Create the combined decission with a possible prescale
    from PrimaryDPDMaker.PrimaryDPDMakerConf import PrimaryDPDPrescaler
    topSequence += PrimaryDPDPrescaler( "DESD_WMUNUStream_CombinedPrescaleFilter",
                                        AcceptAlgs  = [ "DESD_WMUNUStream_StacoMuonFilter",
                                                        "DESD_WMUNUStream_MuidMuonFilter" ],
                                        RequireAlgs = [ "DESD_WMUNUStream_MissingEtFilter",
                                                        "DESD_WMUNUStream_VertexFilter" ],
                                        Prescale = primDPD.WriteD2ESD_WMUNUStream.Prescale
                                        )

    
    pass



##====================================================================
## Define the  Wmunu DPD output stream
##====================================================================
streamName = primDPD.WriteD2ESD_WMUNUStream.StreamName
fileName   = buildFileName( primDPD.WriteD2ESD_WMUNUStream )
if primDPD.isVirtual() == False:
    DESD_WMUNUStream = MSMgr.NewPoolStream( streamName, fileName )
    pass
if primDPD.isVirtual() == True:
    DESD_WMUNUStream = MSMgr.NewVirtualStream( streamName, fileName )
    pass

if primDPD.ApplySkimming() :
    DESD_WMUNUStream.AcceptAlgs( [ "DESD_WMUNUStream_CombinedPrescaleFilter" ] )
    pass


#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
# Take everything from the input
ExcludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList(streamName,ExcludeList)


