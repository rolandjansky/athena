/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     HLTTauGenericHypo.cxx
 * PACKAGE:  Trigger/TrigHypothesis/TrigTauHypo
 *
 * AUTHORS:   P.O. DeViveiros
 * CREATED:   Sometime in early 2015
 *
 * DESCRIPTION:
 *
 * Generic cut-based hypo
 *
 *********************************************************************/

#include <iterator>
#include <sstream>

//
#include "TLorentzVector.h"
#include "TFormula.h"

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"
//
#include "TrigTauHypo/HLTTauGenericHypo.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "xAODTau/TauJet.h"
#include "xAODTau/TauJetContainer.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
HLTTauGenericHypo::HLTTauGenericHypo(const std::string& name, 
				     ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator)
{
  declareProperty("Details", m_member);
  declareProperty("Formulas", m_formula);
}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
HLTTauGenericHypo::~HLTTauGenericHypo()
{  
}

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
/////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------
HLT::ErrorCode HLTTauGenericHypo::hltInitialize()
// ----------------------------------------------------------------------
{
  
  ATH_MSG_INFO("in initialize()");
  
  ATH_MSG_INFO(" REGTEST: HLTTauGenericHypo will cut on "          );
  ATH_MSG_INFO(" REGTEST: ------ "                                      );
  
  
  // Here we store the formulas since they need to compile
  for(unsigned int i=0; i<m_member.size(); i++)
    {
      m_store.push_back(TFormula(("TauHypoCut"+m_formula.at(i)).c_str(), m_formula.at(i).c_str()));
      ATH_MSG_INFO(" REGTEST: " << m_formula.at(i));
    }
  ATH_MSG_INFO(" REGTEST: where x is the ID variables, y is Tau pT and z is Tau eta ");

  ATH_MSG_INFO("Initialization of HLTTauGenericHypo completed successfully");
  return HLT::OK;
}

/////////////////////////////////////////////////////////////////////
// FINALIZE:
/////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------
HLT::ErrorCode HLTTauGenericHypo::hltFinalize(){
// ----------------------------------------------------------------------
  
  ATH_MSG_INFO("in finalize()");
  return HLT::OK;
}


/////////////////////////////////////////////////////////////////////
// EXECUTE:
/////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------
HLT::ErrorCode HLTTauGenericHypo::hltExecute(const HLT::TriggerElement* inputTE, bool& pass){
// ----------------------------------------------------------------------
  
  // Get the messaging service, print where you are
  ATH_MSG_DEBUG("REGTEST:"<< name() << ": in execute()");
  
  // general reset
  pass = false;

  // get tau collection
  std::vector<const xAOD::TauJetContainer*> myTauContainers;
  HLT::ErrorCode status = getFeatures(inputTE, myTauContainers);
  if ( status != HLT::OK || myTauContainers.size()==0) {
    ATH_MSG_ERROR("Input tau collection could not be found ");
    return status;
  } else {
    ATH_MSG_DEBUG(" Input tau collection has size " << myTauContainers.size());
    if((myTauContainers.back())->size()==0)
      {
	ATH_MSG_DEBUG("No taus in input collection: Rejecting");
	return status;
      }
  }
  
  const xAOD::TauJet* theTau = (myTauContainers.back())->back();
  
  int hasFailed = 0;

  // We have to implement the cuts here - Eventually, declare the formulas in the initialize
  for(unsigned int i=0; i<m_member.size(); i++)
    {
      float theValue = 0;
      
      // Detail -1 ignores by convention the ID variables
      if(m_member.at(i) != -1)
	{
	  xAOD::TauJetParameters::Detail myDetail = xAOD::TauJetParameters::Detail(m_member.at(i));
	  theTau->detail(myDetail, theValue);
	}
      
      // What about upper and lower bounds? can work using x, y, z, t...
      double theResult = m_store.at(i).Eval(theValue, theTau->pt(), theTau->eta());
      ATH_MSG_DEBUG(" Evaluating Hypothesis on ID Variable #: " << m_member.at(i));
      ATH_MSG_DEBUG(" With Cut = " << m_formula.at(i).c_str());
      ATH_MSG_DEBUG(" And value x (ID), y (pT), z (Eta) = " << theValue <<", "<< theTau->pt() << ", " << theTau->eta());
      ATH_MSG_DEBUG(" Result = " << (theResult > 0.5));

      if(theResult < 0.5)
	{
	  hasFailed = i+1;
	  break;
	}
    }
  
  if (hasFailed)
    {
  ATH_MSG_DEBUG(" REGTEST: Cut Number: "
             << hasFailed-1
             << " did not pass the threshold" );
      return HLT::OK;
    }

  pass = true;
  
  ATH_MSG_DEBUG(" REGTEST: TE accepted !! ");
  
  
  return HLT::OK;
}
