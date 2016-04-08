/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileTTL1ToNtuple.cxx
//  Author   : Gia
//  Created  : March, 2003
//
//  DESCRIPTION:
//     Implement the algorithm
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

//Gaudi includes
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

//Atlas include
#include "AthenaKernel/errorcheck.h"

// Calo include
#include "CaloIdentifier/CaloLVL1_ID.h" 
#include "CaloIdentifier/TileTBID.h"

//TileCalo include
#include "TileEvent/TileTTL1Container.h"  
#include "TileRec/TileTTL1ToNtuple.h"

TileTTL1ToNtuple::TileTTL1ToNtuple(const std::string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_ntuplePtr(0)
  , m_TT_ID(0)
  , m_tileTBID(0)
{
  m_ttl1Container="TileTTL1Cnt";
  m_ntupleLoc="/FILE1/TileRec";
  m_ntupleID="h130";

  declareProperty("TileTTL1Container", m_ttl1Container);    
  declareProperty("NTupleLoc", m_ntupleLoc);
  declareProperty("NTupleID", m_ntupleID);
  declareProperty("CommitNtuple", m_commitNtuple = true);
  declareProperty("MaxLength", m_maxLength = 2048);
  declareProperty("NSamples", m_nSamples = 7);

}

TileTTL1ToNtuple::~TileTTL1ToNtuple()
{
}

//****************************************************************************
//* Initialization
//****************************************************************************

StatusCode TileTTL1ToNtuple::initialize() 
{ 
   
  ATH_MSG_INFO( "Initialization started" );

  // retrieve TileID helper from det store

  CHECK( detStore()->retrieve(m_TT_ID) );
  CHECK( detStore()->retrieve(m_tileTBID) );

  m_ntupleLoc="/NTUPLES" + m_ntupleLoc;

  SmartDataPtr<NTuple::Directory> DirPtr(ntupleSvc(), m_ntupleLoc);
  if(!DirPtr) DirPtr=ntupleSvc()->createDirectory(m_ntupleLoc);
  if(!DirPtr) {
    ATH_MSG_ERROR( "Invalid Ntuple Directory: " );
    return StatusCode::FAILURE;
  }
  m_ntuplePtr=ntupleSvc()->book(DirPtr.ptr(), m_ntupleID, 
                                    CLID_ColumnWiseTuple, "TileTTL1-Ntuple");
  if(!m_ntuplePtr) {
    
    std::string ntupleCompleteID=m_ntupleLoc+"/"+m_ntupleID;

    NTuplePtr nt(ntupleSvc(),ntupleCompleteID);
    if (!nt) {
      ATH_MSG_ERROR( "Failed to book or to retrieve ntuple "
         << ntupleCompleteID );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO( "Reaccessing ntuple " << ntupleCompleteID );
      m_ntuplePtr = nt;
    }
  }

  CHECK( m_ntuplePtr->addItem("TileTTL1/nttl1",m_nchan,0,m_maxLength) );
  CHECK( m_ntuplePtr->addItem("TileTTL1/side",m_nchan,m_side,-1,1) );
  CHECK( m_ntuplePtr->addItem("TileTTL1/eta",m_nchan,m_eta,0,15) );
  CHECK( m_ntuplePtr->addItem("TileTTL1/phi",m_nchan,m_phi,0,63) );
  CHECK( m_ntuplePtr->addItem("TileTTL1/samples",m_maxLength,m_nSamples,m_samples) );

  ATH_MSG_INFO( "Initialization completed" );
  return StatusCode::SUCCESS;
}

//****************************************************************************
//* Execution
//****************************************************************************

StatusCode TileTTL1ToNtuple::execute()
{

  // step1: read TileTTL1s from TDS
  const TileTTL1Container* TTL1Cnt; 
  CHECK( evtStore()->retrieve(TTL1Cnt, m_ttl1Container) );

  // step2: put items in ntuple
  TileTTL1Container::const_iterator it=TTL1Cnt->begin();
  TileTTL1Container::const_iterator end=TTL1Cnt->end();
  
  m_nchan=0;
  for(; it != end; ++it) {

    if (m_nchan >= m_maxLength) {
      ATH_MSG_DEBUG( "Number of ttl1s exceeds maximum ("
          << m_maxLength << "), ignore all the rest");
      break;
    }

    const TileTTL1 * cinp = (*it);
    Identifier id=cinp->TTL1_ID();

    if (m_tileTBID->is_tiletb(id)) {
      m_side[m_nchan]=m_tileTBID->type(id);
      m_eta[m_nchan]=m_tileTBID->channel(id);
      m_phi[m_nchan]=m_tileTBID->module(id);
    } else {
      m_side[m_nchan]=m_TT_ID->pos_neg_z(id);
      m_eta[m_nchan]=m_TT_ID->eta(id);
      m_phi[m_nchan]=m_TT_ID->phi(id);
    }
    
    std::vector<float> samples = cinp->fsamples();
    samples.resize(m_nSamples);
    for(int i=0;i<m_nSamples;++i)
      m_samples[m_nchan][i] = samples[i];

    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << " ichan=" << static_cast<int>(m_nchan) << " "
          << m_side[m_nchan] << " /" << m_eta[m_nchan] << " /"
          << m_phi[m_nchan] << " samples=";

      for (int i = 0; i < m_nSamples; ++i)
        msg(MSG::VERBOSE)  << samples[i] << " ";

      msg(MSG::VERBOSE)  << endmsg;
    }
    m_nchan++;
  }

  // step3: commit ntuple
  if ( m_commitNtuple ) {
    ATH_MSG_DEBUG( "Committing Ntuple" );
    CHECK( ntupleSvc()->writeRecord(m_ntuplePtr) );
  }

  // Execution completed.
  ATH_MSG_DEBUG( "execute() completed successfully" );
  return StatusCode::SUCCESS;
}

//****************************************************************************
//* Finalize
//****************************************************************************

StatusCode TileTTL1ToNtuple::finalize()
{
  ATH_MSG_INFO( "finalize() completed successfully" );
  return StatusCode::SUCCESS;
}

