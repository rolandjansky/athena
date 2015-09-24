##-----------------------------------------------------------------------------
## Name: RAWPerfDPD_Wmunu.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: This defines the selection of the ByteStream (RAW) format DPD
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
## Define the skimming (event selection) for the RAW Wmunu DPD output stream
##====================================================================
if primDPD.ApplySkimming() :
    # The primary vertex filter
    from D2PDMaker.D2PDMakerConf import D2PDVertexSelector
    topSequence += D2PDVertexSelector( "DRAW_WMUNUStream_VertexFilter",
                                       OutputLevel          = INFO,
                                       inputCollection      = 'VxPrimaryCandidate',
                                       minNumberVxTracks    = 3,
                                       vertexZMin           = -150.0*Units.mm,
                                       vertexZMax           = 150.0*Units.mm,
                                       )

    # The missing Et filter
    from PrimaryDPDMaker.MissingEtFilter import MissingEtFilter
    topSequence += MissingEtFilter( "DRAW_WMUNUStream_MissingEtRefFinalFilter",
                                    metCollectionName = "MET_RefFinal",
                                    cutMetMin         = 20.0*Units.GeV
                                    )

    from PrimaryDPDMaker.CompositeMissingEtFilter import CompositeMissingEtFilter
    topSequence += CompositeMissingEtFilter( "DRAW_WMUNUStream_MissingEtLocHadTopoFilter",
                                             metCollectionTypeList = [ "MissingET", "MissingET", "MissingET" ],
                                             metCollectionNameList = [ "MET_LocHadTopo", "MET_MuonBoy", "MET_RefMuon_Track" ],
                                             metCombinationMetric  = [ 1.0, 1.0, -1.0 ],
                                             cutMetMin             = 20.0*Units.GeV
                                             )

    # Combine the two different MET selections
    from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
    topSequence += LogicalFilterCombiner( "DRAW_WMUNUStream_MissingEtFilter",
                                          cmdstring = "DRAW_WMUNUStream_MissingEtRefFinalFilter or DRAW_WMUNUStream_MissingEtLocHadTopoFilter"
                                          )


    # Create a Staco muon filter
    from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
    topSequence += D2PDMuonSelector( "DRAW_WMUNUStream_StacoMuonFilter",
                                     OutputLevel              = INFO,
                                     inputCollection          = 'StacoMuonCollection',
                                     outputCollection         = 'DRAW_WMUNUStream_StacoMuonCollection',
                                     minNumberPassed          = 1,
                                     ptMin                    = 20.0*Units.GeV,
                                     muonRequireIsCombined    = True
                                     )

    # Create a MuID muon filter
    from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
    topSequence += D2PDMuonSelector( "DRAW_WMUNUStream_MuidMuonFilter",
                                     OutputLevel              = INFO,
                                     inputCollection          = 'MuidMuonCollection',
                                     outputCollection         = 'DRAW_WMUNUStream_MuidMuonCollection',
                                     minNumberPassed          = 1,
                                     ptMin                    = 20.0*Units.GeV,
                                     muonRequireIsCombined    = True
                                     )


    # Create the combined decission with a possible prescale
    from PrimaryDPDMaker.PrimaryDPDMakerConf import PrimaryDPDPrescaler
    topSequence += PrimaryDPDPrescaler( "DRAW_WMUNUStream_CombinedPrescaleFilter",
                                        AcceptAlgs  = [ "DRAW_WMUNUStream_StacoMuonFilter",
                                                        "DRAW_WMUNUStream_MuidMuonFilter" ],
                                        RequireAlgs = [ "DRAW_WMUNUStream_MissingEtFilter",
                                                        "DRAW_WMUNUStream_VertexFilter" ],
                                        Prescale = primDPD.WriteRAWPerfDPD_WMUNU.Prescale
                                        )

    
    pass



##====================================================================
## Define the RAW Wmunu DPD output stream
##====================================================================
streamName = primDPD.WriteRAWPerfDPD_WMUNU.StreamName
fileName   = buildFileName( primDPD.WriteRAWPerfDPD_WMUNU )
# Remove the .pool.root ending in the file name, this is a RAW file!
if fileName.endswith(".pool.root") :
    fileName = fileName.rstrip(".pool.root")
    pass
DRAW_WMUNUStream = MSMgr.NewByteStream( streamName, fileName )

# Don't write an output RAW file if it is empty
DRAW_WMUNUStream.bsOutputSvc.WriteEventlessFiles = primDPD.WriteEventlessFiles()

#set max output file size to 10 GB
DRAW_WMUNUStream.bsOutputSvc.MaxFileMB = primDPD.OutputRAWMaxFileSize()
DRAW_WMUNUStream.bsOutputSvc.MaxFileNE = primDPD.OutputRAWMaxNEvents()


# Assign some meta data
from RecExConfig.InputFilePeeker import inputFileSummary
bsMetadataDict = inputFileSummary['bs_metadata']
inputStreamType = bsMetadataDict['Stream'].split("_")[0]
inputStreamName = bsMetadataDict['Stream'].split("_")[1]
DRAW_WMUNUStream.bsOutputSvc.StreamType = inputStreamType
DRAW_WMUNUStream.bsOutputSvc.StreamName = inputStreamName
DRAW_WMUNUStream.bsOutputSvc.ProjectTag = bsMetadataDict['Project']



if primDPD.ApplySkimming() :
    DRAW_WMUNUStream.AddRequireAlgs( [ "DRAW_WMUNUStream_CombinedPrescaleFilter" ] )
    pass

