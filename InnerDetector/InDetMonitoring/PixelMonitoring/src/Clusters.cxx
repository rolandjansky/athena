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
#include "PixelMonitoring/DBMMon2DMaps.h"
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

   std::string addOnTrack = "_OnTrack";

   int hxbin = 0; double hxmin = 0.; double hxmax = 0.;
   //int hybin = 0; double hymin = 0.; double hymax = 0.;
   int nbins_LB = 2500; double min_LB   = -0.5; double max_LB = min_LB + (1.0*nbins_LB);        
   int nbins_npix = 300; double min_npix = -0.5; double max_npix = min_npix + (1.0*nbins_npix);
   int nbins_nwid = 50; double min_nwid = -0.5; double max_nwid = min_nwid + (1.0*nbins_nwid);
   int nbins_eta = 13; double min_eta = -6.5;  double max_eta = min_eta + (1.0*nbins_eta);
   int nbins_tot = 300; double min_tot = -0.5; double max_tot = min_tot + (1.0*nbins_tot);
   std::string atext_LB = ";lumi block"; 
   std::string atext_clu = ";# clusters/event"; 
   std::string atext_npix = ";# pixels/cluster"; 
   std::string atext_nclu = ";# clusters"; 
   std::string atext_cluw = ";cluster width"; 
   std::string atext_eta = ";Module eta index"; 
   std::string atext_tot = ";ToT"; 
   std::string tmp;
   std::string tmp2;
   std::string atitles;
   std::string modlabel[8];
   modlabel[0]="ECA"; modlabel[1]="ECC";
   modlabel[2]="B0";  modlabel[3]="B1";  modlabel[4]="B2"; modlabel[5]="IBL"; modlabel[6]="IBL2D"; modlabel[7]="IBL3D"; 

   tmp = "ClusterSize_v_eta"; tmp2 = "Cluster size as a function of barrel module eta"; atitles = ";Module eta index;cluster width";
   //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
   sc = clusterExpert.regHist(m_clusterSize_eta           = TProfile_LW::create(tmp.c_str(), (tmp2 + m_histTitleExt + atitles).c_str(), nbins_eta, min_eta, max_eta));

   tmp = "LargeClusters_per_lumi"; tmp2 = "Clusters (with >10 pixels) per event per LB"; atitles = ";lumi block;# clusters/event";
   if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
   sc = clusterShift.regHist(m_largeclusters_per_lumi     = TProfile_LW::create(tmp.c_str(), (tmp2 + m_histTitleExt + atitles).c_str(), nbins_LB, min_LB, max_LB));

   tmp = "VeryLargeClusters_per_lumi"; tmp2 = "Clusters (with >50 pixels) per event per LB"; atitles = ";lumi block;# clusters/event";
   if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
   sc = clusterShift.regHist(m_verylargeclusters_per_lumi = TProfile_LW::create(tmp.c_str(), (tmp2 + m_histTitleExt + atitles).c_str(), nbins_LB, min_LB, max_LB));

   tmp = "HighNClusters_per_lumi"; tmp2 = "Rate of events with >1000 clusters/event per LB"; atitles = ";lumi block;# event";
   if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
   sc = clusterExpert.regHist(m_highNclusters_per_lumi    = TH1I_LW::create(tmp.c_str(),     (tmp2 + m_histTitleExt + atitles).c_str(), nbins_LB, min_LB, max_LB));

   tmp = "Clusters_per_lumi"; tmp2 = "Number of pixel clusters per event per LB"; atitles = ";lumi block;# clusters/event";
   if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
   sc = clusterShift.regHist(m_clusters_per_lumi      = TProfile_LW::create(tmp.c_str(),            (tmp2 +                      m_histTitleExt + atitles).c_str(), nbins_LB, min_LB, max_LB));

   tmp = "Clusters_per_lumi"; tmp2 = "Number of pixel clusters per event per LB"; atitles = ";lumi block;# clusters/event";
   sc = clusterShift.regHist(m_clusters_per_lumi_PIX  = TProfile_LW::create((tmp + "_PIX").c_str(), (tmp2 + ", Pixel"          + m_histTitleExt + atitles).c_str(), nbins_LB, min_LB, max_LB));

   tmp = "TotalClusters_per_lumi"; tmp2 = "Total number of pixel clusters per event per LB"; atitles = ";lumi block;# clusters";
   if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
   sc = clusterShift.regHist(m_totalclusters_per_lumi      = TH1I_LW::create(tmp.c_str() ,           (tmp2 +                 m_histTitleExt + atitles).c_str(), nbins_LB, min_LB, max_LB));

   tmp = "Cluster_groupsize"; tmp2 = "Number of pixels per cluster"; atitles = ";# pixels/cluster;# clusters";  
   if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
   sc = clusterExpert.regHist(m_cluster_groupsize     = TH1F_LW::create(tmp.c_str(),            (tmp2 +          m_histTitleExt + atitles).c_str(), nbins_npix, min_npix, max_npix));

   tmp = "Cluster_column_width"; tmp2 = "Column width of cluster"; atitles = ";cluster width;# clusters";
   //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
   sc = clusterExpert.regHist(m_cluster_col_width     = TH1F_LW::create(tmp.c_str(),            (tmp2 +          m_histTitleExt + atitles).c_str(), nbins_nwid, min_nwid, max_nwid)); 

   tmp = "Cluster_row_width"; tmp2 = "Row width of cluster"; atitles = ";cluster width;# clusters";  
   //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
   sc = clusterExpert.regHist(m_cluster_row_width     = TH1F_LW::create(tmp.c_str(),            (tmp2 +          m_histTitleExt + atitles).c_str(), nbins_nwid, min_nwid, max_nwid));

   /// Histograms for each layer
   for(int i=0; i<PixLayer::COUNT-1+(int)(m_doIBL); i++){
         tmp = "Clusters_per_lumi"; tmp2 = "Number of pixel clusters per event per LB"; atitles = ";lumi block;# clusters/event";
         sc = clusterExpert.regHist(m_clusters_per_lumi_mod[i] = TProfile_LW::create((tmp + "_" + modlabel[i]).c_str(), (tmp2 + ", " + modlabel[i] + m_histTitleExt + atitles).c_str(), nbins_LB, min_LB, max_LB));
         tmp = "TotalClusters_per_lumi"; tmp2 = "Total number of pixel clusters per event per LB"; atitles = ";lumi block;# clusters";
         sc = clusterExpert.regHist(m_totalclusters_per_lumi_mod[i] = TH1I_LW::create((tmp + "_" + modlabel[i]).c_str(), (tmp2 + ", " + modlabel[i] + m_histTitleExt + atitles).c_str(), nbins_LB, min_LB, max_LB));
         tmp = "Cluster_groupsize"; tmp2 = "Number of pixels per cluster"; atitles = ";# pixels/cluster;# clusters";
         sc = clusterExpert.regHist(m_cluster_groupsize_mod[i] = TH1F_LW::create((tmp + "_" + modlabel[i]).c_str(), (tmp2 + ", " + modlabel[i] + m_histTitleExt + atitles).c_str(), nbins_npix+1, min_npix, max_npix+1));
         tmp = "Cluster_column_width"; tmp2 = "Column width of cluster"; atitles = ";cluster width;# clusters";
         sc = clusterExpert.regHist(m_cluster_col_width_mod[i] = TH1F_LW::create((tmp + "_" + modlabel[i]).c_str(), (tmp2 + ", " + modlabel[i] + m_histTitleExt + atitles).c_str(), nbins_nwid, min_nwid, max_nwid));
         tmp = "Cluster_row_width"; tmp2 = "Row width of cluster"; atitles = ";cluster width;# clusters";
         sc = clusterExpert.regHist(m_cluster_row_width_mod[i] = TH1F_LW::create((tmp + "_" + modlabel[i]).c_str(), (tmp2 + ", " + modlabel[i] + m_histTitleExt + atext_cluw + atext_nclu).c_str(), nbins_nwid, min_nwid, max_nwid));
   }

   
   if(m_doESD && !m_doOnline){
      hxbin = 200; hxmin = 0.; hxmax = 200000.;  

      tmp = "clusQ_vs_eta"; tmp2 = "Cluster Q vs Eta"; atitles = ";Module eta index;Charge";
      for(int i=0; i<PixLayer::COUNT-1+(int)(m_doIBL); i++){
         sc = clusterExpert.regHist(m_clusQ_vs_eta_mod[i] = TH2F_LW::create((tmp + "_" + modlabel[i]).c_str(), (tmp2 + ", " + modlabel[i] + m_histTitleExt + atitles).c_str(), nbins_eta, min_eta, max_eta, hxbin, hxmin, hxmax));
      }
      tmp = "clustersize_vs_eta"; tmp2 = "Cluster Size vs Eta"; atitles = ";Module eta index;Cluster size";
      for(int i=0; i<PixLayer::COUNT-1+(int)(m_doIBL); i++){
         sc = clusterExpert.regHist(m_clussize_vs_eta_mod[i] = TH2F_LW::create((tmp + "_" + modlabel[i]).c_str(), (tmp2 + ", " + modlabel[i] + m_histTitleExt + atitles).c_str(), nbins_eta, min_eta, max_eta,40,-0.5,39.5));
      }

      //tmp = "m_clusToT_vs_eta"; tmp2 = "Cluster ToT vs Eta";
      tmp = "clusToT_vs_eta"; tmp2 = "Cluster ToT vs Eta";
      //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
      sc = clusterExpert.regHist(m_clusToT_vs_eta_B0  = TH2F_LW::create((tmp + "_B0").c_str(), (tmp2 + ", B0" + m_histTitleExt + atext_eta + atext_tot).c_str(), nbins_eta, min_eta, max_eta, nbins_tot, min_tot, max_tot));
      sc = clusterExpert.regHist(m_clusToT_vs_eta_B1  = TH2F_LW::create((tmp + "_B1").c_str(), (tmp2 + ", B1" + m_histTitleExt + atext_eta + atext_tot).c_str(), nbins_eta, min_eta, max_eta, nbins_tot, min_tot, max_tot));
      sc = clusterExpert.regHist(m_clusToT_vs_eta_B2  = TH2F_LW::create((tmp + "_B2").c_str(), (tmp2 + ", B2" + m_histTitleExt + atext_eta + atext_tot).c_str(), nbins_eta, min_eta, max_eta, nbins_tot, min_tot, max_tot));
      //tmp = "m_clustersize_vs_eta"; tmp2 = "Cluster Size vs Eta";
      //tmp = "m_ToT_vs_clussize"; tmp2 = "Cluster ToT vs Size";
      tmp = "ToT_vs_clussize"; tmp2 = "Cluster ToT vs Size";
      //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
      sc= clusterExpert.regHist(m_ToT_vs_clussize_B0 = TH2F_LW::create((tmp + "_B0").c_str(), (tmp2 + ", B0" + m_histTitleExt + ";Cluster Size" + atext_tot).c_str(),40,-0.5,39.5, nbins_tot, min_tot, max_tot));
      sc= clusterExpert.regHist(m_ToT_vs_clussize_B1 = TH2F_LW::create((tmp + "_B1").c_str(), (tmp2 + ", B1" + m_histTitleExt + ";Cluster Size" + atext_tot).c_str(),40,-0.5,39.5, nbins_tot, min_tot, max_tot));
      sc= clusterExpert.regHist(m_ToT_vs_clussize_B2 = TH2F_LW::create((tmp + "_B2").c_str(), (tmp2 + ", B2" + m_histTitleExt + ";Cluster Size" + atext_tot).c_str(),40,-0.5,39.5, nbins_tot, min_tot, max_tot));

     hxbin = 200; hxmin = 0.; hxmax = 200000.;
     //tmp = "m_Q_vs_clussize"; tmp2 = "Cluster Q vs Size";
      tmp = "Q_vs_clussize"; tmp2 = "Cluster Q vs Size";
      //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
      sc= clusterExpert.regHist(m_Q_vs_clussize_B0  = TH2F_LW::create((tmp + "_B0").c_str(),  (tmp2 + ", B0" + m_histTitleExt + ";Clsuter Size;Charge").c_str(),40,-0.5,39.5,hxbin, hxmin, hxmax));
      sc= clusterExpert.regHist(m_Q_vs_clussize_B1  = TH2F_LW::create((tmp + "_B1").c_str(),  (tmp2 + ", B1" + m_histTitleExt + ";Clsuter Size;Charge").c_str(),40,-0.5,39.5,hxbin, hxmin, hxmax));
      sc= clusterExpert.regHist(m_Q_vs_clussize_B2  = TH2F_LW::create((tmp + "_B2").c_str(),  (tmp2 + ", B2" + m_histTitleExt + ";Clsuter Size;Charge").c_str(),40,-0.5,39.5,hxbin, hxmin, hxmax));
      sc= clusterExpert.regHist(m_Q_vs_clussize_ECC = TH2F_LW::create((tmp + "_ECC").c_str(), (tmp2 + ", ECC"+ m_histTitleExt + ";Clsuter Size;Charge").c_str(),40,-0.5,39.5,hxbin, hxmin, hxmax));
      sc= clusterExpert.regHist(m_Q_vs_clussize_ECA = TH2F_LW::create((tmp + "_ECA").c_str(), (tmp2 + ", ECA"+ m_histTitleExt + ";Clsuter Size;Charge").c_str(),40,-0.5,39.5,hxbin, hxmin, hxmax));
      //tmp = "m_nlowToT_vs_clussize"; tmp2 = "# Low ToT Hits vs Size";
      tmp = "nlowToT_vs_clussize"; tmp2 = "# Low ToT Hits vs Size";
      //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
      sc= clusterExpert.regHist(m_nlowToT_vs_clussize_B0  = TH2F_LW::create((tmp + "_B0").c_str(),  (tmp2 + ", B0" + m_histTitleExt + ";Cluster Size; frac low ToT hits").c_str(),40,-0.5,39.5,40,-0.5,39.5));
      sc= clusterExpert.regHist(m_nlowToT_vs_clussize_B1  = TH2F_LW::create((tmp + "_B1").c_str(),  (tmp2 + ", B1" + m_histTitleExt + ";Cluster Size; frac low ToT hits").c_str(),40,-0.5,39.5,40,-0.5,39.5));
      sc= clusterExpert.regHist(m_nlowToT_vs_clussize_B2  = TH2F_LW::create((tmp + "_B2").c_str(),  (tmp2 + ", B2" + m_histTitleExt + ";Cluster Size; frac low ToT hits").c_str(),40,-0.5,39.5,40,-0.5,39.5));
      sc= clusterExpert.regHist(m_nlowToT_vs_clussize_ECC = TH2F_LW::create((tmp + "_ECC").c_str(), (tmp2 + ", ECC"+ m_histTitleExt + ";Cluster Size; frac low ToT hits").c_str(),40,-0.5,39.5,40,-0.5,39.5));
      sc= clusterExpert.regHist(m_nlowToT_vs_clussize_ECA = TH2F_LW::create((tmp + "_ECA").c_str(), (tmp2 + ", ECA"+ m_histTitleExt + ";Cluster Size; frac low ToT hits").c_str(),40,-0.5,39.5,40,-0.5,39.5));
   }

   if (m_doOnTrack || m_doOnPixelTrack) { 
     sc = clusterExpert.regHist(m_clustersOnOffTrack_per_lumi = TProfile_LW::create("ClustersOnOffTrack_per_lumi",("Fraction pixel clusters on track per event per LB" + m_histTitleExt + ";lumi block; fraction clusters/event").c_str(),2500,-0.5,2499.5));
   }

   tmp = "Cluster_ToT"; tmp2 = "Cluster ToT";
   //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
   sc = clusterShift.regHist(m_cluster_ToT_PIX     = TH1F_LW::create((tmp + "_PIX").c_str(), (tmp2 + ":Pixel" + m_histTitleExt + atext_tot + atext_nclu).c_str(), nbins_tot, min_tot, max_tot));
   sc = clusterExpert.regHist(m_cluster_ToT_ECA    = TH1F_LW::create((tmp + "_ECA").c_str(), (tmp2 + ":ECA"   + m_histTitleExt + atext_tot + atext_nclu).c_str(), nbins_tot, min_tot, max_tot));
   sc = clusterExpert.regHist(m_cluster_ToT_ECC    = TH1F_LW::create((tmp + "_ECC").c_str(), (tmp2 + ":ECC"   + m_histTitleExt + atext_tot + atext_nclu).c_str(), nbins_tot, min_tot, max_tot));
   sc = clusterExpert.regHist(m_cluster_ToT_B0     = TH1F_LW::create((tmp + "_B0").c_str(),  (tmp2 + ":B0"    + m_histTitleExt + atext_tot + atext_nclu).c_str(), nbins_tot, min_tot, max_tot));
   sc = clusterExpert.regHist(m_cluster_ToT_B1     = TH1F_LW::create((tmp + "_B1").c_str(),  (tmp2 + ":B1"    + m_histTitleExt + atext_tot + atext_nclu).c_str(), nbins_tot, min_tot, max_tot));
   sc = clusterExpert.regHist(m_cluster_ToT_B2     = TH1F_LW::create((tmp + "_B2").c_str(),  (tmp2 + ":B2"    + m_histTitleExt + atext_tot + atext_nclu).c_str(), nbins_tot, min_tot, max_tot));
   tmp = "Big_Cluster_ToT"; tmp2 = "ToT for Clusters of size>3";
   //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
   sc = clusterExpert.regHist(m_bigcluster_ToT_B0  = TH1F_LW::create((tmp + "_B0").c_str(),  (tmp2 + ", B0"  + m_histTitleExt + atext_tot + atext_nclu).c_str(), nbins_tot, min_tot, max_tot));
   sc = clusterExpert.regHist(m_bigcluster_ToT_B1  = TH1F_LW::create((tmp + "_B1").c_str(),  (tmp2 + ", B1"  + m_histTitleExt + atext_tot + atext_nclu).c_str(), nbins_tot, min_tot, max_tot));
   sc = clusterExpert.regHist(m_bigcluster_ToT_B2  = TH1F_LW::create((tmp + "_B2").c_str(),  (tmp2 + ", B2"  + m_histTitleExt + atext_tot + atext_nclu).c_str(), nbins_tot, min_tot, max_tot));
   sc = clusterExpert.regHist(m_bigcluster_ToT_ECA = TH1F_LW::create((tmp + "_ECA").c_str(), (tmp2 + ", ECA" + m_histTitleExt + atext_tot + atext_nclu).c_str(), nbins_tot, min_tot, max_tot));
   sc = clusterExpert.regHist(m_bigcluster_ToT_ECC = TH1F_LW::create((tmp + "_ECC").c_str(), (tmp2 + ", ECC" + m_histTitleExt + atext_tot + atext_nclu).c_str(), nbins_tot, min_tot, max_tot));
   tmp = "1Hit_Cluster_ToT"; tmp2 = "ToT for 1-hit clusters";
   //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
   sc = clusterExpert.regHist(m_1cluster_ToT_B0    = TH1F_LW::create((tmp + "_B0").c_str(),  (tmp2 + ", B0"  + m_histTitleExt + atext_tot + atext_nclu).c_str(), nbins_tot, min_tot, max_tot));
   sc = clusterExpert.regHist(m_1cluster_ToT_B1    = TH1F_LW::create((tmp + "_B1").c_str(),  (tmp2 + ", B1"  + m_histTitleExt + atext_tot + atext_nclu).c_str(), nbins_tot, min_tot, max_tot));
   sc = clusterExpert.regHist(m_1cluster_ToT_B2    = TH1F_LW::create((tmp + "_B2").c_str(),  (tmp2 + ", B2"  + m_histTitleExt + atext_tot + atext_nclu).c_str(), nbins_tot, min_tot, max_tot));
   sc = clusterExpert.regHist(m_1cluster_ToT_ECA   = TH1F_LW::create((tmp + "_ECA").c_str(), (tmp2 + ", ECA" + m_histTitleExt + atext_tot + atext_nclu).c_str(), nbins_tot, min_tot, max_tot));
   sc = clusterExpert.regHist(m_1cluster_ToT_ECC   = TH1F_LW::create((tmp + "_ECC").c_str(), (tmp2 + ", ECC" + m_histTitleExt + atext_tot + atext_nclu).c_str(), nbins_tot, min_tot, max_tot));   
   tmp = "2Hit_Cluster_ToT"; tmp2 = "ToT for 2-hit clusters";
   //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
   sc = clusterExpert.regHist(m_2cluster_ToT_B0    = TH1F_LW::create((tmp + "_B0").c_str(),  (tmp2 + ", B0"  + m_histTitleExt + atext_tot + atext_nclu).c_str(), nbins_tot, min_tot, max_tot));
   sc = clusterExpert.regHist(m_2cluster_ToT_B1    = TH1F_LW::create((tmp + "_B1").c_str(),  (tmp2 + ", B1"  + m_histTitleExt + atext_tot + atext_nclu).c_str(), nbins_tot, min_tot, max_tot));
   sc = clusterExpert.regHist(m_2cluster_ToT_B2    = TH1F_LW::create((tmp + "_B2").c_str(),  (tmp2 + ", B2"  + m_histTitleExt + atext_tot + atext_nclu).c_str(), nbins_tot, min_tot, max_tot));
   sc = clusterExpert.regHist(m_2cluster_ToT_ECA   = TH1F_LW::create((tmp + "_ECA").c_str(), (tmp2 + ", ECA" + m_histTitleExt + atext_tot + atext_nclu).c_str(), nbins_tot, min_tot, max_tot));
   sc = clusterExpert.regHist(m_2cluster_ToT_ECC   = TH1F_LW::create((tmp + "_ECC").c_str(), (tmp2 + ", ECC" + m_histTitleExt + atext_tot + atext_nclu).c_str(), nbins_tot, min_tot, max_tot)); 
   tmp = "3Hit_Cluster_ToT"; tmp2 = "ToT for 3-hit clusters";
   //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
   sc = clusterExpert.regHist(m_3cluster_ToT_B0    = TH1F_LW::create((tmp + "_B0").c_str(),  (tmp2 + ", B0"  + m_histTitleExt + atext_tot + atext_nclu).c_str(), nbins_tot, min_tot, max_tot));
   sc = clusterExpert.regHist(m_3cluster_ToT_B1    = TH1F_LW::create((tmp + "_B1").c_str(),  (tmp2 + ", B1"  + m_histTitleExt + atext_tot + atext_nclu).c_str(), nbins_tot, min_tot, max_tot));
   sc = clusterExpert.regHist(m_3cluster_ToT_B2    = TH1F_LW::create((tmp + "_B2").c_str(),  (tmp2 + ", B2"  + m_histTitleExt + atext_tot + atext_nclu).c_str(), nbins_tot, min_tot, max_tot));
   sc = clusterExpert.regHist(m_3cluster_ToT_ECA   = TH1F_LW::create((tmp + "_ECA").c_str(), (tmp2 + ", ECA" + m_histTitleExt + atext_tot + atext_nclu).c_str(), nbins_tot, min_tot, max_tot));
   sc = clusterExpert.regHist(m_3cluster_ToT_ECC   = TH1F_LW::create((tmp + "_ECC").c_str(), (tmp2 + ", ECC" + m_histTitleExt + atext_tot + atext_nclu).c_str(), nbins_tot, min_tot, max_tot)); 
   
   if(m_doESD && !m_doOnline){
      hxbin = 200; hxmin = 0.; hxmax = 200000.;
      tmp = "Big_Cluster_Q"; tmp2 = "Charge for Clusters of size>3";
      //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
      sc = clusterExpert.regHist(m_bigcluster_Q_B0  = TH1F_LW::create((tmp + "_B0").c_str(),  (tmp2 + ", B0"  + m_histTitleExt + ";Charge" + atext_nclu).c_str(), hxbin, hxmin, hxmax));
      sc = clusterExpert.regHist(m_bigcluster_Q_B1  = TH1F_LW::create((tmp + "_B1").c_str(),  (tmp2 + ", B1"  + m_histTitleExt + ";Charge" + atext_nclu).c_str(), hxbin, hxmin, hxmax));
      sc = clusterExpert.regHist(m_bigcluster_Q_B2  = TH1F_LW::create((tmp + "_B2").c_str(),  (tmp2 + ", B2"  + m_histTitleExt + ";Charge" + atext_nclu).c_str(), hxbin, hxmin, hxmax));
      sc = clusterExpert.regHist(m_bigcluster_Q_ECA = TH1F_LW::create((tmp + "_ECA").c_str(), (tmp2 + ", ECA" + m_histTitleExt + ";Charge" + atext_nclu).c_str(), hxbin, hxmin, hxmax));
      sc = clusterExpert.regHist(m_bigcluster_Q_ECC = TH1F_LW::create((tmp + "_ECC").c_str(), (tmp2 + ", ECC" + m_histTitleExt + ";Charge" + atext_nclu).c_str(), hxbin, hxmin, hxmax));
      tmp = "1Hit_Cluster_Q"; tmp2 = "Charge for 1-hit clusters";
      //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
      sc = clusterExpert.regHist(m_1cluster_Q_B0    = TH1F_LW::create((tmp + "_B0").c_str(),  (tmp2 + ", B0"  + m_histTitleExt + ";Charge" + atext_nclu).c_str(), hxbin, hxmin, hxmax));
      sc = clusterExpert.regHist(m_1cluster_Q_B1    = TH1F_LW::create((tmp + "_B1").c_str(),  (tmp2 + ", B1"  + m_histTitleExt + ";Charge" + atext_nclu).c_str(), hxbin, hxmin, hxmax));
      sc = clusterExpert.regHist(m_1cluster_Q_B2    = TH1F_LW::create((tmp + "_B2").c_str(),  (tmp2 + ", B2"  + m_histTitleExt + ";Charge" + atext_nclu).c_str(), hxbin, hxmin, hxmax));
      sc = clusterExpert.regHist(m_1cluster_Q_ECA   = TH1F_LW::create((tmp + "_ECA").c_str(), (tmp2 + ", ECA" + m_histTitleExt + ";Charge" + atext_nclu).c_str(), hxbin, hxmin, hxmax));
      sc = clusterExpert.regHist(m_1cluster_Q_ECC   = TH1F_LW::create((tmp + "_ECC").c_str(), (tmp2 + ", ECC" + m_histTitleExt + ";Charge" + atext_nclu).c_str(), hxbin, hxmin, hxmax));   
      tmp = "2Hit_Cluster_Q"; tmp2 = "Charge for 2-hit clusters";
      //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
      sc = clusterExpert.regHist(m_2cluster_Q_B0    = TH1F_LW::create((tmp + "_B0").c_str(),  (tmp2 + ", B0"  + m_histTitleExt + ";Charge" + atext_nclu).c_str(), hxbin, hxmin, hxmax));
      sc = clusterExpert.regHist(m_2cluster_Q_B1    = TH1F_LW::create((tmp + "_B1").c_str(),  (tmp2 + ", B1"  + m_histTitleExt + ";Charge" + atext_nclu).c_str(), hxbin, hxmin, hxmax));
      sc = clusterExpert.regHist(m_2cluster_Q_B2    = TH1F_LW::create((tmp + "_B2").c_str(),  (tmp2 + ", B2"  + m_histTitleExt + ";Charge" + atext_nclu).c_str(), hxbin, hxmin, hxmax));
      sc = clusterExpert.regHist(m_2cluster_Q_ECA   = TH1F_LW::create((tmp + "_ECA").c_str(), (tmp2 + ", ECA" + m_histTitleExt + ";Charge" + atext_nclu).c_str(), hxbin, hxmin, hxmax));
      sc = clusterExpert.regHist(m_2cluster_Q_ECC   = TH1F_LW::create((tmp + "_ECC").c_str(), (tmp2 + ", ECC" + m_histTitleExt + ";Charge" + atext_nclu).c_str(), hxbin, hxmin, hxmax)); 
      tmp = "3Hit_Cluster_Q"; tmp2 = "Charge for 3-hit clusters";
      //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
      sc = clusterExpert.regHist(m_3cluster_Q_B0    = TH1F_LW::create((tmp + "_B0").c_str(),  (tmp2 + ", B0"  + m_histTitleExt + ";Charge" + atext_nclu).c_str(), hxbin, hxmin, hxmax));
      sc = clusterExpert.regHist(m_3cluster_Q_B1    = TH1F_LW::create((tmp + "_B1").c_str(),  (tmp2 + ", B1"  + m_histTitleExt + ";Charge" + atext_nclu).c_str(), hxbin, hxmin, hxmax));
      sc = clusterExpert.regHist(m_3cluster_Q_B2    = TH1F_LW::create((tmp + "_B2").c_str(),  (tmp2 + ", B2"  + m_histTitleExt + ";Charge" + atext_nclu).c_str(), hxbin, hxmin, hxmax));
      sc = clusterExpert.regHist(m_3cluster_Q_ECA   = TH1F_LW::create((tmp + "_ECA").c_str(), (tmp2 + ", ECA" + m_histTitleExt + ";Charge" + atext_nclu).c_str(), hxbin, hxmin, hxmax));
      sc = clusterExpert.regHist(m_3cluster_Q_ECC   = TH1F_LW::create((tmp + "_ECC").c_str(), (tmp2 + ", ECC" + m_histTitleExt + ";Charge" + atext_nclu).c_str(), hxbin, hxmin, hxmax)); 
   }

   tmp = "Cluster_LVL1A"; tmp2 = "Cluster Level 1 Accept";
   if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
   sc = timeShift.regHist(m_cluster_LVL1A          = TH1F_LW::create(tmp.c_str(),                (tmp2 +                         m_histTitleExt + ";LVL1A" + atext_nclu).c_str(), 14,-1.5,12.5));
   tmp = "Cluster_LVL1A"; tmp2 = "Cluster Level 1 Accept";
   sc = timeShift.regHist(m_cluster_LVL1A_PIX      = TH1F_LW::create((tmp + "_PIX").c_str(),     (tmp2 + " with ToT>15, Pixel" + m_histTitleExt + ";LVL1A" + atext_nclu).c_str(), 18,-1.5,16.5));
   sc = timeExpert.regHist(m_cluster_LVL1A_B0      = TH1F_LW::create((tmp + "_B0").c_str(),      (tmp2 + " with ToT>15, B0"    + m_histTitleExt + ";LVL1A" + atext_nclu).c_str(), 14,-1.5,12.5));
   sc = timeExpert.regHist(m_cluster_LVL1A_B1      = TH1F_LW::create((tmp + "_B1").c_str(),      (tmp2 + " with ToT>15, B1"    + m_histTitleExt + ";LVL1A" + atext_nclu).c_str(), 14,-1.5,12.5));
   sc = timeExpert.regHist(m_cluster_LVL1A_B2      = TH1F_LW::create((tmp + "_B2").c_str(),      (tmp2 + " with ToT>15, B2"    + m_histTitleExt + ";LVL1A" + atext_nclu).c_str(), 14,-1.5,12.5));
   sc = timeExpert.regHist(m_cluster_LVL1A_ECA     = TH1F_LW::create((tmp + "_ECA").c_str(),     (tmp2 + " with ToT>15, ECA"   + m_histTitleExt + ";LVL1A" + atext_nclu).c_str(), 14,-1.5,12.5));
   sc = timeExpert.regHist(m_cluster_LVL1A_ECC     = TH1F_LW::create((tmp + "_ECC").c_str(),     (tmp2 + " with ToT>15, ECC"   + m_histTitleExt + ";LVL1A" + atext_nclu).c_str(), 14,-1.5,12.5));
   sc = timeExpert.regHist(m_cluster_LVL1A_highToT = TH1F_LW::create((tmp + "_highToT").c_str(), (tmp2 + " with ToT>15"        + m_histTitleExt + ";LVL1A" + atext_nclu).c_str(), 14,-1.5,12.5));

   hxbin = 200; hxmin = 0.; hxmax = 200000.;
   tmp = "Cluster_charge"; tmp2 = "Cluster total charge";
   //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
   sc = clusterExpert.regHist(m_cluster_charge_B0  = TH1F_LW::create((tmp + "_B0").c_str(),  (tmp2 + ", B0"  + m_histTitleExt + ";Charge" + atext_nclu).c_str(), hxbin, hxmin, hxmax));
   sc = clusterExpert.regHist(m_cluster_charge_B1  = TH1F_LW::create((tmp + "_B1").c_str(),  (tmp2 + ", B1"  + m_histTitleExt + ";Charge" + atext_nclu).c_str(), hxbin, hxmin, hxmax));
   sc = clusterExpert.regHist(m_cluster_charge_B2  = TH1F_LW::create((tmp + "_B2").c_str(),  (tmp2 + ", B2"  + m_histTitleExt + ";Charge" + atext_nclu).c_str(), hxbin, hxmin, hxmax));
   sc = clusterExpert.regHist(m_cluster_charge_ECA = TH1F_LW::create((tmp + "_ECA").c_str(), (tmp2 + ", ECA" + m_histTitleExt + ";Charge" + atext_nclu).c_str(), hxbin, hxmin, hxmax));
   sc = clusterExpert.regHist(m_cluster_charge_ECC = TH1F_LW::create((tmp + "_ECC").c_str(), (tmp2 + ", ECC" + m_histTitleExt + ";Charge" + atext_nclu).c_str(), hxbin, hxmin, hxmax));
   
   if(m_doIBL){
      tmp = "Cluster_ToT"; tmp2 = "Cluster ToT";
      //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
      sc = clusterExpert.regHist(m_cluster_ToT_IBL     = TH1F_LW::create((tmp + "_IBL").c_str(),   (tmp2 + ", IBL"                   + m_histTitleExt + atext_tot + atext_nclu).c_str(), 50,-0.5,49.5));
      sc = clusterExpert.regHist(m_cluster_ToT_IBL2D   = TH1F_LW::create((tmp + "_IBL2D").c_str(), (tmp2 + " for IBL Planar Modules" + m_histTitleExt + atext_tot + atext_nclu).c_str(), 100,-0.5,99.5));
      sc = clusterExpert.regHist(m_cluster_ToT_IBL3D   = TH1F_LW::create((tmp + "_IBL3D").c_str(), (tmp2 + " for IBL 3D Modules"     + m_histTitleExt + atext_tot + atext_nclu).c_str(), 100,-0.5,99.5));
      tmp = "Big_Cluster_ToT"; tmp2 = "ToT for Clusters of size>3";
      //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
      sc = clusterExpert.regHist(m_bigcluster_ToT_IBL  = TH1F_LW::create((tmp + "_IBL").c_str(),   (tmp2 + ", IBL" + m_histTitleExt + atext_tot + atext_nclu).c_str(), 300,-0.5,299.5));
      tmp = "1Hit_Cluster_ToT"; tmp2 = "ToT for 1-hit clusters";
      //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
      sc = clusterExpert.regHist(m_1cluster_ToT_IBL    = TH1F_LW::create((tmp + "_IBL").c_str(), (tmp2 + ", IBL" + m_histTitleExt + atext_tot + atext_nclu).c_str(), 50, -0.5, 49.5));
      tmp = "2Hit_Cluster_ToT"; tmp2 = "ToT for 2-hit clusters";
      //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
      sc = clusterExpert.regHist(m_2cluster_ToT_IBL    = TH1F_LW::create((tmp + "_IBL").c_str(), (tmp2 + ", IBL" + m_histTitleExt + atext_tot + atext_nclu).c_str(), 100, -0.5, 99.5));
      tmp = "3Hit_Cluster_ToT"; tmp2 = "ToT for 3-hit clusters";
      //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
      sc = clusterExpert.regHist(m_3cluster_ToT_IBL    = TH1F_LW::create((tmp + "_IBL").c_str(), (tmp2 + ", IBL" + m_histTitleExt + atext_tot + atext_nclu).c_str(), 150, -0.5, 149.5));
      tmp = "Cluster_LVL1A"; tmp2 = "Cluster Level 1 Accept";
      //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
      sc = timeExpert.regHist(m_cluster_LVL1A_IBL      = TH1F_LW::create((tmp + "_IBL").c_str(), (tmp2 + " with ToT >4, IBL" + m_histTitleExt + "; LVL1A" + atext_nclu).c_str(), 18, -1.5, 16.5));
      tmp = "Cluster_charge"; tmp2 = "Cluster total charge";
      //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
      sc = clusterExpert.regHist(m_cluster_charge_IBL  = TH1F_LW::create((tmp + "_IBL").c_str(), (tmp2 + ", IBL" + m_histTitleExt + ";Charge" + atext_nclu).c_str(), hxbin, hxmin, hxmax)); 
      tmp = "Cluster_groupsize"; tmp2 = "Number of pixels per cluster";
      //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
      //sc = clusterExpert.regHist(m_cluster_groupsize_IBL = TH1F_LW::create((tmp + "_IBL").c_str(), (tmp2 + ", IBL" + m_histTitleExt + atext_npix + atext_nclu).c_str(), nbins_npix, min_npix, max_npix));
      tmp = "Cluster_column_width"; tmp2 = "Column width of cluster";
      //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
      //sc = clusterExpert.regHist(m_cluster_col_width_IBL = TH1F_LW::create((tmp + "_IBL").c_str(), (tmp2 + ", IBL" + m_histTitleExt + atext_cluw + atext_nclu).c_str(), nbins_nwid, min_nwid, max_nwid));
      tmp = "Cluster_row_width"; tmp2 = "Row width of cluster";
      //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
      sc = clusterExpert.regHist(m_cluster_row_width_IBL = TH1F_LW::create((tmp + "_IBL").c_str(), (tmp2 + ", IBL" + m_histTitleExt + atext_cluw + atext_nclu).c_str(), nbins_nwid, min_nwid, max_nwid));
    
      if(m_doESD && ! m_doOnline){
         hxbin = 200; hxmin = 0.; hxmax = 200000.;
         tmp = "Big_Cluster_Q"; tmp2 = "Charge for Clusters of size>3";
         //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
         sc = clusterExpert.regHist(m_bigcluster_Q_IBL   = TH1F_LW::create((tmp + "_IBL").c_str(), (tmp2 + ", IBL" + m_histTitleExt + ";Charge" + atext_nclu).c_str(), hxbin, hxmin, hxmax));
         tmp = "1Hit_Cluster_Q"; tmp2 = "Charge for 1-hit clusters";
         //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
         sc = clusterExpert.regHist(m_1cluster_Q_IBL     = TH1F_LW::create((tmp + "_IBL").c_str(), (tmp2 + ", IBL" + m_histTitleExt + ";Charge" + atext_nclu).c_str(), hxbin, hxmin, hxmax));
         tmp = "2Hit_Cluster_ToT"; tmp2 = "ToT for 2-hit clusters";
         //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
         sc = clusterExpert.regHist(m_2cluster_Q_IBL     = TH1F_LW::create((tmp + "_IBL").c_str(), (tmp2 + ", IBL" + m_histTitleExt + ";Charge" + atext_nclu).c_str(), hxbin, hxmin, hxmax));
         tmp = "3Hit_Cluster_Q"; tmp2 = "Charge for 3-hit clusters";
         //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
         sc = clusterExpert.regHist(m_3cluster_Q_IBL     = TH1F_LW::create((tmp + "_IBL").c_str(), (tmp2 + ", IBL" + m_histTitleExt + ";Charge" + atext_nclu).c_str(), hxbin, hxmin, hxmax));
         //tmp = "m_nlowToT_vs_clussize"; tmp2 = "# Low ToT Hits vs Size";
         tmp = "nlowToT_vs_clussize"; tmp2 = "# Low ToT Hits vs Size";
         //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
         sc = clusterExpert.regHist(m_nlowToT_vs_clussize_IBL = TH2F_LW::create((tmp + "_IBL").c_str(), (tmp2 + ", IBL" + m_histTitleExt + ";Cluster Size; frac low ToT hits").c_str(),40,-0.5,39.5,40,-0.5,39.5));
         //tmp = "m_clusToT_vs_eta"; tmp2 = "Cluster ToT vs Eta";
         tmp = "clusToT_vs_eta"; tmp2 = "Cluster ToT vs Eta";
         //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
         sc = clusterExpert.regHist(m_clusToT_vs_eta_IBL  = TH2F_LW::create((tmp + "_IBL").c_str(), (tmp2 + ", IBL" + m_histTitleExt + atext_eta + atext_tot).c_str(),20,-10.5,9.5,300,-0.5,299.5));
         //tmp = "m_clustersize_vs_eta"; tmp2 = "Cluster Size vs Eta";
         tmp = "clustersize_vs_eta"; tmp2 = "Cluster Size vs Eta";
         //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
         //sc = clusterExpert.regHist(m_clussize_vs_eta_IBL = TH2F_LW::create((tmp + "_IBL").c_str(), (tmp2 + ", IBL" + m_histTitleExt + atext_eta + ";Cluster size").c_str(),20,-10.5,9.5,40,-0.5,39.5));
         //tmp = "m_ToT_vs_clussize"; tmp2 = "Cluster ToT vs Size";
         tmp = "ToT_vs_clussize"; tmp2 = "Cluster ToT vs Size";
         //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
         sc = clusterExpert.regHist(m_ToT_vs_clussize_IBL = TH2F_LW::create((tmp + "_IBL").c_str(), (tmp2 + ", IBL" + m_histTitleExt + ";Cluster Size" + atext_tot).c_str(),40,-0.5,39.5,300,-0.5,299.5));
      }
   }
   if(m_do2DMaps)
   {
      tmp = "Cluster_Occupancy"; tmp2 = "Cluster occupancy";
      //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
      m_cluster_occupancy = new PixelMon2DMaps(tmp.c_str(), (tmp2 + m_histTitleExt).c_str());
      sc = m_cluster_occupancy->regHist(clusterShift);
      //tmp = "Cluster_Occupancy_DBM"; tmp2 = "Cluster occupancy";
      //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
      //m_clusocc_DBM = new DBMMon2DMaps(tmp.c_str(), (tmp2 + m_histTitleExt).c_str());
      //sc = m_clusocc_DBM->regHist(clusterShift);
      tmp = "Average_Cluster_Occupancy"; tmp2 = "Average cluster occupancy";
      //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
      m_average_cluster_occupancy = new PixelMon2DMaps(tmp.c_str(), (tmp2 + m_histTitleExt).c_str());
      sc = m_average_cluster_occupancy->regHist(clusterShift);
      tmp = "Cluster_LVL1A_Mod"; tmp2 = "Cluster Level 1 Accept";
      //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
      m_cluster_LVL1A_mod = new PixelMonProfiles(tmp.c_str(), (tmp2 + m_histTitleExt).c_str());
      sc = m_cluster_LVL1A_mod->regHist(timeShift);
      tmp = "Clus_Occ_SizeCut"; tmp2 = "Size>1 Cluster occupancy";
      //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
      m_clusocc_sizenot1 = new PixelMon2DMaps(tmp.c_str(), (tmp2 + m_histTitleExt).c_str()); 
      sc = m_clusocc_sizenot1->regHist(clusterShift); 
      tmp = "Clus_LVL1A_SizeCut"; tmp2 = "Size>1 Cluster Level 1 Accept";
      //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
      m_clus_LVL1A_sizenot1 = new PixelMonProfiles(tmp.c_str(), (tmp2 + m_histTitleExt).c_str());
      sc = m_clus_LVL1A_sizenot1->regHist(timeShift); 

      if(!m_doOnline){
         tmp = "Cluster_Size_Map"; tmp2 = "Cluster size map";
         //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
	      m_clussize_map = new PixelMonProfiles(tmp.c_str(), (tmp2 + m_histTitleExt).c_str());
	      sc = m_clussize_map->regHist(clusterExpert);
         tmp = "Cluster_Charge_Map"; tmp2 = "Cluster charge map";
         //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
	      m_cluscharge_map = new PixelMonProfiles(tmp.c_str(), (tmp2 + m_histTitleExt).c_str());
	      sc = m_cluscharge_map->regHist(clusterExpert);
         tmp = "Cluster_ToT_Map"; tmp2 = "Cluster ToT map";
         //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
	      m_clusToT_map = new PixelMonProfiles(tmp.c_str(), (tmp2 + m_histTitleExt).c_str());
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
     tmp = "num_clusters"; tmp2 = "Number of pixel clusters per event";
     if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
     tmp = "num_clusters"; tmp2 = "Number of pixel clusters per event";
     sc = clusterShift.regHist(m_num_clusters      = TH1I_LW::create(tmp.c_str(),            (tmp2 + ";# pixel clusters/event" + m_histTitleExt + ";# events").c_str(), 1000,0.,(int)max_clusters));
     sc = clusterExpert.regHist(m_num_clusters_PIX = TH1I_LW::create((tmp + "_PIX").c_str(), (tmp2 + ", Pixel" + m_histTitleExt + ";# pixel clusters/event;# events").c_str(),1000,0.,(int)max_clusters));
     sc = clusterExpert.regHist(m_num_clusters_B0  = TH1I_LW::create((tmp + "_B0").c_str(),  (tmp2 + ", B0" + m_histTitleExt + ";# pixel clusters/event;# events").c_str(),160,0.,(int)max_clusters_region));
     sc = clusterExpert.regHist(m_num_clusters_B1  = TH1I_LW::create((tmp + "_B1").c_str(),  (tmp2 + ", B1" + m_histTitleExt + ";# pixel clusters/event;# events").c_str(),160,0.,(int)max_clusters_region));
     sc = clusterExpert.regHist(m_num_clusters_B2  = TH1I_LW::create((tmp + "_B2").c_str(),  (tmp2 + ", B2" + m_histTitleExt + ";# pixel clusters/event;# events").c_str(),160,0.,(int)max_clusters_region));
     sc = clusterExpert.regHist(m_num_clusters_ECA = TH1I_LW::create((tmp + "_ECA").c_str(), (tmp2 + ", ECA"   + m_histTitleExt + ";# pixel clusters/event;# events").c_str(),160,0.,(int)max_clusters_region));
     sc = clusterExpert.regHist(m_num_clusters_ECC = TH1I_LW::create((tmp + "_ECC").c_str(), (tmp2 + ", ECC"   + m_histTitleExt + ";# pixel clusters/event;# events").c_str(),160,0.,(int)max_clusters_region));
     if(m_doIBL){sc = clusterExpert.regHist(m_num_clusters_IBL  = TH1I_LW::create((tmp + "_IBL").c_str(), (tmp2 + ", IBL" + m_histTitleExt + ";# pixel clusters/event;# events").c_str(),160,0.,(int)max_clusters_region));}

     tmp = "cluster_occupancy_summary"; tmp2 = "Average cluster occupancy";
     //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
     sc = clusterExpert.regHist(m_ecA_cluster_occupancy_summary     = TH1F_LW::create(("A_" + tmp).c_str(),   (tmp2 + ", ECA" + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 100,0.,(int)max_avclusters));
     sc = clusterExpert.regHist(m_ecC_cluster_occupancy_summary     = TH1F_LW::create(("C_" + tmp).c_str(),   (tmp2 + ", ECC" + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 100,0.,(int)max_avclusters));
     sc = clusterExpert.regHist(m_bar_layI_cluster_occupancy_summary= TH1F_LW::create(("IBL_" + tmp).c_str(), (tmp2 + ", IBL" + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 100,0.,(int)max_avclusters));
     sc = clusterExpert.regHist(m_bar_lay0_cluster_occupancy_summary= TH1F_LW::create(("B0_" + tmp).c_str(),  (tmp2 + ", B0"  + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 100,0.,(int)max_avclusters));
     sc = clusterExpert.regHist(m_bar_lay1_cluster_occupancy_summary= TH1F_LW::create(("B1_" + tmp).c_str(),  (tmp2 + ", B1"  + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 100,0.,(int)max_avclusters));
     sc = clusterExpert.regHist(m_bar_lay2_cluster_occupancy_summary= TH1F_LW::create(("B2_" + tmp).c_str(),  (tmp2 + ", B2"  + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 100,0.,(int)max_avclusters));
   }
   if(m_doLowOccupancy)
   {
      tmp = "num_clusters_low_occupancy"; tmp2 = "Number of pixel clusters per event";
      if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
      sc = clusterShift.regHist(m_num_clusters_low                        = TH1I_LW::create(tmp.c_str(),  (tmp2 + m_histTitleExt + ";# pixel clusters/event;# events").c_str(), 200,-0.5,199.5));
      tmp = "cluster_occupancy_summary_low_occupancy"; tmp2 = "Average cluster occupancy";
      //if(m_doOnTrack) { tmp = tmp + addOnTrack; tmp2 = tmp2 + addOnTrack; }
      sc = clusterExpert.regHist(m_ecA_cluster_occupancy_summary_low      = TH1F_LW::create(("A_" + tmp).c_str(),   (tmp2 + ", ECA" + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 50,0.,2.));
      sc = clusterExpert.regHist(m_ecC_cluster_occupancy_summary_low      = TH1F_LW::create(("C_" + tmp).c_str(),   (tmp2 + ", ECC" + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 50,0.,2.));
      sc = clusterExpert.regHist(m_bar_layI_cluster_occupancy_summary_low = TH1F_LW::create(("IBL_" + tmp).c_str(), (tmp2 + ", IBL" + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 50,0.,2.));
      sc = clusterExpert.regHist(m_bar_lay0_cluster_occupancy_summary_low = TH1F_LW::create(("B0_" + tmp).c_str(),  (tmp2 + ", B0"  + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 50,0.,2.));
      sc = clusterExpert.regHist(m_bar_lay1_cluster_occupancy_summary_low = TH1F_LW::create(("B1_" + tmp).c_str(),  (tmp2 + ", B1"  + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 50,0.,2.));
      sc = clusterExpert.regHist(m_bar_lay2_cluster_occupancy_summary_low = TH1F_LW::create(("B2_" + tmp).c_str(),  (tmp2 + ", B2"  + m_histTitleExt + ";average # of clusters per module per event;# modules").c_str(), 50,0.,2.));
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
   InDet::PixelClusterContainer::const_iterator colNext = m_Pixel_clcontainer->begin();
   InDet::PixelClusterContainer::const_iterator lastCol = m_Pixel_clcontainer->end();
   DataVector<PixelCluster>::const_iterator p_clus;
     
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
         int pixlayer = GetPixLayerID(m_pixelid->barrel_ec(clusID), m_pixelid->layer_disk(clusID), m_doIBL);

	      const PixelCluster& cluster = **p_clus;
	      const InDet::SiWidth clusWidth = cluster.width();
	      nclusters_all++; // count all clusters for ontrack

         if(m_doOnTrack || m_doOnPixelTrack) if(!OnTrack(clusID,true) ){
	         continue; //if we only want hits on track, and the hit is NOT on the track, skip filling.  true means doing clusters, false means rdos
	      }
	      nclusters_ontrack++;

         /////////////Start main fill block here///////////////
         //be sure to check each histo exists before filling it

         /// Fill LVL1 Accepted
         if(m_cluster_LVL1A)     m_cluster_LVL1A->Fill(cluster.LVL1A());   
	      if(m_cluster_LVL1A_mod) m_cluster_LVL1A_mod->Fill(clusID,m_pixelid,cluster.LVL1A()+0.00001,m_doIBL); //avoid filling exactly zero to distinguish from disabled modules
	      if(cluster.rdoList().size()>1 
            && m_clus_LVL1A_sizenot1) m_clus_LVL1A_sizenot1->Fill(clusID,m_pixelid,cluster.LVL1A()+0.00001,m_doIBL); //avoid filling exactly zero to distinguish from disabled modules
         /// Fill ToT
	      if(m_cluster_ToT_PIX && !(m_pixelid->barrel_ec(clusID)==0 && m_pixelid->layer_disk(clusID)==0 && m_doIBL)) m_cluster_ToT_PIX->Fill(cluster.totalToT());     
	      if(m_cluster_ToT_ECA && m_pixelid->barrel_ec(clusID)==2 ) m_cluster_ToT_ECA->Fill(cluster.totalToT());   
         if(m_cluster_ToT_ECC && m_pixelid->barrel_ec(clusID)==-2) m_cluster_ToT_ECC->Fill(cluster.totalToT());      
	      if(m_pixelid->barrel_ec(clusID)==0) {
	         if(m_doIBL && m_cluster_ToT_IBL2D && m_pixelid->layer_disk(clusID)==0 && m_pixelid->eta_module(clusID)<6 && m_pixelid->eta_module(clusID)>-7) m_cluster_ToT_IBL2D->Fill(cluster.totalToT());
            if(m_doIBL && m_cluster_ToT_IBL3D && m_pixelid->layer_disk(clusID)==0 && !(m_pixelid->eta_module(clusID)<6 && m_pixelid->eta_module(clusID)>-7)) m_cluster_ToT_IBL3D->Fill(cluster.totalToT());
	         if(m_doIBL && m_cluster_ToT_IBL && m_pixelid->layer_disk(clusID)==0) m_cluster_ToT_IBL->Fill(cluster.totalToT());
	         if(m_cluster_ToT_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL ) m_cluster_ToT_B0->Fill(cluster.totalToT());     
	         if(m_cluster_ToT_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL ) m_cluster_ToT_B1->Fill(cluster.totalToT()); 
	         if(m_cluster_ToT_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL ) m_cluster_ToT_B2->Fill(cluster.totalToT()); 
	      }
	      if(cluster.rdoList().size()==1){  // Cluster size = 1
            if(m_1cluster_ToT_ECA && m_pixelid->barrel_ec(clusID)==2 ) m_1cluster_ToT_ECA->Fill(cluster.totalToT());
            if(m_1cluster_ToT_ECC && m_pixelid->barrel_ec(clusID)==-2) m_1cluster_ToT_ECC->Fill(cluster.totalToT());
            if (m_pixelid->barrel_ec(clusID)==0) {
               if(m_1cluster_ToT_IBL && m_pixelid->layer_disk(clusID)==0 &&m_doIBL) m_1cluster_ToT_IBL->Fill(cluster.totalToT());
               if(m_1cluster_ToT_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL ) m_1cluster_ToT_B0->Fill(cluster.totalToT());
               if(m_1cluster_ToT_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL ) m_1cluster_ToT_B1->Fill(cluster.totalToT());
               if(m_1cluster_ToT_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL ) m_1cluster_ToT_B2->Fill(cluster.totalToT());
            }
         }
	      if(cluster.rdoList().size()==2){ // Cluster size = 2
            if(m_2cluster_ToT_ECA && m_pixelid->barrel_ec(clusID)==2 ) m_2cluster_ToT_ECA->Fill(cluster.totalToT());
            if(m_2cluster_ToT_ECC && m_pixelid->barrel_ec(clusID)==-2) m_2cluster_ToT_ECC->Fill(cluster.totalToT());
            if (m_pixelid->barrel_ec(clusID)==0) {
               if(m_2cluster_ToT_IBL && m_pixelid->layer_disk(clusID)==0 &&m_doIBL) m_2cluster_ToT_IBL->Fill(cluster.totalToT());
               if(m_2cluster_ToT_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL ) m_2cluster_ToT_B0->Fill(cluster.totalToT());
               if(m_2cluster_ToT_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL ) m_2cluster_ToT_B1->Fill(cluster.totalToT());
               if(m_2cluster_ToT_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL ) m_2cluster_ToT_B2->Fill(cluster.totalToT());
            }
         }
	      if(cluster.rdoList().size()==3){ // Cluster size = 3
            if(m_3cluster_ToT_ECA && m_pixelid->barrel_ec(clusID)==2 ) m_3cluster_ToT_ECA->Fill(cluster.totalToT());
            if(m_3cluster_ToT_ECC && m_pixelid->barrel_ec(clusID)==-2) m_3cluster_ToT_ECC->Fill(cluster.totalToT());
            if (m_pixelid->barrel_ec(clusID)==0) {
               if(m_3cluster_ToT_IBL && m_pixelid->layer_disk(clusID)==0 &&m_doIBL) m_3cluster_ToT_IBL->Fill(cluster.totalToT());
               if(m_3cluster_ToT_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL ) m_3cluster_ToT_B0->Fill(cluster.totalToT());
               if(m_3cluster_ToT_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL ) m_3cluster_ToT_B1->Fill(cluster.totalToT());
               if(m_3cluster_ToT_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL ) m_3cluster_ToT_B2->Fill(cluster.totalToT());
            }
         }
	      if(cluster.rdoList().size()>3){  // CLuster size > 3
	         if(m_bigcluster_ToT_ECA && m_pixelid->barrel_ec(clusID)==2 )m_bigcluster_ToT_ECA->Fill(cluster.totalToT());
	         if(m_bigcluster_ToT_ECC && m_pixelid->barrel_ec(clusID)==-2)m_bigcluster_ToT_ECC->Fill(cluster.totalToT());
	         if (m_pixelid->barrel_ec(clusID)==0) {
	           if(m_bigcluster_ToT_IBL && m_pixelid->layer_disk(clusID)==0 &&m_doIBL) m_bigcluster_ToT_IBL->Fill(cluster.totalToT());
	           if(m_bigcluster_ToT_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL ) m_bigcluster_ToT_B0->Fill(cluster.totalToT());
	           if(m_bigcluster_ToT_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL ) m_bigcluster_ToT_B1->Fill(cluster.totalToT());
	           if(m_bigcluster_ToT_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL ) m_bigcluster_ToT_B2->Fill(cluster.totalToT());
	         }
	      }
         /// Fill Charge
	      if(m_cluster_charge_ECA && m_pixelid->barrel_ec(clusID)==2 )m_cluster_charge_ECA->Fill(cluster.totalCharge());
	      if(m_cluster_charge_ECC && m_pixelid->barrel_ec(clusID)==-2)m_cluster_charge_ECC->Fill(cluster.totalCharge());
	      if (m_pixelid->barrel_ec(clusID)==0) {
	         if(m_cluster_charge_IBL && m_pixelid->layer_disk(clusID)==0 &&m_doIBL)m_cluster_charge_IBL->Fill(cluster.totalCharge());
	         if(m_cluster_charge_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL )m_cluster_charge_B0->Fill(cluster.totalCharge());
	         if(m_cluster_charge_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL )m_cluster_charge_B1->Fill(cluster.totalCharge());
	         if(m_cluster_charge_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL )m_cluster_charge_B2->Fill(cluster.totalCharge());
	      }
	      if(cluster.rdoList().size()==1){ // Cluster size = 1
            if(m_1cluster_Q_ECA && m_pixelid->barrel_ec(clusID)==2 )m_1cluster_Q_ECA->Fill(cluster.totalCharge());
	         if(m_1cluster_Q_ECC && m_pixelid->barrel_ec(clusID)==-2)m_1cluster_Q_ECC->Fill(cluster.totalCharge());
	         if (m_pixelid->barrel_ec(clusID)==0) {
               if(m_1cluster_Q_IBL && m_pixelid->layer_disk(clusID)==0 &&m_doIBL)m_1cluster_Q_IBL->Fill(cluster.totalCharge());
               if(m_1cluster_Q_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL )m_1cluster_Q_B0->Fill(cluster.totalCharge());
               if(m_1cluster_Q_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL )m_1cluster_Q_B1->Fill(cluster.totalCharge());
               if(m_1cluster_Q_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL )m_1cluster_Q_B2->Fill(cluster.totalCharge());
            }
         }
	      if(cluster.rdoList().size()==2){ // Cluster size = 2
	         if(m_2cluster_Q_ECA && m_pixelid->barrel_ec(clusID)==2 )m_2cluster_Q_ECA->Fill(cluster.totalCharge());
            if(m_2cluster_Q_ECC && m_pixelid->barrel_ec(clusID)==-2)m_2cluster_Q_ECC->Fill(cluster.totalCharge());
            if (m_pixelid->barrel_ec(clusID)==0) {
               if(m_2cluster_Q_IBL && m_pixelid->layer_disk(clusID)==0 &&m_doIBL)m_2cluster_Q_IBL->Fill(cluster.totalCharge());
               if(m_2cluster_Q_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL )m_2cluster_Q_B0->Fill(cluster.totalCharge());
               if(m_2cluster_Q_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL )m_2cluster_Q_B1->Fill(cluster.totalCharge());
               if(m_2cluster_Q_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL )m_2cluster_Q_B2->Fill(cluster.totalCharge());
            }
         }
	      if(cluster.rdoList().size()==3){ // Cluster size = 3
	         if(m_3cluster_Q_ECA && m_pixelid->barrel_ec(clusID)==2 )m_3cluster_Q_ECA->Fill(cluster.totalCharge());
            if(m_3cluster_Q_ECC && m_pixelid->barrel_ec(clusID)==-2)m_3cluster_Q_ECC->Fill(cluster.totalCharge());
            if (m_pixelid->barrel_ec(clusID)==0) {
               if(m_3cluster_Q_IBL && m_pixelid->layer_disk(clusID)==0 &&m_doIBL)m_3cluster_Q_IBL->Fill(cluster.totalCharge());
               if(m_3cluster_Q_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL )m_3cluster_Q_B0->Fill(cluster.totalCharge());
               if(m_3cluster_Q_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL )m_3cluster_Q_B1->Fill(cluster.totalCharge());
               if(m_3cluster_Q_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL )m_3cluster_Q_B2->Fill(cluster.totalCharge());
            }
         }
	      if(cluster.rdoList().size()>3){  // Cluster size > 3
	         if(m_bigcluster_Q_ECA && m_pixelid->barrel_ec(clusID)==2 )m_bigcluster_Q_ECA->Fill(cluster.totalCharge());
	         if(m_bigcluster_Q_ECC && m_pixelid->barrel_ec(clusID)==-2)m_bigcluster_Q_ECC->Fill(cluster.totalCharge());
	         if (m_pixelid->barrel_ec(clusID)==0) {
	            if(m_bigcluster_Q_IBL && m_pixelid->layer_disk(clusID)==0 &&m_doIBL)m_bigcluster_Q_IBL->Fill(cluster.totalCharge());
	            if(m_bigcluster_Q_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL )m_bigcluster_Q_B0->Fill(cluster.totalCharge());
	            if(m_bigcluster_Q_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL )m_bigcluster_Q_B1->Fill(cluster.totalCharge());
	            if(m_bigcluster_Q_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL )m_bigcluster_Q_B2->Fill(cluster.totalCharge());
	         }
	      }
	      if(cluster.totalToT() > 15)  //Cluster LVL1A histograms for ToT > 15 (corresponding to charge > 10000 electrons)  
	      { 
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
	      if(cluster.totalToT() > 4 
            && m_cluster_LVL1A_IBL && m_pixelid->layer_disk(clusID)==0 
            && m_pixelid->barrel_ec(clusID)==0 
            &&m_doIBL){
            m_cluster_LVL1A_IBL->Fill(cluster.LVL1A());
         }
         if(m_cluster_ToT_mod) m_cluster_ToT_mod->Fill(cluster.totalToT(),clusID,m_pixelid,m_doIBL);   

         // Fill Cluster efficiency
	      if(m_cluseff_mod){
            if(OnTrack(clusID,true)){
	            m_cluseff_mod->Fill(m_manager->lumiBlockNumber(),1.,clusID,m_pixelid,m_doIBL);
            }else{
               m_cluseff_mod->Fill(m_manager->lumiBlockNumber(),0.,clusID,m_pixelid,m_doIBL);
            }
         }
         
         /// Fill the number of pixels per cluster
         if(m_cluster_groupsize) m_cluster_groupsize->Fill(cluster.rdoList().size());  
         if(m_cluster_col_width) m_cluster_col_width->Fill(clusWidth.colRow().y()); //note: description is wrong in SiWidth! <x,y> = <row,col>                        
         if(m_cluster_row_width) m_cluster_row_width->Fill(clusWidth.colRow().x());                         
         if(m_clusterSize_eta && m_pixelid->barrel_ec(clusID)==0 ) m_clusterSize_eta->Fill(m_pixelid->eta_module(clusID),cluster.rdoList().size());    

         /// Fill Occupancy
         if(m_cluster_occupancy) m_cluster_occupancy->Fill(clusID,m_pixelid,m_doIBL);
         if(m_clusocc_DBM && m_doIBL) m_clusocc_DBM->Fill(clusID,m_pixelid);
         if(m_average_cluster_occupancy) m_average_cluster_occupancy->Fill(clusID,m_pixelid,m_doIBL);
	      if(cluster.rdoList().size()>1 && m_clusocc_sizenot1) m_clusocc_sizenot1->Fill(clusID,m_pixelid,m_doIBL); 

         /// 2D Map
	      if(m_clussize_map) m_clussize_map->Fill(clusID,m_pixelid,cluster.rdoList().size(),m_doIBL);
	      if(m_cluscharge_map) m_cluscharge_map->Fill(clusID,m_pixelid,cluster.totalCharge(),m_doIBL);
	      if(m_clusToT_map) m_clusToT_map->Fill(clusID,m_pixelid,cluster.totalToT(),m_doIBL);

	      if(m_cluster_size_mod)m_cluster_size_mod->Fill(cluster.rdoList().size(),clusID,m_pixelid,m_doIBL);  

	      /// Total Clusters per lumi block
         if(m_totalclusters_per_lumi)m_totalclusters_per_lumi->Fill(m_manager->lumiBlockNumber()); 
         if( pixlayer != 99 && m_totalclusters_per_lumi_mod[pixlayer] ) m_totalclusters_per_lumi_mod[pixlayer]->Fill( m_manager->lumiBlockNumber() );
	 
	      float nlowToT=0;
	      for(unsigned int i=0;i<cluster.totList().size();i++){
	         if(cluster.totList().at(i)<8) {nlowToT++;}
	      }
	 
	      if (m_pixelid->barrel_ec(clusID)==0) {	   

	         if(m_clusToT_vs_eta_IBL && m_pixelid->layer_disk(clusID)==0 && m_doIBL) m_clusToT_vs_eta_IBL->Fill(m_pixelid->eta_module(clusID) ,cluster.totalToT());
            if(m_clusToT_vs_eta_B0  && m_pixelid->layer_disk(clusID)==0+m_doIBL )   m_clusToT_vs_eta_B0->Fill(m_pixelid->eta_module(clusID) ,cluster.totalToT());
            if(m_clusToT_vs_eta_B1  && m_pixelid->layer_disk(clusID)==1+m_doIBL )   m_clusToT_vs_eta_B1->Fill(m_pixelid->eta_module(clusID) ,cluster.totalToT());
            if(m_clusToT_vs_eta_B2  && m_pixelid->layer_disk(clusID)==2+m_doIBL )   m_clusToT_vs_eta_B2->Fill(m_pixelid->eta_module(clusID) ,cluster.totalToT());
	         if(m_ToT_vs_clussize_IBL && m_pixelid->layer_disk(clusID)==0 && m_doIBL) m_ToT_vs_clussize_IBL->Fill(cluster.rdoList().size() ,cluster.totalToT());
            if(m_ToT_vs_clussize_B0  && m_pixelid->layer_disk(clusID)==0+m_doIBL )   m_ToT_vs_clussize_B0->Fill(cluster.rdoList().size() ,cluster.totalToT());
            if(m_ToT_vs_clussize_B1  && m_pixelid->layer_disk(clusID)==1+m_doIBL )   m_ToT_vs_clussize_B1->Fill(cluster.rdoList().size() ,cluster.totalToT());
            if(m_ToT_vs_clussize_B2  && m_pixelid->layer_disk(clusID)==2+m_doIBL )   m_ToT_vs_clussize_B2->Fill(cluster.rdoList().size() ,cluster.totalToT());
	         if(m_nlowToT_vs_clussize_IBL && m_pixelid->layer_disk(clusID)==0 && m_doIBL) m_nlowToT_vs_clussize_IBL->Fill(cluster.rdoList().size(),nlowToT);
            if(m_nlowToT_vs_clussize_B0  && m_pixelid->layer_disk(clusID)==0+m_doIBL )   m_nlowToT_vs_clussize_B0->Fill(cluster.rdoList().size(),nlowToT);
            if(m_nlowToT_vs_clussize_B1  && m_pixelid->layer_disk(clusID)==1+m_doIBL )   m_nlowToT_vs_clussize_B1->Fill(cluster.rdoList().size(),nlowToT);
            if(m_nlowToT_vs_clussize_B2  && m_pixelid->layer_disk(clusID)==2+m_doIBL )   m_nlowToT_vs_clussize_B2->Fill(cluster.rdoList().size(),nlowToT);

            if(m_Q_vs_clussize_B0 && m_pixelid->layer_disk(clusID)==0+m_doIBL ) m_Q_vs_clussize_B0->Fill(cluster.rdoList().size() ,cluster.totalCharge());
            if(m_Q_vs_clussize_B1 && m_pixelid->layer_disk(clusID)==1+m_doIBL ) m_Q_vs_clussize_B1->Fill(cluster.rdoList().size() ,cluster.totalCharge());
            if(m_Q_vs_clussize_B2 && m_pixelid->layer_disk(clusID)==2+m_doIBL ) m_Q_vs_clussize_B2->Fill(cluster.rdoList().size() ,cluster.totalCharge());
         }
	 
	      if (m_pixelid->barrel_ec(clusID)==2) {
            if(m_Q_vs_clussize_ECA)m_Q_vs_clussize_ECA->Fill(cluster.rdoList().size() ,cluster.totalCharge());
	         if(m_nlowToT_vs_clussize_ECA)m_nlowToT_vs_clussize_ECA->Fill(cluster.rdoList().size(),nlowToT);
	      }
	      if (m_pixelid->barrel_ec(clusID)==-2) {
            if(m_Q_vs_clussize_ECC)m_Q_vs_clussize_ECC->Fill(cluster.rdoList().size() ,cluster.totalCharge());
	         if(m_nlowToT_vs_clussize_ECC)m_nlowToT_vs_clussize_ECC->Fill(cluster.rdoList().size(),nlowToT);
	      }

         if(pixlayer != 99 && m_cluster_groupsize_mod[pixlayer]){
            if( cluster.rdoList().size() <= 299.5 ){
               m_cluster_groupsize_mod[pixlayer]->Fill( cluster.rdoList().size() );
            }else{
               m_cluster_groupsize_mod[pixlayer]->Fill( 300 );
            }
         }

         if(pixlayer != 99 && m_cluster_col_width_mod[pixlayer]) m_cluster_col_width_mod[pixlayer]->Fill( clusWidth.colRow().y() );
         if(pixlayer != 99 && m_cluster_row_width_mod[pixlayer]) m_cluster_row_width_mod[pixlayer]->Fill( clusWidth.colRow().x() );
         //if(m_clusQ_vs_eta_mod[pixlayer]) m_clusQ_vs_eta_mod[pixlayer]->Fill( m_pixelid->eta_module(clusID), cluster.totalCharge() );
         //if(m_clussize_vs_eta_mod[pixlayer]) m_clussize_vs_eta_mod[pixlayer]->Fill( m_pixelid->eta_module(clusID), cluster.rdoList().size() );

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
         if(pixlayer != 99 ) nclusters_mod[pixlayer]++;

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
      } // PixelClusterContainer loop
   }//end of event loop
   
   //////////////////////Fill after event block////////////////
   if(m_doOnline)//should we fill these mid run or only at the end?
   {
      FillSummaryHistos(m_cluster_occupancy,m_ecA_cluster_occupancy_summary,m_ecC_cluster_occupancy_summary,m_bar_layI_cluster_occupancy_summary,m_bar_lay0_cluster_occupancy_summary,m_bar_lay1_cluster_occupancy_summary,m_bar_lay2_cluster_occupancy_summary);
      FillSummaryHistos(m_cluster_occupancy,m_ecA_cluster_occupancy_summary_low,m_ecC_cluster_occupancy_summary_low,m_bar_layI_cluster_occupancy_summary_low,m_bar_lay0_cluster_occupancy_summary_low,m_bar_lay1_cluster_occupancy_summary_low,m_bar_lay2_cluster_occupancy_summary_low);
   }                              

   if(m_clusters_per_lumi)m_clusters_per_lumi->Fill(m_manager->lumiBlockNumber(),nclusters); 
   if(m_clusters_per_lumi_PIX)m_clusters_per_lumi_PIX->Fill(m_manager->lumiBlockNumber(),nclusters-nclusters_IBL); 
   for( int i=0; i<PixLayer::COUNT-1+(int)(m_doIBL); i++){
      if(m_clusters_per_lumi_mod[i]) m_clusters_per_lumi_mod[i]->Fill( m_manager->lumiBlockNumber(), nclusters_mod[i]);
   }

   if(m_largeclusters_per_lumi)m_largeclusters_per_lumi->Fill(m_manager->lumiBlockNumber(),nlargeclusters); 
   if(m_verylargeclusters_per_lumi)m_verylargeclusters_per_lumi->Fill(m_manager->lumiBlockNumber(),nverylargeclusters); 
   if( (nclusters>=1000) && m_highNclusters_per_lumi) m_highNclusters_per_lumi->Fill(m_manager->lumiBlockNumber()); 
   if (m_doOnTrack || m_doOnPixelTrack) {
     if(m_clustersOnOffTrack_per_lumi && nclusters_all>0) m_clustersOnOffTrack_per_lumi->Fill(m_manager->lumiBlockNumber(),(float)nclusters_ontrack/nclusters_all); 
   }

   ////Fill some histograms only if =< 50% of modules disabled
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
    if( m_event != m_event2) events = m_event2;

   if (m_average_cluster_occupancy) {  //scale to get average occupancies
      m_average_cluster_occupancy->Scale(events, m_doIBL);  
   }

   return StatusCode::SUCCESS;
}

