#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from ROOT import TH1F
from .MdtMonUtils import putBoxMdtGlobal, getTubeLength, MDT2DHWName, MDTTubeEff, MDTFitTDC, putBoxMdtRegional
from math import sqrt
import numpy as np

   

def make_hits_per_evt(inputs):
   inputs=list(inputs)
   i01 = inputs[0][1]
   EvtOccBCap = i01[1].Clone()
   EvtOccECap = i01[2].Clone()
   EvtOccBCap.Reset()
   EvtOccECap.Reset()
   EvtOccBCap.SetName("HitsPerEvtInBarrelPerChamber_ADCCut")
   EvtOccBCap.SetTitle("Avg # hits/evt Barrel, ADCCut")
   EvtOccECap.SetName("HitsPerEvtInEndCapPerChamber_ADCCut")
   EvtOccECap.SetTitle("Avg # hits/evt Endcap, ADCCut")
   
   VolumeMapBCap = i01[1].Clone()
   VolumeMapECap = i01[2].Clone()
   VolumeMapBCap.Reset()
   VolumeMapECap.Reset()
   VolumeMapBCap.SetName("VolumeMapBarrel")
   VolumeMapBCap.SetTitle("Volume Map (#tubes*tubeVol in m^3) Barrel")
   VolumeMapECap.SetName("VolumeMapEncap")
   VolumeMapECap.SetTitle("Volume Map (#tubes*tubeVol in m^3) Endcap")


   OccBCap_norm = i01[4].Clone() 
   OccECap_norm = i01[5].Clone() 
   OccBCap_norm.SetName("HitsPerEventInBarrelPerChamber_onSegm_ADCCut")
   OccBCap_norm.SetTitle("Avg # hits-on-segm/evt/m^3 Barrel")
   OccECap_norm.SetName("HitsPerEventInEndCapPerChamber_onSegm_ADCCut")
   OccECap_norm.SetTitle("Avg # hits-on-segm/evt/m^3 Endcap")

   hflag=i01[6]
   geo=hflag.GetMean()   

   size = len(inputs)
   chamberHits_vec = [_[1][0].GetEntries() for _ in inputs]

   sorted_chamberHits_vec = np.sort(chamberHits_vec)
   medianChamberHits = 0
   den = int(size/2)
   if (size % 2 == 0 and size > 2):
      medianChamberHits = (sorted_chamberHits_vec[den] + sorted_chamberHits_vec[den + 1])/size
   elif(size > 0):
      medianChamberHits = sorted_chamberHits_vec[den]

   h_trigger = inputs[0][1][3]
   nTriggers = int(h_trigger.GetEntries())
   for i in range(size):
      hvOff = False
      hi = inputs[i][1][0]
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


   EvtTubeNormOccBCap = EvtOccBCap.Clone()
   EvtTubeNormOccECap = EvtOccECap.Clone()
   EvtTubeNormOccBCap.SetName("NormedHitsInBarrelPerChamber_ADCCut")
   EvtTubeNormOccBCap.SetTitle("Avg # hits/evt/m^3 Barrel, ADCCut")
   EvtTubeNormOccECap.SetName("NormedHitsInEndCapPerChamber_ADCCut")
   EvtTubeNormOccECap.SetTitle("Avg # hits/evt/m^3 Endcap, ADCCut")

   EvtTubeNormOccBCap.Divide(VolumeMapBCap)
   EvtTubeNormOccECap.Divide(VolumeMapECap)

   OccBCap_norm.Scale(1./nTriggers)
   OccBCap_norm.Divide(VolumeMapBCap)
   OccECap_norm.Scale(1./nTriggers)
   OccECap_norm.Divide(VolumeMapECap)

   putBoxMdtGlobal(EvtOccBCap, "B", geo)
   putBoxMdtGlobal(EvtOccECap, "E", geo)
   putBoxMdtGlobal(VolumeMapBCap, "B", geo)
   putBoxMdtGlobal(VolumeMapECap, "E", geo)
   putBoxMdtGlobal(EvtTubeNormOccBCap, "B", geo)
   putBoxMdtGlobal(EvtTubeNormOccECap, "E", geo)
   putBoxMdtGlobal(OccBCap_norm, "B", geo)
   putBoxMdtGlobal(OccECap_norm, "E", geo)
   

   return [EvtOccBCap, EvtOccECap, VolumeMapBCap, VolumeMapECap, EvtTubeNormOccBCap, EvtTubeNormOccECap, OccBCap_norm, OccECap_norm]


def make_eff_histo(inputs, ec):
   ecap = ["BA", "BC", "EA", "EC"]
   ecap_str= ecap[ec]
   ecap_fullStr_lower = "mdt"+ecap_str
   heff = TH1F(ecap_fullStr_lower+"_TUBE_eff",ecap_fullStr_lower+"_TUBE_eff",100,0,1)

   dencut = 10

   for itr in inputs: 
      if itr is None:
         continue
      hi_num = itr[1][0]
      hi_den = itr[1][1]
      nbin=hi_den.GetNbinsX()
      for ibin in range(nbin):
         if( hi_den.At(ibin) > dencut ):
            heff.Fill( (hi_num.At(ibin))/(hi_den.At(ibin)) )

   return [heff]

def make_eff_histo_perChamber(inputs):
   if inputs[0] is None:
      return []

   EffBCap = inputs[0][1][2].Clone()
   EffECap = inputs[0][1][3].Clone()
   EffBCap.Reset()
   EffECap.Reset()
   EffBCap_N = EffBCap.Clone()
   EffECap_N = EffECap.Clone()

   EffBCap.SetName("effsInBarrelPerChamber_ADCCut")
   EffBCap.SetTitle("effsInBarrelPerChamber_ADCCut")
   EffECap.SetName("effsInEndCapPerChamber_ADCCut")
   EffECap.SetTitle("effsInEndCapPerChamber_ADCCut")

   hflag=inputs[0][1][4]
   geo=hflag.GetMean()

   for itr in inputs:
      if itr is None:
         continue
      hi_num = itr[1][0]
      name_num = hi_num.GetName()
      hi_den = itr[1][1]
      name=name_num[0:7]
      countsML1, countsML2, entriesML1, entriesML2 = MDTTubeEff(name,hi_num,hi_den)
      ch_name = name[0:7]
      stateta_IMO_c, statphi_IMO_c, stateta_c, statphi_c, statphi_c2 = MDT2DHWName(ch_name)
      cut=10

      if( entriesML1 + entriesML2 > cut ):
         if(name[0:1]=="B"):
            EffBCap.Fill(stateta_IMO_c,statphi_IMO_c,countsML1+countsML2)
            EffBCap_N.Fill(stateta_IMO_c,statphi_IMO_c,entriesML1+entriesML2)
         else:
            EffECap.Fill(stateta_IMO_c,statphi_IMO_c,countsML1+countsML2)
            EffECap_N.Fill(stateta_IMO_c,statphi_IMO_c,entriesML1+entriesML2)
               
   EffECap.Divide(EffECap_N)
   EffBCap.Divide(EffBCap_N)

   putBoxMdtGlobal(EffBCap, "B", geo)
   putBoxMdtGlobal(EffECap, "E", geo)

   return [EffBCap, EffECap]


def make_eff_histo_perML(inputs, ec):
   if inputs[0] is None:
      return []
   
   ecap = ["BA", "BC", "EA", "EC"]
   ecap_str= ecap[ec]


   heff_outer = inputs[0][1][2].Clone()
   heff_middle = inputs[0][1][3].Clone()
   heff_inner = inputs[0][1][4].Clone()
   heff_extra = inputs[0][1][5].Clone()
   hflag=inputs[0][1][6]
   geo=hflag.GetMean()

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

   ecap_fullStr_lower = "mdt"+ecap_str
   heffML = TH1F(ecap_fullStr_lower+"_ML_eff",ecap_fullStr_lower+"_ML_eff",50,0,1)

   h2=inputs[0][1][2].Clone()
   h2.SetTitle((inputs[0][1][2].GetTitle()).replace("_forpp", ""))
   h3=inputs[0][1][3].Clone()
   h3.SetTitle((inputs[0][1][3].GetTitle()).replace("_forpp", ""))
   if(not ecap_str[0] == "E"):
      putBoxMdtRegional(h2, ecap_str[0]+"O"+ecap_str[1], geo)
      putBoxMdtRegional(h3, ecap_str[0]+"M"+ecap_str[1], geo)

   h4=inputs[0][1][4].Clone()
   h4.SetTitle((inputs[0][1][4].GetTitle()).replace("_forpp", ""))
   h5=inputs[0][1][5].Clone()
   h5.SetTitle((inputs[0][1][5].GetTitle()).replace("_forpp", ""))
   putBoxMdtRegional(h4, ecap_str[0]+"I"+ecap_str[1], geo)
   if(not ecap_str[0] == "B"):
      putBoxMdtRegional(h5, ecap_str[0]+"E"+ecap_str[1], geo)

   for itr in inputs:
      if itr is None:
         continue
      hi_num = itr[1][0]
      name_num = hi_num.GetName()
      hi_den = itr[1][1]
      name=name_num[0:7]
      countsML1, countsML2, entriesML1, entriesML2 = MDTTubeEff(name,hi_num,hi_den)
      ch_name = name[0:7]
      stateta_IMO_c, statphi_IMO_c, stateta_c, statphi_c, statphi_c2 = MDT2DHWName(ch_name)
      cut=10

      if( entriesML2 > cut ):
         heffML.Fill(countsML2/entriesML2)
      if( entriesML1 > cut ):
         heffML.Fill(countsML1/entriesML1)
         
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


   if(not ecap_str[0] == "E"):
      putBoxMdtRegional(heff_middle,ecap_str[0]+"M"+ecap_str[1], geo)
      putBoxMdtRegional(heff_outer,ecap_str[0]+"O"+ecap_str[1], geo)

   putBoxMdtRegional(heff_inner,ecap_str[0]+"I"+ecap_str[1], geo)
   if(not ecap_str[0] == "B"):
      putBoxMdtRegional(heff_extra,ecap_str[0]+"E"+ecap_str[1], geo)


   return [heff_outer, heff_middle, heff_inner, heff_extra, heffML, h2, h3, h4, h5]
   
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

   
   for i in range(size):
      currentbin=i+1
      h = inputs[i][1][0]
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
   EvtOccBCap = inputs[0][1][0].Clone()
   EvtOccBCap.SetTitle((inputs[0][1][0].GetTitle()).replace("_forpp", ""))
   EvtOccECap = inputs[0][1][1].Clone()
   EvtOccECap.SetTitle((inputs[0][1][1].GetTitle()).replace("_forpp", ""))
   hflag=inputs[0][1][2]
   geo=hflag.GetMean()
   putBoxMdtGlobal(EvtOccBCap, "B", geo)
   putBoxMdtGlobal(EvtOccECap, "E", geo)
   return [EvtOccBCap, EvtOccECap]


def test(inputs):
   """ HitsPerEventInXXPerChamber_[onSegm]_ADCCut """
   print("hello!")
   print(inputs)
   print(len(inputs))
   return []
