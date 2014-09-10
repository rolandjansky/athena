# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
## @file TrackD3PDMaker/python/VertexGroupD3PD.py
## @brief Setup D3PD objects for vertex group D3PD production
## @author Simone Pagan Griso
## @date Mar, 2012
##

"""
This file is used to add Vertex Group D3PD objects
to a D3PD Maker algorithm
"""

from TrackD3PDMaker.VertexD3PDAnalysisFlags import VertexD3PDAnalysisFlags
from TrackD3PDMaker.VertexD3PDAnalysisKeys import VertexD3PDAnalysisKeys

import D3PDMakerCoreComps
import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject
from RecExConfig.RecFlags import rec


### Parse arguments with 'name' prefix and strip prefix
#  for specific options to target D3PD objects
# E.g. ei_level -> sets options 'level' of EventInfoD3PDObject
def _args (name, kwin, **kw):
    kw = kw.copy()
    for (k, v) in kwin.items():
        if k.startswith (name + '_'):
            kw[k[len(name)+1:]] = v
    return kw

### Main routing to add objects to algorithm 'alg'
def VertexGroupD3PD(alg = None,
                    file=VertexD3PDAnalysisKeys.D3PDFileName(),
                    treeName = VertexD3PDAnalysisKeys.D3PDTreeName(),
                    **kw):

    print '[TrackD3PDMaker.VertexGroupD3PD] Vertex group D3PD settings:'
    print 'VertexD3PDAnalysisFlags'
    print VertexD3PDAnalysisFlags
    print 'VertexD3PDAnalysisKeys'
    print VertexD3PDAnalysisKeys

    if not alg:
        ## Create a new algorithm
        from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
        alg = MSMgr.NewRootStream(treeName, file)

    ## General event information (custom options prefix: 'ei_')
    if VertexD3PDAnalysisFlags.useEventInfo():
        from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
        alg += EventInfoD3PDObject(**_args('ei_', kw, level=0, prefix='ei_'))        

    ## Trigger information (custom options prefix: 'trig_')
    if VertexD3PDAnalysisFlags.useTrigger():
        from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
        cfg = TriggerConfigGetter("ReadPool")
        from TriggerD3PDMaker.defineTriggerBits import defineTriggerBits
        from TriggerD3PDMaker.TrigConfMetadata  import addTrigConfMetadata
        from TriggerD3PDMaker.TrigDecisionD3PDObject import TrigDecisionD3PDObject
        alg += TrigDecisionD3PDObject(**_args('trig_', kw, level=2))
        addTrigConfMetadata(alg)
    if VertexD3PDAnalysisFlags.useTriggerRaw():
        from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
        cfg = TriggerConfigGetter("ReadPool")
        from TriggerD3PDMaker.defineTriggerBits import defineTriggerBits
        from TriggerD3PDMaker.TrigConfMetadata  import addTrigConfMetadata
        #Setup custom D3PD object with raw trigger info only (e.g. calibration_vdM streams)
        from D3PDMakerCoreComps.D3PDObject import make_Void_D3PDObject
        import TriggerD3PDMaker
        TrigDecisionD3PDObjectRaw = make_Void_D3PDObject( "trigRaw_", "TrigDecisionD3PDObjectRaw",default_name = "TrigDecisionFillerRaw" )
        if not VertexD3PDAnalysisFlags.useTrigger():
            TrigDecisionD3PDObjectRaw.defineBlock( 0, "TriggerRawInfo",
                                                   TriggerD3PDMaker.TrigDecisionFillerTool,
                                                   SaveLVL1Raw = True, SaveHLTRaw=True, SaveBGCode = True)
        else:
            TrigDecisionD3PDObjectRaw.defineBlock( 0, "TriggerRawInfo",
                                                   TriggerD3PDMaker.TrigDecisionFillerTool,
                                                   SaveHLTRaw=True, SaveBGCode = True)            
        #Finally add object to algorithms
        alg += TrigDecisionD3PDObjectRaw(**_args('trig_', kw, level=0))
        if not VertexD3PDAnalysisFlags.useTrigger():
            #unless we've already added it, add meta-data as well
            addTrigConfMetadata(alg)

    ## Beam background information (custom options prefix: 'bkg_')
    if VertexD3PDAnalysisFlags.useBackgroundWord():
        from BackgroundD3PDMaker.BackgroundWordD3PDObject import BackgroundWordD3PDObject
        alg += BackgroundWordD3PDObject (**_args('bkg_', kw, level=0))

    ## Tracks and V0 information (custom options prefix: 'trk_', and 'v0_')
    if VertexD3PDAnalysisFlags.useTracks():
        labelDefaultVtx=VertexD3PDAnalysisKeys.LabelDefaultVtx()
        prefixDefaultVtx=labelDefaultVtx+'_' #Add trailing '_'    

        import TrackD3PDMaker
        from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags
        from TrackD3PDMaker.TrackD3PDObject import TrackD3PDObject
        from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
        from TruthD3PDMaker.TruthD3PDMakerFlags import TruthD3PDFlags
        from TruthD3PDMaker.TruthD3PDMakerKeys import TruthD3PDKeys
        VtxD3PD_Track_Exclude=['vx_weight']
        TrackD3PDMaker.PerigeeUnbiasedIPAtPVFillerTool.DoBiased = False #Switch off biased perigee
        TrackParticleD3PDObjectForVertex = TrackD3PDObject(_label='trk',
                                                           _prefix='trk_',
                                                           _sgkey=D3PDMakerFlags.TrackSGKey(),
                                                           _object_name='TrackParticleD3PDObjectForVertex',
                                                           typeName='Rec::TrackParticleContainer',
                                                           vertexTarget=labelDefaultVtx,
                                                           vertexPrefix=prefixDefaultVtx,
                                                           vertexSGKey='VxPrimaryCandidate',
                                                           truthTarget=TruthD3PDFlags.GenParticleAssocLabel(),
                                                           truthPrefix=TruthD3PDKeys.GenParticlePrefix(),
                                                           detailedTruthPrefix='', # 'mc_detailed_'
                                                           SGKeyForTruth=D3PDMakerFlags.TrackSGKey(),
                                                           truthMapKey='TrackParticleTruthCollection',
                                                           detailedTruthMapKey='', #'DetailedTrackTruth'
                                                           flags=TrackD3PDFlags)       
        alg += TrackParticleD3PDObjectForVertex(**_args('trk_', kw, level=8, exclude=VtxD3PD_Track_Exclude))

    if VertexD3PDAnalysisFlags.useSecondaryVertex():
        from TrackD3PDMaker.V0D3PDObject import V0D3PDObject
        alg += V0D3PDObject(**_args('v0_', kw, level=10))

    ## Store beamspot information (custom option perfix: 'bs_')
    if VertexD3PDAnalysisFlags.useBeamspot():
        from TrackD3PDMaker.BeamSpotD3PDObject import BeamSpotD3PDObject
        alg += BeamSpotD3PDObject(**_args('bs_', kw, level=0))

    ## Store vertex information (custom options prefix: 'vtx_' or 'vtx' for BuildVertexD3PDObject)
    from TrackD3PDMaker.VertexD3PDObject import PrimaryVertexD3PDObject
    from TrackD3PDMaker.VertexD3PDObject import BuildVertexD3PDObject
    from InDetRecExample.InDetKeys import InDetKeys
    from InDetRecExample.InDetJobProperties import InDetFlags                
    #First dump existing content
    labelDefaultVtx=VertexD3PDAnalysisKeys.LabelDefaultVtx()
    prefixDefaultVtx=labelDefaultVtx+'_' #Add trailing '_'    
    VtxDefaultD3PDObj = BuildVertexD3PDObject(**_args('vtx', kw,
                                                      _prefix=prefixDefaultVtx,
                                                      _label=labelDefaultVtx,
                                                      _sgkey=InDetKeys.PrimaryVertices(),
                                                      trackTarget='trk',
                                                      trackPrefix='trk_',
                                                      trackType='Rec::TrackParticleContainer'))
    alg += VtxDefaultD3PDObj(**_args('vtx_', kw, level=10))
    #Then also dump the other vertex collections
    if VertexD3PDAnalysisFlags.useAllVertexCollections:
        #Use non BC and split collections (sorry, no customization of parameters here)
        if not InDetFlags.doSplitVertexFindingForMonitoring():
            #raise NameError('Requesting split vertices but algorith has not run!')
            #pass
            print('[TrackD3PDMaker.VertexGroupD3PD] WARNING: Requested split vertices, but algorithm has not run! Skipped.')
        else:            
            SplitPrimaryVertexD3PDObject = BuildVertexD3PDObject(_prefix='vxsplit_',
                                                                 _label='vxsplit',
                                                                 _sgkey=InDetKeys.PrimaryVerticesSplitStream(),
                                                                 trackTarget='trk',
                                                                 trackPrefix='trk_',
                                                                 trackType='Rec::TrackParticleContainer')
            alg += SplitPrimaryVertexD3PDObject(8)

        #Then store also non beamspot constrained ones
        if not InDetFlags.doVertexFindingForMonitoring():
            #raise NameError('Requesting NBC vertices but algorith has not run!')
            #pass
            print('[TrackD3PDMaker.VertexGroupD3PD] WARNING: Requested non beamspot constrained vertices, but algorithm has not run! Skipped.')
        else:
            NoBeamConstraintPrimaryVertexD3PDObject = BuildVertexD3PDObject(_prefix='vxnbc_',
                                                               _label='vxnbc',
                                                               _sgkey=InDetKeys.PrimaryVerticesWithoutBeamConstraint(),
                                                               trackTarget='trk',
                                                               trackPrefix='trk_',
                                                               trackType='Rec::TrackParticleContainer')
            alg += NoBeamConstraintPrimaryVertexD3PDObject(8)
            
    ## Store truth-level information (custom options prefix: 'tuthVtx_', 'truthEvt_', 'truthPart_')
    if VertexD3PDAnalysisFlags.useTruth():
        from TruthD3PDAnalysis.TruthD3PDAnalysisConf import D3PD__GenObjectsFilterTool
        trackGenObjectsFilterTool= D3PD__GenObjectsFilterTool( "trackGenObjectsFilterTool" )
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += trackGenObjectsFilterTool

        if (VertexD3PDAnalysisFlags.filterTightTruth()):
            trackGenObjectsFilterTool.SelectTruthTracks=True
            trackGenObjectsFilterTool.PtMin = VertexD3PDAnalysisKeys.TruthTrackCutPtMin()
            trackGenObjectsFilterTool.EtaMax = VertexD3PDAnalysisKeys.TruthTrackCutEtaMax()
            trackGenObjectsFilterTool.RemoveEmptyEvents = True
            trackGenObjectsFilterTool.RemoveDummyEvents = True
            trackGenObjectsFilterTool.RemoveInTimePileUp = False
            trackGenObjectsFilterTool.Remove2BCPileUp = True
            trackGenObjectsFilterTool.Remove800nsPileUp = True
            trackGenObjectsFilterTool.RemoveCavernBkg = False
            trackGenObjectsFilterTool.AddOnlyFirstVertex = VertexD3PDAnalysisKeys.TruthAddOnlyFirstVertex()            
        else:
            #Use a more inclusive truth policy
            trackGenObjectsFilterTool.SelectTruthTracks=False	  
            trackGenObjectsFilterTool.PtMin = VertexD3PDAnalysisKeys.TruthTrackCutPtMin()
            trackGenObjectsFilterTool.EtaMax = VertexD3PDAnalysisKeys.TruthTrackCutEtaMax()
            trackGenObjectsFilterTool.RemoveEmptyEvents = True
            trackGenObjectsFilterTool.RemoveDummyEvents = True
            trackGenObjectsFilterTool.AddOnlyFirstVertex = VertexD3PDAnalysisKeys.TruthAddOnlyFirstVertex()
	  
        from TruthD3PDMaker.GenEventD3PDObject import GenEventD3PDObject
        alg += GenEventD3PDObject(**_args('truthEvt_', kw,
                                          level=10, filter = trackGenObjectsFilterTool,
                                          pileup_CollectionGetterRegistry=alg.name()+'_CollectionGetterRegistry'))
        
        from TruthD3PDMaker.GenVertexD3PDObject import GenVertexD3PDObject
        alg += GenVertexD3PDObject(**_args('truthVtx_', kw,
                                           level=1,
                                           filter = trackGenObjectsFilterTool))
        
        from TruthD3PDMaker.GenParticleD3PDObject import GenParticleD3PDObject
        alg += GenParticleD3PDObject(**_args('truthPart_', kw,
                                             level=10,
                                             filter = trackGenObjectsFilterTool))

        from TruthD3PDMaker.GenParticleD3PDObject import GenTruthTrackD3PDObject
        alg += GenTruthTrackD3PDObject(**_args('truthTrack_', kw, level=0))
        
    ## Dump higher level objects	  
    # Missing Energy (custom options prefix with VertexD3PDAnalysisKeys.MetCollections+'_')
    if VertexD3PDAnalysisFlags.useMET():
        for MET in VertexD3PDAnalysisKeys.MetCollections():
            VtxD3PD_Met_Key=MET
            VtxD3PD_Met_Prefix=MET.replace('_','')+'_'
            VtxD3PD_Met_Level=0 #Only basic info
            VtxD3PD_Met_Exclude=['MET_Regions', 'L1_', 'L2_', 'EF_']
            from MissingETD3PDMaker.MissingETD3PDObject import RefFinalMETD3PDObject
            VtxD3PD_Met_Obj = RefFinalMETD3PDObject(**_args(MET+'_', kw,
                                                           level=VtxD3PD_Met_Level,
                                                           sgkey=VtxD3PD_Met_Key,
                                                           prefix=VtxD3PD_Met_Prefix,
                                                           exclude=VtxD3PD_Met_Exclude))
            alg += VtxD3PD_Met_Obj

    #Muons (custom options prefix VtxD3PD_Muon_Prefix -- see below [e.g. muid_, staco_, ...])
    if VertexD3PDAnalysisFlags.useMuons():
        from MuonD3PDMaker.MuonD3PDObject import MuonD3PDObject
        for Muon in VertexD3PDAnalysisKeys.MuonCollections():
            VtxD3PD_Muon_Key=Muon
            VtxD3PD_Muon_Prefix=Muon+'_'
            if VtxD3PD_Muon_Key == 'MuidMuonCollection':
                VtxD3PD_Muon_Prefix='muid_'
            elif VtxD3PD_Muon_Key == 'StacoMuonCollection':
                VtxD3PD_Muon_Prefix = 'staco_'
            
            VtxD3PD_Muon_Level=0 #Only basic info
            VtxD3PD_Muon_Exclude=['EFCBInfo', 'EFMGInfo', 'EFMEInfo', 'L2CBInfo', 'L1Info', 'L1_', 'L2_', 'EF_']
            VtxD3PD_Muon_Obj = MuonD3PDObject(**_args(VtxD3PD_Muon_Prefix, kw,
                                                     level=VtxD3PD_Muon_Level,
                                                     sgkey=VtxD3PD_Muon_Key,
                                                     prefix=VtxD3PD_Muon_Prefix,
                                                     allowMissing = True,
                                                     exclude=VtxD3PD_Muon_Exclude))
            alg += VtxD3PD_Muon_Obj
            
    #Electrons (custom options prefix: 'el_')
    if VertexD3PDAnalysisFlags.useElectrons():
        from egammaD3PDMaker.ElectronD3PDObject import ElectronD3PDObject
        alg += ElectronD3PDObject(**_args('el_', kw, prefix='el_', level=0))
        
    #Photons (custom options prefix: 'ph_')
    if VertexD3PDAnalysisFlags.usePhotons():
        from egammaD3PDMaker.PhotonD3PDObject import PhotonD3PDObject
        alg += PhotonD3PDObject(**_args('ph_', kw, prefix='ph_', level=0))
        
    #Jets, JVF, b-jets (custom options prefix: 'jet_')
    if VertexD3PDAnalysisFlags.useJets():
        from JetD3PDMaker.JetD3PDObject import JetD3PDObject
        from JetTagD3PDMaker.JetTagD3PDMakerKeys import JetTagD3PDKeys
        from JetTagD3PDMaker.AddBTagD3PDInfo import addBTagInfoToJetObject
        addBTagInfoToJetObject(JetD3PDObject, btagLevelOffset=0)        
        VtxD3PD_Jet_Include=['Kinematics', 'JetVertexFraction', 'JVtx', 'EMFraction','TrueFlavorComponents',
                             JetTagD3PDKeys.BTagWeightsBlockName()]
        for JET in VertexD3PDAnalysisKeys.JetCollections():
            VtxD3PD_Jet_Prefix=JET.replace('_','')+'_'
            if len(VertexD3PDAnalysisKeys.JetCollections()) == 1:
                #only 1 element, simplify prefix
                VtxD3PD_Jet_Prefix='jet_'
            alg += JetD3PDObject(**_args('jet_', kw,                                         
                                         sgkey=JET,
                                         prefix=VtxD3PD_Jet_Prefix,
                                         level=0, include=VtxD3PD_Jet_Include))
    
    #Taus
    if VertexD3PDAnalysisFlags.useTaus():
        from TauD3PDMaker.TauD3PDObject import TauD3PDObject
        VtxD3PD_Tau_Include=['TauPriVtx']
        alg += TauD3PDObject(**_args('tau_', kw, prefix='tau_', level=0, include=VtxD3PD_Tau_Include))

    ## Return algorithm
    return alg
