/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
  Test byte stream converters, write to and read from byte stream ...
  Ketevi A. Assamagan, CERN Jan-28-2003
  Adapted from H. Ma's test example!

  Re-adapted by Michela Biglietti for Mdt Rods
*/

#include "MuonByteStreamCnvTest/ReadMdtDigit.h"
#include "MuonDigitContainer/MdtDigitCollection.h"
#include "MuonDigitContainer/MdtDigitContainer.h"

static const int maxColl =   1200;
static const int maxDig =    5000;

/////////////////////////////////////////////////////////////////////////////

ReadMdtDigit::ReadMdtDigit(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), m_ntuplePtr(0),
  m_activeStore("ActiveStoreSvc", name)
{
  declareProperty("NtupleLocID",m_NtupleLocID);
  declareProperty("WriteMdtNtuple", m_mdtNtuple = false);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadMdtDigit::initialize()
{
  ATH_MSG_DEBUG( " in initialize()"  );
  ATH_CHECK( m_activeStore.retrieve() );
  ATH_CHECK( m_idHelperSvc.retrieve() );
 

  if (!m_mdtNtuple) return StatusCode::SUCCESS;

  ATH_CHECK( accessNtuple() );

  ATH_CHECK( m_ntuplePtr -> addItem ("mdtdig/ncoll",   m_nColl,       0, maxColl) );
  ATH_CHECK( m_ntuplePtr -> addItem ("mdtdig/ndig",    m_nDig,        0, maxDig) );
  ATH_CHECK( m_ntuplePtr -> addItem ("mdtdig/tdc",     m_nDig,        m_tdc) );
  ATH_CHECK( m_ntuplePtr -> addItem ("mdtdig/adc",     m_nDig,        m_adc) );
  ATH_CHECK( m_ntuplePtr -> addItem ("mdtdig/imulti",  m_nDig,   m_multi) );
  ATH_CHECK( m_ntuplePtr -> addItem ("mdtdig/ilayer",  m_nDig,   m_layer) );
  ATH_CHECK( m_ntuplePtr -> addItem ("mdtdig/iwire",   m_nDig,   m_wire) );
  
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadMdtDigit::execute()
{
  ATH_MSG_DEBUG( "in execute()"  );

  std::string	key = "MDT_DIGITS";
  const MdtDigitContainer* mdt_container = nullptr;
  ATH_CHECK( (*m_activeStore)->retrieve(mdt_container, key) );
 
  ATH_MSG_DEBUG("****** mdt->size() : " << mdt_container->size() );

  if (!m_mdtNtuple) return StatusCode::SUCCESS;

  m_nColl = 0;
  m_nDig = 0;
  IdentifierHash ch_id;
  Identifier dig_id;

  // Access by Collection
  for (MdtDigitContainer::const_iterator c = mdt_container->begin(); 
       c != mdt_container->end(); ++c) {

    ch_id = (*c)->identifierHash();

    if ((*c)->size()>0) {
     
      for (MdtDigitCollection::const_iterator dig = (*c)->begin(); 
	   dig != (*c)->end(); ++dig) {

	dig_id = (*dig)->identify();

	m_tdc[m_nDig] = (*dig)->tdc();
	m_adc[m_nDig] = (*dig)->adc();

	m_multi[m_nDig] = m_idHelperSvc->mdtIdHelper().multilayer(dig_id); 
	m_layer[m_nDig] = m_idHelperSvc->mdtIdHelper().tubeLayer(dig_id);
	m_wire[m_nDig]  = m_idHelperSvc->mdtIdHelper().tube(dig_id); 
	++m_nDig;
        ATH_MSG_DEBUG( " Digit number  " << m_nDig );

	if (m_nDig > maxDig-1) {
	  ATH_MSG_WARNING( "Maximum number of MdtDigit in the ntuple reached: " 
                           << maxDig  );
	  return StatusCode::SUCCESS;
	}
	

      }
      ++m_nColl;

      ATH_MSG_DEBUG( " Collection number  " << m_nColl );
      if (m_nColl > maxColl-1) {
	ATH_MSG_WARNING( "Maximum number of MdtDigitCollection in the ntuple reached: " 
                         << maxColl  );
	return StatusCode::SUCCESS;
      }

    }
  }
 
  ATH_MSG_DEBUG( " done collecting histograms"  );
  ATH_MSG_DEBUG( "ReadMdtDigit::execute reports success" );
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadMdtDigit::accessNtuple()
{
  m_NtupleLocID = "/NTUPLES" + m_NtupleLocID ;

  //try to access it  
  NTuplePtr nt(ntupleSvc(), m_NtupleLocID );

  if ((int) nt)     {
     m_ntuplePtr=nt;
     ATH_MSG_INFO( "Ntuple " << m_NtupleLocID << " reaccessed! "  );
  } 
  else {
    ATH_MSG_FATAL( "Cannot reaccess " << m_NtupleLocID  );
     return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


