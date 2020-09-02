/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStreamCnvTest/ReadCscDigit.h"
#include "MuonDigitContainer/CscDigitCollection.h"
#include "MuonDigitContainer/CscDigitContainer.h"

const int MAXCOLL = 32;
const int MAXDIGIT = 4096;

/////////////////////////////////////////////////////////////////////////////

ReadCscDigit::ReadCscDigit(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator), m_ntuplePtr(0),
    m_activeStore("ActiveStoreSvc", name)
{
  // Declare the properties
  declareProperty("NtupleLocID",m_NtupleLocID);
  declareProperty("WriteCscNtuple", m_cscNtuple = false);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadCscDigit::initialize()
{
  ATH_MSG_DEBUG( " in initialize()"  );
  ATH_CHECK( m_activeStore.retrieve() );
  ATH_CHECK( m_idHelperSvc.retrieve() );

  if (!m_cscNtuple) return StatusCode::SUCCESS;

  ATH_CHECK( accessNtuple() );

  // add items
  ATH_CHECK( m_ntuplePtr -> addItem ("cscdig/ncollections",   m_nColl, 0, MAXCOLL) );
  ATH_CHECK( m_ntuplePtr -> addItem ("cscdig/ndigits",        m_nDig,  0, MAXDIGIT) );
  ATH_CHECK( m_ntuplePtr -> addItem ("cscdig/stationName", m_nDig,  m_stationName) );
  ATH_CHECK( m_ntuplePtr -> addItem ("cscdig/stationEta",  m_nDig,  m_stationEta) );
  ATH_CHECK( m_ntuplePtr -> addItem ("cscdig/stationPhi",  m_nDig,  m_stationPhi) );
  ATH_CHECK( m_ntuplePtr -> addItem ("cscdig/chamberLayer",m_nDig,  m_chamberLayer) );
  ATH_CHECK( m_ntuplePtr -> addItem ("cscdig/wireLayer",   m_nDig,  m_wireLayer) );
  ATH_CHECK( m_ntuplePtr -> addItem ("cscdig/measuresPhi", m_nDig,  m_measuresPhi) );
  ATH_CHECK( m_ntuplePtr -> addItem ("cscdig/strip",       m_nDig,  m_strip) );
  ATH_CHECK( m_ntuplePtr -> addItem ("cscdig/charge",      m_nDig,  m_charge) );
  
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadCscDigit::execute()
{
  ATH_MSG_DEBUG( "in execute()"  );

  // get CSC digit container
  const std::string key = "CSC_DIGITS";
  const CscDigitContainer* csc_container = nullptr;
  ATH_CHECK( (*m_activeStore)->retrieve(csc_container, key) );
 
  ATH_MSG_DEBUG("****** csc->size() : " << csc_container->size() );

  if (!m_cscNtuple) return StatusCode::SUCCESS;

  m_nColl = 0;
  m_nDig = 0;

  // loop over collections
  for (CscDigitContainer::const_iterator c = csc_container->begin(); 
       c != csc_container->end(); ++c)
    {
      if ( (*c)->size() > 0 ) {
        ATH_MSG_DEBUG( "*******************************************"  );
        ATH_MSG_DEBUG( "Collection hash id = " << (unsigned int) (*c)->identifierHash() );
        // loop over digits
        for (CscDigitCollection::const_iterator dig = (*c)->begin(); 
	     dig != (*c)->end(); ++dig) 
	  {
            ATH_MSG_DEBUG( "Digit number " << m_nDig  );
	    Identifier id = (*dig)->identify();

	    // ID information
	    m_stationName[m_nDig]  = m_idHelperSvc->cscIdHelper().stationName(id);
	    m_stationEta [m_nDig]  = m_idHelperSvc->cscIdHelper().stationEta(id);
	    m_stationPhi [m_nDig]  = m_idHelperSvc->cscIdHelper().stationPhi(id);
	    m_chamberLayer[m_nDig] = m_idHelperSvc->cscIdHelper().chamberLayer(id);
	    m_wireLayer[m_nDig]    = m_idHelperSvc->cscIdHelper().wireLayer(id);
	    m_measuresPhi[m_nDig]  = m_idHelperSvc->cscIdHelper().measuresPhi(id);
	    m_strip[m_nDig]        = m_idHelperSvc->cscIdHelper().strip(id); 
	    m_charge[m_nDig]       = (*dig)->charge(); 
            ATH_MSG_DEBUG( "Digit = " << m_idHelperSvc->cscIdHelper().show_to_string(id) << " charge = " 
                           << m_charge[m_nDig]  );
	    ++m_nDig;
	  }
        ++m_nColl;
      }
  }
  ATH_MSG_DEBUG( "Number of collections = " << m_nColl  );
  ATH_MSG_DEBUG( "Number of digits = " << m_nDig  );
  ATH_MSG_DEBUG( "execute() completed" );
 return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadCscDigit::finalize()
{
  ATH_MSG_DEBUG( "in finalize()"  );
  return StatusCode::SUCCESS;
}


StatusCode ReadCscDigit::accessNtuple()
{
  m_NtupleLocID = "/NTUPLES" + m_NtupleLocID ;

  //try to access it  
  NTuplePtr nt(ntupleSvc(), m_NtupleLocID );

  if (static_cast<int>(nt))
    {
      m_ntuplePtr=nt;
      ATH_MSG_INFO( "Ntuple " << m_NtupleLocID  << " reaccessed! "  );
    } 
  else
    {
      ATH_MSG_FATAL( "Cannot reaccess " << m_NtupleLocID  );
      return StatusCode::FAILURE;
    }

  return StatusCode::SUCCESS;
}


