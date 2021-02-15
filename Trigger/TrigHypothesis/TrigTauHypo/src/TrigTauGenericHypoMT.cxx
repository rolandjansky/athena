/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include "GaudiKernel/StatusCode.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "xAODJet/Jet.h"
#include "xAODTau/TauJet.h"

#include "TrigTauGenericHypoMT.h"
#include "TrigCompositeUtils/HLTIdentifier.h"


TrigTauGenericHypoMT::TrigTauGenericHypoMT( const std::string& type,
                  const std::string& name, 
                  const IInterface* parent ) 
  : base_class( type, name, parent ),
    m_decisionId( HLT::Identifier::fromToolName( name ) ) 
{
  declareProperty("Details", m_member);
  declareProperty("Formulas", m_formula);
}


StatusCode TrigTauGenericHypoMT::initialize() {

  if ( !m_monTool.empty() ) CHECK( m_monTool.retrieve() );

  ATH_MSG_DEBUG("Tool configured for chain/id: " << m_decisionId <<
                ", AcceptAll = " << (m_acceptAll ? "True" : "False"));

  // Here we store the formulas since they need to compile
  m_store.reserve(m_member.size());
  msg(MSG::INFO) << "Cuts: ";
  for(unsigned int i=0; i<m_member.size(); i++)
  {
    m_store.push_back(TFormula(("TauHypoCut"+m_formula.at(i)).c_str(), m_formula.at(i).c_str()));
    msg(MSG::INFO) << "(" << m_formula.at(i) << ") ";
    // x is the ID variables, y is Tau pT and z is Tau eta
  }
  msg(MSG::INFO) << endmsg;

  return StatusCode::SUCCESS;
}


bool TrigTauGenericHypoMT::decide( const ITrigTauGenericHypoTool::ClusterInfo& input ) const {

  bool pass = false;

  auto passedCuts        = Monitored::Scalar<unsigned int>( "CutCounter", 0 );
  auto monitorIt         = Monitored::Group( m_monTool, passedCuts );

  if ( m_acceptAll ) {
    pass = true;
    ATH_MSG_DEBUG( "AcceptAll property is set: taking all events" );
  }

  // get tau collection
  auto pTauCont = input.taucontainer;

  if( not pTauCont->empty() ){
    ATH_MSG_DEBUG("Input tau collection has size " << pTauCont->size());
  }else {
    ATH_MSG_DEBUG("No taus in input collection: Rejecting");
    return false;
  }

  const xAOD::TauJet* aTau = pTauCont->back();
  
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
      ATH_MSG_DEBUG("Evaluating Hypothesis on ID Variable #: " << m_member.at(i) <<
                    " with cut = " << m_formula.at(i).c_str() <<
                    " and value x (ID), y (pT), z (Eta) = " << theValue <<", "<< aTau->pt() <<
                    ", " << aTau->eta() <<
                    ", Result = " << (theResult > 0.5));

      if(theResult < 0.5)
        break;  // cut failed
      else
        ++passedCuts;
    }
  
  if ( passedCuts!=m_member.size() )
    {
      ATH_MSG_DEBUG(" REGTEST: Cut Number: " << passedCuts
                    << " did not pass the threshold");
      return pass;
    }

  pass = true;
  ATH_MSG_DEBUG(" REGTEST: TE accepted !! ");

  return pass;
}



StatusCode TrigTauGenericHypoMT::decide(  std::vector<ClusterInfo>& input )  const {

  using namespace TrigCompositeUtils;

  for ( auto& i: input ) {
    if ( passed ( m_decisionId.numeric(), i.previousDecisionIDs ) ) {
      if ( decide( i ) ) {
        addDecisionID( m_decisionId, i.decision );
      }
    }
  }
  return StatusCode::SUCCESS;
}

