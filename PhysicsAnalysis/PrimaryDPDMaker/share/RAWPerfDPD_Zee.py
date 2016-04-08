##-----------------------------------------------------------------------------
## Name: PrimaryDPD_EGammaStream.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: This defines the selection of the ByteStream (RAW) format DPD
##              containing Z->ee events only.
##
##-----------------------------------------------------------------------------


## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package and from RecExCommon
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

## This handels multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr


##====================================================================
## Define the skimming (event selection) for the Photon-Jet DPD output stream
##====================================================================
if primDPD.ApplySkimming() :

    from AthenaCommon.AlgSequence import AlgSequence 
    topSequence = AlgSequence() 


    # Medium electron filter
    ToolSvc += CfgMgr.ParticleSelectionTool( "DRAW_ZEEStream_ElectronSelector",
                                            #  OutputLevel         = VERBOSE,
                                             InputContainer      = "Electrons",
                                             OutputLinkContainer = "DRAW_ZEEStream_MediumElectronCollection",
                                             Selection           = "( Electrons.Medium || Electrons.LHMedium ) && (Electrons.pt > 20.0*GeV)"
                                             )

    # Define the Z -> e e filter
    ToolSvc += CfgMgr.ParticleCombinerTool( "DRAW_ZEEStream_ZeeCombiner",
                                            # OutputLevel        = VERBOSE,
                                            InputContainerList = ["DRAW_ZEEStream_MediumElectronCollection",
                                                                  "DRAW_ZEEStream_MediumElectronCollection"],
                                            OutputContainer    = "DRAW_ZEEStream_ZeeBosonCollection",
                                            SetPdgId           = 23
                                            )


    # The name of the kernel (DRAW_ZEEStream_Kernel in this case) must be unique to this derivation
    from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
    topSequence += CfgMgr.DerivationFramework__DerivationKernel("DRAW_ZEEStream_Kernel",
                                                                # OutputLevel       = VERBOSE,
                                                                AugmentationTools = [ToolSvc.DRAW_ZEEStream_ElectronSelector,
                                                                                     ToolSvc.DRAW_ZEEStream_ZeeCombiner]
                                                                #SkimmingTools = [ToolSvc.DRAW_ZEEStream_ZeeFilter],
                                                                )

    # topSequence += CfgMgr.CutAlg( "DRAW_ZEEStream_ZeeFilterAlg",
    #                               # OutputLevel = VERBOSE,
    #                               Cut         = "count(DRAW_ZEEStream_ZeeBosonCollection.m > 55.0*GeV) >= 1" )

    # A temporary solution
    from PrimaryDPDMaker.MassFilter import MassFilter
    topSequence += MassFilter( "DRAW_ZEEStream_ZeeFilterAlg",
                            #    OutputLevel = VERBOSE,
                               InputContainer = "DRAW_ZEEStream_ZeeBosonCollection",
                               MinMassCut     = 55.0*Units.GeV
                               )

    # Create the combined decission with a possible prescale
    from PrimaryDPDMaker.PrimaryDPDMakerConf import PrimaryDPDPrescaler
    topSequence += PrimaryDPDPrescaler( "DRAW_ZEEStream_CombinedPrescaleFilter",
                                        AcceptAlgs = [ "DRAW_ZEEStream_ZeeFilterAlg" ],
                                        Prescale   = primDPD.WriteRAWPerfDPD_ZEE.Prescale )

    pass


##====================================================================
## Define the RAW Zee DPD output stream
##====================================================================
streamName = primDPD.WriteRAWPerfDPD_ZEE.StreamName
fileName   = buildFileName( primDPD.WriteRAWPerfDPD_ZEE )
# Remove the .pool.root ending in the file name, this is a RAW file!
if fileName.endswith(".pool.root") :
    fileName = fileName.rstrip(".pool.root")
    pass
ZeeByteStream = MSMgr.NewByteStream( streamName, fileName )

# Don't write an output RAW file if it is empty
ZeeByteStream.bsOutputSvc.WriteEventlessFiles = primDPD.WriteEventlessFiles()

#set max output file size to 10 GB
ZeeByteStream.bsOutputSvc.MaxFileMB = primDPD.OutputRAWMaxFileSize()
ZeeByteStream.bsOutputSvc.MaxFileNE = primDPD.OutputRAWMaxNEvents()


# Assign some meta data
from RecExConfig.InputFilePeeker import inputFileSummary
bsMetadataDict = inputFileSummary['bs_metadata']
inputStreamType = bsMetadataDict['Stream'].split("_")[0]
inputStreamName = bsMetadataDict['Stream'].split("_")[1]
ZeeByteStream.bsOutputSvc.StreamType = inputStreamType
ZeeByteStream.bsOutputSvc.StreamName = inputStreamName
ZeeByteStream.bsOutputSvc.ProjectTag = bsMetadataDict['Project']


if primDPD.ApplySkimming() :
    ZeeByteStream.AddAcceptAlgs("DRAW_ZEEStream_CombinedPrescaleFilter")
    pass
