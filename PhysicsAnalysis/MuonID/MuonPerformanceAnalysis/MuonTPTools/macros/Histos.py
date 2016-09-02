# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Defs import *
import Utils
import sys
import ROOT
import math
import DSConfig
import HistoDefs
import ParseGRL
graphList = []

## This class provides the basic functionality to read individual histos
class TPHisto:

    ## normal init: based on conent parameters
    def __init__ (self, name,probe,match,region,chargeprod,kind,var, histo=None,infile=None,doClosure=False,LumiScale=-1,useAsymEff=True,analysis=Analysis.Z_Reco,GRLHandler=None):
        self.Name = name
        self.Analysis = analysis
        self.Probe = probe
        self.Match = match
        self.Region = region
        self.ChargeProd = chargeprod
        self.Kind = kind
        self.Var = var
        self.Histo = histo
        self.Infile = infile
        self.DoClosure = doClosure
        self.LumiScale = LumiScale
        self.UseAsymEff = useAsymEff
        self.GRLHandler = GRLHandler
        
        if self.Histo == None and self.Infile != None:
            self.Load()

    @classmethod
    def Copy (out, other):
        return out(name=other.Name+"Clone",probe=other.Probe,match=other.Match,region=other.Region,chargeprod=other.ChargeProd,kind=other.Kind,var=other.Kind,histo=other.Histo.Clone("CloneOf%s"%other.Histo.GetName()),infile=other.Infile,doClosure=other.DoClosure,LumiScale=other.LumiScale,useAsymEff=other.UseAsymEff,analysis=other.Analysis)
    ## create instance from an existing histo
    @classmethod
    def FromHist (out,name,h_in,fin=0):
        if h_in == None:
            return
        fname = h_in.GetName()
        Args = fname.split("_")
        kindcand = Args[5]
        kind = ""
        for cand in [bla for bla in PlotKinds.__dict__.keys() if not "__" in bla ]:
            if cand in kindcand:
                kind = cand
        var = kindcand.split(cand)[-1]
        hcl = h_in.Clone("Cl_"+name+"_"+h_in.GetName())

        return out(name=name,probe = Args[0],region=Args[2], chargeprod = Args[3],match=Args[4],kind=kind,var=var,histo=hcl,infile=fin)

    def Add (self, other, weight = 1.):
        self.Histo.Add(other.Histo, weight)
    # Utility methods for I/O purposes
    def GetPlotName (self,spacer=""):
        #return "_".join([self.Probe,self.Probe,self.Region,self.ChargeProd,self.Match,self.Kind])+self.Var
        return "_".join([self.Probe,self.Region,self.Probe,self.ChargeProd,self.Match,self.Kind])+spacer+self.Var
    def GetTreeName(self):
        return "_".join(["TPTree",self.Probe,self.ChargeProd])
    def GetCFName (self):
        #return "_".join([self.Probe,self.Probe,self.Region,self.ChargeProd,self.Match,self.Kind])+self.Var
        return "_".join([self.Probe,DetRegions.All,self.Probe,self.ChargeProd,"CutFlowsMainSelection"])
    def GetTreePath(self):
        return "%s/Trees/"%self.Analysis+"_".join([self.Probe,self.ChargeProd])
    def GetPlotPath(self):
        return  "/".join([self.Analysis,self.Probe+"_"+self.Region,self.Probe+"_"+self.ChargeProd,self.Match,self.Kind])
    def GetFullPath(self):
        return self.GetPlotPath()+"/"+self.GetPlotName()
    def GetCFPath(self):
        return  "/".join([self.Analysis,self.Probe+"_"+DetRegions.All,self.Probe+"_"+self.ChargeProd,"CutFlows"])
    def GetFullTreePath(self):
        return self.GetTreePath()+"/"+self.GetTreeName()
    def GetFullCFPath(self):
        return self.GetCFPath()+"/"+self.GetCFName()
    # Write the histo to a file
    def Write(self, File = None, Dir = None):
        if File == None:
            File = self.Infile
        if Dir == None:
            Dir = self.GetPlotPath()
        if not File == None or self.Histo == None:
            dir = ROOT.gDirectory.GetName()
            File.cd()
            subdirs = Dir.split("/")
            for subdir in subdirs:
                if not ROOT.gDirectory.GetDirectory(subdir,False):
                    ROOT.gDirectory.mkdir(subdir)
                    if not ROOT.gDirectory.cd(subdir):
                        print "Writing failed - unable to walk through directory structure "+dir
            self.Histo.Write()
            File.cd("")
    # Load the histo from a file
    def Load(self, File=None, Dir=None):
        if (self.UseAsymEff) and not (HistoDefs.initHisto(self.Var.replace("_posq","").replace("_negq",""))[0].InheritsFrom("TH2") or HistoDefs.initHisto(self.Var.replace("_posq","").replace("_negq",""))[0].InheritsFrom("TH2Poly")):
            if self.Kind == PlotKinds.Efficiency:
                graph = ROOT.TGraphAsymmErrors()
                graph.SetName(self.Name+"graph")
                self.Histo = graph
                graphList.append(graph)
                return
        bla = HistoDefs.initHisto(self.Var)
        #bla.SetName(self.GetPlotName())
        has_special_setup = False
        if not bla == None:
            if bla[2] != "":
                has_special_setup = True
        
        
        if File == None:
            File = self.Infile
        if Dir == None:
            Dir = self.GetPlotPath()
        h = File.Get(Dir+"/"+self.GetPlotName())
        if h == None:
            h = File.Get(Dir+"/"+self.GetPlotName("_"))
        if has_special_setup or h == None or self.DoClosure or self.LumiScale!=-1:
            self.ExtractFromTree(File,LumiScale=self.LumiScale)
        else:
            self.ExtractDirect(File)
    def ExtractDirect (self,File=None,Dir=None):
        if File == None:
            File = self.Infile
        if Dir == None:
            Dir = self.GetPlotPath()
        h = File.Get(Dir+"/"+self.GetPlotName())
        if h == None:
            h = File.Get(Dir+"/"+self.GetPlotName("_"))
        if h == None or not hasattr(h,"GetNbinsX"):
            print "Unable to load histo "+Dir+"/"+self.GetPlotName()
        else:
            self.Histo = h.Clone("Cl_"+self.Name+"_"+h.GetName())
            if self.Var == "DetRegions":
                #h2 = HistoSetups["DetRegions"][0].Clone(self.Histo.GetName()+"2")
                h2 = HistoDefs.initHisto("DetRegions")[0].Clone(self.Histo.GetName()+"2")
                h2.GetXaxis().SetTitleSize(self.Histo.GetXaxis().GetTitleSize())
                h2.GetXaxis().SetTitleOffset(self.Histo.GetXaxis().GetTitleOffset())
                h2.GetXaxis().SetLabelSize(self.Histo.GetXaxis().GetLabelSize())
                h2.GetXaxis().SetLabelOffset(self.Histo.GetXaxis().GetLabelOffset())
                h2.GetYaxis().SetTitleSize(self.Histo.GetYaxis().GetTitleSize())
                h2.GetYaxis().SetTitleOffset(self.Histo.GetYaxis().GetTitleOffset())
                h2.GetYaxis().SetLabelSize(self.Histo.GetYaxis().GetLabelSize())
                h2.GetYaxis().SetLabelOffset(self.Histo.GetYaxis().GetLabelOffset())
                h2.GetXaxis().SetTitle(self.Histo.GetXaxis().GetTitle())
                h2.GetYaxis().SetTitle(self.Histo.GetYaxis().GetTitle())
                for i in range (1, self.Histo.GetNbinsX()+1):
                    if (i > h2.GetNbinsX()):
                        h2.SetBinContent(h2.GetNbinsX(),h2.GetBinContent(h2.GetNbinsX())+self.Histo.GetBinContent(i))
                        h2.SetBinError(h2.GetNbinsX(),math.sqrt(h2.GetBinError(h2.GetNbinsX())**2+self.Histo.GetBinError(i)**2))
                    else:
                        h2.SetBinContent(i,self.Histo.GetBinContent(i))
                        h2.SetBinError(i,self.Histo.GetBinError(i))
                        
                self.Histo = h2
            self.Histo.SetDirectory(0)
            #print "Note: Reading %s directly"%self.GetPlotName()
    def PassesCuts(self, tree, eventNumber, cutlist=[]):
        
        if not self.GRLHandler == None:
            if not self.GRLHandler.HasRunLumiBlock(getattr(tree,"runNumber"),getattr(tree,"lumiblock")):
                #print 'did not pass'
                return 0.
            #print 'passed'
        
        weight = 1.
        # get current event and check cuts
        tree.GetEntry(eventNumber)
        for cut in cutlist:
            if "weight" in cut:
                #print "event %s: weight %s: %s"%(eventNumber,cut,getattr(tree,cut))
                weight = weight*float(getattr(tree,cut))
            if cut != "1":
                if ">=" in cut or "<=" in cut:
                    print "%s in cuts currently not supported, take </> instead"%cut
                if ">" in cut:
                    if "abs[" in cut:
                        cut = cut.replace("abs[", "")
                        if math.fabs(getattr(tree,cut.split(">")[0])) <= float(cut.split(">")[1]):
                            return 0.
                    else:
                        if getattr(tree,cut.split(">")[0]) <= float(cut.split(">")[1]):
                            return 0.
                elif "<" in cut:
                    if "abs[" in cut:
                        cut = cut.replace("abs[", "")
                        if math.fabs(getattr(tree,cut.split("<")[0])) >= float(cut.split("<")[1]):
                            return 0.
                    else:
                        if getattr(tree,cut.split("<")[0]) >= float(cut.split("<")[1]):
                            return 0.
                else:
                    #print "event %s: cut %s: %s"%(eventNumber,cut,getattr(tree,cut))
                    if getattr(tree,cut) != 1:
                        return 0.

        return weight

    # reverse engineer from the existing histo
    def ExtractFromTree(self, File=None, Dir=None, LumiScale=-1):
        if File == None:
            File = self.Infile
        if Dir == None:
            Dir = self.GetTreePath()

        h = HistoDefs.initHisto(self.Var.replace("_posq","").replace("_negq",""))[0].Clone(self.GetPlotName())
        if h == None:
            print "Unsupported histo requested from tree: %s"%(self.Var)
            return
        h.SetName(self.GetPlotName())
        print Dir+"/"+self.GetTreeName()
        # what to do here: TH2F plots need PlotKinds.Efficiency!
#         if self.Kind != PlotKinds.Efficiency:

        t = File.Get(Dir+"/"+self.GetTreeName())
        try:
            ntodo = t.GetEntries()
            if LumiScale > 0:
                ntodo = int(ntodo * LumiScale)
        except:
            print "Failed to load %s"%self.GetTreeName()
            throw
        if not h.InheritsFrom("TH2Poly"):
            h.Sumw2()
        h.GetXaxis().SetLabelSize(0.05)
        h.GetXaxis().SetTitleSize(0.05)
        h.GetXaxis().SetTitleOffset(1.4)
        h.GetYaxis().SetLabelSize(0.05)
        h.GetYaxis().SetTitleSize(0.05)
        h.GetYaxis().SetTitleOffset(1.4)

        tmpVar = self.Var
        
        cuts = "(%s) * (%s)"%(HistoDefs.initHisto(self.Var.replace("_posq","").replace("_negq",""))[2], ExtraCuts[self.Region])
        if "TruthProbes" in self.Probe:
            cuts = "(%s)"%(ExtraCuts[self.Region])
            
        if "_posq" in tmpVar:
            #print tmpVar
            cuts = "(%s) * ( probe_q > 0 )"%cuts
            tmpVar = ((self.Var).split('_posq'))[0]
            #print tmpVar
        elif "_negq" in tmpVar:
            #print tmpVar
            cuts = "(%s) * ( probe_q < 0 )"%cuts
            tmpVar = ((self.Var).split('_negq'))[0]
            #print tmpVar
        #if len((self.Var).split('_'))>1:
            #if "posq" in ((self.Var).split('_'))[1]:
                #cuts = "(%s) * ( q > 0)"%cuts
            #elif "negq" in ((self.Var).split('_'))[1]:
                #cuts = "(%s) * ( q < 0)"%cuts
            #else:
                #print 'Error: During reading from tree, unknown variable detected: %s'%(((self.Var).split('_'))[1])
            #tmpVar = ((self.Var).split('_'))[0]

        #if not "Truth" in self.Probe:
        if not h.InheritsFrom("TH2"):
            h.GetYaxis().SetTitle("Probes")
        if self.Kind == PlotKinds.Matches:
            cuts = "(%s) * (probe_matched_%s)"%(cuts,self.Match)
            #print cuts
            if self.DoClosure:
                matchstr = self.Match
                if matchstr == "LooseMuons_noCaloTag":
                    matchstr = "LooseMuons"
                if cuts != "":
                    cuts = "(%s) * (probe_scale_factor_%s)"%(cuts,matchstr)
                else:
                    cuts = "probe_scale_factor_%s"%(matchstr)
                #print cuts
            if not h.InheritsFrom("TH2"):
                h.GetYaxis().SetTitle("Matched Probes")
        cuts = cuts.replace("()","(1)")
        #print " ++++++++ Cuts are %s ++++++++++++++++"%cuts
        if LumiScale==-1 and self.GRLHandler == None:
            t.Project(self.GetPlotName(),HistoDefs.initHisto(tmpVar)[1],cuts)
            #if HistoDefs.initHisto(tmpVar)[1] == "probe_detRegion":
                #for binno in range(Utils.GetNbins(h)-1):
                    #if binno > 12:
                        #h.SetBinContent(12,h.GetBinContent(12)+h.GetBinContent(binno))
                        #h.SetBinError(12,math.sqrt(h.GetBinError(12)**2.+h.GetBinError(binno)**2.))
        else:
            if ":" in HistoDefs.initHisto(tmpVar)[1]:
                #print "Using LumiScale in 2D histos currently not supported! Running without LumiScale instead."
                t.Project(self.GetPlotName(),HistoDefs.initHisto(tmpVar)[1],cuts)
            else:
                cuts = cuts.replace(" ", "")
                # adjust abs() naming in order to use it later
                cuts = cuts.replace("abs(", "abs[")
                cuts = cuts.replace("(","")
                cuts = cuts.replace(")","")
                cuts = cuts.replace("&&","*")
                cutlist = cuts.split("*")

                cutlist = filter(lambda a: a != "1", cutlist)
                
                nentries = t.GetEntries()
                if nentries==0:
                    print "Empty files!"

                curEvt = 0
                for event in t:
                    if curEvt>ntodo:
                        #print "Max Number of events=%i reached!"%ntodo
                        break
                    else:
                        binno = getattr(t,HistoDefs.initHisto(tmpVar)[1])
                        #if HistoDefs.initHisto(tmpVar)[1] == "probe_detRegion" and binno > 12:
                            #binno = 12
                        h.Fill(binno,self.PassesCuts(t,curEvt,cutlist))
                        
                        if curEvt % 5000 == 0:
                            pass
                            #print "Event number %d out of %d " % (curEvt, nentries)
                    curEvt += 1

        #print "Note: Reading %s from the TP tree"%self.GetPlotName()

        h.SetDirectory(0)
        if h.InheritsFrom("TH2Poly"):
            Utils.SetPolyErrors(h)
        self.Histo = h

# this guy provides a whole set of matching histos
class TPHistoSet:
    def __init__(self,name,probe,match,region,var,infile=None,doClosure=False,LumiScale=-1,useAsymEff=True,analysis=Analysis.Z_Reco,load=True,GRLHandler=None):
       self.Histos={}
       self.Name = name
       self.Analysis = analysis
       self.Infile = infile
       self.Probe = probe
       self.Match = match
       self.Region = region
       self.Var = var
       self.DoClosure = doClosure
       self.LumiScale = LumiScale
       self.UseAsymEff = useAsymEff
       self.XSscale = 1.00
       self.GRLHandler = GRLHandler
       print name
       print infile
       if load:
            #print '\nCreating TPHistoSet %s for %s'%(name,infile)
            for CP in [bla for bla in ChargeProducts.__dict__.keys() if not "__" in bla]:
                self.Histos[CP] = {}
                if (self.Probe == Probes.TruthToID or self.Probe == Probes.TruthToMu) and ("SC" in CP or "AntiIso" in CP):
                        continue
                for Kind in [bla for bla in PlotKinds.__dict__.keys() if not "__" in bla]:
                    self.Histos[CP][Kind] = TPHisto(name=name,probe=probe,match=match,region=region,var=var,chargeprod = getattr(ChargeProducts,CP), kind=getattr(PlotKinds,Kind), infile=infile,histo=None,doClosure=self.DoClosure,LumiScale=self.LumiScale,useAsymEff=self.UseAsymEff,analysis=self.Analysis,GRLHandler=self.GRLHandler)
       #self.UpdateEff()
 

    @classmethod
    ## copy constructor
    def Copy (out, other):
        
        Histos = {}
        for CP, histos in other.Histos.iteritems():
            Histos[CP] = {}
            for kind, hist in histos.iteritems():
                Histos[CP][kind] = TPHisto.Copy(hist)
        out = TPHistoSet(name=other.Name+"Clone",analysis=other.Analysis  ,infile=other.Infile,probe=other.Probe,match=other.Match,region=other.Region,var=other.Var,doClosure=other.DoClosure, LumiScale=other.LumiScale, useAsymEff=other.UseAsymEff,load=False)
        out.Histos = Histos
        return out
        #self.UpdateEff()
        
    def UpdateEff(self):
        Utils.EffDivide(self.Histos[ChargeProducts.OC][PlotKinds.Matches].Histo,self.Histos[ChargeProducts.OC][PlotKinds.Probes].Histo,self.Histos[ChargeProducts.OC][PlotKinds.Efficiency].Histo)

    def ApplyIDeff (self, ideff=None):
        if ideff!= None:
            if self.Histos[ChargeProducts.OC][PlotKinds.Efficiency].Histo.InheritsFrom("TH2Poly"):
                Utils.PolyMultiply(self.Histos[ChargeProducts.OC][PlotKinds.Efficiency].Histo,ideff.Histos[ChargeProducts.OC][PlotKinds.Efficiency].Histo,self.Histos[ChargeProducts.OC][PlotKinds.Efficiency].Histo)
            elif self.Histos[ChargeProducts.OC][PlotKinds.Efficiency].Histo.InheritsFrom("TGraphAsymmErrors"):
                Utils.GraphMultiply(self.Histos[ChargeProducts.OC][PlotKinds.Efficiency].Histo,ideff.Histos[ChargeProducts.OC][PlotKinds.Efficiency].Histo,self.Histos[ChargeProducts.OC][PlotKinds.Efficiency].Histo)
            else:
                self.Histos[ChargeProducts.OC][PlotKinds.Efficiency].Histo.Multiply(ideff.Histos[ChargeProducts.OC][PlotKinds.Efficiency].Histo)
    def Add (self, other, weight=1.):
        for CP in [bla for bla in ChargeProducts.__dict__.keys() if not "__" in bla]:
            #self.Histos[CP] = {}
            for Kind in [bla for bla in PlotKinds.__dict__.keys() if not "__" in bla and bla != PlotKinds.Efficiency]:
                #try:
                #if self.Histo.InheritsFrom("TH2Poly"):
                    #Utils.PolyAdd(self.Histos[CP][Kind],other.Histos[CP][Kind],weight) 
                #else:
                if self.Histos[CP][Kind].Histo.InheritsFrom("TH2Poly"):
                    Utils.PolyAdd(self.Histos[CP][Kind].Histo,other.Histos[CP][Kind].Histo,self.Histos[CP][Kind].Histo,weight=weight)
                else:
                    self.Histos[CP][Kind].Add(other.Histos[CP][Kind],weight) 
                #except KeyError:
                #print "Unable to load "+CP+" "+Kind
                    #pass
        try:
            self.UpdateEff()
        except KeyError:
            print "Unable to calc eff"
            pass
       
    # Data-driven background correction procedure
    def DataDrivenBG (self,SignalMC=[],IrredMC=[],RedMC=[], SysVar = ""):
        # produce a clone of the original
        #print "Producing a background template with fraction %.3f%% of entries"%(self.LumiScale*100.)
        Bkg = TPHistoSet("BackgroundTemplateFrom"+self.Name,self.Probe,self.Match,self.Region,self.Var,self.Infile,doClosure=False, LumiScale=self.LumiScale,useAsymEff=self.UseAsymEff,analysis=self.Analysis,GRLHandler=self.GRLHandler)
        # and update its contents
        for CP in [ChargeProducts.OC, ChargeProducts.SC]:
            for thing in ["Probes","Matches"]:
                h_bkg = Bkg.Histos[CP][thing].Histo
                if h_bkg == None:
                    Bkg.Histos[CP][thing] = self.Histos[ChargeProducts.SC][thing].Histo.Clone("bkg_from"+self.Histos[ChargeProducts.SC][thing].GetName())
                    h_bkg = Bkg.Histos[CP][thing].Histo
                # Start out with an empty histogram
                h_bkg.Reset("")

                # a) Take the same-charge distribution from data
                h_bkg.Add(self.Histos[ChargeProducts.SC][thing].Histo,1.)

                # b) Subtract any irreducibe same-charge pairs (they are taken from MC)
                #for subtractme in SignalMC+IrredMC:                 # subtract irreducibe same sign
                    #h_bkg.Add(subtractme.Histos[ChargeProducts.SC][thing].Histo,-1.00)
                # c) Correct the OC version for a non-unity OC/SC ratio in QCD using MC or anti-isolated data
                if CP == ChargeProducts.OC and not self.Probe == Probes.ID:
                    #qcdOC = TPHisto(name="qcdOCtmp",probe=self.Probe,match=self.Match,region=self.Region,var="mll",chargeprod=ChargeProducts.OC_AntiIso, kind=thing, infile=self.Infile, histo=None, doClosure=False,useAsymEff=False,analysis=self.Analysis)
                    #n_qcdOC = qcdOC.Histo.Integral(1,qcdOC.Histo.GetXaxis().FindBin(85e3)) + qcdOC.Histo.Integral(qcdOC.Histo.GetXaxis().FindBin(95e3),qcdOC.Histo.GetNbinsX())
                    #qcdSC = TPHisto(name="qcdOCtmp",probe=self.Probe,match=self.Match,region=self.Region,var="mll",chargeprod=ChargeProducts.SC_AntiIso, kind=thing, infile=self.Infile, histo=None, doClosure=False,useAsymEff=False,analysis=self.Analysis)
                    #n_qcdSC = qcdSC.Histo.Integral(1,qcdSC.Histo.GetXaxis().FindBin(85e3)) + qcdSC.Histo.Integral(qcdSC.Histo.GetXaxis().FindBin(95e3),qcdSC.Histo.GetNbinsX())
                    qcdOC = TPHisto(name="qcdOCtmp",probe=self.Probe,match=self.Match,region=self.Region,var="mll",chargeprod=ChargeProducts.OC_AntiIso, kind=thing, infile=self.Infile, histo=None, doClosure=False,useAsymEff=False,analysis=self.Analysis,GRLHandler=self.GRLHandler)
                    n_qcdOC = qcdOC.Histo.Integral()
                    qcdSC = TPHisto(name="qcdOCtmp",probe=self.Probe,match=self.Match,region=self.Region,var="mll",chargeprod=ChargeProducts.SC_AntiIso, kind=thing, infile=self.Infile, histo=None, doClosure=False,useAsymEff=False,analysis=self.Analysis,GRLHandler=self.GRLHandler)
                    n_qcdSC = qcdSC.Histo.Integral()
                    
                    for ibin in range(1,Utils.GetNbins(h_bkg)):
                        if h_bkg.GetBinContent(ibin) < 0:
                            h_bkg.SetBinContent(ibin, 0)
                            h_bkg.SetBinError(ibin, 0)
                    
                    if n_qcdSC == 0:
                        scale = 1
                    else:
                        scale = n_qcdOC / n_qcdSC
                        if SysVar == "up":
                            scale = 1 + 2.*(scale - 1)
                        if SysVar == "down":
                            scale = 1
                    #print "\n\n\n\nBG scale is %.3f / %.3f = %.3f"%(n_qcdOC, n_qcdSC, scale)
                    if SysVar == "up":
                        for ibin in range(1,Utils.GetNbins(h_bkg)):
                            if h_bkg.GetBinContent(ibin) < 1:
                                h_bkg.SetBinContent(ibin, 1)
                                h_bkg.SetBinError(ibin, 1)
                    h_bkg.Scale(scale)
                # that's it!
        return Bkg

    def ScaleToData(self,data):
        dataint = data.Histos[ChargeProducts.OC][PlotKinds.Probes].Histo.Integral()
        myint = self.Histos[ChargeProducts.OC][PlotKinds.Probes].Histo.Integral()
        scale = dataint / myint
        #print "Scale via norm: %.4f"%scale
        for CP in [bla for bla in ChargeProducts.__dict__.keys() if not "__" in bla]:
            for thing in ["Probes","Matches"]:
                if self.Histos[CP][thing].Histo.InheritsFrom("TH2Poly"):
                    #self.Histos[CP][thing].Histo.Scale(scale)
                    #Utils.SetPolyErrors(self.Histos[CP][thing].Histo)
                    for i in range (0, Utils.GetNbins(self.Histos[CP][thing].Histo)):
		        #print 'ScaleToData before: %s'%self.Histos[CP][thing].Histo.GetBinError(i)
                        self.Histos[CP][thing].Histo.SetBinContent(i,self.Histos[CP][thing].Histo.GetBinContent(i)*scale)
                        self.Histos[CP][thing].Histo.SetBinError(i,self.Histos[CP][thing].Histo.GetBinError(i)*scale)
                        #print 'ScaleToData after: %s'%self.Histos[CP][thing].Histo.GetBinError(i)
                    #Utils.PolyScale(self.Histos[CP][thing].Histo,scale)
                else:
                    self.Histos[CP][thing].Histo.Scale(scale)
        #sys.exit(0)
    def ApplyXSScale(self,xs,nsample=-1,lumi=20300.):
        if nsample < 0:
            if self.Infile == None:
                print "Error: Please provide either the number of MC events or a valid iput file with a cut flow if you want to do XS weighting"
            cf = self.Histos[ChargeProducts.OC][PlotKinds.Matches].GetFullCFPath()
            #print cf
            hcf = self.Infile.Get(cf)
            nsample = float(hcf.GetBinContent(1))
        #print "nsample %.2f"%nsample
        scale = xs * lumi / nsample
        #print "xs is %.2f, lumi is %.4f"%(xs,lumi)
        #print "XS scale is %.4f"%scale
        for CP in [bla for bla in ChargeProducts.__dict__.keys() if not "__" in bla]:
            for thing in ["Probes","Matches"]:
                if self.Histos[CP][thing].Histo.InheritsFrom("TH2Poly"):
                    #for i in range(0, Utils.GetNbins(self.Histos[CP][thing].Histo)):
                        #print 'ApplyXSScale: %s pm: %s'%(self.Histos[CP][thing].Histo.GetBinContent(i),self.Histos[CP][thing].Histo.GetBinError(i))
                    #Utils.PolyScale(self.Histos[CP][thing].Histo,scale)
                    #self.Histos[CP][thing].Histo.Scale(scale)
                    #Utils.SetPolyErrors(self.Histos[CP][thing].Histo)
                    #self.Histos[CP][thing].Histo.Scale(scale)
                    for i in range (0, Utils.GetNbins(self.Histos[CP][thing].Histo)):
                        #print 'ApplyXSScale before content: %s'%self.Histos[CP][thing].Histo.GetBinContent(i)
                        #print 'ApplyXSScale before: %s'%self.Histos[CP][thing].Histo.GetBinError(i+1)
                        self.Histos[CP][thing].Histo.SetBinContent(i,self.Histos[CP][thing].Histo.GetBinContent(i)*scale)
                        self.Histos[CP][thing].Histo.SetBinError(i+1,self.Histos[CP][thing].Histo.GetBinError(i+1)*scale)
                        #print 'ApplyXSScale after content: %s'%self.Histos[CP][thing].Histo.GetBinContent(i)
                        #print 'ApplyXSScale after: %s'%self.Histos[CP][thing].Histo.GetBinError(i+1)
                    #for i in range(0, Utils.GetNbins(self.Histos[CP][thing].Histo)):
                        #print 'after ApplyXSScale: %s pm: %s'%(self.Histos[CP][thing].Histo.GetBinContent(i),self.Histos[CP][thing].Histo.GetBinError(i))
                else:
                    self.Histos[CP][thing].Histo.Scale(scale)
        #sys.exit(0)
    def SubtractBG(self,Backgrounds=[]):
        #return
        if self.Probe == Probes.TruthToID or self.Probe == Probes.TruthToMu:
            return
        for CP in [bla for bla in ChargeProducts.__dict__.keys() if not "__" in bla]:
            for thing in ["Probes","Matches"]:
                tosub = self.Histos[CP][thing].Histo
                for BG in Backgrounds:
                    subthis = BG.Histos[CP][thing].Histo
                    if subthis.InheritsFrom("TH2Poly"):
                        Utils.PolySubtract(tosub,subthis,tosub)
                    else:
                        tosub.Add(subthis,-1.)

class TPFinalHistos:
    def __init__(self,name,probe,match,region,var,infiles,corr = True,sysVar = "",doClosure=False,LumiScale=-1,doPreRecEtaCopy=False,useAsymEff=True,analysis=Analysis.Z_Reco):
        self.Histos={}
        self.Name = name
        self.Analysis = analysis
        self.Probe = probe
        self.Match = match
        self.Region = region
        self.Var = var
        self.Infiles = infiles
        self.Corr = corr
        self.SysVar = sysVar
        self.DoClosure = doClosure
        self.LumiScale = LumiScale
        self.DoPreRecEtaCopy = doPreRecEtaCopy
        self.UseAsymEff = useAsymEff

        self.Histos[HistoKinds.Efficiency] = {}
        self.Histos[HistoKinds.SF] = {}
        self.Histos[PlotKinds.Probes] = {}

        MCHists = None
        MCHistsScaled = None
        DataHists = None
        BkgHists = []
        
        for [thing, sample] in infiles:
            if sample.SampleType == SampleTypes.Signal:
                ahisto = TPHistoSet(thing, var=var,probe=probe,match = match, region = region, infile = ROOT.TFile(sample.Filepath,"READ"), doClosure=self.DoClosure,useAsymEff=self.UseAsymEff,analysis=self.Analysis) 
                ahistoscaled = TPHistoSet.Copy(ahisto)
                #print ahistoscaled
                ahistoscaled.ApplyXSScale(sample.XS, sample.nEvents, sample.Lumi)
                if MCHists == None:
                    MCHists = ahisto
                    MCHistsScaled = ahistoscaled
                else: 
                    MCHists.Add(ahisto)
                    MCHistsScaled.Add(ahistoscaled)
            if sample.SampleType == SampleTypes.Data:
                GRLHandler = None
                if not sample.GRLs == None:
                    GRLHandler = ParseGRL.GRLHandler(sample.GRLs)
                ahisto = TPHistoSet(thing, var=var,probe=probe,match = match, region = region, infile = ROOT.TFile(sample.Filepath,"READ"), doClosure=False,useAsymEff=self.UseAsymEff,analysis=self.Analysis,GRLHandler=GRLHandler) 
                if DataHists == None:
                    DataHists = ahisto
                else: 
                    DataHists.Add(ahisto)
            if sample.SampleType == SampleTypes.Irreducible:
                ahisto = TPHistoSet(thing, var=var,probe=probe,match = match, region = region, infile = ROOT.TFile(sample.Filepath,"READ"), doClosure=False,useAsymEff=self.UseAsymEff,analysis=self.Analysis) 
                ahisto.ApplyXSScale(sample.XS, sample.nEvents, sample.Lumi)
                BkgHists.append(ahisto)
        
        #MCHists = TPHistoSet(infiles[0][0],var=var,probe=probe,match = match, region = region, infile = ROOT.TFile(infiles[0][1].Filepath,"READ"), doClosure=self.DoClosure,useAsymEff=self.UseAsymEff,analysis=self.Analysis) 
        #MCHistsScaled = TPHistoSet(infiles[0][0],var=var,probe=probe,match = match, region = region, infile = ROOT.TFile(infiles[0][1].Filepath,"READ"), doClosure=self.DoClosure,useAsymEff=self.UseAsymEff,analysis=self.Analysis)
        
        #DataHists = TPHistoSet(infiles[1][0],var=var,probe=probe,match = match, region = region, infile = ROOT.TFile(infiles[1][1].Filepath,"READ"), LumiScale=self.LumiScale,useAsymEff=self.UseAsymEff,analysis=self.Analysis)

        # scale MCHists first, since it is used in SubtractBG
        MCHistsScaled.ScaleToData(DataHists)
        
        #TODO: reinclude ApplyXSScale?
        #MCHistsScaled.ApplyXSScale(infiles[0][1].XS, infiles[0][1].nEvents, infiles[0][1].Lumi)
        BkgDataHists = DataHists.DataDrivenBG(SignalMC=[MCHistsScaled],IrredMC=BkgHists,SysVar = sysVar)
        #BkgDataHists = DataHists.DataDrivenBG(SignalMC=[MCHistsScaled],IrredMC=BkgHists,SysVar = sysVar)
        DataHists.SubtractBG([BkgDataHists]+BkgHists)
        #print "\n\n\n"+"I have %.5f BG probes"%BkgDataHists.Histos[ChargeProducts.OC][PlotKinds.Probes].Histo.Integral()

        if not 'dR' in self.Match:
            if Matches.ID in match:
                Truth = TPHistoSet("Truth",var=var,probe=Probes.TruthToID,match = match, region = region, infile = ROOT.TFile(infiles[0][1].Filepath,"READ"), doClosure=False,useAsymEff=self.UseAsymEff,analysis=self.Analysis)
            else:
                Truth = TPHistoSet("Truth",var=var,probe=Probes.TruthToMu,match = match, region = region, infile = ROOT.TFile(infiles[0][1].Filepath,"READ"), doClosure=False,useAsymEff=self.UseAsymEff,analysis=self.Analysis)
            Truth.UpdateEff()
        else:
            Truth = MCHists
        
        DataHists.UpdateEff()
        MCHists.UpdateEff()

        # Apply the ID eff if we have to
        if corr and (probe == Probes.ID or probe == Probes.Calo):
            if "dRUp" in match:
                match = "%s_dRUp"%(Matches.ID)
            elif "dRDown" in match:
                match = "%s_dRDown"%(Matches.ID)
            else:
                match=Matches.ID
            
            MCIDHists = None
            MCIDHistsScaled = None
            DataIDHists = None
            BkgIDHists = []
            
            for [thing, sample] in infiles:
                if sample.SampleType == SampleTypes.Signal:
                    ahisto = TPHistoSet(thing+"ID", var=var,probe=Probes.MStoID,match = match, region = region, infile = ROOT.TFile(sample.Filepath,"READ"), doClosure=self.DoClosure,useAsymEff=self.UseAsymEff,analysis=self.Analysis) 
                    ahistoscaled = TPHistoSet.Copy(ahisto)
                    ahistoscaled.ApplyXSScale(sample.XS, sample.nEvents, sample.Lumi)
                    if MCIDHists == None:
                        MCIDHists = ahisto
                        MCIDHistsScaled = ahistoscaled
                    else: 
                        MCIDHists.Add(ahisto)
                        MCIDHistsScaled.Add(ahistoscaled)
                if sample.SampleType == SampleTypes.Data:
                    ahisto = TPHistoSet(thing+"ID", var=var,probe=Probes.MStoID,match = match, region = region, infile = ROOT.TFile(sample.Filepath,"READ"), doClosure=False,useAsymEff=self.UseAsymEff,analysis=self.Analysis) 
                    if DataIDHists == None:
                        DataIDHists = ahisto
                    else: 
                        DataIDHists.Add(ahisto)
                if sample.SampleType == SampleTypes.Irreducible:
                    ahisto = TPHistoSet(thing+"ID", var=var,probe=Probes.MStoID,match = match, region = region, infile = ROOT.TFile(sample.Filepath,"READ"), doClosure=False,useAsymEff=self.UseAsymEff,analysis=self.Analysis) 
                    ahisto.ApplyXSScale(sample.XS, sample.nEvents, sample.Lumi)
                    BkgIDHists.append(ahisto)
            #DataIDHists = TPHistoSet("DataID", var=var, probe=Probes.MStoID, match=match, region=region, infile=ROOT.TFile(infiles[1][1].Filepath, "READ"),useAsymEff=self.UseAsymEff,analysis=self.Analysis)
            #MCIDHists = TPHistoSet("MCID", var=var, probe=Probes.MStoID, match=match, region=region, infile=ROOT.TFile(infiles[0][1].Filepath, "READ"), doClosure=False,useAsymEff=self.UseAsymEff,analysis=self.Analysis)
            #MCIDHistsScaled = TPHistoSet("MCID", var=var, probe=Probes.MStoID, match=match, region=region, infile=ROOT.TFile(infiles[0][1].Filepath, "READ"), doClosure=False,useAsymEff=self.UseAsymEff,analysis=self.Analysis)
            #TODO: reinclude ApplyXSScale?
            #MCIDHistsScaled.ApplyXSScale(infiles[0][1].XS, infiles[0][1].nEvents, infiles[0][1].Lumi)
            
            
            
            MCIDHistsScaled.ScaleToData(DataIDHists)
            DataIDHists.SubtractBG([DataIDHists.DataDrivenBG(SignalMC=[MCIDHistsScaled],IrredMC=BkgIDHists,SysVar = sysVar)]+BkgIDHists)
            #DataIDHists.SubtractBG([DataIDHists.DataDrivenBG(SignalMC=[MCIDHistsScaled],IrredMC=BkgIDHists,SysVar = sysVar)]+BkgIDHists)
            MCIDHists.UpdateEff()
            DataIDHists.UpdateEff()
            
            
            DataHists.ApplyIDeff(DataIDHists)
            MCHists.ApplyIDeff(MCIDHists)
            
        # fill probe histos:
        self.Histos[PlotKinds.Probes]["Data"] = DataHists.Histos[ChargeProducts.OC][PlotKinds.Probes].Histo
        self.Histos[PlotKinds.Probes]["MC"] = MCHistsScaled.Histos[ChargeProducts.OC][PlotKinds.Probes].Histo
        self.Histos[PlotKinds.Probes]["Bkg"] = BkgDataHists.Histos[ChargeProducts.OC][PlotKinds.Probes].Histo
        #for i in range(0, Utils.GetNbins(self.Histos[PlotKinds.Probes]["Bkg"])):
            #print 'self.Histos[PlotKinds.Probes]["Bkg"]: %s pm: %s'%(self.Histos[PlotKinds.Probes]["Bkg"].GetBinContent(i),self.Histos[PlotKinds.Probes]["Bkg"].GetBinError(i))
            #print 'self.Histos[PlotKinds.Probes]["MC"]: %s pm: %s'%(self.Histos[PlotKinds.Probes]["MC"].GetBinContent(i),self.Histos[PlotKinds.Probes]["MC"].GetBinError(i))
        h_model = self.Histos[PlotKinds.Probes]["MC"].Clone('%s_model'%(self.Histos[PlotKinds.Probes]["MC"].GetName()))
        h_dataVsModel = self.Histos[PlotKinds.Probes]["Data"].Clone('%s_DataVsModel'%(self.Histos[PlotKinds.Probes]["Data"].GetName()))
        if not self.Histos[PlotKinds.Probes]["MC"].InheritsFrom("TGraphAsymmErrors"):
            h_model.SetDirectory(0)
            h_dataVsModel.SetDirectory(0)
        if self.Histos[PlotKinds.Probes]["MC"].InheritsFrom("TH2Poly"):
            Utils.PolyAdd(self.Histos[PlotKinds.Probes]["MC"],self.Histos[PlotKinds.Probes]["Bkg"],h_model)
        else:
            h_model.Add(self.Histos[PlotKinds.Probes]["MC"],self.Histos[PlotKinds.Probes]["Bkg"],1.,1.)
        self.Histos[PlotKinds.Probes]["Model"] = h_model
        if self.Histos[PlotKinds.Probes]["MC"].InheritsFrom("TH2Poly"):
            #for i in range(0, Utils.GetNbins(self.Histos[PlotKinds.Probes]["Model"])):
                #print 'Histos[PlotKinds.Probes]["Data"]: %s pm: %s'%(self.Histos[PlotKinds.Probes]["Data"].GetBinContent(i),self.Histos[PlotKinds.Probes]["Data"].GetBinError(i))
                #print 'Histos[PlotKinds.Probes]["Model"]: %s pm: %s'%(self.Histos[PlotKinds.Probes]["Model"].GetBinContent(i),self.Histos[PlotKinds.Probes]["Model"].GetBinError(i))
            Utils.PolyDivide(self.Histos[PlotKinds.Probes]["Data"],self.Histos[PlotKinds.Probes]["Model"],h_dataVsModel)
        elif self.Histos[PlotKinds.Probes]["MC"].InheritsFrom("TGraphAsymmErrors"):
            Utils.GraphDivide(self.Histos[PlotKinds.Probes]["Data"],self.Histos[PlotKinds.Probes]["Model"],h_dataVsModel)
        else:
            h_dataVsModel.Divide(self.Histos[PlotKinds.Probes]["Data"],self.Histos[PlotKinds.Probes]["Model"],1.,1.)
        self.Histos[PlotKinds.Probes]["DataVsModel"] = h_dataVsModel

        # fill efficiency histos:
        h_data = DataHists.Histos[ChargeProducts.OC][PlotKinds.Efficiency].Histo            
        self.Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency] = h_data
        h_mc = MCHists.Histos[ChargeProducts.OC][PlotKinds.Efficiency].Histo
        h_mc.SetName(h_mc.GetName().replace('Efficiency','MCEfficiency'))
        self.Histos[HistoKinds.Efficiency][HistoKinds.MCEfficiency] = h_mc
        h_truth = Truth.Histos[ChargeProducts.OC][PlotKinds.Efficiency].Histo
        h_truth.SetName(h_truth.GetName().replace('Efficiency','TruthEfficiency'))
        self.Histos[HistoKinds.Efficiency][HistoKinds.TruthEfficiency] = h_truth
                
        # for pre-recommendations, copy negative eta bins of transition region to positive ones:
        if self.DoPreRecEtaCopy:
            Utils.CopyEtaBins(h_data)
            Utils.CopyEtaBins(h_mc)
            Utils.CopyEtaBins(h_truth)

        # replace 'MCEfficiency' since it was added to h_mc above
        ratio = h_mc.Clone(h_mc.GetName().replace('MCEfficiency','SF'))
        ratiotruth = h_truth.Clone(h_mc.GetName().replace('MCEfficiency','SFTruth'))
        ratiotruthMC = h_truth.Clone(h_mc.GetName().replace('MCEfficiency','SFMCByTruth'))
        if not h_mc.InheritsFrom("TGraphAsymmErrors"):
            ratio.SetDirectory(0)
            ratiotruth.SetDirectory(0)
            ratiotruthMC.SetDirectory(0)

        if h_mc.InheritsFrom("TH2Poly"):
            Utils.PolyDivide(h_data,h_mc,ratio)
            Utils.PolyDivide(h_data,h_truth,ratiotruth)
            Utils.PolyDivide(h_mc,h_truth,ratiotruthMC)
            #for i in range(0, Utils.GetNbins(h_mc)):
                #print 'h_mc: %s pm: %s'%(h_mc.GetBinContent(i),h_mc.GetBinError(i))
                #print 'h_data: %s pm: %s'%(h_data.GetBinContent(i),h_data.GetBinError(i))
                #print 'ratio: %s pm: %s'%(ratio.GetBinContent(i),ratio.GetBinError(i))
        elif h_mc.InheritsFrom("TGraphAsymmErrors"):
            Utils.GraphDivide(h_data,h_mc,ratio)
            Utils.GraphDivide(h_data,h_truth,ratiotruth)
            Utils.GraphDivide(h_mc,h_truth,ratiotruthMC)
        else:
            ratio.Divide(h_data,h_mc,1.,1.)
            ratiotruth.Divide(h_data,h_truth,1.,1.)
            ratiotruthMC.Divide(h_mc,h_truth,1.,1.)
        
        
        for hist in [ratio,ratiotruth,ratiotruthMC]:
            for i in range(0, Utils.GetNbins(hist)):
                if hist.InheritsFrom("TGraphAsymmErrors"):
                    if hist.GetErrorYhigh(i)>1.0:
                        hist.SetPointEYhigh(i,1.0)
                    if hist.GetErrorYlow(i)>1.0:
                        hist.SetPointEYlow(i,1.0)
                else:
                    if hist.GetBinError(i)>1.0:
                        hist.SetBinError(i,1.0)

        # fill scale factor histos:
        self.Histos[HistoKinds.SF][HistoKinds.SF] = ratio
        self.Histos[HistoKinds.SF][HistoKinds.TruthSF] = ratiotruth
        self.Histos[HistoKinds.SF][HistoKinds.MCTruthSF] = ratiotruthMC


class TPFinalSysHistos:
    def __init__(self,name,probe,match,region,var,infiles,charge=None,corr=True,doClosure=False,LumiScale=-1,doPreRecEtaCopy=False,useAsymEff=True,analysis = Analysis.Z_Reco):
        self.HistoSets={}
        self.Name = name
        self.Analysis = analysis
        self.Probe = probe
        self.Match = match
        self.Region = region
        self.Var = var
        self.Infiles = infiles
        self.Charge = charge
        self.Corr = corr
        self.DoClosure = doClosure
        self.LumiScale = LumiScale
        self.DoPreRecEtaCopy = doPreRecEtaCopy
        self.UseAsymEff = useAsymEff

        #print '\nCreating TPFinalSysHistos for %s%s'%(var,charge)
        # fill nominal TPFinalHistos first, since they are needed for systematic evaluation
        self.HistoSets["Nominal"] = self.CreateTPFinalHistos (self.Name,self.Probe,self.Match,self.Region,self.Var,self.Infiles,None,self.Charge,self.Corr,analysis=self.Analysis)

        for syst in [bla for bla in Systematics.__dict__.keys() if not "__" in bla ]:
            if Systematics.All != getattr(Systematics,syst):
                #print '\n\n\nSystematic: %s'%getattr(Systematics,syst)
                
                # don't evaluate charge sys if we are running charge dependent SF!
                if not self.Charge==None and Systematics.Charge == getattr(Systematics,syst):
                    continue
                
                self.HistoSets[getattr(Systematics,syst)] = self.CreateTPFinalHistos (self.Name+"_"+syst,self.Probe,self.Match,self.Region,self.Var,self.Infiles,syst,self.Charge,self.Corr,analysis=self.Analysis)

        # fill total systematics histos -> Clone dR syst and add others in quadrature
        self.HistoSets[Systematics.All] = {}
        for bla in self.HistoSets[Systematics.dR].keys():
            self.HistoSets[Systematics.All][bla] = self.HistoSets[Systematics.dR][bla].Clone(self.HistoSets[Systematics.dR][bla].GetName().replace(Systematics.dR,Systematics.All))
        #totalSys = []
        #for hist in self.HistoSets[Systematics.dR]:
            #totalSys.append( hist.Clone(hist.GetName().replace(Systematics.dR,Systematics.All)) )
        #self.HistoSets[Systematics.All] = totalSys

        # use add_in_quadrature(hist_add_to, hist_add_this)
        #Utils.add_in_quadrature(self.HistoSets[Systematics.All][0],self.HistoSets[Systematics.truth][0])
        Utils.add_in_quadrature(self.HistoSets[Systematics.All][HistoKinds.DataEfficiency],self.HistoSets[Systematics.truth][HistoKinds.DataEfficiency])
        #Utils.add_in_quadrature(self.HistoSets[Systematics.All][0],self.HistoSets[Systematics.BG][0])
        Utils.add_in_quadrature(self.HistoSets[Systematics.All][HistoKinds.DataEfficiency],self.HistoSets[Systematics.BG][HistoKinds.DataEfficiency])
        #Utils.add_in_quadrature(self.HistoSets[Systematics.All][0],self.HistoSets[Systematics.Det][0])
        #Utils.add_in_quadrature(self.HistoSets[Systematics.All][1],self.HistoSets[Systematics.truth][1])
        #Utils.add_in_quadrature(self.HistoSets[Systematics.All][1],self.HistoSets[Systematics.BG][1])
        Utils.add_in_quadrature(self.HistoSets[Systematics.All][HistoKinds.MCEfficiency],self.HistoSets[Systematics.truth][HistoKinds.MCEfficiency])
        Utils.add_in_quadrature(self.HistoSets[Systematics.All][HistoKinds.MCEfficiency],self.HistoSets[Systematics.BG][HistoKinds.MCEfficiency])
        #Utils.add_in_quadrature(self.HistoSets[Systematics.All][1],self.HistoSets[Systematics.Det][1])
        #Utils.add_in_quadrature(self.HistoSets[Systematics.All][2],self.HistoSets[Systematics.truth][2])
        #Utils.add_in_quadrature(self.HistoSets[Systematics.All][2],self.HistoSets[Systematics.BG][2])
        Utils.add_in_quadrature(self.HistoSets[Systematics.All][HistoKinds.SF],self.HistoSets[Systematics.truth][HistoKinds.SF])
        Utils.add_in_quadrature(self.HistoSets[Systematics.All][HistoKinds.SF],self.HistoSets[Systematics.BG][HistoKinds.SF])
        #Utils.add_in_quadrature(self.HistoSets[Systematics.All][2],self.HistoSets[Systematics.Det][2])
        if self.Charge==None:
            #Utils.add_in_quadrature(self.HistoSets[Systematics.All][0],self.HistoSets[Systematics.Charge][0])
            #Utils.add_in_quadrature(self.HistoSets[Systematics.All][1],self.HistoSets[Systematics.Charge][1])
            #Utils.add_in_quadrature(self.HistoSets[Systematics.All][2],self.HistoSets[Systematics.Charge][2])
            Utils.add_in_quadrature(self.HistoSets[Systematics.All][HistoKinds.DataEfficiency],self.HistoSets[Systematics.Charge][HistoKinds.DataEfficiency])
            Utils.add_in_quadrature(self.HistoSets[Systematics.All][HistoKinds.MCEfficiency],self.HistoSets[Systematics.Charge][HistoKinds.MCEfficiency])
            Utils.add_in_quadrature(self.HistoSets[Systematics.All][HistoKinds.SF],self.HistoSets[Systematics.Charge][HistoKinds.SF])
            
            

    def CreateTPFinalHistos (self,name,probe,match,region,var,infiles,syst,charge,corr,analysis=Analysis.Z_Reco):
        chargestr = ""
        if not charge == None:
            chargestr = charge
        syststr = ""
        if not syst == None:
            syststr = syst
        if syst != None:
            if Systematics.dR == getattr(Systematics,syst):
                TPFinalHistos_dRup = self.CreateTPHistos(name,probe,match,region,var,infiles,"dRUp",charge,corr)
                TPFinalHistos_dRDown = self.CreateTPHistos(name,probe,match,region,var,infiles,"dRDown",charge,corr)
                # compare with nominal:
                h_dR_sys_data_eff = Utils.sysCompare(self.HistoSets["Nominal"].Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency], TPFinalHistos_dRup.Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency], TPFinalHistos_dRDown.Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency], "dR", 1.)
                h_dR_sys_mc_eff = Utils.sysCompare(self.HistoSets["Nominal"].Histos[HistoKinds.Efficiency][HistoKinds.MCEfficiency], TPFinalHistos_dRup.Histos[HistoKinds.Efficiency][HistoKinds.MCEfficiency], TPFinalHistos_dRDown.Histos[HistoKinds.Efficiency][HistoKinds.MCEfficiency], "dR", 1.)
                h_dR_sys_sf = Utils.sysCompare(self.HistoSets["Nominal"].Histos[HistoKinds.SF][HistoKinds.SF], TPFinalHistos_dRup.Histos[HistoKinds.SF][HistoKinds.SF], TPFinalHistos_dRDown.Histos[HistoKinds.SF][HistoKinds.SF], "dR", 1.)
                #return [h_dR_sys_data_eff, h_dR_sys_mc_eff, h_dR_sys_sf]
                return { HistoKinds.DataEfficiency : h_dR_sys_data_eff, HistoKinds.MCEfficiency : h_dR_sys_mc_eff, HistoKinds.SF : h_dR_sys_sf }
            elif Systematics.truth == getattr(Systematics,syst):
                h_truth_sys_data_eff = Utils.sysCompare(self.HistoSets["Nominal"].Histos[HistoKinds.Efficiency][HistoKinds.TruthEfficiency], self.HistoSets["Nominal"].Histos[HistoKinds.Efficiency][HistoKinds.MCEfficiency], self.HistoSets["Nominal"].Histos[HistoKinds.Efficiency][HistoKinds.MCEfficiency], "truth", 1.)
                h_truth_sys_mc_eff = h_truth_sys_data_eff
                h_truth_sys_sf = Utils.sysCompare(self.HistoSets["Nominal"].Histos[HistoKinds.SF][HistoKinds.TruthSF], self.HistoSets["Nominal"].Histos[HistoKinds.SF][HistoKinds.SF], self.HistoSets["Nominal"].Histos[HistoKinds.SF][HistoKinds.SF], "truth", 0.5)
                #return [h_truth_sys_data_eff, h_truth_sys_mc_eff, h_truth_sys_sf]
                return { HistoKinds.DataEfficiency : h_truth_sys_data_eff, HistoKinds.MCEfficiency : h_truth_sys_mc_eff, HistoKinds.SF : h_truth_sys_sf }
            elif Systematics.BG == getattr(Systematics,syst):
                TPFinalHistos_BGUp = self.CreateTPHistos(name,probe,match,region,var,infiles,"BGUp",charge,corr)
                TPFinalHistos_BGDown = self.CreateTPHistos(name,probe,match,region,var,infiles,"BGDown",charge,corr)
                # compare with nominal:
                h_BG_sys_data_eff = Utils.sysCompare(self.HistoSets["Nominal"].Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency], TPFinalHistos_BGUp.Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency], TPFinalHistos_BGDown.Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency], "BG", 1.)
                h_BG_sys_mc_eff = Utils.sysCompare(self.HistoSets["Nominal"].Histos[HistoKinds.Efficiency][HistoKinds.MCEfficiency], TPFinalHistos_BGUp.Histos[HistoKinds.Efficiency][HistoKinds.MCEfficiency], TPFinalHistos_BGDown.Histos[HistoKinds.Efficiency][HistoKinds.MCEfficiency], "BG", 1.)
                h_BG_sys_sf = Utils.sysCompare(self.HistoSets["Nominal"].Histos[HistoKinds.SF][HistoKinds.SF], TPFinalHistos_BGUp.Histos[HistoKinds.SF][HistoKinds.SF], TPFinalHistos_BGDown.Histos[HistoKinds.SF][HistoKinds.SF], "BG", 1.)
                #return [h_BG_sys_data_eff, h_BG_sys_mc_eff, h_BG_sys_sf]
                return { HistoKinds.DataEfficiency : h_BG_sys_data_eff, HistoKinds.MCEfficiency : h_BG_sys_mc_eff, HistoKinds.SF : h_BG_sys_sf }
            elif Systematics.Charge == getattr(Systematics,syst):
                TPFinalHistos_pos = self.CreateTPHistos(name,probe,match,region,var,infiles,None,"pos",corr)
                TPFinalHistos_neg = self.CreateTPHistos(name,probe,match,region,var,infiles,None,"neg",corr)
                # compare with nominal:
                h_Charge_sys_data_eff = Utils.sysCompare(self.HistoSets["Nominal"].Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency], TPFinalHistos_pos.Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency], TPFinalHistos_neg.Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency], "Charge", 1.)
                h_Charge_sys_mc_eff = Utils.sysCompare(self.HistoSets["Nominal"].Histos[HistoKinds.Efficiency][HistoKinds.MCEfficiency], TPFinalHistos_pos.Histos[HistoKinds.Efficiency][HistoKinds.MCEfficiency], TPFinalHistos_neg.Histos[HistoKinds.Efficiency][HistoKinds.MCEfficiency], "Charge", 1.)
                h_Charge_sys_sf = Utils.sysCompare(self.HistoSets["Nominal"].Histos[HistoKinds.SF][HistoKinds.SF], TPFinalHistos_pos.Histos[HistoKinds.SF][HistoKinds.SF], TPFinalHistos_neg.Histos[HistoKinds.SF][HistoKinds.SF], "Charge", 1.)
                #return [h_Charge_sys_data_eff, h_Charge_sys_mc_eff, h_Charge_sys_sf]
                return { HistoKinds.DataEfficiency : h_Charge_sys_data_eff, HistoKinds.MCEfficiency : h_Charge_sys_mc_eff, HistoKinds.SF : h_Charge_sys_sf }
                
                
                
                #print "\n\n\n\n\n In Sys compare: "
                #print "Nom:"
                #print self.HistoSets["Nominal"].Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency]
                #self.HistoSets["Nominal"].Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency].Print()
                #print "Up:"
                #print TPFinalHistos_BGUp.Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency]
                #TPFinalHistos_BGUp.Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency].Print()
                #print "Down:"
                #print TPFinalHistos_BGDown.Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency]
                #TPFinalHistos_BGDown.Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency].Print()
                #print "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
            #elif Systematics.Det == getattr(Systematics,syst):
                ## detector systematic compares MC14 vs MC15
                #if "MC14" in infiles[0][1].Label:
                    #print "use %s for detector systematic evaluation"%DSConfig.Zmumu_mc15.Filepath
                    #TPFinalHistos_MC15 = self.CreateTPHistos(name,probe,match,region,var,[["MC",DSConfig.Zmumu_mc15],self.Infiles[1]],None,charge,corr,doPreRecCopy=False)
                #else:
                    #print "use %s for detector systematic evaluation"%DSConfig.Zmumu_r20_1_3_3.Filepath
                    #TPFinalHistos_MC15 = self.CreateTPHistos(name,probe,match,region,var,[["MC",DSConfig.Zmumu_r20_1_3_3],self.Infiles[1]],None,charge,corr,doPreRecCopy=False)
                ## compare with MC14:
                #h_Det_sys_data_eff = Utils.sysCompare(self.HistoSets["Nominal"].Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency], TPFinalHistos_MC15.Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency], TPFinalHistos_MC15.Histos[HistoKinds.Efficiency][HistoKinds.DataEfficiency], "Det", 1.)
                #h_Det_sys_mc_eff = Utils.sysCompare(self.HistoSets["Nominal"].Histos[HistoKinds.Efficiency][HistoKinds.MCEfficiency], TPFinalHistos_MC15.Histos[HistoKinds.Efficiency][HistoKinds.MCEfficiency], TPFinalHistos_MC15.Histos[HistoKinds.Efficiency][HistoKinds.MCEfficiency], "Det", 1.)
                #h_Det_sys_sf = Utils.sysCompare(self.HistoSets["Nominal"].Histos[HistoKinds.SF][HistoKinds.SF], TPFinalHistos_MC15.Histos[HistoKinds.SF][HistoKinds.SF], TPFinalHistos_MC15.Histos[HistoKinds.SF][HistoKinds.SF], "Det", 1.)
                #return [h_Det_sys_data_eff, h_Det_sys_mc_eff, h_Det_sys_sf]
            else:
                print 'CreateTPFinalHistos() was called with unknown systematic type: %s! Abort!'%(syst)
                return None
        else:
            return self.CreateTPHistos(name,probe,match,region,var,infiles,syst,charge,corr)

    def CreateTPHistos (self,name,probe,match,region,var,infiles,syst,charge,corr,doPreRecCopy=True):

        if charge != None:
            if "pos" in charge:
                var = var+"_posq"
            elif "neg" in charge:
                var = var+"_negq"
            else:
                print "Error: charge %s not recognized!"%(charge)
                return None

        BGvar = ""
        if syst == "BGUp":
            BGvar = "up"
        elif syst == "BGDown":
            BGvar = "down"

        if syst == "dRUp":
            match = "%s_dRUp"%(match)
        elif syst == "dRDown":
            match = "%s_dRDown"%(match)

        name = 'hists_%s_%s_%s_%s'%(var, probe, match, syst)

        if not doPreRecCopy:
            #print 'doing MC15 systematic: set doPreRecCopy to False'
            return TPFinalHistos(name,probe,match,region,var,infiles,sysVar = BGvar,doClosure=self.DoClosure,LumiScale=self.LumiScale,doPreRecEtaCopy=False,useAsymEff=self.UseAsymEff,analysis=self.Analysis)
        else:
            return TPFinalHistos(name,probe,match,region,var,infiles,sysVar = BGvar,doClosure=self.DoClosure,LumiScale=self.LumiScale,doPreRecEtaCopy=self.DoPreRecEtaCopy,useAsymEff=self.UseAsymEff,analysis=self.Analysis)


########## some testing code ##############
if __name__ == "__main__":
    bla = TPHisto (name="Test",probe = Probes.ID, match = Matches.Medium, region = DetRegions.All, kind = PlotKinds.Efficiency, var = "eta", chargeprod = ChargeProducts.OC)


    #print bla.GetFullPath()
    #print bla.GetFullTreePath()

    import ROOT
    #f = ROOT.TFile.Open("/afs/ipp-garching.mpg.de/home/g/goblirsc/analysis/TP_nightly/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonPerformanceAlgs/run/test.merge.root","READ")

    #bla.ExtractFromTree(f)

    #bla2 = TPHisto (name="Test",probe = Probes.ID, match = Matches.Medium, region = DetRegions.All, kind = PlotKinds.Efficiency, var = "eta", chargeprod = ChargeProducts.OC,infile=ROOT.TFile.Open("/afs/ipp-garching.mpg.de/home/g/goblirsc/analysis/TP_nightly/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonPerformanceAlgs/run/test.merge.root","READ"))


    #print f.Get(bla.GetFullTreePath())

    ##print [bla for bla in Matches.__dict__.keys() if not "__" in bla ]
    #import ROOT
    #moo = ROOT.TH1F(bla.GetPlotName(),bla.GetFullPath(),100,0,1)
    #hist2 = TPHisto.FromHist("TestMC2",moo,None)
    #print hist2.GetPlotName()
    #print hist2.Histo.GetName()
    #hist2.Write(ROOT.TFile("out.root","RECREATE"),"Test/Dir/Structure")

    #bla.Load(ROOT.TFile("out.root","READ"),"Test/Dir/Structure")
    #print bla.Histo



    ## Larger scale test
    infile = ROOT.TFile("/afs/ipp-garching.mpg.de/home/g/goblirsc/analysis/TP_nightly/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonPerformanceAlgs/run/test.merge.root")
    aset = TPHistoSet("Blah",probe=Probes.ID, match = Matches.Medium, region = DetRegions.All, var = "etaphi", infile = infile,analysis=self.Analysis)
    can = ROOT.TCanvas()
    can.cd()
    ROOT.gStyle.SetPalette(54)
    ROOT.gPad.SetRightMargin(0.2)
    aset.Histos[ChargeProducts.OC][PlotKinds.Efficiency].Histo.Draw("COLZ")
    #can.SaveAs("test.pdf")
    #for kind,hist in aset.Histos["OC"].iteritems():
        #hist.Histo.Add(aset.Histos["SC"][kind].Histo,-1)
    #aset.UpdateEff()
    ##aset.Histos["OC"]["Probes"].Histo.Draw()
    #aset.Histos["OC"]["Efficiency"].Histo.Draw()
    can.SaveAs("Plots/test.pdf")

