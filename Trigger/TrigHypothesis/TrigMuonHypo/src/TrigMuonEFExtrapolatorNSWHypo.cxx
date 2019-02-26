/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonHypo/TrigMuonEFExtrapolatorNSWHypo.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrkSegment/SegmentCollection.h"
#include "MuonSegment/MuonSegment.h"
#include "TrigT1Interfaces/RecMuonRoI.h"

class ISvcLocator;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigMuonEFExtrapolatorNSWHypo::TrigMuonEFExtrapolatorNSWHypo(const std::string & name, ISvcLocator* pSvcLocator) :
   HLT::HypoAlgo(name, pSvcLocator),
   m_edmhelperTool("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
   m_idhelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool")
{
   declareProperty("AcceptAll", m_acceptAll=true);

   declareMonitoredVariable("n_seg_passed",    m_fex_n_seg_passed);
   declareMonitoredVariable("n_seg_sameside",  m_fex_n_seg_sameside);
   declareMonitoredVariable("failed_eta",      m_fex_failed_eta);
   declareMonitoredVariable("failed_phi",      m_fex_failed_phi);

   declareMonitoredStdContainer("dTheta",  m_fex_dTheta);
   declareMonitoredStdContainer("dLEta",   m_fex_dLeta);
   declareMonitoredStdContainer("dLPhi",   m_fex_dLphi);

   m_storeGate=0;
}

TrigMuonEFExtrapolatorNSWHypo::~TrigMuonEFExtrapolatorNSWHypo(){
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode TrigMuonEFExtrapolatorNSWHypo::hltInitialize()
{
   if( m_acceptAll ) {
      msg() << MSG::INFO << "Accepting all the events with not cut!" << endmsg;
   }

   StatusCode sc = m_edmhelperTool.retrieve();
   if ( sc.isSuccess() ) {
      msg() << MSG::INFO << "Retrieved " << m_edmhelperTool << endmsg;
   } else {
      msg() << MSG::ERROR << "Could not get " << m_edmhelperTool << endmsg; 
      return HLT::ERROR;
   }

   sc = m_idhelperTool.retrieve();
   if ( sc.isSuccess() ) {
      msg() << MSG::INFO << "Retrieved " << m_idhelperTool << endmsg;
   } else {
      msg() << MSG::ERROR << "Could not get " << m_idhelperTool << endmsg; 
      return HLT::ERROR;
   }

   msg() << MSG::INFO << "Initialization completed successfully" << endmsg;

   return HLT::OK;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode TrigMuonEFExtrapolatorNSWHypo::hltFinalize()
{
   msg() << MSG::INFO << "in finalize()" << endmsg;
   return HLT::OK;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode TrigMuonEFExtrapolatorNSWHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{
   m_storeGate = store();

   bool debug = msgLvl() <= MSG::DEBUG;

   if(debug) msg() << MSG::DEBUG << "in execute()" << endmsg;
   
   //resetting the monitoring variables
   m_fex_n_seg_passed   = -1;
   m_fex_n_seg_sameside = -1;
   m_fex_failed_eta = -5;
   m_fex_failed_phi = -5;
   m_fex_dTheta.clear();
   m_fex_dLeta.clear();
   m_fex_dLphi.clear();

   // if acceptAll
   if( m_acceptAll ) {
      pass = true;
      if(debug) msg() << MSG::DEBUG << "Accept property is set: taking all the events" << endmsg;
      return HLT::OK;
   }

   if(debug) msg() << MSG::DEBUG << "outputTE->ID(): " << outputTE->getId() << endmsg;

   // get initial RoI info
   double roi_eta = 0;
   double roi_phi = 0;
   std::vector<const LVL1::RecMuonRoI*> vRecMuonRoI;
   HLT::ErrorCode hltStatus = getFeatures(outputTE, vRecMuonRoI, "");
   if( hltStatus==HLT::OK && vRecMuonRoI.size()==1) {
      roi_eta = vRecMuonRoI[0]->eta();
      roi_phi = vRecMuonRoI[0]->phi();
   }
   else {
      if(debug) msg() << MSG::DEBUG << "Failure in retrieving RecMuonRoI. Trying forMS RoiDescriptor" << endmsg;
      const TrigRoiDescriptor* muonRoI = 0;
      hltStatus = getFeature(outputTE, muonRoI, "forMS");
      if (hltStatus != HLT::OK) {
	 if (debug) msg() << MSG::DEBUG << "Failed to retrieve forMS RoiDescriptor" << endmsg;
	 return hltStatus;
      }
      else if (!muonRoI) {
	 if (debug) msg() << MSG::DEBUG  << "Could not find RoIDescriptor <<forMS>>, "
			  << "trying to recover the initial one." << endmsg;
	 hltStatus = getFeature(outputTE, muonRoI, "");
	 if (hltStatus != HLT::OK) {
	    if (debug) msg() << MSG::DEBUG << "Failed to retrieve muon RoI" << endmsg;
	    return hltStatus;
	 } else if (!muonRoI) {
	    if (debug) msg() << MSG::DEBUG << "Initial one not accessible, problem here!" << endmsg;
	    return HLT::NAV_ERROR;
	 } else {
	    if (debug) msg() << MSG::DEBUG << "Found RoIDescriptor" << endmsg;
	 }
      } else {
	 if (debug) msg() << MSG::DEBUG  << "Found RoIDescriptor <<forMS>>" << endmsg;
      }
      //
      roi_eta = muonRoI->eta();
      roi_phi = muonRoI->phi();
   }
   if(debug) {
      msg() << MSG::DEBUG  << "RoI: eta/phi=" << roi_eta << "/" << roi_phi << endmsg;
   }

   // if outside NSW no cut at all
   const double ETA_NSW = 1.3;
   if( fabs(roi_eta) < ETA_NSW ) {
      pass = true;
      if (debug) msg() << MSG::DEBUG << "outside NSW -> pass" << endmsg;
      return HLT::OK;
   }

   // do NSW cuts
   int n_seg_passed   = 0;
   int n_seg_sameside = 0; // for monitoring

   // get MuonSegmentCombinationCollection
   std::vector<const Trk::SegmentCollection*> vectorOfSegmentCollection;
   if(getFeatures(outputTE, vectorOfSegmentCollection)!=HLT::OK) {
      if (debug) msg() << MSG::DEBUG << "No SegmentCollection found" << endmsg;
      return HLT::MISSING_FEATURE;
   } 
   else {
      if (debug) msg() << MSG::DEBUG << "vector of SegmentCollection found with size=" << vectorOfSegmentCollection.size() << endmsg;
   } // this size should be 1.

   // loop on MuonSegmentCombination
   for (unsigned int i=0; i<vectorOfSegmentCollection.size(); i++) {

      if (debug) msg() << MSG::DEBUG << "++ Element " << i << " of vector of SegmentCollection ++" << endmsg;

      const Trk::SegmentCollection* segColl = vectorOfSegmentCollection[i];
      if(!segColl){
	 msg() << MSG::ERROR << "Retrieval of SegmentCollection from vector failed" << endmsg;
	 return HLT::NAV_ERROR;
      }
      else {
	 if (debug) msg() << MSG::DEBUG << "SegmentCollection OK with size=" << segColl->size() << endmsg;
      }

      Trk::SegmentCollection::const_iterator segItr  = segColl->begin();
      Trk::SegmentCollection::const_iterator segItrE = segColl->end();
      for (int j=0; segItr != segItrE; ++segItr, ++j ) {

	 msg() << MSG::DEBUG << "-- Segment:" << j << " --" << endmsg;

	 const Muon::MuonSegment* segment=dynamic_cast<const Muon::MuonSegment*>(*segItr);

	 // get chamber identifier, chamber index and station index
	 Identifier chid = m_edmhelperTool->chamberId( *segment );
	 Muon::MuonStationIndex::ChIndex chIndex = m_idhelperTool->chamberIndex(chid);
	 if (debug) msg() << MSG::DEBUG << "  chamber index=" << chIndex << endmsg;

	 // only in EI and CSC
	 if( chIndex!=Muon::MuonStationIndex::EIS && chIndex!=Muon::MuonStationIndex::EIL &&
	     chIndex!=Muon::MuonStationIndex::CSS && chIndex!=Muon::MuonStationIndex::CSL ) {
	   continue;
	 }

	 const Amg::Vector3D& dir = segment->globalDirection();
	 const Amg::Vector3D& pos = segment->globalPosition();
	 if (debug) {
	   msg() << MSG::DEBUG << "    GlobalDirection eta/phi=" << dir.eta() << "/" << dir.phi() << endmsg;
	   msg() << MSG::DEBUG << "    GlobalPosition x/y/z=" << pos.x() << "/" << pos.y() << "/" << pos.z() << endmsg;
	 }

	 // same side segments only
	 if( (pos.eta() * roi_eta < 0) ) continue;

	 n_seg_sameside++;

	 // dTheta: direction and position theta difference
	 double dTheta = pos.theta() - dir.theta();
	 if( roi_eta < 0 ) dTheta *= -1;
	 bool isPassed_dTheta = dThetaCut(roi_eta, dTheta);

	 // dL: NSW position and RoI position difference
	 double dLeta = fabs(pos.eta() - roi_eta);
	 double dLphi = fabs(pos.phi() - roi_phi);
	 if( dLphi > CLHEP::pi ) dLphi = CLHEP::twopi - dLphi;
	 bool isPassed_dL = dLCut(roi_eta, dLeta, dLphi);

	 // monitoring
	 m_fex_dTheta.push_back(dTheta);
	 m_fex_dLeta.push_back(dLeta);
	 m_fex_dLphi.push_back(dLphi);
	 if(debug) {
	   msg() << MSG::DEBUG << "  dTheta=" << dTheta << ": isPassed=" << isPassed_dTheta << endmsg;
	   msg() << MSG::DEBUG << "  dLeta/phi=" << dLeta << "/" << dLphi << ": isPassed_dL=" << isPassed_dL << endmsg;
	 }
	     
	 // this segment passes cut
	 if( isPassed_dTheta && isPassed_dL ) n_seg_passed++;
      } //end of segment loop
   } // end of vector SegColl loop

   // monitoring
   m_fex_n_seg_passed   = n_seg_passed;
   m_fex_n_seg_sameside = n_seg_sameside;

   // decision
   pass = (n_seg_passed>=1);
   if(debug) msg() << MSG::DEBUG << " REGTEST pass=" << pass << endmsg;
   if( ! pass ) {
      m_fex_failed_eta = roi_eta;
      m_fex_failed_phi = roi_phi;
   }

   return HLT::OK;
}  

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

bool TrigMuonEFExtrapolatorNSWHypo::dThetaCut(double eta, double dTheta)
{
   double feta    = fabs(eta);
   double fdtheta = fabs(dTheta);

   if( feta < 1.2 ) return true;

   bool retval = false;
   if( feta < 1.4 ) {
      if( fdtheta < 0.015 ) retval = true;
   }
   else if( feta < 1.6 ) {
      if( fdtheta < 0.015 ) retval = true;
   }
   else if( feta < 1.8 ) {
      if( fdtheta < 0.010 ) retval = true;
   }
   else if( feta < 2.0 ) {
      if( fdtheta < 0.025 ) retval = true;
   }
   else if( feta < 2.2 ) {
      if( fdtheta < 0.070 ) retval = true;
   }
   else {
      if( fdtheta < 0.070 ) retval = true;
   }
   
   return retval;
}

bool TrigMuonEFExtrapolatorNSWHypo::dLCut(double eta, double dLeta, double dLphi)
{
   double feta   = fabs(eta);
   double fdleta = fabs(dLeta);
   double fdlphi = fabs(dLphi);

   if( fdlphi > 0.06 ) return false;

   bool retval = false;
   if( feta < 2.0 ) {
      if( fdleta < 0.05 ) retval = true;
   }
   else {
      if( fdleta < 0.07 ) retval = true;
   }

   return retval;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
