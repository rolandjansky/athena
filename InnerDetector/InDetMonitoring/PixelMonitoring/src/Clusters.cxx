/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// Book and fill histograms showing pixel cluster properties
///////////////////////////////////////////////////////////////////////////////

#include "PixelMonitoring/PixelMainMon.h"
#include "InDetRawData/InDetTimeCollection.h"
//#include "TrkSpacePoint/SpacePointContainer.h"
#include "InDetPrepRawData/PixelClusterContainer.h"         
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
//#include "TrkParameters/MeasuredAtaPlane.h"
//#include "TrkParameters/MeasuredPerigee.h"
//#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "PixelConditionsServices/IPixelByteStreamErrorsSvc.h"
#include "InDetIdentifier/PixelID.h"
#include "TH1F.h"   
#include "TH1I.h"   
#include "TH2F.h"
#include "TH2I.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH1I_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"
#include "TProfile.h"
#include "LWHists/TProfile_LW.h"
#include "LWHists/TProfile2D_LW.h"
#include <sstream>

//#include "InDetRawData/InDetRawDataContainer.h"
//#include "InDetRawData/InDetRawDataCLASS_DEF.h" 
//#include "TrkTrack/TrackCollection.h"            
//#include "InDetReadoutGeometry/SiDetectorElement.h"
//#include "TrkTrackSummary/TrackSummary.h"
#include "PixelMonitoring/PixelMon2DMaps.h"
#include "PixelMonitoring/DBMMon2DMaps.h"
#include "PixelMonitoring/PixelMonModules.h"
#include "PixelMonitoring/PixelMonProfiles.h"
#include "PixelMonitoring/PixelMon2DProfilesLW.h"

//#include "EventInfo/EventInfo.h"
//#include "EventInfo/TriggerInfo.h"
#include "PixelCabling/IPixelCablingSvc.h"


///////////////////////////////////////////////////////////////////////////////
//////////////////////booking methods//////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

StatusCode PixelMainMon::BookClustersMon(void)
{
   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "starting Book Clusters" << endmsg;  
   
   std::string path = "Pixel/Clusters";
   if(m_doOnTrack) path.replace(path.begin(), path.end(), "Pixel/ClustersOnTrack");
   if(m_doOnPixelTrack) path.replace(path.begin(), path.end(), "Pixel/ClustersOnPixelTrack");
   MonGroup clusterShift(   this, path.c_str(), run, ATTRIB_MANAGED ); //declare a group of histograms                         
   MonGroup clusterExpert ( this, path.c_str(), run, ATTRIB_MANAGED ); //declare a group of histograms                                      

   std::string pathT = "Pixel/Timing";
   if(m_doOnTrack) pathT.replace(pathT.begin(), pathT.end(), "Pixel/TimingOnTrack");
   if(m_doOnPixelTrack) pathT.replace(pathT.begin(), pathT.end(), "Pixel/TimingOnPixelTrack");
   MonGroup timeShift(  this, pathT.c_str(), run, ATTRIB_MANAGED ); //declare a group of histograms                                 
   MonGroup timeExpert( this, pathT.c_str(), run, ATTRIB_MANAGED ); //declare a group of histograms                                         

   std::string addOnTrack = "_OnTrack";

   int nbins_LB = m_lbRange;  double min_LB   = -0.5; double max_LB = min_LB + (1.0*nbins_LB);        
   int nbins_BCID = m_bcidRange; double min_BCID  = -0.5; double max_BCID = min_BCID + (1.0*nbins_BCID);        
   int nbins_npix = 300; double min_npix = -0.5; double max_npix = min_npix + (1.0*nbins_npix);
   int nbins_nwid = 50;  double min_nwid = -0.5; double max_nwid = min_nwid + (1.0*nbins_nwid);
   int nbins_eta = 13;   double min_eta = -6.5;  double max_eta = min_eta + (1.0*nbins_eta);
   int nbins_tot = 300;  double min_tot = -0.5; double max_tot = min_tot + (1.0*nbins_tot);
   int nbins_ibl_tot = 150;  double min_ibl_tot = -0.5; double max_ibl_tot = min_tot + (1.0*nbins_tot);
   int nbins_Q = 70;     double min_Q = -0.5; double max_Q = min_Q + (3000.0*nbins_Q);
   //int nbins_nevt = 50;  double min_nevt = -0.5; double max_nevt = min_nevt + (1.0*nbins_nevt);
   int nbins_lvl1 = 14;  double min_lvl1 = -1.5; double max_lvl1 = min_lvl1 + (1.0*nbins_lvl1);
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
   std::string modlabel[8];
   modlabel[0]="ECA"; modlabel[1]="ECC";
   modlabel[2]="B0";  modlabel[3]="B1";  modlabel[4]="B2"; modlabel[5]="IBL"; modlabel[6]="IBL2D"; modlabel[7]="IBL3D"; 

   std::string modlabel2[PixLayerIBL2D3DDBM::COUNT];
   modlabel2[0]="ECA0"; modlabel2[1]="ECA1"; modlabel2[2]="ECA2"; 
   modlabel2[3]="ECC0"; modlabel2[4]="ECC1"; modlabel2[5]="ECC2"; 
   modlabel2[6]="B0";  modlabel2[7]="B1";  modlabel2[8]="B2"; 
   modlabel2[9]="DBMA";modlabel2[10]="DBMC";
   modlabel2[11]="IBL"; modlabel2[12]="IBL2D"; modlabel2[13]="IBL3D"; 

   hname = makeHistname("ClusterSize_vs_eta", false);
   htitles = makeHisttitle("Average cluster size as a function of barrel module eta", (atext_eta+atext_cluw), false);
   sc = clusterExpert.regHist(m_clusterSize_eta = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_eta, min_eta, max_eta));

   hname = makeHistname("LargeClusters_per_lumi", true);
   htitles = makeHisttitle("Avetarge number of large clusters (with >10 pixels) per event", (atext_LB+atext_clu), true);
   sc = clusterShift.regHist(m_largeclusters_per_lumi = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

   hname = makeHistname("VeryLargeClusters_per_lumi", true);
   htitles = makeHisttitle("Average number of very large clusters (with >50 pixels) per event", (atext_LB+atext_clu), true);
   sc = clusterShift.regHist(m_verylargeclusters_per_lumi = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

   hname = makeHistname("HighNClusters_per_lumi", true);
   htitles = makeHisttitle("Number of events with >1000 clusters/event", (atext_LB+atext_nevt), true);
   sc = clusterExpert.regHist(m_highNclusters_per_lumi = TH1I_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

   hname = makeHistname("Clusters_per_lumi", true);
   htitles = makeHisttitle("Average number of pixel clusters per event", (atext_LB+atext_clu), true);
   sc = clusterShift.regHist(m_clusters_per_lumi = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

   hname = makeHistname("TotalClusters_per_lumi", true);
   htitles = makeHisttitle("Total number of pixel clusters", (atext_LB+atext_nclu), true);
   sc = clusterShift.regHist(m_totalclusters_per_lumi = TH1I_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

   hname = makeHistname("Cluster_groupsize", true);
   htitles = makeHisttitle("Number of pixels in a cluster", (atext_npix+atext_nclu), true);
   sc = clusterExpert.regHist(m_cluster_groupsize = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_npix, min_npix, max_npix));

   hname = makeHistname("Cluster_column_width", false);
   htitles = makeHisttitle("Column width of a cluster", (atext_cluw+atext_nclu), false);
   sc = clusterExpert.regHist(m_cluster_col_width = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_nwid, min_nwid, max_nwid)); 

   hname = makeHistname("Cluster_row_width", false);
   htitles = makeHisttitle("Row width of a cluster", (atext_cluw+atext_nclu), false);
   sc = clusterExpert.regHist(m_cluster_row_width = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_nwid, min_nwid, max_nwid));

   hname = makeHistname("Cluster_LVL1A", true);
   htitles = makeHisttitle("Cluster Level 1 Accept", (atext_lvl1+atext_nclu), true);
   sc = timeShift.regHist(m_cluster_LVL1A = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_lvl1, min_lvl1, max_lvl1));

   /// Histograms for each layer
   for(int i=0; i<PixLayer::COUNT-1+(int)(m_doIBL); i++){
      hname = makeHistname(("Clusters_per_lumi_"+modlabel[i]), false);
      htitles = makeHisttitle(("Average number of pixel clusters per event per LB, "+modlabel[i]), (atext_LB+atext_clu), false);
      sc = clusterExpert.regHist(m_clusters_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

      //hname = makeHistname(("TotalClusters_per_lumi_"+modlabel[i]), false);
      //htitles = makeHisttitle(("Total number of pixel clusters per LB, "+modlabel[i]), (atext_LB+atext_nclu), false);
      //sc = clusterExpert.regHist(m_totalclusters_per_lumi_mod[i] = TH1I_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

      //hname = makeHistname(("TotalClusters_per_bcid_"+modlabel[i]), false);
      //htitles = makeHisttitle(("Total number of pixel clusters per BCID, "+modlabel[i]), (atext_LB+atext_nclu), false);
      //sc = clusterExpert.regHist(m_totalclusters_per_bcid_mod[i] = TH1I_LW::create(hname.c_str(), htitles.c_str(), nbins_BCID, min_BCID, max_BCID));

      //hname = makeHistname(("Cluster_groupsize_"+modlabel[i]), false);
      //htitles = makeHisttitle(("Number of pixels in a cluster, "+modlabel[i]), (atext_npix+atext_nclu), false);
      //sc = clusterExpert.regHist(m_cluster_groupsize_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_npix+1, min_npix, max_npix+1));

      //hname = makeHistname(("Cluster_column_width_"+modlabel[i]), false);
      //htitles = makeHisttitle(("Column width of a cluster, "+modlabel[i]), (atext_cluw+atext_nclu), false);
      //sc = clusterExpert.regHist(m_cluster_col_width_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_nwid, min_nwid, max_nwid));

      //hname = makeHistname(("Cluster_row_width_"+modlabel[i]), false);
      //htitles = makeHisttitle(("Row width of a cluster, "+modlabel[i]), (atext_cluw+atext_nclu), false);
      //sc = clusterExpert.regHist(m_cluster_row_width_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_nwid, min_nwid, max_nwid));

      //hname = makeHistname(("Clusters_row_width_per_lumi_"+modlabel[i]), true);
      //htitles = makeHisttitle("Average row width per LB", (atext_LB+atext_clu), true);
      //sc = clusterShift.regHist(m_clusters_row_width_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

      //hname = makeHistname(("Clusters_column_width_per_lumi_"+modlabel[i]), true);
      //htitles = makeHisttitle("Average column width per LB", (atext_LB+atext_clu), true);
      //sc = clusterShift.regHist(m_clusters_col_width_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

      //hname = makeHistname(("Clusters_row_width_per_bcid_"+modlabel[i]), true);
      //htitles = makeHisttitle("Average row width per BCID", (atext_BCID+atext_clu), true);
      //sc = clusterShift.regHist(m_clusters_row_width_per_bcid_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_BCID, min_BCID, max_BCID));

      //hname = makeHistname(("Clusters_column_width_per_bcid_"+modlabel[i]), true);
      //htitles = makeHisttitle("Average column width per BCID", (atext_BCID+atext_clu), true);
      //sc = clusterShift.regHist(m_clusters_col_width_per_bcid_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_BCID, min_BCID, max_BCID));

      //hname = makeHistname(("Cluster_ToT_"+modlabel[i]), false);
      //htitles = makeHisttitle(("Cluster ToT, "+modlabel[i]), (atext_tot+atext_nclu), false);
      //sc = clusterExpert.regHist(m_cluster_ToT1d_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_tot, min_tot, max_tot));

      hname = makeHistname(("1Hit_Cluster_ToT_"+modlabel[i]), false);
      htitles = makeHisttitle(("ToT for 1-hit clusters, "+modlabel[i]), (atext_tot+atext_nclu), false);
      sc = clusterExpert.regHist(m_1cluster_ToT_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_tot, min_tot, max_tot));

      hname = makeHistname(("2Hit_Cluster_ToT_"+modlabel[i]), false);
      htitles = makeHisttitle(("ToT for 2-hit clusters, "+modlabel[i]), (atext_tot+atext_nclu), false);
      sc = clusterExpert.regHist(m_2cluster_ToT_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_tot, min_tot, max_tot));

      hname = makeHistname(("3Hit_Cluster_ToT_"+modlabel[i]), false);
      htitles = makeHisttitle(("ToT for 3-hit clusters, "+modlabel[i]), (atext_tot+atext_nclu), false);
      sc = clusterExpert.regHist(m_3cluster_ToT_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_tot, min_tot, max_tot));

      hname = makeHistname(("BigHit_Cluster_ToT_"+modlabel[i]), false);
      htitles = makeHisttitle(("ToT for cluster size > 3, "+modlabel[i]), (atext_tot+atext_nclu), false);
      sc = clusterExpert.regHist(m_bigcluster_ToT_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_tot, min_tot, max_tot));

      //hname = makeHistname(("Cluster_Q_"+modlabel[i]), false);
      //htitles = makeHisttitle(("Charge, "+modlabel[i]), (atext_Q+atext_nclu), false);
      //sc = clusterExpert.regHist(m_cluster_Q_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_Q, min_Q, max_Q));

      hname = makeHistname(("1Hit_Cluster_Q_"+modlabel[i]), false);
      htitles = makeHisttitle(("Charge for 1-hit clusters, "+modlabel[i]), (atext_Q+atext_nclu), false);
      sc = clusterExpert.regHist(m_1cluster_Q_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_Q, min_Q, max_Q));

      hname = makeHistname(("2Hit_Cluster_Q_"+modlabel[i]), false);
      htitles = makeHisttitle(("Charge for 2-hit clusters, "+modlabel[i]), (atext_Q+atext_nclu), false);
      sc = clusterExpert.regHist(m_2cluster_Q_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_Q, min_Q, max_Q));

      hname = makeHistname(("3Hit_Cluster_Q_"+modlabel[i]), false);
      htitles = makeHisttitle(("Charge for 3-hit clusters, "+modlabel[i]), (atext_Q+atext_nclu), false);
      sc = clusterExpert.regHist(m_3cluster_Q_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_Q, min_Q, max_Q));

      hname = makeHistname(("BigHit_Cluster_Q_"+modlabel[i]), false);
      htitles = makeHisttitle(("Charge for cluster size > 3, "+modlabel[i]), (atext_Q+atext_nclu), false);
      sc = clusterExpert.regHist(m_bigcluster_Q_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_Q, min_Q, max_Q));

      hname = makeHistname(("Cluster_LVL1A_"+modlabel[i]), true);
      if(i != PixLayer::kIBL) htitles = makeHisttitle(("Cluster Level 1 Accept with ToT > 15, "+modlabel[i]), (atext_lvl1+atext_nclu), false);
      else htitles = makeHisttitle(("Cluster Level 1 Accept with ToT > 4, "+modlabel[i]), (atext_lvl1+atext_nclu), false);
      sc = timeExpert.regHist(m_cluster_LVL1A1d_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_lvl1, min_lvl1, max_lvl1));

      //hname = makeHistname(("ClusterSize_OnTrack_"+modlabel[i]), false);
      //htitles = makeHisttitle(("cluster size  for clusters on tracks, "+modlabel[i]), ";cluster size;#clusters", false);
      //sc = clusterExpert.regHist(m_clusize_ontrack_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), 300, -0.5, -0.5+300.0));

      //hname = makeHistname(("ClusterSize_OffTrack_"+modlabel[i]), false);
      //htitles = makeHisttitle(("cluster size  for clusters out of tracks , "+modlabel[i]), ";cluster size;#clusters", false);
      //sc = clusterExpert.regHist(m_clusize_offtrack_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), 300, -0.5, -0.5+300.0));

      hname = makeHistname(("Cluster_groupsize_vs_eta_"+modlabel[i]), false);
      htitles = makeHisttitle(("Number of pixels per cluster vs eta, "+modlabel[i]), (atext_eta+atext_npix), false);
      sc = clusterExpert.regHist(m_clussize_vs_eta_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_eta, min_eta, max_eta));

      if(m_doESD && !m_doOnline){
         //hname = makeHistname(("ClusterQ_vs_eta_"+modlabel[i]), false);
         //htitles = makeHisttitle(("Cluster charge vs eta, "+modlabel[i]), (atext_eta+atext_Q), false);
         //sc = clusterExpert.regHist(m_clusQ_vs_eta_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_eta, min_eta, max_eta ));

         //hname = makeHistname(("Cluster_groupsize_vs_eta_"+modlabel[i]), false);
         //htitles = makeHisttitle(("Number of pixels per cluster vs eta, "+modlabel[i]), (atext_eta+atext_npix), false);
         //sc = clusterExpert.regHist(m_clussize_vs_eta_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_eta, min_eta, max_eta));

         hname = makeHistname(("ClusterToT_vs_eta_"+modlabel[i]), false);
         htitles = makeHisttitle(("Cluster ToT vs eta, "+modlabel[i]), (atext_eta+atext_tot), false);
         sc = clusterExpert.regHist(m_clusToT_vs_eta_mod[i] = TH2F_LW::create(hname.c_str(), htitles.c_str(), nbins_eta, min_eta, max_eta, nbins_tot, min_tot, max_tot));

         hname = makeHistname(("ClusterToT_vs_groupsize_"+modlabel[i]), false);
         htitles = makeHisttitle(("Cluster ToT vs groupsize, "+modlabel[i]), (atext_tot+atext_npix), false);
         sc= clusterExpert.regHist(m_ToT_vs_clussize_mod[i] = TH2F_LW::create(hname.c_str(), htitles.c_str(), nbins_tot, min_tot, max_tot, nbins_npix, min_npix, max_npix));

         hname = makeHistname(("ClusterQ_vs_groupsize_"+modlabel[i]), false);
         htitles = makeHisttitle(("Cluster Charge vs groupsize, "+modlabel[i]), (atext_tot+atext_npix), false);
         //sc= clusterExpert.regHist(m_Q_vs_clussize_mod[i] = TH2F_LW::create(hname.c_str(), htitles.c_str(), nbins_Q, min_Q, max_Q, nbins_npix, min_npix, max_npix));

      }
   }
   for(int i=0; i<PixLayerIBL2D3DDBM::COUNT; i++){
      hname = makeHistname(("Cluster_ToT_"+modlabel2[i]), false);
      htitles = makeHisttitle(("Cluster ToT, "+modlabel2[i]), (atext_tot+atext_nclu), false);
      if( i<PixLayer::kIBL ){
         sc = clusterExpert.regHist(m_cluster_ToT1d_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_tot, min_tot, max_tot));
      }else if(m_doIBL){
         sc = clusterExpert.regHist(m_cluster_ToT1d_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_ibl_tot, min_ibl_tot, max_ibl_tot));
      }

      hname = makeHistname(("Cluster_Q_"+modlabel2[i]), false);
      htitles = makeHisttitle(("Charge, "+modlabel2[i]), (atext_Q+atext_nclu), false);
      sc = clusterExpert.regHist(m_cluster_Q_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_Q, min_Q, max_Q));
   }
   for(int i=0; i<PixLayerIBL2D3D::COUNT; i++){
      hname = makeHistname(("Cluster_groupsize_"+modlabel[i]), false);
      htitles = makeHisttitle(("Number of pixels in a cluster, "+modlabel[i]), (atext_npix+atext_nclu), false);
      sc = clusterExpert.regHist(m_cluster_groupsize_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_npix+1, min_npix, max_npix+1));

      hname = makeHistname(("Cluster_column_width_"+modlabel[i]), false);
      htitles = makeHisttitle(("Column width of a cluster, "+modlabel[i]), (atext_cluw+atext_nclu), false);
      sc = clusterExpert.regHist(m_cluster_col_width_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_nwid, min_nwid, max_nwid));

      hname = makeHistname(("Cluster_row_width_"+modlabel[i]), false);
      htitles = makeHisttitle(("Row width of a cluster, "+modlabel[i]), (atext_cluw+atext_nclu), false);
      sc = clusterExpert.regHist(m_cluster_row_width_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_nwid, min_nwid, max_nwid));

      hname = makeHistname(("TotalClusters_per_lumi_"+modlabel[i]), false);
      htitles = makeHisttitle(("Total number of pixel clusters per LB, "+modlabel[i]), (atext_LB+atext_nclu), false);
      sc = clusterExpert.regHist(m_totalclusters_per_lumi_mod[i] = TH1I_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

      hname = makeHistname(("TotalClusters_per_bcid_"+modlabel[i]), false);
      htitles = makeHisttitle(("Total number of pixel clusters per BCID, "+modlabel[i]), (atext_LB+atext_nclu), false);

      hname = makeHistname(("Clusters_row_width_per_lumi_"+modlabel[i]), true);
      htitles = makeHisttitle("Average row width per LB", (atext_LB+atext_clu), true);
      sc = clusterShift.regHist(m_clusters_row_width_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

      hname = makeHistname(("Clusters_column_width_per_lumi_"+modlabel[i]), true);
      htitles = makeHisttitle("Average column width per LB", (atext_LB+atext_clu), true);
      sc = clusterShift.regHist(m_clusters_col_width_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

      hname = makeHistname(("Clusters_row_width_per_bcid_"+modlabel[i]), true);
      htitles = makeHisttitle("Average row width per BCID", (atext_BCID+atext_clu), true);
      sc = clusterShift.regHist(m_clusters_row_width_per_bcid_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_BCID, min_BCID, max_BCID));

      hname = makeHistname(("Clusters_column_width_per_bcid_"+modlabel[i]), true);
      htitles = makeHisttitle("Average column width per BCID", (atext_BCID+atext_clu), true);
      sc = clusterShift.regHist(m_clusters_col_width_per_bcid_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_BCID, min_BCID, max_BCID));

      hname = makeHistname(("ClusterSize_OnTrack_"+modlabel[i]), false);
      htitles = makeHisttitle(("cluster size  for clusters on tracks, "+modlabel[i]), ";cluster size;#clusters", false);
      sc = clusterExpert.regHist(m_clusize_ontrack_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), 300, -0.5, -0.5+300.0));

      hname = makeHistname(("ClusterSize_OffTrack_"+modlabel[i]), false);
      htitles = makeHisttitle(("cluster size  for clusters out of tracks , "+modlabel[i]), ";cluster size;#clusters", false);
      sc = clusterExpert.regHist(m_clusize_offtrack_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), 300, -0.5, -0.5+300.0));

   }

   
   if (m_doOnTrack || m_doOnPixelTrack) { 
     sc = clusterExpert.regHist(m_clustersOnOffTrack_per_lumi = TProfile_LW::create("ClustersOnOffTrack_per_lumi",("Fraction pixel clusters on track per event per LB" + m_histTitleExt + ";lumi block; fraction clusters/event").c_str(),2500,-0.5,2499.5));
   }

   
   if(m_do2DMaps)
   {
      tmp = "Cluster_Occupancy"; tmp2 = "Cluster occupancy";
      m_cluster_occupancy = new PixelMon2DMaps(tmp.c_str(), (tmp2 + m_histTitleExt).c_str());
      sc = m_cluster_occupancy->regHist(clusterShift);

      tmp = "Cluster_LVL1A_Mod"; tmp2 = "Average cluster Level 1 Accept";
      m_cluster_LVL1A_mod = new PixelMonProfiles(tmp.c_str(), (tmp2 + m_histTitleExt).c_str());
      sc = m_cluster_LVL1A_mod->regHist(timeShift);

      tmp = "Clus_Occ_SizeCut"; tmp2 = "Size>1 Cluster occupancy";
      m_clusocc_sizenot1 = new PixelMon2DMaps(tmp.c_str(), (tmp2 + m_histTitleExt).c_str()); 
      sc = m_clusocc_sizenot1->regHist(clusterShift); 

      tmp = "Clus_LVL1A_SizeCut"; tmp2 = "Average Size>1 Cluster Level 1 Accept";
      m_clus_LVL1A_sizenot1 = new PixelMonProfiles(tmp.c_str(), (tmp2 + m_histTitleExt).c_str());
      sc = m_clus_LVL1A_sizenot1->regHist(timeShift); 

      if(m_doOnline){
        tmp = "ClusterMap_Mon"; tmp2 = "Cluster map for monitoring";
        m_clustermap_mon = new PixelMon2DMaps(tmp.c_str(), (tmp2 + m_histTitleExt).c_str());
        sc = m_clustermap_mon->regHist(clusterShift);

        tmp = "ClusterMap_tmp"; tmp2 = "Cluster map for monitoring";
        m_clustermap_tmp = new PixelMon2DMaps(tmp.c_str(), (tmp2 + m_histTitleExt).c_str());
        sc = m_clustermap_tmp->regHist(clusterShift);
      }
      if(!m_doOnline){
         tmp = "Cluster_Size_Map"; tmp2 = "Average cluster size map";
         //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
	      m_clussize_map = new PixelMonProfiles(tmp.c_str(), (tmp2 + m_histTitleExt).c_str());
	      sc = m_clussize_map->regHist(clusterExpert);

         tmp = "Cluster_Charge_Map"; tmp2 = "Average cluster charge map";
         //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
	      m_cluscharge_map = new PixelMonProfiles(tmp.c_str(), (tmp2 + m_histTitleExt).c_str());
	      sc = m_cluscharge_map->regHist(clusterExpert);

         tmp = "Cluster_ToT_Map"; tmp2 = "Average cluster ToT map";
         //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
	      m_clusToT_map = new PixelMonProfiles(tmp.c_str(), (tmp2 + m_histTitleExt).c_str());
	      sc = m_clusToT_map->regHist(clusterExpert);

         //hname = makeHistname("Cluster_lowToTHitsFraction7_Map", true);
         //tmp2 = "Average fraction of low tot (<7) hits in one cluster";
	      //m_lowToTHitsFraction_7 = new PixelMon2DProfilesLW(hname.c_str(), (tmp2 + m_histTitleExt).c_str(), m_doIBL, false);
	      //sc = m_lowToTHitsFraction_7->regHist(clusterExpert, m_doIBL, false);

         hname = makeHistname("Cluster_lowToTHitsFraction9_Map", true);
         tmp2 = "Average fraction of low tot (<9) hits in one cluster";
	      m_lowToTHitsFraction_9 = new PixelMon2DProfilesLW(hname.c_str(), (tmp2 + m_histTitleExt).c_str(), m_doIBL, false);
	      sc = m_lowToTHitsFraction_9->regHist(clusterExpert, m_doIBL, false);

         hname = makeHistname("Cluster_lowToTHitsFraction11_Map", true);
         tmp2 = "Average fraction of low tot (<11) hits in one cluster";
	      m_lowToTHitsFraction_11= new PixelMon2DProfilesLW(hname.c_str(), (tmp2 + m_histTitleExt).c_str(), m_doIBL, false);
	      sc = m_lowToTHitsFraction_11->regHist(clusterExpert, m_doIBL, false);

         //hname = makeHistname("Cluster_lowToTHitsFraction13_Map", true);
         //tmp2 = "Average fraction of low tot (<13) hits in one cluster";
	      //m_lowToTHitsFraction_13 = new PixelMon2DProfilesLW(hname.c_str(), (tmp2 + m_histTitleExt).c_str(), m_doIBL, false);
	      //sc = m_lowToTHitsFraction_13->regHist(clusterExpert, m_doIBL, false);

         hname = makeHistname("Cluster_Occupancy_FEMap_mon", true);
         tmp2 = "Cluster Occ.";
         sc = clusterExpert.regHist(m_cluster_occupancy_FE_B0_mon = TH2F_LW::create((hname+"_B0").c_str(), (tmp2 + ", B0 " + ";eta index of FE;phi index of FE").c_str(), 13*8, -48.5, -48.5+(13*8), 22*2, -0.5, -0.5+(22*2)));

         ///
         /// Study for Quick Status
         ///
         //hname = makeHistname("Cluster_Occupamcy_L0_B11_S2_C6", true);
         //tmp2 = "Cluster Occupancy., L0_B11_S2_C6;lumi block;FE ID (8*(pix_phi/164) + (eta_pix/18);# clusters";
         //sc = clusterExpert.regHist(m_cluster_occupancy_FE_L0_B11_S2_C6 = TH2F_LW::create((hname+"_L0_B11_S2_C6").c_str(), tmp2.c_str(), nbins_LB, min_LB, max_LB, 16, -0.5, -0.5+16));
         //m_cluster_occupancy_FE_L0_B11_S2_C6->SetOption("colz");

         //hname = makeHistname("Cluster_Occupamcy_isGood_L0_B11_S2_C6", true);
         //tmp2 = "Cluster Occupancy., L0_B11_S2_C6(isGood);lumi block;FE ID (8*(pix_phi/164) + (eta_pix/18);# clusters";
         //sc = clusterExpert.regHist(m_cluster_occupancy_isgood_FE_L0_B11_S2_C6 = TH2F_LW::create((hname+"_L0_B11_S2_C6").c_str(), tmp2.c_str(), nbins_LB, min_LB, max_LB, 16, -0.5, -0.5+16));
         //m_cluster_occupancy_isgood_FE_L0_B11_S2_C6->SetOption("colz");

         //hname = makeHistname("Cluster_ToTMean_L0_B11_S2_C6", true);
         //tmp2 = "Cluster ToT Mean, L0_B11_S2_C6;lumi block;FE ID (8*(pix_phi/164) + (eta_pix/18);Mean Cluster ToT";
         //sc = clusterExpert.regHist(m_cluster_totmean_L0_B11_S2_C6 = TProfile2D_LW::create((hname+"_L0_B11_S2_C6").c_str(), tmp2.c_str(), nbins_LB, min_LB, max_LB, 16, -0.5, -0.5+16));
         //m_cluster_totmean_L0_B11_S2_C6->SetOption("colz");

         ///
         /// For FE map (temporary)
         ///
         const int nmod = 13;
         const char *mod[nmod] = { "M6C", "M5C", "M4C", "M3C", "M2C", "M1C", "M0","M1A", "M2A", "M3A", "M4A", "M5A", "M6A" } ;
         const int nstave0 = 22;
         const char *stave0[nstave0] = {"B11_S2", "B01_S1", "B01_S2", "B02_S1", "B02_S2",
                                        "B03_S1", "B03_S2", "B04_S1", "B04_S2", "B05_S1",
                                        "B05_S2", "B06_S1", "B06_S2", "B07_S1", "B07_S2",
                                        "B08_S1", "B08_S2", "B09_S1", "B09_S2", "B10_S1", "B10_S2", "B11_S1"
                                        };
         for(unsigned int x=0 ; x < m_cluster_occupancy_FE_B0_mon->GetNbinsX() ; x++){
            //if( x%8 == 0 ) m_cluster_occupancy_FE_B0->GetXaxis()->SetBinLabel( x+1, mod[x/8] );
            if( x%8 == 0 ) m_cluster_occupancy_FE_B0_mon->GetXaxis()->SetBinLabel( x+1, mod[x/8] );
         }
         for(unsigned int y=0 ; y < m_cluster_occupancy_FE_B0_mon->GetNbinsY() ; y++){
            //if( y%2 == 0 ) m_cluster_occupancy_FE_B0->GetYaxis()->SetBinLabel( y+1, stave0[y/2] );
            if( y%2 == 0 ) m_cluster_occupancy_FE_B0_mon->GetYaxis()->SetBinLabel( y+1, stave0[y/2] );
         }
         //m_cluster_occupancy_FE_B0->GetXaxis()->LabelsOption("v");
         //m_cluster_occupancy_FE_B0_mon->GetXaxis()->LabelsOption("v");
         //m_cluster_occupancy_FE_B0->GetYaxis()->SetLabelSize(0.03);
         m_cluster_occupancy_FE_B0_mon->GetYaxis()->SetLabelSize(0.03);
         //m_cluster_occupancy_FE_B0->SetOption("colz");
         m_cluster_occupancy_FE_B0_mon->SetOption("colz");
      }
   }

   if(m_doModules)
   {
      m_cluseff_mod = new PixelMonModulesProf("Clus_track_eff", ("Proportion of clusters on track vs t in module" + m_histTitleExt).c_str(), 2500,-0.5,2499.5,m_doIBL);
      sc = m_cluseff_mod->regHist(this,(path+"/Modules_Cluseff").c_str(),run, m_doIBL);

      m_cluster_size_mod = new PixelMonModules1D("Cluster_size", ("Cluster size in Module" + m_histTitleExt).c_str(), 20,-0.5,19.5,m_doIBL);
      sc = m_cluster_size_mod->regHist(this,(path+"/Modules_ClusterSize").c_str(),run,m_doIBL);
      
      m_cluster_num_mod = new PixelMonModules1D("Cluster_num", ("Number of clusters per event in module" + m_histTitleExt).c_str(), 30,-0.5,29.5,m_doIBL);
      sc = m_cluster_num_mod->regHist(this,(path+"/Modules_NumberOfClusters").c_str(),run,m_doIBL);
      
      //m_cluster_num_mod_ontrack = new PixelMonModules1D("Cluster_num_ontrack", ("Number of clusters on track per track per event in module" + m_histTitleExt).c_str(), 30,-0.5,29.5,m_doIBL);
      //sc = m_cluster_num_mod->regHist(this,(path+"/Modules_NumberOfClusters").c_str(),run,m_doIBL);
      
      m_cluster_ToT_mod = new PixelMonModules1D("Cluster_ToT", ("Cluster ToT in Module" + m_histTitleExt).c_str(), 200,0.,200.,m_doIBL);
      sc = m_cluster_ToT_mod->regHist(this,(path+"/Modules_ClusToT").c_str(),run,m_doIBL);
   }
   if(m_doOffline)
   {
   }
   if(m_doOnline)
   {
     sc = clusterShift.regHist(m_cluster_occupancy_time1= new TProfile("cluster_occupancy_time_10min", ("Module hit occupancy as function of time over 10 minutes. 6 sec/bin" + m_histTitleExt + ";time;module occupancy").c_str(), 99,0.,1.,"i"));
     sc = clusterShift.regHist(m_cluster_occupancy_time2= new TProfile("cluster_occupancy_time_1hr",   ("Module hit occupancy as function of time over 1 hour.  36 sec/bin" + m_histTitleExt + ";time;module occupancy").c_str(), 99,0.,1.,"i"));
     sc = clusterShift.regHist(m_cluster_occupancy_time3= new TProfile("cluster_occupancy_time_6hr",   ("Module hit occupancy as function of time over 6 hours.  3.6 min/bin" + m_histTitleExt + ";time;module occupancy").c_str(), 99,0.,1.,"i"));
   }
   if(m_doHighOccupancy)
   {
     int max_clusters = 40000;
     int max_clusters_region = 8000;
     int max_avclusters = 50;
     if (m_doHeavyIonMon) { max_clusters = 100000; max_clusters_region = 40000; max_avclusters = 1000; }
     tmp = "num_clusters"; tmp2 = "Number of pixel clusters in an event";
     if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
     tmp = "num_clusters"; tmp2 = "Number of pixel clusters in an event";
     sc = clusterShift.regHist(m_num_clusters      = TH1I_LW::create(tmp.c_str(),            (tmp2 + ";# pixel clusters/event" + m_histTitleExt + ";# events").c_str(), 1000,0.,(int)max_clusters));
     
     for(int i=0; i<PixLayer::COUNT-1+(int)(m_doIBL); i++){
       hname = makeHistname(("num_clusters_"+modlabel[i]), false);
       tmp2 = "Number of pixel clusters in an event, " + modlabel[i] + m_histTitleExt + ";# pixel clusters/event;# events";
       sc = clusterExpert.regHist(m_num_clusters_mod[i] = TH1I_LW::create(hname.c_str(), tmp2.c_str(), 160, 0., (int)max_clusters_region));

       if(m_doOnTrack || m_doOnPixelTrack) { 
          hname = makeHistname(("num_clusters_per_track_per_lumi_"+modlabel[i]), true);
          tmp2 = "Number of pixel clusters in an event, " + modlabel[i] + m_histTitleExt + atext_LB + ";Avg #clusters/track/event";
          sc = clusterExpert.regHist(m_clusters_per_track_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), tmp2.c_str(), nbins_LB, min_LB, max_LB));
       }

       hname = makeHistname(("cluster_occupancy_summary_" + modlabel[i]), true);
       tmp2 = "Average cluster occupancy, " + modlabel[i] + m_histTitleExt + ";average # of clusters per module per event;# modules";
       sc = clusterExpert.regHist(m_cluster_occupancy_summary_mod[i]= TH1F_LW::create(hname.c_str(), tmp2.c_str(), 100, 0., max_avclusters));
     }

     //tmp = "cluster_occupancy_summary"; tmp2 = "Average cluster occupancy";
     //sc = clusterExpert.regHist(m_ecA_cluster_occupancy_summary     = TH1F_LW::create((tmp + "_ECA").c_str(), (tmp2 + ", ECA" + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 100,0.,(int)max_avclusters));
     //sc = clusterExpert.regHist(m_ecC_cluster_occupancy_summary     = TH1F_LW::create((tmp + "_ECC").c_str(), (tmp2 + ", ECC" + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 100,0.,(int)max_avclusters));
     //sc = clusterExpert.regHist(m_bar_layI_cluster_occupancy_summary= TH1F_LW::create((tmp + "_IBL").c_str(), (tmp2 + ", IBL" + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 100,0.,(int)max_avclusters));
     //sc = clusterExpert.regHist(m_bar_lay0_cluster_occupancy_summary= TH1F_LW::create((tmp + "_B0").c_str(),  (tmp2 + ", B0"  + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 100,0.,(int)max_avclusters));
     //sc = clusterExpert.regHist(m_bar_lay1_cluster_occupancy_summary= TH1F_LW::create((tmp + "_B1").c_str(),  (tmp2 + ", B1"  + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 100,0.,(int)max_avclusters));
     //sc = clusterExpert.regHist(m_bar_lay2_cluster_occupancy_summary= TH1F_LW::create((tmp + "_B2").c_str(),  (tmp2 + ", B2"  + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 100,0.,(int)max_avclusters));
   }
   if(m_doLowOccupancy)
   {
      tmp = "num_clusters_low_occupancy"; tmp2 = "Number of pixel clusters in an event";
      if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
      sc = clusterShift.regHist(m_num_clusters_low                        = TH1I_LW::create(tmp.c_str(),  (tmp2 + m_histTitleExt + ";# pixel clusters/event;# events").c_str(), 200,-0.5,199.5));
      //tmp = "cluster_occupancy_summary_low_occupancy"; tmp2 = "Average cluster occupancy";
      ////if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
      //sc = clusterExpert.regHist(m_ecA_cluster_occupancy_summary_low      = TH1F_LW::create((tmp + "_ECA").c_str(), (tmp2 + ", ECA" + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 50,0.,2.));
      //sc = clusterExpert.regHist(m_ecC_cluster_occupancy_summary_low      = TH1F_LW::create((tmp + "_ECC").c_str(), (tmp2 + ", ECC" + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 50,0.,2.));
      //sc = clusterExpert.regHist(m_bar_layI_cluster_occupancy_summary_low = TH1F_LW::create((tmp + "_IBL").c_str(), (tmp2 + ", IBL" + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 50,0.,2.));
      //sc = clusterExpert.regHist(m_bar_lay0_cluster_occupancy_summary_low = TH1F_LW::create((tmp + "_B0").c_str(),  (tmp2 + ", B0"  + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 50,0.,2.));
      //sc = clusterExpert.regHist(m_bar_lay1_cluster_occupancy_summary_low = TH1F_LW::create((tmp + "_B1").c_str(),  (tmp2 + ", B1"  + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 50,0.,2.));
      //sc = clusterExpert.regHist(m_bar_lay2_cluster_occupancy_summary_low = TH1F_LW::create((tmp + "_B2").c_str(),  (tmp2 + ", B2"  + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 50,0.,2.));

      for(int i=0; i<PixLayer::COUNT-1+(int)(m_doIBL); i++){
        hname = makeHistname(("cluster_occupancy_summary_low_occupancy_" + modlabel[i]), true);
        tmp2 = "Average cluster occupancy, " + modlabel[i] + m_histTitleExt + ";average # of clusters per module per event;# modules";
        sc = clusterExpert.regHist(m_cluster_occupancy_summary_low_mod[i]= TH1F_LW::create(hname.c_str(), tmp2.c_str(), 50, 0., 2.));
      }
   }

   /// Quick Status
   if(m_doOnTrack || m_doOnPixelTrack) { 
     hname = makeHistname("Clusters_onTrack_per_lumi_L0_B11_S2_C6", false);
     htitles = makeHisttitle("Number of clusters on track, L0_B11_S2_C6", ";lumi block;FE ID (16*(6-eta_mod) + 8*(pix_phi/164) + (eta_pix/18);#hits", false);
     sc = clusterExpert.regHist(m_clusters_onTrack_L0_B11_S2_C6 = TH2F_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB, 96, -0.5, -0.5+96));
     m_clusters_onTrack_L0_B11_S2_C6->SetOption("colz");

     hname = makeHistname("Clusters_offTrack_per_lumi_L0_B11_S2_C6", false);
     htitles = makeHisttitle("Number of clusters not on track, L0_B11_S2_C6", ";lumi block;FE ID (16*(6-eta_mod) + 8*(pix_phi/164) + (eta_pix/18);#hits", false);
     sc = clusterExpert.regHist(m_clusters_offTrack_L0_B11_S2_C6 = TH2F_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB, 96, -0.5, -0.5+96));
     m_clusters_offTrack_L0_B11_S2_C6->SetOption("colz");
   }


   if(sc.isFailure())if(msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "histograms not booked" << endmsg;   
   return StatusCode::SUCCESS;
}


StatusCode PixelMainMon::BookClustersLumiBlockMon(void)
{
   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "starting Book Clusters for lowStat" << endmsg;  
   
   std::string path = "Pixel/LumiBlock";
   if(m_doOnTrack) path.replace(path.begin(), path.end(), "Pixel/LumiBlockOnTrack");
   if(m_doOnPixelTrack) path.replace(path.begin(), path.end(), "Pixel/LumiBlockOnPixelTrack");
   MonGroup lumiBlockHist(   this, path.c_str(), lowStat, ATTRIB_MANAGED); //declare a group of histograms                                  

   sc = lumiBlockHist.regHist(m_cluster_ToT_LB  = TH1F_LW::create("Cluster_ToT_LB", ("Cluster Time over Threshold" + m_histTitleExt + ";ToT;# clusters").c_str(), 300,-0.5,299.5));   

   if(m_do2DMaps) {
     m_cluster_occupancy_LB = new PixelMon2DMaps("Cluster_Occupancy_LB", ("Cluster Occupancy" + m_histTitleExt).c_str());
     sc = m_cluster_occupancy_LB->regHist(lumiBlockHist);
   }
   if(m_doHighOccupancy) {
     sc = lumiBlockHist.regHist(m_num_clusters_LB = TH1I_LW::create("num_clusters_LB", ("Number of pixel clusters per event" + m_histTitleExt + ";# pixel clusters/event;# events").c_str(), 150,0.,15000.));
   }
   if(m_doLowOccupancy) {
     sc = lumiBlockHist.regHist(m_num_clusters_low_LB = TH1I_LW::create("num_clusters_low_occupancy_LB", ("Number of pixel clusters per event" + m_histTitleExt + ";# pixel clusters/event;# events").c_str(), 200,-0.5,199.5));
   }

   if(m_doModules) {
     m_cluster_num_mod_LB = new PixelMonModules1D("Cluster_num_LB", ("Number of clusters per event in module" + m_histTitleExt).c_str(), 20,-0.5,59.5,m_doIBL);
     sc = m_cluster_num_mod_LB->regHist(this,(path+"/Modules_NumberOfClusters").c_str(),lowStat,m_doIBL);
     m_cluster_ToT_mod_LB = new PixelMonModules1D("Cluster_ToT_mod_LB", ("Cluster ToT in Module" + m_histTitleExt).c_str(), 75,0.,300.,m_doIBL);
     sc = m_cluster_ToT_mod_LB->regHist(this,(path+"/Modules_ClusToT").c_str(),lowStat,m_doIBL);
   }
   
   if(sc.isFailure())if(msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "histograms not booked" << endmsg;   
   return StatusCode::SUCCESS;
}
 
StatusCode PixelMainMon::FillClustersMon(void)
{
   sc = evtStore()->retrieve(m_Pixel_clcontainer, m_Pixel_SiClustersName);
   if (sc.isFailure()  || !m_Pixel_clcontainer)
   {
      if(msgLvl(MSG::INFO)) msg(MSG::INFO)  <<"Pixel Cluster container for Pixels not found"<< endmsg;
      if(m_storegate_errors) m_storegate_errors->Fill(3.,3.);  
      return StatusCode::SUCCESS;
   } else {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  <<"Pixel Cluster container for Pixels found" <<endmsg;
   }

   int nclusters=0;
   //int nclusters_ECA=0;
   //int nclusters_ECC=0;
   //int nclusters_IBL=0;
   //int nclusters_B0=0;
   //int nclusters_B1=0;
   //int nclusters_B2=0;
   int nclusters_mod[PixLayer::COUNT] = {0};
   int nlargeclusters=0;
   int nverylargeclusters=0;
   int nclusters_all=0;
   int nclusters_ontrack=0;

   if (m_doModules){
      if(m_doIBL){
	      for(int i=0;i<20;i++){
	         for(int j=0;j<14;j++) m_ClusPerEventArray_lI[j][i]=0;
	      }
      }
      for(int i=0;i<13;i++){
	      for(int j=0;j<22;j++) m_ClusPerEventArray_l0[j][i]=0;
	      for(int j=0;j<38;j++) m_ClusPerEventArray_l1[j][i]=0;
	      for(int j=0;j<52;j++) m_ClusPerEventArray_l2[j][i]=0;
	   }
      for(int j=0;j<3;j++){
	      for(int i=0;i<48;i++){
	         m_ClusPerEventArray_disksA[i][j]=0;
	         m_ClusPerEventArray_disksC[i][j]=0;
	      }
	   }
   }//end Initialize

   Identifier clusID;
   IdentifierHash id_hash;
   InDet::PixelClusterContainer::const_iterator colNext = m_Pixel_clcontainer->begin();
   InDet::PixelClusterContainer::const_iterator lastCol = m_Pixel_clcontainer->end();
   DataVector<PixelCluster>::const_iterator p_clus;
     
   for (; colNext != lastCol; ++colNext) 
   {
      const InDet::PixelClusterCollection* ClusterCollection(*colNext);
      
      if (!ClusterCollection) 
      {
         if(m_storegate_errors) m_storegate_errors->Fill(3.,5.);  //first entry (1). is for RDO, second (4) is for data problem
         continue;
      }
      for(p_clus=ClusterCollection->begin(); p_clus!=ClusterCollection->end(); ++p_clus)
      {
         clusID=(*p_clus)->identify();
         id_hash = m_pixelid->wafer_hash(clusID);
         int pixlayer = GetPixLayerID(m_pixelid->barrel_ec(clusID), m_pixelid->layer_disk(clusID), m_doIBL);
         int pixlayerdbm = GetPixLayerIDDBM(m_pixelid->barrel_ec(clusID), m_pixelid->layer_disk(clusID), m_doIBL);
         int pixlayeribl2d3d = pixlayer;
         if( pixlayeribl2d3d == PixLayerIBL2D3D::kIBL ){
            pixlayeribl2d3d = GetPixLayerIDIBL2D3D(m_pixelid->barrel_ec(clusID), m_pixelid->layer_disk(clusID), m_pixelid->eta_module(clusID), m_doIBL);
         }
         int pixlayeribl2d3ddbm = pixlayerdbm;
         if( pixlayeribl2d3ddbm == PixLayerDBM::kIBL ){
            pixlayeribl2d3ddbm = GetPixLayerIDIBL2D3DDBM(m_pixelid->barrel_ec(clusID), m_pixelid->layer_disk(clusID), m_pixelid->eta_module(clusID), m_doIBL);
         }
         if(pixlayer == 99) continue;

         //std::cout << id_hash << std::endl;

         int lumiblock = m_manager->lumiBlockNumber();

	      const PixelCluster& cluster = **p_clus;
	      const InDet::SiWidth clusWidth = cluster.width();

         //note: description is wrong in SiWidth! <x,y> = <row,col>
         int npixHitsInClusterRaw = cluster.rdoList().size();
         int npixHitsInCluster = cluster.rdoList().size();
         if( npixHitsInCluster > m_cluster_groupsize_mod[pixlayer]->getXMax()){
            npixHitsInCluster = (int)(m_cluster_groupsize_mod[pixlayer]->getXMax()-0.5);
         }
         int colWidth = clusWidth.colRow().y();
         int rowWidth = clusWidth.colRow().x();


	      nclusters_all++; // count all clusters for ontrack

         
         /// Quick Status
         int fephi=0;
         int feeta=0;
         if( pixlayer == PixLayer::kB0 && GetFEID( pixlayer, m_pixelid->phi_index(clusID), m_pixelid->eta_index(clusID), fephi, feeta) ){
            if(m_doOnTrack || m_doOnPixelTrack) { 
               if(m_pixelid->phi_module(clusID) == 0 && m_pixelid->eta_module(clusID) < 0 ){
                  if(OnTrack(clusID,true) ){
                     if(m_clusters_onTrack_L0_B11_S2_C6) m_clusters_onTrack_L0_B11_S2_C6->Fill( m_manager->lumiBlockNumber(), (16*fabs(6+m_pixelid->eta_module(clusID)))+(8.0*fephi)+feeta );
                  }else{
                     if(m_clusters_offTrack_L0_B11_S2_C6) m_clusters_offTrack_L0_B11_S2_C6->Fill( m_manager->lumiBlockNumber(), (16*fabs(6+m_pixelid->eta_module(clusID)))+(8.0*fephi)+feeta );
                  }
               }
            }
         }


         if( (m_doOnTrack || m_doOnPixelTrack) && !OnTrack(clusID,true) ){
	         continue;
            /// if we only want hits on track, and the hit is NOT on the track, skip filling.
            /// true means doing clusters, false means rdos
	      }


	      nclusters_ontrack++;


         //////////////////////////////////////////////////////
         /////////////Start main fill block here///////////////
         //be sure to check each histo exists before filling it
         //////////////////////////////////////////////////////
         
         //if( m_currentBCID != ) continue;
         ///
         /// Fill LVL1 Accepted
         ///
         if(m_cluster_LVL1A)     m_cluster_LVL1A->Fill(cluster.LVL1A());   
	      if(m_cluster_LVL1A_mod) m_cluster_LVL1A_mod->Fill(clusID,m_pixelid,cluster.LVL1A()+0.00001,m_doIBL); //avoid filling exactly zero to distinguish from disabled modules
	      if(cluster.rdoList().size()>1 
            && m_clus_LVL1A_sizenot1) m_clus_LVL1A_sizenot1->Fill(clusID,m_pixelid,cluster.LVL1A()+0.00001,m_doIBL); //avoid filling exactly zero to distinguish from disabled modules
	      if(pixlayer != PixLayer::kIBL){
            if(cluster.totalToT() > 15 && m_cluster_LVL1A1d_mod[pixlayer]) m_cluster_LVL1A1d_mod[pixlayer]->Fill(cluster.LVL1A());
	      }else{
            if(cluster.totalToT() > 4  && m_cluster_LVL1A1d_mod[pixlayer]) m_cluster_LVL1A1d_mod[pixlayer]->Fill(cluster.LVL1A());
         }

         ///
         /// Fill ToT
         ///
         if(m_cluster_ToT_mod) m_cluster_ToT_mod->Fill(cluster.totalToT(),clusID,m_pixelid,m_doIBL);   
	      if(m_cluster_ToT1d_mod[pixlayerdbm]) m_cluster_ToT1d_mod[pixlayerdbm]->Fill(cluster.totalToT());     
         if(pixlayerdbm == PixLayerDBM::kIBL && m_cluster_ToT1d_mod[pixlayeribl2d3ddbm]) m_cluster_ToT1d_mod[pixlayeribl2d3ddbm]->Fill(cluster.totalToT());     
         if(cluster.rdoList().size()==1 && m_1cluster_ToT_mod[pixlayer] ) m_1cluster_ToT_mod[pixlayer]->Fill(cluster.totalToT());
	      if(cluster.rdoList().size()==2 && m_2cluster_ToT_mod[pixlayer] ) m_2cluster_ToT_mod[pixlayer]->Fill(cluster.totalToT());
	      if(cluster.rdoList().size()==3 && m_3cluster_ToT_mod[pixlayer] ) m_3cluster_ToT_mod[pixlayer]->Fill(cluster.totalToT());
	      if(cluster.rdoList().size()>3  && m_bigcluster_ToT_mod[pixlayer] ) m_bigcluster_ToT_mod[pixlayer]->Fill(cluster.totalToT());

         ///
         /// Fill Charge
	      ///
         if(m_cluster_Q_mod[pixlayerdbm]) m_cluster_Q_mod[pixlayerdbm]->Fill(cluster.totalCharge());
	      if(pixlayerdbm == PixLayerDBM::kIBL  && m_cluster_Q_mod[pixlayeribl2d3ddbm]) m_cluster_Q_mod[pixlayeribl2d3ddbm]->Fill(cluster.totalCharge());
	      if(cluster.rdoList().size()==1 && m_1cluster_Q_mod[pixlayer] ) m_1cluster_Q_mod[pixlayer]->Fill(cluster.totalCharge());
	      if(cluster.rdoList().size()==2 && m_2cluster_Q_mod[pixlayer] ) m_2cluster_Q_mod[pixlayer]->Fill(cluster.totalCharge());
	      if(cluster.rdoList().size()==3 && m_3cluster_Q_mod[pixlayer] ) m_3cluster_Q_mod[pixlayer]->Fill(cluster.totalCharge());
	      if(cluster.rdoList().size()> 3 && m_bigcluster_Q_mod[pixlayer] ) m_bigcluster_Q_mod[pixlayer]->Fill(cluster.totalCharge());

         ///
         /// Fill Cluster efficiency
         ///
         if(OnTrack(clusID,true)){
	         if(m_cluseff_mod) m_cluseff_mod->Fill(m_manager->lumiBlockNumber(),1.,clusID,m_pixelid,m_doIBL);
            if(m_clusize_ontrack_mod[pixlayer]) m_clusize_ontrack_mod[pixlayer]->Fill(cluster.rdoList().size());
            if(pixlayer == PixLayer::kIBL && m_clusize_ontrack_mod[pixlayeribl2d3d]) m_clusize_ontrack_mod[pixlayeribl2d3d]->Fill( npixHitsInClusterRaw );
         }else{
            if(m_cluseff_mod) m_cluseff_mod->Fill(m_manager->lumiBlockNumber(),0.,clusID,m_pixelid,m_doIBL);
            if(m_clusize_offtrack_mod[pixlayer]) m_clusize_offtrack_mod[pixlayer]->Fill(cluster.rdoList().size());
            if(pixlayer == PixLayer::kIBL && m_clusize_offtrack_mod[pixlayeribl2d3d]) m_clusize_offtrack_mod[pixlayeribl2d3d]->Fill(npixHitsInClusterRaw);
         }
         
         ///
         /// Fill the number of pixel hits in a cluster
         ///
         if(m_cluster_groupsize) m_cluster_groupsize->Fill(npixHitsInClusterRaw);  
         if(m_cluster_col_width) m_cluster_col_width->Fill(colWidth);                         
         if(m_cluster_row_width) m_cluster_row_width->Fill(rowWidth);                         
         if(m_clusterSize_eta && m_pixelid->barrel_ec(clusID)==0 ) m_clusterSize_eta->Fill(m_pixelid->eta_module(clusID), npixHitsInClusterRaw);    
         if(m_cluster_groupsize_mod[pixlayer]) m_cluster_groupsize_mod[pixlayer]->Fill( npixHitsInCluster );
         if(pixlayer == PixLayer::kIBL && m_cluster_groupsize_mod[pixlayeribl2d3d]) m_cluster_groupsize_mod[pixlayeribl2d3d]->Fill( npixHitsInCluster );
         if(m_cluster_col_width_mod[pixlayer]) m_cluster_col_width_mod[pixlayer]->Fill( colWidth );
         if(pixlayer == PixLayer::kIBL && m_cluster_col_width_mod[pixlayeribl2d3d]) m_cluster_col_width_mod[pixlayeribl2d3d]->Fill( colWidth );
         if(m_cluster_row_width_mod[pixlayer]) m_cluster_row_width_mod[pixlayer]->Fill( rowWidth );
         if(pixlayer == PixLayer::kIBL && m_cluster_row_width_mod[pixlayeribl2d3d]) m_cluster_row_width_mod[pixlayeribl2d3d]->Fill( rowWidth );
         if(m_clusters_col_width_per_lumi_mod[pixlayer]) m_clusters_col_width_per_lumi_mod[pixlayer]->Fill(m_manager->lumiBlockNumber(), colWidth);
         if(pixlayer == PixLayer::kIBL && m_clusters_col_width_per_lumi_mod[pixlayeribl2d3d]) m_clusters_col_width_per_lumi_mod[pixlayeribl2d3d]->Fill(lumiblock, colWidth);
         if(m_clusters_row_width_per_lumi_mod[pixlayer]) m_clusters_row_width_per_lumi_mod[pixlayer]->Fill(m_manager->lumiBlockNumber(), rowWidth);
         if(pixlayer == PixLayer::kIBL && m_clusters_row_width_per_lumi_mod[pixlayeribl2d3d]) m_clusters_row_width_per_lumi_mod[pixlayeribl2d3d]->Fill(lumiblock, rowWidth);
         if(m_clusters_col_width_per_bcid_mod[pixlayer]) m_clusters_col_width_per_bcid_mod[pixlayer]->Fill(1.0*m_currentBCID, colWidth);
         if(pixlayer == PixLayer::kIBL && m_clusters_col_width_per_bcid_mod[pixlayeribl2d3d]) m_clusters_col_width_per_bcid_mod[pixlayeribl2d3d]->Fill(1.0*m_currentBCID, colWidth);
         if(m_clusters_row_width_per_bcid_mod[pixlayer]) m_clusters_row_width_per_bcid_mod[pixlayer]->Fill(1.0*m_currentBCID, rowWidth);
         if(pixlayer == PixLayer::kIBL && m_clusters_row_width_per_bcid_mod[pixlayeribl2d3d]) m_clusters_row_width_per_bcid_mod[pixlayeribl2d3d]->Fill(1.0*m_currentBCID, rowWidth);


	      ///
         /// Total Number of Clusters
         ///
         if(m_totalclusters_per_lumi) m_totalclusters_per_lumi->Fill(lumiblock); 
         if(m_totalclusters_per_lumi_mod[pixlayer] ) m_totalclusters_per_lumi_mod[pixlayer]->Fill( lumiblock );
         if(pixlayer == PixLayer::kIBL && m_totalclusters_per_lumi_mod[pixlayeribl2d3d] ) m_totalclusters_per_lumi_mod[pixlayeribl2d3d]->Fill( lumiblock );
         if(m_totalclusters_per_bcid_mod[pixlayer] ) m_totalclusters_per_bcid_mod[pixlayer]->Fill( 1.0*m_currentBCID );
         if(pixlayer == PixLayer::kIBL && m_totalclusters_per_bcid_mod[pixlayeribl2d3d] ) m_totalclusters_per_bcid_mod[pixlayeribl2d3d]->Fill( 1.0*m_currentBCID );

         ///
         /// Fill Occupancy
         ///
         if(m_cluster_occupancy) m_cluster_occupancy->Fill(clusID,m_pixelid,m_doIBL);
	      if(cluster.rdoList().size()>1 && m_clusocc_sizenot1) m_clusocc_sizenot1->Fill(clusID,m_pixelid,m_doIBL); 
	      if(m_doOnline && m_clustermap_tmp) m_clustermap_tmp->Fill(clusID, m_pixelid, m_doIBL);

         /// 2D Map
	      if(m_clussize_map) m_clussize_map->Fill(clusID,m_pixelid,cluster.rdoList().size(),m_doIBL);
	      if(m_cluscharge_map) m_cluscharge_map->Fill(clusID,m_pixelid,cluster.totalCharge(),m_doIBL);
	      if(m_clusToT_map) m_clusToT_map->Fill(clusID,m_pixelid,cluster.totalToT(),m_doIBL);

	      if(m_cluster_size_mod)m_cluster_size_mod->Fill(cluster.rdoList().size(),clusID,m_pixelid,m_doIBL);  

	 
	      float nhits=0;
	      float nlowToT_7=0;
	      float nlowToT_9=0;
	      float nlowToT_11=0;
	      float nlowToT_13=0;
	      for(unsigned int i=0;i<cluster.totList().size();i++){
	         nhits++;
            float tmptot = cluster.totList().at(i);
            if(tmptot<7) {nlowToT_7++;}
	         if(tmptot<9) {nlowToT_9++;}
	         if(tmptot<11){nlowToT_11++;}
	         if(tmptot<13){nlowToT_13++;}
	      }
         //if(m_lowToTHitsFraction_7)  m_lowToTHitsFraction_7->Fill(clusID, m_pixelid, m_doIBL, false, (1.0*nlowToT_7)/(1.0*nhits));
         if(m_lowToTHitsFraction_9  && nhits>0) m_lowToTHitsFraction_9->Fill(clusID, m_pixelid, m_doIBL, false, (1.0*nlowToT_9)/(1.0*nhits));
         if(m_lowToTHitsFraction_11 && nhits>0) m_lowToTHitsFraction_11->Fill(clusID,m_pixelid, m_doIBL, false, (1.0*nlowToT_11)/(1.0*nhits));
         //if(m_lowToTHitsFraction_13) m_lowToTHitsFraction_13->Fill(clusID,m_pixelid, m_doIBL, false, (1.0*nlowToT_13)/(1.0*nhits));
	 
	      if(m_clusToT_vs_eta_mod[pixlayer])  m_clusToT_vs_eta_mod[pixlayer]->Fill(m_pixelid->eta_module(clusID), cluster.totalToT());
	      if(m_ToT_vs_clussize_mod[pixlayer]) m_ToT_vs_clussize_mod[pixlayer]->Fill(cluster.totalToT(), cluster.rdoList().size());
         if(m_clussize_vs_eta_mod[pixlayer]) m_clussize_vs_eta_mod[pixlayer]->Fill( 1.0*m_pixelid->eta_module(clusID), cluster.rdoList().size() );


         //if(m_clusQ_vs_eta_mod[pixlayer]) m_clusQ_vs_eta_mod[pixlayer]->Fill( m_pixelid->eta_module(clusID), cluster.totalCharge() );
         //if(m_clussize_vs_eta_mod[pixlayer]) m_clussize_vs_eta_mod[pixlayer]->Fill( 1.0, 1.0);

         nclusters++;
         nclusters_mod[pixlayer]++;
	      if(cluster.rdoList().size() > 10) nlargeclusters++;
	      if(cluster.rdoList().size() > 50) nverylargeclusters++;

         if (m_doModules)//fill module cluster arrays 
         {
            if(m_pixelid->barrel_ec(clusID)==2 ) m_ClusPerEventArray_disksA[m_pixelid->phi_module(clusID)][m_pixelid->layer_disk(clusID)]++;
            if(m_pixelid->barrel_ec(clusID)==-2) m_ClusPerEventArray_disksC[m_pixelid->phi_module(clusID)][m_pixelid->layer_disk(clusID)]++;
            if(m_pixelid->barrel_ec(clusID)==0 )
            {
   	         if(m_doIBL && m_pixelid->layer_disk(clusID)==0) m_ClusPerEventArray_lI[m_pixelid->phi_module(clusID)][m_pixelid->eta_module(clusID)+10]++;
               if(m_pixelid->layer_disk(clusID)==0+m_doIBL)    m_ClusPerEventArray_l0[m_pixelid->phi_module(clusID)][m_pixelid->eta_module(clusID)+6]++;
               if(m_pixelid->layer_disk(clusID)==1+m_doIBL)    m_ClusPerEventArray_l1[m_pixelid->phi_module(clusID)][m_pixelid->eta_module(clusID)+6]++;
               if(m_pixelid->layer_disk(clusID)==2+m_doIBL)    m_ClusPerEventArray_l2[m_pixelid->phi_module(clusID)][m_pixelid->eta_module(clusID)+6]++;
            }  
         }

	      if (m_doLumiBlock) {
	         if (m_cluster_occupancy_LB) m_cluster_occupancy_LB->Fill(clusID,m_pixelid,m_doIBL);
	         if (m_cluster_ToT_LB)m_cluster_ToT_LB->Fill(cluster.totalToT());     
	         if (m_cluster_ToT_mod_LB) m_cluster_ToT_mod_LB->Fill(cluster.totalToT(),clusID,m_pixelid,m_doIBL);   
	      }

         /// Quick Status
         fephi=0;
         feeta=0;
         if( pixlayer == PixLayer::kB0 && GetFEID( pixlayer, m_pixelid->phi_index(clusID), m_pixelid->eta_index(clusID), fephi, feeta) ){
            if(m_doOnline){
              if(m_cluster_occupancy_FE_B0_mon) m_cluster_occupancy_FE_B0_mon->Fill( (8.0*m_pixelid->eta_module(clusID))+(1.0*feeta), (2.0*m_pixelid->phi_module(clusID))+(1.0*fephi));
              if(m_doRefresh5min && m_cluster_occupancy_FE_B0_mon) m_cluster_occupancy_FE_B0_mon->Reset();
            }
         }
	 
         ///////////End of main fill block////////////////////
      } // PixelClusterContainer loop
   }//end of event loop
   
   //////////////////////Fill after event block////////////////
   if(m_doOnline)//should we fill these mid run or only at the end?
   {
      //FillSummaryHistos(m_cluster_occupancy,m_ecA_cluster_occupancy_summary,m_ecC_cluster_occupancy_summary,m_bar_layI_cluster_occupancy_summary,m_bar_lay0_cluster_occupancy_summary,m_bar_lay1_cluster_occupancy_summary,m_bar_lay2_cluster_occupancy_summary);
      FillSummaryHistos(m_cluster_occupancy,
                        m_cluster_occupancy_summary_mod[PixLayer::kECA],
                        m_cluster_occupancy_summary_mod[PixLayer::kECC],
                        m_cluster_occupancy_summary_mod[PixLayer::kIBL],
                        m_cluster_occupancy_summary_mod[PixLayer::kB0],
                        m_cluster_occupancy_summary_mod[PixLayer::kB1],
                        m_cluster_occupancy_summary_mod[PixLayer::kB2]);
      FillSummaryHistos(m_cluster_occupancy,
                        m_cluster_occupancy_summary_low_mod[PixLayer::kECA],
                        m_cluster_occupancy_summary_low_mod[PixLayer::kECC],
                        m_cluster_occupancy_summary_low_mod[PixLayer::kIBL],
                        m_cluster_occupancy_summary_low_mod[PixLayer::kB0],
                        m_cluster_occupancy_summary_low_mod[PixLayer::kB1],
                        m_cluster_occupancy_summary_low_mod[PixLayer::kB2]);
      //FillSummaryHistos(m_cluster_occupancy,m_ecA_cluster_occupancy_summary_low,m_ecC_cluster_occupancy_summary_low,m_bar_layI_cluster_occupancy_summary_low,m_bar_lay0_cluster_occupancy_summary_low,m_bar_lay1_cluster_occupancy_summary_low,m_bar_lay2_cluster_occupancy_summary_low);
     if(m_doRefresh) {
       if(m_clustermap_mon && m_clustermap_tmp) m_clustermap_mon->Fill2DMon(m_clustermap_tmp);
     }
   }      


   if(m_clusters_per_lumi)m_clusters_per_lumi->Fill(m_manager->lumiBlockNumber(), nclusters); 
   //if(m_clusters_per_lumi_PIX)m_clusters_per_lumi_PIX->Fill(m_manager->lumiBlockNumber(),nclusters-nclusters_IBL); 
   for( int i=0; i<PixLayer::COUNT-1+(int)(m_doIBL); i++){
      if(m_clusters_per_lumi_mod[i]) m_clusters_per_lumi_mod[i]->Fill( m_manager->lumiBlockNumber(), nclusters_mod[i]);
      if(m_doOnTrack || m_doOnPixelTrack) { 
        if(m_clusters_per_track_per_lumi_mod[i] && m_ntracksPerEvent>0) m_clusters_per_track_per_lumi_mod[i]->Fill(m_manager->lumiBlockNumber(), (1.0*nclusters_mod[i])/(1.0*m_ntracksPerEvent));
      }
   }

   if(m_largeclusters_per_lumi)m_largeclusters_per_lumi->Fill(m_manager->lumiBlockNumber(), nlargeclusters); 
   if(m_verylargeclusters_per_lumi)m_verylargeclusters_per_lumi->Fill(m_manager->lumiBlockNumber(),nverylargeclusters); 
   if((nclusters>=1000) && m_highNclusters_per_lumi) m_highNclusters_per_lumi->Fill(m_manager->lumiBlockNumber()); 
   if (m_doOnTrack || m_doOnPixelTrack) {
     if(m_clustersOnOffTrack_per_lumi && nclusters_all>0) m_clustersOnOffTrack_per_lumi->Fill(m_manager->lumiBlockNumber(),(float)nclusters_ontrack/nclusters_all); 
   }

   ////Fill some histograms only if =< 50% of modules disabled
   if (!m_majorityDisabled) {
     
      if(m_num_clusters) m_num_clusters->Fill(nclusters);
      if(m_num_clusters_low) m_num_clusters_low->Fill(nclusters);
      
      for( int i=0; i<PixLayer::COUNT-1+(int)(m_doIBL); i++){
         if(m_num_clusters_mod[i]) m_num_clusters_mod[i]->Fill( nclusters_mod[i] );
      }

      if(m_cluster_occupancy_time1&&m_cluster_occupancy_time2&&m_cluster_occupancy_time3) FillTimeHisto(double(nclusters/(1744.0+280*m_doIBL)),m_cluster_occupancy_time1, m_cluster_occupancy_time2, m_cluster_occupancy_time3,10.,60.,360. );
      if(m_doLumiBlock && m_num_clusters_LB) m_num_clusters_LB->Fill(nclusters);
      if(m_doLumiBlock && m_num_clusters_low_LB) m_num_clusters_low_LB->Fill(nclusters);
     
      if (m_doModules)
      {
	      PixelID::const_id_iterator idIt       = m_pixelid->wafer_begin();
	      PixelID::const_id_iterator idItEnd    = m_pixelid->wafer_end();
	      for (; idIt != idItEnd; ++idIt) 
	      {
	         Identifier WaferID = *idIt;
	         if(m_pixelid->barrel_ec(WaferID)==2 ) m_cluster_num_mod->Fill( m_ClusPerEventArray_disksA[m_pixelid->phi_module(WaferID)][m_pixelid->layer_disk(WaferID)], WaferID, m_pixelid ,m_doIBL);
	         if(m_pixelid->barrel_ec(WaferID)==-2) m_cluster_num_mod->Fill( m_ClusPerEventArray_disksC[m_pixelid->phi_module(WaferID)][m_pixelid->layer_disk(WaferID)], WaferID, m_pixelid ,m_doIBL);
	         if(m_pixelid->barrel_ec(WaferID)==0 )                                                                                                                                       
	         {
		         if(m_doIBL && m_pixelid->layer_disk(WaferID)==0) m_cluster_num_mod->Fill( m_ClusPerEventArray_lI[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID)+10], WaferID, m_pixelid ,m_doIBL);
		         if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) m_cluster_num_mod->Fill( m_ClusPerEventArray_l0[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID)+6], WaferID, m_pixelid ,m_doIBL);
		         if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) m_cluster_num_mod->Fill( m_ClusPerEventArray_l1[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID)+6], WaferID, m_pixelid ,m_doIBL);
		         if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) m_cluster_num_mod->Fill( m_ClusPerEventArray_l2[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID)+6], WaferID, m_pixelid ,m_doIBL);
	         }  
	         if (m_doLumiBlock) {
	            if(m_pixelid->barrel_ec(WaferID)==2 ) m_cluster_num_mod_LB->Fill( m_ClusPerEventArray_disksA[m_pixelid->phi_module(WaferID)][m_pixelid->layer_disk(WaferID)], WaferID, m_pixelid ,m_doIBL);
	            if(m_pixelid->barrel_ec(WaferID)==-2) m_cluster_num_mod_LB->Fill( m_ClusPerEventArray_disksC[m_pixelid->phi_module(WaferID)][m_pixelid->layer_disk(WaferID)], WaferID, m_pixelid ,m_doIBL);
	            if(m_pixelid->barrel_ec(WaferID)==0 )                                                                                                                                       
		         {
		            if(m_doIBL && m_pixelid->layer_disk(WaferID)==0) m_cluster_num_mod_LB->Fill( m_ClusPerEventArray_lI[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID)+10], WaferID, m_pixelid ,m_doIBL);
		            if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) m_cluster_num_mod_LB->Fill( m_ClusPerEventArray_l0[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID)+6], WaferID, m_pixelid ,m_doIBL);
		            if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) m_cluster_num_mod_LB->Fill( m_ClusPerEventArray_l1[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID)+6], WaferID, m_pixelid ,m_doIBL);
		            if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) m_cluster_num_mod_LB->Fill( m_ClusPerEventArray_l2[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID)+6], WaferID, m_pixelid ,m_doIBL);
		         }  
	         }
	      }
      }
   }
   ////////////////////End fill after event block///////////
   if(nclusters==0 && m_storegate_errors) m_storegate_errors->Fill(3.,4.);//first entry for RDO, second for size = 0

   return StatusCode::SUCCESS;
}


StatusCode PixelMainMon::ProcClustersMon(void)
{
   if(m_doOffline) //run these only for offline
   {
      //FillSummaryHistos(m_cluster_occupancy,m_ecA_cluster_occupancy_summary,m_ecC_cluster_occupancy_summary,m_bar_layI_cluster_occupancy_summary,m_bar_lay0_cluster_occupancy_summary,m_bar_lay1_cluster_occupancy_summary,m_bar_lay2_cluster_occupancy_summary);
      FillSummaryHistos(m_cluster_occupancy,
                        m_cluster_occupancy_summary_mod[PixLayer::kECA],
                        m_cluster_occupancy_summary_mod[PixLayer::kECC],
                        m_cluster_occupancy_summary_mod[PixLayer::kIBL],
                        m_cluster_occupancy_summary_mod[PixLayer::kB0],
                        m_cluster_occupancy_summary_mod[PixLayer::kB1],
                        m_cluster_occupancy_summary_mod[PixLayer::kB2]);
      FillSummaryHistos(m_cluster_occupancy,
                        m_cluster_occupancy_summary_low_mod[PixLayer::kECA],
                        m_cluster_occupancy_summary_low_mod[PixLayer::kECC],
                        m_cluster_occupancy_summary_low_mod[PixLayer::kIBL],
                        m_cluster_occupancy_summary_low_mod[PixLayer::kB0],
                        m_cluster_occupancy_summary_low_mod[PixLayer::kB1],
                        m_cluster_occupancy_summary_low_mod[PixLayer::kB2]);
      //FillSummaryHistos(m_cluster_occupancy,m_ecA_cluster_occupancy_summary_low,m_ecC_cluster_occupancy_summary_low,m_bar_layI_cluster_occupancy_summary_low,m_bar_lay0_cluster_occupancy_summary_low,m_bar_lay1_cluster_occupancy_summary_low,m_bar_lay2_cluster_occupancy_summary_low);
   }
   
   double events = m_event;
   if(events==0) return StatusCode::SUCCESS; //if no events, the rest of the test is pointless and would divide by 0
   if( m_event != m_event2) events = m_event2;

   return StatusCode::SUCCESS;
}


