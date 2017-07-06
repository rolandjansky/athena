#!/usr/bin/env python

import os,commands,sys,logging
import ROOT

class myHist:
    def __init__(self,histName,histComment,histBins,histMinX,histMaxX,histFormula,histCondForm):
        self.histName=histName
        self.histComment=histComment
        self.histMinX=histMinX
        self.histMaxX=histMaxX
        self.histFormula=histFormula
        self.histCondForm=histCondForm
        self.histBins=histBins
        

    def setHistName(self,name):
        self.histName=name

    def setHistComment(self,Comment):
        self.histComment=Comment

    def setHistFormula(self,form):
        self.histFormula=form

    def setHistCondForm(self,condform):
        self.histCondForm=condform

    def setHistName(self,minX):
        self.HistMinx=minX

    def setHistName(self,maxX):
        self.histMaxX=maxX

    def setHistBins(self,nbins):
        self.histBins=nbins

    def getHistName(self):
        return self.histName

    def getHistComment(self):
        return self.histComment

    def getHistMinX(self):
        return self.histMinX

    def getHistMaxX(self):
        return self.histMaxX

    def getHistBins(self):
        return self.histBins

    def getHistFormula(self):
        return self.histFormula

    def getHistCondForm(self):
        return self.histCondForm


class DrawHists:
    def __init__(self,log,logLevel):
        if log=='':
            self.logger = logging.getLogger('DrawHists')
            hdlr = logging.FileHandler('DrawHists.log',"w")
            formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
            hdlr.setFormatter(formatter)
            self.logger.addHandler(hdlr)
            self.logger.setLevel(logLevel)
        else:
            self.logger = log
    
    def normH(self,m_hist):
        if m_hist.GetEntries() != 0 :
            scale = 1/m_hist.GetEntries()
            m_hist.Scale(scale)
        else:
            self.logger.error("No entries in ",m_hist.GetTitle())
            
    
    def makeHist(self,ROOTntuple):

        ROOT.gStyle.SetMarkerStyle(8)
        ROOT.gStyle.SetStatW(0.4)
        ROOT.gStyle.SetStatFontSize(0.12)
        ROOT.gStyle.SetLabelSize(0.06)
        ROOT.gStyle.SetOptStat(111110)
        ROOT.gStyle.SetCanvasBorderMode(0)
        ROOT.gStyle.SetPadBorderMode(0)
        ROOT.gStyle.SetPadColor(0)
        ROOT.gStyle.SetCanvasColor(0)
        ROOT.gStyle.SetTitleColor(0)
        ROOT.gStyle.SetStatColor(0)

        # only 3 digits
        ROOT.TGaxis.SetMaxDigits(3);

        myCanvas = ROOT.TCanvas("myCanvas","Cluster",1)
        myCanvas.Divide(2,8)

        file=open('DrawHists.txt','r')
        hists=[]
        n=0
        for line in file:
            i=+1
            myCanvas.cd(n)
            tmp=line.strip().split(',')
            if len(tmp)==7:
                h=myHist(tmp[0],tmp[1],tmp[2],tmp[3],tmp[4],tmp[5],tmp[6])
            else:
                h=myHist(tmp[0],tmp[1],tmp[2],tmp[3],tmp[4],tmp[5],'')
            hists.append(h)
        file.close()
        
        rootFile = ROOT.TFile(ROOTntuple,'READ')
        mytree = ROOT.gDirectory.Get('m_ntuple')
        mytree.UseCurrentStyle()

        hfile = ROOT.TFile("Cluster.hist","RECREATE","Clusters hist")
        for h in hists:
            #self.logger.debug(h.getHistName(),',',h.getHistComment(),',',h.getHistBins(),',',h.getHistMinX(),',',h.getHistBins(),',',h.getHistMaxX())
            m_h = ROOT.TH1F(h.getHistName(),h.getHistComment(),int(h.getHistBins()),float(h.getHistMinX()),float(h.getHistMaxX()))
            mytree.Draw(h.getHistFormula(),h.getHistCondForm())
            mytree.Draw(h.getHistFormula()+'>>'+h.getHistName(),h.getHistCondForm())
            self.normH(m_h)
            hfile.Write()
        
        rootFile.Close()
        
    def initHTMLDraw(self):
        f=open("DrawHistos.html","w")
        f.write("<html><head><title>Draw</title><link rel=\"StyleSheet\" href=\"rtt.css\" type=\"text/css\" /><meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\"><meta http-equiv=\"Content-Language\" content=\"en\"></head>")
        f.write("<body>")
        f.close();
        
    def closeHTMLDraw(self):
        f=open("DrawHistos.html","a")
        f.write("</body>")
        f.write("</html>")



    def Draw(self,histFile):
        self.initHTMLDraw()
        file = ROOT.TFile(histFile)
        for i in file.GetListOfKeys():
            self.DrawHisto(i.ReadObj())
        file.Close()
        self.closeHTMLDraw()

    def Draw2(self,histFile):

        file = ROOT.TFile(histFile)
        mytree = ROOT.gDirectory.Get('analysis')
        
        for i in mytree.GetListOfKeys():
           self.DrawHisto(i.ReadObj()) 
        file.Close()
       

    def DrawHisto(self,hist):
        f=open("DrawHistos.html","a")

        tmp=hist.GetTitle()
        name=""
        for i in tmp:
            if i==' ':
                i='_'
            if i=='(':
                i='z'
            if i==')':
                i='z'
            name+=i

        ROOT.gStyle.SetOptStat(ROOT.kFALSE)
        can=ROOT.TCanvas()
 
        hist.Draw()

        legend=ROOT.TLegend(0.7,0.7,0.89,0.85)
        legend.SetTextFont(72)
        legend.SetTextSize(0.04)
 
        legend.AddEntry(hist,"NEW","l")
        legend.Draw()
 
        can.Modified()
        can.Update()

        epsname=name+".eps"
        gifname=name+".gif"

        can.SaveAs(epsname)

        com1='pstopnm -ppm -xborder 0 -yborder 0 -portrait '+epsname
        com2='ppmtogif '+epsname+'001.ppm >'+gifname
        
        os.system(com1)
        os.system(com2)
        
        f.write("<CENTER><BR>")
        f.write("Name : "+hist.GetTitle()+'<BR>')
        f.write("<BR>")
        f.write("<TABLE border=1>")
        f.write("<TR>")
        f.write("<TD>Entries</TD>")
        f.write("<TD>"+str(hist.GetEntries())+'</TD>')
        f.write("</TR>")      
        f.write("<TR>")
        f.write("<TD>Mean</TD>")
        f.write("<TD>"+str(hist.GetMean())+'</TD>')
        f.write("</TR>")        
        f.write("<TR>")
        f.write("<TD>RMS</TD>")
        f.write('<TD>'+str(hist.GetRMS())+'</TD>')
        f.write("</TR>")       
        f.write("</TABLE>")       
        f.write("<IMG SRC=\""+gifname+'\"><BR>')
        f.write('<HR><BR>')
        f.close()
            
    def run(self):
        #self.makeHist('test.root')
        #res=self.Draw("Cluster.hist");
        res=self.Draw2("test.root");
        
        return res

    
a=DrawHists("",logging.INFO)
a.run()
