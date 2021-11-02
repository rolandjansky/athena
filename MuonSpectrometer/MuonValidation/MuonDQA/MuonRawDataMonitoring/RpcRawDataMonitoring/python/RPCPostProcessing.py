#
#Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration                                                                                           
#

# local
import RpcRawDataMonitoring.RPCRawDataMonUtils as RPCRawDataMonUtils
import RpcRawDataMonitoring.CoreClass as CoreClass
import RpcRawDataMonitoring.GetLBInfoFromCOOL as GetLBInfoFromCOOL

#############################################################################
### The algorithems should be ran for a **FULL** and **SINGLE** run
#############################################################################

#############################################################################
def getPanelsPropertiesFromHist(hists):

  Ranges = {
    "stationName" : range(1,  53+1),
    "stationEta"  : range(-8, 8+1),
    "stationPhi"  : range(1,  8+1),
    "doubletR"    : [1,2],
    "doubletZ"    : [1,2,3],
    "doubletPhi"  : [1,2],
    "gasGap"      : [1,2],
    "measPhi"     : [0,1],
  }

  NPanels       = hists["stationName"].GetNbinsY()
  Dic_panels    = {}
  missVarPanels = []

  for i_panel in range(1, NPanels+1):
    dic_perPanel = {}
    missVarPanel = False
    
    for key, hist in hists.items():
      Value      = -10000
      maxEntries = 0
      nbin_x = hist.GetNbinsX()

      for i_binX in range(1, nbin_x+1):
        content = hist.GetBinContent(i_binX, i_panel)

        if content > maxEntries: 
          maxEntries = content
          Value      = Ranges[key][i_binX-1]

      if maxEntries > 0:
        dic_perPanel[key] = int(Value)
      else:
        missVarPanel = True

    if not missVarPanel:
      Dic_panels[i_panel] = dic_perPanel
    else:
      missVarPanels.append(i_panel)

  print("getPanelsPropertiesFromHist::there is %d panels missinng properties!!!", len(missVarPanels))
  print("getPanelsPropertiesFromHist::finds %d panels !!!", len(Dic_panels))

  return Dic_panels

#############################################################################
def getRun(h_run):
  xbins = h_run.GetNbinsX()
  runs  = []

  for i_bin in range(1, xbins+1):
    if h_run.GetBinContent(i_bin) > 0:
      runs.append(int(h_run.GetBinLowEdge(i_bin))+1)

  return runs

#############################################################################
def make_evt_lumi(inputs):
  h_NEvt_LB = inputs[0][1][0].Clone()
  
  h_run     = inputs[0][1][1].Clone()
  runs      = getRun(h_run)

  Dic_LBLumi = GetLBInfoFromCOOL.GetLumiInfoDic(runs[0], runs[-1]+1)

  g_name   = 'NEvent_VS_Lumi'
  g_title  = 'NEvent VS Lumi'
  g_Xtitle = 'Inst Luminosity [10^{34} cm^{-2}s^{-1}]'
  g_Ytitle = 'NEvent'

  x = []
  y = []
  y_err = []

  for LB, lbInfo in Dic_LBLumi.items():
    if lbInfo['AtlasPhysics'] == 'false' or float(lbInfo['Duration'])<55. or float(lbInfo['Duration'])>65. :
      continue

    hit_content = h_NEvt_LB.GetBinContent(LB)
    hit_err     = h_NEvt_LB.GetBinError(LB) 

    x.append( float(lbInfo['InstLumi']) )
    y.append( hit_content )
    y_err.append( hit_err )

  x_err = [0]*len(x)

  g = RPCRawDataMonUtils.creatGraph(x, y, x_err, y_err, g_name, g_title, g_Xtitle, g_Ytitle)
  
  return [g]

#############################################################################
def make_hit_rate(inputs):
  hists_prop = {
    "stationName" :inputs[0][1][0].Clone(),
    "stationEta"  :inputs[0][1][1].Clone(),
    "stationPhi"  :inputs[0][1][2].Clone(),
    "doubletR"    :inputs[0][1][3].Clone(),
    "doubletZ"    :inputs[0][1][4].Clone(),
    "doubletPhi"  :inputs[0][1][5].Clone(),
    "gasGap"      :inputs[0][1][6].Clone(),
    "measPhi"     :inputs[0][1][7].Clone()
  }

  h_hit_panels = inputs[0][1][8].Clone() # "NPRDHit_Panels_All"
  h_NEvt_LB    = inputs[0][1][9].Clone() # evtLB
  h_run        = inputs[0][1][10].Clone() # run
  
  runs       = getRun(h_run)
  if len(runs) > 1:
    print ("make_hit_rate::WARNING - Input file contain data in ", len(runs), " **runs**, which should only include 1-run and full data !!!")
    return

  Dic_LBLumi = GetLBInfoFromCOOL.GetLumiInfoDic(runs[0], runs[0]+1)
  print ("len(Dic_LBLumi) = ", len(Dic_LBLumi))
  DicPanels  = getPanelsPropertiesFromHist(hists_prop)

  ######### Draw_Occupancy ######################
  draw_occu = CoreClass.Draw_Occupancy(h_hit_panels)
  draw_occu.SetPanelDic(DicPanels)
  draw_occu.SetLumiInfoDic(Dic_LBLumi)
  draw_occu.doNEvtScale(h_NEvt_LB)

  h_name   = "NPRDHit_Panels_All"
  timeTag   = "All"

  # -----------------------------------------------------------------------
  list_hist_all         = []
  list_hist_layer       = []
  list_hist_subDetector = []
  for i_var in ["p0", "p1", "chi2", "predRate", "meanRate"]:
    ###
    ### Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/HitRate_vs_InstLumi
    # "(p0|p1|chi2|predRate|meanRate)_All_per_sectors_per_layers_(eta|phi|etaAndPhi)Panels",
    list_hist1D_secLayer = draw_occu.GetHist1D_ySectorsAndLayers([h_name, i_var, timeTag])
    
    # "(p0|p1|chi2|predRate|meanRate)_All_per_panel_(eta|phi|etaAndPhi)View",
    list_hist1D_panels   = draw_occu.GetHist1D_yPanels([h_name, i_var, timeTag])
    
    list_hist_all += list_hist1D_secLayer+list_hist1D_panels
  
    ###
    ### Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/HitRate_vs_InstLumi/Layers
    # "Summary_layer[1-8]_(p0|p1|chi2|predRate|meanRate)_All", 
    list_summary_allSectorsAndLayers  = draw_occu.GetSummary_allSectorsAndLayers([h_name, i_var, timeTag])

    # "(p0|p1|chi2|predRate|meanRate)_All_layer[1-6]_measPhi[01]",
    list_hist2d_EtaPhi_allLayer       = draw_occu.GetHist2D_EtaPhi_allLayer([h_name, i_var, timeTag])

    list_hist_layer += list_summary_allSectorsAndLayers+list_hist2d_EtaPhi_allLayer

    ###
    ### Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/HitRate_vs_InstLumi/SubDetector
    # "Summary_Sector-?[0-9][0-6]?_Layer[1-8]_dbPhi[12]_measPhi[01]_(p0|p1|chi2|predRate|meanRate)_All",
    if i_var in ["p0", "chi2", "predRate"]:
      list_summary_eachSectorsAndLayers = draw_occu.GetSummary_eachSectorsAndLayers([h_name, i_var, timeTag])

      list_hist_subDetector += list_summary_eachSectorsAndLayers

  list_hist = list_hist_all + list_hist_layer + list_hist_subDetector
  # list_graph_panels    = draw_occu.GetGraphs_rate_perPanel()
  # list_graph_badPanels = draw_occu.GetGraphs_rate_BadPanels()

  return list_hist

#############################################################################
def make_hitMulti(inputs):
  hists_prop = {
    "stationName" :inputs[0][1][0].Clone(),
    "stationEta"  :inputs[0][1][1].Clone(),
    "stationPhi"  :inputs[0][1][2].Clone(),
    "doubletR"    :inputs[0][1][3].Clone(),
    "doubletZ"    :inputs[0][1][4].Clone(),
    "doubletPhi"  :inputs[0][1][5].Clone(),
    "gasGap"      :inputs[0][1][6].Clone(),
    "measPhi"     :inputs[0][1][7].Clone()
  }
  hist = inputs[0][1][8].Clone()
  DicPanels  = getPanelsPropertiesFromHist(hists_prop)

  draw_hitmulti = CoreClass.Draw_HitMultiplicity(hist)
  draw_hitmulti.SetPanelDic(DicPanels)

  # -----------------------------------------------------------------------
  # draw hit multiplicity
  h_name   = "HitMultiplicity_Panels"
  variable = "hitMultiplicity"

  list_hist1D_secLayer_hitMu = draw_hitmulti.GetHist1D_ySectorsAndLayers([h_name, variable])
  # "hitMultiplicity_per_sectors_per_layers_(eta|phi|etaAndPhi)Panels",
  
  list_hist1D_panels_hitMu   = draw_hitmulti.GetHist1D_yPanels([h_name, variable])
  # "hitMultiplicity_per_panel_(eta|phi|etaAndPhi)View",

  # -----------------------------------------------------------------------
  # draw cluster size
  h2_CS    = "ClusterSize_Panels"
  variable = "averageClusterSize"
  hist_clus = inputs[0][1][9].Clone()
  draw_clus = CoreClass.Draw_HitMultiplicity(hist_clus)
  draw_clus.SetPanelDic(DicPanels)
  
  list_hist1D_secLayer_cluster = draw_hitmulti.GetHist1D_ySectorsAndLayers([h2_CS, variable])
  # "averageClusterSize_per_sectors_per_layers_(eta|phi|etaAndPhi)Panels",
  
  list_hist1D_panels_cluster   = draw_hitmulti.GetHist1D_yPanels([h2_CS, variable])
  # "averageClusterSize_per_panel_(eta|phi|etaAndPhi)View",

  return list_hist1D_secLayer_hitMu+list_hist1D_panels_hitMu+list_hist1D_secLayer_cluster+list_hist1D_panels_cluster

#############################################################################
def make_detection_eff(inputs):
  hists_prop = {
    "stationName" :inputs[0][1][0].Clone(),
    "stationEta"  :inputs[0][1][1].Clone(),
    "stationPhi"  :inputs[0][1][2].Clone(),
    "doubletR"    :inputs[0][1][3].Clone(),
    "doubletZ"    :inputs[0][1][4].Clone(),
    "doubletPhi"  :inputs[0][1][5].Clone(),
    "gasGap"      :inputs[0][1][6].Clone(),
    "measPhi"     :inputs[0][1][7].Clone()
  }

  hist = inputs[0][1][8].Clone()
  DicPanels  = getPanelsPropertiesFromHist(hists_prop)

  draw_eff = CoreClass.Draw_DetectEfficiency(hist)
  draw_eff.SetPanelDic(DicPanels)
  
  h_name  = "Panel_Efficiency"

  # -----------------------------------------------------------------------
  variable = "detEff"
  config = [h_name, variable]
  list_summary_allSectorsAndLayers = draw_eff.GetSummary_allSectorsAndLayers(config)
    # "Summary_layer[1-8]_detEff", 

  list_summary_eachSectorsAndLayers= draw_eff.GetSummary_eachSectorsAndLayers(config)
    # "Summary_Sector-?[0-9][0-6]?_Layer[1-8]_dbPhi[12]_measPhi[01]_detEff",

  list_hist2d_EtaPhi_allLayer      = draw_eff.GetHist2D_EtaPhi_allLayer(config)
    # "detEff_layer[1-6]_measPhi[01]",

  list_hist1D_secLayer             = draw_eff.GetHist1D_ySectorsAndLayers(config)
    # "detEff_per_sectors_per_layers_(eta|phi|etaAndPhi)Panels",

  list_hist1D_panels               = draw_eff.GetHist1D_yPanels(config)
    # "detEff_per_panel_(eta|phi|etaAndPhi)View",


  return list_summary_allSectorsAndLayers+list_hist2d_EtaPhi_allLayer+list_hist1D_secLayer+list_hist1D_panels+list_summary_eachSectorsAndLayers

#############################################################################
def make_hitFrac(inputs):
  hists_prop = {
    "stationName" :inputs[0][1][0].Clone(),
    "stationEta"  :inputs[0][1][1].Clone(),
    "stationPhi"  :inputs[0][1][2].Clone(),
    "doubletR"    :inputs[0][1][3].Clone(),
    "doubletZ"    :inputs[0][1][4].Clone(),
    "doubletPhi"  :inputs[0][1][5].Clone(),
    "gasGap"      :inputs[0][1][6].Clone(),
    "measPhi"     :inputs[0][1][7].Clone()
  }
  DicPanels  = getPanelsPropertiesFromHist(hists_prop)

  # -----------------------------------------------------------------------
  hist = inputs[0][1][8].Clone()
  draw_histFrac = CoreClass.Draw_HitOuttimeFraction(hist)
  draw_histFrac.SetPanelDic(DicPanels)

  h_name = "OuttimeHitFraction_PRDHit"
  var     = "outTimeHitFrac"
  config  = [h_name, var]

  list_hist1D_secLayer_hitFrac = draw_histFrac.GetHist1D_ySectorsAndLayers(config)
  # "outTimeHitFrac_per_sectors_per_layers_(eta|phi|etaAndPhi)Panels",

  list_hist1D_panels_hitFrac   = draw_histFrac.GetHist1D_yPanels(config)
  # "outTimeHitFrac_per_panel_(eta|phi|etaAndPhi)View",

  # -----------------------------------------------------------------------
  hist_onTrack = inputs[0][1][9].Clone()
  draw_histFrac_onTrack = CoreClass.Draw_HitOuttimeFraction(hist_onTrack)
  draw_histFrac_onTrack.SetPanelDic(DicPanels)

  h_name = "OuttimeHitFraction_PRDHit_onTrack"
  var     = "outTimeHitFrac_onTrack"
  config  = [h_name, var]
  list_hist1D_secLayer_hitFracOnTrack = draw_histFrac_onTrack.GetHist1D_ySectorsAndLayers(config)
  # "outTimeHitFrac_onTrack_per_sectors_per_layers_(eta|phi|etaAndPhi)Panels",

  list_hist1D_panels_hitFracOnTrack   = draw_histFrac_onTrack.GetHist1D_yPanels(config)
  # "outTimeHitFrac_onTrack_per_panel_(eta|phi|etaAndPhi)View",

  return list_hist1D_secLayer_hitFrac+list_hist1D_panels_hitFrac+list_hist1D_secLayer_hitFracOnTrack+list_hist1D_panels_hitFracOnTrack


#############################################################################
def printHistNames():
  inputfileName = "/home/ssu/DQ_area/run/RPCMonitoringOutput_1000.root" # 1000 event

  # make_hit_rate
  # inputHistNames = ['run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_stationName_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_stationEta_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_stationPhi_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_doubletR_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_doubletZ_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_doubletPhi_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_gasgap_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_measPhi_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_Panels_All', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/evtLB', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/Run']
  
  # make_hitMulti
  # inputHistNames = ['run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_stationName_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_stationEta_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_stationPhi_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_doubletR_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_doubletZ_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_doubletPhi_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_gasgap_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_measPhi_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/TrackMatch/HitMultiplicity_Panels', 'run_358615/Muon/MuonRawDataMonitoring/RPC/TrackMatch/ClusterSize_Panels']

  # make_detection_eff
  inputHistNames = ['run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_stationName_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_stationEta_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_stationPhi_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_doubletR_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_doubletZ_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_doubletPhi_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_gasgap_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_measPhi_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/TrackMatch/Panel_Efficiency']
  
  # make_hitFrac
  # inputHistNames = ['run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_stationName_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_stationEta_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_stationPhi_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_doubletR_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_doubletZ_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_doubletPhi_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_gasgap_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/NPRDHit_measPhi_vs_panelIndex', 'run_358615/Muon/MuonRawDataMonitoring/RPC/TrackMatch/OuttimeHitFraction_PRDHit_onTrack']
  import ROOT

  inFile  = ROOT.TFile.Open(inputfileName, 'READ')
  inHists = [[0, [inFile.Get(i_file) for i_file in inputHistNames]]]

  # outHists = make_hit_rate(inHists)
  # outHists = make_hitMulti(inHists)
  outHists = make_detection_eff(inHists)
  # outHists = make_hitFrac(inHists)

  print ("=================================================")
  print ("Output hist names:")
  for i_out in outHists:
    print (i_out.GetName())

  print ("=================================================")


#############################################################################
if __name__ ==  '__main__':
  print ("RPCPostProcessing:  Hello, World !")
  printHistNames()