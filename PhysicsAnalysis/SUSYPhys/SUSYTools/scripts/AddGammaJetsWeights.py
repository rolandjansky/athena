#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os,sys,subprocess,datetime,copy,math,array,shutil,ROOT,re,string
from ROOT import gROOT
from subprocess import call
from array import array

__author__ = "John Anders"
__doc__    = """Script to add Sherpa Systematic Weight branches to existing file."""   
def generateGammapTMapping(dictionary):
    
    # 0-70, bin 1, 70-140 bin 2 etc.
    print "Adding gamma+jets to the dictionary"
    print "Range 0-70"
    for i in range(361039, 361042):
        dictionary[i] = 1
        print i
        
    print "Range 70-140"
    for i in range(361042, 361045):
        dictionary[i] = 2
        print i
    
    print "Range 140-280"
    for i in range(361045, 361048):
        dictionary[i] = 3
        print i
    

    print "Range 280-500"
    for i in range(361048, 361051):
        dictionary[i] = 4
        print i
    
    print "Range 500-1000"
    for i in range(361051, 361054):
        dictionary[i] = 5
        print i
        
    print "Range 1000-2000"
    for i in range(361054, 361057):
        dictionary[i] = 6
        print i
    
    print "Range 2000-ECMS"
    for i in range(361057, 361061):
        dictionary[i] = 7
        print i
    
    return dictionary
    

def main(argv):

    # infile should be the ZJets file to add the weights to. 
    script, infile = argv

    print "Creating MCID, GammapT mapping"
    GammapTMapping = dict()
    generateGammapTMapping(GammapTMapping)

    print "Loading histograms with weights"
    Histograms = []
    
    # Change here to the location of the variation file: 
    fileWithWeights = "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/GammaJets_SysParameterization/AllVariationsGamma.root"

    g = ROOT.TFile(fileWithWeights)
    systList = ["ckkw15", "ckkw30", "fact025", "fact4", "renorm025", "renorm4", "qsf025", "qsf4"]
    
    for syst in systList:
        print 'syst',syst
        h = ROOT.TH2F(g.Get(syst))
        Histograms.append(h)

    print "Loading File"
    f = ROOT.TFile(infile,"update")
    
    # Change here to the name of your nominal tree:
    #treeName = "CollectionTree_"
    #treeName = "Z_CRWT"#"CollectionTree_"                                                                                                                                                                         
    #treeName = "Z_SRAll"                                                                                                                                                                                          
    #treeName = "Z_CRZ"                                                                                                                                                                                            
    treeName = "GAMMA_CRY"
    #treeName = "Z_VRWT"                                                                                                                                                                                           
    #treeName = "GAMMA_CR3L"


    print "Loading Tree"

    # empty arrays for adding branches
    ckkw15 = array('f', [0.])
    ckkw30 = array('f', [0.])
    fact025 = array('f', [0.])
    fact4 = array('f', [0.])
    renorm025 = array('f', [0.])
    renorm4 = array('f', [0.])
    qsf025 = array('f', [0.])
    qsf4 = array('f', [0.])
    
    # Add branches
    T = f.Get(treeName)
    ckkw15branch = T.Branch("ckkw15_Weight", ckkw15,"ckkw15_Weight/F")
    ckkw30branch = T.Branch("ckkw30_Weight", ckkw30,"ckkw30_Weight/F")
    fact025branch = T.Branch("fact025_Weight", fact025,"fact025_Weight/F")
    fact4branch = T.Branch("fact4_Weight", fact4,"fact4_Weight/F")
    renorm025branch = T.Branch("renorm025_Weight", renorm025,"renorm025_Weight/F")
    renorm4branch = T.Branch("renorm4_Weight", renorm4,"renorm4_Weight/F")
    qsf025branch = T.Branch("qsf025_Weight", qsf025,"qsf025_Weight/F")
    qsf4branch = T.Branch("qsf4_Weight", qsf4,"qsf4_Weight/F")
        
    print "Adding Weights"
    nEvts = T.GetEntries()
    for iEvt in range(nEvts):
        
        if iEvt%1000 == 0:
            print "Current Event being Processed is: ", iEvt
            
        T.GetEntry(iEvt)
            
            # change this to whatever the MCID and nTruthJet variables are saved as in your tree 
        MCID = T.RunNumber #mcID
        nTruthJets = T.nJet
        
            # find which ZpT bin we want
        GammapTBin = GammapTMapping.get(MCID)
        TruthJetBin = nTruthJets+1
        if nTruthJets >= 11:
            TruthJetBin = 12
        if GammapTBin == None:
                #this MCID isn't in the mapping, so skip and default to 1
            ckkw15[0] = 1 
            ckkw30[0] = 1
            fact025[0] = 1
            fact4[0]   = 1
            renorm025[0] = 1
            renorm4[0] = 1
            qsf025[0] = 1
            qsf4[0] = 1

        else:
            ckkw15[0] = Histograms[0].GetBinContent(GammapTBin,TruthJetBin) 
            ckkw30[0] =  Histograms[1].GetBinContent(GammapTBin,TruthJetBin)
            fact025[0] =  Histograms[2].GetBinContent(GammapTBin,TruthJetBin)
            fact4[0] =  Histograms[3].GetBinContent(GammapTBin,TruthJetBin)
            renorm025[0] =  Histograms[4].GetBinContent(GammapTBin,TruthJetBin)
            renorm4[0] =  Histograms[5].GetBinContent(GammapTBin,TruthJetBin)
            qsf025[0] =  Histograms[6].GetBinContent(GammapTBin,TruthJetBin)
            qsf4[0] =  Histograms[7].GetBinContent(GammapTBin,TruthJetBin)
        
        ckkw15branch.Fill()
        ckkw30branch.Fill()
        fact025branch.Fill()
        fact4branch.Fill()
        renorm025branch.Fill()
        renorm4branch.Fill()
        qsf025branch.Fill()
        qsf4branch.Fill()
        
        
        
    T.Write()

    print "Done."
    
if __name__ == '__main__':
    main(sys.argv)




