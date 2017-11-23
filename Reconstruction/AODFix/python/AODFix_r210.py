# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.Logging import logging

logAODFix_r210 = logging.getLogger( 'AODFix_r210' )

from AODFix_base import AODFix_base

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
            if "trklinks" not in oldMetadataList:
                self.trklinks_postSystemRec(topSequence)
                pass

            if "egammaStrips" not in oldMetadataList:
                self.egammaStrips_postSystemRec(topSequence)
                pass

            if "btagging" not in oldMetadataList:
                self.btagging_postSystemRec(topSequence)
                pass

            # Reset all of the ElementLinks. To be safe.
            from AthenaCommon import CfgMgr
            topSequence += \
                CfgMgr.xAODMaker__ElementLinkResetAlg( "AODFix_ElementLinkReset" )
            pass

        return


    # Below are the individual AODfixes, split up and documented
    # Name must follow format: <fixID>_<whereCalled>

 
    def trklinks_postSystemRec(self, topSequence):
        """This fixes the links to tracks in muons and btagging
        JIRA: https://its.cern.ch/jira/browse/ATLASRECTS-3988
        """
        from AthenaCommon import CfgMgr
        topSequence += \
            CfgMgr.xAODMaker__DynVarFixerAlg( "AODFix_DynAuxVariables", Containers = ["CombinedMuonTrackParticlesAux.","BTagging_AntiKt4EMTopoAux.","MuonsAux."] )

    def btagging_postSystemRec(self, topSequence):
        """
        This fixes the uptodate BTagging calibration conditions tag.
        """

        from AthenaCommon.AppMgr import ToolSvc

        JetCollectionList = [ 'AntiKt4EMTopoJets',]

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
                                                       TaggerList = ['IP2D', 'IP3D', 'MultiSVbb1',  'MultiSVbb2', 'SV1', 'JetFitterNN', 'SoftMu', 'MV2c10', 'MV2c10mu', 'MV2c10rnn', 'JetVertexCharge', 'MV2c100', 'MV2cl100' , 'DL1', 'DL1rnn', 'DL1mu', 'RNNIP'])
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
        

                
