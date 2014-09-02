#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from optparse import OptionParser
import os, glob
import string

__author__  = 'James Walder'
__version__ = '$Id$'
__usage__   = '%prog [options] '

def makeListFromString(s,token=','):
  names = s.split(token)
  return names
  
parser = OptionParser(usage=__usage__, version=__version__)
parser.add_option('-f','--files',dest='files', default='bs.root', help='Root files for data plots')
parser.add_option('-d','--dir',dest='dir', default='Beamspot', help='Root directory for beamspots')
parser.add_option('-b','--batch', dest='batch', action='store_true', default=False, help='Run in batch mode')

(options,args) = parser.parse_args()

if options.batch:
  os.unsetenv('DISPLAY')


from ROOT import TH1D, TCanvas  
from ROOT import gSystem, gROOT, TFile, TH1D, TH2D, TCanvas, TTree, TChain
from ROOT import TGraphErrors
from ROOT import TMultiGraph
from ROOT import EColor
from ROOT import TLegend,TLine
from ROOT import TLatex,TGaxis

legends = [] # holder 
pdglines= [] # holder
labels  = []
ghists  = [] # additional hists
failed  = [] # list of failed plots
graphs  = []
canvases = []

PointSize = 1.6
MaxHeightFactor = 1.5

plotTextSize = 0.04
gROOT.ProcessLine(".L AtlasStyle.C")
gROOT.ProcessLine(".L AtlasUtils.C")
gROOT.ProcessLine("SetAtlasStyle();")

def findBeamspots(file,dir, pattern):
  import re
  from ROOT import gDirectory
  file.cd(dir)
  p = re.compile(pattern)
  print p.pattern
  n = gDirectory.GetListOfKeys().GetSize()
  print "N: ", n
  matched = []
  for i in range(n):
    name = gDirectory.GetListOfKeys().At(i).GetName()
    if not p.match(name):
      continue
    print name
    #print p.match(name)
    h = gDirectory.Get(gDirectory.GetListOfKeys().At(i).GetName())
    #print h, h.GetName()
    matched.append(h)
    
  file.cd()
  return matched


def makeFloatFromString(s):
  subs = { 'p':'.', 'm':'-' }
  n = ""
  for i in s:
    c = i
    if i in subs.keys():
      c = subs[i]
    n = n+c
  return float(n)


def splitBeamspotTreeTitle(title, cutToken="CUT"):
  t = title.split(cutToken)
  namestring = t[0].strip('_')
  cutstring = t[-1].strip('_')
  cuts = cutstring.split('_')
  output = {}
  for i in range(0,len(cuts),2):
    output[cuts[i]] = makeFloatFromString(cuts[i+1])
  
  return output,namestring

def getTree(name, file, dir):
  tr = file.Get( dir +"/" + name)
  print type(tr)
  return tr

# beamspot tree-type stuff
def makePull( tree, xval='x0',xcov='x0x0', xTrue=0., isCovErr=True, xlow=-8, xhigh=8, xbins=100, doGausFit=False, cuts=""):
  h = TH1D("hPull_"+tree.GetName()+"_"+xval, "hPull_"+tree.GetName()+"_"+xval, xbins, xlow,xhigh)
  if isCovErr:
    p = '(%(x)s - %(t)s)/sqrt(%(e)s) >> %(h)s' % { 'x':xval, 't':xTrue, 'e':xcov, 'h':h.GetName()}
  else:
    p = '(%(x)s - %(t)s)/(%(e)s) >> %(h)s' % { 'x':xval, 't':xTrue, 'e':xcov, 'h':h.GetName()}
  print p
  tree.Draw( p , cuts);
  h.Print()
  
  mean    = h.GetMean()
  meanErr = h.GetMeanError()
  rms     = h.GetRMS()
  rmsErr  = h.GetRMSError() 
  if doGausFit:
    h.Fit("gaus","LL")
    f = h.GetFunction("gaus")
    mean = f.GetParameter(1)
    meanErr = f.GetParError(1)
    rms = f.GetParameter(2)
    rmsErr = f.GetParError(2)
    
  return (mean, meanErr, rms, rmsErr)


# Start of main programs

#Open the files

files = makeListFromString(options.files)
if len(files) == 1:
  fii = TFile(files[0])
else:
  print "Multiple files not yet supported"
  sys.exit()


# matched = findBeamspots(fii, options.dir, "Beamspots_.*MinVt.*")

def makeBeamspotCutPlots(name, points):
  nPoints = len(points)
  
  x = TGraphErrors(nPoints)
  x.SetName(name)
  x.SetTitle(name + "  x")
  y = TGraphErrors(nPoints)
  y.SetName(name)
  y.SetTitle(name + "  y")
  z = TGraphErrors(nPoints)
  z.SetName(name)
  z.SetTitle(name + "  z")
  sx = TGraphErrors(nPoints)
  sx.SetName(name)
  sx.SetTitle(name + "  sx")
  sy = TGraphErrors(nPoints)
  sy.SetName(name)
  sy.SetTitle(name + "  sy")
  sz = TGraphErrors(nPoints)
  sz.SetName(name)
  sz.SetTitle(name + "  sz")

  xrms = TGraphErrors(nPoints)
  xrms.SetName(name+"_rms")
  xrms.SetTitle(name+"_rms" + "  x")
  yrms = TGraphErrors(nPoints)
  yrms.SetName(name+"_rms")
  yrms.SetTitle(name+"_rms" + "  y")
  zrms = TGraphErrors(nPoints)
  zrms.SetName(name+"_rms")
  zrms.SetTitle(name+"_rms" + "  z")
  sxrms = TGraphErrors(nPoints)
  sxrms.SetName(name+"_rms")
  sxrms.SetTitle(name+"_rms" + "  sx")
  syrms = TGraphErrors(nPoints)
  syrms.SetName(name+"_rms")
  syrms.SetTitle(name+"_rms" + "  sy")
  szrms = TGraphErrors(nPoints)
  szrms.SetName(name+"_rms")
  szrms.SetTitle(name+"_rms" + "  sz")

  graphs.extend([x,y,z,sx,sy,sz,xrms,yrms,zrms,sxrms,syrms,szrms])
  print len(points)
  for i in range(len(points)):
    xval  = points[i][0]
    yvals = points[i][1] 

    xerr =0;

    x.SetPoint(i,  xval, yvals[0][0])
    y.SetPoint(i,  xval, yvals[1][0])
    z.SetPoint(i,  xval, yvals[2][0])
    sx.SetPoint(i,  xval, yvals[3][0])
    sy.SetPoint(i,  xval, yvals[4][0])
    sz.SetPoint(i,  xval, yvals[5][0])

    x.SetPointError(i,  xerr, yvals[0][1])
    y.SetPointError(i,  xerr, yvals[1][1])
    z.SetPointError(i,  xerr, yvals[2][1])
    sx.SetPointError(i,  xerr, yvals[3][1])
    sy.SetPointError(i,  xerr, yvals[4][1])
    sz.SetPointError(i,  xerr, yvals[5][1])


    xrms.SetPoint(i,  xval, yvals[0][2])
    yrms.SetPoint(i,  xval, yvals[1][2])
    zrms.SetPoint(i,  xval, yvals[2][2])
    sxrms.SetPoint(i,  xval, yvals[3][2])
    syrms.SetPoint(i,  xval, yvals[4][2])
    szrms.SetPoint(i,  xval, yvals[5][2])

    xrms.SetPointError(i,  xerr, yvals[0][3])
    yrms.SetPointError(i,  xerr, yvals[1][3])
    zrms.SetPointError(i,  xerr, yvals[2][3])
    sxrms.SetPointError(i,  xerr, yvals[3][3])
    syrms.SetPointError(i,  xerr, yvals[4][3])
    szrms.SetPointError(i,  xerr, yvals[5][3])

  c = TCanvas("cx")
  c.Divide(3,1)
  c.cd(1)
  x.Draw("ap")
  c.cd(2)
  y.Draw("ap+Y+")
  c.cd(3)
  yaxis_xrms = TGaxis( -1, 0.2, 1 ,0.2, -1, 2,510,"+R")
  yaxis_xrms.ImportAxisAttributes( y.GetHistogram().GetYaxis() )
  #g = TMultiGraph()
  #g.Add(x,"aXp")
  #g.Add(y,"aY*")
  #g.Draw("a")
  # graphs.append(g)
  #   aa = y.GetHistogram()
  
  #x.Draw("ap")
  yaxis_xrms.Draw()
  #axis.PaintAxis(0,0.5,0.1,0.6,0.4,1.4,510,"+R")
  #y.Draw("pY+sames")
  #b.Draw("sames")
  c.Modified()
  c.Update()
  #c.Print()
  canvases.append(c)

def makeCutPlotPulls( file, dir, pattern, name):
  trueVals = { 'x0':-0.15,  'y0':1., 'z':-9., 'sx':'0.72', 'sy':'0.42', 'sz':44, 'ax':'0.', 'ay':0., 'k':1, 'rhoxy':0.0}

  # list of trees
  matched = findBeamspots(file, dir, pattern)
  points= []

  for i in matched:
    # loop over trees and get cut values
    o, n = splitBeamspotTreeTitle( i.GetName(), "CUT" )
    # make assumption of only one cut value 
    xval = o[o.keys()[0]]
    print "JW: ", xval
    xpos = makePull( getTree( i.GetName(), file, dir), xval='x0', xcov='x0x0', xTrue=trueVals['x0'])
    ypos = makePull( getTree( i.GetName(), file, dir), xval='y0', xcov='y0y0', xTrue=trueVals['y0'])
    zpos = makePull( getTree( i.GetName(), file, dir), xval='z', xcov='zz', xTrue=trueVals['z'])

    sx = makePull( getTree( i.GetName(), file, dir), xval='sx', xcov='sxsx', xTrue=trueVals['sx'])
    sy = makePull( getTree( i.GetName(), file, dir), xval='sy', xcov='sysy', xTrue=trueVals['sy'])
    sz = makePull( getTree( i.GetName(), file, dir), xval='sz', xcov='szsz', xTrue=trueVals['sz'])
    ax = makePull( getTree( i.GetName(), file, dir), xval='ax', xcov='axax', xTrue=trueVals['ax'])
    ay = makePull( getTree( i.GetName(), file, dir), xval='ay', xcov='ayay', xTrue=trueVals['ay'])
    k = makePull( getTree( i.GetName(), file, dir), xval='k', xcov='kk', xTrue=trueVals['k'])
    rhoxy = makePull( getTree( i.GetName(), file, dir), xval='rhoxy', xcov='rhoxyrhoxy', xTrue=trueVals['rhoxy'])
    points.append( (xval, [xpos, ypos, zpos, sx, sy, sz, ax, ay, k, rhoxy ]) )


  plots = makeBeamspotCutPlots(name, points)


makeCutPlotPulls(fii, options.dir, "Beamspots_.*Min.*","MinProb")


#vals = []
#for i in matched:
#  vals.append( [i.GetName(), splitBeamspotTreeTitle(i.GetName())] )##
#
#for i in vals:
#  i.append( makePull(getTree( i[0]  ,fii,options.dir),doGausFit=True) )#
##
#
#for i in vals:
#  print i[1][0][0], i[2][0],i[2][1]


s = raw_input('--> ')
exit()

