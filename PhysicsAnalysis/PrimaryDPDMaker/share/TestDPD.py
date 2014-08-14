##-----------------------------------------------------------------------------
## Name: TestDPD.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: This is a file for testing.
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
## Define this DPD output stream
##====================================================================
streamName = "StreamDAODM_TEST"
fileName   = "myTestDPD.pool.root"
TestDPDStream = MSMgr.NewPoolStream( streamName, fileName )
if primDPD.ApplySkimming() :
    #RequireAlgs: all filters are required to be fulfilled
    #AcceptAlgs: one filter is required to be fulfilled
    TestDPDStream.AcceptAlgs( [ "DRAW_ZMUMUStream_CombinedPrescaleFilter" ] )
    #"DRAW_ZEEStream_CombinedPrescaleFilter" ] )
    pass
#TestDPDStream.AddOtherAlgsToBookkeep( testAlgsToBookkeep )


#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

# Take all items from the input, except for the ones listed in the excludeList
excludeList = [ "" ]
#dpdOutput.addAllItemsFromInputExceptExcludeList( streamName,excludeList )

# TestDPDStream.AddItem( ['EventInfo#*'] )
# TestDPDStream.AddItem( ["egammaContainer#ElectronAODCollection"] )
TestDPDStream.AddItem("xAOD::MuonContainer_v1#*")
TestDPDStream.AddItem("xAOD::MuonAuxContainer_v1#*")






# ##====================================================================
# ## Finalize the thinning of the containers for this stream
# ##====================================================================
# from AthenaServices.Configurables import ThinningSvc, createThinningSvc
# augStream = MSMgr.GetStream( streamName )
# evtStream = augStream.GetEventStream()
# svcMgr += createThinningSvc( svcName="TestStreamThinning", outStreams=[evtStream] )
