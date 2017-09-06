#!/bin/env python
#
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

import ROOT
import sys
import time
import os

import PlotCalibrationGains

import PlotCalibrationHV

from PyCool import cool
from optparse import OptionParser

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


  geometry_convertor = PlotCalibrationGains.L1CaloGeometryConvertor()
  receiver_gains     = PlotCalibrationGains.GainReader()
  geometry_convertor.LoadReceiverPPMMap()

  ROOT.gStyle.SetPalette(1)
  ROOT.gStyle.SetOptStat(111111)
  ROOT.gStyle.SetCanvasColor(10)
  
  c1 = ROOT.TCanvas('c1','Example',200,10,700,500)
  c2 = ROOT.TCanvas('c2','Example Partitions',200,10,700,500)
  c2.Divide(3,2)

# now define histograms

  h_gains_em  = PlotCalibrationGains.L1CaloMap("Eta-phi map of EM gains","#eta bin","#phi bin")
  h_gains_had = PlotCalibrationGains.L1CaloMap("Eta-phi map of HAD gains","#eta bin","#phi bin")

  h_gains_em_noHV  = PlotCalibrationGains.L1CaloMap("Eta-phi map of EM gains without HV corrections","#eta bin","#phi bin")
  h_gains_had_noHV = PlotCalibrationGains.L1CaloMap("Eta-phi map of HAD gains without HV corrections","#eta bin","#phi bin")

  h_gains_em_ratio  = PlotCalibrationGains.L1CaloMap("ratio of EM gains with and without HV corrections","#eta bin","#phi bin")
  h_gains_had_ratio = PlotCalibrationGains.L1CaloMap("ratio of HAD gains with and without HV corrections","#eta bin","#phi bin")

  h1D_gains_em_ratio  = PlotCalibrationGains.EmPartitionPlots("ratio of EM gains with and without HV corrections",40,0.,2.5,"gain/gain(no HV)","N")
  h1D_gains_had_ratio = PlotCalibrationGains.HadPartitionPlots("ratio of HAD gains with and without HV corrections",40,0.,2.5,"gain/gain(no HV)","N")

  h_corr_em  = PlotCalibrationGains.L1CaloMap("Eta-phi map of mean HV corrections in EM layer","#eta bin","#phi bin")
  h_corr_had = PlotCalibrationGains.L1CaloMap("Eta-phi map of mean HV corrections in HAD layer","#eta bin","#phi bin")

  h_corrGainPredictor_em  = PlotCalibrationGains.L1CaloMap("Eta-phi map of HV corrections from Gain Predictor in EM layer","#eta bin","#phi bin")
  h_corrGainPredictor_had = PlotCalibrationGains.L1CaloMap("Eta-phi map of HV corrections from Gain Predictor in HAD layer","#eta bin","#phi bin")


  h1D_corr_em  = PlotCalibrationGains.EmPartitionPlots("mean HV corrections in EM layer",40,0.,2.5,"mean HV correction","N")
  h1D_corr_had = PlotCalibrationGains.HadPartitionPlots("mean HV corrections in HAD layer",40,0.,2.5,"mean HV correction","N")

  h_corrHVdiff_em  = PlotCalibrationGains.L1CaloMap("(ratio-meanHV)/meanHV in EM layer","#eta bin","#phi bin")
  h_corrHVdiff_had = PlotCalibrationGains.L1CaloMap("(ratio-meanHV)/meanHV in HAD layer","#eta bin","#phi bin")

  h1D_corrHVdiff_em  = PlotCalibrationGains.EmPartitionPlots("(ratio-meanHV)/meanHV in EM layer",40,-0.1,0.1,"(ratio-meanHV)/meanHV","N")
  h1D_corrHVdiff_had = PlotCalibrationGains.HadPartitionPlots("(ratio-meanHV)/meanHV in HAD layer",40,-0.1,0.1,"(ratio-meanHV)/meanHV","N")

  h_corrHVdiffGainPredictor_em  = PlotCalibrationGains.L1CaloMap("(ratio-GainPredictor)/GainPredictor in EM layer","#eta bin","#phi bin")
  h_corrHVdiffGainPredictor_had = PlotCalibrationGains.L1CaloMap("(ratio-GainPredictor)/GainPredictor in HAD layer","#eta bin","#phi bin")


# input files are define here

  fileDefault = '/afs/cern.ch/work/j/juraj/public/testarea/21.0.18/CalibrationProcessing/326189_HV/energyscanresults.sqlite'
  fileNoHV    = '/afs/cern.ch/work/j/juraj/public/testarea/21.0.18/CalibrationProcessing/326189/energyscanresults.sqlite'

  hvCorrFile = '/afs/cern.ch/work/j/juraj/public/testarea/20.7.8.3/HVDumps/hvcorrections_9jun17.sqlite'

  hvGainPredictor = '/afs/cern.ch/work/j/juraj/public/testarea/20.7.8.3/HVDumps/hvUpdate_9jun17_0p.txt'

  strategyString = 'GainOneOvEmecFcalLowEta'       # defines receiver to TT mapping for EMB/EMEC overlap and hadronic FCAL
#  strategyString = 'GainOneOvEmbFcalHighEta'


  receiver_gains.LoadGainsSqlite(fileDefault)
  receiver_gains.LoadReferenceSqlite(fileNoHV)

  hv_status = PlotCalibrationHV.L1CaloHVReader(hvCorrFile)

  strange_channel_file = open('checkHVCorrections.txt','w')

  gpReceiver,gpCool,gpEta,gpPhi,gpCorrection = np.genfromtxt(hvGainPredictor,unpack=True,dtype='str')

  gainPredictorCorrections = {}

  for iii in range(len(gpReceiver)):
     gainPredictorCorrections[gpReceiver[iii]] = float(gpCorrection[iii])

  for i_eta in range(-49,45):
     for i_phi in range(0,64):
     
       coolEm  = geometry_convertor.getCoolEm(i_eta,i_phi)
       coolHad = geometry_convertor.getCoolHad(i_eta,i_phi)
       

       if not coolEm == '':                           # there is a channel for this eta-phi
       
         gain   = receiver_gains.getGain(coolEm)
         reference_gain = receiver_gains.getReferenceGain(coolEm)
         receiverEm  = geometry_convertor.getReceiverfromPPM(coolEm,strategyString)

         try:
           meanEmHVCorrection = (hv_status.GetMeanCorections())[receiverEm]
         except KeyError:
           meanEmHVCorrection = 1.

         try:
           gainPredictorEmHVCorrection = gainPredictorCorrections[receiverEm]
         except KeyError:
           gainPredictorEmHVCorrection = 1.

         if (not gain == '') and (not reference_gain == ''):        # both  gains should be available
	 
           h_gains_em.Fill(i_eta,i_phi,gain)
           h_gains_em_noHV.Fill(i_eta,i_phi,reference_gain)
           h_corr_em.Fill(i_eta,i_phi,meanEmHVCorrection)
           h_corrGainPredictor_em.Fill(i_eta,i_phi,gainPredictorEmHVCorrection)

           h1D_corr_em.Fill(i_eta,meanEmHVCorrection)
           if reference_gain > 0.1 :
             if gain/reference_gain > 0.01:                            # to make histo look nicer
               h_gains_em_ratio.Fill(i_eta,i_phi,gain/reference_gain)
             h1D_gains_em_ratio.Fill(i_eta,gain/reference_gain)

             if gainPredictorEmHVCorrection > 0.00001:                    # just to avoid division by zero, usually >=1.
               relDifference = (gain/reference_gain - gainPredictorEmHVCorrection)/gainPredictorEmHVCorrection

               if relDifference > 0.001:                               # to make histo look nicer
                 h_corrHVdiffGainPredictor_em.Fill(i_eta,i_phi,relDifference)
               if relDifference > 0.05:                               #  print out interesting channels
                 strange_channel_file.write('%s %s %i %i EM  corr(RampMaker)= %.4f  corr(GainPredictor)= %.4f  diff= %.4f  \n' % (coolEm , receiverEm, \
                                             i_eta , i_phi, gain/reference_gain,gainPredictorEmHVCorrection,relDifference) )  

             if meanEmHVCorrection > 0.00001:                              # just to avoid division by zero, usually >=1.
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

         try:
           gainPredictorHadHVCorrection = gainPredictorCorrections[receiverHad]
         except KeyError:
           gainPredictorHadHVCorrection = 1.


         if (not gain == '') and (not reference_gain == ''):       # both gains should be available

           h_gains_had.Fill(i_eta,i_phi,gain)
           h_gains_had_noHV.Fill(i_eta,i_phi,reference_gain)
           h_corr_had.Fill(i_eta,i_phi,meanHadHVCorrection) 
           h_corrGainPredictor_had.Fill(i_eta,i_phi,gainPredictorHadHVCorrection) 

           h1D_corr_had.Fill(i_eta,meanHadHVCorrection)

           if reference_gain > 0.1 :
             if gain/reference_gain > 0.01:                            # to make histo look nicer
               h_gains_had_ratio.Fill(i_eta,i_phi,gain/reference_gain)
             h1D_gains_had_ratio.Fill(i_eta,gain/reference_gain)

             if gainPredictorHadHVCorrection > 0.00001:                      # to avoid accidental zeros
               relDifference = (gain/reference_gain - gainPredictorHadHVCorrection)/gainPredictorHadHVCorrection
               if relDifference > 0.001:                               # to make histo look nicer
                 h_corrHVdiffGainPredictor_had.Fill(i_eta,i_phi,relDifference)
               if relDifference > 0.05:                               # print interesting channels
                 strange_channel_file.write('%s %s %i %i HAD  corr(RampMaker)= %.4f  corr(GainPredictor)= %.4f  diff= %.4f  \n' % (coolHad,receiverHad, \
                                             i_eta , i_phi, gain/reference_gain,gainPredictorHadHVCorrection,relDifference) )  


             if meanHadHVCorrection > 0.00001:
               relDifference = (gain/reference_gain - meanHadHVCorrection)/meanHadHVCorrection
               if relDifference > 0.01:                               # to make histo look nicer
                 h_corrHVdiff_had.Fill(i_eta,i_phi,relDifference)
               h1D_corrHVdiff_had.Fill(i_eta,relDifference)


  c1.cd()
  ROOT.gPad.SetLogy(0)

  h_gains_em.SetMinimum(0.6)
  h_gains_em.SetMaximum(1.4)
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

  h_corrGainPredictor_em.SetMinimum(1.0)
  h_corrGainPredictor_em.SetMaximum(2.1)
  h_corrGainPredictor_em.Draw()
  c1.Print("checkHVCorrections.ps")

  h_corrGainPredictor_had.SetMinimum(1.0)
  h_corrGainPredictor_had.SetMaximum(2.1)
  h_corrGainPredictor_had.Draw()
  c1.Print("checkHVCorrections.ps")


  h_corrHVdiff_em.SetMinimum(-.1)
  h_corrHVdiff_em.SetMaximum( .1)
  h_corrHVdiff_em.Draw()
  c1.Print("checkHVCorrections.ps")

  h_corrHVdiff_had.SetMinimum(-.1)
  h_corrHVdiff_had.SetMaximum( .1)
  h_corrHVdiff_had.Draw()
  c1.Print("checkHVCorrections.ps")

  h_corrHVdiffGainPredictor_em.SetMinimum(-.5)
  h_corrHVdiffGainPredictor_em.SetMaximum( .5)
  h_corrHVdiffGainPredictor_em.Draw()
  c1.Print("checkHVCorrections.ps")

  h_corrHVdiffGainPredictor_had.SetMinimum(-.5)
  h_corrHVdiffGainPredictor_had.SetMaximum( .5)
  h_corrHVdiffGainPredictor_had.Draw()
  c1.Print("checkHVCorrections.ps")

  h_corrHVdiffGainPredictor_em.SetMinimum(-.1)
  h_corrHVdiffGainPredictor_em.SetMaximum( .1)
  h_corrHVdiffGainPredictor_em.Draw()
  c1.Print("checkHVCorrections.ps")

  h_corrHVdiffGainPredictor_had.SetMinimum(-.1)
  h_corrHVdiffGainPredictor_had.SetMaximum( .1)
  h_corrHVdiffGainPredictor_had.Draw()
  c1.Print("checkHVCorrections.ps")

  h_corrHVdiffGainPredictor_em.SetMinimum(-.05)
  h_corrHVdiffGainPredictor_em.SetMaximum( .05)
  h_corrHVdiffGainPredictor_em.Draw()
  c1.Print("checkHVCorrections.ps")

  h_corrHVdiffGainPredictor_had.SetMinimum(-.05)
  h_corrHVdiffGainPredictor_had.SetMaximum( .05)
  h_corrHVdiffGainPredictor_had.Draw()
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

  strange_channel_file.close()

