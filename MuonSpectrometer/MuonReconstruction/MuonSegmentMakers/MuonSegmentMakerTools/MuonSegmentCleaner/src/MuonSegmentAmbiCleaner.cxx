/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentCleaner/MuonSegmentAmbiCleaner.h"

#include "MuonSegment/MuonSegment.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonRIO_OnTrack/TgcClusterOnTrack.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "EventPrimitives/EventPrimitives.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"

#include <sstream>
#include <iostream>
#include <vector>

MuonSegmentAmbiCleaner::MuonSegmentAmbiCleaner(const std::string& type,const std::string& name,const IInterface* parent):AthAlgTool(type,name,parent)
{
  declareInterface<IMuonSegmentCleaner>(this);

  m_debug = false;
  declareProperty("DoDebug",m_debug);

  m_summary = false;
  declareProperty("DoSummary",m_summary);

}

StatusCode MuonSegmentAmbiCleaner::initialize()
{
  ATH_MSG_VERBOSE(" MuonSegmentiAmbiCleaner::Initializing ");
  ATH_CHECK( m_idHelperSvc.retrieve() );
  ATH_MSG_VERBOSE("End of Initializing");
  return StatusCode::SUCCESS;
}

const Muon::MuonSegment* MuonSegmentAmbiCleaner::resolve(const Muon::MuonSegment* segment)const
{
  ATH_MSG_VERBOSE(" Executing MuonSegmentAmbiCleanerTools ");

  DataVector<const Trk::MeasurementBase>* meas_keep = new DataVector<const Trk::MeasurementBase>();

// create new surface 
  Trk::PlaneSurface* psf = (segment->associatedSurface()).clone();
  Amg::Transform3D globalToLocal = psf->transform().inverse();
  Amg::Vector3D lSegmentPos = globalToLocal*(segment->globalPosition());
  Amg::Vector3D lSegmentDir = globalToLocal*(segment->globalDirection());


  const std::vector<const Trk::MeasurementBase*>& meas = segment->containedMeasurements();
  std::vector<const Trk::MeasurementBase*>::const_iterator mit = meas.begin();
  std::vector<const Trk::MeasurementBase*>::const_iterator mit_end = meas.end();
  // loop over hits
  int nphirpc = 0;
  int nphitgc = 0;
  int nphicsc = 0;
  int netamdt = 0;
  int netarpc = 0;
  int netatgc = 0;
  int netacsc = 0;
  int irio = 0;
  
  std::vector<const Trk::RIO_OnTrack*> rots;
  std::vector<const Trk::CompetingRIOsOnTrack*> crots; // lookup vector to check if rot is part of competing rio. vector contains 0 if not part of competing rio
  rots.reserve(2*meas.size()); // factor 2, to be on safe side
  crots.reserve(2*meas.size());

  for( ; mit!=mit_end;++mit ){

    //dynamic cast to either rio or competingrio:
    const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(*mit);    
    if (rot)
      {
	rots.push_back(rot);
	crots.push_back(0);
      }

    else 
      {
	const Trk::CompetingRIOsOnTrack* crio = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(*mit);    
	if (crio)
	  {
	    for (unsigned int i = 0; i<crio->numberOfContainedROTs(); i++)
	      {
		rot = &crio->rioOnTrack(i);
		rots.push_back(rot);
		crots.push_back(crio);
	      }
	  }
      }
  }

  unsigned int nMeas = rots.size();
  unsigned int nphi = 0;

  // Vectors for the phi hits 
  std::vector <const Trk::RIO_OnTrack*> rots_phi(nMeas);
  std::vector <const Trk::CompetingRIOsOnTrack*> crots_phi(nMeas);
  std::vector <const Trk::MeasurementBase*> meas_phi(nMeas);
  std::vector <double> dis_phi(nMeas); // distance to segment
  std::vector <int> chambercode_phi(nMeas); 
  std::vector <int> stripcode_phi(nMeas);
  std::vector <int> ok_phi(nMeas); // 0 Not selected 1 selected/
  std::vector <int> det_phi(nMeas); // 1 = RPC 2 = TGC /
  std::vector <Identifier> id_phi(nMeas); // 1 = RPC 2 = TGC /

  if  (m_debug) std::cout << " MuonSegmentAmbiCleanerTool nMeas " << nMeas << " competing rios: " << crots.size() << std::endl; 

  for (unsigned int i=0; i<rots.size(); i++){
    
    const Trk::RIO_OnTrack* rot = rots[i];
    const Trk::PrepRawData* prd = rot->prepRawData();
    Identifier id = prd->identify();
    irio++;
    if( m_idHelperSvc->isMdt( rot->identify() ) ){
      meas_keep->push_back(rot->clone());
      netamdt++;
      continue;
    }else if( m_idHelperSvc->isRpc( rot->identify() ) ){
      if( m_idHelperSvc->rpcIdHelper().measuresPhi(id) != 1) {
	meas_keep->push_back(rot->clone());
        netarpc++;
	continue ;
      }
    }else if( m_idHelperSvc->isTgc( rot->identify() ) ){
      if( m_idHelperSvc->tgcIdHelper().isStrip(id) != 1 ) {
	meas_keep->push_back(rot->clone());
        netatgc++;
	continue;
      }
    }else if( m_idHelperSvc->isCsc( rot->identify() ) ){
      meas_keep->push_back(rot->clone());
      if( m_idHelperSvc->cscIdHelper().measuresPhi(id) != 1) {
	  netacsc++;
      } else {
	nphicsc++;
      }
      continue;
    }

    //add Phi Hits
    id_phi[nphi] = id;
    rots_phi[nphi] = rot;
    crots_phi[nphi] = crots[i];
    chambercode_phi[nphi] = 0;
    stripcode_phi[nphi] = 0;
    ok_phi[nphi] = 0; 
    det_phi[nphi] = 0; 
    dis_phi[nphi] = 10000000; 
    if (m_idHelperSvc->isRpc( rot->identify())) {
      nphirpc++;
      int code = 1000000*(m_idHelperSvc->rpcIdHelper().stationName(id));
      code = code + 2*((m_idHelperSvc->rpcIdHelper().doubletR(id))-1)+16*((m_idHelperSvc->rpcIdHelper().gasGap(id))-1);
      chambercode_phi[nphi] = code;
      stripcode_phi[nphi] = m_idHelperSvc->rpcIdHelper().strip(id);
      ok_phi[nphi] = 1;
      det_phi[nphi] = 1; 
      const Muon::RpcClusterOnTrack* rrot = dynamic_cast<const Muon::RpcClusterOnTrack*>(rot);
      if( !rrot ){
	ATH_MSG_WARNING("This is not a  RpcClusterOnTrack!!! ");
	continue;
      }
      const Muon::RpcPrepData* rprd = rrot->prepRawData();
      Amg::Vector3D gHitPos = rprd->globalPosition();
      Amg::Vector3D lHitPos = globalToLocal*gHitPos;
      
      // In the barrel local z is measured 
      double disRPC = lSegmentPos.z() - lHitPos.z() + lSegmentDir.z()*(lHitPos.y()-lSegmentPos.y())/lSegmentDir.y();
      if (m_debug) {
	std::cout << " ghit pos x " << gHitPos.x() << " y " << gHitPos.y() << " z "  << gHitPos.z() << std::endl;
	std::cout << " dis RPC " << disRPC <<  std::endl;
      }
      dis_phi[nphi] = disRPC;
    } else if ( m_idHelperSvc->isTgc( rot->identify())) {
      nphitgc++;
      int code = 1000000*(m_idHelperSvc->tgcIdHelper().stationName(id))+100*(m_idHelperSvc->tgcIdHelper().stationEta(id)+10);
      code = code + m_idHelperSvc->tgcIdHelper().gasGap(id);
      chambercode_phi[nphi] = code;
      stripcode_phi[nphi] = m_idHelperSvc->tgcIdHelper().channel(id);
      ok_phi[nphi] = 1;
      det_phi[nphi] = 2; 
      
      const Muon::TgcClusterOnTrack* rrot = dynamic_cast<const Muon::TgcClusterOnTrack*>(rot);
      if( !rrot ){
	ATH_MSG_WARNING("This is not a  TgcClusterOnTrack!!! ");
	continue;
      }
      const Muon::TgcPrepData* rprd = rrot->prepRawData();
      Amg::Vector3D gHitPos = rprd->globalPosition();
      Amg::Vector3D lHitPos = globalToLocal*gHitPos;
      // In the forward local y is measured 
      double disTGC = lSegmentPos.y() - lHitPos.y() + lSegmentDir.y()*(lHitPos.z()-lSegmentPos.z())/lSegmentDir.z();
      if (m_debug) {
          std::cout << " ghit pos x " << gHitPos.x() << " y " << gHitPos.y() << " z "  << gHitPos.z() << std::endl;
          std::cout << "  dis TGC " << disTGC <<  std::endl;
      }
      dis_phi[nphi] = disTGC;
    } else {
      dis_phi[nphi] = 0.;
    }
    if (m_debug) std::cout << " Distance to segment " << dis_phi[nphi] << std::endl; 
    if (ok_phi[nphi] == 1 ) nphi++;
  }
// Code to select and flag ambiguous phi hits

  bool changeSegment = false;
 int nphirpcn = 0;   
 int nphitgcn = 0;   
 if (nphi > 0) {
   for(unsigned int i = 0; i < nphi-1  ; ++i )  {
     if (ok_phi[i] == 0) continue;
     for(unsigned int j = i+1 ; j < nphi  ; ++j )  {
       if (ok_phi[j] == 0) continue;
       bool ambi = false;
       if ( stripcode_phi[i] == stripcode_phi[j] && chambercode_phi[i] == chambercode_phi[j] ) ambi = true;
       if ( ambi ) {
         Identifier id1 = id_phi[i];
         Identifier id2 = id_phi[j];
         if (det_phi[i] == 1 && det_phi[j] == 1 && m_debug) {
           ATH_MSG_INFO(" RPC Station 1 eta " << m_idHelperSvc->rpcIdHelper().stationEta(id1) << " phi " <<  m_idHelperSvc->rpcIdHelper().stationPhi(id1));
           ATH_MSG_INFO(" RPC Station 2 eta " << m_idHelperSvc->rpcIdHelper().stationEta(id2) << " phi " <<  m_idHelperSvc->rpcIdHelper().stationPhi(id2));
         }
         if (det_phi[i] == 2 && det_phi[j] == 2 && m_debug) {
           ATH_MSG_INFO(" TGC Station 1 eta " << m_idHelperSvc->tgcIdHelper().stationEta(id1) << " phi " <<  m_idHelperSvc->tgcIdHelper().stationPhi(id1));
           ATH_MSG_INFO(" TGC Station 2 eta " << m_idHelperSvc->tgcIdHelper().stationEta(id2) << " phi " <<  m_idHelperSvc->tgcIdHelper().stationPhi(id2));
         }

	 if (m_debug) { ATH_MSG_DEBUG(" Ambiguous " << " Distance1 " << dis_phi[i] << " Distance1 " << dis_phi[j]); }
	 if (dis_phi[i]!= 0.&& dis_phi[j]!=0) {
	   if ( fabs(dis_phi[i]) < fabs(dis_phi[j]) ) {
	     ok_phi[j] = 0;
	   } else {
	     ok_phi[i] = 0;
	   }
	 }
	 if (m_debug) {
	   if (det_phi[i] == 1) { ATH_MSG_DEBUG(" RPC Ambiguities "); }
	   if (det_phi[i] == 2) { ATH_MSG_DEBUG(" TGC Ambiguities "); }
	   ATH_MSG_DEBUG(" index " << i << " strip " << stripcode_phi [i] << " chambercode " << chambercode_phi[i] << " selected " << ok_phi[i] <<  " segment distance " << dis_phi[i]);
	   ATH_MSG_DEBUG(" index " << j << " strip " << stripcode_phi [j] << " chambercode " << chambercode_phi[j] << " selected " << ok_phi[j] <<  " segment distance " << dis_phi[j]);
	 }
       }    
     } 
   } 

   // if any phi hits belonging to a competing rio is removed, remove the pointer to the competing rio and only store the single phi hit

   for(unsigned int i = 0; i < nphi  ; i++ )  {
     if (ok_phi[i] == 0)
       {
	 for(unsigned int j = 0; j < nphi ; j++ )  {
	   if (crots_phi[j] == crots_phi[i] && j!=i)
	     {
	       crots_phi[j]=0;
	     }
	 }
	 crots_phi[i]=0;
       }
   }

//  Put kept hits on segment
//  Put selected phi  hits on segment
   
   std::set <const Trk::CompetingRIOsOnTrack*> selected_crots;

   for (unsigned int i=0;i<nphi;++i){
     if (ok_phi[i] == 1) {
       if (!crots_phi[i]) // not a competing measurement
	 {
	   if (det_phi[i] == 1) nphirpcn++;
	   if (det_phi[i] == 2) nphitgcn++;
	   meas_keep->push_back(rots_phi[i]->clone());
	 }
       else if (selected_crots.count(crots_phi[i]) == 0) // competing measurement not yet added
	 {
	   meas_keep->push_back(crots_phi[i]->clone());
	   selected_crots.insert(crots_phi[i]);
	   if (det_phi[i] == 1) nphirpcn++;
	   if (det_phi[i] == 2) nphitgcn++;
	 }
     } if (ok_phi[i] == 0 && (det_phi[i] == 1||det_phi[i] == 2)) {
       changeSegment = true;
     }
   }
 }  

 if ((m_summary&&changeSegment)||m_debug) {
    std::cout << " Summary MuonSegmentAmbiCleaner (not accurate with competing rios!)"  << std::endl;
    std::cout << " Input Segment with " << netamdt << " MDT hits " << netacsc << " eta CSC hits " << netatgc << " eta TGC Hits " << netarpc << " eta RPC hits "  << std::endl;
    std::cout << " and " << nphicsc << " phi CSC hits " << nphitgc << " phi TGC Hits " << nphirpc << " phi RPC hits " << std::endl;
    std::cout << " Output after Ambiguity removal "  << nphitgcn << " phi TGC Hits " << nphirpcn << " phi RPC hits " << std::endl;
 } 

  // Make new segment with cleaned up rios
// MuonSegment( const Trk::LocalPosition& segLocPos,         // 2 local position coordinates
//          const Trk::LocalDirection* segLocDir,        // 2 local direction coordinates
//          const Trk::ErrorMatrix* segLocalErr,         // 4 x 4 full local error 
//          Trk::PlaneSurface* psf,                      // plane surface to define frame
//          std::vector<const Trk::RIO_OnTrack*>* crots, // vector of contained rios on track
//          Trk::FitQuality* fqual);  

 
  const Trk::LocalDirection locSegmentDir(segment->localDirection());
  Amg::Vector2D locSegmentPos(lSegmentPos.x(),lSegmentPos.y());
  const Amg::MatrixX locSegmenterr(segment->localCovariance());
  Trk::FitQuality* fitQuality = segment->fitQuality()->clone();
  Muon::MuonSegment* newSegment = new Muon::MuonSegment(locSegmentPos,locSegmentDir,locSegmenterr,psf,meas_keep,fitQuality); 
  
  return newSegment; 
} // execute

