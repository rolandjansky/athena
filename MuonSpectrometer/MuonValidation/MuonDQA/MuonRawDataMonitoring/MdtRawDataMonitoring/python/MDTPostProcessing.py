#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from ROOT import TH1F
from .MdtMonUtils import putBoxMdtGlobal, getTubeLength
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
