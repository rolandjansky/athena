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
  
parser = OptionParser(usage=__usage__, version=__version__)
parser.add_option('-f','--files',dest='files', default='vtx.root', help='Root files for data vertex plots')
parser.add_option('-g','--bs',dest='beamspotFile', default='', help='Root files for data beamspot plots')

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
from ROOT import TLatex,TGaxis, gFile

legends = [] # holder 
pdglines= [] # holder
labels  = []
ghists  = [] # additional hists
failed  = [] # list of failed plots
graphs  = []
canvases = []

PointSize = 1.6
MaxHeightFactor = 1.5

colours = [ EColor.kRed+1, EColor.kBlue+1, EColor.kGreen-8, EColor.kYellow+2]
markers = [20,25,22,27,23,28,30,21]
plotTextSize = 0.03

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

def makePoint( tree,prefix ,xval='x0',xlow=-8, xhigh=8, xbins=100, doGausFit=True, cuts=""):
  h = TH1D(prefix+"_"+xval, "h_"+tree.GetName()+"_"+xval, xbins, xlow,xhigh)
  p = '(%(x)s) >> %(h)s' % { 'x':xval, 'h':h.GetName()}
  h.SetXTitle(xval)
  ylabel = "Entries / XXX"
  n = h.GetBinWidth(1)
  l = '%(n).2f'%{'n':n}
  s = ylabel.replace("XXX",l)
  h.SetYTitle(s)

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
  ghists.append(h)
  print h.GetName(), mean, meanErr, rms, rmsErr
  return (mean, meanErr, rms, rmsErr,h)


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
  tr = file.Get(dir +"/" + name)
  print type(tr)
  return tr


# Start of main programs

#Open the files

files = makeListFromString(options.files)
if len(files) == 1:
  fii = TFile(files[0])
else:
  print "Multiple files not yet supported"
  sys.exit()




def getObject(chain,name):
  try:
    h = chain.Get(name)
    #h=chain.Get(name)
    h.Print()
  except:
    print "No object in ", chain.GetName(), dir,name
    h = None
  return h


def AddStatText(h, minx=0.7,miny=0.7,title="",units="",pars=['Mean','RMS']):
  n  = h.GetEntries()
  En = sqrt(n)
  
  m  = h.GetMean()
  Em = h.GetMeanError()
  
  r  = h.GetRMS()
  Er = h.GetRMSError()

  step =0.033
  ypos = miny;
  if len(title) > 0:
    AddText(minx, ypos, title,plotTextSize)
    ypos = ypos - step
  if "Entries" in pars:
    nsig = 'N = %(v).0f #pm %(ev).0f' %  { 'v': n, 'ev': En}
    AddText(minx, ypos, nsig,plotTextSize)
    ypos = ypos - step
  if "Mean" in pars:
    mass = '#mu= %(v).04f #pm %(ev).04f %(u)s'% { 'v': m, 'ev': Em, 'u':units}
    AddText(minx, ypos, mass,plotTextSize)
    ypos = ypos - step
  if "RMS" in pars:
    sigma= 'RMS   = %(v)2.4f #pm %(ev)2.4f %(u)s'% { 'v': r, 'ev': Er, 'u':units}
    AddText(minx, ypos, sigma,plotTextSize)
    ypos = ypos - step


def makeHists( tree, name):
  points= []

  cuts='nEvents>5000&&fitID==1&&fitStatus==1'

  xpos = makePoint( tree,name, xval='xc',xbins=100,xlow=-0.16,xhigh=-0.12,doGausFit=False,cuts=cuts)[-1]
  ypos = makePoint( tree,name, xval='yc',xbins=100,xlow=0.99,xhigh=1.02,doGausFit=False,cuts=cuts)[-1]
  zpos = makePoint( tree,name, xval='z', xbins=50,xlow=-2,xhigh=3,doGausFit=False,cuts=cuts)[-1]

  sx = makePoint( tree,name,xval='sx',xbins=100,xlow=0.06,xhigh=0.1,doGausFit=False,cuts=cuts)[-1]
  sy = makePoint( tree,name, xval='sy',xbins=100,xlow=0.06,xhigh=0.1,doGausFit=False,cuts=cuts)[-1]
  sz = makePoint( tree,name, xval='sz',xbins=100,xlow=55,xhigh=65,doGausFit=False,cuts=cuts)[-1]
  ax = makePoint( tree,name, xval='ax',xbins=50,xlow=400e-6,xhigh=700e-6,doGausFit=False,cuts=cuts)[-1]
  ay = makePoint( tree,name, xval='ay',xbins=50,xlow=-20e-6,xhigh=100e-6,doGausFit=False,cuts=cuts)[-1]
  k  = makePoint( tree,name, xval='k',  xbins=100,xlow=0.9,xhigh=1.3,doGausFit=False,cuts=cuts)[-1]
  rhoxy = makePoint( tree,name, xval='rhoxy',xbins=100,xlow=-0.3,xhigh=0.3,doGausFit=False,cuts=cuts)[-1]

  return (xpos, ypos, zpos, sx, sy, sz, ax, ay, k, rhoxy)



def makeCanvas(name, hists,  xlabel, ylabel, doStats=True,doLegend=True, units="", logy=False,setMinZero=True,drawOptions="",histLabels=[]):
  canv = TCanvas(name, "",800,600)
  
  min=10e6
  max=-10e6
  for i in hists:
    m = i.GetMaximum()
    max = ( m if m>max else max)
    m = i.GetMinimum()
    min = (m if m < min else min)
    
  if setMinZero and not logy:
    min=0
  if min<=0 and logy:
    min=0.01
  #if min > 1 and logy:
  #  min=0.1
  
  c=0
  m=0
  isFirst=True
  for i in hists:
    i.SetMarkerColor(colours[c]) 
    i.SetLineColor(colours[c])
    i.SetMarkerStyle(markers[m])
    c = c+1
    if c == len(colours):
      m = m+1
      c=0
    if m == len(markers):
      m=0

    if isFirst:
      i.SetMinimum( min)
      i.SetMaximum( max*1.5)
      i.SetXTitle(xlabel)
      if "XXX" not in ylabel :
        print "XX"
        s = ylabel
      else:
        print "YY"
        n = i.GetBinWidth(1)
        l = '%(n).4f'%{'n':n}
        s = ylabel.replace("XXX",l)
      i.SetYTitle(s)
      i.Draw(drawOptions)
      isFirst = False
    else:
      i.Draw(drawOptions+"sames")
  if logy:
    canv.SetLogy(1)
  if doStats:
    pos = 0.9
    for i in range(len(hists)):
      if len(histLabels) == 0:
        AddStatText(hists[i], minx=0.7, miny=pos, title = hists[i].GetName())
      else:
        AddStatText(hists[i], minx=0.7, miny=pos, title = histLabels[i])
      pos = pos -0.15
  if doLegend:
    leg = TLegend(0.2,0.7,0.4,0.9)
    leg.SetFillColor(0);
    leg.SetFillStyle(0);
    leg.SetBorderSize(0);        
    for i in range(len(hists)):
      if len(histLabels) == 0:
        leg.AddEntry(hists[i], hists[i].GetName())
      else:
        leg.AddEntry(hists[i], histLabels[i])

    legends.append(leg)
    leg.Draw()
    
  return canv
    

def getObjects(file, prefix,names):
  o=[]
  for i in names:
    o.append(getObject(file,prefix+i))
  return o

def makeStandardPlots(file, prefix,names=[],nameLabels=[], doMC=False, logy=True):

  cs = []
  cs.append(makeCanvas(prefix+"_nTracks",getObjects(file,"hnTracks_",names)
    , xlabel="nTracks", ylabel = "Entries/ XXX", logy=logy,histLabels=nameLabels))

  cs.append(makeCanvas(prefix+"_chi2",getObjects(file,"hchi2_",names)
    , xlabel="chi2", ylabel = "Entries/ XXX", logy=logy,histLabels=nameLabels))
  cs.append(makeCanvas(prefix+"_prob",getObjects(file,"hprob_",names)
    , xlabel="prob", ylabel = "Entries/ XXX", logy=logy,histLabels=nameLabels))
  cs.append(makeCanvas(prefix+"_sumpt",getObjects(file,"hsumpt_",names)
    , xlabel="sumpt", ylabel = "Entries/ XXX", logy=logy,histLabels=nameLabels))

  cs.append(makeCanvas(prefix+"_x",getObjects(file,"hvx_",names)
    ,xlabel="x [mm]", ylabel = "Entries/ XXX", logy=logy,histLabels=nameLabels))
  cs.append(makeCanvas(prefix+"_y",getObjects(file,"hvy_",names)
    ,xlabel="y [mm]", ylabel = "Entries/ XXX", logy=logy,histLabels=nameLabels))
  cs.append(makeCanvas(prefix+"_z",getObjects(file,"hvz_",names)
    ,xlabel="z [mm]", ylabel = "Entries/ XXX", logy=logy,histLabels=nameLabels))

  cs.append(makeCanvas(prefix+"_Ex",getObjects(file,"hvEx_",names)
    ,xlabel="#sigma(x) [mm]", ylabel = "Entries/ XXX", logy=logy,histLabels=nameLabels))
  cs.append(makeCanvas(prefix+"_Ey",getObjects(file,"hvEy_",names)
    ,xlabel="#sigma(y) [mm]", ylabel = "Entries/ XXX", logy=logy,histLabels=nameLabels))
  cs.append(makeCanvas(prefix+"_Ez",getObjects(file,"hvEz_",names)
    ,xlabel="#sigma(z) [mm]", ylabel = "Entries/ XXX", logy=logy,histLabels=nameLabels))

  if doMC:
    cs.append(makeCanvas(prefix+"_Pullx",getObjects(file,"hvxPullMC_",names)
      ,xlabel="Pull x", ylabel = "Entries/ XXX", logy=logy,histLabels=nameLabels))
    cs.append(makeCanvas(prefix+"_Pully",getObjects(file,"hvyPullMC_",names)
      ,xlabel="Pull y", ylabel = "Entries/ XXX", logy=logy,histLabels=nameLabels))
    cs.append(makeCanvas(prefix+"_Pullz",getObjects(file,"hvzPullMC_",names)
      ,xlabel="Pull z", ylabel = "Entries/ XXX", logy=logy,histLabels=nameLabels))


  return cs

def makeDeltaPlots(file, prefix,names=[],nameLabels=[]):

  cs = []
  cs.append(makeCanvas(prefix+"_DnTracks",getObjects(file,"hDnTracks_",names)
    , xlabel="#DeltanTracks", ylabel = "Entries/ XXX", logy=True,histLabels=nameLabels))
  cs.append(makeCanvas(prefix+"_Dsumpt",getObjects(file,"hDsumpt_",names)
    , xlabel="#Deltasumpt", ylabel = "Entries/ XXX", logy=True,histLabels=nameLabels))
    
  cs.append(makeCanvas(prefix+"_Dx",getObjects(file,"hvDx_",names)
    ,xlabel="#Deltax [mm]", ylabel = "Entries/ XXX", logy=True,histLabels=nameLabels))
  cs.append(makeCanvas(prefix+"_Dy",getObjects(file,"hvDy_",names)
    ,xlabel="#Deltay [mm]", ylabel = "Entries/ XXX", logy=True,histLabels=nameLabels))
  cs.append(makeCanvas(prefix+"_Dz",getObjects(file,"hvDz_",names)
    ,xlabel="#Deltaz [mm]", ylabel = "Entries/ XXX", logy=True,histLabels=nameLabels))

  cs.append(makeCanvas(prefix+"_DEx",getObjects(file,"hvDEx_",names)
    ,xlabel="#Delta#sigma(x) [mm]", ylabel = "Entries/ XXX", logy=True,histLabels=nameLabels))
  cs.append(makeCanvas(prefix+"_DEy",getObjects(file,"hvDEy_",names)
    ,xlabel="#Delta#sigma(y) [mm]", ylabel = "Entries/ XXX", logy=True,histLabels=nameLabels))
  cs.append(makeCanvas(prefix+"_DEz",getObjects(file,"hvDEz_",names)
    ,xlabel="#Delta#sigma(z) [mm]", ylabel = "Entries/ XXX", logy=True,histLabels=nameLabels))



  return cs


# For the analys
#Open the files

files = makeListFromString(options.files)
if len(files) == 1:
  fii = TFile(files[0])
else:
  print "Multiple files not yet supported"
  sys.exit()


#chainVtx  = TChain("")
#for i in files:
#  chainVtx.Add(i)
#print "Found: ", chainVtx.GetNtrees()

fii = TFile(files[0])


cs = []

#cs.extend( makeStandardPlots(fii,prefix="VtxAllCombRes", names=['AllDefault','AllRes'],nameLabels=['Combined','Resolved']) )
#cs.extend( makeStandardPlots(fii,prefix="VtxMatCombRes", names=['MatchedDefault','resMatched'],nameLabels=['Combined','Resolved']) )

#cs.extend( makeStandardPlots(fii,prefix="VtxAllcombpixsct",  names=['AllDefault','AllPix','AllSCT'],nameLabels=['Combined','Pix','SCT']) )

cs.extend( makeStandardPlots(fii,prefix="VtxAllCombRespixsct", names=['AllDefault','AllRes','AllPix','AllSCT'],nameLabels=['Combined','Resolved','Pix','SCT']) )
cs.extend( makeStandardPlots(fii,prefix="VtxMatcombrespixsct",  names=['MatchedDefault','resMatched','pixMatched','sctMatched'],nameLabels=['Combined','resMatched','Pix','SCT'],doMC=True) )
cs.extend( makeStandardPlots(fii,prefix="VtxMatcombrespixsctNoLog",  names=['MatchedDefault','resMatched','pixMatched','sctMatched'],nameLabels=['Combined','resMatched','Pix','SCT'],doMC=True,logy=False) )

cs.extend( makeDeltaPlots(fii,   prefix="VtxDpixcomb",   names=['pixMatchedComb'],nameLabels=['Comb-Pix']) )
cs.extend( makeDeltaPlots(fii,   prefix="VtxDrescomb",   names=['resMatchedComb'],nameLabels=['Comb-Res']) )
cs.extend( makeDeltaPlots(fii,   prefix="VtxDsctcomb",   names=['sctMatchedComb'],nameLabels=['Comb-SCT']) )
cs.extend( makeDeltaPlots(fii,   prefix="VtxDpixsct",    names=['sctMatchedPix'], nameLabels=['Pix-SCT']) )

cs.extend( makeDeltaPlots(fii,   prefix="VtxDrespixsctcomb",   names=['resMatchedComb','pixMatchedComb','sctMatchedComb'],nameLabels=['Res-Comb','Pix-Comb','SCT-Comb']) )


#  def makeCanvas(name, hists,  xlabel, ylabel, doStats=True,doLegend=True, units="", logy=False,setMinZero=True,drawOptions="",histLabels=[]):



if len(options.beamspotFile):
  bsfii = TFile(options.beamspotFile)

  bsfii.cd()
  bs = findBeamspots(bsfii,"Beamspot","Beamspots.*")
      
  type = ['Adaptive','InDetPriVxFinderFastFinder','InDetPriVxFinderFullFinder']
  for t in type:
    bsAdaptive = findBeamspots(bsfii,"Beamspot","Beamspots.*"+t+".*")

    hx=[]
    hy=[] 
    hz=[]
    hsx=[]
    hsy=[]
    hsz=[]
    hax=[]
    hay=[]
    hk=[]
    hrhoxy=[]
          
          
  
    for i in bsAdaptive:
      if "TRT" in i.GetName():
        continue
      if "SCT" in i.GetName():
        name = "SCT"
      elif "Pix" in i.GetName():
        name = "Pix"
      elif "Combined" in i.GetName():
        name = "Combined"
      elif "Res" in i.GetName():
        name = "Resolved"
      else:
        name = i.GetName()
    
      
      hists = makeHists(i,name)
      hx.append(hists[0])
      hy.append(hists[1])
      hz.append(hists[2])
      hsx.append(hists[3])
      hsy.append(hists[4])
      hsz.append(hists[5])
      hax.append(hists[6])
      hay.append(hists[7])
      hk.append(hists[8])
      hrhoxy.append(hists[9])
  
    cs.append( makeCanvas(t+"_x",hx,"x [mm]","Entries / (XXX mm)"))
    cs.append( makeCanvas(t+"_y",hy,"y [mm]","Entries / (XXX mm)"))
    cs.append( makeCanvas(t+"_z",hz,"z [mm]","Entries / (XXX mm)"))
    cs.append( makeCanvas(t+"_sx",hsx,"sx [mm]","Entries / (XXX mm)"))
    cs.append( makeCanvas(t+"_sy",hsy,"sy [mm]","Entries / (XXX mm)"))
    cs.append( makeCanvas(t+"_sz",hsz,"sz [mm]","Entries / (XXX mm)"))
    cs.append( makeCanvas(t+"_ax",hax,"ax [mm]","Entries / (XXX mm)"))
    cs.append( makeCanvas(t+"_ay",hay,"ay [mm]","Entries / (XXX mm)"))
    cs.append( makeCanvas(t+"_k",hk,"k [mm]","Entries / (XXX mm)"))
    cs.append( makeCanvas(t+"_rhoxy",hrhoxy,"rhoxy [mm]","Entries / (XXX mm)"))

for i in cs:
  i.SaveAs("plots/"+i.GetName()+".eps")
  i.SaveAs("plots/"+i.GetName()+".png")



s = raw_input('--> ')
exit()

