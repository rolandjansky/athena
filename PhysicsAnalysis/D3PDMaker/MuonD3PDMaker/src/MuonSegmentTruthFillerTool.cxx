/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////
//
// Author : Daniel Ventura <ventura@cern.ch>
//
//////////////////////////////////////////////////////
#include "MuonSegmentTruthFillerTool.h"
#include "AthenaKernel/errorcheck.h"
#include "TrkTruthData/TruthTrajectory.h"
#include "AtlasHepMC/GenParticle.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "MuonPattern/DetailedMuonPatternTruthCollection.h"
#include "MuonTruthAlgs/MuonPatternCombinationDetailedTrackTruthMaker.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrkPrepRawData/PrepRawData.h"

#include "TrkTruthData/DetailedSegmentTruth.h"

#include "MuonSegment/MuonSegment.h"

using HepGeom::Point3D;
using HepGeom::Vector3D;
using namespace std;

namespace D3PD {
  
  MuonSegmentTruthFillerTool::MuonSegmentTruthFillerTool (const std::string& type,
								      const std::string& name,
								      const IInterface* parent)
    : BlockFillerTool<Trk::Segment> (type, name, parent) ,
      m_idHelper(0),
      m_truthTool("Trk::DetailedMuonPatternTruthBuilder/DetailedMuonPatternTruthBuilder") {

    declareProperty("DetailedMuonPatternTruthTool", m_truthTool);
    book().ignore(); // Avoid coverity warnings.
  }
  
  StatusCode MuonSegmentTruthFillerTool::book() {
    //branches
    CHECK( addVariable("res_x", m_res_x) );
    CHECK( addVariable("res_y", m_res_y) );
    CHECK( addVariable("dAngleYZ", m_dAngleYZ) );
    CHECK( addVariable("dAngleXZ", m_dAngleXZ) );
    CHECK( addVariable("truth_barcode", m_truth_barcode) );
    CHECK( addVariable("truth_nSTGC", m_truth_nSTGC) );
    CHECK( addVariable("truth_nMM", m_truth_nMM) );
    CHECK( addVariable("truth_nMDT", m_truth_nMDT) );
    CHECK( addVariable("truth_nRPC", m_truth_nRPC) );
    CHECK( addVariable("truth_nTGC", m_truth_nTGC) );
    CHECK( addVariable("truth_nCSC", m_truth_nCSC) );
    CHECK( addVariable("noise_nSTGC", m_noise_nSTGC) );
    CHECK( addVariable("noise_nMM", m_noise_nMM) );
    CHECK( addVariable("noise_nMDT", m_noise_nMDT) );
    CHECK( addVariable("noise_nRPC", m_noise_nRPC) );
    CHECK( addVariable("noise_nTGC", m_noise_nTGC) );
    CHECK( addVariable("noise_nCSC", m_noise_nCSC) );
    CHECK( addVariable("common_nSTGC", m_common_nSTGC) );
    CHECK( addVariable("common_nMM", m_common_nMM) );
    CHECK( addVariable("common_nMDT", m_common_nMDT) );
    CHECK( addVariable("common_nRPC", m_common_nRPC) );
    CHECK( addVariable("common_nTGC", m_common_nTGC) );
    CHECK( addVariable("common_nCSC", m_common_nCSC) );
    
    return StatusCode::SUCCESS;
  }
  
  
  StatusCode MuonSegmentTruthFillerTool::initialize()
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


  StatusCode MuonSegmentTruthFillerTool::fill(const Trk::Segment& ts) {
    const Muon::MuonSegment& mSeg = dynamic_cast<const Muon::MuonSegment&> (ts);

//    const std::vector<const Trk::RIO_OnTrack*>& cROTv = mSeg.containedROTs();

    // global position
//    const Point3D<double>& segGlobalPos = ts.globalPosition();
    
    // global direction
//    const Vector3D<double>& segGlobalDir = mSeg.globalDirection();

    //----------------------------------------------------------------
    // Retrieve prep raw data truth
    std::vector<std::string> PRD_TruthNames;
    PRD_TruthNames.push_back("CSC_TruthMap");
    PRD_TruthNames.push_back("RPC_TruthMap");
    PRD_TruthNames.push_back("TGC_TruthMap");
    PRD_TruthNames.push_back("MDT_TruthMap");
    PRD_TruthNames.push_back("MM_TruthMap");
    PRD_TruthNames.push_back("STGC_TruthMap");

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
    std::vector<DetailedSegmentTruth> *dtt = new std::vector<DetailedSegmentTruth>;

    m_truthTool->buildDetailedTrackTruthFromSegments(dtt, mSeg, prdCollectionVector);

    bool isTruthMatched(false);

    // Find the best dtt to store.  Best is defined as the one with the most total hits in common
    std::vector<DetailedSegmentTruth>::const_iterator best_dtit = dtt->begin();
    int best_nHits = 0; 

    for(std::vector<DetailedSegmentTruth>::const_iterator dtit=dtt->begin(); dtit!=dtt->end(); ++dtit) {
      int nHits = 0;
      for(unsigned i = 0; i < SubDetHitStatistics::NUM_SUBDETECTORS; i++) {
        nHits += dtit->statsCommon()[ (SubDetHitStatistics::SubDetType)i ];
      }

      if(nHits > best_nHits) {
        best_dtit = dtit;
        best_nHits = nHits;
      }
    }

    if(dtt->begin() != dtt->end() && ((*best_dtit).trajectory())[0].isValid()) {
      const TruthTrajectory traj = (*best_dtit).trajectory();

      isTruthMatched = true;


      vector<int> barcode_list;
      for(unsigned int i = 0; i < traj.size(); i++) {
        barcode_list.push_back(traj[i].barcode());
      }
      *m_truth_barcode = barcode_list;
      *m_truth_nSTGC = best_dtit->statsTruth()[SubDetHitStatistics::STGC];
      *m_truth_nMM = best_dtit->statsTruth()[SubDetHitStatistics::MM];
      *m_truth_nMDT = best_dtit->statsTruth()[SubDetHitStatistics::MDT];
      *m_truth_nRPC = best_dtit->statsTruth()[SubDetHitStatistics::RPC];
      *m_truth_nTGC = best_dtit->statsTruth()[SubDetHitStatistics::TGC];
      *m_truth_nCSC = best_dtit->statsTruth()[SubDetHitStatistics::CSC];
      *m_common_nSTGC = best_dtit->statsCommon()[SubDetHitStatistics::STGC];
      *m_common_nMM = best_dtit->statsCommon()[SubDetHitStatistics::MM];
      *m_common_nMDT = best_dtit->statsCommon()[SubDetHitStatistics::MDT];
      *m_common_nRPC = best_dtit->statsCommon()[SubDetHitStatistics::RPC];
      *m_common_nTGC = best_dtit->statsCommon()[SubDetHitStatistics::TGC];
      *m_common_nCSC = best_dtit->statsCommon()[SubDetHitStatistics::CSC];
      *m_noise_nSTGC = best_dtit->statsTrack()[SubDetHitStatistics::STGC]-best_dtit->statsCommon()[SubDetHitStatistics::STGC];
      *m_noise_nMM = best_dtit->statsTrack()[SubDetHitStatistics::MM]-best_dtit->statsCommon()[SubDetHitStatistics::MM];
      *m_noise_nMDT = best_dtit->statsTrack()[SubDetHitStatistics::MDT]-best_dtit->statsCommon()[SubDetHitStatistics::MDT];
      *m_noise_nRPC = best_dtit->statsTrack()[SubDetHitStatistics::RPC]-best_dtit->statsCommon()[SubDetHitStatistics::RPC];
      *m_noise_nTGC = best_dtit->statsTrack()[SubDetHitStatistics::TGC]-best_dtit->statsCommon()[SubDetHitStatistics::TGC];
      *m_noise_nCSC = best_dtit->statsTrack()[SubDetHitStatistics::CSC]-best_dtit->statsCommon()[SubDetHitStatistics::CSC];

      if(best_dtit->truthSegPos().x() != -15000.0) {
        Amg::Transform3D gToLocal = mSeg.associatedSurface().transform().inverse();
        Amg::Vector3D localSegPos = gToLocal * mSeg.globalPosition(); 
        Amg::Vector3D localTrueSegPos = gToLocal * best_dtit->truthSegPos();
        Amg::Vector3D localSegDir = gToLocal * mSeg.globalDirection(); 
        Amg::Vector3D localTrueSegDir = gToLocal * best_dtit->truthSegDir();

        double scaleFactor = localTrueSegPos.z() / localTrueSegDir.z();
        Amg::Vector3D localTrueSegPosOnSurface = -scaleFactor * localTrueSegDir + localTrueSegPos; 

        *m_res_x = localSegPos.x() - localTrueSegPosOnSurface.x();
        *m_res_y = localSegPos.y() - localTrueSegPosOnSurface.y();

        if( fabs(localSegPos.z() - localTrueSegPosOnSurface.z()) > 0.1)
          ATH_MSG_WARNING("Error: localTrueSegPosOnSurface.z() has value " << localTrueSegPosOnSurface.z() << 
			  ", but should be extremely close to zero.");

        Amg::Vector3D temp = localSegDir;
        Amg::Vector3D tempTrue = localTrueSegDir;
        temp.x() = 0;
        tempTrue.x() = 0;
        temp = temp.unit();
        tempTrue = tempTrue.unit();
        //double cosAngleYZ = temp * tempTrue;
        //double dAngleYZ = acos(cosAngleYZ);

        double new_dAngleYZ = temp.theta() - tempTrue.theta();
 

        temp = localSegDir;
        tempTrue = localTrueSegDir;
        temp.y() =  0;
        tempTrue.y() = 0;
        temp = temp.unit();
        tempTrue = tempTrue.unit();
        //double cosAngleXZ = temp * tempTrue;
        //double dAngleXZ = acos(cosAngleXZ);

        double new_dAngleXZ = temp.theta() - tempTrue.theta();


        *m_dAngleYZ = new_dAngleYZ;
        *m_dAngleXZ = new_dAngleXZ;
      }
    }
    if(!isTruthMatched) {
      vector<int> temp;
      temp.push_back(0);
      *m_truth_barcode = temp;
      *m_truth_nSTGC = 0;
      *m_truth_nMM = 0;
      *m_truth_nMDT = 0;
      *m_truth_nRPC = 0;  
      *m_truth_nTGC = 0;
      *m_truth_nCSC = 0;
      *m_common_nSTGC = 0;
      *m_common_nMM = 0;
      *m_common_nMDT = 0;
      *m_common_nRPC = 0;
      *m_common_nTGC = 0;
      *m_common_nCSC = 0;
      *m_noise_nSTGC = 0;
      *m_noise_nMM = 0;
      *m_noise_nMDT = 0;
      *m_noise_nRPC = 0;
      *m_noise_nTGC = 0;
      *m_noise_nCSC = 0;
      *m_dAngleYZ = 0;
      *m_dAngleXZ = 0;
      *m_res_x = 0;
      *m_res_y = 0;
    }

//    if(dtt) delete dtt;
    return StatusCode::SUCCESS;
  }
  
  double MuonSegmentTruthFillerTool::deltaR(double eta1, double eta2, double phi1, double phi2) {
    double Deta = eta1 - eta2;
    double Dphi = phi1 - phi2;
    if(Dphi > M_PI) Dphi -= 2*M_PI;
    else if(Dphi < -M_PI) Dphi += 2*M_PI;
    double DR = sqrt(Deta*Deta + Dphi*Dphi);
    return DR;
  }

} // namespace D3PD
