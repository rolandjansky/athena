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
primaryDPD_msg.info('Configuring PrimaryDPDMaker')

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

if rec.OutputLevel() <= INFO:
    primaryDPD_msg.info('Values of all PrimaryDPD flags:')    
    print(primDPD)


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

    
