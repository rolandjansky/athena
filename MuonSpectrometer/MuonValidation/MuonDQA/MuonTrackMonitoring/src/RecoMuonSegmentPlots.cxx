/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RecoMuonSegmentPlots.h"
#include "MuonIdHelpers/MuonStationIndex.h"
#include "EventPrimitives/EventPrimitives.h"
#include "MuonIdHelpers/MuonStationIndex.h"


RecoMuonSegmentPlots::RecoMuonSegmentPlots(PlotBase* pParent, std::string sDir):PlotBase(pParent, sDir)
  ,  m_oMuonSegmentPlots(this,"/")
  ,  DQ_delta_phi_segtrk(NULL)
  ,  DQ_delta_eta_segtrk(NULL)
  ,  DQ_delta_R_segtrk(NULL)
  ,  DQ_nGoodSegments(NULL)
  ,  DQ_close_Segments(NULL)
  ,  DQ_close_dir_Segments(NULL)
  ,  DQ_close_pos_Segments(NULL)
  ,  DQ_resdiual_Pos_Segments(NULL)

{}

void RecoMuonSegmentPlots::fill(const xAOD::MuonSegment& muonSeg) {
  //General Plots
  m_oMuonSegmentPlots.fill(muonSeg);
}

void RecoMuonSegmentPlots::initializePlots()
{
  //booking histograms
  for (int i = 0; i < Muon::MuonStationIndex::StIndexMax; i++){
    DQ_nSegment_sector_etaIndex.push_back(Book2D(Form("nSegment_sector_etaIndex_%s", StationName[i]), Form("Number of Segment in %s; #phi Sector; #eta Index", StationName[i]), 17, -0.5, 16.5, 19, -9.5, 9.5));
    //DQ_nGoodSegment_sector_etaIndex.push_back(Book2D(Form("nGoodSegment_sector_etaIndex_%s", StationName[i]), Form("Number of Good Segment in %s; #phi Sector; #eta Index", StationName[i]), 17, -0.5, 16.5, 19, -9.5, 9.5));
    //DQ_nBadSegment_sector_etaIndex.push_back(Book2D(Form("nBadSegment_sector_etaIndex_%s", StationName[i]), Form("Number of Bad Segment in %s; #phi Sector; #eta Index", StationName[i]), 17, -0.5, 16.5, 19, -9.5, 9.5));
  }

  DQ_delta_eta_segtrk = Book1D("TrkSeg_delta_eta_segtrk","delta_eta_segtrk;#Delta #eta_segtrk;Entries", 21,-5.5,5.5);
  DQ_delta_phi_segtrk = Book1D("TrkSeg_delta_phi_segtrk","delta_phi_segtrk;#Delta #phi_segtrk;Entries", 21,-5.5,5.5);
  DQ_delta_R_segtrk = Book1D("TrkSeg_delta_R_segtrk","delta_R_segtrk;#Delta R segtrk;Entries", 21,-0.5,5.5);

  DQ_nGoodSegments = Book1D("TrkSeg_nGoodSegments","Number of Good MS Segments;Number of Good MS Segments;Entries", 50,-0.5, 49.5);
  DQ_close_Segments = Book1D("TrkSeg_nclose_Segments","Number of close in same sector MS Segments pairs;Number of close MS Segments;Entries", 50,-0.5, 49.5);
  DQ_close_dir_Segments = Book1D("TrkSeg_nclose_dir_Segments","Number of close in direction MS Segments paris;Number of close dir MS Segments;Entries", 50,-0.5, 49.5);
  DQ_close_pos_Segments = Book1D("TrkSeg_nclose_pos_Segments","Number of close in position MS Segments paris;Number of close pos MS Segments;Entries", 50,-0.5, 49.5);

  DQ_resdiual_Pos_Segments = Book1D("TrkSeg_residual_Pos","Residual for Middle layer segments;Residual, mm;Entries", 1000, -1000.5, 1000.5);
}

void RecoMuonSegmentPlots::fill(std::vector<xAOD::MuonSegment*> selectedSegments, const xAOD::TrackParticleContainer& MSTracks)
{
  //loop through segments
  int Nclose_Segments = 0;
  int Nclose_dir_Segments = 0;
  int Nclose_pos_Segments = 0;


  //Sort Segment by station, for efficency and alginment
  std::vector<xAOD::MuonSegment*> muonInn;
  std::vector<xAOD::MuonSegment*> muonMid;
  std::vector<xAOD::MuonSegment*> muonOut;

  //start the loop over Segments:
  for (const auto muSeg: selectedSegments){

    //for monitoring purpose
    const int StIndex = Muon::MuonStationIndex::toStationIndex(muSeg->chamberIndex());
    const int etaIndex = muSeg->etaIndex();
    const int sectorIndex = muSeg->sector();

    DQ_nSegment_sector_etaIndex[StIndex]->Fill(muSeg->sector(), muSeg->etaIndex());
    //check if the segmmnet is a good quality segment
    if (!goodSegmentQuality(muSeg)){
      //DQ_nBadSegment_sector_etaIndex[StIndex]->Fill(muSeg->sector(), muSeg->etaIndex());
      continue;
    }
    //sort the segment into the groups of detector regions
    //DQ_nGoodSegment_sector_etaIndex[StIndex]->Fill(muSeg->sector(), muSeg->etaIndex());
    if (StIndex == 0 || StIndex == 4){muonInn.push_back(muSeg);}
    else if (StIndex == 1 || StIndex == 5){muonMid.push_back(muSeg);}
    else {muonOut.push_back(muSeg);}

    // Do this only for MDT for now
    if (muSeg->technology() != 0) continue;

    const Amg::Vector3D seg_globalDir(muSeg->px(),muSeg->py(),muSeg->pz());
    float seg_eta = seg_globalDir.eta();
    float seg_phi = seg_globalDir.phi();
    const Amg::Vector3D seg_globalPos(muSeg->x(),muSeg->y(),muSeg->z());
      
    //loop through all the segments again
    for (const auto muSeg_2: selectedSegments){
      if(muSeg == muSeg_2) continue;
      //for now just test the MDT overlapping segments
      if (muSeg_2->technology() != 0) continue;
      
      const int StIndex_2 = Muon::MuonStationIndex::toStationIndex(muSeg_2->chamberIndex());
      const int etaIndex_2 = muSeg_2->etaIndex();
      const int sectorIndex_2 = muSeg_2->sector();

      const Amg::Vector3D seg_globalDir_2(muSeg_2->px(),muSeg_2->py(),muSeg_2->pz());
      const Amg::Vector3D seg_globalPos_2(muSeg_2->x(),muSeg_2->y(),muSeg_2->z());

      if ((StIndex_2 == StIndex) && (etaIndex_2 == etaIndex) && (sectorIndex_2 == sectorIndex)){
      	Nclose_Segments++;
      }
      if (matchPairSegment_dir(seg_globalDir, seg_globalDir_2)){
      	Nclose_dir_Segments++;
      }
      if (matchPairSegment_pos(seg_globalPos, seg_globalPos_2)){
      	 Nclose_pos_Segments++;
      }
    }

    //for segment and track
    if (seg_phi>myPi) seg_phi-=2*myPi;
    for (const auto trk : MSTracks){
      DQ_delta_eta_segtrk->Fill(seg_eta - trk->eta());
      DQ_delta_phi_segtrk->Fill(seg_phi - trk->phi());
      DQ_delta_R_segtrk->Fill(DeltaR(seg_eta, trk->eta(), seg_phi, trk->phi()));
    }
  }

  DQ_nGoodSegments->Fill(selectedSegments.size());
  DQ_close_Segments->Fill(Nclose_Segments);
  DQ_close_dir_Segments->Fill(Nclose_dir_Segments);
  DQ_close_pos_Segments->Fill(Nclose_pos_Segments);

  // //clean up the selected segments
  // std::cout << "Total number of Segments " << selectedSegments.size() << " Number of good Inner Segments " << muonInn.size() << " Number of good Middle Segments " << muonMid.size() << " Number of good Outer Segments " << muonOut.size() << std::endl;
  removeOverlapSegments(muonInn);
  removeOverlapSegments(muonMid);
  removeOverlapSegments(muonOut);
  // std::cout << "After number of Segments " << selectedSegments.size() << " Number of good Inner Segments " << muonInn.size() << " Number of good Middle Segments " << muonMid.size() << " Number of good Outer Segments " << muonOut.size() << std::endl;


  //start to loop over the 3 layers, can finally do efficiency/alignment plots
  if ( muonInn.size() >0 && muonMid.size() >0 && muonOut.size() > 0 ) {

    for (unsigned int j = 0; j < muonMid.size(); j++){
      xAOD::MuonSegment* muSeg_M = muonMid[j];
      const Amg::Vector3D seg_globalDir_M(muSeg_M->px(),muSeg_M->py(),muSeg_M->pz());
      const Amg::Vector3D seg_globalPos_M(muSeg_M->x(),muSeg_M->y(),muSeg_M->z());
      bool isEndcap_M = Muon::MuonStationIndex::toStationIndex(muSeg_M->chamberIndex()) > 3;
      float segment_resdiual_Pos = 1000.;

      for (unsigned int i = 0; i < muonInn.size(); i++){
        xAOD::MuonSegment* muSeg_I = muonInn[i];
        const Amg::Vector3D seg_globalDir_I(muSeg_I->px(),muSeg_I->py(),muSeg_I->pz());
        const Amg::Vector3D seg_globalPos_I(muSeg_I->x(),muSeg_I->y(),muSeg_I->z());
        bool isEndcap_I = Muon::MuonStationIndex::toStationIndex(muSeg_I->chamberIndex()) > 3;
        if (isEndcap_M != isEndcap_I) continue;
        if (!matchPairSegment_dir(seg_globalDir_M, seg_globalDir_I)) continue;//match middle with inner

        for (unsigned int k = 0; k < muonOut.size(); k++){
          xAOD::MuonSegment* muSeg_O = muonOut[k];
          const Amg::Vector3D seg_globalDir_O(muSeg_O->px(),muSeg_O->py(),muSeg_O->pz());
          const Amg::Vector3D seg_globalPos_O(muSeg_O->x(),muSeg_O->y(),muSeg_O->z());
          //make sure the two segments are in the same region? waiting update
          bool isEndcap_O = Muon::MuonStationIndex::toStationIndex(muSeg_O->chamberIndex()) > 3;
          if (isEndcap_O != isEndcap_I) continue;

          if (!matchPairSegment_dir(seg_globalDir_I, seg_globalDir_O)) continue;//if the two segmnets are not alinged
          const Amg::Vector3D seg_globalPos_Diff(muSeg_O->x() - muSeg_I->x(),muSeg_O->y() - muSeg_I->y(),muSeg_O->z() - muSeg_I->z());
          if (!matchPairSegment_dir(seg_globalDir_M, seg_globalDir_O)) continue;//match middle with outer
          

          //now we have 3 segments that are somwhat aligned, time to do extrapolation
          double temp_segment_resdiual_Pos;
          if (isEndcap_I){//for endcap extrapolations
            temp_segment_resdiual_Pos = seg_globalPos_M.perp() - (seg_globalPos_Diff.perp()/seg_globalPos_Diff.z()*(seg_globalPos_M.z() - seg_globalPos_I.z()) + seg_globalPos_I.perp());
          }
          else{//for barrel extrapolations
            temp_segment_resdiual_Pos = seg_globalPos_M.z() - (seg_globalPos_Diff.z()/seg_globalPos_Diff.perp()*(seg_globalPos_M.perp() - seg_globalPos_I.perp()) + seg_globalPos_I.z());
          }
          if (abs(temp_segment_resdiual_Pos) < abs(segment_resdiual_Pos)){segment_resdiual_Pos = temp_segment_resdiual_Pos;}
          // //fill the histogram then
          // std::cout << "calculated segment resdual " << j << " value " << temp_segment_resdiual_Pos << " saved " << segment_resdiual_Pos << std::endl;
        }//loop the outer layer
      }//loop the inner layer
      DQ_resdiual_Pos_Segments->Fill(segment_resdiual_Pos);
    }//loop the middle layer
  }//end of looping the 3 layers
}

bool RecoMuonSegmentPlots::goodSegmentQuality(const xAOD::MuonSegment_v1* muSeg) {

  bool pass = true;

  const int station = Muon::MuonStationIndex::toStationIndex(muSeg->chamberIndex());
  const int TechIndex = muSeg->technology();
  const float chi2 = muSeg->chiSquared();
  const float nhits = muSeg->nPrecisionHits();//always add 2 here
  // std::cout << "station " << station <<  " tech " << TechIndex << " chi2 " << chi2 << " nhits " << nhits << " x " << muSeg->x() << " y " << muSeg->y()<< " z " << muSeg->z() <<std::endl;
  // MDT segemnt constraint
  if ( TechIndex == 0 ) {//This is just for MDT

    int mdtNhitsMin   = m_bMdtnhitsmin;
    float mdtChi2Max  = m_bMdtchi2max;

    if (station >= 4 ) {
      mdtNhitsMin = m_eMdtnhitsmin;
      mdtChi2Max  = m_eMdtchi2max;
    }
    // BI and EI station has 8 layers instead of 6.
    // Adjust accordingly, add one more hit/hole
    if ( station == 4 || station == 0) {
      mdtNhitsMin += 2;//require the hit to be high quality
    }
    // Cuts for case where dealing with MDT
    // Otherwise, dealing with CSC
    if ( chi2   > mdtChi2Max )  pass =  false;
    if ( nhits  < mdtNhitsMin ) pass = false;
  }
  // CSC segment constraint
  else if (TechIndex == 1){
    // This is relatively more straightforward
    if ( chi2   > m_eCscchi2max )  pass =  false;
    if ( nhits  < m_eCscnhitsmin ) pass = false;
  }
  // maybe in the future add other technologies

  // if get to here, a good segment!
  return pass;
}

bool RecoMuonSegmentPlots::matchPairSegment_dir( const Amg::Vector3D& globalDir1,const Amg::Vector3D& globalDir2 ) {

  bool pass = false;

  float dtheta = globalDir1.theta()-globalDir2.theta();
  if ( dtheta < 0. ) dtheta = -dtheta;
  if ( dtheta < m_mdtdelthetamax ) pass =  true;  //if it is too close


  float dphi = globalDir1.phi()-globalDir2.phi();
  if ( dphi < 0.) dphi = -dphi;
  if ( dphi < m_mdtdelphimax ) pass =  true; //if it is too close
  
  float deta = fabs( globalDir1.eta() -globalDir2.eta() );
  float dR = sqrt ( dphi*dphi + deta*deta );

  if ( dR < m_mdtdelphimax ) pass =  true; // this is saying R ~ phi < phimax

  return pass;
}

bool RecoMuonSegmentPlots::matchPairSegment_pos( const Amg::Vector3D& globalPos1,const Amg::Vector3D& globalPos2 ) {

  bool pass = false;

  float dz = globalPos1.z()-globalPos2.z();
  if ( dz < 0. ) dz = -dz;
  if ( dz < m_mdtdelzmax ) pass =  true;  //if it is too close


  float dr = globalPos1.perp()-globalPos2.perp();
  if ( dr < 0.) dr = -dr;
  if ( dr < m_mdtdelrmax ) pass =  true;  //if it is too close

  return pass;
}

void RecoMuonSegmentPlots::removeOverlapSegments(std::vector<xAOD::MuonSegment*> &muonSegments){

  //luckily the segment container is sorted by chi2 already
  for (int i = muonSegments.size() - 1; i > -1; i--){

    xAOD::MuonSegment* muSeg = muonSegments[i];
    const Amg::Vector3D seg_globalDir(muSeg->px(),muSeg->py(),muSeg->pz());
    const Amg::Vector3D seg_globalPos(muSeg->x(),muSeg->y(),muSeg->z());
    const int StIndex = Muon::MuonStationIndex::toStationIndex(muSeg->chamberIndex());
    //const int etaIndex = muSeg->etaIndex();
    const int sectorIndex = muSeg->sector();

    for (int j = i - 1; j > -1; j--){

      xAOD::MuonSegment* muSeg_2 = muonSegments[j];
      const Amg::Vector3D seg_globalDir_2(muSeg_2->px(),muSeg_2->py(),muSeg_2->pz());
      const Amg::Vector3D seg_globalPos_2(muSeg_2->x(),muSeg_2->y(),muSeg_2->z());
      const int StIndex_2 = Muon::MuonStationIndex::toStationIndex(muSeg_2->chamberIndex());
      //const int etaIndex_2 = muSeg_2->etaIndex();
      const int sectorIndex_2 = muSeg_2->sector();

      //if the two segments are in the same region, without eat requirement
      //(abs(etaIndex_2 - etaIndex) <= 1) && 
      if ((StIndex_2 == StIndex) && (sectorIndex_2 == sectorIndex)){
        // std::cout << "Segment 1 x " << muSeg->x() << " y " << muSeg->y()<< " z " << muSeg->z() <<std::endl;
        // std::cout << "Segment 2 x " << muSeg_2->x() << " y " << muSeg_2->y()<< " z " << muSeg_2->z() <<std::endl;
        if (matchPairSegment_dir(seg_globalDir, seg_globalDir_2)){
          muonSegments.erase(muonSegments.begin()+i);
          break;
        }
        else if (matchPairSegment_pos(seg_globalPos, seg_globalPos_2)){
          muonSegments.erase(muonSegments.begin()+i);
          break;
        }
      }

    }//second loop over segments
  }//main loop over segments
}
