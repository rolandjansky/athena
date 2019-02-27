# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#!/usr/bin/env python
import ROOT, math

class DiagnosticHisto(object):
    def __init__(self, name = "",
                       axis_title = "",
                       bins = -1,
                       bmin = 0.,
                       bmax = -1.e9,
                       bin_width = -1,
                       bdir = None):
        self.__name = name
        self.__xTitle = axis_title
        self.__min = bmin
        self.__width = bin_width
        self.__entries = 0
        self.__content = {}
        self.__error = {}
        self.__TH1 = None
        self.__TDir = bdir
        
        if bins > 0:
            self.__width = (bmax-bmin)/ bins 
            self.__TH1 = ROOT.TH1D(name, "Diagnostic histogram", bins, bmin, bmax)
            self.__TH1.SetDirectory(bdir) 
                
    def name(self): return self.__name
    def TH1(self): return self.__TH1
    def fill(self, value, weight=1.):
        self.__entries +=1
        if self.__TH1: self.__TH1.Fill(value, weight)  
        else:
            rng = self.__getInterval(value)
            try: self.__content[rng] += weight
            except: self.__content[rng] = weight            
            try: self.__error[rng] += weight**2
            except: self.__error[rng] = weight**2
    def __getInterval(self,value):
        i =  math.ceil( (value - self.__min) / self.__width)
        return (self.__min +self.__width*(i-1), self.__min+ self.__width*i)
    def write(self):
        self.fixHisto()
        if not self.__TH1: return            
        self.__TH1.GetXaxis().SetTitle(self.__xTitle)
        ### Pull back the overflow
        self.TH1().SetBinContent(self.TH1().GetNbinsX(),self.TH1().GetBinContent(self.TH1().GetNbinsX()+1) + self.TH1().GetBinContent(self.TH1().GetNbinsX()))
        self.TH1().SetBinError(self.TH1().GetNbinsX(), math.sqrt( (self.TH1().GetBinError(self.TH1().GetNbinsX()+1) + self.TH1().GetBinError(self.TH1().GetNbinsX()))**2))
        ### Pull back the underflow
        self.TH1().SetBinContent(1,self.TH1().GetBinContent(0) + self.TH1().GetBinContent(1))
        self.TH1().SetBinError(1, math.sqrt( (self.TH1().GetBinError(0) + self.TH1().GetBinError(1))**2))
        
        self.__TH1.SetEntries(self.__entries)
        if self.__TDir: self.__TDir.WriteObject(self.__TH1, self.__name)
    def max(self):
        try: return sorted(self.__content.iterkeys(), key=lambda Rng: Rng[1])[-1][1]
        except: return float('nan')
    def min(self):
        try: return sorted(self.__content.iterkeys(), key=lambda Rng: Rng[0])[0][1]
        except: return float('nan')
    def setMinimum(self, minimum): self.__min = minimum
    def setMaximum(self, maximum): self.__max = maximum
    def fixHisto(self):
        if self.__TH1: return
        bins = min (int((self.__max - self.__min) / self.__width), 10000)
        print self.name(), bins, self.__max, self.__min
        self.__TH1 = ROOT.TH1D(self.name(), "Diagnostic histogram", bins, self.__min, self.__max)
        self.__TH1.SetDirectory(self.__TDir) 
        
        for rng in self.__content.iterkeys():
            bin = self.__TH1.FindBin((rng[1] + rng[0])/2.)
            self.__TH1.SetBinContent(bin, self.__content[rng])
            self.__TH1.SetBinError(bin, math.sqrt(self.__error[rng]))
        



class Probes:
    ID = "IDProbes"
    Calo = "CaloProbes"
    MStoID = "MSProbes_ID"
    MStoMu = "MSProbes_Muon"
    TrueID = "TrueIDProbes"
    TrueCalo = "TrueCaloProbes"
    TrueMStoID = "TrueMSProbes_ID"
    TrueMStoMu = "TrueMSProbes_Muon"
    TruthToID = "TruthProbes_ID"
    TruthToMu = "TruthProbes_Muon"
    LooseNoProbeIP = "LooseProbes_noProbeIP"
    VeryLooseForHighEta = "VeryLooseProbes_ForHighEta"
class Matches:
    Calo ="CaloTaggedMuons"
    CB ="CombinedMuons"
    Loose ="LooseMuons"
    LooseNoCalo ="LooseMuons_noCaloTag"
    Medium ="MediumMuons"
    Tight ="TightMuons"
    HighPt ="HighPtMuons"
    ID ="IDTracks"
    ID_noMCP ="IDTracks_IDCutCheck_NoIDQualityCut"
    ID_Pix ="IDTracks_IDCutCheck_PixCut"
    ID_Sct ="IDTracks_IDCutCheck_SctCut"
    ID_Trt ="IDTracks_IDCutCheck_TrtCut"
    ID_Holes ="IDTracks_IDCutCheck_SiHolesCut"
    InnerLayerEff ="InnerLayerEff"
    InnerLayerEffGeq1 ="InnerLayerEffGeq1"
    InnerLayerEffGeq2 ="InnerLayerEffGeq2"
    MiddleLayerEff ="MiddleLayerEff"
    OuterLayerEff ="OuterLayerEff"

class PlotUtils:
    def __init__(self,size=18,status="Internal", lumi = "20.3", sqrts = "13"):
        self.Status = status
        self.Size = size
        self.Lumi = lumi # in fb^-1
        self.SqrtS = sqrts
        self.VerticalCanvasSplit = 0.4
        self.Objects = []
        
    def DrawTLatex(self, x,y,text,size=18,font=43,align=11):
        tex = ROOT.TLatex()
        tex.SetTextAlign(align)
        tex.SetTextSize(size)
        tex.SetTextFont(font)
        tex.SetNDC()
        self.Objects.append(tex)
        tex.DrawLatex(x,y,text)
        
    def DrawAtlas(self, x,y,align=11):
        #print "Size is %.2f"%self.Size
        self.DrawTLatex(x,y,"#font[72]{ATLAS} %s"%self.Status,self.Size,43,align)
    def DrawSqrtSOnly(self,x,y,align=11):
        self.DrawTLatex(x,y,"#sqrt{s} = %s TeV"%(self.SqrtS),self.Size,43,align)
    def DrawLumiSqrtS(self,x,y,align=11,lumi="20.3"):
        if self.Lumi >= 999999999:
            self.DrawTLatex(x,y,"#sqrt{s} = %s TeV, ?? pb^{-1}"%(self.SqrtS),self.Size,43,align)
        else:
            if self.Lumi < 0.1:
                lumiToPrint = "%.0f"%(self.Lumi*1e3)
                self.DrawTLatex(x,y,"#sqrt{s} = %s TeV, %s nb^{-1}"%(self.SqrtS, lumiToPrint),self.Size,43,align)
            elif  self.Lumi >= 1000.:
                lumiToPrint = "%.1f"%(self.Lumi*1e-3)
                self.DrawTLatex(x,y,"#sqrt{s} = %s TeV, %s fb^{-1}"%(self.SqrtS, lumiToPrint),self.Size,43,align)
            else:
                lumiToPrint = "%.0f"%(self.Lumi)
                self.DrawTLatex(x,y,"#sqrt{s} = %s TeV, %s pb^{-1}"%(self.SqrtS, lumiToPrint),self.Size,43,align)
    def DrawLumiSqrtSEvolution(self,x,y,align=11,lumi="20.3"):
        self.DrawTLatex(x,y,"#sqrt{s} = %s TeV, %s pb^{-1}"%(self.SqrtS,lumi),self.Size,43,align)
        
    def ProbeName(self,probe):
        text = ""
        if probe == Probes.ID:
            text = "ID Probes"
        if probe == Probes.Calo:
            text = "CaloTag Probes"
        if probe == Probes.MStoID or probe == Probes.MStoMu:
            text = "MS Probes"
        if probe == Probes.TruthToID or probe == Probes.TruthToMu:
            text = "Truth Efficiencies"
        if probe == Probes.LooseNoProbeIP:
            text = "#it{Medium} Muons"
        return text
        
    def DrawSource(self,probe,x,y,align=11):
        self.DrawTLatex(x,y,self.ProbeName(probe),self.Size,43,align)
        
    def MatchName(self,match):
        text = ""
        if match == Matches.Calo:
            text = "CaloTag muons"
        if match == Matches.CB:
            text = "Combined muons"
        if match == Matches.HighPt:
            text = "#it{High-p_{T}} muons"
        if match == Matches.Loose:
            text = "#it{Loose} muons"
        if match == Matches.LooseNoCalo:
            text = "#it{Loose} muons (no CaloTag)"
        if match == Matches.Medium:
            text = "#it{Medium} muons"
        if match == Matches.Tight:
            text = "#it{Tight} muons"
        if match == Matches.ID:
            text = "ID tracks"
        if match == Matches.ID:
            text = "ID tracks"
        if match == Matches.ID_noMCP:
            text = "ID tracks, no MCP criteria"
        if match == Matches.ID_Pix:
            text = "ID tracks, MCP Pixel cut"
        if match == Matches.ID_Sct:
            text = "ID tracks, MCP SCT cut"
        if match == Matches.ID_Trt:
            text = "ID tracks, MCP TRT cut"
        if match == Matches.ID_Holes:
            text = "ID tracks, MCP Si Hole veto"
        return text 
        
    def DrawTarget(self,match,x,y,align=11):
        self.DrawTLatex(x,y,self.MatchName(match),self.Size,43,align)
        
    def DrawRegionLabel(self,region,x,y,align=11):
        text = ""
        if region == DetRegions.Crack:
            text = "|#eta|<0.1"
        elif region == DetRegions.noCrack:
            text = "|#eta|>0.1"
        elif region == DetRegions.Barrel:
            text = "0.1<|#eta|<1.1"
        elif region == DetRegions.Transition:
            text = "1.1<|#eta|<1.3"
        elif region == DetRegions.Endcap:
            text = "1.3<|#eta|<2.0"
        elif region == DetRegions.CSC:
            text = "2.0<|#eta|<2.5"
        self.DrawTLatex(x,y,text,self.Size,43,align)
        
    def DrawLegend (self,histos,x1,y1,x2,y2):
        leg = ROOT.TLegend(x1,y1,x2,y2)
        self.Objects.append(leg)
        leg.SetFillStyle(0)
        leg.SetBorderSize(0)
        leg.SetTextFont(43)
        leg.SetTextSize(24)
        for histo in histos:
            leg.AddEntry(histo[0],histo[0].GetTitle(),histo[1])
        leg.Draw()
        return leg
             
