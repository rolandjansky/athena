#
# jobO to include all the fragments specifying which containers
# are written to AOD. 
# Each fragment should return a list to be included into fullAODList 
# to be declared in StreamAOD
#
# Sublist included:
#  EventAODList  
#  InDetAODList
#  CalorimeterAODList
#  MuonAODList
#  MissingEtAODList
#  JetAODList
#  TauAODList
#  egammaAODList
#  TriggerAODList
#  McTruthAODList
#  AtlfastAODList
#

#part of this is now filled through object key store. but this require the "persistent" one to be properly filled.

#athena -c "doDumpProperties=True" RecExCommon/rdotoesd.py
#cp OKS_streamAOD_rdotoesd.py RecExPers/share/OKS_streamAOD_fromESD.py
#cp OKS_streamESD_rdotoesd.py RecExPers/share/OKS_streamESD.py
#then
#athena -c "doDumpProperties=True" RecExCommon/esdtoaod.py
#cp OKS_streamAOD_esdtoaod.py RecExPers/share/OKS_streamAOD.py
# once we are confident we are reading correctly OKS from the input file, only OKS_streamAOD_fromESD.py is needed.

#there is small logic flow in KeyStore.py :
# object added to any streams are also added to transient,
#  this is wrong if stream is an output stream. But KeyStore does not know which are input, which are output stream.
# sets this up at construction? Adding to outpustream is inconsistent


from AthenaCommon.Resilience import treatException,protectedInclude


from AthenaCommon.Logging import logging
logRecoOutputItemList_jobOptions = logging.getLogger( 'py:RecoOutputItemList_jobOptions' )

from RecExConfig.RecFlags import rec

from AthenaCommon.KeyStore import CfgItemList

# Start with an empty list:
fullAODList = CfgItemList( "AodList" )

# EventInfo stuff
fullItemList = ["xAOD::EventInfo#*","xAOD::EventAuxInfo#*"]

try:
    include ( "EventAthenaPool/EventAthenaPoolItemList_joboptions.py")
    fullAODList += CfgItemList( "EventAthenaPool",
                            items = fullItemList,
                            allowWildCard = True )
except Exception:
    treatException("Could not include EventAthenaPool/EventAthenaPoolItemList_joboptions.py")

# RawInfoSummaryForTag
try:
    fullItemList = []
    protectedInclude ( "EventTagAthenaPOOL/EventTagAthenaPOOLItemList_joboptions.py")
    fullAODList += CfgItemList( "EventTagAthenaPOOL",
                                items = fullItemList,
                                allowWildCard = True )
except Exception:
    treatException("Could not include EventTagAthenaPOOL/EventTagAthenaPOOLItemList_joboptions.py")
    
# MC Event Collection. Should be moved to a separate jobO
if rec.doTruth():
    McTruthAODList=["xAOD::TruthEventContainer#TruthEvents",
                    "xAOD::TruthEventAuxContainer#TruthEventsAux.",
                    "xAOD::TruthParticleContainer#TruthParticles",
                    "xAOD::TruthParticleAuxContainer#TruthParticlesAux.-caloExtension",
                    "xAOD::TruthVertexContainer#TruthVertices", 
                    "xAOD::TruthVertexAuxContainer#TruthVerticesAux.",
                    "xAOD::TruthPileupEventContainer#TruthPileupEvents",
                    "xAOD::TruthPileupEventAuxContainer#TruthPileupEventsAux."]
    fullAODList += CfgItemList( "McTruthAod",
                                items = McTruthAODList )

# Inner Detector
if DetFlags.detdescr.ID_on():
    try:
        include ("InDetRecExample/WriteInDetAOD.py")
        #FIXME if ESD to AOD InDetAODList is not properly filled
        #InDetAODList+=["VxContainer#"+InDetKeys.PrimaryVertices()]
        #InDetAODList+=["VxContainer#"+InDetKeys.Conversions()]
        #InDetAODList+=["V0Container#V0Candidates"]
        #InDetAODList+=["TrackParticleTruthCollection#"+InDetKeys.TrackParticlesTruth()]
        #InDetAODList+=["Rec::TrackParticleContainer#"+InDetKeys.TrackParticles()]
        
        from InDetRecExample.InDetKeys import InDetKeys
        fullAODList+= CfgItemList(
                     "InDetAod",
                     items = InDetAODList
                     )

    except Exception:
       treatException("Could not include InDetRecExample/WriteInDetAOD.py" )


# FTK 
if DetFlags.detdescr.FTK_on():
    protectedInclude ("FTK_RecExample/WriteFTK_AOD.py") 
    fullAODList += CfgItemList( "FTKAod", items = FtkAODList )

# Calorimeters 
if DetFlags.detdescr.Calo_on():
    try:
       include ("CaloRecEx/CaloRecOutputItemList_jobOptions.py") 
       fullAODList += CfgItemList( "CaloAod", items = CaloAODList )
    except Exception:
       treatException("Could not include CaloRecEx/CaloRecOutputItemList_jobOptions.py" )

# Muon Spectrometer
if DetFlags.detdescr.Muon_on():
    try:
        include("MuonRecExample/MuonRecOutputItemList_jobOptions.py")
        fullAODList += CfgItemList( "MuonAod", items = MuonAODList )

    except Exception:
        treatException("Could not MuonRecExample/MuonRecOutputItemList_jobOptions.py")



if DetFlags.detdescr.ALFA_on():
    protectedInclude("ForwardRec/ALFARec_OuputItemList_jobOptions.py")
    fullAODList += CfgItemList("ALFA", items = AlfaItemList)


if DetFlags.detdescr.AFP_on():
    include ("ForwardRec/AFP_Rec_OutputItemList_jobOptions.py")
    fullAODList += CfgItemList("AFP",items = AFP_ItemList)

        
# Tau:
try:
    protectedInclude ("tauRec/TauAODList.py")
    fullAODList += CfgItemList( "TauAod", items = TauAODList )
except Exception:
    treatException("Could not tauRec/TauAODList.py")

# MissingET: 
try:
    include ("METReconstruction/METReconstructionOutputAODList_jobOptions.py")
    fullAODList += CfgItemList( "MissingETAod", items = MissingETAODList )
except Exception:
    treatException("Could not load MET item list")

# Jet classes. 
try:
   from JetRec.JetRecUtils import retrieveAODList
   jetAODList = retrieveAODList()
   fullAODList += CfgItemList( "jetAOD", items = jetAODList )
except Exception:
   treatException("Could not load jet item list")


#BTagging
try:
    include("BTagging/BTaggingReconstructionOutputAODList_jobOptions.py")
    fullAODList += CfgItemList( "BTaggingAod", items = BTaggingAODList, allowWildCard = True )
except Exception:
    treatException("Could not load BTagging item list")    

#isolation, EventShape containers for ED correction
try:
    include("IsolationAlgs/IsoEventShapeOutputItemList_jobOptions.py")
    fullAODList += CfgItemList( "Isolation", items = IsoAODESList)
except Exception:
    treatException("Could not load IsoEventShape item list")   

# uncomment
# FIXME : there is a logic inconsistency
# either the jet should add streamAOD at ESD building stage (preferred)
# either `from TrigEDMConfig.TriggerEDM import getTriggerEDMList` should be used for trigger
# # Jets. : pass jets from input or from StreamESD to StreamAOD (some merging  inCfgItemList beforehand just in case)
## from RecExConfig.ObjKeyStore import cfgKeyStore
## jetAodList = CfgItemList( "JetAodList" )
## for cfgKS in ['inputFile','streamESD']:
##    _adict=cfgKeyStore[cfgKS].dict()
##    if _adict.has_key('JetCollection'):
##          jetAodList += CfgItemList( "JetAodJetc", items = {'JetCollection': _adict['JetCollection']}, allowWildCard = True )
##    if _adict.has_key('JetKeyDescriptor'):
##          jetAodList += CfgItemList( "JetAodJetKey", items = {'JetKeyDescriptor': _adict['JetKeyDescriptor']}, allowWildCard = True )


##    cfgKeyStore.addManyTypesStreamAOD(jetAodList())

                                    


##     "JetCollection#Cone4H1TopoJets",
##     "JetCollection#Cone4H1TowerJets",
##     "JetCollection#Cone4TruthJets",
##     "JetCollection#Cone7H1TopoJets",
##     "JetCollection#Cone7H1TowerJets",
##     "JetCollection#Cone7TruthJets",
##     "JetCollection#Kt4H1TopoJets",
##     "JetCollection#Kt4H1TowerJets",
##     "JetCollection#Kt4TruthJets",
##     "JetCollection#Kt6H1TopoJets",
##     "JetCollection#Kt6H1TowerJets",
##     "JetCollection#Kt6TruthJets",
##     "JetKeyDescriptor#JetKeyMap",    
##    ] )


# egamma

if rec.doEgamma():
    try:
        include ( "egammaRec/egammaOutputItemList_jobOptions.py" )
        fullAODList += CfgItemList( "egammaAod", items = egammaAODList )
    except Exception:
        treatException("Could not load egammaRec/egammaOutputItemList_jobOptions.py" )

    if InDetFlags.doR3LargeD0() and InDetFlags.storeSeparateLargeD0Container():
        try:
            include ( "egammaRec/egammaLRTOutputItemList_jobOptions.py" )
            fullAODList += CfgItemList( "egammaLRTAod", items = egammaLRTAODList )
        except Exception:
            treatException("Could not load egammaRec/egammaLRTOutputItemList_jobOptions.py" )


#eflow
try:
    include("eflowRec/eflowRecAODList.py")
    fullAODList += CfgItemList("eflowAod",items=eflowRecAODList)
except:
    treatException("Could not load eflow AOD item list")
    pass


# Muon Spectrometer
if DetFlags.detdescr.Muon_on() or DetFlags.detdescr.Calo_on():
    try:
        include("MuonCombinedRecExample/MuonCombined_OutputItemsAOD.py")
        fullAODList += CfgItemList( "MuonCombinedAod", items = MuonCombinedAODList )
    except Exception:
        treatException("Could not load MCRE/MuonCombined_OutputItemsAOD.py" )

trackParticleCellAssociationList=["xAOD::CaloClusterContainer#InDetTrackParticlesAssociatedClusters",
                                  "xAOD::CaloClusterAuxContainer#InDetTrackParticlesAssociatedClustersAux.",
                                  "CaloClusterCellLinkContainer#InDetTrackParticlesAssociatedClusters_links",
                                  "xAOD::TrackParticleClusterAssociationContainer#InDetTrackParticlesClusterAssociations",
                                  "xAOD::TrackParticleClusterAssociationAuxContainer#InDetTrackParticlesClusterAssociationsAux."]
fullAODList += CfgItemList("trackParticleCellAssoAod",items=trackParticleCellAssociationList)

# Atlfast, note ESD and AOD lists are equal

from RecExConfig.RecAlgsFlags import recAlgs
from ParticleBuilderOptions.AODFlags import AODFlags
if recAlgs.doAtlfast() or AODFlags.FastSimulation :
    try:
        include ("AtlfastAlgs/AtlfastOutputList_jobOptions.py") 
        fullAODList += CfgItemList( "AtlfastAod", items = AtlfastAODList)
    except Exception:
        treatException("could not import AtlfastAlgs/AtlfastOutputList_jobOptions.py") 

##     # This should move to some atlfast jobO
##     fullAODList += CfgItemList( "Atlfast", items = [
##         "ElectronContainer#AtlfastElectronCollection",
##         "PhotonContainer#AtlfastPhotonCollection",
##         "MuonContainer#AtlfastMuonCollection",
##         "MuonContainer#AtlfastNonIsoMuonCollection",
##         "Analysis::MuonContainer#AtlfastMuonCollection",
##         "Analysis::MuonContainer#AtlfastNonIsoMuonCollection",
##         "Analysis::TauJetContainer#AtlfastTauJetContainer",
##         "Analysis::TauJetContainer#AtlfastTauJet1p3pContainer",
##         "ParticleJetContainer#AtlfastParticleJetContainer",
##         "Rec::TrackParticleContainer#AtlfastTrackParticles"
##         ] )

# Trigger

# Now filled through objKeyStore in configurables (see HLTTriggerGetter)



# Common items: tracks and cells for e/mu/tau
fullAODList += CfgItemList( "CommonAod",
                             #items = [ "TrackCollection#Tracks", "CaloCellContainer#AODCellContainer" ] ) # Turned off for DC14 (no slimming yet)
                            items = [ "CaloCellContainer#AODCellContainer" ] )
                            

# Request for W. Ehrenfeld: Add Calibration hits to ESD/AOD files if present in input RDO
if rec.doWriteCalibHits():
    fullAODList += CfgItemList( "CalibrationHits",
                                items=["CaloCalibrationHitContainer#LArCalibrationHitActive",
                                       "CaloCalibrationHitContainer#LArCalibrationHitInactive",
                                       "CaloCalibrationHitContainer#LArCalibrationHitDeadMaterial",
                                       "CaloCalibrationHitContainer#TileCalibrationDMHitCnt",
                                       "CaloCalibrationHitContainer#TileCalibrationCellHitCnt",
                                       ]
                                )



# Heavy Ion:
if rec.doHeavyIon():
    try:
        include ("HIRecExample/HIRecOutputAODList_jobOptions.py")
        fullAODList += CfgItemList( "HeavyIonsAod", items = HIAODItemList )
    except Exception:
        treatException("Could not load HIRecExample/HIRecOutputAODList_jobOptions.py")

# ring-shaped calorimetry
if rec.doCaloRinger():
    try:
        include ( "CaloRingerAlgs/CaloRingerOutputItemList_jobOptions.py" )
        fullAODList += CfgItemList( "caloRingerAod", items = caloRingerAODList )
        StreamAOD_Augmented.AddMetaDataItem( caloRingerMetaDataList )
    except Exception:
        treatException("Could not load CaloRingerAlgs/CaloRingerOutputItemList_jobOptions.py" )


# now merge the explicit AOD list to the one coming from ObjKeyStore
# (more and more will be taken from ObjKeyStore)
from AthenaCommon.KeyStore import CfgItemList, CfgKeyStore
theCKS=CfgKeyStore("KeyStore")
CILMergeAOD=CfgItemList("AODMerge",allowWildCard=True)
CILMergeAOD.add(fullAODList())
CILMergeAOD.add(theCKS.streamAOD())

#now prune the itemlist from the object which are soft linked

# this doe not work CILMergeAOD.remove("DataVector<INavigable4Momentum>")
#this does
for k in CILMergeAOD('DataVector<INavigable4Momentum>'):
    CILMergeAOD.removeItem(k)

# The only items declared in StreamAOD are those written here.
# Note the use of "=" instead of "+=" is deliberate

StreamAOD.ItemList = CILMergeAOD()

if rec.OutputLevel() <= DEBUG:
    printfunc ("RecoOutputESDList_jobOptions:: AOD Output list is :")
    printfunc (StreamAOD.ItemList)
