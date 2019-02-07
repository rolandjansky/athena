# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.Logging import logging
from AthenaCommon import CfgMgr
import PyJobTransforms.trfJobOptions

logAODFix_r210 = logging.getLogger( 'AODFix_r210' )

from AODFix_base import AODFix_base

from RecExConfig.InputFilePeeker import inputFileSummary
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
            
            if "evtRunNum" not in oldMetadataList:
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

        """ This fixes the wrong run number arising from buggy EVNT-to-EVNT transform workflows. 
          --- > JIRA: https://its.cern.ch/jira/browse/AGENE-1655
        """
        schedule_evtRunNum = False
        if 'mc_channel_number' in inputFileSummary:
          input_mcChanNb = inputFileSummary['mc_channel_number'][0]
          """
          MC15.366000.Sherpa_221_NNPDF30NNLO_Znunu_PTV0_70.py
          MC15.366001.Sherpa_221_NNPDF30NNLO_Znunu_PTV70_100.py
          MC15.366002.Sherpa_221_NNPDF30NNLO_Znunu_PTV100_140_MJJ0_500.py
          MC15.366003.Sherpa_221_NNPDF30NNLO_Znunu_PTV100_140_MJJ500_1000.py
          MC15.366004.Sherpa_221_NNPDF30NNLO_Znunu_PTV100_140_MJJ1000_E_CMS.py
          MC15.366005.Sherpa_221_NNPDF30NNLO_Znunu_PTV140_280_MJJ0_500.py
          MC15.366006.Sherpa_221_NNPDF30NNLO_Znunu_PTV140_280_MJJ500_1000.py
          MC15.366007.Sherpa_221_NNPDF30NNLO_Znunu_PTV140_280_MJJ1000_E_CMS.py
          MC15.366008.Sherpa_221_NNPDF30NNLO_Znunu_PTV280_500.py
          """
          if input_mcChanNb>= 366000 and input_mcChanNb<=366008: 
            schedule_evtRunNum = True
          """
          MC15.304784.Pythia8EvtGen_A14NNPDF23LO_jetjet_Powerlaw.py
          """
          if input_mcChanHb==304784:
            schedule_evtRunNum = True
          """
          mc15_13TeV.364310.Sherpa_222_NNPDF30NNLO_Wenu_MAXHTPTV70_140.evgen.EVNT.e6209
          mc15_13TeV.364311.Sherpa_222_NNPDF30NNLO_Wmunu_MAXHTPTV70_140.evgen.EVNT.e6209 
          mc15_13TeV.364312.Sherpa_222_NNPDF30NNLO_Wtaunu_MAXHTPTV70_140.evgen.EVNT.e6209
          mc15_13TeV.364103.Sherpa_221_NNPDF30NNLO_Zmumu_MAXHTPTV70_140_CVetoBVeto.evgen.EVNT.e5271
          mc15_13TeV.364132.Sherpa_221_NNPDF30NNLO_Ztautau_MAXHTPTV70_140_CFilterBVeto.evgen.EVNT.e5307
          mc15_13TeV.364145.Sherpa_221_NNPDF30NNLO_Znunu_MAXHTPTV70_140_CVetoBVeto.evgen.EVNT.e5308
          mc15_13TeV.364146.Sherpa_221_NNPDF30NNLO_Znunu_MAXHTPTV70_140_CFilterBVeto.evgen.EVNT.e5308
          mc15_13TeV.364106.Sherpa_221_NNPDF30NNLO_Zmumu_MAXHTPTV140_280_CVetoBVeto.evgen.EVNT.e5271
          mc15_13TeV.364107.Sherpa_221_NNPDF30NNLO_Zmumu_MAXHTPTV140_280_CFilterBVeto.evgen.EVNT.e5271
          mc15_13TeV.364120.Sherpa_221_NNPDF30NNLO_Zee_MAXHTPTV140_280_CVetoBVeto.evgen.EVNT.e5299
          mc15_13TeV.364134.Sherpa_221_NNPDF30NNLO_Ztautau_MAXHTPTV140_280_CVetoBVeto.evgen.EVNT.e5307
          mc15_13TeV.364148.Sherpa_221_NNPDF30NNLO_Znunu_MAXHTPTV140_280_CVetoBVeto.evgen.EVNT.e5308
          mc15_13TeV.364162.Sherpa_221_NNPDF30NNLO_Wmunu_MAXHTPTV140_280_CVetoBVeto.evgen.EVNT.e5340
          mc15_13TeV.364163.Sherpa_221_NNPDF30NNLO_Wmunu_MAXHTPTV140_280_CFilterBVeto.evgen.EVNT.e5340
          mc15_13TeV.364176.Sherpa_221_NNPDF30NNLO_Wenu_MAXHTPTV140_280_CVetoBVeto.evgen.EVNT.e5340
          mc15_13TeV.364177.Sherpa_221_NNPDF30NNLO_Wenu_MAXHTPTV140_280_CFilterBVeto.evgen.EVNT.e5340
          mc15_13TeV.364190.Sherpa_221_NNPDF30NNLO_Wtaunu_MAXHTPTV140_280_CVetoBVeto.evgen.EVNT.e5340
          mc15_13TeV.364191.Sherpa_221_NNPDF30NNLO_Wtaunu_MAXHTPTV140_280_CFilterBVeto.evgen.EVNT.e5340
        """
          if input_mcChanNb>=364310 and input_mcChanNb<=364312:
            schedule_evtRunNum = True
          if input_mcChanNb==364132:
            schedule_evtRunNum = True
          if input_mcChanNb>=364145 and input_mcChanNb<=364146:
            schedule_evtRunNum = True
          if input_mcChanNb>=364106 and input_mcChanNb<=364107:
            schedule_evtRunNum = True
          if input_mcChanNb==364120:
            schedule_evtRunNum = True
          if input_mcChanNb==364134:
            schedule_evtRunNum = True
          if input_mcChanNb==364148:
            schedule_evtRunNum = True
          if input_mcChanNb>=364162 and input_mcChanNb<=364163:
            schedule_evtRunNum = True
          if input_mcChanNb>=364176 and input_mcChanNb<=364177:
            schedule_evtRunNum = True
          if input_mcChanNb>=364190 and input_mcChanNb<=364191:
            schedule_evtRunNum = True

        if schedule_evtRunNum:
          variables = {}
          runNumber = None
          if os.access('runargs.AODtoDAOD.py',os.R_OK):
            execfile('runargs.AODtoDAOD.py',variables)
          if 'runArgs' in variables and hasattr(variables['runArgs'],'runNumber'):
            runNumber = variables['runArgs'].runNumber

          if runNumber != input_mcChanNb:
            from xAODEventInfoCnv.xAODEventInfoCnvConf import xAOD__EventInfoRunNumberFixAlg 
            EventInfoRunNumberFixAlg = xAOD__EventInfoRunNumberFixAlg( McChannelNumber = int(runNumber) )
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
