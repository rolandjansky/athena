# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import TFile, TH1F

#########################################################################################
def getTH1FsfromFile(ifile,triggerName,antype="Analysis",level="HLT"):
        inputfile=ifile
        histEffEt  = TH1F()
        histEffEta = TH1F()
        histEffPhi = TH1F()
        histEffMu  = TH1F()
        temp1 = TH1F()
        temp2 = TH1F()
        print "HLT/Egamma/"+antype+"/"+triggerName+"/Efficiency/"+level+"/match_et"
        # et eff compututation
        if False:
            temp1 = inputfile.Get("HLT/Egamma/"+antype+"/"+triggerName+"/Efficiency/"+level+"/match_highet")
            temp2 = inputfile.Get("HLT/Egamma/"+antype+"/"+triggerName+"/Efficiency/"+level+"/highet")
        else:
            temp1 = inputfile.Get("HLT/Egamma/"+antype+"/"+triggerName+"/Efficiency/"+level+"/match_et")
            temp2 = inputfile.Get("HLT/Egamma/"+antype+"/"+triggerName+"/Efficiency/"+level+"/et")
        histEffEt = temp1
        histEffEt.Divide(temp1,temp2,1,1,"b")
        # phi eff computation
        temp1 = inputfile.Get("HLT/Egamma/"+antype+"/"+triggerName+"/Efficiency/"+level+"/match_eta")
        temp2 = inputfile.Get("HLT/Egamma/"+antype+"/"+triggerName+"/Efficiency/"+level+"/eta")
        histEffEta = temp1
        histEffEta.Divide(temp1,temp2,1,1,"b")
        # eta eff computation
        temp1 = inputfile.Get("HLT/Egamma/"+antype+"/"+triggerName+"/Efficiency/"+level+"/match_phi")
        temp2 = inputfile.Get("HLT/Egamma/"+antype+"/"+triggerName+"/Efficiency/"+level+"/phi")
        histEffPhi = temp1
        histEffPhi.Divide(temp1,temp2,1,1,"b")
        # mu eff computation
        temp1 = inputfile.Get("HLT/Egamma/"+antype+"/"+triggerName+"/Efficiency/"+level+"/match_mu")
        temp2 = inputfile.Get("HLT/Egamma/"+antype+"/"+triggerName+"/Efficiency/"+level+"/mu")
        histEffMu = temp1
        histEffMu.Divide(temp1,temp2,1,1,"b")
        # set axis labels
        histEffEt.GetYaxis().SetTitle("#epsilon")
        histEffEta.GetYaxis().SetTitle("#epsilon")
        histEffEta.GetXaxis().SetTitle("#eta")
        histEffPhi.GetYaxis().SetTitle("#epsilon")
        histEffPhi.GetXaxis().SetTitle("#phi")
        histEffMu.GetYaxis().SetTitle("#epsilon")
        histEffMu.GetXaxis().SetTitle("#mu")
        return histEffEt, histEffEta, histEffPhi, histEffMu

# for Distributions (also Eff if changed)
def getSingleTH1F(ifile,triggerName,histoname,folder0="Analysis",folder1="Distributions",folder2="Offline"):
    inputfile=ifile
    print "HLT/Egamma/"+folder0+"/"+triggerName+"/"+folder1+"/"+folder2+"/"+histoname
    histogram=inputfile.Get("HLT/Egamma/"+folder0+"/"+triggerName+"/"+folder1+"/"+folder2+"/"+histoname)
    return histogram
