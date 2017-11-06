#!/bin/env python

#
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

#from ROOT import gRandom,TCanvas,TH1F,TH2F
import ROOT
import sys
import time
import os
#from ctypes import *
import struct 
from array import *

from PyCool import cool
from optparse import OptionParser

import PlotCalibrationGains
import PlotCalibrationHV

import mergeEnergyRamps

class HVCorrectionCalculator:

  def __init__(self, mapping):
    
    self.geometry_convertor = mapping

    self.layer_weights_em  = {} # per eta bin
    self.layer_weights_had = {} # per eta bin

    file_name = "HVcorrPhysicsWeights.txt"

    ### retrieve layer weights from .txt file
    
    try:
      
      file = open(file_name)
      
    except IOError:

      print "\ncould not find file: %s ....exiting\n" % file_name

      sys.exit()

    for line in file.readlines():

      parts = line.split()

      self.layer_weights_em [int(parts[0])] = [float(parts[1]),float(parts[2]),float(parts[3]),float(parts[4]),float(parts[5])]
      self.layer_weights_had[int(parts[0])] = [float(parts[6]),float(parts[7]),float(parts[8]),float(parts[9])]

    file.close()
    

  def GetCorrection(self, receiver, layer_corr, layer_names):

    coolid = self.geometry_convertor.getPPMfromReceiver(receiver)
    
    eta_bin = self.geometry_convertor.getEtaBin(coolid)
 
    hv_coef = 0. # new hv coefficient
    normalisation = 0.

    ### check if tower is  em

    if self.geometry_convertor.isCoolEm (coolid):

      is_overlap = self.geometry_convertor.isPPMOverlap(coolid)

      ### loop over layers

      for n in range(len(layer_names)):

        layer = layer_names[n]
        
        if PlotCalibrationHV.isEmPresampler(layer):

          hv_coef += layer_corr[n] * self.layer_weights_em [eta_bin][0]
          normalisation += self.layer_weights_em [eta_bin][0]
 
        if PlotCalibrationHV.isEmFront(layer,is_overlap):

          hv_coef += layer_corr[n] * self.layer_weights_em [eta_bin][1]
          normalisation += self.layer_weights_em [eta_bin][1]

        if PlotCalibrationHV.isEmMiddle(layer,is_overlap):

          hv_coef += layer_corr[n] * self.layer_weights_em [eta_bin][2]
          normalisation += self.layer_weights_em [eta_bin][2]

        if PlotCalibrationHV.isEmBack(layer,is_overlap):

          hv_coef += layer_corr[n] * self.layer_weights_em [eta_bin][3]
          normalisation += self.layer_weights_em [eta_bin][3]

        if PlotCalibrationHV.isEmOverlapBack(layer,is_overlap):

          hv_coef += layer_corr[n] * self.layer_weights_em [eta_bin][4]          
          normalisation += self.layer_weights_em [eta_bin][4]   

    ### check if tower is had
   
    if self.geometry_convertor.isCoolHad(coolid):

      regions = "EmbFcalHighEta" # (?)

      ### loop over layers

      for n in range(len(layer_names)):

        layer = layer_names[n]

        if PlotCalibrationHV.isHadFirstLayer(layer,regions):
        
          hv_coef += layer_corr[n] * self.layer_weights_had[eta_bin][0]
          normalisation +=  self.layer_weights_had[eta_bin][0]

        if PlotCalibrationHV.isHadSecondLayer(layer,regions):
            
          hv_coef += layer_corr[n] * self.layer_weights_had[eta_bin][1]
          normalisation += self.layer_weights_had[eta_bin][1]
          
        if PlotCalibrationHV.isHadThirdLayer(layer,regions):

          hv_coef += layer_corr[n] * self.layer_weights_had[eta_bin][2]
          normalisation += self.layer_weights_had[eta_bin][2]
          
        if PlotCalibrationHV.isHadFourthLayer(layer,regions):

          hv_coef += layer_corr[n] * self.layer_weights_had[eta_bin][3]
          normalisation += self.layer_weights_had[eta_bin][3]

    totalCorrection = hv_coef / normalisation        # correction is a weighted sum of layer corrections

    return totalCorrection

def writeHVToSqlite(name,input_dict):
  
  UNIX2COOL = 1000000000
  
  dbSvc = cool.DatabaseSvcFactory.databaseService()
  connectString = 'sqlite://;schema='+name+';dbname=L1CALO'

  print '\nrecreating database file:',name
  dbSvc.dropDatabase( connectString )
  db = dbSvc.createDatabase( connectString )

  spec = cool.RecordSpecification()
  spec.extend("factor", cool.StorageType.Float)
  spec.extend("status", cool.StorageType.UInt32 )
  folderSpec = cool.FolderSpecification(cool.FolderVersioning.SINGLE_VERSION, spec)

  now = int(time.time())

  since = now*UNIX2COOL
# since = 0
#  until = sys.maxint
  until = cool.ValidityKeyMax
  db.createFolderSet('/TRIGGER')
  db.createFolderSet('/TRIGGER/Receivers')
  db.createFolderSet('/TRIGGER/Receivers/Factors')

  folder_description = '<timeStamp>time</timeStamp><addrHeader><address_header service_type="71" clid="1238547719"/></addrHeader><typeName>CondAttrListCollection</typeName>'
  f = db.createFolder( "/TRIGGER/Receivers/Factors/HVCorrections", folderSpec, folder_description)

  print " Now creating sqlite file for ", len(input_dict.keys()), " channels"
  for i in input_dict.keys():
    data = cool.Record( spec )
    data['factor'] = input_dict[i][0]
    data['status'] = input_dict[i][1]
    f.storeObject(since,until, data, int(i,16) )

  db.closeDatabase()


if __name__ == "__main__":
   
  print " Starting script for calculating L1Calo HV corrections"

  ### configure options

  parser = OptionParser(add_help_option=False)
  
  parser.add_option("-i", "--hv_input", action = "store", type = "string", dest = "hv_input", default =             "") 
  parser.add_option("-t", "--hv_corr_diff", action = "store", type =  "float", dest = "hv_corr_diff", default =           0.01)
  parser.add_option("-c", "--channel_list", action = "store", type = "string", dest = "channel_list", default =             "")
  parser.add_option("-o", "--output_files", action = "store", type = "string", dest = "output_files", default = "doL1CaloHVCorrections")

  parser.add_option("--noFCAL", action = "store_true", dest = "noFCAL", default = False)
  parser.add_option("--noFCAL23",action = "store_true", dest = "noFCAL23", default = False)

  parser.add_option("-h", "--help", action = "store_true")

  (options, args) = parser.parse_args()

  if options.help: # print helpful info 
    
    print "\nusage:    python doL1CaloHVCorrections.py [options]"
    
    print "\noptions:\n"                  

    print "-i, --hv_input        hv input .sqlite file     (default:           '') *"
    print "-t, --hv_corr_diff    minimum abs. change in hv corrections    (default:         0.01)"
    print "-c, --channel_list    file containing an input channel list    (default:           '')"
    print "-o, --output_files    name assigned to all the output files    (default: doL1CaloHVCorrections)"
    print "--noFCAL              receiver channels in FCAL not considered"
    print "--noFCAL23            receiver channels in hadronic FCAL (FCAL23) not considered"
   

    print "\n[*] minimal requirements"

    print "\nexample:  python doL1CaloHVCorrections.py -i file_1 "

    print "\nnote: if no input channel list is given all the channels will be considered by default"

    print "\n(see https://twiki.cern.ch/twiki/bin/save/Atlas/LevelOneCaloGainPredictor for more info)\n"

    sys.exit()

  if options.hv_input == "" :
    print "\ntoo few input arguments given ....exiting , need at least sqlite file with input HV corrections\n"
        
    sys.exit()

# root-y stuff

  ROOT.gStyle.SetPalette(1)
  ROOT.gStyle.SetOptStat(111111)
  ROOT.gStyle.SetCanvasColor(10)

  c1 = ROOT.TCanvas('c1','Example',200,10,700,500)

  h_corrEmb_em  = PlotCalibrationGains.L1CaloMap("Calculated HV corrections for EM  (EMB in overlap) ","#eta bin","#phi bin")
  h_corrFcalLowEta_had = PlotCalibrationGains.L1CaloMap("Calculated HV corrections for HAD (FCAL low #eta)","#eta bin","#phi bin")

  h_corrEmec_em  = PlotCalibrationGains.L1CaloMap("Calculated HV corrections for EM overlap (EMEC)","#eta bin","#phi bin")
  h_corrFcalHighEta_had = PlotCalibrationGains.L1CaloMap("Calculated HV corrections for HAD FCAL (high #eta)","#eta bin","#phi bin")

    
  hv_input = PlotCalibrationHV.L1CaloHVReader(options.hv_input) 

  geometry_convertor = PlotCalibrationGains.L1CaloGeometryConvertor()
  geometry_convertor.LoadReceiverPPMMap()

  correctionCalculator = HVCorrectionCalculator(geometry_convertor)

  ### initialise output .txt file
 
  print "Creating output file", options.output_files+".txt"
  output_text = open(options.output_files+".txt","w") 


  # now loop over receivers, either those in the list, or all of them

  if options.channel_list != "": # from .txt file

    try:

      channel_list = open(options.channel_list)

    except IOError:

      print "\ncould not find file: %s ....exiting\n" % options.channel_list

      sys.exit()

    print "\nreading input channel list from:", options.channel_list
   
    receiver_list = []

    for channel in channel_list.readlines():

      parts = channel.split()
      receiver_list.append(parts[0])

    channel_list.close()
    
  else: # from oracle database

    print "\nreading input channel list from oracle database"
        
    receiver_list = geometry_convertor.receiver_to_ppm_map.keys()

  ### check if channel list is empty

  if not receiver_list:

    print "\ninput channel list is empty ....exiting\n"

    sys.exit()
    
  print "\nsearching %s channels for hv correction changes, at least one layer > %s" % (len(receiver_list), options.hv_corr_diff)

  ### loop over receivers

  calculatedCorrections = {}

  for receiver in receiver_list:
    
    if receiver not in hv_input.GetNLayers().keys():

      continue # skip this receiver (it's non-LAr)


    coolid = geometry_convertor.getPPMfromReceiver(receiver)
        
    eta_bin = geometry_convertor.getEtaBin(coolid)
    phi_bin = geometry_convertor.getPhiBin(coolid)

    if options.noFCAL and geometry_convertor.isPPMFCAL(coolid):
      continue # skip this receiver (it's non-LAr)

    if options.noFCAL23 and geometry_convertor.isPPMFCAL(coolid) and geometry_convertor.isCoolHad(coolid):
      continue # skip this receiver (it's non-LAr)

    ### retrieve num layers and layer names (from new or ref hv input)

    num_layers = (hv_input.GetNLayers())[receiver]

    layer_names = [-1,-1,-1,-1] # default layer names (see... Calorimeter/CaloIdentifier/CaloIdentifier/CaloCell_ID.h)
    
    if num_layers > 0:
      layer_names[0] = (hv_input.GetName1()[receiver])
    if num_layers > 1:
      layer_names[1] = (hv_input.GetName2()[receiver])
    if num_layers > 2:
      layer_names[2] = (hv_input.GetName3()[receiver])
    if num_layers > 3:
      layer_names[3] = (hv_input.GetName4()[receiver])
  
    ### retrieve hv corrections from new hv input

    layer_corr = [1.,1.,1.,1.] # default hv corrections

    if receiver in hv_input.GetMeanCorections().keys(): # if mean hv correction > 1
      
      if num_layers > 0:
        layer_corr[0] = (hv_input.GetCorLayer1()[receiver])
      if num_layers > 1:
        layer_corr[1] = (hv_input.GetCorLayer2()[receiver])
      if num_layers > 2:
        layer_corr[2] = (hv_input.GetCorLayer3()[receiver])
      if num_layers > 3:
        layer_corr[3] = (hv_input.GetCorLayer4()[receiver])
        
    ### check if the channel has overall HV correction larger than the threshold
    
    predictedCorrection = correctionCalculator.GetCorrection(receiver, layer_corr, layer_names)

    if abs(predictedCorrection - 1) <= options.hv_corr_diff:
      continue # skip this receiver, the correction is not high enough

    calculatedCorrections[receiver] = [predictedCorrection,0]
    print >> output_text, ("%5s %9s  %3i %2i  %.3f") % (receiver, coolid, eta_bin, phi_bin, predictedCorrection)


    if geometry_convertor.isCoolEm(coolid):
      if not geometry_convertor.isPPMOverlap(coolid):
        h_corrEmb_em.Fill(eta_bin,phi_bin,predictedCorrection)
      else:
        if geometry_convertor.getOverlapLayer(receiver)=='EMB':
          h_corrEmb_em.Fill(eta_bin,phi_bin,predictedCorrection)
        else:
          h_corrEmec_em.Fill(eta_bin,phi_bin,predictedCorrection)


    if geometry_convertor.isCoolHad(coolid):
      if not geometry_convertor.isPPMFCAL(coolid):
        h_corrFcalLowEta_had.Fill(eta_bin,phi_bin,predictedCorrection)
      else:
        if geometry_convertor.getFCAL23RecEta(receiver)=='HighEta':
          h_corrFcalHighEta_had.Fill(eta_bin,phi_bin,predictedCorrection)
        else: 
          h_corrFcalLowEta_had.Fill(eta_bin,phi_bin,predictedCorrection)


  writeHVToSqlite(options.output_files+".sqlite",calculatedCorrections)
  output_text.close()

  # Draw mean corrections

  h_corrEmb_em.SetMinimum(1.)
  h_corrEmb_em.SetMaximum(2.1)
  h_corrEmb_em.Draw()
  c1.Print(options.output_files+".ps(")

  h_corrFcalLowEta_had.SetMinimum(1.)
  h_corrFcalLowEta_had.SetMaximum(2.1)
  h_corrFcalLowEta_had.Draw()
  c1.Print(options.output_files+".ps")


  h_corrEmec_em.SetMinimum(1.)
  h_corrEmec_em.SetMaximum(2.1)
  h_corrEmec_em.Draw()
  c1.Print(options.output_files+".ps")

  h_corrFcalHighEta_had.SetMinimum(1.)
  h_corrFcalHighEta_had.SetMaximum(2.1)
  h_corrFcalHighEta_had.Draw()
  c1.Print(options.output_files+".ps)")

  os.system("ps2pdf " + options.output_files+".ps")
 # os.system("cp HVStatus.pdf /home/jb/public_web/tmp ")

 
  print "Done!"


