##-----------------------------------------------------------------------------
## Name: PhysDPD_PreselectElectron.py
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
##
## Description: This defines the DPD that pre-select events with one loose electron.
##
##-----------------------------------------------------------------------------

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package and from RecExCommon
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

## This handels multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

## for messaging
from AthenaCommon.Logging import logging
preselElDPDStream_msg = logging.getLogger( 'PrimaryDPD_PreselElectron' )


##====================================================================
## Check if we are running on an input ESD or AOD and configure
## this output stream accordingly
##====================================================================
from RecExConfig.RecFlags import rec
if rec.readESD() :
    if not primDPD.WritePreselectElectronStreamFromESDInput.is_locked() :
        # Default writing of ESD content when reading an ESD
        # (unless the flag is already locked by the user)
        if primDPD.WritePreselectElectronStreamFromESDInput.writeAODContent == primDPD.WritePreselectElectronStreamFromESDInput.writeESDContent :
            preselElDPDStream_msg.error("Inconsitent configuration! You should set EITHER primDPD.WritePreselectElectronStreamFromESDInput.writeAODContent=True ")
            preselElDPDStream_msg.error(" OR primDPD.WritePreselectElectronStreamFromESDInput.writeESDContent=True")
            pass
        elif primDPD.WritePreselectElectronStreamFromESDInput.writeESDContent :
            primDPD.WritePreselectElectronStreamFromESDInput.StreamName = 'ESD_SKIMEL'
            pass
        elif primDPD.WritePreselectElectronStreamFromESDInput.writeAODContent :
            primDPD.WritePreselectElectronStreamFromESDInput.StreamName = 'AOD_SKIMEL'
            pass
        pass
    pass


##====================================================================
## Make sure the configuration is consitent
##====================================================================
isConsistent = True
if primDPD.WritePreselectElectronStreamFromESDInput.writeESDContent and primDPD.WritePreselectElectronStreamFromESDInput.writeAODContent :
    isConsistent = False
    pass
if primDPD.WritePreselectElectronStreamFromESDInput.writeESDContent and ( not rec.readESD() or rec.readAOD() ) :
    isConsistent = False
    pass
if primDPD.WritePreselectElectronStreamFromAODInput() and rec.readESD() and not rec.doAOD() :
    isConsistent = False
    pass


if isConsistent :
##====================================================================
## Define the skimming (event selection)
##====================================================================
    if primDPD.ApplySkimming() :
        from PrimaryDPDMaker.ElectronFilter import ElectronFilter
        topSequence += ElectronFilter("PreselectElectronStream_ElectronFilter")
        topSequence.PreselectElectronStream_ElectronFilter.electronCollectionName = "ElectronAODCollection"
        if primDPD.WritePreselectElectronStreamFromESDInput() :
            topSequence.PreselectElectronStream_ElectronFilter.cutEtMin = primDPD.WritePreselectElectronStreamFromESDInput.ElectronEt
            topSequence.PreselectElectronStream_ElectronFilter.cutIsEM  = primDPD.WritePreselectElectronStreamFromESDInput.ElectronIsEM
            pass
        elif primDPD.WritePreselectElectronStreamFromAODInput() :
            topSequence.PreselectElectronStream_ElectronFilter.cutEtMin = primDPD.WritePreselectElectronStreamFromAODInput.ElectronEt
            topSequence.PreselectElectronStream_ElectronFilter.cutIsEM  = primDPD.WritePreselectElectronStreamFromAODInput.ElectronIsEM
            pass
        pass

##====================================================================
## Define the DPD output stream name and file name
##====================================================================
    if primDPD.WritePreselectElectronStreamFromESDInput() :
        streamName = primDPD.WritePreselectElectronStreamFromESDInput.StreamName
        fileName   = buildFileName( primDPD.WritePreselectElectronStreamFromESDInput )
        pass
    elif  primDPD.WritePreselectElectronStreamFromAODInput() :
        streamName = primDPD.WritePreselectElectronStreamFromAODInput.StreamName
        fileName   = buildFileName( primDPD.WritePreselectElectronStreamFromAODInput )
        pass
    # Define the output stream
    PreselectElectronStream = MSMgr.NewPoolStream( streamName, fileName )
    if primDPD.ApplySkimming() :
        PreselectElectronStream.AcceptAlgs( ["PreselectElectronStream_ElectronFilter"] )
        pass


    
##====================================================================
## Build the output stream, depending on the configuration
##====================================================================
    from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput

    if ( primDPD.WritePreselectElectronStreamFromESDInput.writeESDContent and rec.readESD() ) \
           or ( primDPD.WritePreselectElectronStreamFromAODInput() and rec.readAOD() ) :
        print "I am trying to write the ESD content now"
        # Take everything from the input
        ExcludeList = []
        dpdOutput.addAllItemsFromInputExceptExcludeList( streamName, ExcludeList )
        pass
    
    elif primDPD.WritePreselectElectronStreamFromESDInput.writeAODContent and rec.readESD() and rec.doAOD() :
        # Do AOD building. This is a copy from RecExCommon_topOptions.py
        # It must be maintained by hand... this is not good!!!
        # Needs improvement!!!
        # A solution would be to change the AOD building in RecExCommon_topOptions.py to
        # use the MultipleStreamManager. This would allow to simply do red.doWriteAOD=True
        # and then get the StreamAOD object and add an event filter to it.
        print "I am trying to write the AOD content now"
        from ParticleBuilderOptions.AODFlags import AODFlags
        # Particle Builders
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        from AthenaServices.Configurables import ThinningSvc
        if not hasattr(svcMgr, 'ThinningSvc'):
            svcMgr += ThinningSvc(OutputLevel=INFO)
            pass
        svcMgr.ThinningSvc.Streams += ['StreamAOD']

        if AODFlags.Photon or AODFlags.Electron:
            doEgammaPhoton = AODFlags.Photon
            doEgammaElectron= AODFlags.Electron
            from egammaRec.egammaAODGetter import egammaAODGetter
            egammaAODGetter()
            if AODFlags.egammaTrackSlimmer:
                from egammaRec.egammaTrackSlimmer import egammaTrackSlimmer
                egammaTrackSlimmer()
                pass
            pass
        
        if AODFlags.MuonTrackSlimmer:
            protectedInclude( "MuonAlgs/MuonTrackSlimmer_jobOptions.py" )
            pass
        
        if AODFlags.TauTrackSlimmer:
            protectedInclude("tauRec/tauMerged_trackslim_jobOptions.py")
            pass

        # Define the output stream
        StreamAOD = PreselectElectronStream.GetEventStream()
        StreamAOD.ForceRead = True
        
        # protectedInclude( "ParticleBuilderOptions/AOD_OutputList_jobOptions.py")
        protectedInclude( "RecExPers/RecoOutputAODList_jobOptions.py")    
        #FIXME HACK remove faulty object
        StreamAOD.ItemList = [ e for e in StreamAOD.ItemList if not e in [ 'CaloTowerContainer#HLT_TrigCaloTowerMaker'] ] 
                
        pass


    pass # End: if isConsistent
