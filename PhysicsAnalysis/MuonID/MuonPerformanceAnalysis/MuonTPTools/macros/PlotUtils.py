# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT, math
from Defs import * 

class PlotUtils:
    def __init__(self,size=0.042,status="Internal", lumi = "20.3", sqrts = "13"):
        self.Status = status
        self.Size = size
        self.Lumi = lumi # in fb^-1
        self.SqrtS = sqrts
        self.VerticalCanvasSplit = 0.4
        self.Objects = []
        
    def DrawTLatex(self, x,y,text,size=0.042,font=42,align=11):
        tex = ROOT.TLatex()
        tex.SetTextAlign(align)
        tex.SetTextSize(size)
        tex.SetTextFont(font)
        tex.SetNDC()
        self.Objects.append(tex)
        tex.DrawLatex(x,y,text)
        
    def DrawAtlas(self, x,y,align=11):
        self.DrawTLatex(x,y,"#font[72]{ATLAS} %s"%self.Status,self.Size,42,align)
    def DrawLumiSqrtS(self,x,y,align=11,lumi="20.3"):
        if self.Lumi < 0.001:
            lumiToPrint = "%.1f"%(self.Lumi*1e6)
            self.DrawTLatex(x,y,"#sqrt{s} = %s TeV, %s nb^{-1}"%(self.SqrtS, lumiToPrint),self.Size,42,align)
        elif  self.Lumi < 1.:
            lumiToPrint = "%.1f"%(self.Lumi*1e3)
            self.DrawTLatex(x,y,"#sqrt{s} = %s TeV, %s pb^{-1}"%(self.SqrtS, lumiToPrint),self.Size,42,align)
        else:
            lumiToPrint = "%.1f"%(self.Lumi)
            self.DrawTLatex(x,y,"#sqrt{s} = %s TeV, %s fb^{-1}"%(self.SqrtS, lumiToPrint),self.Size,42,align)
    def DrawLumiSqrtSEvolution(self,x,y,align=11,lumi="20.3"):
        self.DrawTLatex(x,y,"#sqrt{s} = %s TeV, %s pb^{-1}"%(self.SqrtS,lumi),self.Size,42,align)
    def DrawSource(self,probe,x,y,align=11):
        text = ""
        if probe == Probes.ID:
            text = "ID Probes"
        if probe == Probes.Calo:
            text = "CaloTag Probes"
        if probe == Probes.MStoID or probe == Probes.MStoMu:
            text = "MS Probes"
        if probe == Probes.TruthToID or probe == Probes.TruthToMu:
            text = "Truth Efficiencies"
        self.DrawTLatex(x,y,text,self.Size,52,align)
    def DrawTarget(self,match,x,y,align=11):
        text = ""
        if match == Matches.Calo:
            text = "CaloTag Muons"
        if match == Matches.CB:
            text = "Combined Muons"
        if match == Matches.Loose:
            text = "Loose Muons"
        if match == Matches.LooseNoCalo:
            text = "Loose Muons (no CaloTag)"
        if match == Matches.Medium:
            text = "Medium Muons"
        if match == Matches.Tight:
            text = "Tight Muons"
        if match == Matches.ID:
            text = "ID Tracks"
        self.DrawTLatex(x,y,text,self.Size,42,align)
        
    def DrawLegend (self,histos,x1,y1,x2,y2):
        leg = ROOT.TLegend(x1,y1,x2,y2)
        self.Objects.append(leg)
        leg.SetFillStyle(0)
        leg.SetBorderSize(0)
        leg.SetTextFont(42)
        for histo in histos:
            #if histo.GetFillColor() != ROOT.kBlack:
                #leg.AddEntry(histo,histo.GetTitle(),"F")
            #else:
            leg.AddEntry(histo[0],histo[0].GetTitle(),histo[1])
        leg.Draw()
        
    def Prepare2PadCanvas(self, cname, width=800, height=800):
        can = ROOT.TCanvas(cname,cname,width,height)
        self.Objects.append(can)
        p1 = ROOT.TPad("p1_"+cname,cname,0.0,self.VerticalCanvasSplit,1.0,1.0)
        p2 = ROOT.TPad("p2_"+cname,cname,0.0,0.0,1.0,self.VerticalCanvasSplit)
        p1.SetBottomMargin(0)
        p1.SetTopMargin(0.09)
        p2.Draw()
        p2.SetTopMargin(0)
        p2.SetBottomMargin(0.4)
        p2.SetGridy()
        self.Objects.append(can)
        self.Objects.append(p1)
        self.Objects.append(p2)
        can.cd()
        p1.Draw()
        p2.Draw()
        return [can,p1,p2]
    
    def AdaptLabelsTopPad (self,histos):
        labelscalefact = 1. / (1. - self.VerticalCanvasSplit)
        for hist in histos:
            hist.GetXaxis().SetTitleSize(labelscalefact * hist.GetXaxis().GetTitleSize())
            hist.GetYaxis().SetTitleSize(labelscalefact * hist.GetYaxis().GetTitleSize())
            hist.GetXaxis().SetLabelSize(labelscalefact * hist.GetXaxis().GetLabelSize())
            hist.GetYaxis().SetLabelSize(labelscalefact * hist.GetYaxis().GetLabelSize())
            hist.GetXaxis().SetTitleOffset(1./labelscalefact * hist.GetXaxis().GetTitleOffset())
            hist.GetYaxis().SetTitleOffset(1./labelscalefact * hist.GetYaxis().GetTitleOffset())
    
    def AdaptLabelsBottomPad (self,histos):
        labelscalefact = 1. / (self.VerticalCanvasSplit)
        for hist in histos:
            hist.GetXaxis().SetTitleSize(labelscalefact * hist.GetXaxis().GetTitleSize())
            hist.GetYaxis().SetTitleSize(labelscalefact * hist.GetYaxis().GetTitleSize())
            hist.GetXaxis().SetLabelSize(labelscalefact * hist.GetXaxis().GetLabelSize())
            hist.GetYaxis().SetLabelSize(labelscalefact * hist.GetYaxis().GetLabelSize())
            #hist.GetXaxis().SetTitleOffset(1./labelscalefact * hist.GetXaxis().GetTitleOffset())
            hist.GetYaxis().SetTitleOffset(1.1/labelscalefact * hist.GetYaxis().GetTitleOffset())
            
    def SetFancyAxisRanges_SF (self,histos,minmax=1.0,maxmin=0.9,fixRange=False):
        ymin = maxmin
        ymax = minmax
        
        finalRanges = []
        
        if fixRange:
            for histo in histos:
                histo.GetYaxis().SetRangeUser(ymin - (0.5 * abs(1. - ymin)),ymax + 0.5 * abs(ymax - 1))
                #histo.GetYaxis().SetRangeUser(0.925,1.075)
                #histo.GetYaxis().SetRangeUser(0.4,1.6)
                histo.GetYaxis().SetNdivisions(3,ROOT.kTRUE)
            # not yet clear yet how to apply this on the y-axis only
            ROOT.TGaxis.SetMaxDigits(3)
            finalRanges.append([ymin,ymax])
        else:
        
            for histo in histos:
                if histo.InheritsFrom("TGraphAsymmErrors"):
                    if (ROOT.TMath.MaxElement(histo.GetN(),histo.GetY())<1.4) and (ROOT.TMath.MaxElement(histo.GetN(),histo.GetY()) > ymax):
                        ymax = ROOT.TMath.MaxElement(histo.GetN(),histo.GetY())
                    if (ROOT.TMath.MinElement(histo.GetN(),histo.GetY())>0.6) and (ROOT.TMath.MinElement(histo.GetN(),histo.GetY()) < ymin):
                        ymin = ROOT.TMath.MinElement(histo.GetN(),histo.GetY())
                else:
                    if histo.GetMaximum(1.4) > ymax:
                        ymax = histo.GetMaximum(1.4)
                    if histo.GetMinimum(0.6) < ymin:
                        ymin = histo.GetMinimum(0.6)
                    
            GoodStops = [0.0,0.3,0.4,0.5,0.65,0.7,0.75,0.8,0.9,0.95,0.98,0.99,0.995,1.005,1.01,1.02,1.05,1.1,1.2,1.25,1.3,1.35,1.5,1.6,1.7,1.72]
            
            ymaxl = [p for p in GoodStops  if p + 0.5 * abs(p - 1) > ymax]
            if len (ymaxl) > 0:
                ymax = ymaxl[0]
            yminl = [p for p in GoodStops if p- 0.5 * abs(p - 1)  < ymin ]
            if len (yminl) > 0:
                ymin = yminl[-1]
                            
            ymin = 1. - max (abs(1.-ymax), abs(1.-ymin))
            ymax = 1. + (1.-ymin)
            
            for histo in histos:
                histo.GetYaxis().SetRangeUser(ymin - (0.5 * abs(1. - ymin)),ymax + 0.5 * abs(ymax - 1))
                histo.GetYaxis().SetNdivisions(3,ROOT.kTRUE)
            # not yet clear yet how to apply this on the y-axis only
            ROOT.TGaxis.SetMaxDigits(3)
            finalRanges.append([ymin,ymax])
        return finalRanges
    
    def SetFancyAxisRanges_SF_Probes (self,histos,minmax=1.0,maxmin=0.9,fixRange=False):
        ymin = maxmin
        ymax = minmax
        
        finalRanges = []
        
        if fixRange:
            if ymin < .334:
                histo.GetYaxis().SetRangeUser(ymin - 0.05,ymax + 0.05)
            else:
                for histo in histos:
                    histo.GetYaxis().SetRangeUser(ymin - (0.5 * abs(1. - ymin)),ymax + 0.5 * abs(ymax - 1))
                    histo.GetYaxis().SetNdivisions(3,ROOT.kTRUE)
                    # show exponent (smaller numbers) but shift it away
                    histo.GetXaxis().SetNoExponent(ROOT.kFALSE)
            # not yet clear yet how to apply this on the y-axis only
            ROOT.TGaxis.SetMaxDigits(3)
            ROOT.TGaxis.SetExponentOffset(1.)
            finalRanges.append([ymin,ymax])
        else:
            for histo in histos:
                histo.GetXaxis().SetNoExponent(ROOT.kTRUE)
                if histo.InheritsFrom("TGraphAsymmErrors"):
                    if (ROOT.TMath.MaxElement(histo.GetN(),histo.GetY())<1.9) and (ROOT.TMath.MaxElement(histo.GetN(),histo.GetY()) > ymax):
                        ymax = ROOT.TMath.MaxElement(histo.GetN(),histo.GetY())
                    if (ROOT.TMath.MinElement(histo.GetN(),histo.GetY())>0.1) and (ROOT.TMath.MinElement(histo.GetN(),histo.GetY()) < ymin):
                        ymin = ROOT.TMath.MinElement(histo.GetN(),histo.GetY())
                else:
                    if histo.GetMaximum(1.9) > ymax:
                        ymax = histo.GetMaximum(1.9)
                    if histo.GetMinimum(0.1) < ymin:
                        ymin = histo.GetMinimum(0.1)
                    
            GoodStops = [0.0,0.1,0.3,0.4,0.5,0.65,0.7,0.75,0.8,0.9,0.95,0.98,0.99,0.995,1.005,1.01,1.02,1.05,1.1,1.2,1.25,1.3,1.35,1.5,1.6,1.7,1.9,1.72]
            
            ymaxl = [p for p in GoodStops  if p + 0.5 * abs(p - 1) > ymax]
            if len (ymaxl) > 0:
                ymax = ymaxl[0]
            yminl = [p for p in GoodStops if p- 0.5 * abs(p - 1)  < ymin ]
            if len (yminl) > 0:
                ymin = yminl[-1]
                            
            ymin = 1. - max (abs(1.-ymax), abs(1.-ymin))
            ymax = 1. + (1.-ymin)
            
            for histo in histos:
                histo.GetYaxis().SetRangeUser(ymin - (0.5 * abs(1. - ymin)),ymax + 0.5 * abs(ymax - 1))
                histo.GetYaxis().SetNdivisions(3,ROOT.kTRUE)
            # not yet clear yet how to apply this on the y-axis only
            #ROOT.TGaxis.SetMaxDigits(2)
            finalRanges.append([ymin,ymax])
        return finalRanges

    def SetFancyAxisRanges_Eff (self,histos,isSym=False,maxmin=0.9,fixRange=False):
        ymin = maxmin
        finalRanges = []

        if fixRange:
            for histo in histos:
                histo.GetYaxis().SetRangeUser(ymin,1.001)
                histo.GetYaxis().SetNdivisions(4)
                histo.GetXaxis().SetNoExponent(ROOT.kTRUE)
            finalRanges.append([ymin])
        else:
            for histo in histos:
                histo.GetXaxis().SetNoExponent(ROOT.kTRUE)
                if histo.InheritsFrom("TGraphAsymmErrors"):
                    if (ROOT.TMath.MinElement(histo.GetN(),histo.GetY())>0.4) and (ROOT.TMath.MinElement(histo.GetN(),histo.GetY()) < ymin):
                        ymin = ROOT.TMath.MinElement(histo.GetN(),histo.GetY())
                else:
                    if histo.GetMinimum(0.4) < ymin:
                        ymin = histo.GetMinimum(0.4)
                
            GoodStops = [0.01,0.401,0.501,0.601,0.701,0.801,0.851,0.931]

            yminl = [p for p in GoodStops if p < ymin]
            if len (yminl) > 0:
                ymin = yminl[-1]

            for histo in histos:
                histo.GetYaxis().SetRangeUser(ymin,1.001)
                histo.GetYaxis().SetNdivisions(4)
            finalRanges.append([ymin])

        return finalRanges

    def SetFancyAxisRanges (self,histos,maxmin=0.2,minmax=1.01,fixRange=False):
        ymin = maxmin
        ymax = minmax
        finalRanges = []

        if fixRange:
            for histo in histos:
                histo.GetYaxis().SetRangeUser(ymin,ymax)
            
        else:
            for histo in histos:
                if histo.GetMinimum(0.4) < ymin:
                    ymin = histo.GetMinimum(0.4)
                if histo.GetMaximum()>ymax:
                    ymax = histo.GetMaximum()*1.5

            GoodStops = [0.01,0.401,0.501,0.601,0.701,0.801,0.901]

            yminl = [p for p in GoodStops if p < ymin]
            if len (yminl) > 0:
                ymin = yminl[-1]

            for histo in histos:
                histo.GetYaxis().SetRangeUser(ymin,ymax)
            finalRanges.append([ymin,ymax])
            
        return finalRanges
            
