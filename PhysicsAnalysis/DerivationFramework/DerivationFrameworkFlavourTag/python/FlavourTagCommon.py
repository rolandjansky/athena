# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

#********************************************************************

# FlavourTagCommon.py

# Schedules all tools needed for FlavourTag analyses and writes the

# results into SG. These may then be accessed along the train

#********************************************************************

import AthenaCommon.Constants as Lvl
from AthenaCommon import Logging
ftaglog = Logging.logging.getLogger('FlavourTagCommon')


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

    from ParticleJetTools.ParticleJetToolsConf import JetAssocConstAlg
    from BTagging.BTaggingConfiguration import defaultTrackAssoc, defaultMuonAssoc
    assocalg = \
        JetAssocConstAlg(
            "BTaggingRetagAssocAlg",
            JetCollections=JetCollections,
            Associators=[defaultTrackAssoc, defaultMuonAssoc]
        )

    Sequencer += assocalg

    for JetCollection in JetCollections:
        name = JetCollection.replace('ZTrack', 'Track').replace('PV0Track', 'Track')
        author= btag+name[:-4]+suffix_name
        algname = SA + author.lower()


        if algname in SAJetBTaggerAlgs:
            ftaglog.info("Tagger {} already exists. Add to {}".format(algname,Sequencer))
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
                                                    DuplicatePFlow = BTaggingFlags.Do2019Retraining
                                                    )
            if Sequencer is None:
                global DerivationFrameworkJob
                Sequencer=DerivationFrameworkJob
            Sequencer += SAbtagger
            SAJetBTaggerAlgs[SA + AuthorSubString[i].lower()] = SAbtagger
            ftaglog.info("Create {} in {}".format(SAbtagger,Sequencer))
            # print (SAbtagger)
            #global DerivationFrameworkJob
            #DerivationFrameworkJob += SAbtagger
        except AttributeError as error:
            print ('#BTAG# --> ' + str(error))
            print ('#BTAG# --> ' + jet[1])
            print ('#BTAG# --> ' + AuthorSubString[i])
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

def FlavorTagInit(DoReduceInfo = False,
                  DoMSV = False,
                  Rel20 = True,
                  DoRetag = True,
                  scheduleFlipped = False,
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


    #if the user has defined a list of desired taggers use that one, otherwise use only the active taggers
    if len( myTaggers ) >0:
      Taggers = myTaggers
    else:
      from BTagging.BTaggingFlags import BTaggingFlags
      if scheduleFlipped:
        Taggers = BTaggingFlags.ExpertTaggers
      else:
        Taggers = BTaggingFlags.StandardTaggers


    ##### VD: THIS IS ALSO NOT NEEDED?????
    ##write minimal amount of info on the output file
    #if DoReduceInfo==False:
    #    DontReduceInfo(Rel20)
    #ReTag JetCollections
    if DoRetag:
        ReTag(Taggers, JetCollections, Sequencer, DoFullRetag)


######################################################################

def applyBTagging(jetalg,algname,sequence):
    btagWPlist = [ 'FixedCutBEff_60', 'FixedCutBEff_70', 'FixedCutBEff_77', 'FixedCutBEff_85' ]
    btagAlglist = [ 'MV2c10', 'MV2r', 'MV2rmu', 'DL1', 'DL1r', 'DL1rmu']

    btagtooldict = {}
    from AthenaCommon.AppMgr import ToolSvc
    for btagWP in btagWPlist:
        for btagAlg in btagAlglist:
            btagtoolname = 'DFBtagSel'+btagWP+'_'+btagAlg+'_'+jetalg
            ftaglog.info("Add B-tag WP {} of the {} algorithm for {}".format(btagWP,btagAlg,jetalg))
            btagtool = None
            if hasattr(ToolSvc,btagtoolname):
                btagtool = getattr(ToolSvc,btagtoolname)
            else:
                btagtool = CfgMgr.BTaggingSelectionTool(btagtoolname)
                ToolSvc += btagtool
                btagtool.TaggerName = btagAlg
                # In the absence of properly defined FlatBEff WP we alias them on the flat cut ones
                btagtool.OperatingPoint = btagWP
                btagtool.JetAuthor = jetalg+"Jets"
                btagtool.MinPt = 0
                if '_BTagging' in jetalg:
                    btagtool.JetAuthor = jetalg.replace('_BTagging','Jets_BTagging')
                btagtool.ErrorOnTagWeightFailure = False #avoid an error when the jets tagweight cannot be retrieved, and only print a warning
                btagtool.FlvTagCutDefinitionsFileName = "xAODBTaggingEfficiency/13TeV/2019-21-13TeV-MC16-CDI-2019-10-07_v1.root"
            btagKey = btagWP+'_'+btagAlg
            btagtooldict[btagKey] = btagtool

    from DerivationFrameworkJetEtMiss.ExtendedJetCommon import applyBTaggingAugmentation
    applyBTaggingAugmentation(jetalg,algname,sequence,btagtooldict)

def applyBTagging_xAODColl(jetalg='AntiKt4EMTopo',sequence=DerivationFrameworkJob):
    if BTaggingFlags.Do2019Retraining:
      supportedJets = ['AntiKt4EMTopo_BTagging201810', 'AntiKt4EMPFlow_BTagging201810']
    else:
      supportedJets = ['AntiKt4EMTopo', 'AntiKt4EMPFlow']
    if not jetalg in supportedJets:
        ftaglog.warning('B-tagging requested for unsupported jet collection {}!'.format(jetalg))
        return
    else:
        applyBTagging(jetalg,'JetCommonKernel_{0}'.format(jetalg),sequence)
