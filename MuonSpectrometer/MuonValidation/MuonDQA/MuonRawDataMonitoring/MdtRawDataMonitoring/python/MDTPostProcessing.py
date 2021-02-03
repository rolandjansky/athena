#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from .MdtMonUtils import putBoxMdtGlobal
import numpy as np


def make_hits_per_evt(inputs):
   EvtOccBCap = inputs[0][1][1].Clone()
   EvtOccBCap.Reset()
   EvtOccECap = inputs[0][1][2].Clone()
   EvtOccECap.Reset()
   EvtOccBCap.SetName("HitsPerEvtInBarrelPerChamber_ADCCut")
   EvtOccBCap.SetTitle("Avg # hits/evt Barrel, ADCCut")
   EvtOccECap.SetName("HitsPerEvtInEndCapPerChamber_ADCCut")
   EvtOccECap.SetTitle("Avg # hits/evt Endcap, ADCCut")

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
      if (not hvOff):
         if(len(yAxis) == 4):
            yAxis2 = yAxis[0]+' '+yAxis[1:3]+' '+yAxis[3]
            # yAxis2=yAxis[0]+','+yAxis[1:3]+','+yAxis[3]
         else:
            yAxis2 = yAxis[0]+' '+yAxis[1:3]
            # yAxis2=yAxis[0]+','+yAxis[1:3]
         if(xAxis.startswith("B")):
            EvtOccBCap.Fill(xAxis, yAxis2, nhits/(float(nTriggers)))
         else:
            EvtOccECap.Fill(xAxis, yAxis2, nhits/(float(nTriggers)))

   putBoxMdtGlobal(EvtOccBCap, "B")
   putBoxMdtGlobal(EvtOccECap, "E")

   return [EvtOccBCap, EvtOccECap]


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
