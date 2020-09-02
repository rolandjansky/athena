/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**

Fill an ntuple with MdtPrepData

S. Rosati Dec 2005

*/

#include "MuonByteStreamCnvTest/ReadMdtPRD.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"

static const int maxColl =   1200;
static const int maxPrd =    5000;

/////////////////////////////////////////////////////////////////////////////

ReadMdtPRD::ReadMdtPRD(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), m_ntuplePtr(0)
{
  // Declare the properties

  declareProperty("NtupleLocID",m_NtupleLocID);
  declareProperty("WriteMdtNtuple", m_mdtNtuple = true);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadMdtPRD::initialize()
{
  ATH_MSG_DEBUG( " in initialize()"  );
  ATH_CHECK( m_idHelperSvc.retrieve() );

  if (!m_mdtNtuple) return StatusCode::SUCCESS;

  ATH_CHECK( accessNtuple() );

  ATH_CHECK( m_ntuplePtr -> addItem ("mdtprd/nprdcoll",   m_nColl,       0, maxColl) );
  ATH_CHECK( m_ntuplePtr -> addItem ("mdtprd/nprd",    m_nPrd,        0, maxPrd) );
  ATH_CHECK( m_ntuplePtr -> addItem ("mdtprd/tdc",     m_nPrd,        m_tdc) );
  ATH_CHECK( m_ntuplePtr -> addItem ("mdtprd/adc",     m_nPrd,        m_adc) );
  ATH_CHECK( m_ntuplePtr -> addItem ("mdtprd/imulti",  m_nPrd,   m_multi) );
  ATH_CHECK( m_ntuplePtr -> addItem ("mdtprd/ilayer",  m_nPrd,   m_layer) );
  ATH_CHECK( m_ntuplePtr -> addItem ("mdtprd/iwire",   m_nPrd,   m_wire) );
  
  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadMdtPRD::execute ATLAS_NOT_THREAD_SAFE ()
{
  ATH_MSG_DEBUG( "in execute()"  );
  
  std::string	key = "MDT_PREPDATA_NEW";
  const Muon::MdtPrepDataContainer* mdt_container;
  ATH_CHECK( evtStore()->retrieve(mdt_container, key) );
 
  ATH_MSG_DEBUG("****** mdt->size() : " << mdt_container->size() );

  const DataHandle<Muon::MdtPrepDataCollection> mdtCollection;
  const DataHandle<Muon::MdtPrepDataCollection> lastColl;

  if (!m_mdtNtuple) return StatusCode::SUCCESS;

  m_nColl = 0;
  m_nPrd = 0;
  //Identifier ch_id;
  Identifier dig_id;

  // Access by Collection
  if (evtStore()->retrieve(mdtCollection,lastColl) ==StatusCode::SUCCESS) {

    for ( ; mdtCollection != lastColl ; ++mdtCollection ) {
      
      //ch_id = mdtCollection->identify();
      
      if (mdtCollection->size()>0) {
	
	for (Muon::MdtPrepDataCollection::const_iterator mdtPrd = mdtCollection->begin(); 
	     mdtPrd != mdtCollection->end(); ++mdtPrd) {
	  
	  dig_id = (*mdtPrd)->identify();
	  
	  m_tdc[m_nPrd] = (*mdtPrd)->tdc();
	  m_adc[m_nPrd] = (*mdtPrd)->adc();
	  
	  m_multi[m_nPrd] = m_idHelperSvc->mdtIdHelper().multilayer(dig_id); 
	  m_layer[m_nPrd] = m_idHelperSvc->mdtIdHelper().tubeLayer(dig_id);
	  m_wire[m_nPrd]  = m_idHelperSvc->mdtIdHelper().tube(dig_id); 
	  ++m_nPrd;
	  ATH_MSG_DEBUG( " MdtPrepData number:  " << m_nPrd );
	  
	  if (m_nPrd > maxPrd-1) {
	    ATH_MSG_WARNING( "Maximum number of MdtPrepData in the ntuple reached: " 
                             << maxPrd  );
	    return StatusCode::SUCCESS;
	  }
	  
	  
	}
	++m_nColl;
	
        ATH_MSG_DEBUG( " Collection number  " << m_nColl );
	if (m_nColl > maxColl-1) {
	  ATH_MSG_WARNING( "Maximum number of MdtPrepDataCollection in the ntuple reached: " 
                           << maxColl  );
	  return StatusCode::SUCCESS;
	}
	
      }
    }
  }
 
  ATH_MSG_DEBUG( " done collecting histograms"  );
  ATH_MSG_DEBUG( "ReadMdtPRD::execute reports success" );
 return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadMdtPRD::accessNtuple() {

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


