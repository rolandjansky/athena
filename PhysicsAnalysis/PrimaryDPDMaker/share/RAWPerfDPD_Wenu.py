##-----------------------------------------------------------------------------
## Name: RAWPerfDPD_Weny.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: This defines the selection of the ByteStream (RAW) format DPD
##              containing W->enu events only.
##
##-----------------------------------------------------------------------------


## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package and from RecExCommon
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

## This handels multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr


# ---- Load the egammaPID and egammaParameters information
# This is needed to always be up-to-date with the egamma
# IsEM selections and also the author selections
import PyUtils.RootUtils as ru
ROOT = ru.import_root()
import PyCintex
PyCintex.loadDictionary('egammaEnumsDict')
from ROOT import egammaPID
from ROOT import egammaParameters


##====================================================================
## Define the skimming (event selection) for the RAW Wenu DPD output stream
##====================================================================
if primDPD.ApplySkimming():
    # The missing Et filters
    from PrimaryDPDMaker.MissingEtFilter import MissingEtFilter
    topSequence += MissingEtFilter( "DRAW_WENUStream_MissingEtRefFinalFilter",
                                    metCollectionName = "MET_RefFinal",
                                    cutMetMin         = 20.0*Units.GeV
                                    )

    from PrimaryDPDMaker.CompositeMissingEtFilter import CompositeMissingEtFilter
    topSequence += CompositeMissingEtFilter( "DRAW_WENUStream_MissingEtLocHadTopoFilter",
                                             metCollectionTypeList = [ "MissingET", "MissingET", "MissingET" ],
                                             metCollectionNameList = [ "MET_LocHadTopo", "MET_MuonBoy", "MET_RefMuon_Track" ],
                                             metCombinationMetric  = [ 1.0, 1.0, -1.0 ],
                                             cutMetMin             = 20.0*Units.GeV
                                             )

    # Combine the two different MET selections
    from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
    topSequence += LogicalFilterCombiner( "DRAW_WENUStream_MissingEtFilter",
                                          cmdstring = "DRAW_WENUStream_MissingEtRefFinalFilter or DRAW_WENUStream_MissingEtLocHadTopoFilter"
                                          )

    # The electron filter
    from D2PDMaker.D2PDMakerConf import D2PDElectronSelector
    topSequence += D2PDElectronSelector( "DRAW_WENUStream_ElectronFilter",
                                         OutputLevel            = INFO,
                                         inputCollection        = 'ElectronAODCollection',
                                         outputCollection       = 'DRAW_WENUStream_ElectronCollection',
                                         minNumberPassed        = 1,
                                         electronVetoAuthorList = [ egammaParameters.AuthorFrwd ],
                                         electronIsEM           = egammaPID.ElectronMedium,
                                         clusterEtMin           = 20.0*Units.GeV
                                         )


    # Create the combined decission with a possible prescale
    from PrimaryDPDMaker.PrimaryDPDMakerConf import PrimaryDPDPrescaler
    topSequence += PrimaryDPDPrescaler( "DRAW_WENUStream_CombinedPrescaleFilter",
                                        RequireAlgs = [ "DRAW_WENUStream_MissingEtFilter", "DRAW_WENUStream_ElectronFilter" ],
                                        Prescale    = primDPD.WriteRAWPerfDPD_WENU.Prescale )

    pass



##====================================================================
## Define the RAW Wenu DPD output stream
##====================================================================
streamName = primDPD.WriteRAWPerfDPD_WENU.StreamName
fileName   = buildFileName( primDPD.WriteRAWPerfDPD_WENU )
# Remove the .pool.root ending in the file name, this is a RAW file!
if fileName.endswith(".pool.root") :
    fileName = fileName.rstrip(".pool.root")
    pass
DRAW_WENUStream = MSMgr.NewByteStream( streamName, fileName )

# Don't write an output RAW file if it is empty
DRAW_WENUStream.bsOutputSvc.WriteEventlessFiles = primDPD.WriteEventlessFiles()

# set max output file size to 10 GB
DRAW_WENUStream.bsOutputSvc.MaxFileMB = primDPD.OutputRAWMaxFileSize()
DRAW_WENUStream.bsOutputSvc.MaxFileNE = primDPD.OutputRAWMaxNEvents()


# Assign some meta data
from RecExConfig.InputFilePeeker import inputFileSummary
bsMetadataDict = inputFileSummary['bs_metadata']
inputStreamType = bsMetadataDict['Stream'].split("_")[0]
inputStreamName = bsMetadataDict['Stream'].split("_")[1]
DRAW_WENUStream.bsOutputSvc.StreamType = inputStreamType
DRAW_WENUStream.bsOutputSvc.StreamName = inputStreamName
DRAW_WENUStream.bsOutputSvc.ProjectTag = bsMetadataDict['Project']


if primDPD.ApplySkimming() :
    DRAW_WENUStream.AddRequireAlgs( [ "DRAW_WENUStream_CombinedPrescaleFilter" ] )
    pass

