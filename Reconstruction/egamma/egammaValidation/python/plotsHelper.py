#
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
#

from ROOT import gStyle, TCanvas, TH1, TH1F, kBlack, kBlue, TLegend

def plotComp(HistBase, HistART, padNmb, mycanvas, myleg=None):

    mycanvas.cd(padNmb)
    HistBase.SetMarkerStyle(3);
    HistART.SetLineColor(kBlue);
    HistBase.Draw("hist p");
    HistART.Draw("same");

    if padNmb == 1 and myleg != None:
    
        myleg.SetName("myleg")
        myleg.AddEntry(HistBase, "Baseline", "p")
        myleg.AddEntry(HistART , "ART"     , "l")
        myleg.Draw()    

    pass
