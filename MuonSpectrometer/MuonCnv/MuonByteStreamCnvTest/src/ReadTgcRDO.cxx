/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*
  Test byte stream converters, write to and read from byte stream ...

  @author Tadashi Maeno
     based on ReadRpcRDO by Ketevi A. Assamagan
*/

#include <cassert>

#include "MuonRDO/TgcRawData.h"
#include "MuonRDO/TgcRdo.h"
#include "MuonRDO/TgcRdoContainer.h"

#include "MuonByteStreamCnvTest/ReadTgcRDO.h"

/////////////////////////////////////////////////////////////////////////////

const int MAX_RDO = 24;
const int MAX_DATA = 1024;

ReadTgcRDO::ReadTgcRDO(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_ntuplePtr(nullptr), 
  m_hashFunc(nullptr)
{
  // Declare the properties
  declareProperty("NtupleLocID",m_NtupleLocID);
  declareProperty("WriteTgcNtuple", m_tgcNtuple = false);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadTgcRDO::initialize()
{
  ATH_MSG_DEBUG( " in initialize()"  );

  if (!m_tgcNtuple) return StatusCode::SUCCESS;

  ATH_CHECK( accessNtuple() );

  //  Add the following items to the ntuple:

  ATH_CHECK( m_ntuplePtr -> addItem ("tgcrod/nrdo",   m_nRdo,     0, MAX_RDO) );
  ATH_CHECK( m_ntuplePtr -> addItem ("tgcrod/ndata",  m_nRawData, 0, MAX_DATA) );
  ATH_CHECK( m_ntuplePtr -> addItem ("tgcrod/l1Id",   m_nRdo,        m_l1Id) );
  ATH_CHECK( m_ntuplePtr -> addItem ("tgcrod/bcId",   m_nRdo,        m_bcId) );
  ATH_CHECK( m_ntuplePtr -> addItem ("tgcrod/bcTag",  m_nRawData,    m_bcTag) );
  ATH_CHECK( m_ntuplePtr -> addItem ("tgcrod/l1IdS",  m_nRawData,    m_l1IdSLB) );
  ATH_CHECK( m_ntuplePtr -> addItem ("tgcrod/bcIdS",  m_nRawData,    m_bcIdSLB) );
  ATH_CHECK( m_ntuplePtr -> addItem ("tgcrod/subID",  m_nRawData,    m_subId) );
  ATH_CHECK( m_ntuplePtr -> addItem ("tgcrod/rodID",  m_nRawData,    m_rodId) );
  ATH_CHECK( m_ntuplePtr -> addItem ("tgcrod/sswID",  m_nRawData,    m_sswId) );
  ATH_CHECK( m_ntuplePtr -> addItem ("tgcrod/slbID",  m_nRawData,    m_slbId) );
  ATH_CHECK( m_ntuplePtr -> addItem ("tgcrod/bitPos", m_nRawData,    m_bitPos) );
  ATH_CHECK( m_ntuplePtr -> addItem ("tgcrod/isCoincidence",m_nRawData,m_isCoincidence) );
  ATH_CHECK( m_ntuplePtr -> addItem ("tgcrod/type",m_nRawData,m_type) );
  ATH_CHECK( m_ntuplePtr -> addItem ("tgcrod/index",m_nRawData,m_index) );
  ATH_CHECK( m_ntuplePtr -> addItem ("tgcrod/pos",m_nRawData,m_pos) );
  ATH_CHECK( m_ntuplePtr -> addItem ("tgcrod/delta",m_nRawData,m_delta) );

  m_hashFunc = new TgcRdoIdHash();
   
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadTgcRDO::execute()
{
  ATH_MSG_DEBUG( "in execute()"  );

  SG::ReadHandle<TgcRdoContainer> hndl("TGCRDO");
  const TgcRdoContainer* TgcRDO = hndl.get();
  ATH_CHECK(TgcRDO != nullptr);

  ATH_MSG_DEBUG("****** TgcRDO->size() : " << TgcRDO->size() );

  if (!m_tgcNtuple) return StatusCode::SUCCESS;

 // Access by Collection 

 m_nRdo = 0;
 m_nRawData = 0;

 // loop over Rdo
 TgcRdoContainer::const_iterator itR   = TgcRDO->begin(); 
 TgcRdoContainer::const_iterator itR_e = TgcRDO->end();
 for (; itR!=itR_e; ++itR)
   {
     const TgcRdo * rdo = *itR;
     assert (m_nRdo < MAX_RDO);

     ATH_MSG_DEBUG( " Number of RawData in this rdo "  << rdo->size()  );

     if ((*itR)->size() == 0) continue;

     //m_l1Id  [m_nRdo] = (long) rdo->l1Id();
     //m_bcId  [m_nRdo] = (long) rdo->bcId();

     // for each Rdo, loop over RawData
     TgcRdo::const_iterator itD   = rdo->begin(); 
     TgcRdo::const_iterator itD_e = rdo->end();
     for (; itD!=itD_e; ++itD)
       { 
	 const TgcRawData * data = (*itD);
	 assert (m_nRawData < MAX_DATA);
	 
	 m_bcTag  [m_nRawData] = (long) data->bcTag();
	 //m_l1IdSLB[m_nRawData] = (long) data->l1Id();
	 //m_bcIdSLB[m_nRawData] = (long) data->bcId();
	 m_subId  [m_nRawData] = (long) data->subDetectorId();
	 m_rodId  [m_nRawData] = (long) data->rodId();
	 m_sswId  [m_nRawData] = (long) data->sswId();
	 m_slbId  [m_nRawData] = (long) data->slbId();
	 m_bitPos [m_nRawData] = (long) data->bitpos();
	 if (data->isCoincidence()) {
	   m_isCoincidence[m_nRawData] = 1;
	   m_type [m_nRawData] = (long) data->type();
	   m_index[m_nRawData] = (long) data->index();
	   m_pos  [m_nRawData] = (long) data->position();
	   m_delta[m_nRawData] = (long) data->delta();
	 }
	 else {
	   m_isCoincidence[m_nRawData] = 0;
	   m_type [m_nRawData] = 0;
	   m_index[m_nRawData] = 0;
	   m_pos  [m_nRawData] = 0;
	   m_delta[m_nRawData] = 0;
	 }

	 ATH_MSG_DEBUG( MSG::hex
                        << " Sub : " << data->subDetectorId()
                        << " ROD : " << data->rodId() 
                        << " SSW : " << data->sswId()
                        << " SLB : " << data->slbId()
                        << " Ch  : " << data->bitpos() );

	 ++m_nRawData;
       }
     ++m_nRdo;
   }
 
 ATH_MSG_DEBUG( " done collecting histograms"  );
 ATH_MSG_DEBUG( "ReadTgcRDO::execute reports success"  );

 return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode ReadTgcRDO::finalize()
{
  ATH_MSG_INFO( "in finalize()"  );
  if (m_hashFunc) delete m_hashFunc;
  return StatusCode::SUCCESS;
}


StatusCode ReadTgcRDO::accessNtuple()
{
  m_NtupleLocID = "/NTUPLES" + m_NtupleLocID;

  //try to access it  
  NTuplePtr nt(ntupleSvc(), m_NtupleLocID );

  if ((int) nt)
    {
      m_ntuplePtr=nt;
      ATH_MSG_INFO( "Ntuple " << m_NtupleLocID 
                    << " reaccessed! "  );
    }
  else
    {
      ATH_MSG_FATAL( "Cannot reaccess " << m_NtupleLocID  );
      return StatusCode::FAILURE;
    }
  
  return StatusCode::SUCCESS;
}


