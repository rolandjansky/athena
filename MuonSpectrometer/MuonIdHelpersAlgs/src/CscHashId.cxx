/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"

#include "MuonIdHelpers/CscIdHelper.h"

#include "MuonIdHelpersAlgs/CscHashId.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include <algorithm>
#include <cmath>

using namespace std;


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

CscHashId::CscHashId(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_cscId(NULL), m_muon_mgr(NULL)
   {

}

CscHashId::~CscHashId()
{}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode CscHashId::initialize(){

  ATH_MSG_DEBUG( " in initialize()"  );
            
  ATH_CHECK( detStore()->retrieve( m_muon_mgr ) );
  m_cscId = m_muon_mgr->cscIdHelper();

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 


StatusCode CscHashId::execute() {

  ATH_MSG_DEBUG( "in execute()"  );
  ATH_CHECK(  cscHash() );
  return StatusCode::SUCCESS;
}

StatusCode CscHashId::finalize() {
 
  ATH_MSG_DEBUG( "in finalize()"  );
  return StatusCode::SUCCESS;
}

StatusCode CscHashId::cscHash() {
  ATH_MSG_DEBUG( "in execute(): testing CSC IdHelper"  );

  // Loop over csc detector elements and add in the hash ids
  std::vector<Identifier> modules = m_cscId->idVector();

  typedef std::vector<Identifier>::const_iterator csc_id;

  csc_id firstId = modules.begin();
  csc_id lastId  = modules.end();

  IdContext moduleContext  =  m_cscId->module_context();
  IdContext channelContext =  m_cscId->channel_context();
  IdentifierHash moduleHashId;
  IdentifierHash channelHashId;

  for(; firstId != lastId; ++firstId) {
    Identifier id = *firstId;
    if (m_cscId->validElement(id)) {
      if(!m_cscId->get_hash(id, moduleHashId, &moduleContext)) {
	std::cout << "The CSC Chamber hash id is " << moduleHashId
	    << " for " << m_cscId->show_to_string(id,&moduleContext) << std::endl;
        std::cout << "*****The strips in this Chamber ********************" << std::endl;
        std::vector<Identifier> vect;
        m_cscId->idChannels(id, vect);
        for (unsigned int i=0; i<vect.size(); ++i) {
           if(!m_cscId->get_hash(vect[i], channelHashId, &channelContext)) {
	      std::cout << "The CSC strip hash id is " << channelHashId
	          << " for " << m_cscId->show_to_string(vect[i],&channelContext) << std::endl;
           }
        } 
      }
      else {
	ATH_MSG_ERROR( "Unable to set csc hash id for det elem " 
                       << "context begin_index = " << moduleContext.begin_index()
                       << " context end_index  = " << moduleContext.end_index()
                       << " the identifier is " );
	id.show();
      }
    } else {
      ATH_MSG_ERROR( "Invalid CSC id "  );
      id.show();
    }
  }  
  
 return StatusCode::SUCCESS;

}

