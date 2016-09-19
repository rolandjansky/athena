/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>

#include "TrigNavigation/TriggerElement.h"
//#include "TrigInterfaces/AlgoConfig.h"
#include "TrigNavigation/Navigation.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigGenericAlgs/OverlapRemoval.h"

////////////////////////////////////////////////////////////////////////////////////
/// NB: This code is actually quite dangerous - it assumes "overlap in eta-phi" space 
///     only - when Rois can actually be at different z positions. an RoI is *not* 
///     simple a pyramid purely arising from z=0 !!!!! and the resolution 
///     "Delta eta" does *not* represent the opening of the RoI in eta, since the 
///     eta at the most forward, and most rear z of the RoI are *not* simply 
///     from the "eta resolutions at z=0"
////////////////////////////////////////////////////////////////////////////////////


OverlapRemoval::OverlapRemoval(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::ComboAlgo(name, pSvcLocator),  m_sqMinCentDist(-1.0)
{
  //User can declare either 1) a minimum distance between centers or 2) Minimum Distances in Eta and Phi
  declareProperty("MinCentDist", m_MinCentDist = -1, "This is the minimum center distance for two RoIs to not overlap.");
  declareProperty("MinPhiDist", m_MinPhiDist = -1, "This is the minimum phi distance for two RoIs to not ovverlap.");
  declareProperty("MinEtaDist", m_MinEtaDist = -1, "This is the minimum eta distance for two RoIs to not overlap.");
  declareProperty("RoICheck",   m_RoICheck = true, "Use RoI nodes to retrieve eta, phi ?");

  declareMonitoredVariable("DeltaRSq",m_deltaRSq);
  declareMonitoredVariable("DeltaEta",m_deltaEta);
  declareMonitoredVariable("DeltaPhi",m_deltaPhi);

  declareMonitoredVariable("Eta",m_eta);
  declareMonitoredVariable("Phi",m_phi);



}

HLT::ErrorCode OverlapRemoval::hltInitialize()
{
  if (m_MinPhiDist>0 && m_MinEtaDist>0 && m_MinCentDist >0) {
    msg() << MSG::ERROR << "Can only declare MinCentDist or both MinPhiDist and MinEtaDist for the check! Configuration is not correct" << endmsg;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP) ;
  }

  if ((m_MinPhiDist>0 && m_MinEtaDist <0) || (m_MinPhiDist<0 && m_MinEtaDist>0)) {
    msg() << MSG::ERROR << "Both MinPhiDist and MinEtaDist must be specified! Configuration is not correct" << endmsg;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP) ;
  }

  // calculate delta_R**2 so that we can skip the sqrt operation inside the acceptInputs method !
  m_sqMinCentDist = m_MinCentDist*m_MinCentDist;

  return HLT::OK;
}

HLT::ErrorCode OverlapRemoval::acceptInputs(HLT::TEConstVec& tes_in, bool& pass)
{
  if ( msgLvl() <= MSG::VERBOSE )
    msg() << MSG::VERBOSE << "Executing this OverlapRemoval " << name() << endmsg;

  double phi0, phi1, eta0, eta1;

  m_deltaRSq = -99.0;
  m_deltaEta = -99.0;
  m_deltaPhi = -99.0;
  m_eta = -99.0;
  m_phi = -99.0;

  int NumArg = tes_in.size(); //Find the number of types to compare
  const HLT::TriggerElement* te0;
  const HLT::TriggerElement* te1;

  //Nested for loops to run the algorithm over all combinations
  for(int j=0; j < (NumArg-1); ++j) {
    for(int i=(j+1); i < NumArg; ++i) {

      // some DEBUG output
      if ( msgLvl() <= MSG::DEBUG ) {
	msg() << MSG::DEBUG << "Executing this OverlapRemoval " << name() << " for types "
	      << tes_in[j]->getId() << " and " << tes_in[i]->getId() <<  endmsg;
      }

      // the following line would prevent delta_R checks of TEs of the same type, let's say EM25i, EM25i !!
      // if ( tes_in[j]->getId() == tes_in[i]->getId() ) continue;

      // get feature like TrigRoiDescriptor

      if (m_RoICheck) {
	HLT::TEVec rois0 = config()->getNavigation()->getRoINodes( tes_in[j] );
	HLT::TEVec rois1 = config()->getNavigation()->getRoINodes( tes_in[i] );
	if (rois0.empty()) {
	  msg() << MSG::ERROR << tes_in[j]->getId() << " not seeded from any RoI!" << endmsg;
	  return HLT::ErrorCode(HLT::Action::ABORT_EVENT, HLT::Reason::NAV_ERROR);
	}
	if (rois1.empty()) {
	  msg() << MSG::ERROR << tes_in[i]->getId() << " not seeded from any RoI!" << endmsg;
	  return HLT::ErrorCode(HLT::Action::ABORT_EVENT, HLT::Reason::NAV_ERROR);
	}
	// okay, but how does that work for topological triggers ?
	// let's say, we want to do an eta,phi check of two Zs coming from 4 EM RoIs in total ... ?
	te0 = *rois0.begin();
	te1 = *rois1.begin();
      } else {
	te0 = tes_in[j];
	te1 = tes_in[i];
      }
      std::vector<const TrigRoiDescriptor*> features0;
      std::vector<const TrigRoiDescriptor*> features1;
      const TrigRoiDescriptor* descr0 = 0;
      const TrigRoiDescriptor* descr1 = 0;

      // get eta,phi from first TE node:
      if (HLT::OK != getFeatures(te0, features0) ) {
	msg() << MSG::ERROR << "TE/RoI with no TrigRoiDescriptor attached!" << endmsg;
	return HLT::ERROR;
      } else {
	if (!features0.empty()) {
	  descr0 = *features0.begin();
	  if (descr0 != 0) {
	    if ( msgLvl() <= MSG::VERBOSE ) {
	      msg() << MSG::VERBOSE << "TE/RoI node has TrigRoiDescriptor attached!: "  << features0.size()
		    << *descr0 << endmsg;
	    }
	    phi0 = descr0->phi();
	    eta0 = descr0->eta();
	  } else {
	    msg() << MSG::ERROR << "the received TrigRoiDescriptor object is 0 ?!? " << endmsg;
	    return HLT::ERROR;
	  }
	} else {
	  msg() << MSG::ERROR << "TE/RoI node has no TrigRoiDescriptor attached!: "  << endmsg;
	  return HLT::ERROR;
	}
      }
      // get eta,phi from second TE node:
      if (HLT::OK != getFeatures(te1, features1) ) {
	msg() << MSG::ERROR << "  RoI with no TrigRoiDescriptor attached!" << endmsg;
	return HLT::ERROR;
      } else {
	if (!features1.empty()) {
	  descr1 = *features1.begin();
	  if ( descr1 != 0) {
	    if ( msgLvl() <= MSG::VERBOSE ) {
	      msg() << MSG::VERBOSE << "TE/RoI node has TrigRoiDescriptor attached!: "  << features1.size()
		    << *descr1 << endmsg;
	    }
	    phi1 = descr1->phi();
	    eta1 = descr1->eta();
	  } else {
	    msg() << MSG::ERROR << "the received TrigRoiDescriptor object is 0 ?!? " << endmsg;
	    return HLT::ERROR;
	  }
	} else {
	  msg() << MSG::ERROR << "TE/RoI node has no TrigRoiDescriptor attached!: "  << endmsg;
	  return HLT::ERROR;
	}
      }

      // Account for the fact that the eta-phi plane surface is actually on a cylinder
      double deltaphi;
      if(fabs(phi0-phi1) <= M_PI) {
	deltaphi = phi0-phi1;
      } else {
	deltaphi = 2*(M_PI) - fabs(phi0-phi1);
      }

      deltaphi = phi0-phi1;
      if(deltaphi<-M_PI) deltaphi+=2.0*M_PI;
      if(deltaphi>M_PI) deltaphi-=2.0*M_PI;

      double deltaeta = fabs(eta0-eta1);

      double sqActCentDist = deltaphi*deltaphi + deltaeta*deltaeta;
      

      /// this is dangerous indeed !!!! it determines whether RoIs "overlap"
      /// based on purely pyramidal RoIs from z=0, based purely on the 
      /// difference i the eta-phi location with *no* reference to the 
      /// actual size of the RoIs themselves!! This is not a "overlap"
      /// at all, but a "central separation" 
      m_deltaRSq = sqActCentDist;
      m_deltaEta = deltaeta;
      m_deltaPhi = deltaphi;
      m_eta = eta0;
      m_phi = phi0;

     
      if (m_MinPhiDist >0 && m_MinEtaDist>0) {
	// Compare Deltaphi and Deltaphi with user-chosen minimum deltaphi and deltaeta
	// This if statement will only work as long as the user doesn't input deltaphi/deltaeta which are smaller
	// than the widths of the RoI's so choose your inputs carefully!
	if (deltaphi <= m_MinPhiDist || deltaeta<=m_MinEtaDist) {
	  if ( msgLvl() <= MSG::DEBUG ) {
	    msg() << MSG::DEBUG << "The two RoIs are overlapping:" << endmsg;
	    msg() << MSG::DEBUG << *descr0 << endmsg;
	    msg() << MSG::DEBUG << *descr1 << endmsg;
	    msg() << MSG::DEBUG << "Deltaphi between centers is: " << deltaphi  << " Deltaeta betwen centers is: "
		  << deltaeta << ". MinimumDeltaPhi between centers is: " << m_MinPhiDist
		  << " MinimumDeltaEta between centers is: " << m_MinEtaDist <<endmsg;
	  }
	  pass = false;
	  return HLT::OK;
	} else {
	  if ( msgLvl() <= MSG::DEBUG ) {
	    msg() << MSG::DEBUG <<"The two RoIs do not not overlap: " << endmsg;
	    msg() << MSG::DEBUG << *descr0 << endmsg;
	    msg() << MSG::DEBUG << *descr1 << endmsg;
	    msg() << MSG::DEBUG << "Deltaphi between centers is: " << deltaphi  << " Deltaeta betwen centers is: " << deltaeta
		  << ". MinimumDeltaPhi between centers is: " << m_MinPhiDist << " MinimumDeltaEta between centers is: "
		  << m_MinEtaDist <<endmsg;
	  }
	}
      } else {
	//Compare Euclidean distance with the user-chosen minimum distance
	if ( sqActCentDist <= m_sqMinCentDist ) {
	  if ( msgLvl() <= MSG::DEBUG ) {
	    msg() << MSG::DEBUG << "The two RoIs are overlapping: " << endmsg;
	    msg() << MSG::DEBUG << *descr0 << endmsg;
	    msg() << MSG::DEBUG << *descr1 << endmsg;
	    msg() << MSG::DEBUG << "SqDistance between centers is: " << sqActCentDist
		  << ". sqMinimumDistance between centers is: " << m_sqMinCentDist << endmsg;
	  }
	  pass = false;
	  return HLT::OK;
	} else {
	  if ( msgLvl() <= MSG::DEBUG ) {
	    msg() << MSG::DEBUG <<"The two RoIs do not not overlap: " << endmsg;
	    msg() << MSG::DEBUG << *descr0 << endmsg;
	    msg() << MSG::DEBUG << *descr1 << endmsg;
	    msg() << MSG::DEBUG << "sqMinimumDistance between centers is: " << m_sqMinCentDist << ". sqActual Distance is: "
		  << sqActCentDist << endmsg;
	  }
	}
      }
    }
  }
  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "This combination is non-overlaping" << endmsg;
  }
  pass = true; // we pass this combination
  return HLT::OK;
}


HLT::ErrorCode OverlapRemoval::hltExecute(std::vector<const HLT::TriggerElement*>& /*inputs*/,
			  HLT::TriggerElement* /*output*/) {
  //  output->setGhostState(true);
  return HLT::OK;
}

