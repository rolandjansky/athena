##-----------------------------------------------------------------------------
## Name: DESD_WENU.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: This defines the selection of the ESD
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
## Define the skimming (event selection) for the  Wenu DPD output stream
##====================================================================
if primDPD.ApplySkimming():
    # The missing Et filters
    from PrimaryDPDMaker.MissingEtFilter import MissingEtFilter
    topSequence += MissingEtFilter( "DESD_WENUStream_MissingEtRefFinalFilter",
                                    metCollectionName = "MET_RefFinal",
                                    cutMetMin         = 20.0*Units.GeV
                                    )

    from PrimaryDPDMaker.CompositeMissingEtFilter import CompositeMissingEtFilter
    topSequence += CompositeMissingEtFilter( "DESD_WENUStream_MissingEtLocHadTopoFilter",
                                             metCollectionTypeList = [ "MissingET", "MissingET", "MissingET" ],
                                             metCollectionNameList = [ "MET_LocHadTopo", "MET_MuonBoy", "MET_RefMuon_Track" ],
                                             metCombinationMetric  = [ 1.0, 1.0, -1.0 ],
                                             cutMetMin             = 20.0*Units.GeV
                                             )

    # Combine the two different MET selections
    from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
    topSequence += LogicalFilterCombiner( "DESD_WENUStream_MissingEtFilter",
                                          cmdstring = "DESD_WENUStream_MissingEtRefFinalFilter or DESD_WENUStream_MissingEtLocHadTopoFilter"
                                          )

    # The electron filter
    from D2PDMaker.D2PDMakerConf import D2PDElectronSelector
    topSequence += D2PDElectronSelector( "DESD_WENUStream_ElectronFilter",
                                         OutputLevel            = INFO,
                                         inputCollection        = 'ElectronAODCollection',
                                         outputCollection       = 'DESD_WENUStream_ElectronCollection',
                                         minNumberPassed        = 1,
                                         electronVetoAuthorList = [ egammaParameters.AuthorFrwd ],
                                         electronIsEM           = egammaPID.ElectronMedium,
                                         clusterEtMin           = 20.0*Units.GeV
                                         )


    # Create the combined decission with a possible prescale
    from PrimaryDPDMaker.PrimaryDPDMakerConf import PrimaryDPDPrescaler
    topSequence += PrimaryDPDPrescaler( "DESD_WENUStream_CombinedPrescaleFilter",
                                        RequireAlgs = [ "DESD_WENUStream_MissingEtFilter", "DESD_WENUStream_ElectronFilter" ],
                                        Prescale    = primDPD.WriteD2ESD_WENUStream.Prescale )

    pass



##====================================================================
## Define the Wenu DPD output stream
##====================================================================
streamName = primDPD.WriteD2ESD_WENUStream.StreamName
fileName   = buildFileName( primDPD.WriteD2ESD_WENUStream )
if primDPD.isVirtual() == False:
    DESD_WENUStream = MSMgr.NewPoolStream( streamName, fileName )
    pass
if primDPD.isVirtual() == True:
    DESD_WENUStream = MSMgr.NewVirtualStream( streamName, fileName )
    pass

if primDPD.ApplySkimming() :
    DESD_WENUStream.AcceptAlgs( [ "DESD_WENUStream_CombinedPrescaleFilter" ] )
    pass


#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
# Take everything from the input
ExcludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList( streamName, ExcludeList )

