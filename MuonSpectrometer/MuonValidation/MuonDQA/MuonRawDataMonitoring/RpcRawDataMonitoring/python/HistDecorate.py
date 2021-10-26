#
#Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration                                                                                           
#

import ROOT

Dic_etaStation_dbZ = {
  1:[1,2], 
  2:[1,2,3], 
  3:[1,2], 
  4:[1,2,3], 
  5:[1,2], 
  6:[1,2], 
  7:[1,2], 
  8:[1,2]
}

#############################################################################
xTitleOffset = {"TH1"      : 0.9,
              "TH2"         : 0.9,
              "TGraph"      : 1.0,
              "TEfficiency" : 1.0
}
yTitleOffset = {"TH1"      : 1.6,
              "TH2"         : 1.0,
              "TGraph"      : 1.6,
              "TEfficiency" : 1.6
}
zTitleOffset = {"TH1"      : 0.,
              "TH2"         : 1.2,
              "TGraph"      : 0.,
              "TEfficiency" : 0.
}


# -----------------------------------------------------------------------
def classifyInstance(hist):
    if isinstance(hist, ROOT.TH1D) or isinstance(hist, ROOT.TH1I) or isinstance(hist, ROOT.TProfile) or isinstance(hist, ROOT.TH1F):
      instance = "TH1"
    elif isinstance(hist, ROOT.TH2D) or isinstance(hist, ROOT.TH2I) or isinstance(hist, ROOT.TH2F) or isinstance(hist, ROOT.TProfile2D):
      instance = "TH2"
    elif isinstance(hist, ROOT.TGraph) or isinstance(hist, ROOT.TGraphErrors):
      instance = "TGraph"
    elif isinstance(hist, ROOT.TEfficiency):
      instance = "TEfficiency"

    return instance

# -----------------------------------------------------------------------
def setTitleOffset(hist, instance=""):
  if instance == "":
    instance = classifyInstance(hist)

  if instance == "TEfficiency":
    hist.GetPaintedGraph().GetXaxis().SetTitleOffset(xTitleOffset[instance])
    hist.GetPaintedGraph().GetYaxis().SetTitleOffset(yTitleOffset[instance])
  else:
    hist.GetXaxis().SetTitleOffset(xTitleOffset[instance])
    hist.GetYaxis().SetTitleOffset(yTitleOffset[instance])
    if instance == "TH2":
      hist.GetZaxis().SetTitleOffset(zTitleOffset[instance])


# -----------------------------------------------------------------------
def set_verticalGrid(hist2d):
    y_min = 0.
    y_max = 32.
    dashlines_x  = [-18., -16., -14., -12., -10., -9., -7., -5., -4., -2., 2., 4., 5., 7., 9., 10., 12., 14., 16., 18.]
    solidlines_x = [-17., -15., -13., -11., -8.,  -6., -3., -1., 1., 3., 6., 8., 11., 13., 15., 17.]

    for i_x in dashlines_x:
      dash_line = ROOT.TLine(i_x, y_min, i_x, y_max)
      dash_line.SetLineStyle(2) 
      dash_line.SetLineWidth(1) 
      dash_line.SetLineColor(ROOT.kBlack) 

      hist2d.GetListOfFunctions().Add(dash_line)

    for i_x in solidlines_x:
      solid_line = ROOT.TLine(i_x, y_min, i_x, y_max)
      solid_line.SetLineStyle(1) 
      solid_line.SetLineWidth(1) 
      solid_line.SetLineColor(ROOT.kBlack) 

      hist2d.GetListOfFunctions().Add(solid_line)

  # -----------------------------------------------------------------------
def set_horizonGrid(hist2d):
    xmin  = -19.
    xmax  = 19.
    for i_sec in range(1, 16+1):
      solid_line = ROOT.TLine(xmin, i_sec*2, xmax, i_sec*2)
      solid_line.SetLineStyle(1) 
      solid_line.SetLineWidth(1)
      solid_line.SetLineColor(ROOT.kBlack)
      hist2d.GetListOfFunctions().Add(solid_line)

      dash_line = ROOT.TLine(xmin, i_sec*2-1, xmax, i_sec*2-1)
      dash_line .SetLineStyle(2)
      dash_line .SetLineWidth(1)
      dash_line .SetLineColor(ROOT.kBlack) 
      hist2d.GetListOfFunctions().Add(dash_line)

#############################################################################
def setAxisLabel_EtaPhi(hist2d):
  x_axis = hist2d.GetXaxis()
  y_axis = hist2d.GetYaxis()

  XBins_Labeled = { # bin : eta station
    1 : '-8', 
    3 : '-7', 
    5 : '-6', 
    7 : '-5', 
    10: '-4', 
    12: '-3', 
    15: '-2', 
    17: '-1', 
    19:  '0', 
    21:  '1', 
    24:  '2', 
    26:  '3', 
    29:  '4', 
    32:  '5', 
    34:  '6', 
    36:  '7', 
    38:  '8'
  }

  YBins_Labeled = { # bin : Sector name
    1:"L1",
    2:"S2",
    3:"L3",
    4:"S4",
    5:"L5",
    6:"S6",
    7:"L7",
    8:"S8",
    9:"L9",
    10:"S10",
    11:"L11",
    12:"FG12",
    13:"L13",
    14:"FG14",
    15:"L15",
    16:"S16"
  }

  for i_bin, i_label in XBins_Labeled.items():
    x_axis.SetBinLabel(i_bin, i_label)

  for i_bin, i_label in YBins_Labeled.items():
    y_axis.SetBinLabel(i_bin*2-1, i_label)

  hist2d.GetXaxis().SetTitleOffset(0.9)
  hist2d.GetYaxis().SetTitleOffset(1.0)
  hist2d.GetZaxis().SetTitleOffset(1.2)

#############################################################################
if __name__ ==  '__main__':
  print ("HistDecorate:  Hello, World !")
