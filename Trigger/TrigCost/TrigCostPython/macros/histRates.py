#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
from TrigCostPython import TrigCostAnalysis, TrigCostXML 
from optparse import OptionParser
import os, sys, string, math, shutil
import ROOT
ROOT.gROOT.SetBatch(True)

#-----------------------------------------------------------------------
#  Options definitions
#-----------------------------------------------------------------------
from optparse import OptionParser

p = OptionParser(usage='<rates1 xml> ... <ratesN xml> -o <output location>')

p.add_option('-r', '--ratelist', type = 'string', default = '',
             dest = 'ratelist', help = 'comma seperated list of rates to plot' )

p.add_option('-n', '--ratenames', type = 'string', default = '',
             dest = 'ratenames', help = 'comma seperated list of names for rate (default is the rate name from the -r list)' )

p.add_option('-o', '--outname', type = 'string', default = '',
             dest = 'outname', help = 'output file name, specifies graphics file type as ROOT does' )

p.add_option( "-l", "--labels", type    = "string", default = "",
              dest = "labels",
              help = "comma separated list of column labels" )

p.add_option( "-p", "--plotlabel", type    = "string", default = "",
              dest = "plotlabel",
              help = "Label for the top of the plot" )

p.add_option('-m', '--leftmargin', type = 'float', default = 0.0,
             dest = 'lmargin', help = 'left margin size')

p.add_option('', '--lumicorrect', type = 'float', default = 1.0,
             dest = 'lumicorrect', help = 'luminosity correction')             

(opts, args) = p.parse_args()

if opts.labels=="" or opts.outname=="" or opts.ratelist=="":
    print "Arguments lables, outname, and ratelist are required"
    sys.exit(-1)
    
labels=string.split(opts.labels,",")
if len(labels) != len(args):
    print "Labels not the same length as list of xml files"
    print "Labels: ",labels
    print "XMLs: ",args
    sys.exit(-1)

ratelist=string.split(opts.ratelist,",")
if len(ratelist)<1:
    print "Need at least one rate in ratelist to plot"
    sys.exit(-1)

ratenames=[]
if opts.ratenames!='':
    ratenames=string.split(opts.ratenames,",")
    if len(ratelist)!=len(ratenames):
        print "ratenames must have the same number of entries are ratelist"
        sys.exit(-1)
else:
    ratenames=ratelist

plotlabels=[]
if opts.plotlabel!="":
    plotlabels=string.split(opts.plotlabel,'\\\\')

#-----------------------------------------------------------------------
#  Read in XML files
#-----------------------------------------------------------------------
results = []
for i in xrange(len(args)) :    
    results.append(TrigCostXML.ReadXmlFile(args[i]))

#-----------------------------------------------------------------------
#  Make plot
#-----------------------------------------------------------------------
canvas = ROOT.TCanvas('rates_'+opts.outname, 'rates_'+opts.outname,10,10,700,90*len(ratelist))
canvas.SetFillColor(10)
canvas.SetLogx(1)

legend=ROOT.TLegend(0.725,0.16,1.00,0.280)
legend.SetFillColor(10)
legend.SetFillStyle(0)
legend.SetTextSize(0.035)
legend.SetBorderSize(0)

rateBars = {}
max_label_width = 0
max_rate=0
for r in xrange(len(results)):
    result = results[r]
    name   = labels[r]
    rateBars[name] = ROOT.TH1F("rateBar_"+name, "", len(ratelist), 0, len(ratelist))

    barspacing = 0.08
    barmargin  = 0.0
    barwidth   = (1.0-2*barspacing)/float(len(results))
    
    rateBars[name].SetBarOffset(barspacing+(len(results)-r-1)*barwidth)
    rateBars[name].SetBarWidth(barwidth-barmargin)
    rateBars[name].SetFillColor(2*r+2)
    rateBars[name].SetLineColor(2*r+2)
    stylenum=1001
    #stylenum+=2*(r%2)+5*(1-(r%2)) # 5 if odd mean no strips one way
    #stylenum+=10*(2*(1-r%2)+5*(r%2)) # 5 if even mean no strips the other way
    if (r%2):
        stylenum=3350+2*(r%2)
    print stylenum
    rateBars[name].SetFillStyle(stylenum)
    rateBars[name].SetYTitle('Rate (Hz)')
    rateBars[name].SetStats(0)
    
    for i in xrange(len(ratelist)):
        ratename=ratelist[i]
        max_label_width = max(max_label_width,len(ratename))
        if not result.GetChain(ratename):
            print "Missing rate for requested chain ",ratename," in sample ",labels[r]
            sys.exit(-1)
        rate=result.GetChain(ratename).GetRate()
        
        if name=='Prediction' and opts.lumicorrect!=1.0:
            print "correcting lumi",name
            rate=rate*opts.lumicorrect
        max_rate=max(max_rate,rate)        
        rateBars[name].SetBinContent(len(ratelist)-i,rate)

lmargin=max_label_width/45.0
if opts.lmargin !=0.0:
    lmargin=opts.lmargin

tmargin=len(plotlabels)*0.05+0.025
canvas.SetTopMargin(tmargin)
canvas.SetBottomMargin(0.15)
canvas.SetRightMargin(0.05)
canvas.SetLeftMargin(lmargin)

for r in xrange(len(results)):
    legend.AddEntry(rateBars[labels[r]],labels[r],"f")
    
    if r==0:
        for i in xrange(len(ratenames)):
            rateBars[labels[r]].GetXaxis().SetBinLabel(len(ratenames)-i,ratenames[i])
        rateBars[labels[r]].GetXaxis().SetLabelSize(0.05)
        rateBars[labels[r]].GetYaxis().SetLabelSize(0.05)
        rateBars[labels[r]].GetYaxis().SetLabelOffset(0.0)
        rateBars[labels[r]].GetYaxis().SetTitleOffset(1.0)
        rateBars[labels[r]].GetYaxis().SetTitleSize(0.05)
        rateBars[labels[r]].SetMaximum(1.1*max_rate)
        rateBars[labels[r]].Draw('hbar')
    else:
        rateBars[labels[r]].Draw('hbar same')

legend.Draw()

yval=0.945
latex=[]
for l in plotlabels:
    latex.append(ROOT.TLatex(lmargin,yval,l.strip()))
    latex[-1].SetTextFont(42)
    latex[-1].SetTextSize(0.0425)
    latex[-1].SetNDC()
    latex[-1].Draw()
    yval=yval-0.05

print opts.outname[:-4]
if opts.outname[-4:]=='.set':
    #print 'root',ROOT.gSystem.AccessPathName(opts.outname[:-4]+'.root',ROOT.kWritePermission)
    #print 'eps',ROOT.gSystem.AccessPathName(opts.outname[:-4]+'.eps',ROOT.kWritePermission)
    canvas.SaveAs(opts.outname[:-4]+'.eps')
    canvas.SaveAs(opts.outname[:-4]+'.root')
    canvas.SaveAs(opts.outname[:-4]+'.gif')
    canvas.SaveAs(opts.outname[:-4]+'.pdf')
    
else:
    canvas.SaveAs(opts.outname)
