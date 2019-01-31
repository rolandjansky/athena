/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// Book and fill histograms showing pixel cluster properties
///////////////////////////////////////////////////////////////////////////////

#include <sstream>
#include "InDetIdentifier/PixelID.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetRawData/InDetTimeCollection.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH1I_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"
#include "LWHists/TProfile2D_LW.h"
#include "LWHists/TProfile_LW.h"
#include "PixelConditionsServices/IPixelByteStreamErrorsSvc.h"
#include "PixelMonitoring/PixelMainMon.h"
#include "TH1F.h"
#include "TH1I.h"
#include "TH2F.h"
#include "TH2I.h"
#include "TProfile.h"

#include "PixelMonitoring/Components.h"
#include "PixelMonitoring/PixelMon2DMapsLW.h"
#include "PixelMonitoring/PixelMon2DProfilesLW.h"
#include "PixelMonitoring/PixelMonModules.h"

#include "PixelCabling/IPixelCablingSvc.h"

///////////////////////////////////////////////////////////////////////////////
//////////////////////booking methods//////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

StatusCode PixelMainMon::bookClustersMon(void) {
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "start booking pixel cluster monitoring histograms" << endmsg;

  std::string path = "Pixel/Clusters";
  if (m_doOnTrack) path.replace(path.begin(), path.end(), "Pixel/ClustersOnTrack");
  MonGroup clusterShift(this, path.c_str(), run, ATTRIB_MANAGED);   // declare a group of histograms
  MonGroup clusterExpert(this, path.c_str(), run, ATTRIB_MANAGED);  // declare a group of histograms

  std::string pathT = "Pixel/Timing";
  if (m_doOnTrack) pathT.replace(pathT.begin(), pathT.end(), "Pixel/TimingOnTrack");
  MonGroup timeShift(this, pathT.c_str(), run, ATTRIB_MANAGED);   // declare a group of histograms
  MonGroup timeExpert(this, pathT.c_str(), run, ATTRIB_MANAGED);  // declare a group of histograms

  std::string addOnTrack = "_OnTrack";

  int nbins_LB = m_lbRange;
  double min_LB = -0.5;
  double max_LB = min_LB + (1.0 * nbins_LB);
  int nbins_BCID = m_bcidRange;
  double min_BCID = -0.5;
  double max_BCID = min_BCID + (1.0 * nbins_BCID);
  int nbins_npix = 300;
  double min_npix = -0.5;
  double max_npix = min_npix + (1.0 * nbins_npix);
  int nbins_nwid = 50;
  double min_nwid = -0.5;
  double max_nwid = min_nwid + (1.0 * nbins_nwid);
  int nbins_eta = 13;
  double min_eta = -6.5;
  double max_eta = min_eta + (1.0 * nbins_eta);
  int nbins_tot = 300;
  double min_tot = -0.5;
  double max_tot = min_tot + (1.0 * nbins_tot);
  int nbins_ibl_tot = 300;
  double min_ibl_tot = -0.5;
  double max_ibl_tot = min_ibl_tot + (1.0 * nbins_ibl_tot);
  int nbins_Q = 70;
  double min_Q = -0.5;
  double max_Q = min_Q + (3000.0 * nbins_Q);
  int nbins_lvl1 = 14;
  double min_lvl1 = -1.5;
  double max_lvl1 = min_lvl1 + (1.0 * nbins_lvl1);
  std::string atext_LB = ";lumi block";
  std::string atext_BCID = ";BCID";
  std::string atext_clu = ";# clusters/event";
  std::string atext_npix = ";# pixels/cluster";
  std::string atext_nclu = ";# clusters";
  std::string atext_nevt = ";# events";
  std::string atext_cluw = ";cluster width";
  std::string atext_eta = ";Module eta index";
  std::string atext_tot = ";ToT [BC]";
  std::string atext_Q = ";Charge [e]";
  std::string atext_lvl1 = ";LVL1A";
  std::string tmp;
  std::string tmp2;
  std::string atitles;
  std::string hname;
  std::string htitles;

  StatusCode sc;

  hname = makeHistname("ClusterSize_vs_eta", false);
  htitles = makeHisttitle("Average cluster size as a function of barrel module eta", (atext_eta + atext_cluw), false);
  sc = clusterExpert.regHist(m_clusterSize_eta = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_eta, min_eta, max_eta));

  hname = makeHistname("Clusters_per_lumi", true);
  htitles = makeHisttitle("Average number of pixel clusters per event", (atext_LB + atext_clu), true);
  sc = clusterShift.regHist(m_clusters_per_lumi = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

  hname = makeHistname("TotalClusters_per_lumi", true);
  htitles = makeHisttitle("Total number of pixel clusters", (atext_LB + atext_nclu), true);
  sc = clusterShift.regHist(m_totalclusters_per_lumi = TH1I_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

  hname = makeHistname("Cluster_groupsize", true);
  htitles = makeHisttitle("Number of pixels in a cluster", (atext_npix + atext_nclu), true);
  sc = clusterExpert.regHist(m_cluster_groupsize = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_npix, min_npix, max_npix));

  hname = makeHistname("Cluster_column_width", false);
  htitles = makeHisttitle("Column width of a cluster", (atext_cluw + atext_nclu), false);
  sc = clusterExpert.regHist(m_cluster_col_width = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_nwid, min_nwid, max_nwid));

  hname = makeHistname("Cluster_row_width", false);
  htitles = makeHisttitle("Row width of a cluster", (atext_cluw + atext_nclu), false);
  sc = clusterExpert.regHist(m_cluster_row_width = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_nwid, min_nwid, max_nwid));

  hname = makeHistname("Cluster_LVL1A", true);
  htitles = makeHisttitle("Cluster Level 1 Accept", (atext_lvl1 + atext_nclu), true);
  sc = timeShift.regHist(m_cluster_LVL1A = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_lvl1, min_lvl1, max_lvl1));

  // per-layer histograms
  const unsigned int kNumLayers = m_doIBL ? PixLayer::COUNT : PixLayer::COUNT - 1;
  for (unsigned int i = 0; i < kNumLayers; i++) {
    enum PixLayerIBL2D3D::PixLayerIBL2D3DID i1 = (enum PixLayerIBL2D3D::PixLayerIBL2D3DID) i;
    hname = makeHistname(("Clusters_per_lumi_" + m_modLabel_PixLayerIBL2D3D[i1]), false);
    htitles = makeHisttitle(("Average number of pixel clusters per event per LB, " + m_modLabel_PixLayerIBL2D3D[i1]), (atext_LB + atext_clu), false);
    sc = clusterExpert.regHist(m_clusters_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

    hname = makeHistname(("Cluster_LVL1A_" + m_modLabel_PixLayerIBL2D3D[i1]), true);
    if (i != PixLayer::kIBL) {
      htitles = makeHisttitle(("Cluster Level 1 Accept with ToT > 15, " + m_modLabel_PixLayerIBL2D3D[i1]), (atext_lvl1 + atext_nclu), false);
    } else {
      htitles = makeHisttitle(("Cluster Level 1 Accept with ToT > 4, " + m_modLabel_PixLayerIBL2D3D[i1]), (atext_lvl1 + atext_nclu), false);
    }
    sc = timeExpert.regHist(m_cluster_LVL1A1d_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_lvl1, min_lvl1, max_lvl1));

    hname = makeHistname(("Cluster_groupsize_vs_eta_" + m_modLabel_PixLayerIBL2D3D[i1]), false);
    htitles = makeHisttitle(("Number of pixels per cluster vs eta, " + m_modLabel_PixLayerIBL2D3D[i1]), (atext_eta + atext_npix), false);
    sc = clusterExpert.regHist(m_clussize_vs_eta_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_eta, min_eta, max_eta));
  }

  for (int i = 0; i < PixLayerIBL2D3DDBM::COUNT; i++) {
    hname = makeHistname(("Cluster_ToT_" + m_modLabel_PixLayerIBL2D3DDBM[i]), false);
    htitles = makeHisttitle(("Cluster ToT, " + m_modLabel_PixLayerIBL2D3DDBM[i]), (atext_tot + atext_nclu), false);
    if (i < PixLayerIBL2D3DDBM::kIBL) {
      sc = clusterExpert.regHist(m_cluster_ToT1d_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_tot, min_tot, max_tot));
    } else {
      sc = clusterExpert.regHist(m_cluster_ToT1d_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_ibl_tot, min_ibl_tot, max_ibl_tot));
    }

    hname = makeHistname(("Cluster_Q_" + m_modLabel_PixLayerIBL2D3DDBM[i]), false);
    htitles = makeHisttitle(("Charge, " + m_modLabel_PixLayerIBL2D3DDBM[i]), (atext_Q + atext_nclu), false);
    sc = clusterExpert.regHist(m_cluster_Q_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_Q, min_Q, max_Q));

    if (m_doOnTrack) {
      hname = makeHistname(("Cluster_ToTxCosAlpha_" + m_modLabel_PixLayerIBL2D3DDBM[i]), false);
      htitles = makeHisttitle(("Cluster ToTxCosAlpha, " + m_modLabel_PixLayerIBL2D3DDBM[i]), (atext_tot + atext_nclu), false);
      if (i < PixLayerIBL2D3DDBM::kIBL) {
        sc = clusterExpert.regHist(m_cluster_ToT1d_corr[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_tot, min_tot, max_tot));
      } else {
        sc = clusterExpert.regHist(m_cluster_ToT1d_corr[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_ibl_tot, min_ibl_tot, max_ibl_tot));
      }
      hname = makeHistname(("Cluster_QxCosAlpha_" + m_modLabel_PixLayerIBL2D3DDBM[i]), false);
      htitles = makeHisttitle(("Corrected charge, " + m_modLabel_PixLayerIBL2D3DDBM[i]), (atext_Q + atext_nclu), false);
      sc = clusterExpert.regHist(m_cluster_Q_corr[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_Q, min_Q, max_Q));
    }

    hname = makeHistname(("Cluster_groupsize_" + m_modLabel_PixLayerIBL2D3DDBM[i]), false);
    htitles = makeHisttitle(("Number of pixels in a cluster, " + m_modLabel_PixLayerIBL2D3DDBM[i]), (atext_npix + atext_nclu), false);
    sc = clusterExpert.regHist(m_cluster_groupsize_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_npix + 1, min_npix, max_npix + 1));
  }

  for (int i = 0; i < PixLayerIBL2D3D::COUNT; i++) {
    hname = makeHistname(("Cluster_column_width_" + m_modLabel_PixLayerIBL2D3D[i]), false);
    htitles = makeHisttitle(("Column width of a cluster, " + m_modLabel_PixLayerIBL2D3D[i]), (atext_cluw + atext_nclu), false);
    sc = clusterExpert.regHist(m_cluster_col_width_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_nwid, min_nwid, max_nwid));

    hname = makeHistname(("Cluster_row_width_" + m_modLabel_PixLayerIBL2D3D[i]), false);
    htitles = makeHisttitle(("Row width of a cluster, " + m_modLabel_PixLayerIBL2D3D[i]), (atext_cluw + atext_nclu), false);
    sc = clusterExpert.regHist(m_cluster_row_width_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_nwid, min_nwid, max_nwid));

    hname = makeHistname(("TotalClusters_per_lumi_" + m_modLabel_PixLayerIBL2D3D[i]), false);
    htitles = makeHisttitle(("Total number of pixel clusters per LB, " + m_modLabel_PixLayerIBL2D3D[i]), (atext_LB + atext_nclu), false);
    sc = clusterExpert.regHist(m_totalclusters_per_lumi_mod[i] = TH1I_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

    hname = makeHistname(("TotalClusters_per_bcid_" + m_modLabel_PixLayerIBL2D3D[i]), false);
    htitles = makeHisttitle(("Total number of pixel clusters per BCID, " + m_modLabel_PixLayerIBL2D3D[i]), (atext_LB + atext_nclu), false);

    hname = makeHistname(("Clusters_row_width_per_lumi_" + m_modLabel_PixLayerIBL2D3D[i]), true);
    htitles = makeHisttitle("Average row width per LB", (atext_LB + atext_clu), true);
    sc = clusterShift.regHist(m_clusters_row_width_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

    hname = makeHistname(("Clusters_column_width_per_lumi_" + m_modLabel_PixLayerIBL2D3D[i]), true);
    htitles = makeHisttitle("Average column width per LB", (atext_LB + atext_clu), true);
    sc = clusterShift.regHist(m_clusters_col_width_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

    hname = makeHistname(("Clusters_row_width_per_bcid_" + m_modLabel_PixLayerIBL2D3D[i]), true);
    htitles = makeHisttitle("Average row width per BCID", (atext_BCID + atext_clu), true);
    sc = clusterShift.regHist(m_clusters_row_width_per_bcid_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_BCID, min_BCID, max_BCID));

    hname = makeHistname(("Clusters_column_width_per_bcid_" + m_modLabel_PixLayerIBL2D3D[i]), true);
    htitles = makeHisttitle("Average column width per BCID", (atext_BCID + atext_clu), true);
    sc = clusterShift.regHist(m_clusters_col_width_per_bcid_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_BCID, min_BCID, max_BCID));

    hname = makeHistname(("ClusterSize_OnTrack_" + m_modLabel_PixLayerIBL2D3D[i]), false);
    htitles = makeHisttitle(("cluster size  for clusters on tracks, " + m_modLabel_PixLayerIBL2D3D[i]), ";cluster size;#clusters", false);
    sc = clusterExpert.regHist(m_clusize_ontrack_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), 300, -0.5, -0.5 + 300.0));

    hname = makeHistname(("ClusterSize_OffTrack_" + m_modLabel_PixLayerIBL2D3D[i]), false);
    htitles = makeHisttitle(("cluster size  for clusters out of tracks , " + m_modLabel_PixLayerIBL2D3D[i]), ";cluster size;#clusters", false);
    sc = clusterExpert.regHist(m_clusize_offtrack_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), 300, -0.5, -0.5 + 300.0));
  }

  if (m_doOnTrack) {
    sc = clusterExpert.regHist(m_clustersOnOffTrack_per_lumi = TProfile_LW::create("ClustersOnOffTrack_per_lumi", ("Fraction pixel clusters on track per event per LB" + m_histTitleExt + ";lumi block; fraction clusters/event").c_str(), 2500, -0.5, 2499.5));

    hname = makeHistname("Zoomed_Cluster_ToTxCosAlpha_lumi_IBL", false);
    htitles = makeHisttitle("Zoomed Cluster ToTxCosAlpha, IBL", ";lumi block;ToT [BC]", false);
    sc = clusterExpert.regHist(m_zoomed_clusterToTcosA_lumi_IBL = TH2F_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB, 20, -0.5, 19.5));
    if (m_doOnline) {
      hname = makeHistname("Zoomed_Cluster_ToTxCosAlpha_last100lb_IBL", false);
      htitles = makeHisttitle("Zoomed Cluster ToTxCosAlpha last 100 LB, IBL", ";last 100 lumi blocks;ToT [BC]", false);
      sc = clusterExpert.regHist(m_zoomed_clusterToTcosA_lastXlb_IBL = new TH2F(hname.c_str(), htitles.c_str(), 100, 0.5, 100.5, 20, -0.5, 19.5));
      if (m_zoomed_clusterToTcosA_lastXlb_IBL) m_zoomed_clusterToTcosA_lastXlb_IBL->SetOption("colz");
    }
    hname = makeHistname("Zoomed_Cluster_ToTxCosAlpha_lumi_B0", false);
    htitles = makeHisttitle("Zoomed Cluster ToTxCosAlpha, B0", ";lumi block;ToT [BC]", false);
    sc = clusterExpert.regHist(m_zoomed_clusterToTcosA_lumi_B0 = TH2F_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB, 20, 7.5, 27.5));
    if (m_doOnline) {
      hname = makeHistname("Zoomed_Cluster_ToTxCosAlpha_last100lb_B0", false);
      htitles = makeHisttitle("Zoomed Cluster ToTxCosAlpha last 100 LB, B0", ";last 100 lumi blocks;ToT [BC]", false);
      sc = clusterExpert.regHist(m_zoomed_clusterToTcosA_lastXlb_B0 = new TH2F(hname.c_str(), htitles.c_str(), 100, 0.5, 100.5, 20, 7.5, 27.5));
      if (m_zoomed_clusterToTcosA_lastXlb_B0) m_zoomed_clusterToTcosA_lastXlb_B0->SetOption("colz");
    }
    hname = makeHistname("Zoomed_Cluster_ToTxCosAlpha_lumi_B1", false);
    htitles = makeHisttitle("Zoomed Cluster ToTxCosAlpha, B1", ";lumi block;ToT [BC]", false);
    sc = clusterExpert.regHist(m_zoomed_clusterToTcosA_lumi_B1 = TH2F_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB, 20, 19.5, 39.5));
    if (m_doOnline) {
      hname = makeHistname("Zoomed_Cluster_ToTxCosAlpha_last100lb_B1", false);
      htitles = makeHisttitle("Zoomed Cluster ToTxCosAlpha last 100 LB, B1", ";last 100 lumi blocks;ToT [BC]", false);
      sc = clusterExpert.regHist(m_zoomed_clusterToTcosA_lastXlb_B1 = new TH2F(hname.c_str(), htitles.c_str(), 100, 0.5, 100.5, 20, 19.5, 39.5));
      if (m_zoomed_clusterToTcosA_lastXlb_B1) m_zoomed_clusterToTcosA_lastXlb_B1->SetOption("colz");
    }
    hname = makeHistname("Zoomed_Cluster_ToTxCosAlpha_lumi_B2", false);
    htitles = makeHisttitle("Zoomed Cluster ToTxCosAlpha, B2", ";lumi block;ToT [BC]", false);
    sc = clusterExpert.regHist(m_zoomed_clusterToTcosA_lumi_B2 = TH2F_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB, 20, 19.5, 39.5));
    if (m_doOnline) {
      hname = makeHistname("Zoomed_Cluster_ToTxCosAlpha_last100lb_B2", false);
      htitles = makeHisttitle("Zoomed Cluster ToTxCosAlpha last 100 LB, B2", ";last 100 lumi blocks;ToT [BC]", false);
      sc = clusterExpert.regHist(m_zoomed_clusterToTcosA_lastXlb_B2 = new TH2F(hname.c_str(), htitles.c_str(), 100, 0.5, 100.5, 20, 19.5, 39.5));
      if (m_zoomed_clusterToTcosA_lastXlb_B2) m_zoomed_clusterToTcosA_lastXlb_B2->SetOption("colz");
    }
    hname = makeHistname("Zoomed_Cluster_ToTxCosAlpha_lumi_ECA", false);
    htitles = makeHisttitle("Zoomed Cluster ToTxCosAlpha, ECA", ";lumi block;ToT [BC]", false);
    sc = clusterExpert.regHist(m_zoomed_clusterToTcosA_lumi_ECA = TH2F_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB, 20, 19.5, 39.5));
    if (m_doOnline) {
      hname = makeHistname("Zoomed_Cluster_ToTxCosAlpha_last100lb_ECA", false);
      htitles = makeHisttitle("Zoomed Cluster ToTxCosAlpha last 100 LB, ECA", ";last 100 lumi blocks;ToT [BC]", false);
      sc = clusterExpert.regHist(m_zoomed_clusterToTcosA_lastXlb_ECA = new TH2F(hname.c_str(), htitles.c_str(), 100, 0.5, 100.5, 20, 19.5, 39.5));
      if (m_zoomed_clusterToTcosA_lastXlb_ECA) m_zoomed_clusterToTcosA_lastXlb_ECA->SetOption("colz");
    }
    hname = makeHistname("Zoomed_Cluster_ToTxCosAlpha_lumi_ECC", false);
    htitles = makeHisttitle("Zoomed Cluster ToTxCosAlpha, ECC", ";lumi block;ToT [BC]", false);
    sc = clusterExpert.regHist(m_zoomed_clusterToTcosA_lumi_ECC = TH2F_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB, 20, 19.5, 39.5));
    if (m_doOnline) {
      hname = makeHistname("Zoomed_Cluster_ToTxCosAlpha_last100lb_ECC", false);
      htitles = makeHisttitle("Zoomed Cluster ToTxCosAlpha last 100 LB, ECC", ";last 100 lumi blocks;ToT [BC]", false);
      sc = clusterExpert.regHist(m_zoomed_clusterToTcosA_lastXlb_ECC = new TH2F(hname.c_str(), htitles.c_str(), 100, 0.5, 100.5, 20, 19.5, 39.5));
      if (m_zoomed_clusterToTcosA_lastXlb_ECC) m_zoomed_clusterToTcosA_lastXlb_ECC->SetOption("colz");
    }

  }

  if (m_do2DMaps) {
    tmp = "Cluster_Occupancy";
    tmp2 = "Cluster occupancy";
    m_cluster_occupancy = std::make_unique<PixelMon2DMapsLW>(tmp.c_str(), (tmp2 + m_histTitleExt).c_str(), PixMon::HistConf::kPixDBMIBL2D3D);
    sc = m_cluster_occupancy->regHist(clusterShift);

    tmp = "Cluster_LVL1A_Mod";
    tmp2 = "Average cluster Level 1 Accept";
    m_cluster_LVL1A_mod = std::make_unique<PixelMon2DProfilesLW>(tmp.c_str(), (tmp2 + m_histTitleExt).c_str(), PixMon::HistConf::kPixIBL2D3D);
    sc = m_cluster_LVL1A_mod->regHist(timeShift);

    tmp = "Clus_Occ_SizeCut";
    tmp2 = "Size>1 Cluster occupancy";
    m_clusocc_sizenot1 = std::make_unique<PixelMon2DMapsLW>(tmp.c_str(), (tmp2 + m_histTitleExt).c_str(), PixMon::HistConf::kPixDBMIBL2D3D);
    sc = m_clusocc_sizenot1->regHist(clusterShift);

    tmp = "Clus_LVL1A_SizeCut";
    tmp2 = "Average Size>1 Cluster Level 1 Accept";
    m_clus_LVL1A_sizenot1 = std::make_unique<PixelMon2DProfilesLW>(tmp.c_str(), (tmp2 + m_histTitleExt).c_str(), PixMon::HistConf::kPixIBL2D3D);
    sc = m_clus_LVL1A_sizenot1->regHist(timeShift);

    if (m_doOnline) {
      tmp = "ClusterMap_Mon";
      tmp2 = "Cluster map for monitoring";
      m_clustermap_mon = std::make_unique<PixelMon2DMapsLW>(tmp.c_str(), (tmp2 + m_histTitleExt).c_str(), PixMon::HistConf::kPixDBMIBL2D3D);
      sc = m_clustermap_mon->regHist(clusterShift);

      tmp = "ClusterMap_tmp";
      tmp2 = "Cluster map for monitoring";
      m_clustermap_tmp = std::make_unique<PixelMon2DMapsLW>(tmp.c_str(), (tmp2 + m_histTitleExt).c_str(), PixMon::HistConf::kPixDBMIBL2D3D);
      sc = m_clustermap_tmp->regHist(clusterShift);
    }
    if (!m_doOnline) {
      tmp = "Cluster_Size_Map";
      tmp2 = "Average cluster size map";
      m_clussize_map = std::make_unique<PixelMon2DProfilesLW>(tmp.c_str(), (tmp2 + m_histTitleExt).c_str(), PixMon::HistConf::kPixIBL2D3D);
      sc = m_clussize_map->regHist(clusterExpert);

      tmp = "Cluster_Charge_Map";
      tmp2 = "Average cluster charge map";
      m_cluscharge_map = std::make_unique<PixelMon2DProfilesLW>(tmp.c_str(), (tmp2 + m_histTitleExt).c_str(), PixMon::HistConf::kPixIBL2D3D);
      sc = m_cluscharge_map->regHist(clusterExpert);

      tmp = "Cluster_ToT_Map";
      tmp2 = "Average cluster ToT map";
      m_clusToT_map = std::make_unique<PixelMon2DProfilesLW>(tmp.c_str(), (tmp2 + m_histTitleExt).c_str(), PixMon::HistConf::kPixIBL2D3D);
      sc = m_clusToT_map->regHist(clusterExpert);
    }
  }

  if (m_doModules && m_doOnTrack) {
    m_cluseff_mod = std::make_unique<PixelMonModulesProf>("Clus_track_eff", ("Proportion of clusters on track vs t in module" + m_histTitleExt).c_str(), 2500, -0.5, 2499.5);
    sc = m_cluseff_mod->regHist(this, (path + "/Modules_Cluseff").c_str(), run);

    m_cluster_size_mod = std::make_unique<PixelMonModules1D>("Cluster_size", ("Cluster size in Module" + m_histTitleExt).c_str(), 20, -0.5, 19.5);
    sc = m_cluster_size_mod->regHist(this, (path + "/Modules_ClusterSize").c_str(), run);

    m_cluster_num_mod = std::make_unique<PixelMonModules1D>("Cluster_num", ("Number of clusters per event in module" + m_histTitleExt).c_str(), 30, -0.5, 29.5);
    sc = m_cluster_num_mod->regHist(this, (path + "/Modules_NumberOfClusters").c_str(), run);

    m_cluster_ToT_mod = std::make_unique<PixelMonModules1D>("Cluster_ToT", ("Cluster ToT in Module" + m_histTitleExt).c_str(), 200, 0., 200.);
    sc = m_cluster_ToT_mod->regHist(this, (path + "/Modules_ClusToT").c_str(), run);
  }
  if (m_doOnline) {
    if (m_doOnTrack) {
      if (m_doIBL) {
	hname = makeHistname("Cluster_Occupancy_PP0_IBLA", false);
	htitles = makeHisttitle("Average per FE cluster occupancy reset every 5 min, IBL A-side", ";stave ;average # clusters per FE per event", false);
	sc = clusterExpert.regHist(m_cluster_occupancy_PP0_IBLA = TProfile_LW::create(hname.c_str(), htitles.c_str(), 
									  PixMon::kNumStavesIBL, 0.5, 0.5 + PixMon::kNumStavesIBL));
	hname = makeHistname("Cluster_Occupancy_PP0_IBLC", false);
	htitles = makeHisttitle("Average per FE cluster occupancy reset every 5 min, IBL C-side", ";stave ;average # clusters per FE per event", false);
	sc = clusterExpert.regHist(m_cluster_occupancy_PP0_IBLC = TProfile_LW::create(hname.c_str(), htitles.c_str(), 
									  PixMon::kNumStavesIBL, 0.5, 0.5 + PixMon::kNumStavesIBL));
      }
      hname = makeHistname("Cluster_Occupancy_PP0_B0", false);
      htitles = makeHisttitle("Average per module cluster occupancy reset every 5 min, B0", ";stave ;average # clusters per module per event", false);
      sc = clusterExpert.regHist(m_cluster_occupancy_PP0_B0 = TProfile_LW::create(hname.c_str(), htitles.c_str(),
								      PixMon::kNumStavesL0, 0.5, 0.5 + PixMon::kNumStavesL0));
      hname = makeHistname("Cluster_Occupancy_PP0_B1", false);
      htitles = makeHisttitle("Average per module cluster occupancy reset every 5 min, B1", ";stave ;average # clusters per module per event", false);
      sc = clusterExpert.regHist(m_cluster_occupancy_PP0_B1 = TProfile_LW::create(hname.c_str(), htitles.c_str(),
								      PixMon::kNumStavesL1, 0.5, 0.5 + PixMon::kNumStavesL1));
      hname = makeHistname("Cluster_Occupancy_PP0_B2", false);
      htitles = makeHisttitle("Average per module cluster occupancy reset every 5 min, B2", ";stave ;average # clusters per module per event", false);
      sc = clusterExpert.regHist(m_cluster_occupancy_PP0_B2 = TProfile_LW::create(hname.c_str(), htitles.c_str(),
								      PixMon::kNumStavesL2, 0.5, 0.5 + PixMon::kNumStavesL2));
      hname = makeHistname("Cluster_Occupancy_PP0_ECA", false);
      htitles = makeHisttitle("Average per module cluster occupancy reset every 5 min, ECA", ";sector ;average # clusters per module per event", false);
      sc = clusterExpert.regHist(m_cluster_occupancy_PP0_ECA = TProfile_LW::create(hname.c_str(), htitles.c_str(),
								       PixMon::kNumPP0sEC, 0.5, 0.5 + PixMon::kNumPP0sEC));
      hname = makeHistname("Cluster_Occupancy_PP0_ECC", false);
      htitles = makeHisttitle("Average per module cluster occupancy reset every 5 min, ECC", ";sector ;average # clusters per module per event", false);
      sc = clusterExpert.regHist(m_cluster_occupancy_PP0_ECC = TProfile_LW::create(hname.c_str(), htitles.c_str(),
								       PixMon::kNumPP0sEC, 0.5, 0.5 + PixMon::kNumPP0sEC));
      formatPP0Histos(m_cluster_occupancy_PP0_ECA, m_cluster_occupancy_PP0_ECC,
			m_cluster_occupancy_PP0_B0, m_cluster_occupancy_PP0_B1, m_cluster_occupancy_PP0_B2,
			m_cluster_occupancy_PP0_IBLA, m_cluster_occupancy_PP0_IBLC);
    }
  }

  if (m_doLowOccupancy || m_doHighOccupancy) {
    int nbins_clusters = 1000;
    float max_clusters = 40000.;
    int nbins_clusters_region = 200;
    float max_clusters_region = 8000.;
    int nbins_avclusters = 100;
    float max_avclusters = 50.;

    if (m_doLowOccupancy) {
      nbins_clusters = 200;
      max_clusters = 200.;
      nbins_clusters_region = 200;
      max_clusters_region = 8000.;
      nbins_avclusters = 50;
      max_avclusters = 2.;
    }
    if (m_doHeavyIonMon) {
      max_clusters = 100000;
      max_clusters_region = 40000;
      max_avclusters = 1000;
    }

    tmp = "num_clusters";
    tmp2 = "Number of pixel clusters in an event";
    sc = clusterShift.regHist(m_num_clusters = TH1I_LW::create(tmp.c_str(), (tmp2 + ";# pixel clusters/event" + m_histTitleExt + ";# events").c_str(), nbins_clusters, 0., max_clusters));

    for (int i = 0; i < PixLayer::COUNT - 1 + (int)(m_doIBL); i++) {
      hname = makeHistname(("num_clusters_" + m_modLabel_PixLayerIBL2D3D[i]), false);
      tmp2 = "Number of pixel clusters in an event, " + m_modLabel_PixLayerIBL2D3D[i] + m_histTitleExt + ";# pixel clusters/event;# events";
      sc = clusterExpert.regHist(m_num_clusters_mod[i] = TH1I_LW::create(hname.c_str(), tmp2.c_str(), nbins_clusters_region, 0., max_clusters_region));

      hname = makeHistname(("cluster_occupancy_summary_" + m_modLabel_PixLayerIBL2D3D[i]), true);
      tmp2 = "Average cluster occupancy, " + m_modLabel_PixLayerIBL2D3D[i] + m_histTitleExt + ";average # of clusters per module per event;# modules";
      sc = clusterExpert.regHist(m_cluster_occupancy_summary_mod[i] = TH1F_LW::create(hname.c_str(), tmp2.c_str(), nbins_avclusters, 0., max_avclusters));

      if (m_doOnTrack) {
        hname = makeHistname(("num_clusters_per_track_per_lumi_" + m_modLabel_PixLayerIBL2D3D[i]), true);
        tmp2 = "Number of pixel clusters in an event, " + m_modLabel_PixLayerIBL2D3D[i] + m_histTitleExt + atext_LB + ";Avg #clusters/track/event";
        sc = clusterExpert.regHist(m_clusters_per_track_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), tmp2.c_str(), nbins_LB, min_LB, max_LB));
      }
    }
  }

  if (sc.isFailure() && msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Pixel cluster monitoring histograms not booked" << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::bookClustersLumiBlockMon(void) {
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "starting Book Clusters for lowStat" << endmsg;

  std::string path = "Pixel/LumiBlock";
  if (m_doOnTrack) path.replace(path.begin(), path.end(), "Pixel/LumiBlockOnTrack");
  MonGroup lumiBlockHist(this, path.c_str(), lowStat, ATTRIB_MANAGED);  // declare a group of histograms

  StatusCode sc;

  sc = lumiBlockHist.regHist(m_cluster_ToT_LB = TH1F_LW::create("Cluster_ToT_LB", ("Cluster Time over Threshold" + m_histTitleExt + ";ToT;# clusters").c_str(), 300, -0.5, 299.5));

  if (m_do2DMaps) {
    m_cluster_occupancy_LB = std::make_unique<PixelMon2DMapsLW>("Cluster_Occupancy_LB", ("Cluster Occupancy" + m_histTitleExt).c_str(), PixMon::HistConf::kPixDBMIBL2D3D);
    sc = m_cluster_occupancy_LB->regHist(lumiBlockHist);
  }
  if (m_doLowOccupancy || m_doHighOccupancy) {
    int nbins_nclusters = 150;
    double min_nclusters = -0.5;
    double max_nclusters = min_nclusters + 15000.0;
    if (m_doLowOccupancy) {
      nbins_nclusters = 200;
      max_nclusters = min_nclusters + 200.0;
    }
    sc = lumiBlockHist.regHist(m_num_clusters_LB = TH1I_LW::create("num_clusters_LB", ("Number of pixel clusters per event" + m_histTitleExt + ";# pixel clusters/event;# events").c_str(), nbins_nclusters, min_nclusters, max_nclusters));
  }

  // normally not booked, as doModules is online, doLumiBlock is offline
  if (m_doModules && m_doOnTrack) {
    m_cluster_num_mod_LB = std::make_unique<PixelMonModules1D>(PixelMonModules1D("Cluster_num_LB", ("Number of clusters per event in module" + m_histTitleExt).c_str(), 20, -0.5, 59.5));
    sc = m_cluster_num_mod_LB->regHist(this, (path + "/Modules_NumberOfClusters").c_str(), lowStat);
    m_cluster_ToT_mod_LB = std::make_unique<PixelMonModules1D>(PixelMonModules1D("Cluster_ToT_mod_LB", ("Cluster ToT in Module" + m_histTitleExt).c_str(), 75, 0., 300.));
    sc = m_cluster_ToT_mod_LB->regHist(this, (path + "/Modules_ClusToT").c_str(), lowStat);
  }

  if (sc.isFailure() && msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Per LB pixel cluster monitoring histograms not booked" << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::fillClustersMon(void) {
  auto pixel_clcontainer = SG::makeHandle(m_Pixel_SiClustersName);
  if (!(pixel_clcontainer.isValid())) {
    if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Pixel Cluster container for Pixels not found" << endmsg;
    if (m_storegate_errors) m_storegate_errors->Fill(3., 3.);
    return StatusCode::SUCCESS;
  } else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Pixel Cluster container for Pixels found" << endmsg;
  }

  int nclusters = 0;
  int nclusters_mod[PixLayer::COUNT] = {0};
  int nclusters_all = 0;
  int nclusters_ontrack = 0;

  if (m_doModules && m_doOnTrack) {
    if (m_doIBL) {
      for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 14; j++) {
          m_ClusPerEventArray_lI[j][i] = 0;
        }
      }
    }
    for (int i = 0; i < 13; i++) {
      for (int j = 0; j < 22; j++) {
        m_ClusPerEventArray_l0[j][i] = 0;
      }
      for (int j = 0; j < 38; j++) {
        m_ClusPerEventArray_l1[j][i] = 0;
      }
      for (int j = 0; j < 52; j++) {
        m_ClusPerEventArray_l2[j][i] = 0;
      }
    }
    for (int j = 0; j < 3; j++) {
      for (int i = 0; i < 48; i++) {
        m_ClusPerEventArray_disksA[i][j] = 0;
        m_ClusPerEventArray_disksC[i][j] = 0;
      }
    }
  }

  Identifier clusID;
  InDet::PixelClusterContainer::const_iterator colNext = pixel_clcontainer->begin();
  InDet::PixelClusterContainer::const_iterator lastCol = pixel_clcontainer->end();
  DataVector<PixelCluster>::const_iterator p_clus;

  for (; colNext != lastCol; ++colNext) {
    const InDet::PixelClusterCollection* ClusterCollection(*colNext);

    if (!ClusterCollection) {
      // first entry is for RDO, second is for data problem
      if (m_storegate_errors) m_storegate_errors->Fill(3., 5.);
      continue;
    }
    for (p_clus = ClusterCollection->begin(); p_clus != ClusterCollection->end(); ++p_clus) {
      clusID = (*p_clus)->identify();
      int pixlayer = getPixLayerID(m_pixelid->barrel_ec(clusID), m_pixelid->layer_disk(clusID), m_doIBL);
      int pixlayeribl2d3d = pixlayer;
      if (pixlayeribl2d3d == PixLayerIBL2D3D::kIBL) {
        pixlayeribl2d3d = getPixLayerIDIBL2D3D(m_pixelid->barrel_ec(clusID), m_pixelid->layer_disk(clusID), m_pixelid->eta_module(clusID), m_doIBL);
      }

      int pixlayerdbm = getPixLayerIDDBM(m_pixelid->barrel_ec(clusID), m_pixelid->layer_disk(clusID), m_doIBL);
      int pixlayeribl2d3ddbm = pixlayerdbm;
      if (pixlayeribl2d3ddbm == PixLayerIBL2D3DDBM::kIBL) {
        pixlayeribl2d3ddbm = getPixLayerIDIBL2D3DDBM(m_pixelid->barrel_ec(clusID), m_pixelid->layer_disk(clusID), m_pixelid->eta_module(clusID), m_doIBL);
      }

      int lumiblock = m_manager->lumiBlockNumber();

      const PixelCluster& cluster = **p_clus;
      const InDet::SiWidth clusWidth = cluster.width();
      // note: description is wrong in SiWidth! <x,y> = <row,col>
      int colWidth = clusWidth.colRow().y();
      int rowWidth = clusWidth.colRow().x();

      int npixHitsInClusterRaw = cluster.rdoList().size();
      int npixHitsInCluster = cluster.rdoList().size();
      if (pixlayeribl2d3ddbm != 99 && npixHitsInCluster > m_cluster_groupsize_mod[pixlayeribl2d3ddbm]->getXMax()) {
        npixHitsInCluster = (int)(m_cluster_groupsize_mod[pixlayeribl2d3ddbm]->getXMax() - 0.5);
      }

      if (pixlayer != 99) nclusters_all++;  // count all (no DBM) clusters on and off track

      if (isOnTrack(clusID, true)) {
        if (m_cluseff_mod) m_cluseff_mod->fill(m_manager->lumiBlockNumber(), 1., clusID, m_pixelid);
        if (pixlayeribl2d3d != 99 && m_clusize_ontrack_mod[pixlayeribl2d3d]) m_clusize_ontrack_mod[pixlayeribl2d3d]->Fill(npixHitsInClusterRaw);
        if (pixlayer == PixLayer::kIBL && m_clusize_ontrack_mod[PixLayerIBL2D3D::kIBL]) m_clusize_ontrack_mod[PixLayerIBL2D3D::kIBL]->Fill(npixHitsInClusterRaw);      
      } else {
        if (m_cluseff_mod) m_cluseff_mod->fill(m_manager->lumiBlockNumber(), 0., clusID, m_pixelid);
        if (pixlayeribl2d3d != 99 && m_clusize_offtrack_mod[pixlayeribl2d3d]) m_clusize_offtrack_mod[pixlayeribl2d3d]->Fill(npixHitsInClusterRaw);
        if (pixlayer == PixLayer::kIBL && m_clusize_offtrack_mod[PixLayerIBL2D3D::kIBL]) m_clusize_offtrack_mod[PixLayerIBL2D3D::kIBL]->Fill(npixHitsInClusterRaw);
      }

      double cosalpha(0.);
      if (m_doOnTrack && !isOnTrack(clusID, cosalpha)) {
        continue;
        // if we only want hits on track, and the hit is NOT on the track, skip filling.
        // true means doing clusters, false means rdos
      }

      //////////////////////////////////////////////////////
      /////////////Start main fill block here///////////////
      //////////////////////////////////////////////////////

      // Fill first histograms with DBM
      if (pixlayeribl2d3ddbm != 99 && m_cluster_ToT1d_mod[pixlayeribl2d3ddbm]) m_cluster_ToT1d_mod[pixlayeribl2d3ddbm]->Fill(cluster.totalToT());
      if (pixlayerdbm == PixLayerDBM::kIBL && m_cluster_ToT1d_mod[PixLayerIBL2D3DDBM::kIBL]) m_cluster_ToT1d_mod[PixLayerIBL2D3DDBM::kIBL]->Fill(cluster.totalToT());
      if (pixlayeribl2d3ddbm != 99 && m_cluster_Q_mod[pixlayeribl2d3ddbm]) m_cluster_Q_mod[pixlayeribl2d3ddbm]->Fill(cluster.totalCharge());
      if (pixlayerdbm == PixLayerDBM::kIBL && m_cluster_Q_mod[PixLayerIBL2D3DDBM::kIBL]) m_cluster_Q_mod[PixLayerIBL2D3DDBM::kIBL]->Fill(cluster.totalCharge());
      if (m_doOnTrack) {
        if (pixlayeribl2d3ddbm != 99 && m_cluster_ToT1d_corr[pixlayeribl2d3ddbm]) m_cluster_ToT1d_corr[pixlayeribl2d3ddbm]->Fill(cluster.totalToT() * cosalpha);
        if (pixlayerdbm == PixLayerDBM::kIBL && m_cluster_ToT1d_corr[PixLayerIBL2D3DDBM::kIBL]) m_cluster_ToT1d_corr[PixLayerIBL2D3DDBM::kIBL]->Fill(cluster.totalToT() * cosalpha);
        if (pixlayeribl2d3ddbm != 99 && m_cluster_Q_corr[pixlayeribl2d3ddbm]) m_cluster_Q_corr[pixlayeribl2d3ddbm]->Fill(cluster.totalCharge() * cosalpha);
        if (pixlayerdbm == PixLayerDBM::kIBL && m_cluster_Q_corr[PixLayerIBL2D3DDBM::kIBL]) m_cluster_Q_corr[PixLayerIBL2D3DDBM::kIBL]->Fill(cluster.totalCharge() * cosalpha);
	if (pixlayer == PixLayer::kIBL && m_zoomed_clusterToTcosA_lumi_IBL) m_zoomed_clusterToTcosA_lumi_IBL->Fill(m_manager->lumiBlockNumber(), cluster.totalToT() * cosalpha);
	if (pixlayer == PixLayer::kB0 && m_zoomed_clusterToTcosA_lumi_B0) m_zoomed_clusterToTcosA_lumi_B0->Fill(m_manager->lumiBlockNumber(), cluster.totalToT() * cosalpha);
	if (pixlayer == PixLayer::kB1 && m_zoomed_clusterToTcosA_lumi_B1) m_zoomed_clusterToTcosA_lumi_B1->Fill(m_manager->lumiBlockNumber(), cluster.totalToT() * cosalpha);
	if (pixlayer == PixLayer::kB2 && m_zoomed_clusterToTcosA_lumi_B2) m_zoomed_clusterToTcosA_lumi_B2->Fill(m_manager->lumiBlockNumber(), cluster.totalToT() * cosalpha);
	if (pixlayer == PixLayer::kECA && m_zoomed_clusterToTcosA_lumi_ECA) m_zoomed_clusterToTcosA_lumi_ECA->Fill(m_manager->lumiBlockNumber(), cluster.totalToT() * cosalpha);
	if (pixlayer == PixLayer::kECC && m_zoomed_clusterToTcosA_lumi_ECC) m_zoomed_clusterToTcosA_lumi_ECC->Fill(m_manager->lumiBlockNumber(), cluster.totalToT() * cosalpha);
      }
      if (pixlayeribl2d3ddbm != 99 && m_cluster_groupsize_mod[pixlayeribl2d3ddbm]) m_cluster_groupsize_mod[pixlayeribl2d3ddbm]->Fill(npixHitsInCluster);
      if (pixlayerdbm == PixLayerDBM::kIBL && m_cluster_groupsize_mod[PixLayerIBL2D3DDBM::kIBL]) m_cluster_groupsize_mod[PixLayerIBL2D3DDBM::kIBL]->Fill(npixHitsInCluster);
      if (m_cluster_occupancy) m_cluster_occupancy->fill(clusID, m_pixelid);
      if (pixlayer == 99) continue;  // DBM case

      nclusters_ontrack++;

      // Fill LVL1 Accepted
      if (m_cluster_LVL1A) m_cluster_LVL1A->Fill(cluster.LVL1A());
      if (m_cluster_LVL1A_mod) m_cluster_LVL1A_mod->fill(clusID, m_pixelid, cluster.LVL1A() + 0.00001);                                      // avoid filling exactly zero to distinguish from disabled modules
      if (cluster.rdoList().size() > 1 && m_clus_LVL1A_sizenot1) m_clus_LVL1A_sizenot1->fill(clusID, m_pixelid, cluster.LVL1A() + 0.00001);  // avoid filling exactly zero to distinguish from disabled modules
      if (pixlayer != PixLayer::kIBL) {
        if (cluster.totalToT() > 15 && m_cluster_LVL1A1d_mod[pixlayer]) m_cluster_LVL1A1d_mod[pixlayer]->Fill(cluster.LVL1A());
      } else {
        if (cluster.totalToT() > 4 && m_cluster_LVL1A1d_mod[pixlayer]) m_cluster_LVL1A1d_mod[pixlayer]->Fill(cluster.LVL1A());
      }

      if (m_cluster_ToT_mod) m_cluster_ToT_mod->fill(cluster.totalToT(), clusID, m_pixelid);

      // Fill the number of pixel hits in a cluster
      if (m_cluster_groupsize) m_cluster_groupsize->Fill(npixHitsInClusterRaw);
      if (m_cluster_col_width) m_cluster_col_width->Fill(colWidth);
      if (m_cluster_row_width) m_cluster_row_width->Fill(rowWidth);
      if (m_clusterSize_eta && m_pixelid->barrel_ec(clusID) == 0) m_clusterSize_eta->Fill(m_pixelid->eta_module(clusID), npixHitsInClusterRaw);

      if (pixlayeribl2d3d != 99 && m_cluster_col_width_mod[pixlayeribl2d3d]) m_cluster_col_width_mod[pixlayeribl2d3d]->Fill(colWidth);
      if (pixlayer == PixLayer::kIBL && m_cluster_col_width_mod[PixLayerIBL2D3D::kIBL]) m_cluster_col_width_mod[PixLayerIBL2D3D::kIBL]->Fill(colWidth);
      if (pixlayeribl2d3d != 99 && m_cluster_row_width_mod[pixlayeribl2d3d]) m_cluster_row_width_mod[pixlayeribl2d3d]->Fill(rowWidth);
      if (pixlayer == PixLayer::kIBL && m_cluster_row_width_mod[PixLayerIBL2D3D::kIBL]) m_cluster_row_width_mod[PixLayerIBL2D3D::kIBL]->Fill(rowWidth);
      if (pixlayeribl2d3d != 99 && m_clusters_col_width_per_lumi_mod[pixlayeribl2d3d]) m_clusters_col_width_per_lumi_mod[pixlayeribl2d3d]->Fill(m_manager->lumiBlockNumber(), colWidth);
      if (pixlayer == PixLayer::kIBL && m_clusters_col_width_per_lumi_mod[PixLayerIBL2D3D::kIBL]) m_clusters_col_width_per_lumi_mod[PixLayerIBL2D3D::kIBL]->Fill(lumiblock, colWidth);
      if (pixlayeribl2d3d != 99 && m_clusters_row_width_per_lumi_mod[pixlayeribl2d3d]) m_clusters_row_width_per_lumi_mod[pixlayeribl2d3d]->Fill(m_manager->lumiBlockNumber(), rowWidth);
      if (pixlayer == PixLayer::kIBL && m_clusters_row_width_per_lumi_mod[PixLayerIBL2D3D::kIBL]) m_clusters_row_width_per_lumi_mod[PixLayerIBL2D3D::kIBL]->Fill(lumiblock, rowWidth);
      if (pixlayeribl2d3d != 99 && m_clusters_col_width_per_bcid_mod[pixlayeribl2d3d]) m_clusters_col_width_per_bcid_mod[pixlayeribl2d3d]->Fill(1.0 * m_currentBCID, colWidth);
      if (pixlayer == PixLayer::kIBL && m_clusters_col_width_per_bcid_mod[PixLayerIBL2D3D::kIBL]) m_clusters_col_width_per_bcid_mod[PixLayerIBL2D3D::kIBL]->Fill(1.0 * m_currentBCID, colWidth);
      if (pixlayeribl2d3d != 99 && m_clusters_row_width_per_bcid_mod[pixlayeribl2d3d]) m_clusters_row_width_per_bcid_mod[pixlayeribl2d3d]->Fill(1.0 * m_currentBCID, rowWidth);
      if (pixlayer == PixLayer::kIBL && m_clusters_row_width_per_bcid_mod[PixLayerIBL2D3D::kIBL]) m_clusters_row_width_per_bcid_mod[PixLayerIBL2D3D::kIBL]->Fill(1.0 * m_currentBCID, rowWidth);

      // Total Number of Clusters
      if (m_totalclusters_per_lumi) m_totalclusters_per_lumi->Fill(lumiblock);
      if (pixlayeribl2d3d != 99 && m_totalclusters_per_lumi_mod[pixlayeribl2d3d]) m_totalclusters_per_lumi_mod[pixlayeribl2d3d]->Fill(lumiblock);
      if (pixlayer == PixLayer::kIBL && m_totalclusters_per_lumi_mod[PixLayerIBL2D3D::kIBL]) m_totalclusters_per_lumi_mod[PixLayerIBL2D3D::kIBL]->Fill(lumiblock);
      if (pixlayeribl2d3d != 99 && m_totalclusters_per_bcid_mod[pixlayeribl2d3d]) m_totalclusters_per_bcid_mod[pixlayeribl2d3d]->Fill(1.0 * m_currentBCID);
      if (pixlayer == PixLayer::kIBL && m_totalclusters_per_bcid_mod[PixLayerIBL2D3D::kIBL]) m_totalclusters_per_bcid_mod[PixLayerIBL2D3D::kIBL]->Fill(1.0 * m_currentBCID);

      // Fill Occupancy
      if (cluster.rdoList().size() > 1 && m_clusocc_sizenot1) m_clusocc_sizenot1->fill(clusID, m_pixelid);
      if (m_doOnline && m_clustermap_tmp) m_clustermap_tmp->fill(clusID, m_pixelid);

      // 2D Map
      if (m_clussize_map) m_clussize_map->fill(clusID, m_pixelid, cluster.rdoList().size());
      if (m_cluscharge_map) m_cluscharge_map->fill(clusID, m_pixelid, cluster.totalCharge());
      if (m_clusToT_map) m_clusToT_map->fill(clusID, m_pixelid, cluster.totalToT());
      if (m_cluster_size_mod) m_cluster_size_mod->fill(cluster.rdoList().size(), clusID, m_pixelid);

      if (m_clussize_vs_eta_mod[pixlayer]) m_clussize_vs_eta_mod[pixlayer]->Fill(1.0 * m_pixelid->eta_module(clusID), cluster.rdoList().size());

      nclusters++;
      nclusters_mod[pixlayer]++;

      if (m_doModules && m_doOnTrack) {
        if (m_pixelid->barrel_ec(clusID) == 2) m_ClusPerEventArray_disksA[m_pixelid->phi_module(clusID)][m_pixelid->layer_disk(clusID)]++;
        if (m_pixelid->barrel_ec(clusID) == -2) m_ClusPerEventArray_disksC[m_pixelid->phi_module(clusID)][m_pixelid->layer_disk(clusID)]++;
        if (m_pixelid->barrel_ec(clusID) == 0) {
          if (m_doIBL && m_pixelid->layer_disk(clusID) == 0) m_ClusPerEventArray_lI[m_pixelid->phi_module(clusID)][m_pixelid->eta_module(clusID) + 10]++;
          if (m_pixelid->layer_disk(clusID) == 0 + m_doIBL) m_ClusPerEventArray_l0[m_pixelid->phi_module(clusID)][m_pixelid->eta_module(clusID) + 6]++;
          if (m_pixelid->layer_disk(clusID) == 1 + m_doIBL) m_ClusPerEventArray_l1[m_pixelid->phi_module(clusID)][m_pixelid->eta_module(clusID) + 6]++;
          if (m_pixelid->layer_disk(clusID) == 2 + m_doIBL) m_ClusPerEventArray_l2[m_pixelid->phi_module(clusID)][m_pixelid->eta_module(clusID) + 6]++;
        }
      }

      if (m_doLumiBlock) {
        if (m_cluster_occupancy_LB) m_cluster_occupancy_LB->fill(clusID, m_pixelid);
        if (m_cluster_ToT_LB) m_cluster_ToT_LB->Fill(cluster.totalToT());
        if (m_cluster_ToT_mod_LB) m_cluster_ToT_mod_LB->fill(cluster.totalToT(), clusID, m_pixelid);
      }
    }  // PixelClusterContainer loop
  }    // end of cluster collections loop

  if (m_doOnline) {
    fillSummaryHistos(m_cluster_occupancy.get(),
                      m_cluster_occupancy_summary_mod[PixLayer::kECA],
                      m_cluster_occupancy_summary_mod[PixLayer::kECC],
                      m_cluster_occupancy_summary_mod[PixLayer::kIBL],
                      m_cluster_occupancy_summary_mod[PixLayer::kB0],
                      m_cluster_occupancy_summary_mod[PixLayer::kB1],
                      m_cluster_occupancy_summary_mod[PixLayer::kB2]);
    if (m_doOnTrack && m_doRefresh5min) {
      if (m_cluster_occupancy_PP0_ECA) m_cluster_occupancy_PP0_ECA->Reset(); 
      if (m_cluster_occupancy_PP0_ECC) m_cluster_occupancy_PP0_ECC->Reset();
      if (m_cluster_occupancy_PP0_B0) m_cluster_occupancy_PP0_B0->Reset();
      if (m_cluster_occupancy_PP0_B1) m_cluster_occupancy_PP0_B1->Reset();
      if (m_cluster_occupancy_PP0_B2) m_cluster_occupancy_PP0_B2->Reset();
      if (m_cluster_occupancy_PP0_IBLA) m_cluster_occupancy_PP0_IBLA->Reset();
      if (m_cluster_occupancy_PP0_IBLC) m_cluster_occupancy_PP0_IBLC->Reset();
    }
    if (m_doRefresh) { //set to true for testing
      if (m_clustermap_mon && m_clustermap_tmp) {
	if (m_doOnTrack) {
	  fillPP0Histos(m_clustermap_tmp.get(),
			m_cluster_occupancy_PP0_ECA, m_cluster_occupancy_PP0_ECC,
			m_cluster_occupancy_PP0_B0, m_cluster_occupancy_PP0_B1, m_cluster_occupancy_PP0_B2,
			m_cluster_occupancy_PP0_IBLA, m_cluster_occupancy_PP0_IBLC);
	}
	m_clustermap_mon->fill2DMon(m_clustermap_tmp.get());
      }
    }
  }

  if (m_clusters_per_lumi) m_clusters_per_lumi->Fill(m_manager->lumiBlockNumber(), nclusters);
  for (int i = 0; i < PixLayer::COUNT - 1 + (int)(m_doIBL); i++) {
    if (m_clusters_per_lumi_mod[i]) m_clusters_per_lumi_mod[i]->Fill(m_manager->lumiBlockNumber(), nclusters_mod[i]);
    if (m_doOnTrack && m_clusters_per_track_per_lumi_mod[i] && m_ntracksPerEvent > 0) {
      m_clusters_per_track_per_lumi_mod[i]->Fill(m_manager->lumiBlockNumber(), (1.0 * nclusters_mod[i]) / (1.0 * m_ntracksPerEvent));
    }
  }
  if (m_doOnTrack && m_clustersOnOffTrack_per_lumi && nclusters_all > 0) {
    m_clustersOnOffTrack_per_lumi->Fill(m_manager->lumiBlockNumber(), (float)nclusters_ontrack / nclusters_all);
  }

  // Fill some histograms only if =< 50% of modules disabled
  if (!m_majorityDisabled) {
    if (m_num_clusters) m_num_clusters->Fill(nclusters);

    for (int i = 0; i < PixLayer::COUNT - 1 + (int)(m_doIBL); i++) {
      if (m_num_clusters_mod[i]) m_num_clusters_mod[i]->Fill(nclusters_mod[i]);
    }
    if (m_doLumiBlock && m_num_clusters_LB) m_num_clusters_LB->Fill(nclusters);

    if (m_doModules && m_doOnTrack) {
      PixelID::const_id_iterator idIt = m_pixelid->wafer_begin();
      PixelID::const_id_iterator idItEnd = m_pixelid->wafer_end();
      for (; idIt != idItEnd; ++idIt) {
        Identifier WaferID = *idIt;
        if (m_pixelid->barrel_ec(WaferID) == 2) m_cluster_num_mod->fill(m_ClusPerEventArray_disksA[m_pixelid->phi_module(WaferID)][m_pixelid->layer_disk(WaferID)], WaferID, m_pixelid);
        if (m_pixelid->barrel_ec(WaferID) == -2) m_cluster_num_mod->fill(m_ClusPerEventArray_disksC[m_pixelid->phi_module(WaferID)][m_pixelid->layer_disk(WaferID)], WaferID, m_pixelid);
        if (m_pixelid->barrel_ec(WaferID) == 0) {
          if (m_doIBL && m_pixelid->layer_disk(WaferID) == 0) m_cluster_num_mod->fill(m_ClusPerEventArray_lI[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID) + 10], WaferID, m_pixelid);
          if (m_pixelid->layer_disk(WaferID) == 0 + m_doIBL) m_cluster_num_mod->fill(m_ClusPerEventArray_l0[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID) + 6], WaferID, m_pixelid);
          if (m_pixelid->layer_disk(WaferID) == 1 + m_doIBL) m_cluster_num_mod->fill(m_ClusPerEventArray_l1[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID) + 6], WaferID, m_pixelid);
          if (m_pixelid->layer_disk(WaferID) == 2 + m_doIBL) m_cluster_num_mod->fill(m_ClusPerEventArray_l2[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID) + 6], WaferID, m_pixelid);
        }
        if (m_doLumiBlock) {
          if (m_pixelid->barrel_ec(WaferID) == 2) m_cluster_num_mod_LB->fill(m_ClusPerEventArray_disksA[m_pixelid->phi_module(WaferID)][m_pixelid->layer_disk(WaferID)], WaferID, m_pixelid);
          if (m_pixelid->barrel_ec(WaferID) == -2) m_cluster_num_mod_LB->fill(m_ClusPerEventArray_disksC[m_pixelid->phi_module(WaferID)][m_pixelid->layer_disk(WaferID)], WaferID, m_pixelid);
          if (m_pixelid->barrel_ec(WaferID) == 0) {
            if (m_doIBL && m_pixelid->layer_disk(WaferID) == 0) m_cluster_num_mod_LB->fill(m_ClusPerEventArray_lI[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID) + 10], WaferID, m_pixelid);
            if (m_pixelid->layer_disk(WaferID) == 0 + m_doIBL) m_cluster_num_mod_LB->fill(m_ClusPerEventArray_l0[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID) + 6], WaferID, m_pixelid);
            if (m_pixelid->layer_disk(WaferID) == 1 + m_doIBL) m_cluster_num_mod_LB->fill(m_ClusPerEventArray_l1[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID) + 6], WaferID, m_pixelid);
            if (m_pixelid->layer_disk(WaferID) == 2 + m_doIBL) m_cluster_num_mod_LB->fill(m_ClusPerEventArray_l2[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID) + 6], WaferID, m_pixelid);
          }
        }
      }
    }
  }  // if (!m_majorityDisabled)

  if (nclusters == 0 && m_storegate_errors) m_storegate_errors->Fill(3., 4.);  //first entry for RDO, second for size = 0

  return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::procClustersMon(void) {
  if (m_doOnline && m_doOnTrack) {
    int lastlb = m_manager->lumiBlockNumber()-1; //remove -1 for testing
    float cont(0.0);
    if (m_zoomed_clusterToTcosA_lumi_IBL && m_zoomed_clusterToTcosA_lastXlb_IBL
	&& m_zoomed_clusterToTcosA_lumi_B0 && m_zoomed_clusterToTcosA_lastXlb_B0
	&& m_zoomed_clusterToTcosA_lumi_B1 && m_zoomed_clusterToTcosA_lastXlb_B1
	&& m_zoomed_clusterToTcosA_lumi_B2 && m_zoomed_clusterToTcosA_lastXlb_B2
	&& m_zoomed_clusterToTcosA_lumi_ECA && m_zoomed_clusterToTcosA_lastXlb_ECA
	&& m_zoomed_clusterToTcosA_lumi_ECC && m_zoomed_clusterToTcosA_lastXlb_ECC) {

      int bingx  = m_zoomed_clusterToTcosA_lumi_B2->GetXaxis()->FindBin(lastlb);
      int nbingy = m_zoomed_clusterToTcosA_lumi_B2->GetNbinsY();
      int nXbins = m_zoomed_clusterToTcosA_lastXlb_B2->GetNbinsX();

      m_zoomed_clusterToTcosA_lastXlb_IBL->GetXaxis()->Set(nXbins, lastlb-nXbins+0.5, lastlb+0.5);
      m_zoomed_clusterToTcosA_lastXlb_IBL->Reset();
      m_zoomed_clusterToTcosA_lastXlb_B0->GetXaxis()->Set(nXbins, lastlb-nXbins+0.5, lastlb+0.5);
      m_zoomed_clusterToTcosA_lastXlb_B0->Reset();
      m_zoomed_clusterToTcosA_lastXlb_B1->GetXaxis()->Set(nXbins, lastlb-nXbins+0.5, lastlb+0.5);
      m_zoomed_clusterToTcosA_lastXlb_B1->Reset();
      m_zoomed_clusterToTcosA_lastXlb_B2->GetXaxis()->Set(nXbins, lastlb-nXbins+0.5, lastlb+0.5);
      m_zoomed_clusterToTcosA_lastXlb_B2->Reset();
      m_zoomed_clusterToTcosA_lastXlb_ECA->GetXaxis()->Set(nXbins, lastlb-nXbins+0.5, lastlb+0.5);
      m_zoomed_clusterToTcosA_lastXlb_ECA->Reset();
      m_zoomed_clusterToTcosA_lastXlb_ECC->GetXaxis()->Set(nXbins, lastlb-nXbins+0.5, lastlb+0.5);
      m_zoomed_clusterToTcosA_lastXlb_ECC->Reset();

      for (int binfx=nXbins; binfx>0; binfx--) {
	if (bingx>0) {
	  for (int bingy = 1; bingy <= nbingy; bingy++) {
	    cont = m_zoomed_clusterToTcosA_lumi_IBL->GetBinContent(bingx, bingy);
	    if (cont!=0) m_zoomed_clusterToTcosA_lastXlb_IBL->SetBinContent(binfx, bingy, cont);
	    cont = m_zoomed_clusterToTcosA_lumi_B0->GetBinContent(bingx, bingy);
	    if (cont!=0) m_zoomed_clusterToTcosA_lastXlb_B0->SetBinContent(binfx, bingy, cont);
	    cont = m_zoomed_clusterToTcosA_lumi_B1->GetBinContent(bingx, bingy);
	    if (cont!=0) m_zoomed_clusterToTcosA_lastXlb_B1->SetBinContent(binfx, bingy, cont);
	    cont = m_zoomed_clusterToTcosA_lumi_B2->GetBinContent(bingx, bingy);
	    if (cont!=0) m_zoomed_clusterToTcosA_lastXlb_B2->SetBinContent(binfx, bingy, cont);
	    cont = m_zoomed_clusterToTcosA_lumi_ECA->GetBinContent(bingx, bingy);
	    if (cont!=0) m_zoomed_clusterToTcosA_lastXlb_ECA->SetBinContent(binfx, bingy, cont);
	    cont = m_zoomed_clusterToTcosA_lumi_ECC->GetBinContent(bingx, bingy);
	    if (cont!=0) m_zoomed_clusterToTcosA_lastXlb_ECC->SetBinContent(binfx, bingy, cont);
	  }
	  bingx--;
	} 
      }
      //m_zoomed_clusterToTcosA_lastXlb_B2->SetEntries(lastlb);      // for testing 
    }

  }
  if (m_doOffline) {
    fillSummaryHistos(m_cluster_occupancy.get(),
                      m_cluster_occupancy_summary_mod[PixLayer::kECA],
                      m_cluster_occupancy_summary_mod[PixLayer::kECC],
                      m_cluster_occupancy_summary_mod[PixLayer::kIBL],
                      m_cluster_occupancy_summary_mod[PixLayer::kB0],
                      m_cluster_occupancy_summary_mod[PixLayer::kB1],
                      m_cluster_occupancy_summary_mod[PixLayer::kB2]);
  }

  return StatusCode::SUCCESS;
}
