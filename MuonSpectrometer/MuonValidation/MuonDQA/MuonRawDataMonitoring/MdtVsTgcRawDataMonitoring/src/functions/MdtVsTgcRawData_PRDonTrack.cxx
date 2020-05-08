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
 
#include "TrkSurfaces/TrapezoidBounds.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "muonEvent/MuonContainer.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"

#include <inttypes.h>

#include <sstream>
#include <algorithm>
#include <fstream>

// Search for PRD around tracks
void
MdtVsTgcRawDataValAlg::CheckTGConTrack(std::vector<SegmTrack> (&matchedSegments)[2],
                                       const Muon::TgcPrepDataContainer *tgc_prepcontainer){
  // Define Cuts:
  // Loose cut for PRD and Extrapolated
  const float dPhiCut_Loose    = M_PI/8;
  // Cut for Global Position  Efficiencies
  const float dPhiCutGlobal[2] = {M_PI/24,M_PI/12};         //[WireStrip]
  const float dRhoCutGlobal[2] = {   0.08,    0.5};         //[WireStrip]
  // Cut for Sector Efficiencies
  const float dPhiCutSector[2] = {    0.2,    0.1};         //[WireStrip]
  const float dRhoCutSector[2] = {    200,   2000};         //[WireStrip]
  
  // Loop over sides
  for(int i=0;i<2;i++){// AC
    // Get number of tracks
    unsigned int nTrack=matchedSegments[i].size();
    
    // Fill nEvents histograms
    m_mvt_cutspassed[i]->Fill(1);
    if(nTrack==0)m_mvt_cutspassed[i]->Fill(2);
    if(nTrack>1)m_mvt_cutspassed[i]->Fill(3);
    if(nTrack==1)m_mvt_cutspassed[i]->Fill(4);
    
    // Cut events without exactly one set of matched Segments
    if(nTrack!=1)continue;
    
    // Declare Position variables for inner
    //Trk::GlobalPosition innerSegmPos;
    Amg::Vector3D innerSegmPos = {0, 0, 0};

    // float innerSegmEta=0; 
    float innerSegmRho=0; float innerSegmPhi=0; float innerSegmZ=0;
    //Trk::GlobalDirection innerSegmDirzunit;
    Amg::Vector3D innerSegmDirzunit = {0, 0, 0};
    
    // Declare Position variables for midstation
    //Trk::GlobalPosition midSegmPos;
    Amg::Vector3D midSegmPos = {0, 0, 0};

    // float midSegmRho  =0; float midSegmEta  =0; 
    float midSegmPhi  =0; float midSegmZ  =0;
    //Trk::GlobalDirection midSegmDirzunit;
    Amg::Vector3D midSegmDirzunit = {0, 0, 0};
    
    // Check which layers have sufficienct segments to operate on for global coordinates
    bool canCheckGlobal[4] = {0, 0, 0, 0};// [TGCStation]
    if(matchedSegments[i].at(0).at(2)!=0){
      // Check Midstation
      canCheckGlobal[0]=true; canCheckGlobal[1]=true; canCheckGlobal[2]=true;
      // Read Midstation Segment Values into variables
      midSegmPos   = Amg::Vector3D(matchedSegments[i].at(0).at(2)->globalPosition());
      midSegmPhi   = midSegmPos.phi();
      midSegmZ     = midSegmPos.z();
      if(midSegmPhi<0)midSegmPhi+=2*M_PI;
      midSegmDirzunit = Amg::Vector3D(midSegmPos/std::abs(midSegmZ));
    }
    if((matchedSegments[i].at(0).at(0)!=0)&&(matchedSegments[i].at(0).at(2)!=0)){
      // Check EIFI
      canCheckGlobal[3]=true;
      // Read Inner Segment Values
      innerSegmPos = Amg::Vector3D(matchedSegments[i].at(0).at(0)->globalPosition());
      innerSegmRho = std::abs(innerSegmPos.perp());
      innerSegmZ   = std::abs(innerSegmPos.z());
      // Modify position
      innerSegmPhi = midSegmPhi;

      innerSegmPos      = Amg::Vector3D(innerSegmRho*std::cos(innerSegmPhi), innerSegmRho*std::sin(innerSegmPhi), innerSegmZ);
      innerSegmDirzunit = Amg::Vector3D(innerSegmPos/std::abs(innerSegmZ));
    } 
    
    // If no layers can be checked (i.e. no midstation segment matched) skip side
    bool skipSide = true;
    for(int jTGC=0;jTGC<4;jTGC++)if(canCheckGlobal[jTGC]==true)skipSide=false;
    if(skipSide==true)continue;
    
    // Initialize variables for TRE array search
    int TGCStationNames[8]      ={41, 42, 43, 44, 45, 46, 47, 48};
    int TGCstation_StationFE[4] ={-1,-1,-1,-1};// [TGCStation]
    int TGCstation_StationEta[4]={ 0, 0, 0, 0};// [TGCStation]
    int TGCstation_StationPhi[4]={ 0, 0, 0, 0};// [TGCStation]
    int nStationMatch[4]        ={ 0, 0, 0, 0};// [TGCStation]
    bool canCheckSector[4]      ={ true, true, true, true};// [TGCStation]
    
    // Loop through TRE array, finding sectors which match the track in each layer
    for(int stationnameindex=0; stationnameindex<8; stationnameindex++){// Station {T1F,T1E,T2F,T2E,T3F,T3E,T4F,T4E}
      // Skip stations which don't have sufficient Segments to run efficiency check
      int stationName = TGCStationNames[stationnameindex];
      int stationIndex= TGCstationname2stationindex(stationName);
      if(stationIndex<0) continue;
      if(!canCheckGlobal[stationIndex])continue;
      
      // Loop over StationEta&StationPhi
      for(int stationeta=1; stationeta<=8; stationeta++){// AbsStationEta
        for(int stationphi=1; stationphi<=48; stationphi++){// StationPhi
          // Cut Station EtaPhi combinations with no TGC element
          if(m_TREarray[stationnameindex][i][stationeta][stationphi]==0)continue;
          const MuonGM::TgcReadoutElement *tre=m_TREarray[stationnameindex][i][stationeta][stationphi];
          
          // Extrapolate position from nearest Station's Segment to Sector's Z
          float sectorZ=tre->globalPosition().z();
          //Trk::GlobalPosition sectorExtrapolatedPos;
          Amg::Vector3D sectorExtrapolatedPos;
	  
          if(stationIndex==3){// Inner
            float dZ_sector=std::abs(sectorZ)-std::abs(innerSegmZ);
            //sectorExtrapolatedPos = Trk::GlobalPosition(innerSegmPos+(innerSegmDirzunit*dZ_sector));
            sectorExtrapolatedPos = Amg::Vector3D(innerSegmPos+(innerSegmDirzunit*dZ_sector)); 
	  }
          else{// Midstation
            float dZ_sector=std::abs(sectorZ)-std::abs(midSegmZ);
            sectorExtrapolatedPos = Amg::Vector3D(midSegmPos+(midSegmDirzunit*dZ_sector));
          }
          
          // Convert extrapolated position to local position on the Sector
          Identifier sector_id=tre->identify();
          const Amg::Vector3D sectorLocalPos3D=tre->globalToLocalCoords(sectorExtrapolatedPos, sector_id);
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
        //m_log << MSG::WARNING << "SegmTrack: Number of matches for TGC" << jTGC+1 << " is " << nStationMatch[jTGC] << endl;
      }
    }// TGC Station
    
    // Loop through segments to check number of TGC Strips in each
    int nTGCStrips[4] = {0, 0, 0, 0};//[TGCStation]
    for(int jMDT=0;jMDT<4;jMDT++){// MDT Station
      if(matchedSegments[i].at(0).at(jMDT)==0)continue;
      const Muon::MuonSegment *segm=matchedSegments[i].at(0).at(jMDT);
      // Loop through contained ROTs and identify used stations
      const std::vector<const Trk::MeasurementBase*> mMeasTrk = segm->containedMeasurements();
      ATH_MSG_DEBUG( "number of MeasurementBase: "<<mMeasTrk.size() );
      for (unsigned int i=0; i<mMeasTrk.size(); i++) {
	const Trk::MeasurementBase* m  = mMeasTrk[i];
	//const Trk::RIO_OnTrack* rio = dynamic_cast<const Trk::RIO_OnTrack*>(m);
	const Muon::CompetingMuonClustersOnTrack* crot = dynamic_cast<const Muon::CompetingMuonClustersOnTrack*>(m);
	if(crot) { 
	  const std::vector<const Muon::MuonClusterOnTrack*> mc_list = crot->containedROTs();
	  for(unsigned int iROT=0; iROT< mc_list.size(); iROT++){
	   const Muon::MuonClusterOnTrack * rio = mc_list[iROT];
	   //const Trk::RIO_OnTrack* rio = crot->rioOnTrack(iROT);
	   Identifier id = rio->identify();
	   int stationName = int(m_idHelperSvc->mdtIdHelper().stationName(id));
           // 41=T1F 42=T1E 43=T2F 44=T2E 45=T3F 46=T3E 47=T4F 48=T4E
           if(m_idHelperSvc->tgcIdHelper().isStrip(id)){
             if((jMDT==2)&&((stationName==41)||(stationName==42)))nTGCStrips[0]++;// TGC
             if((jMDT==2)&&((stationName==43)||(stationName==44)))nTGCStrips[1]++;// TGC
             if((jMDT==2)&&((stationName==45)||(stationName==46)))nTGCStrips[2]++;// TGC
             if((jMDT==0)&&((stationName==47)||(stationName==48)))nTGCStrips[3]++;// TGC
           }

	   ATH_MSG_DEBUG( " check if TGC strip: "<<m_idHelperSvc->tgcIdHelper().isStrip(id)<<" StationName: "<<stationName );
	  }
	}
      }
    }// MDT Station

    
    // Don't check mid-stations when there are no strips in other mid-stations  
    if((nTGCStrips[1]==0)&&(nTGCStrips[2]==0)){canCheckSector[0]=false;canCheckGlobal[0]=false;}
    if((nTGCStrips[0]==0)&&(nTGCStrips[2]==0)){canCheckSector[1]=false;canCheckGlobal[1]=false;}
    if((nTGCStrips[0]==0)&&(nTGCStrips[1]==0)){canCheckSector[2]=false;canCheckGlobal[2]=false;}
    
    // Initialise hit registered arrays
    // bool hitregistered[9][2];
    bool sectorhitregistered[9][2];
    for(int l=0;l<9;l++){// Layer
      for(int k=0;k<2;k++){// WireStrip
        // hitregistered[l][k]=false;
        sectorhitregistered[l][k]=false;
      }// WireStrip
    }// Layer
    
    // Initialise flags for whether a layer in this event has PRD and PRD which matched the current track
    bool HasPRD[9]  ={false,false,false,false,false,false,false,false,false};
    bool PRDMatch[9]={false,false,false,false,false,false,false,false,false};
    
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
        
        // Cut hits except those from same side EIFI & Midstation
        if(tgcAC!=i) continue;
        if(stationName>48 || stationName<41) continue;

        // Get layer number and stationIndex
        int layer        = TGCgetlayer(stationName,gasGap);
        int stationIndex = TGCstationname2stationindex(stationName);

        // Skip PRD in stations which can't be checked
        if(stationIndex<0) continue;
        if(!(canCheckGlobal[stationIndex]||canCheckSector[stationIndex]))continue;
	if(layer<0) continue;
        HasPRD[layer]=true;
        
        // Get position variables
        // const Trk::GlobalPosition prdPos = tpd->globalPosition();
        const Amg::Vector3D prdPos = tpd->globalPosition();
	float tgcRho = std::abs(prdPos.perp());
        float tgcPhi = prdPos.phi();
        float tgcZ   = prdPos.z();
        if(tgcPhi<0)tgcPhi+=2*M_PI;
        
        // Run Extrapolation
       // Trk::GlobalPosition tgcExtrapolatedPos;
	Amg::Vector3D tgcExtrapolatedPos;
        if(stationIndex==3){// Extrapolate position from Inner Position to PRD Z position
          //if(innerSegmPos=0)m_log << MSG::WARNING << "MidstationOnly: innerSegmPos=0 but passed canCheckGlobal"  );
          float dZ = std::abs(tgcZ) - std::abs(innerSegmZ);
          //tgcExtrapolatedPos = Trk::GlobalPosition(innerSegmPos+(innerSegmDirzunit*dZ));
          tgcExtrapolatedPos = Amg::Vector3D(innerSegmPos+(innerSegmDirzunit*dZ)); 
	}
        else{// Extrapolate position from Midstation Position to PRD Z position
          float dZ = std::abs(tgcZ) - std::abs(midSegmZ);
          //tgcExtrapolatedPos = Trk::GlobalPosition(midSegmPos+(midSegmDirzunit*dZ));
          tgcExtrapolatedPos = Amg::Vector3D(midSegmPos+(midSegmDirzunit*dZ)); 
	}
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
          if(m_mvt_extrprdsag[i][stationIndex][tgcFE][tgcWS][0]) m_mvt_extrprdsag[i][stationIndex][tgcFE][tgcWS][0]->Fill(dRho);
          if(m_mvt_extrprdsag[i][stationIndex][tgcFE][tgcWS][2]) m_mvt_extrprdsag[i][stationIndex][tgcFE][tgcWS][2]->Fill(dPhi);
          
          // Global efficiency check
          if(canCheckGlobal[stationIndex]){
            // Do check
            float dRhoCut = dRhoCutGlobal[tgcWS]*tgcExtrRho;
            if(std::abs(dPhi)<dPhiCutGlobal[tgcWS] && std::abs(dRho)<dRhoCut){
            }
          }
          
          // Sector Efficiency Check
          if(canCheckSector[stationIndex]){// If this station can be checked
            if((stationEta==TGCstation_StationEta[stationIndex])&&
               (stationPhi==TGCstation_StationPhi[stationIndex])&&
               (tgcFE==TGCstation_StationFE[stationIndex])){// If Station FE&Eta&Phi match
              if(std::abs(dPhi)<dPhiCutSector[tgcWS] && std::abs(dRho)<dRhoCutSector[tgcWS]){
                sectorhitregistered[layer][tgcWS]=true;
              }
            }// Station EtaPhi
          }

        }// dPhi Loose Cut
      }// TGC PRD Collection
    }// TGC PRD Container
    
    // Fill Efficiency Histograms
    for(int l=0;l<9;l++){// Layer
      // Get Station Number
      int stationIndex=TGClayer2stationindex(l);
      if(stationIndex<0) continue;
      for(int k=0;k<2;k++){// WireStrip
        // If Segment Track matches a Sector
        if(canCheckSector[stationIndex]){
          if((TGCstation_StationFE[stationIndex]<0)||(TGCstation_StationEta[stationIndex]==0)||(TGCstation_StationPhi[stationIndex]==0)){
            ATH_MSG_WARNING( "SegmTrack: canCheckSector passed for jTGC=" << stationIndex
                             << " but, FE=" << TGCstation_StationFE[stationIndex]
                             << " Eta=" << TGCstation_StationEta[stationIndex]
                             << " Phi=" << TGCstation_StationPhi[stationIndex]  );
            continue;
          }
          // Get Sector histogram indexes
          int stationMap_EtaIndex=getStationMapIndex(1, l, TGCstation_StationFE[stationIndex], TGCstation_StationEta[stationIndex], TGCstation_StationPhi[stationIndex]);
          int stationMap_PhiIndex=getStationMapIndex(2, l, TGCstation_StationFE[stationIndex], TGCstation_StationEta[stationIndex], TGCstation_StationPhi[stationIndex]);
          // Fill Sector efficiency histograms
          if(sectorhitregistered[l][k]){// Hit in Sector matches extrapolated track
            m_eff_stationmapbase[i][k][1]->Fill(stationMap_EtaIndex, stationMap_PhiIndex);
          }
          m_eff_stationmapbase[i][k][2]->Fill(stationMap_EtaIndex, stationMap_PhiIndex);
        }
      }// WireStrip
    }// Layer
    
    // Fill +Has Station bins of histogram
    if(HasPRD[0]||HasPRD[1]||HasPRD[2])m_mvt_cutspassed[i]->Fill(7);
    if(HasPRD[3]||HasPRD[4])m_mvt_cutspassed[i]->Fill(9);
    if(HasPRD[5]||HasPRD[6])m_mvt_cutspassed[i]->Fill(11);
    if(HasPRD[7]||HasPRD[8])m_mvt_cutspassed[i]->Fill(5);
    
    // Fill Match Station bins of histogram
    if(PRDMatch[0]||PRDMatch[1]||PRDMatch[2])m_mvt_cutspassed[i]->Fill(8);
    if(PRDMatch[3]||PRDMatch[4])m_mvt_cutspassed[i]->Fill(10);
    if(PRDMatch[5]||PRDMatch[6])m_mvt_cutspassed[i]->Fill(12);
    if(PRDMatch[7]||PRDMatch[8])m_mvt_cutspassed[i]->Fill(6);
    if((PRDMatch[0]||PRDMatch[1]||PRDMatch[2])&&
       (PRDMatch[3]||PRDMatch[4])&&
       (PRDMatch[5]||PRDMatch[6])&&
       (PRDMatch[7]||PRDMatch[8]))m_mvt_cutspassed[i]->Fill(13);
    if((PRDMatch[0]&&PRDMatch[1]&&PRDMatch[2])&&
       (PRDMatch[3]&&PRDMatch[4])&&
       (PRDMatch[5]&&PRDMatch[6])&&
       (PRDMatch[7]&&PRDMatch[8]))m_mvt_cutspassed[i]->Fill(14);
    
    
  }// AC
}// End of function
