# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# JetMomentFix.py
# Schedules jet moment corrections
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *

from JetRec.JetRecConf import JetAlgorithm
from DerivationFrameworkJetEtMiss.DFJetMetFlags import *

if dfjmFlags.doJetTrackMomentFix:
    print "Scheduling jet track moment fix: "
    jtm.modifiersMap["jetmomfix"] = [jtm.trkmoms]
else:
    print "Copying jet track moments: "
    jtm.modifiersMap["jetmomfix"] = []

jetmomfix_LC = jtm.addJetCopier("DFJetFix_AntiKt4LCTopoJets", "AntiKt4LCTopoJets", "jetmomfix")
DerivationFrameworkJob += JetAlgorithm("jetalgDFJetFix_AntiKt4LCTopoJets", Tools = [jetmomfix_LC] )

jetmomfix_EM = jtm.addJetCopier("DFJetFix_AntiKt4EMTopoJets", "AntiKt4EMTopoJets", "jetmomfix")
DerivationFrameworkJob += JetAlgorithm("jetalgDFJetFix_AntiKt4EMTopoJets", Tools = [jetmomfix_EM] )

from DerivationFrameworkJetEtMiss.DerivationFrameworkJetEtMissConf import DerivationFramework__JetDecorAlg
DerivationFrameworkJob += DerivationFramework__JetDecorAlg("DecorJet_LC")
DerivationFrameworkJob.DecorJet_LC.InputCollection="DFJetFix_AntiKt4LCTopoJets"
DerivationFrameworkJob.DecorJet_LC.MomentsToCopy=[
    "vector<int>#NumTrkPt500@NumTrkPt500_IDLoose","vector<float>#SumPtTrkPt500@SumPtTrkPt500_IDLoose","vector<float>#TrackWidthPt500@TrackWidthPt500_IDLoose",
    "vector<int>#NumTrkPt1000@NumTrkPt1000_IDLoose","vector<float>#SumPtTrkPt1000@SumPtTrkPt1000_IDLoose","vector<float>#TrackWidthPt1000@TrackWidthPt1000_IDLoose"]

DerivationFrameworkJob += DerivationFramework__JetDecorAlg("DecorJet_EM")
DerivationFrameworkJob.DecorJet_EM.InputCollection="DFJetFix_AntiKt4EMTopoJets"
DerivationFrameworkJob.DecorJet_EM.MomentsToCopy=[
    "vector<int>#NumTrkPt500@NumTrkPt500_IDLoose","vector<float>#SumPtTrkPt500@SumPtTrkPt500_IDLoose","vector<float>#TrackWidthPt500@TrackWidthPt500_IDLoose",
    "vector<int>#NumTrkPt1000@NumTrkPt1000_IDLoose","vector<float>#SumPtTrkPt1000@SumPtTrkPt1000_IDLoose","vector<float>#TrackWidthPt1000@TrackWidthPt1000_IDLoose"]
