/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TgcDataPreparator.h"
#include "TgcData.h"
#include "RecMuonRoIUtils.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::TgcDataPreparator::TgcDataPreparator(const std::string& type,
						   const std::string& name,
						   const IInterface*  parent):
  AthAlgTool(type,name,parent)
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::TgcDataPreparator::initialize()
{
   // Get a message stream instance
  ATH_MSG_DEBUG("Initializing TgcDataPreparator - package version " << PACKAGE_VERSION );

   ATH_CHECK(m_idHelperSvc.retrieve());

   ATH_CHECK(m_tgcContainerKey.initialize());

   return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::TgcDataPreparator::prepareData(const LVL1::RecMuonRoI*  p_roi,
							TrigL2MuonSA::TgcHits&  tgcHits) const
{
   float roi_eta = p_roi->eta();
   float roi_phi = p_roi->phi();
   if (roi_phi < 0) roi_phi += 2.0 * M_PI;

   //const Muon::TgcPrepDataContainer* tgcPrepContainer = 0;
   const Muon::TgcPrepDataContainer* tgcPrepContainer;
   int gasGap;
   int channel;

   const bool isLowPt = m_recMuonRoIUtils.isLowPt(p_roi);

   // Select the eta cut based on ROI Pt.
   const double mid_eta_test = (isLowPt) ? m_options.roadParameters().deltaEtaAtMiddleForLowPt()
     : m_options.roadParameters().deltaEtaAtMiddleForHighPt();
   const double inn_eta_test = (isLowPt) ? m_options.roadParameters().deltaEtaAtInnerForLowPt()
     : m_options.roadParameters().deltaEtaAtInnerForHighPt();
   const double mid_phi_test = m_options.roadParameters().deltaPhiAtMiddle();
   const double inn_phi_test = m_options.roadParameters().deltaPhiAtInner();

   auto tgcContainerHandle = SG::makeHandle(m_tgcContainerKey);
   tgcPrepContainer = tgcContainerHandle.cptr();
   if (!tgcContainerHandle.isValid()) {
     ATH_MSG_ERROR("Could not retrieve PrepDataContainer key:" << m_tgcContainerKey.key());
     return StatusCode::FAILURE;
   } else {
     ATH_MSG_DEBUG("Retrieved PrepDataContainer: " << tgcPrepContainer->numberOfCollections());
   }

   //Find closest wires in Middle
   float min_dphi_wire=1000.;
   float second_dphi_wire=1000.;
   std::vector<float> ov_dphi;
   ov_dphi.clear();
   for( const Muon::TgcPrepDataCollection* wi : *tgcPrepContainer ) { // loop over collections
     if( !wi ) continue;
     for( const Muon::TgcPrepData* cwi : *wi ){ // loop over data in the collection
       if( !cwi ) continue;
       const Muon::TgcPrepData& prepDataWi = *cwi;
       if (!m_idHelperSvc->tgcIdHelper().isStrip(prepDataWi.identify())) {//wire
         int stationNumWi = m_idHelperSvc->tgcIdHelper().stationRegion(prepDataWi.identify())-1;
         if (stationNumWi==-1) stationNumWi=3;
         if (stationNumWi<3 && std::abs(prepDataWi.globalPosition().eta() - roi_eta) < mid_eta_test ) {
           const float dphi = std::acos(std::cos(prepDataWi.globalPosition().phi()-roi_phi));
           bool overlap=false;
           for (unsigned int ov=0;ov<ov_dphi.size();ov++)
             if (std::abs(dphi-ov_dphi[ov])<1e-5) overlap=true;
           if (overlap) continue;
           ov_dphi.push_back(dphi);
           if (dphi<second_dphi_wire){
             second_dphi_wire=dphi;
           }
           if (dphi<min_dphi_wire) {
             second_dphi_wire=min_dphi_wire;
             min_dphi_wire=dphi;
           }
         }
       }
     }
   }

   //Check if there are enough number of hits
   int num_min_hits=0;
   int num_second_hits=0;
   for( const Muon::TgcPrepDataCollection* hit : *tgcPrepContainer ) { // loop over collections
     if( !hit ) continue;
     for( const Muon::TgcPrepData* chit : *hit ){ // loop over data in the collection
       if( !chit ) continue;
       const Muon::TgcPrepData& prepDataHit = *chit;
       if (!m_idHelperSvc->tgcIdHelper().isStrip(prepDataHit.identify())) {//strip
         int stationNumHit = m_idHelperSvc->tgcIdHelper().stationRegion(prepDataHit.identify())-1;
         if (stationNumHit==-1) stationNumHit=3;
         if (stationNumHit<3 && std::abs(prepDataHit.globalPosition().eta() - roi_eta) < mid_eta_test ) {
           const float dphi = std::acos(std::cos(prepDataHit.globalPosition().phi()-roi_phi));
           if (std::abs(dphi-min_dphi_wire)<1e-5) num_min_hits++;
           if (std::abs(dphi-second_dphi_wire)<1e-5) num_second_hits++;
         }
       }
     }
   }

   float dphi_wire=min_dphi_wire;
   bool useDefault=false;
   if (num_min_hits<5) {
     if (num_second_hits>5) dphi_wire=second_dphi_wire;
     else useDefault=true;
   }

   for( const Muon::TgcPrepDataCollection* col : *tgcPrepContainer ) { // loop over collections
     if( !col ) continue;
     for( const Muon::TgcPrepData* cit : *col ){ // loop over data in the collection
       if( !cit ) continue;
       const Muon::TgcPrepData& prepData = *cit;

       bool isInRoad = false;
       int stationNum = m_idHelperSvc->tgcIdHelper().stationRegion(prepData.identify())-1;
       if (stationNum==-1) stationNum=3;
       if (m_idHelperSvc->tgcIdHelper().isStrip(prepData.identify())) {
	 double dphi = std::abs(prepData.globalPosition().phi() - roi_phi);
	 if( dphi > M_PI*2 ) dphi = dphi - M_PI*2;
	 if( dphi > M_PI ) dphi = M_PI*2 - dphi;
	 // For strips, apply phi cut
	 if     ( stationNum < 3  && dphi < mid_phi_test ) { isInRoad = true; }
	 else if( stationNum == 3 && dphi < inn_phi_test ) { isInRoad = true; }
       }
       else {
	 // For wires, apply eta cut.
         const float dphi = std::acos(std::cos(prepData.globalPosition().phi()-roi_phi));
	 if     ( stationNum < 3  && std::abs(prepData.globalPosition().eta() - roi_eta) < mid_eta_test ) {
           if (useDefault) isInRoad = true;//default
           else if (std::abs(dphi-dphi_wire)<1e-5) isInRoad = true;//for close-by muon
         }
	 else if( stationNum == 3 && std::abs(prepData.globalPosition().eta() - roi_eta) < inn_eta_test ) { isInRoad = true; }
       }
       if( ! isInRoad ) continue;

       const MuonGM::TgcReadoutElement* tgcReadout = prepData.detectorElement();
       gasGap = m_idHelperSvc->tgcIdHelper().gasGap(prepData.identify());
       channel = m_idHelperSvc->tgcIdHelper().channel(prepData.identify());

       TrigL2MuonSA::TgcHitData lutDigit;

       lutDigit.eta = prepData.globalPosition().eta();
       lutDigit.phi = prepData.globalPosition().phi();
       lutDigit.r = prepData.globalPosition().perp();
       lutDigit.z = prepData.globalPosition().z();
       lutDigit.sta = stationNum;
       lutDigit.isStrip = m_idHelperSvc->tgcIdHelper().isStrip(prepData.identify());
       if(m_idHelperSvc->tgcIdHelper().isStrip(prepData.identify())){
	 lutDigit.width = tgcReadout->stripWidth(gasGap, channel);
       }
       else{
	 lutDigit.width = tgcReadout->gangLength(gasGap, channel);
       }
       lutDigit.bcTag = 2;
       lutDigit.inRoad = false;


       tgcHits.push_back(lutDigit);

     }
   }

   return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::TgcDataPreparator::prepareData(const xAOD::MuonRoI*    p_roi,
							TrigL2MuonSA::TgcHits&  tgcHits) const
{
   float roi_eta = p_roi->eta();
   float roi_phi = p_roi->phi();
   if (roi_phi < 0) roi_phi += 2.0 * M_PI;

   //const Muon::TgcPrepDataContainer* tgcPrepContainer = 0;
   const Muon::TgcPrepDataContainer* tgcPrepContainer;
   int gasGap;
   int channel;

   const bool isLowPt = m_recMuonRoIUtils.isLowPt(p_roi);

   // Select the eta cut based on ROI Pt.
   const double mid_eta_test = (isLowPt) ? m_options.roadParameters().deltaEtaAtMiddleForLowPt()
     : m_options.roadParameters().deltaEtaAtMiddleForHighPt();
   const double inn_eta_test = (isLowPt) ? m_options.roadParameters().deltaEtaAtInnerForLowPt()
     : m_options.roadParameters().deltaEtaAtInnerForHighPt();
   const double mid_phi_test = m_options.roadParameters().deltaPhiAtMiddle();
   const double inn_phi_test = m_options.roadParameters().deltaPhiAtInner();

   auto tgcContainerHandle = SG::makeHandle(m_tgcContainerKey);
   tgcPrepContainer = tgcContainerHandle.cptr();
   if (!tgcContainerHandle.isValid()) {
     ATH_MSG_ERROR("Could not retrieve PrepDataContainer key:" << m_tgcContainerKey.key());
     return StatusCode::FAILURE;
   } else {
     ATH_MSG_DEBUG("Retrieved PrepDataContainer: " << tgcPrepContainer->numberOfCollections());
   }

   //Find closest wires in Middle
   float min_dphi_wire=1000.;
   float second_dphi_wire=1000.;
   std::vector<float> ov_dphi;
   ov_dphi.clear();
   for( const Muon::TgcPrepDataCollection* wi : *tgcPrepContainer ) { // loop over collections
     if( !wi ) continue;
     for( const Muon::TgcPrepData* cwi : *wi ){ // loop over data in the collection
       if( !cwi ) continue;
       const Muon::TgcPrepData& prepDataWi = *cwi;
       if (!m_idHelperSvc->tgcIdHelper().isStrip(prepDataWi.identify())) {//wire
         int stationNumWi = m_idHelperSvc->tgcIdHelper().stationRegion(prepDataWi.identify())-1;
         if (stationNumWi==-1) stationNumWi=3;
         if (stationNumWi<3 && std::abs(prepDataWi.globalPosition().eta() - roi_eta) < mid_eta_test ) {
           const float dphi = std::acos(std::cos(prepDataWi.globalPosition().phi()-roi_phi));
           bool overlap=false;
           for (unsigned int ov=0;ov<ov_dphi.size();ov++)
             if (std::abs(dphi-ov_dphi[ov])<1e-5) overlap=true;
           if (overlap) continue;
           ov_dphi.push_back(dphi);
           if (dphi<second_dphi_wire){
             second_dphi_wire=dphi;
           }
           if (dphi<min_dphi_wire) {
             second_dphi_wire=min_dphi_wire;
             min_dphi_wire=dphi;
           }
         }
       }
     }
   }

   //Check if there are enough number of hits
   int num_min_hits=0;
   int num_second_hits=0;
   for( const Muon::TgcPrepDataCollection* hit : *tgcPrepContainer ) { // loop over collections
     if( !hit ) continue;
     for( const Muon::TgcPrepData* chit : *hit ){ // loop over data in the collection
       if( !chit ) continue;
       const Muon::TgcPrepData& prepDataHit = *chit;
       if (!m_idHelperSvc->tgcIdHelper().isStrip(prepDataHit.identify())) {//strip
         int stationNumHit = m_idHelperSvc->tgcIdHelper().stationRegion(prepDataHit.identify())-1;
         if (stationNumHit==-1) stationNumHit=3;
         if (stationNumHit<3 && std::abs(prepDataHit.globalPosition().eta() - roi_eta) < mid_eta_test ) {
           const float dphi = std::acos(std::cos(prepDataHit.globalPosition().phi()-roi_phi));
           if (std::abs(dphi-min_dphi_wire)<1e-5) num_min_hits++;
           if (std::abs(dphi-second_dphi_wire)<1e-5) num_second_hits++;
         }
       }
     }
   }

   float dphi_wire=min_dphi_wire;
   bool useDefault=false;
   if (num_min_hits<5) {
     if (num_second_hits>5) dphi_wire=second_dphi_wire;
     else useDefault=true;
   }

   for( const Muon::TgcPrepDataCollection* col : *tgcPrepContainer ) { // loop over collections
     if( !col ) continue;
     for( const Muon::TgcPrepData* cit : *col ){ // loop over data in the collection
       if( !cit ) continue;
       const Muon::TgcPrepData& prepData = *cit;

       bool isInRoad = false;
       int stationNum = m_idHelperSvc->tgcIdHelper().stationRegion(prepData.identify())-1;
       if (stationNum==-1) stationNum=3;
       if (m_idHelperSvc->tgcIdHelper().isStrip(prepData.identify())) {
	 double dphi = std::abs(prepData.globalPosition().phi() - roi_phi);
	 if( dphi > M_PI*2 ) dphi = dphi - M_PI*2;
	 if( dphi > M_PI ) dphi = M_PI*2 - dphi;
	 // For strips, apply phi cut
	 if     ( stationNum < 3  && dphi < mid_phi_test ) { isInRoad = true; }
	 else if( stationNum == 3 && dphi < inn_phi_test ) { isInRoad = true; }
       }
       else {
	 // For wires, apply eta cut.
         const float dphi = std::acos(std::cos(prepData.globalPosition().phi()-roi_phi));
	 if     ( stationNum < 3  && std::abs(prepData.globalPosition().eta() - roi_eta) < mid_eta_test ) {
           if (useDefault) isInRoad = true;//default
           else if (std::abs(dphi-dphi_wire)<1e-5) isInRoad = true;//for close-by muon
         }
	 else if( stationNum == 3 && std::abs(prepData.globalPosition().eta() - roi_eta) < inn_eta_test ) { isInRoad = true; }
       }
       if( ! isInRoad ) continue;

       const MuonGM::TgcReadoutElement* tgcReadout = prepData.detectorElement();
       gasGap = m_idHelperSvc->tgcIdHelper().gasGap(prepData.identify());
       channel = m_idHelperSvc->tgcIdHelper().channel(prepData.identify());

       TrigL2MuonSA::TgcHitData lutDigit;

       lutDigit.eta = prepData.globalPosition().eta();
       lutDigit.phi = prepData.globalPosition().phi();
       lutDigit.r = prepData.globalPosition().perp();
       lutDigit.z = prepData.globalPosition().z();
       lutDigit.sta = stationNum;
       lutDigit.isStrip = m_idHelperSvc->tgcIdHelper().isStrip(prepData.identify());
       if(m_idHelperSvc->tgcIdHelper().isStrip(prepData.identify())){
	 lutDigit.width = tgcReadout->stripWidth(gasGap, channel);
       }
       else{
	 lutDigit.width = tgcReadout->gangLength(gasGap, channel);
       }
       lutDigit.bcTag = 2;
       lutDigit.inRoad = false;


       tgcHits.push_back(lutDigit);

     }
   }

   return StatusCode::SUCCESS;
}
