#!/bin/env python
# Author: nils.gollub@cern.ch

import sys, os
import time
import ROOT
import logging
from ROOT import TCanvas, TH1D, TH2D, TGraph, TArrayD
from TileCoolDcs import TileDCSDataGrabber
from TileCoolDcs import ProgressBar


class TileDCSDataPlotter:

    #_______________________________________________________________________________________
    def __init__( self, argv, useCool, useTestBeam, verbose, dbstring=None, putDuplicates=False ):

        dbSource = "ORACLE"
        self.useCool = useCool
        if useCool:
            dbSource = "COOL"
        self.useTestBeam = useTestBeam
        if useTestBeam:
            dbSource = "TESTBEAM"
        logLvl = logging.WARNING
        if verbose:
            logLvl = logging.DEBUG

#        self.dataGrabber = TileDCSDataGrabber.TileDCSDataGrabber(dbSource, logLvl, dbstring, putDuplicates)
#        self.info = self.dataGrabber.info

        self.cmd     = argv[1]
        self.drawer  = argv[2]
        self.varExp  = argv[3]
        beg          = argv[4]
        end          = argv[5]
        self.iovBeg  = int(time.mktime(time.strptime(beg,"%Y-%m-%d %H:%M:%S")))
        self.iovEnd  = int(time.mktime(time.strptime(end,"%Y-%m-%d %H:%M:%S")))

        self.dataGrabber = TileDCSDataGrabber.TileDCSDataGrabber(dbSource, logLvl, dbstring, putDuplicates, self.iovBeg)
        self.info = self.dataGrabber.info

        self.cutExp  = ""
        if len(argv) >6:
            self.cutExp = argv[6]
        beg = beg.replace(' ','_')
        end = end.replace(' ','_')
        self.outName   = self.drawer+"_"+self.varExp+"__"+beg+"__"+end
        self.outName   = self.outName.replace('/',':')
        self.outName   = self.outName.replace('(','0')
        self.outName   = self.outName.replace(')','0')
        print "---> OUTNAME: " , self.outName
        if len(argv) >7:
            self.outName = argv[7]

        timeBegInfo = time.localtime(self.iovBeg);
        timeEndInfo = time.localtime(self.iovEnd);
        self.rangeStr = "FROM: "+time.asctime(timeBegInfo)+"  UNTIL: "+time.asctime(timeEndInfo)

        part=["LBA","LBC","EBA","EBC"]
        if self.drawer == "ALL":
            self.drawer=""
            for p in xrange(4):
                for m in xrange(64):
                    self.drawer+="%s%2.2d," % (part[p],m+1)
            self.drawer=self.drawer[:-1]
        elif self.drawer in part:
            p=self.drawer
            self.drawer=""
            for m in xrange(64):
                self.drawer+="%s%2.2d," % (p,m+1)
            self.drawer=self.drawer[:-1]


    #_______________________________________________________________________________________
    def parseVarExpression(self, varExpression):

        knownVars = self.info.get_all_variables()
        varDict = {}
        for var in knownVars:
            pos = varExpression.find(var)
            if pos>-1:
                if pos in varDict:
                    #=== other variable found at same place?
                    #=== -> choose the longer one
                    oldVar = varDict[pos]
                    if len(oldVar) > len(var):
                        var = oldVar
                varDict[pos] = var

        #=== check for overlap
        positions = sorted(varDict.keys())
        posLength = len(positions)
        if posLength > 1:
            iPos = 1
            while iPos < posLength:
                if positions[iPos] < positions[iPos-1]+len(varDict[positions[iPos-1]]):
                    varDict.pop(positions[iPos])
                    positions.pop(iPos)
                    posLength=len(positions)
                else:
                    iPos+=1

        varList = varDict.values()
        if "ALL_LVPS_AI" in varExpression:
            varList.extend(self.info.vars_LVPS_AI.keys())
        if "ALL_LVPS_STATES" in varExpression:
            varList.extend(self.info.vars_LVPS_STATES.keys())
        if "ALL_HVSET" in varExpression or "ALL_SETHV" in varExpression:
            varList.extend(self.info.vars_HVSET.keys())
            if self.useCool:
                for i in xrange(2):
                    varList.remove("Set.vFix1%d" % (i+1))
                for i in xrange(4):
                    varList.remove("Set.hvIn%d" % (i+1))
                for i in xrange(7):
                    varList.remove("Set.volt%d" % (i+1))
                for i in xrange(7):
                    varList.remove("Set.temp%d" % (i+1))
        if "ALL_HV" in varExpression and not "ALL_HVSET" in varExpression:
            varList.extend(self.info.vars_HV.keys())
            if self.useCool:
                for i in xrange(4):
                    varList.remove("hvIn%d" % (i+1))
                for i in xrange(7):
                    varList.remove("volt%d" % (i+1))
                for i in xrange(7):
                    varList.remove("temp%d" % (i+1))

        return list(set(varList))


    #_______________________________________________________________________________________
    def getTree(self, drawer=None, var=None,lastOnly=False,firstAndLast=False):

        #=== parse for variables needed
        varList = self.parseVarExpression(self.varExp+self.cutExp)

        #=== get the variable
        if drawer is None:
            t = self.dataGrabber.getDCSTree(self.iovBeg, self.iovEnd, self.drawer.split(","), varList, lastOnly, firstAndLast)
        elif var is None:
            drl=[drawer]
            t = self.dataGrabber.getDCSTree(self.iovBeg, self.iovEnd, drl, varList, lastOnly, firstAndLast)
        else:
            drl=[drawer]
            varl=[var]
            t = self.dataGrabber.getDCSTree(self.iovBeg, self.iovEnd, drl, varl, lastOnly, firstAndLast)

        if t.GetEntries()==0:
            print "ERROR: No data in time intervall!"
            sys.exit(1)
        else:
            print "Found number of entries: ", t.GetEntries()

        #=== append drawer to all variables
        if self.cmd!="tree" and drawer is None and var is None:
            for var in varList:
                if "," in self.drawer:
                    newvar=""
                    newcut=""
                    if var in self.cutExp: cut=self.cutExp
                    else: cut=None
                    for dr in self.drawer.split(","):
                        newvar += dr+"."+var+","
                        if cut: newcut += dr+"."+cut+","
                    self.varExp = self.varExp.replace(var,newvar[:-1])
                    if cut: self.cutExp = newcut[:-1]
                    self.varExp = self.varExp.replace("Set."+newvar[:-1],"Set."+var)
                    if cut and "Set." in cut and not "Set." in var: self.cutExp=cut
                else:
                    self.varExp = self.varExp.replace(var,self.drawer+"."+var)
                    self.cutExp = self.cutExp.replace(var,self.drawer+"."+var)
                    self.varExp = self.varExp.replace("Set."+self.drawer,"Set")
                    self.cutExp = self.cutExp.replace("Set."+self.drawer,"Set")
                    self.varExp = self.varExp.replace(self.drawer+"."+self.drawer,self.drawer)
                    self.cutExp = self.cutExp.replace(self.drawer+"."+self.drawer,self.drawer)
            if "ALL_LVPS_AI" in self.varExp:
                newvar=""
                for dr in self.drawer.split(","):
                    for var in self.info.vars_LVPS_AI.keys():
                        newvar += dr+"."+var+","
                self.varExp = self.varExp.replace("ALL_LVPS_AI",newvar[:-1])

            if "ALL_LVPS_STATES" in self.varExp:
                newvar=""
                for dr in self.drawer.split(","):
                    for var in self.info.vars_LVPS_STATES.keys():
                        newvar += dr+"."+var+","
                self.varExp = self.varExp.replace("ALL_LVPS_STATES",newvar[:-1])

            if "ALL_HVSET" in self.varExp or "ALL_SETHV" in self.varExp:
                newvar=""
                for dr in self.drawer.split(","):
                    for var in self.info.vars_HVSET.keys():
                        if not self.useCool or not ("Set.vFix" in var or "Set.hvIn" in var or "Set.volt" in var or "Set.temp" in var):
                            newvar += dr+"."+var+","
                self.varExp = self.varExp.replace("ALL_HVSET",newvar[:-1])
                self.varExp = self.varExp.replace("ALL_SETHV",newvar[:-1])
            if "ALL_HV" in self.varExp and not "ALL_HVSET" in self.varExp :
                newvar=""
                for dr in self.drawer.split(","):
                    for var in self.info.vars_HV.keys():
                        if not self.useCool or not ("hvIn" in var or "volt" in var or "temp" in var):
                            newvar += dr+"."+var+","
                self.varExp = self.varExp.replace("ALL_HV",newvar[:-1])

            print "self.drawer: ", self.drawer
            print "self.varExp: ", self.varExp
            print "self.cutExp: ", self.cutExp

        return t


    #_______________________________________________________________________________________
    def getTimelinePlot(self):
        """
        Returns a TCanvas with the variable plotted vs time.

        """

        ROOT.gStyle.SetOptStat(0)
        t = self.getTree()

        if t.GetEntries()==1:
            print "ERROR: Only one data point, need at least 2"
            print "---> Try increasing the time span"
            sys.exit(1)

        #=== extract the values
        hh=[]
        ih=0
        hmin=1000000000
        hmax=-1000000000
        color = [2,4,3,5,6,7]
        for var in self.varExp.split(","):
          cut=self.cutExp
          cut = cut.replace("ALL_SETHV",var)
          cut = cut.replace("ALL_HVSET",var)
          cut = cut.replace("ALL_HV",var)
          cut = cut.replace("ALL",var)
          #print var
          #print cut
          t.Draw(var+":EvTime",cut,"goff");
          n = t.GetSelectedRows()
          #print "n=",n
          if n>0:
            x = t.GetV2()
            y = t.GetV1()

            #=== fix for **** root time convention and add end time
            offset = 788918400 # = 1.1.1995(UTC), the root offset
            xarr = TArrayD(n+1)
            for i in xrange(n):
                xarr[i] = x[i]-offset
            xarr[n] = self.iovEnd-offset

            #=== create and fill histogram with values
            title = self.rangeStr+";;"+self.varExp
            h = TH1D("hDCS"+str(ih),title,n,xarr.GetArray())
            for i in xrange(n):
                center = h.GetBinCenter(i+1)
                h.Fill(center,y[i])
                if y[i]>hmax: hmax=y[i]
                if y[i]<hmin: hmin=y[i]

            #=== set time display
            sec_min = 60
            sec_hrs = sec_min*60
            sec_day = sec_hrs*24
            sec_mon = sec_day*30
            timeDiff = self.iovEnd-self.iovBeg
            h.GetXaxis().SetTimeFormat("%S")
            h.SetXTitle("time [seconds]")
            if timeDiff > 2*sec_mon:
                h.GetXaxis().SetTimeFormat("%m")
                h.SetXTitle("time [month]")
            elif timeDiff > 2*sec_day:
                h.GetXaxis().SetTimeFormat("%d(%H)")
                h.SetXTitle("time [day(hour)]")
            elif timeDiff > 2*sec_hrs:
                h.GetXaxis().SetTimeFormat("%H:%M")
                h.SetXTitle("time [hour:min]")
            elif   timeDiff > 2*sec_min:
                h.GetXaxis().SetTimeFormat("%M")
                h.SetXTitle("time [min]")
            h.GetXaxis().SetTimeDisplay(1)
            h.SetLineColor(color[ih%len(color)])

            ih+=1
            hh+=[h]

        if len(hh)>1:
            delta=(hmax-hmin)/20.
            if delta<=0: delta=0.5
            hmin-=delta
            hmax+=delta
            for h in hh:
                h.SetMaximum(hmax)
                h.SetMinimum(hmin)

        return hh


    #_______________________________________________________________________________________
    def getDiffPlot(self,opt):
        """
        Returns a TCanvas with 2D plot showing difference between last and first entries.

        """

        ROOT.gStyle.SetOptStat(0)
        tree = self.getTree(firstAndLast=(opt==1))

        if tree.GetEntries()==1:
            print "ERROR: Only one data point, need at least 2"
            print "---> Try increasing the time span"
            sys.exit(1)

        if len(self.cutExp) and not "ALL" in self.cutExp:
            print "Cut expression is ignored in diff plot"

        #=== extract the values
        leaves=tree.GetListOfLeaves()
        xaxis=[]
        yaxis=[]
        for leaf in leaves:
          z=leaf.GetTitle().split('.')
          if len(z)==2 or (len(z)==3 and z[1]=="Set"):
            x=z[0]
            y=z[1]
            if len(z)==3: y+= "."+z[2]
            if not y[-2].isdigit() and y[-1].isdigit():
              y=y[:-2]+'0'+y[-1]
            if not x in xaxis: xaxis+=[x]
            if not y in yaxis: yaxis+=[y]
        xaxis.sort()
        yaxis.sort()
        nx=len(xaxis)
        ny=len(yaxis)
        if opt==1: pref="Diff "
        elif opt==2: pref="Diff_Prof "
        elif opt==3: pref="Prof "
        else: pref=""
        if opt==1: hist=ROOT.TH2F("hist2D",pref+self.outName,nx,-0.5,nx-0.5,ny,-0.5,ny-0.5)
        else:      hist=ROOT.TProfile2D("hist2D",pref+self.outName,nx,-0.5,nx-0.5,ny,-0.5,ny-0.5)

        XA=hist.GetXaxis()
        n=0
        for i in xrange(nx):
          if n: XA.SetBinLabel(i + 1, '')
          else: XA.SetBinLabel(i + 1, xaxis[i])
          n=1-n

        YA=hist.GetYaxis()
        for i in xrange(ny):
          YA.SetBinLabel(i + 1, yaxis[i])

        tree.GetEntry(0)
        val = {}
        for leaf in leaves:
          if opt>2:
            val[leaf.GetTitle()] = 0
          else:
            val[leaf.GetTitle()] = leaf.GetValue()

        ne=tree.GetEntries()
        if opt==1:
            n1=ne-1
        elif opt==2:
            n1=1
        else:
            n1=0
        bar = ProgressBar.progressBar(n1,ne, 78)
        for n in xrange(n1,ne):
          tree.GetEntry(n)
          bar.update(n)
          for leaf in leaves:
            v1=val[leaf.GetTitle()]
            z=leaf.GetTitle().split('.')
            if len(z)==2 or (len(z)==3 and z[1]=="Set"):
              x=z[0]
              y=z[1]
              if len(z)==3: y+= "."+z[2]
              if not y[-2].isdigit() and y[-1].isdigit():
                y=y[:-2]+'0'+y[-1]
              v2=leaf.GetValue()
              if "ALL" in self.cutExp:
                  var=str(v2)
                  vardiff=str(v2-v1)
                  cut=self.cutExp
                  cut = cut.replace("ALL_DIFF",vardiff)
                  cut = cut.replace("ALL_SETHV",var)
                  cut = cut.replace("ALL_HVSET",var)
                  cut = cut.replace("ALL_HV",var)
                  cut = cut.replace("ALL",var)
                  ok = eval(cut)
                  #print cut
                  #print ok
              else:
                  ok = True
              if ok:
                  hist.Fill(xaxis.index(x),yaxis.index(y),v2-v1)
        bar.done()

        return hist


    #_______________________________________________________________________________________
    def getDistributionPlot(self):
        """
        Returns a TCanvas with the variable distribution plotted (using weights)

        """

        t = self.getTree()

        hh=[]
        ih=0
        color = [2,4,3,5,6,7]
        xmin=1000000000
        xmax=-1000000000
        ymin=1000000000
        ymax=-1000000000
        hmin=1000000000
        hmax=-1000000000
        dim=0
        xtit=""
        ytit=""
        for var in sorted(self.varExp.split(",")):
            cut=self.cutExp
            dim = var.count(':')
            if dim>0:
                varX = var.split(":")[1]
                varY = var.split(":")[0]
                if len(cut):
                    cutY = cut
                    cut = cut.replace("ALL_SETHV",varX)
                    cut = cut.replace("ALL_HVSET",varX)
                    cut = cut.replace("ALL_HV",varX)
                    cut = cut.replace("ALL",varX)
                    cutY = cutY.replace("ALL_SETHV",varY)
                    cutY = cutY.replace("ALL_HVSET",varY)
                    cutY = cutY.replace("ALL_HV",varY)
                    cutY = cutY.replace("ALL",varY)
                    cut = "("+cut+") && ("+cutY+")"
            else:
                varX=var
                varY=""
                if len(cut):
                    cut = cut.replace("ALL_SETHV",varX)
                    cut = cut.replace("ALL_HVSET",varX)
                    cut = cut.replace("ALL_HV",varX)
                    cut = cut.replace("ALL",varX)
            if cut!="":
                cut = "weight*("+cut+")"
            else:
                cut = "weight"
            #print var
            #print cut

            t.Draw(var,cut,"goff");
            h = ROOT.gDirectory.Get("htemp")
            h.SetXTitle(var.split(":")[0])
            h.SetTitle(self.rangeStr)
            h.SetName("TileDCSDataPlotter"+str(ih))
            #print "n=",h.GetEntries()

            if h.GetEntries()>0:
                xmi = h.GetXaxis().GetBinLowEdge(1)
                xma = h.GetXaxis().GetBinLowEdge(h.GetNbinsX())+h.GetXaxis().GetBinWidth(h.GetNbinsX())
                if xmi<xmin: xmin=xmi
                if xma>xmax: xmax=xma
                if dim>0:
                    ymi = h.GetYaxis().GetBinLowEdge(1)
                    yma = h.GetYaxis().GetBinLowEdge(h.GetNbinsY())+h.GetYaxis().GetBinWidth(h.GetNbinsY())
                    if ymi<ymin: ymin=ymi
                    if yma>ymax: ymax=yma
                hmi = h.GetMinimum()
                hma = h.GetMaximum()
                if hmi<hmin: hmin=hmi
                if hma>hmax: hmax=hma
                xtit+=varX+","
                if dim>0: ytit+=varY+","

            h.SetLineColor(color[ih%len(color)])
            h.SetMarkerColor(color[ih%len(color)])

            ih+=1
            hh+=[h]

        if len(hh)>1:
            if xmin>xmax:
                xmin=0
                xmax=0
            delta=(xmax-xmin)/20.
            if delta<=0: delta=0.5
            xmin-=delta
            xmax+=delta
            if ymin>ymax:
                ymin=0
                ymax=0
            delta=(ymax-ymin)/20.
            if delta<=0: delta=0.5
            ymin-=delta
            ymax+=delta
            if hmin>hmax:
                hmin=0
                hmax=1
            delta=(hmax-hmin)/20.
            if delta<=0: delta=0.5
            if hmin!=0: hmin-=delta
            hmax+=delta
            if dim>0:
                h0 = TH2D("hDCS","dummy",2,xmin,xmax,2,ymin,ymax)
                h0.SetYTitle(ytit[:-1])
            else:
                h0 = TH1D("hDCS","dummy",2,xmin,xmax)
            h0.SetXTitle(xtit[:-1])
            h0.SetTitle(self.rangeStr)
            h0.SetMaximum(hmax)
            if hmin!=0: h0.SetMinimum(hmin)
            hh=[h0]+hh
            ROOT.gStyle.SetOptStat(0)

        return hh


    #_______________________________________________________________________________________
    def printDistributionStatAll(self):
        """
        Print mean values from histograms

        """

        t = self.getTree()
        if self.cutExp!="":
            for v, c in zip(self.varExp.split(","), self.cutExp.split(",")):
                t.Draw(v,c,"goff");
                h = ROOT.gDirectory.Get("htemp")

                print "%s  %s  %s  Nentries %d  Mean %7.3f  RMS %8.4f" % \
                    (v,c,self.rangeStr,h.GetEntries(),h.GetMean(),h.GetRMS())
        else:
            cut = ""
            for v in self.varExp.split(","):
                t.Draw(v,cut,"goff");
                h = ROOT.gDirectory.Get("htemp")
                print "%s  %s  Nentries %d  Mean %7.3f  RMS %8.4f" % \
                    (v,self.rangeStr,h.GetEntries(),h.GetMean(),h.GetRMS())

        return 0


    #_______________________________________________________________________________________
    def printDistributionStat(self, lastOnly=False):
        """
        Print mean values from histograms

        """

        if "ALL_LVPS_AI" in self.varExp:
            self.varExp = self.varExp.replace("ALL_LVPS_AI",",".join(sorted(self.info.vars_LVPS_AI.keys())))
        if "ALL_LVPS_STATES" in self.varExp:
            self.varExp = self.varExp.replace("ALL_LVPS_STATES",",".join(sorted(self.info.vars_LVPS_STATES.keys())))
        if "ALL_HVSET" in self.varExp or "ALL_SETHV" in self.varExp:
            vlist = self.info.vars_HVSET.keys()
            if self.useCool:
                for i in xrange(2):
                    vlist.remove("Set.vFix1%d" % (i+1))
                for i in xrange(4):
                    vlist.remove("Set.hvIn%d" % (i+1))
                for i in xrange(7):
                    vlist.remove("Set.volt%d" % (i+1))
                for i in xrange(7):
                    vlist.remove("Set.temp%d" % (i+1))
            self.varExp = self.varExp.replace("ALL_HVSET",",".join(sorted(vlist)))
            self.varExp = self.varExp.replace("ALL_SETHV",",".join(sorted(vlist)))
        if "ALL_HV" in self.varExp and not "ALL_HVSET" in self.varExp:
            vlist = self.info.vars_HV.keys()
            if self.useCool:
                for i in xrange(4):
                    vlist.remove("hvIn%d" % (i+1))
                for i in xrange(7):
                    vlist.remove("volt%d" % (i+1))
                for i in xrange(7):
                    vlist.remove("temp%d" % (i+1))
            self.varExp = self.varExp.replace("ALL_HV",",".join(sorted(vlist)))
        print self.varExp

        reslist=[]
        cutlist = self.parseVarExpression(self.cutExp)
        print cutlist
        t = None
        drlistprev=[]
        varlistprev=[]
        for drawer in self.drawer.split(","):
            drlist=[drawer]
            for v in self.varExp.split(","):
                varlist=self.parseVarExpression(v)
                varlist += cutlist
                varlist=sorted(list(set(varlist)))
                #for drawer in self.drawer.split(","):
                #    drlist=[drawer]
                if not (t and drlist==drlistprev and varlist==varlistprev):
                    t = self.dataGrabber.getDCSTree(self.iovBeg, self.iovEnd, drlist, varlist, lastOnly)
                    drlistprev=drlist
                    varlistprev=varlist
                if t and t.GetEntries()>0:
                    var=v
                    cut=self.cutExp
                    cut = cut.replace("ALL_SETHV",var)
                    cut = cut.replace("ALL_HVSET",var)
                    cut = cut.replace("ALL_HV",var)
                    cut = cut.replace("ALL",var)
                    for va in varlist:
                        var = var.replace(va,drawer+"."+va)
                        cut = cut.replace(va,drawer+"."+va)
                    var = var.replace("Set."+drawer,"Set")
                    cut = cut.replace("Set."+drawer,"Set")
                    var = var.replace(drawer+"."+drawer,drawer)
                    cut = cut.replace(drawer+"."+drawer,drawer)
                    print "var is",var
                    print "cut is",cut
                    t.Draw(var,cut,"goff");
                    h = ROOT.gDirectory.Get("htemp")
                    if h:
                        if cut!="":
                            reslist.append((var+" CUT "+cut,self.rangeStr,h.GetEntries(),h.GetMean(),h.GetRMS()))
                        else:
                            reslist.append((var,self.rangeStr,h.GetEntries(),h.GetMean(),h.GetRMS()))
                    else:
                        reslist.append((var,self.rangeStr,-1,0.0,0.0))

        if lastOnly:
            for res in reslist:
                print "%-25s  %s        Last %-9.3f" % (res[0],res[1],res[3])
        else:
            for res in reslist:
                print "%-25s  %s        Nentries %-7d  Mean %-9.3f  RMS %-9.4f" % res
        return 0


#================================================================================================


#==== interactive use
callName = os.path.basename(sys.argv[0])
if callName=="TileDCSDataPlotter.py":

    #=== check if we are in batch mode
    batch = False
    if "-b" in sys.argv:
        batch=True
        sys.argv.remove("-b")


    #=== check if we are in Oracle mode
    useCool = False
    if "--cool" in sys.argv:
        useCool=True
        sys.argv.remove("--cool")

    #=== check if we are in Oracle mode
    useTestBeam = False
    if "--testbeam" in sys.argv:
        useTestBeam=True
        sys.argv.remove("--testbeam")


    #=== check if we are in verbose mode
    verbose = False
    if "-v" in sys.argv:
        verbose=True
        sys.argv.remove("-v")

    #=== catch invalid commands
    cmd = sys.argv[1]
    if not(cmd=="plot" or cmd=="diff" or cmd=="diffprof" or cmd=="prof" or cmd=="dist" or cmd=="tree"  or cmd=="mean"  or cmd=="last"):
        print """ Please use one of the following commands: plot, diff, dist, tree, mean, last !"""
        sys.exit(1)

    #=== command is recognized, we go on....
    ROOT.gROOT.SetBatch()
    dp = TileDCSDataPlotter(sys.argv, useCool, useTestBeam, verbose, putDuplicates=(cmd =="mean" or cmd =="dist"))

    #=== no graphics if only get tree
    if cmd == "tree":
        t = dp.getTree()
        f = ROOT.TFile(dp.outName+".root", "recreate")
        t.Write()
        f.Close()

    #=== distribution plot (using weights)
    elif cmd =="mean":
        dp.printDistributionStat()
    elif cmd =="last":
        dp.printDistributionStat(True)

    else:
        #=== graphics output below -> configure style and canvas
        ROOT.gROOT.SetStyle("Plain")
        ROOT.gROOT.ForceStyle()
        if cmd =="diff" or cmd=="diffprof" or cmd=="prof":
            can = TCanvas("c_TileDCSDataPlotter","c_TileDCSDataPlotter",0,0,1200,900)
        else:
            can = TCanvas("c_TileDCSDataPlotter","c_TileDCSDataPlotter")

        #=== simple timeline plot
        if cmd =="plot":
            hh = dp.getTimelinePlot()
            can.SetGridx()
            can.SetGridy()
            opt="HIST"
            for h in hh:
                h.Draw(opt)
                opt="HISTSAME"

        #=== simple diff plot
        if cmd =="diff":
            h = dp.getDiffPlot(1)
            h.Draw('COLZ')

        #=== simple diff profile plot
        if cmd =="diffprof":
            h = dp.getDiffPlot(2)
            h.Draw('COLZ')

        #=== simple diff profile plot
        if cmd =="prof":
            h = dp.getDiffPlot(3)
            h.Draw('COLZ')

        #=== distribution plot (using weights)
        elif cmd =="dist":
            hh = dp.getDistributionPlot()
            if len(hh)>0:
                dim = dp.varExp.split(',')[0].count(':')
                opt=""
                pr=(len(hh)==1)
                for h in hh:
                    if dim==0:
                        h.Draw("EHIST"+opt)
                        if pr:
                            print "%s  %s  Nentries %d  Mean %7.3f  RMS %8.4f" % \
                                (h.GetXaxis().GetTitle(),h.GetTitle(),h.GetEntries(),h.GetMean(),h.GetRMS())
                        else:
                            pr=True
                    elif dim==1:
                        print "2D plot, opt ",opt
                        h.Draw("BOX"+opt)
                    else:
                        h.Draw(opt)
                    opt="SAME"

        #=== save output in eps and png
        can.Print(dp.outName+"_"+cmd+".eps")
        can.Print(dp.outName+"_"+cmd+".png")

        #=== display plot
        if not batch:
            os.system("display "+dp.outName+"_"+cmd+".png")
