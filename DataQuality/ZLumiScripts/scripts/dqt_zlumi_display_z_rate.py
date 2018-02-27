#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration                  

import ROOT
import sys, os
import array
import argparse
import time

parser = argparse.ArgumentParser()
parser.add_argument('infile', type=str, help='input HIST file')
parser.add_argument('--indir', type=str, help='Directory with input file',
                    default='')
parser.add_argument('--plotdir', type=str, help='Directory to dump plots',
                    default='plots')
args = parser.parse_args()

# runnum = sys.argv[1].split('_')[0]
runnum = args.infile.split('_')[0]
# f = ROOT.TFile.Open(sys.argv[1], 'UPDATE')
f = ROOT.TFile.Open(os.path.join(args.indir, args.infile), 'UPDATE')
c1 = ROOT.TCanvas()
lumitree = f.lumitree
#get range
runs=[0,1]; fills=[0,1]
lumitree.Draw("run:lhcfill", "", "goff")
if lumitree.GetSelectedRows() > 0:
    runs = list(lumitree.GetV1()[_] for _ in xrange(lumitree.GetSelectedRows()))
    fills = list(lumitree.GetV2()[_] for _ in xrange(lumitree.GetSelectedRows()))
titlestr = ''
if min(fills)==max(fills):
    titlestr += 'Fill %d' % min(fills)
if min(runs) == max(runs):
    titlestr += ' Run %d' % min(runs)

lumitree.Draw("zrate:lb+10:zratestat", "", "goff")
print 'Selected rows', lumitree.GetSelectedRows()
if lumitree.GetSelectedRows() > 0: 
    gr = ROOT.TGraphErrors(lumitree.GetSelectedRows(), lumitree.GetV2(), lumitree.GetV1(), ROOT.nullptr, lumitree.GetV3())
    gr.Draw("ap")
    gr.GetHistogram().SetXTitle('LB')
    gr.GetHistogram().SetYTitle('Fiducial Z yield/second')
    gr.SetMarkerStyle(20)
    gr.SetTitle('')
    f.WriteTObject(gr, 'fid_z_rate')
    c1.Update()
    c1.Print(os.path.join(args.plotdir, '%s_fidyield.eps' % runnum))

# dump CSV
csvout = open(os.path.join(args.plotdir, '%s_zrate.csv' % runnum), 'w')
lumitree.Draw("zrate:lbwhen[0]:lbwhen[1]:lhcfill:lblive:offlumi", "", "goff,para")
timeformat = '%y/%m/%d %H:%M:%S'
#timeformat = '%m/%d %H:%M:%S'
for i in xrange(lumitree.GetSelectedRows()):
    zrate = lumitree.GetV1()[i]
    instlumi = lumitree.GetVal(5)[i]
    livetime = lumitree.GetVal(4)[i]
    print >>csvout, '%d, %s, %s, %6f, %6f, %4f, %6f' % (lumitree.GetV4()[i],
                                                    time.strftime(timeformat, time.gmtime(lumitree.GetV2()[i])), 
                                                    time.strftime(timeformat, time.gmtime(lumitree.GetV3()[i])), 
                                                    lumitree.GetV1()[i],
                                                    instlumi/1e3,
                                                    instlumi*livetime/1e3,
                                                    lumitree.GetV1()[i]*livetime
                                                    )
csvout.close()

lumitree.Draw("zlumi:lb+10:zlumistat", "", "goff")
if lumitree.GetSelectedRows() > 0:
    gr = ROOT.TGraphErrors(lumitree.GetSelectedRows(), lumitree.GetV2(), lumitree.GetV1(), ROOT.nullptr, lumitree.GetV3())
    zlumi = list(lumitree.GetV1()[_] for _ in xrange(lumitree.GetSelectedRows())); 
    zlumierr = list(lumitree.GetV3()[_] for _ in xrange(lumitree.GetSelectedRows()))
    gr.Draw("ap")
    gr.GetHistogram().SetXTitle('LB')
    gr.GetHistogram().SetYTitle('Luminosity (x10^{33})')
    gr.SetMarkerStyle(20)
    gr.SetTitle(titlestr)
    f.WriteTObject(gr, 'z_lumi')
    lumitree.Draw("offlumi:lb+10", "", "goff")
    gr2 = ROOT.TGraphErrors(lumitree.GetSelectedRows(), lumitree.GetV2(), lumitree.GetV1())
    offlumi = list(lumitree.GetV1()[_] for _ in xrange(lumitree.GetSelectedRows()))
    gr2.SetMarkerStyle(21)
    gr2.SetMarkerSize(0.5)
    gr2.SetMarkerColor(ROOT.kRed)
    gr2.SetLineColor(ROOT.kRed)
    gr2.Draw('same,l')
    f.WriteTObject(gr2, 'official_lumi')
    leg = ROOT.TLegend(0.65, 0.7, 0.89, 0.89)
    leg.SetBorderSize(0)
    leg.AddEntry(gr, 'Z luminosity', 'pl')
    leg.AddEntry(gr2, 'Official', 'l')
    leg.Draw()
    c1.Update()
    c1.Print(os.path.join(args.plotdir, '%s_lumicomp.eps' % runnum))
    f.WriteTObject(c1, 'lumicomp_canvas')
    zlumirat = array.array('d', [_[0]/_[1] for _ in zip(zlumi, offlumi)])
    zlumiraterr = array.array('d', [_[0]/_[1] for _ in zip(zlumierr, offlumi)])
    gr3 = ROOT.TGraphErrors(lumitree.GetSelectedRows(), lumitree.GetV2(), zlumirat, ROOT.nullptr, zlumiraterr)
    c1.Clear()
    gr3.SetMarkerStyle(20)
    gr3.Draw('ap')
    gr3.SetTitle(titlestr)
    gr3.GetHistogram().SetXTitle('LB')
    gr3.GetHistogram().SetYTitle('Z Counting/Official Lumi')
    c1.Print(os.path.join(args.plotdir, '%s_lumicompratio.eps' % runnum))
    f.WriteTObject(c1, 'lumicompratio_canvas')
    
