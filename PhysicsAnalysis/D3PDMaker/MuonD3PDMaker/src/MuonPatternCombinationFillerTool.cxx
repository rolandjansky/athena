/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////
//
// Author : Daniel Ventura <ventura@cern.ch>
//
//////////////////////////////////////////////////////
#include "MuonPatternCombinationFillerTool.h"
#include "AthenaKernel/errorcheck.h"
#include "TrkTruthData/TruthTrajectory.h"
#include "AtlasHepMC/GenParticle.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "MuonPattern/DetailedMuonPatternTruthCollection.h"
#include "MuonTruthAlgs/MuonPatternCombinationDetailedTrackTruthMaker.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrkPrepRawData/PrepRawData.h"


using namespace std;

namespace D3PD {
  
  
  MuonPatternCombinationFillerTool::MuonPatternCombinationFillerTool (const std::string& type,
								      const std::string& name,
								      const IInterface* parent)
    : BlockFillerTool<Muon::MuonPatternCombination> (type, name, parent) ,
      m_idHelper(0),
      m_truthTool("Trk::DetailedMuonPatternTruthBuilder/DetailedMuonPatternTruthBuilder") {

    declareProperty("DetailedMuonPatternTruthTool", m_truthTool);
    declareProperty("MuonPatternCombinationCollection", m_patternKey = "MuonRoadPatternCombinations");

    book().ignore(); // Avoid coverity warnings.
  }
  
  
  StatusCode MuonPatternCombinationFillerTool::book() {
    //branches
    CHECK( addVariable("gpos_eta", m_pattern_gpos_eta) );
    CHECK( addVariable("gpos_phi", m_pattern_gpos_phi) );
    CHECK( addVariable("gpos_r", m_pattern_gpos_r) );
    CHECK( addVariable("gpos_z", m_pattern_gpos_z) );
    CHECK( addVariable("gdir_x", m_pattern_gdir_x) );
    CHECK( addVariable("gdir_y", m_pattern_gdir_y) );
    CHECK( addVariable("gdir_z", m_pattern_gdir_z) );
    CHECK( addVariable("seedType", m_pattern_seedType) );
    CHECK( addVariable("nMDT", m_pattern_nMDT) );
    CHECK( addVariable("nRPC", m_pattern_nRPC) );
    CHECK( addVariable("nTGC", m_pattern_nTGC) );
    CHECK( addVariable("nCSC", m_pattern_nCSC) );
    CHECK( addVariable("truth_barcode", m_truth_barcode) );
    CHECK( addVariable("truth_nMDT", m_truth_nMDT) );
    CHECK( addVariable("truth_nRPC", m_truth_nRPC) );
    CHECK( addVariable("truth_nTGC", m_truth_nTGC) );
    CHECK( addVariable("truth_nCSC", m_truth_nCSC) );
    CHECK( addVariable("noise_nMDT", m_noise_nMDT) );
    CHECK( addVariable("noise_nRPC", m_noise_nRPC) );
    CHECK( addVariable("noise_nTGC", m_noise_nTGC) );
    CHECK( addVariable("noise_nCSC", m_noise_nCSC) );
    CHECK( addVariable("common_nMDT", m_common_nMDT) );
    CHECK( addVariable("common_nRPC", m_common_nRPC) );
    CHECK( addVariable("common_nTGC", m_common_nTGC) );
    CHECK( addVariable("common_nCSC", m_common_nCSC) );
    
    return StatusCode::SUCCESS;
  }
  
  
   StatusCode MuonPatternCombinationFillerTool::initialize()
   {
     //get tools
     if(m_truthTool.retrieve().isFailure() ){
       ATH_MSG_FATAL( "Could not get " << m_truthTool );
       return StatusCode::FAILURE;
     }
     if(!detStore()->retrieve(m_idHelper, "AtlasID").isSuccess()) {
       ATH_MSG_FATAL("Unable to initialize ID helper.");
       return StatusCode::FAILURE;
     }
    
    return StatusCode::SUCCESS;
   }

  StatusCode MuonPatternCombinationFillerTool::fill(const Muon::MuonPatternCombination& pattern) {
    if(pattern.chamberData().size() == 0) return StatusCode::SUCCESS;
    Amg::Vector3D gpos = pattern.chamberData().front().intersectPosition();
    *m_pattern_gpos_eta = static_cast<float>(gpos.eta());
    *m_pattern_gpos_phi = static_cast<float>(gpos.phi());
    *m_pattern_gpos_r = static_cast<float>(gpos.norm());
    *m_pattern_gpos_z = static_cast<float>(gpos.z());
    
    Amg::Vector3D gdir = pattern.chamberData().front().intersectDirection();
    *m_pattern_gdir_x = static_cast<float>(gdir.x());
    *m_pattern_gdir_y = static_cast<float>(gdir.y());
    *m_pattern_gdir_z = static_cast<float>(gdir.z());
    *m_pattern_seedType = static_cast<int>(pattern.trackRoadType());
    
    //hits from the pattern combination
    int nPatternMDT(0),nPatternRPC(0),nPatternTGC(0),nPatternCSC(0);
    std::vector< Muon::MuonPatternChamberIntersect >::const_iterator chit = pattern.chamberData().begin();
    for(; chit!=pattern.chamberData().end(); ++chit) {
      if((*chit).prepRawDataVec().size() == 0) continue;
      SubDetHitStatistics::SubDetType subdet = findSubDetType( (*chit).prepRawDataVec().at(0)->identify() );
      if(subdet == SubDetHitStatistics::MDT) nPatternMDT += (*chit).prepRawDataVec().size();
      else if(subdet == SubDetHitStatistics::RPC) nPatternRPC += (*chit).prepRawDataVec().size();
      else if(subdet == SubDetHitStatistics::TGC) nPatternTGC += (*chit).prepRawDataVec().size();
      else if(subdet == SubDetHitStatistics::CSC) nPatternCSC += (*chit).prepRawDataVec().size();
    }
    *m_pattern_nMDT = static_cast<float>(nPatternMDT);
    *m_pattern_nRPC = static_cast<float>(nPatternRPC);
    *m_pattern_nTGC = static_cast<float>(nPatternTGC);
    *m_pattern_nCSC = static_cast<float>(nPatternCSC);

    //----------------------------------------------------------------
    // Retrieve prep raw data truth
    std::vector<std::string> PRD_TruthNames;
    PRD_TruthNames.push_back("CSC_TruthMap");
    PRD_TruthNames.push_back("RPC_TruthMap");
    PRD_TruthNames.push_back("TGC_TruthMap");
    PRD_TruthNames.push_back("MDT_TruthMap");
    
    std::vector<const PRD_MultiTruthCollection*> prdCollectionVector;
    for(std::vector<std::string>::const_iterator ikey=PRD_TruthNames.begin(); ikey!=PRD_TruthNames.end(); ++ikey) {
      prdCollectionVector.push_back(0);
      StatusCode sc = evtStore()->retrieve(*prdCollectionVector.rbegin(), *ikey);
      if (!sc.isSuccess()){
	ATH_MSG_WARNING( "PRD_MultiTruthCollection " << *ikey << " NOT found");
      } else {
	ATH_MSG_DEBUG( "Got PRD_MultiTruthCollection " << *ikey);
      }
    }
    
    //----------------------------------------------------------------
    // Produce and store the output.  
    std::vector<DetailedTrackTruth> *dtt = new std::vector<DetailedTrackTruth>;
    m_truthTool->buildDetailedTrackTruth(dtt, pattern, prdCollectionVector);

    bool isTruthMatched(false);
    for(std::vector<DetailedTrackTruth>::const_iterator dtit=dtt->begin(); dtit!=dtt->end(); ++dtit) {
      const TruthTrajectory traj = (*dtit).trajectory();
      if(traj[0].isValid()) {
	isTruthMatched = true;

        vector<int> barcode_list;
        for(unsigned int i = 0; i < traj.size(); i++) {
          barcode_list.push_back(traj[i].barcode());
        }
        m_truth_barcode->push_back(barcode_list);
        m_truth_nMDT->push_back(dtit->statsTruth()[SubDetHitStatistics::MDT]);
        m_truth_nRPC->push_back(dtit->statsTruth()[SubDetHitStatistics::RPC]);
        m_truth_nTGC->push_back(dtit->statsTruth()[SubDetHitStatistics::TGC]);
        m_truth_nCSC->push_back(dtit->statsTruth()[SubDetHitStatistics::CSC]);
        m_common_nMDT->push_back(dtit->statsCommon()[SubDetHitStatistics::MDT]);
        m_common_nRPC->push_back(dtit->statsCommon()[SubDetHitStatistics::RPC]);
        m_common_nTGC->push_back(dtit->statsCommon()[SubDetHitStatistics::TGC]);
        m_common_nCSC->push_back(dtit->statsCommon()[SubDetHitStatistics::CSC]);
        m_noise_nMDT->push_back(dtit->statsTrack()[SubDetHitStatistics::MDT]-dtit->statsCommon()[SubDetHitStatistics::MDT]);
        m_noise_nRPC->push_back(dtit->statsTrack()[SubDetHitStatistics::RPC]-dtit->statsCommon()[SubDetHitStatistics::RPC]);
        m_noise_nTGC->push_back(dtit->statsTrack()[SubDetHitStatistics::TGC]-dtit->statsCommon()[SubDetHitStatistics::TGC]);
        m_noise_nCSC->push_back(dtit->statsTrack()[SubDetHitStatistics::CSC]-dtit->statsCommon()[SubDetHitStatistics::CSC]);
      }
    }
    if(!isTruthMatched) {
      vector<int> temp;
      temp.push_back(0);
      m_truth_barcode->push_back(temp); 
      m_truth_nMDT->push_back(0);
      m_truth_nRPC->push_back(0);  
      m_truth_nTGC->push_back(0);
      m_truth_nCSC->push_back(0);
      m_common_nMDT->push_back(0);
      m_common_nRPC->push_back(0);
      m_common_nTGC->push_back(0);
      m_common_nCSC->push_back(0);
      m_noise_nMDT->push_back(0);
      m_noise_nRPC->push_back(0);
      m_noise_nTGC->push_back(0);
      m_noise_nCSC->push_back(0);
    }
       
    delete dtt;

    return StatusCode::SUCCESS;
  }
  
  double MuonPatternCombinationFillerTool::deltaR(double eta1, double eta2, double phi1, double phi2) {
    double Deta = eta1 - eta2;
    double Dphi = phi1 - phi2;
    if(Dphi > M_PI) Dphi -= 2*M_PI;
    else if(Dphi < -M_PI) Dphi += 2*M_PI;
    double DR = sqrt(Deta*Deta + Dphi*Dphi);
    return DR;
  }
  
  SubDetHitStatistics::SubDetType MuonPatternCombinationFillerTool::findSubDetType(Identifier id) {
    if (m_idHelper->is_pixel(id))
      return SubDetHitStatistics::Pixel;
    if (m_idHelper->is_sct(id))
      return SubDetHitStatistics::SCT;
    if (m_idHelper->is_trt(id))
      return SubDetHitStatistics::TRT;
    if (m_idHelper->is_mdt(id))
      return SubDetHitStatistics::MDT;
    if (m_idHelper->is_rpc(id))
      return SubDetHitStatistics::RPC;
    if (m_idHelper->is_tgc(id))
      return SubDetHitStatistics::TGC;
    if (m_idHelper->is_csc(id))
      return SubDetHitStatistics::CSC;


    ATH_MSG_WARNING("findSubDetType(): UNKNOWN subdet for id="<<id);
    return SubDetHitStatistics::NUM_SUBDETECTORS;
  }


} // namespace D3PD
