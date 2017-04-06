#!/bin/usr/env python
import re,sys
from decimal import Decimal, ROUND_HALF_UP
from math import *
from array import array
#from ROOT import TCanvas, TFile, TH1, TH2, TGaxis, TPaletteAxis, gStyle, gROOT, gPad, TObject, TLegend, TPaveText, TColor
from ROOT import *
#from ROOT import kBlack, kBlue, kRed, kTRUE, kFALSE

colors = [kBlack, kRed+2, kBlue+2, kGreen+4]
marker = [kOpenCircle,kFullCircle,kOpenSquare,kFullSquare]


def set_plot_style():
  NRGBs = 5;
  NCont = 255;

  stops = array("d",[ 0.00, 0.34, 0.61, 0.84, 1.00 ])
  red   = array("d",[ 0.00, 0.00, 0.87, 1.00, 0.51 ])
  green = array("d",[ 0.00, 0.81, 1.00, 0.20, 0.00 ])
  blue  = array("d",[ 0.51, 1.00, 0.12, 0.00, 0.00 ])
  TColor.CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  gStyle.SetNumberContours(NCont);


   #Set more style

  gStyle.SetOptStat(0);
  gStyle.SetOptTitle(0);
  icol=0
  gStyle.SetFrameBorderMode(icol)
  gStyle.SetFrameFillColor(icol)
  gStyle.SetCanvasBorderMode(icol)
  gStyle.SetCanvasColor(icol)
  gStyle.SetPadBorderMode(icol)
  gStyle.SetPadColor(icol)
  gStyle.SetStatColor(icol)
  
  gStyle.SetPaperSize(20,26)
  
  gStyle.SetPadTopMargin(0.05)
  gStyle.SetPadRightMargin(0.17)
  gStyle.SetPadBottomMargin(0.14)
  gStyle.SetPadLeftMargin(0.08)
  
  
  font=42
  tsize=0.08
  gStyle.SetTextFont(font)
  gStyle.SetTextSize(tsize)
  gStyle.SetLabelFont(font,"x")
  gStyle.SetTitleFont(font,"x")
  gStyle.SetLabelFont(font,"y")
  gStyle.SetTitleFont(font,"y")
  gStyle.SetLabelFont(font,"z")
  gStyle.SetTitleFont(font,"z")
  gStyle.SetLabelSize(tsize,"x")
  gStyle.SetTitleSize(tsize,"x")
  gStyle.SetLabelSize(tsize,"y")
  gStyle.SetTitleSize(tsize,"y")
  gStyle.SetLabelSize(tsize,"z")
  gStyle.SetTitleSize(tsize,"z")
  gStyle.SetMarkerStyle(20)
  gStyle.SetMarkerSize(1.2)
  gStyle.SetHistLineWidth(2)
  gStyle.SetLineStyleString(2,"[12 12]")
  gStyle.SetEndErrorSize(0.)
  
  gStyle.SetTitleXOffset(1.4)
  gStyle.SetTitleYOffset(1.4)
  
  
  
  gStyle.SetOptFit(0)
  gStyle.SetPadTickX(1)
  gStyle.SetPadTickY(1)
  
def optParsing():
  from optparse import OptionParser
  parser = OptionParser()
  parser.add_option("--tag",dest="tag",help="Tag you want to give to the outputplots",default="Refit1_SiAndTRT")
  parser.add_option("--legends",dest="legends",help="Comma separated list for the legend",default="Refit1_SiAndTRT")
  parser.add_option("--inputFiles",dest="inputFiles",help="Input File name without tag or parameters. Comma separateted list",default="")
  parser.add_option("--outdir",dest="outdir",help="output directory to collect the plots",default="./outputmaps")
  (config, sys.argv[1:]) = parser.parse_args(sys.argv[1:])
  return config
  
def createCanvasPads():
  width = 8000
  height = 5000
  c1 = TCanvas("c1", "canvas1", width, height)
  c1.Divide(2,2)
  return c1

def getPlot(loc, fin, plot):
  print '###############'+loc+fin+plot
  f = TFile.Open(loc+fin)
  histo = f.Get(plot)
  print histo
  histo.SetDirectory(0)
  
  return histo

def printAtlas():
    
  
    return

def handlePalettes(hist,canvas):
    gPad.Update()
    
    hist.SetTitle(hist.GetName())

    stats = hist.FindObject("stats")
    if (stats):
      stats.SetX1NDC(0.831)
      stats.SetX2NDC(0.87)
      stats.SetY1NDC(0.8)
      stats.SetY2NDC(.9)
    palette = hist.FindObject("palette")
    palette.SetX1NDC(0.85)
    palette.SetX2NDC(0.90)
    palette.SetY1NDC(0.132)
    palette.SetY2NDC(0.951)
    axis = palette.GetAxis()
    axis.SetNoExponent(kTRUE)
    gPad.Modified()
    gPad.Update()
    

def printMap(histo_m,min_r,max_r,outname):
  set_plot_style()
  c_map = TCanvas()

  
  if "d0" in histo_m.GetName():
    histo_m.Scale(1000.)
    histo_m.GetZaxis().SetTitle("#deltad_{0} [#mum]")
    
  if "z0" in histo_m.GetName():
    histo_m.Scale(1000.)
    histo_m.GetZaxis().SetTitle("#deltaz_{0} [#mum]")

  histo_m.GetZaxis().SetRangeUser(min_r,max_r)
  histo_m.GetXaxis().SetTitleSize(0.05)
  histo_m.GetYaxis().SetTitleSize(0.05)
  histo_m.GetZaxis().SetTitleSize(0.05)

  histo_m.GetXaxis().SetLabelSize(0.04)
  histo_m.GetYaxis().SetLabelSize(0.04)
  histo_m.GetZaxis().SetLabelSize(0.04)
  
  histo_m.GetYaxis().SetTitleOffset(0.6)
  histo_m.GetZaxis().SetTitleOffset(0.85)
  
  
  histo_m.Draw("colz")
  handlePalettes(histo_m,c_map)

  text = TLatex()
  text.SetNDC()
  text.SetTextFont(42)
  text.SetTextSize(0.05)
  text.SetTextColor(kBlack)
  text.DrawLatex(0.15,0.85,"Data 2016")#, Period A-E")
  text.DrawLatex(0.15,0.79,"Z#rightarrow#mu#mu") #sqrt{s} = 13 TeV")
  text.SetTextSize(0.04)
  text.DrawLatex(0.15,0.73,"#sqrt{s} = 13 TeV, #intLdt = 74 pb^{-1}") #0.8 fb-1 for period A-E

  c_map.SaveAs(outname)


#def printProjection(histo_m,min_r,max_r,outname,oFext):
#  
#  c_proj = TCanvas()
#
#  if "d0" in histo_m.GetName():
#    d0.Scale(1000.)
#  histo_m->GetYaxis()->SetRangeUser(min_r,max_r)
#  
#  histo_m.SetMarkerStyle(20)
#  histo_m.SetMarkerSize(0.4)
#  histo_m.SetMarkerColor(kBlack)
#  
#  
#  text = TLatex()
#  text.SetNDC()
#  text.SetTextFont(42)
#  text.SetTextSize(0.05)
#  text.SetTextColor(kBlack)
#  text.DrawLatex(0.15,0.85,"Data 2016, Period A-E")
#  text.DrawLatex(0.15,0.79,"Z#rightarrow#mu#mu") #sqrt{s} = 13 TeV")
#  text.SetTextSize(0.04)
#  text.DrawLatex(0.15,0.73,"#sqrt{s} = 13 TeV, #intLdt = 0.8 fb^{-1}")
#  c_proj.SaveAs(outname+"_"+axis+oFext)


def printPlots(tag,f_files,outdir):
    set_plot_style()
    
    for f_file in f_files:
      
      outname_file = f_file.strip(".root")

      h2_d0_maps = getPlot("./",                f_file,                "d0CorrectionVsEtaPhi"                )
      h2_z0_maps = getPlot("./",                f_file,                "z0CorrectionVsEtaPhi"                )
      h2_lambda_maps = getPlot("./",            f_file,                    "LambdaCorrectionVsEtaPhi"        )
      h2_deltar_maps = getPlot("./",            f_file,                "DeltaPCorrectionVsEtaPhi"            )

      h2_d0_maps.Sumw2()
      h2_z0_maps.Sumw2()
      h2_lambda_maps.Sumw2()
      h2_deltar_maps.Sumw2()



      c1 = createCanvasPads()
      c1.cd(1)
      h2_d0_maps.GetZaxis().SetRangeUser(-.052,.052)
      h2_d0_maps.Draw("colz")
      handlePalettes(h2_d0_maps,c1)

      c1.cd(2)
      h2_z0_maps.GetZaxis().SetRangeUser(-.8,.8)
      h2_z0_maps.Draw("colz")
      handlePalettes(h2_z0_maps,c1)

      c1.cd(3)
      h2_lambda_maps.GetZaxis().SetRangeUser(-2.6,2.6)
      h2_lambda_maps.Draw("colz")
      handlePalettes(h2_lambda_maps,c1)

      c1.cd(4)
      h2_deltar_maps.GetZaxis().SetRangeUser(-.038,0.038)
      h2_deltar_maps.Draw("colz")
      handlePalettes(h2_deltar_maps,c1)
      gStyle.SetOptStat(0)


  #    c1.Update()

      fout = TFile.Open(outdir+"/correctionmaps_"+tag+outname_file+".root","RECREATE")
      h2_d0_maps.Write()
      h2_z0_maps.Write()
      h2_lambda_maps.Write()
      h2_deltar_maps.Write()
      fout.Close()

      c1.SaveAs(outdir+"/correctionmaps_"+tag+".pdf")



      c2 = createCanvasPads()


      c2.cd(1)
      h2_d0_maps.GetZaxis().SetRangeUser(-.0152,.0152)
      h2_d0_maps.GetZaxis().SetRangeUser(-.0052,.0052)
      h2_d0_maps.Draw("colz")
  #    handlePalettes(h2_d0_maps,c2)

      c2.cd(2)
      h2_z0_maps.GetZaxis().SetRangeUser(-.2,.2)
      h2_z0_maps.Draw("colz")
  #    handlePalettes(h2_z0_maps,c2)

      c2.cd(3)
      h2_lambda_maps.GetZaxis().SetRangeUser(-0.8,0.8)
      h2_lambda_maps.Draw("colz")
  #    handlePalettes(h2_lambda_maps,c2)

      c2.cd(4)
      h2_deltar_maps.GetZaxis().SetRangeUser(-.015,0.015)
      h2_deltar_maps.Draw("colz")
  #    handlePalettes(h2_deltar_maps,c2)
      gStyle.SetOptStat(0)

      c2.SaveAs(outdir+"/correctionmaps_zoom_"+tag+outname_file+".pdf")
  #    c1.Update()



      #Projection Eta
      #printProjection(h2_d0_maps,-12,12,"eta","outputmaps/d0_correction",".pdf")

      #Print the maps singularly

      printMap(h2_d0_maps,-12,12,outdir+"/d0_correction"+tag+outname_file+".pdf")
      printMap(h2_z0_maps,-200,200,outdir+"/z0_correction"+tag+outname_file+".pdf")
      printMap(h2_lambda_maps,-2,2,outdir+"/sagitta_correction"+tag+outname_file+".pdf")


def histo1DCosmetics(h1,min_r,max_r,counter):
  h1.Sumw2()
  h1.SetMarkerColor(colors[counter])
  h1.SetLineColor(colors[counter])
  h1.SetMarkerStyle(marker[counter])
    
  h1.GetXaxis().SetLabelSize(0.05)
  h1.GetXaxis().SetLabelOffset(0.01)
  h1.GetXaxis().SetTitleSize(0.05)
  h1.GetXaxis().SetTitleOffset(1.05)
  
  h1.GetYaxis().SetLabelSize(0.05)
  h1.GetYaxis().SetLabelOffset(0.01)
  h1.GetYaxis().SetTitleSize(0.05)
  h1.GetYaxis().SetTitleOffset(1.05)
  
  if "d0" in h1.GetName():
    h1.Scale(1000.)
    h1.GetYaxis().SetTitle("#deltad0 [#mum]")
  if "z0" in h1.GetName():
    h1.Scale(1000.)
    h1.GetYaxis().SetTitle("#deltaz0 [#mum]")
  if "Lambda" in h1.GetName():
    h1.GetYaxis().SetTitle("#delta_{sagitta} [TeV^{-1}]")
  if "DeltaP" in h1.GetName():
    h1.Scale(1000.)
    h1.GetYaxis().SetTitle("#deltar [#mum]")
                              
    
  h1.GetYaxis().SetRangeUser(min_r,max_r)  
    
  

def printProjectionLinearAvg(tag, name, f_files,outdir,min_r,max_r,legends,oFext,meanType):  #This function shouldn't be used since returns values of the mean and RMS that are not accurate. It is ok only for a first check. 
  


  bin = None
  if ("d0CorrectionVsEtaPhi" in name):
    bin =  TH1D(name+"bin","bin",100000000,-500,500)
  if ("z0CorrectionVsEtaPhi" in name):
    bin =  TH1D(name+"bin","bin",10000000,-.5,.5)
  if ("LambdaCorrectionVsEtaPhi"):
    bin =  TH1D(name+"bin","bin",10000000,-3.5,3.5)

  bin.Sumw2()
  num_bins=1
  h2_list = []
  h1_eta_list = []
  h1_phi_list = []
  
  for f_file in f_files:
    h2_list.append(getPlot("./",f_file,name))
    
  count=0
  for h2 in h2_list:
    count+=1
    graph1 = TH1D(h2.GetName()+str(count)+"proj_eta_mean",h2.GetName(),h2.GetXaxis().GetNbins(),h2.GetXaxis().GetXmin(),h2.GetXaxis().GetXmax())
    graph2 = TH1D(h2.GetName()+str(count)+"proj_eta_rms",h2.GetName(),h2.GetXaxis().GetNbins(),h2.GetXaxis().GetXmin(),h2.GetXaxis().GetXmax())
    graph3 = TH1D(h2.GetName()+str(count)+"proj_phi_mean",h2.GetName(),h2.GetYaxis().GetNbins(),h2.GetYaxis().GetXmin(),h2.GetYaxis().GetXmax())
    graph4 = TH1D(h2.GetName()+str(count)+"proj_phi_rms",h2.GetName(),h2.GetYaxis().GetNbins(),h2.GetYaxis().GetXmin(),h2.GetYaxis().GetXmax())

    for i in range(1,h2.GetNbinsX()+1):
      value_x = (h2.GetXaxis().GetBinLowEdge(i) + h2.GetXaxis().GetBinUpEdge(i+num_bins-1))/2
      sumContent = 0;
      sumWeights  = 0;
      sumErrors   = 0;
      for j in range(1,h2.GetNbinsY()+1):
        bin.Fill(h2.GetBinContent(i,j))
        sumContent += h2.GetBinContent(i,j) * (1./h2.GetBinError(i,j)) * (1./h2.GetBinError(i,j))
        sumWeights   += (1./h2.GetBinError(i,j)) * (1./h2.GetBinError(i,j))                                                                  
        sumErrors   += (h2.GetBinError(i,j)) * (h2.GetBinError(i,j))                                                                  
        
        
      
      if meanType == 0:
        graph1.SetBinContent(i,bin.GetMean());
        graph1.SetBinError(i,bin.GetMeanError());
        graph2.SetBinContent(i,bin.GetRMS());
        graph2.SetBinError(i,bin.GetRMSError())
      
      else:
        graph1.SetBinContent(i,sumContent/sumWeights);
        graph1.SetBinError(i,sqrt(sumWeights) / sumWeights);   #!#!
        graph2.SetBinContent(i,0);
        graph2.SetBinError(i,0)
      
      bin.Reset()
    h1_eta_list.append(graph1)

    
    for i in range(1,h2.GetNbinsX()+1):
      value_x = (h2.GetXaxis().GetBinLowEdge(i) + h2.GetXaxis().GetBinUpEdge(i+num_bins-1))/2
      sumContent = 0;
      sumWeights  = 0;
      sumErrors   = 0;
      for j in range(1,h2.GetNbinsY()+1):
        bin.Fill(h2.GetBinContent(j,i))
        sumContent += h2.GetBinContent(j,i) * (1./h2.GetBinError(j,i)) * (1./h2.GetBinError(j,i))
        sumWeights   += (1./h2.GetBinError(j,i)) * (1./h2.GetBinError(j,i))                                                                  
        sumErrors   += (h2.GetBinError(j,i)) * (h2.GetBinError(j,i))                                                                  
      
      if meanType == 0:
        graph3.SetBinContent(i,bin.GetMean());
        graph3.SetBinError(i,bin.GetMeanError());
        graph4.SetBinContent(i,bin.GetRMS());
        graph4.SetBinError(i,bin.GetRMSError())

      else:
        graph3.SetBinContent(i,sumContent/sumWeights);
        graph3.SetBinError(i,sqrt(sumWeights) / sumWeights );   #!#!
        graph4.SetBinContent(i,0);
        graph4.SetBinError(i,0)

        
      bin.Reset()
    h1_phi_list.append(graph3)
      
    #I have the 1D lists here. 
      
  PrintH1List(h1_eta_list,name,min_r,max_r,legends,"LinearAvg_eta",".pdf")
  PrintH1List(h1_phi_list,name,min_r,max_r,legends,"LinearAvg_phi",".pdf")


def PrintH1List(h1_list,name,min_r,max_r,legends,tag,oFext):
  
  counter=0
  
  can = TCanvas()
  can.SetMargin(0.12,0.1,0.15,0.05)
  print "##PrintH1List::DEBUG::h1_list lenght: ",len(h1_list)
  for h1 in h1_list:
    
    h1.Sumw2()
    
    h1.SetMarkerColor(colors[counter])
    h1.SetLineColor(colors[counter])
    h1.SetMarkerStyle(marker[counter])
    
    h1.GetXaxis().SetLabelSize(0.05)
    h1.GetXaxis().SetLabelOffset(0.01)
    h1.GetXaxis().SetTitleSize(0.05)
    h1.GetXaxis().SetTitleOffset(1.05)

    h1.GetYaxis().SetLabelSize(0.05)
    h1.GetYaxis().SetLabelOffset(0.01)
    h1.GetYaxis().SetTitleSize(0.05)
    h1.GetYaxis().SetTitleOffset(1.05)
    
    if "d0" in name:
      h1.Scale(1000.)
      h1.GetYaxis().SetTitle("#deltad0 [#mum]")
    if "z0" in name:
      h1.Scale(1000.)
      h1.GetYaxis().SetTitle("#deltaz0 [#mum]")
    if "Lambda" in name:
      h1.GetYaxis().SetTitle("#delta_{sagitta} [TeV^{-1}]")
    if "DeltaP" in name:
      h1.Scale(1000.)
      h1.GetYaxis().SetTitle("#deltar [#mum]")
                              
    
    h1.GetYaxis().SetRangeUser(min_r,max_r)  
    
    
    
    if (counter==0):
      h1.Draw()
    else:
      h1.Draw("same")
    counter+=1
    
  text = TLatex()
  text.SetNDC()
  text.SetTextFont(42)
  text.SetTextSize(0.05)
  text.SetTextColor(kBlack)
  text.DrawLatex(0.16,0.85,"Data 2016")#, Period A-E")
  text.DrawLatex(0.16,0.79,"Z#rightarrow#mu#mu") #sqrt{s} = 13 TeV")
  text.SetTextSize(0.04)
  text.DrawLatex(0.16,0.73,"#sqrt{s} = 13 TeV, #intLdt = 74 pb^{-1}") #0.8


  if len(legends)>0:
    print "legends= ",legends
    leg = TLegend(0.55,0.9,0.8,0.75)
    leg.SetBorderSize(0)
    leg.SetFillColor(0)
    
    for i_leg in xrange(len(legends)):
      print "Adding Entry ",legends[i_leg]
      leg.AddEntry(h1_list[i_leg],legends[i_leg],"lpf")
    leg.Draw()
  can.SaveAs(outdir+"/"+name+"_LinearAvg_"+tag+"_"+oFext)



def printProjection(tag,name,f_files,outdir,min_r,max_r,legends,oFext):
  
  
  h1_list = []


  print "file lenght", len(f_files)
  for f_file in f_files:
    h1_list.append(getPlot("./",f_file,name))
    
    
  can = TCanvas()
  # left,right,bottom,top
  can.SetMargin(0.12,0.1,0.15,0.05)
  
  counter=0

  print "projection list  = ",h1_list
  print "projections size = ",len(h1_list)
  
  for h1 in h1_list:
    
    h1.Sumw2()
    
    h1.SetMarkerColor(colors[counter])
    h1.SetLineColor(colors[counter])
    h1.SetMarkerStyle(marker[counter])
    
    h1.GetXaxis().SetLabelSize(0.05)
    h1.GetXaxis().SetLabelOffset(0.01)
    h1.GetXaxis().SetTitleSize(0.05)
    h1.GetXaxis().SetTitleOffset(1.05)

    h1.GetYaxis().SetLabelSize(0.05)
    h1.GetYaxis().SetLabelOffset(0.01)
    h1.GetYaxis().SetTitleSize(0.05)
    h1.GetYaxis().SetTitleOffset(1.05)
    
    if "d0" in name:
      h1.Scale(1000.)
      h1.GetYaxis().SetTitle("#deltad0 [#mum]")
    if "z0" in name:
      h1.Scale(1000.)
      h1.GetYaxis().SetTitle("#deltaz0 [#mum]")
    if "Lambda" in name:
      h1.GetYaxis().SetTitle("#delta_{sagitta} [TeV^{-1}]")
    if "DeltaP" in name:
      h1.Scale(1000.)
      h1.GetYaxis().SetTitle("#deltar [#mum]")
                              
    
    h1.GetYaxis().SetRangeUser(min_r,max_r)  
    
    
    
    if (counter==0):
      h1.Draw()
    else:
      h1.Draw("same")
    counter+=1
    
  text = TLatex()
  text.SetNDC()
  text.SetTextFont(42)
  text.SetTextSize(0.05)
  text.SetTextColor(kBlack)
  text.DrawLatex(0.16,0.85,"Data 2016")#, Period A-E")
  text.DrawLatex(0.16,0.79,"Z#rightarrow#mu#mu") #sqrt{s} = 13 TeV")
  text.SetTextSize(0.04)
  text.DrawLatex(0.16,0.73,"#sqrt{s} = 13 TeV, #intLdt = 74 pb^{-1}") #0.8


  if len(legends)>0:
    print "legends= ",legends
    leg = TLegend(0.55,0.9,0.8,0.75)
    leg.SetBorderSize(0)
    leg.SetFillColor(0)
    
    for i_leg in xrange(len(legends)):
      print "Adding Entry ",legends[i_leg]
      leg.AddEntry(h1_list[i_leg],legends[i_leg],"lpf")
    leg.Draw()
  can.SaveAs(outdir+"/"+name+"_"+tag+"_"+oFext)


def PrintTextOnCanvas():
  text = TLatex()
  text.SetNDC()
  text.SetTextFont(42)
  text.SetTextSize(0.05)
  text.SetTextColor(kBlack)
  text.DrawLatex(0.16,0.85,"Data 2016")#, Period A-E")
  text.DrawLatex(0.16,0.79,"Z#rightarrow#mu#mu") #sqrt{s} = 13 TeV")
  text.SetTextSize(0.04)
  text.DrawLatex(0.16,0.73,"#sqrt{s} = 13 TeV, #intLdt = 74 pb^{-1}") #0.8
  

def PrintLegend(legends,can,h1_list,x1,y1,x2,y2):

  if len(legends)>0:
    print "legends= ",legends
    leg = TLegend(x1,y1,x2,y2)
    leg.SetBorderSize(0)
    leg.SetFillColor(0)
    
    for i_leg in xrange(len(legends)):
      print "Adding Entry ",legends[i_leg]
      leg.AddEntry(h1_list[i_leg],legends[i_leg],"lpf")
    leg.Draw()

def DrawProjectionsOnCanvas(h1_list,can,min_r,max_r,legends):
  
  can.cd()
  counter=0
  for h1 in h1_list:
    counter+=1
    histo1DCosmetics(h1,min_r,max_r,counter)
    if (counter==0):
      h1.Draw()
    else:
      h1.Draw("same")
      for i in range(h1.GetNbinsX()):
        print h1.GetName(), h1.GetBinContent(i)
        
        
      
  PrintTextOnCanvas()
  PrintLegend(legends,can,h1_list,0.55,0.9,0.8,0.75)

def printProjectionEtaPhi(tag,name,f_file,outdir,min_r,max_r,legends,oFext):
  
  h2_list = []
  
  for f_file in f_files:
    h2_list.append(getPlot("./",f_file,name))
  
  #Eta projection

  can = TCanvas()
  can.SetMargin(0.12,0.1,0.15,0.05)
  counter = 0
  
  h1_eta_list = []
  h1_phi_list = []

  counter=0
  for h2 in h2_list:
    counter+=1
    h2.Sumw2()
    h1_eta = TH1F(h2.GetName()+"eta"+str(counter),h2.GetName()+"eta",h2.GetNbinsX(),h2.GetXaxis().GetXmin(),h2.GetXaxis().GetXmax())
    h1_eta.Sumw2()
    h1_phi = TH1F(h2.GetName()+"phi"+str(counter),h2.GetName()+"phi",h2.GetNbinsY(),h2.GetYaxis().GetXmin(),h2.GetYaxis().GetXmax())
    h1_phi.Sumw2()


#I assume the same binning.
    for i in range(1,h2.GetNbinsX()+1):
      mean_eta  = 0.
      error_eta = 0.
      
      for j in range(1,h2.GetNbinsY()+1):
        mean_eta += h2.GetBinContent(i,j) / (h2.GetBinError(i,j)*h2.GetBinError(i,j))
        error_eta+= (h2.GetBinError(i,j)*h2.GetBinError(i,j)) 
        
      h1_eta.SetBinContent(i,mean_eta / error_eta) 
      h1_eta.SetBinError(i,mean_eta*0.1)
      #print "eta",mean_eta, "pm" ,sqrt(error_eta)
      
      
    for i in range(1,h2.GetNbinsX()+1):
      mean_phi  = 0.
      error_phi = 0.
      
      for j in range(1,h2.GetNbinsY()+1):
        mean_phi += h2.GetBinContent(j,i) / float(h2.GetNbinsX())
        error_phi+= h2.GetBinError(j,i)*h2.GetBinError(i,j) / float(h2.GetNbinsY())
      

      h1_phi.SetBinContent(j,mean_phi)
      h1_phi.SetBinError(j,mean_phi*0.1)
      #print "phi",mean_phi, "pm" ,sqrt(error_eta)

    h1_eta_list.append(h1_eta)
    h1_phi_list.append(h1_phi)
    
  #Histograms Cosmetics
  
  can_eta = TCanvas()
  can_phi = TCanvas()
  
  DrawProjectionsOnCanvas(h1_eta_list,can_eta,min_r,max_r,legends)
  
  DrawProjectionsOnCanvas(h1_phi_list,can_phi,min_r,max_r,legends)
  can_eta.SaveAs(outdir+"/"+name+"_"+tag+"_projectionEta"+oFext)
  can_eta.SaveAs(outdir+"/"+name+"_"+tag+"_projectionPhi"+oFext)
  

def print1Dplot(tag,name,f_files,outdir,Nbins,min_r,max_r,legends,oFext):
  
  h2_list=[]
  can = TCanvas()
  can.SetMargin(0.12,0.1,0.15,0.05)
  for f_file in f_files:
    h2_list.append(getPlot("./",f_file,name))
    
  h1_list=[]
  
  counter=0
  for h2 in h2_list:
    counter+=1
    h2.Sumw2()
    if "d0" in h2.GetName() or "z0" in h2.GetName() or "DeltaP" in h2.GetName():
      h2.Scale(1000.)
    
    h = TH1F(h2.GetName()+"_1D_"+str(counter)+"_projection",h2.GetName()+"_1Dprojection",Nbins,min_r,max_r)

    
    for i in range(1,h2.GetXaxis().GetNbins()+1):
      for j in range(1,h2.GetYaxis().GetNbins()+1):
        h.Fill(h2.GetBinContent(i,j))
    h1_list.append(h)

  counter=0

  maxY = h1_list[0].GetMaximum()
  

  for h1 in h1_list:
    if h1.GetMaximum() > maxY:
      maxY=h1.GetMaximum()
  
  for h1 in h1_list:
    
    h1.Sumw2()
    
    h1.SetMarkerColor(colors[counter])
    h1.SetLineColor(colors[counter])
    h1.SetMarkerStyle(marker[counter])
    h1.SetMarkerSize(0)
    
    h1.GetXaxis().SetLabelSize(0.05)
    h1.GetXaxis().SetLabelOffset(0.01)
    h1.GetXaxis().SetTitleSize(0.05)
    h1.GetXaxis().SetTitleOffset(1.05)

    h1.GetYaxis().SetLabelSize(0.05)
    h1.GetYaxis().SetLabelOffset(0.01)
    h1.GetYaxis().SetTitleSize(0.05)
    h1.GetYaxis().SetTitleOffset(1.05)
    

    h1.GetXaxis().SetRangeUser(min_r,max_r)  
    h1.GetYaxis().SetRangeUser(0,maxY*1.4)

    if "d0" in name:
      h1.GetXaxis().SetTitle("#deltad0 [#mum]")
      h1.GetYaxis().SetTitle("Entries per "+str(h1.GetXaxis().GetBinWidth(1))+" #mum")
    if "z0" in name:
      h1.GetXaxis().SetTitle("#deltaz0 [#mum]")
      h1.GetYaxis().SetTitle("Entries per "+str(h1.GetXaxis().GetBinWidth(1))+" #mum")
    if "Lambda" in name:
      h1.GetXaxis().SetTitle("#delta_{sagitta} [TeV^{-1}]")
      h1.GetYaxis().SetTitle("Entries per "+str(h1.GetXaxis().GetBinWidth(1))+" TeV^{-1}")
    if "DeltaP" in name:
      h1.GetXaxis().SetTitle("#deltar [#mum]")
      h1.GetYaxis().SetTitle("Entries per "+str(h1.GetXaxis().GetBinWidth(1))+" #mum")
    
    
    if (counter==0):
      h1.Draw("hist")
    else:
      h1.Draw("hist same")
    counter+=1

  text = TLatex()
  text.SetNDC()
  text.SetTextFont(42)
  text.SetTextSize(0.05)
  text.SetTextColor(kBlack)
  text.DrawLatex(0.16,0.85,"Data 2016")#, Period A-E")
  text.DrawLatex(0.16,0.79,"Z#rightarrow#mu#mu") #sqrt{s} = 13 TeV")
  text.SetTextSize(0.04)
  text.DrawLatex(0.16,0.73,"#sqrt{s} = 13 TeV, #intLdt = 74 pb^{-1}")


  if len(legends)>0:
    leg_list=[]
    for i_leg in xrange(len(legends)):
      x1=0.55
      x2=0.8
      y1=0.9  - (i_leg)*0.13
      y2=0.85 - (i_leg)*0.13
      leg_list.append(TLegend(x1,y1,x2,y2))
      leg_list[i_leg].SetBorderSize(0)
      leg_list[i_leg].SetFillColor(0)
      leg_list[i_leg].AddEntry(h1_list[i_leg],legends[i_leg],"lpf")
      text2=TLatex()
      text2.SetTextSize(0.03)
      text2.SetNDC()
      text2.SetTextFont(42)
      text2.SetTextColor(kBlack)
      mean    = Decimal(Decimal(h1_list[i_leg].GetMean()).quantize(Decimal('.01'), rounding=ROUND_HALF_UP))
      meanErr = Decimal(Decimal(h1_list[i_leg].GetMeanError()).quantize(Decimal('.01'), rounding=ROUND_HALF_UP))
      RMS     = Decimal(Decimal(h1_list[i_leg].GetRMS()).quantize(Decimal('.01'), rounding=ROUND_HALF_UP))
      RMSErr  = Decimal(Decimal(h1_list[i_leg].GetRMSError()).quantize(Decimal('.01'), rounding=ROUND_HALF_UP))
      
      text2.DrawLatex(x1,y2-0.03,"Mean="+str(mean)+" #pm "+str(meanErr))
      text2.DrawLatex(x1,y2-0.07,"RMS ="+str(RMS)+ " #pm "+str(RMSErr))
      leg_list[i_leg].Draw()

  can.SaveAs(outdir+"/"+name+"_1D_"+tag+"_"+oFext)


  
if __name__ == "__main__":
  
    config=optParsing()
    tag = config.tag#"Refit1Params"
    legends = (config.legends).split(",")
    outdir = config.outdir
    f_files = (config.inputFiles).split(",")


    gStyle.SetOptTitle(0)
    gROOT.SetBatch()
    printPlots(tag,f_files,outdir)
    printProjection(tag,"d0CorrectionVsEta",f_files,outdir,-4,4,legends,".pdf")
    printProjection(tag,"z0CorrectionVsEta",f_files,outdir,-20,100,legends,".pdf")
    printProjection(tag,"LambdaCorrectionVsEta",f_files,outdir,-1,1,legends,".pdf")
    printProjection(tag,"DeltaPCorrectionVsEta",f_files,outdir,-20,20,legends,".pdf")


    printProjection(tag,"d0CorrectionVsPhi",f_files,outdir,-4,4,legends,".pdf")
    printProjection(tag,"z0CorrectionVsPhi",f_files,outdir,-20,100,legends,".pdf")
    printProjection(tag,"LambdaCorrectionVsPhi",f_files,outdir,-1,1,legends,".pdf")
    #printProjection(tag,"DeltaPCorrectionVsPhi",f_files,outdir,-20,20,legends,".pdf")

    printProjectionLinearAvg(tag, "d0CorrectionVsEtaPhi", f_files,outdir,-10,10,legends,".pdf",0)
    printProjectionLinearAvg(tag, "z0CorrectionVsEtaPhi", f_files,outdir,-20,100,legends,".pdf",0)
    printProjectionLinearAvg(tag, "LambdaCorrectionVsEtaPhi", f_files,outdir,-1,1,legends,".pdf",0)

    #printProjectionLinearAvg(tag+"weightmean", "d0CorrectionVsEtaPhi", f_files,outdir,-10,10,legends,".pdf",1)
    #printProjectionLinearAvg(tag+"weightmean", "z0CorrectionVsEtaPhi", f_files,outdir,-20,100,legends,".pdf",1)
    #printProjectionLinearAvg(tag+"weightmean", "LambdaCorrectionVsEtaPhi", f_files,outdir,-1,1,legends,".pdf",1)

    
    
    
    
    print1Dplot(tag,"d0CorrectionVsEtaPhi",f_files,outdir,100,-10,10,legends,".pdf")
    print1Dplot(tag,"z0CorrectionVsEtaPhi",f_files,outdir,100,-100,100,legends,".pdf")
    print1Dplot(tag,"LambdaCorrectionVsEtaPhi",f_files,outdir,100,-2,2,legends,".pdf")
    print1Dplot(tag,"DeltaPCorrectionVsEtaPhi",f_files,outdir,100,-40,40,legends,".pdf")
    
    
