#!/bin/env python
# Author: Michael Miller (miller@uchicago.edu)


import sys
import ROOT


def SetupDraw():

    ROOT.gROOT.SetStyle("Plain")
    ROOT.gStyle.SetCanvasBorderMode(0)
    ROOT.gStyle.SetPadBorderMode(0)
    ROOT.gStyle.SetTitleX(0.12)
    ROOT.gStyle.SetTitleY(1.0)
    ROOT.gStyle.SetStatX(0.84)
    ROOT.gStyle.SetStatY(0.84)
    ROOT.gStyle.SetLabelFont(42,"XYZ")
    ROOT.gStyle.SetTextFont(42)
    ROOT.gStyle.SetOptStat(111110)                                                                                                                                                   
    ROOT.gStyle.SetPalette(1)
    ROOT.gStyle.SetTitleFont(42,"XYZ")
    ROOT.gStyle.SetTitleBorderSize(0)
    ROOT.gStyle.SetPadColor(0)
    ROOT.gStyle.SetCanvasColor(0)
    ROOT.gStyle.SetOptFit(0)

    c1 = ROOT.TCanvas()
    c1.SetFrameBorderMode(0)
    c1.SetBorderSize(0)
    c1.SetBorderMode(0)
    c1.SetFillColor(0)
    c1.SetTickx()
    c1.SetTicky()

    return c1


def main():



   ROOT.gROOT.SetBatch()
   mb_names = ['MB 1', 'MB 2', 'MB 3', 'MB 4']
   hists = [ROOT.TH1F('MB%d' % mb, mb_names[mb], 55, 0., 255./2.+1) for mb in range(4)]   

   for hist in hists:
      hist.SetFillColor(ROOT.kGray)
      hist.GetXaxis().SetTitle('Current delay [ns]')
      hist.GetXaxis().CenterTitle()
      hist.GetYaxis().SetTitle('Number of mother boards')
      hist.GetYaxis().CenterTitle()
   infile=open("oks_mb_delays.txt", 'r')  
    


   for line in infile:
      a = line.strip().split(' ')
      for ind in range(4):
         hists[ind].Fill(float(a[ind+1])/2., 1.0)    
   
   cnt = 1
   can = SetupDraw() 
   can.Divide(2,2)
   for hist in hists:
       can.cd(cnt)    
       hist.Draw() 
       can.Update()
       can.Modified()
       cnt = cnt + 1        
   can.SaveAs('mb_attrs.ps')    



if __name__ == '__main__':
  main()
