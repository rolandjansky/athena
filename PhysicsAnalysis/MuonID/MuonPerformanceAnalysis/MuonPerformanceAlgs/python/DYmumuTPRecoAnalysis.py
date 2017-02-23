# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from MuonPerformanceAlgs import CommonMuonTPConfig 
################################################################
# Add a version of the Z TP algorithm and all associated tools
################################################################
def AddConfiguredDYmumuTPAlg(name_suffix="myProbes",
                            ProbeKind = "CaloTag", # options: "CaloTag","MS", "Truth", "ID"
                            MatchContainer="Muons",
                            doID=False,
                            doDetailedID=False,
                            doCB = False, doLoose=True,
                            doMedium=True,doTight=True,
                            doHighPt=True,
                            doStandalone=False, doCaloTag=False,
                            doIndividualAuthors=False,
                            doEtaSlices=False, writeNtuple=False,
                            doClosure = False, doDRSys = False,
                            DoProbeMatchPlots=True,
                            writeProbeIso=True,
                            ProduceEfficiencies=False,
                            IsRunOnDAOD=False):


    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.AppMgr import ToolSvc
    job = AlgSequence()

    MuonContainerToUse = CommonMuonTPConfig.GetRightMuonContainer()
    #if hasattr(job, "MuonQualityUpdater"):
        #MuonContainerToUse = "UpdatedMuons"

    ProbeContainer=CommonMuonTPConfig.GetRightMuonContainer()

    if ProbeKind == "MSTrackPart":
        ProbeContainer = "ExtrapolatedMuonTrackParticles"
        ProbeKind = "MS"
        
    if ProbeKind == "ID":
        ProbeContainer = "InDetTrackParticles"
    if ProbeKind == "Truth":
        ProbeContainer = "MuonTruthParticles"

    theAlg = CommonMuonTPConfig.AddTagProbeAlg(name="DYmumuTPAlg_%s"%name_suffix,
                           ProbeCont=ProbeContainer,
                           MatchCont=MatchContainer)
    theAlg.TopLevelFolderName = "DYmumuTPReco"

    #AddMuonSelectionTool()


    # add a plotting tool
    #PlotTool = AddPlottingTool(name="DYmumuTPPlottingTool_%s"%name_suffix,EffiFlag=name_suffix)

    # add four TP selection tools (iso and antiiso OC and SC)
    SelectionTools = []
    SelectionTools += [CommonMuonTPConfig.AddZmumuTPSelectionTool(name="DYmumuTPSelectionTool_OC_%s"%name_suffix,
                                                                  EffiFlag="%s_OC"%name_suffix, 
                                                                  ProbeType = ProbeKind,IsRunOnDAOD=IsRunOnDAOD)]
    SelectionTools += [CommonMuonTPConfig.AddZmumuTPSelectionTool(name="DYmumuTPSelectionTool_SC_%s"%name_suffix,
                                                                  EffiFlag="%s_SC"%name_suffix, 
                                                                  ProbeType = ProbeKind,IsRunOnDAOD=IsRunOnDAOD, 
                                                                  SameSign=True)]
    SelectionTools += [CommonMuonTPConfig.AddZmumuTPSelectionTool(name="DYmumuTPSelectionTool_OCAntiIso_%s"%name_suffix,
                                                                  EffiFlag="%s_OCAntiIso"%name_suffix, 
                                                                  ProbeType = ProbeKind,IsRunOnDAOD=IsRunOnDAOD, 
                                                                  AntiIso=True)]
    SelectionTools += [CommonMuonTPConfig.AddZmumuTPSelectionTool(name="DYmumuTPSelectionTool_SCAntiIso_%s"%name_suffix,
                                                                  EffiFlag="%s_SCAntiIso"%name_suffix, 
                                                                  ProbeType = ProbeKind,IsRunOnDAOD=IsRunOnDAOD, 
                                                                  SameSign=True, 
                                                                  AntiIso=True)]
    for tool in SelectionTools:
        tool.LowMassWindow = 81000
        tool.HighMassWindow = 50e6 # should be safe at LHC as long as energy is conserved... 
        tool.TagPtCut = 30e3  # accept tags above 30 GeV.
        tool.ProbePtCut = 30e3  # accept probes above 30 GeV.
        

    PlotTools = []
    # add plotting tools in slices
    if doEtaSlices:
        Slices = {
                   "All" : (0.0,10.0),
                   "noCrack" : (0.1,10.0),
                   "Crack" : (0.0,0.1),
                   "Barrel" : (0.1,1.1),
                   "Transition" : (1.1, 1.3),
                   "Endcap" : (1.3,2.0),
                   "CSC" : (2.0,2.7)
                   }

        for etaslice, interval in Slices.iteritems():
            PlotTool = CommonMuonTPConfig.AddPlottingTool(name="DYmumuTPPlottingTool_%s_%s"%(name_suffix,etaslice),
                                                          EffiFlag=name_suffix+"_"+etaslice,
                                                          applySF = False,
                                                          DoProbeMatchPlots=DoProbeMatchPlots,
                                                          ProduceEfficiencies=ProduceEfficiencies,
                                                          IsRunOnDAOD=IsRunOnDAOD)
            PlotTool.ProbeAbsEtaMin = interval[0]
            PlotTool.ProbeAbsEtaMax = interval[1]
            PlotTools.append(PlotTool)
            if doClosure:
                PlotToolCL = CommonMuonTPConfig.AddPlottingTool(name="DYmumuTPPlottingToolClosure_%s_%s"%(name_suffix,etaslice),
                                                                EffiFlag=name_suffix+"_"+etaslice+"_Closure",
                                                                applySF = True,
                                                                DoProbeMatchPlots=DoProbeMatchPlots,
                                                                ProduceEfficiencies=ProduceEfficiencies,
                                                                IsRunOnDAOD=IsRunOnDAOD)
                PlotToolCL.ProbeAbsEtaMin = interval[0]
                PlotToolCL.ProbeAbsEtaMax = interval[1]
                PlotTools.append(PlotToolCL)
                
    else:
        PlotTools = [CommonMuonTPConfig.AddPlottingTool(name="DYmumuTPPlottingTool_%s_All"%name_suffix,
                                                        EffiFlag=name_suffix+"_All",
                                                        applySF = False,
                                                        DoProbeMatchPlots=DoProbeMatchPlots,
                                                        ProduceEfficiencies=ProduceEfficiencies,
                                                        IsRunOnDAOD=IsRunOnDAOD)]
        if doClosure:
            PlotTools.append(CommonMuonTPConfig.AddPlottingTool(name="DYmumuTPPlottingToolClosure_%s_All"%name_suffix,
                                                                EffiFlag=name_suffix+"_All_Closure",
                                                                applySF = True,
                                                                DoProbeMatchPlots=DoProbeMatchPlots,
                                                                ProduceEfficiencies=ProduceEfficiencies,
                                                                IsRunOnDAOD=IsRunOnDAOD))
        
    # Config the plot tools
    for thetool in PlotTools:
        thetool.doZmumuKinematicPlots = False
        thetool.doFineEtaPhiPlots = True
        thetool.doDetectorRegionPlots = True
    
    # add any desired matching tools
    MATCH_DR = 0.05

    
    MatchingTools = []
    if doCB:
        thistoolCB = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_CB"%name_suffix,
                                                                    EffiFlag="CombinedMuons",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=True)
        thistoolCB.MatchToCB = True
        thistoolCB.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Medium",doClosure=doClosure)
        thistoolCB.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolCB]
        if doDRSys:
            thistoolCB_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_CB_dRDown"%name_suffix,
                                                                               EffiFlag="CombinedMuons_dRDown",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=False)                   
            thistoolCB_dRDown.MatchToCB = True
            thistoolCB_dRDown.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Medium",doClosure=doClosure)
            thistoolCB_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolCB_dRDown]
            
            thistoolCB_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_CB_dRUp"%name_suffix,
                                                                             EffiFlag="CombinedMuons_dRUp",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=False)
            thistoolCB_dRUp.MatchToCB = True
            thistoolCB_dRUp.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Medium",doClosure=doClosure)
            thistoolCB_dRUp.MaximumDrCut = 2. * MATCH_DR
            MatchingTools += [thistoolCB_dRUp]

    if doLoose:
        thistoolLoose = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_Loose"%name_suffix,
                                                                       EffiFlag="LooseMuons",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=True)
        thistoolLoose.MatchToLoose = True
        thistoolLoose.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Loose",doClosure=doClosure)
        thistoolLoose.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolLoose]
        if doDRSys:
            thistoolLoose_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_Loose_dRDown"%name_suffix,
                                                                                  EffiFlag="LooseMuons_dRDown",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=False)
            thistoolLoose_dRDown.MatchToLoose = True
            thistoolLoose_dRDown.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Loose",doClosure=doClosure)
            thistoolLoose_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolLoose_dRDown]
            
            thistoolLoose_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_Loose_dRUp"%name_suffix,
                                                                                EffiFlag="LooseMuons_dRUp",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=False)
            thistoolLoose_dRUp.MatchToLoose = True
            thistoolLoose_dRUp.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Loose",doClosure=doClosure)
            thistoolLoose_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolLoose_dRUp]
            
        # also run a version without CaloTag
        thistoolLoose_noCaloTag = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_Loose_noCaloTag"%name_suffix,
                                                                                 EffiFlag="LooseMuons_noCaloTag",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=True)
        thistoolLoose_noCaloTag.MatchToLoose_noCaloTag = True
        thistoolLoose_noCaloTag.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Loose",doClosure=doClosure)
        thistoolLoose_noCaloTag.ApplyScaleFactors=doClosure
        #thistoolLoose_noCaloTag.ApplyScaleFactors = True
        thistoolLoose_noCaloTag.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolLoose_noCaloTag]
        if doDRSys:
            thistoolLoose_noCaloTag_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_Loose_noCaloTag_dRDown"%name_suffix,
                                                                                            EffiFlag="LooseMuons_noCaloTag_dRDown",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=False)
            thistoolLoose_noCaloTag_dRDown.MatchToLoose_noCaloTag = True
            thistoolLoose_noCaloTag_dRDown.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Loose",doClosure=doClosure)
            thistoolLoose_noCaloTag_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolLoose_noCaloTag_dRDown]
            
            thistoolLoose_noCaloTag_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_Loose_noCaloTag_dRUp"%name_suffix,
                                                                                          EffiFlag="LooseMuons_noCaloTag_dRUp",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=False)
            thistoolLoose_noCaloTag_dRUp.MatchToLoose_noCaloTag = True
            thistoolLoose_noCaloTag_dRUp.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Loose",doClosure=doClosure)
            thistoolLoose_noCaloTag_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolLoose_noCaloTag_dRUp]
        
    if doMedium:
        thistoolMed = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_Medium"%name_suffix,
                                                                     EffiFlag="MediumMuons",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=True)
        thistoolMed.MatchToMedium = True
        thistoolMed.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Medium",doClosure=doClosure)
        thistoolMed.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolMed]
        if doDRSys:
            thistoolMed_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_Medium_dRDown"%name_suffix,
                                                                                EffiFlag="MediumMuons_dRDown",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=False)
            thistoolMed_dRDown.MatchToMedium = True
            thistoolMed_dRDown.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Medium",doClosure=doClosure)
            thistoolMed_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolMed_dRDown]

            thistoolMed_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_Medium_dRUp"%name_suffix,
                                                                              EffiFlag="MediumMuons_dRUp",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=False)
            thistoolMed_dRUp.MatchToMedium = True
            thistoolMed_dRUp.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Medium",doClosure=doClosure)
            thistoolMed_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolMed_dRUp]
            
        thistoolMed_PtrMatch = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_PtrMatch_%s_Medium"%name_suffix,
                                                                              EffiFlag="MediumMuons_PtrMatching",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=True)
        thistoolMed_PtrMatch.MatchToMedium = True
        thistoolMed_PtrMatch.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Medium",doClosure=doClosure)
        thistoolMed_PtrMatch.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolMed_PtrMatch]

        
    if doTight:
        thistoolTight = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_Tight"%name_suffix,
                                                                       EffiFlag="TightMuons",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=True)
        thistoolTight.MatchToTight = True
        thistoolTight.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Tight",doClosure=doClosure)
        thistoolTight.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolTight]
        if doDRSys:
            thistoolTight_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_Tight_dRDown"%name_suffix,
                                                                                  EffiFlag="TightMuons_dRDown",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=False)
            thistoolTight_dRDown.MatchToTight = True
            thistoolTight_dRDown.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Tight",doClosure=doClosure)
            thistoolTight_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolTight_dRDown]
            
            thistoolTight_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_Tight_dRUp"%name_suffix,
                                                                                EffiFlag="TightMuons_dRUp",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=False)
            thistoolTight_dRUp.MatchToTight = True
            thistoolTight_dRUp.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Tight",doClosure=doClosure)
            thistoolTight_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolTight_dRUp]

        
    if doHighPt:
        thistoolHighPt = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_HighPt"%name_suffix,
                                                                       EffiFlag="HighPtMuons",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=True)
        thistoolHighPt.MatchToHighPt = True
        thistoolHighPt.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Tight",doClosure=doClosure)  # UPDATE when SF available for high pt
        thistoolHighPt.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolHighPt]
        if doDRSys:
            thistoolHighPt_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_HighPt_dRDown"%name_suffix,
                                                                                  EffiFlag="HighPtMuons_dRDown",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=False)
            thistoolHighPt_dRDown.MatchToHighPt = True
            thistoolHighPt_dRDown.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("HighPt",doClosure=doClosure)  # UPDATE when SF available for high pt
            thistoolHighPt_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolHighPt_dRDown]
            
            thistoolHighPt_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_HighPt_dRUp"%name_suffix,
                                                                                EffiFlag="HighPtMuons_dRUp",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=False)
            thistoolHighPt_dRUp.MatchToHighPt = True
            thistoolHighPt_dRUp.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Tight",doClosure=doClosure)  # UPDATE when SF available for high pt
            thistoolHighPt_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolHighPt_dRUp]
            

    if doStandalone:
        thistoolSA = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_SA"%name_suffix,
                                                                    EffiFlag="StandaloneMuons",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=True)
        thistoolSA.MatchToAnyMS = True
        thistoolSA.IDhitCut = False
        thistoolSA.ApplyScaleFactors = False
        thistoolSA.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolSA]
        if doDRSys:
            thistoolSA_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_SA_dRDown"%name_suffix,
                                                                               EffiFlag="StandaloneMuons_dRDown",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=False)
            thistoolSA_dRDown.MatchToAnyMS = True
            thistoolSA_dRDown.IDhitCut = False
            thistoolSA_dRDown.IsNominal=False
            thistoolSA_dRDown.ApplyScaleFactors = doClosure
            thistoolSA_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolSA_dRDown]

            thistoolSA_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_SA_dRUp"%name_suffix,
                                                                             EffiFlag="StandaloneMuons_dRUp",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=False)
            thistoolSA_dRUp.MatchToAnyMS = True
            thistoolSA_dRUp.IDhitCut = False
            thistoolSA_dRUp.IsNominal=False
            thistoolSA_dRUp.ApplyScaleFactors = doClosure
            thistoolSA_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolSA_dRUp]

    if doCaloTag:
        thistoolCT = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_CT"%name_suffix,
                                                                    EffiFlag="CaloTaggedMuons",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=True)
        thistoolCT.MatchToCaloTag = True
        thistoolCT.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Loose",doClosure=doClosure)
        thistoolCT.ApplyScaleFactors=doClosure
        thistoolCT.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolCT]
        if doDRSys:
            thistoolCT_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_CT_dRDown"%name_suffix,
                                                                               EffiFlag="CaloTaggedMuons_dRDown",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=False)
            thistoolCT_dRDown.MatchToCaloTag = True
            thistoolCT_dRDown.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Loose",doClosure=doClosure)
            thistoolCT_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolCT_dRDown]

            thistoolCT_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_CT_dRUp"%name_suffix,
                                                                             EffiFlag="CaloTaggedMuons_dRUp",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=False)
            thistoolCT_dRUp.MatchToCaloTag = True
            thistoolCT_dRUp.RecoScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Loose",doClosure=doClosure)
            thistoolCT_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolCT_dRUp]

    if doID:
        thistoolID = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_ID"%name_suffix,
                                                                    EffiFlag="IDTracks",MatchCont="InDetTrackParticles",doClosure=doClosure,IsNominal=True)
        thistoolID.MatchToID = True
        thistoolID.ApplyScaleFactors = False
        thistoolID.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolID]
        if doDRSys:
            thistoolID_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_ID_dRUp"%name_suffix,
                                                                             EffiFlag="IDTracks_dRUp",MatchCont="InDetTrackParticles",doClosure=doClosure,IsNominal=False)
            thistoolID_dRUp.MatchToID = True
            thistoolID_dRUp.ApplyScaleFactors = False
            thistoolID_dRUp.MaximumDrCut = 2 * MATCH_DR
            thistoolID_dRUp.IsNominal=False
            MatchingTools += [thistoolID_dRUp]

            thistoolID_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_ID_dRDown"%name_suffix,
                                                                               EffiFlag="IDTracks_dRDown",MatchCont="InDetTrackParticles",doClosure=doClosure,IsNominal=False)
            thistoolID_dRDown.MatchToID = True
            thistoolID_dRDown.ApplyScaleFactors = False
            thistoolID_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            thistoolID_dRDown.IsNominal=False
            MatchingTools += [thistoolID_dRDown]
            
    if doDetailedID:
        IDCuts= ["TrtCut","SctCut","PixCut","SiHolesCut"]
        for cut in IDCuts+["NoIDQualityCut"]:
            CutList = [x+"Off" for x in IDCuts if not x == cut]
            
            thistoolIDsub =  CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_ID_CutCheck_%s"%(name_suffix,cut),
                                                                    EffiFlag="IDTracks_IDCutCheck_%s"%cut,MatchCont="InDetTrackParticles",doClosure=doClosure,IsNominal=True)
            thistoolIDsub.SelectionTool = CommonMuonTPConfig.GetMuonSelectionTool(CutList)
            
            thistoolIDsub.MatchToID = True
            thistoolIDsub.ApplyScaleFactors = False
            thistoolIDsub.MaximumDrCut = MATCH_DR
            MatchingTools += [thistoolIDsub]
            if doDRSys:
                thistoolIDsub_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_ID_CutCheck_%s_dRUp"%(name_suffix,cut),
                                                                                EffiFlag="IDTracks_IDCutCheck_%s_dRUp"%cut,MatchCont="InDetTrackParticles",doClosure=doClosure,IsNominal=False)
                thistoolIDsub_dRUp.MatchToID = True
                thistoolIDsub_dRUp.SelectionTool = CommonMuonTPConfig.GetMuonSelectionTool(CutList)
                thistoolIDsub_dRUp.ApplyScaleFactors = False
                thistoolIDsub_dRUp.MaximumDrCut = 2 * MATCH_DR
                thistoolIDsub_dRUp.IsNominal=False
                MatchingTools += [thistoolIDsub_dRUp]

                thistoolIDsub_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_ID_CutCheck_%s_dRDown"%(name_suffix,cut),
                                                                                EffiFlag="IDTracks_IDCutCheck_%s_dRDown"%cut,MatchCont="InDetTrackParticles",doClosure=doClosure,IsNominal=False)
                thistoolIDsub_dRDown.MatchToID = True
                
                thistoolIDsub_dRDown.SelectionTool = CommonMuonTPConfig.GetMuonSelectionTool(CutList)
                thistoolIDsub_dRDown.ApplyScaleFactors = False
                thistoolIDsub_dRDown.MaximumDrCut = 0.5 * MATCH_DR
                thistoolIDsub_dRDown.IsNominal=False
                MatchingTools += [thistoolIDsub_dRDown]

    if doIndividualAuthors:
        thistoolMuidCB = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_MuidCB"%name_suffix,
                                                                        EffiFlag="MuidCB",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=True)
        thistoolMuidCB.MatchToMuidCB = True
        thistoolMuidCB.ApplyScaleFactors = False
        thistoolMuidCB.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolMuidCB]
        if doDRSys:
            thistoolMuidCB_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_MuidCB_dRDown"%name_suffix,
                                                                                   EffiFlag="MuidCB_dRDown",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=False)
            thistoolMuidCB_dRDown.MatchToMuidCB = True
            thistoolMuidCB_dRDown.ApplyScaleFactors = False
            thistoolMuidCB_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            thistoolMuidCB_dRDown.IsNominal=False
            MatchingTools += [thistoolMuidCB_dRDown]

            thistoolMuidCB_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_MuidCB_dRUp"%name_suffix,
                                                                                 EffiFlag="MuidCB_dRUp",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=False)
            thistoolMuidCB_dRUp.MatchToMuidCB = True
            thistoolMuidCB_dRUp.ApplyScaleFactors = False
            thistoolMuidCB_dRUp.MaximumDrCut = 2 * MATCH_DR
            thistoolMuidCB_dRUp.IsNominal=False
            MatchingTools += [thistoolMuidCB_dRUp]

        thistoolSTACO = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_STACO"%name_suffix,
                                                                       EffiFlag="STACO",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=True)
        thistoolSTACO.MatchToSTACO = True
        thistoolSTACO.ApplyScaleFactors = False
        thistoolSTACO.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolSTACO]
        if doDRSys:
            thistoolSTACO_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_STACO_dRDown"%name_suffix,
                                                                                  EffiFlag="STACO_dRDown",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=False)
            thistoolSTACO_dRDown.MatchToSTACO = True
            thistoolSTACO_dRDown.ApplyScaleFactors = False
            thistoolSTACO_dRDown.IsNominal=False
            thistoolSTACO_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolSTACO_dRDown]

            thistoolSTACO_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_STACO_dRUp"%name_suffix,
                                                                                EffiFlag="STACO_dRUp",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=False)
            thistoolSTACO_dRUp.MatchToSTACO = True
            thistoolSTACO_dRUp.ApplyScaleFactors = False
            thistoolSTACO_dRUp.IsNominal=False
            thistoolSTACO_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolSTACO_dRUp]

        thistoolMuTag = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_MuTag"%name_suffix,
                                                                       EffiFlag="MuTag",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=True)
        thistoolMuTag.MatchToMuTag = True
        thistoolMuTag.ApplyScaleFactors = False
        thistoolMuTag.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolMuTag]
        if doDRSys:
            thistoolMuTag_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_MuTag_dRDown"%name_suffix,
                                                                                  EffiFlag="MuTag_dRDown",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=False)
            thistoolMuTag_dRDown.MatchToMuTag = True
            thistoolMuTag_dRDown.ApplyScaleFactors = False
            thistoolMuTag_dRDown.IsNominal=False
            thistoolMuTag_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolMuTag_dRDown]

            thistoolMuTag_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_MuTag_dRUp"%name_suffix,
                                                                                EffiFlag="MuTag_dRUp",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=False)
            thistoolMuTag_dRUp.MatchToMuTag = True
            thistoolMuTag_dRUp.ApplyScaleFactors = False
            thistoolMuTag_dRUp.IsNominal=False
            thistoolMuTag_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolMuTag_dRUp]

        thistoolMuTagIMO = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_MuTagIMO"%name_suffix,
                                                                          EffiFlag="MuTagIMO",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=True)
        thistoolMuTagIMO.MatchToMuTagIMO = True
        thistoolMuTagIMO.ApplyScaleFactors = False
        thistoolMuTagIMO.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolMuTagIMO]
        if doDRSys:
            thistoolMuTagIMO_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_MuTagIMO_dRDown"%name_suffix,
                                                                                     EffiFlag="MuTagIMO_dRDown",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=False)
            thistoolMuTagIMO_dRDown.MatchToMuTagIMO = True
            thistoolMuTagIMO_dRDown.ApplyScaleFactors = False
            thistoolMuTagIMO_dRDown.IsNominal=False
            thistoolMuTagIMO_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolMuTagIMO_dRDown]

            thistoolMuTagIMO_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_MuTagIMO_dRUp"%name_suffix,
                                                                                   EffiFlag="MuTagIMO_dRUp",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=False)
            thistoolMuTagIMO_dRUp.MatchToMuTagIMO = True
            thistoolMuTagIMO_dRUp.ApplyScaleFactors = False
            thistoolMuTagIMO_dRUp.IsNominal=False
            thistoolMuTagIMO_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolMuTagIMO_dRUp]

        thistoolMuidSA = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_MuidSA"%name_suffix,
                                                                        EffiFlag="MuidSA",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=True)
        thistoolMuidSA.MatchToMuidSA = True
        thistoolMuidSA.ApplyScaleFactors = False
        thistoolMuidSA.IDhitCut = False
        thistoolMuidSA.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolMuidSA]
        if doDRSys:
            thistoolMuidSA_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_MuidSA_dRDown"%name_suffix,
                                                                                   EffiFlag="MuidSA_dRDown",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=False)
            thistoolMuidSA_dRDown.MatchToMuidSA = True
            thistoolMuidSA_dRDown.ApplyScaleFactors = False
            thistoolMuidSA_dRDown.IsNominal=False
            thistoolMuidSA_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolMuidSA_dRDown]

            thistoolMuidSA_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_MuidSA_dRUp"%name_suffix,
                                                                                 EffiFlag="MuidSA_dRUp",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=False)
            thistoolMuidSA_dRUp.MatchToMuidSA = True
            thistoolMuidSA_dRUp.ApplyScaleFactors = False
            thistoolMuidSA_dRUp.IsNominal=False
            thistoolMuidSA_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolMuidSA_dRUp]

        thistoolMuGirl = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_MuidGirl"%name_suffix,
                                                                        EffiFlag="MuGirl",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=True)
        thistoolMuGirl.MatchToMuGirl = True
        thistoolMuGirl.ApplyScaleFactors = False
        thistoolMuGirl.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolMuGirl]
        if doDRSys:
            thistoolMuGirl_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_MuidGirl_dRDown"%name_suffix,
                                                                                   EffiFlag="MuGirl_dRDown",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=False)
            thistoolMuGirl_dRDown.MatchToMuGirl = True
            thistoolMuGirl_dRDown.ApplyScaleFactors = False
            thistoolMuGirl_dRDown.IsNominal=False
            thistoolMuGirl_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolMuGirl_dRDown]

            thistoolMuGirl_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="DYmumuTPEfficiencyTool_%s_MuidGirl_dRUp"%name_suffix,
                                                                                 EffiFlag="MuGirl_dRUp",MatchCont=MuonContainerToUse,doClosure=doClosure,IsNominal=False)
            thistoolMuGirl_dRUp.MatchToMuGirl = True
            thistoolMuGirl_dRUp.IsNominal=False
            thistoolMuGirl_dRUp.ApplyScaleFactors = False
            thistoolMuGirl_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolMuGirl_dRUp]
            
        
    if writeProbeIso:
        
        isotool_Loose = CommonMuonTPConfig.AddIsolTPEfficiencyTool(name="DYmumuTPIsoEfficiencyTool_Reco_%s_Loose"%name_suffix,EffiFlag="IsoLoose",doClosure=doClosure,IsNominal=True)
        isotool_Loose.IsoSelectionTool = CommonMuonTPConfig.AddIsolationSelectionTool("Loose")
        isotool_Loose.IsolScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("LooseIso",doClosure=doClosure)
        MatchingTools += [isotool_Loose]

        isotool_Tight = CommonMuonTPConfig.AddIsolTPEfficiencyTool(name="DYmumuTPIsoEfficiencyTool_Reco_%s_Tight"%name_suffix,EffiFlag="IsoTight",doClosure=doClosure,IsNominal=True)
        isotool_Tight.IsoSelectionTool = CommonMuonTPConfig.AddIsolationSelectionTool("Tight")
        isotool_Tight.IsolScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("TightIso",doClosure=doClosure)
        MatchingTools += [isotool_Tight]

        isotool_Gradient = CommonMuonTPConfig.AddIsolTPEfficiencyTool(name="DYmumuTPIsoEfficiencyTool_Reco_%s_Gradient"%name_suffix,EffiFlag="IsoGradient",doClosure=doClosure,IsNominal=True)
        isotool_Gradient.IsoSelectionTool = CommonMuonTPConfig.AddIsolationSelectionTool("Gradient")
        isotool_Gradient.IsolScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("GradientIso",doClosure=doClosure)
        MatchingTools += [isotool_Gradient]

        isotool_GradientLoose = CommonMuonTPConfig.AddIsolTPEfficiencyTool(name="DYmumuTPIsoEfficiencyTool_Reco_%s_GradientLoose"%name_suffix,EffiFlag="IsoGradientLoose",doClosure=doClosure,IsNominal=True)
        isotool_GradientLoose.IsoSelectionTool = CommonMuonTPConfig.AddIsolationSelectionTool("GradientLoose")
        isotool_GradientLoose.IsolScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("GradientLooseIso",doClosure=doClosure)
        MatchingTools += [isotool_GradientLoose]

        isotool_FixedCutLoose = CommonMuonTPConfig.AddIsolTPEfficiencyTool(name="DYmumuTPIsoEfficiencyTool_Reco_%s_FixedCutLoose"%name_suffix,EffiFlag="IsoFixedCutLoose",doClosure=doClosure,IsNominal=True)
        isotool_FixedCutLoose.IsoSelectionTool = CommonMuonTPConfig.AddIsolationSelectionTool("FixedCutLoose")
        isotool_FixedCutLoose.IsolScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("FixedCutLooseIso",doClosure=doClosure)
        MatchingTools += [isotool_FixedCutLoose]

        isotool_FixedCutTightTrackOnly = CommonMuonTPConfig.AddIsolTPEfficiencyTool(name="DYmumuTPIsoEfficiencyTool_Reco_%s_FixedCutTightTrackOnly"%name_suffix,EffiFlag="IsoFixedCutTightTrackOnly",doClosure=doClosure,IsNominal=True)
        isotool_FixedCutTightTrackOnly.IsoSelectionTool = CommonMuonTPConfig.AddIsolationSelectionTool("FixedCutTightTrackOnly")
        isotool_FixedCutTightTrackOnly.IsolScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("FixedCutTightTrackOnlyIso",doClosure=doClosure)
        MatchingTools += [isotool_FixedCutTightTrackOnly]
    ntuples = []
    if writeNtuple:
        ntuples.append(CommonMuonTPConfig.AddTreeTool(name="DYmumuTPTreeTool_%s"%name_suffix, 
                                                      EffiFlag="Trees",WriteSFInfo=doClosure,
                                                      IsRunOnDAOD=IsRunOnDAOD))
    for ntuple in ntuples:
        ntuple.AddExtendedIsolation = True
    # create the TP tool itself
    TheTPTool = CommonMuonTPConfig.AddMuonTPTool(name = "DYmumuTPTool_%s"%name_suffix,
                                                 EffiFlag=name_suffix)
    TheTPTool.MuonTPSelectionTools = SelectionTools
    TheTPTool.PlottingTools = PlotTools
    TheTPTool.MuonTPEfficiencyTools = MatchingTools
    TheTPTool.TreeTools = ntuples
    theAlg.MuonTPTools += [TheTPTool]


def AddDYmumuTPRecoAnalysis(doEtaSlices=False,writeNtuple=False,doClosure=False,
                            doDRSys=False, doValid=False,DoProbeMatchPlots=True,
                            writeProbeIso=True,ProduceEfficiencies=False,
                            IsRunOnDAOD=False):


    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.AppMgr import ToolSvc
    job = AlgSequence()

    MuonContainerToUse = CommonMuonTPConfig.GetRightMuonContainer()

    ##########################################################################################
    # Add the Zmm TP algorithm using ID tracks as probes and matching to muons
    AddConfiguredDYmumuTPAlg(name_suffix = "IDProbes",
                            ProbeKind = "ID",
                            MatchContainer = MuonContainerToUse,
                            doID = False, doDetailedID=False, doCB = False, doLoose=True, doMedium = True,
                            doTight=True, doStandalone = False, doCaloTag = True, doIndividualAuthors = doValid,
                            doEtaSlices=doEtaSlices, writeNtuple = writeNtuple,doClosure=doClosure, doDRSys = doDRSys,
                            DoProbeMatchPlots=DoProbeMatchPlots,writeProbeIso=writeProbeIso,
                            ProduceEfficiencies=ProduceEfficiencies,
                            IsRunOnDAOD=IsRunOnDAOD)


    #########################################################################################
    #Add the Zmm TP algorithm using CaloTag muons as probes and matching to muons
    AddConfiguredDYmumuTPAlg(name_suffix = "CaloProbes",
                            ProbeKind = "CaloTag",
                            MatchContainer = MuonContainerToUse,
                            doID = False, doDetailedID=False, doCB = False, doLoose=True, doMedium = True,
                            doTight=True, doHighPt=True,
                            doStandalone = False, doCaloTag = False, doIndividualAuthors = doValid,
                            doEtaSlices=doEtaSlices, writeNtuple = writeNtuple,doClosure=doClosure, doDRSys = doDRSys,
                            DoProbeMatchPlots=DoProbeMatchPlots,writeProbeIso=writeProbeIso,
                            ProduceEfficiencies=ProduceEfficiencies,
                            IsRunOnDAOD=IsRunOnDAOD)


    ##########################################################################################
    # Add the Zmm TP algorithm using MS muons as probes and matching to ID tracks
   # AddConfiguredDYmumuTPAlg(name_suffix = "MSTrackPartProbes_ID",
   #                         ProbeKind = "MSTrackPart",
   #                         MatchContainer = "InDetTrackParticles",
   #                         doID = True, doCB = False, doLoose=False, doMedium = False,
   #                         doTight=False, doHighPt=True,
   #                         doStandalone = False, doCaloTag = False,
   #                         doEtaSlices=doEtaSlices, writeNtuple = writeNtuple,doClosure=doClosure, doDRSys = doDRSys,
                            #DoProbeMatchPlots=DoProbeMatchPlots,writeProbeIso=writeProbeIso,
                            #ProduceEfficiencies=ProduceEfficiencies,
#                             IsRunOnDAOD=IsRunOnDAOD)


    ##########################################################################################
    # Add the Zmm TP algorithm using MS muons as probes and matching to ID tracks
    AddConfiguredDYmumuTPAlg(name_suffix = "MSProbes_ID",
                            ProbeKind = "MS",
                            MatchContainer = "InDetTrackParticles",
                            doID = True, doDetailedID=True,  doCB = False, doLoose=False, doMedium = False,
                            doTight=False, doHighPt=False,
                            doStandalone = False, doCaloTag = False, doIndividualAuthors = False,
                            doEtaSlices=doEtaSlices, writeNtuple = writeNtuple,doClosure=doClosure, doDRSys = doDRSys,
                            DoProbeMatchPlots=DoProbeMatchPlots,writeProbeIso=writeProbeIso,
                            ProduceEfficiencies=ProduceEfficiencies,
                            IsRunOnDAOD=IsRunOnDAOD)


    ##########################################################################################
    # Add the Zmm TP algorithm using MS muons as probes and matching to CaloTag muons
    AddConfiguredDYmumuTPAlg(name_suffix = "MSProbes_Muon",
                            ProbeKind = "MS",
                            MatchContainer = MuonContainerToUse,
                            doID = False, doDetailedID=False,  doCB = False, doLoose=False, doMedium = False,
                            doTight=False, doHighPt=False,
                            doStandalone = False, doCaloTag = True, doIndividualAuthors = False,
                            doEtaSlices=doEtaSlices, writeNtuple = writeNtuple,doClosure=doClosure, doDRSys = doDRSys,
                            DoProbeMatchPlots=DoProbeMatchPlots,writeProbeIso=writeProbeIso,
                            ProduceEfficiencies=ProduceEfficiencies,
                            IsRunOnDAOD=IsRunOnDAOD)
    
    
    # Only add truth probes if we are running on MC!
    from AthenaCommon.GlobalFlags import globalflags
    if not globalflags.DataSource()=='data':
        ##########################################################################################
        # Add the Zmm TP algorithm using Truth probes for Muons
        AddConfiguredDYmumuTPAlg(name_suffix = "TruthProbes_Muon",
                                ProbeKind = "Truth",
                                MatchContainer = MuonContainerToUse,
                                doID = False, doDetailedID=False,  doCB = False, doLoose=True, doMedium = True,
                                doTight=True, doHighPt=True,
                                doStandalone = False, doCaloTag = True, doIndividualAuthors = doValid,
                                doEtaSlices=doEtaSlices, writeNtuple = writeNtuple,doClosure=False, doDRSys = False,
                                DoProbeMatchPlots=DoProbeMatchPlots,writeProbeIso=False,
                                ProduceEfficiencies=ProduceEfficiencies,
                                IsRunOnDAOD=IsRunOnDAOD)

        ##########################################################################################
        # Add the Zmm TP algorithm using Truth probes for Tracks
        AddConfiguredDYmumuTPAlg(name_suffix = "TruthProbes_ID",
                                ProbeKind = "Truth",
                                MatchContainer = "InDetTrackParticles",
                                doID = True, doDetailedID=True,  doCB = False, doLoose=False, doMedium = False,
                                doTight=False, doHighPt=False,
                                doStandalone = False, doCaloTag = False, doIndividualAuthors = False,
                                doEtaSlices=doEtaSlices, writeNtuple = writeNtuple,doClosure=False, doDRSys = False,
                                DoProbeMatchPlots=DoProbeMatchPlots,writeProbeIso=False,
                                ProduceEfficiencies=ProduceEfficiencies,
                            IsRunOnDAOD=IsRunOnDAOD)
