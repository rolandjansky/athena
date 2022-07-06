#
#Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration                                                                                           
#

# local
import RpcRawDataMonitoring.RPCRawDataMonUtils as RPCRawDataMonUtils
import RpcRawDataMonitoring.CoreClass as CoreClass
import RpcRawDataMonitoring.GetLBInfoFromCOOL as GetLBInfoFromCOOL

#############################################################################
def readElementFromXML():
  # from xml.dom.minidom import parse
  import xml.dom.minidom as Dom
  import os
  import ROOT

  # -- Get the validation xml file path
  xml_file   = ROOT.PathResolver.find_file("Element.xml", "DATAPATH", ROOT.PathResolver.RecursiveSearch)

  if not os.path.isfile(xml_file):
    print ("ERROR: can NOT find xml file: %s!" %xml_file)
    return

  dom       = Dom.parse(xml_file)
  root_node = dom.documentElement
  ele_nodes = root_node.childNodes

  Dic_panels        = {}
  panel_property    = {}
  BMBO_StationNames = {2, 3, 4, 5, 8, 9, 10, 53}
  for i_node in ele_nodes:
    if i_node.nodeType != 1:
        continue

    # node_name = i_node.nodeName
    ele_index = int(i_node.getAttribute("index"))
    
    panel_property["stationName"] = int(i_node.getAttribute("stationName"))
    panel_property["stationEta" ] = int(i_node.getAttribute("stationEta"))
    panel_property["stationPhi" ] = int(i_node.getAttribute("stationPhi"))
    panel_property["doubletR"   ] = int(i_node.getAttribute("doubletR"))
    panel_property["doubletZ"   ] = int(i_node.getAttribute("doubletZ"))

    if panel_property["stationName"] in BMBO_StationNames:
      ngasgap = 2
    else:
      ngasgap = 3

    for dbPhi in [1,2]:
      for gasgap in range(1, ngasgap+1):
        for measPhi in [0, 1]:
          panel_property["doubletPhi"] = dbPhi
          panel_property["gasGap"]     = gasgap
          panel_property["measPhi"]    = measPhi
          
          panel_index = (ele_index-1)*8 + (dbPhi - 1)*4 + (gasgap - 1)*2 + measPhi
          i_panel  = RPCRawDataMonUtils.Panel(panel_property, panel_index)

          if not (panel_index in Dic_panels):
            Dic_panels[panel_index] = i_panel
          else:
            print ("ERROR: duplicated panel index!!!")

  print ("RPCPostProcessing::readElementFromXML::INFO: count of read panels = %d" %(len(Dic_panels)))

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
  print ("len(Dic_LBLumi) = ", len(Dic_LBLumi))

  g_name   = 'NEvent_VS_Lumi'
  g_title  = 'NEvent VS Lumi'
  g_Xtitle = 'Inst Luminosity [10^{34} cm^{-2}s^{-1}]'
  g_Ytitle = 'NEvent'

  x = []
  y = []
  y_err = []

  for LB, lbInfo in Dic_LBLumi.items():
    if lbInfo['AtlasPhysics'] == 'false' or float(lbInfo['Duration'])<50.:
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
  h_hit_panels = inputs[0][1][0].Clone() # "NPRDHit_Panels_All"
  h_NEvt_LB    = inputs[0][1][1].Clone() # evtLB
  h_run        = inputs[0][1][2].Clone() # run
  
  runs       = getRun(h_run)
  if len(runs) > 1:
    print ("make_hit_rate::WARNING - Input file contain data in ", len(runs), " **runs**, which should only include 1-run and full data !!!")
    return

  Dic_LBLumi = GetLBInfoFromCOOL.GetLumiInfoDic(runs[0], runs[0]+1)
  print ("len(Dic_LBLumi) = ", len(Dic_LBLumi))
  # GetLBInfoFromCOOL.printLumiInfo(Dic_LBLumi)

  DicPanels  = readElementFromXML()

  ######### Draw_Occupancy ######################
  draw_occu = CoreClass.Draw_Occupancy(h_hit_panels)
  draw_occu.SetPanelDic(DicPanels)
  draw_occu.SetLumiInfoDic(Dic_LBLumi)
  draw_occu.doNEvtScale(h_NEvt_LB)

  h_name   = "NPRDHit_Panels_All"
  timeTag   = "All"

  # -----------------------------------------------------------------------
  dic_hists             = {}
  list_hist_all         = []
  list_hist_layer       = []
  list_hist_subDetector = []
  for i_var in ["p0", "p1", "chi2", "predRate", "meanRate"]:
    ###
    ### Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/HitRate_vs_InstLumi
    ### "(p0|p1|chi2|predRate|meanRate)_All_per_sectors_per_layers_etaAndPhiPanels",
    list_hist1D_secLayer = draw_occu.GetHist1D_ySectorsAndLayers([h_name, i_var, timeTag])
    
    ###
    ### "(p0|p1|chi2|predRate|meanRate)_All_per_panel_(etaAndPhi｜eta|phi)View",
    ###
    list_hist1D_panels   = draw_occu.GetHist1D_yPanels([h_name, i_var, timeTag])
    
    list_hist_all += list_hist1D_secLayer+list_hist1D_panels
  
    ###
    ### Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/HitRate_vs_InstLumi/Layers
    ### "Summary_layer[1-8]_(p0|p1|chi2|predRate|meanRate)_All", 
    list_summary_allSectorsAndLayers  = draw_occu.GetSummary_allSectorsAndLayers([h_name, i_var, timeTag])

    ###
    ### "(p0|p1|chi2|predRate|meanRate)_All_layer[1-6]_measPhi[01]",
    ###
    list_hist2d_EtaPhi_allLayer       = draw_occu.GetHist2D_EtaPhi_allLayer([h_name, i_var, timeTag])

    list_hist_layer += list_summary_allSectorsAndLayers+list_hist2d_EtaPhi_allLayer

    ###
    ### Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/HitRate_vs_InstLumi/SubDetector
    ### "Summary_Sector[-16...-1, 1...16]_Layer[1...8]_dbPhi[1,2]_measPhi[0,1]_(p0|p1|chi2|predRate|meanRate)_All",
    if i_var in ["p0", "chi2", "predRate"]:
      list_summary_eachSectorsAndLayers = draw_occu.GetSummary_eachSectorsAndLayers([h_name, i_var, timeTag])

      list_hist_subDetector += list_summary_eachSectorsAndLayers

  getHistNames(list_hist_all, "Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/HitRate_vs_InstLumi", dic_hists)
  getHistNames(list_hist_layer, "Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/HitRate_vs_InstLumi/Layers", dic_hists)
  getHistNames(list_hist_subDetector, "Muon/MuonRawDataMonitoring/RPC/RpcOccupancy/HitRate_vs_InstLumi/SubDetector", dic_hists)

  return dic_hists

#############################################################################
def make_hitMulti(inputs):
  hist = inputs[0][1][0].Clone()
  DicPanels  = readElementFromXML()

  print ("make_hitMulti - hist = ", hist)

  draw_hitmulti = CoreClass.Draw_HitMultiplicity(hist)
  draw_hitmulti.SetPanelDic(DicPanels)

  # -----------------------------------------------------------------------
  # draw hit multiplicity
  h_name   = "HitMultiplicity_Panels"
  variable = "hitMultiplicity"

  ###
  # "hitMultiplicity_per_sectors_per_layers_etaAndPhiPanels",
  ###
  list_hist1D_secLayer_hitMu = draw_hitmulti.GetHist1D_ySectorsAndLayers([h_name, variable])
  
  ###
  # "hitMultiplicity_per_panel_(etaAndPhi|eta|phi)View",
  ###
  list_hist1D_panels_hitMu   = draw_hitmulti.GetHist1D_yPanels([h_name, variable])

  # -----------------------------------------------------------------------
  # draw cluster size
  h2_CS    = "ClusterSize_Panels"
  variable = "averageClusterSize"
  hist_clus = inputs[0][1][1].Clone()
  draw_clus = CoreClass.Draw_HitMultiplicity(hist_clus)
  draw_clus.SetPanelDic(DicPanels)
  
  ###
  # "averageClusterSize_per_sectors_per_layers_etaAndPhiPanels",
  ###
  list_hist1D_secLayer_cluster = draw_hitmulti.GetHist1D_ySectorsAndLayers([h2_CS, variable])
  
  ###
  # "averageClusterSize_per_panel_(etaAndPhi|eta|phi)View",
  ###
  list_hist1D_panels_cluster   = draw_hitmulti.GetHist1D_yPanels([h2_CS, variable])

  return list_hist1D_secLayer_hitMu+list_hist1D_panels_hitMu+list_hist1D_secLayer_cluster+list_hist1D_panels_cluster

#############################################################################
def make_detection_eff(inputs):
  hist = inputs[0][1][0].Clone()
  DicPanels  = readElementFromXML()

  draw_eff = CoreClass.Draw_DetectEfficiency(hist)
  draw_eff.SetPanelDic(DicPanels)
  
  h_name  = "Panel_Efficiency"

  # -----------------------------------------------------------------------
  variable   = "detEff"
  config     = [h_name, variable]
  dic_histos = {}

  ###
  ### "Summary_layer[1-8]_detEff", 
  ###
  list_summary_allSectorsAndLayers = draw_eff.GetSummary_allSectorsAndLayers(config)
  getHistNames(list_summary_allSectorsAndLayers, "Muon/MuonRawDataMonitoring/RPC/TrackMatch/MuonDetectionEff", dic_histos)

  ###
  ### "Summary_Sector[-16...-1, 1...16]_Layer[1...8]_dbPhi[1,2]_measPhi[0,1]_detEff",
  ###
  list_summary_eachSectorsAndLayers= draw_eff.GetSummary_eachSectorsAndLayers(config)
  getHistNames(list_summary_eachSectorsAndLayers, "Muon/MuonRawDataMonitoring/RPC/TrackMatch/MuonDetectionEff/SubDetector", dic_histos)

  ###
  ### "detEff_layer[1-6]_measPhi[01]",
  ###
  list_hist2d_EtaPhi_allLayer      = draw_eff.GetHist2D_EtaPhi_allLayer(config)
  getHistNames(list_hist2d_EtaPhi_allLayer, "Muon/MuonRawDataMonitoring/RPC/TrackMatch/MuonDetectionEff", dic_histos)

  ###
  ### "detEff_per_sectors_per_layers_(eta|phi|etaAndPhi)Panels",
  ###
  list_hist1D_secLayer             = draw_eff.GetHist1D_ySectorsAndLayers(config)
  getHistNames(list_hist1D_secLayer, "Muon/MuonRawDataMonitoring/RPC/TrackMatch/MuonDetectionEff", dic_histos)

  ###
  ### "detEff_per_panel_(eta|phi|etaAndPhi)View",
  ###
  list_hist1D_panels               = draw_eff.GetHist1D_yPanels(config)
  getHistNames(list_hist1D_panels, "Muon/MuonRawDataMonitoring/RPC/TrackMatch/MuonDetectionEff", dic_histos)

  return dic_histos

#############################################################################
def make_hitFrac(inputs):
  DicPanels  = readElementFromXML()


  # -----------------------------------------------------------------------
  # out-of-time fraction of hits
  # -----------------------------------------------------------------------
  hist = inputs[0][1][0].Clone()
  draw_histFrac = CoreClass.Draw_HitOuttimeFraction(hist)
  draw_histFrac.SetPanelDic(DicPanels)

  h_name = "OuttimeHitFraction_PRDHit"
  var     = "outTimeHitFrac"
  config  = [h_name, var]

  ###
  ### "outTimeHitFrac_per_sectors_per_layers_etaAndPhiPanels",
  ###
  list_hist1D_secLayer_hitFrac = draw_histFrac.GetHist1D_ySectorsAndLayers(config)

  ###
  ### "outTimeHitFrac_per_panel_(etaAndPhi|eta|phi)View",
  ###
  list_hist1D_panels_hitFrac   = draw_histFrac.GetHist1D_yPanels(config)

  # -----------------------------------------------------------------------
  # out-of-time fraction of hits on track
  # -----------------------------------------------------------------------
  hist_onTrack = inputs[0][1][1].Clone()
  draw_histFrac_onTrack = CoreClass.Draw_HitOuttimeFraction(hist_onTrack)
  draw_histFrac_onTrack.SetPanelDic(DicPanels)

  h_name = "OuttimeHitFraction_PRDHit_onTrack"
  var     = "outTimeHitFrac_onTrack"
  config  = [h_name, var]

  ###
  ### "outTimeHitFrac_onTrack_per_sectors_per_layers_etaAndPhiPanels",
  ###
  list_hist1D_secLayer_hitFracOnTrack = draw_histFrac_onTrack.GetHist1D_ySectorsAndLayers(config)

  ###
  ### "outTimeHitFrac_onTrack_per_panel_(etaAndPhi|eta|phi)View",
  ###
  list_hist1D_panels_hitFracOnTrack   = draw_histFrac_onTrack.GetHist1D_yPanels(config)

  return list_hist1D_secLayer_hitFrac+list_hist1D_panels_hitFrac+list_hist1D_secLayer_hitFracOnTrack+list_hist1D_panels_hitFracOnTrack


#############################################################################
def getHistNames(hist_list, prefix, dic_hist):
    for i_hist in hist_list:
        i_name = "%s/%s" %(prefix, i_hist.GetName())
        dic_hist[i_name] = i_hist

#############################################################################
if __name__ ==  '__main__':
  print ("RPCPostProcessing:  Hello, World !")
