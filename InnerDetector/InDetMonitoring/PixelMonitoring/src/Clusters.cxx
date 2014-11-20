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
//#include "TrkParameters/MeasuredAtaPlane.h"
//#include "TrkParameters/MeasuredPerigee.h"
//#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
//#include "PixelConditionsServices/IPixelByteStreamErrorsSvc.h"
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
#include <sstream>

//#include "InDetRawData/InDetRawDataContainer.h"
//#include "InDetRawData/InDetRawDataCLASS_DEF.h" 
//#include "TrkTrack/TrackCollection.h"            
//#include "InDetReadoutGeometry/SiDetectorElement.h"
//#include "TrkTrackSummary/TrackSummary.h"
#include "PixelMonitoring/PixelMon2DMaps.h"
#include "PixelMonitoring/PixelMonModules.h"
#include "PixelMonitoring/PixelMonProfiles.h"

//#include "EventInfo/EventInfo.h"
//#include "EventInfo/TriggerInfo.h"
#include "PixelCabling/IPixelCablingSvc.h"


///////////////////////////////////////////////////////////////////////////////
//////////////////////booking methods//////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

StatusCode PixelMainMon::BookClustersMon(void)
{
   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "starting Book Clusters" << endreq;  
   
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

   sc = clusterShift.regHist(m_clusters_per_lumi  = TProfile_LW::create("Clusters_per_lumi",    ("Number of pixel clusters per event per LB" + m_histTitleExt + ";lumi block;# clusters/event").c_str(),2500,-0.5,2499.5));
   sc = clusterShift.regHist(m_clusters_per_lumi_PIX  = TProfile_LW::create("Clusters_per_lumi_PIX",    ("Number of pixel clusters per event per LB, pixel" + m_histTitleExt + ";lumi block;# clusters/event").c_str(),2500,-0.5,2499.5));
   sc = clusterExpert.regHist(m_clusters_per_lumi_ECA= TProfile_LW::create("Clusters_per_lumi_ECA",("Number of pixel clusters per event per LB, endcap A" + m_histTitleExt + ";lumi block;# clusters/event").c_str(),2500,-0.5,2499.5));
   sc = clusterExpert.regHist(m_clusters_per_lumi_ECC= TProfile_LW::create("Clusters_per_lumi_ECC",("Number of pixel clusters per event per LB, endcap C" + m_histTitleExt + ";lumi block;# clusters/event").c_str(),2500,-0.5,2499.5));
   sc = clusterExpert.regHist(m_clusters_per_lumi_B0 = TProfile_LW::create("Clusters_per_lumi_B0", ("Number of pixel clusters per event per LB, barrel layer 0" + m_histTitleExt + ";lumi block;# clusters/event").c_str(),2500,-0.5,2499.5));
   sc = clusterExpert.regHist(m_clusters_per_lumi_B1 = TProfile_LW::create("Clusters_per_lumi_B1", ("Number of pixel clusters per event per LB, barrel layer 1" + m_histTitleExt + ";lumi block;# clusters/event").c_str(),2500,-0.5,2499.5));
   sc = clusterExpert.regHist(m_clusters_per_lumi_B2 = TProfile_LW::create("Clusters_per_lumi_B2", ("Number of pixel clusters per event per LB, barrel layer 2" + m_histTitleExt + ";lumi block;# clusters/event").c_str(),2500,-0.5,2499.5));

   sc = clusterShift.regHist(m_totalclusters_per_lumi     = TH1I_LW::create("TotalClusters_per_lumi", ("Total number of pixel clusters per LB" + m_histTitleExt + ";lumi block;# clusters").c_str(),2500,-0.5,2499.5));
   sc = clusterExpert.regHist(m_totalclusters_per_lumi_ECA= TH1I_LW::create("TotalClusters_per_lumi_ECA",("Total number of pixel clusters per LB, endcap A" + m_histTitleExt + ";lumi block;# clusters").c_str(),2500,-0.5,2499.5));
   sc = clusterExpert.regHist(m_totalclusters_per_lumi_ECC= TH1I_LW::create("TotalClusters_per_lumi_ECC",("Total number of pixel clusters per LB, endcap C" + m_histTitleExt + ";lumi block;# clusters").c_str(),2500,-0.5,2499.5));
   sc = clusterExpert.regHist(m_totalclusters_per_lumi_B0 = TH1I_LW::create("TotalClusters_per_lumi_B0", ("Total number of pixel clusters per LB, barrel layer 0" + m_histTitleExt + ";lumi block;# clusters").c_str(),2500,-0.5,2499.5));
   sc = clusterExpert.regHist(m_totalclusters_per_lumi_B1 = TH1I_LW::create("TotalClusters_per_lumi_B1", ("Total number of pixel clusters per LB, barrel layer 1" + m_histTitleExt + ";lumi block;# clusters").c_str(),2500,-0.5,2499.5));
   sc = clusterExpert.regHist(m_totalclusters_per_lumi_B2 = TH1I_LW::create("TotalClusters_per_lumi_B2", ("Total number of pixel clusters per LB, barrel layer 2" + m_histTitleExt + ";lumi block;# clusters").c_str(),2500,-0.5,2499.5));

   sc = clusterShift.regHist(m_largeclusters_per_lumi    = TProfile_LW::create("LargeClusters_per_lumi",("Clusters (with >10 pixels) per event per LB" + m_histTitleExt + ";lumi block;# clusters/event").c_str(),2500,-0.5,2499.5));
   sc = clusterShift.regHist(m_verylargeclusters_per_lumi= TProfile_LW::create("VeryLargeClusters_per_lumi",("Clusters (with >50 pixels) per event per LB" + m_histTitleExt + ";lumi block;# clusters/event").c_str(),2500,-0.5,2499.5));
   sc = clusterExpert.regHist(m_highNclusters_per_lumi   = TH1I_LW::create("HighNClusters_per_lumi",    ("Rate of events with >1000 clusters/event per LB" + m_histTitleExt + ";lumi block;# event").c_str(),2500,-0.5,2499.5));

   sc = clusterExpert.regHist(m_cluster_groupsize    = TH1F_LW::create("Cluster_groupsize",   ("Number of pixels per cluster" + m_histTitleExt + "; # pixels/cluster; # clusters").c_str(), 300,-0.5,299.5));            
   sc = clusterExpert.regHist(m_cluster_col_width    = TH1F_LW::create("Cluster_column_width",("Column width of cluster" + m_histTitleExt + ";cluster width; # clusters").c_str(), 150,-0.5,149.5)); 
   sc = clusterExpert.regHist(m_cluster_row_width    = TH1F_LW::create("Cluster_row_width",   ("Row width of cluster" + m_histTitleExt + ";cluster width; # clusters").c_str(), 150,-0.5,149.5));
   
   sc = clusterExpert.regHist(m_clusterSize_eta      = TProfile_LW::create("ClusterSize_v_eta", ("Cluster size as a function of barrel module eta" + m_histTitleExt + ";module eta index;cluster width").c_str(), 13,-6.5,6.5));

   sc = clusterExpert.regHist(m_cluster_groupsize_B0    = TH1F_LW::create("Cluster_groupsize, B0",   ("Number of pixels per cluster, B0" + m_histTitleExt + "; # pixels/cluster; # clusters").c_str(),300,-0.5,299.5));
   sc = clusterExpert.regHist(m_cluster_col_width_B0    = TH1F_LW::create("Cluster_column_width, B0",("Column width of cluster, B0" + m_histTitleExt + ";cluster width; # clusters").c_str(), 150,-0.5,149.5));
   sc = clusterExpert.regHist(m_cluster_row_width_B0    = TH1F_LW::create("Cluster_row_width, B0",   ("Row width of cluster, B0" + m_histTitleExt + ";cluster width; # clusters").c_str(), 150,-0.5,149.5));
   sc = clusterExpert.regHist(m_cluster_groupsize_B1    = TH1F_LW::create("Cluster_groupsize, B1",   ("Number of pixels per cluster, B1" + m_histTitleExt + "; # pixels/cluster; # clusters").c_str(),300,-0.5,299.5));
   sc = clusterExpert.regHist(m_cluster_col_width_B1    = TH1F_LW::create("Cluster_column_width, B1",("Column width of cluster, B1" + m_histTitleExt + ";cluster width; # clusters").c_str(), 150,-0.5,149.5));
   sc = clusterExpert.regHist(m_cluster_row_width_B1    = TH1F_LW::create("Cluster_row_width, B1",   ("Row width of cluster, B1" + m_histTitleExt + ";cluster width; # clusters").c_str(), 150,-0.5,149.5));
   sc = clusterExpert.regHist(m_cluster_groupsize_B2    = TH1F_LW::create("Cluster_groupsize, B2",   ("Number of pixels per cluster, B2" + m_histTitleExt + "; # pixels/cluster; # clusters").c_str(),300,-0.5,299.5));
   sc = clusterExpert.regHist(m_cluster_col_width_B2    = TH1F_LW::create("Cluster_column_width, B2",("Column width of cluster, B2" + m_histTitleExt + ";cluster width; # clusters").c_str(), 150,-0.5,149.5));
   sc = clusterExpert.regHist(m_cluster_row_width_B2    = TH1F_LW::create("Cluster_row_width, B2",   ("Row width of cluster, B2" + m_histTitleExt + ";cluster width; # clusters").c_str(), 150,-0.5,149.5));
   sc = clusterExpert.regHist(m_cluster_groupsize_ECC    = TH1F_LW::create("Cluster_groupsize, ECC",   ("Number of pixels per cluster, ECC" + m_histTitleExt + "; # pixels/cluster; # clusters").c_str(),300,-0.5,299.5));
   sc = clusterExpert.regHist(m_cluster_col_width_ECC    = TH1F_LW::create("Cluster_column_width, ECC",("Column width of cluster, ECC" + m_histTitleExt + ";cluster width; # clusters").c_str(), 150,-0.5,149.5));
   sc = clusterExpert.regHist(m_cluster_row_width_ECC    = TH1F_LW::create("Cluster_row_width, ECC",   ("Row width of cluster, ECC" + m_histTitleExt + ";cluster width; # clusters").c_str(), 150,-0.5,149.5));
   sc = clusterExpert.regHist(m_cluster_groupsize_ECA    = TH1F_LW::create("Cluster_groupsize, ECA",   ("Number of pixels per cluster, ECA" + m_histTitleExt + "; # pixels/cluster; # clusters").c_str(),300,-0.5,299.5));
   sc = clusterExpert.regHist(m_cluster_col_width_ECA    = TH1F_LW::create("Cluster_column_width, ECA",("Column width of cluster, ECA" + m_histTitleExt + ";cluster width; # clusters").c_str(), 150,-0.5,149.5));
   sc = clusterExpert.regHist(m_cluster_row_width_ECA    = TH1F_LW::create("Cluster_row_width, ECA",   ("Row width of cluster, ECA" + m_histTitleExt + ";cluster width; # clusters").c_str(), 150,-0.5,149.5));

   if(m_doESD && !m_doOnline){
     sc= clusterExpert.regHist(m_clusQ_vs_eta_B0 = TH2F_LW::create("m_clusQ_vs_eta_B0", ("Cluster Q vs Eta, B0" + m_histTitleExt + ";  Module eta; Q").c_str(),13,-6.5,6.5,150,0,200000));
     sc= clusterExpert.regHist(m_clusQ_vs_eta_B1 = TH2F_LW::create("m_clusQ_vs_eta_B1", ("Cluster Q vs Eta, B1" + m_histTitleExt + "; Module eta; Q").c_str(),13,-6.5,6.5,150,0,200000));
     sc= clusterExpert.regHist(m_clusQ_vs_eta_B2 = TH2F_LW::create("m_clusQ_vs_eta_B2", ("Cluster Q vs Eta, B2" + m_histTitleExt + "; Module eta; Q").c_str(),13,-6.5,6.5,150,0,200000));
     sc= clusterExpert.regHist(m_clusQ_vs_eta_ECC = TH2F_LW::create("m_clusQ_vs_eta_ECC", ("Cluster Q vs Eta, ECC" + m_histTitleExt + "; Module eta; Q").c_str(),13,-6.5,6.5,150,0,200000));
     sc= clusterExpert.regHist(m_clusQ_vs_eta_ECA = TH2F_LW::create("m_clusQ_vs_eta_ECA", ("Cluster Q vs Eta, ECA" + m_histTitleExt + "; Module eta; Q").c_str(),13,-6.5,6.5,150,0,200000));

     sc= clusterExpert.regHist(m_clusToT_vs_eta_B0 = TH2F_LW::create("m_clusToT_vs_eta_B0", ("Cluster ToT vs Eta, B0" + m_histTitleExt + ";  Module eta; ToT").c_str(),13,-6.5,6.5,300,-0.5,299.5));
     sc= clusterExpert.regHist(m_clusToT_vs_eta_B1 = TH2F_LW::create("m_clusToT_vs_eta_B1", ("Cluster ToT vs Eta, B1" + m_histTitleExt + "; Module eta; ToT").c_str(),13,-6.5,6.5,300,-0.5,299.5));
     sc= clusterExpert.regHist(m_clusToT_vs_eta_B2 = TH2F_LW::create("m_clusToT_vs_eta_B2", ("Cluster ToT vs Eta, B2" + m_histTitleExt + "; Module eta; ToT").c_str(),13,-6.5,6.5,300,-0.5,299.5));
     sc= clusterExpert.regHist(m_clussize_vs_eta_B0 = TH2F_LW::create("m_clustersize_vs_eta_B0", ("Cluster Size vs Eta, B0" + m_histTitleExt + "; Module eta; Cluster size").c_str(),13,-6.5,6.5,40,-0.5,39.5));
     sc= clusterExpert.regHist(m_clussize_vs_eta_B1 = TH2F_LW::create("m_clustersize_vs_eta_B1", ("Cluster Size vs Eta, B1" + m_histTitleExt + "; Module eta; Cluster size").c_str(),13,-6.5,6.5,40,-0.5,39.5));
     sc= clusterExpert.regHist(m_clussize_vs_eta_B2 = TH2F_LW::create("m_clustersize_vs_eta_B2", ("Cluster Size vs Eta, B2" + m_histTitleExt + "; Module eta; Cluster size").c_str(),13,-6.5,6.5,40,-0.5,39.5));
     sc= clusterExpert.regHist(m_clussize_vs_eta_ECC = TH2F_LW::create("m_clustersize_vs_eta_ECC", ("Cluster Size vs Eta, ECC" + m_histTitleExt + "; Module eta; Cluster size").c_str(),13,-6.5,6.5,40,-0.5,39.5));
     sc= clusterExpert.regHist(m_clussize_vs_eta_ECA = TH2F_LW::create("m_clustersize_vs_eta_ECA", ("Cluster Size vs Eta, ECA" + m_histTitleExt + "; Module eta; Cluster size").c_str(),13,-6.5,6.5,40,-0.5,39.5));

     sc= clusterExpert.regHist(m_ToT_vs_clussize_B0 = TH2F_LW::create("m_ToT_vs_clussize_B0", ("Cluster ToT vs Size, B0" + m_histTitleExt + "; Size; ToT").c_str(),40,-0.5,39.5,300,-0.5,299.5));
     sc= clusterExpert.regHist(m_ToT_vs_clussize_B1 = TH2F_LW::create("m_ToT_vs_clussize_B1", ("Cluster ToT vs Size, B1" + m_histTitleExt + "; Size; ToT").c_str(),40,-0.5,39.5,300,-0.5,299.5));
     sc= clusterExpert.regHist(m_ToT_vs_clussize_B2 = TH2F_LW::create("m_ToT_vs_clussize_B2", ("Cluster ToT vs Size, B2" + m_histTitleExt + "; Size; ToT").c_str(),40,-0.5,39.5,300,-0.5,299.5));

     sc= clusterExpert.regHist(m_Q_vs_clussize_B0 = TH2F_LW::create("m_Q_vs_clussize_B0", ("Cluster Q vs Size, B0" + m_histTitleExt + "; Size; charge").c_str(),40,-0.5,39.5,150,0,200000));
     sc= clusterExpert.regHist(m_Q_vs_clussize_B1 = TH2F_LW::create("m_Q_vs_clussize_B1", ("Cluster Q vs Size, B1" + m_histTitleExt + "; Size; charge").c_str(),40,-0.5,39.5,150,0,200000));
     sc= clusterExpert.regHist(m_Q_vs_clussize_B2 = TH2F_LW::create("m_Q_vs_clussize_B2", ("Cluster Q vs Size, B2" + m_histTitleExt + "; Size; charge").c_str(),40,-0.5,39.5,150,0,200000));
     sc= clusterExpert.regHist(m_Q_vs_clussize_ECC = TH2F_LW::create("m_Q_vs_clussize_ECC", ("Cluster Q vs Size, ECC" + m_histTitleExt + "; Size; charge").c_str(),40,-0.5,39.5,150,0,200000));
     sc= clusterExpert.regHist(m_Q_vs_clussize_ECA = TH2F_LW::create("m_Q_vs_clussize_ECA", ("Cluster Q vs Size, ECA" + m_histTitleExt + "; Size; charge").c_str(),40,-0.5,39.5,150,0,200000));

     sc= clusterExpert.regHist(m_nlowToT_vs_clussize_B0 = TH2F_LW::create("m_nlowToT_vs_clussize_B0", ("# Low ToT Hits vs Size, B0" + m_histTitleExt + "; Size; frac low ToT hits").c_str(),40,-0.5,39.5,40,-0.5,39.5));
     sc= clusterExpert.regHist(m_nlowToT_vs_clussize_B1 = TH2F_LW::create("m_nlowToT_vs_clussize_B1", ("# Low ToT Hits vs Size, B1" + m_histTitleExt + "; Size; frac low ToT hits").c_str(),40,-0.5,39.5,40,-0.5,39.5));
     sc= clusterExpert.regHist(m_nlowToT_vs_clussize_B2 = TH2F_LW::create("m_nlowToT_vs_clussize_B2", ("# Low ToT Hits vs Size, B2" + m_histTitleExt + "; Size; frac low ToT hits").c_str(),40,-0.5,39.5,40,-0.5,39.5));
     sc= clusterExpert.regHist(m_nlowToT_vs_clussize_ECC = TH2F_LW::create("m_nlowToT_vs_clussize_ECC", ("# Low ToT Hits vs Size, ECC" + m_histTitleExt + "; Size; frac low ToT hits").c_str(),40,-0.5,39.5,40,-0.5,39.5));
     sc= clusterExpert.regHist(m_nlowToT_vs_clussize_ECA = TH2F_LW::create("m_nlowToT_vs_clussize_ECA", ("# Low ToT Hits vs Size, ECA" + m_histTitleExt + "; Size; frac low ToT hits").c_str(),40,-0.5,39.5,40,-0.5,39.5));
     
     // m_clusQ_vs_eta_B0->SetOption("colz");m_clusQ_vs_eta_B0->SetMinimum(0.);m_clusQ_vs_eta_B0->SetStats(0.);
     // m_clusQ_vs_eta_B1->SetOption("colz");m_clusQ_vs_eta_B1->SetMinimum(0.);m_clusQ_vs_eta_B1->SetStats(0.);
     // m_clusQ_vs_eta_B2->SetOption("colz");m_clusQ_vs_eta_B2->SetMinimum(0.);m_clusQ_vs_eta_B2->SetStats(0.);
     // m_clusQ_vs_eta_ECC->SetOption("colz");m_clusQ_vs_eta_ECC->SetMinimum(0.);m_clusQ_vs_eta_ECC->SetStats(0.);
     // m_clusQ_vs_eta_ECA->SetOption("colz");m_clusQ_vs_eta_ECA->SetMinimum(0.);m_clusQ_vs_eta_ECA->SetStats(0.);

     // m_clusToT_vs_eta_B0->SetOption("colz");m_clusToT_vs_eta_B0->SetMinimum(0.);m_clusToT_vs_eta_B0->SetStats(0.);
     // m_clusToT_vs_eta_B1->SetOption("colz");m_clusToT_vs_eta_B1->SetMinimum(0.);m_clusToT_vs_eta_B1->SetStats(0.);
     // m_clusToT_vs_eta_B2->SetOption("colz");m_clusToT_vs_eta_B2->SetMinimum(0.);m_clusToT_vs_eta_B2->SetStats(0.);
     
     // m_clussize_vs_eta_B0->SetOption("colz");m_clussize_vs_eta_B0->SetMinimum(0.);m_clussize_vs_eta_B0->SetStats(0.);
     // m_clussize_vs_eta_B1->SetOption("colz");m_clussize_vs_eta_B1->SetMinimum(0.);m_clussize_vs_eta_B1->SetStats(0.);
     // m_clussize_vs_eta_B2->SetOption("colz");m_clussize_vs_eta_B2->SetMinimum(0.);m_clussize_vs_eta_B2->SetStats(0.);
     // m_clussize_vs_eta_ECC->SetOption("colz");m_clussize_vs_eta_ECC->SetMinimum(0.);m_clussize_vs_eta_ECC->SetStats(0.);
     // m_clussize_vs_eta_ECA->SetOption("colz");m_clussize_vs_eta_ECA->SetMinimum(0.);m_clussize_vs_eta_ECA->SetStats(0.);
     
     // m_ToT_vs_clussize_B0->SetOption("colz");m_ToT_vs_clussize_B0->SetMinimum(0.);m_ToT_vs_clussize_B0->SetStats(0.);
     // m_ToT_vs_clussize_B1->SetOption("colz");m_ToT_vs_clussize_B1->SetMinimum(0.);m_ToT_vs_clussize_B1->SetStats(0.);
     // m_ToT_vs_clussize_B2->SetOption("colz");m_ToT_vs_clussize_B2->SetMinimum(0.);m_ToT_vs_clussize_B2->SetStats(0.);
     
     // m_nlowToT_vs_clussize_B0->SetOption("colz");m_nlowToT_vs_clussize_B0->SetMinimum(0.);m_nlowToT_vs_clussize_B0->SetStats(0.);
     // m_nlowToT_vs_clussize_B1->SetOption("colz");m_nlowToT_vs_clussize_B1->SetMinimum(0.);m_nlowToT_vs_clussize_B1->SetStats(0.);
     // m_nlowToT_vs_clussize_B2->SetOption("colz");m_nlowToT_vs_clussize_B2->SetMinimum(0.);m_nlowToT_vs_clussize_B2->SetStats(0.);
     // m_nlowToT_vs_clussize_ECC->SetOption("colz");m_nlowToT_vs_clussize_ECC->SetMinimum(0.);m_nlowToT_vs_clussize_ECC->SetStats(0.);
     // m_nlowToT_vs_clussize_ECA->SetOption("colz");m_nlowToT_vs_clussize_ECA->SetMinimum(0.);m_nlowToT_vs_clussize_ECA->SetStats(0.);

     // m_Q_vs_clussize_B0->SetOption("colz");m_Q_vs_clussize_B0->SetMinimum(0.);m_Q_vs_clussize_B0->SetStats(0.);
     // m_Q_vs_clussize_B1->SetOption("colz");m_Q_vs_clussize_B1->SetMinimum(0.);m_Q_vs_clussize_B1->SetStats(0.);
     // m_Q_vs_clussize_B2->SetOption("colz");m_Q_vs_clussize_B2->SetMinimum(0.);m_Q_vs_clussize_B2->SetStats(0.);
     // m_Q_vs_clussize_ECC->SetOption("colz");m_Q_vs_clussize_ECC->SetMinimum(0.);m_Q_vs_clussize_ECC->SetStats(0.);
     // m_Q_vs_clussize_ECA->SetOption("colz");m_Q_vs_clussize_ECA->SetMinimum(0.);m_Q_vs_clussize_ECA->SetStats(0.);

   }

   if (m_doOnTrack || m_doOnPixelTrack) { 
     sc = clusterExpert.regHist(m_clustersOnOffTrack_per_lumi = TProfile_LW::create("ClustersOnOffTrack_per_lumi",("Fraction pixel clusters on track per event per LB" + m_histTitleExt + ";lumi block; fraction clusters/event").c_str(),2500,-0.5,2499.5));
   }

   sc = clusterShift.regHist(m_cluster_ToT_PIX         = TH1F_LW::create("Cluster_ToT_PIX",    ("Cluster ToT: Pixel" + m_histTitleExt + ";Time over Threshold;# clusters").c_str(), 300,-0.5,299.5));
   sc = clusterExpert.regHist(m_cluster_ToT_ECA    = TH1F_LW::create("Cluster_ToT_ECA",("Cluster ToT: endcap A" + m_histTitleExt + ";ToT;# clusters").c_str(), 300,-0.5,299.5));
   sc = clusterExpert.regHist(m_cluster_ToT_ECC    = TH1F_LW::create("Cluster_ToT_ECC",("Cluster ToT: endcap C" + m_histTitleExt + ";ToT;# clusters").c_str(), 300,-0.5,299.5));
   sc = clusterExpert.regHist(m_cluster_ToT_B0     = TH1F_LW::create("Cluster_ToT_B0", ("Cluster ToT: barrel layer 0" + m_histTitleExt + ";ToT;# clusters").c_str(), 300,-0.5,299.5));
   sc = clusterExpert.regHist(m_cluster_ToT_B1     = TH1F_LW::create("Cluster_ToT_B1", ("Cluster ToT: barrel layer 1" + m_histTitleExt + ";ToT;# clusters").c_str(), 300,-0.5,299.5));
   sc = clusterExpert.regHist(m_cluster_ToT_B2     = TH1F_LW::create("Cluster_ToT_B2", ("Cluster ToT: barrel layer 2" + m_histTitleExt + ";ToT;# clusters").c_str(), 300,-0.5,299.5));
   sc = clusterExpert.regHist(m_bigcluster_ToT_B0     = TH1F_LW::create("Big_Cluster_ToT_B0", ("ToT for Clusters of size>3, B0" + m_histTitleExt + ";ToT;# clusters").c_str(), 300,-0.5,299.5));
   sc = clusterExpert.regHist(m_bigcluster_ToT_B1     = TH1F_LW::create("Big_Cluster_ToT_B1", ("ToT for Clusters of size>3, B1" + m_histTitleExt + ";ToT;# clusters").c_str(), 300,-0.5,299.5));
   sc = clusterExpert.regHist(m_bigcluster_ToT_B2     = TH1F_LW::create("Big_Cluster_ToT_B2", ("ToT for Clusters of size>3, B2" + m_histTitleExt + ";ToT;# clusters").c_str(), 300,-0.5,299.5));
   sc = clusterExpert.regHist(m_bigcluster_ToT_ECA     = TH1F_LW::create("Big_Cluster_ToT_ECA", ("ToT for Clusters of size>3, ECA" + m_histTitleExt + ";ToT;# clusters").c_str(), 300,-0.5,299.5));
   sc = clusterExpert.regHist(m_bigcluster_ToT_ECC     = TH1F_LW::create("Big_Cluster_ToT_ECC", ("ToT for Clusters of size>3, ECC" + m_histTitleExt + ";ToT;# clusters").c_str(), 300,-0.5,299.5));
   sc = clusterExpert.regHist(m_1cluster_ToT_B0     = TH1F_LW::create("1Hit_Cluster_ToT_B0", ("ToT for 1-hit clusters, B0" + m_histTitleExt + ";ToT;# clusters").c_str(), 300,-0.5,299.5));
   sc = clusterExpert.regHist(m_1cluster_ToT_B1     = TH1F_LW::create("1Hit_Cluster_ToT_B1", ("ToT for 1-hit clusters, B1" + m_histTitleExt + ";ToT;# clusters").c_str(), 300,-0.5,299.5));
   sc = clusterExpert.regHist(m_1cluster_ToT_B2     = TH1F_LW::create("1Hit_Cluster_ToT_B2", ("ToT for 1-hit clusters, B2" + m_histTitleExt + ";ToT;# clusters").c_str(), 300,-0.5,299.5));
   sc = clusterExpert.regHist(m_1cluster_ToT_ECA     = TH1F_LW::create("1Hit_Cluster_ToT_ECA", ("ToT for 1-hit clusters, ECA" + m_histTitleExt + ";ToT;# clusters").c_str(), 300,-0.5,299.5));
   sc = clusterExpert.regHist(m_1cluster_ToT_ECC     = TH1F_LW::create("1Hit_Cluster_ToT_ECC", ("ToT for 1-hit clusters, ECC" + m_histTitleExt + ";ToT;# clusters").c_str(), 300,-0.5,299.5));   
   sc = clusterExpert.regHist(m_2cluster_ToT_B0     = TH1F_LW::create("2Hit_Cluster_ToT_B0", ("ToT for 2-hit clusters, B0" + m_histTitleExt + ";ToT;# clusters").c_str(), 300,-0.5,299.5));
   sc = clusterExpert.regHist(m_2cluster_ToT_B1     = TH1F_LW::create("2Hit_Cluster_ToT_B1", ("ToT for 2-hit clusters, B1" + m_histTitleExt + ";ToT;# clusters").c_str(), 300,-0.5,299.5));
   sc = clusterExpert.regHist(m_2cluster_ToT_B2     = TH1F_LW::create("2Hit_Cluster_ToT_B2", ("ToT for 2-hit clusters, B2" + m_histTitleExt + ";ToT;# clusters").c_str(), 300,-0.5,299.5));
   sc = clusterExpert.regHist(m_2cluster_ToT_ECA     = TH1F_LW::create("2Hit_Cluster_ToT_ECA", ("ToT for 2-hit clusters, ECA" + m_histTitleExt + ";ToT;# clusters").c_str(), 300,-0.5,299.5));
   sc = clusterExpert.regHist(m_2cluster_ToT_ECC     = TH1F_LW::create("2Hit_Cluster_ToT_ECC", ("ToT for 2-hit clusters, ECC" + m_histTitleExt + ";ToT;# clusters").c_str(), 300,-0.5,299.5)); 
   sc = clusterExpert.regHist(m_3cluster_ToT_B0     = TH1F_LW::create("3Hit_Cluster_ToT_B0", ("ToT for 3-hit clusters, B0" + m_histTitleExt + ";ToT;# clusters").c_str(), 300,-0.5,299.5));
   sc = clusterExpert.regHist(m_3cluster_ToT_B1     = TH1F_LW::create("3Hit_Cluster_ToT_B1", ("ToT for 3-hit clusters, B1" + m_histTitleExt + ";ToT;# clusters").c_str(), 300,-0.5,299.5));
   sc = clusterExpert.regHist(m_3cluster_ToT_B2     = TH1F_LW::create("3Hit_Cluster_ToT_B2", ("ToT for 3-hit clusters, B2" + m_histTitleExt + ";ToT;# clusters").c_str(), 300,-0.5,299.5));
   sc = clusterExpert.regHist(m_3cluster_ToT_ECA     = TH1F_LW::create("3Hit_Cluster_ToT_ECA", ("ToT for 3-hit clusters, ECA" + m_histTitleExt + ";ToT;# clusters").c_str(), 300,-0.5,299.5));
   sc = clusterExpert.regHist(m_3cluster_ToT_ECC     = TH1F_LW::create("3Hit_Cluster_ToT_ECC", ("ToT for 3-hit clusters, ECC" + m_histTitleExt + ";ToT;# clusters").c_str(), 300,-0.5,299.5)); 
   
   if(m_doESD && !m_doOnline){
     sc = clusterExpert.regHist(m_bigcluster_Q_B0     = TH1F_LW::create("Big_Cluster_Q_B0", ("Charge for Clusters of size>3, B0" + m_histTitleExt + ";Charge;# clusters").c_str(), 150,0,200000));
     sc = clusterExpert.regHist(m_bigcluster_Q_B1     = TH1F_LW::create("Big_Cluster_Q_B1", ("Charge for Clusters of size>3, B1" + m_histTitleExt + ";Charge;# clusters").c_str(), 150,0,200000));
     sc = clusterExpert.regHist(m_bigcluster_Q_B2     = TH1F_LW::create("Big_Cluster_Q_B2", ("Charge for Clusters of size>3, B2" + m_histTitleExt + ";Charge;# clusters").c_str(), 150,0,200000));
     sc = clusterExpert.regHist(m_bigcluster_Q_ECA     = TH1F_LW::create("Big_Cluster_Q_ECA", ("Charge for Clusters of size>3, ECA" + m_histTitleExt + ";Charge;# clusters").c_str(), 150,0,200000));
     sc = clusterExpert.regHist(m_bigcluster_Q_ECC     = TH1F_LW::create("Big_Cluster_Q_ECC", ("Charge for Clusters of size>3, ECC" + m_histTitleExt + ";Charge;# clusters").c_str(), 150,0,200000));
     sc = clusterExpert.regHist(m_1cluster_Q_B0     = TH1F_LW::create("1Hit_Cluster_Q_B0", ("Charge for 1-hit clusters, B0" + m_histTitleExt + ";Charge;# clusters").c_str(), 150,0,200000));
     sc = clusterExpert.regHist(m_1cluster_Q_B1     = TH1F_LW::create("1Hit_Cluster_Q_B1", ("Charge for 1-hit clusters, B1" + m_histTitleExt + ";Charge;# clusters").c_str(), 150,0,200000));
     sc = clusterExpert.regHist(m_1cluster_Q_B2     = TH1F_LW::create("1Hit_Cluster_Q_B2", ("Charge for 1-hit clusters, B2" + m_histTitleExt + ";Charge;# clusters").c_str(), 150,0,200000));
     sc = clusterExpert.regHist(m_1cluster_Q_ECA     = TH1F_LW::create("1Hit_Cluster_Q_ECA", ("Charge for 1-hit clusters, ECA" + m_histTitleExt + ";Charge;# clusters").c_str(), 150,0,200000));
     sc = clusterExpert.regHist(m_1cluster_Q_ECC     = TH1F_LW::create("1Hit_Cluster_Q_ECC", ("Charge for 1-hit clusters, ECC" + m_histTitleExt + ";Charge;# clusters").c_str(), 150,0,200000));   
     sc = clusterExpert.regHist(m_2cluster_Q_B0     = TH1F_LW::create("2Hit_Cluster_Q_B0", ("Charge for 2-hit clusters, B0" + m_histTitleExt + ";Charge;# clusters").c_str(), 150,0,200000));
     sc = clusterExpert.regHist(m_2cluster_Q_B1     = TH1F_LW::create("2Hit_Cluster_Q_B1", ("Charge for 2-hit clusters, B1" + m_histTitleExt + ";Charge;# clusters").c_str(), 150,0,200000));
     sc = clusterExpert.regHist(m_2cluster_Q_B2     = TH1F_LW::create("2Hit_Cluster_Q_B2", ("Charge for 2-hit clusters, B2" + m_histTitleExt + ";Charge;# clusters").c_str(), 150,0,200000));
     sc = clusterExpert.regHist(m_2cluster_Q_ECA     = TH1F_LW::create("2Hit_Cluster_Q_ECA", ("Charge for 2-hit clusters, ECA" + m_histTitleExt + ";Charge;# clusters").c_str(), 150,0,200000));
     sc = clusterExpert.regHist(m_2cluster_Q_ECC     = TH1F_LW::create("2Hit_Cluster_Q_ECC", ("Charge for 2-hit clusters, ECC" + m_histTitleExt + ";Charge;# clusters").c_str(), 150,0,200000)); 
     sc = clusterExpert.regHist(m_3cluster_Q_B0     = TH1F_LW::create("3Hit_Cluster_Q_B0", ("Charge for 3-hit clusters, B0" + m_histTitleExt + ";Charge;# clusters").c_str(), 150,0,200000));
     sc = clusterExpert.regHist(m_3cluster_Q_B1     = TH1F_LW::create("3Hit_Cluster_Q_B1", ("Charge for 3-hit clusters, B1" + m_histTitleExt + ";Charge;# clusters").c_str(), 150,0,200000));
     sc = clusterExpert.regHist(m_3cluster_Q_B2     = TH1F_LW::create("3Hit_Cluster_Q_B2", ("Charge for 3-hit clusters, B2" + m_histTitleExt + ";Charge;# clusters").c_str(), 150,0,200000));
     sc = clusterExpert.regHist(m_3cluster_Q_ECA     = TH1F_LW::create("3Hit_Cluster_Q_ECA", ("Charge for 3-hit clusters, ECA" + m_histTitleExt + ";Charge;# clusters").c_str(), 150,0,200000));
     sc = clusterExpert.regHist(m_3cluster_Q_ECC     = TH1F_LW::create("3Hit_Cluster_Q_ECC", ("Charge for 3-hit clusters, ECC" + m_histTitleExt + ";Charge;# clusters").c_str(), 150,0,200000)); 
   }

   sc = timeShift.regHist(m_cluster_LVL1A          = TH1F_LW::create("Cluster_LVL1A", ("Cluster Level 1 Accept" + m_histTitleExt + ";LVL1A;# clusters").c_str(), 14,-1.5,12.5));
   sc = timeShift.regHist(m_cluster_LVL1A_PIX          = TH1F_LW::create("Cluster_LVL1A_PIX", ("Cluster Level 1 Accept with ToT>15, pixel" + m_histTitleExt + ";LVL1A;# clusters").c_str(), 18,-1.5,16.5));

   sc = timeExpert.regHist(m_cluster_LVL1A_highToT = TH1F_LW::create("Cluster_LVL1A_highToT",("Cluster Level 1 Accept with ToT>15" + m_histTitleExt + ";LVL1A;# clusters").c_str(), 14,-1.5,12.5));
   sc = timeExpert.regHist(m_cluster_LVL1A_ECA     = TH1F_LW::create("Cluster_LVL1A_ECA",("Cluster Level 1 Accept with ToT>15, endcap A" + m_histTitleExt + ";LVL1A;# clusters").c_str(), 14,-1.5,12.5));
   sc = timeExpert.regHist(m_cluster_LVL1A_ECC     = TH1F_LW::create("Cluster_LVL1A_ECC",("Cluster Level 1 Accept with ToT>15, endcap C" + m_histTitleExt + ";LVL1A;# clusters").c_str(), 14,-1.5,12.5));
   sc = timeExpert.regHist(m_cluster_LVL1A_B0      = TH1F_LW::create("Cluster_LVL1A_B0", ("Cluster Level 1 Accept with ToT>15, barrel layer 0" + m_histTitleExt + "; LVL1A;# clusters").c_str(), 14,-1.5,12.5));
   sc = timeExpert.regHist(m_cluster_LVL1A_B1      = TH1F_LW::create("Cluster_LVL1A_B1", ("Cluster Level 1 Accept with ToT>15, barrel layer 1" + m_histTitleExt + "; LVL1A;# clusters").c_str(), 14,-1.5,12.5));
   sc = timeExpert.regHist(m_cluster_LVL1A_B2      = TH1F_LW::create("Cluster_LVL1A_B2", ("Cluster Level 1 Accept with ToT>15, barrel layer 2" + m_histTitleExt + "; LVL1A;# clusters").c_str(), 14,-1.5,12.5));

   sc = clusterExpert.regHist(m_cluster_charge_ECA = TH1F_LW::create("Cluster_charge_ECA", ("Cluster total charge, endcap A" + m_histTitleExt + ";charge;# clusters").c_str(), 150,0,200000));
   sc = clusterExpert.regHist(m_cluster_charge_ECC = TH1F_LW::create("Cluster_charge_ECC", ("Cluster total charge, endcap C" + m_histTitleExt + ";charge;# clusters").c_str(), 150,0,200000));
   sc = clusterExpert.regHist(m_cluster_charge_B0  = TH1F_LW::create("Cluster_charge_B0",  ("Cluster total charge, barrel layer 0" + m_histTitleExt + "; charge;# clusters").c_str(), 150,0,200000));
   sc = clusterExpert.regHist(m_cluster_charge_B1  = TH1F_LW::create("Cluster_charge_B1",  ("Cluster total charge, barrel layer 1" + m_histTitleExt + "; charge;# clusters").c_str(), 150,0,200000));
   sc = clusterExpert.regHist(m_cluster_charge_B2  = TH1F_LW::create("Cluster_charge_B2",  ("Cluster total charge, barrel layer 2" + m_histTitleExt + "; charge;# clusters").c_str(), 150,0,200000));
   
   if(m_doIBL){
     sc = clusterExpert.regHist(m_clusters_per_lumi_IBL = TProfile_LW::create("Clusters_per_lumi_IBL", ("Number of pixel clusters per event per LB, IBL" + m_histTitleExt + ";lumi block;# clusters/event").c_str(),2500,-0.5,2499.5));
     sc = clusterExpert.regHist(m_totalclusters_per_lumi_IBL = TH1I_LW::create("TotalClusters_per_lumi_IBL", ("Total number of pixel clusters per LB, IBL" + m_histTitleExt + ";lumi block;# clusters").c_str(),2500,-0.5,2499.5));
     sc = clusterExpert.regHist(m_cluster_ToT_IBL     = TH1F_LW::create("Cluster_ToT_IBL", ("Cluster ToT: IBL" + m_histTitleExt + ";Time over Threshold;# clusters").c_str(), 50,-0.5,49.5));
     sc = clusterExpert.regHist(m_cluster_ToT_IBL2D     = TH1F_LW::create("Cluster_ToT_IBL2D", ("Cluster ToT for IBL Planar Modules" + m_histTitleExt + ";ToT;# clusters").c_str(), 100,-0.5,99.5));
     sc = clusterExpert.regHist(m_cluster_ToT_IBL3D     = TH1F_LW::create("Cluster_ToT_IBL3D", ("Cluster ToT for IBL 3D Modules" + m_histTitleExt + ";ToT;# clusters").c_str(), 100,-0.5,99.5));
     sc = clusterExpert.regHist(m_bigcluster_ToT_IBL     = TH1F_LW::create("Big_Cluster_ToT_IBL", ("ToT for Clusters of size>3, IBL" + m_histTitleExt + ";ToT;# clusters").c_str(), 300,-0.5,299.5));
     sc = clusterExpert.regHist(m_1cluster_ToT_IBL     = TH1F_LW::create("1Hit_Cluster_ToT_IBL", ("ToT for 1-hit clusters, IBL" + m_histTitleExt + ";ToT;# clusters").c_str(), 300,-0.5,299.5));
     sc = clusterExpert.regHist(m_2cluster_ToT_IBL     = TH1F_LW::create("2Hit_Cluster_ToT_IBL", ("ToT for 2-hit clusters, IBL" + m_histTitleExt + ";ToT;# clusters").c_str(), 300,-0.5,299.5));
     sc = clusterExpert.regHist(m_3cluster_ToT_IBL     = TH1F_LW::create("3Hit_Cluster_ToT_IBL", ("ToT for 3-hit clusters, IBL" + m_histTitleExt + ";ToT;# clusters").c_str(), 300,-0.5,299.5));
     sc = timeExpert.regHist(m_cluster_LVL1A_IBL      = TH1F_LW::create("Cluster_LVL1A_IBL", ("Cluster Level 1 Accept with ToT >4, IBL" + m_histTitleExt + "; LVL1A;# clusters").c_str(), 18,-1.5,16.5));
     sc = clusterExpert.regHist(m_cluster_charge_IBL  = TH1F_LW::create("Cluster_charge_IBL",  ("Cluster total charge, IBL" + m_histTitleExt + "; charge;# clusters").c_str(), 150,0,200000)); 
     sc = clusterExpert.regHist(m_cluster_groupsize_IBL    = TH1F_LW::create("Cluster_groupsize, IBL",   ("Number of pixels per cluster, IBL" + m_histTitleExt + "; # pixels/cluster; # clusters").c_str(),300,-0.5,299.5));
     sc = clusterExpert.regHist(m_cluster_col_width_IBL    = TH1F_LW::create("Cluster_column_width, IBL",("Column width of cluster, IBL" + m_histTitleExt + ";cluster width; # clusters").c_str(), 150,-0.5,149.5));
     sc = clusterExpert.regHist(m_cluster_row_width_IBL    = TH1F_LW::create("Cluster_row_width, IBL",   ("Row width of cluster, IBL" + m_histTitleExt + ";cluster width; # clusters").c_str(), 150,-0.5,149.5));
    
     if(m_doESD && ! m_doOnline){
       sc = clusterExpert.regHist(m_bigcluster_Q_IBL     = TH1F_LW::create("Big_Cluster_Q_IBL", ("ToT for Clusters of size>3, IBL" + m_histTitleExt + ";ToT;# clusters").c_str(), 150,0,200000));
       sc = clusterExpert.regHist(m_1cluster_Q_IBL     = TH1F_LW::create("1Hit_Cluster_Q_IBL", ("ToT for 1-hit clusters, IBL" + m_histTitleExt + ";ToT;# clusters").c_str(), 150,0,200000));
       sc = clusterExpert.regHist(m_2cluster_Q_IBL     = TH1F_LW::create("2Hit_Cluster_Q_IBL", ("ToT for 2-hit clusters, IBL" + m_histTitleExt + ";ToT;# clusters").c_str(), 150,0,200000));
       sc = clusterExpert.regHist(m_3cluster_Q_IBL     = TH1F_LW::create("3Hit_Cluster_Q_B0", ("Charge for 3-hit clusters, B0" + m_histTitleExt + ";Charge;# clusters").c_str(), 150,0,200000));
       sc= clusterExpert.regHist(m_nlowToT_vs_clussize_IBL = TH2F_LW::create("m_nlowToT_vs_clussize_IBL", ("# Low ToT Hits vs Size, IBL" + m_histTitleExt + "; Size; frac low ToT hits").c_str(),40,-0.5,39.5,40,-0.5,39.5));
       sc= clusterExpert.regHist(m_clusToT_vs_eta_IBL      = TH2F_LW::create("m_clusToT_vs_eta_IBL", ("Cluster ToT vs Eta, IBL" + m_histTitleExt + ";  Module eta; ToT").c_str(),20,-10.5,9.5,300,-0.5,299.5));
       sc= clusterExpert.regHist(m_clussize_vs_eta_IBL = TH2F_LW::create("m_clustersize_vs_eta_IBL", ("Cluster Size vs Eta, IBL" + m_histTitleExt + "; Module eta; Cluster size").c_str(),20,-10.5,9.5,40,-0.5,39.5));
       sc= clusterExpert.regHist(m_ToT_vs_clussize_IBL = TH2F_LW::create("m_ToT_vs_clussize_IBL", ("Cluster ToT vs Size, IBL" + m_histTitleExt + "; Size; ToT").c_str(),40,-0.5,39.5,300,-0.5,299.5));
       // m_clusToT_vs_eta_IBL->SetOption("colz");m_clusToT_vs_eta_IBL->SetMinimum(0.);m_clusToT_vs_eta_IBL->SetStats(0.);
       // m_clussize_vs_eta_IBL->SetOption("colz");m_clussize_vs_eta_IBL->SetMinimum(0.);m_clussize_vs_eta_IBL->SetStats(0.);
       // m_ToT_vs_clussize_IBL->SetOption("colz");m_ToT_vs_clussize_IBL->SetMinimum(0.);m_ToT_vs_clussize_IBL->SetStats(0.);
       // m_nlowToT_vs_clussize_IBL->SetOption("colz");m_nlowToT_vs_clussize_IBL->SetMinimum(0.);m_nlowToT_vs_clussize_IBL->SetStats(0.);
     }
   }
   if(m_do2DMaps)
   {
      m_cluster_occupancy = new PixelMon2DMaps("Cluster_Occupancy", ("Cluster occupancy" + m_histTitleExt).c_str());
      sc = m_cluster_occupancy->regHist(clusterShift);
      m_average_cluster_occupancy = new PixelMon2DMaps("Average_Cluster_Occupancy", ("Average cluster occupancy" + m_histTitleExt).c_str());
      sc = m_average_cluster_occupancy->regHist(clusterShift);
      m_cluster_LVL1A_mod = new PixelMonProfiles("Cluster_LVL1A_Mod", ("Cluster Level 1 Accept" + m_histTitleExt).c_str());
      sc = m_cluster_LVL1A_mod->regHist(timeShift);
      m_clusocc_sizenot1 = new PixelMon2DMaps("Clus_Occ_SizeCut", ("Size>1 Cluster occupancy" + m_histTitleExt).c_str()); 
      sc = m_clusocc_sizenot1->regHist(clusterShift); 
      m_clus_LVL1A_sizenot1 = new PixelMonProfiles("Clus_LVL1A_SizeCut", ("Size>1 Cluster Level 1 Accept" + m_histTitleExt).c_str()); 
      sc = m_clus_LVL1A_sizenot1->regHist(timeShift); 

      if(!m_doOnline){
	m_clussize_map = new PixelMonProfiles("Cluster_Size_Map", ("Cluster size map" + m_histTitleExt).c_str());
	sc = m_clussize_map->regHist(clusterExpert);
	m_cluscharge_map = new PixelMonProfiles("Cluster_Charge_Map", ("Cluster charge map" + m_histTitleExt).c_str());
	sc = m_cluscharge_map->regHist(clusterExpert);
	m_clusToT_map = new PixelMonProfiles("Cluster_ToT_Map", ("Cluster ToT map" + m_histTitleExt).c_str());
	sc = m_clusToT_map->regHist(clusterExpert);
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
     m_cluster_ToT_mod = new PixelMonModules1D("Cluster_ToT", ("Cluster ToT in Module" + m_histTitleExt).c_str(), 75,0.,300.,m_doIBL);
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
      sc = clusterShift.regHist(m_num_clusters      = TH1I_LW::create("num_clusters",    ("Number of pixel clusters per event;# pixel clusters/event" + m_histTitleExt + ";# events").c_str(), 1000,0.,(int)max_clusters));
      sc = clusterExpert.regHist(m_num_clusters_PIX = TH1I_LW::create("num_clusters_PIX",("Number of pixel clusters per event, pixel" + m_histTitleExt + ";# pixel clusters/event;# events").c_str(),1000,0.,(int)max_clusters));
      sc = clusterExpert.regHist(m_num_clusters_ECA = TH1I_LW::create("num_clusters_ECA",("Number of pixel clusters per event, endcap A" + m_histTitleExt + ";# pixel clusters/event;# events").c_str(),160,0.,(int)max_clusters_region));
      sc = clusterExpert.regHist(m_num_clusters_ECC = TH1I_LW::create("num_clusters_ECC",("Number of pixel clusters per event, endcap C" + m_histTitleExt + ";# pixel clusters/event;# events").c_str(),160,0.,(int)max_clusters_region));
      if(m_doIBL){sc = clusterExpert.regHist(m_num_clusters_IBL  = TH1I_LW::create("num_clusters_IBL", ("Number of pixel clusters per event, IBL" + m_histTitleExt + ";# pixel clusters/event;# events").c_str(),160,0.,(int)max_clusters_region));}
      sc = clusterExpert.regHist(m_num_clusters_B0  = TH1I_LW::create("num_clusters_B0", ("Number of pixel clusters per event, barrel layer 0" + m_histTitleExt + ";# pixel clusters/event;# events").c_str(),160,0.,(int)max_clusters_region));
      sc = clusterExpert.regHist(m_num_clusters_B1  = TH1I_LW::create("num_clusters_B1", ("Number of pixel clusters per event, barrel layer 1" + m_histTitleExt + ";# pixel clusters/event;# events").c_str(),160,0.,(int)max_clusters_region));
      sc = clusterExpert.regHist(m_num_clusters_B2  = TH1I_LW::create("num_clusters_B2", ("Number of pixel clusters per event, barrel layer 2" + m_histTitleExt + ";# pixel clusters/event;# events").c_str(),160,0.,(int)max_clusters_region));

      sc = clusterExpert.regHist(m_ecA_cluster_occupancy_summary     = TH1F_LW::create("A_cluster_occupancy_summary",  ("Average Endcap A cluster occupancy" + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 100,0.,(int)max_avclusters));
      sc = clusterExpert.regHist(m_ecC_cluster_occupancy_summary     = TH1F_LW::create("C_cluster_occupancy_summary",  ("Average Endcap C cluster occupancy" + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 100,0.,(int)max_avclusters));
      sc = clusterExpert.regHist(m_bar_layI_cluster_occupancy_summary= TH1F_LW::create("IBL_cluster_occupancy_summary", ("Average IBL cluster occupancy" + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 100,0.,(int)max_avclusters));
      sc = clusterExpert.regHist(m_bar_lay0_cluster_occupancy_summary= TH1F_LW::create("B0_cluster_occupancy_summary", ("Average Barrel layer 0 cluster occupancy" + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 100,0.,(int)max_avclusters));
      sc = clusterExpert.regHist(m_bar_lay1_cluster_occupancy_summary= TH1F_LW::create("B1_cluster_occupancy_summary", ("Average Barrel layer 1 cluster occupancy" + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 100,0.,(int)max_avclusters));
      sc = clusterExpert.regHist(m_bar_lay2_cluster_occupancy_summary= TH1F_LW::create("B2_cluster_occupancy_summary", ("Average Barrel layer 2 cluster occupancy" + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 100,0.,(int)max_avclusters));
   }
   if(m_doLowOccupancy)
   {
      sc = clusterShift.regHist(m_num_clusters_low                       = TH1I_LW::create("num_clusters_low_occupancy",  ("Number of pixel clusters per event" + m_histTitleExt + ";# pixel clusters/event;# events").c_str(), 200,-0.5,199.5));
      sc = clusterExpert.regHist(m_ecA_cluster_occupancy_summary_low     = TH1F_LW::create("A_cluster_occupancy_summary_low_occupancy",  ("Average Endcap A cluster occupancy" + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 50,0.,2.));
      sc = clusterExpert.regHist(m_ecC_cluster_occupancy_summary_low     = TH1F_LW::create("C_cluster_occupancy_summary_low_occupancy",  ("Average Endcap C cluster occupancy" + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 50,0.,2.));
      sc = clusterExpert.regHist(m_bar_layI_cluster_occupancy_summary_low= TH1F_LW::create("IBL_cluster_occupancy_summary_low_occupancy", ("Average IBL cluster occupancy" + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 50,0.,2.));
      sc = clusterExpert.regHist(m_bar_lay0_cluster_occupancy_summary_low= TH1F_LW::create("B0_cluster_occupancy_summary_low_occupancy", ("Average Barrel layer 0 cluster occupancy" + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 50,0.,2.));
      sc = clusterExpert.regHist(m_bar_lay1_cluster_occupancy_summary_low= TH1F_LW::create("B1_cluster_occupancy_summary_low_occupancy", ("Average Barrel layer 1 cluster occupancy" + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 50,0.,2.));
      sc = clusterExpert.regHist(m_bar_lay2_cluster_occupancy_summary_low= TH1F_LW::create("B2_cluster_occupancy_summary_low_occupancy", ("Average Barrel layer 2 cluster occupancy" + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 50,0.,2.));
   }
   //   int max_clusters = 500;

   if(m_doPixelOccupancy){ }
   if(m_doDetails){ }

   if(sc.isFailure())if(msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "histograms not booked" << endreq;   
   return StatusCode::SUCCESS;
}


StatusCode PixelMainMon::BookClustersLumiBlockMon(void)
{
   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "starting Book Clusters for lowStat" << endreq;  
   
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
     m_cluster_ToT_mod_LB = new PixelMonModules1D("Cluster_ToT_LB", ("Cluster ToT in Module" + m_histTitleExt).c_str(), 75,0.,300.,m_doIBL);
     sc = m_cluster_ToT_mod_LB->regHist(this,(path+"/Modules_ClusToT").c_str(),lowStat,m_doIBL);
   }
   
   if(sc.isFailure())if(msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "histograms not booked" << endreq;   
   return StatusCode::SUCCESS;
}
 
StatusCode PixelMainMon::FillClustersMon(void)
{
   sc = evtStore()->retrieve(m_Pixel_clcontainer, m_Pixel_SiClustersName);
   if (sc.isFailure()  || !m_Pixel_clcontainer)
   {
      if(msgLvl(MSG::INFO)) msg(MSG::INFO)  <<"Pixel Cluster container for Pixels not found"<< endreq;
      m_storegate_errors->Fill(3.,2.);  
      return StatusCode::SUCCESS;
   } else {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  <<"Pixel Cluster container for Pixels found" <<endreq;
   }

   int nclusters=0;
   int nclusters_ECA=0;
   int nclusters_ECC=0;
   int nclusters_IBL=0;
   int nclusters_B0=0;
   int nclusters_B1=0;
   int nclusters_B2=0;
   int nlargeclusters=0;
   int nverylargeclusters=0;
   int nclusters_all=0;
   int nclusters_ontrack=0;

   if (m_doModules)
     {
       if(m_doIBL){
	 for(int i=0;i<20;i++)
	   {
	     for(int j=0;j<14;j++) m_ClusPerEventArray_lI[j][i]=0;
	   }
       }
       for(int i=0;i<13;i++)
	 {
	   for(int j=0;j<22;j++) m_ClusPerEventArray_l0[j][i]=0;
	   for(int j=0;j<38;j++) m_ClusPerEventArray_l1[j][i]=0;
	   for(int j=0;j<52;j++) m_ClusPerEventArray_l2[j][i]=0;
	 }
       for(int j=0;j<3;j++)
	 {
	   for(int i=0;i<48;i++)
	     {
	       m_ClusPerEventArray_disksA[i][j]=0;
	       m_ClusPerEventArray_disksC[i][j]=0;
	     }
	 }
     }//end doModules

   Identifier clusID;
   InDet::PixelClusterContainer::const_iterator colNext   = m_Pixel_clcontainer->begin();
   InDet::PixelClusterContainer::const_iterator lastCol   = m_Pixel_clcontainer->end();
   DataVector<PixelCluster>::const_iterator p_clus;
   //  
   for (; colNext != lastCol; ++colNext) 
   {
      const InDet::PixelClusterCollection* ClusterCollection(*colNext);
      if (!ClusterCollection) 
      {
         m_storegate_errors->Fill(3.,4.);  //first entry (1). is for RDO, second (4) is for data problem
         continue;
      }
      for(p_clus=ClusterCollection->begin(); p_clus!=ClusterCollection->end(); ++p_clus)
      {
         clusID=(*p_clus)->identify();

	 const PixelCluster& cluster = **p_clus;

	 const InDet::SiWidth clusWidth = cluster.width();

	 nclusters_all++; // count all clusters for ontrack

         if(m_doOnTrack || m_doOnPixelTrack) if(!OnTrack(clusID,true) ){
	     continue; //if we only want hits on track, and the hit is NOT on the track, skip filling.  true means doing clusters, false means rdos
	   }
	 nclusters_ontrack++;

         /////////////Start main fill block here///////////////
         //be sure to check each histo exists before filling it
         if(m_cluster_LVL1A)m_cluster_LVL1A->Fill(cluster.LVL1A());   
	 if(m_cluster_LVL1A_mod) m_cluster_LVL1A_mod->Fill(clusID,m_pixelid,cluster.LVL1A()+0.00001,m_doIBL); //avoid filling exactly zero to distinguish from disabled modules
	 if(cluster.rdoList().size()>1 && m_clus_LVL1A_sizenot1) m_clus_LVL1A_sizenot1->Fill(clusID,m_pixelid,cluster.LVL1A()+0.00001,m_doIBL); //avoid filling exactly zero to distinguish from disabled modules
	 if(m_cluster_ToT_PIX && !(m_pixelid->barrel_ec(clusID)==0 && m_pixelid->layer_disk(clusID)==0 && m_doIBL))m_cluster_ToT_PIX->Fill(cluster.totalToT());     
	 if(m_cluster_ToT_ECA && m_pixelid->barrel_ec(clusID)==2 )m_cluster_ToT_ECA->Fill(cluster.totalToT());   
         if(m_cluster_ToT_ECC && m_pixelid->barrel_ec(clusID)==-2)m_cluster_ToT_ECC->Fill(cluster.totalToT());      
	 if (m_pixelid->barrel_ec(clusID)==0) {
	   if(m_doIBL && m_cluster_ToT_IBL2D && m_pixelid->layer_disk(clusID)==0 && m_pixelid->eta_module(clusID)<6 && m_pixelid->eta_module(clusID)>-7) m_cluster_ToT_IBL2D->Fill(cluster.totalToT());
           if(m_doIBL && m_cluster_ToT_IBL3D && m_pixelid->layer_disk(clusID)==0 && !(m_pixelid->eta_module(clusID)<6 && m_pixelid->eta_module(clusID)>-7)) m_cluster_ToT_IBL3D->Fill(cluster.totalToT());
	   if(m_doIBL && m_cluster_ToT_IBL && m_pixelid->layer_disk(clusID)==0) m_cluster_ToT_IBL->Fill(cluster.totalToT());
	   if(m_cluster_ToT_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL )m_cluster_ToT_B0->Fill(cluster.totalToT());     
	   if(m_cluster_ToT_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL )m_cluster_ToT_B1->Fill(cluster.totalToT()); 
	   if(m_cluster_ToT_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL )m_cluster_ToT_B2->Fill(cluster.totalToT()); 
	 }
	 if(cluster.rdoList().size()>3){
	   if(m_bigcluster_ToT_ECA && m_pixelid->barrel_ec(clusID)==2 )m_bigcluster_ToT_ECA->Fill(cluster.totalToT());
	   if(m_bigcluster_ToT_ECC && m_pixelid->barrel_ec(clusID)==-2)m_bigcluster_ToT_ECC->Fill(cluster.totalToT());
	   if (m_pixelid->barrel_ec(clusID)==0) {
	     if(m_bigcluster_ToT_IBL && m_pixelid->layer_disk(clusID)==0 &&m_doIBL)m_bigcluster_ToT_IBL->Fill(cluster.totalToT());
	     if(m_bigcluster_ToT_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL )m_bigcluster_ToT_B0->Fill(cluster.totalToT());
	     if(m_bigcluster_ToT_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL )m_bigcluster_ToT_B1->Fill(cluster.totalToT());
	     if(m_bigcluster_ToT_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL )m_bigcluster_ToT_B2->Fill(cluster.totalToT());
	   }
	 }
	 if(cluster.rdoList().size()==1){
           if(m_1cluster_ToT_ECA && m_pixelid->barrel_ec(clusID)==2 )m_1cluster_ToT_ECA->Fill(cluster.totalToT());
           if(m_1cluster_ToT_ECC && m_pixelid->barrel_ec(clusID)==-2)m_1cluster_ToT_ECC->Fill(cluster.totalToT());
           if (m_pixelid->barrel_ec(clusID)==0) {
             if(m_1cluster_ToT_IBL && m_pixelid->layer_disk(clusID)==0 &&m_doIBL)m_1cluster_ToT_IBL->Fill(cluster.totalToT());
             if(m_1cluster_ToT_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL )m_1cluster_ToT_B0->Fill(cluster.totalToT());
             if(m_1cluster_ToT_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL )m_1cluster_ToT_B1->Fill(cluster.totalToT());
             if(m_1cluster_ToT_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL )m_1cluster_ToT_B2->Fill(cluster.totalToT());
           }
         }
	 if(cluster.rdoList().size()==2){
           if(m_2cluster_ToT_ECA && m_pixelid->barrel_ec(clusID)==2 )m_2cluster_ToT_ECA->Fill(cluster.totalToT());
           if(m_2cluster_ToT_ECC && m_pixelid->barrel_ec(clusID)==-2)m_2cluster_ToT_ECC->Fill(cluster.totalToT());
           if (m_pixelid->barrel_ec(clusID)==0) {
             if(m_2cluster_ToT_IBL && m_pixelid->layer_disk(clusID)==0 &&m_doIBL)m_2cluster_ToT_IBL->Fill(cluster.totalToT());
             if(m_2cluster_ToT_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL )m_2cluster_ToT_B0->Fill(cluster.totalToT());
             if(m_2cluster_ToT_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL )m_2cluster_ToT_B1->Fill(cluster.totalToT());
             if(m_2cluster_ToT_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL )m_2cluster_ToT_B2->Fill(cluster.totalToT());
           }
         }
	 if(cluster.rdoList().size()==3){
           if(m_3cluster_ToT_ECA && m_pixelid->barrel_ec(clusID)==2 )m_3cluster_ToT_ECA->Fill(cluster.totalToT());
           if(m_3cluster_ToT_ECC && m_pixelid->barrel_ec(clusID)==-2)m_3cluster_ToT_ECC->Fill(cluster.totalToT());
           if (m_pixelid->barrel_ec(clusID)==0) {
             if(m_3cluster_ToT_IBL && m_pixelid->layer_disk(clusID)==0 &&m_doIBL)m_3cluster_ToT_IBL->Fill(cluster.totalToT());
             if(m_3cluster_ToT_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL )m_3cluster_ToT_B0->Fill(cluster.totalToT());
             if(m_3cluster_ToT_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL )m_3cluster_ToT_B1->Fill(cluster.totalToT());
             if(m_3cluster_ToT_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL )m_3cluster_ToT_B2->Fill(cluster.totalToT());
           }
         }
	 
	 /////////////////
	 if(cluster.rdoList().size()>3){
	   if(m_bigcluster_Q_ECA && m_pixelid->barrel_ec(clusID)==2 )m_bigcluster_Q_ECA->Fill(cluster.totalCharge());
	   if(m_bigcluster_Q_ECC && m_pixelid->barrel_ec(clusID)==-2)m_bigcluster_Q_ECC->Fill(cluster.totalCharge());
	   if (m_pixelid->barrel_ec(clusID)==0) {
	     if(m_bigcluster_Q_IBL && m_pixelid->layer_disk(clusID)==0 &&m_doIBL)m_bigcluster_Q_IBL->Fill(cluster.totalCharge());
	     if(m_bigcluster_Q_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL )m_bigcluster_Q_B0->Fill(cluster.totalCharge());
	     if(m_bigcluster_Q_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL )m_bigcluster_Q_B1->Fill(cluster.totalCharge());
	     if(m_bigcluster_Q_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL )m_bigcluster_Q_B2->Fill(cluster.totalCharge());
	   }
	 }
	 
	 if(cluster.rdoList().size()==1){
           if(m_1cluster_Q_ECA && m_pixelid->barrel_ec(clusID)==2 )m_1cluster_Q_ECA->Fill(cluster.totalCharge());
	   if(m_1cluster_Q_ECC && m_pixelid->barrel_ec(clusID)==-2)m_1cluster_Q_ECC->Fill(cluster.totalCharge());
	   if (m_pixelid->barrel_ec(clusID)==0) {
             if(m_1cluster_Q_IBL && m_pixelid->layer_disk(clusID)==0 &&m_doIBL)m_1cluster_Q_IBL->Fill(cluster.totalCharge());
             if(m_1cluster_Q_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL )m_1cluster_Q_B0->Fill(cluster.totalCharge());
             if(m_1cluster_Q_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL )m_1cluster_Q_B1->Fill(cluster.totalCharge());
             if(m_1cluster_Q_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL )m_1cluster_Q_B2->Fill(cluster.totalCharge());
           }
         }
	 
	 if(cluster.rdoList().size()==2){
	   if(m_2cluster_Q_ECA && m_pixelid->barrel_ec(clusID)==2 )m_2cluster_Q_ECA->Fill(cluster.totalCharge());
           if(m_2cluster_Q_ECC && m_pixelid->barrel_ec(clusID)==-2)m_2cluster_Q_ECC->Fill(cluster.totalCharge());
           if (m_pixelid->barrel_ec(clusID)==0) {
             if(m_2cluster_Q_IBL && m_pixelid->layer_disk(clusID)==0 &&m_doIBL)m_2cluster_Q_IBL->Fill(cluster.totalCharge());
             if(m_2cluster_Q_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL )m_2cluster_Q_B0->Fill(cluster.totalCharge());
             if(m_2cluster_Q_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL )m_2cluster_Q_B1->Fill(cluster.totalCharge());
             if(m_2cluster_Q_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL )m_2cluster_Q_B2->Fill(cluster.totalCharge());
           }
         }
	 
	 if(cluster.rdoList().size()==3){
	   if(m_3cluster_Q_ECA && m_pixelid->barrel_ec(clusID)==2 )m_3cluster_Q_ECA->Fill(cluster.totalCharge());
           if(m_3cluster_Q_ECC && m_pixelid->barrel_ec(clusID)==-2)m_3cluster_Q_ECC->Fill(cluster.totalCharge());
           if (m_pixelid->barrel_ec(clusID)==0) {
             if(m_3cluster_Q_IBL && m_pixelid->layer_disk(clusID)==0 &&m_doIBL)m_3cluster_Q_IBL->Fill(cluster.totalCharge());
             if(m_3cluster_Q_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL )m_3cluster_Q_B0->Fill(cluster.totalCharge());
             if(m_3cluster_Q_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL )m_3cluster_Q_B1->Fill(cluster.totalCharge());
             if(m_3cluster_Q_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL )m_3cluster_Q_B2->Fill(cluster.totalCharge());
           }
         }
	 	 
	 if(cluster.totalToT() > 15)
	 { 
	   //Cluster LVL1A histograms for ToT > 15 (corresponding to charge > 10000 electrons)
	   if(m_cluster_LVL1A_highToT)m_cluster_LVL1A_highToT->Fill(cluster.LVL1A()); 
           if(m_cluster_LVL1A_PIX && !(m_pixelid->barrel_ec(clusID)==0 && m_pixelid->layer_disk(clusID)==0 && m_doIBL))m_cluster_LVL1A_PIX->Fill(cluster.LVL1A());	     

           if(m_cluster_LVL1A_ECA && m_pixelid->barrel_ec(clusID)==2 )m_cluster_LVL1A_ECA->Fill(cluster.LVL1A());	     
	   if(m_cluster_LVL1A_ECC && m_pixelid->barrel_ec(clusID)==-2)m_cluster_LVL1A_ECC->Fill(cluster.LVL1A());
	   if (m_pixelid->barrel_ec(clusID)==0) 
	     {
	       if(m_cluster_LVL1A_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL )m_cluster_LVL1A_B0->Fill(cluster.LVL1A());
	       if(m_cluster_LVL1A_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL )m_cluster_LVL1A_B1->Fill(cluster.LVL1A());
	       if(m_cluster_LVL1A_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL )m_cluster_LVL1A_B2->Fill(cluster.LVL1A());
	     }
	 }
	 if(cluster.totalToT() > 4 && m_cluster_LVL1A_IBL && m_pixelid->layer_disk(clusID)==0 && m_pixelid->barrel_ec(clusID)==0 &&m_doIBL)m_cluster_LVL1A_IBL->Fill(cluster.LVL1A());

         if(m_cluster_ToT_mod) m_cluster_ToT_mod->Fill(cluster.totalToT(),clusID,m_pixelid,m_doIBL);   
	 if(m_cluseff_mod && !m_doOnTrack){
           if(OnTrack(clusID,true)){
	     m_cluseff_mod->Fill(m_lumiBlockNum,1.,clusID,m_pixelid,m_doIBL);
           }
           else{
             m_cluseff_mod->Fill(m_lumiBlockNum,0.,clusID,m_pixelid,m_doIBL);
           }
         }

	 if(m_cluster_charge_ECA && m_pixelid->barrel_ec(clusID)==2 )m_cluster_charge_ECA->Fill(cluster.totalCharge());
	 if(m_cluster_charge_ECC && m_pixelid->barrel_ec(clusID)==-2)m_cluster_charge_ECC->Fill(cluster.totalCharge());
	 if (m_pixelid->barrel_ec(clusID)==0) {
	   if(m_cluster_charge_IBL && m_pixelid->layer_disk(clusID)==0 &&m_doIBL)m_cluster_charge_IBL->Fill(cluster.totalCharge());
	   if(m_cluster_charge_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL )m_cluster_charge_B0->Fill(cluster.totalCharge());
	   if(m_cluster_charge_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL )m_cluster_charge_B1->Fill(cluster.totalCharge());
	   if(m_cluster_charge_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL )m_cluster_charge_B2->Fill(cluster.totalCharge());
	 }

         if(m_cluster_groupsize)m_cluster_groupsize->Fill(cluster.rdoList().size());  

         if(m_cluster_col_width)m_cluster_col_width->Fill(clusWidth.colRow().y()); //note: description is wrong in SiWidth! <x,y> = <row,col>                        
         if(m_cluster_row_width)m_cluster_row_width->Fill(clusWidth.colRow().x());                         
         if(m_clusterSize_eta && m_pixelid->barrel_ec(clusID)==0 )m_clusterSize_eta->Fill(m_pixelid->eta_module(clusID),cluster.rdoList().size());    
         if(m_cluster_occupancy)m_cluster_occupancy->Fill(clusID,m_pixelid,m_doIBL);
         if(m_average_cluster_occupancy)m_average_cluster_occupancy->Fill(clusID,m_pixelid,m_doIBL);
	 if(cluster.rdoList().size()>1 && m_clusocc_sizenot1)m_clusocc_sizenot1->Fill(clusID,m_pixelid,m_doIBL); 

	 if(m_clussize_map)m_clussize_map->Fill(clusID,m_pixelid,cluster.rdoList().size(),m_doIBL);
	 if(m_cluscharge_map)m_cluscharge_map->Fill(clusID,m_pixelid,cluster.totalCharge(),m_doIBL);
	 if(m_clusToT_map)m_clusToT_map->Fill(clusID,m_pixelid,cluster.totalToT(),m_doIBL);

	 if(m_cluster_size_mod)m_cluster_size_mod->Fill(cluster.rdoList().size(),clusID,m_pixelid,m_doIBL);  

	 if(m_totalclusters_per_lumi)m_totalclusters_per_lumi->Fill(m_lumiBlockNum); 

	 if(m_totalclusters_per_lumi_ECA && m_pixelid->barrel_ec(clusID)==2 )m_totalclusters_per_lumi_ECA->Fill(m_lumiBlockNum); 
	 if(m_totalclusters_per_lumi_ECC && m_pixelid->barrel_ec(clusID)==-2)m_totalclusters_per_lumi_ECC->Fill(m_lumiBlockNum); 
	 if (m_pixelid->barrel_ec(clusID)==0) {
	   if(m_totalclusters_per_lumi_IBL && m_pixelid->layer_disk(clusID)==0 && m_doIBL)m_totalclusters_per_lumi_IBL->Fill(m_lumiBlockNum);
	   if(m_totalclusters_per_lumi_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL )m_totalclusters_per_lumi_B0->Fill(m_lumiBlockNum); 
	   if(m_totalclusters_per_lumi_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL )m_totalclusters_per_lumi_B1->Fill(m_lumiBlockNum); 
	   if(m_totalclusters_per_lumi_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL )m_totalclusters_per_lumi_B2->Fill(m_lumiBlockNum); 
	 }
	 
	 float nlowToT=0;
	 for(unsigned int i=0;i<cluster.totList().size();i++){
	   if(cluster.totList().at(i)<8){nlowToT++;}
	 }
	 
	 if (m_pixelid->barrel_ec(clusID)==0) {	   
	   if(m_cluster_groupsize_IBL && m_pixelid->layer_disk(clusID)==0 && m_doIBL)m_cluster_groupsize_IBL->Fill(cluster.rdoList().size());
           if(m_cluster_groupsize_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL )m_cluster_groupsize_B0->Fill(cluster.rdoList().size());
           if(m_cluster_groupsize_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL )m_cluster_groupsize_B1->Fill(cluster.rdoList().size());
           if(m_cluster_groupsize_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL )m_cluster_groupsize_B2->Fill(cluster.rdoList().size());
	   if(m_cluster_col_width_IBL && m_pixelid->layer_disk(clusID)==0 && m_doIBL)m_cluster_col_width_IBL->Fill(clusWidth.colRow().y());
           if(m_cluster_col_width_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL )m_cluster_col_width_B0->Fill(clusWidth.colRow().y());
           if(m_cluster_col_width_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL )m_cluster_col_width_B1->Fill(clusWidth.colRow().y());
           if(m_cluster_col_width_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL )m_cluster_col_width_B2->Fill(clusWidth.colRow().y());
	   if(m_cluster_row_width_IBL && m_pixelid->layer_disk(clusID)==0 && m_doIBL)m_cluster_row_width_IBL->Fill(clusWidth.colRow().x());
           if(m_cluster_row_width_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL )m_cluster_row_width_B0->Fill(clusWidth.colRow().x());
           if(m_cluster_row_width_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL )m_cluster_row_width_B1->Fill(clusWidth.colRow().x());
           if(m_cluster_row_width_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL )m_cluster_row_width_B2->Fill(clusWidth.colRow().x());

	   if(m_clusToT_vs_eta_IBL && m_pixelid->layer_disk(clusID)==0 && m_doIBL)m_clusToT_vs_eta_IBL->Fill(m_pixelid->eta_module(clusID) ,cluster.totalToT());
           if(m_clusToT_vs_eta_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL )m_clusToT_vs_eta_B0->Fill(m_pixelid->eta_module(clusID) ,cluster.totalToT());
           if(m_clusToT_vs_eta_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL )m_clusToT_vs_eta_B1->Fill(m_pixelid->eta_module(clusID) ,cluster.totalToT());
           if(m_clusToT_vs_eta_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL )m_clusToT_vs_eta_B2->Fill(m_pixelid->eta_module(clusID) ,cluster.totalToT());
	   if(m_ToT_vs_clussize_IBL && m_pixelid->layer_disk(clusID)==0 && m_doIBL)m_ToT_vs_clussize_IBL->Fill(cluster.rdoList().size() ,cluster.totalToT());
           if(m_ToT_vs_clussize_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL )m_ToT_vs_clussize_B0->Fill(cluster.rdoList().size() ,cluster.totalToT());
           if(m_ToT_vs_clussize_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL )m_ToT_vs_clussize_B1->Fill(cluster.rdoList().size() ,cluster.totalToT());
           if(m_ToT_vs_clussize_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL )m_ToT_vs_clussize_B2->Fill(cluster.rdoList().size() ,cluster.totalToT());
	   if(m_nlowToT_vs_clussize_IBL && m_pixelid->layer_disk(clusID)==0 && m_doIBL)m_nlowToT_vs_clussize_IBL->Fill(cluster.rdoList().size(),nlowToT);
           if(m_nlowToT_vs_clussize_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL )m_nlowToT_vs_clussize_B0->Fill(cluster.rdoList().size(),nlowToT);
           if(m_nlowToT_vs_clussize_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL )m_nlowToT_vs_clussize_B1->Fill(cluster.rdoList().size(),nlowToT);
           if(m_nlowToT_vs_clussize_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL )m_nlowToT_vs_clussize_B2->Fill(cluster.rdoList().size(),nlowToT);
	   if(m_clussize_vs_eta_IBL && m_pixelid->layer_disk(clusID)==0 && m_doIBL)m_clussize_vs_eta_IBL->Fill(m_pixelid->eta_module(clusID) ,cluster.rdoList().size());
           if(m_clussize_vs_eta_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL )m_clussize_vs_eta_B0->Fill(m_pixelid->eta_module(clusID) ,cluster.rdoList().size());
           if(m_clussize_vs_eta_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL )m_clussize_vs_eta_B1->Fill(m_pixelid->eta_module(clusID) ,cluster.rdoList().size());
           if(m_clussize_vs_eta_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL )m_clussize_vs_eta_B2->Fill(m_pixelid->eta_module(clusID) ,cluster.rdoList().size());

           if(m_clusQ_vs_eta_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL )m_clusQ_vs_eta_B0->Fill(m_pixelid->eta_module(clusID) ,cluster.totalCharge());
           if(m_clusQ_vs_eta_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL )m_clusQ_vs_eta_B1->Fill(m_pixelid->eta_module(clusID) ,cluster.totalCharge());
           if(m_clusQ_vs_eta_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL )m_clusQ_vs_eta_B2->Fill(m_pixelid->eta_module(clusID) ,cluster.totalCharge());
           if(m_Q_vs_clussize_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL )m_Q_vs_clussize_B0->Fill(cluster.rdoList().size() ,cluster.totalCharge());
           if(m_Q_vs_clussize_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL )m_Q_vs_clussize_B1->Fill(cluster.rdoList().size() ,cluster.totalCharge());
           if(m_Q_vs_clussize_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL )m_Q_vs_clussize_B2->Fill(cluster.rdoList().size() ,cluster.totalCharge());
         }
	 
	 if (m_pixelid->barrel_ec(clusID)==2) {
           if(m_clussize_vs_eta_ECA)m_clussize_vs_eta_ECA->Fill(m_pixelid->eta_module(clusID) ,cluster.rdoList().size());
	   if(m_clusQ_vs_eta_ECA)m_clusQ_vs_eta_ECA->Fill(m_pixelid->eta_module(clusID) ,cluster.totalCharge());	  
           if(m_Q_vs_clussize_ECA)m_Q_vs_clussize_ECA->Fill(cluster.rdoList().size() ,cluster.totalCharge());
	   if(m_nlowToT_vs_clussize_ECA)m_nlowToT_vs_clussize_ECA->Fill(cluster.rdoList().size(),nlowToT);
           if(m_cluster_groupsize_ECA)m_cluster_groupsize_ECA->Fill(cluster.rdoList().size());
	   if(m_cluster_col_width_ECA)m_cluster_col_width_ECA->Fill(clusWidth.colRow().y());
	   if(m_cluster_row_width_ECA)m_cluster_row_width_ECA->Fill(clusWidth.colRow().x());
	 }
	 if (m_pixelid->barrel_ec(clusID)==-2) {
           if(m_clussize_vs_eta_ECC)m_clussize_vs_eta_ECC->Fill(m_pixelid->eta_module(clusID) ,cluster.rdoList().size());
           if(m_clusQ_vs_eta_ECC)m_clusQ_vs_eta_ECC->Fill(m_pixelid->eta_module(clusID) ,cluster.totalCharge());
           if(m_Q_vs_clussize_ECC)m_Q_vs_clussize_ECC->Fill(cluster.rdoList().size() ,cluster.totalCharge());
	   if(m_nlowToT_vs_clussize_ECC)m_nlowToT_vs_clussize_ECC->Fill(cluster.rdoList().size(),nlowToT);
           if(m_cluster_groupsize_ECC)m_cluster_groupsize_ECC->Fill(cluster.rdoList().size());
	   if(m_cluster_col_width_ECC)m_cluster_col_width_ECC->Fill(clusWidth.colRow().y());
	   if(m_cluster_row_width_ECC)m_cluster_row_width_ECC->Fill(clusWidth.colRow().x());
	 }

         nclusters++;

	 if(cluster.rdoList().size() > 10) nlargeclusters++;
	 if(cluster.rdoList().size() > 50) nverylargeclusters++;
	 if(m_pixelid->barrel_ec(clusID)==2 ) nclusters_ECA++; 
	 if(m_pixelid->barrel_ec(clusID)==-2) nclusters_ECC++; 
	 if(m_pixelid->barrel_ec(clusID)==0) {
	   if(m_doIBL && m_pixelid->layer_disk(clusID)==0) nclusters_IBL++;
	   if(m_pixelid->layer_disk(clusID)==0+m_doIBL) nclusters_B0++;
	   if(m_pixelid->layer_disk(clusID)==1+m_doIBL) nclusters_B1++;
	   if(m_pixelid->layer_disk(clusID)==2+m_doIBL) nclusters_B2++; 
	 }

         if (m_doModules)//fill module cluster arrays 
         {
            if(m_pixelid->barrel_ec(clusID)==2 ) m_ClusPerEventArray_disksA[m_pixelid->phi_module(clusID)][m_pixelid->layer_disk(clusID)]++;
            if(m_pixelid->barrel_ec(clusID)==-2) m_ClusPerEventArray_disksC[m_pixelid->phi_module(clusID)][m_pixelid->layer_disk(clusID)]++;
            if(m_pixelid->barrel_ec(clusID)==0 )
            {
   	       if(m_doIBL && m_pixelid->layer_disk(clusID)==0) m_ClusPerEventArray_lI[m_pixelid->phi_module(clusID)][m_pixelid->eta_module(clusID)+10]++;
               if(m_pixelid->layer_disk(clusID)==0+m_doIBL) m_ClusPerEventArray_l0[m_pixelid->phi_module(clusID)][m_pixelid->eta_module(clusID)+6]++;
               if(m_pixelid->layer_disk(clusID)==1+m_doIBL) m_ClusPerEventArray_l1[m_pixelid->phi_module(clusID)][m_pixelid->eta_module(clusID)+6]++;
               if(m_pixelid->layer_disk(clusID)==2+m_doIBL) m_ClusPerEventArray_l2[m_pixelid->phi_module(clusID)][m_pixelid->eta_module(clusID)+6]++;
            }  
         }

	 if (m_doLumiBlock) {
	   if (m_cluster_occupancy_LB) m_cluster_occupancy_LB->Fill(clusID,m_pixelid,m_doIBL);
	   if (m_cluster_ToT_LB)m_cluster_ToT_LB->Fill(cluster.totalToT());     
	   if (m_cluster_ToT_mod_LB) m_cluster_ToT_mod_LB->Fill(cluster.totalToT(),clusID,m_pixelid,m_doIBL);   
	 }
	 
         ///////////End of main fill block////////////////////
	 //	 }
      }
   }//end of event loop
   
   ////////////////////Fill after event block////////////////
   if(m_doOnline)//should we fill these mid run or only at the end?
   {
     FillSummaryHistos(m_cluster_occupancy,m_ecA_cluster_occupancy_summary,m_ecC_cluster_occupancy_summary,m_bar_layI_cluster_occupancy_summary,m_bar_lay0_cluster_occupancy_summary,m_bar_lay1_cluster_occupancy_summary,m_bar_lay2_cluster_occupancy_summary);
     FillSummaryHistos(m_cluster_occupancy,m_ecA_cluster_occupancy_summary_low,m_ecC_cluster_occupancy_summary_low,m_bar_layI_cluster_occupancy_summary_low,m_bar_lay0_cluster_occupancy_summary_low,m_bar_lay1_cluster_occupancy_summary_low,m_bar_lay2_cluster_occupancy_summary_low);
   }                              

   if(m_clusters_per_lumi)m_clusters_per_lumi->Fill(m_lumiBlockNum,nclusters); 
   if(m_clusters_per_lumi_PIX)m_clusters_per_lumi_PIX->Fill(m_lumiBlockNum,nclusters-nclusters_IBL); 
   if(m_clusters_per_lumi_ECA)m_clusters_per_lumi_ECA->Fill(m_lumiBlockNum,nclusters_ECA); 
   if(m_clusters_per_lumi_ECC)m_clusters_per_lumi_ECC->Fill(m_lumiBlockNum,nclusters_ECC); 
   if(m_clusters_per_lumi_IBL )m_clusters_per_lumi_IBL->Fill(m_lumiBlockNum,nclusters_IBL);
   if(m_clusters_per_lumi_B0 )m_clusters_per_lumi_B0->Fill(m_lumiBlockNum,nclusters_B0); 
   if(m_clusters_per_lumi_B1 )m_clusters_per_lumi_B1->Fill(m_lumiBlockNum,nclusters_B1); 
   if(m_clusters_per_lumi_B2 )m_clusters_per_lumi_B2->Fill(m_lumiBlockNum,nclusters_B2);  

   if(m_largeclusters_per_lumi)m_largeclusters_per_lumi->Fill(m_lumiBlockNum,nlargeclusters); 
   if(m_verylargeclusters_per_lumi)m_verylargeclusters_per_lumi->Fill(m_lumiBlockNum,nverylargeclusters); 
   if( (nclusters>=1000) && m_highNclusters_per_lumi) m_highNclusters_per_lumi->Fill(m_lumiBlockNum); 
   if (m_doOnTrack || m_doOnPixelTrack) {
     if(m_clustersOnOffTrack_per_lumi && nclusters_all>0) m_clustersOnOffTrack_per_lumi->Fill(m_lumiBlockNum,(float)nclusters_ontrack/nclusters_all); 
   }

   //Fill some histograms only if =< 50% of modules disabled
   if (!m_majorityDisabled) {
     
     if(m_num_clusters) m_num_clusters->Fill(nclusters);
     if(m_num_clusters_PIX) m_num_clusters_PIX->Fill(nclusters-nclusters_IBL);
     if(m_num_clusters_low) m_num_clusters_low->Fill(nclusters);
     if(m_num_clusters_ECA) m_num_clusters_ECA->Fill(nclusters_ECA);
     if(m_num_clusters_ECC) m_num_clusters_ECC->Fill(nclusters_ECC);
     if(m_num_clusters_IBL)  m_num_clusters_IBL->Fill(nclusters_IBL);
     if(m_num_clusters_B0)  m_num_clusters_B0->Fill(nclusters_B0);
     if(m_num_clusters_B1)  m_num_clusters_B1->Fill(nclusters_B1);
     if(m_num_clusters_B2)  m_num_clusters_B2->Fill(nclusters_B2);

     if(m_cluster_occupancy_time1&&m_cluster_occupancy_time2&&m_cluster_occupancy_time3) FillTimeHisto(double(nclusters/(1744.0+224*m_doIBL)),m_cluster_occupancy_time1, m_cluster_occupancy_time2, m_cluster_occupancy_time3,10.,60.,360. );
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
   if(nclusters==0) m_storegate_errors->Fill(3.,3.);//first entry for RDO, second for size = 0

   return StatusCode::SUCCESS;
}


StatusCode PixelMainMon::ProcClustersMon(void)
{
   if(m_doOffline) //run these only for offline
   {
     FillSummaryHistos(m_cluster_occupancy,m_ecA_cluster_occupancy_summary,m_ecC_cluster_occupancy_summary,m_bar_layI_cluster_occupancy_summary,m_bar_lay0_cluster_occupancy_summary,m_bar_lay1_cluster_occupancy_summary,m_bar_lay2_cluster_occupancy_summary);
     FillSummaryHistos(m_cluster_occupancy,m_ecA_cluster_occupancy_summary_low,m_ecC_cluster_occupancy_summary_low,m_bar_layI_cluster_occupancy_summary_low,m_bar_lay0_cluster_occupancy_summary_low,m_bar_lay1_cluster_occupancy_summary_low,m_bar_lay2_cluster_occupancy_summary_low);
   }
   
   double events = m_event;
   if(events==0) return StatusCode::SUCCESS; //if no events, the rest of the test is pointless and would divide by 0

   if (m_average_cluster_occupancy) {  //scale to get average occupancies
     m_average_cluster_occupancy->Scale(events);  
   }

   return StatusCode::SUCCESS;
}

