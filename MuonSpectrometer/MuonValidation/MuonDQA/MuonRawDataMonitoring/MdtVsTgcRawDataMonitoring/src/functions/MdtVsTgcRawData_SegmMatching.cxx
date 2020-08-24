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

// Matches together segments from different stations into a track
void
MdtVsTgcRawDataValAlg::MatchMDTSegments(std::vector<const Muon::MuonSegment*> (&sortedSegments)[2][4],
                                        std::vector<const Muon::MuonSegment*> (&disqualifiedSegments)[2][4],
                                        std::vector<SegmTrack> (&matchedSegments)[2]){// Define Cuts:
  // Cut for matching Segments
  const float dRhoCutSegmentMatching = 1000;
  const float dPhiCutSegmentMatching = M_PI/8;
  const float dPhiCutSegmentDirectionChecking[4][4]={{     0,M_PI/8,   0.1,  0.01},
                                                     {M_PI/8,     0,M_PI/8,M_PI/8},
                                                     {  0.06,M_PI/8,     0,  0.01},
                                                     {  0.05,M_PI/8,  0.05,     0}};
  const float dTheCutSegmentDirectionChecking[4][4]={{     0,M_PI/8,  0.05,  0.06},
                                                     {M_PI/8,     0,M_PI/8,M_PI/8},
                                                     {  0.04,M_PI/8,     0, 0.005},
                                                     {  0.02,M_PI/8, 0.005,     0}};
  // Loop over sides
  for(int i=0;i<2;i++){// AC
    bool skipSegm;int nDisqualifiedSegm; // used when checking the disqualified list for a segment
    //bool HasStationMatchSegm[4] = {false, false, false, false};// flags for whether the there are segments in each MDT station which were included in tracks
    //bool HasMatchedTrack = false;// flag for whether a track was found on current side
    
    for(int jMDT1=3;jMDT1>=0;jMDT1--){// MDT Stations in reverse]
      // Get number of segments
      int nSegm1=sortedSegments[i][jMDT1].size();
      
      // Loop over Segments in Station
      for(int n1=0; n1<nSegm1;n1++){
        // Get segment
        const Muon::MuonSegment *segm1 = sortedSegments[i][jMDT1].at(n1);
        
        // Check disqualifiedSegments for current segment
        skipSegm=false;
        nDisqualifiedSegm=disqualifiedSegments[i][jMDT1].size();
        for(int ndis=0;ndis<nDisqualifiedSegm;ndis++)if(segm1==disqualifiedSegments[i][jMDT1].at(ndis))skipSegm=true;
        if(skipSegm)continue;
        
        // Get position variables
        const Amg::Vector3D segm1Pos = segm1->globalPosition();
        
        float segm1PosPhi = segm1Pos.phi();
        float segm1PosZ   = segm1Pos.z();
        if(segm1PosPhi<0)segm1PosPhi+=2*M_PI;
        Amg::Vector3D segm1PosZunit(segm1Pos/std::abs(segm1PosZ));
        Amg::Vector3D segm1Dir = segm1->globalDirection();
        
	float segm1DirThe = segm1Dir.theta();
        float segm1DirPhi = segm1Dir.phi();
        if(segm1DirThe>M_PI/2) segm1DirThe=M_PI-segm1DirThe;
        if(segm1DirPhi<0) segm1DirPhi+=2*M_PI;
        
        // Initialise segment matching variables
        bool stationMatchFound[4] = {false,false,false,false};
        std::vector<const Muon::MuonSegment*> matchingSegments[4];
        for(int jMDT2=0;jMDT2<4;jMDT2++)matchingSegments[jMDT2] = std::vector<const Muon::MuonSegment*>();
        matchingSegments[jMDT1].push_back(segm1);
        int nStationMatch=0;
        
        for(int jMDT2=3;jMDT2>=0;jMDT2--){// MDT Station in reverse
          // Get number of segments
          int nSegm2=sortedSegments[i][jMDT2].size();

          // Loop over Segments in Station
          for(int n2=0; n2<nSegm2;n2++){
            // Get segment
            const Muon::MuonSegment *segm2 = sortedSegments[i][jMDT2].at(n2);
            if(segm1==segm2)continue; //do not compare same segments
            
            // Check disqualifiedSegments for current segment
            skipSegm = false;
            nDisqualifiedSegm=disqualifiedSegments[i][jMDT2].size();
            for(int ndis=0;ndis<nDisqualifiedSegm;ndis++)if(segm1==disqualifiedSegments[i][jMDT2].at(ndis))skipSegm=true;
            if(skipSegm)continue;
            
            //////////////////////////////////////////////////////
            // Position Cut
            // Fill position variables
            const Amg::Vector3D segm2Pos = segm2->globalPosition();
           
	    float segm2PosRho = std::abs(segm2Pos.perp());
            float segm2PosPhi = segm2Pos.phi();
            float segm2PosThe = segm2Pos.theta();
            float segm2PosZ   = segm2Pos.z();
            if(segm2PosThe>M_PI/2) segm2PosThe=M_PI-segm2PosThe;
            if(segm2PosPhi<0)segm2PosPhi+=2*M_PI;
            Amg::Vector3D segm2PosZunit(segm2Pos/std::abs(segm2PosZ));
            
            // Apply preliminary phi cut between segm1 and segm2 positions
            float dPhi_Segm1_Segm2 = segm1PosPhi-segm2PosPhi;
            if(dPhi_Segm1_Segm2<-M_PI)dPhi_Segm1_Segm2+=2*M_PI;
            if(dPhi_Segm1_Segm2> M_PI)dPhi_Segm1_Segm2-=2*M_PI;
            if(std::abs(dPhi_Segm1_Segm2)>dPhiCutSegmentMatching)continue;
            
            // Extrapolate segm1 position to segm2's Z position
            float dZ = std::abs(segm2PosZ)-std::abs(segm1PosZ);
            Amg::Vector3D extrPos(segm1Pos+(segm1PosZunit*dZ));
            
	    float extrPosRho = std::abs(extrPos.perp());
            float extrPosThe = extrPos.theta();
            float extrPosPhi = extrPos.phi();
            if(extrPosThe>M_PI/2) extrPosThe=M_PI-extrPosThe;
            if(extrPosPhi<0)extrPosPhi+=2*M_PI;
            
            // Get differences between extrapolated and segm2 positions
            float dRho_Extr_Segm2 = extrPosRho-segm2PosRho;
            float dPhi_Extr_Segm2 = extrPosPhi-segm2PosPhi;
            float dThe_Extr_Segm2 = extrPosThe-segm2PosThe;
            if(dPhi_Extr_Segm2<-M_PI)dPhi_Extr_Segm2+=2*M_PI;
            if(dPhi_Extr_Segm2> M_PI)dPhi_Extr_Segm2-=2*M_PI;
            
            // Fill difference histograms
            if(m_mdt_segmmatchsag[i][jMDT1][jMDT2][0]) m_mdt_segmmatchsag[i][jMDT1][jMDT2][0]->Fill(dRho_Extr_Segm2);
            if(m_mdt_segmmatchsag[i][jMDT1][jMDT2][2]) m_mdt_segmmatchsag[i][jMDT1][jMDT2][2]->Fill(dPhi_Extr_Segm2);
            if(m_mdt_segmmatchsag[i][jMDT1][jMDT2][3]) m_mdt_segmmatchsag[i][jMDT1][jMDT2][3]->Fill(dThe_Extr_Segm2);
            
            // Cut segments (segm2) which are outside difference cuts
            if(std::abs(dPhi_Extr_Segm2)>dPhiCutSegmentMatching)continue;
            if(std::abs(dRho_Extr_Segm2)>dRhoCutSegmentMatching)continue;
            
            //////////////////////////////////////////////////////
            // Direction Cut
            // Fill direction variables
            //const Trk::GlobalDirection segm2Dir = segm2->globalDirection();
            const Amg::Vector3D segm2Dir = segm2->globalDirection();
            
	    float segm2DirThe = segm2Dir.theta();
            float segm2DirPhi = segm2Dir.phi();
            if(segm2DirThe>M_PI/2) segm2DirThe=M_PI-segm2DirThe;
            if(segm2DirPhi<0) segm2DirPhi+=2*M_PI;
            
            // Vector Method
            // Make vector between segment positions
            //Trk::GlobalDirection segmVector;
            Amg::Vector3D segmVector;
            
	    if(jMDT2>jMDT1)segmVector = segm2Pos-segm1Pos;
            else           segmVector = segm1Pos-segm2Pos;
            float segmVecThe = segmVector.theta();
            float segmVecPhi = segmVector.phi();
            if(segmVecThe>M_PI/2) segmVecThe=M_PI-segmVecThe;
            if(segmVecPhi<0) segmVecPhi+=2*M_PI;
            
            // Get difference between vector direction and segment directions
            float dThe_Vec_Segm1 = segmVecThe-segm1DirThe;
            float dPhi_Vec_Segm1 = segmVecPhi-segm1DirPhi;
            if(dPhi_Vec_Segm1<-M_PI)dPhi_Vec_Segm1+=2*M_PI;
            if(dPhi_Vec_Segm1> M_PI)dPhi_Vec_Segm1-=2*M_PI;
            float dThe_Vec_Segm2 = segmVecThe-segm2DirThe;
            float dPhi_Vec_Segm2 = segmVecPhi-segm2DirPhi;
            if(dPhi_Vec_Segm2<-M_PI)dPhi_Vec_Segm2+=2*M_PI;
            if(dPhi_Vec_Segm2> M_PI)dPhi_Vec_Segm2-=2*M_PI;
            
            // Fill histograms
            if(jMDT1>jMDT2){
              if(m_mdt_trackchecksag[i][jMDT2][jMDT1][2][0]) m_mdt_trackchecksag[i][jMDT2][jMDT1][2][0]->Fill(dPhi_Vec_Segm2);
              if(m_mdt_trackchecksag[i][jMDT2][jMDT1][3][0]) m_mdt_trackchecksag[i][jMDT2][jMDT1][3][0]->Fill(dThe_Vec_Segm2);
              if(m_mdt_trackchecksag[i][jMDT2][jMDT1][2][1]) m_mdt_trackchecksag[i][jMDT2][jMDT1][2][1]->Fill(dPhi_Vec_Segm1);
              if(m_mdt_trackchecksag[i][jMDT2][jMDT1][3][1]) m_mdt_trackchecksag[i][jMDT2][jMDT1][3][1]->Fill(dThe_Vec_Segm1);
            }
            else if(jMDT1<jMDT2){
              if(m_mdt_trackchecksag[i][jMDT1][jMDT2][2][0]) m_mdt_trackchecksag[i][jMDT1][jMDT2][2][0]->Fill(dPhi_Vec_Segm1);
              if(m_mdt_trackchecksag[i][jMDT1][jMDT2][3][0]) m_mdt_trackchecksag[i][jMDT1][jMDT2][3][0]->Fill(dThe_Vec_Segm1);
              if(m_mdt_trackchecksag[i][jMDT1][jMDT2][2][1]) m_mdt_trackchecksag[i][jMDT1][jMDT2][2][1]->Fill(dPhi_Vec_Segm2);
              if(m_mdt_trackchecksag[i][jMDT1][jMDT2][3][1]) m_mdt_trackchecksag[i][jMDT1][jMDT2][3][1]->Fill(dThe_Vec_Segm2);
            }
            
            // Direction Comparison Method
            float dTheDir_Segm1_Segm2 = segm1DirThe-segm2DirThe;
            float dPhiDir_Segm1_Segm2 = segm1PosPhi-segm2PosPhi;
            if(dPhiDir_Segm1_Segm2<-M_PI)dPhiDir_Segm1_Segm2+=2*M_PI;
            if(dPhiDir_Segm1_Segm2> M_PI)dPhiDir_Segm1_Segm2-=2*M_PI;
            if(m_mdt_trackdirdirsag[i][jMDT1][jMDT2][2]) m_mdt_trackdirdirsag[i][jMDT1][jMDT2][2]->Fill(dTheDir_Segm1_Segm2);
            if(m_mdt_trackdirdirsag[i][jMDT1][jMDT2][3]) m_mdt_trackdirdirsag[i][jMDT1][jMDT2][3]->Fill(dPhiDir_Segm1_Segm2);
            // Cut using Vector Method
            if(dPhi_Vec_Segm1>dPhiCutSegmentDirectionChecking[jMDT1][jMDT2] ||
               dThe_Vec_Segm1>dTheCutSegmentDirectionChecking[jMDT1][jMDT2] ||
               dPhi_Vec_Segm2>dPhiCutSegmentDirectionChecking[jMDT2][jMDT1] ||
               dThe_Vec_Segm2>dTheCutSegmentDirectionChecking[jMDT2][jMDT1]) continue;
            
            // Match is found, increment counter and assign segm2 to match found array
            matchingSegments[jMDT2].push_back(segm2);
          }// nSegms2
          
          if(matchingSegments[jMDT2].size()==1){
            stationMatchFound[jMDT2]=true;
            nStationMatch++;
          }
        }// Reverse MDT Stations2
        
        // If matches found add to matchedSegments and disqualify all segments in array
        if(nStationMatch>1){
          //HasMatchedTrack=true;
          const Muon::MuonSegment *segmArray[4] = {0,0,0,0};
          for(int jMDT2=0;jMDT2<4;jMDT2++){
            if(stationMatchFound[jMDT2]){
              segmArray[jMDT2]=matchingSegments[jMDT2].at(0);
              //HasStationMatchSegm[jMDT2]=true;
              disqualifiedSegments[i][jMDT2].push_back(matchingSegments[jMDT2].at(0));
            }
          }
          SegmTrack newTrack(segmArray);
          matchedSegments[i].push_back(newTrack);
        }// If Matched Track Found
        
      }// nSegms1
    }// Reverse MDT Stations1
    
  }// AC
  
  return;
}// End of function
