/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
  Test byte stream converters, write to and read from byte stream ...
  Ketevi A. Assamagan, CERN Jan-28-2003
  Adapted from H. Ma's test example!

  Re-adapted by Michela Biglietti for Mdt Rods
*/

//#include <strstream>
//#include <cassert>

#include "MuonRDO/MdtCsm.h"
#include "MuonRDO/MdtCsmContainer.h"

#include "MuonByteStreamCnvTest/ReadMdtRDO.h"

static const int maxCsm =   1200;//?????
static const int maxAmt =   5000;//?????

/////////////////////////////////////////////////////////////////////////////

ReadMdtRDO::ReadMdtRDO(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), m_ntuplePtr(0),
  m_activeStore("ActiveStoreSvc", name)
{
  declareProperty("NtupleLocID",m_NtupleLocID);
  declareProperty("WriteMdtNtuple",m_mdtNtuple = false);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadMdtRDO::initialize()
{
  ATH_MSG_DEBUG( " in initialize()"  );
  ATH_CHECK( m_activeStore.retrieve() );

  if (!m_mdtNtuple) return StatusCode::SUCCESS;

  ATH_CHECK( accessNtuple() );

  ATH_CHECK( m_ntuplePtr -> addItem ("mdtrod/ncsm",   m_nCsm,          0, maxCsm) );
  ATH_CHECK( m_ntuplePtr -> addItem ("mdtrod/namt",   m_nAmt,           0, maxAmt) );
  ATH_CHECK( m_ntuplePtr -> addItem ("mdtrod/subid",  m_nCsm,          m_subId) );
  ATH_CHECK( m_ntuplePtr -> addItem ("mdtrod/rodid",  m_nCsm,          m_rodId) );
  ATH_CHECK( m_ntuplePtr -> addItem ("mdtrod/csmid",  m_nCsm,         m_csmId) );
  ATH_CHECK( m_ntuplePtr -> addItem ("mdtrod/tdcid",  m_nAmt,          m_tdcId) );
  ATH_CHECK( m_ntuplePtr -> addItem ("mdtrod/chanid",  m_nAmt,          m_channelId) );
  ATH_CHECK( m_ntuplePtr -> addItem ("mdtrod/fine",    m_nAmt,          m_fine) );
  ATH_CHECK( m_ntuplePtr -> addItem ("mdtrod/coarse",  m_nAmt,          m_coarse) );
  ATH_CHECK( m_ntuplePtr -> addItem ("mdtrod/width",  m_nAmt,          m_width) );

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadMdtRDO::execute()
{
  ATH_MSG_DEBUG( "in execute()"  );

  const MdtCsmContainer* MdtRDO = nullptr; 
  ATH_CHECK( (*m_activeStore)->retrieve( MdtRDO, "MDTCSM" ) );

 // Access by Collection 

  ATH_MSG_DEBUG("****** MdtRDO->size() : " << MdtRDO->size() );

 if (!m_mdtNtuple) return StatusCode::SUCCESS;

 m_nCsm = 0;
 m_nAmt = 0;

 MdtCsmContainer::const_iterator it1 = MdtRDO->begin(); 
 MdtCsmContainer::const_iterator it2 = MdtRDO->end();

 for (; it1!=it2; ++it1){

   // loop over pads
   if ( (*it1)->size() > 0 ) {

     const MdtCsm* csm = (*it1);

     m_subId[m_nCsm]    = (long) csm->SubDetId();
     m_csmId[m_nCsm]    = (long) csm->CsmId();
     m_rodId[m_nCsm]    = (long) csm->MrodId();
     
     ATH_MSG_DEBUG( "ncsm : " << m_nCsm << " Subdet : " << csm->SubDetId()
                    << " Mrodid : " << csm->MrodId()
                    << " Csmid : " <<  csm->CsmId()  );
     ATH_MSG_DEBUG( "Subdet ntuple: " << m_subId[m_nCsm] 
                    << " Mrodid ntuple: " << m_rodId[m_nCsm]
                    << " Csmid ntuple: " << m_csmId[m_nCsm]  );
     ATH_MSG_DEBUG( " Number of Amt in this Csm " << (*it1)->size()  );
 
     // for each cms, loop over amt
     MdtCsm::const_iterator it3 = (*it1)->begin(); 
     MdtCsm::const_iterator it4 = (*it1)->end();
     for (; it3!=it4; ++it3) { 
       const MdtAmtHit * amt = (*it3);
       m_tdcId[m_nAmt] = (long)amt->tdcId();
       m_channelId[m_nAmt] = (long)amt->channelId();
       m_fine[m_nAmt] = (long)amt->fine();
       m_coarse[m_nAmt] = (long)amt->coarse();
       m_width[m_nAmt]= (long)amt->width();
       m_nAmt++;

       if (m_nAmt > maxAmt-1) {
	 ATH_MSG_WARNING( "Maximum number of AmtHit in the ntuple reached: " 
                          << maxAmt  );
	 return StatusCode::SUCCESS;
       }

     }
     m_nCsm++;
     if ( m_nCsm > maxCsm-1 ) {
       ATH_MSG_WARNING( "Maximum number of MdtCsm in the ntuple reached: " 
                        << maxCsm  );
       return StatusCode::SUCCESS;
     }
   }
 }
 ATH_MSG_DEBUG( " done collecting histograms"  );
 
 ATH_MSG_DEBUG( "ReadMdtRDO::execute reports success" );
 return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadMdtRDO::finalize()
{
  ATH_MSG_INFO( "in finalize()"  );
  return StatusCode::SUCCESS;
}


StatusCode ReadMdtRDO::accessNtuple() {

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


