/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////////////////////
// Package : MdtVsTgcRawDataValAlg
// Author:   M.King(Kobe)
// Feb.  2011
//
// DESCRIPTION:
// Subject: TGC Efficiency     -->TGC Efficiency plots including EIFI by comparing with MDT Segments
///////////////////////////////////////////////////////////////////////////////////////////// 

#include "MdtVsTgcRawDataMonitoring/MdtVsTgcRawDataValAlg.h"

#include "TrkEventPrimitives/TrkEventPrimitivesDict.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "muonEvent/MuonContainer.h"
//use new MDT segment container 
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODMuon/MuonSegment.h"

#include <TH1F.h>
#include <TH2F.h>
#include <TH1.h>
#include <TH2.h>
#include <inttypes.h> 

#include <sstream>
#include <algorithm>
#include <fstream>

using namespace std;
void
MdtVsTgcRawDataValAlg::maphists(const xAOD::MuonSegmentContainer *newsegment,
                                const Muon::TgcPrepDataContainer *tgc_prepcontainer){//use new mdt segment
  ATH_MSG_DEBUG("inside maptgchits" );
  
  ///////////////////////////////////////////////////////////////////////////////////////////////
  // loop over MDT Segment Collection
  xAOD::MuonSegmentContainer::const_iterator mdtseg_itr = newsegment->begin();
  xAOD::MuonSegmentContainer::const_iterator mdtseg_end = newsegment->end();
  for(; mdtseg_itr!=mdtseg_end; ++mdtseg_itr){
    if(!(*mdtseg_itr)->muonSegment().isValid())continue;
    // Get segm
    const Muon::MuonSegment* segm = dynamic_cast<const Muon::MuonSegment*>(*(*mdtseg_itr)->muonSegment());
    if (segm == 0) {
      ATH_MSG_ERROR( "no pointer to segm!!!" );
      break;
    }
    // Initialize variables
    int stationName=0;
    int nMdtMeas[4] = {0,0,0,0};
    bool isMdt=false, isEndcap=false;
    
    // Loop through contained ROTs and count used MDT and TGC stations
    for(unsigned int i=0;i<segm->numberOfContainedROTs();++i){
      // Get station information
      const Trk::RIO_OnTrack* rio = segm->rioOnTrack(i);
      Identifier id = rio->identify();
      stationName = int(m_mdtIdHelper->stationName(id));
      // Flag Segments with ROTs in the MDT & Endcap
      if(m_mdtIdHelper->is_mdt(id))isMdt=true;
      if(m_mdtIdHelper->isEndcap(id))isEndcap=true;
      // If ROT is MDT
      if((stationName==13)||(stationName==49)){nMdtMeas[0]++;}
      if((stationName==14)||(stationName==15)){nMdtMeas[1]++;}
      if((stationName==17)||(stationName==18)){nMdtMeas[2]++;}
      if((stationName==20)||(stationName==21)){nMdtMeas[3]++;}
    }
    if(!isMdt)continue;
    if(!isEndcap)continue;

    // Establish which MDT stations the Segment is in
    int nStations = 0;
    for(int jMDT=0;jMDT<4;jMDT++){
      if(nMdtMeas[jMDT]){
        nStations++;
      }
    }
    // Check that the Segment only occupies one Station
    if(nStations!=1)continue;

    // Get Position pointer and segmAC index
    const Amg::Vector3D segmGlobalPos = segm->globalPosition();
    int segmAC = (segmGlobalPos.z()<0);// a:0, c:1

    // Fill MDT data
    for(int jMDT=0;jMDT<4;jMDT++){// jMDT
      if(nMdtMeas[jMDT]){// If hits in this Station
        // Get position variables
        float  segmGlobalEta  = abs(segmGlobalPos.eta());
        float  segmGlobalPhi  = segmGlobalPos.phi();
        if(segmGlobalPhi<0) segmGlobalPhi+=2*M_PI;
        // Fill position histogram
        m_mdt_segmmap[segmAC][jMDT]->Fill(segmGlobalPhi, segmGlobalEta);
      }// If hits in Station
    }// jMDT
  }// MDT Segment Collection
  
  ///////////////////////////////////////////////////////////////////////////////////////////////
  //loop over TGC Prep Data container
  Muon::TgcPrepDataContainer::const_iterator tgc_it_end=tgc_prepcontainer->end();
  for( Muon::TgcPrepDataContainer::const_iterator tgc_it=tgc_prepcontainer->begin();
       tgc_it!=tgc_it_end;
       ++tgc_it){
    
    //loop over TGC Prep Data collection
    Muon::TgcPrepDataCollection::const_iterator tgc_itc_end=(*tgc_it)->end();
    for( Muon::TgcPrepDataCollection::const_iterator tgc_itc=(*tgc_it)->begin();
         tgc_itc!= tgc_itc_end;
         ++tgc_itc){
      const Muon::TgcPrepData* tpd=*tgc_itc;
      
      // Get detector information
      const MuonGM::TgcReadoutElement *tre = tpd->detectorElement();
      const std::string tgcStationType = tre->getStationType();
      
      // Get detector variables
      Identifier tgcid=(*tgc_itc)->identify();
      int tgcStationName = m_tgcIdHelper->stationName(tgcid);
      int gasGap         = m_tgcIdHelper->gasGap(tgcid);
      
      // Get position variables
      const Amg::Vector3D tgcGlobalPos  = tpd->globalPosition();
      float tgcGlobalPhi = tgcGlobalPos.phi();
      if(tgcGlobalPhi<0)tgcGlobalPhi+=2*M_PI;
      
      // Cut non-TGC-Endcap, get layer number
      if(tgcStationName<41 || tgcStationName>48)continue;
      // 41=T1F,42=T1E,43=T2F,44=T2E,45=T3F,46=T3E,47=T4F,48=T4E
      int layer = gasGap-1;
      if(tgcStationName==43||tgcStationName==44){
        layer+=3;
      }else if(tgcStationName==45||tgcStationName==46){
        layer+=5;
      }else if(tgcStationName==47||tgcStationName==48){
        layer+=7;
      }
    }// TGC PRD Collection
  }// TGC PRD Container
  
  return;
}// End of function

void
MdtVsTgcRawDataValAlg::maphists(const Trk::SegmentCollection *segmcollection,
                                const Muon::TgcPrepDataContainer *tgc_prepcontainer){//use old mdt segment
  ATH_MSG_DEBUG("inside maptgchits" );
  
  //////////////////////////////////////////////////////
  // Declare flags for data types present
  //bool MDTSegm[2]    = {false,false};
  //bool TGCMidHit[2]  = {false,false};
  //bool TGCEIFIHit[2] = {false,false};
  
  //int nMDTSegm[2]  = {0,0};
  //int nTGCPrep[2]  = {0,0};

  
  ///////////////////////////////////////////////////////////////////////////////////////////////
  // loop over MDT Segment Collection
  for(Trk::SegmentCollection::const_iterator s = segmcollection->begin();
      s!=segmcollection->end();
      ++s){
    // Get segm
    const Muon::MuonSegment *segm=dynamic_cast<const Muon::MuonSegment*>(*s);
    if (segm == 0) {
      ATH_MSG_ERROR( "no pointer to segm!!!" );
      break;
    }
    // Initialize variables
    int stationName=0;
    int nMdtMeas[4] = {0,0,0,0};
    bool isMdt=false, isEndcap=false;
    
    // Loop through contained ROTs and count used MDT and TGC stations
    for(unsigned int i=0;i<segm->numberOfContainedROTs();++i){
      // Get station information
      const Trk::RIO_OnTrack* rio = segm->rioOnTrack(i);
      Identifier id = rio->identify();
      stationName = int(m_mdtIdHelper->stationName(id));
      // Flag Segments with ROTs in the MDT & Endcap
      if(m_mdtIdHelper->is_mdt(id))isMdt=true;
      if(m_mdtIdHelper->isEndcap(id))isEndcap=true;
      // If ROT is MDT
      if((stationName==13)||(stationName==49)){nMdtMeas[0]++;}
      if((stationName==14)||(stationName==15)){nMdtMeas[1]++;}
      if((stationName==17)||(stationName==18)){nMdtMeas[2]++;}
      if((stationName==20)||(stationName==21)){nMdtMeas[3]++;}
    }
    if(!isMdt)continue;
    if(!isEndcap)continue;

    // Establish which MDT stations the Segment is in
    int nStations = 0;
    for(int jMDT=0;jMDT<4;jMDT++){
      if(nMdtMeas[jMDT]){
        nStations++;
      }
    }
    // Check that the Segment only occupies one Station
    if(nStations!=1)continue;

    // Get Position pointer and segmAC index
    const Amg::Vector3D segmGlobalPos = segm->globalPosition();
    int segmAC = (segmGlobalPos.z()<0);// a:0, c:1

    // Fill MDT data
    for(int jMDT=0;jMDT<4;jMDT++){// jMDT
      if(nMdtMeas[jMDT]){// If hits in this Station
        // Get position variables
        //float  segmGlobalRho  = abs(segmGlobalPos.perp());
        float  segmGlobalEta  = abs(segmGlobalPos.eta());
        float  segmGlobalPhi  = segmGlobalPos.phi();
        //float  segmGlobalThe  = segmGlobalPos.theta();
        //float  segmGlobalZ    = segmGlobalPos.z();
        if(segmGlobalPhi<0) segmGlobalPhi+=2*M_PI;
        //if(segmGlobalThe>M_PI/2) segmGlobalThe=M_PI-segmGlobalThe;
        /*
        // Get direction variables
        const Trk::GlobalDirection segmGlobalDir = segm->globalDirection();
        float  segmDirRho     = abs(segmGlobalDir.perp());
        float  segmDirEta     = abs(segmGlobalDir.eta());
        float  segmDirPhi     = segmGlobalDir.phi();
        float  segmDirThe     = segmGlobalDir.theta();
        float  segmDirZ       = segmGlobalDir.z();
        if(segmDirPhi<0) segmDirPhi+=2*M_PI;
        if(segmDirThe>M_PI/2) segmDirThe=M_PI-segmDirThe;
        
        // Normalized Dir-Pos sagittas
        float dRho_Pos_Dir = (segmGlobalRho/segmGlobalZ)-(segmDirRho/segmDirZ);
        float dEta_Pos_Dir = segmGlobalEta-segmDirEta;
        float dPhi_Pos_Dir = segmGlobalPhi-segmDirPhi;
        float dThe_Pos_Dir = segmGlobalThe-segmDirThe;
        if(dPhi_Pos_Dir<-M_PI)dPhi_Pos_Dir+=2*M_PI;
        if(dPhi_Pos_Dir> M_PI)dPhi_Pos_Dir-=2*M_PI;
        */
        // Flag event
        //MDTSegm[segmAC] = true;
        //nMDTSegm[segmAC]++;
        
        // Fill position histogram
        m_mdt_segmmap[segmAC][jMDT]->Fill(segmGlobalPhi, segmGlobalEta);
      }// If hits in Station
    }// jMDT
  }// MDT Segment Collection
  
  ///////////////////////////////////////////////////////////////////////////////////////////////
  //loop over TGC Prep Data container
  Muon::TgcPrepDataContainer::const_iterator tgc_it_end=tgc_prepcontainer->end();
  for( Muon::TgcPrepDataContainer::const_iterator tgc_it=tgc_prepcontainer->begin();
       tgc_it!=tgc_it_end;
       ++tgc_it){
    
    //loop over TGC Prep Data collection
    Muon::TgcPrepDataCollection::const_iterator tgc_itc_end=(*tgc_it)->end();
    for( Muon::TgcPrepDataCollection::const_iterator tgc_itc=(*tgc_it)->begin();
         tgc_itc!= tgc_itc_end;
         ++tgc_itc){
      const Muon::TgcPrepData* tpd=*tgc_itc;
      
      // Get detector information
      const MuonGM::TgcReadoutElement *tre = tpd->detectorElement();
      const std::string tgcStationType = tre->getStationType();
      
      // Get detector variables
      Identifier tgcid=(*tgc_itc)->identify();
      //int tgcAC=(tre->sideA()==false);//isNotAside a:0, c:1
      //int tgcFE=(tre->forward()==false);//isNotForward f:0, e:1
      //int tgcWS=(m_tgcIdHelper->isStrip(tgcid));//isStrip w=0, s=1
      int tgcStationName = m_tgcIdHelper->stationName(tgcid);
      //int tgcStationEta  = abs(tre->getStationEta());
      //int tgcStationPhi  = tre->getStationPhi();
      int gasGap         = m_tgcIdHelper->gasGap(tgcid);
      
      // Get position variables
      const Amg::Vector3D tgcGlobalPos  = tpd->globalPosition();
      //float tgcGlobalRho = abs(tgcGlobalPos.perp());
      //float tgcGlobalEta = abs(tgcGlobalPos.eta());
      float tgcGlobalPhi = tgcGlobalPos.phi();
      if(tgcGlobalPhi<0)tgcGlobalPhi+=2*M_PI;
      
      // Cut non-TGC-Endcap, get layer number
      if(tgcStationName<41 || tgcStationName>48)continue;
      // 41=T1F,42=T1E,43=T2F,44=T2E,45=T3F,46=T3E,47=T4F,48=T4E
      int layer = gasGap-1;
      if(tgcStationName==43||tgcStationName==44){
        layer+=3;
      }else if(tgcStationName==45||tgcStationName==46){
        layer+=5;
      }else if(tgcStationName==47||tgcStationName==48){
        layer+=7;
      }
      
      // Flag event and increment counter
      //if(tgcStationName==47 || tgcStationName==48)TGCEIFIHit[tgcAC] = true;
      //else TGCMidHit[tgcAC] = true;
      //nTGCPrep[tgcAC]++;
    }// TGC PRD Collection
  }// TGC PRD Container
  
  return;
}// End of function

// Finalize histograms which need post processing
void MdtVsTgcRawDataValAlg::maphistsfinalize(){
  
}// End of function
