##-----------------------------------------------------------------------------
## Name: PrimaryDPD_EGamTauCommStream.py
##
## Author: Karsten Koeneke and David Cote (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: This defines the content of the EGamma and Tau Commissioning DPD.
##
##-----------------------------------------------------------------------------


## for messaging
from AthenaCommon.Logging import logging
egammaCommDPDStream_msg = logging.getLogger( 'PrimaryDPD_EGamTauCommStream' )

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package and from RecExCommon
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

## This handels multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr


##====================================================================
## Write the used options to the log file
##====================================================================
egammaCommDPDStream_msg.info( 'starting...\n\n******************  Final setup from PrimaryDPD_EGammaCommStream.py  *****************' )


##====================================================================
## Define the skimming (event selection) for the DPD output stream
##====================================================================
if primDPD.ApplySkimming():
    from PrimaryDPDMaker.EGammaCountingFilter import EGammaCountingFilter
        
    topSequence += EGammaCountingFilter("EGamTauCommStream_EGammaCountingFilter")
    topSequence.EGamTauCommStream_EGammaCountingFilter.useElectronCollection  = True
    topSequence.EGamTauCommStream_EGammaCountingFilter.electronCollectionType = "ElectronContainer"
    topSequence.EGamTauCommStream_EGammaCountingFilter.electronCollectionName = "ElectronCollection"
    topSequence.EGamTauCommStream_EGammaCountingFilter.usePhotonCollection  = True
    topSequence.EGamTauCommStream_EGammaCountingFilter.photonCollectionType = "PhotonContainer"
    topSequence.EGamTauCommStream_EGammaCountingFilter.photonCollectionName = "PhotonCollection"
    topSequence.EGamTauCommStream_EGammaCountingFilter.useEMClusterCollection = False
    #topSequence.EGamTauCommStream_EGammaCountingFilter.OutputLevel            = DEBUG
    
    from PrimaryDPDMaker.TauFilter import TauFilter
    topSequence += TauFilter("EGamTauCommStream_TauFilter")
    topSequence.EGamTauCommStream_TauFilter.tauCollectionName = "TauRecContainer"    
    topSequence.EGamTauCommStream_TauFilter.cutTauEtMin       = -1.0
    topSequence.EGamTauCommStream_TauFilter.cutNTrk           = 0
    topSequence.EGamTauCommStream_TauFilter.doPreselection    = False
    #topSequence.EGamTauCommStream_TauFilter.OutputLevel       = DEBUG


##====================================================================
## Define the DPD output stream event selection (skimming)
##====================================================================
streamName = primDPD.WriteEGamTauCommStream.StreamName
fileName   = buildFileName( primDPD.WriteEGamTauCommStream )
if primDPD.isVirtual() == False:
    dpd=MSMgr.NewPoolStream( streamName, fileName ) 
    pass
if primDPD.isVirtual() == True:
    dpd=MSMgr.NewVirtualStream( streamName, fileName ) 
    pass    

if primDPD.ApplySkimming():
    dpd.AddAcceptAlgs( "EGamTauCommStream_EGammaCountingFilter" )
    dpd.AddAcceptAlgs( "EGamTauCommStream_TauFilter" )
    

##====================================================================
## Define the DPD output stream event content
##====================================================================
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
excludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList( streamName,excludeList )


