#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import gROOT
gROOT.SetBatch(True)

import sys
import numpy as np

from ROOT import *
import AtlasStyle


from optparse import OptionParser
p = OptionParser()
p.add_option('--input',  type = 'string', default = "resStudyHists.root",   dest = 'inFile', help = 'intput File' )
p.add_option('--output', type = 'string', default = "fitResPlots.root", dest = 'outFile', help = 'output File' )
p.add_option('--mode', type = 'string', default = "sigma", dest = 'mode', help = 'mode' )
p.add_option('--eps', action = 'store_true', default = False, dest = 'doEPSs', help = 'save epss' )

p.add_option('--doPtStudy',   action='store_true', default = False, dest = 'doPtStudy',  help = 'Study the Pt  dependence vs Eta' )
p.add_option('--doEtaStudy',  action='store_true', default = False, dest = 'doEtaStudy', help = 'Study the ETa dependence vs Pt' )

p.add_option('--noFTK',action='store_false',default=True,dest='doFTK', help='File has no FTK tracks')
p.add_option('--noOff',action='store_false',default=True,dest='doOff', help='File has no Off tracks')

(o,a) = p.parse_args()


##open the root file
file0 = TFile(o.inFile,"READ");
outFile = TFile(o.outFile,"RECREATE");

##define functions for fitting
linear = TF1("linear", "[0]+x*[1]");
abslinear = TF1("abslinear", "[0]+[1]*abs(x)");
sq = TF1("sq", "sqrt([0]+[1]*x^2)");
poly2 = TF1("poly2", "[0]+[1]*x^2");
poly4 = TF1("poly4", "[0]+[1]*x^2+[2]*x^4");
lconst = TF1("lconst", "[0]", -1, 0);
rconst = TF1("rconst", "[1]", 0, 1);

def drawText(text):
    Tl = TLatex();
    Tl.SetNDC()
    for line in text:
        theWords = line["text"]
        xVal = line["xVal"]
        yVal = line["yVal"]
        Tl.SetTextSize(line["size"])
        if "color" in line:
            Tl.SetTextColor(line["color"])
        if "font" in line:
            Tl.SetTextFont(line["font"])
        Tl.DrawLatex(xVal, yVal, theWords);
    return

def printText(text):

    for line in text:
        theWords = line["text"]
        print theWords
    return

def SillyQuantiles(hist):
    tot = hist.GetEntries()
    nbins = hist.GetNbinsX()
    tempfracoldL =0
    tempfracoldH = 1
    lowbin = 0
    highbin = nbins
    print "Low Bin"
    for i in range(nbins):
        tempfrac=hist.Integral(0,i)/tot
        print tempfrac,i
        if tempfracoldL<0.025 and tempfrac > 0.025:
            lowbin = i
            if abs(tempfracoldL-0.025) < abs(tempfrac -0.025):
                lowbin = i-1
            break
        else:
            tempfracoldL = tempfrac
    print "High Bin"
    for i in range(nbins):
        tempfrac=hist.Integral(nbins-i,nbins+1)/tot
        print tempfrac,i
        if tempfracoldH<0.025 and tempfrac > 0.025:
            highbin = nbins-i
            if abs(tempfracoldH-0.025) < abs(tempfrac-0.025):
                highbin = nbins - (i-1)
            break
        else:
            tempfracoldH = tempfrac

    return lowbin,highbin

    

def fitPoints(inputHist, fit_type, textOffset=0.8):
    inputHist.Fit(fit_type,"q","same")

    if not inputHist.GetFunction(fit_type):
        return

    from math import log10, floor

    rawPar0 = inputHist.GetFunction(fit_type).GetParameter(0)
    par0 = str('%.2e' % rawPar0)
    par0Text = [{"text":"par0: "+par0, "xVal":0.4,"yVal":textOffset, "size":0.05, "color":kBlack, "font":132}]

    rawPar1 = inputHist.GetFunction(fit_type).GetParameter(1)
    par1 = str('%.2e' % rawPar1)
    par1Text = [{"text":"par1: "+par1, "xVal":0.4,"yVal":textOffset-0.05, "size":0.05, "color":kBlack, "font":132}]

    return par0Text+par1Text


def fillSigmaRMS(inputHist,thisName,ref,fit_plot):

  Xmin = inputHist.GetYaxis().GetXmin()
  Xmax = inputHist.GetYaxis().GetXmax()

  sigmaVsTP  = TH1F("sigma_"+thisName,"Sigma "+thisName,inputHist.GetNbinsY(),Xmin,Xmax);
  sigmaVsTP.SetMinimum(0)
  sigmaVsTP.SetMaximum(inputHist.GetXaxis().GetXmax()/2)
  sigmaVsTP.GetXaxis().SetTitle(ref)
  sigmaVsTP.GetYaxis().SetTitle("sigma "+fit_plot)

  rmsVsTP    = TH1F("rms_"+thisName,  "RMS   "+thisName,inputHist.GetNbinsY(),Xmin,Xmax);
  rmsVsTP.SetMinimum(0)
  rmsVsTP.SetMaximum(2*inputHist.GetXaxis().GetXmax()/3)
  rmsVsTP.GetXaxis().SetTitle(ref)
  rmsVsTP.GetYaxis().SetTitle("rms "+fit_plot)  

  for ibin in range(inputHist.GetNbinsY()):
      histThisBin = inputHist.ProjectionX(inputHist.GetName()+"_"+str(ibin),ibin,ibin)
      if not histThisBin.GetEntries(): continue

      histThisBin.Fit("gaus","q")
      fitRes = histThisBin.FindObject("gaus")
      if not fitRes:
        continue

      sigmaFit = fitRes.GetParameter(2)
      sigmaErr = fitRes.GetParError(2)
      rmsRaw   = histThisBin.GetRMS()
      lb,hb = SillyQuantiles(histThisBin)
      print lb,hb
      histThisBin.GetXaxis().SetRange(lb,hb)
      rmsRaw = histThisBin.GetRMS()
      print rmsRaw

      sigmaVsTP.SetBinContent(ibin,sigmaFit)
      sigmaVsTP.SetBinError  (ibin,sigmaErr)
      rmsVsTP  .SetBinContent(ibin,rmsRaw)
      rmsVsTP  .SetBinError  (ibin,0.05*rmsRaw)
      
      sigmaVsTP.SetMarkerColor(2)
      sigmaVsTP.SetLineColor(2)
      
      histThisBin.Write()


  return sigmaVsTP, rmsVsTP


def doResPlot(fit_plot,ref,text={}):

  #
  # fit_op =["sq", "abslinear", "poly2"]
  #

  print "Doing",fit_plot,"vs",ref
  thisName = fit_plot+"_vs_"+ref

  thisDir = outFile.mkdir(thisName)
  thisDir.cd()

  #
  # Get the sigma and RMS plots (BL) 
  #
  plotname = "histo_res" + fit_plot + "_" + ref 
  histo1 = file0.Get(plotname)

  if not histo1:
    file0.ls()
    print "Cannot get ",plotname
    assert histo1

  sigmaVsTP, rmsVsTP = fillSigmaRMS(histo1,thisName, ref, fit_plot)

  #
  # Get the sigma and RMS plots (no BL) 
  #
  plotname_noBL = "histo_res" + fit_plot + "_" + ref + "_nb"
  histo2 = file0.Get(plotname_noBL)

  if not histo2:
    print "Cannot get ",plotname_noBL
    assert histo2

  sigmaVsTPNoBL, rmsVsTPNoBL = fillSigmaRMS(histo2,thisName+"_NoBL",ref,fit_plot)


  #
  # Compare the RMS and the Sigma
  #
  
  outFile.cd()
  cRMSvsSigma = TCanvas("can_rmsVsSigma"+thisName,thisName)
  cRMSvsSigma.SetTopMargin(0.1)
  sigmaVsTP.Draw("e")
  rmsVsTP.SetLineColor(ROOT.kRed)
  rmsVsTP.SetMarkerColor(ROOT.kRed)
  rmsVsTP  .Draw("esame")
  cRMSvsSigma.Write()

  if o.doEPSs:
    print o.outFile.rstrip(".root")+"/"+cRMSvsSigma.GetName()+".eps"
    import os
    if not os.path.isdir(o.outFile.rstrip(".root")):
      os.mkdir(o.outFile.rstrip(".root"))

    cRMSvsSigma.SaveAs(o.outFile.rstrip(".root")+"/"+cRMSvsSigma.GetName()+".eps")

  #
  #  Do the fitting
  #

  #fit_op =["sq", "abslinear", "poly2"]
  fit_type = "abslinear"

  fitTextBL   = fitPoints(sigmaVsTP,     fit_type)
  fitTextNoBL = fitPoints(sigmaVsTPNoBL, fit_type,0.6)
  if fitTextNoBL:
      fitTextNoBL[0]["color"] = kRed
      fitTextNoBL[1]["color"] = kRed

  if text:
      text += fitTextBL
      if fitTextNoBL:
          text += fitTextNoBL
  else:
      text  = fitTextBL
      if fitTextNoBL:
          text += fitTextNoBL

  #
  #  BLayer
  #

  c1 = TCanvas("canFIT_"+thisName+"_w/b_"+fit_type+"fit",thisName+"_w/b_"+fit_type+"fit",700,500)
  c1.cd()
  
  sigmaVsTP.Draw()
  sigmaVsTPNoBL.SetLineColor(kRed)
  sigmaVsTPNoBL.SetMarkerColor(kRed)
  sigmaVsTPNoBL.Draw("same")

  if text:
      drawText(text)

  c1.Write()

  return
#
#  histo1.FitSlicesX(0,0,-1,0,"RNSq")
#  slicename1 = plotname+"_2"
#  slice1 = ROOT.gDirectory.Get(slicename1)
#  assert slice1
#
#  histo2.FitSlicesX(0,0,-1,0,"RNSq")
#  slicename2 = plotname_noBL+"_2"
#  slice2 = ROOT.gDirectory.Get(slicename2)
#  assert slice2
#
#  ## fit the plot
#  if fit_type != "const":
#    slice1.Fit(fit_type,"q")
#    gStyle.SetOptFit(11111)
#    gStyle.SetStatW(0.12)
#    gStyle.SetStatH(0.08)
#    gStyle.SetStatX(0.95)
#    gStyle.SetStatY(1)
#    gStyle.SetTextSize(0.1)
#    slice2.Fit(fit_type,"q")
#    gStyle.SetOptFit(11111)
#    gStyle.SetStatW(0.12)
#    gStyle.SetStatH(0.08)
#    gStyle.SetStatX(0.95)
#    gStyle.SetStatY(1)
#    gStyle.SetTextSize(0.1)
#    
#  else: #fit_type == "const":
#    slice1.Fit(lconst, "Rq")
#    slice1.Fit(rconst, "R+q")
#    
#  #
#  #  BLayer
#  #
#
#  c1 = TCanvas("can_"+thisName+"_w/b_"+fit_type+"fit",thisName+"_w/b_"+fit_type+"fit",700,500)
#  c1.SetTopMargin(0.1)
#  c1.cd()
#  
#  slice1.SetLineColor(1)
#  slice1.Draw()
#
#  slice1.GetXaxis().SetTitle(ref)
#  slice1.GetYaxis().SetTitle("res" + "_" + fit_plot)
#  slice1.SetTitle("Res_" + fit_plot + " Vs. " +ref + "_w/b")
#
#  xl1=.1
#  yl1=0.76
#  xl2=xl1+.2
#  yl2=yl1+.17;
#
#  leg1 = TLegend(xl1,yl1,xl2,yl2)
#  leg1.SetFillColor(0)
#  leg1.AddEntry(slice1,"W/ B Layer","l");
#  leg1.Draw()
#
#  c1.Write()
#
#  #
#  #  No BLayer
#  #
#
#  c2 = TCanvas("can_"+thisName+"_wo/b_"+fit_type+"fit",thisName+"_wo/b_"+fit_type+"fit",700,500)
#  c2.SetTopMargin(0.1)
#  slice2.SetLineColor(4)
#  slice2.SetMarkerColor(4)
#  slice2.Draw()
#
#  slice2.GetXaxis().SetTitle(ref)
#  slice2.GetYaxis().SetTitle("res" + "_" + fit_plot)
#  slice2.SetTitle("Res_" + fit_plot + " Vs. " +ref + "_wo/b")
#
#  leg2 = TLegend(xl1,yl1,xl2,yl2)
#  leg2.SetFillColor(0)
#  leg2.AddEntry(slice2,"WO/ B Layer","l");
#
#  leg2.Draw()
#  outFile.cd()
#  c2.Write()
#  
#  return

#
#  make the res vs TP
#

def makeResVs(fit_plot, refName, r, xTitle, yTitle, BL = ""):

  plotname = "histo_res" + fit_plot + "_" + r + BL

  thisName = fit_plot+"_vs_"+refName+"_PtStudy"

  #
  # getting histograms and fit slices
  #
  hist = file0.Get(plotname)

  if not hist:
    file0.ls()
    print "Cannot get ",plotname
    
  Xmin = hist.GetYaxis().GetXmin()
  Xmax = hist.GetYaxis().GetXmax()

  sigmaVsTP  = TH1F("sigma_"+thisName,"Sigma "+thisName+";"+xTitle+";"+yTitle,hist.GetNbinsY(),Xmin,Xmax);
  sigmaVsTP.SetMinimum(0)
  
  max = 0

  histIDs = []
  
  for ibin in range(hist.GetNbinsY()):
    histID = str(round(sigmaVsTP.GetBinCenter(ibin),3)).replace("-","n")
    histIDs.append(histID)
    histThisBin = hist.ProjectionX(hist.GetName()+"_"+histID,ibin,ibin)
    
    if not histThisBin.GetEntries(): continue
    
    histThisBin.Fit("gaus","Q")
    fitRes = histThisBin.FindObject("gaus")
    if not fitRes:
      continue

    sigmaFit = fitRes.GetParameter(2)
    sigmaErr = fitRes.GetParError(2)
    rmsRaw   = histThisBin.GetRMS()

    if sigmaFit > max and sigmaErr/sigmaFit < 0.3 :
      max = sigmaFit
    
    sigmaVsTP.SetBinContent(ibin,sigmaFit)
    sigmaVsTP.SetBinError  (ibin,sigmaErr)
    
    histThisBin.Write()

  sigmaVsTP.SetMaximum(1.2*max)

  return sigmaVsTP



#
#  Pt Study
#

def doPtStudy(fit_plot,refName,refs,colors = [ROOT.kBlack,ROOT.kBlue,ROOT.kRed,ROOT.kGreen,ROOT.kOrange],xTitle="",yTitle="",text={},BL=""):

  print "PtStudy",fit_plot,"vs",refName
  thisName = fit_plot+"_vs_"+refName+"_PtStudy"

  thisDir = outFile.mkdir(thisName)
  thisDir.cd()
  
  sigmaVsTPs = []

  max = 0

  for r in refs:
    sigmaVsTP  =  makeResVs(fit_plot, refName, r, xTitle, yTitle, BL )
    sigmaVsTPs.append(sigmaVsTP)
    
    localMax = sigmaVsTP.GetMaximum()
    if localMax > max:
      max = localMax
      

  outFile.cd()
  cPtStudy     = TCanvas("PtStudy"+thisName,thisName)
  fitsText = []

  doFit = False

  for itr, s in enumerate(sigmaVsTPs):
    if itr:
      s.SetLineColor(colors[itr])
      s.SetMarkerColor(colors[itr])
      
      if doFit:
          thisText = fitPoints(s,     "abslinear" , textOffset=0.9-itr*0.1)
          thisText[0]["color"] = colors[itr]
          thisText[1]["color"] = colors[itr]
          fitsText += thisText
     
      s.SetName(s.GetName()+"Bin%s" %itr)
      s.Write()
      s.Draw("esame")

    else:
      s.SetMaximum(max)
      s.SetStats(0)

      if doFit:
          thisText = fitPoints(s,     "abslinear" , textOffset=0.9-itr*0.1)
          thisText[0]["color"] = colors[itr]
          thisText[1]["color"] = colors[itr]
          fitsText += thisText

      s.SetName(s.GetName()+"Bin%s" %itr)
      print "writing %s" %s.GetName()
      s.Write()
      s.Draw("e")


  cPtStudy.cd()
  if doFit:
      if fitsText:
          print
          print "============================"
          print "Constants for", fit_plot,"vs",refName
          printText(fitsText)
          print "============================"
          print
          drawText(fitsText)
  elif text:
    drawText(text)
  cPtStudy.Write()


  if o.doEPSs:
    print o.outFile.rstrip(".root")+"/"+cPtStudy.GetName()+".eps"
    import os
    if not os.path.isdir(o.outFile.rstrip(".root")):
      os.mkdir(o.outFile.rstrip(".root"))
    cPtStudy.SaveAs(o.outFile.rstrip(".root")+"/"+cPtStudy.GetName()+".eps")
    cPtStudy.SaveAs(o.outFile.rstrip(".root")+"/"+cPtStudy.GetName()+".pdf")


  return


#
#  Pt Study
#

def compRes(fit_var1,fit_var2, refName, ref, colors = [ROOT.kRed,ROOT.kBlack],xTitle="",yTitle="",text={},saveTGraphs=False):

  print "Comparing",fit_var1,"and",fit_var2,"vs",refName,"for bin",ref
  thisName = fit_var1+"_and_"+fit_var2+"_vs_"+ref

  outFile.mkdir(thisName).cd()

  if o.doFTK:
      resVar1  =  makeResVs(fit_var1, refName, ref, xTitle, yTitle )

  if o.doOff:
      resVar2  =  makeResVs(fit_var2, refName, ref, xTitle, yTitle) 

  outFile.cd()
  can = TCanvas("CompRes_"+thisName,thisName)
  can.SetTopMargin(0.1)

  if o.doFTK:
      resVar1.SetLineColor  (colors[0])
      resVar1.SetMarkerColor(colors[0])
      resVar1.SetStats(0)
      resVar1.Draw("e")

  if o.doOff:
      resVar2.SetLineColor  (colors[1])
      resVar2.SetMarkerColor(colors[1])
      if o.doFTK:
          resVar2.Draw("esame")
      else:
          resVar2.Draw("e")
  if text:
    drawText(text)

  if saveTGraphs:
      if o.doFTK:
          resVar1.Write()
      if o.doOff:
          resVar2.Write()

  can.Write()

  if o.doEPSs:
    print o.outFile.rstrip(".root")+"/"+can.GetName()+".eps"
    import os
    if not os.path.isdir(o.outFile.rstrip(".root")):
      os.mkdir(o.outFile.rstrip(".root"))
    can.SaveAs(o.outFile.rstrip(".root")+"/"+can.GetName()+".eps")
    can.SaveAs(o.outFile.rstrip(".root")+"/"+can.GetName()+".pdf")


  return




if o.doPtStudy:
  compTPList = ["d0","z0","z0st","Ipt","eta","phi"]
  for cTP in compTPList:

    #
    # The Single differential distributions
    #
    compRes(cTP,  "off_"+cTP, "eta", "eta", xTitle="eta", yTitle=cTP+" resolution",  saveTGraphs=True)
    compRes(cTP,  "off_"+cTP, "Ipt", "InvPt", xTitle="Ipt", yTitle=cTP+" resolution",saveTGraphs=True)


    #
    # The double differential distributions
    #


    text = [{"text":"Offline Tracks",    "xVal":0.3,"yVal":0.8, "size":0.07,   "font":132},
            {"text":"FTK Tracks",        "xVal":0.3,"yVal":0.75, "size":0.07, "color":ROOT.kRed, "font":132},
            ]

    
    textPt1 = [{"text":"1/2Pt < 1e-4",        "xVal":0.3,"yVal":0.7, "size":0.05, "font":132}]
    compRes(cTP,  "off_"+cTP, "eta", "eta01", xTitle="eta", yTitle=cTP+" resolution",text=textPt1+text)

    textPt2 = [{"text":"1e-4 < 1/2Pt < 2e-4",        "xVal":0.3,"yVal":0.7, "size":0.05, "font":132}]
    compRes(cTP,  "off_"+cTP, "eta", "eta02", xTitle="eta", yTitle=cTP+" resolution",text=textPt2+text)

    textPt3 = [{"text":"2e-4 < 1/2Pt < 3e-4",        "xVal":0.3,"yVal":0.7, "size":0.05, "font":132}]
    compRes(cTP,  "off_"+cTP, "eta", "eta03", xTitle="eta", yTitle=cTP+" resolution",text=textPt3+text)

    textPt4 = [{"text":"3e-4 < 1/2Pt < 4e-4",        "xVal":0.3,"yVal":0.7, "size":0.05, "font":132}]
    compRes(cTP,  "off_"+cTP, "eta", "eta04", xTitle="eta", yTitle=cTP+" resolution",text=textPt4+text)

    textPt5 = [{"text":"4e-4 < 1/2Pt ",        "xVal":0.3,"yVal":0.7, "size":0.05, "font":132}]
    compRes(cTP,  "off_"+cTP, "eta", "eta05", xTitle="eta", yTitle=cTP+" resolution",text=textPt5+text)


    textPt1 = [{"text":"|eta| < 0.5",        "xVal":0.3,"yVal":0.7, "size":0.05, "font":132}]
    compRes(cTP,  "off_"+cTP, "Ipt", "pt0", xTitle="Ipt", yTitle=cTP+" resolution",text=textPt1+text)

    textPt1 = [{"text":"0.5 < |eta| < 1.0",        "xVal":0.3,"yVal":0.7, "size":0.05, "font":132}]
    compRes(cTP,  "off_"+cTP, "Ipt", "pt1", xTitle="Ipt", yTitle=cTP+" resolution",text=textPt1+text)

    textPt1 = [{"text":"1.0 < |eta| < 1.5",        "xVal":0.3,"yVal":0.7, "size":0.05, "font":132}]
    compRes(cTP,  "off_"+cTP, "Ipt", "pt2", xTitle="Ipt", yTitle=cTP+" resolution",text=textPt1+text)

    textPt1 = [{"text":"1.5 < |eta| < 2.0",        "xVal":0.3,"yVal":0.7, "size":0.05, "font":132}]
    compRes(cTP,  "off_"+cTP, "Ipt", "pt3", xTitle="Ipt", yTitle=cTP+" resolution",text=textPt1+text)

    textPt1 = [{"text":"2.0 < |eta| < 2.5",        "xVal":0.3,"yVal":0.7, "size":0.05, "font":132}]
    compRes(cTP,  "off_"+cTP, "Ipt", "pt4", xTitle="Ipt", yTitle=cTP+" resolution",text=textPt1+text)

resTP=[]

if o.doFTK:
    resTP.extend(["d0","z0","z0st","Ipt","eta","phi"])
if o.doOff:
    resTP.extend(["off_d0","off_z0","off_z0st","off_Ipt","off_eta","off_phi"])

for tp in resTP:
  
  if o.doPtStudy:
    doPtStudy(tp,"eta",["eta01","eta02","eta03","eta04","eta05"],xTitle="eta",yTitle=tp+" resolution",
              text = [
              {"text":"1/2Pt < 1e-4",               "xVal":0.4,"yVal":0.8,  "size":0.05, "color":kBlack, "font":132},
              {"text":"1e-4 < 1/2Pt < 2e-4",        "xVal":0.4,"yVal":0.75, "size":0.05, "color":kBlue,  "font":132},
              {"text":"2e-4 < 1/2Pt < 3e-4",        "xVal":0.4,"yVal":0.7,  "size":0.05, "color":kRed,   "font":132},
              {"text":"3e-4 < 1/2Pt < 4e-4",        "xVal":0.4,"yVal":0.65, "size":0.05, "color":kGreen, "font":132},
              {"text":"4e-4 < 1/2Pt ",              "xVal":0.4,"yVal":0.6,  "size":0.05, "color":kOrange,"font":132},
              ],
              )

  if o.doEtaStudy:
    doPtStudy(tp,"Ipt",["pt0","pt1","pt2","pt3","pt4"],xTitle="Ipt",yTitle=tp+" resolution",
              text = [
            {"text":"|#eta| < 0.5 ",     "xVal":0.4,"yVal":0.8,  "size":0.05, "color":kBlack, "font":132},
            {"text":"0.5 |#eta| < 1.0 ", "xVal":0.4,"yVal":0.75, "size":0.05, "color":kBlue,  "font":132},
            {"text":"1.0 |#eta| < 1.5 ", "xVal":0.4,"yVal":0.70, "size":0.05, "color":kRed,   "font":132},
            {"text":"1.5 |#eta| < 2.0 ", "xVal":0.4,"yVal":0.65, "size":0.05, "color":kGreen, "font":132},
            {"text":"2.0 |#eta| < 2.5 ", "xVal":0.4,"yVal":0.60, "size":0.05, "color":kOrange,"font":132},
            ],
              )

  doResPlot(tp,"InvPt")
  doResPlot(tp,"eta"  )
  doResPlot(tp,"phi"  )  


