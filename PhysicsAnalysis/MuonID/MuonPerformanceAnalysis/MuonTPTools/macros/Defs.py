# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# definitions for the offline Zmumu analysis
import ROOT, math
import HistoDefs

class Analysis:
    Z_Reco = "ZmumuTPReco"
    Z_Isol = "ZmumuTPIsol"
    Z_Trig = "ZmumuTPTrig"
    JP_Reco = "JPsiTPReco"
    JP_Isol = "JPsiTPIsol"
    JP_Trig = "JPsiTPTrig"
    
    
class ChargeProducts:
    OC="OC"
    SC="SC"
    OC_AntiIso="OCAntiIso"
    SC_AntiIso="SCAntiIso"

class Probes:
    ID = "IDProbes"
    Calo = "CaloProbes"
    MStoID = "MSProbes_ID"
    MStoMu = "MSProbes_Muon"
    TruthToID = "TruthProbes_ID"
    TruthToMu = "TruthProbes_Muon"

class DetRegions:
    All="All"
    noCrack="noCrack"
    Crack="Crack"
    Barrel="Barrel"
    Transition="Transition"
    Endcap="Endcap"
    CSC="CSC"

class Matches:
    Calo ="CaloTaggedMuons"
    CB ="CombinedMuons"
    Loose ="LooseMuons"
    LooseNoCalo ="LooseMuons_noCaloTag"
    Medium ="MediumMuons"
    Tight ="TightMuons"
    ID ="IDTracks"
    ID_noMCP ="IDTracks_noMCP"

class PlotKinds:
    Probes="Probes"
    Matches="Matches"
    Efficiency="Efficiency"

class HistoKinds:
    Efficiency="Efficiency"
    DataEfficiency="Data Efficiency"
    MCEfficiency="MC Efficiency"
    TruthEfficiency="Truth Efficiency"
    SF="ScaleFactor"
    TruthSF="Truth ScaleFactor"
    MCTruthSF="MC/Truth ScaleFactor"

class Systematics:
    dR="dR"
    truth="Truth"
    BG="BG"
    Det="DetectorRun1"
    All="Total"

ExtraCuts = {
    DetRegions.All: "",
    DetRegions.Crack: "abs(eta) < 0.1",
    DetRegions.noCrack: "abs(eta) > 0.1",
    DetRegions.Barrel: "abs(eta) > 0.1 && abs(eta) < 1.1",
    DetRegions.Transition: "abs(eta) > 1.1 && abs(eta) < 1.3",
    DetRegions.Endcap: "abs(eta) > 1.3 && abs(eta) < 2.0",
    DetRegions.CSC: "abs(eta) > 2.0 && abs(eta) < 2.5",
}

