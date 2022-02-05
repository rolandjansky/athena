# some input are now redundantly taken rom ObjectKeyStore.streamESD(), they should be removed
#
# jobO to include all the fragments specifying which containers
# are written to AOD. 
# Each fragment should return a list to be included into fullESDList 
# to be declared in StreamESD
#
# Sublist included:
#  EventESDList  
#  DetectorStatusESDList
#  InDetESDList
#  CalorimeterESDList
#  MuonESDList
#  MissingEtESDList
#  JetESDList
#  TauESDList
#  egammaESDList
#  TriggerESDList
#  McTruthESDList
#  AtlfastESDList
#

from AthenaCommon.Resilience import treatException,protectedInclude


from AthenaCommon.GlobalFlags import globalflags


from AthenaCommon.Logging import logging
logRecoOutputItemList_jobOptions = logging.getLogger( 'py:RecoOutputItemList_jobOptions' )

from RecExConfig.RecFlags import rec

from AthenaCommon.KeyStore import CfgItemList

# Start with an empty list:
fullESDList = CfgItemList( "EsdList" )

# EventInfo stuff
fullItemList = ["xAOD::EventInfo#*","xAOD::EventAuxInfo#*"]
protectedInclude ( "EventAthenaPool/EventAthenaPoolItemList_joboptions.py")
fullESDList += CfgItemList( "EventAthenaPool",
                            items = fullItemList,
                            allowWildCard = True )

# DetectorStatus
#obsolete fullESDList += [ "DetStatusMap#DetectorStatus" ]

# RawInfoSummaryForTag
try:
    fullItemList = []
    protectedInclude ( "EventTagAthenaPOOL/EventTagAthenaPOOLItemList_joboptions.py")
    fullESDList += CfgItemList( "EventTagAthenaPOOL",
                                items = fullItemList,
                                allowWildCard = True )
except Exception:
    treatException("Could not include EventTagAthenaPOOL/EventTagAthenaPOOLItemList_joboptions.py")
    
# MC Event Collection. Should be moved to a separate jobO
if rec.doTruth():
    fullESDList += CfgItemList( "McTruthEsd",
                                items = [ "McEventCollection#TruthEvent",
                                          "xAOD::TruthEventContainer#TruthEvents",
                                          "xAOD::TruthEventAuxContainer#TruthEventsAux.",
                                          "xAOD::TruthParticleContainer#TruthParticles",
                                          "xAOD::TruthParticleAuxContainer#TruthParticlesAux.",
                                          "xAOD::TruthParticleContainer#TruthPileupParticles",
                                          "xAOD::TruthParticleAuxContainer#TruthPileupParticlesAux.",
                                          "xAOD::TruthVertexContainer#TruthVertices", 
                                          "xAOD::TruthVertexAuxContainer#TruthVerticesAux.",
                                          "xAOD::TruthPileupEventContainer#TruthPileupEvents",
                                          "xAOD::TruthPileupEventAuxContainer#TruthPileupEventsAux."
                                          ] )
    if jobproperties.Beam.beamType=="cosmics" and globalflags.DataSource=="geant4":
        fullESDList += CfgItemList( "McTruthTracRecord",
                   items = [ "TrackRecordCollection#CosmicRecord", "TrackRecordCollection#CosmicPerigee" ] )



# Inner Detector
if DetFlags.detdescr.ID_on():
    # needed because WriteInDetEsd is filling directly StreamESD which is not defined anymore at this point
    try:
        protectedInclude ("InDetRecExample/WriteInDetESD.py")
    except Exception:
        import traceback
        tra=traceback.format_exc()
        # check if this is the expected error, otherwise rethrow the exception
        if tra.find("NameError: name 'StreamESD' is not defined")<0:
            raise

    fullESDList+= CfgItemList(
                     "InDetEsd",
                     items = InDetESDList
                     )
# FTK 
if DetFlags.detdescr.FTK_on():
    protectedInclude ("FTK_RecExample/WriteFTK_ESD.py") 
    fullESDList += CfgItemList( "FTKEsd", items = FtkESDList )

# Calorimeters 
if DetFlags.detdescr.Calo_on():
    protectedInclude ("CaloRec/CaloRecOutputItemList_jobOptions.py") 
    fullESDList += CfgItemList( "CaloEsd", items = CaloESDList )

# Muon Spectrometer
if DetFlags.detdescr.Muon_on():
    protectedInclude("MuonRecExample/MuonRecOutputItemList_jobOptions.py")
    fullESDList += CfgItemList( "MuonEsd", items = MuonESDList )

#LUCID
if DetFlags.detdescr.Lucid_on():
    fullESDList += CfgItemList("Lucid", items = ["LUCID_RawDataContainer#Lucid_RawData"])

#ZDC
if DetFlags.detdescr.ZDC_on():
    fullESDList += CfgItemList("ZDC", items = ["ZdcDigitsCollection#ZdcDigitsCollection","ZdcRawChannelCollection#ZdcRawChannelCollection"])


if DetFlags.detdescr.ALFA_on():
    protectedInclude("ForwardRec/ALFARec_OuputItemList_jobOptions.py")
    fullESDList += CfgItemList("ALFA", items = AlfaItemList)


if DetFlags.detdescr.AFP_on():
    include ("ForwardRec/AFP_Rec_OutputItemList_jobOptions.py")
    fullESDList+=CfgItemList("AFP",items = AFP_ItemList)


# Tau:
try:
    include ("tauRec/TauESDList.py")
    fullESDList += CfgItemList( "TauEsd", items = TauESDList )
except Exception:
    treatException("Could not tauRec/TauESDList.py")

# MissingET 
try:
    include ("METReconstruction/METReconstructionOutputESDList_jobOptions.py")
    fullESDList += CfgItemList( "MissingETEsd", items = MissingETESDList )
except Exception:
    treatException("Could not load MET item list")

# Jet classes. 
try:
   from JetRec.JetRecUtils import retrieveAODList
   jetAODList = retrieveAODList(True)
   fullESDList += CfgItemList( "jets", items = jetAODList )
except Exception:
   treatException("Could not load jet item list")

#BTagging
try:
    if rec.doBTagging():
        include("BTagging/BTaggingReconstructionOutputAODList_jobOptions.py")
        fullESDList += CfgItemList( "BTaggingEsd", items = BTaggingESDList, allowWildCard = True )
except Exception:
    treatException("Could not load BTagging item list")    

#isolation, EventShape containers for ED correction
try:
    include("IsolationAlgs/IsoEventShapeOutputItemList_jobOptions.py")
    fullESDList += CfgItemList( "Isolation", items = IsoAODESList)
except Exception:
    treatException("Could not load IsoEventShape item list")   

#egamma
if rec.doEgamma():
    try:
        include ( "egammaRec/egammaOutputItemList_jobOptions.py" )
        fullESDList += CfgItemList( "egammaEsd", items = egammaESDList )
    except:
        treatException("Could not load egamma ESD item list")
        pass

    if InDetFlags.doR3LargeD0() and InDetFlags.storeSeparateLargeD0Container():
        try:
            include ( "egammaRec/egammaLRTOutputItemList_jobOptions.py" )
            fullESDList += CfgItemList( "egammaLRTEsd", items = egammaLRTESDList )
        except Exception:
            treatException("Could not load egamma ESD item list")
    pass   

#eflow
if recAlgs.doEFlow():
    try:
        include("eflowRec/eflowRecESDList.py")
        fullESDList += CfgItemList("eflowEsd",items=eflowRecESDList)
    except:
        treatException("Could not load eflow ESD item list")
        pass
    pass

if recAlgs.doTrackParticleCellAssociation():    
    trackParticleCellAssociationList=["xAOD::CaloClusterContainer#InDetTrackParticlesAssociatedClusters",
                                      "xAOD::CaloClusterAuxContainer#InDetTrackParticlesAssociatedClustersAux.",
                                      "CaloClusterCellLinkContainer#InDetTrackParticlesAssociatedClusters_links",
                                      "xAOD::TrackParticleClusterAssociationContainer#InDetTrackParticlesClusterAssociations",
                                      "xAOD::TrackParticleClusterAssociationAuxContainer#InDetTrackParticlesClusterAssociationsAux."]
    fullESDList += CfgItemList("trackParticleCellAssoEsd",items=trackParticleCellAssociationList)
    
#CaloRinger
if rec.doCaloRinger():
    try:
        include ( "CaloRingerAlgs/CaloRingerOutputItemList_jobOptions.py" )
        fullESDList += CfgItemList( "caloRingerEsd", items = caloRingerESDList )
        from RecExConfig.ObjKeyStore import objKeyStore
        objKeyStore['metaData'] += CfgItemList( "caloRingerMeta" , items = caloRingerMetaDataList )
    except:
        treatException("Could not load CaloRinger ESD item list")
        pass
    pass


# Muon combined reconstruction
if DetFlags.detdescr.Muon_on() or DetFlags.detdescr.Calo_on():
    protectedInclude("MuonCombinedRecExample/MuonCombined_OutputItemsESD.py")
    fullESDList += CfgItemList( "MuonCombinedEsd", items = MuonCombinedESDList )


from RecExConfig.RecAlgsFlags import recAlgs

# Atlfast, note ESD and AOD lists are equal
if recAlgs.doAtlfast():
    protectedInclude ("AtlfastAlgs/AtlfastOutputList_jobOptions.py")
    fullESDList += CfgItemList( "AtlfastEsd", items = AtlfastAODList)

# Trigger
# Now filled through objKeyStore in configurables (see HLTTriggerGetter)


# Heavy Ion:
if rec.doHeavyIon():
    protectedInclude ("HIRecExample/HIRecOutputESDList_jobOptions.py")
    fullESDList += CfgItemList( "HeavyIonsEsd", items = HIESDItemList )

# remove decorations that might be created by monitoring
if rec.doMonitoring():
    fullESDList += CfgItemList( "MonitoringEsd", 
                                items = ["xAOD::JetAuxContainer#AntiKt4EMTopoJetsAux.-jetClean_LooseBad"]
                              )

## StreamESD_Augmented.AddItem( "RecoTimingObj#RAWtoESD_timings" )
#fullESDList += CfgItemList( "TimingEsd",
#                            items=["RecoTimingObj#RAWtoESD_timings",
#                                   "RecoTimingObj#RAWtoESD_mems",
#                                   "RecoTimingObj#EVNTtoHITS_timings",
#                                   "RecoTimingObj#HITStoRDO_timings"] )


# Request for W. Ehrenfeld: Add Calibration hits to ESD/AOD files if present in input RDO
if rec.doWriteCalibHits():
    fullESDList += CfgItemList( "CalibrationHits",
                                items=["CaloCalibrationHitContainer#LArCalibrationHitActive",
                                       "CaloCalibrationHitContainer#LArCalibrationHitInactive",
                                       "CaloCalibrationHitContainer#LArCalibrationHitDeadMaterial",
                                       "CaloCalibrationHitContainer#TileCalibrationDMHitCnt",
                                       "CaloCalibrationHitContainer#TileCalibrationCellHitCnt",
                                       ]
                                )



# now merge the explicit ESD list to the one coming from ObjKeyStore
# (more and more will be taken from ObjKeyStore)
from AthenaCommon.KeyStore import CfgItemList, CfgKeyStore
theCKS=CfgKeyStore("KeyStore")
CILMergeESD=CfgItemList("ESDMerge",allowWildCard=True)
CILMergeESD.add(fullESDList())
CILMergeESD.add(theCKS.streamESD())

#now prune the itemlist from the object which are soft linked

# this doe not work CILMergeESD.remove("DataVector<INavigable4Momentum>")
#this does
for k in CILMergeESD('DataVector<INavigable4Momentum>'):
    CILMergeESD.removeItem(k)

# The only items declared in StreamESD are those written here.
# Note the use of "=" instead of "+=" is deliberate
#DR moved to RecExCommon_topOptions.py
#StreamESD.ItemList = CILMergeESD()

#merge back the complete list in objkeystore
#(note that oks and theCKS are two interfaces on the same underlying object)
# need to remove the wild card object like EventInfo etc...
CILMergeESDNoWild=CfgItemList("ESDMergeNoWild",allowWildCard=False)
for item in CILMergeESD():
    if not item.endswith("#*"):
        CILMergeESDNoWild.add(item)

objKeyStore.addManyTypesStreamESD(CILMergeESDNoWild())



if rec.OutputLevel() <= DEBUG:
    printfunc ("RecoOutputESDList_jobOptions:: ESD Output list is :")
    printfunc (CILMergeESD())
