/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Framework include files
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaBaseComps/FilteredAlgorithm.h"
#include <cassert>
#include <string>
#include <vector>

using std::string;
using std::vector;

// Standard Constructor
FilteredAlgorithm::FilteredAlgorithm(const string& name, 
                                     ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_decSvc("DecisionSvc/DecisionSvc",name)
{
  assert( pSvcLocator );
  declareProperty("AcceptAlgs",
                  m_acceptNames,
                  "Filters which if any are passed enable output");
  declareProperty("RequireAlgs",
                  m_requireNames,
                  "Filters which must all be passed to enable output");
  declareProperty("VetoAlgs",
                  m_vetoNames,
                  "Filters which if any are passed disable output");

  declareProperty("decSvc", m_decSvc);

}

// Standard Destructor
FilteredAlgorithm::~FilteredAlgorithm()
{}

// initialize data writer
StatusCode 
FilteredAlgorithm::initialize() 
{
  ATH_MSG_DEBUG ("In initialize ");

  // Decode the accept, required and veto Algorithms. 
  // The logic is the following:
  //  a. The event is accepted if all lists are empty.
  //  b. The event is provisionally accepted if any Algorithm in the 
  //     accept list
  //     has been executed and has indicated that its filter is passed. This
  //     provisional acceptance can be overridden by the other lists.
  //  c. The event is rejected unless all Algorithms in the required list have
  //     been executed and have indicated that their filter passed.
  //  d. The event is rejected if any Algorithm in the veto list has been
  //     executed and has indicated that its filter has passed.

  // Use IDecisionSvc, FilteredAlgorithm is a wrapper around DecisionSvc
  ATH_CHECK(m_decSvc.retrieve());
  ATH_MSG_DEBUG( "Found IDecisionSvc." );

  // Register stream, no matter what Properties it has
  if (!m_decSvc->addStream(this->name()).isSuccess()) {
    ATH_MSG_ERROR("Couldn't add stream " << this->name());
  }

  // Propagate the FilteredAlgorithm's Properties to IDecisionSvc
  for (std::vector<std::string>::const_iterator 
        it = m_acceptNames.value().begin(),
         end= m_acceptNames.value().end();
       it != end;
       ++it) {
    ATH_MSG_DEBUG("Trying to add " << (*it) << " of stream " 
                  << this->name() << " to AcceptAlg list");
    if (!m_decSvc->addAcceptAlg((*it), this->name()).isSuccess()) {
      ATH_MSG_ERROR("Couldn't add acceptAlg");
    }
  }

  for (std::vector<std::string>::const_iterator 
        it = m_requireNames.value().begin(),
        end= m_requireNames.value().end();
      it != end;
      ++it){
    ATH_MSG_DEBUG("Trying to add " << (*it) << " of stream " 
                  << this->name() << " to RequireAlg list");
    if (!m_decSvc->addRequireAlg((*it), this->name()).isSuccess()) {
      ATH_MSG_ERROR("Couldn't add requireAlg");
    }
  }

  for (std::vector<std::string>::const_iterator 
        it = m_vetoNames.value().begin(),
        end= m_vetoNames.value().end();
      it != end;
      ++it){
    ATH_MSG_DEBUG("Trying to add " << (*it) << " of stream " 
                  << this->name() << " to VetoAlg list");
    if (!m_decSvc->addVetoAlg((*it), this->name()).isSuccess()) {
      ATH_MSG_ERROR("Couldn't add vetoAlg");
    }
  }

  ATH_MSG_DEBUG ("End initialize ");
  
  return StatusCode::SUCCESS;
}


StatusCode 
FilteredAlgorithm::finalize() 
{

  return StatusCode::SUCCESS;
}

StatusCode
FilteredAlgorithm::execute() 
{
  return StatusCode::SUCCESS;
}

bool
FilteredAlgorithm::isEventAccepted( ) const
{
  bool result = true;
  result = m_decSvc->isEventAccepted(this->name());
  return result;
}

