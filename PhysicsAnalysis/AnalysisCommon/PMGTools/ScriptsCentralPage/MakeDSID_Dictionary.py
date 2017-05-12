#! /usr/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os,sys
import glob

sys.path.append('NewScripts/')

import StatusFiles

StatusList = glob.glob("AllSampleInfo/*/*/StatusFile_*")

# loop over input files
# read in all contents from MainFiles


LinkLocation = "https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/"

Dict = [
    ["MC15TTbarSamplesPMG",       "TTbarBulk"],
    ["MC15TTbarSamplesSlicedPMG", "TTbar"],
    ["MC15SingleTopSamplesPMG",   "SingleTop"],
    ["MC15WjetsPowPy8InclSamplesPMG", "Wjets_PowPy8_incl"],
    ["MC15WjetsPowPy8SliceSamplesPMG", "Wjets_PowPy8_slice"],
    ["MC15WjetsSherpaLightSamplesPMG", "Wjets_Sherpa_light"],
    ["MC15WjetsSherpaCSamplesPMG", "Wjets_Sherpa_Cfilter"],
    ["MC15WjetsSherpaBSamplesPMG", "Wjets_Sherpa_Bfilter"],
    ["MC15WSherpaSystPMG", "Wjets_Sherpa_Systematics"],
    ["MC15WjetsSherpa22LightSamplesPMG", "Wjets_Sherpa22_light"],
    ["MC15WjetsSherpa22CSamplesPMG", "Wjets_Sherpa22_Cfilter"],
    ["MC15WjetsSherpa22BSamplesPMG", "Wjets_Sherpa22_Bfilter"],
    ["MC15WjetsSherpa221LightSamplesPMG", "Wjets_Sherpa221"],
    ["MC15WjetsMadgraphPy8SamplesPMG", "Wjets_MadGraphPy8"],
    ["MC15WjetsAlpgenPy6SamplesPMG", "Wjets_AlpgenPy6"],
    ["MC15WjetsOtherSamplesPMG", "Wjets_Other"],
    ["MC15ZSherpaSystPMG",               "Zjets_Sherpa_Systematics"],
    ["MC15ZjetsSherpa22LightSamplesPMG", "Zjets_Sherpa22_light"],
    ["MC15ZjetsSherpa22CSamplesPMG", "Zjets_Sherpa22_Cfilter"],
    ["MC15ZjetsSherpa22BSamplesPMG", "Zjets_Sherpa22_Bfilter"],
    ["MC15ZjetsSherpa221LightSamplesPMG", "Zjets_Sherpa221"],
    ["MC15ZjetsMadgraphPy8SamplesPMG", "Zjets_MadGraphPy8"],
    ["MC15ZjetsAlpgenPy6SamplesPMG", "Zjets_AlpgenPy6"],
    ["MC15ZjetsOtherSamplesPMG", "Zjets_Other"],
    ["MC15ZjetsPowPy8InclSamplesPMG", "Zjets_PowPy8_incl"],
    ["MC15ZjetsPowPy8SliceSamplesPMG", "Zjets_PowPy8_slice"],
    ["MC15ZjetsSherpaLightSamplesPMG", "Zjets_Sherpa_light"],
    ["MC15ZjetsSherpaCSamplesPMG", "Zjets_Sherpa_Cfilter"],
    ["MC15ZjetsSherpaBSamplesPMG", "Zjets_Sherpa_Bfilter"],
    ["MC15ZjetsSherpaMll10to40PMG", "Zjets_Sherpa_Mll10to40"],
    ["MC15DibosonSherpa", "Diboson_Sherpa_Nominal"],
    ["MC15DibosonSherpaMassSlices", "Diboson_Sherpa_MassSlice"],
    ["MC15DibosonSherpaSystematics", "Diboson_Sherpa_Systematic"],
    ["MC15DibosonPowheg", "Diboson_Powheg"],
    ["MC15DibosonPowhegDC14", "Diboson_DC14_MC15sim"],
    ["MC15DibosonOther", "Diboson_Other"],
    ["MC15TTbarXPMG", "TTbarX"],
    ["MC15ggHiggsPMG", "Higgs_ggH"],
    ["MC15VBFHiggsPMG", "Higgs_VBF"],
    ["MC15VHiggsPMG", "Higgs_VH"],
    ["MC15ttHiggsPMG", "Higgs_ttH"],
    ["MC15BSMggHiggsPMG", "Higgs_BSM_ggH"],
    ["MC15BSMVBFHiggsPMG", "Higgs_BSM_VBF"],
    ["MC15BSMHiggsPMG", "Higgs_BSM_Other"],
    ["MC15ExoticsWprimePMG", "Exotics_Wprime"],
    ["MC15ExoticsZprimePMG", "Exotics_Zprime"],
    ["MC15ExoticsBlackHolePMG", "Exotics_BlackHole"],
    ["MC15ExoticsDMPMG", "Exotics_DM"],
    ["MC15ExoticsRSPMG", "Exotics_RS"],
    ["MC15ExoticsMonotopPMG", "Exotics_Monotop"],
    ["MC15ExoticsCIPMG", "Exotics_ContactInt"],
    ["MC15ExoticsOthersPMG", "Exotics_Other"],
    ["MC15SUSYGGPMG", "SUSY_GG"],
    ["MC15SUSYSSPMG", "SUSY_SS"],
    ["MC15SUSYTTBBPMG", "SUSY_TT_BB"],
    ["MC15SUSYEWKPMG", "SUSY_EWK"],
    ["MC15SUSYOthersPMG", "SUSY_Other"],
    ["MC15TribosonPMG", "Triboson"],
    ["MC15MultijetPMG", "Multijet"],
    ["MC15MinBiasPMG", "Minbias"],
    ["MC15GammaJetsPMG", "GammaJets"],
    ["MC15UnsortedPMG", "Unsorted"],
    ["MC15PerformancePMG", "Performance"],
    ["MC15BPhysicsPMG", "BPhysics"],
    ["MC15DrellYanPMG", "DrellYan"],
    ["MC15SingleParticlePMG", "SingleParticle"],
    ["MC15GammaJetsPMG", "GammaJets"],
    ]


def GetLink(Tag):
    for entry in Dict:
        if Tag == entry[1]:
            return entry[0]
        

StatusList = sorted(StatusList)

#print StatusList
#print len(StatusList)

outputTable  = ""
outputTable += "%TABLE{tablewidth=\"100%\"}% \n"
outputTable += "| *DSID* | *Link to corresponding twiki* | \n "

for StatusFile in StatusList:

    AllInfo    = StatusFiles.StatusFileReader(StatusFile)
    SortingTag = AllInfo.SortingTag
    DSID       = AllInfo.DSID
    
    PartLink   = GetLink(SortingTag)
    Link       = LinkLocation+PartLink

    outputTable += " | " + str(DSID) + " | [[" + str(Link) + "][" + str(PartLink) + "]] | \n"
    



outputFile = open("/afs/cern.ch/user/m/mcgensvc/www/mc15twiki/CentralMC15ProductionPage/SummaryDSID.twiki", "w")
outputFile.write(outputTable)
outputFile.close()
