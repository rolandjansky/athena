# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import *
import sys
import os
from optparse import OptionParser

def passesSelection(gain,offset):
    if  ((gain > 0.5 and gain <1.6) and (offset > -10 and offset < 10)):
        return True	   
    else:	   
        return False


def PlotRamps(input_file_name=None):

  if input_file_name == None:
    print "No input file name, assuming graphs.root"
    input_file_name = "graphs.root"

  gStyle.SetPalette(1)
  gStyle.SetOptStat(111111)
  gStyle.SetOptFit(0011)
  gStyle.SetCanvasColor(10)
  gStyle.SetFrameFillColor(10)
  gStyle.SetTitleFillColor(0)
  gStyle.SetTitleBorderSize(1)
  gStyle.SetStatBorderSize(1)
  gStyle.SetStatFontSize(0.075)
  gStyle.SetStatY(0.6)
  gStyle.SetStatX(1.)
 
  gStyle.SetTitleFontSize(0.075)
  
  gStyle.SetPadTopMargin(0.10)
  gStyle.SetPadBottomMargin(0.12)
  gStyle.SetPadRightMargin(0.12)
  gStyle.SetPadLeftMargin(0.12)
  gStyle.SetHatchesSpacing(4.0)

  c1 = TCanvas('c1','Example',200,10,700,500)
  c1.Divide(8,9)

  graphs = TFile(input_file_name)
  key_list = graphs.GetListOfKeys()

  nPlot = 0
  nPage = 0

  c1.Print("rampPlots.ps[")

  list_of_histos=[]

  for iii in key_list:
    iii_str = str(iii)
    line = iii_str.split(" ")
    histo= line[2]
    my_name = histo[2:-2]
    list_of_histos.append(my_name)
#    print my_name
    
#  list_of_histos.sort()

  list_of_histos_dec=[int(iii,16) for iii in list_of_histos]
  list_of_histos_dec.sort()
  list_of_histos = [hex(iii) for iii in list_of_histos_dec]

#  for iii in list_of_histos:
#    print iii

  page_ppm = list_of_histos[0][:-4]

  for my_name in list_of_histos:

    my_graph = gDirectory.Get(my_name)
    graph_ppm = my_name[:-4]
    
    if (nPlot % 64) == 0 or  (not graph_ppm == page_ppm):         # end of page
      if (nPage==0) and (nPlot==0):
        print "start plotting"
      else:	
        c1.cd(0)
        ppm_number=page_ppm[-1]
        ppm_crate =page_ppm[2:-2]        
        if len(ppm_crate) == 0:
          ppm_crate='0'
        title = "PP  " + ppm_crate + "   PPM " + ppm_number 
        ltit =  TPaveLabel(0.35,0.90,0.65,1.0,title)
        ltit.SetTextAlign(22);
        ltit.SetTextSize(0.70);
        ltit.SetFillStyle(0);
        ltit.SetBorderSize(0);
        ltit.Draw()     

#        print title, "  ", ppm_crate, " ", ppm_number 
        c1.Print("rampPlots.ps")
        c1.Clear()
        c1.Divide(8,9)            
        nPage = nPage+1
        page_ppm = graph_ppm
        nPlot=0
	
    c1.cd(nPlot % 64 +1+8)
    my_graph.SetMarkerStyle(21)
    my_graph.SetMarkerSize(0.75)  
    my_graph.SetLineWidth(1)

    my_graph.GetYaxis().SetRangeUser(0.,my_graph.GetXaxis().GetXmax())  
   
    my_graph.GetXaxis().SetTitleSize(0.04)
    my_graph.GetYaxis().SetTitleSize(0.04)
    my_graph.GetXaxis().SetTitleOffset(1.5)
    my_graph.GetYaxis().SetTitleOffset(1.5)
#    my_graph.GetXaxis().SetLabelSize(0.03)
#    my_graph.GetYaxis().SetLabelSize(0.03)
    my_graph.GetXaxis().SetLabelSize(0.04)
    my_graph.GetYaxis().SetLabelSize(0.04)

    my_graph.GetXaxis().SetLabelOffset(0.04)
    my_graph.GetYaxis().SetLabelOffset(0.015)
    my_graph.GetXaxis().SetTitle("L1Calo energies")
    my_graph.GetYaxis().SetTitle("Calo energies")

    function_list = my_graph.GetListOfFunctions()
    offset = function_list[0].GetParameter(0)
    slope  = function_list[0].GetParameter(1)
    chi2   = function_list[0].GetChisquare()
    if not passesSelection(slope,offset):
      gPad.SetFrameFillColor(kYellow)      

    my_graph.Draw("APL")
    nPlot = nPlot+1    
    

  c1.cd(0)        
  ppm_number=page_ppm[-1]
  ppm_crate =page_ppm[2:-2]        
  if len(ppm_crate) == 0:
    ppm_crate='0'
  title = "PP  " + ppm_crate + "   PPM " + ppm_number 
  ltit =  TPaveLabel(0.35,0.90,0.65,1.0,title)
  ltit.SetTextAlign(22);
  ltit.SetTextSize(0.70);
  ltit.SetFillStyle(0);
  ltit.SetBorderSize(0);
  ltit.Draw()     

  c1.Print("rampPlots.ps")

  c1.Print("rampPlots.ps]")
  os.system("ps2pdf rampPlots.ps")
  print "Finished!"

if __name__ == "__main__":

  print "Starting PlotRamps"

  parser = OptionParser()
  parser.add_option("-f","--InputFile",action="store",type="string",dest="input_file_name",help="Name of input file")
  (options, args) = parser.parse_args()

  PlotRamps(options.input_file_name)
