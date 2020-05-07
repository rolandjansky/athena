# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

def doSlimSMQCD(treeName,outputFile,inputFiles,year):
    from ROOT import TChain
    from ROOT import TFile
    from ROOT import ROOT
    #import rootlogon
    ROOT.gROOT.SetBatch(1)

    import sys
    print ("sys.argv = ", sys.argv)

    if not len(sys.argv)>=2:
        raise Exception ("Must specify inputFiles as argument!")

    #inputFiles = sys.argv[1].split(',')
    inputFiles = inputFiles
    print ("inputFiles = ", inputFiles)

    #get main tree
    ch = TChain(treeName)
    for file in inputFiles:
        ch.Add(file)

    nEntries = ch.GetEntries()
    #print ("nEntries = ", nEntries)

    #*****set branches*****

    #set branch status, at first, all off
    ch.SetBranchStatus("*", 0)

    from JetN2N.SlimSMQCDBranches import SlimSMQCDBranchesList
    from JetN2N.SlimSMQCDBranches2011 import SlimSMQCDBranchesList2011
    #event information
    if year == '2011':
       for branch in SlimSMQCDBranchesList2011:
         print (branch)
         ch.SetBranchStatus(branch,1)
    else:
       for branch in SlimSMQCDBranchesList:
         print (branch)
         ch.SetBranchStatus(branch,1)

    #*****set branches end*****

    #get trigger tree
    chTri = TChain(treeName+"Meta/TrigConfTree")
    for file in inputFiles:
        chTri.Add(file)
    if chTri.LoadTree(0) < 0:
        chTri = 0

    #get bunch tree
    chBunch = TChain(treeName+"Meta/BunchConfTree")
    for file in inputFiles:
        chBunch.Add(file)
    if chBunch.LoadTree(0) < 0:
        chBunch = 0

    #get cut flow tree
    #chCutFlow = TChain(treeName+"Meta/CutFlowTree")
    #for file in inputFiles:
    #    chCutFlow.Add(file)

    # Write to new file
    outFile = outputFile
    newFile = TFile(outFile, "RECREATE")

    #new tree
    ch_new = ch.CloneTree(0)

    # event counter
    #cutName=["preD3PD","postD3PD","trigCut"]
    #evnum=[0,0,0]
    #weight=[0,0,0]

    #event selection
    for i in range(nEntries):
        ch.GetEntry(i)
        #evnum[1]+=1
        #if hasattr(ch,"mcevt_weight") \
        #   and ch.mcevt_weight.size() !=0 \
        #   and ch.mcevt_weight[0].size() !=0:
        #    w=ch.mcevt_weight[0][0]
        #else:
        #    w=1
        #weight[1]+=w

        ch_new.Fill()
        #evnum[2]+=1
        #weight[2]+=w

    newFile.cd()
    ch_new.Write()
    #nEntriesNew = ch_new.GetEntries()
    #print ("nEntriesForNewFile = ", nEntriesNew)

    #check cut flow at D3PD level
    #if chCutFlow.GetEntries() != 0:
    #    for i in range (chCutFlow.GetEntries()):
    #        chCutFlow.GetEntry(i)
    #        cutFlowName=chCutFlow.name
    #        for j in range(cutFlowName.size()):
    #            if cutName.at(j) == "AllExecutedEvents":
    #                evnum[0]+=chCutFlow.nAcceptedEvents.at(j)
    #                weight[0]+=chCutFlow.nWeightedAcceptedEvents.at(j)
    #else:
    #    evnum[0]=evnum[1]
    #    weight[0]=weight[1]

    #copy trigger meta data
    newFile.cd()
    newdir = newFile.mkdir( treeName+"Meta", treeName+"Meta" )
    newdir.cd()
    if chTri != 0:
        chTri_new = chTri.CloneTree()
        chTri_new.Write()

    if chBunch != 0:
        chBunch_new = chBunch.CloneTree()
        chBunch_new.Write()

    # cut flow histgrams
    #newFile.cd()
    #from ROOT import TH1F
    #h_evnum=TH1F("evnum","evnum",len(cutName),0,len(cutName))
    #h_weight=TH1F("weight","weight",len(cutName),0,len(cutName))
    #print ("")
    #print ("******Cut Flow for ",outFile, "******")
    #print ("%10s %10s %12s" % ("CutName", "Events", "Weights"))
    #for i in range(len(cutName)):
    #    print ("%10s %10d %12.2f" % (cutName[i], evnum[i], weight[i]))
    #    h_evnum.GetXaxis().SetBinLabel(i+1,cutName[i])
    #    h_evnum.SetBinContent(i+1,evnum[i])
    #    h_weight.GetXaxis().SetBinLabel(i+1,cutName[i])
    #    h_weight.SetBinContent(i+1,weight[i])
    #print ("****************************")
    #print ("")
    #h_evnum.Write()
    #h_weight.Write()
