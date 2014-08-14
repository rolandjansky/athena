##-----------------------------------------------------------------------------
## Name: RAWPerfDPD_Zmumu.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: This defines the selection of the ByteStream (RAW) format DPD
##              containing Z->mu mu events only.
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


# The list for the algorithm instance names to use for event filtering in OR
acceptAlgList = []


##====================================================================
## Define the skimming (event selection) for the Photon-Jet DPD output stream
##====================================================================
if primDPD.ApplySkimming() :
    # Create empyt lists for the different tools
    augmentationToolList = []
    acceptAlgList = []

    ToolSvc += CfgMgr.CutAlg( "DRAW_ZMUMUStream_VertexFilter",
                              # OutputLevel = VERBOSE,
                              Cut = "count( (PrimaryVertices.nTrackParticles >= 3) && (abs(PrimaryVertices.z)<=150*mm) ) >= 1" )



    # Create a third chain muon particle selector
    ToolSvc += CfgMgr.ParticleSelectionTool( "DRAW_ZMUMUStream_MuonsSelector",
                                             # OutputLevel         = VERBOSE,
                                             InputContainer      = "Muons",
                                             OutputLinkContainer = "DRAW_ZMUMUStream_MuonsLinkCollection",
                                             Selection           = "(Muons.pt > 25.0*GeV)"
                                             #Selection           = "( (Muons.quality>0.5) && (Muons.quality<1.5) && (Muons.pt > 25.0*GeV) )"
                                             #Selection           = "( (Muons.quality==1) && (Muons.pt > 25.0*GeV) )"
                                             )
    augmentationToolList.append( ToolSvc.DRAW_ZMUMUStream_MuonsSelector )

    # Create the standard Z -> mu mu builder (Muons)
    ToolSvc += CfgMgr.ParticleCombinerTool( "DRAW_ZMUMUStream_ZmumuMuonsSelector",
                                            # OutputLevel        = VERBOSE,
                                            InputContainerList = ["DRAW_ZMUMUStream_MuonsLinkCollection",
                                                                  "DRAW_ZMUMUStream_MuonsLinkCollection"],
                                            OutputContainer    = "DRAW_ZMUMUStream_ZmumuMuonsBosonCollection",
                                            SetPdgId           = 23
                                            )
    augmentationToolList.append( ToolSvc.DRAW_ZMUMUStream_ZmumuMuonsSelector )


    # Check if we have the calo muons available or not
    hasCaloMuon = False
    from RecExConfig.ObjKeyStore import objKeyStore
    if objKeyStore.isInInput("xAOD::MuonContainer", "CaloMuonCollection") \
           or objKeyStore['transient'].has_item("xAOD::MuonContainer#CaloMuonCollection"):

        hasCaloMuon =  True

        # Select the calo muons
        ToolSvc += CfgMgr.ParticleSelectionTool( "DRAW_ZMUMUStream_CaloMuonSelector",
                                                 # OutputLevel         = VERBOSE,
                                                 InputContainer      = "CaloMuonCollection",
                                                 OutputLinkContainer = "DRAW_ZMUMUStream_CaloMuonLinkCollection",
                                                 Selection           = "(CaloMuonCollection.pt > 25.0*GeV)"
                                                 )
        augmentationToolList.append( ToolSvc.DRAW_ZMUMUStream_CaloMuonSelector )

        # Create the standard Z -> mu mu builder (Muons+CaloMuon)
        ToolSvc += CfgMgr.ParticleCombinerTool( "DRAW_ZMUMUStream_ZmumuMuonsCaloSelector",
                                                # OutputLevel        = VERBOSE,
                                                InputContainerList = ["DRAW_ZMUMUStream_MuonsLinkCollection",
                                                                      "DRAW_ZMUMUStream_CaloMuonLinkCollection"],
                                                OutputContainer    = "DRAW_ZMUMUStream_ZmumuMuonsCaloBosonCollection",
                                                SetPdgId           = 23
                                                )
        augmentationToolList.append( ToolSvc.DRAW_ZMUMUStream_ZmumuMuonsCaloSelector )
        pass # End of calo muon selection


    # The name of the kernel (DRAW_ZMUMUStream_Kernel in this case) must be unique to this derivation
    from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
    topSequence += CfgMgr.DerivationFramework__DerivationKernel("DRAW_ZMUMUStream_Kernel",
                                                                AugmentationTools = augmentationToolList
                                                                #SkimmingTools = [ToolSvc.DRAW_ZEEStream_ZeeFilter],
                                                                )


    # A temporary solution
    from PrimaryDPDMaker.MassFilter import MassFilter
    topSequence += MassFilter( "DRAW_ZMUMUStream_ZmumuMuonsFilter",
                               #OutputLevel = VERBOSE,
                               InputContainer = "DRAW_ZMUMUStream_ZmumuMuonsBosonCollection",
                               MinMassCut     = 55.0*Units.GeV
                               )
    acceptAlgList.append( "DRAW_ZMUMUStream_ZmumuMuonsFilter" )

    if hasCaloMuon :
        topSequence += MassFilter( "DRAW_ZMUMUStream_ZmumuMuonsCaloFilter",
                                   #OutputLevel = VERBOSE,
                                   InputContainer = "DRAW_ZMUMUStream_ZmumuMuonsCaloBosonCollection",
                                   MinMassCut     = 55.0*Units.GeV
                                   )
        acceptAlgList.append( "DRAW_ZMUMUStream_ZmumuMuonsCaloFilter" )
        pass # End: hasCaloMuon


    # Create the combined decission with a possible prescale
    from PrimaryDPDMaker.PrimaryDPDMakerConf   import PrimaryDPDPrescaler
    topSequence += PrimaryDPDPrescaler( "DRAW_ZMUMUStream_CombinedPrescaleFilter",
                                        AcceptAlgs  = acceptAlgList,
                                        RequireAlgs = [ "DRAW_ZMUMUStream_VertexFilter" ],
                                        Prescale    = primDPD.WriteRAWPerfDPD_ZMUMU.Prescale
                                        )
    pass


##====================================================================
## Define the RAW Zmumu DPD output stream
##====================================================================
streamName = primDPD.WriteRAWPerfDPD_ZMUMU.StreamName
fileName   = buildFileName( primDPD.WriteRAWPerfDPD_ZMUMU )
# Remove the .pool.root ending in the file name, this is a RAW file!
if fileName.endswith(".pool.root") :
    fileName = fileName.rstrip(".pool.root")
    pass
DRAW_ZMUMUStream = MSMgr.NewByteStream( streamName, fileName )

# Don't write an output RAW file if it is empty
DRAW_ZMUMUStream.bsOutputSvc.WriteEventlessFiles = primDPD.WriteEventlessFiles()

#set max output file size to 10 GB
DRAW_ZMUMUStream.bsOutputSvc.MaxFileMB = primDPD.OutputRAWMaxFileSize()
DRAW_ZMUMUStream.bsOutputSvc.MaxFileNE = primDPD.OutputRAWMaxNEvents()


# Assign some meta data
from RecExConfig.InputFilePeeker import inputFileSummary
bsMetadataDict = inputFileSummary['bs_metadata']
inputStreamType = bsMetadataDict['Stream'].split("_")[0]
inputStreamName = bsMetadataDict['Stream'].split("_")[1]
DRAW_ZMUMUStream.bsOutputSvc.StreamType = inputStreamType
DRAW_ZMUMUStream.bsOutputSvc.StreamName = inputStreamName
DRAW_ZMUMUStream.bsOutputSvc.ProjectTag = bsMetadataDict['Project']


if primDPD.ApplySkimming() :
    DRAW_ZMUMUStream.AddAcceptAlgs("DRAW_ZMUMUStream_CombinedPrescaleFilter")
    pass
