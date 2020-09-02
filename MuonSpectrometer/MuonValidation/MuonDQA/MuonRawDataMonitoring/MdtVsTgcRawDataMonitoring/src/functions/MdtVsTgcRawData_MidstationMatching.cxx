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

// Use Midstation Segments to look for additional Midstation Segments
// Only the Midstation covers some trajectories so normal SegmTracks cannot be constructed
void
MdtVsTgcRawDataValAlg::MidstationOnlyCheck(std::vector<const Muon::MuonSegment*> (&sortedSegments)[2][4],
                                           std::vector<const Muon::MuonSegment*> (&disqualifiedSegments)[2][4],
                                           const Muon::TgcPrepDataContainer *tgc_prepcontainer){
  // Define Cuts:
  // Cut for matching Segments
  const float dRhoCutSegmentMatching = 1000;
  const float dPhiCutSegmentMatching = M_PI/8;
  
  // Cuts for number of Mdt Measurements required for Midstation only track
  const int nMeasCutMdtMidstation = 5;
  const int nMeasCutTGCMidPRD[2]  = {2,2};
  
  // Loose cut for PRD and Extrapolated
  const float dPhiCut_Loose    = M_PI/8;
  // Cut for Global Position  Efficiencies
  const float dPhiCutGlobal[2] = {M_PI/24,M_PI/12};//[WireStrip]
  const float dRhoCutGlobal[2] = {   0.08,    0.5};//[WireStrip]
  // Cut for Sector Efficiencies
  const float dPhiCutSector[2] = {    0.2,    0.1};//[WireStrip]
  const float dRhoCutSector[2] = {    300,   3000};//[WireStrip]
  // Cut for TgcPrepData comparison
  const float dPhiCutTPD[2]  = {   0.15,   0.02};//[WireStrip]
  const float dRhoCutTPD[2]  = {    150,   3000};//[WireStrip]
  
  // Loop over sides
  for(int i=0;i<2;i++){// AC
    // Number of Segments found which passed all cuts
    int  nValidatedSegm              = 0;
    
    // Following "Fill" variables are only used if one segment passed all cuts
    // Flags for whether different stations can be checked by the segment
    //bool canCheckGlobalFill[4]       = {0, 0, 0, 0};
    bool canCheckSectorFill[4]       = {0, 0, 0, 0};
    // Segment Global Position variables
    // float posThetaFill               = 0;
    // float posPhiFill                 = 0;
    // Segment Sector Position
    int TGCstation_StationFEFill[4]  = {-1,-1,-1,-1};// [TGCStation]
    int TGCstation_StationEtaFill[4] = { 0, 0, 0, 0};// [TGCStation]
    int TGCstation_StationPhiFill[4] = { 0, 0, 0, 0};// [TGCStation]
    // Hit registered arrays
    // bool hitregisteredFill[9][2]       = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
    bool sectorhitregisteredFill[9][2] = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
    
    bool skipSegm;int nDisqualifiedSegm; // used when checking the disqualified list for a segment
    
    // Make copy of disqualifiedSegment vector array
    std::vector<const Muon::MuonSegment*> copyDisqualifiedSegments;
    nDisqualifiedSegm=disqualifiedSegments[i][2].size();
    for(int ndis=0;ndis<nDisqualifiedSegm;ndis++)copyDisqualifiedSegments.push_back(disqualifiedSegments[i][2].at(ndis));
    
    ////////////////////////////////////////////////////////////////////////
    // Apply preliminary nMdtMeas Cut
    int nSegm = sortedSegments[i][2].size();
    for(int n0=0; n0<nSegm;n0++){
      // Get segment
      const Muon::MuonSegment *segm0 = sortedSegments[i][2].at(n0);
      
      // Check copyDisqualifiedSegments for current segment
      skipSegm=false;
      nDisqualifiedSegm=copyDisqualifiedSegments.size();
      for(int ndis=0;ndis<nDisqualifiedSegm;ndis++)if(segm0==copyDisqualifiedSegments.at(ndis))skipSegm=true;
      if(skipSegm)continue;
      
      // Apply nMDTMeasurements Cut to Segment Collection
      int stationName = 0;
      int nMdtMeas    = 0;
      // Loop through contained ROTs and identify used stations
      for(unsigned int iROT=0; iROT<segm0->numberOfContainedROTs(); ++iROT) {
        const Trk::RIO_OnTrack* rio = segm0->rioOnTrack(iROT);
	if(!rio) continue;
        Identifier id = rio->identify();
        stationName = int(m_idHelperSvc->mdtIdHelper().stationName(id));
        
        if((stationName==17)||(stationName==18))nMdtMeas++;// MDT
      }
      // Cut Segments with insufficient numbers of hits in the stations
      if(nMdtMeas<nMeasCutMdtMidstation){
        copyDisqualifiedSegments.push_back(segm0);
      }
    }
    
    // Loop over Segments in MidStation
    for(int n1=0; n1<nSegm;n1++){
      ////////////////////////////////////////////////////////////////////////
      // Start operation on Segment
      // Get segment
      const Muon::MuonSegment *segm1 = sortedSegments[i][2].at(n1);
      
      // Check copyDisqualifiedSegments for current segment
      skipSegm=false;
      nDisqualifiedSegm=copyDisqualifiedSegments.size();
      for(int ndis=0;ndis<nDisqualifiedSegm;ndis++)if(segm1==copyDisqualifiedSegments.at(ndis))skipSegm=true;
      if(skipSegm)continue;
      
      // Flag for Grouping cut
      bool failedGroupingCut = false;
      // Flags for whether different stations can be checked by the segment
      bool canCheckGlobal[4] = {true, true, true, false};
      bool canCheckSector[4] = {true, true, true, false};
      
      // Get position variables
      //const Trk::GlobalPosition segm1Pos = segm1->globalPosition();
      const Amg::Vector3D segm1Pos = segm1->globalPosition();

      float segm1PosPhi = segm1Pos.phi();
      float segm1PosThe = segm1Pos.theta();
      float segm1PosZ   = segm1Pos.z();
      if(segm1PosPhi<0)segm1PosPhi+=2*M_PI;
      if(segm1PosThe>M_PI/2) segm1PosThe=M_PI-segm1PosThe;
      Amg::Vector3D segm1PosZunit(segm1Pos/std::abs(segm1PosZ));
      
      
      ////////////////////////////////////////////////////////////////////////
      // Apply nTGCStripMeasurements Cut to canCheck arrays
      int stationName   = 0;
      int nTGCStrips[4] = { 0, 0, 0, 0};
      // Loop through contained ROTs and identify used stations
      for(unsigned int iROT=0; iROT<segm1->numberOfContainedROTs(); ++iROT){
        const Trk::RIO_OnTrack* rio = segm1->rioOnTrack(iROT);
	if(!rio) continue;
        Identifier id = rio->identify();
        stationName = int(m_idHelperSvc->mdtIdHelper().stationName(id));
        bool isStrip = m_idHelperSvc->tgcIdHelper().isStrip(id);
        
        if(((stationName==41)||(stationName==42))&&isStrip)nTGCStrips[0]++;// TGC
        if(((stationName==43)||(stationName==44))&&isStrip)nTGCStrips[1]++;// TGC
        if(((stationName==45)||(stationName==46))&&isStrip)nTGCStrips[2]++;// TGC
        if(((stationName==47)||(stationName==48))&&isStrip)nTGCStrips[3]++;// TGC
      }
    
      // Don't check mid-stations when there are no strips in other mid-stations
      if((nTGCStrips[1]==0)&&(nTGCStrips[2]==0)){canCheckSector[0]=false;canCheckGlobal[0]=false;}
      if((nTGCStrips[0]==0)&&(nTGCStrips[2]==0)){canCheckSector[1]=false;canCheckGlobal[1]=false;}
      if((nTGCStrips[0]==0)&&(nTGCStrips[1]==0)){canCheckSector[2]=false;canCheckGlobal[2]=false;}
            
      
      ////////////////////////////////////////////////////////////////////////
      // Do Grouping Cut
      // Ignore any segments which are too close to any other segments not in the original disqualified list
      for(int n2=0; n2<nSegm;n2++){
        if(n1==n2)continue; // don't compare segment to itself
        // Get segment
        const Muon::MuonSegment *segm2 = sortedSegments[i][2].at(n2);
        
        // Check disqualifiedSegments for current segment
        skipSegm=false;
        nDisqualifiedSegm=disqualifiedSegments[i][2].size();
        for(int ndis=0;ndis<nDisqualifiedSegm;ndis++)if(segm1==disqualifiedSegments[i][2].at(ndis))skipSegm=true;
        if(skipSegm)continue;
        
        // Get position variables
        const Amg::Vector3D segm2Pos = segm2->globalPosition(); 
	float segm2PosRho = std::abs(segm2Pos.perp());
        float segm2PosPhi = segm2Pos.phi();
        float segm2PosZ   = segm2Pos.z();
        if(segm2PosPhi<0)segm2PosPhi+=2*M_PI;
        Amg::Vector3D segm2PosZunit(segm2Pos/std::abs(segm2PosZ));
        
        // Apply preliminary phi cut between segm1 and segm2 positions
        float dPhi_Segm1_Segm2 = segm1PosPhi-segm2PosPhi;
        if(dPhi_Segm1_Segm2<-M_PI)dPhi_Segm1_Segm2+=2*M_PI;
        if(dPhi_Segm1_Segm2> M_PI)dPhi_Segm1_Segm2-=2*M_PI;
        if(std::abs(dPhi_Segm1_Segm2)<dPhiCutSegmentMatching){
          failedGroupingCut=true;
          break;
        }
        
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
        if(dPhi_Extr_Segm2<-M_PI)dPhi_Extr_Segm2+=2*M_PI;
        if(dPhi_Extr_Segm2> M_PI)dPhi_Extr_Segm2-=2*M_PI;
        
        // Cut segments (segm1) which are inside difference cuts
        if((std::abs(dPhi_Extr_Segm2)<dPhiCutSegmentMatching)||
           (std::abs(dRho_Extr_Segm2)<dRhoCutSegmentMatching)){
          failedGroupingCut=true;
          break;
        }
      }// nSegm2
      if(failedGroupingCut)continue;
      
      ////////////////////////////////////////////////////////////////////////
      // Check track against TGC Sectors to find which it passes
      // Initialize variables for TRE array search
      int TGCStationNames[8]      ={41, 42, 43, 44, 45, 46, 47, 48};
      int TGCstation_StationFE[4] ={-1,-1,-1,-1};// [TGCStation]
      int TGCstation_StationEta[4]={ 0, 0, 0, 0};// [TGCStation]
      int TGCstation_StationPhi[4]={ 0, 0, 0, 0};// [TGCStation]
      int nStationMatch[4]        ={ 0, 0, 0, 0};// [TGCStation]
      
      // Loop through TRE array, finding sectors which match the track in each layer
      for(int stationnameindex=0; stationnameindex<6; stationnameindex++){// Station {T1F,T1E,T2F,T2E,T3F,T3E}
        // Skip stations which don't have sufficient Segments to run efficiency check
        int stationName = TGCStationNames[stationnameindex];
        int stationIndex= TGCstationname2stationindex(stationName);
        
        // Loop over StationEta&StationPhi
        for(int stationeta=1; stationeta<=8; stationeta++){// AbsStationEta
          for(int stationphi=1; stationphi<=48; stationphi++){// StationPhi
            // Cut Station EtaPhi combinations with no TGC element
            if(m_TREarray[stationnameindex][i][stationeta][stationphi]==0)continue;
            const MuonGM::TgcReadoutElement *tre=m_TREarray[stationnameindex][i][stationeta][stationphi];
            
            // Extrapolate position from nearest Station's Segment to Sector's Z
            float sectorZ=tre->globalPosition().z();
            float dZ_sector=std::abs(sectorZ)-std::abs(segm1PosZ);
            //Trk::GlobalPosition sectorExtrapolatedPos = segm1Pos+(segm1PosZunit*dZ_sector);
            Amg::Vector3D sectorExtrapolatedPos = segm1Pos+(segm1PosZunit*dZ_sector);
            
            // Convert extrapolated position to local position on the Sector
            Identifier sector_id=tre->identify();
            //const HepGeom::Point3D<double> sectorLocalPos3D=tre->globalToLocalCoords(sectorExtrapolatedPos, sector_id);
            const Amg::Vector3D sectorLocalPos3D=tre->globalToLocalCoords(sectorExtrapolatedPos, sector_id); 
	    //Trk::LocalPosition sectorLocalPos2D(sectorLocalPos3D.y(),sectorLocalPos3D.z());
	    Amg::Vector2D sectorLocalPos2D(sectorLocalPos3D.y(),sectorLocalPos3D.z());
            
            double avWidth = (tre->longWidth()+tre->shortWidth())/2;
            //double dWidth  = (tre->longWidth()-tre->shortWidth());
            double length  = tre->length();
            
            // Cut sectors which the track does not go through the central 80% of (avoids double counts)
            double tol1=-0.1*(avWidth/2);
            double tol2=-0.1*(length/2);
            
            bool insideSectorBounds=tre->bounds().inside(sectorLocalPos2D,tol1,tol2);
            if(!insideSectorBounds)continue;
            // Assign values to matching station variables
            if(stationIndex<0) continue;
            TGCstation_StationFE[stationIndex]= (tre->forward()==false);
            TGCstation_StationEta[stationIndex]=stationeta;
            TGCstation_StationPhi[stationIndex]=stationphi;
            nStationMatch[stationIndex]++;
          }// StationPhi
        }// StationEta
      }// StationName
      
      // Don't check stations that don't have exactly 1 match for this track
      for(int jTGC=0;jTGC<4;jTGC++){// TGC Station
        if(nStationMatch[jTGC]==0){
          canCheckSector[jTGC]=false;
        }
        else if(nStationMatch[jTGC]>1){
          canCheckSector[jTGC]=false;
                // Should be impossible, but happens due a problem with the bounds().inside function
                // commenting out until this bug is resolved
          //m_log << MSG::WARNING << "MidstationOnly: Number of matches for TGC" << jTGC+1 << " is " << nStationMatch[jTGC] << endl;
        }
      }// TGC Station
      
      // Cut Segment if no stations can be checked
      if((!canCheckGlobal[0])&&(!canCheckGlobal[1])&&(!canCheckGlobal[2])&&(!canCheckGlobal[3]))continue;
      ////////////////////////////////////////////////////////////////////////
      // Check which PRD matches Segm1
      // Initialise hit registered arrays
      bool sectorhitregistered[9][2] = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
      std::vector<const Muon::TgcPrepData*> tpdVector[2];
      
      // Loop over TGC Prep Data container
      Muon::TgcPrepDataContainer::const_iterator prepit_end=tgc_prepcontainer->end();
      for( Muon::TgcPrepDataContainer::const_iterator prepit=tgc_prepcontainer->begin();
           prepit!=prepit_end;
           ++prepit){
        
        //loop over TGC Prep Data collection
        Muon::TgcPrepDataCollection::const_iterator prepitc_end=(*prepit)->end();
        for( Muon::TgcPrepDataCollection::const_iterator prepitc=(*prepit)->begin();
             prepitc!= prepitc_end;
             ++prepitc){
          // Get PRD and variables
          const Muon::TgcPrepData* tpd=*prepitc;
          const MuonGM::TgcReadoutElement *tre = tpd->detectorElement();
          const std::string stationType = tre->getStationType();
          
          // Get id values
          Identifier tgcid=(*prepitc)->identify();
          int tgcAC=(tre->sideA()==false);//isNotAside a:0, c:1
          int tgcFE=(tre->forward()==false);//isNotForward f:0, e:1
          int tgcWS=(m_idHelperSvc->tgcIdHelper().isStrip(tgcid));//isStrip w=0, s=1
          int stationName = m_idHelperSvc->tgcIdHelper().stationName(tgcid);
          int stationEta  = std::abs(tre->getStationEta());
          int stationPhi  = tre->getStationPhi();
          int gasGap      = m_idHelperSvc->tgcIdHelper().gasGap(tgcid);
          
          // Cut hits except those from same side Midstation
          if(tgcAC!=i) continue;
          if(stationName>46 || stationName<41) continue;
          
          // Get layer number and stationIndex
          int layer        = TGCgetlayer(stationName,gasGap);
          int stationIndex = TGCstationname2stationindex(stationName);
          if(stationIndex==3)continue;
          
          // Get position variables
          const Amg::Vector3D prdPos = tpd->globalPosition(); 
	  float tgcRho = std::abs(prdPos.perp());
          float tgcPhi = prdPos.phi();
          float tgcZ   = prdPos.z();
          if(tgcPhi<0)tgcPhi+=2*M_PI;
          
          // Extrapolate Segm1 to PRD Z position
          float dZ = std::abs(tgcZ) - std::abs(segm1PosZ);
          Amg::Vector3D tgcExtrapolatedPos = ((segm1Pos)+((segm1PosZunit)*dZ));
          
          // Get extrapolated variables
          float tgcExtrRho = std::abs(tgcExtrapolatedPos.perp());
          float tgcExtrPhi = tgcExtrapolatedPos.phi();
          if(tgcExtrPhi<0)tgcExtrPhi+=2*M_PI;
          
          // Get differences between extrapolated and segm2 positions
          float dRho = tgcRho-tgcExtrRho;
          float dPhi = tgcPhi-tgcExtrPhi;
          if(dPhi<-M_PI)dPhi+=2*M_PI;
          if(dPhi> M_PI)dPhi-=2*M_PI;
          
          // Pass through loose phi cut to eliminate some noise
          if(std::abs(dPhi)<dPhiCut_Loose){
            // Fill PRD sagitta histograms
            if(m_mvt_extrprdsag2[i][stationIndex][tgcFE][tgcWS][0]) m_mvt_extrprdsag2[i][stationIndex][tgcFE][tgcWS][0]->Fill(dRho);
            if(m_mvt_extrprdsag2[i][stationIndex][tgcFE][tgcWS][2]) m_mvt_extrprdsag2[i][stationIndex][tgcFE][tgcWS][2]->Fill(dPhi);
            
            // Do Global check
            if(canCheckGlobal[stationIndex]){
              float dRhoCut = dRhoCutGlobal[tgcWS]*tgcExtrRho;
              if(std::abs(dPhi)<dPhiCutGlobal[tgcWS] && std::abs(dRho)<dRhoCut){
              }
            }// global

            // Add PRD which matches Segm1 position to vector for further analysis
            if(std::abs(dPhi)<dPhiCutSector[tgcWS] && std::abs(dRho)<dRhoCutSector[tgcWS]){
              tpdVector[tgcWS].push_back(tpd);
            }
            // Do Sector Efficiency Check
            if(canCheckSector[stationIndex]){
              // Do check against PRD from matching Sectors
              if((stationEta==TGCstation_StationEta[stationIndex])&&
                 (stationPhi==TGCstation_StationPhi[stationIndex])&&
                 (tgcFE==TGCstation_StationFE[stationIndex])){
                // Do check
                if(std::abs(dPhi)<dPhiCutSector[tgcWS] && std::abs(dRho)<dRhoCutSector[tgcWS]){
                  if(layer>=0)sectorhitregistered[layer][tgcWS]=true;// Sector hit
                }
              }
            }// sector
            
          }// dPhi Loose Cut
        }// TGC PRD Collection
      }// TGC PRD Container

      // Cut Segment if no stations can be checked
      if((!canCheckGlobal[0])&&(!canCheckGlobal[1])&&(!canCheckGlobal[2])&&(!canCheckGlobal[3]))continue;
      ////////////////////////////////////////////////////////////////////////
      // Do PRD checks
      // Find vector of PRD which forms a coherent line in the vicinity of Segm1

      // Variables to hold best PRD matching results
      std::vector<const Muon::TgcPrepData*> *bestTPDmatches[2];
      bestTPDmatches[0] = 0;
      bestTPDmatches[1] = 0;
      if(bestTPDmatches[0]->size()>0) bestTPDmatches[0]->clear();
      if(bestTPDmatches[1]->size()>0) bestTPDmatches[1]->clear();
      int bestTPDlayerMatches[2][9] = {{0,0,0,0,0,0,0,0,0},
               {0,0,0,0,0,0,0,0,0}};

      for(int k=0;k<2;k++){// WireStrip
        // Variables to record quality of best match found
        int nPRDMax   = 0;
        int nlayerMax = 0;

        // Loop over PRD vector
        int nTPD = tpdVector[k].size();
        for(int iTPD1=0;iTPD1<nTPD;iTPD1++){
          // Variables to hold matches found for this PRD
          std::vector<const Muon::TgcPrepData*> *thisTPDmatches;
	  thisTPDmatches = 0;
	  if(thisTPDmatches->size()>0) thisTPDmatches->clear();
          int thisTPDlayerMatches[9] = {0,0,0,0,0,0,0,0,0}; 
          
          // Get position variables
          const Amg::Vector3D prdPos1 = tpdVector[k].at(iTPD1)->globalPosition();
          
          float prd1Phi = prdPos1.phi();
          float prd1Z   = prdPos1.z();
          if(prd1Phi<0)prd1Phi+=2*M_PI;
          Amg::Vector3D prd1PosZunit(prdPos1/std::abs(prd1Z));
          
          // Get id values
          Identifier tgcid1=(tpdVector[k].at(iTPD1))->identify();
          int stationName1 = m_idHelperSvc->tgcIdHelper().stationName(tgcid1);
          int gasGap1      = m_idHelperSvc->tgcIdHelper().gasGap(tgcid1);
          int layer1       = TGCgetlayer(stationName1,gasGap1);
          if(layer1>=0)thisTPDlayerMatches[layer1]++;
          
          // Loop over PRD
          for(int iTPD2=0;iTPD2<nTPD;iTPD2++){
            if(iTPD2==iTPD1)continue;// do not check PRD against itself
            
            // Get position variables
            const Amg::Vector3D prdPos2 = tpdVector[k].at(iTPD2)->globalPosition(); 
	    float prd2Rho = std::abs(prdPos2.perp());
            float prd2Phi = prdPos2.phi();
            float prd2Z   = prdPos2.z();
            if(prd2Phi<0)prd2Phi+=2*M_PI;
            
            // Extrapolate PRD1 to PRD2 Z position
            float dZ = std::abs(prd2Z)- std::abs(prd1Z);
            Amg::Vector3D prdExtrPos = ((prdPos1)+((prd1PosZunit)*dZ));
            
            // Get extrapolated variables
            float prdExtrRho = std::abs(prdExtrPos.perp());
            float prdExtrPhi = prdExtrPos.phi();
            if(prdExtrPhi<0)prdExtrPhi+=2*M_PI;
            
            // Get differences between extrapolated and PRD2 positions
            float dRho = prd2Rho-prdExtrRho;
            float dPhi = prd2Phi-prdExtrPhi;
            if(dPhi<-M_PI)dPhi+=2*M_PI;
            if(dPhi> M_PI)dPhi-=2*M_PI;
            
            // Fill PRD comparison sagitta histograms
            if(m_tgc_prdcompsag[i][k][0]) m_tgc_prdcompsag[i][k][0]->Fill(dRho);
            if(m_tgc_prdcompsag[i][k][2]) m_tgc_prdcompsag[i][k][2]->Fill(dPhi);
            
            // Do check
            if(std::abs(dPhi)<dPhiCutTPD[k] && std::abs(dRho)<dRhoCutTPD[k]){
              // Get id values
              Identifier tgcid2=(tpdVector[k].at(iTPD2))->identify();
              int stationName2 = m_idHelperSvc->tgcIdHelper().stationName(tgcid2);
              int gasGap2      = m_idHelperSvc->tgcIdHelper().gasGap(tgcid2);
              int layer2       = TGCgetlayer(stationName2,gasGap2);
              
              // Add PRD2 to matches for PRD1
              if(layer2>=0)thisTPDlayerMatches[layer2]++;
              thisTPDmatches->push_back(tpdVector[k].at(iTPD2));
            }
          }// nTPD2
          
          // Get quality of matching array produced
          int nPRDCurrent   = 0;
          int nlayerCurrent = 0;
          for(int l=0;l<9;l++){
            if(thisTPDlayerMatches[l]>0)nlayerCurrent++;
            nPRDCurrent+=thisTPDlayerMatches[l];
          }
          
          // Check quality variables against maximum found
          if(nlayerMax <= nlayerCurrent){
            if(nPRDMax < nPRDCurrent){
              // Set maximum values to current segment's values
              nlayerMax = nlayerCurrent;
              nPRDMax   = nPRDCurrent;
              bestTPDmatches[k]    = thisTPDmatches;
              for(int l=0;l<9;l++){
                bestTPDlayerMatches[k][l] = thisTPDlayerMatches[l];
              }
            }
          }
        }// nTPD1
        
        // If matching array was somehow empty (should be impossible)
        if(nlayerMax==0)continue;
        if(bestTPDmatches[k]->size()==0){
          ATH_MSG_WARNING( "MidstationOnly: empty bestTPDmatches["<<k<<"] passed"  );
          continue;
        }
        
        // Set canCheck variables based on contents of matched PRD array
        for(int jTGC1=0;jTGC1<3;jTGC1++){// TGC Stations
          // Number of matched PRD found in other Stations
          int nMatchOther = 0;
          for(int l=0;l<9;l++){
            int jTGC2 = TGClayer2stationindex(l);
            if(jTGC1==jTGC2)continue;
            nMatchOther+=bestTPDlayerMatches[k][l];
          }
          
          // Check against cut
          if(nMatchOther<nMeasCutTGCMidPRD[k]){
            canCheckGlobal[jTGC1]=false;
            canCheckSector[jTGC1]=false;
          }
        }// TGC Stations
      }// WireStrip
      
      // Cut Segment if no stations can be checked
      if((!canCheckGlobal[0])&&(!canCheckGlobal[1])&&(!canCheckGlobal[2])&&(!canCheckGlobal[3]))continue;
      ////////////////////////////////////////////////////////////////////////
      // Segment has passed all global cuts, fill histogram variables
      // If no segments have already passed all cuts
      if(nValidatedSegm==0){
        for(int jTGC=0;jTGC<4;jTGC++){// TGC Stations
          // Assign values of variables to fill histograms
          // posThetaFill = segm1PosThe;
          // posPhiFill   = segm1PosPhi;
          
          //canCheckGlobalFill[jTGC] = canCheckGlobal[jTGC];
          canCheckSectorFill[jTGC] = canCheckSector[jTGC];
          
          TGCstation_StationFEFill[jTGC]  = TGCstation_StationFE[jTGC];
          TGCstation_StationEtaFill[jTGC] = TGCstation_StationEta[jTGC];
          TGCstation_StationPhiFill[jTGC] = TGCstation_StationPhi[jTGC];
        }
        for(int l=0;l<9;l++){
          for(int k=0;k<2;k++){
            sectorhitregisteredFill[l][k] = sectorhitregistered[l][k];
            // hitregisteredFill[l][k]       = hitregistered[l][k];
          }
        }
      }// TGC Stations
      nValidatedSegm++;
    }// nSegm1
    
    // If only one Segment was validated on this side
    if(nValidatedSegm==1){
      for(int l=0;l<9;l++){//Layer
        int stationIndex = TGClayer2stationindex(l);
        for(int k=0;k<2;k++){// WireStrip
          // If this station can be checked
          if(canCheckSectorFill[stationIndex]){
            if((TGCstation_StationFEFill[stationIndex]<0)||(TGCstation_StationEtaFill[stationIndex]==0)||(TGCstation_StationPhiFill[stationIndex]==0)){
              ATH_MSG_WARNING( "MidstationOnly: canCheckSector passed for jTGC=" << stationIndex
                               << " but, FE="<<TGCstation_StationFEFill[stationIndex]
                               << " Eta="<<TGCstation_StationEtaFill[stationIndex]
                               << " Phi=" << TGCstation_StationPhiFill[stationIndex]  );
              continue;
            }
            // Get Sector histogram indexes
            int stationMap_EtaIndex=getStationMapIndex(1, l, TGCstation_StationFEFill[stationIndex], TGCstation_StationEtaFill[stationIndex], TGCstation_StationPhiFill[stationIndex]);
            int stationMap_PhiIndex=getStationMapIndex(2, l, TGCstation_StationFEFill[stationIndex], TGCstation_StationEtaFill[stationIndex], TGCstation_StationPhiFill[stationIndex]);
            
            // Fill Sector efficiency histograms
            if(sectorhitregisteredFill[l][k]){// Hit in Sector matches extrapolated track
              m_eff_stationmapmid[i][k][1]->Fill(stationMap_EtaIndex, stationMap_PhiIndex);
            }
            m_eff_stationmapmid[i][k][2]->Fill(stationMap_EtaIndex, stationMap_PhiIndex);
          }
        }// WireStrip
      }// layer
    }
    
  }// AC
  
}// End of Function
