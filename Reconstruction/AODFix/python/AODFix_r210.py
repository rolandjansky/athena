# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.Logging import logging
from AthenaCommon import CfgMgr
import PyJobTransforms.trfJobOptions

logAODFix_r210 = logging.getLogger( 'AODFix_r210' )

from AODFix_base import AODFix_base
import os

class AODFix_r210(AODFix_base):
    ''' This class just performs AODFix on 21.0.X releases

    Instance variables
      addMetadata (bool) (inherited): whether to add metaData
      doAODFix (bool) (inherited): whether to do AODFix
      prevAODFix (string): previous AODFix applied
      newAODFix (string): new AODFix to apply
      isMC (bool): is the input data MC?
    '''

    @staticmethod
    def latestAODFixVersion():
        """The latest version of the AODFix. Moving to new AODFix version scheme"""

        metadataList = [item.split("_")[0] for item in sorted(AODFix_r210.__dict__.keys())
                        if ("_" in item and "__" not in item)]

        return metadataList


    def postSystemRec(self):
        """
        This function calls the other postSystemRecs base on metadata"
        """

        if self.doAODFix:

            logAODFix_r210.debug("Executing AODFix_r210_postSystemRec")

            from AthenaCommon.AlgSequence import AlgSequence
            topSequence = AlgSequence()

            oldMetadataList = self.prevAODFix.split("-")
            
            if "evtRunNum" not in oldMetadataList;
                self.evtRunNum_postSystemRec(topSequence)
                pass

            if "trklinks" not in oldMetadataList:
                self.trklinks_postSystemRec(topSequence)
                pass

            if "egammaStrips" not in oldMetadataList and not self.isHI:
                self.egammaStrips_postSystemRec(topSequence)
                pass

            if "elIso" not in oldMetadataList:
                self.elIso_postSystemRec(topSequence)
                pass
            if "felIso" not in oldMetadataList and not self.isHI:
                self.felIso_postSystemRec(topSequence)
                pass
            if "phIso" not in oldMetadataList:
                self.phIso_postSystemRec(topSequence)
                pass

            if "btagging" not in oldMetadataList and not self.isHI:
                self.btagging_postSystemRec(topSequence)
                pass

            if "removeMuonDecor" not in oldMetadataList:
                self.removeMuonDecor_postSystemRec( topSequence )
                pass
            if "inDetVars" not in oldMetadataList:
                self.inDetVars_postSystemRec( topSequence )
                pass

            if "tauid" not in oldMetadataList and not self.isHI:
                self.tauid_postSystemRec(topSequence)
                pass

            # Reset all of the ElementLinks. To be safe.
            topSequence += \
                CfgMgr.xAODMaker__ElementLinkResetAlg( "AODFix_ElementLinkReset" )
            pass

        return


    # Below are the individual AODfixes, split up and documented
    # Name must follow format: <fixID>_<whereCalled>

    def evtRunNum_postSystemRec(self, topSequence): 

        """ This fixes the wrong run number arising from buggy EVNT-to-EVNT transform
             workflows. JIRA: https://its.cern.ch/jira/browse/AGENE-1655"""
        
        variables = {}
        runNumber = None
        if os.access('runargs.AODtoDAOD.py',os.R_OK):
          execfile('runargs.AODtoDAOD.py',variables)

        if 'runArgs' in variables and hasattr(variables['runArgs'],'runNumber'):
          runNumber = variables['runArgs'].runNumber

        if runNumber is not None:
          from xAODEventInfoCnv.xAODEventInfoCnvConf import xAOD__EventInfoRunNumberFixAlg 
          EventInfoRunNumberFixAlg = xAOD__EventInfoRunNumberFixAlg( RunNumber = int(runNumber) )
          topSequence+=EventInfoRunNumberFixAlg

    def trklinks_postSystemRec(self, topSequence):
        """This fixes the links to tracks in muons and btagging
        JIRA: https://its.cern.ch/jira/browse/ATLASRECTS-3988
        """
        if self.isHI:
            containers = ["CombinedMuonTrackParticlesAux.","MuonsAux."]
        else:
            containers = ["CombinedMuonTrackParticlesAux.","BTagging_AntiKt4EMTopoAux.","MuonsAux."]

        topSequence += \
            CfgMgr.xAODMaker__DynVarFixerAlg( "AODFix_DynAuxVariables", Containers = containers )

    def btagging_postSystemRec(self, topSequence):
        """
        This fixes the uptodate BTagging calibration conditions tag.
        """


        JetCollectionList = [ 'AntiKt4EMTopoJets',]

        from AthenaCommon.AppMgr import ToolSvc
        from ParticleJetTools.ParticleJetToolsConf import JetAssocConstAlg
        from BTagging.BTaggingConfiguration import defaultTrackAssoc, defaultMuonAssoc

        assocalg = \
            JetAssocConstAlg(
                "BTaggingParticleAssocAlg",
                JetCollections=JetCollectionList,
                Associators=[defaultTrackAssoc, defaultMuonAssoc]
            )

        topSequence += assocalg

        SA = 'AODFix_'
        from BTagging.BTaggingConfiguration import getConfiguration
        BTagConf = getConfiguration("AODFix")
        BTagConf.PrefixxAODBaseName(False)
        BTagConf.PrefixVertexFinderxAODBaseName(False)

        BTagConf.doNotCheckForTaggerObstacles()
        from BTagging.BTaggingConf import Analysis__StandAloneJetBTaggerAlg as StandAloneJetBTaggerAlg

        btag = "BTagging_"
        AuthorSubString = [ btag+name[:-4] for name in JetCollectionList]
        for i, jet in enumerate(JetCollectionList):
            try:
                btagger = BTagConf.setupJetBTaggerTool(ToolSvc, JetCollection=jet[:-4],
                                                       AddToToolSvc=True, SetupScheme="Retag",
                                                       TaggerList = ['IP2D', 'IP3D', 'MultiSVbb1',  'MultiSVbb2', 'SV1', 'JetFitterNN', 'SoftMu', 'MV2c10', 'MV2rmu', 'MV2r', 'JetVertexCharge', 'MV2c100', 'MV2cl100' , 'DL1', 'DL1r', 'DL1rmu','DL1mu', 'RNNIP'])
                SAbtagger = StandAloneJetBTaggerAlg(name=SA + AuthorSubString[i].lower(),
                                          JetBTaggerTool=btagger,
                                          JetCollectionName = jet,
                                          )

                topSequence += SAbtagger
            except AttributeError as error:
                print '#BTAG# --> ' + str(error)
                print '#BTAG# --> ' + jet
                print '#BTAG# --> ' + AuthorSubString[i]


    def egammaStrips_postSystemRec(self, topSequence):
        """This fixes the strips shower shapes for electrons and photons
        JIRA: https://its.cern.ch/jira/browse/ATLASRECTS-4121
        """
        from egammaRec.egammaRecConf import egammaAODFixAlg
        topSequence+=egammaAODFixAlg()

    def IDTide_postSystemRec(self, topSequence):
        """
        A dummy fix to just add metadata--not called
        Please update postSystemRec to call if you want to run it.
        """
        pass


    def elIso_postSystemRec (self, topSequence):
        from IsolationAlgs.IsoAODFixGetter import isoAODFixGetter
        isoAODFixGetter("Electrons")
    def felIso_postSystemRec (self, topSequence):
        from IsolationAlgs.IsoAODFixGetter import isoAODFixGetter
        isoAODFixGetter("ForwardElectrons")
    def phIso_postSystemRec (self, topSequence):
        from IsolationAlgs.IsoAODFixGetter import isoAODFixGetter
        isoAODFixGetter("Photons")

    def tauid_postSystemRec(self, topSequence):
        """
        This fix recalculates the RNN-based tau identification algorithm.
        """
        from AthenaCommon.AppMgr import ToolSvc
        from RecExConfig.AutoConfiguration import IsInInputFile
        from JetRec.JetRecConf import JetAlgorithm
        from JetRecTools.JetRecToolsConf import CaloClusterConstituentsOrigin
        from JetRecTools.JetRecToolsConf import JetConstituentModSequence

        # Rebuild LCOriginTopoClusters container if not present in input
        if not IsInInputFile("xAOD::CaloClusterContainer",
                             "LCOriginTopoClusters"):
            xAOD_Type_CaloCluster = 1
            clusterOrigin = CaloClusterConstituentsOrigin(
                "CaloClusterConstitOrigin_tau_AODFix",
                InputType=xAOD_Type_CaloCluster)
            ToolSvc += clusterOrigin

            jetConstitModSeq = JetConstituentModSequence(
                "JetConstitModSeq_tau_AODFix",
                InputContainer="CaloCalTopoClusters",
                OutputContainer="LCOriginTopoClusters",
                InputType=xAOD_Type_CaloCluster,
                Modifiers=[clusterOrigin]
            )
            ToolSvc += jetConstitModSeq

            # See also: ATLJETMET-958
            jetAlg = JetAlgorithm("jetalgTCOriginLC", Tools=[jetConstitModSeq])
            topSequence += jetAlg

        # Calculate RNN-ID and set working points
        from tauRec.TauRecAODBuilder import TauRecAODProcessor_RNN_ID
        TauRecAODProcessor_RNN_ID()

    def removeMuonDecor_postSystemRec( self, topSequence ):
        """Fix the issue with muon decorations, described in ATLASRECTS-4499.

        It simply schedules an algorithm that turns the variables, which should
        not be in the primary AOD, into decorations. So that derivation jobs
        could freely overwrite them.
        """

        topSequence += \
            CfgMgr.xAODMaker__DynVarToDecorationAlg( "AODFix_MuonDecorFixer",
                                                     ContainerName = "MuonsAux.",
                                                     AuxVariableNames = [
                                                       "DFCommonMuonsLoose",
                                                       "DFCommonMuonsMedium",
                                                       "DFCommonMuonsTight",
                                                       "DFCommonGoodMuon",
                                                     ] )
        return

    def inDetVars_postSystemRec( self, topSequence ):
        """Fix for the inconsistently filled track particle variables.

        Using the same @c xAODMaker::DynVarFixerAlg algorithm that's used in
        @c trklinks_postSystemRec as well.
        """

        topSequence += \
            CfgMgr.xAODMaker__DynVarFixerAlg( "AODFix_InDetTrackParticlesFixer",
                                              Containers = [
                                                "InDetTrackParticlesAux." ] )
        return
