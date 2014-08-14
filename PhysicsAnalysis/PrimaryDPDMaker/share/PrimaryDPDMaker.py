##=============================================================================
## Name:        PrimaryDPDMaker
##
## Author:      Karsten Koeneke (DESY)
## Created:     August 2008
##
## Based on: ElectronPhotonDPDMaker by Karsten Koeneke (DESY) and
##           TauDPDMaker by David Cote and Sylvie Brunet (DESY)
##
## Description: This is the main python macro for the PrimaryDPDMaker. 
##              Its purpose is to creat specialized Derived Physics Data (DPD)
##              for for all ATLAS performance and physics groups.
##              
##              The primary jobs of this macro is to configure RecExCommon to 
##              produce the custon pool.root ouput collections. As an option,
##              part of the reconstruction can be redone, with custom
##              settings defined in PrimaryDPD_RecEx_Preprocess_Setup.py.
##              
## Questions? Please don't hesitate to ask:             
##              Karsten Koeneke (karsten.koeneke@desy.de)
##              
##=============================================================================


##=============================================================================
## Basic setup... most of it is handeled by RecExCommon from now one!
##=============================================================================

## for messaging
from AthenaCommon.Logging import logging
primaryDPD_msg = logging.getLogger( 'PrimaryDPDMaker' )
primaryDPD_msg.info( 'starting... \n\n\n     * * * * Welcome to PrimaryDPDMaker! :-)  * * * * \n\n' )

## Include the job property flags for this package
from PrimaryDPDMaker.PrimaryDPDFlags import *
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr 

## declare "topSequence", the top sequencer which manages all PrimaryDPDMaker algorithms.
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# Include the standard rec flags
from RecExConfig.RecFlags import rec


##=============================================================================
## 1) Mandatory: some configuration of PrimaryDPDMaker itself.
##=============================================================================

##--------------------------------
## Output file names
##--------------------------------
if len( primDPD.OutputDirectoryName() ) >= 1 and not primDPD.OutputDirectoryName().endswith("/") :
    primDPD.OutputDirectoryName = primDPD.OutputDirectoryName()+"/"    
    pass

if rec.OutputLevel() <= INFO:
    primaryDPD_msg.info('Values of all PrimaryDPD flags:')    
    print primDPD
    pass


##=============================================================================
## 3) Make primary DPD
##
## Most options are defined directly for each output DPD stream in the
## corresponding .py file. The goal is to have one self-consistent .py file
## (with the use of the common framework, helpers, and flags) for each produced
## output DPD stream.
## Currently, this is not possible for slimming... the last culprit, to be
## solved soon (hopefully).
##=============================================================================

if primDPD.MakePrimaryDPD():

    ##=======================================================
    ## Check to create Virtual Stream: i.e. only filters, no skimming/thinning
    ##=======================================================
    if primDPD.isVirtual():
        primDPD.ApplyThinning = False # Default is True
        primDPD.ApplySlimming = False # Default is True
        pass

    ##=======================================================================
    ## Include the event (skimming) filters for all DESD performance streams
    ##=======================================================================    
    if primDPD.WriteSingleElectronStream() \
           or primDPD.WriteEGammaStream() \
           or primDPD.WritePhotonJetStream() \
           or primDPD.WriteMuonStream() \
           or primDPD.WriteSingleMuonStream() \
           or primDPD.WriteJetStream() \
           or primDPD.WriteMinBiasStream() \
           or primDPD.WritePrescaledESDStream() \
           or primDPD.WriteLargeMetStream() \
           or primDPD.WriteTrackingStream() \
           or primDPD.WriteDESD_CollCandStream() :
        include("PrimaryDPDMaker/DESDEventSkimmingFilters.py")
        from PrimaryDPDMaker.PrimaryDPDFlags import desdEventSkimmingFilterNamesList
        pass
    
    try:
        if primDPD.WriteRPVLLStream() \
           or primDPD.WriteRPVLL_CosmicCaloStream():
            include("PrimaryDPDMaker/DESDEventSkimmingFilters.py")
            from PrimaryDPDMaker.PrimaryDPDFlags import desdEventSkimmingFilterNamesList            
            pass    
    except(AttributeError):
        pass
    
    ##=======================================================================
    ## Include the event (skimming) filters for all DAOD physics streams (NOT DAODM)
    ##=======================================================================
    from PrimaryDPDMaker.PrimaryDPDFlags import listESDtoDPD,listAODtoDPD
    print "These are the daodFlags: ", listAODtoDPD
    print "Trying to include daodEventSkimmingFilterNamesList"
    if primDPD.WritePhysDPDElectron() \
           or primDPD.WritePhysDPDPhoton() \
           or primDPD.WritePhysDPDMuon() \
           or primDPD.WritePhysDPDTau() \
           or primDPD.WritePhysDPDDiElectron() \
           or primDPD.WritePhysDPDDiPhoton() \
           or primDPD.WritePhysDPDDiMuon() \
           or primDPD.WritePhysDPDElectronMuon() \
           or primDPD.WritePhysDPDElectronTau() \
           or primDPD.WritePhysDPDMuonTau() \
           or primDPD.WritePhysDPDElectronMET() \
           or primDPD.WritePhysDPDMuonMET() \
           or primDPD.WritePhysDPDJetMET() \
           or primDPD.WritePhysDPDJetElectron() \
           or primDPD.WritePhysDPDJetMuon() \
           or primDPD.WritePhysDPDJetTau() :
        include("PrimaryDPDMaker/DAODEventSkimmingFilters.py")
        from PrimaryDPDMaker.PrimaryDPDFlags import daodEventSkimmingFilterNamesList
        pass



    ##=======================================================
    ## Include the Single Electron DPD Output Stream
    ##=======================================================
    if primDPD.WriteSingleElectronStream():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/PerfDPD_SingleElectron.py")
        #include("PrimaryDPDMaker/PerfDPD_SingleElectron.py")
        pass


    ##=======================================================
    ## Include the EGamma DPD Output Stream
    ##=======================================================
    if primDPD.WriteEGammaStream():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/PerfDPD_EGamma.py")
        pass


    ##=======================================================
    ## Include the Photon-Jet DPD Output Stream
    ##=======================================================
    if primDPD.WritePhotonJetStream():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/PerfDPD_PhotonJet.py")
        pass


    ##=======================================================
    ## Include the Muon DPD Output Stream
    ##=======================================================
    if primDPD.WriteMuonStream():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/PerfDPD_Muon.py")
        pass

    ##=======================================================
    ## Include the Single Muon DPD Output Stream
    ##=======================================================
    if primDPD.WriteSingleMuonStream():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/PerfDPD_SingleMuon.py")
        pass

    ##=======================================================
    ## Include the Jet DPD Output Stream
    ##=======================================================
    if primDPD.WriteJetStream():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/PerfDPD_Jet.py")
        pass

    ##=======================================================
    ## Include the Minimum Bias DPD Output Stream
    ##=======================================================
    if primDPD.WriteMinBiasStream():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/PerfDPD_MinBias.py")
        pass

    ##=======================================================
    ## Include the PrescaledESD DPD Output Stream
    ##=======================================================
    if primDPD.WritePrescaledESDStream():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/PerfDPD_PrescaledESD.py")
        pass

    ##=======================================================
    ## Include the Large Met DPD Output Stream
    ##=======================================================
    if primDPD.WriteLargeMetStream():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/PerfDPD_LargeMet.py")
        pass

    ##=======================================================
    ## Include the Tracking DPD Output Stream
    ##=======================================================
    if primDPD.WriteTrackingStream():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/PerfDPD_Tracking.py")
        pass

    ##=======================================================
    ## Include the CollCand DESD Output Stream
    ##=======================================================
    if primDPD.WriteDESD_CollCandStream():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/DESD_GOODTIME.py")
        pass



    ##=======================================================
    ## Include the W -> e nu DPD from ESD Output Stream
    ##=======================================================
    if primDPD.WriteD2ESD_WENUStream():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/DESD_WENU.py")
        pass

    ##=======================================================
    ## Include the W -> mu nu DPD from ESD Output Stream
    ##=======================================================
    if primDPD.WriteD2ESD_WMUNUStream():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/DESD_WMUNU.py")
        pass

    ##=======================================================
    ## Include the DiPhoton DPD from ESD Output Stream
    ##=======================================================
    if primDPD.WriteDiPhotonStream():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/D2ESD_DiPhoton.py")
        pass

    ##=======================================================
    ## Include the DiPhoton DPD from ESD Output Stream
    ##=======================================================
    if primDPD.WriteD2AOD_DIPHOStream():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/D2ADO_DiPhoton.py")
        pass



    ##=======================================================
    ## Include the Zee DPD from ByteStream Output Stream
    ##=======================================================
    if primDPD.WriteRAWPerfDPD_ZEE():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/RAWPerfDPD_Zee.py")
        pass

    ##=======================================================
    ## Include the W->e nu DPD from ByteStream Output Stream
    ##=======================================================
    if primDPD.WriteRAWPerfDPD_WENU():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/RAWPerfDPD_Wenu.py")
        pass

    ##=======================================================
    ## Include the Zmumu DPD from ByteStream Output Stream
    ##=======================================================
    if primDPD.WriteRAWPerfDPD_ZMUMU():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/RAWPerfDPD_Zmumu.py")
        pass

    ##=======================================================
    ## Include the W->mu nu DPD from ByteStream Output Stream
    ##=======================================================
    if primDPD.WriteRAWPerfDPD_WMUNU():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/RAWPerfDPD_Wmunu.py")
        pass


    ##=======================================================
    ## Include the RPV/LL DESDM Output Stream
    ##=======================================================
    try:
        if primDPD.WriteRPVLLStream():
            rec.DPDMakerScripts.append("LongLivedParticleDPDMaker/PhysDESDM_RPVLL.py")
        if primDPD.WriteRPVLL_CosmicCaloStream():
            rec.DPDMakerScripts.append("LongLivedParticleDPDMaker/PhysDESDM_RPVLL_CosmicCalo.py")
    except(AttributeError):
        pass

    ##################################################################################
    ## Commissioning DPD

    ##=======================================================
    ## Include the Prescaled Bytestream Output Stream
    ##=======================================================
    if primDPD.WriteRAWCommDPD_IDPROJ():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/RAWCommDPD_IDProj.py")
        pass

    if primDPD.WritePixelCommStream():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/CommDPD_Pixel.py")
        pass

    if primDPD.WriteSCTCommStream():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/CommDPD_SCT.py")
        pass
    
    if primDPD.WriteIDCommStream():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/CommDPD_ID.py")
        pass

    if primDPD.WriteIDProjCommStream():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/CommDPD_IDProj.py")
        pass

    if primDPD.WriteCaloCommStream():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/CommDPD_Calo.py")
        pass

    if primDPD.WriteTileCommStream():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/CommDPD_Tile.py")
        pass

    if primDPD.WriteEMClustCommStream():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/CommDPD_EMCluster.py")
        pass

    if primDPD.WriteEGamTauCommStream():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/CommDPD_EGamTau.py")
        pass

    if primDPD.WriteRPCCommStream():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/CommDPD_RPC.py")
        pass

    if primDPD.WriteTGCCommStream():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/CommDPD_TGC.py")
        pass

    if primDPD.WriteMUONCommStream():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/CommDPD_MUON.py")
        pass


    if primDPD.WriteRandomCommStream():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/CommDPD_Random.py")
        pass



    ##################################################################################
    ## DPDs for Heavy Ion running

    ##=======================================================
    ## Include the DESD_HIRARE Output Stream
    ##=======================================================
    if primDPD.WriteDESD_HIRAREStream():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/DESD_HIRARE.py")
        pass



    ##################################################################################
    ## Primary Physics DPDs

    ##=======================================================
    ## Include the primary physics DPDs
    ##=======================================================
    if primDPD.WritePhysDPDElectron():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/PhysDPD_Electron.py")
        pass

    if primDPD.WritePhysDPDPhoton():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/PhysDPD_Photon.py")
        pass

    if primDPD.WritePhysDPDMuon():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/PhysDPD_Muon.py")
        pass

    if primDPD.WritePhysDPDTau():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/PhysDPD_Tau.py")
        pass

    if primDPD.WritePhysDPDDiElectron():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/PhysDPD_DiElectron.py")
        pass

    if primDPD.WritePhysDPDDiPhoton():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/PhysDPD_DiPhoton.py")
        pass

    if primDPD.WritePhysDPDDiMuon():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/PhysDPD_DiMuon.py")
        pass

    if primDPD.WritePhysDPDElectronMuon():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/PhysDPD_ElectronMuon.py")
        pass

    if primDPD.WritePhysDPDElectronTau():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/PhysDPD_ElectronTau.py")
        pass

    if primDPD.WritePhysDPDMuonTau():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/PhysDPD_MuonTau.py")
        pass

    if primDPD.WritePhysDPDElectronMET():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/PhysDPD_ElectronMET.py")
        pass

    if primDPD.WritePhysDPDMuonMET():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/PhysDPD_MuonMET.py")
        pass

    if primDPD.WritePhysDPDJetMET():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/PhysDPD_JetMET.py")
        pass

    if primDPD.WritePhysDPDJetElectron():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/PhysDPD_JetElectron.py")
        pass

    if primDPD.WritePhysDPDJetMuon():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/PhysDPD_JetMuon.py")
        pass

    if primDPD.WritePhysDPDJetTau():
        rec.DPDMakerScripts.append("PrimaryDPDMaker/PhysDPD_JetTau.py")
        pass


    ##=======================================================
    ## Include the primary physics DPDs
    ##=======================================================
    if primDPD.WritePreselectElectronStreamFromESDInput() or primDPD.WritePreselectElectronStreamFromAODInput() :
        rec.DPDMakerScripts.append("PrimaryDPDMaker/PreselDPD_Electron.py")
        pass

    ##=======================================================
    ## Include the Monte Carlo DPDs (currently only skimmed AOD or ESD)
    ##=======================================================
    if primDPD.WriteDAOD_EGLOOSEStream() :
        rec.DPDMakerScripts.append("PrimaryDPDMaker/DAOD_EGLOOSE.py")
        pass

    if primDPD.WriteDAOD_MUStream() :
        rec.DPDMakerScripts.append("PrimaryDPDMaker/DAOD_MU.py")
        pass
    
    if primDPD.WriteDAOD_ELLOOSEStream() :
        rec.DPDMakerScripts.append("PrimaryDPDMaker/DAOD_ELLOOSE.py")
        pass    

    if primDPD.WriteDAOD_PHOTONLOOSEStream() :
        rec.DPDMakerScripts.append("PrimaryDPDMaker/DAOD_PHOTONLOOSE.py")
        pass
    
    if primDPD.WriteDESD_CTLFWJETStream() :
        rec.DPDMakerScripts.append("PrimaryDPDMaker/DESD_CTLFWJET.py")
        pass

    if primDPD.WriteDESD_FWJETStream() :
        rec.DPDMakerScripts.append("PrimaryDPDMaker/DESD_FWJET.py")
        pass


    if primDPD.WriteD2ESD_ZMMStream() :
        rec.DPDMakerScripts.append("PrimaryDPDMaker/D2ESD_ZMM.py")
        pass

    if primDPD.WriteD2ESD_JPSIUPSMMStream() :
        rec.DPDMakerScripts.append("PrimaryDPDMaker/D2ESD_JPSIUPSMM.py")
        pass




    ##=======================================================
    ## INCLUDE THE CALLING OF NEW OUTPUT STREAMS HERE!!!
    ##=======================================================
    if primDPD.WriteDAOD_ONIAMUMUStream() :
        rec.DPDMakerScripts.append("PrimaryDPDMaker/DAOD_ONIA_MUMU.py")
        pass


    ##=======================================================
    ## Include the TestDPD
    ##=======================================================
    if primDPD.WriteTestDPDStream() :
        rec.DPDMakerScripts.append("PrimaryDPDMaker/TestDPD.py")
        pass



    #========================================================
    # Setting up the trigger decision tool. It will be only
    # set up once per job. The flag will be set to True by
    # the output streams that need it...
    #========================================================
    include("PrimaryDPDMaker/SetupTrigDecisionTool.py")


    pass # End: if primDPD.MakePrimaryDPD()




##=============================================================================
##Final debugging options...
#ServiceMgr.StoreGateSvc.Dump = True     # Prints the content of StoreGate (EventStore) 
#ServiceMgr.MetaDataStore.Dump = True    # Prints output metadata store content
#ServiceMgr.InputMetaDataStore.Dump=True # Prints input metadata store content
#theApp.Dlls  += ["TruthExamples" ]
#theApp.TopAlg += [ "DumpMC" ]    # These two lines will dump some
#DumpMC = Algorithm ( "DumpMC" )  # info about the MC truth
#DumpMCG4.McEventKey = "SpclMC"
#topSequence.InsertTruthToolLooper.TrueJetInserter.OutputLevel = DEBUG 
##=============================================================================

#primaryDPD_msg.info( ' ******Final PrimaryDPDMaker job configuration: \n\n\n %s', topSequence )
primaryDPD_msg.info( '\n\nYou succesfully configured PrimaryDPDMaker.py ! \
\nThe job will now initialize, execute and finalize itself...\n\n' )

    
