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
    

class HistosComparison:
    def __init__(self,log,logLevel):
        if log=='':
            self.logger = logging.getLogger('HistosComparison')
            hdlr = logging.FileHandler('HistosComparison.log',"w")
            formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
            hdlr.setFormatter(formatter)
            self.logger.addHandler(hdlr)
            self.logger.setLevel(logLevel)
        else:
            self.logger = log
        self.filePar="filePar"
    
    def normH(self,m_hist):
        if m_hist.GetEntries() != 0 :
            scale = 1/m_hist.GetEntries()
            m_hist.Scale(scale)
        else:
            self.logger.error("No entries in ",m_hist.GetTitle())

    def getPar(self,name,par):
        com="grep \"RTTParm_almostIdentical_"+name+"_"+par+"\"  "+self.filePar+ "| cut -d= -f2 | tail -n1"
        print com
        res,val=commands.getstatusoutput(com)
        if val=="":
            val="notfound"
        if res!=0:
            self.logger.error('Problem to get the par of '+name+' : '+max)
            val="notfound"
            
        return val       
    
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

        file=open('HistoComparison.txt','r')
        hists=[]
        n=0
        for line in file:
            i=+1
            myCanvas.cd(n)
            tmp=line.strip().split(',')

            name2=""
            for i in tmp[1]:
                if i==' ':
                    i='_'
                if i=='(':
                    i='z'
                if i==')':
                    i='z'
                name2+=i

            # check bin x
            xmin=self.getPar(name2,"xmin")
            if xmin=="notfound":
                xmin=tmp[3]

            xmax=self.getPar(name2,"xmax")
            if xmax=="notfound":
                xmax=tmp[4]
            
            print "XMIN =", xmin
            print "XMAX =", xmax

            
            if len(tmp)==7:
                h=myHist(tmp[0],tmp[1],tmp[2],xmin,xmax,tmp[5],tmp[6])
            else:
                h=myHist(tmp[0],tmp[1],tmp[2],xmin,xmax,tmp[5],'')
            hists.append(h)
        file.close()
        
        rootFile = ROOT.TFile(ROOTntuple,'READ')
        mytree = ROOT.gDirectory.Get('CollectionTree')
        mytree.UseCurrentStyle()

        hfile = ROOT.TFile("Cluster.hist","RECREATE","Clusters hist")
        for h in hists:
            self.logger.debug(h.getHistName(),',',h.getHistComment(),',',h.getHistBins(),',',h.getHistMinX(),',',h.getHistBins(),',',h.getHistMaxX())
            m_h = ROOT.TH1F(h.getHistName(),h.getHistComment(),int(h.getHistBins()),float(h.getHistMinX()),float(h.getHistMaxX()))
            mytree.Draw(h.getHistFormula(),h.getHistCondForm())
            mytree.Draw(h.getHistFormula()+'>>'+h.getHistName(),h.getHistCondForm())
            self.normH(m_h)
            hfile.Write()
        
        rootFile.Close()
        
    def getFilePar(self):
        com="grep RTTINFO MYRTTINFOS.txt | cut -d= -f2 | tail -n1"
        res,testName=commands.getstatusoutput(com)
        if res!=0 :
            self.logger.error(testName)
            return -1
        
        nTestName=''
        for i in testName:
            if i !=' ':
                nTestName+=i;

        filePar=nTestName+".par";
        com="wget http://cern.ch/CaloRTT/"+filePar;
        print com
        res,out=commands.getstatusoutput(com)
        if res!=0 :
            self.logger.error(out)
            return -1
        return filePar


    def getFileRef(self):
        com="grep RTTINFO MYRTTINFOS.txt | cut -d= -f2 | tail -n1"
        res,testName=commands.getstatusoutput(com)
        if res!=0 :
            self.logger.error(testName)
            return -1
        
        nTestName=''
        for i in testName:
            if i !=' ':
                nTestName+=i;

        fileRef='ref'+nTestName+".hist"
        com="wget http://cern.ch/CaloRTT/"+fileRef
        res,out=commands.getstatusoutput(com)
        if res!=0 :
            self.logger.error(out)
            return -1
        return fileRef

    def initHTMLDraw(self):
        f=open("DrawHistos.html","w")
        f.write("<html><head><title>Draw</title><link rel=\"StyleSheet\" href=\"rtt.css\" type=\"text/css\" /><meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\"><meta http-equiv=\"Content-Language\" content=\"en\"></head>")
        f.write("<body>")
        f.close();
        
    def closeHTMLDraw(self):
        f=open("DrawHistos.html","a")
        f.write("</body>")
        f.write("</html>")


    def initHTMLComparison(self):
        f=open("Comparison.html","w")
        f.write("<html><head><title>Draw</title><link rel=\"StyleSheet\" href=\"rtt.css\" type=\"text/css\" /><meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\"><meta http-equiv=\"Content-Language\" content=\"en\"></head>")
        f.write("<body>")
        f.close();
        
    def closeHTMLComparison(self):
        f=open("Comparison.html","a")
        f.write("</body>")
        f.write("</html>")

    def Draw(self,histFile):
        self.initHTMLDraw()
        file = ROOT.TFile(histFile)
        for i in file.GetListOfKeys():
            self.DrawHisto(i.ReadObj())
        file.Close()
        self.closeHTMLDraw()


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

    def almostIdentical(self,hist,refhist,filePar):
        f=open("Comparison.html","a")

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

        hist.SetLineColor(2)
        hist.SetLineStyle(2)
        hist.Draw()
        if hist:
            refhist.Draw("sames")
        else:
            refhist.Draw()

        legend=ROOT.TLegend(0.7,0.7,0.89,0.85)
        legend.SetTextFont(72)
        legend.SetTextSize(0.04)
 
        legend.AddEntry(hist,"NEW","l")
        legend.AddEntry(refhist,"REF","l");
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

        restxt=open('Comparison.txt','a')
        
        f.write("<CENTER><BR>")
        f.write("Name : "+hist.GetTitle()+'<BR>')
        f.write("<BR>")
        f.write("<TABLE border=1>")
        f.write("<TR>")
        f.write("<TD></TD>")
        f.write("<TD>REF</TD>");
        f.write("<TD>NEW</TD>")
        f.write("</TR>")
        
        f.write("<TD>Entries</TD>")
        f.write("<TD>"+str(refhist.GetEntries())+'</TD>')
        f.write("<TD>"+str(hist.GetEntries())+'</TD>')
        f.write("</TR>")      
        f.write("<TR>")
        f.write("<TD>Mean</TD>")
        f.write("<TD>"+str(refhist.GetMean())+'</TD>')
        f.write("<TD>"+str(hist.GetMean())+'</TD>')
        f.write("</TR>")        
        f.write("<TR>")
        f.write("<TD>RMS</TD>")
        f.write('<TD>'+str(refhist.GetRMS())+'</TD>')
        f.write('<TD>'+str(hist.GetRMS())+'</TD>')
        f.write("</TR>")       
        f.write("</TABLE>")       
        f.write("<IMG SRC=\""+gifname+'\"><BR>')

        com="grep \"RTTParm_almostIdentical_"+name+ " \"  "+filePar+ "| cut -d= -f2 | tail -n1"
        res,max=commands.getstatusoutput(com)
        if res!=0:
            self.logger.error('Problem to the par of '+name+' : '+max)
            return -1

        dA=0.
        A=0.
        for k  in range(0,hist.GetNbinsX()+1):
            dA += float(hist.GetBinContent(k) - refhist.GetBinContent(k))
            A += hist.GetBinContent(k)

        if (A > 0):
            dA = dA / A;

  
        restxt.write('&Sigma;(|&Delta;A|)/&Sigma;A is '+str(dA)+' (cut is '+str(max)+')\n')
        f.write("<BR>")
        f.write('&Sigma;(|&Delta;A|)/&Sigma;A is '+str(dA)+' (cut is '+str(max)+')\n')
        f.write("<BR>")
 
        if ( dA <= max ):
            restxt.write("TEST OK\n")
            restxt.write("###################################\n")
            f.write("<font color=\"green\">TEST OK<BR></font>")
            f.write("<HR>")
            f.write("</CENTER><BR>")
            f.close()
            restxt.close()
            return 0
        else:
            restxt.write("TEST FAILED\n")
            restxt.write("###################################\n")
            f.write("<font color=\"red\">TEST FAILED</font><BR>")
            f.write("<HR>")
            f.write("</CENTER><BR>")
            f.close()
            restxt.close()
            return -1


    def testHistogram(self,hists,refhists,compType,filePar):
        self.logger.info('---- testHistogram '+str(hists.GetTitle())+' ----')
        if compType == "almostIdentical":
            res=self.almostIdentical(hists,refhists,filePar)
            return res
        else:
            self.logger.error('Comp type not known')
            return -1
        return 0

    def histComparison(self,hist,refhist,compType,filePar):
        self.initHTMLComparison()
        fhists = ROOT.TFile(hist); 
        frefhists = ROOT.TFile(refhist);

        if len(fhists.GetListOfKeys())!=len(frefhists.GetListOfKeys()):
            self.logger.warning('the ref and new file doesnt contain the same number of histograms')
            self.Draw('Cluster.hist')
            return 0
            
        if len(fhists.GetListOfKeys())==0:
            self.logger.error('No histograms in the new file')
            return -1

        failed=[]
        hists=[]
        refHists=[]
        for i in fhists.GetListOfKeys():
            hists.append(i.ReadObj())
        for i in frefhists.GetListOfKeys():
            refHists.append(i.ReadObj())

        for k in range(1,len(hists)):
            if self.testHistogram(hists[k],refHists[k],compType,filePar)!=0:
                failed.append(hists[k].GetName())
                self.logger.error(hists[k].GetTitle()+' FAILED')
            else:
                self.logger.info(hists[k].GetTitle()+' OK')
            self.logger.info('---- testHistogram DONE ----')
            self.closeHTMLComparison()  
        if len(failed)==0:
            return 0
        else:
            return -1
            
    def run(self):
        self.filePar=self.getFilePar()
        if self.filePar==-1:
            self.logger.error('getFilePar() failed')
            return -1
        
        self.makeHist('ntuple.root')

        
        fileRef=self.getFileRef()
        if fileRef==-1:
            self.logger.warning('getFileRef() failed')
            self.logger.info('drawing the histos instead')
            self.Draw('Cluster.hist')
            return 0
        
        res=self.histComparison('Cluster.hist',fileRef,"almostIdentical");
        return res
#a=HistosComparison(logging.INFO)
#a.run()
