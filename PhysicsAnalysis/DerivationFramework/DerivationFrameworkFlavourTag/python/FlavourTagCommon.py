# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#********************************************************************

# FlavourTagCommon.py

# Schedules all tools needed for FlavourTag analyses and writes the

# results into SG. These may then be accessed along the train

#********************************************************************

import AthenaCommon.Constants as Lvl

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from BTagging.BTaggingFlags import BTaggingFlags
from AthenaCommon.GlobalFlags import globalflags

DoneJetCollections=set([])

######################################################################
DontReduceInfoRun = False

def DontReduceInfo(Rel20=True):
    
    global DontReduceInfoRun
    if (DontReduceInfoRun):
        return
    DontReduceInfoRun = True

    if globalflags.DataSource()!='geant4':
        return
    

    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParametersForTruthParticles
    
    TTPName = "TruthParticle"
    
    if Rel20:
        TTPName = "TruthParticles"

    TruthDecor = DerivationFramework__TrackParametersForTruthParticles( name = "TruthTPDecor",
                                                                       OutputLevel = Lvl.INFO,
                                                                       DecorationPrefix ="",
                                                                       TruthParticleContainerName=TTPName)
    
    
    global ToolSvc
    if globalflags.DataSource()!='data':
        ToolSvc +=TruthDecor
        augmentationTools = [TruthDecor]
    if globalflags.DataSource()=='data':
        augmentationTools = []

    from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
    
    global DerivationFrameworkJob
    DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("MyDFTSOS_KERN",
                                                                             AugmentationTools = augmentationTools,
                                                                             OutputLevel = Lvl.INFO )
######################################################################
#ReTag JetCollections
SAJetBTaggerAlgs={}
BTaggingFlags.CalibrationChannelAliases += [ "AntiKt4TopoEM->AntiKt4EMTopo" ]
BTaggingFlags.Jets=[]
from BTagging.BTaggingConfiguration import getConfiguration
ConfInst=getConfiguration()


def ReTag(Taggers, JetCollections = ['AntiKt4EMTopoJets' ], Sequencer=None, DoFullRetag=True):
    JetCollectionList = []
    AuthorSubString = []

    btag = "BTagging_"
    suffix_name = ""
    tmpSVname = "SecVtx"
    tmpJFVxname = "JFVtx"
    SA = 'standalone_'

    from AthenaCommon.AppMgr import ToolSvc
    from ParticleJetTools.ParticleJetToolsConf import JetParticleShrinkingConeAssociation, JetModifierAlg

    trackassoc = \
        JetParticleShrinkingConeAssociation(
            "BTaggingTrackAssoc",
            InputParticleCollectionName="InDetTracks",
            OutputCollectionName="MatchedTracks",
            ConeSizeFitPar1=+0.239,
            ConeSizeFitPar2=-1.220,
            ConeSizeFitPar3=-1.64e-5
        )


    muonassoc = \
        JetParticleShrinkingConeAssociation(
            "BTaggingTrackAssoc",
            InputParticleCollectionName="Muons",
            OutputCollectionName="MatchedMuons",
            ConeSizeFitPar1=0.4,
            ConeSizeFitPar2=0.0,
            ConeSizeFitPar3=99999999,
        )

    ToolSvc += trackassoc
    ToolSvc += muonassoc

    trackassocalg = \
        JetModifierAlg(
            "BTaggingTrackAssocAlg",
            JetContainerNames=JetCollections,
            Modifiers=[trackassoc, muonassoc]
        )

    Sequencer += trackassocalg

    for JetCollection in JetCollections:
        name = JetCollection.replace('ZTrack', 'Track').replace('PV0Track', 'Track')
        author= btag+name[:-4]+suffix_name
        algname = SA + author.lower()

        if algname in SAJetBTaggerAlgs:
            print " Tagger ", algname, " already exists. Add to ", Sequencer
            Sequencer += SAJetBTaggerAlgs[algname]
        else:
            JetCollectionList.append((JetCollection,name))
            BTaggingFlags.Jets.append(name[:-4])
            AuthorSubString.append(author)

    global ConfInst
    ######################## WARNING ########################################
    #      from BTagging.BTaggingConfiguration import doNotCheckForTaggerObstacles
    ConfInst.doNotCheckForTaggerObstacles()
    ####BTaggingFlags.PrimaryVertexCollectionName = "PrimaryVertices"
    #########################################################################
    #from BTagging.BTaggingConfiguration import setupJetBTaggerTool, getJetCollectionTool
    from BTagging.BTaggingConf import Analysis__StandAloneJetBTaggerAlg as StandAloneJetBTaggerAlg
    #BTaggingFlags.OutputLevel=VERBOSE
    NotInJetToolManager = [] # For jet collections
    if DoFullRetag:
       retagMode = ""
    else:
       retagMode="Retag"
    for i, jet in enumerate(JetCollectionList):
        try:
            btagger = ConfInst.setupJetBTaggerTool(ToolSvc, JetCollection=jet[1][:-4], AddToToolSvc=True,
                                                   Verbose=True,
                                                   #Verbose=BTaggingFlags.OutputLevel < 3,
                                                   options={"name"         : AuthorSubString[i].lower(),
                                                   "BTagName"     : AuthorSubString[i],
                                                   "BTagJFVtxName": tmpJFVxname,
                                                   "BTagSVName"   : tmpSVname,
                                                   },
#                          TaggerList = ['SV1Flip','JetFitterNNFlip','IP2DNeg','IP3DNeg','MV2c100Flip']
                                               SetupScheme = retagMode,
                                               TaggerList = Taggers
                                                   )
            SAbtagger = StandAloneJetBTaggerAlg(name=SA + AuthorSubString[i].lower(),
                                                    JetBTaggerTool=btagger,
                                                    JetCollectionName = jet[0],
                                                    outputCollectionSuffix = suffix_name,
                                                    #OutputLevel = DEBUG
                                                    )
            if Sequencer is None:
                global DerivationFrameworkJob
                Sequencer=DerivationFrameworkJob
            Sequencer += SAbtagger
            SAJetBTaggerAlgs[SA + AuthorSubString[i].lower()] = SAbtagger
            print " Create ", SAbtagger, " in ", Sequencer
            print SAbtagger
            #global DerivationFrameworkJob
            #DerivationFrameworkJob += SAbtagger
        except AttributeError as error:
            print '#BTAG# --> ' + str(error)
            print '#BTAG# --> ' + jet[1]
            print '#BTAG# --> ' + AuthorSubString[i]
            NotInJetToolManager.append(AuthorSubString[i])

    if len(NotInJetToolManager) > 0:
        AuthorSubString = list(set(AuthorSubString) - set(NotInJetToolManager))
        
    # Both standard and aux container must be listed explicitly.
    # For release 19, the container version must be explicit.
    #BaseName = "xAOD::BTaggingContainer_v1#"
    #BaseAuxName = "xAOD::BTaggingAuxContainer_v1#"
	   
    #AOD list
    #BTaggingFlags.btaggingAODList += [ BaseName + author for author in AuthorSubString]
    #BTaggingFlags.btaggingAODList += [ BaseAuxName + author + 'Aux.' for author in AuthorSubString]
    #BTaggingFlags.btaggingAODList += [ BaseName + author + 'AOD' for author in AuthorSubString]
    #BTaggingFlags.btaggingAODList += [ BaseAuxName + author + 'AODAux.' for author in AuthorSubString]
   
    #ESD list
    #BTaggingFlags.btaggingESDList += [ BaseName + author for author in AuthorSubString]
    #BTaggingFlags.btaggingESDList += [ BaseAuxName + author + 'Aux.' for author in AuthorSubString]
   
    #AOD list SeCVert
    #BaseNameSecVtx = "xAOD::VertexContainer_v1#"
    #BaseAuxNameSecVtx = "xAOD::VertexAuxContainer_v1#"
    #BTaggingFlags.btaggingAODList += [ BaseNameSecVtx + author + tmpSVname for author in AuthorSubString]
    #BTaggingFlags.btaggingAODList += [ BaseAuxNameSecVtx + author + tmpSVname + 'Aux.-vxTrackAtVertex' for author in AuthorSubString]
   
    #ESD list
    #BTaggingFlags.btaggingESDList += [ BaseNameSecVtx + author + tmpSVname for author in AuthorSubString]
    #BTaggingFlags.btaggingESDList += [ BaseAuxNameSecVtx + author + tmpSVname + 'Aux.-vxTrackAtVertex' for author in AuthorSubString]
	   
    #AOD list JFSeCVert
    #BaseNameJFSecVtx = "xAOD::BTagVertexContainer_v1#"
    #BaseAuxNameJFSecVtx = "xAOD::BTagVertexAuxContainer_v1#"
    #BTaggingFlags.btaggingAODList += [ BaseNameJFSecVtx + author + tmpJFVxname for author in AuthorSubString]
    #BTaggingFlags.btaggingAODList += [ BaseAuxNameJFSecVtx + author + tmpJFVxname + 'Aux.' for author in AuthorSubString]
    #ESD list
    #BTaggingFlags.btaggingESDList += [ BaseNameJFSecVtx + author + tmpJFVxname for author in AuthorSubString]
    #BTaggingFlags.btaggingESDList += [ BaseAuxNameJFSecVtx + author + tmpJFVxname + 'Aux.' for author in AuthorSubString]





######################################################################

def FlavorTagInit(DoReduceInfo   =False,
                  DoMSV        =False,
                  Rel20        =True,
                  DoRetag      =True,
                  isFTAG1      = False, 
                  myTaggers  = [],  
                  JetCollections = ['AntiKt4EMTopoJets' ],     #['AntiKt4PV0TrackJets', 'AntiKt4LCTopoJets' ]
                  DoFullRetag=True, 
                  Sequencer=None):   

    # ====================================================================
    # MAIN SWITCHESr
    # ====================================================================
    #ReduceInfo   =False ##write minimal amount of info on the output file
    #DoMSV        =False ##include variables for MSV tagger
    #Rel20        =True ##switch between rel19 and rel20
    #(only option that will work on original DC14 xAOD)
    #doRetag      =True  ## perform retagging
    #adjust configurations
    
    if DoRetag==False:
        DoReduceInfo=True


    #List of taggers used in RUN2
    Taggers = ['IP2D', 'IP3D', 'MultiSVbb1',  'MultiSVbb2', 'SV1', 'JetFitterNN', 'SoftMu', 'MV2c10', 'MV2c10mu', 'MV2c10rnn', 'JetVertexCharge', 'MV2c100', 'MV2cl100' , 'DL1', 'DL1rnn', 'DL1mu', 'DL1Flip', 'RNNIP','MV2c10Flip']
    #Taggers = ['IP2D', 'IP3D', 'MultiSVbb1',  'MultiSVbb2', 'SV1', 'JetFitterNN', 'MV2c10', 'MV2c100', 'JetVertexCharge', 'MV2cl100', 'MVb']
    #if the user has defined a list of desired taggers use that one, otherwise use only the active taggers
    if len( myTaggers ) >0:
        Taggers = myTaggers
 

    if isFTAG1==True:
        FlipTaggers = ['SV1Flip','JetFitterNNFlip','IP2DNeg','IP3DNeg','MV2c10Flip','MV2c100Flip','MV2cl100Flip']
        for tag in FlipTaggers:
            if tag not in Taggers:
                Taggers.append( tag )
    #####################END SWITCHES

 
    ##### VD: THIS IS ALSO NOT NEEDED?????
    ##write minimal amount of info on the output file
    #if DoReduceInfo==False:
    #    DontReduceInfo(Rel20)
    #ReTag JetCollections
    if DoRetag:
        ReTag(Taggers, JetCollections, Sequencer, DoFullRetag)


######################################################################

def applyBTagging(jetalg,algname,sequence):
    btagWPlist = [ 'FixedCutBEff_30', 'FixedCutBEff_50', 'FixedCutBEff_60',
                   'FixedCutBEff_70', 'FixedCutBEff_77', 'FixedCutBEff_80',
                   'FixedCutBEff_85', 'FixedCutBEff_90',
                   'FlatBEff_30', 'FlatBEff_50', 'FlatBEff_60',
                   'FlatBEff_70', 'FlatBEff_77', 'FlatBEff_85' ]

    btagtooldict = {}
    from AthenaCommon.AppMgr import ToolSvc
    for btagWP in btagWPlist:
        btagtoolname = 'DFBtagSel'+btagWP+'_'+jetalg
        print 'FlavourTagCommon: Add B-tag WP '+btagWP+' for '+jetalg
        btagtool = None
        if hasattr(ToolSvc,btagtoolname):
            btagtool = getattr(ToolSvc,btagtoolname)
        else:
            btagtool = CfgMgr.BTaggingSelectionTool(btagtoolname)
            ToolSvc += btagtool
            btagtool.TaggerName = "MV2c10"
            # In the absence of properly defined FlatBEff WP we alias them on the flat cut ones
            btagtool.OperatingPoint = btagWP
            btagtool.JetAuthor = jetalg+"Jets"
            btagtool.FlvTagCutDefinitionsFileName = "xAODBTaggingEfficiency/13TeV/2016-20_7-13TeV-MC15-CDI-May31_v1.root"
        btagtooldict[btagWP] = btagtool

    from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
    applyBTaggingAugmentation(jetalg,algname,sequence,btagtooldict)

def applyBTagging_xAODColl(jetalg='AntiKt4EMTopo',sequence=DerivationFrameworkJob):
    supportedJets = ['AntiKt4EMTopo']
    if not jetalg in supportedJets:
        print 'FlavourTagCommon: *** WARNING: B-tagging requested for unsupported jet collection! ***'
        return
    else:
        applyBTagging(jetalg,'JetCommonKernel_xAODJets',sequence)
