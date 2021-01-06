# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file JetTagD3PDMaker/python/BtaggingD3PD.py
# @author Georges Aad
# @date Mars, 2011
# @brief Construct a btagging d3pd.
#

from AthenaCommon.AppMgr import ServiceMgr
from RecExConfig.RecFlags import rec

from JetTagD3PDMaker.JetTagD3PDMakerKeys import JetTagD3PDKeys
from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags

def _modifyMuonObject(MuonD3PDObject):
    if "BtagMuonTrackMatchingBlock" not in MuonD3PDObject.allBlocknames():
        if JetTagD3PDFlags.TrackAssocLabel() != "":
            from D3PDMakerCoreComps.IndexAssociation import IndexAssociation
            import MuonD3PDMaker
            _ = IndexAssociation(MuonD3PDObject,
                                 MuonD3PDMaker.MuonTrackParticleAssociationTool,
                                 JetTagD3PDFlags.TrackAssocLabel(),
                                 level = 500,
                                 prefix=JetTagD3PDKeys.MuonInDetTrackAssocPrefix(),
                                 blockname="BtagMuonTrackMatchingBlock",
                                 Type =  'InDet')



def _args (name, kwin, **kw):
    kw = kw.copy()
    for (k, v) in kwin.items():
        if k.startswith (name + '_'):
            kw[k[len(name)+1:]] = v
    return kw

def BTaggingD3PD(alg = None,
                 file = JetTagD3PDKeys.D3PDFileName(),
                 algname = JetTagD3PDKeys.D3PDAlgName(),
                 **kw):


    print ("printing additional d3pd properties from dictionary")
    print (kw)


    if not alg:
        from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
        alg = MSMgr.NewRootStream(JetTagD3PDKeys.D3PDAlgName(),  
                                  JetTagD3PDKeys.D3PDFileName(),
                                  JetTagD3PDKeys.D3PDTupleName())


    selectedJetCollections = []

    if len(JetTagD3PDFlags.JetCollections()) != 0:
        
        from JetTagD3PDMaker.JetTagJetD3PDObject import getJetTagJetD3PDObject
        for jetcoll in JetTagD3PDFlags.JetCollections():
            filteredcoll = jetcoll+JetTagD3PDKeys.FilteredJetCollectionPostfix()
            ### you can add specific selection for a certain jet collection as argument to the function bellow
            ### otherwise the one specified by JetTagD3PDFlags will be used
            alg += getJetTagJetD3PDObject(**_args(jetcoll, kw, filteredsgkey=filteredcoll, origsgkey=jetcoll))
            selectedJetCollections.append(filteredcoll)
    else:
        print ("Warning: you didn't specify any jet collection for the JetTag D3PD")
    
    if len(selectedJetCollections) != 0:

    ### add muon in jets object
        if JetTagD3PDFlags.AddMuonInJetsObject():
            from JetTagD3PDMaker.JetTagMuonInJetD3PDObject import getJetTagMuonInJetObject
            alg += getJetTagMuonInJetObject(**_args("muon1injet", kw, level=10, jetCollections=selectedJetCollections))

            if JetTagD3PDFlags.AddSecondMuonCollection():
                alg += getJetTagMuonInJetObject(**_args("muon2injet", kw, level=10, 
                                                        jetCollections=selectedJetCollections,
                                                        prefix=JetTagD3PDKeys.Muon2InJetPrefix(),
                                                        object_name='JetTagMuon2InJetD3PDObject',
                                                        label=JetTagD3PDKeys.Muon2InJetGetterLabel(),
                                                        muonsName="SecondMuons")
                                                  )


    ### add electron in jets object
        if JetTagD3PDFlags.AddElectronInJetsObject():
            from JetTagD3PDMaker.JetTagElectronInJetD3PDObject import getJetTagElectronInJetD3PDObject
            alg +=getJetTagElectronInJetD3PDObject(**_args("electroninjet", kw, 
                                                           level=10, 
                                                           jetCollections=selectedJetCollections)
                                                     )


    ### add photon in jets object
        if JetTagD3PDFlags.AddPhotonInJetsObject():
            from JetTagD3PDMaker.JetTagPhotonInJetD3PDObject import getJetTagPhotonInJetD3PDObject
            alg += getJetTagPhotonInJetD3PDObject(**_args("photoninjet", kw, 
                                                          level=10, 
                                                          jetCollections=selectedJetCollections)

                                                    )

    ### add VxOnJetAxis object
        if JetTagD3PDFlags.AddVxOnJetAxisInJetsObject():
            from JetTagD3PDMaker.JetTagVxOnJetAxisD3PDObject import getJetTagVxOnJetAxisD3PDObject
            alg +=getJetTagVxOnJetAxisD3PDObject(**_args("vxonjetaxis", kw,
                                                         level=0, jetCollections=selectedJetCollections)
                                                   )

    ### add TwoTrackVertex object
        if JetTagD3PDFlags.AddTwoTrackVertexInJetsObject():
            from JetTagD3PDMaker.JetTagTwoTrackVertexD3PDObject import getJetTagTwoTrackVertexD3PDObject
            alg += getJetTagTwoTrackVertexD3PDObject(**_args("twotrackvertex", kw,
                                                              level=0, 
                                                              jetCollections=selectedJetCollections)
                                                        )

    ### add MSVVtxInfo object
        if JetTagD3PDFlags.AddMSVVtxInfoInJetsObject():
            from JetTagD3PDMaker.JetTagMSVVtxInJetD3PDObject import getJetTagMSVVtxInfoInJetD3PDObject
            alg += getJetTagMSVVtxInfoInJetD3PDObject(**_args("msvvtxinfo", kw,
                                                              level=1, 
                                                              jetCollections=selectedJetCollections)
                                                        )




### add event info object
    if JetTagD3PDFlags.AddEventInfoObject():
        from JetTagD3PDMaker.JetTagEventInfoD3PDObject import getJetTagEventInfoD3PDObject
        alg +=getJetTagEventInfoD3PDObject(**_args("eventinfo", kw,
                                                   level=1)
                                          )

### add pileup event info object
    if rec.doTruth  and JetTagD3PDFlags.AddPileUpEventInfoObject():
        from TruthD3PDMaker.PileUpInfoD3PDObject import PileUpInfoD3PDObject
        alg +=PileUpInfoD3PDObject(0,"pileupinfo","pileupinfo_",allowMissing=True)

### add track object
    if JetTagD3PDFlags.AddTrackObject():
        from JetTagD3PDMaker.JetTagTrackD3PDObject import getJetTagTrackD3PDObject
        alg += getJetTagTrackD3PDObject(**_args("track", kw))

### add MET objects
    for MET in JetTagD3PDFlags.METObjects():
        from JetTagD3PDMaker.JetTagMETD3PDObject import getJetTagMETD3PDObject 
        alg += getJetTagMETD3PDObject(**_args(MET, kw,sgkey=MET))


### add event shape objects
    from QcdD3PDMaker.QcdEventShapeD3PDObject import EventShapeD3PDObject
    alg += EventShapeD3PDObject(level=0)


### add pixel clusters object
    if JetTagD3PDFlags.AddPixelClusterObject():
        from JetTagD3PDMaker.JetTagClustersD3PDObject import getJetTagClusterD3PDObject
        alg += getJetTagClusterD3PDObject(**_args("pixelcluster", kw,level=0))

### add primary vertex object
    if JetTagD3PDFlags.AddPrimaryVertexObject():
        from JetTagD3PDMaker.JetTagPrimaryVertexD3PDObject import getJetTagPrimaryVertexD3PDObject
        alg += getJetTagPrimaryVertexD3PDObject(**_args("pv", kw))

### add beam spot object
    if JetTagD3PDFlags.AddBeamSpotObject():
        from InDetD3PDMaker.BeamSpotD3PDObject import BeamSpotD3PDObject
        alg += BeamSpotD3PDObject(10)

### add muid object
    if JetTagD3PDFlags.AddMuonMuidObject():
        from MuonD3PDMaker.MuonD3PDObject import MuonD3PDObject
        _modifyMuonObject(MuonD3PDObject)
        alg += MuonD3PDObject(**_args ('MuonMuid', kw,
                                       level=10,
                                       sgkey='MuidMuonCollection',
                                       prefix='mu_muid_',
                                       allowMissing = True,
                                       exclude=["EFCBInfo", "EFMGInfo", "EFMEInfo", "L2CBInfo", "L1Info"],
                                       include=["BtagMuonTrackMatchingBlock", "EFCBInfoIndex", 
                                                "EFMGInfoIndex", "EFMEInfoIndex", "L2CBInfoIndex", "L1InfoIndex"]))

    if JetTagD3PDFlags.AddMuonStacoObject():
        from MuonD3PDMaker.MuonD3PDObject import MuonD3PDObject
        _modifyMuonObject(MuonD3PDObject)
        alg += MuonD3PDObject(**_args ('MuonStaco', kw,
                                       level=10,
                                       sgkey='StacoMuonCollection',
                                       prefix='mu_staco_',
                                       allowMissing = True,
                                       exclude=["EFCBInfo", "EFMGInfo", "EFMEInfo", "L2CBInfo", "L1Info"],
                                       include=["BtagMuonTrackMatchingBlock", "EFCBInfoIndex", 
                                                "EFMGInfoIndex", "EFMEInfoIndex", "L2CBInfoIndex", "L1InfoIndex"]))


### add truth info with a filter

    if rec.doTruth and JetTagD3PDFlags.AddGenObjects():

        from AthenaCommon.AppMgr import ToolSvc
        from TruthD3PDAnalysis.TruthD3PDAnalysisConf import D3PD__GenObjectsFilterTool
        jetTagGenObjectsFilterTool= D3PD__GenObjectsFilterTool( "JetTagGenObjectsFilterTool" )
        ToolSvc += jetTagGenObjectsFilterTool

        jetTagGenObjectsFilterTool.SelectTruthTracks=False
        jetTagGenObjectsFilterTool.PtMin = JetTagD3PDFlags.MCPtMinCut()
        jetTagGenObjectsFilterTool.EtaMax = JetTagD3PDFlags.MCEtaCut()
        jetTagGenObjectsFilterTool.RemoveEmptyEvents = not JetTagD3PDFlags.MCEmptyEvents()
        jetTagGenObjectsFilterTool.RemoveDummyEvents = not JetTagD3PDFlags.MCDummyEvents()

        if not JetTagD3PDFlags.MCPileUp():
            jetTagGenObjectsFilterTool.RemoveInTimePileUp = True
            jetTagGenObjectsFilterTool.Remove2BCPileUp = True
            jetTagGenObjectsFilterTool.Remove800nsPileUp = True
            jetTagGenObjectsFilterTool.RemoveCavernBkg =  True
            jetTagGenObjectsFilterTool.RemoveEmptyEvents = True
            jetTagGenObjectsFilterTool.RemoveDummyEvents = True

        if JetTagD3PDFlags.AddGenEventObjects():
            from TruthD3PDMaker.GenEventD3PDObject import GenEventD3PDObject
            alg += GenEventD3PDObject(**_args("genevent", kw, level=10, 
                                              filter = jetTagGenObjectsFilterTool,
                                              pileup_CollectionGetterRegistry=\
                                                  alg.name()+'_CollectionGetterRegistry'
                                              )
                                        )

        if JetTagD3PDFlags.AddGenVertexObjects():
            from TruthD3PDMaker.GenVertexD3PDObject import GenVertexD3PDObject
            alg += GenVertexD3PDObject( **_args("genvertex", kw, level=10, 
                                                filter = jetTagGenObjectsFilterTool )
                                          )

        if JetTagD3PDFlags.AddGenParticleObjects():
            from TruthD3PDMaker.GenParticleD3PDObject import GenParticleD3PDObject
            alg += GenParticleD3PDObject( **_args("genparticle", kw, level=10, 
                                                  filter = jetTagGenObjectsFilterTool )
                                            )

        if JetTagD3PDFlags.AddTruthTrackObjects():
            jetTagTruthTrackObjectsFilterTool= D3PD__GenObjectsFilterTool( "JetTagTruthTrackObjectsFilterTool" )
            ToolSvc += jetTagTruthTrackObjectsFilterTool

            jetTagTruthTrackObjectsFilterTool.SelectTruthTracks=True

            jetTagTruthTrackObjectsFilterTool.PtMin = JetTagD3PDFlags.MCPtMinCut()
            jetTagTruthTrackObjectsFilterTool.EtaMax = JetTagD3PDFlags.MCEtaCut()
            jetTagTruthTrackObjectsFilterTool.RemoveEmptyEvents = not JetTagD3PDFlags.MCEmptyEvents()
            jetTagTruthTrackObjectsFilterTool.RemoveDummyEvents = not JetTagD3PDFlags.MCDummyEvents()

            if not JetTagD3PDFlags.MCPileUp():
                jetTagTruthTrackObjectsFilterTool.RemoveInTimePileUp = True
                jetTagTruthTrackObjectsFilterTool.Remove2BCPileUp = True
                jetTagTruthTrackObjectsFilterTool.Remove800nsPileUp = True
                jetTagTruthTrackObjectsFilterTool.RemoveCavernBkg =  True
                jetTagTruthTrackObjectsFilterTool.RemoveEmptyEvents = True
                jetTagTruthTrackObjectsFilterTool.RemoveDummyEvents = True


            from TruthD3PDMaker.GenParticleD3PDObject import GenTruthTrackD3PDObject
            alg += GenTruthTrackD3PDObject( **_args("truthtrack", kw, level=10, 
                                                    filter = jetTagTruthTrackObjectsFilterTool )
                                              )



### add trigger
    if JetTagD3PDFlags.AddTriggerObject():
        from TriggerD3PDMaker.TrigDecisionD3PDObject import TrigDecisionD3PDObject
        alg += TrigDecisionD3PDObject(**_args("trigdecision", kw, level = 2))
        from TriggerD3PDMaker.TrigConfMetadata import addTrigConfMetadata
        addTrigConfMetadata( alg )

        if JetTagD3PDFlags.AddTriggerBits():
            from JetTagD3PDMaker.JetTagTriggerBitsD3PDObject import getJetTagTriggerBitsD3PDObject
            alg += getJetTagTriggerBitsD3PDObject(JetTagD3PDFlags.TriggerBitsList())


        if JetTagD3PDFlags.AddJetTriggerObject(): 
            from TrigJetD3PDMaker.TrigJetD3PD import TrigJetD3PDObjects
            TrigJetD3PDObjects( alg, level = 10, addNaviInfo = True, addConfigInfo = False )

        if JetTagD3PDFlags.AddBJetTriggerObject():            

            from TrigBJetD3PDMaker.TrigBJetD3PD import  TrigBJetD3PDObjects
            bjettriglevel=1
            if JetTagD3PDFlags.AddTrigL2IDTracks() or JetTagD3PDFlags.AddTrigEFIDTracks() : bjettriglevel=11
            TrigBJetD3PDObjects( alg, level = bjettriglevel, 
                                 doAddL2ID = JetTagD3PDFlags.AddTrigL2IDTracks(), 
                                 doAddEFID = JetTagD3PDFlags.AddTrigEFIDTracks() )




    if JetTagD3PDFlags.AddPixCond():
        if hasattr( ServiceMgr, "PixelConditionsSummarySvc" ):
            from JetTagD3PDMaker.JetTagPixModCondD3PDObject import getJetTagPixModCondD3PDObject
            alg += getJetTagPixModCondD3PDObject(PixSummarySvc=ServiceMgr.PixelConditionsSummarySvc)
        else:
            print ("Warning: JetTagPixModCondD3PDObject requested but PixelConditionsSummarySvc not configured")


    #### add fat jets and subjets
    if JetTagD3PDFlags.AddFatJets():
        from JetTagD3PDMaker.JetTagJSD3PDObjects import JSD3PD_Tool
        for xx in JetTagD3PDKeys.FatJetsList():	   
            if xx[0] is not None and xx[1] is not None:
                jsD3PD = JSD3PD_Tool(xx)
                jsD3PD.addToAlg(alg)

### add skiming
    if JetTagD3PDFlags.SkimD3PD():

        __muonAssocObjects=["Muons"]
        if JetTagD3PDFlags.AddSecondMuonCollection():
            __muonAssocObjects=["Muons","Muons2"]

        from JetTagD3PDMaker.JetTagD3PDMakerConf import D3PD__JetTagD3PDEventFilter
        JetTagD3PDEventFilter = D3PD__JetTagD3PDEventFilter("JetTagD3PDEventFilter",
                                                            UseMuonFilter=JetTagD3PDFlags.UseMuonEventFilter(),
                                                            UseJetFilter=JetTagD3PDFlags.UseJetEventFilter(),
                                                            JetCollections=selectedJetCollections,
                                                            JetPtCut=JetTagD3PDFlags.FilterJetPtCut(),
                                                            JetEtaCut=JetTagD3PDFlags.FilterJetEtaCut(),
                                                            JetWithMuonPtCut=JetTagD3PDFlags.FilterJetWithMuonPtCut(),
                                                            SoftMuonPtCut=JetTagD3PDFlags.FilterMuonInJetPtCut(),
                                                            MinNumberOfJets=JetTagD3PDFlags.FilterMinNJets(),
                                                            MinNumberOfJetsWithMuon=JetTagD3PDFlags.FilterMinNJetsMuon(),
                                                            MuonAssocObjects=__muonAssocObjects,
                                                            MuonCollections=JetTagD3PDFlags.FilterMuonCollections(),
                                                            MuonPtCut=JetTagD3PDFlags.FilterMuonPtCut(),
                                                            MuonEtaCut=JetTagD3PDFlags.FilterMuonEtaCut(),
                                                            MuonIsTightCut=JetTagD3PDFlags.FilterMuonTightCut(),
                                                            MinNumberOfMuons=JetTagD3PDFlags.FilterMinNumberOfMuons()
                                                            )

        alg.filterSeq+= JetTagD3PDEventFilter


        from EventCommonD3PDMaker.LBMetadataConfig  import LBMetadataConfig
        alg.MetadataTools += [LBMetadataConfig()]



    return alg
