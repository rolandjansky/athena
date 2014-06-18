#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT
import sys
import math

nTowers = len(sys.argv[1:])

print "Slices in files:", ", ".join(sys.argv[1:])

minPhi = [0 for i in range(nTowers)]
maxPhi = [0 for i in range(nTowers)]
minCTheta = [0 for i in range(nTowers)]
maxCTheta = [0 for i in range(nTowers)]
minIP = [0 for i in range(nTowers)]
maxIP = [0 for i in range(nTowers)]
minZ0 = [0 for i in range(nTowers)]
maxZ0 = [0 for i in range(nTowers)]
minCurv = [0 for i in range(nTowers)]
maxCurv = [0 for i in range(nTowers)]
histoCurv = [None for i in range(nTowers)] 
histoD0 = [None for i in range(nTowers)] 
histoPhi = [None for i in range(nTowers)] 
histoZ0 = [None for i in range(nTowers)] 
histoCTheta = [None for i in range(nTowers)] 

phi_min = -ROOT.TMath.Pi()
phi_max = ROOT.TMath.Pi()
curv_min = -7e-4
curv_max = 7e-4
d0_min = -2.2
d0_max = 2.2
z0_min = -120.
z0_max = 120.
ctheta_min = ROOT.TMath.SinH(-3.);
ctheta_max = ROOT.TMath.SinH(3.);

NCons = 2

def slicesToHisto(name,title,slices,min,max) :
  nslices = slices.GetEntries()
  histo = ROOT.TH1F(name,title,nslices,min,max)
  for ibin in xrange(nslices) :
    # return the bitmask for the sectors in this bin
    sectors_bit = slices.At(ibin)
    # get the number of sector related to this slice
    nsectors = sectors_bit.CountBits()
    histo.SetBinContent(ibin+1,nsectors);
  return histo

def checkNBins(slices, i, n):
  nslices = slices.GetEntries()
  sgn = n/abs(n)
  nr = n
  if i+n>nslices :
    nr = nslices-i
  elif i+n<-1 :
    nr = -i-1

  for ibin in xrange(i,i+nr, sgn) :
    bin = slices.UncheckedAt(ibin)
    if bin.FirstSetBit() == bin.GetNbits() :
      return False
  return True

def slicesRange(slices,min,max,Period=None) :
  nslices = slices.GetEntries()

  imin = 0
  imean = nslices/2
  imax = nslices
  aroundMin = False
  aroundMax = False
  
  ignore = False
  for ibin in xrange(nslices) :
    if checkNBins(slices,ibin,NCons) and not ignore:
      if Period!=None and ibin==0 :
        print "*min"
        aroundMin = True
        ignore = True
        continue
      else :
        imin = ibin
        break
    elif not checkNBins(slices,ibin,NCons) and ignore :
      ignore = False
  # end minimum for
  
  maxbits = 0
  for ibin in xrange(nslices) :
    bin = slices.UncheckedAt(ibin)
    if bin.CountBits()>maxbits :
      maxbits = bin.CountBits()
      imean = ibin
  # end center for
  
  ignore = False
  for ibin in xrange(nslices-1,-1,-1) :
    if checkNBins(slices,ibin,-NCons) and not ignore:
      if Period!=None and ibin == (nslices-1) :
        print "*max"
        aroundMax = True
        ignore = True
        continue
      else :
        imax = ibin
        break
    elif not checkNBins(slices,ibin,-NCons) and ignore :
      ignore = False
  # end max for

  print "***", imin, imean, imax, nslices
  
  step = (max-min)/nslices
  if (aroundMin or aroundMax) and Period!=None:
    if imean<imin :
      mean = imean*step+min+Period
    else :
      mean = imean*step+min
      
    return imin*step+min, mean, Period+max-(nslices-imax-1)*step
  else :
     return imin*step+min, imean*step+min, max-(nslices-imax-1)*step

outfile = ROOT.TFile.Open("slicestest.root","recreate")
grphirange = ROOT.TGraphAsymmErrors(nTowers)
grcurvrange = ROOT.TGraphAsymmErrors(nTowers)
grd0range = ROOT.TGraphAsymmErrors(nTowers)
grcthetarange = ROOT.TGraphAsymmErrors(nTowers)
grz0range = ROOT.TGraphAsymmErrors(nTowers)

for itower, curpath in  enumerate(sys.argv[1:]) :
  # get the file
  print "Studying:", curpath, "(%d/%d)" % (itower,nTowers)
  curfile = ROOT.TFile.Open(curpath)
  # extract the slices' objects

  bits_phi = ROOT.gFile.Get("c_bits_phi")
  bits_c = ROOT.gFile.Get("c_bits_c")
  bits_d = ROOT.gFile.Get("c_bits_d")
  bits_z0 = ROOT.gFile.Get("c_bits_z0")
  bits_ctheta = ROOT.gFile.Get("c_bits_ctheta")

  hphi = slicesToHisto("hphi%d" % itower,"N sectors in #phi",bits_phi,phi_min,phi_max)
  hphi.SetDirectory(outfile)
  histoPhi[itower] = hphi
  phirange = slicesRange(bits_phi,phi_min,phi_max,ROOT.TMath.TwoPi())
  minPhi[itower] = phirange[0]
  maxPhi[itower] = phirange[2]
  print "Phi:", phirange,
  if phirange[1]>ROOT.TMath.TwoPi() :
    print "(*)"
  else :
    print
  grphirange.SetPoint(itower,phirange[1],itower)
  grphirange. SetPointEXhigh(itower,phirange[2]-phirange[1])
  grphirange. SetPointEXlow(itower,phirange[1]-phirange[0])
  
  hcurv = slicesToHisto("hcurv%d" % itower,"N sectors in curvature",bits_c,curv_min,curv_max)
  hcurv.SetDirectory(outfile)
  histoCurv[itower] = hcurv
  curvrange = slicesRange(bits_c,curv_min,curv_max)
  minCurv[itower] = curvrange[0]
  maxCurv[itower] = curvrange[2]
  print "Curvature:", curvrange
  grcurvrange.SetPoint(itower,curvrange[1],itower)
  grcurvrange. SetPointEXhigh(itower,curvrange[2]-curvrange[1])
  grcurvrange. SetPointEXlow(itower,curvrange[1]-curvrange[0])
  
  hd0 = slicesToHisto("hd0%d" % itower,"N sector in d_{0}",bits_d,d0_min,d0_max)
  hd0.SetDirectory(outfile)
  histoD0[itower] = hd0
  d0range = slicesRange(bits_d,d0_min,d0_max)
  minIP[itower] = d0range[0]
  maxIP[itower] = d0range[2]
  print "d0:", d0range
  grd0range.SetPoint(itower,d0range[1],itower)
  grd0range. SetPointEXhigh(itower,d0range[2]-d0range[1])
  grd0range. SetPointEXlow(itower,d0range[1]-d0range[0])
  
  hz0 = slicesToHisto("hz0%d" % itower,"N sector in z_{0}",bits_z0,z0_min,z0_max)
  hz0.SetDirectory(outfile)
  histoZ0[itower] = hz0
  z0range = slicesRange(bits_z0,z0_min,z0_max)
  minZ0[itower] = z0range[0]
  maxZ0[itower] = z0range[2]
  print "z0:", z0range
  grz0range.SetPoint(itower,z0range[1],itower)
  grz0range. SetPointEXhigh(itower,z0range[2]-z0range[1])
  grz0range. SetPointEXlow(itower,z0range[1]-z0range[0])
  
  hctheta = slicesToHisto("hctheta%d" % itower,"N sector in Cot#theta",bits_ctheta,ctheta_min,ctheta_max)
  hctheta.SetDirectory(outfile)
  histoCTheta[itower] = hctheta
  cthetarange = slicesRange(bits_ctheta,ctheta_min,ctheta_max)
  minCTheta[itower] = cthetarange[0]
  maxCTheta[itower] = cthetarange[2]
  print "CotTheta:", cthetarange
  grcthetarange.SetPoint(itower,cthetarange[1],itower)
  grcthetarange. SetPointEXhigh(itower,cthetarange[2]-cthetarange[1])
  grcthetarange. SetPointEXlow(itower,cthetarange[1]-cthetarange[0])
# end tower loop

outfile.cd()
grphirange.Write("phirange")
grcurvrange.Write("curvrange")
grd0range.Write("d0range")
grcthetarange.Write("cthetarange")
grz0range.Write("z0range")

outfile.Write()
outfile.Close()
