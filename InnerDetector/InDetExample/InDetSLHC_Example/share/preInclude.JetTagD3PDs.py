from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
primDPD.SetupTrigDecisionTool.set_Value_and_Lock(False)

JetTagD3PD_slimD3PD=False
JetTagD3PD_type='Full'
JetTagD3PD_flavor='Full'
JetTagD3PD_calibTaggers=False
JetTagD3PD_redoTagging=False
JetTagNtuple_AntiKt4Truth=False
JetTagNtuple_AntiKt4TopoEM=False


JetTagD3PD_AntiKt4TopoEM=False
JetTagD3PD_AntiKt6TopoEM=False
JetTagD3PD_AntiKt4LCTopo=False
JetTagD3PD_AntiKt6LCTopo=False
JetTagD3PD_AntiKt4Truth=True
JetTagD3PD_AntiKt4MuonNonInteractingTruth=False
JetTagD3PD_AntiKt6MuonNonInteractingTruth=False
JetTagD3PD_AntiKt4TruthPurePU=False
JetTagD3PD_AddCAJets=False


from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags
TrackD3PDFlags.storeDetailedTruth.set_Value_and_Lock(False)
TrackD3PDFlags.storeTrackSummary.InfoPlus=True
TrackD3PDFlags.storePixelHitsOnTrack.set_Value_and_Lock(False)
TrackD3PDFlags.storeBLayerHitsOnTrack.set_Value_and_Lock(False)
TrackD3PDFlags.storeSCTHitsOnTrack.set_Value_and_Lock(False)
TrackD3PDFlags.storeBLayerOutliersOnTrack.set_Value_and_Lock(False)
TrackD3PDFlags.storePixelOutliersOnTrack.set_Value_and_Lock(False)
TrackD3PDFlags.storeBLayerHolesOnTrack.set_Value_and_Lock(False)
TrackD3PDFlags.storePixelHolesOnTrack.set_Value_and_Lock(False)
TrackD3PDFlags.storeTrackPredictionAtBLayer.set_Value_and_Lock(False)


from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags

JetTagD3PDFlags.AddJetTriggerObject.set_Value_and_Lock(False)
JetTagD3PDFlags.AddBJetTriggerObject.set_Value_and_Lock(False)
JetTagD3PDFlags.AddTriggerObject.set_Value_and_Lock(False)
JetTagD3PDFlags.AddTriggerBits.set_Value_and_Lock(False)
JetTagD3PDFlags.JetMuonAssoc.set_Value_and_Lock(False)
JetTagD3PDFlags.JetElectronAssoc.set_Value_and_Lock(False)
JetTagD3PDFlags.JetPhotonAssoc.set_Value_and_Lock(False)
JetTagD3PDFlags.AddMuonInJetsObject.set_Value_and_Lock(False)
JetTagD3PDFlags.AddElectronInJetsObject.set_Value_and_Lock(False)
JetTagD3PDFlags.AddPhotonInJetsObject.set_Value_and_Lock(False)
JetTagD3PDFlags.METObjects=[]

JetTagD3PDFlags.AddPixelClusterObject.set_Value_and_Lock(False)
JetTagD3PDFlags.AddTrackObject.set_Value_and_Lock(True)
JetTagD3PDFlags.AddGenObjects.set_Value_and_Lock(True)
JetTagD3PDFlags.AddGenParticleObjects.set_Value_and_Lock(True)
JetTagD3PDFlags.AddTruthTrackObjects.set_Value_and_Lock(True)
JetTagD3PDFlags.JetTrackAssoc.set_Value_and_Lock(True)
JetTagD3PDFlags.JetCollections=['AntiKt4TruthJets']
JetTagD3PDFlags.JetCollections.lock()
JetTagD3PDFlags.AddTriggerObject.set_Value_and_Lock(False)
JetTagD3PDFlags.AddGenVertexObjects.set_Value_and_Lock(True)

BTaggingFlags.Active=True
BTaggingFlags.UseLatestJetCollsForNtuple=True
BTaggingFlags.SoftMuChi2=False
BTaggingFlags.writeSecondaryVertices=True
BTaggingFlags.SoftEl=False
BTaggingFlags.SoftMu=False
BTaggingFlags.SecondSoftMu=False
BTaggingFlags.IP2DFlip=True
BTaggingFlags.IP2DPos=True
BTaggingFlags.IP2DSpc=True
BTaggingFlags.IP2DSpcFlip=True
BTaggingFlags.IP2DSpcPos=True
BTaggingFlags.IP2DSpcNeg=True
BTaggingFlags.IP3DFlip=True
BTaggingFlags.IP3DPos=True
BTaggingFlags.IP3DSpc=True
BTaggingFlags.IP3DSpcFlip=True
BTaggingFlags.IP3DSpcPos=True
BTaggingFlags.IP3DSpcNeg=True
BTaggingFlags.SV1Flip =True
BTaggingFlags.SV2Flip=True
BTaggingFlags.Jets=["AntiKt4Truth"]
BTaggingFlags.RetagJets=["AntiKt4Truth"]
BTaggingFlags.JetsWithInfoPlus=["AntiKt4Truth"]
BTaggingFlags.CalibrationTag = "BTagCalibALL-07-01"


#Turning off JetFitter - doesn't work at the moment, needs training
BTaggingFlags.BasicJetFitter=False
BTaggingFlags.JetFitterTag=False
BTaggingFlags.JetFitterNN=False
BTaggingFlags.JetFitterTagFlip=False
BTaggingFlags.JetFitterNNFlip=False


BTaggingFlags.OutputLevel                = INFO
BTaggingFlags.CalibrationChannelAliases  = ["AntiKt4Truth->AntiKt4TopoEM"]
BTaggingFlags.Runmodus                   = "analysis"
BTaggingFlags.AutoInspectInputFile=False



include( "BTagging/BTagging_LoadTools.py" )

#The lines below are only needed when running from ESD
from JetRec.JetRecFlags import jetFlags
jetFlags.doBTagging       = True

from ParticleBuilderOptions.AODFlags import AODFlags
#AODFlags.JetTag.set_Value_and_Lock(True)
from McParticleAlgs.JobOptCfg import invokeTruthParticleGetter,TruthParticleContainerGetter,PileUpClassification
from JetRec.JetGetters import *
getter = invokeTruthParticleGetter(PileUpClassification.SIGNAL)    
mytruthjet = make_StandardJetGetter('AntiKt',0.4,'Truth').jetAlgorithmHandle()

