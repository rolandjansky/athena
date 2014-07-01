#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

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

from PlotCalibrationGains import *


def isEmPresampler(layerName):

# layer name 0 in EMB and overlap, name 4 in EMEC
 
    if layerName == 0 or layerName == 4:
      return True
    else:
      return False


def isEmFront(layerName,isOverlap):

# layer name 1 in  EMB and overlap, name 5 in EMEC (name 5 in overlap goes with back layer)
# EM overlap has layer names 0, 1, 2 (EMB) and 5,6 (EMEC)

    isAccepted = False
    
    if isOverlap:             
      if layerName == 1:
        isAccepted = True
    else:
      if layerName == 1 or layerName == 5:
        isAccepted = True	
	    
    return isAccepted

def isEmMiddle(layerName,isOverlap):

# layer name 2 in EMB and overlap, name 6 in EMEC and 21 in FCAL0 (name 6 in overlap goes alone)
    isAccepted = False
    
    if isOverlap:             
      if layerName == 2:
        isAccepted = True
    else:
      if layerName == 2 or layerName == 6 or layerName == 21:
        isAccepted = True	
	    
    return isAccepted


def isEmBack(layerName,isOverlap):

# layer number 3 in EMB and overlap, layer number 7 in EMEC, layer nr. 5 in overlap
   isAccepted = False
    
   if isOverlap:
     if layerName == 5:
       isAccepted = True
   else:
     if layerName == 3 or layerName == 7:
       isAccepted = True	
	    
   return isAccepted


def isEmOverlapBack(layerName,isOverlap):

#  layer number 6 in EM overlap, this one doesn't fit anywhere else...
   isAccepted = False
    
   if isOverlap and layerName == 6:
     isAccepted = True	
	    
   return isAccepted


def isHadFirstLayer(layerName,TT_part):

#  layer number 8 in HEC, low eta part of 22 (FCAL2)
   isAccepted = False
   if layerName == 8 or (layerName == 22 and TT_part ==  'EmecFcalLowEta'):
     isAccepted = True	
	    
   return isAccepted

def isHadSecondLayer(layerName,TT_part):

#  layer number 9 in HEC, high eta part of 22 (FCAL2)
   isAccepted = False
   if layerName == 9 or (layerName == 22 and TT_part ==  'EmbFcalHighEta'):
     isAccepted = True	
	    
   return isAccepted

def isHadThirdLayer(layerName,TT_part):

#  layer number 10 in HEC, low eta part of 23 (FCAL3)
   isAccepted = False
   if layerName == 10 or (layerName == 23 and TT_part ==  'EmecFcalLowEta'):
     isAccepted = True	
	    
   return isAccepted

def isHadFourthLayer(layerName,TT_part):

#  layer number 11 in HEC, high eta part of 23 (FCAL3)
   isAccepted = False
   if layerName == 11 or (layerName == 23 and TT_part ==  'EmbFcalHighEta'):
     isAccepted = True	
	    
   return isAccepted

class L1CaloHVReader:

    def __init__(self,file_name):

      self.NLayers  = {}
      self.MeanCorr = {}

      self.CorrLayer1 = {}
      self.CorrLayer2 = {}
      self.CorrLayer3 = {}
      self.CorrLayer4 = {}

      self.AffectedCells1 = {}
      self.AffectedCells2 = {}
      self.AffectedCells3 = {}
      self.AffectedCells4 = {}
      
      self.NCells1 = {}
      self.NCells2 = {}
      self.NCells3 = {}
      self.NCells4 = {}
      
      self.Name1 = {}
      self.Name2 = {}
      self.Name3 = {}
      self.Name4 = {}
    
      self.NLayers = {}

      self.UNIX2COOL = 1000000000

      self.read_corrections(file_name)
      self.read_geometry(file_name)

    def read_corrections(self,file_name):

      # get database service and open database
      dbSvc = cool.DatabaseSvcFactory.databaseService()

      dbString='sqlite://;schema='+file_name+';dbname=L1CALO'
      try:
         db = dbSvc.openDatabase(dbString, False)        
      except Exception, e:
         print 'Error: Problem opening database', e
         sys.exit(1)

      folder_name = '/TRIGGER/L1Calo/V1/Results/HVCorrections'
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
         CoolId = hex(int(row.channelId()))
         payload = row.payload()

         self.MeanCorr[CoolId]  = payload['RxMean']

         self.CorrLayer1[CoolId]  = payload['LayerMean1']
         self.CorrLayer2[CoolId]  = payload['LayerMean2']
         self.CorrLayer3[CoolId]  = payload['LayerMean3']
         self.CorrLayer4[CoolId]  = payload['LayerMean4']
   
         self.AffectedCells1[CoolId]  = struct.unpack('B',payload['AffectedCells1'])[0]
         self.AffectedCells2[CoolId]  = struct.unpack('B',payload['AffectedCells2'])[0]
         self.AffectedCells3[CoolId]  = struct.unpack('B',payload['AffectedCells3'])[0]
         self.AffectedCells4[CoolId]  = struct.unpack('B',payload['AffectedCells4'])[0]
            
#	 print  " CoolId", CoolId ,"AffectedCells",  struct.unpack('B',self.AffectedCells1[CoolId])[0]
	    
      
      # close database
      db.closeDatabase()

    def read_geometry(self,file_name):
    
      # get database service and open database
      dbSvc = cool.DatabaseSvcFactory.databaseService()

      dbString='sqlite://;schema='+file_name+';dbname=L1CALO'
      try:
         db = dbSvc.openDatabase(dbString, False)        
      except Exception, e:
         print 'Error: Problem opening database', e
         sys.exit(1)

      folder_name = '/TRIGGER/L1Calo/V1/Results/RxLayers'
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
         CoolId = hex(int(row.channelId()))
         payload = row.payload()
        
         self.NCells1[CoolId] = struct.unpack('B',payload['NCells1'])[0]
         self.NCells2[CoolId] = struct.unpack('B',payload['NCells2'])[0]
         self.NCells3[CoolId] = struct.unpack('B',payload['NCells3'])[0]
         self.NCells4[CoolId] = struct.unpack('B',payload['NCells4'])[0]

         self.NLayers[CoolId] = struct.unpack('B',payload['NLayers'])[0]
  
         self.Name1[CoolId] = struct.unpack('B',payload['Name1'])[0]
         self.Name2[CoolId] = struct.unpack('B',payload['Name2'])[0]
         self.Name3[CoolId] = struct.unpack('B',payload['Name3'])[0]
         self.Name4[CoolId] = struct.unpack('B',payload['Name4'])[0]


  
       # close database
      db.closeDatabase()
   

    def GetMeanCorections(self):
      return self.MeanCorr

    def GetCorLayer1(self):
      return self.CorrLayer1

    def GetCorLayer2(self):
      return self.CorrLayer2

    def GetCorLayer3(self):
      return self.CorrLayer3

    def GetCorLayer4(self):
      return self.CorrLayer4

    def GetAffectedCells1(self):
      return self.AffectedCells1

    def GetAffectedCells2(self):
      return self.AffectedCells2

    def GetAffectedCells3(self):
      return self.AffectedCells3

    def GetAffectedCells4(self):
      return self.AffectedCells4

    def GetNCells1(self):
      return self.NCells1

    def GetNCells2(self):
      return self.NCells2

    def GetNCells3(self):
      return self.NCells3

    def GetNCells4(self):
      return self.NCells4

    def GetNLayers(self):
      return self.NLayers

    def GetName1(self):
      return self.Name1

    def GetName2(self):
      return self.Name2

    def GetName3(self):
      return self.Name3
      
    def GetName4(self):
      return self.Name4


def PlotCalibrationHV(input_file_name=None):

  HVCutAverage = 1.05          # lower cut on TT-averaged HV corrections
#  HVCutAverage = 1.01          # lower cut on TT-averaged HV corrections
  HVCutLayers  = 1.001         # lower cut on layer-by-layer corrections
   
  ROOT.gStyle.SetPalette(1)
  ROOT.gStyle.SetOptStat(111111)
  ROOT.gStyle.SetCanvasColor(10)

  c1 = ROOT.TCanvas('c1','Example',200,10,700,500)

  h_MeanCorrEmecFcalLowEta_em  = L1CaloMap("Mean HV corrections for EM  (EMEC in overlap) ","#eta bin","#phi bin")
  h_MeanCorrEmecFcalLowEta_had = L1CaloMap("Mean HV corrections for HAD (FCAL low #eta)","#eta bin","#phi bin")

  h_MeanCorrEmbFcalHighEta_em  = L1CaloMap("Mean HV corrections for EM overlap (EMB)","#eta bin","#phi bin")
  h_MeanCorrEmbFcalHighEta_had = L1CaloMap("Mean HV corrections for HAD FCAL (high #eta)","#eta bin","#phi bin")


  h_LayerCorr_presampler     = L1CaloMap("HV corrections for EM  presampler ","#eta bin","#phi bin") 
  h_CellFraction_presampler  = L1CaloMap("Fraction of HV affected cells in EM  presampler","#eta bin","#phi bin")
  h_LayerCorr_em_front       = L1CaloMap("HV corrections for EM front layer ","#eta bin","#phi bin")
  h_CellFraction_em_front    = L1CaloMap("Fraction of HV affected cells in EM front layer","#eta bin","#phi bin")
  h_LayerCorr_em_middle      = L1CaloMap("HV corrections for EM middle layer ","#eta bin","#phi bin")
  h_CellFraction_em_middle   = L1CaloMap("Fraction of HV affected cells in EM middle layer","#eta bin","#phi bin")
  h_LayerCorr_em_back        = L1CaloMap("HV corrections for EM back layer","#eta bin","#phi bin")
  h_CellFraction_em_back     = L1CaloMap("Fraction of HV affected cells in EM back layer","#eta bin","#phi bin")

  h_LayerCorr_em_overlap_back     = L1CaloMap("HV corrections for EM overlap back layer","#eta bin","#phi bin")
  h_CellFraction_em_overlap_back  = L1CaloMap("Fraction of HV affected cells in EM overlap back layer","#eta bin","#phi bin")

  h_LayerCorr_had_first        = L1CaloMap("HV corrections for first HAD layer (low #eta FCAL2)","#eta bin","#phi bin")
  h_CellFraction_had_first     = L1CaloMap("Fraction of HV affected cells in first HAD layer (low #eta FCAL2)","#eta bin","#phi bin")
  h_LayerCorr_had_second       = L1CaloMap("HV corrections for second HAD layer (high #eta FCAL2)","#eta bin","#phi bin")
  h_CellFraction_had_second    = L1CaloMap("Fraction of HV affected cells in second HAD layer (high #eta FCAL2)","#eta bin","#phi bin")
  h_LayerCorr_had_third        = L1CaloMap("HV corrections for third HAD layer (low #eta FCAL3)","#eta bin","#phi bin")
  h_CellFraction_had_third     = L1CaloMap("Fraction of HV affected cells in third HAD layer (low #eta FCAL3)","#eta bin","#phi bin")
  h_LayerCorr_had_fourth       = L1CaloMap("HV corrections for fourth HAD layer (high #eta FCAL3)","#eta bin","#phi bin")
  h_CellFraction_had_fourth    = L1CaloMap("Fraction of HV affected cells in fourth HAD layer (high #eta FCAL3)","#eta bin","#phi bin")



  geometry_convertor = L1CaloGeometryConvertor()
  geometry_convertor.LoadReceiverPPMMap()


  input_file = input_file_name
  print "Taking HV information from file",input_file 
  
  hv_status = L1CaloHVReader(input_file)

  large_hv_file = open('large_hv_corr.txt','w')

  for ReceiverId in  hv_status.GetMeanCorections().keys():

    MeanHVCorrection = (hv_status.GetMeanCorections())[ReceiverId]
    CorrLayers = [0,0,0,0]
    CorrLayers[0]       = (hv_status.GetCorLayer1())[ReceiverId]
    CorrLayers[1]       = (hv_status.GetCorLayer2())[ReceiverId]
    CorrLayers[2]       = (hv_status.GetCorLayer3())[ReceiverId]
    CorrLayers[3]       = (hv_status.GetCorLayer4())[ReceiverId]
    
    NLayers          = (hv_status.GetNLayers())[ReceiverId]

    AffectedCells = [0,0,0,0]
    AffectedCells[0]   = (hv_status.GetAffectedCells1())[ReceiverId]
    AffectedCells[1]   = (hv_status.GetAffectedCells2())[ReceiverId]
    AffectedCells[2]   = (hv_status.GetAffectedCells3())[ReceiverId]
    AffectedCells[3]   = (hv_status.GetAffectedCells4())[ReceiverId]

    NCells = [0,0,0,0]
    NCells[0]   = (hv_status.GetNCells1())[ReceiverId]
    NCells[1]   = (hv_status.GetNCells2())[ReceiverId]
    NCells[2]   = (hv_status.GetNCells3())[ReceiverId]
    NCells[3]   = (hv_status.GetNCells4())[ReceiverId]
    
    LayerName = [0,0,0,0]
    LayerName[0] = (hv_status.GetName1())[ReceiverId]
    LayerName[1] = (hv_status.GetName2())[ReceiverId]
    LayerName[2] = (hv_status.GetName3())[ReceiverId]
    LayerName[3] = (hv_status.GetName4())[ReceiverId]

    PPM_ID =  geometry_convertor.getPPMfromReceiver(ReceiverId)
    eta_bin = geometry_convertor.getEtaBin(PPM_ID)
    phi_bin = geometry_convertor.getPhiBin(PPM_ID)

    TT_part = 'EmecFcalLowEta'
    if geometry_convertor.isPPMFCAL(PPM_ID) and geometry_convertor.getFCAL23RecEta(ReceiverId)=='HighEta':
      TT_part = 'EmbFcalHighEta'
    if geometry_convertor.isPPMOverlap(PPM_ID) and geometry_convertor.getOverlapLayer(ReceiverId)=='EMB':
      TT_part = 'EmbFcalHighEta'

#    if geometry_convertor.isPPMOverlap(PPM_ID):
#      print "Overlap tower COOL Id=%s  Rx=%s  layer=%s" % (PPM_ID,ReceiverId,geometry_convertor.getOverlapLayer(ReceiverId))

#    if geometry_convertor.isPPMFCAL(PPM_ID):
#      print "Overlap tower COOL Id=%s  Rx=%s  layer=%s" % (PPM_ID,ReceiverId,TT_part)

#   Mean corrections

    if MeanHVCorrection > HVCutAverage and TT_part == 'EmecFcalLowEta':
      if geometry_convertor.isCoolEm(PPM_ID):
        h_MeanCorrEmecFcalLowEta_em.Fill(eta_bin,phi_bin,MeanHVCorrection)
        large_hv_file.write("%3i %3i  EM   %12s  Rx=%7s  %.3f  (%.3f %.3f %.3f %.3f) \n" % (eta_bin,phi_bin,PPM_ID,ReceiverId,MeanHVCorrection,CorrLayers[0],CorrLayers[1],CorrLayers[2],CorrLayers[3]))

      if geometry_convertor.isCoolHad(PPM_ID):
        h_MeanCorrEmecFcalLowEta_had.Fill(eta_bin,phi_bin,MeanHVCorrection)
        large_hv_file.write("%3i %3i  HAD  %12s  Rx=%7s  %.3f  (%.3f %.3f %.3f %.3f) \n" % (eta_bin,phi_bin,PPM_ID,ReceiverId,MeanHVCorrection,CorrLayers[0],CorrLayers[1],CorrLayers[2],CorrLayers[3]))

    if MeanHVCorrection > HVCutAverage and TT_part == 'EmbFcalHighEta':
      if geometry_convertor.isCoolEm(PPM_ID):
        h_MeanCorrEmbFcalHighEta_em.Fill(eta_bin,phi_bin,MeanHVCorrection)
        large_hv_file.write("%3i %3i  EM   %12s  Rx=%7s  %.3f  (%.3f %.3f %.3f %.3f) \n" % (eta_bin,phi_bin,PPM_ID,ReceiverId,MeanHVCorrection,CorrLayers[0],CorrLayers[1],CorrLayers[2],CorrLayers[3]))

      if geometry_convertor.isCoolHad(PPM_ID):
        h_MeanCorrEmbFcalHighEta_had.Fill(eta_bin,phi_bin,MeanHVCorrection)
        large_hv_file.write("%3i %3i  HAD  %12s  Rx=%7s  %.3f  (%.3f %.3f %.3f %.3f) \n" % (eta_bin,phi_bin,PPM_ID,ReceiverId,MeanHVCorrection,CorrLayers[0],CorrLayers[1],CorrLayers[2],CorrLayers[3]))

#    Now go layer-by-layer

    for ilayer in range(NLayers):

#     EM layers

      if isEmPresampler(LayerName[ilayer]):
        if CorrLayers[ilayer] > HVCutLayers:
          h_LayerCorr_presampler.Fill(eta_bin,phi_bin,CorrLayers[ilayer])
          h_CellFraction_presampler.Fill(eta_bin,phi_bin,float(AffectedCells[ilayer])/float(NCells[ilayer]))

      if isEmFront(LayerName[ilayer],geometry_convertor.isPPMOverlap(PPM_ID)):
        if CorrLayers[ilayer] > HVCutLayers:
          h_LayerCorr_em_front.Fill(eta_bin,phi_bin,CorrLayers[ilayer])
          h_CellFraction_em_front.Fill(eta_bin,phi_bin,float(AffectedCells[ilayer])/float(NCells[ilayer]))

      if isEmMiddle(LayerName[ilayer],geometry_convertor.isPPMOverlap(PPM_ID)):
        if CorrLayers[ilayer] > HVCutLayers:
          h_LayerCorr_em_middle.Fill(eta_bin,phi_bin,CorrLayers[ilayer])
          h_CellFraction_em_middle.Fill(eta_bin,phi_bin,float(AffectedCells[ilayer])/float(NCells[ilayer]))

      if isEmBack(LayerName[ilayer],geometry_convertor.isPPMOverlap(PPM_ID)):
        if CorrLayers[ilayer] > HVCutLayers:
          h_LayerCorr_em_back.Fill(eta_bin,phi_bin,CorrLayers[ilayer])
          h_CellFraction_em_back.Fill(eta_bin,phi_bin,float(AffectedCells[ilayer])/float(NCells[ilayer]))

      if isEmOverlapBack(LayerName[ilayer],geometry_convertor.isPPMOverlap(PPM_ID)):
        if CorrLayers[ilayer] > HVCutLayers:
          h_LayerCorr_em_overlap_back.Fill(eta_bin,phi_bin,CorrLayers[ilayer])
          h_CellFraction_em_overlap_back.Fill(eta_bin,phi_bin,float(AffectedCells[ilayer])/float(NCells[ilayer]))

#      HAD layers

      if isHadFirstLayer(LayerName[ilayer],TT_part):
        if CorrLayers[ilayer] > HVCutLayers:
          h_LayerCorr_had_first.Fill(eta_bin,phi_bin,CorrLayers[ilayer])
          h_CellFraction_had_first.Fill(eta_bin,phi_bin,float(AffectedCells[ilayer])/float(NCells[ilayer]))

      if isHadSecondLayer(LayerName[ilayer],TT_part):
        if CorrLayers[ilayer] > HVCutLayers:
          h_LayerCorr_had_second.Fill(eta_bin,phi_bin,CorrLayers[ilayer])
          h_CellFraction_had_second.Fill(eta_bin,phi_bin,float(AffectedCells[ilayer])/float(NCells[ilayer]))

      if isHadThirdLayer(LayerName[ilayer],TT_part):
        if CorrLayers[ilayer] > HVCutLayers:
          h_LayerCorr_had_third.Fill(eta_bin,phi_bin,CorrLayers[ilayer])
          h_CellFraction_had_third.Fill(eta_bin,phi_bin,float(AffectedCells[ilayer])/float(NCells[ilayer]))

      if isHadFourthLayer(LayerName[ilayer],TT_part):
        if CorrLayers[ilayer] > HVCutLayers:
          h_LayerCorr_had_fourth.Fill(eta_bin,phi_bin,CorrLayers[ilayer])
          h_CellFraction_had_fourth.Fill(eta_bin,phi_bin,float(AffectedCells[ilayer])/float(NCells[ilayer]))


  large_hv_file.close()

  # Draw mean corrections

  h_MeanCorrEmecFcalLowEta_em.SetMinimum(1.)
  h_MeanCorrEmecFcalLowEta_em.SetMaximum(2.1)
  h_MeanCorrEmecFcalLowEta_em.Draw()
  c1.Print("HVStatus.ps(")

  h_MeanCorrEmecFcalLowEta_had.SetMinimum(1.)
  h_MeanCorrEmecFcalLowEta_had.SetMaximum(2.1)
  h_MeanCorrEmecFcalLowEta_had.Draw()
  c1.Print("HVStatus.ps")


  h_MeanCorrEmbFcalHighEta_em.SetMinimum(1.)
  h_MeanCorrEmbFcalHighEta_em.SetMaximum(2.1)
  h_MeanCorrEmbFcalHighEta_em.Draw()
  c1.Print("HVStatus.ps")

  h_MeanCorrEmbFcalHighEta_had.SetMinimum(1.)
  h_MeanCorrEmbFcalHighEta_had.SetMaximum(2.1)
  h_MeanCorrEmbFcalHighEta_had.Draw()
  c1.Print("HVStatus.ps")


  # Now EM layer-by-layer corrections

  h_LayerCorr_presampler.SetMinimum(1.)
  h_LayerCorr_presampler.SetMaximum(2.1)
  h_LayerCorr_presampler.Draw()
  c1.Print("HVStatus.ps")

  h_CellFraction_presampler.SetMinimum(0.)
  h_CellFraction_presampler.SetMaximum(1.1)
  h_CellFraction_presampler .Draw()
  c1.Print("HVStatus.ps")

  h_LayerCorr_em_front.SetMinimum(1.)
  h_LayerCorr_em_front.SetMaximum(2.1)
  h_LayerCorr_em_front.Draw()
  c1.Print("HVStatus.ps")

  h_CellFraction_em_front.SetMinimum(0.) 
  h_CellFraction_em_front.SetMaximum(1.1) 
  h_CellFraction_em_front.Draw() 
  c1.Print("HVStatus.ps")

  h_LayerCorr_em_middle.SetMinimum(1.)
  h_LayerCorr_em_middle.SetMaximum(2.1)
  h_LayerCorr_em_middle.Draw()
  c1.Print("HVStatus.ps")

  h_CellFraction_em_middle.SetMinimum(0.)
  h_CellFraction_em_middle.SetMaximum(1.1)
  h_CellFraction_em_middle.Draw()
  c1.Print("HVStatus.ps")

  h_LayerCorr_em_back.SetMinimum(1.)
  h_LayerCorr_em_back.SetMaximum(2.1)
  h_LayerCorr_em_back.Draw()
  c1.Print("HVStatus.ps")

  h_CellFraction_em_back.SetMinimum(0.) 
  h_CellFraction_em_back.SetMaximum(1.1) 
  h_CellFraction_em_back.Draw() 
  c1.Print("HVStatus.ps")

  h_LayerCorr_em_overlap_back.SetMinimum(1.)
  h_LayerCorr_em_overlap_back.SetMaximum(2.1)
  h_LayerCorr_em_overlap_back.Draw()
  c1.Print("HVStatus.ps")

  h_CellFraction_em_overlap_back.SetMinimum(0.)
  h_CellFraction_em_overlap_back.SetMaximum(1.1)
  h_CellFraction_em_overlap_back.Draw()
  c1.Print("HVStatus.ps")


  h_LayerCorr_had_first.SetMinimum(1.)
  h_LayerCorr_had_first.SetMaximum(2.1)  
  h_LayerCorr_had_first.Draw()         
  c1.Print("HVStatus.ps")

  h_CellFraction_had_first.SetMinimum(0.)    
  h_CellFraction_had_first.SetMaximum(1.1)   
  h_CellFraction_had_first.Draw()   
  c1.Print("HVStatus.ps")

  h_LayerCorr_had_second.SetMinimum(1.)      
  h_LayerCorr_had_second.SetMaximum(2.1)    
  h_LayerCorr_had_second.Draw()    
  c1.Print("HVStatus.ps")

  h_CellFraction_had_second.SetMinimum(0.)  
  h_CellFraction_had_second.SetMaximum(1.1)  
  h_CellFraction_had_second.Draw()  
  c1.Print("HVStatus.ps")

  h_LayerCorr_had_third.SetMinimum(1.)     
  h_LayerCorr_had_third.SetMaximum(2.1)     
  h_LayerCorr_had_third.Draw()    
  c1.Print("HVStatus.ps")

  h_CellFraction_had_third.SetMinimum(0.)   
  h_CellFraction_had_third.SetMaximum(1.1) 
  h_CellFraction_had_third.Draw() 
  c1.Print("HVStatus.ps")
   
  h_LayerCorr_had_fourth.SetMinimum(1.)    
  h_LayerCorr_had_fourth .SetMaximum(2.1)   
  h_LayerCorr_had_fourth.Draw()   
  c1.Print("HVStatus.ps")

  h_CellFraction_had_fourth.SetMinimum(0.)  
  h_CellFraction_had_fourth.SetMaximum(1.1) 
  h_CellFraction_had_fourth.Draw()  
  c1.Print("HVStatus.ps)")


  os.system("ps2pdf HVStatus.ps")
 # os.system("cp HVStatus.pdf /home/jb/public_web/tmp ")

 
  print "Done!"

if __name__ == "__main__":
  
  print "Starting PlotCalibrationHV"

  parser = OptionParser()
  
  parser.add_option("-f","--InputFile",action="store",type="string",dest="input_file_name",help="Name of input file")

  (options, args) = parser.parse_args()

  PlotCalibrationHV(options.input_file_name)
