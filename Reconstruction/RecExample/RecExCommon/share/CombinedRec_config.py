from AthenaCommon.Logging import logging


mlog = logging.getLogger( 'CombinedRec_config' )


from AthenaCommon.GlobalFlags  import globalflags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from RecExConfig.ObjKeyStore import objKeyStore

from AthenaCommon.Resilience import treatException,protectedInclude


# use to flag domain
import PerfMonComps.DomainsRegistry as pdr

from AODFix.AODFix import *
AODFix_Init()


from CaloRec.CaloRecFlags import jobproperties
#
# functionality : electron photon identification
#
#

pdr.flag_domain('egamma')
if rec.doEgamma():
    protectedInclude( "egammaRec/egammaRec_jobOptions.py" )
AODFix_postEgammaRec()


#
# functionality : Muon combined reconstruction
#
pdr.flag_domain('muoncomb')
if rec.doMuonCombined() and DetFlags.Muon_on() and DetFlags.ID_on():
    try:
        include ("MuonCombinedRecExample/MuonCombinedRec_config.py")
    except Exception:
        treatException("Could not set up combined muon reconstruction. Switched off !")
        rec.doMuonCombined = False

#
#  functionality : add cells crossed by high pt ID tracks 
#
if rec.doESD() and recAlgs.doTrackParticleCellAssociation() and DetFlags.ID_on():
    from AthenaCommon.CfgGetter import getPublicTool
    getPublicTool("MuonCombinedInDetDetailedTrackSelectorTool")
    topSequence += CfgMgr.TrackParticleCellAssociationAlg("TrackParticleCellAssociationAlg")

#
# functionality : energy flow
#                                                                                                 
pdr.flag_domain('eflow')
if recAlgs.doEFlow() and ( rec.readESD() or ( DetFlags.haveRIO.ID_on() and DetFlags.haveRIO.Calo_allOn() and  DetFlags.haveRIO.Muon_allOn()) )  :
    try:
        include( "eflowRec/eflowRec_jobOptions.py" )
    except Exception:
        treatException("Could not set up EFlow. Switched off !")
        recAlgs.doEFlow=False
else:
    recAlgs.doEFlow=False

#AODFix_postMuonCombinedRec()
#
# functionality : CaloTower protojets + preclustering + KT algorithm + CombinedJetAlg
#
pdr.flag_domain('jet')
jetOK=False
if rec.doJetMissingETTag() and DetFlags.Calo_on():
    try:
        from JetRec.JetRecFlags import jetFlags
        if jetFlags.Enabled():
            include( "JetRec/JetRec_jobOptions.py" )
            jetOK=jetFlags.Enabled()
    except Exception:
        treatException("Could not set up jet reconstruction")
        jetOK=False
    AODFix_postJetRec()

#
# functionality : isolation for egamma and combined muon
#
pdr.flag_domain('egmiso')
if (rec.doESD() and (rec.doMuonCombined() or rec.doEgamma()) and
    (jobproperties.CaloRecFlags.doCaloTopoCluster() or
     objKeyStore.isInInput ('xAOD::ParticleContainer', 'CaloCalTopoClusters'))):
    try:
        from IsolationAlgs.IsoGetter import isoGetter
        isoGetter()
    except Exception:
        treatException("Could not set up isolation. Switched off !")

if jetOK and recAlgs.doMuonSpShower() and DetFlags.detdescr.Muon_on() and DetFlags.haveRIO.Calo_on() :
    try:
        include("MuonSpShowerBuilderAlgs/MuonSpShowerBuilder_jobOptions.py")
    except Exception:    
        treatException("Could not set up MuonSpShower. Switched off !")
        recAlgs.doMuonSpShower=False
else:
    recAlgs.doMuonSpShower=False

pdr.flag_domain('btagging')
btaggingOK = False
if jetOK and rec.doBTagging() and  DetFlags.ID_on() and DetFlags.Muon_on():
    try:
        from BTagging.BTaggingFlags import BTaggingFlags
        protectedInclude( "BTagging/BTagging_jobOptions.py")
    except Exception:
        treatException("Could not set up btagging reconstruction")
        btaggingOK=False
        pass
    pass

#
# functionality : tau identification
#
pdr.flag_domain('tau')
if jetOK and rec.doTau():
    protectedInclude ("tauRec/tauRec_config.py")
AODFix_posttauRec()





#
# functionality : Missing Et
#
pdr.flag_domain('jet')
if recAlgs.doMissingET() and DetFlags.Calo_on() and DetFlags.ID_on() and DetFlags.Muon_on():
    try:
        include( "METReconstruction/METReconstruction_jobOptions.py" )
    except Exception:
        treatException("Could not set up MissingET. Switched off !")
        recAlgs.doMissingET=False
        
else:
    recAlgs.doMissingET=False
AODFix_postMissingETRec()

#
# functionality : Missing Et significance
#

if recAlgs.doMissingETSig() and ( rec.readESD() or DetFlags.haveRIO.Calo_on()) :
  include( "MissingETSig/MissingETSig_jobOptions.py" )
else:
  recAlgs.doMissingETSig=False

#
# Functionality: CaloRinger
#
pdr.flag_domain('caloringer')
if rec.doCaloRinger:
  include('CaloRingerAlgs/CaloRinger_jobOptions.py')


        
