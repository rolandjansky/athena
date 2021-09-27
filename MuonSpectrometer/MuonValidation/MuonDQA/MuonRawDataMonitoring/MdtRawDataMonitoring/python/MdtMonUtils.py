#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
import MdtRawDataMonitoring.MdtRawMonLabels as labels
from ROOT import TBox, kGray, TLine, TMath , TF1, kBlue, kRed
from MdtRawDataMonitoring.MdtRawMonLabels import * # noqa

def getMDTLabel(x,y):
    labelx = getattr(labels, x)
    labely = getattr(labels, y)
    return labelx,labely


def getMDTLabelx(x):
    labelx = getattr(labels, x)
    return labelx


def putBoxMdtGlobal(h, ecap):
    if(ecap == "B"):
        offset = 8
        # Left Side
        putBox(h, 0, 0, 6, 16, offset); putLine(h, 6, 0, 6, 16, offset); putLine(h, 2, 16, 6, 16, offset)
        putBox(h, 0, 16, 2, 18, offset); putLine(h, 2, 16, 2, 18, offset); putLine(h, 0, 18, 2, 18, offset)
        putBox(h, 0, 20, 2, 22, offset); putLine(h, 2, 20, 2, 22, offset); putLine(h, 0, 20, 2, 20, offset); putLine(h, 0, 22, 2, 22, offset)  # 3
        putBox(h, 0, 24, 2, 26, offset); putLine(h,2,24,2,26,offset); putLine(h, 0, 24, 2, 24, offset); putLine(h, 0, 26, 2, 26, offset)  # 5
        putBox(h, 0, 28, 2, 30, offset); putLine(h,2,28,2,30,offset); putLine(h,0,28,2,28,offset); putLine(h,0,30,2,30,offset)  # 7
        putBox(h, 0, 32, 2, 34, offset);  putLine(h,2,32,2,34,offset); putLine(h,0,32,2,32,offset); putLine(h,0,34,2,34,offset)  # 9
        putBox(h, 0, 36, 3, 38, offset);  putLine(h,3,36,3,38,offset); putLine(h,0,36,3,36,offset); putLine(h,2,38,3,38,offset)  # 11M
        putBox(h, 0, 38, 2, 40, offset);  putLine(h,2,38,2,40,offset); putLine(h,0,40,2,40,offset)  # 11R
        putBox(h, 0, 42, 2, 44, offset);  putLine(h,0,42,2,42,offset); putLine(h,2,42,2,44,offset); putLine(h,0,44,2,44,offset)  # 13
        putBox(h, 0, 46, 3, 48, offset);  putLine(h,3,46,3,48,offset); putLine(h,0,46,3,46,offset); putLine(h,2,48,3,48,offset)  # 15M
        putBox(h, 0, 48, 2, 50, offset);  putLine(h,2,48,2,50,offset); putLine(h,0,50,2,50,offset)  # 15R
        putBox(h, 0, 52, 2, 106, offset); putLine(h,0,52,2,52,offset); putLine(h,0,106,2,106,offset)
        putBox(h, 0, 108, 1, 110, offset); putLine(h,1,108,1,110,offset); putLine(h,0,108,1,108,offset); putLine(h,0,110,1,110,offset)
        putBox(h, 0, 112, 2, 116, offset); putLine(h,2,112,2,116,offset); putLine(h,0,112,2,112,offset)

        putLine(h,2,52,2,106,offset)

        # Mid Section
        putBox(h, 8, 0, 9, 106, offset); putLine(h,8,0,8,106, offset); putLine(h,8,106,9,106, offset); putLine(h,9,0,9,106, offset)
        putBox(h, 8, 108, 9, 110, offset); putLine(h,8,108,9,108, offset); putLine(h,8,110,9,110, offset); putLine(h,8,108,8,110, offset); putLine(h,9,108,9,110, offset)
        putBox(h, 8, 112, 9, 116, offset); putLine(h,8,112,9,112, offset); putLine(h,8,112,8,116, offset); putLine(h,9,112,9,116, offset)

        # Right Side
        putBox(h, 11, 0, 17, 16, offset);     putLine(h,11,0,11,16, offset); putLine(h,11,16,15,16, offset)
        putBox(h, 15, 16, 17, 18, offset);  putLine(h,15,16,15,18, offset); putLine(h,15,18,17,18, offset)  # 1
        putBox(h, 15, 20, 17, 22, offset);  putLine(h,15,20,15,22, offset); putLine(h,15,20,17,20, offset); putLine(h,15,22,17,22, offset)  # 3
        putBox(h, 15, 24, 17, 26, offset);  putLine(h,15,24,15,26, offset); putLine(h,15,24,17,24, offset); putLine(h,15,26,17,26, offset)  # 5
        putBox(h, 15, 28, 17, 30, offset);  putLine(h,15,28,15,30, offset); putLine(h,15,28,17,28, offset); putLine(h,15,30,17,30, offset)  # 7
        putBox(h, 15, 32, 17, 34, offset);    putLine(h,15,32,15,34, offset); putLine(h,15,32,17,32, offset); putLine(h,15,34,17,34, offset)  # 9
        putBox(h, 14, 36, 17, 38, offset);    putLine(h,14,36,14,38, offset); putLine(h,14,36,17,36, offset); putLine(h,14,38,15,38, offset)  # 11M
        putBox(h, 15, 38, 17, 40, offset);    putLine(h,15,38,15,40, offset); putLine(h,15,40,17,40, offset)  # 11R
        putBox(h, 15, 42, 17, 44, offset);    putLine(h,15,42,15,44, offset); putLine(h,15,42,17,42, offset); putLine(h,15,44,17,44, offset)  # 13
        putBox(h, 14, 46, 17, 48, offset);    putLine(h,14,46,14,48, offset); putLine(h,14,46,17,46, offset); putLine(h,14,48,15,48, offset)  # 15M
        putBox(h, 15, 48, 17, 50, offset);    putLine(h,15,48,15,50, offset); putLine(h,15,50,17,50, offset)  # 15R
        putBox(h, 15, 52, 17, 106, offset);  putLine(h,15,52,17,52, offset); putLine(h,15,106,17,106, offset)
        putBox(h, 16, 108, 17, 110, offset);   putLine(h,16,108,16,110, offset); putLine(h,16,108,17,108, offset); putLine(h,16,110,17,110, offset)
        putBox(h, 15, 112, 17, 116, offset);   putLine(h,15,112,15,116, offset); putLine(h,15,112,17,112, offset)

        putLine(h,15,52,15,106, offset)

    elif(ecap == "E"):
        offset = 6
        # Bottom: NEW
        putBox(h, 0, 0, 4, 32, offset); putBox(h, 4, 8, 5, 10, offset)
        putLine(h, 4, 0, 4, 8, offset); putLine(h, 4, 8, 5, 8, offset); putLine(h, 5, 8, 5, 10, offset); putLine(h, 4, 10, 5, 10, offset); putLine(h, 4, 10, 4, 32, offset)  # Left side
        putBox(h, 8, 0, 12, 32, offset); putBox(h, 7, 8, 8, 10, offset)
        putLine(h, 8, 0, 8, 8, offset); putLine(h, 7, 8, 8, 8, offset); putLine(h, 7, 8, 7, 10, offset); putLine(h, 7, 10, 8, 10, offset);  putLine(h, 8, 10, 8, 32, offset)

        # Left
        putBox(h, 0, 32, 1, 34, offset); putLine(h, 1, 32, 4, 32, offset); putLine(h, 1, 32, 1, 34, offset)  # phi 1
        putBox(h, 0, 34, 4, 36, offset); putLine(h, 1, 34, 4, 34, offset); putLine(h, 4, 34, 4, 36, offset); putLine(h, 2, 36, 4, 36, offset)  # phi 2
        putBox(h, 0, 36, 2, 38, offset); putLine(h, 2, 36, 2, 38, offset)  # phi 3
        putBox(h, 0, 38, 4, 40, offset); putLine(h, 2, 38, 4, 38, offset); putLine(h, 4, 38, 4, 40, offset); putLine(h, 2, 40, 4, 40, offset)  # phi 4
        putBox(h, 0, 40, 2, 42, offset); putLine(h, 2, 40, 2, 42, offset)  # phi 5
        putBox(h, 0, 42, 4, 44, offset); putLine(h, 2, 42, 4, 42, offset); putLine(h, 4, 42, 4, 44, offset); putLine(h, 2, 44, 4, 44, offset)  # phi 6
        putBox(h, 0, 44, 2, 46, offset); putLine(h, 2, 44, 2, 46, offset)  # phi 7
        putBox(h, 0, 46, 4, 48, offset); putLine(h, 2, 46, 4, 46, offset); putLine(h, 4, 46, 4, 48, offset); putLine(h, 1, 48, 4, 48, offset)  # phi 8
        putBox(h, 0, 48, 1, 50, offset); putLine(h, 1, 48, 1, 50, offset)  # phi 9
        putBox(h, 0, 50, 4, 52, offset); putLine(h, 1, 50, 4, 50, offset); putLine(h, 4, 50, 4, 52, offset); putLine(h, 2, 52, 4, 52, offset)  # phi 10
        putBox(h, 0, 52, 2, 54, offset); putLine(h, 2, 52, 2, 54, offset)  # phi 11
        putBox(h, 0, 54, 4, 56, offset); putLine(h, 2, 54, 4, 54, offset); putLine(h, 4, 54, 4, 56, offset); putLine(h, 2, 56, 4, 56, offset)  # phi 12
        putBox(h, 0, 56, 2, 58, offset); putLine(h, 2, 56, 2, 58, offset)  # phi 13
        putBox(h, 0, 58, 4, 60, offset); putLine(h, 2, 58, 4, 58, offset); putLine(h, 4, 58, 4, 60, offset); putLine(h, 2, 60, 4, 60, offset)  # phi 14
        putBox(h, 0, 60, 2, 62, offset); putLine(h, 2, 60, 2, 62, offset)  # phi 15
        putBox(h, 0, 62, 4, 64, offset); putLine(h, 2, 62, 4, 62, offset); putLine(h, 4, 62, 4, 64, offset); putLine(h, 1, 64, 4, 64, offset)  # phi 16
        putBox(h, 0, 64, 1, 96, offset); putLine(h, 1, 64, 1, 96, offset); putLine(h, 0, 96, 1, 96, offset)

        # Right
        putBox(h, 11, 32, 12, 34, offset); putLine(h, 8, 32, 11, 32, offset); putLine(h, 11, 32, 11, 34, offset)  # phi 1
        putBox(h, 8, 34, 12, 36, offset);  putLine(h, 8, 34, 11, 34, offset); putLine(h, 8, 34, 8, 36, offset); putLine(h, 10, 36, 8, 36, offset)  # phi 2
        putBox(h, 10, 36, 12, 38, offset); putLine(h, 10, 36, 10, 38, offset)  # phi 3
        putBox(h, 8, 38, 12, 40, offset);  putLine(h, 10, 38, 8, 38, offset); putLine(h, 8, 38, 8, 40, offset); putLine(h, 10, 40, 8, 40, offset)  # phi 4
        putBox(h, 10, 40, 12, 42, offset); putLine(h, 10, 40, 10, 42, offset)  # phi 5
        putBox(h, 8, 42, 12, 44, offset);  putLine(h, 10, 42, 8, 42, offset); putLine(h, 8, 42, 8, 44, offset); putLine(h, 10, 44, 8, 44, offset)  # phi 6
        putBox(h, 10, 44, 12, 46, offset); putLine(h, 10, 44, 10, 46, offset)  # phi 7
        putBox(h, 8, 46, 12, 48, offset);  putLine(h, 10, 46, 8, 46, offset); putLine(h, 8, 46, 8, 48, offset); putLine(h, 11, 48, 8, 48, offset)  # phi 8
        putBox(h, 11, 48, 12, 50, offset); putLine(h, 11, 48, 11, 50, offset)  # phi 9
        putBox(h, 8, 50, 12, 52, offset);  putLine(h, 11, 50, 8, 50, offset); putLine(h, 8, 50, 8, 52, offset); putLine(h, 10, 52, 8, 52, offset)  # phi 10
        putBox(h, 10, 52, 12, 54, offset); putLine(h, 10, 52, 10, 54, offset)  # phi 11
        putBox(h, 8, 54, 12, 56, offset);  putLine(h, 10, 54, 8, 54, offset); putLine(h, 8, 54, 8, 56, offset); putLine(h, 10, 56, 8, 56, offset)  # phi 12
        putBox(h, 10, 56, 12, 58, offset); putLine(h, 10, 56, 10, 58, offset)  # phi 13
        putBox(h, 8, 58, 12, 60, offset);  putLine(h, 10, 58, 8, 58, offset); putLine(h, 8, 58, 8, 60, offset); putLine(h, 10, 60, 8, 60, offset)  # phi 14
        putBox(h, 10, 60, 12, 62, offset); putLine(h, 10, 60, 10, 62, offset)  # phi 15
        putBox(h, 8, 62, 12, 64, offset);  putLine(h, 10, 62, 8, 62, offset); putLine(h, 8, 62, 8, 64, offset); putLine(h, 11, 64, 8, 64, offset)  # phi 16

        putBox(h, 11, 64, 12, 96, offset); putLine(h, 11, 64, 11, 96, offset); putLine(h, 12, 96, 11, 96, offset)


def putBox(h, x1, y1, x2, y2, offset):
    box = TBox(x1-offset, y1*0.5, x2-offset, y2*0.5)
    box.SetFillColor(kGray)
    box.SetLineColor(kGray)
    h.GetListOfFunctions().Add(box)


def putLine(h, x1, y1, x2, y2, offset, c=1):
    line = TLine(x1-offset, y1*0.5, x2-offset, y2*0.5)
    line.SetLineColor(c)
    h.GetListOfFunctions().Add(line)

tubeLenght_dict = {
    "BEE": 0.9115,
    "BIS": 1.6715,
    "BIS8": 0.8515,
    "BIL": 2.6715,
    "BIM" : 1.5365,
    "BIR" : 1.1055,
    "BIR1" : 2.6715,
    "BIR2" : 1.5365,
    "BIR4": 1.5365,
    "BIR5" : 1.5365,
    "BIR3" : 1.1055,
    "BIR6" : 1.1055,
    "BME" : 2.15, #approximate!                                                                                                                                   
    "BMG" : 1.12, #approximate!                                                                                                               
    "BML" : 3.5515,
    "BMS" : 3.0715,
    "BMF" : 3.0715,
    "BOL" : 4.9615,
    "BOG" : 3.7715,
    "BOS": 3.7733,
    "BOF" : 3.7733,
    "EES1" : 2.1945,
    "EES2" : 2.5905,
    "EEL105": 2.6265,
    "EEL1" : 4.2015,
    "EEL2" : 3.6015,
    "EIS1" : 1.0605,
    "EIS2" : 1.4115,
    "EIL1" : 1.5465,
    "EIL2" : 2.0865,
    "EIL311" : 1.7415,
    "EIL315" : 1.7415,
    "EIL3" : 2.0715,
    "EIL411" : 1.5515,
    "EIL415" : 1.5515,
    "EIL401" : 1.5365,
    "EIL409" : 1.5365,
    "EIL403" : 2.8015,
    "EIL405" : 2.8015,
    "EIL413" : 2.8015,
    "EIL407" : 2.0115,
    "EIL501" : 2.8015,
    "EIL509" : 2.8015,
    "EML1" : 1.5465,
    "EML2" : 2.4465,
    "EML3" : 3.4065,
    "EML4" : 4.3665,
    "EML5" : 5.3265,
    "EMS1" : 1.0875,
    "EMS2" : 1.6635,
    "EMS3" : 2.2395,
    "EMS4" : 2.8155,
    "EMS5" : 3.3915,
    "EOL1" : 2.0415,
    "EOL2" : 3.0015,
    "EOL3" : 3.7815,
    "EOL4" : 4.5015,
    "EOL5" : 5.2215,
    "EOL6" : 5.9415,
    "EOS1" : 1.4655,
    "EOS2" : 1.9695,
    "EOS3" : 2.4735,
    "EOS4" : 2.9415,
    "EOS5" : 3.3735,
    "EOS6" : 3.8055,
}

def getTubeLength( name ):

    name03=name[0:3]
    name04=name[0:4]
    name56= name[5:7]

    tubeLength = 4.9615

    tubeLength = tubeLenght_dict.get(name03, tubeLength)
    tubeLength = tubeLenght_dict.get(name04, tubeLength)
    tubeLength = tubeLenght_dict.get(name03+name56, tubeLength)
    tubeLength = tubeLenght_dict.get(name04+name56, tubeLength)

    return tubeLength 


def MDT2DHWName(name):
   statphi_s = name[5:7]
   eta_s = name[3:4]
   #ME1[AC]14 (mistake - is actually 13 - so account for both cases!) -->BME4[AC]13 in histogram position
   if(name[0:3]=="BME"):
      eta_s = "4"
      statphi_s = "13"

   statphi_c = statphi_s+" 1"
   statphi_c2 = statphi_s+" 2"
   if(name[0:3]=="BIR" or name[0:3]=="BIM") :
      statphi_c = statphi_c +" "+name[2:3]
      statphi_c2 = statphi_c2 +" "+name[2:3]

   #BML[45][AC]13-->BML[56][AC]13
   if(name[0:3] == "BML" and int(name[3:4])>=4 and name[5:7] == "13"):
      eta_s = str(int(name[3:4])+1)

   #BMF1,2,3 corresponds to eta stations 1,3,5
   if( name[0:3] == "BMF"):
      eta_s=str(int(name[3:4])*2-1)

   stateta_c = name[0:2]
   stateta_c += name[4:5]
   stateta_c += eta_s
   
   return (stateta_c, statphi_c, statphi_c2)


def MDTTubeEff(name,hi_num,hi_den):
   nBinsNum = hi_num.GetNbinsX()
   BinsML1 = nBinsNum/2
   if(name[0:4]=="BIS8" or name[0:2]=="BE"):
      BinsML1=nBinsNum
   if(nBinsNum!=hi_den.GetNbinsX()):
      return
   countsML1=0
   countsML2=0
   entriesML1=0
   entriesML2=0
   for ibin in range(nBinsNum):
      entries    = hi_den.GetBinContent(ibin)
      counts     = hi_num.GetBinContent(ibin)
      if(ibin<BinsML1 or BinsML1==nBinsNum) :
         countsML1+=counts
         entriesML1+=entries
      else:
         countsML2+=counts
         entriesML2+=entries

   return (countsML1, countsML2, entriesML1, entriesML2)


def fittzero(x, par):
   fitvaltzero = par[0] + ( par[3] / ( 1 + ( TMath.Exp((-x[0]+par[1])/par[2]) ) ) )
   return fitvaltzero

tf1_fittzero = TF1("func1", fittzero, 0., 200., 4)


def fittmax(x, par):
   fitvaltmax = par[0] + ( par[3] / ( 1 + ( TMath.Exp((x[0]-par[1])/par[2]) ) ) )
   return fitvaltmax

tf1_fittmax = TF1("func2", fittmax, 0., 200., 4)

def MDTFitTDC(h):
   t0 = 0
   tmax = 0
   t0err = 0
   tmaxerr = 0
   up = h.GetBinCenter(h.GetMaximumBin()+1)
   if( up > 200 ):
      up = 200
   down = up + 650
   if( up < 50 ):
      up = 50
   parESD0 = h.GetBinContent(h.GetMinimumBin())
   parESD1 = up
   parESD2 = 20
   parESD3 = h.GetBinContent(h.GetMaximumBin()) - h.GetBinContent(h.GetMinimumBin())
   func1 = tf1_fittzero
   func1.SetRange(0., up)
   func1.SetParameters(parESD0, parESD1, parESD2, parESD3)
   func1.SetLineColor(kBlue+2)
   if(h.GetEntries()>100):
      h.Fit("func1","RQN")
      t0 = func1.GetParameter(1)
      t0err = func1.GetParError(1)
      binAtT0 = h.GetBinContent(h.FindBin(t0))
      if(binAtT0<1):
         binAtT0 = 1
      # // to additionally account for bad fits                                                                                                                                                         
      if(func1.GetNumberFitPoints()!=0):
         t0err += 10.0 * func1.GetChisquare() / (0.01*binAtT0*binAtT0*func1.GetNumberFitPoints())

   parESD0 = h.GetBinContent(h.GetMinimumBin())
   parESD1 = down
   parESD2 = 50
   parESD3 = (h.GetBinContent(h.GetMaximumBin())-h.GetBinContent(h.GetMinimumBin()))/10.
   func2 = tf1_fittmax
   func2.SetRange((down-135), (down+135))
   func2.SetParameters(parESD0,parESD1,parESD2,parESD3)
   func2.SetLineColor(kRed+1)
   if(h.GetEntries()>100):
      func2.SetParLimits(0, parESD0, 2.0*parESD0+1)
      func2.SetParLimits(2, 5, 90)
      func2.SetParLimits(3, 0.2*parESD3, 7*parESD3)
      h.Fit("func2","WWRQN+")
      tmax = func2.GetParameter(1)
      tmaxerr = func2.GetParError(1)
      binAtTmax = h.GetBinContent(h.FindBin(tmax))
      if(binAtTmax<1):
         binAtTmax = 1
      # to additionally account for bad fits                                                                                                                                                            
      tmaxerr += 10.0 * func2.GetChisquare() / (0.01*binAtTmax*binAtTmax*func2.GetNumberFitPoints())

   return [t0, t0err, tmax, tmaxerr]


