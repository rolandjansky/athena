#
#Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration                                                                                           
#

import ROOT

### python ####
import math
# import numpy
# import abc

### local ####
import RpcRawDataMonitoring.RPCRawDataMonUtils as RPCRawDataMonUtils
import RpcRawDataMonitoring.HistDecorate as HistDecorate

colors = [ROOT.kBlack, ROOT.kPink-6, ROOT.kOrange-9, ROOT.kRed, ROOT.kAzure+7, ROOT.kCyan-3, ROOT.kGreen+2, ROOT.kYellow, ROOT.kBlack, ROOT.kBlue]
markers= [20, 24, 21, 25, 22, 26, 23, 32, 33, 27]

# _type1_sectors = [-16,-15,-13,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,1,2,3,4,5,6,7,8,9,10,11,13,15,16]
# _type2_sectors = [-14,-12,12,14]

layerNames = {1:"BM-Confirm-Gasgap1", 2:"BM-Confirm-Gasgap2", 3:"BM-Pivot-Gasgap1", 4:"BM-Pivot-Gasgap2", 5:"BO-Confirm-Gasgap1", 6:"BO-Confirm-Gasgap2", 7:"BOF_BOG-dR2-Gasgap1", 8:"BOF_BOG-dR2-Gasgap2"}
isMeasPhiNames = ["#eta view", "#phi view"]
timeTagName = { "All" : "Whole readout window(200ns)",
                "B3"  : "3 BC before BC0(-100 ~ -25ns)",
                "C1"  : "BC0(-25ns ~ 25ns)",
                "A3"  : "3 BC after BC0(25ns ~ 100 ns)"
              }

Bin_EtaDbZ = {
  # (etaStation, dbZ): bin
  (-8,  2): 1,    (-8,  1): 2,
  (-7,  2): 3,    (-7,  1): 4,
  (-6,  2): 5,    (-6,  1): 6,
  (-5,  2): 7,    (-5,  1): 8,
  (-4,  3): 9,    (-4,  2): 10,   (-4,  1): 11,
  (-3,  2): 12,   (-3,  1): 13,
  (-2,  3): 14,   (-2,  2): 15,   (-2,  1): 16,
  (-1,  2): 17,   (-1,  1): 18,
  ( 0,  2): 19,   ( 0,  1): 20,
  ( 1,  1): 21,   ( 1,  2): 22,
  ( 2,  1): 23,   ( 2,  2): 24,   ( 2,  3): 25,
  ( 3,  1): 26,   ( 3,  2): 27,
  ( 4,  1): 28,   ( 4,  2): 29,   ( 4,  3): 30,
  ( 5,  1): 31,   ( 5,  2): 32,
  ( 6,  1): 33,   ( 6,  2): 34,
  ( 7,  1): 35,   ( 7,  2): 36,
  ( 8,  1): 37,   ( 8,  2): 38,
}


#############################################################################
class Draw_Base(object):

  _rangeOfVariables_perPanel = {
    "p0"                  : (0., 1e5),
    "p1"                  : (0., 5e5),
    "chi2"                : (0., 100.),
    "predRate"            : (0., 1e6),
    "meanRate"            : (0., 1e6),
    "extrapMuons"         : (0., 2000),
    "detEff"              : (0., 1.),
    "detEffError"         : (0., 0.1),
    "hitMultiplicity"     : (-0.5, 10.5),
    "averageClusterSize"  : (1, 4),
    "outTimeHitFrac"      : (0., 1.),
    "outTimeHitFrac_onTrack" : (0., 1.),
    "outTimeHitFracError" : (0., 0.1)
  }

  _rangeOfVariables_allPanels = {
    "p0"                  : (0., 20e6),
    "p1"                  : (0., 20e6),
    "chi2"                : (0., 100.),
    "predRate"            : (0., 30e6),
    "meanRate"            : (0., 30e6),
    "extrapMuons"         : (0., 2000),
    "detEff"              : (0., 1.),
    "detEffError"         : (0., 0.1),
    "hitMultiplicity"     : (-0.5, 10.5),
    "averageClusterSize"  : (1, 4),
    "outTimeHitFrac"      : (0., 1.),
    "outTimeHitFrac_onTrack" : (0., 1.),
    "outTimeHitFracError" : (0., 0.1)
  }

  _nbins_hist = {
    "p0"                  : 20,
    "p1"                  : 20,
    "chi2"                : 20,
    "predRate"            : 20,
    "meanRate"            : 20,
    "extrapMuons"         : 20,
    "detEff"              : 20,
    "detEffError"         : 20,
    "hitMultiplicity"     : 10,
    "averageClusterSize"  : 30,
    "outTimeHitFrac"      : 20,
    "outTimeHitFrac_onTrack" : 20,
    "outTimeHitFracError" : 20
  }

  _unitsOfVariables = {
    "p0"                  : 'Hz',
    "p1"                  : 'Hz/(10^{34} cm^{-2}s^{-1})',
    "chi2"                : '',
    "predRate"            : 'Hz',
    "meanRate"            : 'Hz',
    "extrapMuons"         : '',
    "detEff"              : '',
    "detEffError"         : '',
    "hitMultiplicity"     : '',
    "averageClusterSize"  : '',
    "outTimeHitFrac"      : '',
    "outTimeHitFrac_onTrack" : '',
    "outTimeHitFracError" : ''
  }

  _precision = {
    "p0"                  : -3,
    "p1"                  : -3,
    "chi2"                : 2,
    "predRate"            : -3,
    "meanRate"            : -3,
    "extrapMuons"         : 0,
    "detEff"              : 4,
    "detEffError"         : 4,
    "hitMultiplicity"     : 2,
    "averageClusterSize"  : 3,
    "outTimeHitFrac"      : 4,
    "outTimeHitFrac_onTrack" : 4,
    "outTimeHitFracError" : 4
  }

  _var_title = {
    "p0"                  : "p0"       ,
    "p1"                  : "p1"       ,
    "chi2"                : "Chi2/ndf"     ,
    "predRate"            : "Predicted hit rate" ,
    "meanRate"            : "Mean actual hit rate" ,
    "extrapMuons"         : "Extrapolated muons",
    "detEff"              : "Muon detection efficiency",
    "detEffError"         : "Muon detection efficiency error",
    "hitMultiplicity"     : "RPC hit multiplicity",
    "averageClusterSize"  : "Average cluster size",
    "outTimeHitFrac"      : "Fraction of RPC hits with |t|>12.5 ns",
    "outTimeHitFrac_onTrack" : "Fraction of RPC on-track hits with |t|>12.5 ns",
    "outTimeHitFracError" : "Error of fraction of RPC hits with |t|>12.5 ns"
  }

  _cut_variables = ["p0", "p1", "chi2", "predRate", "meanRate"]

  _cuts_summaryHist_allSectorLayer = {
    "p0"       : 2.e7,
    "p1"       : 0.,
    "chi2"     : 30. ,
    "predRate" : 3.e7,
    "meanRate" : 3.e7
  }

  _cuts_summaryHist_perSectorLayer = {
    "p0"       : 5.e4,
    "p1"       : 0.,
    "chi2"     : 25. ,
    "predRate" : 1.e6,
    "meanRate" : 8.e5
  }

  _in_hname = ''

  _full_panel = True
  _list_outPanelInds  = []
  _list_outPanelNames = []
  _hists_bad_panels   = []

  # Threshold for sector&layer
  T_chi2_sl = 15.
  T_p0_sl = 2e6
  Tlow_p1_sl = 5e3
  Tup_p1_sl = 15e6
  T_predRate = 30e6

  # Threshold for panels
  T_chi2_panel = 15.
  T_p0_panel = 2e5
  Tlow_p1_panel = 5e3
  Tup_p1_panel = 3e5
  T_predRate_panel = 30e6

  # -----------------------------------------------------------------------
  def __init__(self, inHisto):
    self._in_histo          = inHisto
    self._dic_panels_detect  = {}
    self.doneParserPanel     = ''
    self.doneParserSectorAndLayer = ''
  
  # -----------------------------------------------------------------------
  def _prepareHisto(self):
    self._out_htitle  = self._var_title[self._variable]

    nbins_X     = self._in_histo.GetNbinsX()
    self._nbins = nbins_X
    self._low   = self._in_histo.GetXaxis().GetXmin()
    self._up    = self._in_histo.GetXaxis().GetXmax()

  # -----------------------------------------------------------------------
  def parseHisto_allPanels(self):
    self.doneParserPanel     = self._in_hname
    self._prepareHisto()

    for p_ind, panel_properties in self._PanelsProperties.items():
      i_panel          = RPCRawDataMonUtils.Panel(panel_properties, p_ind)
      panel_name       = i_panel.getPanelName()
      
      panelLocalPar = i_panel.getLocalPos()
      sec_layer     = i_panel.getSector()
      i_etaSta      = panelLocalPar[0]
      i_dbZ         = panelLocalPar[1]
      i_dbPhi       = panelLocalPar[2]
      i_measPhi     = panelLocalPar[3]

      ##
      ## First level: sector - layer(dbR+gasGap)
      ##
      try:
        sec_layer_detect = self._dic_panels_detect[sec_layer]
      except KeyError:
        self._dic_panels_detect[sec_layer] = {}
        sec_layer_detect = self._dic_panels_detect[sec_layer]

      ##
      ## Second level: dbPhi - meanPhi
      ##
      try:
        dbPhi_meas_detect = sec_layer_detect[(i_dbPhi, i_measPhi)]
      except KeyError:
        sec_layer_detect[(i_dbPhi, i_measPhi)] = {}
        dbPhi_meas_detect = sec_layer_detect[(i_dbPhi, i_measPhi)]

      ##
      ## Third level: etaStation - dbZ
      ##
      config = [p_ind, panel_name, sec_layer, (i_dbPhi, i_measPhi), (i_etaSta, i_dbZ)]
      dbPhi_meas_detect[(i_etaSta, i_dbZ)] = self.getObject_1panel(config)

      del i_panel

  # -----------------------------------------------------------------------
  def parseHisto_allSectorAndLayers(self):
    self.doneParserSectorAndLayer = self._in_hname
    self._prepareHisto()
    
    temp_dic_sectors_layers = {}
    for p_ind, panel_properties in self._PanelsProperties.items():
      i_panel          = RPCRawDataMonUtils.Panel(panel_properties, p_ind)
      
      panel_name = i_panel.getPanelName()
      sec_layer  = i_panel.getSector()
      i_measPhi  = i_panel.measPhi

      self.getObject_1SectorLayer([p_ind, panel_name, i_measPhi], sec_layer, temp_dic_sectors_layers)
    self._post_allSectorAndLayers(temp_dic_sectors_layers)

  # -----------------------------------------------------------------------
  def _post_allSectorAndLayers(self, temp_dic):
    print ("_post_allSectorAndLayers - do nothing")

  # -----------------------------------------------------------------------
  def GetSummary_allSectorsAndLayers(self, config):
    self._in_hname    = config[0]
    self._variable    = config[1]
    self._out_htitle  = self._var_title[self._variable]

    out_hname  = self._variable
    out_htitle = self._out_htitle
    if len(config) == 3:
      self._timeTag = config[2]
      out_hname     = self._variable+'_'+self._timeTag
      out_htitle    = self._out_htitle+'('+self._timeTag+')'

    if not self.doneParserSectorAndLayer == self._in_hname:
      self.parseHisto_allSectorAndLayers()

    y_unit      = self._unitsOfVariables[self._variable]
    y_title     = out_htitle
    if y_unit != '':
      y_title = out_htitle+'['+y_unit+']'

    dic_hist = {}
    for i_layer in range(1, 8+1):
      h_name  = "Summary_layer"+str(i_layer)+"_"+out_hname
      h_title = "Summary of "+out_htitle.lower()+'('+layerNames[i_layer]+')'

      h1_all  = ROOT.TH1F(h_name, h_title+";Sector;"+y_title,   33, -16.5, 16.5)
      dic_hist[i_layer] = h1_all

    for sec_lay, obj_perSecLay in self._dic_sectors_layers.items():
      contentAndErr =  self.getContent(obj_perSecLay)
      i_bin = sec_lay[0] + 17

      dic_hist[sec_lay[1]].SetBinContent(i_bin, contentAndErr[0])
      dic_hist[sec_lay[1]].SetBinError(i_bin, contentAndErr[1])

    return self.decorateAllSectorSummary(dic_hist)

  # -----------------------------------------------------------------------
  def GetSummary_eachSectorsAndLayers(self, config):
    self._in_hname    = config[0]
    self._variable    = config[1]
    self._out_htitle  = self._var_title[self._variable]

    out_hname  = self._variable
    out_htitle = self._out_htitle
    if len(config) == 3:
      self._timeTag  = config[2]
      out_hname  = self._variable+'_'+self._timeTag
      out_htitle = self._out_htitle+'('+self._timeTag+')'

    if not self.doneParserPanel == self._in_hname:
      self.parseHisto_allPanels()

    y_unit      = self._unitsOfVariables[self._variable]
    y_title     = self._out_htitle
    if y_unit != '':
      y_title = self._out_htitle+'['+y_unit+']'

    list_hists = []
    for i_sector, dic_perSecLayer in self._dic_panels_detect.items():
      dic_hist            = {}
      sector_layer_name   = ("Summary_Sector%d_Layer%d" %(i_sector[0], i_sector[1]))
      sector_layer_title  = "Summary of "+out_htitle.lower()+" of panels in sector"+str(i_sector[0])+'-'+layerNames[i_sector[1]]
      
      for i_dbPhi_meas, dic_dbPhi_meas in dic_perSecLayer.items():
        h_name  = sector_layer_name+("_dbPhi%d_measPhi%d" %(i_dbPhi_meas[0], i_dbPhi_meas[1]))+"_"+out_hname
        h_title = sector_layer_title+"-dbPhi"+str(i_dbPhi_meas[0])+'-'+isMeasPhiNames[i_dbPhi_meas[1]]
        hist = ROOT.TH1F(h_name, h_title+";(#eta station, dbZ);"+y_title, 19, 0, 19)

        for i_eta_dbZ, object_perPanel in dic_dbPhi_meas.items():
          if i_eta_dbZ[0] > 4:
            x_bin = (i_eta_dbZ[0]-1)*2+i_eta_dbZ[1]+3
          elif i_eta_dbZ[0] > 2:
            x_bin = (i_eta_dbZ[0]-1)*2+i_eta_dbZ[1]+2
          else:
            x_bin = (i_eta_dbZ[0]-1)*2+i_eta_dbZ[1]+1
          
          cont_error = self.getContent(object_perPanel)

          hist.SetBinContent(x_bin, cont_error[0])
          hist.SetBinError(x_bin, cont_error[1])

        self.modifyXlabel(hist)
        dic_hist[i_dbPhi_meas] = hist

      list_hists += self.decorateSummary_perSector(dic_hist)

    return list_hists

  # -----------------------------------------------------------------------
  def GetHist1D_ySectorsAndLayers(self, config):
    self._in_hname    = config[0]
    self._variable    = config[1]
    self._out_htitle  = self._var_title[self._variable]

    if len(config) == 3:
      self._timeTag  = config[2]
      out_hname  = self._variable+'_'+self._timeTag
      out_htitle = self._out_htitle+'('+self._timeTag+')'
    else:
      out_hname  = self._variable
      out_htitle = self._out_htitle

    if not self.doneParserSectorAndLayer == self._in_hname:
      self.parseHisto_allSectorAndLayers()

    x_unit      = self._unitsOfVariables[self._variable]
    x_title     = out_htitle
    if x_unit != '':
      x_title = out_htitle+'['+x_unit+']'
    
    nbins = self._nbins_hist[self._variable]
    x_min = self._rangeOfVariables_allPanels[self._variable][0]
    x_max = self._rangeOfVariables_allPanels[self._variable][1]

    h_name  = out_hname+"_per_sectors_per_layers"
    h_title = out_htitle+" per sectors per layers"

    all_hist = ROOT.TH1F(h_name+"_etaAndPhiPanels", h_title+"(#eta & #phi view)"+";"  +x_title+";N_{sectors*layers}", nbins, x_min, x_max)
    all_hist.GetXaxis().SetRange(0, nbins+1)
    for sec_lay, obj_perSecLay in self._dic_sectors_layers.items():
      contentAndErr =  self.getContent(obj_perSecLay)
      all_hist.Fill(contentAndErr[0])
    
    return [all_hist]

  # -----------------------------------------------------------------------
  def GetHist1D_yPanels(self, config):
    self._in_hname    = config[0]
    self._variable    = config[1]
    self._out_htitle  = self._var_title[self._variable]

    out_hname  = self._variable
    out_htitle = self._out_htitle
    if len(config) == 3:
      self._timeTag  = config[2]
      out_hname  = self._variable+'_'+self._timeTag
      out_htitle = self._out_htitle+'('+self._timeTag+')'

    if not self.doneParserPanel == self._in_hname:
      self.parseHisto_allPanels()

    x_unit      = self._unitsOfVariables[self._variable]
    x_title     = self._out_htitle
    if x_unit != '':
      x_title = self._out_htitle+'['+x_unit+']'

    nbins = self._nbins_hist[self._variable]
    x_min = self._rangeOfVariables_perPanel[self._variable][0]
    x_max = self._rangeOfVariables_perPanel[self._variable][1]

    h_name         = out_hname+"_per_panel"
    h_title        = out_htitle+" per panel"

    h_name_all  = h_name+"_etaAndPhiView"
    h_name_eta  = h_name+"_etaView"
    h_name_phi  = h_name+"_phiView"
    h_title_all = h_title + "(#eta & #phi view)"
    h_title_eta = h_title + "(#eta view)"
    h_title_phi = h_title + "(#phi view)"

    hist_allPanels     = ROOT.TH1F(h_name_all, h_title_all+";"+x_title+";N_{panels}",  nbins, x_min, x_max)
    hist_allPanels_eta = ROOT.TH1F(h_name_eta, h_title_eta+";"+x_title+";N_{panels}",  nbins, x_min, x_max)
    hist_allPanels_phi = ROOT.TH1F(h_name_phi, h_title_phi+";"+x_title+";N_{panels}",  nbins, x_min, x_max)
    hist_allPanels.    GetXaxis().SetRange(0, nbins+1) 
    hist_allPanels_eta.GetXaxis().SetRange(0, nbins+1) 
    hist_allPanels_phi.GetXaxis().SetRange(0, nbins+1)

    for i_sector, dic_perSecLayer in self._dic_panels_detect.items():
      for i_dbPhi_meas, dic_dbPhi_meas in dic_perSecLayer.items():
        for i_eta_dbZ, object_perPanel in dic_dbPhi_meas.items():
          cont_error = self.getContent(object_perPanel)
          hist_allPanels.Fill(cont_error[0])
          if i_dbPhi_meas[1] == 0:
            hist_allPanels_eta.Fill(cont_error[0])
          else:
            hist_allPanels_phi.Fill(cont_error[0])

    return [hist_allPanels, hist_allPanels_eta, hist_allPanels_phi]

  # -----------------------------------------------------------------------
  def GetHist2D_EtaPhi_allLayer(self, config, doSetZRange = True):
    self._in_hname    = config[0]
    self._variable    = config[1]
    self._out_htitle  = self._var_title[self._variable]

    if len(config) == 3:
      self._timeTag  = config[2]
      out_hname  = self._variable+'_'+self._timeTag
      out_htitle = self._out_htitle+'('+self._timeTag+')'
    else:
      out_hname  = self._variable
      out_htitle = self._out_htitle

    if not self.doneParserPanel == self._in_hname:
      self.parseHisto_allPanels()

    self._doSetZRange = doSetZRange

    z_unit      = self._unitsOfVariables[self._variable]
    z_title     = self._out_htitle
    if z_unit != '':
      z_title = self._out_htitle+'['+z_unit+']'

    if self._doSetZRange:
      z_min       = self._rangeOfVariables_perPanel[self._variable][0]
      z_max       = self._rangeOfVariables_perPanel[self._variable][1]

    dic_hists = {}
    for i_layer in range(1, 6+1):
      for i_measPhi in [0, 1]:
        h2_name  = out_hname+"_layer"+str(i_layer)+"_measPhi"+str(i_measPhi)
        h2_title = out_htitle+"("+layerNames[i_layer]+"-"+isMeasPhiNames[i_measPhi]+")"
        h2_temp  = ROOT.TH2D(h2_name, h2_title+";#eta index;#phi sector;"+z_title, 38, -19, 19, 32, 0, 32)
        dic_hists[(i_layer, i_measPhi)] = h2_temp

    for i_sector, dic_perSecLayer in self._dic_panels_detect.items():
      i_layer = i_sector[1]
      if i_layer > 6:
        i_layer = i_layer-2

      for i_dbPhi_meas, dic_dbPhi_meas in dic_perSecLayer.items():
        h2_temp = dic_hists[(i_layer, i_dbPhi_meas[1])]
        y_bin = (abs(i_sector[0])-1)*2+i_dbPhi_meas[0]

        side = 1
        if i_sector[0] < 0:
          side = -1

        for i_eta_dbZ, object_perPanel in dic_dbPhi_meas.items():
          x_bin = Bin_EtaDbZ[(i_eta_dbZ[0]*side, i_eta_dbZ[1])]
          (content, error) = self.getContent(object_perPanel)

          h2_temp.SetBinContent(x_bin, y_bin, content)

    list_hists = []
    for i_layer_measphi, h2 in dic_hists.items():
      if self._doSetZRange:
        h2.SetMinimum(z_min)
        h2.SetMaximum(z_max)
        h2.GetZaxis().SetRangeUser(z_min, z_max)

      # decorate histgram
      HistDecorate.setTitleOffset(h2, "TH2")
      HistDecorate.set_verticalGrid(h2)
      HistDecorate.set_horizonGrid(h2)
      HistDecorate.setAxisLabel_EtaPhi(h2)

      list_hists.append(h2)
      
    return list_hists

  # -----------------------------------------------------------------------
  def decorateAllSectorSummary(self, dic_hist):
    minY = 1e6
    maxY = 0.
    for i_layer in range(1, 8+1):
      ind = i_layer-1
      i_histo = dic_hist[i_layer]
      i_histo.SetMarkerSize(1.0)
      i_histo.SetMarkerStyle(markers[ind])
      i_histo.SetMarkerColor(colors[ind])
      i_histo.SetLineColor(colors[ind])
      i_histo.SetLineWidth(2)
      
      minY = min(i_histo.GetMinimum(), minY)
      maxY = max(i_histo.GetMaximum(), maxY)

    diffMaxMin = maxY-minY
    list_hist = []
    for i_layer in range(1, 8+1):
      i_histo = dic_hist[i_layer]
      i_histo.GetXaxis().SetTitleOffset(1.0)
      i_histo.GetYaxis().SetTitleOffset(1.0)
      i_histo.GetYaxis().SetRangeUser(minY-0.2*diffMaxMin, maxY+1.5*diffMaxMin)

      if self._variable in self._cut_variables:
        self.setCutLine(i_histo, 0)

      list_hist.append(i_histo)

    return list_hist

  # -----------------------------------------------------------------------
  def decorateSummary_perSector(self, dic_hist):
    ind = 0
    minY = 1e10
    maxY = 0.
    for i_dbPhi_meas, i_histo in dic_hist.items():
      i_histo.SetMarkerSize(1.0)
      i_histo.SetMarkerStyle(markers[ind])
      i_histo.SetMarkerColor(colors[ind])
      i_histo.SetLineColor(colors[ind])
      i_histo.SetLineWidth(2)
      i_histo.GetXaxis().SetTitleOffset(1.0)
      i_histo.GetYaxis().SetTitleOffset(1.2)

      ind += 1
      minY = min(i_histo.GetMinimum(), minY)
      maxY = max(i_histo.GetMaximum(), maxY)

    diffMaxMin = maxY-minY

    ind = 0
    list_hist = []
    for i_dbPhi_meas, i_histo in dic_hist.items():
      i_histo.GetYaxis().SetRangeUser(minY-0.2*diffMaxMin, maxY+1.3*diffMaxMin)

      if self._variable in self._cut_variables:
        self.setCutLine(i_histo, 1)

      list_hist.append(i_histo)

    return list_hist

  # -----------------------------------------------------------------------
  def modifyXlabel(self, hist):
    etaStation_dbZ = [
      '(0,1)',
      '(1,1)', '(1,2)',
      '(2,1)', '(2,2)', '(2,3)',
      '(3,1)', '(3,2)',
      '(4,1)', '(4,2)', '(4,3)',
      '(5,1)', '(5,2)',
      '(6,1)', '(6,2)',
      '(7,1)', '(7,2)',
      '(8,1)', '(8,2)']

    for i_bin in range(1, 19+1):
      hist.GetXaxis().SetBinLabel(i_bin, etaStation_dbZ[i_bin-1])

  # -----------------------------------------------------------------------
  def SetPanelDic(self, dic_panels):
    self._PanelsProperties = dic_panels
  
  # -----------------------------------------------------------------------
  def SetLumiInfoDic(self, dic_lumiInfo):
    self._Dic_LumiInfo = dic_lumiInfo
  
  # -----------------------------------------------------------------------
  def setOut_Panels(self, list_panelInds = [], list_panelNames = []):
    self._list_outPanelInds  = list_panelInds
    self._list_outPanelNames = list_panelNames

    if len(list_panelInds) > 8000 or len(list_panelNames) > 8000 :
      self._full_panel = True
    else:
      self._full_panel = False

  # -----------------------------------------------------------------------
  def GetBadPanels(self):
    return self._hists_bad_panels

  # -----------------------------------------------------------------------
  def setHistnameVariable(self, config):
    self._in_hname    = config[0]
    self._variable    = config[1]
    if len(config) == 3:
      self._timeTag  = config[2]

  # --- parseHisto_allPanels ----------------------------------------------
  def getObject_1panel(self, config):
    raise NotImplementedError

  # --- plotHist_allSectorsAndLayers --------------------------------------
  def getObject_1SectorLayer(self, config, sec_layer, temp_dic):
    raise NotImplementedError

  # --- for occupancy and hit multiplicity --------------------------------------
  def getHist_1SectorLayer(self, config, sec_layer, temp_dic, titles):
    h_temp = self.getHist_1Panel(config)

    if sec_layer in temp_dic:
      temp_dic[sec_layer].Add(h_temp) 
    else:
      all_name_suffix  = ("_Sector%d_Layer%d" %(sec_layer[0], sec_layer[1]))
      all_title_suffix = "(Sector"+str(sec_layer[0])+'-'+layerNames[sec_layer[1]]+")"
      all_hist         = self.initialHist_1Panel(all_name_suffix, all_title_suffix, titles)

      temp_dic[sec_layer] = all_hist
      temp_dic[sec_layer].Add(h_temp)

    del h_temp

  # --- Get histo for 1 panel of one sector ------------------------------
  def getHist_1Panel(self, config):
    raise NotImplementedError

  # --- Get content for plot function ------------------------------------
  def getContent(self, singleObj):
    raise NotImplementedError

#############################################################################
class Draw_Occupancy(Draw_Base):

  # -----------------------------------------------------------------------
  def __init__(self, inHisto):
    super(Draw_Occupancy, self).__init__(inHisto)
    self.__refInstLumi  = 2. # 2*10^{34} [cm^{-2}s^{-1}]
    self.list_graphs_rate_panel       = []

    self._NEvtScale       = False

  # -----------------------------------------------------------------------
  def __getTimeScale(self):
    if  self._timeTag.find("All") > -1:
      timeScale = 200e-9
    elif  (self._timeTag.find("A3") > -1) or (self._timeTag.find("B3") > -1):
      timeScale = 75e-9
    elif  self._timeTag.find("C1") > -1:
      timeScale = 25e-9
    else:
      timeScale = 1

    return timeScale

  # -----------------------------------------------------------------------
  def genGraph_LB2Lumi(self, h_LB):
    h_name   = h_LB.GetName()
    h_title  = h_LB.GetTitle()
    g_name   = h_name.replace('LB', 'Lumi')
    g_title  = h_title.replace('LB', 'Lumi')
    g_Xtitle = 'Inst luminosity [10^{34} cm^{-2}s^{-1}]'
    g_Ytitle = 'RPC hit rate[Hz]'

    if self._NEvtScale:
      timeScale =  self.__getTimeScale()

    x = []
    y = []
    y_err = []
    for LB, lbInfo in self._Dic_LumiInfo.items():
      if lbInfo['AtlasPhysics'] == 'false' or float(lbInfo['Duration'])<55. or float(lbInfo['Duration'])>65.:
        continue

      hit_content = round(h_LB.GetBinContent(LB), 2)
      hit_err     = round(h_LB.GetBinError(LB),2)

      hit_y       = hit_content
      hit_y_err   = hit_err

      if  hit_content < 1 :
        continue

      if self._NEvtScale:
        evt_content = round(self.__h_NEvt_LB.GetBinContent(LB),2)
        evt_error   = round(self.__h_NEvt_LB.GetBinError(LB),2)
        if  evt_content < 1 :
          continue
        hit_rate = hit_content/(evt_content*timeScale)

        err_temp1 = pow(hit_err/evt_content,2)
        err_temp2 = pow(evt_error*hit_content/pow(evt_content,2), 2)
        hitRate_err = math.sqrt(err_temp1+err_temp2)/timeScale
        hit_y       = hit_rate
        hit_y_err   = hitRate_err

      x.append( float(lbInfo['InstLumi']) )
      y.append( hit_y )
      y_err.append( hit_y_err )
    x_err = [0]*len(x)

    return RPCRawDataMonUtils.creatGraph(x, y, x_err, y_err, g_name, g_title, g_Xtitle, g_Ytitle)

  # -----------------------------------------------------------------------
  def doNEvtScale(self, h_NEvt_LB):
    self._NEvtScale  = True
    self.__h_NEvt_LB = h_NEvt_LB

  # -----------------------------------------------------------------------
  def GetGraphs_rate_perPanel(self):
    return  self.list_graphs_rate_panel

  # -----------------------------------------------------------------------
  def getObject_1panel(self, config):
    panelInd = config[0]
    panelName= config[1]
    
    g_name   = self._in_hname.split('Panels')[0]+panelName
    g_title  = g_name
    g_Xtitle = 'Inst luminosity [10^{34} cm^{-2}s^{-1}]'
    g_Ytitle = 'RPC hit rate[Hz]'

    if self._NEvtScale:
      timeScale =  self.__getTimeScale()

    x = []
    y = []
    y_err = []
    for LB, lbInfo in self._Dic_LumiInfo.items():
      if lbInfo['AtlasPhysics'] == 'false' or float(lbInfo['Duration'])<55. or float(lbInfo['Duration'])>65. :
        continue
      
      hit_content = self._in_histo.GetBinContent(LB, panelInd+1)
      hit_err     = self._in_histo.GetBinError(LB, panelInd+1)
      hit_y       = hit_content
      hit_y_err   = hit_err
      if  hit_content < 1 :
        continue

      if self._NEvtScale:
        evt_content = self.__h_NEvt_LB.GetBinContent(LB)
        evt_error   = self.__h_NEvt_LB.GetBinError(LB)
        if  evt_content < 1 :
          continue
        hit_rate = hit_content/(evt_content*timeScale)

        err_temp1 = pow(hit_err/evt_content,2)
        err_temp2 = pow(evt_error*hit_content/pow(evt_content,2), 2)
        hitRate_err   = math.sqrt(err_temp1+err_temp2)/timeScale
        hit_y         = hit_rate
        hit_y_err     = hitRate_err
      
      x.append( float(lbInfo['InstLumi']) )
      y.append( hit_y )
      y_err.append( hit_y_err )
    x_err = [0]*len(x)

    len_LB  = len(y)
    fit_able = 0
    is_good  = True

    if len_LB == 0 :
      fit_able = 3
      is_good  = False
      fitRe_g1 = {"p0": 0, "p0_err":0, "p1": -1, "p1_err":-1, "chi2":-1, "mean":0, "mean_err":0}
      out_dic  = {'fit_able':fit_able, 'is_good':is_good, 'hit_rate':0}
    else:
      gr       = RPCRawDataMonUtils.creatGraph(x, y, x_err, y_err, g_name, g_title, g_Xtitle, g_Ytitle)
      (fit_able, fitRe_g1) = RPCRawDataMonUtils.linearFit(gr)
      out_dic  = {'fit_able':fit_able, 'is_good':is_good, 'hit_rate':gr}
      self.list_graphs_rate_panel.append(gr)

    out_dic['N_selLBs'] = len_LB

    if (not out_dic['is_good']) or (fitRe_g1["p1"] < self.Tlow_p1_panel) or (fitRe_g1["chi2"] > self.T_chi2_panel):
      out_dic['is_good']  = False
      self._hists_bad_panels.append(out_dic)

    return fitRe_g1

  # -----------------------------------------------------------------------
  def getObject_1SectorLayer(self, config, sec_layer, temp_dic):
    name_pre = self._in_hname.split("_")[0]
    titles = [name_pre+"_"+self._timeTag+"_LB", name_pre+"("+self._timeTag+") vs LB", "LB", "N_{hits}"]
    self.getHist_1SectorLayer(config, sec_layer, temp_dic, titles)

  # -----------------------------------------------------------------------
  def getHist_1Panel(self, config):
    panelInd = config[0]

    h_temp = ROOT.TH1F("h_temp"+str(panelInd), "h_temp"+str(panelInd), self._nbins, self._low, self._up)
    for i_bin in range(1, self._nbins+1):
      hit_content = self._in_histo.GetBinContent(i_bin, panelInd+1)
      hit_err     = self._in_histo.GetBinError(i_bin, panelInd+1)
      
      h_temp.SetBinContent(i_bin, hit_content)
      h_temp.SetBinError(i_bin, hit_err)
    return h_temp

  # -----------------------------------------------------------------------
  def initialHist_1Panel(self, name_suffix, title_suffix, titles):
    hist = ROOT.TH1F(titles[0]+name_suffix, titles[1]+title_suffix+";"+titles[2]+";"+titles[3], self._nbins, self._low, self._up)
    return hist

  # -----------------------------------------------------------------------
  def getContent(self, singleObj):
    if self._variable == 'predRate':
      cont  = singleObj['p0']+self.__refInstLumi*singleObj['p1']
      error = singleObj['p0_err']+self.__refInstLumi*singleObj['p1_err']
    elif self._variable == 'meanRate':
      cont  = singleObj['mean']
      error = singleObj['mean_err']
    else:
      cont  = singleObj[self._variable]
      if self._variable == 'chi2':
        error = 0.01
      else:
        error = singleObj[self._variable+"_err"]

    return (cont, error)

  # -----------------------------------------------------------------------
  def _post_allSectorAndLayers(self, temp_dic):
    self._dic_sectors_layers = {}

    for sec_layer, hist in temp_dic.items():
      grs   = self.genGraph_LB2Lumi(hist)
      fitRe = RPCRawDataMonUtils.linearFit(grs)

      self._dic_sectors_layers[sec_layer] = fitRe[1]

  # -----------------------------------------------------------------------
  def setCutLine(self, i_histo, index = 0):
    if index == 0:
      y = self._cuts_summaryHist_allSectorLayer[self._variable]
      minX = -16.5
      maxX = 16.5
    else:
      y = self._cuts_summaryHist_perSectorLayer[self._variable]
      minX = 0.
      maxX = 19.

    line = ROOT.TLine(minX, y, maxX, y)
    line.SetLineStyle(2) 
    line.SetLineWidth(1) 
    line.SetLineColor(ROOT.kBlue)

    i_histo.GetListOfFunctions().Add(line)

#############################################################################
class Draw_HitMultiplicity(Draw_Base):

  # -----------------------------------------------------------------------
  def __init__(self, inHisto):
    super(Draw_HitMultiplicity, self).__init__(inHisto)

    self.list_hitMulti_perPanel = []

  # -----------------------------------------------------------------------
  def GetGraphs_rate_perPanel(self):
    return  self.list_hitMulti_perPanel

  # -----------------------------------------------------------------------
  def getObject_1panel(self, config):
    panelInd  = config[0]
    panelName = config[1]
    nbins     = self._nbins
    low       = self._low  
    up        = self._up   

    h1_temp    = ROOT.TH1F(self._variable+"_"+panelName, self._out_htitle+"("+panelName+");"+self._out_htitle+";Muon entries",  nbins, low, up)

    for i_bin in range(1, nbins+1):
      h1_temp.SetBinContent(i_bin, self._in_histo.GetBinContent(i_bin, panelInd+1))
      h1_temp.SetBinError(i_bin, self._in_histo.GetBinError(i_bin, panelInd+1))

    self.list_hitMulti_perPanel.append(h1_temp)
    return  h1_temp

  # -----------------------------------------------------------------------
  def getObject_1SectorLayer(self, config, sec_layer, temp_dic):
    titles = [self._variable, self._out_htitle, self._out_htitle, "muon entries"]
    self.getHist_1SectorLayer(config, sec_layer, temp_dic, titles)

  # -----------------------------------------------------------------------
  def getHist_1Panel(self, config):
    panelInd = config[0]

    h_temp = ROOT.TH1F("h_temp"+str(panelInd), "h_temp"+str(panelInd), self._nbins, self._low, self._up)
    for i_bin in range(1, self._nbins+1):
      hit_content = self._in_histo.GetBinContent(i_bin, panelInd+1)
      hit_err     = self._in_histo.GetBinError(i_bin, panelInd+1)
      
      h_temp.SetBinContent(i_bin, hit_content)
      h_temp.SetBinError(i_bin, hit_err)
    return h_temp

  # -----------------------------------------------------------------------
  def initialHist_1Panel(self, name_suffix, title_suffix, titles):
    hist = ROOT.TH1F(titles[0]+name_suffix, titles[1]+title_suffix+";"+titles[2]+";"+titles[3], self._nbins, self._low, self._up)
    return hist

  # -----------------------------------------------------------------------
  def _post_allSectorAndLayers(self, temp_dic):
    self._dic_sectors_layers = temp_dic

  # -----------------------------------------------------------------------
  def GetHist1D_yMuon_wholeRPC(self, config):
    self._in_hname    = config[0]
    self._variable    = config[1]
    self._out_htitle  = self._var_title[self._variable]

    if len(config) == 3:
      self._timeTag  = config[2]
      out_hname  = self._variable+'_'+self._timeTag
      out_htitle = self._out_htitle+'('+self._timeTag+')'
    else:
      out_hname  = self._variable
      out_htitle = self._out_htitle

    if not self.doneParserSectorAndLayer == self._in_hname:
      self.parseHisto_allSectorAndLayers()
    
    all_hist = ROOT.TH1F(out_hname+"_wholeRPC", out_htitle+" of whole RPC;"            +self._out_htitle+";muon entries", self._nbins, self._low, self._up)
    eta_hist = ROOT.TH1F(out_hname+"_etaView",  out_htitle+" in "+isMeasPhiNames[0]+";"+self._out_htitle+";muon entries", self._nbins, self._low, self._up)
    phi_hist = ROOT.TH1F(out_hname+"_phiView",  out_htitle+" in "+isMeasPhiNames[1]+";"+self._out_htitle+";muon entries", self._nbins, self._low, self._up)
    all_hist.GetXaxis().SetRange(0, self._nbins+1)
    eta_hist.GetXaxis().SetRange(0, self._nbins+1)
    phi_hist.GetXaxis().SetRange(0, self._nbins+1)
    for sec_layer, hists_eta_phi in self._dic_sectors_layers.items():
      all_hist.Add(hists_eta_phi[0])
      all_hist.Add(hists_eta_phi[1])
      eta_hist.Add(hists_eta_phi[0])
      phi_hist.Add(hists_eta_phi[1])

    return [all_hist, eta_hist, phi_hist]

  # -----------------------------------------------------------------------
  def GetHist1D_yMuon_perSectorLayer(self, config):
    list_hist = []
    for sec_layer, hists_eta_phi in self._dic_sectors_layers.items():
      for i_hist in hists_eta_phi:
        list_hist.append(i_hist)

    return list_hist

  # -----------------------------------------------------------------------
  def getContent(self, singleObj):
    cont  = singleObj.GetMean()
    error = singleObj.GetMeanError()

    return (cont, error)

#############################################################################
class Draw_DetectEfficiency(Draw_Base):

  # -----------------------------------------------------------------------
  def __init__(self, inHisto):
    super(Draw_DetectEfficiency, self).__init__(inHisto)
    self.doneParser_name_perSector    = ''

  # -----------------------------------------------------------------------
  def _prepareHisto(self):
    self._out_htitle  = self._var_title[self._variable]
    
    self._nbins = 1
    self._low   = 0
    self._up    = 1

  # -----------------------------------------------------------------------
  def getObject_1panel(self, config):
    panelInd = config[0]
    # panelName= config[1]
    
    err_low = self._in_histo.GetEfficiencyErrorLow(panelInd+1) 
    err_up  = self._in_histo.GetEfficiencyErrorUp(panelInd+1)
    cont    = self._in_histo.GetEfficiency(panelInd+1)
    if (cont+err_up) > 1:
      err_up  = 1. - cont
    return (cont, err_low, err_up)
 
  # -----------------------------------------------------------------------
  def getObject_1SectorLayer(self, config, sec_layer, temp_dic):
    titles = [self._variable, self._out_htitle, "", "#epsilon"]
    self.getHist_1SectorLayer(config, sec_layer, temp_dic, titles)

  # -----------------------------------------------------------------------
  def getHist_1Panel(self, config):
    panelInd = config[0]

    num_hist = self._in_histo.GetPassedHistogram()
    den_hist = self._in_histo.GetTotalHistogram()

    h_num_temp = ROOT.TH1F("h_effNum"+str(panelInd), "h_effNum"+str(panelInd), 1, 0., 1.)
    h_den_temp = ROOT.TH1F("h_effDen"+str(panelInd), "h_effDen"+str(panelInd), 1, 0., 1.)
    num = num_hist.GetBinContent(panelInd+1); num_err = num_hist.GetBinError(panelInd+1)
    den = den_hist.GetBinContent(panelInd+1); den_err = den_hist.GetBinError(panelInd+1)
    h_num_temp.SetBinContent(1, num); h_num_temp.SetBinError(1, num_err)
    h_den_temp.SetBinContent(1, den); h_den_temp.SetBinError(1, den_err)

    h_temp = ROOT.TEfficiency(h_num_temp, h_den_temp)

    return h_temp

  # -----------------------------------------------------------------------
  def initialHist_1Panel(self, name_suffix, title_suffix, titles):
    hist = ROOT.TEfficiency(titles[0]+name_suffix, titles[1]+title_suffix+";"+titles[2]+";"+titles[3], self._nbins, self._low, self._up)
    return hist

  # -----------------------------------------------------------------------
  def _post_allSectorAndLayers(self, temp_dic):
    self._dic_sectors_layers = {}

    for sec_layer, hist in temp_dic.items():
      # con_err1 = [hist[0].GetEfficiency(1), hist[0].GetEfficiencyErrorLow(1), hist[0].GetEfficiencyErrorUp(1)]
      # con_err2 = [hist[1].GetEfficiency(1), hist[1].GetEfficiencyErrorLow(1), hist[1].GetEfficiencyErrorUp(1)]
      # con_err  = (con_err1, con_err2)

      con_err = [hist.GetEfficiency(1), hist.GetEfficiencyErrorLow(1), hist.GetEfficiencyErrorUp(1)]

      self._dic_sectors_layers[sec_layer] = con_err

  # -----------------------------------------------------------------------
  def getPanelEff_Dic(self, config):
    self._in_hname    = config[0]
    self._variable    = config[1]

    if not self.doneParserPanel == self._in_hname:
      self.parseHisto_allPanels()

    return self._dic_panels_detect

  # -----------------------------------------------------------------------
  def getSectorAndLayerEff_Dic(self, config):
    self._in_hname    = config[0]
    self._variable    = config[1]

    if not self.doneParserSectorAndLayer == self._in_hname:
      self.parseHisto_allSectorAndLayers()

    return self._dic_sectors_layers

  # -----------------------------------------------------------------------
  def getContent(self, singleObj):
    if self._variable == "extrapMuons" or self._variable == "detEff":
      content = singleObj[0]
      error   = singleObj[1]
    elif self._variable == "detEffError":
      content = singleObj[1]
      error   = 0
    return (content, error)

#############################################################################
class Draw_HitOuttimeFraction(Draw_Base):
  # -----------------------------------------------------------------------
  def __init__(self,inHisto):
    super(Draw_HitOuttimeFraction, self).__init__(inHisto)

  # -----------------------------------------------------------------------
  def _prepareHisto(self):
    self._out_htitle  = self._var_title[self._variable]
    
    self._nbins = 1
    self._low   = 0
    self._up    = 1
  
  # -----------------------------------------------------------------------
  def getObject_1panel(self, config):
    panelInd = config[0]

    frac_cont    = self._in_histo.GetEfficiency(panelInd+1)
    frac_err_low = self._in_histo.GetEfficiencyErrorLow(panelInd+1) 
    frac_err_up  = self._in_histo.GetEfficiencyErrorUp(panelInd+1)

    if (frac_cont+frac_err_up) > 1:
      frac_err_up  = 1. - frac_cont
    return (frac_cont, frac_err_low, frac_err_up)

  # -----------------------------------------------------------------------
  def getObject_1SectorLayer(self, config, sec_layer, temp_dic):
    titles = [self._variable, self._out_htitle, '', '']
    self.getHist_1SectorLayer(config, sec_layer, temp_dic, titles)

  # -----------------------------------------------------------------------
  def getHist_1Panel(self, config):
    panelInd = config[0]

    num_hist = self._in_histo.GetPassedHistogram()
    den_hist = self._in_histo.GetTotalHistogram()

    h_num_temp = ROOT.TH1F("h_effNum"+str(panelInd), "h_effNum"+str(panelInd), 1, 0., 1.)
    h_den_temp = ROOT.TH1F("h_effDen"+str(panelInd), "h_effDen"+str(panelInd), 1, 0., 1.)
    num = num_hist.GetBinContent(panelInd+1); num_err = num_hist.GetBinError(panelInd+1)
    den = den_hist.GetBinContent(panelInd+1); den_err = den_hist.GetBinError(panelInd+1)
    h_num_temp.SetBinContent(1, num); h_num_temp.SetBinError(1, num_err)
    h_den_temp.SetBinContent(1, den); h_den_temp.SetBinError(1, den_err)

    h_temp = ROOT.TEfficiency(h_num_temp, h_den_temp)
    return h_temp

  # -----------------------------------------------------------------------
  def initialHist_1Panel(self, name_suffix, title_suffix, titles):
    hist = ROOT.TEfficiency(titles[0]+name_suffix, titles[1]+title_suffix+";"+titles[2]+";"+titles[3], self._nbins, self._low, self._up)
    return hist

  # -----------------------------------------------------------------------
  def _post_allSectorAndLayers(self, temp_dic):
    self._dic_sectors_layers = {}
    for sec_layer, hist in temp_dic.items():
      # con_err1 = [hist[0].GetEfficiency(1), hist[0].GetEfficiencyErrorLow(1), hist[0].GetEfficiencyErrorUp(1)]
      # con_err2 = [hist[1].GetEfficiency(1), hist[1].GetEfficiencyErrorLow(1), hist[1].GetEfficiencyErrorUp(1)]
      # con_err  = (con_err1, con_err2)
      con_err = [hist.GetEfficiency(1), hist.GetEfficiencyErrorLow(1), hist.GetEfficiencyErrorUp(1)]

      self._dic_sectors_layers[sec_layer] = con_err

  # -----------------------------------------------------------------------
  def getContent(self, singleObj):
    if self._variable.find("Error")>-1:
      content = singleObj[1]
      error   = 0
    else:
      content = singleObj[0]
      error   = singleObj[1]

    return (content, error)

#############################################################################
if __name__ ==  '__main__':
  print ("CoreClass:  Hello, World !")
