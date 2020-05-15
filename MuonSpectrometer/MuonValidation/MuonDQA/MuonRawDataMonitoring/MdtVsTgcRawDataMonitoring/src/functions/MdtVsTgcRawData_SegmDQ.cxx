/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include <inttypes.h>

#include <sstream>
#include <algorithm>
#include <fstream>

// Checks that segments have sufficient DQ
// Segments with insuffient DQ are added to disqualifiedSegments and subsequently ignored by the other functions
void
MdtVsTgcRawDataValAlg::DQCheckMDTSegments(std::vector<const Muon::MuonSegment*> (&sortedSegments)[2][4],
                                          std::vector<const Muon::MuonSegment*> (&disqualifiedSegments)[2][4]){
  // Define Cuts:
  // Cut for miniumum number of measurements necessary to use Segments
  const int   nMeasCutMdt[4]      = { 4, 2, 4, 5};             //[MDTStation]
  const int   nMeasCutTgcStrip[4] = { 0, 0, 1, 0};             //[MDTStation]
  // Cut for differences between direction and position vectors
  const float dPhiCutPosDir[4]    = {0.05,0.05,0.08,0.1 };
  const float dTheCutPosDir[4]    = { 0.2, 0.4, 0.6,0.5 };
  
  // Loop over sides
  for(int i=0;i<2;i++){// AC
    bool skipSegm;int nDisqualifiedSegm; // used when checking the disqualified list for a segment
    bool segmDisqual;                    // used to flag a segment which has been found DQ insufficient
    //bool HasStationDQSegm[4] = {false, false, false, false};// flags for whether the there are segments in each MDT station with sufficienct DQ
    
    for(int jMDT=0;jMDT<4;jMDT++){// MDT Stations
      // Get number of segments
      int nSegm=sortedSegments[i][jMDT].size();
      
      // Loop over segments in this MDT Station
      for(int n=0; n<nSegm;n++){
        segmDisqual = false;// set disqualified flag to false
        // Get segment
        const Muon::MuonSegment *segm=sortedSegments[i][jMDT].at(n);
        if(segm==0)continue;// Cut empty entries
        
        // Check disqualifiedSegments for current segment
        skipSegm=false;
        nDisqualifiedSegm=disqualifiedSegments[i][jMDT].size();
        for(int ndis=0;ndis<nDisqualifiedSegm;ndis++)if(segm==disqualifiedSegments[i][jMDT].at(ndis))skipSegm=true;
        if(skipSegm)continue;
        
        /////////////////////////////////////////////////////////////////////////
        // Apply nHits cuts to segments
        /////////////////////////////////////////////////////////////////////////
        int stationName=0;
        int nMdtMeas = 0;       //             nMDTHits in this Segment
        int nTgcMeas[2] = {0,0};// [WireStrip] nTGCHits in this Segment
        
        // Loop through contained ROTs and identify used stations
        for(unsigned int iROT=0; iROT<segm->numberOfContainedROTs(); ++iROT) {
          const Trk::RIO_OnTrack* rio = segm->rioOnTrack(iROT);
	  if(!rio){
	    ATH_MSG_DEBUG("no RIO");
	    continue;
	  }
          Identifier id = rio->identify();
          stationName = int(m_idHelperSvc->mdtIdHelper().stationName(id));
          int isStrip = m_idHelperSvc->tgcIdHelper().isStrip(id);
          
          if((stationName==41)||(stationName==42))nTgcMeas[isStrip]++;// TGC
          if((stationName==43)||(stationName==44))nTgcMeas[isStrip]++;// TGC
          if((stationName==45)||(stationName==46))nTgcMeas[isStrip]++;// TGC
          if((stationName==47)||(stationName==48))nTgcMeas[isStrip]++;// TGC
          
          if((stationName==13)||(stationName==49))nMdtMeas++;// MDT
          if((stationName==14)||(stationName==15))nMdtMeas++;// MDT
          if((stationName==17)||(stationName==18))nMdtMeas++;// MDT
          if((stationName==20)||(stationName==21))nMdtMeas++;// MDT
        }
        
        // Cut Segments with insufficient numbers of hits in the stations
        if(nMdtMeas<nMeasCutMdt[jMDT]||nTgcMeas[1]<nMeasCutTgcStrip[jMDT]){
          segmDisqual = true;
        }
        
        /////////////////////////////////////////////////////////////////////////
        // Apply Direction-Position cuts to segments
        /////////////////////////////////////////////////////////////////////////
        // Get Position and Direction Variables
       // const Trk::GlobalPosition segmGlobalPos  = segm->globalPosition();
        const Amg::Vector3D segmGlobalPos  = segm->globalPosition();
       
        float  segmPosPhi  = segmGlobalPos.phi();
        float  segmPosThe  = segmGlobalPos.theta();
        if(segmPosPhi<0) segmPosPhi+=2*M_PI;
        if(segmPosThe>M_PI/2) segmPosThe=M_PI-segmPosThe;
        //const Trk::GlobalDirection segmGlobalDir = segm->globalDirection();
        const Amg::Vector3D segmGlobalDir = segm->globalDirection();
        
	float  segmDirPhi     = segmGlobalDir.phi();
        float  segmDirThe     = segmGlobalDir.theta();
        if(segmDirPhi<0) segmDirPhi+=2*M_PI;
        if(segmDirThe>M_PI/2) segmDirThe=M_PI-segmDirThe;
        
        // Get Differences between Position and Direction vectors
        float dPhi_Pos_Dir = segmPosPhi-segmDirPhi;
        float dThe_Pos_Dir = segmPosThe-segmDirThe;
        if(dPhi_Pos_Dir<-M_PI)dPhi_Pos_Dir+=2*M_PI;
        if(dPhi_Pos_Dir> M_PI)dPhi_Pos_Dir-=2*M_PI;
        
        if(!segmDisqual){
          if(m_mdt_segmposdirsag[i][jMDT][2]) m_mdt_segmposdirsag[i][jMDT][2]->Fill(dPhi_Pos_Dir);
          if(m_mdt_segmposdirsag[i][jMDT][3]) m_mdt_segmposdirsag[i][jMDT][3]->Fill(dThe_Pos_Dir);
        }

        // Cut Segments with too great a difference between position and direction vectors
        if(std::abs(dPhi_Pos_Dir)>dPhiCutPosDir[jMDT]||std::abs(dThe_Pos_Dir)>dTheCutPosDir[jMDT]){
          segmDisqual = true;
        }
          
        /////////////////////////////////////////////////////////////////////////
        // Add disqualified segments to the disqualified list
        /////////////////////////////////////////////////////////////////////////
        if(segmDisqual){
          disqualifiedSegments[i][jMDT].push_back(segm);
        }
        else{
          //HasStationDQSegm[jMDT]=true;// Flag event as having a DQ sufficient segment in the current station
        }
      }// Segments in station
    }// MDT Stations
  }// AC
  
  return;
}// End of function
