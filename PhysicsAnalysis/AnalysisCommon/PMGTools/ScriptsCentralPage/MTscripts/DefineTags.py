#!/usr/bin/python

import subprocess,sys
import os,string
import time

import StatusFiles

fLocalPath = os.getcwd()

def ReturnDerivationTags():
    Tags = ["TOPQ", "EXOT", "HIGG", "STDM", "SUSY", "JETM", "FTAG", "TAUP", "MUON", "EGAM", "TRUTH"]
    return Tags


def ReturnProductionTags():
    Tags = ["mc15_13TeV"]
    return Tags

    
def ReturnSampleCategory():
    Tags = ["TTbar", "TTbarBulk", "SingleTop",
            "Wjets_PowPy8_incl", "Wjets_PowPy8_slice", "Wjets_Sherpa_light", "Wjets_Sherpa_Cfilter", "Wjets_Sherpa_Bfilter", "Wjets_Sherpa22_light", "Wjets_Sherpa22_Cfilter", "Wjets_Sherpa22_Bfilter", "Wjets_Sherpa221",  
            #"Wjets_Sherpa221_Cfilter", "Wjets_Sherpa221_Bfilter",
            "Wjets_AlpgenPy6", "Wjets_MadGraphPy8", "Wjets_Sherpa_Systematics","Wjets_Other",
            "Zjets_PowPy8_incl", "Zjets_PowPy8_slice", "Zjets_Sherpa_light", "Zjets_Sherpa_Cfilter", "Zjets_Sherpa_Bfilter",
            "Zjets_Sherpa22_light", "Zjets_Sherpa22_Cfilter", "Zjets_Sherpa22_Bfilter", "Zjets_Sherpa221", #"Zjets_Sherpa221_Cfilter", "Zjets_Sherpa221_Bfilter",
            "Zjets_MadGraphPy8", "Zjets_Sherpa_Systematics", "Zjets_Sherpa_Mll10to40", "Zjets_AlpgenPy6","Zjets_Other",
            "Diboson_DC14_MC15sim", "Diboson_Sherpa_Nominal", "Diboson_Sherpa_MassSlice", "Diboson_Sherpa_Systematic", "Diboson_Powheg", "Diboson_Other",
            "SUSY_GG","SUSY_SS","SUSY_TT_BB","SUSY_EWK","SUSY_Other",
            "Exotics_Wprime", "Exotics_Zprime", "Exotics_BlackHole", "Exotics_ContactInt", "Exotics_Other", "Exotics_RS", "Exotics_DM", "Exotics_Monotop",
            "Higgs_ggH", "Higgs_VBF", "Higgs_ttH", "Higgs_VH", "Higgs_BSM_ggH", "Higgs_BSM_VBF", "Higgs_BSM_Other",
            "Multijet", "Minbias", "Triboson", "TTbarX", "BTag", "Performance", "Upgrade", "SingleParticle", "GammaJets", "DrellYan", "BPhysics","Unsorted",]
    return Tags


def ReturnSystKeywords():
    Keys = ["renormalization", "systematic", "scale", "scaleDown", "scaleUp", "muF", "muR", "muFup", "muRup", "muFdown", "muRdown"]
    return Keys


def HasKeyword(Keywords, Map):
    keyList  = Keywords.split(" ")
    #print keyList
    for entry in Map:
        for key in keyList:
            help = key.replace(",", "")
            #print key,"   ",help
            if entry.lower() == help.lower():
                return True
    return False


def SortKeyWordsToSampleCategory(Keywords, AODname):
    TTbarMap          = ["ttbar"]
    SingleTopMap      = ["singleTop", "singletop"]
    ExoticMap         = ["exotic", "monojet", "blackhole", "technicolor", "RandallSundrum", "Wprime", "Zprime", "magneticMonopole", "extraDimensions", "warpedED", "randallsundrum", "wprime", "zprime", "magneticmonopole", "extradimensions", "warpeded", "contactInteraction","contactinteraction","seeSaw","seesaw"]
    SUSYMap           = ["SUSY", "pMSSM", "leptoSUSY", "RPV", "bino", "susy", "pmssm", "leptosusy", "rpv"]
    HiggsMap          = ["WHiggs", "ZHiggs", "mH125", "Higgs", "VBF", "SMHiggs", "higgs", "mh125", "zhiggs", "whiggs", "bsmhiggs", "chargedHiggs","BSMHiggs","smhiggs","chargedhiggs"]
    MultijetMap       = ["dijet", "multijet", "qcd"]
    PerformanceMap    = ["performance"]
    TribosonMap       = ["tripleGaugeCoupling", "triboson", "ZZW", "WWW", "triplegaugecoupling", "zzw", "www"]
    UpgradeMap        = ["upgrad"]
    WjetsMap          = ["W", "w"]
    ZjetsMap          = ["Z", "z"]
    DibosonMap        = ["diboson","ZZ", "WW", "WZ", "WWbb", "WWll", "zz", "ww", "wz", "wwbb", "wwll"]
    MinbiasMap        = ["minBias", "minbias"]
    TTbarXMap         = ["ttw","ttz","ttv","ttvv","4top","ttW","ttZ","ttV","ttWW","ttVV"]
    BtagMap           = ["bTagging", "btagging"]
    SingleParticleMap = ["singleparticle"]
    GammaJetsMap      = ["photon", "diphoton"]
    DrellYanMap       = ["drellyan"]
    BPhysicsMap       = ["charmonium","Jpsi","Bs","Bd","Bminus","Bplus","bottom","bbbar"]

    if HasKeyword(Keywords, ExoticMap):
        Category = SortStringToSampleCategory(AODname, Keywords, "Exotics")
        return Category
    elif HasKeyword(Keywords,   TTbarMap):
        return "TTbar"
    elif HasKeyword(Keywords, SUSYMap):
        Category = SortStringToSampleCategory(AODname, Keywords, "SUSY")
        return Category
    elif HasKeyword(Keywords, HiggsMap):
        Category = SortStringToSampleCategory(AODname, Keywords, "Higgs")
        return Category
    elif HasKeyword(Keywords, PerformanceMap):
        return "Performance"
    elif HasKeyword(Keywords, UpgradeMap):
        return "Upgrade"
    elif HasKeyword(Keywords, BtagMap):
        return "BTag"
    elif HasKeyword(Keywords, SingleTopMap):
        return "SingleTop"
    elif HasKeyword(Keywords, WjetsMap):
        Category = SortStringToSampleCategory(AODname, Keywords, "Wjets")
        return Category
    elif HasKeyword(Keywords, ZjetsMap):
        Category = SortStringToSampleCategory(AODname, Keywords, "Zjets")
        return Category
    elif HasKeyword(Keywords, DibosonMap):
        Category = SortStringToSampleCategory(AODname, Keywords, "Diboson")
        return Category
    elif HasKeyword(Keywords, TTbarXMap):
        return "TTbarX"
    elif HasKeyword(Keywords, MinbiasMap):
        return "Minbias"
    elif HasKeyword(Keywords, MultijetMap):
        return "Multijet"
    elif HasKeyword(Keywords, TribosonMap):
        return "Triboson"
    elif HasKeyword(Keywords, GammaJetsMap):
        return "GammaJets"
    elif HasKeyword(Keywords, SingleParticleMap):
        return "SingleParticle"
    elif HasKeyword(Keywords, DrellYanMap):
        return "DrellYan"
    elif HasKeyword(Keywords, BPhysicsMap):
        return "BPhysics"
    else:
        return "Unsorted"


def StringIsInFile(AODname, Map):
    for entry in Map:
        if entry in AODname:
            return True
    return False

def StringsAreInFile(AODname, Map):
    for entries in Map:
        AllInMap = True
        for entry in entries:
            if not entry in AODname:
                AllInMap = False
        if AllInMap:
            return True
    return False


def SortStringToSampleCategory(AODname, Keywords, Type):

    if Type == "Zjets":
        Zjets_PowPy8_incl_Map  = [["PowhegPythia8", "Zee."],  ["PowhegPythia8", "Zmumu."],  ["PowhegPythia8", "Ztautau."]]
        Zjets_PowPy8_slice_Map = [["PowhegPythia8", "DYee_"], ["PowhegPythia8", "DYmumu_"], ["PowhegPythia8", "DYtautau_"]]
        Zjets_Sherpa_l_Map     = [["Sherpa_CT10", "CVetoBVeto."]]
        Zjets_Sherpa_c_Map     = [["Sherpa_CT10", "CFilterBVeto."]]
        Zjets_Sherpa_b_Map     = [["Sherpa_CT10", "BFilter."]]
        Zjets_Sherpa22_l_Map     = [["Sherpa_NNPDF", "CVetoBVeto."]]
        Zjets_Sherpa22_c_Map     = [["Sherpa_NNPDF", "CFilterBVeto."]]
        Zjets_Sherpa22_b_Map     = [["Sherpa_NNPDF", "BFilter."]]
        Zjets_Sherpa221_l_Map     = [["Sherpa_221_NNPDF"], ["_MAXHTPTV500_1000","_MAXHTPTV1000_E_CMS"]]
        #Zjets_Sherpa221_c_Map     = ["Sherpa_221_NNPDF", "CFilterBVeto."]
        #Zjets_Sherpa221_b_Map     = ["Sherpa_221_NNPDF", "BFilter."]
        Zjets_Sherpa_Syst_Map  = [["Sherpa_", "CVetoBVeto_"], ["Sherpa_","CFilterBVeto_"], ["Sherpa_","BFilter_"]]
        Zjets_Sherpa_Mll10to40_Map = [["Sherpa_","Mll10to40_"]]
        Zjets_MadGraph_Map     = [["MadGraphPythia8"],["MGPy8EG_N30NLO"]]
        Zjets_AlpgenPy6_Map     = [["Alpgen"]]
        if StringsAreInFile(AODname,   Zjets_PowPy8_incl_Map):
            return "Zjets_PowPy8_incl"
        elif StringsAreInFile(AODname, Zjets_PowPy8_slice_Map):
            return "Zjets_PowPy8_slice"
        elif StringsAreInFile(AODname, Zjets_Sherpa_l_Map):
            return "Zjets_Sherpa_light"
        elif StringsAreInFile(AODname, Zjets_Sherpa_c_Map):
            return "Zjets_Sherpa_Cfilter"
        elif StringsAreInFile(AODname, Zjets_Sherpa_b_Map):
            return "Zjets_Sherpa_Bfilter"
        elif StringsAreInFile(AODname, Zjets_Sherpa22_l_Map):
            return "Zjets_Sherpa22_light"
        elif StringsAreInFile(AODname, Zjets_Sherpa22_c_Map):
            return "Zjets_Sherpa22_Cfilter"
        elif StringsAreInFile(AODname, Zjets_Sherpa22_b_Map):
            return "Zjets_Sherpa22_Bfilter"
        elif StringsAreInFile(AODname, Zjets_Sherpa221_l_Map):
            return "Zjets_Sherpa221"
        #elif StringsAreInFile(AODname, Zjets_Sherpa221_c_Map):
        #    return "Zjets_Sherpa221_Cfilter"
        #elif StringsAreInFile(AODname, Zjets_Sherpa221_b_Map):
        #    return "Zjets_Sherpa221_Bfilter"
        elif StringsAreInFile(AODname, Zjets_Sherpa_Syst_Map):
            return "Zjets_Sherpa_Systematics"
        elif StringsAreInFile(AODname, Zjets_Sherpa_Mll10to40_Map):
            return "Zjets_Sherpa_Mll10to40"
        elif StringsAreInFile(AODname, Zjets_MadGraph_Map):
            return "Zjets_MadGraphPy8"
        elif StringsAreInFile(AODname, Zjets_AlpgenPy6_Map):
            return "Zjets_AlpgenPy6"
        else:
            return "Zjets_Other"

    elif Type == "Wjets":
        Wjets_PowPy8_incl_Map  = [["PowhegPythia8", "nu."]]
        Wjets_PowPy8_slice_Map = [["PowhegPythia8", "nu_"]]
        Wjets_Sherpa_l_Map     = [["Sherpa_CT10", "CVetoBVeto."]]
        Wjets_Sherpa_c_Map     = [["Sherpa_CT10", "CFilterBVeto."]]
        Wjets_Sherpa_b_Map     = [["Sherpa_CT10", "BFilter."]]
        Wjets_Sherpa22_l_Map   = [["Sherpa_NNPDF", "CVetoBVeto."]]
        Wjets_Sherpa22_c_Map   = [["Sherpa_NNPDF", "CFilterBVeto."]]
        Wjets_Sherpa22_b_Map   = [["Sherpa_NNPDF", "BFilter."]]
        Wjets_Sherpa221_l_Map  = [["Sherpa_221_NNPDF"], ["_MAXHTPTV500_1000","_MAXHTPTV1000_E_CMS"]]
        #Wjets_Sherpa221_c_Map  = [["Sherpa_221_NNPDF", "CFilterBVeto."]]
        #Wjets_Sherpa221_b_Map  = [["Sherpa_221_NNPDF", "BFilter."]]
        Wjets_Sherpa_Syst_Map  = [["Sherpa_", "CVetoBVeto_"], ["Sherpa_","CFilterBVeto_"], ["Sherpa_","BFilter_"]]
        Wjets_AlpgenPy6_Map    = [["Alpgen"]]
        Wjets_MadGraph_Map     = [["MadGraphPythia8"],["MGPy8EG_N30NLO"]]
        if StringsAreInFile(AODname,   Wjets_PowPy8_incl_Map):
            return "Wjets_PowPy8_incl"
        elif StringsAreInFile(AODname, Wjets_PowPy8_slice_Map):
            return "Wjets_PowPy8_slice"
        elif StringsAreInFile(AODname, Wjets_Sherpa_l_Map):
            return "Wjets_Sherpa_light"
        elif StringsAreInFile(AODname, Wjets_Sherpa_c_Map):
            return "Wjets_Sherpa_Cfilter"
        elif StringsAreInFile(AODname, Wjets_Sherpa_b_Map):
            return "Wjets_Sherpa_Bfilter"
        elif StringsAreInFile(AODname, Wjets_Sherpa22_l_Map):
            return "Wjets_Sherpa22_light"
        elif StringsAreInFile(AODname, Wjets_Sherpa22_c_Map):
            return "Wjets_Sherpa22_Cfilter"
        elif StringsAreInFile(AODname, Wjets_Sherpa22_b_Map):
            return "Wjets_Sherpa22_Bfilter"
        elif StringsAreInFile(AODname, Wjets_Sherpa221_l_Map):
            return "Wjets_Sherpa221"
        #elif StringsAreInFile(AODname, Wjets_Sherpa221_c_Map):
        #    return "Wjets_Sherpa221_Cfilter"
        #elif StringsAreInFile(AODname, Wjets_Sherpa221_b_Map):
        #    return "Wjets_Sherpa221_Bfilter"
        elif StringsAreInFile(AODname, Wjets_MadGraph_Map):
            return "Wjets_MadGraphPy8"
        elif StringsAreInFile(AODname, Wjets_AlpgenPy6_Map):
            return "Wjets_AlpgenPy6"
        elif StringsAreInFile(AODname, Wjets_Sherpa_Syst_Map):
            return "Wjets_Sherpa_Systematics"
        else:
            return "Wjets_Other"

    elif Type == "Higgs":

        Higgs_ggH_Map     = ["_ggH125"]
        Higgs_VBF_Map     = ["_VBFH125"]
        Higgs_ggH_BSM_Map = ["_ggH"]
        Higgs_VBF_BSM_Map = ["_VBFH","_VBF"]
        Higgs_ttH_Map     = ["_ttH125"]
        Higgs_VH_Map      = ["_WH125", "_ZH125", "_WpH125","_WplustaunuH","_WminustaunuH","_WplusenuH","_WminusenuH","_WplusmunuH","_WminusmunuH"]

        if StringIsInFile(AODname, Higgs_ggH_Map):
            return "Higgs_ggH"
        elif StringIsInFile(AODname, Higgs_VBF_Map):
            return "Higgs_VBF"
        elif StringIsInFile(AODname, Higgs_ttH_Map):
            return "Higgs_ttH"
        elif StringIsInFile(AODname, Higgs_VH_Map):
            return "Higgs_VH"
        elif StringIsInFile(AODname, Higgs_ggH_BSM_Map):
            return "Higgs_BSM_ggH"
        elif StringIsInFile(AODname, Higgs_VBF_BSM_Map):
            return "Higgs_BSM_VBF"
        else:
            return "Higgs_BSM_Other"

    elif Type == "Exotics":
        Wprime_Map    = [["Wprime"], ["wprime"]]
        Zprime_Map    = [["Zprime"], ["zprime"]]
        BlackHole_Map = [["_BH"], ["_QBH"]]
        CI_Map        = [["CI"],["CIee"],["CImumu"],["CItautau"]]
        DM_Map        = [["wimp"], ["Wimp"]]
        RS_Map        = [["randallsundrum"], ["RandallSundrum"]]
        Monotop_Map   = [["monotop"], ["Monotop"]]
        if StringsAreInFile(AODname,   Wprime_Map):
            return "Exotics_Wprime"
        elif StringsAreInFile(AODname, Zprime_Map):
            return "Exotics_Zprime"
        elif StringsAreInFile(AODname, BlackHole_Map):
            return "Exotics_BlackHole"
        elif StringsAreInFile(AODname, CI_Map):
            return "Exotics_ContactInt"
        elif StringsAreInFile(AODname, DM_Map):
            return "Exotics_DM"
        elif StringsAreInFile(AODname, RS_Map):
            return "Exotics_RS"
        elif StringsAreInFile(AODname, Monotop_Map):
            return "Exotics_Monotop"
        else:
            return "Exotics_Other"

    elif Type == "Diboson":
	Diboson_DC14_MC15sim_Map = [["PowhegPythia8", ".187"]]   #"mc12_13TeV"]]
	Diboson_Sherpa_Map = [["Sherpa_"]]
	Diboson_Powheg_Map = [["PowhegPy8"]]
	if StringsAreInFile(AODname, Diboson_DC14_MC15sim_Map):
	    return "Diboson_DC14_MC15sim"
	if StringsAreInFile(AODname, Diboson_Sherpa_Map):
            if "systematic" in Keywords:
                return "Diboson_Sherpa_Systematic"
            else:
                if "0M" in AODname:
                    return "Diboson_Sherpa_MassSlice"
                else:
                    return "Diboson_Sherpa_Nominal"

        # systematic

	if StringsAreInFile(AODname, Diboson_Powheg_Map):
	    return "Diboson_Powheg"
	else:
	    return "Diboson_Other"

    elif Type == "SUSY":
	SUSY_GG_Map = [["_GG_"]]
	SUSY_SS_Map = [["_SS_"]]
	SUSY_TT_BB_Map = [["_TT_"],["_BB_"],["_Tt_"]]
	SUSY_EWK_Map = [["_C1N2_"],["_C1C1_"]]
     	if StringsAreInFile(AODname, SUSY_GG_Map):
	    return "SUSY_GG"
     	if StringsAreInFile(AODname, SUSY_SS_Map):
	    return "SUSY_SS"
     	if StringsAreInFile(AODname, SUSY_TT_BB_Map):
	    return "SUSY_TT_BB"
     	if StringsAreInFile(AODname, SUSY_EWK_Map):
	    return "SUSY_EWK"
	else:
	    return "SUSY_Other"


    #elif Type == "Unsorted":
    #    ChargedHiggs_Map = [["Hplus", "Hminus"]]
    #    if StringsAreInFile(AODname, ChargedHiggs_Map):
    #        return "Higgs_BSM"
    #    else:
    #        return "Unsorted"

    else:
        return "NONE"
