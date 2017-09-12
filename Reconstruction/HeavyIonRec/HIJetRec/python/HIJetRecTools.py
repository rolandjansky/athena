# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from HIJetRec.HIJetRecFlags import HIJetFlags
from JetRec.JetRecFlags import jetFlags
from JetRec.JetRecStandard import jtm
import AthenaCommon.SystemOfUnits as Units

#configuring getter tools
#selection for track jets
from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
from JetRecTools.JetRecToolsConf import TrackPseudoJetGetter
from JetRecTools.JetRecToolsConf import JetTrackSelectionTool
from JetRecTools.JetRecToolsConf import SimpleJetTrackSelectionTool
from JetRecTools.JetRecToolsConf import TrackVertexAssociationTool
from JetMomentTools.JetMomentToolsConf import JetCaloQualityTool 
from JetMomentTools.JetMomentToolsConf import JetCaloCellQualityTool 

#select the tracks
if jetFlags.Enabled() : HIJetFlags.UseHITracks.set_Value_and_Lock(False)
if HIJetFlags.UseHITracks() :
    jtm += InDet__InDetTrackSelectionTool("trk_tracksel_HI",
                                          minPt                = HIJetFlags.TrackInputPtMin(),
                                          maxAbsEta            = 2.5,
                                          minNSiHits           = 7,
                                          maxNSiSharedModules  = 100,
                                          maxNSiHoles          = 2,
                                          maxNPixelHoles       = 1)

    #select the tracks for jet finding
    jtm += JetTrackSelectionTool( "tracksel_HI",
                                  InputContainer  = jtm.trackContainer,
                                  OutputContainer = "JetSelectedTracks_HI",
                                  Selector        = jtm.trk_tracksel_HI)

    #vertex association tool
    jtm += TrackVertexAssociationTool("tvassoc_HI",
                                  TrackParticleContainer  = jtm.trackContainer,
                                  TrackVertexAssociation  = "JetTrackVtxAssoc_HI",
                                  VertexContainer         = jtm.vertexContainer,
                                  MaxTransverseDistance   = 1.5,
                                  MaxLongitudinalDistance = 1.0e7,
                                  MaxZ0SinTheta = 1.5)
    #build PJ getter
    jtm += TrackPseudoJetGetter("trackget_HI",
                                InputContainer = jtm.tracksel_HI.OutputContainer,
                                Label = "Track",
                                OutputContainer = "PseudoJetTracks_HI",
                                TrackVertexAssociation = jtm.tvassoc_HI.TrackVertexAssociation,
                                SkipNegativeEnergy = True,
                                GhostScale = 0.0)
    #now for ghost tracks
    jtm += InDet__InDetTrackSelectionTool("trk_gtracksel_HI",
                                          minPt                = 400.*Units.MeV,
                                          maxAbsEta            = 2.5,
                                          minNSiHits           = 7,
                                          maxNSiSharedModules  = 100,
                                          maxNSiHoles          = 2,
                                          maxNPixelHoles       = 1)
    #select the tracks for jet finding
    jtm += JetTrackSelectionTool( "gtracksel_HI",
                                  InputContainer  = jtm.trackContainer,
                                  OutputContainer = "JetSelectedTracks_HI_ghost",
                                  Selector        = jtm.trk_gtracksel_HI)
    
    
    jtm += TrackPseudoJetGetter("gtrackget_HI",
                                InputContainer = jtm.gtracksel_HI.OutputContainer,
                                Label = "GhostTrack",
                                OutputContainer = "PseudoJetGhostTracks_HI_ghost",
                                TrackVertexAssociation = jtm.tvassoc_HI.TrackVertexAssociation,
                                SkipNegativeEnergy = True,
                                GhostScale = 1e-20)

    jtm.jvf.unlock()
    jtm.jvf.TrackVertexAssociation=jtm.tvassoc_HI.TrackVertexAssociation
    jtm.jvf.lock()

    jtm.trkmoms.unlock()
    jtm.trkmoms.TrackVertexAssociation=jtm.tvassoc_HI.TrackVertexAssociation
    jtm.trkmoms.TrackMinPtCuts = [int(500*Units.MeV), int(1*Units.GeV), int(2*Units.GeV), int(4*Units.GeV)]
    jtm.trkmoms.lock()


from JetRec.JetRecConf import PseudoJetGetter
ClusterKey=HIJetFlags.HIClusterKey()

from HIJetRec.HIJetRecConf import HIClusterPseudoJetGetter
jtm += HIClusterPseudoJetGetter("get_HI",
                                InputContainer = ClusterKey,
                                Label = "LCTopo", #Label = "Tower",
                                OutputContainer = "PseudoJet" + ClusterKey,
                                SkipNegativeEnergy = False,
                                TreatNegativeEnergyAsGhost=True,
                                GhostScale = 1.e-20
                                )

jtm += PseudoJetGetter("gakt4trackget_HI", 
                       InputContainer = HIJetFlags.TrackJetContainerName(),
                       Label = "Ghost" + HIJetFlags.TrackJetContainerName(), 
                       SkipNegativeEnergy = True,
                       OutputContainer = "PseudoJetGhost" +  HIJetFlags.TrackJetContainerName(),
                       GhostScale = 1.e-20)                       

HIgetters_ghost_track = []
HIgetters_common=[]
if jetFlags.useMuonSegments(): HIgetters_common += [jtm.gmusegget]
if jetFlags.useTracks(): 
    HIgetters_ghost_track += [jtm.gakt4trackget_HI]
    if HIJetFlags.UseHITracks() : HIgetters_ghost_track += [jtm.gtrackget_HI]
    else: HIgetters_ghost_track += [jtm.gtrackget]

if jetFlags.useTruth(): 
    #HIgetters_common += [jtm.gtruthget]
    flavorgetters=[]
    for ptype in jetFlags.truthFlavorTags():
        flavorgetters += [getattr(jtm, "gtruthget_" + ptype)]
    HIgetters_common += flavorgetters

jtm.gettersMap['HI'] = [jtm.get_HI]
jtm.gettersMap['HI'] += HIgetters_common
jtm.gettersMap['HI'] += HIgetters_ghost_track

jtm.gettersMap['HITrack'] = []
if HIJetFlags.UseHITracks() : jtm.gettersMap['HITrack'] += [jtm.trackget_HI]
else : jtm.gettersMap['HITrack'] += [jtm.trackget]
jtm.gettersMap['HITrack'] += HIgetters_common


from JetRec.JetRecConf import JetFilterTool
jetfil5=JetFilterTool("jetfil5", PtMin = 5*Units.GeV)
jtm.add(jetfil5)

jetfilHI=JetFilterTool("jetfilHI", PtMin = HIJetFlags.RecoOutputPtMin() )
jtm.add(jetfilHI)

jetfilHISeeds=JetFilterTool("jetfilHISeeds", PtMin = HIJetFlags.SeedPtMin() )
jtm.add(jetfilHISeeds)

#DR association- form element links for all clusters w/in DR of each jet
DR=HIJetFlags.ClusterDRAssociation()
from HIJetRec.HIJetRecConf import HIJetDRAssociationTool
assoc=HIJetDRAssociationTool("HIJetDRAssociation")
assoc.ContainerKey=ClusterKey
assoc.DeltaR=DR
assoc.AssociationName="%s_DR%dAssoc" % (ClusterKey,int(10*DR))
jtm.add(assoc)



#calculate discriminants as moments
from HIJetRec.HIJetRecConf import HIJetMaxOverMeanTool
max_over_mean=HIJetMaxOverMeanTool("HIJetMaxOverMean")
jtm.add(max_over_mean)

#filter jets based on discriminants
from HIJetRec.HIJetRecConf import HIJetDiscriminatorTool
discrim=HIJetDiscriminatorTool("discrim")
discrim.MaxOverMeanCut=HIJetFlags.DCutMaxOverMean()
discrim.MinimumETMaxCut=HIJetFlags.DCutMax()
jtm.add(discrim)

jtm.modifiersMap['HI_Unsubtr']=[assoc,max_over_mean,jetfil5] 

hi_trk_modifiers=[assoc,max_over_mean,jtm.width]
hi_modifiers = []

#helper tool to apply bkgr using cells
from HIJetRec.HIJetRecConf import HIJetCellSubtractorTool
jtm.add(HIJetCellSubtractorTool("HIJetCellSubtractor"))

from HIJetRec.HIJetRecConf import HIJetClusterSubtractorTool
cl_subtr_tool=HIJetClusterSubtractorTool("HIJetClusterSubtractor")
cl_subtr_tool.ConfigDir='HIEventUtils/'
jtm.add(cl_subtr_tool)

hi_calib_map={}
if HIJetFlags.ApplyEtaJESCalibration() :
    from JetCalibTools.JetCalibToolsConf import JetCalibrationTool
    for R in HIJetFlags.AntiKtRValues() : 
        calib_seq='EtaJES'
        JES_is_data=True
        if jetFlags.useTruth(): JES_is_data=False
        elif R is 0.4 : calib_seq='EtaJES_Insitu' #only do in situ for R=0.4 jets in data
        calib_tool=JetCalibrationTool('HICalibToolR%d' % int(10*R),JetCollection='AntiKt%dHI' % int(10*R),
                                      ConfigFile='JES_MC15c_HI_Nov2016.config',CalibSequence=calib_seq,IsData=JES_is_data)

        jtm.add(calib_tool)
        hi_calib_map['AntiKt%dHIJets' % int(10*R)]=calib_tool
jtm.HICalibMap=hi_calib_map
hi_modifiers += [jtm.jetfilHI,jtm.jetsorter]
hi_modifiers+=[jtm.width,jtm.jetens,jtm.larhvcorr]

if jetFlags.useCaloQualityTool():
    hi_modifiers += [jtm.caloqual_cluster]
if jetFlags.useTracks(): 
    hi_modifiers += [jtm.trkmoms, jtm.jvf, jtm.jvt]
#    hi_modifiers += [jtm.jvf, jtm.jvt, jtm.trkmoms]
if jetFlags.useTruth():
    hi_modifiers += [jtm.truthpartondr,jtm.partontruthlabel]
    hi_trk_modifiers += [jtm.truthpartondr,jtm.partontruthlabel]



jtm.modifiersMap['HI']=hi_modifiers
jtm.modifiersMap['HITrack']=hi_trk_modifiers

assoc_name=assoc.AssociationName
HIJetFinderDefaults=dict(ghostArea=0.0, ptminFilter= 5*Units.GeV)

jtm.HIJetRecs=[]
