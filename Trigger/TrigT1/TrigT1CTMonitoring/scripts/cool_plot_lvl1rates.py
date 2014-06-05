#!/bin/env python2.5

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# L1CT crew, Johan.Lundberg@cern.ch, CERN, 2010
#
# Reads histogram created by cool_read_lvl1rates.py

import sys,getopt,os

def usage():
    print "%s" % sys.argv[0]
    print "  --help                                   this printout"
    print " "
    print " --counters List,Of,Regular,Expressions    To force exact match, "
    print "                                           use something like L1_EM3$ " 

    print " --vetos list,Of,Regular,Expressions       Excluded after matched by --counters pattern"
    print " --norunname                               no run name for output plot"
    print " --outbase                                 base name for plot file name"
    print " --maxcurves (default 10)                  maximum number of curves per plot" 
    print " --maxpads (default 5)                     skip all pads after maxpad"
    print " --oneplot                                 force all curves to go into one single plot"
    print " --lb lumiblock range                                  "
    print " --TAV --TAP --TBP                         show only one of TAV,TAP,TBP"
    print " --magnitudes X                            number of magnitudes shown (max*10**-X -> min)"
    print " --forcetitle text                         Set plot tile to this"        
    print " "
    print " Mandatory: "
    print "  --inputfile | -i <filename>              A .root file "    
    print " --timeshift hours                         shift times [to change timezone] "
    print " "
    print " example: "
    print " cool_read_lvl1rates.py --run 152166 --fillHist"
    print " cool_plot_lvl1rates.py -i RUN_152166_hist.root --counters MBTS_1$,L1_LUCID_A$,L1_EM3$,ZDC_A$ --TAV --outbase TAV --forcetitle 'L1 TAV Item rates [Hz]'"
    

    
try:
    longopts=['inputfile=','i=','help','counters=','vetos=','maxcurves=','oneplot',
              'outbase=','norunname','maxpads=','magnitudes=','forcetitle=','lb=','TAV','TAP','TBP']
    opts,args=getopt.getopt(sys.argv[1:],'i:o:',longopts)
except getopt.GetoptError,e:
    usage()
    sys.exit(2)

# root is silly as usual and tries to read argv for whatever and gives warnings
sys.argv[1:]=[]

fname=""
vetos=[]
patterns=["TAV"]
import l1ctpy
maxcurves=10
maxpads=5
skipcurves=0
lbrange='-'
prepattern=""
BaseName="L1_Items_"
DoRunName=True
oneplot=False
magnitudes=3.5 # orders of magnitude in each plot
forcetitle=False

for o,a in opts:
    if o in ('-h','--help'):
        usage()
        sys.exit(0)
    if o in ('--inputfile','-i'):
        fname=a
    if o in ('--vetos'):
        vetos=l1ctpy.text_to_ranges(a)
        vetos=l1ctpy.expandlist(vetos,0,256-1)
    if o in ('--counters'):
        patterns=l1ctpy.text_to_ranges(a)
        patterns=l1ctpy.expandlist(patterns,0,256-1)
    if o in ('--maxcurves'):
        maxcurves=int(a)
    if o in ('--maxpads'):
        maxpads=int(a)
    if o in ('--skipcurves'):
        skipcurves=int(a)
    if o in ('--outbase'):
        BaseName=str(a)
    if o in '--lb' :
        lbrange=a
    if o in '--norunname' :
        DoRunName=0
    if o in '--TAP' :
        prepattern="TAP"
    if o in '--TAV' :
        prepattern="TAV"
    if o in '--TBP' :
        prepattern="TBP"
    if o in '--oneplot' :
        oneplot=True
    if o in '--magnitudes' :
        magnitudes=float(a)
    if o in '--forcetitle' :
        forcetitle=a

if oneplot:
    maxpads=1
    maxcurves=1e9
        
if not fname:
    usage()
    sys.exit(2)

## read file
lbrange=l1ctpy.text_to_ranges(lbrange)
if len(lbrange)>1:
    print "sorry, can not treat composit LB range :",lbrange
    sys.exit(1)


import ROOT
ROOT.gStyle.SetFrameFillColor(0);
#ROOT.gStyle.SetFillColor(2);
ROOT.gROOT.ForceStyle();
ROOT.gStyle.SetOptStat(0);

#fname='RUN_149158_hist.root'

ROOT.gStyle.SetCanvasBorderMode(0);
#ROOT.gStyle.BorderColor(1);
ROOT.gStyle.SetOptTitle(1);
ROOT.gStyle.SetTitleBorderSize(0)
ROOT.gStyle.SetTitleFillColor(0)

    
# find run number
f = ROOT.TFile(fname)
runhist=f.FindObjectAny("RunNR")
runnr=int(0.1+runhist.Integral())

#print runnr

# make full list of root histograms

histos=set()
for thisdir in f.GetListOfKeys():
    try:
        for i in f.GetDirectory(thisdir.GetName()).GetListOfKeys():
#            histos.add(thisdir.GetName()+"/"+i.GetName())
            histos.add(i.GetName())                
    except AttributeError:
        pass

histos=list(histos)
histos.sort()
#print histos
#print patterns
newpat=[]
for p in patterns:
    if p:
        newpat.append(prepattern+".*"+p)
patterns=newpat

def finditems(histos,patterns,vetos):
    import re
    import operator
    myvetos=vetos+['TimeGraph']
    def selector(x):
        return (re.match('.*ctpid.*Item_L1.*',x) and
                max(map(lambda thisrule :bool(re.match(".*"+thisrule+".*",x)),patterns)) and not
                max(map(lambda thisrule :bool(re.match(".*"+thisrule+".*",x)),myvetos)))
    out=filter(selector , histos)    
    return filter(selector, out)


# sort all histograms base on their maximum bin content
#print histos
#print patterns
histos=finditems(histos,patterns,vetos)

    
def sortkey(thisname):
    thisobj=f.FindObjectAny(thisname)
    if 'ROOT.TH' in str(thisobj):
#        print "For curve: ", thisobj.GetName(), " peak rate :", thisobj.GetBinContent(thisobj.GetMaximumBin()), "at lb", thisobj.GetXaxis().GetBinCenter(thisobj.GetMaximumBin())
        return [str(thisobj)[:9],-thisobj.GetBinContent(thisobj.GetMaximumBin())]
    
    else:
        return [str(thisobj)[:9],-thisobj.GetMaximum()]

histos.sort(key=sortkey)

import math

def find_groupcurves():
    istart=0
    i=istart
    selected=[]
    for itm in histos:
        if i == istart:
            scalemax=0
            scalemin=1e15
        thisobj=f.FindObjectAny(itm)
        if 'ROOT.TH' in str(thisobj):
            i+=1
            this_scalemax=thisobj.GetBinContent(thisobj.GetMaximumBin())
            forcenewscale=0
            print "Considering: %-40s min %.1e max %.1e " % (thisobj.GetName(), this_scalemax, scalemax)
            if this_scalemax == 0 :
                print "skipping",thisobj.GetName()
                break
            if scalemax and this_scalemax < scalemax/1e1:
                forcenewscale=1
            scalemax=max(scalemax,this_scalemax)
            if (i-istart)>=maxcurves or forcenewscale:
                scalemin=min(scalemin,thisobj.GetMinimum())
                scalemin=min(scalemin,scalemax*.1)    
                scalemax=scalemax*1.07
                scalemin=max(scalemin*.9,scalemax*math.pow(10,-magnitudes))
                scalemax=max(scalemax,11*scalemin)

                if oneplot and selected:
                    selected[0][1]=i; #[[selected[0][0] ,i,scalemin,selected[0][3]]]                   
                else:
                    selected.append([istart ,i,scalemin,scalemax])
                    istart=i
                

    return selected

groupcurves= find_groupcurves()
#print groupcurves

if (maxpads>0):
    groupcurves=groupcurves[:maxpads]
#groupcurves=groupcurves[:5]
iloop=0
thisname=BaseName
if DoRunName:
    thisname+=str(runnr)

c1=ROOT.TCanvas(thisname, thisname,100,100,900,260*max(1,len(groupcurves)))
c1.SetRightMargin(0.00)
c1.SetLeftMargin(0.1)
c1.SetTopMargin(0.15)
c1.Divide(1,len(groupcurves),0.00,0.05)
#$,0.1,0.2)
#print len(groupcurves)
#sys.exit()
legends=[]

c1.cd(0)
def cleanstrlist(inlist):
    return str(inlist).replace("'.*",'').replace(".*'",'').replace('[','').replace(']','').replace("'",'')

if forcetitle:
    titlename=forcetitle
else:
    titlename="   L1 Item Rates: "+cleanstrlist(patterns)
if vetos:
    titlename+=" (skipping: "
    titlename+=cleanstrlist(vetos)
    titlename+=")"
if groupcurves:
    ypoint=1-0.08/len(groupcurves)
    fudgedown=(min(5,len(groupcurves)))
else:
    ypoint=0
    fudgedown=1
mytext=ROOT.TLatex(.02,ypoint,titlename)
mytext.SetTextSize(3.5*.027/fudgedown)
mytext.SetNDC()
mytext.Draw()

def refill_to_timescale(inhist):


    timelb=f.FindObjectAny("LumiBlocks")                
                        
    nbins=timelb.GetNbinsX()
    if not nbins == inhist.GetNbinsX():
        print "Sorry, Time axis does not have the same number of bins as lb axis" 
        sys.exit(1)

    import random
    randname="%x"%int(1e10*random.uniform(0,1))
    outhisttime=timelb.Clone()    
    outhisttime.SetName(randname)
    outhisttime.SetTitle(inhist.GetTitle())
    for i in range(nbins+1):
        outhisttime.SetBinContent(i,inhist.GetBinContent(i))

    return outhisttime

timelb=f.FindObjectAny("LumiBlocks")
lbaxis=range(1,1+timelb.GetNbinsX())
timeaxis=[]
root_axis=timelb.GetXaxis()
for i in lbaxis:
    timeaxis.append(root_axis.GetBinLowEdge(i))
#    print root_axis.GetBinLowEdge(i)

#print groupcurves

lbrange=l1ctpy.expandlist(lbrange,1,lbaxis[-1])
#print lbrange
# # Work out LB axis ranges in terms of time:

timelow=timeaxis[0]         # default is to cover the whole range
timehigh=timeaxis[-1]

lbrange=[lbrange[0],lbrange[-1]]
#print lbrange
import time
if lbrange[0]:
    for t,l in zip(timeaxis,lbaxis):
        if l<=lbrange[0]:
            timelow=t
        else:
            break
if lbrange[1]:
    for t,l in zip(timeaxis,lbaxis):
        if lbrange[1]==l:
            timehigh=t
            break

#print timelow,timehigh
#print lbrange
#if timelow: print time.localtime(timelow)
#if timehigh: print time.localtime(timehigh)

#sys.exit()
#groupcurves=[[0, 9, 6.4990850753784185e-04, 80.990850753784187]]


memkeep=[]
for istart,istop,scalemin,scalemax in groupcurves:

    iloop+=1
    first=1
#    c1=ROOT.TCanvas(thisname, thisname,100,100,900,300)
    c1.cd(iloop)
    c1.SetLogy()
    c1.SetTopMargin(0.08)

#    ROOT.gStyle.SetLabelFont(63);
#    ROOT.gROOT.ForceStyle()

    ROOT.gPad.SetBottomMargin(0.13)
    ROOT.gPad.SetLeftMargin(0.05)
    ROOT.gPad.SetRightMargin(0.25)
    ROOT.gPad.SetTopMargin(0.1)
    #    ROOT.gPad.SetTopMargin(0.0)
    #if iloop==1 and first==1:
    #
    #        ROOT.gPad.SetBottomMargin(0)
    ROOT.gPad.SetLogy(1)
    tleg=ROOT.TLegend(0.76,0.15,1,0.95)
    tleg.SetFillStyle(0)
    tleg.SetBorderSize(0)
    legends.append(tleg)
    plotstyles=[]
    # thick color style bgcolor bcstyle

    i=0
    icount=0
    for itm in histos:
        baseplot=None
    #    print itm
        thisobj=f.FindObjectAny(itm)
        if 'ROOT.TH' in str(thisobj):

            thisobj=refill_to_timescale(thisobj)
            memkeep.append(thisobj)
                        
            icount+=1
            if icount<=istart:
                i=0
                first=1
                continue
            #print thisobj.GetTitle()
            plotstring="_"
            thisobj.SetStats(0)
            if i==6:
                i+=1
            lw=2+((i+2)%2)
            lc=1+((i)%9)
            ls=1
            thisobj.SetFillStyle(0)
            if 1:
                if lc == 5:
                    lc=14
                ls=1+int((i/6))
                if not i == 0 and lc == 1:
                    lc=32
                if not (i)%3 :            
                    if not i:
                        thisobj.SetFillStyle(1001)
                        thisobj.SetFillColor(lc)
                        lw=lw/2
                        ls=1
                    elif not i%2 :
                        pass
                    #thisobj.SetFillStyle(3115)
                    else:
                        lw=lw/2
                        thisobj.SetFillStyle(3004+(i%5)%2)
                        thisobj.SetFillColor(lc)
                        ls=1

                if ls==1 and i>1:
                    lw=1
                    
                if thisobj.GetFillStyle()>0:
                    ls=1
                    lw=1
                
#                print ls,lw,thisobj.GetFillStyle()
                thisobj.SetLineWidth(lw)
                thisobj.SetLineColor(lc)
                thisobj.SetLineStyle(ls)
                
#                if thisobj.GetFillStyle()>3000:
#                    thisobj.SetLineStyle(0)
#                    thisobj.SetLineWidth(0)


            i+=1

            if first:

                baseplot=thisobj.Clone("hejsan")
                baseplot.Reset()
                baseplot.Clear()
                baseplot.SetMaximum(scalemax)
                baseplot.SetMinimum(scalemin)
                #baseplot.GetXaxis().SetRangeUser(160, 250);
                baseplot.GetXaxis().SetLabelSize(.08)

                # convert number of seconds [in terms of localtime] to number of seconds
                # in UTC:
                import time
                # NOTE - this gives a DT time that is only correct up to the hour::min, not the time and date
                DT=(time.mktime(time.gmtime(int(timelow))[0:-1]+(-1,))-int(timelow))
                print "applying time zone and daylight time diff:",DT," seconds"

                
                baseplot.GetXaxis().SetTimeOffset(ROOT.gStyle.GetTimeOffset()+DT)
                baseplot.GetXaxis().SetLabelSize(.07)
                baseplot.GetYaxis().SetLabelSize(.07)
                baseplot.GetXaxis().SetLabelOffset(.02)
                baseplot.GetYaxis().SetLabelOffset(.00)
                baseplot.GetYaxis().SetTitle("")
                baseplot.GetXaxis().SetTitle("UTC Time")
                baseplot.GetXaxis().SetTitleOffset(1)
                baseplot.GetXaxis().SetTitleSize(.06)
                baseplot.GetYaxis().SetTitleOffset(.0)
                baseplot.SetTitle("")
                baseplot.GetXaxis().SetRangeUser(timelow,timehigh);
                # baseplot.GetXaxis().SetRangeUser(160, 250);
                baseplot.DrawCopy("")
#                if iloop==1:
                l1ctpy.drawaxis_timeLB(baseplot,timeaxis,lbaxis,1,1.7)
                                
            thisobj.GetXaxis().SetRangeUser(timelow,timehigh);
            thisobj.Draw("same")


            nicetitle=thisobj.GetTitle()
            nicetitle=nicetitle[(3+nicetitle.rfind('L1_')):]    
            thisfs=thisobj.GetFillStyle()
            # workaround for bug in root painting mode (for no x-display)
            if thisfs>1000:
                tleg.AddEntry(thisobj,nicetitle,"F")
            else:
                tleg.AddEntry(thisobj,nicetitle,"L")
            first=0
            if icount==istop:
                tleg.Draw()
                c1.SetLogy(1)
                
                legends.append(mytext)

                break

if not groupcurves:
    ROOT.gPad.SetBottomMargin(0.13)
    ROOT.gPad.SetLeftMargin(0.05)
    ROOT.gPad.SetRightMargin(0.25)
    ROOT.gPad.SetTopMargin(0.1)
    h=ROOT.TH1F()
    h.Draw("A")
    tl=ROOT.TLatex(0.2,0.2,"sorry - no data")
    tl.Draw()

c1.Print(thisname+".png")            
            
        #for thisobj in f.FindObject(thisdir.GetName).GetListOfKeys():
        #    print thisobj.GetName()

