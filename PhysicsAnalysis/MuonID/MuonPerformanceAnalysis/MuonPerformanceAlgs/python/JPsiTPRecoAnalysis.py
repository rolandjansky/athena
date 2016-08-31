# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from MuonPerformanceAlgs import CommonMuonTPConfig 

def AddConfiguredJPsiTPAlg(name_suffix="myProbes",
                           ProbeKind = "CaloTag", # options: "CaloTag","MS", "Truth", "ID"
                           MatchContainer="Muons",
                           doID=False,
                           doCB=True,
                           doLoose=True,
                           doMedium=True,
                           doTight=True,
                           doHighPt=True,
                           doStandalone=False,
                           doCaloTag=False,
                           doValid=False,
                           writeNtuple=False,
                           useTrigger=True,
                           doDRSys = False,
                           doClosure=False,
                           DoProbeMatchPlots=True,
                           writeProbeIso=True,
                           ProduceEfficiencies=False,
                           decorateProbeWithCaloDeposits=False,
                           IsRunOnDAOD=False):

    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.AppMgr import ToolSvc
    job = AlgSequence()

    ProbeContainer=CommonMuonTPConfig.GetRightMuonContainer()

    if ProbeKind == "MSTrackPart":
        ProbeContainer = "ExtrapolatedMuonTrackParticles"
        ProbeKind = "MS"
        
    if ProbeKind == "ID":
        ProbeContainer = "InDetTrackParticles"

    if ProbeKind == "Truth":
        ProbeContainer = "MuonTruthParticles"

    theAlg = CommonMuonTPConfig.AddTagProbeAlg(name="JPsiTPAlg_%s"%name_suffix,
                           ProbeCont=ProbeContainer,
                           MatchCont=MatchContainer)
    theAlg.TopLevelFolderName = "JPsiTPReco"

    #AddMuonSelectionTool()


    # add a plotting tool
    #PlotTool = AddPlottingTool(name="JPsiTPPlottingTool_%s"%name_suffix,EffiFlag=name_suffix)

    # add J/Psi TP selection tools
    SelectionTools = []
    SelectionTools += [CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_OC_%s"%name_suffix,EffiFlag="%s_OC"%name_suffix, ProbeType = ProbeKind, UseTrigger = useTrigger,IsRunOnDAOD=IsRunOnDAOD, DecorateProbeWithCaloDeposits=decorateProbeWithCaloDeposits)]
    if not "Truth" in ProbeKind:
        SelectionTools += [CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_SC_%s"%name_suffix,EffiFlag="%s_SC"%name_suffix, ProbeType = ProbeKind, UseTrigger = useTrigger,IsRunOnDAOD=IsRunOnDAOD, SameSign=True)]

        
    PlotTools = []
    # add plotting tools in slices
    PlotTools = [CommonMuonTPConfig.AddPlottingTool(name="JPsiTPPlottingTool_%s_All"%name_suffix,
                                                    EffiFlag=name_suffix+"_All",applySF = False,
                                                    DoProbeMatchPlots=DoProbeMatchPlots,
                                                    ProduceEfficiencies=ProduceEfficiencies,
                                                    IsRunOnDAOD=IsRunOnDAOD)]
    for thetool in PlotTools:
        thetool.doJPsiKinematicPlots = True
        thetool.doJPsiDileptonPlots = True

    # add any desired matching tools

    MATCH_DR = 0.05
    
    MatchingTools = []

    if doID:
        thistoolID = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_ID"%name_suffix,EffiFlag="IDTracks",doClosure=doClosure,IsNominal=True)
        thistoolID.MatchToID = True
        thistoolID.ApplyScaleFactors=False
        thistoolID.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolID]
        
        thistoolID_ptr = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_PtrMatching_%s_ID"%name_suffix,EffiFlag="IDTracks_PtrMatching",doClosure=doClosure,IsNominal=True)
        thistoolID_ptr.MatchToID = True
        thistoolID_ptr.ApplyScaleFactors=False
        thistoolID_ptr.ptrMatching = True
        thistoolID_ptr.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolID_ptr]
        
        if doDRSys:
            thistoolID_dRdown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_ID_dRdown"%name_suffix,EffiFlag="IDTracks_dRDown",doClosure=doClosure,IsNominal=False)
            thistoolID_dRdown.MatchToID = True
            thistoolID_dRdown.MaximumDrCut = 0.5 *MATCH_DR
            MatchingTools += [thistoolID_dRdown]
            
            thistoolID_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_ID_dRUp"%name_suffix,EffiFlag="IDTracks_dRUp",doClosure=doClosure,IsNominal=False)
            thistoolID_dRUp.MatchToID = True
            thistoolID_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolID_dRUp]
        
    if doLoose:
        thistoolLoose = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_Loose"%name_suffix,EffiFlag="LooseMuons",doClosure=doClosure,IsNominal=True)
        thistoolLoose.MatchToLoose = True
        thistoolLoose.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Loose",doClosure=doClosure) 
        thistoolLoose.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolLoose]
        
        thistoolLoose_ptr = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_PtrMatching_%s_Loose"%name_suffix,EffiFlag="LooseMuons_PtrMatching",doClosure=doClosure,IsNominal=True)
        thistoolLoose_ptr.MatchToLoose = True
        thistoolLoose_ptr.ptrMatching = True
        thistoolLoose_ptr.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Loose",doClosure=doClosure) 
        thistoolLoose_ptr.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolLoose_ptr]
        
        if doDRSys:
            thistoolLoose_dRdown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_Loose_dRdown"%name_suffix,EffiFlag="LooseMuons_dRDown",doClosure=doClosure,IsNominal=False)
            thistoolLoose_dRdown.MatchToLoose = True
            thistoolLoose_dRdown.MaximumDrCut = 0.5 *MATCH_DR
            MatchingTools += [thistoolLoose_dRdown]
            
            thistoolLoose_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_Loose_dRUp"%name_suffix,EffiFlag="LooseMuons_dRUp",doClosure=doClosure,IsNominal=False)
            thistoolLoose_dRUp.MatchToLoose = True
            thistoolLoose_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolLoose_dRUp]
            
        # also run a version without CaloTag
        thistoolLoose_noCaloTag = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_Loose_noCaloTag"%name_suffix,EffiFlag="LooseMuons_noCaloTag",doClosure=doClosure,IsNominal=True)
        thistoolLoose_noCaloTag.MatchToLoose_noCaloTag = True
        thistoolLoose_noCaloTag.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Loose",doClosure=doClosure) 
        thistoolLoose_noCaloTag.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolLoose_noCaloTag]
        
        thistoolLoose_noCaloTag_ptr = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_PtrMatching_%s_Loose_noCaloTag"%name_suffix,EffiFlag="LooseMuons_noCaloTag_PtrMatching",doClosure=doClosure,IsNominal=True)
        thistoolLoose_noCaloTag_ptr.MatchToLoose_noCaloTag = True
        thistoolLoose_noCaloTag_ptr.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Loose",doClosure=doClosure) 
        thistoolLoose_noCaloTag_ptr.ptrMatching = True
        thistoolLoose_noCaloTag_ptr.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolLoose_noCaloTag_ptr]
        
        if doDRSys:
            thistoolLoose_noCaloTag_dRdown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_Loose_noCaloTag_dRdown"%name_suffix,EffiFlag="LooseMuons_noCaloTag_dRDown",doClosure=doClosure,IsNominal=False)
            thistoolLoose_noCaloTag_dRdown.MatchToLoose_noCaloTag = True
            thistoolLoose_noCaloTag_dRdown.MaximumDrCut = 0.5 *MATCH_DR
            MatchingTools += [thistoolLoose_noCaloTag_dRdown]
            
            thistoolLoose_noCaloTag_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_Loose_noCaloTag_dRUp"%name_suffix,EffiFlag="LooseMuons_noCaloTag_dRUp",doClosure=doClosure,IsNominal=False)
            thistoolLoose_noCaloTag_dRUp.MatchToLoose_noCaloTag = True
            thistoolLoose_noCaloTag_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolLoose_noCaloTag_dRUp]
        
    if doMedium:
        thistoolMed = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_Medium"%name_suffix,EffiFlag="MediumMuons",doClosure=doClosure,IsNominal=True)
        thistoolMed.MatchToMedium = True
        thistoolMed.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Medium",doClosure=doClosure) 
        thistoolMed.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolMed]
        
        thistoolMed_ptr = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_PtrMatching_%s_Medium"%name_suffix,EffiFlag="MediumMuons_PtrMatching",doClosure=doClosure,IsNominal=True)
        thistoolMed_ptr.MatchToMedium = True
        thistoolMed_ptr.ptrMatching = True
        thistoolMed_ptr.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Medium",doClosure=doClosure) 
        thistoolMed_ptr.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolMed_ptr]
        
        if doDRSys:
            thistoolMedium_dRdown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_Medium_dRdown"%name_suffix,EffiFlag="MediumMuons_dRDown",doClosure=doClosure,IsNominal=False)
            thistoolMedium_dRdown.MatchToMedium = True
            thistoolMedium_dRdown.MaximumDrCut = 0.5 *MATCH_DR
            MatchingTools += [thistoolMedium_dRdown]
            
            thistoolMedium_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_Medium_dRUp"%name_suffix,EffiFlag="MediumMuons_dRUp",doClosure=doClosure,IsNominal=False)
            thistoolMedium_dRUp.MatchToMedium = True
            thistoolMedium_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolMedium_dRUp]
        
    if doTight:
        thistoolTight = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_Tight"%name_suffix,EffiFlag="TightMuons",doClosure=doClosure,IsNominal=True)
        thistoolTight.MatchToTight = True
        thistoolTight.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Tight",doClosure=doClosure) 
        thistoolTight.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolTight]
        
        thistoolTight_ptr = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_PtrMatching_%s_Tight"%name_suffix,EffiFlag="TightMuons_PtrMatching",doClosure=doClosure,IsNominal=True)
        thistoolTight_ptr.MatchToTight = True
        thistoolTight_ptr.ptrMatching = True
        thistoolTight_ptr.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Tight",doClosure=doClosure) 
        thistoolTight_ptr.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolTight_ptr]
        
        
        if doDRSys:
            thistoolTight_dRdown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_Tight_dRdown"%name_suffix,EffiFlag="TightMuons_dRDown",doClosure=doClosure,IsNominal=False)
            thistoolTight_dRdown.MatchToTight = True
            thistoolTight_dRdown.MaximumDrCut = 0.5 *MATCH_DR
            MatchingTools += [thistoolTight_dRdown]
            
            thistoolTight_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_Tight_dRUp"%name_suffix,EffiFlag="TightMuons_dRUp",doClosure=doClosure,IsNominal=False)
            thistoolTight_dRUp.MatchToTight = True
            thistoolTight_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolTight_dRUp]

    if doHighPt:
        thistoolHighPt = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_HighPt"%name_suffix,
                                                                       EffiFlag="HighPtMuons",doClosure=doClosure,IsNominal=True)
        thistoolHighPt.MatchToHighPt = True
        thistoolHighPt.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Tight",doClosure=doClosure)  # UPDATE when SF available for high pt
        thistoolHighPt.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolHighPt]
        
        thistoolHighPt_ptr = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_PtrMatching_%s_HighPt"%name_suffix,
                                                                       EffiFlag="HighPtMuons_PtrMatching",doClosure=doClosure,IsNominal=True)
        thistoolHighPt_ptr.MatchToHighPt = True
        thistoolHighPt_ptr.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Tight",doClosure=doClosure)  # UPDATE when SF available for high pt
        thistoolHighPt_ptr.MaximumDrCut = MATCH_DR
        thistoolHighPt_ptr.ptrMatching=True
        MatchingTools += [thistoolHighPt_ptr]
        if doDRSys:
            thistoolHighPt_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_HighPt_dRDown"%name_suffix,
                                                                                  EffiFlag="HighPtMuons_dRDown",doClosure=doClosure,IsNominal=False)
            thistoolHighPt_dRDown.MatchToHighPt = True
            thistoolHighPt_dRDown.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("HighPt",doClosure=doClosure)  # UPDATE 
            thistoolHighPt_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolHighPt_dRDown]
            
            thistoolHighPt_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_HighPt_dRUp"%name_suffix,
                                                                                EffiFlag="HighPtMuons_dRUp",doClosure=doClosure,IsNominal=False)
            thistoolHighPt_dRUp.MatchToHighPt = True
            thistoolHighPt_dRUp.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Tight",doClosure=doClosure)  # UPDATE 
            thistoolHighPt_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolHighPt_dRUp]
       
    if doCaloTag:
        thistoolCT = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_CT"%name_suffix,
                                                                    EffiFlag="CaloTaggedMuons",doClosure=doClosure,IsNominal=True)
        thistoolCT.MatchToCaloTag = True
        thistoolCT.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Loose",doClosure=doClosure)
        thistoolCT.ApplyScaleFactors=doClosure
        thistoolCT.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolCT]
        
        thistoolCT_ptr = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_PtrMatching_%s_CT"%name_suffix,
                                                                    EffiFlag="CaloTaggedMuons_PtrMatching",doClosure=doClosure,IsNominal=True)
        thistoolCT_ptr.MatchToCaloTag = True
        thistoolCT_ptr.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Loose",doClosure=doClosure)
        thistoolCT_ptr.ApplyScaleFactors=doClosure
        thistoolCT_ptr.ptrMatching = True
        thistoolCT_ptr.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolCT_ptr]
        if doDRSys:
            thistoolCT_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_CT_dRDown"%name_suffix,
                                                                               EffiFlag="CaloTaggedMuons_dRDown",doClosure=doClosure,IsNominal=False)
            thistoolCT_dRDown.MatchToCaloTag = True
            thistoolCT_dRDown.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Loose",doClosure=doClosure)
            thistoolCT_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolCT_dRDown]

            thistoolCT_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_CT_dRUp"%name_suffix,
                                                                             EffiFlag="CaloTaggedMuons_dRUp",doClosure=doClosure,IsNominal=False)
            thistoolCT_dRUp.MatchToCaloTag = True
            thistoolCT_dRUp.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Loose",doClosure=doClosure)
            thistoolCT_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolCT_dRUp]     
            
            
    if doValid:        
        thistoolMuidCB = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_MuidCB"%name_suffix,
                                                                        EffiFlag="MuidCB",doClosure=doClosure,IsNominal=True)
        thistoolMuidCB.MatchToMuidCB = True
        thistoolMuidCB.ApplyScaleFactors = False
        thistoolMuidCB.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolMuidCB]
    
        thistoolSTACO = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_STACO"%name_suffix,
                                                                       EffiFlag="STACO",doClosure=doClosure,IsNominal=True)
        thistoolSTACO.MatchToSTACO = True
        thistoolSTACO.ApplyScaleFactors = False
        thistoolSTACO.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolSTACO]
        thistoolMuTag = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_MuTag"%name_suffix,
                                                                       EffiFlag="MuTag",doClosure=doClosure,IsNominal=True)
        thistoolMuTag.MatchToMuTag = True
        thistoolMuTag.ApplyScaleFactors = False
        thistoolMuTag.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolMuTag]
    
        thistoolMuTagIMO = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_MuTagIMO"%name_suffix,
                                                                          EffiFlag="MuTagIMO",doClosure=doClosure,IsNominal=True)
        thistoolMuTagIMO.MatchToMuTagIMO = True
        thistoolMuTagIMO.ApplyScaleFactors = False
        thistoolMuTagIMO.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolMuTagIMO]
    
        thistoolMuidSA = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_MuidSA"%name_suffix,
                                                                        EffiFlag="MuidSA",doClosure=doClosure,IsNominal=True)
        thistoolMuidSA.MatchToMuidSA = True
        thistoolMuidSA.ApplyScaleFactors = False
        thistoolMuidSA.IDhitCut = False
        thistoolMuidSA.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolMuidSA]
        
        thistoolMuGirl = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_MuGirl"%name_suffix,
                                                                        EffiFlag="MuGirl",doClosure=doClosure,IsNominal=True)
        thistoolMuGirl.MatchToMuGirl = True
        thistoolMuGirl.ApplyScaleFactors = False
        thistoolMuGirl.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolMuGirl]
        
    
    if writeProbeIso:
        
        isotool_Loose = CommonMuonTPConfig.AddIsolTPEfficiencyTool(name="JPsiTPIsoEfficiencyTool_Reco_%s_Loose"%name_suffix,EffiFlag="IsoLoose",doClosure=doClosure,IsNominal=True)
        isotool_Loose.IsoSelectionTool = CommonMuonTPConfig.AddIsolationSelectionTool("Loose")
        isotool_Loose.IsolScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("LooseIso",doClosure=doClosure)
        MatchingTools += [isotool_Loose]

        isotool_Tight = CommonMuonTPConfig.AddIsolTPEfficiencyTool(name="JPsiTPIsoEfficiencyTool_Reco_%s_Tight"%name_suffix,EffiFlag="IsoTight",doClosure=doClosure,IsNominal=True)
        isotool_Tight.IsoSelectionTool = CommonMuonTPConfig.AddIsolationSelectionTool("Tight")
        isotool_Tight.IsolScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("TightIso",doClosure=doClosure)
        MatchingTools += [isotool_Tight]

        isotool_Gradient = CommonMuonTPConfig.AddIsolTPEfficiencyTool(name="JPsiTPIsoEfficiencyTool_Reco_%s_Gradient"%name_suffix,EffiFlag="IsoGradient",doClosure=doClosure,IsNominal=True)
        isotool_Gradient.IsoSelectionTool = CommonMuonTPConfig.AddIsolationSelectionTool("Gradient")
        isotool_Gradient.IsolScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("GradientIso",doClosure=doClosure)
        MatchingTools += [isotool_Gradient]

        isotool_GradientLoose = CommonMuonTPConfig.AddIsolTPEfficiencyTool(name="JPsiTPIsoEfficiencyTool_Reco_%s_GradientLoose"%name_suffix,EffiFlag="IsoGradientLoose",doClosure=doClosure,IsNominal=True)
        isotool_GradientLoose.IsoSelectionTool = CommonMuonTPConfig.AddIsolationSelectionTool("GradientLoose")
        isotool_GradientLoose.IsolScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("GradientLooseIso",doClosure=doClosure)
        MatchingTools += [isotool_GradientLoose]

        isotool_FixedCutLoose = CommonMuonTPConfig.AddIsolTPEfficiencyTool(name="JPsiTPIsoEfficiencyTool_Reco_%s_FixedCutLoose"%name_suffix,EffiFlag="IsoFixedCutLoose",doClosure=doClosure,IsNominal=True)
        isotool_FixedCutLoose.IsoSelectionTool = CommonMuonTPConfig.AddIsolationSelectionTool("FixedCutLoose")
        isotool_FixedCutLoose.IsolScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("FixedCutLooseIso",doClosure=doClosure)
        MatchingTools += [isotool_FixedCutLoose]

        isotool_FixedCutTightTrackOnly = CommonMuonTPConfig.AddIsolTPEfficiencyTool(name="JPsiTPIsoEfficiencyTool_Reco_%s_FixedCutTightTrackOnly"%name_suffix,EffiFlag="IsoFixedCutTightTrackOnly",doClosure=doClosure,IsNominal=True)
        isotool_FixedCutTightTrackOnly.IsoSelectionTool = CommonMuonTPConfig.AddIsolationSelectionTool("FixedCutTightTrackOnly")
        isotool_FixedCutTightTrackOnly.IsolScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("FixedCutTightTrackOnlyIso",doClosure=doClosure)
        MatchingTools += [isotool_FixedCutTightTrackOnly]
            
    ntuples = []
    if writeNtuple:
        ntuples.append(CommonMuonTPConfig.AddTreeTool(name="JPsiTPTreeTool_%s"%name_suffix, 
                                                      EffiFlag="Trees",WriteSFInfo=doClosure,
                                                      IsRunOnDAOD=IsRunOnDAOD))
#     if doValid:
    for ntuple in ntuples:
        ntuple.AddExtendedIsolation = True
    # create the TP tool itself
    TheTPTool = CommonMuonTPConfig.AddMuonTPTool(name = "JPsiTPTool_%s"%name_suffix,EffiFlag=name_suffix)
    TheTPTool.MuonTPSelectionTools = SelectionTools
    TheTPTool.PlottingTools = PlotTools
    TheTPTool.MuonTPEfficiencyTools = MatchingTools
    TheTPTool.TreeTools = ntuples
    theAlg.MuonTPTools += [TheTPTool]


def AddJPsiTPRecoAnalysis(writeNtuple=False, 
                          doClosure=False, 
                          doDRSys = False, 
                          doValid=False,
                          DoProbeMatchPlots=True,
                          writeProbeIso=True,
                          ProduceEfficiencies=False,
                          IsRunOnDAOD=False):


    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.AppMgr import ToolSvc
    job = AlgSequence()

    MuonContainerToUse = CommonMuonTPConfig.GetRightMuonContainer()


    ##########################################################################################
    # Add the JPsi TP algorithm using ID tracks as probes and matching to muons
    AddConfiguredJPsiTPAlg(name_suffix = "IDProbes_JPsi",
                           ProbeKind = "ID",
                           MatchContainer = MuonContainerToUse,
                           doID = False,
                           doCB = False, 
                           doLoose = True,
                           doMedium = True,
                           doTight = True,
                           doHighPt=True,
                           doClosure=doClosure,
                           doDRSys=doDRSys,
                           doStandalone = False,
                           doCaloTag = True,
                           writeProbeIso = writeProbeIso,
                           doValid = doValid,
                           writeNtuple = writeNtuple,
                           DoProbeMatchPlots=DoProbeMatchPlots,
                           ProduceEfficiencies=ProduceEfficiencies,
                           decorateProbeWithCaloDeposits=True,
                           IsRunOnDAOD=IsRunOnDAOD)
    ##########################################################################################
    # Add the JPsi TP algorithm using MS tracks as probes and matching to ID tracks
    AddConfiguredJPsiTPAlg(name_suffix = "MSProbes_ID_JPsi",
                           ProbeKind = "MS",
                           MatchContainer = "InDetTrackParticles",
                           doID = True,
                           doCB = False, 
                           doLoose = False,
                           doMedium = False,
                           doDRSys=doDRSys,
                           doTight = False,
                           doHighPt=False,
                           doClosure=doClosure,
                           doStandalone = False,
                           doCaloTag = False,
                           writeProbeIso = writeProbeIso,
                           doValid = doValid,
                           writeNtuple = writeNtuple,
                           DoProbeMatchPlots=DoProbeMatchPlots,
                           ProduceEfficiencies=ProduceEfficiencies,
                           IsRunOnDAOD=IsRunOnDAOD)
    ##########################################################################################
    # Add the JPsi TP algorithm using MS tracks as probes and matching to ID tracks
    AddConfiguredJPsiTPAlg(name_suffix = "MSProbes_Muon_JPsi",
                           ProbeKind = "MS",
                           MatchContainer = MuonContainerToUse,
                           doID = False,
                           doCB = True, 
                           doLoose = True,
                           doMedium = True,
                           doDRSys=doDRSys,
                           doTight = True,
                           doHighPt=True,
                           doClosure=doClosure,
                           doStandalone = False,
                           doCaloTag = True,
                           writeProbeIso = writeProbeIso,
                           doValid = doValid,
                           writeNtuple = writeNtuple,
                           DoProbeMatchPlots=DoProbeMatchPlots,
                           ProduceEfficiencies=ProduceEfficiencies,
                           IsRunOnDAOD=IsRunOnDAOD)


    #########################################################################################
    #Add the JPsi TP algorithm using CaloTag muons as probes and matching to muons
    AddConfiguredJPsiTPAlg(name_suffix = "CaloProbes_JPsi",
                           ProbeKind = "CaloTag",
                           MatchContainer = MuonContainerToUse,
                           doID = False,
                           doCB = False, 
                           doLoose = True,
                           doMedium = True,
                           doDRSys=doDRSys,
                           doClosure=doClosure,
                           doTight = True,
                           doHighPt=True,
                           doStandalone = False,
                           doCaloTag = False,
                           writeProbeIso = writeProbeIso,
                           doValid = doValid,
                           writeNtuple = writeNtuple,
                           DoProbeMatchPlots=DoProbeMatchPlots,
                           ProduceEfficiencies=ProduceEfficiencies,
                           IsRunOnDAOD=IsRunOnDAOD)
    
    if doValid:
        AddConfiguredJPsiTPAlg(name_suffix = "VeryLooseProbes_JPsi",
                               ProbeKind = "VeryLoose",
                               MatchContainer = MuonContainerToUse,
                               doID = True,
                               doCB = True, 
                               doLoose = True,
                               doMedium = True,
                               doDRSys=doDRSys,
                               doClosure=doClosure,
                               doTight = True,
                               doHighPt=True,
                               doStandalone = True,
                               doCaloTag = True,
                               doValid = doValid,
                               writeProbeIso = writeProbeIso,
                               writeNtuple = writeNtuple,
                               DoProbeMatchPlots=DoProbeMatchPlots,
                               ProduceEfficiencies=ProduceEfficiencies,
                               IsRunOnDAOD=IsRunOnDAOD)


    ##########################################################################################
    # Add the JPsi TP algorithm using Truth probes for Muons
    AddConfiguredJPsiTPAlg(name_suffix = "TruthProbes_Muon_JPsi",
                           ProbeKind = "Truth",
                           MatchContainer = MuonContainerToUse,
                           doID = False,
                           doCB = False, 
                           doLoose = True,
                           doMedium = True,
                           doDRSys=False,
                           doClosure=doClosure,
                           doTight = True,
                           doHighPt=True,
                           doStandalone = False,
                           doCaloTag = True,
                           writeProbeIso = False,
                           doValid = doValid,
                           writeNtuple = writeNtuple,
                           useTrigger = False,
                           DoProbeMatchPlots=DoProbeMatchPlots,
                           ProduceEfficiencies=ProduceEfficiencies,
                           IsRunOnDAOD=IsRunOnDAOD)


    ##########################################################################################
    # Add the JPsi TP algorithm using Truth probes for ID tracks
    AddConfiguredJPsiTPAlg(name_suffix = "TruthProbes_ID_JPsi",
                           ProbeKind = "Truth",
                           MatchContainer = "InDetTrackParticles",
                           doID = True,
                           doCB = False, 
                           doLoose = False,
                           doMedium = False,
                           doTight = False,
                           doHighPt=False,
                           doDRSys=False,
                           doClosure=doClosure,
                           doStandalone = False,
                           doCaloTag = False,
                           writeProbeIso = False,
                           doValid = doValid,
                           writeNtuple = writeNtuple,
                           useTrigger = False,
                           DoProbeMatchPlots=DoProbeMatchPlots,
                           ProduceEfficiencies=ProduceEfficiencies,
                           IsRunOnDAOD=IsRunOnDAOD)
