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
                           doIndividualAuthors=False,
                           writeNtuple=False,
                           useTrigger=True,
                           doDRSys = False,
                           doClosure=False,
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

    theAlg = CommonMuonTPConfig.AddTagProbeAlg(name="JPsiTPAlg_%s"%name_suffix,
                           ProbeCont=ProbeContainer,
                           MatchCont=MatchContainer)
    theAlg.TopLevelFolderName = "JPsiTPReco"

    #AddMuonSelectionTool()


    # add a plotting tool
    #PlotTool = AddPlottingTool(name="JPsiTPPlottingTool_%s"%name_suffix,EffiFlag=name_suffix)

    # add J/Psi TP selection tools
    SelectionTools = []
    SelectionTools += [CommonMuonTPConfig.AddJPsiTPSelectionTool(name="JPsiTPSelectionTool_OC_%s"%name_suffix,EffiFlag="%s_OC"%name_suffix, ProbeType = ProbeKind, UseTrigger = useTrigger)]


    PlotTools = []
    # add plotting tools in slices
    PlotTools = [CommonMuonTPConfig.AddPlottingTool(name="JPsiTPPlottingTool_%s_All"%name_suffix,EffiFlag=name_suffix+"_All",applySF = False,DoProbeMatchPlots=DoProbeMatchPlots,ProduceEfficiencies=ProduceEfficiencies)]
    for thetool in PlotTools:
        thetool.doJPsiKinematicPlots = True
        thetool.doJPsiDileptonPlots = True

    # add any desired matching tools

    MATCH_DR = 0.05
    
    MatchingTools = []

    if doID:
        thistoolID = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_ID"%name_suffix,EffiFlag="IDTracks")
        thistoolID.MatchToID = True
        thistoolID.ApplyScaleFactors = False
        thistoolID.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolID]
        
        if doDRSys:
            thistoolID_dRdown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_ID_dRdown"%name_suffix,EffiFlag="IDTracks_dRDown")
            thistoolID_dRdown.MatchToID = True
            thistoolID_dRdown.ApplyScaleFactors = False
            thistoolID_dRdown.IsNominal=False
            thistoolID_dRdown.MaximumDrCut = 0.5 *MATCH_DR
            MatchingTools += [thistoolID_dRdown]
            
            thistoolID_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_ID_dRUp"%name_suffix,EffiFlag="IDTracks_dRUp")
            thistoolID_dRUp.MatchToID = True
            thistoolID_dRUp.ApplyScaleFactors = False
            thistoolID_dRUp.IsNominal=False
            thistoolID_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolID_dRUp]
        
    if doLoose:
        thistoolLoose = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_Loose"%name_suffix,EffiFlag="LooseMuons")
        thistoolLoose.MatchToLoose = True
        thistoolLoose.ApplyScaleFactors = False
        thistoolLoose.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolLoose]
        
        if doDRSys:
            thistoolLoose_dRdown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_Loose_dRdown"%name_suffix,EffiFlag="LooseMuons_dRDown")
            thistoolLoose_dRdown.MatchToLoose = True
            thistoolLoose_dRdown.ApplyScaleFactors = False
            thistoolLoose_dRdown.IsNominal=False
            thistoolLoose_dRdown.MaximumDrCut = 0.5 *MATCH_DR
            MatchingTools += [thistoolLoose_dRdown]
            
            thistoolLoose_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_Loose_dRUp"%name_suffix,EffiFlag="LooseMuons_dRUp")
            thistoolLoose_dRUp.MatchToLoose = True
            thistoolLoose_dRUp.ApplyScaleFactors = False
            thistoolLoose_dRUp.IsNominal=False
            thistoolLoose_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolLoose_dRUp]
            
        # also run a version without CaloTag
        thistoolLoose_noCaloTag = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_Loose_noCaloTag"%name_suffix,EffiFlag="LooseMuons_noCaloTag")
        thistoolLoose_noCaloTag.MatchToLoose_noCaloTag = True
        thistoolLoose_noCaloTag.ApplyScaleFactors = False
        thistoolLoose_noCaloTag.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolLoose_noCaloTag]
        
        if doDRSys:
            thistoolLoose_noCaloTag_dRdown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_Loose_noCaloTag_dRdown"%name_suffix,EffiFlag="LooseMuons_noCaloTag_dRDown")
            thistoolLoose_noCaloTag_dRdown.MatchToLoose_noCaloTag = True
            thistoolLoose_noCaloTag_dRdown.ApplyScaleFactors = False
            thistoolLoose_noCaloTag_dRdown.IsNominal=False
            thistoolLoose_noCaloTag_dRdown.MaximumDrCut = 0.5 *MATCH_DR
            MatchingTools += [thistoolLoose_noCaloTag_dRdown]
            
            thistoolLoose_noCaloTag_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_Loose_noCaloTag_dRUp"%name_suffix,EffiFlag="LooseMuons_noCaloTag_dRUp")
            thistoolLoose_noCaloTag_dRUp.MatchToLoose_noCaloTag = True
            thistoolLoose_noCaloTag_dRUp.ApplyScaleFactors = False
            thistoolLoose_noCaloTag_dRUp.IsNominal=False
            thistoolLoose_noCaloTag_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolLoose_noCaloTag_dRUp]
        
    if doMedium:
        thistoolMed = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_Medium"%name_suffix,EffiFlag="MediumMuons")
        thistoolMed.MatchToMedium = True
        thistoolMed.ApplyScaleFactors = False
        thistoolMed.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolMed]
        
        if doDRSys:
            thistoolMedium_dRdown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_Medium_dRdown"%name_suffix,EffiFlag="MediumMuons_dRDown")
            thistoolMedium_dRdown.MatchToMedium = True
            thistoolMedium_dRdown.IsNominal=False
            thistoolMedium_dRdown.ApplyScaleFactors = False
            thistoolMedium_dRdown.MaximumDrCut = 0.5 *MATCH_DR
            MatchingTools += [thistoolMedium_dRdown]
            
            thistoolMedium_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_Medium_dRUp"%name_suffix,EffiFlag="MediumMuons_dRUp")
            thistoolMedium_dRUp.MatchToMedium = True
            thistoolMedium_dRUp.IsNominal=False
            thistoolMedium_dRUp.ApplyScaleFactors = False
            thistoolMedium_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolMedium_dRUp]
        
    if doTight:
        thistoolTight = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_Tight"%name_suffix,EffiFlag="TightMuons")
        thistoolTight.MatchToTight = True
        thistoolTight.ApplyScaleFactors = False
        thistoolTight.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolTight]
        
        if doDRSys:
            thistoolTight_dRdown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_Tight_dRdown"%name_suffix,EffiFlag="TightMuons_dRDown")
            thistoolTight_dRdown.MatchToTight = True
            thistoolTight_dRdown.ApplyScaleFactors = False
            thistoolTight_dRdown.IsNominal=False
            thistoolTight_dRdown.MaximumDrCut = 0.5 *MATCH_DR
            MatchingTools += [thistoolTight_dRdown]
            
            thistoolTight_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_Tight_dRUp"%name_suffix,EffiFlag="TightMuons_dRUp")
            thistoolTight_dRUp.MatchToTight = True
            thistoolTight_dRUp.IsNominal=False
            thistoolTight_dRUp.ApplyScaleFactors = False
            thistoolTight_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolTight_dRUp]

    if doHighPt:
        thistoolHighPt = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_HighPt"%name_suffix,
                                                                       EffiFlag="HighPtMuons")
        thistoolHighPt.MatchToHighPt = True
        thistoolHighPt.ScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Tight")  # UPDATE when SF available for high pt
        thistoolHighPt.ApplyScaleFactors=doClosure
        thistoolHighPt.MaximumDrCut = MATCH_DR
        MatchingTools += [thistoolHighPt]
        if doDRSys:
            thistoolHighPt_dRDown = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_HighPt_dRDown"%name_suffix,
                                                                                  EffiFlag="HighPtMuons_dRDown")
            thistoolHighPt_dRDown.MatchToHighPt = True
            thistoolHighPt_dRDown.IsNominal=False
            thistoolHighPt_dRDown.ScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("HighPt")  # UPDATE when SF available for high pt
            #thistoolHighPt_dRDown.ApplyScaleFactors = True
            thistoolHighPt_dRDown.MaximumDrCut = 0.5 * MATCH_DR
            MatchingTools += [thistoolHighPt_dRDown]
            
            thistoolHighPt_dRUp = CommonMuonTPConfig.AddMuonRecoTPEfficiencyTool(name="JPsiTPEfficiencyTool_%s_HighPt_dRUp"%name_suffix,
                                                                                EffiFlag="HighPtMuons_dRUp")
            thistoolHighPt_dRUp.MatchToHighPt = True
            thistoolHighPt_dRUp.IsNominal=False
            thistoolHighPt_dRUp.ScaleFactorTool = CommonMuonTPConfig.AddMuonEfficiencyScaleFactors("Tight")  # UPDATE when SF available for high pt
            #thistoolHighPt_dRUp.ApplyScaleFactors = True
            thistoolHighPt_dRUp.MaximumDrCut = 2 * MATCH_DR
            MatchingTools += [thistoolHighPt_dRUp]
            
    ntuples = []
    if writeNtuple:
        ntuples.append(CommonMuonTPConfig.AddTreeTool(name="JPsiTPTreeTool_%s"%name_suffix, EffiFlag="Trees",WriteSFInfo=doClosure))

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
                          ProduceEfficiencies=False):


    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.AppMgr import ToolSvc
    job = AlgSequence()

    MuonContainerToUse = "Muons"
    #if hasattr(job, "MuonQualityUpdater"):
        #MuonContainerToUse = "UpdatedMuons"


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
                           doCaloTag = False,
                           doIndividualAuthors = doValid,
                           writeNtuple = writeNtuple,
                           DoProbeMatchPlots=DoProbeMatchPlots,
                           ProduceEfficiencies=ProduceEfficiencies)
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
                           doIndividualAuthors = doValid,
                           writeNtuple = writeNtuple,
                           DoProbeMatchPlots=DoProbeMatchPlots,
                           ProduceEfficiencies=ProduceEfficiencies)


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
                           doIndividualAuthors = doValid,
                           writeNtuple = writeNtuple,
                           DoProbeMatchPlots=DoProbeMatchPlots,
                           ProduceEfficiencies=ProduceEfficiencies)


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
                           doCaloTag = False,
                           doIndividualAuthors = doValid,
                           writeNtuple = writeNtuple,
                           useTrigger = False,
                           DoProbeMatchPlots=DoProbeMatchPlots,
                           ProduceEfficiencies=ProduceEfficiencies)


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
                           doIndividualAuthors = doValid,
                           writeNtuple = writeNtuple,
                           useTrigger = False,
                           DoProbeMatchPlots=DoProbeMatchPlots,
                           ProduceEfficiencies=ProduceEfficiencies)