from AthenaCommon.Resilience import treatException,protectedInclude
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.Logging import logging

logCBNT_config = logging.getLogger( 'CBNT_config' )

protectedInclude( "CBNT_Athena/CBNT_AthenaAware_jobOptions.py" )

# in case it ws forgotten
theApp.HistogramPersistency = "ROOT"

# run/event number
protectedInclude( "CBNT_Athena/CBNT_EventInfo_jobOptions.py" )

from AthenaCommon.GlobalFlags  import globalflags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs

if  rec.Commissioning:
    CBNTAthenaAware=True


if rec.doTruth():
    protectedInclude( "RecExCommon/CBNT_Truth_jobOptions.py" )
            
    if not ( globalflags.DetDescrVersion()[0:3]=="DC1" or globalflags.DetDescrVersion()[0:3]=="DC2"):
            protectedInclude ("MCTruthAlgs/CBNT_TrackRecord.py")


if ( DetFlags.haveRIO.ID_on() or rec.readESD() or rec.readAOD()) :
    # no CBNT for ID anymore (use track validation ntuple)
    if jobproperties.Beam.beamType()=="collisions":

        #
        # functionality : put tracks from any tracking algorithm in combined ntuple
        #

        protectedInclude ("InDetPriVxCBNT/InDetPriVxCBNT_jobOptions.py")


        if rec.readAOD():
                try:
                        CBNTAA_VxPrimary.useTrkTrack=False
                        CBNTAA_VxPrimary.McEventCollectionName="GEN_AOD"
                except Exception:
                        logCBNT_config.warning("CBNTAA_VxPrimary undefined")

    protectedInclude ("CBNT_Particle/CBNT_Particle_jobOptions.py")

#if rec.doLArg() or rec.doTile():
#    protectedInclude ("CaloRec/CaloRec_CBNT_jobOptions.py")
#    #FIXME
#    #try:
#    #    CBNT_LArCell.Enable=False
#    #except Exception:
#    #    pass

if rec.doMuon():
    protectedInclude("MuonRecExample/MuonCBNT_jobOptions.py")


if ( ( DetFlags.haveRIO.Muon_on() and ( JetRecFlagsOK and jobproperties.JetRecFlags.Enabled())  and recAlgs.doMuonSpShower() ) or rec.readESD() ) :
        protectedInclude("MuonSpShowerBuilderAlgs/CBNTAA_MuonSpShower.py")

 
#
# Jet
#


if rec.readAOD() or rec.readESD() or ( JetRecFlagsOK and jobproperties.JetRecFlags.Enabled() ):
        try:
                from JetRec.CBNTJets import schedule_standard_CBNTJets
                schedule_standard_CBNTJets()
        except Exception:
                treatException("Could not load CBNT_Jet. Switched off !" )

#temporary
## if rec.readAOD() :
##         try:
##                 from JetRec.CBNTJets import schedule_CBNTJets_fromInputFile
##                 schedule_CBNTJets_fromInputFile(excludeList=["JetTruthParticles","JetTruthPileupParticles"],JetDetailedCBNT = None)
##         except Exception:
##                 treatException("Could not load CBNT_Jet. Switched off !" )





#if rec.readESD() or (tauRecFlagsOK and jobproperties.tauRecFlags.doTauRec() ):#
#
#    #
#    # ----- CBNT_tau  algorithm
#    # 
#    protectedInclude( "tauRec/tauMerged_CBNT_jobOptions.py")



# the various egamma alg should be logically split
if rec.readESD() or (egammaRecFlagsOK and jobproperties.egammaRecFlags.doEgammaCaloSeeded()  ) :

        try:
            if DetFlags.detdescr.ID_on():
                from egammaRec.CBNT_egammaGetter import CBNT_egammaGetter
                CBNT_egammaGetter()

            from egammaRec.CBNT_egammaGetter import CBNT_egammaPhotonGetter
            CBNT_egammaPhotonGetter()

            if DetFlags.detdescr.ID_on():
                from egammaRec.CBNT_egammaGetter import CBNT_egammaSofteGetter
                CBNT_egammaSofteGetter()

                from egammaRec.CBNT_egammaGetter import CBNT_egammaAODGetter
                CBNT_egammaAODGetter()

            from egammaRec.CBNT_egammaGetter import CBNT_egammaPhotonAODGetter
            CBNT_egammaPhotonAODGetter()

            if rec.doTruth():
                from egammaRec.CBNT_egTruthGetter import CBNT_egTruthGetter
                CBNT_egTruthGetter()
        except Exception:
                treatException("Could not load CBNT_egamma. Switched off !" )


if rec.doMuonCombined() or rec.readESD() or rec.readAOD():
        protectedInclude("MuonCombinedRecExample/MuonCombined_CBNT_jobOptions.py")

if recAlgs.doMissingET() or rec.readESD() or rec.readAOD() :
        protectedInclude ("MissingET/MissingET_CBNT_jobOptions.py")

#if recAlgs.doObjMissingET() or rec.readESD() or rec.readAOD():
#        protectedInclude ("ObjMissingET/ObjMissingET_CBNT_jobOptions.py")

#if recAlgs.doMissingETSig() or rec.readESD() or rec.readAOD() :
#        protectedInclude ("MissingETSig/MissingETSig_CBNT_jobOptions.py")

## #comment out until statuscode is fixed
# not supported anymore
## if recAlgs.doEFlow() or rec.readESD():
##          protectedInclude ("eflowRec/jobOptions_CBNT_eflow.py")

## if recAlgs.doEFlowJet():
##         protectedInclude ("eflowRec/ConeJetEflow_CBNT_jobOptions.py")

if rec.doHeavyIon():
        protectedInclude("HIRecExample/HIRec_CBNT_jobOptions.py")

   
if recAlgs.doTrigger() or ( rec.readESD() and not rec.noESDTrigger()) :
        protectedInclude ("TriggerRelease/jobOfragment_TriggerCBNT.py")
            

if rec.doAOD() or rec.readAOD():
        protectedInclude ( "CBNT_AOD/AOD_CBNT_jobOptions.py" ) 

if recAlgs.doAtlfast() :
        protectedInclude("CBNT_AOD/AOD_CBNT_FastTree_jobOptions.py")

protectedInclude( "CBNT_Athena/CBNT_Audit_jobOptions.py" )

#CBNT_Audit.detailedTime = false ; // put in the ntuple CPU time per algrithm
# list of algorithms to be monitored (by default take them all)
#CBNT_Audit.Algs += { "GenzModule", "CBNT_Athena" } ;

