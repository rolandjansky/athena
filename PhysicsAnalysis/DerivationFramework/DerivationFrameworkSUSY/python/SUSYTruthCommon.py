# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
dfsusylog = Logging.logging.getLogger('SUSYTruthCommon')

#==============================================================================
# Tau truth building/matching
#==============================================================================
# not possible for now to pass private sequence to TauTruthCommon
# therefore, replicate functionality (hopefully temporary solution)
def addTruthTaus(augmentationTools=[]):

    if not augmentationTools:
        dfsusylog.warning( "No AugmentationTools passed to addTruthTaus! Will not schedule truth tau building/matching." )
        return

    from AthenaCommon.AppMgr import ToolSvc
    from DerivationFrameworkTau.DerivationFrameworkTauConf import DerivationFramework__TauTruthMatchingWrapper
    from TauAnalysisTools.TauAnalysisToolsConf import TauAnalysisTools__TauTruthMatchingTool
    from RecExConfig.ObjKeyStore import objKeyStore
    if objKeyStore.isInInput( "xAOD::TauJetContainer", "TauJets" ):    
        
        dfsusylog.info( "Scheduling truth tau building/matching." )

        SUSYTauTruthMatchingTool = TauAnalysisTools__TauTruthMatchingTool(name="SUSYTauTruthMatchingTool")
        SUSYTauTruthMatchingTool.WriteTruthTaus = True
        SUSYTauTruthMatchingTool.WriteInvisibleFourMomentum = True
        SUSYTauTruthMatchingTool.MCTruthClassifierTool = ToolSvc.DFCommonTruthClassifier
        ToolSvc += SUSYTauTruthMatchingTool
        
        SUSYTauTruthMatchingWrapper = DerivationFramework__TauTruthMatchingWrapper( name = "SUSYTauTruthMatchingWrapper",
                                                                                    TauTruthMatchingTool = SUSYTauTruthMatchingTool,
                                                                                    TauContainerName     = "TauJets")
        ToolSvc += SUSYTauTruthMatchingWrapper
        augmentationTools.append(SUSYTauTruthMatchingWrapper)





