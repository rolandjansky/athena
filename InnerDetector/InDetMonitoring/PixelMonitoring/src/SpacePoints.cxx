/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// Book and fill histograms for pixel spacepoints
///////////////////////////////////////////////////////////////////////////////

#include "PixelMonitoring/PixelMainMon.h"
//#include "InDetRawData/InDetTimeCollection.h"
#include "TrkSpacePoint/SpacePointContainer.h"
//#include "InDetPrepRawData/PixelClusterContainer.h"         
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
#include <sstream>

//#include "InDetRawData/InDetRawDataContainer.h"
//#include "InDetRawData/InDetRawDataCLASS_DEF.h" 
//#include "TrkTrack/TrackCollection.h"            
//#include "InDetReadoutGeometry/SiDetectorElement.h"
//#include "TrkTrackSummary/TrackSummary.h"
#include "PixelMonitoring/PixelMon2DMaps.h"
#include "PixelMonitoring/PixelMonModules.h"
#include "GeoPrimitives/GeoPrimitives.h"


///////////////////////////////////////////////////////////////////////////////
//////////////////////booking methods//////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

StatusCode PixelMainMon::BookSpacePointMon(void)
{
  std::string path = "Pixel/SpacePoint";
   if(m_doOnTrack) path.replace(path.begin(), path.end(), "Pixel/SpacePointOnTrack");
   if(m_doOnPixelTrack) path.replace(path.begin(), path.end(), "Pixel/SpacePointOnPixelTrack");

   MonGroup spacePointHistos( this, path.c_str(),run,ATTRIB_MANAGED ); //declare a group of histograms
   if(m_doHighOccupancy)
   {
     sc = spacePointHistos.regHist(m_num_spacepoints = TH1I_LW::create("num_spacepoints",  ("number of pixel spacepoint hits per event" + m_histTitleExt + ";# spacepoints;# events").c_str(), 100,0.,25000));
   }
   if(m_doLowOccupancy)
   {
      sc = spacePointHistos.regHist(m_num_spacepoints_low = TH1I_LW::create("num_spacepoints_low_occupancy",  ("number of pixel spacepoint hits per event" + m_histTitleExt + ";# spacepoints;# events").c_str(), 100,-0.5,99.5));
   }
   sc = spacePointHistos.regHist(m_spHit_x   = TH1F_LW::create("pixel_sp_x",("x location of hit" + m_histTitleExt + ";x (mm);# hits").c_str(),320,-160.,160.));                                                      //add each histogram to this group
   sc = spacePointHistos.regHist(m_spHit_y   = TH1F_LW::create("pixel_sp_y",("y location of hit" + m_histTitleExt + ";y (mm);# hits").c_str(),320,-160.,160.));                                                      //summary means the type of histogram (summary, shifter, expert)
   sc = spacePointHistos.regHist(m_spHit_z   = TH1F_LW::create("pixel_sp_z",("z location of hit" + m_histTitleExt + ";z (mm);# hits").c_str(),660,-660.,660.));                                                      //all is for what to integrate over (all, lumiblock, run....)
   sc = spacePointHistos.regHist(m_spHit_r   = TH1F_LW::create("pixel_sp_r",("r location of hit" + m_histTitleExt + ";r (mm);# hits").c_str(),230,-0.  ,230.));                                                      //all is for what to integrate over (all, lumiblock, run....)
   sc = spacePointHistos.regHist(m_spHit_phi = TH1F_LW::create("pixel_sp_phi",("phi location of hit" + m_histTitleExt + ";phi (mm);# hits").c_str(),60,-4.,4.));                                                      //all is for what to integrate over (all, lumiblock, run....)
   sc = spacePointHistos.regHist(m_spHit_xy  = TH2F_LW::create("pixel_sp_x_vs_y",("xy location of barrel hits" + m_histTitleExt + ";x (mm);y (mm)").c_str(),100,-160,160,100,-160,160));                                                      //all is for what to integrate over (all, lumiblock, run....)
   sc = spacePointHistos.regHist(m_spHit_rz  = TH2F_LW::create("pixel_sp_r_vs_z",("rz location of hit" + m_histTitleExt + ";z (mm);r (mm)").c_str(),200,-665,665,200,-0,165));                                                      //all is for what to integrate over (all, lumiblock, run....)

   if(sc.isFailure())if(msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "histograms not booked" << endreq;   

   return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::FillSpacePointMon(void)
{
   sc = evtStore()->retrieve(m_Pixel_spcontainer, m_Pixel_SpacePointsName );
   if (sc.isFailure() || !m_Pixel_spcontainer)
   {
      if(msgLvl(MSG::INFO)) msg(MSG::INFO)  <<"SpacePoint container for Pixels not found"<< endreq;
      if(m_storegate_errors) m_storegate_errors->Fill(2.,3.);  
      return StatusCode::SUCCESS;
   } else{
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  <<"Si SpacePoint container for Pixels found" <<endreq;
   }

   DataVector<SpacePoint>::const_iterator p_sp;             
   //std::pair<IdentifierHash, IdentifierHash> sp_clusInfo;
   //IdentifierHash first_clus;
   Identifier PixelModuleId;
   int nhits=0;

   //loop over Pixel space points collections        //loop over rows    
   for(SpacePointContainer::const_iterator it=m_Pixel_spcontainer->begin(); it!=m_Pixel_spcontainer->end(); ++it)
   {
      const SpacePointCollection *colNext=&(**it);
      if (!colNext)
      {
         if(m_storegate_errors) m_storegate_errors->Fill(2.,5.);  //first entry (1). is for SP, second (4) is for data problem
         continue;
      }

      for(p_sp=colNext->begin(); p_sp!=colNext->end(); ++p_sp)  //loop over columns
      {
         const SpacePoint& sp = **p_sp;
         //sp_clusInfo = sp.elementIdList();
         //first_clus = sp_clusInfo.first;
         PixelModuleId =  sp.clusterList().first->identify();

         if(m_doOnTrack || m_doOnPixelTrack) if(!OnTrack(PixelModuleId,true) )continue; //if we only want hits on track, and the hit is NOT on the track, skip filling
	 Amg::Vector3D point = sp.globalPosition();

	 if(m_spHit_x)m_spHit_x->Fill(point.x());     // spacepoint x.
         if(m_spHit_y)m_spHit_y->Fill(point.y());     // spacepoint y.
         if(m_spHit_z)m_spHit_z->Fill(point.z());     // spacepoint z.
         if(m_spHit_r)m_spHit_r->Fill(sqrt(point.y()*point.y() + point.x()*point.x()));     // spacepoint r.
         if(m_spHit_phi)m_spHit_phi->Fill(point.phi());     // spacepoint phi.
         if(m_spHit_xy && fabs(point.z()) < 400)m_spHit_xy->Fill(point.x(),point.y());
         if(m_spHit_rz)m_spHit_rz->Fill(point.z(),sqrt(point.y()*point.y() + point.x()*point.x()));
         nhits++;
      }  //end cols
   }     //end rows
   if(m_num_spacepoints)m_num_spacepoints->Fill(nhits);
   if(m_num_spacepoints_low)m_num_spacepoints_low->Fill(nhits);
   if(nhits==0 && m_storegate_errors) m_storegate_errors->Fill(2.,4.);//first entry for sp, second for size = 0
   return StatusCode::SUCCESS;
}
