# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from optparse import OptionParser

import PlotCalibrationHV

import PlotCalibrationGains

from PyCool import cool

import time

import sys

import mergeEnergyRamps

import ROOT

import os


class OracleGainReader:

  def __init__(self):

    self.reference_gains={}
    self.UNIX2COOL = 1000000000

    # get database service and open database
    dbSvc = cool.DatabaseSvcFactory.databaseService()

    dbString = 'oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TRIGGER;dbname=COMP200'
    try:
      db = dbSvc.openDatabase(dbString, False)        
    except Exception, e:
      print 'Error: Problem opening database', e
      sys.exit(1)

    folder_name = "/TRIGGER/Receivers/Factors/CalibGains"
    folder=db.getFolder(folder_name)
    ch = folder.listChannels()
       
    startUtime = int(time.time())
    endUtime = int(time.time())
    startValKey = startUtime * self.UNIX2COOL
    endValKey = endUtime * self.UNIX2COOL
    chsel = cool.ChannelSelection(0,sys.maxint)

    try:
      itr=folder.browseObjects(startValKey, endValKey, chsel)
    except Exception, e:
      print e
      sys.exit(1)

    for row in itr:
      ReceiverId = hex(int(row.channelId()))
      payload = row.payload()
      gain = payload['factor']
      self.reference_gains[ReceiverId]=gain

    # close database
    db.closeDatabase()


  def getGain(self,ReceiverId):
           
    if (ReceiverId in self.reference_gains):
      return float(self.reference_gains[ReceiverId])
    else:
      return ''


class GainPredictor:

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
    

  def GetGain(self, receiver, orig_gain, layer_corr_ref, layer_corr_new, layer_names):

    coolid = self.geometry_convertor.getPPMfromReceiver(receiver)
    
    eta_bin = self.geometry_convertor.getEtaBin(coolid)
 
    hv_coef_ref = 0. # ref hv coefficient
    hv_coef_new = 0. # new hv coefficient

    ### check if tower is  em

    if self.geometry_convertor.isCoolEm (coolid):

      is_overlap = self.geometry_convertor.isPPMOverlap(coolid)

      ### loop over layers

      for n in range(len(layer_names)):

        layer = layer_names[n]
        
        if PlotCalibrationHV.isEmPresampler(layer):

          hv_coef_ref += layer_corr_ref[n] * self.layer_weights_em [eta_bin][0]
          hv_coef_new += layer_corr_new[n] * self.layer_weights_em [eta_bin][0]

        if PlotCalibrationHV.isEmFront(layer,is_overlap):

          hv_coef_ref += layer_corr_ref[n] * self.layer_weights_em [eta_bin][1]
          hv_coef_new += layer_corr_new[n] * self.layer_weights_em [eta_bin][1]

        if PlotCalibrationHV.isEmMiddle(layer,is_overlap):

          hv_coef_ref += layer_corr_ref[n] * self.layer_weights_em [eta_bin][2]
          hv_coef_new += layer_corr_new[n] * self.layer_weights_em [eta_bin][2]

        if PlotCalibrationHV.isEmBack(layer,is_overlap):

          hv_coef_ref += layer_corr_ref[n] * self.layer_weights_em [eta_bin][3]
          hv_coef_new += layer_corr_new[n] * self.layer_weights_em [eta_bin][3]

        if PlotCalibrationHV.isEmOverlapBack(layer,is_overlap):

          hv_coef_ref += layer_corr_ref[n] * self.layer_weights_em [eta_bin][4]
          hv_coef_new += layer_corr_new[n] * self.layer_weights_em [eta_bin][4]          

    ### check if tower is had
   
    if self.geometry_convertor.isCoolHad(coolid):

      regions = "EmbFcalHighEta" # (?)

      ### loop over layers

      for n in range(len(layer_names)):

        layer = layer_names[n]

        if PlotCalibrationHV.isHadFirstLayer(layer,regions):
        
          hv_coef_ref += layer_corr_ref[n] * self.layer_weights_had[eta_bin][0]
          hv_coef_new += layer_corr_new[n] * self.layer_weights_had[eta_bin][0]

        if PlotCalibrationHV.isHadSecondLayer(layer,regions):
            
          hv_coef_ref += layer_corr_ref[n] * self.layer_weights_had[eta_bin][1]
          hv_coef_new += layer_corr_new[n] * self.layer_weights_had[eta_bin][1]
          
        if PlotCalibrationHV.isHadThirdLayer(layer,regions):

          hv_coef_ref += layer_corr_ref[n] * self.layer_weights_had[eta_bin][2]
          hv_coef_new += layer_corr_new[n] * self.layer_weights_had[eta_bin][2]
          
        if PlotCalibrationHV.isHadFourthLayer(layer,regions):

          hv_coef_ref += layer_corr_ref[n] * self.layer_weights_had[eta_bin][3]
          hv_coef_new += layer_corr_new[n] * self.layer_weights_had[eta_bin][3]

    pred_gain = orig_gain * (hv_coef_new/hv_coef_ref) # predicted gain

    return pred_gain


if __name__ == "__main__":
   
  ### configure options

  parser = OptionParser(add_help_option=False)
  
  parser.add_option("-n", "--hv_input_new", action = "store", type = "string", dest = "hv_input_new", default =             "") 
  parser.add_option("-r", "--hv_input_ref", action = "store", type = "string", dest = "hv_input_ref", default =             "") 
  parser.add_option("-t", "--hv_corr_diff", action = "store", type =  "float", dest = "hv_corr_diff", default =           0.01)
  parser.add_option("-c", "--channel_list", action = "store", type = "string", dest = "channel_list", default =             "")
  parser.add_option("-o", "--output_files", action = "store", type = "string", dest = "output_files", default = "gain_updates")

  parser.add_option("-h", "--help", action = "store_true")

  (options, args) = parser.parse_args()

  if options.help: # print helpful info 
    
    print "\nusage:    python LArHVGainsPredictor.py [options]"
    
    print "\noptions:\n"                  

    print "-n, --hv_input_new    new hv input .sqlite file (see below)    (default:           '') *"
    print "-r, --hv_input_ref    ref hv input .sqlite file (see below)    (default:           '') *"
    print "-t, --hv_corr_diff    minimum abs. change in hv corrections    (default:         0.01)"
    print "-c, --channel_list    file containing an input channel list    (default:           '')"
    print "-o, --output_files    name assigned to all the output files    (default: gains_update)"

    print "\n[*] minimal requirements"

    print "\nexample:  python LArHVGainsPredictor.py -n file_1 -r file_2"

    print "\nnote: if no input channel list is given all the channels will be considered by default"

    print "\n(see https://twiki.cern.ch/twiki/bin/save/Atlas/LevelOneCaloGainPredictor for more info)\n"

    sys.exit()

  error_code = 2 # which error code (?)

  ### retrieve hv correction info

  if options.hv_input_new == "" \
  or options.hv_input_ref == "":

    print "\ntoo few input arguments given ....exiting  (see python LArHVGainsPredictor.py -h for more info)\n"
        
    sys.exit()
    
  hv_input_new = PlotCalibrationHV.L1CaloHVReader(options.hv_input_new) 

  hv_input_ref = PlotCalibrationHV.L1CaloHVReader(options.hv_input_ref)

  print "\nreading hv input: %s and hv reference: %s" % (options.hv_input_new, options.hv_input_ref)
  
  ### create instance of geometry convertor and load receiver-PPM map
  
  geometry_convertor = PlotCalibrationGains.L1CaloGeometryConvertor()

  geometry_convertor.LoadReceiverPPMMap()

  ### create instance of gain predictor
  
  gain_predictor = GainPredictor(geometry_convertor)

  ### initialise histograms

  h_orig_gains_em  = PlotCalibrationGains.L1CaloMap("Em:  orig. gains"," #eta bin","#phi bin")
  h_orig_gains_had = PlotCalibrationGains.L1CaloMap("Had: orig. gains"," #eta bin","#phi bin")
  
  h_pred_gains_em  = PlotCalibrationGains.L1CaloMap("Em:  pred. gains"," #eta bin","#phi bin")
  h_pred_gains_had = PlotCalibrationGains.L1CaloMap("Had: pred. gains"," #eta bin","#phi bin")
  
  h_diff_gains_em  = PlotCalibrationGains.L1CaloMap("Em:  (pred. gain - orig. gain) / orig. gain","#eta bin","#phi bin")
  h_diff_gains_had = PlotCalibrationGains.L1CaloMap("Had: (pred. gain - orig. gain) / orig. gain","#eta bin","#phi bin")

  ### initialise output .txt file
  
  output_text = open(options.output_files+".txt","w") 

  ### initialise empty dictionary for predicted gains
  
  pred_gains = {}

  ### retrieve channel list (with the original gains)

  if options.channel_list != "": # from .txt file

    try:

      channel_list = open(options.channel_list)

    except IOError:

      print "\ncould not find file: %s ....exiting\n" % options.channel_list

      sys.exit()

    print "\nreading input channel list from:", options.channel_list
   
    orig_gains = {}

    for channel in channel_list.readlines():

      parts = channel.split()
    
      orig_gains[parts[0]] = float(parts[1])

    channel_list.close()
    
    receiver_list = orig_gains.keys()

  else: # from oracle database

    print "\nreading input channel list from oracle database"
    
    orig_gains = OracleGainReader()
    
    receiver_list = geometry_convertor.receiver_to_ppm_map.keys()

  ### check if channel list is empty

  if not receiver_list:

    print "\ninput channel list is empty ....exiting\n"

    sys.exit()
    
  print "\nsearching %s channels for hv correction changes > %s" % (len(receiver_list), options.hv_corr_diff)

  ### loop over receivers
    
  print "\npreparing gain updates for the following list of channels:"
    
  for receiver in receiver_list:
    
    if receiver not in hv_input_ref.GetNLayers().keys():

      continue # skip this receiver (it's non-LAr)

    coolid = geometry_convertor.getPPMfromReceiver(receiver)
        
    eta_bin = geometry_convertor.getEtaBin(coolid)
    phi_bin = geometry_convertor.getPhiBin(coolid)
    
    ### retrieve num layers and layer names (from new or ref hv input)

    num_layers = (hv_input_ref.GetNLayers())[receiver]

    layer_names = [-1,-1,-1,-1] # default layer names (see... Calorimeter/CaloIdentifier/CaloIdentifier/CaloCell_ID.h)
    
    if num_layers > 0:
      layer_names[0] = (hv_input_ref.GetName1()[receiver])
    if num_layers > 1:
      layer_names[1] = (hv_input_ref.GetName2()[receiver])
    if num_layers > 2:
      layer_names[2] = (hv_input_ref.GetName3()[receiver])
    if num_layers > 3:
      layer_names[3] = (hv_input_ref.GetName4()[receiver])
  
    ### retrieve hv corrections from new hv input

    layer_corr_new = [1.,1.,1.,1.] # default hv corrections

    if receiver in hv_input_new.GetMeanCorections().keys(): # if mean hv correction > 1
      
      if num_layers > 0:
        layer_corr_new[0] = (hv_input_new.GetCorLayer1()[receiver])
      if num_layers > 1:
        layer_corr_new[1] = (hv_input_new.GetCorLayer2()[receiver])
      if num_layers > 2:
        layer_corr_new[2] = (hv_input_new.GetCorLayer3()[receiver])
      if num_layers > 3:
        layer_corr_new[3] = (hv_input_new.GetCorLayer4()[receiver])
        
    ### retrieve hv corrections from ref hv input

    layer_corr_ref = [1.,1.,1.,1.] # default hv corrections

    if receiver in hv_input_ref.GetMeanCorections().keys(): # if mean hv correction > 1
      
      if num_layers > 0:
        layer_corr_ref[0] = (hv_input_ref.GetCorLayer1()[receiver])
      if num_layers > 1:
        layer_corr_ref[1] = (hv_input_ref.GetCorLayer2()[receiver])
      if num_layers > 2:
        layer_corr_ref[2] = (hv_input_ref.GetCorLayer3()[receiver])
      if num_layers > 3:
        layer_corr_ref[3] = (hv_input_ref.GetCorLayer4()[receiver])
        
    ### check if change in hv correction (in any layer) is > the threshold

    if  abs(layer_corr_new[0] - layer_corr_ref[0]) <= options.hv_corr_diff \
    and abs(layer_corr_new[1] - layer_corr_ref[1]) <= options.hv_corr_diff \
    and abs(layer_corr_new[2] - layer_corr_ref[2]) <= options.hv_corr_diff \
    and abs(layer_corr_new[3] - layer_corr_ref[3]) <= options.hv_corr_diff:

      continue # skip this receiver
     
    ### retrieve current gain for this receiver
      
    if options.channel_list != "": # from .txt file
      
      orig_gain = orig_gains[receiver]

    else: # from oracle database
        
      orig_gain = orig_gains.getGain(receiver)
        
    ### calculate new gain for this receiver

    pred_gain = gain_predictor.GetGain(receiver,orig_gain,layer_corr_ref,layer_corr_new,layer_names)

    if not pred_gains:
      
      print "" # just some formatting
    
    pred_gains[receiver] = [pred_gain, error_code]
      
    diff_gain = ((pred_gain - orig_gain) / orig_gain) * 100.0 # % change in gain

    ### fill histograms
      
    if geometry_convertor.isCoolEm (coolid):

      layer =  "EM"
  
      h_orig_gains_em. Fill(eta_bin,phi_bin,orig_gain)
      h_pred_gains_em. Fill(eta_bin,phi_bin,pred_gain)
      h_diff_gains_em. Fill(eta_bin,phi_bin,diff_gain)
          
    if geometry_convertor.isCoolHad(coolid):
      
      layer = "HAD" 
      
      h_orig_gains_had.Fill(eta_bin,phi_bin,orig_gain)
      h_pred_gains_had.Fill(eta_bin,phi_bin,pred_gain)
      h_diff_gains_had.Fill(eta_bin,phi_bin,diff_gain)
      
    ### print output to screen

    print ("%9s   %3s   %3i %2i   %6s   %.3f   %.3f   %6.1f %%   %1i") % (coolid, layer, eta_bin, phi_bin, receiver, orig_gain, pred_gain, diff_gain, num_layers),

    print ("  [%s]") % ", ".join("%.3f" % x for x in layer_corr_ref),
    
    print ("  [%s]") % ", ".join("%.3f" % x for x in layer_corr_new),

    print ("  [%s]") % ", ".join( "%2i" % x for x in layer_names)

    ### write output to text file
    
    print >> output_text, ("%6s   %.3f   %s") % (receiver, pred_gain, " ".join("%.3f" % x for x in layer_corr_new)),

    print >> output_text, ("  #  "),

    print >> output_text, ("%9s   %3i %2i   %3s   %.3f") % (coolid, eta_bin, phi_bin, layer, orig_gain), 

    print >> output_text, ("  %s") % " ".join("%6.3f" % (layer_corr_new[x] - layer_corr_ref[x]) for x in range (4))

  output_text.close()

  ### write output to .sqlite file

  mergeEnergyRamps.WriteSqlite(options.output_files+".sqlite",pred_gains)

  ### write output to .ps/pdf file  
  
  print "\nrecreating text and ps/pdf files: %s and %s" % (options.output_files+".txt", options.output_files+".ps/pdf")

  canvas = ROOT.TCanvas("canvas","",200,10,700,500)

  ROOT.gStyle.SetPalette(1)
  ROOT.gStyle.SetOptStat(111111)
  ROOT.gStyle.SetCanvasColor(10)

  canvas.Print(options.output_files+".ps[")
  
  h_orig_gains_em.Draw()
  canvas.Print(options.output_files+".ps")

  h_orig_gains_had.Draw()
  canvas.Print(options.output_files+".ps")
  
  h_pred_gains_em.Draw()
  canvas.Print(options.output_files+".ps")  
   
  h_pred_gains_had.Draw()
  canvas.Print(options.output_files+".ps")  

  h_diff_gains_em.Draw()
  canvas.Print(options.output_files+".ps")
 
  h_diff_gains_had.Draw()
  canvas.Print(options.output_files+".ps")

  canvas.Print(options.output_files+".ps]")
  
  os.system("ps2pdf "+options.output_files+".ps")
  
  print "\nfinished.... need a drink now!\n"
