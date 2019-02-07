/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TrigTauGenericHypoMT.cxx
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
#include "TrigTauGenericHypoMT.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "xAODTau/TauJet.h"
#include "xAODTau/TauJetContainer.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/Combinators.h"
#include "AthenaMonitoring/Monitored.h"

//class ISvcLocator;

using namespace TrigCompositeUtils;

TrigTauGenericHypoMT::TrigTauGenericHypoMT( const std::string& type,
                  const std::string& name, 
                  const IInterface* parent ) 
  : base_class( type, name, parent ),
    m_decisionId( HLT::Identifier::fromToolName( name ) ) 
{
  declareProperty("Details", m_member);
  declareProperty("Formulas", m_formula);
}

TrigTauGenericHypoMT::~TrigTauGenericHypoMT()
{}

StatusCode TrigTauGenericHypoMT::initialize()
{

  if ( !m_monTool.empty() ) CHECK( m_monTool.retrieve() );

  ATH_MSG_DEBUG( "AcceptAll            = " 
      << ( m_acceptAll==true ? "True" : "False" ) ); 

  //m_multiplicity = m_lowerPtCut.size();
  ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_decisionId  );

  ATH_MSG_INFO(" REGTEST: TrigTauGenericHypoMT will cut on ");
  ATH_MSG_INFO(" REGTEST: ------ ");

  // Here we store the formulas since they need to compile
  for(unsigned int i=0; i<m_member.size(); i++)
  {
     m_store.push_back(TFormula(("TauHypoCut"+m_formula.at(i)).c_str(), m_formula.at(i).c_str()));
     ATH_MSG_INFO(" REGTEST: " << m_formula.at(i));
  }
  ATH_MSG_INFO(" REGTEST: where x is the ID variables, y is Tau pT and z is Tau eta ");
  ATH_MSG_INFO("Initialization of TrigTauGenericHypoMT completed successfully");

  return StatusCode::SUCCESS;
}


bool TrigTauGenericHypoMT::decide( const ITrigTauGenericHypoTool::ClusterInfo& input ) const {

  bool pass = false;


  auto PassedCuts        = Monitored::Scalar<int>( "CutCounter", -1 );
  auto monitorIt         = Monitored::Group( m_monTool, PassedCuts );

  // when leaving scope it will ship data to monTool
  PassedCuts = PassedCuts + 1; //got called (data in place)

  if ( m_acceptAll ) {
    pass = true;
    ATH_MSG_DEBUG( "AcceptAll property is set: taking all events" );
  } else {
    pass = false;
    ATH_MSG_DEBUG( "AcceptAll property not set: applying selection" );
  }

  // get tau collection
  auto pTauCont = input.taucontainer;

  if(pTauCont->size()!=0){
    ATH_MSG_DEBUG(" Input tau collection has size " << pTauCont->size());
  }else {
	 ATH_MSG_DEBUG("No taus in input collection: Rejecting");
   pass=false;
	return pass;
  }

  const xAOD::TauJet* aTau = pTauCont->back();
  
  int hasFailed = 0;

  // We have to implement the cuts here - Eventually, declare the formulas in the initialize
  for(unsigned int i=0; i<m_member.size(); i++)
    {
      float theValue = 0;
      
      // Detail -1 ignores by convention the ID variables
      if(m_member.at(i) != -1)
	{
	  xAOD::TauJetParameters::Detail myDetail = xAOD::TauJetParameters::Detail(m_member.at(i));
	  aTau->detail(myDetail, theValue);
	}
      
      // What about upper and lower bounds? can work using x, y, z, t...
      double theResult = m_store.at(i).Eval(theValue, aTau->pt(), aTau->eta());
      ATH_MSG_DEBUG(" Evaluating Hypothesis on ID Variable #: " << m_member.at(i) );
      ATH_MSG_DEBUG(" With Cut = " << m_formula.at(i).c_str() );
      ATH_MSG_DEBUG(" And value x (ID), y (pT), z (Eta) = " << theValue <<", "<< aTau->pt() << ", " << aTau->eta() );
      ATH_MSG_DEBUG(" Result = " << (theResult > 0.5) );

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
         << " did not pass the threshold");
      return pass;
    }

  pass = true;
  
  ATH_MSG_DEBUG(" REGTEST: TE accepted !! ");
  
  
  return pass;
}



StatusCode TrigTauGenericHypoMT::decide(  std::vector<ClusterInfo>& input )  const {

  for ( auto& i: input ) {
    if ( passed ( m_decisionId.numeric(), i.previousDecisionIDs ) ) {
      if ( decide( i ) ) {
   addDecisionID( m_decisionId, i.decision );
      }
    }
  }
  return StatusCode::SUCCESS;
}

