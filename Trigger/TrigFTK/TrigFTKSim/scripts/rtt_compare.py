#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Author: Noel Dawe
# Description: This script compares two sets of histograms to validate the
# simulation with respect to an earlier reference version.

import argparse

parser = argparse.ArgumentParser()
parser.add_argument("--RefName", help="Name to print for reference sample",default="Ref")
parser.add_argument("--RefFile", help="Reference ntuple", required=True)
parser.add_argument("--ChkName", help="Name to print for sample to be checked",default="New")
parser.add_argument("--ChkFile", help="Merged ntuple to be checked", default="tempout.root")
parser.add_argument("--OutFile", help="Name of output file <name.root> (optional)", nargs='?')
parser.add_argument("--PlotTowers", help="Plot Towers and Subregions in addition to Merge", action="store_true")
args = parser.parse_args()

RefName = args.RefName
RefFile = args.RefFile
ChkName = args.ChkName
ChkFile = args.ChkFile

# If the name is specified, use it, else use the default
OutFile = args.OutFile or (RefName + "vs" + ChkName + ".root")
pdfname = OutFile[:-4]+"pdf"

import ROOT
import sys
import time

ROOT.gErrorIgnoreLevel = ROOT.kError
ROOT.gROOT.SetBatch(True)
if not ROOT.gSystem.Load("libTrigFTKSimLib.so"):
    pass
    #sys.exit("unable to load libTrigFTKSimLib.so")

######## Variables to plot & compare defnined here. Edit as required. ########

# Variables to plot for merged ntuple
# track type variables: varname, "name", "display name", binno, binlow, binhigh
TrackVarsMerge = [
    ["m_ntracks","ntracks","Number of Tracks",100,0,400],
    ["m_tracks.m_invpt","Curvature","1/(2*pT)",100,-0.001,0.001],
]

# Variables to plot for each tower
# track type variables: varname, "name", "display name", binno, binlow, binhigh
TrackVars = [
    ["m_ntracks","ntracks","Number of Tracks",20,0,20],
]

# road type variables: varname, "name", "display name", binno, binlow, binhigh
RoadVars = [
    ["m_nroads","nroads","Number of Roads",100,0,1000],
]


def plot(fileref, filechk, outfile, name, varname, dispname, bins, binlow, binhigh, tower=None):
    collection_name = 'FTKMergedTracksStream'
    histo_name = 'histo'
    if tower is not None:
        histo_name += '_tower{0:d}'.format(tower)
        collection_name += '{0:d}'.format(tower)
        histo_title = 'Tower {0};{1};'.format(tower + 1, dispname)
    else:
        histo_name += '_merged'
        histo_title = 'All Towers;{0};'.format(dispname)
    histo_name += '_{0}'.format(name)
    fileref.cd()
    histo_ref = ROOT.TH1F(histo_name + "_ref",histo_title,bins,binlow,binhigh)
    histo_ref.SetLineColor(ROOT.kBlack)
    histo_ref.SetFillColor(ROOT.kBlack)
    histo_ref.SetFillStyle(3004)
    refftkdata = fileref.Get("ftkdata")
    refftkdata.Draw("{0}.{1}>>{2}_ref".format(collection_name, varname, histo_name))
    histo_ref.SetDirectory(outfile)
    filechk.cd()
    histo_chk = ROOT.TH1F(histo_name + "_chk",";{0};".format(dispname),bins,binlow,binhigh)
    histo_chk.SetLineColor(ROOT.kRed)
    chkftkdata = filechk.Get("ftkdata")
    chkftkdata.Draw("{0}.{1}>>{2}_chk".format(collection_name, varname, histo_name))
    histo_chk.SetDirectory(outfile)
    outfile.cd()
    histo_ref.Write()
    histo_chk.Write()
    canvas = ROOT.TCanvas(histo_name,histo_name,1)
    if histo_ref.GetMaximum() < histo_chk.GetMaximum():
        histo_chk.Draw()
        histo_ref.Draw("sames")
    else:
        histo_ref.Draw()
        histo_chk.Draw("sames")
    canvas.Update()
    #stats = histo_chk.GetFunction("stats")
    #stats.SetTextColor(ROOT.kRed)
    #stats.SetLineColor(ROOT.kRed)
    #height = stats.GetY2NDC()-stats.GetY1NDC()
    #stats.SetY1NDC(.6)
    #stats.SetY2NDC(.6+height)
    canvas.Write()
    outfile.Write()


def equal(histo1, histo2):
    for i in range(histo1.GetNbinsX() + 1):
        if histo1.GetBinContent(i) != histo2.GetBinContent(i):
            return False
    return True

def blankpage(pdfname, msg=""):
    emptycanvas = ROOT.TCanvas("empty","empty",1)
    lblref = ROOT.TText(.5,.5,msg)
    lblref.SetTextColor(ROOT.kBlack)
    lblref.SetTextAlign(22)
    lblref.SetNDC()
    lblref.Draw()
    emptycanvas.Print(pdfname)


fileref = ROOT.TFile.Open(RefFile)
filechk = ROOT.TFile.Open(ChkFile)
outfile = ROOT.TFile.Open(OutFile,"recreate")

for varname, name, dispname, bins, binlow, binhigh in TrackVarsMerge:
    plot(fileref, filechk, outfile, name, varname, dispname, bins, binlow, binhigh)

if args.PlotTowers:
    # Loop over towers
    for tower in range(64):
        for varname, name, dispname, bins, binlow, binhigh in TrackVars:
            plot(fileref, filechk, outfile, name, varname, dispname, bins, binlow, binhigh, tower=tower)

covercanvas = ROOT.TCanvas("cover","cover",1)
lblchk = ROOT.TText(.5,.5,ChkName)
lblchk.SetTextColor(ROOT.kRed)
lblchk.SetTextAlign(21)
lblchk.SetNDC()
lblchk.Draw()
lblref = ROOT.TText(.5,.4,RefName)
lblref.SetTextColor(ROOT.kBlack)
lblref.SetTextAlign(21)
lblref.SetNDC()
lblref.Draw()
lbldate = ROOT.TText(.5,.3,time.strftime("%Y/%m/%d %H:%M:%S", time.localtime()))
lbldate.SetTextColor(ROOT.kBlack)
lbldate.SetTextAlign(21)
lbldate.SetNDC()
lbldate.Draw()
covercanvas.Print(pdfname + "(")

nfailed = 0
nskipped = 0
npassed = 0

for varname, name, dispname, bins, binlow, binhigh in TrackVarsMerge:
    curcanvas = outfile.Get("histo_merged_{0}".format(name))
    if curcanvas:
        href = outfile.Get("histo_merged_{0}_ref".format(name))
        hchk = outfile.Get("histo_merged_{0}_chk".format(name))
        if not equal(href,hchk):
            curcanvas.SetFillColor(ROOT.kRed)
            nfailed += 1
        else:
            npassed += 1
        curcanvas.Print(pdfname)
    else:
        blankpage(pdfname,"No merged tracks")
        nskipped += 1

if args.PlotTowers:
    # Loop over towers
    for tower in range(64):
        for varname, name, dispname, bins, binlow, binhigh in TrackVars:
            curcanvas = outfile.Get("histo_tower{0}_{1}".format(tower, name))
            if not curcanvas:
                blankpage(pdfname,"No valid data for Tower {0}".format(tower))
                nskipped += 1
                continue
            href = outfile.Get("histo_tower{0}_{1}_ref".format(tower, name))
            hchk = outfile.Get("histo_tower{0}_{1}_chk".format(tower, name))
            if not equal(href,hchk):
                curcanvas.SetFillColor(ROOT.kRed)
                nfailed += 1
            else:
                npassed += 1
            curcanvas.Print(pdfname)

outfile.Close()
fileref.Close()
filechk.Close()

finalcanvas = ROOT.TCanvas("final","final",1)
tally = "Number of tests failed: {0:d}, skipped: {1:d}, passed: {2:d}".format(nfailed,nskipped,npassed)
lblres = ROOT.TText(.5,.5,tally)
lblres.SetTextAlign(22)
lblres.SetNDC()
lblres.Draw()
finalcanvas.Print(pdfname+")")

print tally
if nfailed != 0 or nskipped != 0:
    sys.exit(10)
