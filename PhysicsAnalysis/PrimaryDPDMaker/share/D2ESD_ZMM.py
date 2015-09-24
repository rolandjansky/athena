##-----------------------------------------------------------------------------
## Name: D2ESD_ZMM.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: This defines the selection of the D2ESD
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


##====================================================================
## Define the skimming (event selection) for the Photon-Jet DPD output stream
##====================================================================
if primDPD.ApplySkimming() :
    from D2PDMaker.D2PDMakerConf import D2PDVertexSelector
    topSequence += D2PDVertexSelector( "D2ESD_ZMM_VertexFilter",
                                       OutputLevel          = INFO,
                                       inputCollection      = 'VxPrimaryCandidate',
                                       minNumberVxTracks    = 3,
                                       vertexZMin           = -150.0*Units.mm,
                                       vertexZMax           = 150.0*Units.mm,
                                       )

    
    from PrimaryDPDMaker.ZmumuFilter import ZmumuFilter
    topSequence += ZmumuFilter( "D2ESD_ZMM_ZmumuStacoFilter",
                                muonCollectionType     = "Analysis::MuonContainer",
                                muonCollectionName     = "StacoMuonCollection",
                                cutEtMin               = 10.0*Units.GeV,
                                cutLeadingEtMin        = 20.0*Units.GeV,
                                cutEtaMax              = 99.9,
                                requireLooseMuon       = True,
                                minNumberCombinedMuons = 1,
                                removeEtaCrack         = False,
                                cutDiMuMassLower       = 45.0*Units.GeV,
                                cutDiMuMassUpper       = 999999.0*Units.GeV
                                )

    topSequence += ZmumuFilter( "D2ESD_ZMM_ZmumuMuidFilter",
                                muonCollectionType     = "Analysis::MuonContainer",
                                muonCollectionName     = "MuidMuonCollection",
                                cutEtMin               = 10.0*Units.GeV,
                                cutLeadingEtMin        = 20.0*Units.GeV,
                                cutEtaMax              = 99.9,
                                requireLooseMuon       = True,
                                minNumberCombinedMuons = 1,
                                removeEtaCrack         = False,
                                cutDiMuMassLower       = 45.0*Units.GeV,
                                cutDiMuMassUpper       = 999999.0*Units.GeV
                                )


    # Combine the Staco and Vertex filter
    from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
    topSequence += LogicalFilterCombiner( "D2ESD_ZMM_ZmumuStacoVertexFilter",
                                          cmdstring = "D2ESD_ZMM_VertexFilter and D2ESD_ZMM_ZmumuStacoFilter"
                                          )

    # Combine the Muid and Vertex filter
    topSequence += LogicalFilterCombiner( "D2ESD_ZMM_ZmumuMuidVertexFilter",
                                          cmdstring = "D2ESD_ZMM_VertexFilter and D2ESD_ZMM_ZmumuMuidFilter"
                                          )


    # Create the combined decission with a possible prescale
    from PrimaryDPDMaker.PrimaryDPDMakerConf   import PrimaryDPDPrescaler
    topSequence += PrimaryDPDPrescaler( "D2ESD_ZMM_CombinedPrescaleFilter",
                                        AcceptAlgs = [ "D2ESD_ZMM_ZmumuStacoVertexFilter", "D2ESD_ZMM_ZmumuMuidVertexFilter" ],
                                        Prescale = primDPD.WriteD2ESD_ZMMStream.Prescale
                                        )

    pass


##====================================================================
## Define the D2ESD Zmumu output stream
##====================================================================
streamName = primDPD.WriteD2ESD_ZMMStream.StreamName
fileName   = buildFileName( primDPD.WriteD2ESD_ZMMStream )
D2ESD_ZMMStream = MSMgr.NewPoolStream( streamName, fileName )

if primDPD.ApplySkimming() :
    D2ESD_ZMMStream.AcceptAlgs( [ "D2ESD_ZMM_CombinedPrescaleFilter" ] )
    pass

# Add all intermediate filters to the bookkeeping
D2ESD_ZMMStream.AddOtherAlgsToBookkeep( [ "D2ESD_ZMM_CombinedPrescaleFilter",
                                          "D2ESD_ZMM_ZmumuMuidVertexFilter",
                                          "D2ESD_ZMM_ZmumuStacoVertexFilter",
                                          "D2ESD_ZMM_ZmumuMuidFilter",
                                          "D2ESD_ZMM_ZmumuStacoFilter",
                                          "D2ESD_ZMM_VertexFilter" ] )

#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
# Take everything from the input
ExcludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList( streamName, ExcludeList )

