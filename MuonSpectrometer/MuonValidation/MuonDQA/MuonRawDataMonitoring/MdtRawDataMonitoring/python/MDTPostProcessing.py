#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from ROOT import TH1F
from .MdtMonUtils import putBoxMdtGlobal, getTubeLength, MDT2DHWName, MDTTubeEff, MDTFitTDC
from math import sqrt
import numpy as np

def make_hits_per_evt(inputs):
   EvtOccBCap = inputs[0][1][1].Clone()
   EvtOccECap = inputs[0][1][2].Clone()
   EvtOccBCap.Reset()
   EvtOccECap.Reset()
   EvtOccBCap.SetName("HitsPerEvtInBarrelPerChamber_ADCCut")
   EvtOccBCap.SetTitle("Avg # hits/evt Barrel, ADCCut")
   EvtOccECap.SetName("HitsPerEvtInEndCapPerChamber_ADCCut")
   EvtOccECap.SetTitle("Avg # hits/evt Endcap, ADCCut")
   
   VolumeMapBCap = inputs[0][1][1].Clone()
   VolumeMapECap = inputs[0][1][2].Clone()
   VolumeMapBCap.Reset()
   VolumeMapECap.Reset()
   VolumeMapBCap.SetName("VolumeMapBarrel")
   VolumeMapBCap.SetTitle("Volume Map (#tubes*tubeVol in m^3) Barrel")
   VolumeMapECap.SetName("VolumeMapEncap")
   VolumeMapECap.SetTitle("Volume Map (#tubes*tubeVol in m^3) Endcap")

   size = len(inputs)
   chamberHits_vec = []
   for i in range(size):
      hi = inputs[i][1][0].Clone()
      chamberHits_vec.append(hi.GetEntries())

   sorted_chamberHits_vec = np.sort(chamberHits_vec)
   medianChamberHits = 0
   den = int(size/2)
   if (size % 2 == 0 and size > 2):
      medianChamberHits = (sorted_chamberHits_vec[den] + sorted_chamberHits_vec[den + 1])/size
   elif(size > 0):
      medianChamberHits = sorted_chamberHits_vec[den]

   for i in range(size):
      hvOff = False
      hi = inputs[i][1][0].Clone()
      name = hi.GetName()
      xAxis = name[0]+name[4]+name[3]
      yAxis = name[1]+name[5]+name[6]
      if((name[0:3] == "BIR" or name[0:3] == "BIM") and (name[5:7] == "11" or name[5:7] == "15")):
         yAxis += name[2]
      # BML[45][AC]13-->BML[56][AC]13
      if(name[0:3] == "BML" and int(name[3]) > 3 and name[5:7] == "13"):
         xAxis = name[0]+name[4]+str(int(name[3])+1)

      if(name[0:3] == "BME"):
         xAxis = name[0] + name[4] + "4"
         yAxis = "M13"
      if(name[0:3] == "BMF"):
         xAxis = name[0] + name[4] + (str(int(name[3])*2-1))

      nhits = hi.GetEntries()

      if (name.startswith("BE") or name.startswith("BIS8")):
         if (nhits < 0.01*medianChamberHits+0.1):
            hvOff = True
      elif(nhits < 0.07 * medianChamberHits + 0.1):
         hvOff = True

      h_trigger = inputs[0][1][3]
      nTriggers = int(h_trigger.GetEntries())

      tubeRadiusScale = 1
      tubeLength =      getTubeLength(name)
      if(name[0:3] == "BME" or name[0:3] == "BMG"):
         tubeRadiusScale = 0.25 
      numTubesInChamber=hi.GetNbinsX()
      #0.0006881 m2 = pi*tube_r^2
      chamb_vol = numTubesInChamber*tubeLength*0.0006881*tubeRadiusScale


      if(len(yAxis) == 4):
         yAxis2 = yAxis[0]+' '+yAxis[1:3]+' '+yAxis[3]
         #yAxis2=yAxis[0]+','+yAxis[1:3]+','+yAxis[3]
      else:
         yAxis2 = yAxis[0]+' '+yAxis[1:3]
         #yAxis2=yAxis[0]+','+yAxis[1:3]
      if(xAxis.startswith("B")):
         VolumeMapBCap.Fill(xAxis, yAxis2, chamb_vol)
         if (not hvOff):
            EvtOccBCap.Fill(xAxis, yAxis2, nhits/(float(nTriggers)))
      else:
         VolumeMapECap.Fill(xAxis, yAxis2, chamb_vol)
         if (not hvOff):
            EvtOccECap.Fill(xAxis, yAxis2, nhits/(float(nTriggers)))

   putBoxMdtGlobal(EvtOccBCap, "B")
   putBoxMdtGlobal(EvtOccECap, "E")
   putBoxMdtGlobal(VolumeMapBCap, "B")
   putBoxMdtGlobal(VolumeMapECap, "E")

   return [EvtOccBCap, EvtOccECap, VolumeMapBCap, VolumeMapECap]


def make_eff_histo(inputs, ec):
   ecap = ["BA", "BC", "EA", "EC"]
   ecap_str= ecap[ec]
   ecap_fullStr_lower = "mdt"+ecap_str
   heff = TH1F(ecap_fullStr_lower+"_TUBE_eff",ecap_fullStr_lower+"_TUBE_eff",100,0,1)

   size = len(inputs)
   dencut = 10

   for i in range(size): 
      hi_num = inputs[i][1][0].Clone()
      hi_den = inputs[i][1][1].Clone()
      nbin=hi_den.GetNbinsX()
      for ibin in range(nbin):
         if( hi_den.At(ibin) > dencut ):
            heff.Fill( (hi_num.At(ibin))/(hi_den.At(ibin)) )

   return [heff]

def make_eff_histo_perML(inputs, ec):

   ecap = ["BA", "BC", "EA", "EC"]
   ecap_str= ecap[ec]
   heff_outer = inputs[0][1][2].Clone()
   heff_middle = inputs[0][1][3].Clone()
   heff_inner = inputs[0][1][4].Clone()
   heff_extra = inputs[0][1][5].Clone()
   heff_outer.Reset()
   heff_outer.SetName("effsIn"+ecap_str+"BAOuterPerMultiLayer_ADCCut")
   heff_outer.SetTitle("effsIn"+ecap_str+"OuterPerMultiLayer, ADCCut")
   heff_outer_N = heff_outer.Clone()
   name = heff_outer.GetName()  

   heff_middle.Reset()
   heff_middle.SetName("effsIn"+ecap_str+"MiddlePerMultiLayer_ADCCut")
   heff_middle.SetTitle("effsIn"+ecap_str+"MiddlePerMultiLayer, ADCCut")
   heff_middle_N = heff_middle.Clone()

   heff_inner.Reset()
   heff_inner.SetName("effsIn"+ecap_str+"InnerPerMultiLayer_ADCCut")
   heff_inner.SetTitle("effsIn"+ecap_str+"InnerPerMultiLayer, ADCCut")
   heff_inner_N = heff_inner.Clone()
   
   heff_extra.Reset()
   heff_extra.SetName("effsIn"+ecap_str+"ExtraPerMultiLayer_ADCCut")
   heff_extra.SetTitle("effsIn"+ecap_str+"ExtraPerMultiLayer, ADCCut")
   heff_extra_N = heff_extra.Clone()

   size = len(inputs)
   for i in range(size):
      hi_num = inputs[i][1][0].Clone()
      name_num = hi_num.GetName()
      hi_den = inputs[i][1][1].Clone()
      name=name_num[0:7]
      countsML1, countsML2, entriesML1, entriesML2 = MDTTubeEff(name,hi_num,hi_den)
      ch_name = name[0:7]
      stateta_c, statphi_c, statphi_c2 = MDT2DHWName(ch_name)
      cut=10
      if(ch_name[1:2]=="O"):
         if( entriesML1 > cut ):
            heff_outer.Fill(stateta_c, statphi_c, countsML1)
            heff_outer_N.Fill(stateta_c, statphi_c, entriesML1)
         if( entriesML2 > cut ):
            heff_outer.Fill(stateta_c, statphi_c2, countsML2)
            heff_outer_N.Fill(stateta_c, statphi_c2, entriesML2)
      if(ch_name[1:2]=="M"):
         if( entriesML1 > cut ):
            heff_middle.Fill(stateta_c, statphi_c, countsML1)
            heff_middle_N.Fill(stateta_c, statphi_c, entriesML1)
         if( entriesML2 > cut ):
            heff_middle.Fill(stateta_c, statphi_c2, countsML2)
            heff_middle_N.Fill(stateta_c, statphi_c2, entriesML2)            
      if(ch_name[1:2]=="I"):
         if( entriesML1 > cut ):
            heff_inner.Fill(stateta_c, statphi_c, countsML1)
            heff_inner_N.Fill(stateta_c, statphi_c, entriesML1)
         if( entriesML2 > cut ):
            heff_inner.Fill(stateta_c, statphi_c2, countsML2)
            heff_inner_N.Fill(stateta_c, statphi_c2, entriesML2)
      if(ch_name[1:2]=="E"):
         if( entriesML1 > cut ):
            heff_extra.Fill(stateta_c, statphi_c, countsML1)
            heff_extra_N.Fill(stateta_c, statphi_c, entriesML1)
         if( entriesML2 > cut ):
            heff_extra.Fill(stateta_c, statphi_c2, countsML2)
            heff_extra_N.Fill(stateta_c, statphi_c2, entriesML2)
      
   heff_outer.Divide(heff_outer_N)
   heff_middle.Divide(heff_middle_N)
   heff_inner.Divide(heff_inner_N)
   heff_extra.Divide(heff_extra_N)

   return [heff_outer, heff_middle, heff_inner, heff_extra]
   
def drift_time_monitoring(inputs, ec):

   ecap = ["BA", "BC", "EA", "EC"]
   ecap_str= ecap[ec]

   size = len(inputs)

   sumt0_name = "MDT_t0_"+ecap_str
   sumt0 = TH1F(sumt0_name,sumt0_name,1,0,1)
   sumt0.SetFillColor(42)
   sumt0.SetMarkerStyle(20)
   sumt0.SetMarkerColor(42)
   sumt0.SetAxisRange(0,300,"y")
   sumt0.Reset()

   sumtmax_name = "MDT_tmax_"+ecap_str
   sumtmax = TH1F(sumtmax_name,sumtmax_name,1,0,1)
   sumtmax.SetFillColor(42)
   sumtmax.SetMarkerStyle(20)
   sumtmax.SetMarkerColor(42)
   sumtmax.SetAxisRange(0,1500,"y")
   sumtmax.Reset()

   sumtdrift_name = "MDT_tdrift_"+ecap_str
   sumtdrift = TH1F(sumtdrift_name,sumtdrift_name,1,0,1)
   sumtdrift.SetFillColor(42)
   sumtdrift.SetMarkerStyle(20)
   sumtdrift.SetMarkerColor(42)
   sumtdrift.SetAxisRange(0,1200,"y")
   sumtdrift.Reset()

   h=TH1F()
   
   for i in range(size):
      currentbin=i+1
      h = inputs[i][1][0].Clone()
      t0, t0err, tmax, tmaxerr = MDTFitTDC(h)

      layer=""
      if(currentbin<17):
         layer = "In"
      elif (currentbin<33):
         layer = "Mid"
      elif (currentbin<49):
         layer = "Out"
      else:
         layer = "Ext"

      stPhi = currentbin%16
      if(layer=="Ext" and (ecap_str=="BA" or ecap_str=="BC")):
         stPhi = 2*currentbin%16
                    
      if(stPhi==0):
         stPhi=16

      phi = str(stPhi)
      sector = ecap_str+"_"+layer+"_"+phi
      if(h.GetEntries()<100):
         sector+="_OFF"
      elif(t0err>t0):
         sector+="_ERR"
      
      sumt0.Fill(sector,t0)
      sumt0.SetBinError(currentbin,t0err)
      sumtmax.Fill(sector,tmax)
      sumtmax.SetBinError(currentbin,tmaxerr)
      if(tmax-t0 < 0 or tmax-t0 > 2000) :
         sumtdrift.Fill(sector,0)
         sumtdrift.SetBinError(currentbin,2000)
      else:
         sumtdrift.Fill(sector,tmax-t0)
         sumtdrift.SetBinError(currentbin,sqrt(tmaxerr*tmaxerr + t0err*t0err))
                                  

   sumt0.LabelsDeflate("x")
   sumtmax.LabelsDeflate("x")
   sumtdrift.LabelsDeflate("x")
   if(sumt0.GetNbinsX()>1) :
      sumt0.LabelsOption("v","x")
   if(sumtmax.GetNbinsX()>1) :
      sumtmax.LabelsOption("v","x")
   if(sumtdrift.GetNbinsX()>1) :
      sumtdrift.LabelsOption("v","x")

   return [sumt0, sumtmax, sumtdrift]

def MdtGlobalBox(inputs):
   EvtOccBCap = inputs[0][1][0]
   EvtOccECap = inputs[0][1][1]
   putBoxMdtGlobal(EvtOccBCap, "B")
   putBoxMdtGlobal(EvtOccECap, "E")
   return [EvtOccBCap, EvtOccECap]


def test(inputs):
   """ HitsPerEventInXXPerChamber_[onSegm]_ADCCut """
   print("hello!")
   print(inputs)
   print(len(inputs))
   return []
