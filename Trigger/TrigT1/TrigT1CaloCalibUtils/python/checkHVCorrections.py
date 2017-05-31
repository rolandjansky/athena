#!/bin/env python

#from ROOT import gRandom,TCanvas,TH1F,TH2F
import ROOT
import sys
import time
import os

from PlotCalibrationGains import *
from array import *

from PlotCalibrationHV import *

from PyCool import cool
from optparse import OptionParser

#from coolTools import *

from math import fabs

import numpy as np
import matplotlib.mlab as mlab
import matplotlib.pyplot as plt

plt.xkcd()

if __name__ == "__main__":

  print "Starting checkHVCorrections script"

  parser = OptionParser()
  
  parser.add_option("-f","--InputFile",action="store",type="string",dest="input_file_name",help="Name of input file")
  (options, args) = parser.parse_args()


  geometry_convertor = L1CaloGeometryConvertor()
  receiver_gains     = GainReader()
  geometry_convertor.LoadReceiverPPMMap()

  ROOT.gStyle.SetPalette(1)
  ROOT.gStyle.SetOptStat(111111)
  ROOT.gStyle.SetCanvasColor(10)
  
  c1 = ROOT.TCanvas('c1','Example',200,10,700,500)
  c2 = ROOT.TCanvas('c2','Example Partitions',200,10,700,500)
  c2.Divide(3,2)

# now define histograms
  h_gains_em  = L1CaloMap("Eta-phi map of EM gains","#eta bin","#phi bin")
  h_gains_had = L1CaloMap("Eta-phi map of HAD gains","#eta bin","#phi bin")

  h_gains_em_noHV  = L1CaloMap("Eta-phi map of EM gains without HV corrections","#eta bin","#phi bin")
  h_gains_had_noHV = L1CaloMap("Eta-phi map of HAD gains without HV corrections","#eta bin","#phi bin")

  h_gains_em_ratio  = L1CaloMap("ratio of EM gains with and without HV corrections","#eta bin","#phi bin")
  h_gains_had_ratio = L1CaloMap("ratio of HAD gains with and without HV corrections","#eta bin","#phi bin")

  h1D_gains_em_ratio  = EmPartitionPlots("ratio of EM gains with and without HV corrections",40,0.,2.5,"gain/gain(no HV)","N")
  h1D_gains_had_ratio = HadPartitionPlots("ratio of HAD gains with and without HV corrections",40,0.,2.5,"gain/gain(no HV)","N")

  h_corr_em  = L1CaloMap("Eta-phi map of mean HV corrections in EM layer","#eta bin","#phi bin")
  h_corr_had = L1CaloMap("Eta-phi map of mean HV corrections in HAD layer","#eta bin","#phi bin")

  h1D_corr_em  = EmPartitionPlots("mean HV corrections in EM layer",40,0.,2.5,"mean HV correction","N")
  h1D_corr_had = HadPartitionPlots("mean HV corrections in HAD layer",40,0.,2.5,"mean HV correction","N")

  h_corrHVdiff_em  = L1CaloMap("(ratio-meanHV)/meanHV in EM layer","#eta bin","#phi bin")
  h_corrHVdiff_had = L1CaloMap("(ratio-meanHV/meanHV in HAD layer","#eta bin","#phi bin")

  h1D_corrHVdiff_em  = EmPartitionPlots("(ratio-meanHV)/meanHV in EM layer",40,-0.1,0.1,"(ratio-meanHV)/meanHV","N")
  h1D_corrHVdiff_had = HadPartitionPlots("(ratio-meanHV)/meanHV in HAD layer",40,-0.1,0.1,"(ratio-meanHV)/meanHV","N")


# read in gains
  fileDefault = 'energyscanresults_321849.sqlite'
  fileNoHV    = 'energyscanresults_321849noHV.sqlite'

#  fileDefault = 'energyscanresults_321852.sqlite'
#  fileNoHV    = 'energyscanresults_321852noHV.sqlite'

  hvCorrFile = 'hvcorrections_3may17.sqlite'

  strategyString = 'GainOneOvEmecFcalLowEta'
#  strategyString = 'GainOneOvEmbFcalHighEta'

  receiver_gains.LoadGainsSqlite(fileDefault)
  receiver_gains.LoadReferenceSqlite(fileNoHV)

  hv_status = L1CaloHVReader(hvCorrFile)

#  print hv_status.GetMeanCorections()

  for i_eta in range(-49,45):
     for i_phi in range(0,64):
     
       coolEm  = geometry_convertor.getCoolEm(i_eta,i_phi)
       coolHad = geometry_convertor.getCoolHad(i_eta,i_phi)
       

       if not coolEm == '':                           # there is a channel for this eta-phi
       
         gain   = receiver_gains.getGain(coolEm)
         reference_gain = receiver_gains.getReferenceGain(coolEm)
         receiverEm  = geometry_convertor.getReceiverfromPPM(coolEm,strategyString)

#         print "COOL Id=", coolEm, " receiverId=", receiverEm,  " is overlap?" , geometry_convertor.isPPMOverlap(coolEm)

         try:
           meanEmHVCorrection = (hv_status.GetMeanCorections())[receiverEm]
         except KeyError:
           meanEmHVCorrection = 1.

         if (not gain == '') and (not reference_gain == ''):        # both  gains should be available
	 
           h_gains_em.Fill(i_eta,i_phi,gain)
           h_gains_em_noHV.Fill(i_eta,i_phi,reference_gain)
           h_corr_em.Fill(i_eta,i_phi,meanEmHVCorrection)
           h1D_corr_em.Fill(i_eta,meanEmHVCorrection)
           if reference_gain > 0.1 :
             if gain/reference_gain > 0.01:                            # to make histo look nicer
               h_gains_em_ratio.Fill(i_eta,i_phi,gain/reference_gain)
             h1D_gains_em_ratio.Fill(i_eta,gain/reference_gain)
             if meanEmHVCorrection > 0.1:
               relDifference = (gain/reference_gain - meanEmHVCorrection)/meanEmHVCorrection
               if relDifference > 0.01:                               # to make histo look nicer
                 h_corrHVdiff_em.Fill(i_eta,i_phi,relDifference)
               h1D_corrHVdiff_em.Fill(i_eta,relDifference)

       if not coolHad == '':                         # there is a channel for this eta-phi

         gain = receiver_gains.getGain(coolHad)
         reference_gain = receiver_gains.getReferenceGain(coolHad)
         receiverHad = geometry_convertor.getReceiverfromPPM(coolHad,strategyString)

         try:
           meanHadHVCorrection = (hv_status.GetMeanCorections())[receiverHad]
         except KeyError:
           meanHadHVCorrection = 1.

         if (not gain == '') and (not reference_gain == ''):       # both gains should be available

           h_gains_had.Fill(i_eta,i_phi,gain)
           h_gains_had_noHV.Fill(i_eta,i_phi,reference_gain)
           h_corr_had.Fill(i_eta,i_phi,meanHadHVCorrection) 
           h1D_corr_had.Fill(i_eta,meanHadHVCorrection)
           if reference_gain > 0.1 :
             if gain/reference_gain > 0.01:                            # to make histo look nicer
               h_gains_had_ratio.Fill(i_eta,i_phi,gain/reference_gain)
             h1D_gains_had_ratio.Fill(i_eta,gain/reference_gain)
             if meanHadHVCorrection > 0.1:
               relDifference = (gain/reference_gain - meanEmHVCorrection)/meanEmHVCorrection
               if relDifference > 0.01:                               # to make histo look nicer
                 h_corrHVdiff_had.Fill(i_eta,i_phi,relDifference)
               h1D_corrHVdiff_had.Fill(i_eta,relDifference)


  c1.cd()
  ROOT.gPad.SetLogy(0)

  h_gains_em.SetMinimum(0.6)
  h_gains_em.SetMaximum(1.4)
#  h_gains_em.SetMaximum(2.1)
  h_gains_em.Draw()
  c1.Print("checkHVCorrections.ps(")

  h_gains_had.SetMinimum(0.6)
  h_gains_had.SetMaximum(1.4)
  h_gains_had.Draw()
  c1.Print("checkHVCorrections.ps")

  h_gains_em_noHV.SetMinimum(0.6)
  h_gains_em_noHV.SetMaximum(1.4)
  h_gains_em_noHV.Draw()
  c1.Print("checkHVCorrections.ps")
  
  h_gains_had_noHV.SetMinimum(0.6)
  h_gains_had_noHV.SetMaximum(1.4)
  h_gains_had_noHV.Draw()
  c1.Print("checkHVCorrections.ps")

  h_gains_em_ratio.SetMinimum(1.05)
  h_gains_em_ratio.SetMaximum(2.2)
  h_gains_em_ratio.Draw()
  c1.Print("checkHVCorrections.ps")

  h_gains_had_ratio.SetMinimum(1.05)
  h_gains_had_ratio.SetMaximum(2.2)
  h_gains_had_ratio.Draw()
  c1.Print("checkHVCorrections.ps")

  h_corr_em.SetMinimum(1.05)
  h_corr_em.SetMaximum(2.2)
  h_corr_em.Draw()
  c1.Print("checkHVCorrections.ps")

  h_corr_had.SetMinimum(1.05)
  h_corr_had.SetMaximum(2.2)
  h_corr_had.Draw()
  c1.Print("checkHVCorrections.ps")

  h_corrHVdiff_em.SetMinimum(-.1)
  h_corrHVdiff_em.SetMaximum( .1)
  h_corrHVdiff_em.Draw()
  c1.Print("checkHVCorrections.ps")

  h_corrHVdiff_had.SetMinimum(-.1)
  h_corrHVdiff_had.SetMaximum( .1)
  h_corrHVdiff_had.Draw()
  c1.Print("checkHVCorrections.ps")

  c2.Clear()
  c2.Divide(3,2)
  for i_p in range(0,h1D_gains_em_ratio.nPartitions):
    c2.cd(i_p+1)
    if h1D_gains_em_ratio.his_partitions[i_p].GetEntries() > 0:
      ROOT.gPad.SetLogy()
    else:
      ROOT.gPad.SetLogy(0)
    h1D_gains_em_ratio.his_partitions[i_p].Draw()
  
  c2.Print("checkHVCorrections.ps")
  
  #c2.cd()
  c2.Clear()
  c2.Divide(3,2)
  for i_p in range(0,h1D_gains_had_ratio.nPartitions):
    c2.cd(i_p+1)
    if h1D_gains_had_ratio.his_partitions[i_p].GetEntries() > 0:
      ROOT.gPad.SetLogy()
    else:
      ROOT.gPad.SetLogy(0)
    h1D_gains_had_ratio.his_partitions[i_p].Draw()
  
  c2.Print("checkHVCorrections.ps")

  c2.Clear()
  c2.Divide(3,2)
  for i_p in range(0,h1D_corr_em.nPartitions):
    c2.cd(i_p+1)
    if h1D_corr_em.his_partitions[i_p].GetEntries() > 0:
      ROOT.gPad.SetLogy()
    else:
      ROOT.gPad.SetLogy(0)
    h1D_corr_em.his_partitions[i_p].Draw()
  
  c2.Print("checkHVCorrections.ps")
  
  #c2.cd()
  c2.Clear()
  c2.Divide(3,2)
  for i_p in range(0,h1D_corr_had.nPartitions):
    c2.cd(i_p+1)
    if h1D_corr_had.his_partitions[i_p].GetEntries() > 0:
      ROOT.gPad.SetLogy()
    else:
      ROOT.gPad.SetLogy(0)
    h1D_corr_had.his_partitions[i_p].Draw()
  
  c2.Print("checkHVCorrections.ps")
  c2.Clear()
  c2.Divide(3,2)
  for i_p in range(0,h1D_corrHVdiff_em.nPartitions):
    c2.cd(i_p+1)
    if h1D_corrHVdiff_em.his_partitions[i_p].GetEntries() > 0:
      ROOT.gPad.SetLogy()
    else:
      ROOT.gPad.SetLogy(0)
    h1D_corrHVdiff_em.his_partitions[i_p].Draw()
  
  c2.Print("checkHVCorrections.ps")
  
  #c2.cd()
  c2.Clear()
  c2.Divide(3,2)
  for i_p in range(0,h1D_corrHVdiff_had.nPartitions):
    c2.cd(i_p+1)
    if h1D_corrHVdiff_had.his_partitions[i_p].GetEntries() > 0:
      ROOT.gPad.SetLogy()
    else:
      ROOT.gPad.SetLogy(0)
    h1D_corrHVdiff_had.his_partitions[i_p].Draw()
  
  c2.Print("checkHVCorrections.ps)")

  os.system("ps2pdf checkHVCorrections.ps")



