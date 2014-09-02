#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from optparse import OptionParser
import os, glob
import string
from math import sqrt

__author__  = 'James Walder'
__version__ = '$Id$'
__usage__   = '%prog [options] '

def makeListFromString(s,token=','):
  names = s.split(token)
  return names
  

def AddText(xmin,ymin,text,size=0.015, font=None):
    m = TLatex()  # //l.SetTextAlign(12); l.SetTextSize(tsize); 
    m.SetNDC();
    #m.SetTextFont(72);
    m.SetTextColor(EColor.kBlack);
    m.SetTextSize(size)
    if font != None:
      m.SetTextFont(font)
    m.DrawLatex(xmin,ymin,text);
    labels.append(m)


class Event:
  def __init__(self):
    self.run =0.
    self.lbStart =0.
    self.lumiRange=0.
    self.nEvents = 0.
    self.statusWord =0.
    self.bcid =0.


class Beamspot:
  def __init__(self):
    self.x0 = 0.
    self.y0 = 0.
    self.z = 0.
    self.sx = 0.
    self.sy = 0.
    self.sz = 0.
    self.ax = 0.
    self.ay = 0.
    self.k = 0.
    self.rhoxy = 0.
    self.xc = 0.
    self.yc = 0.
    
    self.Ex0 = 0.
    self.Ey0 = 0.
    self.Ez = 0.
    self.Esx = 0.
    self.Esy = 0.
    self.Esz = 0.
    self.Eax = 0.
    self.Eay = 0.
    self.Ek = 0.
    self.Erhoxy = 0.
    self.Exc = 0.
    self.Eyc = 0.
  def clear(self):
    self.x0 = 0.
    self.y0 = 0.
    self.z = 0.
    self.sx = 0.
    self.sy = 0.
    self.sz = 0.
    self.ax = 0.
    self.ay = 0.
    self.k = 0.
    self.rhoxy = 0.
    self.xc = 0.
    self.yc = 0.
    
    self.Ex0 = 0.
    self.Ey0 = 0.
    self.Ez = 0.
    self.Esx = 0.
    self.Esy = 0.
    self.Esz = 0.
    self.Eax = 0.
    self.Eay = 0.
    self.Ek = 0.
    self.Erhoxy = 0.
    self.Exc = 0.
    self.Eyc = 0.
  def __str__(self):
    out = "x0: " + str(self.x0) +  " +- " + str(self.Ex0)
    return out
          
parser = OptionParser(usage=__usage__, version=__version__)
parser.add_option('-f','--files',dest='files', default='bs.root', help='Root files for data plots')
parser.add_option('-d','--dir',dest='dir', default='Beamspot', help='Root directory for beamspots')
parser.add_option('-b','--batch', dest='batch', action='store_true', default=False, help='Run in batch mode')
parser.add_option('-t','--tree',dest='tree', default='Beamspots', help='Beamspot root-tree name')

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
from ROOT import EColor

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

def makeBeamspotPlots( name, beamspots):
  if len(beamspots) == 0: 
    print "No beamspot"

  nPoints = len(beamspots)
  print "Found", nPoints

  x = TGraphErrors(nPoints)
  x.SetName(name + "_x")
  x.SetTitle(name + "  x; lb; x [mm]")
  y = TGraphErrors(nPoints)
  y.SetName(name + "_y")
  y.SetTitle(name + "  y; lb; y [mm]")
  z = TGraphErrors(nPoints)
  z.SetName(name + "_z")
  z.SetTitle(name + "  z; lb; z [mm]")
  sx = TGraphErrors(nPoints)
  sx.SetName(name + "_sx")
  sx.SetTitle(name + "  sx; lb; #sigma(x) [mm]")
  sy = TGraphErrors(nPoints)
  sy.SetName(name + "_sy")
  sy.SetTitle(name + "  sy; lb; #sigma(y) [mm]")
  sz = TGraphErrors(nPoints)
  sz.SetName(name + "_sz")
  sz.SetTitle(name + "  sz; lb; #sigma(z) [mm]")
  ax = TGraphErrors(nPoints)
  ax.SetName(name + "_ax")
  ax.SetTitle(name + "  ax; lb; slope x-z [rad]")
  ay = TGraphErrors(nPoints)
  ay.SetName(name + "_ay")
  ay.SetTitle(name + "  ay; lb; slope y-z [rad]")
  k = TGraphErrors(nPoints)
  k.SetName(name + "_k")
  k.SetTitle(name + "  k; lb; k")
  rhoxy = TGraphErrors(nPoints)
  rhoxy.SetName(name + "_rhoxy")
  rhoxy.SetTitle(name + "  rhoxy; lb; #rho_{xy}")

  nEvents= TGraphErrors(nPoints)
  nEvents.SetName(name + "_nEvents")
  nEvents.SetTitle(name + "  nEvents; lb; nEvents")


  
  graphs.extend([x,y,z,sx,sy,sz,ax,ay,k,rhoxy,nEvents])

  point =0
  for evt,bs in beamspots:
    xmid = evt.lbStart + 0.5*evt.lumiRange
    xerr =  0.5*evt.lumiRange

    x.SetPoint(point,  xmid, bs.xc)
    y.SetPoint(point,  xmid, bs.yc)
    z.SetPoint(point,  xmid, bs.z)
    sx.SetPoint(point,  xmid,bs.sx )
    sy.SetPoint(point,  xmid,bs.sy )
    sz.SetPoint(point,  xmid,bs.sz )
    ax.SetPoint(point,  xmid, bs.ax)
    ay.SetPoint(point,  xmid, bs.ay)
    k.SetPoint(point,  xmid, bs.k)
    rhoxy.SetPoint(point,  xmid, bs.rhoxy)

    nEvents.SetPoint(point, xmid, evt.nEvents)

    x.SetPointError(point,  xerr,bs.Exc )
    y.SetPointError(point,  xerr,bs.Eyc )
    z.SetPointError(point,  xerr,bs.Ez  )
    sx.SetPointError(point,  xerr,bs.Esx )
    sy.SetPointError(point,  xerr,bs.Esy )
    sz.SetPointError(point,  xerr,bs.Esz )
    ax.SetPointError(point,  xerr,bs.Eax )
    ay.SetPointError(point,  xerr,bs.Eay )

    k.SetPointError(point,  xerr, bs.Ek)
    rhoxy.SetPointError(point,  xerr, bs.Erhoxy)
    nEvents.SetPointError(point, xerr, sqrt(evt.nEvents))

    point = point + 1
  return (x,y,z,sx,sy,sz,ax,ay,k,rhoxy,nEvents)

def makeBeamspotCutPlots(name, points):
  nPoints = len(points)
  
  x = TGraphErrors(nPoints)
  x.SetName(name)
  x.SetTitle(name + "  x; lb; x [mm]")
  y = TGraphErrors(nPoints)
  y.SetName(name)
  y.SetTitle(name + "  y; lb; y [mm]")
  z = TGraphErrors(nPoints)
  z.SetName(name)
  z.SetTitle(name + "  z; lb; z [mm]")
  sx = TGraphErrors(nPoints)
  sx.SetName(name)
  sx.SetTitle(name + "  sx; lb; #sigma(x) [mm]")
  sy = TGraphErrors(nPoints)
  sy.SetName(name)
  sy.SetTitle(name + "  sy; lb; #sigma(y) [mm]")
  sz = TGraphErrors(nPoints)
  sz.SetName(name)
  sz.SetTitle(name + "  sz; lb; #sigma(z) [mm]")

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


#######################
#open the files
infiles = makeListFromString(options.files)
chain = TChain(options.dir+"/"+options.tree)

for i in infiles:
  chain.Add(i)
print "Added: ", chain.GetNtrees() , " files."

nEntries = chain.GetEntries()

lrun      = chain.GetLeaf("event/run")
lbcid     = chain.GetLeaf("event/bcid")
lnEvents  = chain.GetLeaf("event/nEvents")
llbStart  = chain.GetLeaf("event/lumiStart")
llumiRange= chain.GetLeaf("event/lumiRange")
lstatusWord=chain.GetLeaf("event/statusWord")

lx0  = chain.GetLeaf("bs/x0")
ly0  = chain.GetLeaf("bs/y0")
lz   = chain.GetLeaf("bs/z")
lsx  = chain.GetLeaf("bs/sx")
lsy  = chain.GetLeaf("bs/sy")
lsz  = chain.GetLeaf("bs/sz")
lax  = chain.GetLeaf("bs/ax")
lay  = chain.GetLeaf("bs/ay")
lrhoxy= chain.GetLeaf("bs/rhoxy")
lk  = chain.GetLeaf("bs/k")
lxc  = chain.GetLeaf("bsCentroid/xc")
lyc  = chain.GetLeaf("bsCentroid/yc")

lCovx0  = chain.GetLeaf("bsCov/x0x0")
lCovy0  = chain.GetLeaf("bsCov/y0y0")
lCovz   = chain.GetLeaf("bsCov/zz")
lCovsx  = chain.GetLeaf("bsCov/sxsx")
lCovsy  = chain.GetLeaf("bsCov/sysy")
lCovsz  = chain.GetLeaf("bsCov/szsz")
lCovax  = chain.GetLeaf("bsCov/axax")
lCovay  = chain.GetLeaf("bsCov/ayay")
lCovrhoxy= chain.GetLeaf("bsCov/rhoxyrhoxy")
lCovk  = chain.GetLeaf("bsCov/kk")
lCovxc  = chain.GetLeaf("bsCovCentroid/xcxc")
lCovyc  = chain.GetLeaf("bsCovCentroid/ycyc")

## format of data
bcidEvents = {}

count = 0
for entry in range(nEntries):
  # Begin loop
  chain.GetEntry(entry)
  run        = lrun.GetValue()
  bcid       = lbcid.GetValue()
  nEvents    = lnEvents.GetValue()
  lbStart    = llbStart.GetValue()
  lumiRange  = llumiRange.GetValue()
  statusWord = lstatusWord.GetValue()

  if nEvents < 200:
    continue
  count = count +1
  if statusWord != 59:
    continue
  if count > 1e10:
    break
  
  evt = Event()
  evt.run = run
  evt.lbStart = lbStart
  evt.lumiRange = lumiRange
  evt.statusWord = statusWord
  evt.bcid = bcid
  evt.nEvents = nEvents

  bs = Beamspot()
  bs.x0 = lx0.GetValue()
  bs.y0 = ly0.GetValue()
  bs.xc = lxc.GetValue()
  bs.yc = lyc.GetValue()
  bs.z  = lz.GetValue()
  bs.sx = lsx.GetValue()
  bs.sy = lsy.GetValue()
  bs.sz = lsz.GetValue()
  bs.ax = lax.GetValue()
  bs.ay = lay.GetValue()
  bs.k = lk.GetValue()
  bs.rhoxy = lrhoxy.GetValue()

  bs.Ek  = sqrt(lCovk.GetValue())
  bs.Ex0 = sqrt(lCovx0.GetValue())
  bs.Ey0 = sqrt(lCovy0.GetValue())
  bs.Exc = sqrt(lCovx0.GetValue())
  bs.Eyc = sqrt(lCovy0.GetValue())
  bs.Ez  = sqrt(lCovz.GetValue())
  bs.Esx = sqrt(lCovsx.GetValue())
  bs.Esy = sqrt(lCovsy.GetValue())
  bs.Esz = sqrt(lCovsz.GetValue())
  bs.Eax = sqrt(lCovax.GetValue())
  bs.Eay = sqrt(lCovay.GetValue())
  bs.Erhoxy = sqrt(lCovrhoxy.GetValue())
  bs.Ek  = sqrt(lCovk.GetValue())

  if bcid not in bcidEvents.keys():
    bcidEvents[bcid] = []
  bcidEvents[bcid].append( (evt, bs) )
  
  
print bcidEvents
for k,v in bcidEvents.items():
  print k,v

plots = {}
for k,v in bcidEvents.items():
  plots[k] = makeBeamspotPlots(str(k), v)

#for k,v in plots.items():
#  c = TCanvas(str(int(k)))
#  c.Divide(3,3)#
#
#  c.cd(3)
#  v[10].Draw("ap")
#
#
#  c.cd(4)
#  v[0].Draw("ap")
#  c.cd(5)
#  v[1].Draw("ap")
#  c.cd(6)
#  v[2].Draw("ap")
#  c.cd(7)
#  v[3].Draw("ap")
#  c.cd(8)
#  v[4].Draw("ap")
#  c.cd(9)
#  v[5].Draw("ap")
#  c.cd(10)
##  v[6].Draw("ap")
##  c.cd(11)
#  v[7].Draw("ap")
#  canvases.append(c)


cx = TCanvas("cx","",800,600)
cy = TCanvas("cy","",800,600)
cz = TCanvas("cz","",800,600)
csx = TCanvas("csx","",800,600)
csy = TCanvas("csy","",800,600)
csz = TCanvas("csz","",800,600)
cax = TCanvas("cax","",800,600)
cay = TCanvas("cay","",800,600)
ck = TCanvas("ck","",800,600)
crhoxy = TCanvas("crhoxy","",800,600)
cnEvents = TCanvas("cnEvents","",800,600)

legx  = TLegend(0.75,0.65,0.92,0.94)
legy  = TLegend(0.75,0.65,0.92,0.94)
legz  = TLegend(0.75,0.65,0.92,0.94)
legsx = TLegend(0.75,0.65,0.92,0.94)
legsy = TLegend(0.75,0.65,0.92,0.94)
legsz = TLegend(0.75,0.65,0.92,0.94)
legax = TLegend(0.75,0.65,0.92,0.94)
legay = TLegend(0.75,0.65,0.92,0.94)
legk  = TLegend(0.75,0.65,0.92,0.94)
legrhoxy   = TLegend(0.75,0.65,0.92,0.94)
legnEvents = TLegend(0.75,0.65,0.92,0.94)
legs = [ legx,legy ,legz ,legsx,legsy,legsz,legax,legay,legk ,legrhoxy,legnEvents]
for leg in legs:
  leg.SetFillColor(0);
  leg.SetFillStyle(0);
  leg.SetBorderSize(0);        


c=0
m=0
first = True
colours = [ EColor.kRed+1, EColor.kBlue+1, EColor.kGreen-8, EColor.kYellow+4]
markers = [20,25,22,27,23,28,30,21]
for k,v in plots.items():
  if first:
    dr = "apl"
    first = False
    x = [v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],v[10]]
    for i in v:
      a = i.GetYaxis()
      min = a.GetXmin()
      max = a.GetXmax()
      a.SetRangeUser( min - 0.1*(max-min), max + 0.5*(max-min))
        
  else:
    dr = "pl"

  for i in v:
    i.SetMarkerColor(colours[c]) 
    i.SetLineColor(colours[c])
    i.SetMarkerStyle(markers[m])

  legx.AddEntry(v[0],str(int(k)),"p")
  legy.AddEntry(v[1],str(int(k)),"p")
  legz.AddEntry(v[2],str(int(k)),"p")
  legsx.AddEntry(v[3],str(int(k)),"p")
  legsy.AddEntry(v[4],str(int(k)),"p")
  legsz.AddEntry(v[5],str(int(k)),"p")
  legax.AddEntry(v[6],str(int(k)),"p")
  legay.AddEntry(v[7],str(int(k)),"p")
  legk.AddEntry(v[8],str(int(k)),"p")
  legrhoxy.AddEntry(v[9],str(int(k)),"p")
  legnEvents.AddEntry(v[10],str(int(k)),"p")



  cx.cd()
  v[0].Draw(dr)
  cy.cd()
  v[1].Draw(dr)
  cz.cd()
  v[2].Draw(dr)
  csx.cd()
  v[3].Draw(dr)
  csy.cd()
  v[4].Draw(dr)
  csz.cd()
  v[5].Draw(dr)
  cax.cd()
  v[6].Draw(dr)
  cay.cd()
  v[7].Draw(dr)
  ck.cd()
  v[8].Draw(dr)
  crhoxy.cd()
  v[9].Draw(dr)
  cnEvents.cd()
  v[10].Draw(dr)

  c = c+1
  if c == len(colours):
    m = m+1
    c=0

  if m == len(markers):
    m=0
    

print "Found BICDs: ", bcidEvents.keys()

fontSize=0.1
cx.cd()
AddText(0.2,0.85,"x-position",fontSize)
legx.Draw()
cy.cd()
AddText(0.2,0.85,"y-position",fontSize)
legy.Draw()
cz.cd()
AddText(0.2,0.85,"z-position",fontSize)
legz.Draw()
csx.cd()
AddText(0.2,0.85,"Width #sigma(x)",fontSize)
legsx.Draw()
csy.cd()
AddText(0.2,0.85,"Width #sigma(y)",fontSize)
legsy.Draw()
csz.cd()
AddText(0.2,0.85,"Width #sigma(z)",fontSize)
legsz.Draw()
cax.cd()
AddText(0.2,0.85,"Slope: x-z",fontSize)
legax.Draw()
cay.cd()
AddText(0.2,0.85,"Slope: y-z",fontSize)
legay.Draw()
ck.cd()
AddText(0.2,0.85,"k-factor",fontSize)
legk.Draw()
crhoxy.cd()
legrhoxy.Draw()
AddText(0.2,0.85,"#rho_{xy}",fontSize)
cnEvents.cd()
AddText(0.2,0.85,"N passed vertices",fontSize)
legnEvents.Draw()

cx.Print("cx.pdf")
cy.Print("cy.pdf")
cz.Print("cz.pdf")
csx.Print("csx.pdf")
csy.Print("csy.pdf")
csz.Print("csz.pdf")
cax.Print("cax.pdf")
cay.Print("cay.pdf")
ck.Print("ck.pdf")
crhoxy.Print("crhoxy.pdf")
cnEvents.Print("cnEvents.pdf")


s = raw_input('--> ')
exit()

