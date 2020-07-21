# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#====================================================================
# Common file used for TOPQ skimming
# Call with:
#   import DerivationFrameworkTop.TOPQCommonSelection
#   skimmingTools_lep = DerivationFrameworkTop.TOPQCommonSelection.setup_lep('TOPQ1', ToolSvc)
#   skimmingTools_jet = DerivationFrameworkTop.TOPQCommonSelection.setup_jet('TOPQ1', ToolSvc)
#====================================================================

#================================
# SKIMMING SELECTIONS
#================================
# *all selection objects required to have |eta|<2.5
# *all selection electrons must be medium OR LHMedium
# *all selection muons must be DFCommonGoodMuons AND muonType==0
# *largeR jets == AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets(calibrated)
#
# TOPQ1 (single top)
#   >=1 electron(pT>20 GeV) OR
#   >=1 muon(pT>20 GeV)
#
# TOPQ2 (dilepton)
#  >=2 leptons(pT>15 GeV) OR
#  >=2 leptons(pT>10 GeV) && >=1 lepton(pT>20 GeV)
#
# TOPQ3 (lepton + jets)
#  >=1 lepton(pT>13 GeV) AND
#  >=4 akt4calibjet(pT>15 GeV) OR >=1 largeRjet(pT>200 GeV)
#
# TOPQ4 (all hadronic)
#  >=5 akt4calibjet(pT>20 GeV) OR
#  >=1 largeRjet(pT>200 GeV)
#
# TOPQ5 (single top selection plus J/Psi->mumu candidate)
#   >=1 electron(pT>20GeV) OR
#   >=1 muon(pT>20GeV)
#   2 additional muons with 2GeV<M_mumu<4GeV
# reductionConf flag TOPQ5 in Reco_tf.py
# Shares most settings with TOPQ1
#
# TOPQ6 (single top)
#   >=1 electron(pT>23 GeV) AND
#   >=1 muon(pT>23 GeV)
# otherwise same as TOPQ1 but no thinning on tracks

#================================
# IMPORTS
#================================
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from AthenaCommon.GlobalFlags import globalflags

#================
# ELECTRONS
#================
EL10 = "(Electrons.pt > 10*GeV && abs(Electrons.eta) < 2.7 && Electrons.DFCommonElectronsLHLoose)"
EL13 = "(Electrons.pt > 13*GeV && abs(Electrons.eta) < 2.7 && Electrons.DFCommonElectronsLHLoose)"
EL15 = "(Electrons.pt > 15*GeV && abs(Electrons.eta) < 2.7 && Electrons.DFCommonElectronsLHLoose)"
EL15_tight = "(Electrons.pt > 15*GeV && abs(Electrons.eta) < 2.7 && Electrons.DFCommonElectronsLHTight)"
EL17 = "(Electrons.pt > 17*GeV && abs(Electrons.eta) < 2.7 && Electrons.DFCommonElectronsLHLoose)"
EL20 = "(Electrons.pt > 20*GeV && abs(Electrons.eta) < 2.7 && Electrons.DFCommonElectronsLHLoose)"
EL22 = "(Electrons.pt > 22*GeV && abs(Electrons.eta) < 2.7 && Electrons.DFCommonElectronsLHLoose)"
EL23 = "(Electrons.pt > 23*GeV && abs(Electrons.eta) < 2.7 && Electrons.DFCommonElectronsLHLoose)"
EL25 = "(Electrons.pt > 25*GeV && abs(Electrons.eta) < 2.7 && Electrons.DFCommonElectronsLHLoose)"

#================
# MUONS
#================
MU10 = "(Muons.pt > 10*GeV && abs(Muons.eta) < 2.7 && Muons.muonType == 0 && Muons.DFCommonGoodMuon)"
MU13 = "(Muons.pt > 13*GeV && abs(Muons.eta) < 2.7 && Muons.muonType == 0 && Muons.DFCommonGoodMuon)"
MU15 = "(Muons.pt > 15*GeV && abs(Muons.eta) < 2.7 && Muons.muonType == 0 && Muons.DFCommonGoodMuon)"
MU15_tight = "(Muons.pt > 15*GeV && abs(Muons.eta) < 2.7 && Muons.muonType == 0 && Muons.DFCommonGoodMuon && Muons.DFCommonMuonsTight)"
MU17 = "(Muons.pt > 17*GeV && abs(Muons.eta) < 2.7 && Muons.muonType == 0 && Muons.DFCommonGoodMuon)"
MU20 = "(Muons.pt > 20*GeV && abs(Muons.eta) < 2.7 && Muons.muonType == 0 && Muons.DFCommonGoodMuon)"
MU22 = "(Muons.pt > 22*GeV && abs(Muons.eta) < 2.7 && Muons.muonType == 0 && Muons.DFCommonGoodMuon)"
MU23 = "(Muons.pt > 23*GeV && abs(Muons.eta) < 2.7 && Muons.muonType == 0 && Muons.DFCommonGoodMuon)"
MU25 = "(Muons.pt > 25*GeV && abs(Muons.eta) < 2.7 && Muons.muonType == 0 && Muons.DFCommonGoodMuon)"

#================
# JETS
#================
akt4EMcalib_15 = "(AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 15*GeV && abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta) < 2.5)"
akt4EMcalib_17 = "(AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 17*GeV && abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta) < 2.5)"
akt4EMcalib_20 = "(AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 20*GeV && abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta) < 2.5)"
akt4EMcalib_22 = "(AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 22*GeV && abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta) < 2.5)"
akt4EMcalib_50 = "(AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 50*GeV && abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta) < 2.5)"
akt4EMPFlowcalib_20 = "(AntiKt4EMPFlowJets.DFCommonJets_Calib_pt > 20*GeV && abs(AntiKt4EMPFlowJets.DFCommonJets_Calib_eta) < 2.5)"
akt10LCtrimmedcalib_200 = "(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 200*GeV && abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta) < 2.5)"
akt10LCtrimmedcalib_220 = "(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 220*GeV && abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta) < 2.5)"
akt10LCtrimmedcalib_350 = "(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 350*GeV && abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta) < 2.5)"
akt10LCtrimmedcalib_200_masscut = "(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 200*GeV && abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta) < 2.5 && AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_m > 30*GeV)"
akt4EM_FCBEff_85_MV2c10 = "(AntiKt4EMTopoJets_BTagging201810.DFCommonJets_FixedCutBEff_85_MV2c10)"
akt4EM_FCBEff_85_DL1 = "(AntiKt4EMTopoJets_BTagging201810.DFCommonJets_FixedCutBEff_85_DL1)"
akt4EMPFlow_FCBEff_85_MV2c10 = "(AntiKt4EMPFlowJets_BTagging201810.DFCommonJets_FixedCutBEff_85_MV2c10)"
akt4EMPFlow_FCBEff_85_DL1 = "(AntiKt4EMPFlowJets_BTagging201810.DFCommonJets_FixedCutBEff_85_DL1)"

#LargeR jets
largeR_200="("+akt10LCtrimmedcalib_200+")"
largeR_220="("+akt10LCtrimmedcalib_220+")"
largeR_350="("+akt10LCtrimmedcalib_350+")"
largeR_200_masscut="("+akt10LCtrimmedcalib_200_masscut+")"

#=============================================
# LEPTON EVENT SELECTION
#=============================================
def setup_lep(TOPQname, ToolSvc):

    skimmingTools_lep=[]

    #=============================
    # YEAR dependent selection
    #=============================
    print "TOPQ.py: Project tag: " + rec.projectName()

    if (rec.projectName.get_Value() in ['data16_13TeV', 'data17_13TeV','data18_13TeV']) :
        EL = EL25
        MU = MU25
    else :
        EL = EL20
        MU = MU20

    if TOPQname == 'TOPQ1':
        TOPQ_Selection_lep = "( (count("+MU+") >= 1) || (count("+EL+") >= 1) )"
    elif TOPQname == 'TOPQ2':
        TOPQ2_LEP10 = "( (count("+EL10+") >= 2) || (count("+MU10+") >= 2) || (count("+EL10+")>= 1 && count("+MU10+") >= 1) )"
        TOPQ2_LEP15 = "( (count("+EL15+") >= 2) || (count("+MU15+") >= 2) || (count("+EL15+")>= 1 && count("+MU15+") >= 1) )"
        TOPQ2_LEP17 = "( (count("+EL17+") >= 2) || (count("+MU17+") >= 2) || (count("+EL17+")>= 1 && count("+MU17+") >= 1) )"
        TOPQ2_LEP20 = "( (count("+EL20+") >= 1) || (count("+MU20+") >= 1) )"
        TOPQ2_LEP22 = "( (count("+EL22+") >= 1) || (count("+MU22+") >= 1) )"
        TOPQ_Selection_lep = "( ("+TOPQ2_LEP15+") || (("+TOPQ2_LEP10+") && ("+TOPQ2_LEP20+")) )"
    elif TOPQname == 'TOPQ3':
        TOPQ_Selection_lep = "( (count("+MU13+") >= 1) || (count("+EL13+") >= 1) )"
    elif TOPQname == 'TOPQ4':
        TOPQ_Selection_lep = "1"
    elif TOPQname == 'TOPQ5':
        TOPQ_Selection_lep = "( (count("+MU+") >= 1) || (count("+EL+") >= 1) )"
    elif TOPQname == 'TOPQ6':
        TOPQ_Selection_lep = "( ((count("+MU23+") >= 1) && (count("+EL23+") >= 1)) )"
    else:
        TOPQ_Selection_lep = "1"

    from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
    TOPQStringSkimmingTool_lep = DerivationFramework__xAODStringSkimmingTool(
        name       = TOPQname+"StringSkimmingTool_lep",
        expression = TOPQ_Selection_lep)

    ToolSvc += TOPQStringSkimmingTool_lep

    skimmingTools_lep.append(TOPQStringSkimmingTool_lep)

    print TOPQname+".py", TOPQname+"StringSkimmingTool_lep: ", TOPQStringSkimmingTool_lep

    return skimmingTools_lep
# end setup_lep(TOPQname, ToolSvc)

#=============================================
# JET EVENT SELECTION
#=============================================
def setup_jet(TOPQname, ToolSvc):

    skimmingTools_jet=[]

    if TOPQname == 'TOPQ1':
        TOPQ_Selection_jet = "1"
    elif TOPQname == 'TOPQ2':
        TOPQ_Selection_jet = "1"
    elif TOPQname == 'TOPQ3':
        TOPQ_Selection_jet = "( (count("+akt4EMcalib_15+") >= 4) || (count("+largeR_200+") >= 1) )"
    elif TOPQname == 'TOPQ4':
        largeR_sel = 'count({}) >= 1'.format(largeR_200)
        smallR_EMTopo_sel = 'count({}) >= 5'.format(akt4EMcalib_20)
        smallR_EMTopo_btag_sel = ('(count({}) > 0 || count({}) > 0)'.format(akt4EM_FCBEff_85_MV2c10, akt4EM_FCBEff_85_DL1))
        smallR_EMPFlow_sel = 'count({}) >= 5'.format(akt4EMPFlowcalib_20)
        smallR_EMPFlow_btag_sel = ('(count({}) > 0 || count({}) > 0)'.format(akt4EMPFlow_FCBEff_85_MV2c10, akt4EMPFlow_FCBEff_85_DL1))
        TOPQ_Selection_jet = '({} || ({} && {}) || ({} && {}))'.format(largeR_sel,
                                                                       smallR_EMTopo_sel,
                                                                       smallR_EMTopo_btag_sel,
                                                                       smallR_EMPFlow_sel,
                                                                       smallR_EMPFlow_btag_sel)
    elif TOPQname == 'TOPQ5':
        TOPQ_Selection_jet = "1"
    elif TOPQname == 'TOPQ6':
        TOPQ_Selection_jet = "1"
    else:
        TOPQ_Selection_jet = "1"

    from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
    TOPQStringSkimmingTool_jet = DerivationFramework__xAODStringSkimmingTool(
        name       = TOPQname+"StringSkimmingTool_jet",
        expression = TOPQ_Selection_jet)

    ToolSvc += TOPQStringSkimmingTool_jet

    skimmingTools_jet.append(TOPQStringSkimmingTool_jet)

    print TOPQname+".py", TOPQname+"StringSkimmingTool_jet: ", TOPQStringSkimmingTool_jet

    return skimmingTools_jet
# end setup_jet(TOPQname, ToolSvc)
