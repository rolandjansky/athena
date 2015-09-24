##-----------------------------------------------------------------------------
## Name: PrimaryDPD_EMClusterCommStream.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: This defines the content of the EGamma Commissioning DPD output stream.
##
##-----------------------------------------------------------------------------


## for messaging
from AthenaCommon.Logging import logging
emClusterCommDPDStream_msg = logging.getLogger( 'PrimaryDPD_EMClusterCommStream' )

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
emClusterCommDPDStream_msg.info( 'starting...\n\n******************  Final setup from PrimaryDPD_EMClusterCommStream.py  *****************' )


##====================================================================
## Define the skimming (event selection) for the DPD output stream
##====================================================================
#if primDPD.ApplySkimming() and primEMClusterCommDPD.ApplySkimming() :
from PrimaryDPDMaker.EGammaCountingFilter import EGammaCountingFilter
        
topSequence += EGammaCountingFilter("EMClusterCommStream_EGammaCountingFilter")
topSequence.EMClusterCommStream_EGammaCountingFilter.useElectronCollection  = True
topSequence.EMClusterCommStream_EGammaCountingFilter.electronCollectionType = "ElectronContainer"
topSequence.EMClusterCommStream_EGammaCountingFilter.electronCollectionName = "ElectronCollection"
topSequence.EMClusterCommStream_EGammaCountingFilter.usePhotonCollection  = True
topSequence.EMClusterCommStream_EGammaCountingFilter.photonCollectionType = "PhotonContainer"
topSequence.EMClusterCommStream_EGammaCountingFilter.photonCollectionName = "PhotonCollection"
topSequence.EMClusterCommStream_EGammaCountingFilter.useEMClusterCollection = False



##====================================================================
## Define the DPD output stream event selection (skimming)
##====================================================================
streamName = primDPD.WriteEMClustCommStream.StreamName
fileName   = buildFileName( primDPD.WriteEMClustCommStream )
if primDPD.isVirtual() == False:
    dpd=MSMgr.NewPoolStream( streamName, fileName) 
    pass
if primDPD.isVirtual() == True:
    dpd=MSMgr.NewVirtualStream( streamName, fileName) 
    pass
dpd.AcceptAlgs( ["EMClusterCommStream_EGammaCountingFilter"] )



##====================================================================
## Define the DPD output stream event content
##====================================================================
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
excludeList=[]
dpdOutput.addAllItemsFromInputExceptExcludeList( streamName,excludeList )

