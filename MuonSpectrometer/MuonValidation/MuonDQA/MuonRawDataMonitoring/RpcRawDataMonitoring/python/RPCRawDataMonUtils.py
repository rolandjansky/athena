#
#Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration                                                                                           
#

import ROOT

#############################################################################
def creatGraph(xs, ys, x_errs, y_errs, g_name, g_title, g_Xtitle, g_Ytitle):
  import array
  x_arr = array.array("f",xs)
  y_arr = array.array("f",ys)
  x_err_arr = array.array("f",x_errs)
  y_err_arr = array.array("f",y_errs)

  g1 =  ROOT.TGraphErrors(len(x_arr),x_arr,y_arr, x_err_arr, y_err_arr)

  g1.SetNameTitle(g_name, g_title)
  g1.SetMarkerSize(0.5)
  g1.GetXaxis().SetTitle(g_Xtitle)
  g1.GetYaxis().SetTitle(g_Ytitle)    
  g1.GetYaxis().SetRangeUser(0.8*min(y_arr),1.2*max(y_arr))

  return g1

#############################################################################
  # return type values:
  # 1  : normal
  # 0  : fit fail or (Not exist this quantity of  certain panel)
  # -1 : number of points < 2
  # -2 : Chi2/NDF < 0.0001
  # -3 : NDF == 0 except the reason number of points <2
def linearFit(h_temp, opt="QNS+"):
  default_dic = {"p0": 0., "p0_err":0., "p1": -1., "p1_err":-1., "chi2":-1., "mean":0., "mean_err":0.}
  # Qtag, un-fit-able
  # 0 -- normal
  # 1 -- N points = 1
  # 2 -- N points = 2
  # 3 -- N points = 0

  Qtag = 0
  if h_temp.GetN()< 2:
    Qtag = 1
    return (Qtag, default_dic)

  # fit_result = h_temp.Fit("pol1", opt, "", 0, 2.2)
  fit_result = h_temp.Fit("pol1", opt)

  par_a   = round(fit_result.Value(0), 2)
  par_a_E = round(fit_result.ParError(0), 2)
  par_b   = round(fit_result.Value(1), 2)
  par_b_E = round(fit_result.ParError(1), 2)

  if fit_result.Ndf() == 0:
    Qtag = 2
    chi2  = 0
  else:
    chi2    = round(fit_result.Chi2()/fit_result.Ndf(), 2)

  mean    = round(h_temp.GetMean(2), 2)
  mean_rms= round(h_temp.GetRMS(2), 2)

  # print("par_a = ", par_a, "; par_b = ", par_b, "; chi2=", chi2)
  dic = {"p0": par_a, "p0_err":par_a_E, "p1": par_b, "p1_err":par_b_E, "chi2":chi2, "mean":mean, "mean_err":mean_rms}
  return (Qtag, dic)

# ================================================================================================
## Panel class
# ================================================================================================
class Panel():
  panel_name = ''

  # -----------------------------------------------------------------------
  def __init__(self, properties, index):
  # def __init__(self, panel_name):
    self.index  = index
    self.setPanelProp(properties)

  # -----------------------------------------------------------------------
  def getSector(self):
    myphi_part = (2 * self.stationPhi) - 1
    if (self.stationName == 3 or self.stationName == 5 or self.stationName == 8 or self.stationName == 9 or self.stationName == 10) :
      myphi_part += 1

    sector = myphi_part
    if   self.stationEta < 0:
      sector = -sector
  
    if (self.stationName == 2 or self.stationName == 3 or self.stationName == 8 or self.stationName == 53) :
      layer = (self.doubletR-1)*2 + self.gasGap
    else:
      layer = 4+(self.doubletR-1)*2 + self.gasGap
    
    return  (sector, layer)

  # -----------------------------------------------------------------------
  def getPanelName(self):
    self.stationNames  = {2:'BML', 3:'BMS', 4:'BOL', 5:'BOS', 8:'BMF' , 9:'BOF', 10:'BOG', 53:'BME'}

    str_name = self.stationNames[self.stationName]
    str_eta  =        str(abs(self.stationEta))
    str_dPhi = "DP" + str(self.doubletPhi) + "."
    str_gap  = "Ly" + str(self.gasGap - 1) + "."
    str_dZ   = "DZ" + str(self.doubletZ)

    myphi_part = (2 * self.stationPhi) - 1

    if   self.stationEta > 0:
      str_side = "A"
    elif self.stationEta < 0:
      str_side = "C"
    else:
      str_side = "N"  # N: NULL, "ETA NOT DEFINED"

    if (self.stationName == 3 or self.stationName == 5 or self.stationName == 8 or self.stationName == 9 or self.stationName == 10) :
      myphi_part += 1

    if myphi_part < 10: 
      str_phi = "0" + str(myphi_part)
    else:
      str_phi =       str(myphi_part)

    if   ((self.stationName == 2  and self.doubletR == 1) or
          (self.stationName == 3  and self.doubletR == 1) or
          (self.stationName == 53 and self.doubletR == 1) or
          (self.stationName == 8  and self.doubletR == 1)):
      str_PICO = ".CO."
    elif ((self.stationName == 2  and self.doubletR == 2) or 
          (self.stationName == 3  and self.doubletR == 2) or 
          (self.stationName == 53 and self.doubletR == 2) or 
          (self.stationName == 8  and self.doubletR == 2)):
      str_PICO = ".PI."
    elif ((self.stationName == 4  and self.doubletR == 1) or
          (self.stationName == 5  and self.doubletR == 1) or 
          (self.stationName == 9  and self.doubletR == 1) or
          (self.stationName == 10 and self.doubletR == 1)):
      str_PICO = ".CO."
    else:
      str_PICO = ".NU"+str(self.doubletR)+"." # NU: NULL, Not defined

    if(self.measPhi == 0): 
      str_ETAPHI = "ETA."
    else: 
      str_ETAPHI = "PHI."

    self.panel_name = str_name + str_eta + str_side + str_phi + str_PICO + str_dPhi + str_gap + str_dZ + str_ETAPHI
    return self.panel_name

  # -----------------------------------------------------------------------
  def setPanelProp(self, properties):
    self.properties = properties

    self.stationName = self.properties["stationName"]
    self.stationPhi  = self.properties["stationPhi"]
    self.stationEta  = self.properties["stationEta"]
    self.doubletR    = self.properties["doubletR"]
    self.doubletPhi  = self.properties["doubletPhi"]
    self.doubletZ    = self.properties["doubletZ"]
    self.gasGap      = self.properties["gasGap"]
    self.measPhi     = self.properties["measPhi"]

  # -----------------------------------------------------------------------
  def getLocalPos(self):
    etaStation = abs(self.stationEta)
    (sector, layer) = self.getSector()

    if self.stationName == 10 : #'BOG':
      etaStation = 2*etaStation
    elif self.stationName == 9:
      etaStation = 2*etaStation-1

    if abs(sector) == 13 and self.stationName == 2: #BME BML
      etaStation += 1

    return [etaStation, self.doubletZ, self.doubletPhi, self.measPhi]

#############################################################################
if __name__ ==  '__main__':
  print ("RPCRawDataMonUtils:  Hello, World !")