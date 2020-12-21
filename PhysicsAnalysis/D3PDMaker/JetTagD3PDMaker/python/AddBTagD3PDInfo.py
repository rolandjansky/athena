# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
## @file JetTagD3PDMaker/python/AddBTagInfoToJetObject.py
## @brief Python function to add BTagging specific fillers and association tools to a JetD3PDObject
## @author Georges Aad
## @date Nov, 2010
##



"""
This file is used to add btagging specific fillers and association tools to a JetD3PDObject.
The btagLevelOffset argument is used to specify an offset for the block fillers levels.
You can also include/exclude blocks using two different ways:
1. Using the exclude/include functionality of the D3PD framework: D3PDObject(...,exclude/include=[list of blocks],...)
   Use JetTagD3PDKeys to access block names.
2. Using JetTagD3PDFlags
The default behaviour is to include all fillers with different levels.
The associators are included if the corresponding label (to the getter) is specified (default is None).
In some cases (tracks, muons, electrons) if the label to the getter is not 
specified the four momentum is filled using a contained association tool.

Typical usage:
from JetD3PDMaker.JetD3PDObject import JetD3PDObject
from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags
JetTagD3PDFlags.<prop> = <value>
from JetTagD3PDMaker.AddBTagD3PDInfo import addBTagInfoToJetObject
addBTagInfoToJetObject(JetD3PDObject, btagLevelOffset=X)
alg+=JetD3PDObject(level, <additional options>)
"""
import JetTagD3PDMaker
import EventCommonD3PDMaker
import MuonD3PDMaker
import TruthD3PDMaker

from D3PDMakerCoreComps.IndexAssociation import IndexAssociation
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation
from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation
from D3PDMakerCoreComps.release_version import at_least_version
from D3PDMakerConfig.D3PDMakerFlags  import D3PDMakerFlags
from RecExConfig.RecFlags import rec
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator


from JetTagD3PDMaker.JetTagD3PDMakerKeys import JetTagD3PDKeys
from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags

def _jetTagAssocLevel (reqlev, args):
    if not args.has_key ('target') : 
        return False
    return True



def addBTagInfoToJetObject(JetD3PDObject, btagLevelOffset=0, LocalFlags=JetTagD3PDFlags):

    ## first check if the blocks are already added to the JetD3PDObject
    ## the weight block is always added so check for it
    from AthenaCommon.Logging import logging
    addBTagInfoLogger = logging.getLogger( "addBTagInfoToJetObject" )

    if JetTagD3PDKeys.BTagWeightsBlockName() in JetD3PDObject.allBlocknames():
        addBTagInfoLogger.warning("btag blocks already added to JetD3PDObject - ignore")
        return

 
    addBTagInfoLogger.info("Adding btag blocks to JetD3PDObject")

    #
    ## now add the block fillers
    ## basic info weights and truth if available
    ## specify a list of taggers via LocalFlags.Taggers()
    #



    JetD3PDObject.defineBlock(btagLevelOffset+0,JetTagD3PDKeys.BTagWeightsBlockName(),
                              JetTagD3PDMaker.JetTagBTagWeightsFillerTool,
                              prefix=JetTagD3PDKeys.BTagWeightsPrefix(),
                              TaggerNames=LocalFlags.Taggers())


    if rec.doTruth:

        JetD3PDObject.defineBlock(btagLevelOffset+0, JetTagD3PDKeys.TruthInfoBlockName(),
                                  JetTagD3PDMaker.JetTagTruthInfoFillerTool,
                                  prefix=JetTagD3PDKeys.TruthInfoPrefix(),
                                  InfoType="TruthInfo")

        if LocalFlags.QGInfo():
            JetD3PDObject.defineBlock(btagLevelOffset+0, JetTagD3PDKeys.QGPartonTruthInfoBlockName(),
                                      JetTagD3PDMaker.JetTagQGPartonTruthInfoFillerTool,
                                      prefix=JetTagD3PDKeys.QGPartonTruthInfoPrefix(),
                                      InfoType="QGPartonTruthInfo")

        if LocalFlags.PileupLabeling():
            JetD3PDObject.defineBlock(btagLevelOffset+0, JetTagD3PDKeys.PUTruthInfoBlockName(),
                                      JetTagD3PDMaker.JetTagTruthInfoFillerTool,
                                      prefix=JetTagD3PDKeys.PUTruthInfoPrefix(),
                                      InfoType="TruthInfoPU")

            if LocalFlags.QGInfo():
                JetD3PDObject.defineBlock(btagLevelOffset+0, JetTagD3PDKeys.PUQGPartonTruthInfoBlockName(),
                                          JetTagD3PDMaker.JetTagQGPartonTruthInfoFillerTool,
                                          prefix=JetTagD3PDKeys.PUQGPartonTruthInfoPrefix(),
                                          InfoType="QGPartonTruthInfoPU")
    #
    ##association to tracks, electron, muons, and photons
    ##if the getter labels are not specified no association is done
    ##can add contained information if needed
    #

    if LocalFlags.JetTrackAssoc():

        JetTrackAssoc = IndexMultiAssociation(JetD3PDObject,
                                              JetTagD3PDMaker.JetTagJetTrackAssociationTool,
                                              '', ## set target when calling the JetD3PDObject
                                              level = _jetTagAssocLevel,
                                              prefix=JetTagD3PDKeys.JetTrackAssocPrefix(),
                                              blockname=JetTagD3PDKeys.JetTrackAssocBlockName(),
                                              FillVariables=False)

        JetTagD3PDTrackToVertexIPEstimator=None
        from AthenaCommon.AppMgr import ToolSvc
        if hasattr(ToolSvc, 'BTagTrackToVertexIPEstimator'):
            JetTagD3PDTrackToVertexIPEstimator=ToolSvc.BTagTrackToVertexIPEstimator
            print ("JetTagD3PD Info: found BTagTrackToVertexIPEstimator")
        elif hasattr(ToolSvc, 'JetTagD3PDTrackToVertexIPEstimator'):
            print ("JetTagD3PD Info: found JetTagD3PDTrackToVertexIPEstimator")
            JetTagD3PDTrackToVertexIPEstimator=ToolSvc.JetTagD3PDTrackToVertexIPEstimator
        else:
            print ("JetTagD3PD Info: configure TrackToVertexIPEstimator")
            from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__FullLinearizedTrackFactory
            extrap = AtlasExtrapolator()
            ToolSvc += extrap
            JetTagD3PDLinTrkFactory = Trk__FullLinearizedTrackFactory(name= "JetTagD3PDFullLinearizedTrackFactory",
                                                                Extrapolator = extrap)
            ToolSvc += JetTagD3PDLinTrkFactory
            from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__TrackToVertexIPEstimator
            JetTagD3PDTrackToVertexIPEstimator = Trk__TrackToVertexIPEstimator(\
                name="JetTagD3PDTrackToVertexIPEstimator",
                Extrapolator=extrap,
                LinearizedTrackFactory=JetTagD3PDLinTrkFactory)
            ToolSvc += JetTagD3PDTrackToVertexIPEstimator

        

        JetTrack = ContainedVectorMultiAssociation(JetD3PDObject,  # noqa: F841
                                                   JetTagD3PDMaker.JetTagJetTrackAssociationTool,
                                                   level = btagLevelOffset+4,
                                                   prefix=JetTagD3PDKeys.JetTrackAssocPrefix(),
                                                   blockname=JetTagD3PDKeys.JetTrackBlockName(),
                                                   nrowName = '',
                                                   TrackToVertexIPEstimator=JetTagD3PDTrackToVertexIPEstimator,
                                                   PrimaryVertexCollection=JetTagD3PDFlags.PrimaryVertexSGKey())


    if LocalFlags.JetTrackGhostAssoc():
        JetTrackAssoc = IndexMultiAssociation(JetD3PDObject,  # noqa: F841
                                              JetTagD3PDMaker.JetTagJetTrackAssociationTool,
                                              '', ## set target when calling the JetD3PDObject
                                              level = _jetTagAssocLevel,
                                              prefix=JetTagD3PDKeys.JetTrackGhostAssocPrefix(),
                                              blockname=JetTagD3PDKeys.JetTrackGhostAssocBlockName(),
                                              FillVariables=False,
                                              TrackAssocName=LocalFlags.JetTrackGhostAssocName())


    if LocalFlags.JetBHadronGhostAssoc():
        JetBHadronAssoc = ContainedVectorMultiAssociation(\
            JetD3PDObject,
            JetTagD3PDMaker.JetTagJetNavigable4MomentumAssociationTool,
            JetTagD3PDKeys.JetBHadronGhostAssocPrefix(),
            level = 900,
            blockname=JetTagD3PDKeys.JetBHadronGhostAssocBlockName()+"tmp",
            Navigable4MomentumAssocName=LocalFlags.JetBHadronGhostAssocName())

        IndexAssociation(JetBHadronAssoc,
                         TruthD3PDMaker.TruthParticleGenParticleAssociationTool,
                         '',
                         level = _jetTagAssocLevel,
                         blockname= JetTagD3PDKeys.JetBHadronGhostAssocBlockName(),
                         prefix="")



    if LocalFlags.JetTruthGhostAssoc():
        JetTruthAssoc = ContainedVectorMultiAssociation(JetD3PDObject,
                                                        JetTagD3PDMaker.JetTagJetNavigable4MomentumAssociationTool,
                                                        JetTagD3PDKeys.JetTruthGhostAssocPrefix(),
                                                        level = 900,
                                                        blockname=JetTagD3PDKeys.JetTruthGhostAssocBlockName()+"tmp",
                                                        Navigable4MomentumAssocName=LocalFlags.JetTruthGhostAssocName())

    

        IndexAssociation(JetTruthAssoc,
                         TruthD3PDMaker.TruthParticleGenParticleAssociationTool,
                         '',
                         level = _jetTagAssocLevel,
                         blockname= JetTagD3PDKeys.JetTruthGhostAssocBlockName(),
                         prefix="")


    if LocalFlags.JetConstituantTruthAssoc():
        ConstitAssoc = ContainedVectorMultiAssociation \
            (JetD3PDObject,
             EventCommonD3PDMaker.NavigableConstituentAssociationTool,
             'constit_',
             nrowName = '',
             TypeName = 'TruthParticle',
             blockname= JetTagD3PDKeys.JetConstituantTruthAssocBlockName()+"tmp",
             level = 900 )
     
        IndexAssociation(ConstitAssoc,
                         TruthD3PDMaker.TruthParticleGenParticleAssociationTool,
                         '',
                         level = _jetTagAssocLevel,
                         blockname= JetTagD3PDKeys.JetConstituantTruthAssocBlockName(),
                         prefix="mcpart_")


    if LocalFlags.JetMuonAssoc():

        JetMuonAssoc = IndexMultiAssociation(JetD3PDObject,  # noqa: F841
                                             JetTagD3PDMaker.JetTagJetMuonAssociationTool,
                                             '', ## set target when calling the JetD3PDObject
                                             level =_jetTagAssocLevel,
                                             prefix=JetTagD3PDKeys.JetMuonAssocPrefix(),
                                             blockname=JetTagD3PDKeys.JetMuonAssocBlockName())

        if LocalFlags.AddSecondMuonCollection():

            JetMuon2Assoc = IndexMultiAssociation(JetD3PDObject,  # noqa: F841
                                                  JetTagD3PDMaker.JetTagJetMuonAssociationTool,
                                                  '',
                                                  level = _jetTagAssocLevel,
                                                  prefix=JetTagD3PDKeys.JetMuon2AssocPrefix(),
                                                  blockname=JetTagD3PDKeys.JetMuon2AssocBlockName(),
                                                  MuonsName="SecondMuons")

    if LocalFlags.JetElectronAssoc():

        JetElectronAssoc = IndexMultiAssociation(JetD3PDObject,  # noqa: F841
                                                 JetTagD3PDMaker.JetTagJetElectronAssociationTool,
                                                 '',
                                                 level = _jetTagAssocLevel,
                                                 prefix=JetTagD3PDKeys.JetElectronAssocPrefix(),
                                                 blockname=JetTagD3PDKeys.JetElectronAssocBlockName())

    if LocalFlags.JetPhotonAssoc():

        JetPhotonAssoc = IndexMultiAssociation(JetD3PDObject,  # noqa: F841
                                               JetTagD3PDMaker.JetTagJetPhotonAssociationTool,
                                               '',
                                               level = _jetTagAssocLevel,
                                               prefix=JetTagD3PDKeys.JetPhotonAssocPrefix(),
                                               blockname=JetTagD3PDKeys.JetPhotonAssocBlockName())


 
    if rec.doTruth and LocalFlags.JetGenSoftLeptonAssoc():

        JetGenSoftLeptonAssoc = IndexMultiAssociation(JetD3PDObject,  # noqa: F841
            JetTagD3PDMaker.JetTagJetGenSoftLeptonAssociationTool,
            '',
            level = _jetTagAssocLevel,
            prefix=JetTagD3PDKeys.JetGenSoftLeptonAssocPrefix(),
            blockname=JetTagD3PDKeys.JetGenSoftLeptonAssocBlockName(),
            MCCollections = D3PDMakerFlags.TruthSGKey(),
            FillVariables=False)


    
        JetGenSoftLepton = ContainedVectorMultiAssociation(JetD3PDObject,  # noqa: F841
            JetTagD3PDMaker.JetTagJetGenSoftLeptonAssociationTool,
            level = btagLevelOffset+4,
            prefix=JetTagD3PDKeys.JetGenSoftLeptonAssocPrefix(),
            blockname=JetTagD3PDKeys.JetGenSoftLeptonBlockName(),
            nrowName = '',
            MCCollections = D3PDMakerFlags.TruthSGKey())


        JetGenSoftLepton.defineBlock (btagLevelOffset+20,
                                      JetTagD3PDKeys.JetGenSoftLeptonPropBlockName(),
                                      EventCommonD3PDMaker.GenParticleFillerTool)

    
    #
    ## info base for each tagger: basicaly B/U/C probabilities
    #

    if LocalFlags.InfoBase():

        JetD3PDObject.defineBlock(btagLevelOffset+1, JetTagD3PDKeys.IP2DInfoBaseBlockName(),
                                  JetTagD3PDMaker.JetTagInfoBaseFillerTool,
                                  prefix=JetTagD3PDKeys.IP2DInfoBasePrefix(),
                                  InfoType="IP2D",
                                  AddPC=True)

        JetD3PDObject.defineBlock(btagLevelOffset+1, JetTagD3PDKeys.IP2DIPInfoBaseBlockName(),
                                  JetTagD3PDMaker.JetTagIPInfoBaseFillerTool,
                                  prefix=JetTagD3PDKeys.IP2DInfoBasePrefix(),
                                  InfoType="IP2D")

        JetD3PDObject.defineBlock(btagLevelOffset+1, JetTagD3PDKeys.IP3DInfoBaseBlockName(),
                                  JetTagD3PDMaker.JetTagInfoBaseFillerTool,
                                  prefix=JetTagD3PDKeys.IP3DInfoBasePrefix(),
                                  InfoType="IP3D",
                                  AddPC=True)

        JetD3PDObject.defineBlock(btagLevelOffset+1, JetTagD3PDKeys.IP3DIPInfoBaseBlockName(),
                                  JetTagD3PDMaker.JetTagIPInfoBaseFillerTool,
                                  prefix=JetTagD3PDKeys.IP3DInfoBasePrefix(),
                                  InfoType="IP3D")

        JetD3PDObject.defineBlock(btagLevelOffset+1, JetTagD3PDKeys.JetProbInfoBaseBlockName(),
                                  JetTagD3PDMaker.JetTagJetProbInfoBaseFillerTool,
                                  prefix=JetTagD3PDKeys.JetProbInfoBasePrefix(),
                                  InfoType="JetProb",
                                  # This may be missing in 17.2 samples.
                                  AllowMissing = at_least_version('17.2.0'))

        JetD3PDObject.defineBlock(btagLevelOffset+1, JetTagD3PDKeys.SV1InfoBaseBlockName(),
                                  JetTagD3PDMaker.JetTagInfoBaseFillerTool,
                                  prefix=JetTagD3PDKeys.SV1InfoBasePrefix(),
                                  InfoType="SV1",
                                  AddPC=True)

        JetD3PDObject.defineBlock(btagLevelOffset+1, JetTagD3PDKeys.SV2InfoBaseBlockName(),
                                  JetTagD3PDMaker.JetTagInfoBaseFillerTool,
                                  prefix=JetTagD3PDKeys.SV2InfoBasePrefix(),
                                  InfoType="SV2")

        JetD3PDObject.defineBlock(btagLevelOffset+0, JetTagD3PDKeys.JetFitterInfoBaseBlockName(),
                                  JetTagD3PDMaker.JetTagInfoBaseFillerTool,
                                  prefix=JetTagD3PDKeys.JetFitterTagInfoPrefix(),
                                  InfoType="JetFitterTagNN,JetFitterTag",
                                  AddPC=True)


        JetD3PDObject.defineBlock(btagLevelOffset+0, JetTagD3PDKeys.JetFitterCombInfoBaseBlockName(),
                                  JetTagD3PDMaker.JetTagInfoBaseFillerTool,
                                  prefix=JetTagD3PDKeys.JetFitterCombInfoBasePrefix(),
                                  InfoType="JetFitterCOMBNN,JetFitterCOMB",
                                  AddPC=True)


        if LocalFlags.GbbNNInfo():
            JetD3PDObject.defineBlock(btagLevelOffset+1, JetTagD3PDKeys.GbbNNInfoBlockName(),
                                      JetTagD3PDMaker.JetTagGbbNNInfoFillerTool,
                                      prefix=JetTagD3PDKeys.GbbNNInfoPrefix(),
                                      InfoType="GbbNN")


        if LocalFlags.NewGbbNNInfo():
            JetD3PDObject.defineBlock(btagLevelOffset+1, JetTagD3PDKeys.NewGbbNNInfoBlockName(),
                                      JetTagD3PDMaker.JetTagHadFlavorTagInfoFillerTool,
                                      prefix=JetTagD3PDKeys.NewGbbNNInfoPrefix(),
                                      InfoType="NewGbbNN")


        if LocalFlags.QGInfo():
            JetD3PDObject.defineBlock(btagLevelOffset+1, JetTagD3PDKeys.QGInfoBlockName(),
                                      JetTagD3PDMaker.JetTagHadFlavorTagInfoFillerTool,
                                      prefix=JetTagD3PDKeys.QGInfoPrefix(),
                                      InfoType="QG")



        if LocalFlags.InfoBaseCalib():

            JetD3PDObject.defineBlock(btagLevelOffset+3, JetTagD3PDKeys.JetFitterFlipInfoBaseBlockName(),
                                  JetTagD3PDMaker.JetTagInfoBaseFillerTool,
                                  prefix=JetTagD3PDKeys.JetFitterTagFlipInfoPrefix(),
                                  InfoType="JetFitterTagNNFlip,JetFitterTagFlip",
                                  AddPC=True)

            JetD3PDObject.defineBlock(btagLevelOffset+3, JetTagD3PDKeys.JetFitterCombIP3DPosInfoBaseBlockName(),
                                  JetTagD3PDMaker.JetTagInfoBaseFillerTool,
                                  prefix=JetTagD3PDKeys.JetFitterCombIP3DPosInfoBasePrefix(),
                                  InfoType="JetFitterCOMBNNIP3DPos,JetFitterCOMBIP3DPos",
                                  AddPC=True)

            JetD3PDObject.defineBlock(btagLevelOffset+3, JetTagD3PDKeys.JetFitterCombIP3DNegInfoBaseBlockName(),
                                  JetTagD3PDMaker.JetTagInfoBaseFillerTool,
                                  prefix=JetTagD3PDKeys.JetFitterCombIP3DNegInfoBasePrefix(),
                                  InfoType="JetFitterCOMBNNIP3DNeg,JetFitterCOMBIP3DNeg",
                                  AddPC=True)
    


    # if LocalFlags.InfoBaseGaia():

    #     JetD3PDObject.defineBlock(
    #         btagLevelOffset+1, "JetTag_Gaia", 
    #         JetTagD3PDMaker.JetTagInfoBaseFillerTool,
    #         prefix="flavor_component_gaia_",
    #         InfoType="Gaia", 
    #         AddPC=True,
    #         AddPTau=False)

    #     JetD3PDObject.defineBlock(
    #         btagLevelOffset+1, "JetTag_GaiaNeg", 
    #         JetTagD3PDMaker.JetTagInfoBaseFillerTool,
    #         prefix="flavor_component_gaianeg_",
    #         InfoType="GaiaNeg", 
    #         AddPC=True,
    #         AddPTau=False)

    #     # TODO: this should get its own block
    #     JetD3PDObject.defineBlock(
    #         btagLevelOffset+1, "JetTag_IPFordG", # also needs a rename
    #         JetTagD3PDMaker.JetTagInfoBaseFillerTool,
    #         prefix="flavor_component_ip3dloose_",
    #         InfoType="IPFordG", # also needs to be changed in JetTagTools
    #         AddPC=True)


    ## some additional info for jet fitter
    #


    if LocalFlags.JetFitterTagInfo():

        JetD3PDObject.defineBlock(btagLevelOffset+2, JetTagD3PDKeys.JetFitterTagInfoBlockName(),
                                  JetTagD3PDMaker.JetTagJetFitterTagInfoFillerTool,
                                  prefix=JetTagD3PDKeys.JetFitterTagInfoPrefix(),
                                  InfoType="JetFitterTagNN,JetFitterTag")

        if LocalFlags.JetFitterTagFlipInfo():

            JetD3PDObject.defineBlock(btagLevelOffset+2, JetTagD3PDKeys.JetFitterTagFlipInfoBlockName(),
                                      JetTagD3PDMaker.JetTagJetFitterTagInfoFillerTool,
                                      prefix=JetTagD3PDKeys.JetFitterTagFlipInfoPrefix(),
                                      InfoType="JetFitterTagNNFlip,JetFitterTagFlip")


    if LocalFlags.JetFitterCharmTagInfo():

        JetD3PDObject.defineBlock(btagLevelOffset+2, JetTagD3PDKeys.JetFitterCharmTagInfoBlockName(),
                                  JetTagD3PDMaker.JetTagJetFitterGenericTagInfoFillerTool,
                                  prefix=JetTagD3PDKeys.JetFitterCharmTagInfoPrefix(),
                                  InfoType="JetFitterCharm")
       
        JetD3PDObject.defineBlock(btagLevelOffset+0, JetTagD3PDKeys.JetFitterCharmInfoBaseBlockName(),
                                  JetTagD3PDMaker.JetTagInfoBaseFillerTool,
                                  prefix=JetTagD3PDKeys.JetFitterCharmTagInfoPrefix(),
                                  InfoType="JetFitterCharm",
                                  AddPC=True)


        if LocalFlags.InfoBaseCalib():
            JetD3PDObject.defineBlock(btagLevelOffset+0, JetTagD3PDKeys.JetFitterCharmInfoBaseBlockName()+"NEG",
                                      JetTagD3PDMaker.JetTagInfoBaseFillerTool,
                                      prefix="flavor_component_jfitcneg_",
                                      InfoType="JetFitterCharmNeg",
                                      AddPC=True)
            

    #
    ## track information used for IP taggers
    #


    if LocalFlags.IPInfoPlus():

        IPInfoPlusTrackAssoc =  IndexMultiAssociation(JetD3PDObject,  # noqa: F841
                                                      JetTagD3PDMaker.JetTagIPInfoPlusTrackAssociationTool,
                                                      '',
                                                      level = _jetTagAssocLevel,
                                                      prefix=JetTagD3PDKeys.IPInfoPlusTrackAssocPrefix(),
                                                      blockname=JetTagD3PDKeys.IPInfoPlusTrackAssocBlockName(),
                                                      FillVariables=False,
                                                      AllowMissing = True)


        IPInfoPlusTrack =  ContainedVectorMultiAssociation(JetD3PDObject,
                                                           JetTagD3PDMaker.JetTagIPInfoPlusTrackAssociationTool,
                                                           level = btagLevelOffset+2,
                                                           prefix=JetTagD3PDKeys.IPInfoPlusTrackAssocPrefix(),
                                                           blockname=JetTagD3PDKeys.IPInfoPlusTrackBlockName(),
                                                           nrowName = '',
                                                           AllowMissing = True)



        IPInfoPlusTrack.defineBlock(btagLevelOffset+20, JetTagD3PDKeys.IPInfoPlusTrkP4BlockName(),
                                         EventCommonD3PDMaker.FourMomFillerTool,
                                         prefix="",
                                         WriteM=False)



    #
    ## vertex information used for SV1/SV2 taggers
    #
 
    if LocalFlags.SVInfoPlus():
        JetD3PDObject.defineBlock(btagLevelOffset+2, JetTagD3PDKeys.SVInfoPlusBlockName(),
                                  JetTagD3PDMaker.JetTagSVInfoPlusFillerTool,
                                  prefix=JetTagD3PDKeys.SVInfoPlusPrefix(),
                                  InfoType="SVInfoPlus",
                                  AllowMissing = True, 
                                  AddNormDist=True)


        SVInfoPlusTrackAssoc =  IndexMultiAssociation(JetD3PDObject,  # noqa: F841
                                                      JetTagD3PDMaker.JetTagSVInfoPlusTrackAssociationTool,
                                                      '',
                                                      level = _jetTagAssocLevel,
                                                      prefix=JetTagD3PDKeys.SVInfoPlusTrackAssocPrefix(),
                                                      blockname=JetTagD3PDKeys.SVInfoPlusTrackAssocBlockName(),
                                                      AllowMissing = True)

        SVInfoPlusTrack =  ContainedVectorMultiAssociation(JetD3PDObject,
                                                           JetTagD3PDMaker.JetTagSVInfoPlusTrackAssociationTool,
                                                           level = btagLevelOffset+20,
                                                           prefix=JetTagD3PDKeys.SVInfoPlusTrackAssocPrefix(),
                                                           blockname=JetTagD3PDKeys.SVInfoPlusTrackBlockName(),
                                                           nrowName = '',
                                                           AllowMissing = True)

    
        SVInfoPlusTrack.defineBlock(btagLevelOffset+20, JetTagD3PDKeys.SVInfoPlusTrkP4BlockName(),
                                    EventCommonD3PDMaker.FourMomFillerTool,
                                    prefix='',
                                    WriteM=False)
 

        

    #
    ## vertex information used for the SV0 tagger
    #
 
    if LocalFlags.SV0InfoPlus():

        JetD3PDObject.defineBlock(btagLevelOffset+2, JetTagD3PDKeys.SV0InfoPlusBlockName(),
                                  JetTagD3PDMaker.JetTagSVInfoPlusFillerTool,
                                  prefix=JetTagD3PDKeys.SV0InfoPlusPrefix(),
                                  InfoType="SV0InfoPlus",
                                  AllowMissing = True)

        SV0InfoPlusTrackAssoc =  IndexMultiAssociation(JetD3PDObject,  # noqa: F841
                                                       JetTagD3PDMaker.JetTagSVInfoPlusTrackAssociationTool,
                                                       '',
                                                       level = _jetTagAssocLevel,
                                                       prefix=JetTagD3PDKeys.SV0InfoPlusTrackAssocPrefix(),
                                                       blockname=JetTagD3PDKeys.SV0InfoPlusTrackAssocBlockName(),
                                                       InfoType="SV0InfoPlus",
                                                       AllowMissing = True)

        SV0InfoPlusTrack =  ContainedVectorMultiAssociation(JetD3PDObject,
                                                            JetTagD3PDMaker.JetTagSVInfoPlusTrackAssociationTool,
                                                            level = btagLevelOffset+20,
                                                            prefix=JetTagD3PDKeys.SV0InfoPlusTrackAssocPrefix(),
                                                            blockname=JetTagD3PDKeys.SV0InfoPlusTrackBlockName(),
                                                            nrowName = '',
                                                            InfoType="SV0InfoPlus",
                                                            AllowMissing = True)


        SV0InfoPlusTrack.defineBlock(btagLevelOffset+20, JetTagD3PDKeys.SV0InfoPlusTrkP4BlockName(),
                                     EventCommonD3PDMaker.FourMomFillerTool,
                                     prefix='',
                                     WriteM=False)
 
      
    #
    ## muon information used for the soft muon tagger
    #

    if LocalFlags.SoftMuonInfo():

        SoftMuonInfoMuonAssoc =  IndexMultiAssociation(JetD3PDObject,  # noqa: F841
                                                       JetTagD3PDMaker.JetTagSoftMuonInfoMuonAssociationTool,
                                                       '',
                                                       level = _jetTagAssocLevel,
                                                       prefix=JetTagD3PDKeys.SoftMuonInfoMuonAssocPrefix(),
                                                       blockname=JetTagD3PDKeys.SoftMuonInfoMuonAssocBlockName(),
                                                       FillVariables=False,
                                                       AllowMissing = True)

        SoftMuonInfoMuon =  ContainedVectorMultiAssociation(JetD3PDObject,
                                                            JetTagD3PDMaker.JetTagSoftMuonInfoMuonAssociationTool,
                                                            level = btagLevelOffset+2,
                                                            prefix=JetTagD3PDKeys.SoftMuonInfoMuonAssocPrefix(),
                                                            blockname=JetTagD3PDKeys.SoftMuonInfoMuonBlockName(),
                                                            nrowName = '',
                                                            AllowMissing = True)

        leveloffset=20
        if LocalFlags.AddInlineSoftMuonProperties():
            leveloffset=-btagLevelOffset


        SoftMuonInfoMuon.defineBlock(btagLevelOffset+leveloffset, JetTagD3PDKeys.SoftMuonInfoP4BlockName(),
                                     EventCommonD3PDMaker.FourMomFillerTool,
                                     prefix='',
                                     WriteM=False,
                                     WriteE=True)
        
        SoftMuonInfoMuon.defineBlock(btagLevelOffset+leveloffset, JetTagD3PDKeys.SoftMuonInfoChargeBlockName(),
                                     EventCommonD3PDMaker.ChargeFillerTool)
        SoftMuonInfoMuon.defineBlock(btagLevelOffset+leveloffset, JetTagD3PDKeys.SoftMuonInfoAuthorBlockName(),
                                     MuonD3PDMaker.MuonAuthorFillerTool)
        SoftMuonInfoMuon.defineBlock(btagLevelOffset+leveloffset, JetTagD3PDKeys.SoftMuonInfomatchChi2BlockName(),
                                     MuonD3PDMaker.MuonMatchQualityFillerTool)
        SoftMuonInfoMuon.defineBlock(btagLevelOffset+leveloffset, JetTagD3PDKeys.SoftMuonInfoAcceptBlockName(),
                                     MuonD3PDMaker.MuonAcceptMethodFillerTool)
        SoftMuonInfoMuon.defineBlock(btagLevelOffset+leveloffset, JetTagD3PDKeys.SoftMuonInfoELossBlockName(),
                                     MuonD3PDMaker.MuonELossFillerTool)
        SoftMuonInfoMuon.defineBlock(btagLevelOffset+leveloffset, JetTagD3PDKeys.SoftMuonInfoHitsBlockName(),
                                     MuonD3PDMaker.MuonTrkHitFillerTool,
                                     SaveIDMuonHitSummary=False)



            
        if LocalFlags.AddSecondMuonCollection():

            SoftMuon2InfoMuon2Assoc =  IndexMultiAssociation(JetD3PDObject,  # noqa: F841
                JetTagD3PDMaker.JetTagSoftMuonInfoMuonAssociationTool,
                '',
                level = _jetTagAssocLevel,
                prefix=JetTagD3PDKeys.SoftMuon2InfoMuon2AssocPrefix(),
                blockname=JetTagD3PDKeys.SoftMuon2InfoMuon2AssocBlockName(),
                InfoType="SecondSoftMuonTag",
                FillVariables=False,
                AllowMissing = True)

            SoftMuon2InfoMuon2 =  ContainedVectorMultiAssociation(\
                JetD3PDObject,
                JetTagD3PDMaker.JetTagSoftMuonInfoMuonAssociationTool,
                level = btagLevelOffset+3,
                prefix=JetTagD3PDKeys.SoftMuon2InfoMuon2AssocPrefix(),
                blockname=JetTagD3PDKeys.SoftMuon2InfoMuon2BlockName(),
                nrowName = '',
                InfoType="SecondSoftMuonTag",
                AllowMissing = True)


            SoftMuon2InfoMuon2.defineBlock(btagLevelOffset+20, JetTagD3PDKeys.SoftMuon2InfoP4BlockName(),
                                           EventCommonD3PDMaker.FourMomFillerTool,
                                           prefix='',
                                           WriteM=False)
       
    #
    ## muon information used for soft muon chi2 tagger
    #

    if LocalFlags.SoftMuonChi2Info():

        SoftMuonChi2InfoMuonAssoc =  IndexMultiAssociation(JetD3PDObject,  # noqa: F841
                                                       JetTagD3PDMaker.JetTagSoftMuonInfoMuonAssociationTool,
                                                       '',
                                                       level = _jetTagAssocLevel,
                                                       prefix=JetTagD3PDKeys.SoftMuonChi2InfoMuonAssocPrefix(),
                                                       blockname=JetTagD3PDKeys.SoftMuonChi2InfoMuonAssocBlockName(),
                                                       InfoType="SoftMuonTagChi2",
                                                       FillVariables=False,
                                                       AllowMissing = True)

        SoftMuonChi2InfoMuon =  ContainedVectorMultiAssociation(JetD3PDObject,
                                                            JetTagD3PDMaker.JetTagSoftMuonInfoMuonAssociationTool,
                                                            level = btagLevelOffset+2,
                                                            prefix=JetTagD3PDKeys.SoftMuonChi2InfoMuonAssocPrefix(),
                                                            blockname=JetTagD3PDKeys.SoftMuonChi2InfoMuonBlockName(),
                                                            InfoType="SoftMuonTagChi2",
                                                            nrowName = '',
                                                            AllowMissing = True)

        leveloffset=20
        if LocalFlags.AddInlineSoftMuonChi2Properties():
            leveloffset=-btagLevelOffset


        SoftMuonChi2InfoMuon.defineBlock(btagLevelOffset+leveloffset, JetTagD3PDKeys.SoftMuonChi2InfoP4BlockName(),
                                         EventCommonD3PDMaker.FourMomFillerTool,
                                         prefix='',
                                         WriteM=False,
                                         WriteE=True)
        
        SoftMuonChi2InfoMuon.defineBlock(btagLevelOffset+leveloffset, JetTagD3PDKeys.SoftMuonChi2InfoChargeBlockName(),
                                     EventCommonD3PDMaker.ChargeFillerTool)
        SoftMuonChi2InfoMuon.defineBlock(btagLevelOffset+leveloffset, JetTagD3PDKeys.SoftMuonChi2InfoAuthorBlockName(),
                                     MuonD3PDMaker.MuonAuthorFillerTool)
        SoftMuonChi2InfoMuon.defineBlock(btagLevelOffset+leveloffset, JetTagD3PDKeys.SoftMuonChi2InfomatchChi2BlockName(),
                                     MuonD3PDMaker.MuonMatchQualityFillerTool)
        SoftMuonChi2InfoMuon.defineBlock(btagLevelOffset+leveloffset, JetTagD3PDKeys.SoftMuonChi2InfoAcceptBlockName(),
                                     MuonD3PDMaker.MuonAcceptMethodFillerTool)
        SoftMuonChi2InfoMuon.defineBlock(btagLevelOffset+leveloffset, JetTagD3PDKeys.SoftMuonChi2InfoELossBlockName(),
                                     MuonD3PDMaker.MuonELossFillerTool)
        SoftMuonChi2InfoMuon.defineBlock(btagLevelOffset+leveloffset, JetTagD3PDKeys.SoftMuonChi2InfoHitsBlockName(),
                                     MuonD3PDMaker.MuonTrkHitFillerTool,
                                     SaveIDMuonHitSummary=False)



            
        if LocalFlags.AddSecondMuonCollection():

            SoftMuon2Chi2InfoMuon2Assoc =  IndexMultiAssociation(JetD3PDObject,  # noqa: F841
                JetTagD3PDMaker.JetTagSoftMuonInfoMuonAssociationTool,
                '',
                level = _jetTagAssocLevel,
                prefix=JetTagD3PDKeys.SoftMuon2Chi2InfoMuon2AssocPrefix(),
                blockname=JetTagD3PDKeys.SoftMuon2Chi2InfoMuon2AssocBlockName(),
                InfoType="SecondSoftMuonTagChi2",
                FillVariables=False,
                AllowMissing = True)

            SoftMuon2Chi2InfoMuon2 =  ContainedVectorMultiAssociation(\
                JetD3PDObject,
                JetTagD3PDMaker.JetTagSoftMuonInfoMuonAssociationTool,
                level = btagLevelOffset+3,
                prefix=JetTagD3PDKeys.SoftMuon2Chi2InfoMuon2AssocPrefix(),
                blockname=JetTagD3PDKeys.SoftMuon2Chi2InfoMuon2BlockName(),
                nrowName = '',
                InfoType="SecondSoftMuonTagChi2",
                AllowMissing = True)


            SoftMuon2Chi2InfoMuon2.defineBlock(btagLevelOffset+20, JetTagD3PDKeys.SoftMuon2Chi2InfoP4BlockName(),
                                           EventCommonD3PDMaker.FourMomFillerTool,
                                           prefix='',
                                           WriteM=False)
       
                       
                       
    #
    ## electron information used for the soft electron tagger
    #

    if LocalFlags.SoftElectronInfo():

        SoftElectronInfoElectronAssoc =  IndexMultiAssociation(JetD3PDObject,  # noqa: F841
            JetTagD3PDMaker.JetTagSoftElecInfoegammaAssociationTool,
            '',
            level = _jetTagAssocLevel,
            prefix=JetTagD3PDKeys.SoftElectronInfoElectronAssocPrefix(),
            blockname=JetTagD3PDKeys.SoftElectronInfoElectronAssocBlockName(),
            FillVariables=False,
            AllowMissing = True)


        SoftElectronInfoElectron =  ContainedVectorMultiAssociation(\
            JetD3PDObject,
            JetTagD3PDMaker.JetTagSoftElecInfoegammaAssociationTool,
            level = btagLevelOffset+2,
            prefix=JetTagD3PDKeys.SoftElectronInfoElectronAssocPrefix(),
            blockname=JetTagD3PDKeys.SoftElectronInfoElectronBlockName(),
            nrowName = '',
            AllowMissing = True)

        SoftElectronInfoElectron.defineBlock(\
            btagLevelOffset+20, JetTagD3PDKeys.SoftElectronInfoP4BlockName(),
            EventCommonD3PDMaker.FourMomFillerTool,
            prefix='',
            WriteM=False)

       

    if LocalFlags.MultiSVInfoPlus():

        JetD3PDObject.defineBlock(btagLevelOffset+3, JetTagD3PDKeys.MultiSVInfoPlusBlockName(),
                                  JetTagD3PDMaker.JetTagMultiSVInfoPlusFillerTool,
                                  prefix=JetTagD3PDKeys.MultiSVInfoPlusPrefix(),
                                  InfoType="MultiSVInfoPlus",
                                  AllowMissing = True)


        MultiSVInfoPlusTrackAssoc = IndexMultiAssociation(JetD3PDObject,  # noqa: F841
            JetTagD3PDMaker.JetTagMultiSVInfoMSVVtxInfoAssociationTool,
                '',
            level = _jetTagAssocLevel,
            prefix=JetTagD3PDKeys.MultiSVInfoPlusMSVVtxAssocPrefix(),
            blockname=JetTagD3PDKeys.MultiSVInfoPlusMSVVtxAssocBlockName(),
            AllowMissing = True)



 
#### from here keep the ifs for association labels. These are used only by btag.


    #
    ## additional information about the VKal secondary vertex finder used for SV tagger
    #


    if LocalFlags.JetVKalVxBadTrack():

        if LocalFlags.TrackAssocLabel() != "":

            JetVKalVxBadTrackAssoc = IndexMultiAssociation(\
                JetD3PDObject,
                JetTagD3PDMaker.JetTagVKalVxInJetBadTrackAssociationTool,
                LocalFlags.TrackAssocLabel(),
                level = btagLevelOffset+5,
                prefix=JetTagD3PDKeys.JetVKalVxBadTrackAssocPrefix(),
                blockname=JetTagD3PDKeys.JetVKalVxBadTrackAssocBlockName())


            if  LocalFlags.AddContainedTrackP4():
                JetVKalVxBadTrackAssoc.defineBlock(btagLevelOffset+10, JetTagD3PDKeys.SV0InfoPlusTrkP4BlockName(),
                                                  EventCommonD3PDMaker.FourMomFillerTool,
                                                  prefix='',
                                                  WriteM=False)

        else:
            JetVKalVxBadTrackAssoc = ContainedVectorMultiAssociation(\
                JetD3PDObject,
                JetTagD3PDMaker.JetTagVKalVxInJetBadTrackAssociationTool,
                level = btagLevelOffset+6,
                prefix=JetTagD3PDKeys.JetVKalVxBadTrackAssocPrefix(),
                blockname=JetTagD3PDKeys.JetVKalVxBadTrackAssocBlockName())

            JetVKalVxBadTrackAssoc.defineBlock(btagLevelOffset+6, JetTagD3PDKeys.SV0InfoPlusTrkP4BlockName(),
                                               EventCommonD3PDMaker.FourMomFillerTool,
                                               prefix='',
                                               WriteM=False)


    #
    ## additional information about the vertex finding for JetFitter*
    ## need to be associated with three additional D3PD objects defined in:
    ## JetTagVxOnJetAxisD3PDObject.py
    ## JetTagTwoTrackVertexD3PDObject.py
    #

    if LocalFlags.JetFitterVertexFinderInfo():

        JetD3PDObject.defineBlock(btagLevelOffset+5, JetTagD3PDKeys.JetFitterVxAssocBlockName(),
                                  JetTagD3PDMaker.JetTagJetFitterVxFillerTool,
                                  prefix=JetTagD3PDKeys.JetFitterVxAssocPrefix())
                                     

        if LocalFlags.JFVxOnJetAxisAssocLabel() != "":

            JetVxOnJetAxisAssoc = IndexMultiAssociation(JetD3PDObject,  # noqa: F841
                                                        JetTagD3PDMaker.JetTagJetVxOnJetAxisAssociationTool,
                                                        LocalFlags.JFVxOnJetAxisAssocLabel(),
                                                        level = btagLevelOffset+5,
                                                        prefix=JetTagD3PDKeys.JetJFVxOnJetAxisAssocPrefix(),
                                                        blockname=JetTagD3PDKeys.JetJFVxOnJetAxisAssocBlockName(),
                                                        VxOnJetHolderName=LocalFlags.JetJFVxOnJetAxisAssocSGKey(),
                                                        InfoName="NewJetFitterVxFinder")

            if LocalFlags.JetFitterFlipVxOnJetAxisAssoc():

                JFFlipJetVxOnJetAxisAssoc = IndexMultiAssociation(JetD3PDObject,  # noqa: F841
                    JetTagD3PDMaker.JetTagJetVxOnJetAxisAssociationTool,
                    LocalFlags.JFVxOnJetAxisAssocLabel(),
                    level = btagLevelOffset+6,
                    prefix=JetTagD3PDKeys.JetJFFlipVxOnJetAxisAssocPrefix(),
                    blockname=JetTagD3PDKeys.JetJFFlipVxOnJetAxisAssocBlockName(),
                    VxOnJetHolderName=LocalFlags.JetJFVxOnJetAxisAssocSGKey(),
                    InfoName="NewJetFitterVxFinderFlip")


        if LocalFlags.JFTwoTrackVertexAssocLabel() != "":

            JetJFTwoTrackVertexAssoc = IndexMultiAssociation(JetD3PDObject,  # noqa: F841
                JetTagD3PDMaker.JetTagJetJFTwoTrackVertexAssociationTool,
                LocalFlags.JFTwoTrackVertexAssocLabel(),
                level = btagLevelOffset+5,
                prefix=JetTagD3PDKeys.JetJFTwoTrackVertexAssocPrefix(),
                blockname=JetTagD3PDKeys.JetJFTwoTrackVertexAssocBlockName(),
                TwoTrackVertexName=LocalFlags.JetJFTwoTrackVertexAssocSGKey(),
                InfoName="NewJetFitterVxFinder")

            if LocalFlags.JetFitterFlipTwoTrackVertexAssoc():

                JetJFFlipTwoTrackVertexAssoc = IndexMultiAssociation(JetD3PDObject,  # noqa: F841
                JetTagD3PDMaker.JetTagJetJFTwoTrackVertexAssociationTool,
                LocalFlags.JFTwoTrackVertexAssocLabel(),
                level = btagLevelOffset+6,
                prefix=JetTagD3PDKeys.JetJFFlipTwoTrackVertexAssocPrefix(),
                blockname=JetTagD3PDKeys.JetJFFlipTwoTrackVertexAssocBlockName(),
                TwoTrackVertexName=LocalFlags.JetJFTwoTrackVertexAssocSGKey(),
                InfoName="NewJetFitterVxFinderFlip")
