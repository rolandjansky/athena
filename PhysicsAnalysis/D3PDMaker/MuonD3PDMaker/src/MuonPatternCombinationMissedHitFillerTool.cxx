/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////
//
// Author : Daniel Ventura <ventura@cern.ch>
//
//////////////////////////////////////////////////////
#include "MuonPatternCombinationMissedHitFillerTool.h"
#include "AthenaKernel/errorcheck.h"
#include "TrkTruthData/TruthTrajectory.h"
#include "AtlasHepMC/GenParticle.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "MuonPattern/DetailedMuonPatternTruthCollection.h"
#include "MuonTruthAlgs/MuonPatternCombinationDetailedTrackTruthMaker.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/MuonPrepDataCollection.h"
#include "MuonPattern/MuonPatternChamberIntersect.h"


using namespace std;


namespace D3PD {
  
  
  MuonPatternCombinationMissedHitFillerTool::MuonPatternCombinationMissedHitFillerTool (const std::string& type,
								      const std::string& name,
								      const IInterface* parent)
    : BlockFillerTool< EventInfo > (type, name, parent) ,
      m_idHelper(0),
      m_truthTool("Trk::DetailedMuonPatternTruthBuilder/DetailedMuonPatternTruthBuilder") {

    declareProperty("DetailedMuonPatternTruthMissedHitTool", m_truthTool);

    book().ignore(); // Avoid coverity warning.
  }
  
  
  StatusCode MuonPatternCombinationMissedHitFillerTool::book() {
    // branches
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
  
  
  StatusCode MuonPatternCombinationMissedHitFillerTool::initialize()
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


  StatusCode MuonPatternCombinationMissedHitFillerTool::fill(const EventInfo&) {
    //build a MuonPatternCombination including all hits
    Amg::Vector3D patpos(0,0,0);
    Amg::Vector3D patdir(0,0,0);
    std::vector<Muon::MuonPatternChamberIntersect> chambers;    
    //MDT
    const Muon::MdtPrepDataContainer* mdtPrds = 0;
    if(StatusCode::SUCCESS != evtStore()->retrieve(mdtPrds, "MDT_DriftCircles")) {
      ATH_MSG_ERROR( " Cannot retrieve MDT PRD Container MDT_DriftCircles");
      return StatusCode::FAILURE;
    }
    Muon::MdtPrepDataContainer::const_iterator mdtit     = mdtPrds->begin();
    Muon::MdtPrepDataContainer::const_iterator mdtit_end = mdtPrds->end();
    for(; mdtit!=mdtit_end; ++mdtit) {
      if( (*mdtit)->size() == 0 ) continue;

      std::vector< const Trk::PrepRawData* > rios;
      Muon::MdtPrepDataCollection::const_iterator it = (*mdtit)->begin();
      Muon::MdtPrepDataCollection::const_iterator it_end = (*mdtit)->end();
      for(; it != it_end; ++it) rios.push_back( *it );
      Muon::MuonPatternChamberIntersect chIntersect(patpos,patdir,rios);
      chambers.push_back(chIntersect);      
    }
    //RPC
    const Muon::RpcPrepDataContainer* rpcPrds = 0;
    if(StatusCode::SUCCESS != evtStore()->retrieve(rpcPrds,"RPC_Measurements")) {
      ATH_MSG_ERROR( "Cannot retrieve the RPC PRD container " );
      return StatusCode::FAILURE;
    }
    Muon::RpcPrepDataContainer::const_iterator rpcit     = rpcPrds->begin();
    Muon::RpcPrepDataContainer::const_iterator rpcit_end = rpcPrds->end();
    for(; rpcit!=rpcit_end; ++rpcit) {
      if( (*rpcit)->size() == 0 ) continue;
      std::vector< const Trk::PrepRawData* > rios;
      Muon::RpcPrepDataCollection::const_iterator it = (*rpcit)->begin();
      Muon::RpcPrepDataCollection::const_iterator it_end = (*rpcit)->end();
      for(; it != it_end; ++it) rios.push_back( *it );
      Muon::MuonPatternChamberIntersect chIntersect(patpos,patdir,rios);
      chambers.push_back(chIntersect);
    }
    //TGC
    const Muon::TgcPrepDataContainer* tgcPrds = 0;
    if(StatusCode::SUCCESS != evtStore()->retrieve(tgcPrds,"TGC_Measurements")) {
      ATH_MSG_ERROR( "Cannot retrieve the TGC PRD container " );
      return StatusCode::FAILURE;
    }
    Muon::TgcPrepDataContainer::const_iterator tgcit     = tgcPrds->begin();
    Muon::TgcPrepDataContainer::const_iterator tgcit_end = tgcPrds->end();
    for(; tgcit!=tgcit_end; ++tgcit) {
      if( (*tgcit)->size() == 0 ) continue;
      std::vector< const Trk::PrepRawData* > rios;
      Muon::TgcPrepDataCollection::const_iterator it = (*tgcit)->begin();
      Muon::TgcPrepDataCollection::const_iterator it_end = (*tgcit)->end();
      for(; it != it_end; ++it) rios.push_back( *it );
      Muon::MuonPatternChamberIntersect chIntersect(patpos,patdir,rios);
      chambers.push_back(chIntersect);
    }
    //CSC
    const Muon::CscPrepDataContainer* cscPrds = 0;
    if(StatusCode::SUCCESS != evtStore()->retrieve(cscPrds,"CSC_Clusters")) {
      ATH_MSG_ERROR( "Cannot retrieve the CSC PRD container " );
      return StatusCode::FAILURE;
    }
    Muon::CscPrepDataContainer::const_iterator cscit     = cscPrds->begin();
    Muon::CscPrepDataContainer::const_iterator cscit_end = cscPrds->end();
    for(; cscit!=cscit_end; ++cscit) {
      if( (*cscit)->size() == 0 ) continue;
      std::vector< const Trk::PrepRawData* > rios;
      Muon::CscPrepDataCollection::const_iterator it = (*cscit)->begin();
      Muon::CscPrepDataCollection::const_iterator it_end = (*cscit)->end();
      for(; it != it_end; ++it) rios.push_back( *it );
      Muon::MuonPatternChamberIntersect chIntersect(patpos,patdir,rios);
      chambers.push_back(chIntersect);
    }
    //build the pattern combination
    auto pattern = std::make_unique<Muon::MuonPatternCombination>
      (new Trk::Perigee(patpos,patdir,1,patpos), chambers);
    pattern->setTrackRoadType(3);


// BEGIN

    if(pattern->chamberData().size() == 0) return StatusCode::SUCCESS;
    
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
    m_truthTool->buildDetailedTrackTruth(dtt, (*pattern), prdCollectionVector);

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

// END HISTO FILLING    


  
    //clean memory and return
    return StatusCode::SUCCESS;
  }
  
  SubDetHitStatistics::SubDetType MuonPatternCombinationMissedHitFillerTool::findSubDetType(Identifier id) {
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
