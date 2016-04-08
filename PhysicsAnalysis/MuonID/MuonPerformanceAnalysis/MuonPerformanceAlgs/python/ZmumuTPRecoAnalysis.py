# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from MuonPerformanceAlgs import CommonMuonTPConfig 
################################################################
# Add a version of the Z TP algorithm and all associated tools
################################################################
def AddConfiguredZmumuTPAlg(name_suffix="myProbes",
                            ProbeKind = "CaloTag", # options: "CaloTag","MS", "Truth", "ID"
                            MatchContainer="Muons",
                            doID=False,
                            doCB = True, doLoose=True,
                            doMedium=True,doTight=True,
                            doHighPt=True,
                            doStandalone=False, doCaloTag=False,
                            doIndividualAuthors=False,
                            doEtaSlices=False, writeNtuple=False,
                            doClosure = False, doDRSys = False,
                            DoProbeMatchPlots=True,
                            ProduceEfficiencies=False):


    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.AppMgr import ToolSvc
    job = AlgSequence()

    MuonContainerToUse = "Muons"
    #if hasattr(job, "MuonQualityUpdater"):
        #MuonContainerToUse = "UpdatedMuons"

    ProbeContainer=MuonContainerToUse

    if ProbeKind == "MSTrackPart":
        ProbeContainer = "ExtrapolatedMuonTrackParticles"
        ProbeKind = "MS"
        
    if ProbeKind == "ID":
        ProbeContainer = "InDetTrackParticles"
    if ProbeKind == "Truth":
        ProbeContainer = "MuonTruthParticles"

    theAlg = CommonMuonTPConfig.AddTagProbeAlg(name="ZmumuTPAlg_%s"%name_suffix,
                           ProbeCont=ProbeContainer,
                           MatchCont=MatchContainer)
    theAlg.TopLevelFolderName = "ZmumuTPReco"

    #AddMuonSelectionTool()


    # add a plotting tool
    #PlotTool = AddPlottingTool(name="ZmumuTPPlottingTool_%s"%name_suffix,EffiFlag=name_suffix)

    # add four TP selection tools (iso and antiiso OC and SC)
    SelectionTools = []
    SelectionTools += [CommonMuonTPConfig.AddZmumuTPSelectionTool(name="ZmumuTPSelectionTool_OC_%s"%name_suffix,
                                                                  EffiFlag="%s_OC"%name_suffix, 
                                                                  ProbeType = ProbeKind)]
    SelectionTools += [CommonMuonTPConfig.AddZmumuTPSelectionTool(name="ZmumuTPSelectionTool_SC_%s"%name_suffix,
                                                                  EffiFlag="%s_SC"%name_suffix, 
                                                                  ProbeType = ProbeKind, 
                                                                  SameSign=True)]
    SelectionTools += [CommonMuonTPConfig.AddZmumuTPSelectionTool(name="ZmumuTPSelectionTool_OCAntiIso_%s"%name_suffix,
                                                                  EffiFlag="%s_OCAntiIso"%name_suffix, 
                                                                  ProbeType = ProbeKind, 
                                                                  AntiIso=True)]
    SelectionTools += [CommonMuonTPConfig.AddZmumuTPSelectionTool(name="ZmumuTPSelectionTool_SCAntiIso_%s"%name_suffix,
                                                                  EffiFlag="%s_SCAntiIso"%name_suffix, 
                                                                  ProbeType = ProbeKind, 
                                                                  SameSign=True, 
                                                                  AntiIso=True)]

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
            PlotTool = CommonMuonTPConfig.AddPlottingTool(name="ZmumuTPPlottingTool_%s_%s"%(name_suffix,etaslice),
                                                          EffiFlag=name_suffix+"_"+etaslice,
                                                          applySF = False,
                                                          DoProbeMatchPlots=DoProbeMatchPlots,
                                                          ProduceEfficiencies=ProduceEfficiencies)
            PlotTool.ProbeAbsEtaMin = interval[0]
            PlotTool.ProbeAbsEtaMax = interval[1]
            PlotTools.append(PlotTool)
            if doClosure:
                PlotToolCL = CommonMuonTPConfig.AddPlottingTool(name="ZmumuTPPlottingToolClosure_%s_%s"%(name_suffix,etaslice),
                                                                EffiFlag=name_suffix+"_"+etaslice+"_Closure",
                                                                applySF = True,
                                                                DoProbeMatchPlots=DoProbeMatchPlots,
                                                                ProduceEfficiencies=ProduceEfficiencies)
                PlotToolCL.ProbeAbsEtaMin = interval[0]
                PlotToolCL.ProbeAbsEtaMax = interval[1]
                PlotTools.append(PlotToolCL)
                
    else:
        PlotTools = [CommonMuonTPConfig.AddPlottingTool(name="ZmumuTPPlottingTool_%s_All"%name_suffix,
                                                        EffiFlag=name_suffix+"_All",
                                                        applySF = False,
                                                        DoProbeMatchPlots=DoProbeMatchPlots,
                                                        ProduceEfficiencies=ProduceEfficiencies)]
        if doClosure:
            PlotTools.append(CommonMuonTPConfig.AddPlottingTool(name="ZmumuTPPlottingToolClosure_%s_All"%name_suffix,
                                                                EffiFlag=name_suffix+"_All_Closure",
                                                                applySF = True,
                                                                DoProbeMatchPlots=DoProbeMatchPlots,
                                                                ProduceEfficiencies=ProduceEfficiencies))
        
    # Config the plot tools
    for thetool in PlotTools:
        thetool.doZmumuKinematicPlots = False
        thetool.doFineEtaPhiPlots = True
        thetool.doDetectorRegionPlots = True
    
    # add any desired matching tools
    MATCH_DR = 0.05

    
    MatchingTools = []
    if doCB:
        thistoolCB = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_CB"%name_suffix,
                                                                    EffiFlag="CombinedMuons")
        thistoolCB.MatchToCB = True
        thistoolCB.ScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Medium")
        thistoolCB.ApplyScaleFactors=doClosure
        thistoolCB.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolCB]
        if doDRSys:
            thistoolCB_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_CB_dRDown"%name_suffix,
                                                                               EffiFlag="CombinedMuons_dRDown")
            thistoolCB_dRDown.IsNominal=False                            
            thistoolCB_dRDown.MatchToCB = True
            thistoolCB_dRDown.ScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Medium")
            #thistoolCB_dRDown.ApplyScaleFactors = True
            thistoolCB_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolCB_dRDown]
            
            thistoolCB_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_CB_dRUp"%name_suffix,
                                                                             EffiFlag="CombinedMuons_dRUp")
            thistoolCB_dRUp.MatchToCB = True
            thistoolCB_dRUp.IsNominal=False
            thistoolCB_dRUp.ScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Medium")
            #thistoolCB_dRUp.ApplyScaleFactors = True
            thistoolCB_dRUp.MaximumDrCut = 2. * MATCH_DR
            MatchingTools += [thistoolCB_dRUp]

    if doLoose:
        thistoolLoose = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_Loose"%name_suffix,
                                                                       EffiFlag="LooseMuons")
        thistoolLoose.MatchToLoose = True
        thistoolLoose.ScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Loose")
        thistoolLoose.ApplyScaleFactors=doClosure
        #thistoolLoose.ApplyScaleFactors = True
        thistoolLoose.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolLoose]
        if doDRSys:
            thistoolLoose_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_Loose_dRDown"%name_suffix,
                                                                                  EffiFlag="LooseMuons_dRDown")
            thistoolLoose_dRDown.MatchToLoose = True
            thistoolLoose_dRDown.IsNominal=False
            thistoolLoose_dRDown.ScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Loose")
            #thistoolLoose_dRDown.ApplyScaleFactors = True
            thistoolLoose_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolLoose_dRDown]
            
            thistoolLoose_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_Loose_dRUp"%name_suffix,
                                                                                EffiFlag="LooseMuons_dRUp")
            thistoolLoose_dRUp.MatchToLoose = True
            thistoolLoose_dRUp.IsNominal=False
            thistoolLoose_dRUp.ScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Loose")
            #thistoolLoose_dRUp.ApplyScaleFactors = True
            thistoolLoose_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolLoose_dRUp]
            
        # also run a version without CaloTag
        thistoolLoose_noCaloTag = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_Loose_noCaloTag"%name_suffix,
                                                                                 EffiFlag="LooseMuons_noCaloTag")
        thistoolLoose_noCaloTag.MatchToLoose_noCaloTag = True
        thistoolLoose_noCaloTag.ScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Loose")
        thistoolLoose_noCaloTag.ApplyScaleFactors=doClosure
        #thistoolLoose_noCaloTag.ApplyScaleFactors = True
        thistoolLoose_noCaloTag.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolLoose_noCaloTag]
        if doDRSys:
            thistoolLoose_noCaloTag_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_Loose_noCaloTag_dRDown"%name_suffix,
                                                                                            EffiFlag="LooseMuons_noCaloTag_dRDown")
            thistoolLoose_noCaloTag_dRDown.MatchToLoose_noCaloTag = True
            thistoolLoose_noCaloTag_dRDown.ScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Loose")
            #thistoolLoose_noCaloTag_dRDown.ApplyScaleFactors = True
            thistoolLoose_noCaloTag_dRDown.IsNominal=False
            thistoolLoose_noCaloTag_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolLoose_noCaloTag_dRDown]
            
            thistoolLoose_noCaloTag_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_Loose_noCaloTag_dRUp"%name_suffix,
                                                                                          EffiFlag="LooseMuons_noCaloTag_dRUp")
            thistoolLoose_noCaloTag_dRUp.MatchToLoose_noCaloTag = True
            thistoolLoose_noCaloTag_dRUp.ScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Loose")
            #thistoolLoose_noCaloTag_dRUp.ApplyScaleFactors = True
            thistoolLoose_noCaloTag_dRUp.IsNominal=False
            thistoolLoose_noCaloTag_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolLoose_noCaloTag_dRUp]
        
    if doMedium:
        thistoolMed = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_Medium"%name_suffix,
                                                                     EffiFlag="MediumMuons")
        thistoolMed.MatchToMedium = True
        thistoolMed.ScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Medium")
        thistoolMed.ApplyScaleFactors=doClosure
        thistoolMed.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolMed]
        if doDRSys:
            thistoolMed_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_Medium_dRDown"%name_suffix,
                                                                                EffiFlag="MediumMuons_dRDown")
            thistoolMed_dRDown.MatchToMedium = True
            thistoolMed_dRDown.IsNominal=False
            thistoolMed_dRDown.ScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Medium")
            #thistoolMed_dRDown.ApplyScaleFactors = True
            thistoolMed_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolMed_dRDown]

            thistoolMed_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_Medium_dRUp"%name_suffix,
                                                                              EffiFlag="MediumMuons_dRUp")
            thistoolMed_dRUp.MatchToMedium = True
            thistoolMed_dRUp.IsNominal=False
            thistoolMed_dRUp.ScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Medium")
            #thistoolMed_dRUp.ApplyScaleFactors = True
            thistoolMed_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolMed_dRUp]
            
        thistoolMed_PtrMatch = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_PtrMatch_%s_Medium"%name_suffix,
                                                                              EffiFlag="MediumMuons_PtrMatching")
        thistoolMed_PtrMatch.MatchToMedium = True
        thistoolMed_PtrMatch.ScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Medium")
        thistoolMed_PtrMatch.ApplyScaleFactors=doClosure
        #thistoolMed_PtrMatch.ApplyScaleFactors = True
        thistoolMed_PtrMatch.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolMed_PtrMatch]

        
    if doTight:
        thistoolTight = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_Tight"%name_suffix,
                                                                       EffiFlag="TightMuons")
        thistoolTight.MatchToTight = True
        thistoolTight.ScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Tight")
        thistoolTight.ApplyScaleFactors=doClosure
        #thistoolTight.ApplyScaleFactors = True
        thistoolTight.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolTight]
        if doDRSys:
            thistoolTight_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_Tight_dRDown"%name_suffix,
                                                                                  EffiFlag="TightMuons_dRDown")
            thistoolTight_dRDown.MatchToTight = True
            thistoolTight_dRDown.IsNominal=False
            thistoolTight_dRDown.ScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Tight")
            #thistoolTight_dRDown.ApplyScaleFactors = True
            thistoolTight_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolTight_dRDown]
            
            thistoolTight_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_Tight_dRUp"%name_suffix,
                                                                                EffiFlag="TightMuons_dRUp")
            thistoolTight_dRUp.MatchToTight = True
            thistoolTight_dRUp.IsNominal=False
            thistoolTight_dRUp.ScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Tight")
            #thistoolTight_dRUp.ApplyScaleFactors = True
            thistoolTight_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolTight_dRUp]

        
    if doHighPt:
        thistoolHighPt = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_HighPt"%name_suffix,
                                                                       EffiFlag="HighPtMuons")
        thistoolHighPt.MatchToHighPt = True
        thistoolHighPt.ScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Tight")  # UPDATE when SF available for high pt
        thistoolHighPt.ApplyScaleFactors=doClosure
        #thistoolHighPt.ApplyScaleFactors = True
        thistoolHighPt.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolHighPt]
        if doDRSys:
            thistoolHighPt_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_HighPt_dRDown"%name_suffix,
                                                                                  EffiFlag="HighPtMuons_dRDown")
            thistoolHighPt_dRDown.MatchToHighPt = True
            thistoolHighPt_dRDown.IsNominal=False
            thistoolHighPt_dRDown.ScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("HighPt")  # UPDATE when SF available for high pt
            #thistoolHighPt_dRDown.ApplyScaleFactors = True
            thistoolHighPt_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolHighPt_dRDown]
            
            thistoolHighPt_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_HighPt_dRUp"%name_suffix,
                                                                                EffiFlag="HighPtMuons_dRUp")
            thistoolHighPt_dRUp.MatchToHighPt = True
            thistoolHighPt_dRUp.IsNominal=False
            thistoolHighPt_dRUp.ScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Tight")  # UPDATE when SF available for high pt
            #thistoolHighPt_dRUp.ApplyScaleFactors = True
            thistoolHighPt_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolHighPt_dRUp]
            

    if doStandalone:
        thistoolSA = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_SA"%name_suffix,
                                                                    EffiFlag="StandaloneMuons")
        thistoolSA.MatchToAnyMS = True
        thistoolSA.IDhitCut = False
        thistoolSA.ApplyScaleFactors = doClosure
        thistoolSA.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolSA]
        if doDRSys:
            thistoolSA_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_SA_dRDown"%name_suffix,
                                                                               EffiFlag="StandaloneMuons_dRDown")
            thistoolSA_dRDown.MatchToAnyMS = True
            thistoolSA_dRDown.IDhitCut = False
            thistoolSA_dRDown.IsNominal=False
            thistoolSA_dRDown.ApplyScaleFactors = doClosure
            thistoolSA_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolSA_dRDown]

            thistoolSA_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_SA_dRUp"%name_suffix,
                                                                             EffiFlag="StandaloneMuons_dRUp")
            thistoolSA_dRUp.MatchToAnyMS = True
            thistoolSA_dRUp.IDhitCut = False
            thistoolSA_dRUp.IsNominal=False
            thistoolSA_dRUp.ApplyScaleFactors = doClosure
            thistoolSA_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolSA_dRUp]

    if doCaloTag:
        thistoolCT = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_CT"%name_suffix,
                                                                    EffiFlag="CaloTaggedMuons")
        thistoolCT.MatchToCaloTag = True
        thistoolCT.ScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Loose")
        thistoolCT.ApplyScaleFactors=doClosure
        #thistoolCT.ApplyScaleFactors = True
        thistoolCT.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolCT]
        if doDRSys:
            thistoolCT_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_CT_dRDown"%name_suffix,
                                                                               EffiFlag="CaloTaggedMuons_dRDown")
            thistoolCT_dRDown.MatchToCaloTag = True
            thistoolCT_dRDown.ScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Loose")
            #thistoolCT_dRDown.ApplyScaleFactors = True
            thistoolCT_dRDown.IsNominal=False
            thistoolCT_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolCT_dRDown]

            thistoolCT_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_CT_dRUp"%name_suffix,
                                                                             EffiFlag="CaloTaggedMuons_dRUp")
            thistoolCT_dRUp.MatchToCaloTag = True
            thistoolCT_dRUp.ScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Loose")
            #thistoolCT_dRUp.ApplyScaleFactors = True
            thistoolCT_dRUp.IsNominal=False
            thistoolCT_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolCT_dRUp]

    if doID:
        thistoolID = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_ID"%name_suffix,
                                                                    EffiFlag="IDTracks")
        thistoolID.MatchToID = True
        thistoolID.ApplyScaleFactors = doClosure
        thistoolID.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolID]
        if doDRSys:
            thistoolID_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_ID_dRUp"%name_suffix,
                                                                             EffiFlag="IDTracks_dRUp")
            thistoolID_dRUp.MatchToID = True
            thistoolID_dRUp.ApplyScaleFactors = doClosure
            thistoolID_dRUp.MaximumDrCut = 2 * MATCH_DR
            thistoolID_dRUp.IsNominal=False
            MatchingTools += [thistoolID_dRUp]

            thistoolID_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_ID_dRDown"%name_suffix,
                                                                               EffiFlag="IDTracks_dRDown")
            thistoolID_dRDown.MatchToID = True
            thistoolID_dRDown.ApplyScaleFactors = doClosure
            thistoolID_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            thistoolID_dRDown.IsNominal=False
            MatchingTools += [thistoolID_dRDown]

        #thistoolID_noMCPcuts = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_ID_noMCP"%name_suffix,EffiFlag="IDTracks_noMCP")
        #thistoolID_noMCPcuts.MatchToID = True
        #thistoolID_noMCPcuts.IDhitCut = False
        #thistoolID_noMCPcuts.ApplyScaleFactors = doClosure
        #thistoolID_noMCPcuts.MaximumDrCut = MATCH_DR
        #MatchingTools += [thistoolID_noMCPcuts]
        #if doDRSys:
            #thistoolID_noMCPcuts_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_ID_noMCP_dRUp"%name_suffix,EffiFlag="IDTracks_noMCP_dRUp")
            #thistoolID_noMCPcuts_dRUp.MatchToID = True
            #thistoolID_noMCPcuts_dRUp.ApplyScaleFactors = doClosure
            #thistoolID_noMCPcuts_dRUp.MaximumDrCut = 2 * MATCH_DR
            #MatchingTools += [thistoolID_noMCPcuts_dRUp]

            #thistoolID_noMCPcuts_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_ID_noMCP_dRDown"%name_suffix,EffiFlag="IDTracks_noMCP_dRDown")
            #thistoolID_noMCPcuts_dRDown.MatchToID = True
            #thistoolID_noMCPcuts_dRDown.ApplyScaleFactors = doClosure
            #thistoolID_noMCPcuts_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            #MatchingTools += [thistoolID_noMCPcuts_dRDown]
            


    if doIndividualAuthors:
        thistoolMuidCB = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuidCB"%name_suffix,
                                                                        EffiFlag="MuidCB")
        thistoolMuidCB.MatchToMuidCB = True
        thistoolMuidCB.ApplyScaleFactors = doClosure
        thistoolMuidCB.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolMuidCB]
        if doDRSys:
            thistoolMuidCB_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuidCB_dRDown"%name_suffix,
                                                                                   EffiFlag="MuidCB_dRDown")
            thistoolMuidCB_dRDown.MatchToMuidCB = True
            thistoolMuidCB_dRDown.ApplyScaleFactors = doClosure
            thistoolMuidCB_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            thistoolMuidCB_dRDown.IsNominal=False
            MatchingTools += [thistoolMuidCB_dRDown]

            thistoolMuidCB_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuidCB_dRUp"%name_suffix,
                                                                                 EffiFlag="MuidCB_dRUp")
            thistoolMuidCB_dRUp.MatchToMuidCB = True
            thistoolMuidCB_dRUp.ApplyScaleFactors = doClosure
            thistoolMuidCB_dRUp.MaximumDrCut = 2 * MATCH_DR
            thistoolMuidCB_dRUp.IsNominal=False
            MatchingTools += [thistoolMuidCB_dRUp]

        thistoolSTACO = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_STACO"%name_suffix,
                                                                       EffiFlag="STACO")
        thistoolSTACO.MatchToSTACO = True
        thistoolSTACO.ApplyScaleFactors = doClosure
        thistoolSTACO.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolSTACO]
        if doDRSys:
            thistoolSTACO_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_STACO_dRDown"%name_suffix,
                                                                                  EffiFlag="STACO_dRDown")
            thistoolSTACO_dRDown.MatchToSTACO = True
            thistoolSTACO_dRDown.ApplyScaleFactors = doClosure
            thistoolSTACO_dRDown.IsNominal=False
            thistoolSTACO_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolSTACO_dRDown]

            thistoolSTACO_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_STACO_dRUp"%name_suffix,
                                                                                EffiFlag="STACO_dRUp")
            thistoolSTACO_dRUp.MatchToSTACO = True
            thistoolSTACO_dRUp.ApplyScaleFactors = doClosure
            thistoolSTACO_dRUp.IsNominal=False
            thistoolSTACO_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolSTACO_dRUp]

        thistoolMuTag = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuTag"%name_suffix,
                                                                       EffiFlag="MuTag")
        thistoolMuTag.MatchToMuTag = True
        thistoolMuTag.ApplyScaleFactors = doClosure
        thistoolMuTag.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolMuTag]
        if doDRSys:
            thistoolMuTag_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuTag_dRDown"%name_suffix,
                                                                                  EffiFlag="MuTag_dRDown")
            thistoolMuTag_dRDown.MatchToMuTag = True
            thistoolMuTag_dRDown.ApplyScaleFactors = doClosure
            thistoolMuTag_dRDown.IsNominal=False
            thistoolMuTag_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolMuTag_dRDown]

            thistoolMuTag_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuTag_dRUp"%name_suffix,
                                                                                EffiFlag="MuTag_dRUp")
            thistoolMuTag_dRUp.MatchToMuTag = True
            thistoolMuTag_dRUp.ApplyScaleFactors = doClosure
            thistoolMuTag_dRUp.IsNominal=False
            thistoolMuTag_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolMuTag_dRUp]

        thistoolMuTagIMO = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuTagIMO"%name_suffix,
                                                                          EffiFlag="MuTagIMO")
        thistoolMuTagIMO.MatchToMuTagIMO = True
        thistoolMuTagIMO.ApplyScaleFactors = doClosure
        thistoolMuTagIMO.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolMuTagIMO]
        if doDRSys:
            thistoolMuTagIMO_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuTagIMO_dRDown"%name_suffix,
                                                                                     EffiFlag="MuTagIMO_dRDown")
            thistoolMuTagIMO_dRDown.MatchToMuTagIMO = True
            thistoolMuTagIMO_dRDown.ApplyScaleFactors = doClosure
            thistoolMuTagIMO_dRDown.IsNominal=False
            thistoolMuTagIMO_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolMuTagIMO_dRDown]

            thistoolMuTagIMO_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuTagIMO_dRUp"%name_suffix,
                                                                                   EffiFlag="MuTagIMO_dRUp")
            thistoolMuTagIMO_dRUp.MatchToMuTagIMO = True
            thistoolMuTagIMO_dRUp.ApplyScaleFactors = doClosure
            thistoolMuTagIMO_dRUp.IsNominal=False
            thistoolMuTagIMO_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolMuTagIMO_dRUp]

        thistoolMuidSA = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuidSA"%name_suffix,
                                                                        EffiFlag="MuidSA")
        thistoolMuidSA.MatchToMuidSA = True
        thistoolMuidSA.ApplyScaleFactors = doClosure
        thistoolMuidSA.IDhitCut = False
        thistoolMuidSA.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolMuidSA]
        if doDRSys:
            thistoolMuidSA_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuidSA_dRDown"%name_suffix,
                                                                                   EffiFlag="MuidSA_dRDown")
            thistoolMuidSA_dRDown.MatchToMuidSA = True
            thistoolMuidSA_dRDown.ApplyScaleFactors = doClosure
            thistoolMuidSA_dRDown.IsNominal=False
            thistoolMuidSA_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolMuidSA_dRDown]

            thistoolMuidSA_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuidSA_dRUp"%name_suffix,
                                                                                 EffiFlag="MuidSA_dRUp")
            thistoolMuidSA_dRUp.MatchToMuidSA = True
            thistoolMuidSA_dRUp.ApplyScaleFactors = doClosure
            thistoolMuidSA_dRUp.IsNominal=False
            thistoolMuidSA_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolMuidSA_dRUp]

        thistoolMuGirl = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuidGirl"%name_suffix,
                                                                        EffiFlag="MuGirl")
        thistoolMuGirl.MatchToMuGirl = True
        thistoolMuGirl.ApplyScaleFactors = doClosure
        thistoolMuGirl.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolMuGirl]
        if doDRSys:
            thistoolMuGirl_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuidGirl_dRDown"%name_suffix,
                                                                                   EffiFlag="MuGirl_dRDown")
            thistoolMuGirl_dRDown.MatchToMuGirl = True
            thistoolMuGirl_dRDown.ApplyScaleFactors = doClosure
            thistoolMuGirl_dRDown.IsNominal=False
            thistoolMuGirl_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolMuGirl_dRDown]

            thistoolMuGirl_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="ZmumuTPEfficiencyTool_%s_MuidGirl_dRUp"%name_suffix,
                                                                                 EffiFlag="MuGirl_dRUp")
            thistoolMuGirl_dRUp.MatchToMuGirl = True
            thistoolMuGirl_dRUp.IsNominal=False
            thistoolMuGirl_dRUp.ApplyScaleFactors = doClosure
            thistoolMuGirl_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolMuGirl_dRUp]
            
    ntuples = []
    if writeNtuple:
        ntuples.append(CommonMuonTPConfig.AddTreeTool(name="ZmumuTPTreeTool_%s"%name_suffix, 
                                                      EffiFlag="Trees",WriteSFInfo=doClosure))

    # create the TP tool itself
    TheTPTool = CommonMuonTPConfig.AddMuonTPTool(name = "ZmumuTPTool_%s"%name_suffix,
                                                 EffiFlag=name_suffix)
    TheTPTool.MuonTPSelectionTools = SelectionTools
    TheTPTool.PlottingTools = PlotTools
    TheTPTool.MuonTPEfficiencyTools = MatchingTools
    TheTPTool.TreeTools = ntuples
    theAlg.MuonTPTools += [TheTPTool]


def AddZmumuTPRecoAnalysis(doEtaSlices=False,writeNtuple=False,doClosure=False,doDRSys=False, doValid=False,DoProbeMatchPlots=True,ProduceEfficiencies=False):


    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.AppMgr import ToolSvc
    job = AlgSequence()

    MuonContainerToUse = "Muons"
    #if hasattr(job, "MuonQualityUpdater"):
        #MuonContainerToUse = "UpdatedMuons"


    ###########################################################################################
    ## Add the Zmm TP algorithm using ID tracks as probes and matching to muons
    #AddConfiguredZmumuTPAlg(name_suffix = "IDProbes",
                            #ProbeKind = "ID",
                            #MatchContainer = MuonContainerToUse,
                            #doID = False, doCB = True, doLoose=True, doMedium = True,
                            #doTight=True, doStandalone = False, doCaloTag = True, doIndividualAuthors = doValid,
                            #doEtaSlices=doEtaSlices, writeNtuple = writeNtuple,doClosure=doClosure, doDRSys = doDRSys)


    #########################################################################################
    #Add the Zmm TP algorithm using CaloTag muons as probes and matching to muons
    AddConfiguredZmumuTPAlg(name_suffix = "CaloProbes",
                            ProbeKind = "CaloTag",
                            MatchContainer = MuonContainerToUse,
                            doID = False, doCB = True, doLoose=True, doMedium = True,
                            doTight=True, doHighPt=True,
                            doStandalone = False, doCaloTag = False, doIndividualAuthors = doValid,
                            doEtaSlices=doEtaSlices, writeNtuple = writeNtuple,doClosure=doClosure, doDRSys = doDRSys,
                            DoProbeMatchPlots=DoProbeMatchPlots,
                            ProduceEfficiencies=ProduceEfficiencies)


    ##########################################################################################
    # Add the Zmm TP algorithm using MS muons as probes and matching to ID tracks
   # AddConfiguredZmumuTPAlg(name_suffix = "MSTrackPartProbes_ID",
   #                         ProbeKind = "MSTrackPart",
   #                         MatchContainer = "InDetTrackParticles",
   #                         doID = True, doCB = False, doLoose=False, doMedium = False,
   #                         doTight=False, doHighPt=True,
   #                         doStandalone = False, doCaloTag = False,
   #                         doEtaSlices=doEtaSlices, writeNtuple = writeNtuple,doClosure=doClosure, doDRSys = doDRSys)


    ##########################################################################################
    # Add the Zmm TP algorithm using MS muons as probes and matching to ID tracks
    AddConfiguredZmumuTPAlg(name_suffix = "MSProbes_ID",
                            ProbeKind = "MS",
                            MatchContainer = "InDetTrackParticles",
                            doID = True, doCB = False, doLoose=False, doMedium = False,
                            doTight=False, doHighPt=False,
                            doStandalone = False, doCaloTag = False, doIndividualAuthors = False,
                            doEtaSlices=doEtaSlices, writeNtuple = writeNtuple,doClosure=doClosure, doDRSys = doDRSys,
                            DoProbeMatchPlots=DoProbeMatchPlots,
                            ProduceEfficiencies=ProduceEfficiencies)


    ##########################################################################################
    # Add the Zmm TP algorithm using MS muons as probes and matching to CaloTag muons
    AddConfiguredZmumuTPAlg(name_suffix = "MSProbes_Muon",
                            ProbeKind = "MS",
                            MatchContainer = MuonContainerToUse,
                            doID = False, doCB = False, doLoose=False, doMedium = False,
                            doTight=False, doHighPt=False,
                            doStandalone = False, doCaloTag = True, doIndividualAuthors = False,
                            doEtaSlices=doEtaSlices, writeNtuple = writeNtuple,doClosure=doClosure, doDRSys = doDRSys,
                            DoProbeMatchPlots=DoProbeMatchPlots,
                            ProduceEfficiencies=ProduceEfficiencies)
    
    
    # Only add truth probes if we are running on MC!
    from AthenaCommon.GlobalFlags import globalflags
    if not globalflags.DataSource()=='data':
        ##########################################################################################
        # Add the Zmm TP algorithm using Truth probes for Muons
        AddConfiguredZmumuTPAlg(name_suffix = "TruthProbes_Muon",
                                ProbeKind = "Truth",
                                MatchContainer = MuonContainerToUse,
                                doID = False, doCB = True, doLoose=True, doMedium = True,
                                doTight=True, doHighPt=True,
                                doStandalone = False, doCaloTag = True, doIndividualAuthors = doValid,
                                doEtaSlices=doEtaSlices, writeNtuple = writeNtuple,doClosure=False, doDRSys = False,
                                DoProbeMatchPlots=DoProbeMatchPlots,
                                ProduceEfficiencies=ProduceEfficiencies)

        ##########################################################################################
        # Add the Zmm TP algorithm using Truth probes for Tracks
        AddConfiguredZmumuTPAlg(name_suffix = "TruthProbes_ID",
                                ProbeKind = "Truth",
                                MatchContainer = "InDetTrackParticles",
                                doID = True, doCB = False, doLoose=False, doMedium = False,
                                doTight=False, doHighPt=False,
                                doStandalone = False, doCaloTag = False, doIndividualAuthors = False,
                                doEtaSlices=doEtaSlices, writeNtuple = writeNtuple,doClosure=False, doDRSys = False,
                                DoProbeMatchPlots=DoProbeMatchPlots,
                                ProduceEfficiencies=ProduceEfficiencies)