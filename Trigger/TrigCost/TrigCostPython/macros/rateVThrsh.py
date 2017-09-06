#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
from TrigCostPython import TrigCostAnalysis, TrigCostXML 
from optparse import OptionParser
import os, re, sys, string, math, shutil
import ROOT
#ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)

#-----------------------------------------------------------------------
#  Options definitions
#-----------------------------------------------------------------------
from optparse import OptionParser

p = OptionParser(usage='<rate xml> -r <rate template> -o <output location>')

p.add_option('-r', '--ratetemplate', type = 'string', default = '',
             dest = 'ratetemplate', help = 'rate with a NNN in the spot where the threhold number goes,'+
             ' if comma seperated multiple versions will be put in the legend' )

p.add_option( "-p", "--plotlabel", type    = "string", default = "",
              dest = "plotlabel",
              help = "Label for the top of the plot" )

p.add_option('-o', '--outname', type = 'string', default = 'test.gif',
             dest = 'outname', help = 'output file name, specifies graphics file type as ROOT does' )

p.add_option('', '--logx',   action="store_true",  default=False, help="set the x-axis to log")
p.add_option('', '--logy',   action="store_true",  default=False, help="set the y-axis to log")

(opts, args) = p.parse_args()

if opts.ratetemplate=="":
    print "Argument ratetempate is required"
    sys.exit(-1)
    
#-----------------------------------------------------------------------
#  Read in XML file
#-----------------------------------------------------------------------
if len(args)!=1:
    print "Expected exactly one XML input file"
    sys.exit(-1)
result = TrigCostXML.ReadXmlFile(args[0])

#-----------------------------------------------------------------------
#  Find matching triggers
#-----------------------------------------------------------------------
chains = {}
thrmin=1.0e10
thrmax=0.0
ratemin=1.0e10
ratemax=0.0
for ratetemplate in string.split(opts.ratetemplate,","):
    # chains will be list of chains(=1) with thresholds(=0)
    chains[ratetemplate] = []
    
    retmp  = ratetemplate.replace("NNN","(?P<thr>[0-9]*)")
    print "regexptmp = ",retmp
    search = re.compile(retmp)


    
    for r in result.GetChains():
        res = search.match(r.GetName())
        # match found and matches whole string
        if res and len(res.group(0)) == len(res.string):
            print r.GetName(),res.group("thr")
            # don't try to log zero rate if log scale on y-axis
            if r.GetRate() > 0.0 and opts.logy:
                chains[ratetemplate].append((r,string.atof(res.group("thr"))))

    chains[ratetemplate] = sorted(chains[ratetemplate], key=lambda ch: ch[1])
    thrmin = min(thrmin,chains[ratetemplate][0][1])
    thrmax = max(thrmax,chains[ratetemplate][-1][1])
    ratemin = min(ratemin,chains[ratetemplate][-1][0].GetRate())
    ratemax = max(ratemax,chains[ratetemplate][0][0].GetRate())

#-----------------------------------------------------------------------
#  Make plot
#-----------------------------------------------------------------------
canvas = ROOT.TCanvas('rates_threshold_scan','rates_threshold_scan',10,10,800,600)
canvas.SetFillColor(10)
if opts.logx:
    canvas.SetLogx(1)
if opts.logy:
    canvas.SetLogy(1)
print thrmin,thrmax,ratemin,ratemax
axes = ROOT.TH2D("","",1,0.5*thrmin,2*thrmax,1,0.5*ratemin,2.0*ratemax)
axes.Draw()

# add legend
legend=ROOT.TLegend(0.55,0.66,0.90,0.85)
legend.SetFillColor(10)
legend.SetFillStyle(0)
legend.SetTextSize(0.035)
legend.SetBorderSize(0)

# make graphs
grs = []
for ratetmpl in string.split(opts.ratetemplate,","):
    chainlist = chains[ratetmpl]
    gr = ROOT.TGraph(len(chainlist))
    i=0
    for (ch,thr) in chainlist:
        print i,thr,ch.GetRate()
        gr.SetPoint(i,thr,ch.GetRate())    
        i=i+1

    gr.SetMarkerStyle(22+len(grs))
    gr.SetMarkerSize(2.0)
    gr.SetLineWidth(2)
    gr.SetLineColor(1+len(grs))
    gr.SetMarkerColor(1+len(grs))
    gr.Draw("LP")
    grs.append(gr)
    gr.Print()
    legend.AddEntry(gr,ratetmpl,"PL")
        
axes.GetXaxis().SetTitle("Threshold [GeV]")
axes.GetYaxis().SetTitle("Rate [Hz]")

legend.Draw()

if opts.plotlabel!="":
    latex=ROOT.TLatex(0.15,0.945,opts.plotlabel)
    latex.SetTextFont(42)
    latex.SetTextSize(0.0425)
    latex.SetNDC()
    latex.Draw()

canvas.SaveAs(opts.outname)
