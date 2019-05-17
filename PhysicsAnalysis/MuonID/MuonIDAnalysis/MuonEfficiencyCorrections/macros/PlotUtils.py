# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#!/usr/bin/env python
import ROOT, math, os
from array import array

def RemoveSpecialChars(In):
    Out = In
    NotWantedChars = ["{", "}", "[", "]", "#", "^", ")", "(", "'", " ", "-"]
    ReplaceChars = [".", ","]
    for C in NotWantedChars:
        Out = Out.replace(C, "")
    for C in ReplaceChars:
        Out = Out.replace(C, "_")
    return Out


class DiagnosticHisto(object):
    def __init__(self, name = "",
                       axis_title = "",
                       bins = -1,
                       bmin = 0.,
                       bmax = -1.e9,
                       bin_width = -1,
                       bdir = None,
                       log_binning = False):
        self.__name = name
        self.__xTitle = axis_title
        self.__min = bmin
        self.__width = bin_width
        self.__entries = 0
        self.__content = {}
        self.__error = {}
        self.__TH1 = None
        self.__TDir = bdir
        
        self.__log_binning = log_binning
        if bins > 0 and not log_binning:
            self.__width = (bmax-bmin)/ bins 
            self.__TH1 = ROOT.TH1D(name, "Diagnostic histogram", bins, bmin, bmax)
            self.__TH1.SetDirectory(bdir) 
        elif bins > 0:
            log_start = math.log(bmin)
            log_end = math.log(bmax)
            log_step = (log_end -log_start) / bins
            binning = array("f", [math.exp(log_start +n*log_step) for n in range(bins+1)])
            self.__TH1 = ROOT.TH1D(name, "Diagnostic histogram", bins, binning)
            self.__TH1.SetDirectory(bdir)
            
    def has_log_binnnig(self): return self.__log_binning
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
        self.__TH1 = ROOT.TH1D(self.name(), "Diagnostic histogram", bins, self.__min, self.__max)
        self.__TH1.SetDirectory(self.__TDir) 
        
        for rng in self.__content.iterkeys():
            bin = self.__TH1.FindBin((rng[1] + rng[0])/2.)
            self.__TH1.SetBinContent(bin, self.__content[rng])
            self.__TH1.SetBinError(bin, math.sqrt(self.__error[rng]))

class PlotUtils(object):
    def __init__(self, size=18, status="Internal", lumi=1., sqrts="13", normalizedToUnity=False):
        self.__Status = status
        self.__Size = size
        self.__Lumi = lumi  # in fb^-1
        self.__SqrtS = sqrts
        self.__VerticalCanvasSplit = 0.4
        self.__Canvas = None
        self.__Pad1 = None        
        self.__Pad2 = None
        self.__Styling = None
        self.__RatStyling = None
        self.__Legend = None
        self.__NLegEnt = 0
        self.__normalizedToUnity = normalizedToUnity
        self.__Objects = []

    def DrawTLatex(self, x, y, text, size=18, font=43, align=11, ndc=True, color=-1):
        tex = ROOT.TLatex()
        tex.SetTextAlign(align)
        tex.SetTextSize(size)
        tex.SetTextFont(font)
        if ndc: tex.SetNDC()
        if color > -1: tex.SetTextColor(color)
        self.__Objects.append(tex)
        tex.DrawLatex(x, y, text)

    def GetStatus(self):
        return self.__Status

    def GetLumi(self):
        return self.__Lumi

    def DrawAtlas(self, x, y, align=11):
        # print "Size is %.2f"%self.__Size
        self.DrawTLatex(x, y, "#font[72]{ATLAS} %s" % self.__Status, self.__Size, 43, align)

    def DrawLumiSqrtS(self, x, y, align=11, lumi=1.):
        if self.__normalizedToUnity: self.DrawTLatex(x, y, "#sqrt{s} = %s TeV" % (self.__SqrtS), self.__Size, 43, align)
        elif self.__Lumi >= 999999999: self.DrawTLatex(x, y, "#sqrt{s} = %s TeV, ?? pb^{-1}" % (self.__SqrtS), self.__Size, 43, align)
        elif self.__Lumi < 1.e-6:
            lumiToPrint = "%.0f" % (self.__Lumi * 1e6)
            self.DrawTLatex(x, y, "#sqrt{s} = %s TeV, %s nb^{-1}" % (self.__SqrtS, lumiToPrint), self.__Size, 43, align)
        elif self.__Lumi < 1.e-3:
            lumiToPrint = "%.0f" % (self.__Lumi * 1e3)
            self.DrawTLatex(x, y, "#sqrt{s} = %s TeV, %s pb^{-1}" % (self.__SqrtS, lumiToPrint), self.__Size, 43, align)
        else:
            lumiToPrint = "%.1f" % (self.__Lumi)
            self.DrawTLatex(x, y, "#sqrt{s} = %s TeV, %s fb^{-1}" % (self.__SqrtS, lumiToPrint), self.__Size, 43, align)

    def DrawSqrtS(self, x, y, align=11):
        self.DrawTLatex(x, y, "#sqrt{s} = %s TeV" % (self.__SqrtS), self.__Size, 43, align)
    
    
    def CreateLegend(self, x1, y1, x2, y2, textsize=22):
        if self.__Legend:
            print "WARNING: There already exists a legend. Will delete the old one"
            del self.__Legend
        self.__Legend = ROOT.TLegend(x1, y1, x2, y2)
        self.__Legend.SetFillStyle(0)
        self.__Legend.SetBorderSize(0)
        self.__Legend.SetTextFont(43)
        self.__Legend.SetTextSize(textsize)
        return self.__Legend

    def GetLegend(self, x1=0, y1=1., x2=1., y2=1.):
        if not self.__Legend: return CreateLegend(x1, y1, x2, y2)
        return self.__Legend

    def AddToLegend(self, Items, Style="FL"):
        if isinstance(Items, list):
            for I in Items:
                self.__AddItemToLegend(I, Style)
        else:
            self.__AddItemToLegend(Items, Style)

    def __AddItemToLegend(self, Item, Style):
        try:
            self.__Legend.AddEntry(Item, Item.GetTitle(), Style)
        except:
            self.__Legend.AddEntry(Item.get(), Item.GetTitle(), Style)

        self.__NLegEnt += 1

    def DrawLegend(self, NperCol=3):
        if not self.__Legend:
            print "No legend has been defined yet"
            return
        N = self.__NLegEnt
        Col = int((N - N % NperCol) / NperCol + (N % NperCol > 0))
        if Col < 1: Col = 1
        self.__Legend.SetNColumns(Col)
        self.__Legend.Draw()

    def DrawSource(self, probe, x, y, align=11):
        self.DrawTLatex(x, y, self.ProbeName(probe), self.__Size, 43, align)

    def Prepare1PadCanvas(self, cname, width=800, height=600, isQuad=False):
        if isQuad: height = width
        if self.__Canvas:
            if self.__Canvas.GetName() != cname: self.__Canvas = None
            else:
                print "INFO Already found a canvas named: " + cname
                return self.__Canvas
        self.__Canvas = ROOT.TCanvas(cname, cname, width, height)
        self.__Canvas.cd()
    
    def GetCanvas(self):
        if not self.__Canvas: print "WARNING: No Canvas has been created yet. Please call CreateXPadCanvas() before"
        return self.__Canvas

    def Prepare2PadCanvas(self, cname, width=800, height=600, isQuad=False, DoLogX=False):
        self.Prepare1PadCanvas(cname, width, height, isQuad)

        self.__Pad1 = ROOT.TPad(cname+"Pad1", cname, 0.0, self.__VerticalCanvasSplit - 0.03, 1.0, 1.0)
        self.__Pad2 = ROOT.TPad(cname+"Pad2", cname, 0.0, 0.0, 1.0, self.__VerticalCanvasSplit - 0.03)
        if DoLogX:
            self.__Pad1.SetLogx()
            self.__Pad2.SetLogx()
        self.__Pad1.SetBottomMargin(0.03)  # set to 0 for space between top and bottom pad
        self.__Pad1.SetTopMargin(0.09)
        self.__Pad1.Draw()
        self.__Pad2.SetTopMargin(0.01)  # set to 0 for space between top and bottom pad
        self.__Pad2.SetBottomMargin(0.35)
        self.__Pad2.SetGridy()
        self.__Pad2.Draw()

    def GetTopPad(self):
        return self.__Pad1
    def GetBottomPad(self):
        return self.__Pad2
    def AdaptLabelsTopPad(self, histos):
        labelscalefact = 1. / (1. - self.__VerticalCanvasSplit)
        for hist in histos:
            hist.GetXaxis().SetTitleSize(labelscalefact * hist.GetXaxis().GetTitleSize())
            hist.GetYaxis().SetTitleSize(labelscalefact * hist.GetYaxis().GetTitleSize())
            hist.GetXaxis().SetLabelSize(labelscalefact * hist.GetXaxis().GetLabelSize())
            hist.GetYaxis().SetLabelSize(labelscalefact * hist.GetYaxis().GetLabelSize())
            hist.GetXaxis().SetTitleOffset(1. / labelscalefact * hist.GetXaxis().GetTitleOffset())
            hist.GetYaxis().SetTitleOffset(1. / labelscalefact * hist.GetYaxis().GetTitleOffset())

    def AdaptLabelsBottomPad(self, histos, scale=1.):
        if self.__VerticalCanvasSplit == 0:
            labelscalefact = 1.
        else:
            labelscalefact = 1. / (scale * self.__VerticalCanvasSplit)
        for hist in histos:
            hist.GetXaxis().SetTitleSize(labelscalefact * hist.GetXaxis().GetTitleSize())
            hist.GetYaxis().SetTitleSize(labelscalefact * hist.GetYaxis().GetTitleSize())
            hist.GetXaxis().SetLabelSize(labelscalefact * hist.GetXaxis().GetLabelSize())
            hist.GetYaxis().SetLabelSize(labelscalefact * hist.GetYaxis().GetLabelSize())
            hist.GetXaxis().SetTitleOffset(2.2 / labelscalefact * hist.GetXaxis().GetTitleOffset())
            hist.GetYaxis().SetTitleOffset(1.05 / labelscalefact * hist.GetYaxis().GetTitleOffset())

    def drawStyling(self, Template, ymin, ymax, TopPad=True, RemoveLabel=False):
        self.__Styling = Template.Clone("Style" + Template.GetName())
        self.__Styling.SetMinimum(ymin)
        self.__Styling.SetMaximum(ymax)

        if RemoveLabel == True: self.__Styling.GetXaxis().SetLabelOffset(10.)
        if TopPad: self.AdaptLabelsTopPad([self.__Styling])
        self.__Styling.Draw("AXIS")

    def addValuesInText(self, histo, on_top=True, off_set_y=0.005):
        plot_range_x = (histo.GetXaxis().GetBinUpEdge(histo.GetNbinsX()) - histo.GetXaxis().GetBinLowEdge(1)) / (
            (1 - self.GetCanvas().GetRightMargin()) - self.GetCanvas().GetLeftMargin())
        plot_range_y = (histo.GetMaximum() - histo.GetMinimum())

        left_margin = self.GetCanvas().GetLeftMargin()
        bottom_margin = 0
        if on_top and self.__Pad1:
            plot_range_y /= (1 - self.__Pad1.GetTopMargin() - self.__Pad1.GetBottomMargin())
            bottom_margin = self.__Pad1.GetBottomMargin()

        elif on_top and self.GetCanvas():
            plot_range_y /= (1 - self.GetCanvas().GetTopMargin() - self.GetCanvas().GetBottomMargin())
            bottom_margin = self.GetCanvas().GetBottomMargin()
        elif not on_top and self.__Pad2:
            plot_range_y /= (1 - self.__Pad2.GetTopMargin() - self.__Pad2.GetBottomMargin())
            bottom_margin = self.__Pad2.GetBottomMargin()

        for i in range(1, histo.GetNbinsX() + 1):
            tex = self.DrawTLatex(
                left_margin + ((histo.GetBinCenter(i) - histo.GetXaxis().GetBinLowEdge(1)) / plot_range_x),
                bottom_margin + (histo.GetBinContent(i) + off_set_y - histo.GetMinimum()) / plot_range_y,
                "%.3f" % (histo.GetBinContent(i)),
                size=10,
                align=21,
                color=histo.GetLineColor(),
            )
    def saveHisto(self, name, outputfiletypes):
        Path = RemoveSpecialChars(name)
        if Path.find("/") != -1: 
            dir_name = Path[ : Path.rfind("/") ]
            os.system("mkdir -p %s"%(dir_name))
        if "png" in outputfiletypes:
            # need a 'pdf' file for using 'convert' to create a 'png'
            if not "pdf" in outputfiletypes:
                outputfiletypes.append("pdf")
            a, b = outputfiletypes.index('pdf'), outputfiletypes.index('png')
            outputfiletypes[b], outputfiletypes[a] = outputfiletypes[a], outputfiletypes[b]
        if self.GetBottomPad(): self.GetBottomPad().RedrawAxis()
        if self.GetTopPad(): self.GetTopPad().RedrawAxis()

        for otype in outputfiletypes:
            if otype != "png":
                self.__Canvas.SaveAs("%s.%s" % (Path, otype))
            else:
                # due to problems in png creation, convert pdfs to pngs manually
                os.system("convert -density 300 %s.pdf %s.png" % (Path, Path))
