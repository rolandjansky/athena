##=============================================================================
## Name:        LongLivedParticleDPDMaker
##
## Authors:     Nick Barlow, Christian Ohm
## Created:     November 2009
##
## Based on: PrimaryDPDMaker by Karsten Koeneke (DESY) and
##           TauDPDMaker by David Cote and Sylvie Brunet (DESY)
##
## Description: This is the main python macro for the LongLivedParticleDPDMaker. 
##              Its purpose is to creat specialized Derived Physics Data (DPD)
##              for for all ATLAS performance and physics groups.
##              
##              The primary jobs of this macro is to configure RecExCommon to 
##              produce the custon pool.root ouput collections.
##              
##=============================================================================


##=============================================================================
## Basic setup... most of it is handeled by RecExCommon from now one!
##=============================================================================

## for messaging
from AthenaCommon.Logging import logging
llDPD_msg = logging.getLogger( 'LongLivedParticleDPDMaker' )
llDPD_msg.info( 'starting... \n\n\n     * * * * Welcome to LongLivedParticleDPDMaker  * * * * \n\n' )

## Include the job property flags for this package
from LongLivedParticleDPDMaker.LongLivedDPDFlags import llDPD
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr 

## declare "theJob", the top sequencer which manages all PrimaryDPDMaker algorithms.
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()


##=============================================================================
## 1) Mandatory: some configuration of LongLivedParticleDPDMaker itself.
##=============================================================================

##--------------------------------
## Output file names
##--------------------------------
if len( llDPD.OutputDirectoryName() ) >= 1 and not llDPD.OutputDirectoryName().endswith("/") :
    llDPD.OutputDirectoryName = llDPD.OutputDirectoryName()+"/"    
    pass

if rec.OutputLevel() <= INFO:
    llDPD_msg.info('Values of all LongLivedDPD flags:')    
    print(llDPD)
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

if llDPD.MakePrimaryDPD():

    ##=======================================================
    ## Include the RPVLL DESDM Output Stream
    ##=======================================================
    if llDPD.WriteRPVLLStream():
        include("LongLivedParticleDPDMaker/PhysDESDM_RPVLL.py")
        pass
        pass

    #========================================================
    # Setting up the trigger decision tool. It will be only
    # set up once per job. The flag will be set to True by
    # the output streams that need it...
    #========================================================
    if llDPD.SetupTrigDecisionTool():
        include("PrimaryDPDMaker/SetupTrigDecisionTool.py")
        pass

    pass


##=============================================================================
##Final debugging options...
#ServiceMgr.StoreGateSvc.Dump = True     # Prints the content of StoreGate (EventStore) 
#ServiceMgr.MetaDataStore.Dump = True    # Prints output metadata store content
#ServiceMgr.InputMetaDataStore.Dump=True # Prints input metadata store content
#theApp.Dlls  += ["TruthExamples" ]
#theApp.TopAlg += [ "DumpMC" ]    # These two lines will dump some
#DumpMC = Algorithm ( "DumpMC" )  # info about the MC truth
#DumpMCG4.McEventKey = "SpclMC" 
#theJob.InsertTruthToolLooper.TrueJetInserter.OutputLevel = DEBUG 
##=============================================================================

#primaryDPD_msg.info( ' ******Final PrimaryDPDMaker job configuration: \n\n\n %s', theJob )
llDPD_msg.info( '\n\nYou succesfully configured LongLivedParticleDPDMaker.py ! \
\nThe job will now initialize, execute and finalize itself...\n\n' )

    
