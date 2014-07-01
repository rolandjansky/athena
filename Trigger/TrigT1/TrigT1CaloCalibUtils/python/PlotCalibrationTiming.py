# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import *
import sys
import os
from optparse import OptionParser


def PlotCalibrationTiming(input_file_name=None):

  if input_file_name == None:
    print "No input file name, assuming output.root"
    input_file_name = "output.root"

  gStyle.SetPalette(1)
  gStyle.SetOptStat(0)
  gStyle.SetOptFit(0)
  gStyle.SetCanvasColor(10)
  gStyle.SetFrameFillColor(10)
  gStyle.SetStatY(0.85)
  gStyle.SetStatX(0.85)
 

  c1 = TCanvas('c1','Example',200,10,700,500)
  c1.Divide(8,9)

  graphs = TFile(input_file_name)
  graphs.cd('ADC/FineTime')
  key_list = gDirectory.GetListOfKeys()

  directory_name = None              # find name of directory with histos

  try:
    for iii in key_list:
      iii_str = str(iii)
      line = iii_str.split("\"")
      for name in line:
        if 'run#' in name:
          run_directory_name = name
      if run_directory_name == None:
        raise NameError
  except Exception, e:
    print "ERROR, the file doesn't contain run# directory!"
    sys.exit(1)


  print "Entering directory: ", run_directory_name
  gDirectory.cd(run_directory_name)


  nPlot = 0
  nPage = 0

  c1.Print("CalibrationTimingPlots.ps[")

  ppm_em_2d_profile_etaPhi_adc_fineTime.SetMinimum(-10.)
  ppm_em_2d_profile_etaPhi_adc_fineTime.SetMaximum(10.)
  ppm_em_2d_profile_etaPhi_adc_fineTime.Draw("colz")
  c1.Print("CalibrationTimingPlots.ps")


  ppm_had_2d_profile_etaPhi_adc_fineTime.SetMinimum(-10.)
  ppm_had_2d_profile_etaPhi_adc_fineTime.SetMaximum(10.)
  ppm_had_2d_profile_etaPhi_adc_fineTime.Draw("colz")
  c1.Print("CalibrationTimingPlots.ps")

# now produce histograms for each partition
  directory_content = gDirectory.GetListOfKeys()


  gStyle.SetOptStat(111111)
  partition_histos=[]
  for key in directory_content:
    print key," Class name= ", key.GetClassName(), " Name=", key.GetName()
    if key.GetClassName() == 'TDirectoryFile':
      print "OK, I will create new histogram for this partition!"
      print "Name of partition is: ", key.GetName()
      partition_histos.append(TH1F(key.GetName()+'_his','Timing distribution for partition: '+key.GetName(),100,-10.,10.))
      gDirectory.cd()
      gDirectory.cd("/ADC/FineTime/"+run_directory_name+"/"+key.GetName())
      
      list_of_histos = gDirectory.GetListOfKeys()
      for his_key in list_of_histos:
        #timing_value=his_key.ReadObj().GetMaximum()
        timing_value=his_key.ReadObj().GetBinContent(2)
        partition_histos[-1].Fill(timing_value)


  for iHisto in partition_histos:
    iHisto.Draw()
    c1.Print("CalibrationTimingPlots.ps")

  c1.Print("CalibrationTimingPlots.ps]")

  os.system("ps2pdf CalibrationTimingPlots.ps")

  print "Finished!"


if __name__ == "__main__":

  print "Starting PlotCalibrationTiming"

  parser = OptionParser()
  parser.add_option("-f","--InputFile",action="store",type="string",dest="input_file_name",help="Name of input file")
  (options, args) = parser.parse_args()

  PlotCalibrationTiming(options.input_file_name)




