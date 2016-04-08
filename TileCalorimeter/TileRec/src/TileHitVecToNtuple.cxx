/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileHitVecToNtuple.cxx
//  Author   : Mikhail
//  Created  : July, 2005
//
//  DESCRIPTION:
//     Implement the algorithm
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

// Gaudi includes
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

//Atlas include
#include "AthenaKernel/errorcheck.h"

// Calo include
#include "CaloIdentifier/TileID.h"

//TileCalo include
#include "TileSimEvent/TileHitVector.h"
#include "TileRec/TileHitVecToNtuple.h"

TileHitVecToNtuple::TileHitVecToNtuple(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_ntuplePtr(0)
  , m_tileID(0)
{
  m_hitVector = "TileHitVec";
  m_ntupleLoc="/FILE1/TileRec";
  m_ntupleID="h32";
  
  declareProperty("TileHitVector", m_hitVector);
  declareProperty("NTupleLoc", m_ntupleLoc);
  declareProperty("NTupleID", m_ntupleID);
  declareProperty("CommitNtuple", m_commitNtuple = true);
  declareProperty("MaxLength", m_maxLength = 4999);
  
}

TileHitVecToNtuple::~TileHitVecToNtuple()
{
}

//****************************************************************************
//* Initialization
//****************************************************************************

StatusCode TileHitVecToNtuple::initialize()
{
  
  ATH_MSG_INFO( "Initialization started" );
  
  // retrieve TileID helper from det store
  
  CHECK( detStore()->retrieve(m_tileID) );
  
  m_ntupleLoc="/NTUPLES" + m_ntupleLoc;
  
  SmartDataPtr<NTuple::Directory> DirPtr(ntupleSvc(), m_ntupleLoc);
  if(!DirPtr) DirPtr=ntupleSvc()->createDirectory(m_ntupleLoc);
  if(!DirPtr) {
    ATH_MSG_ERROR( "Invalid Ntuple Directory: " );
    return StatusCode::FAILURE;
  }

  m_ntuplePtr = ntupleSvc()->book(DirPtr.ptr(), m_ntupleID,
      CLID_ColumnWiseTuple, "TileHit-Ntuple");

  if(!m_ntuplePtr) {
    std::string ntupleCompleteID=m_ntupleLoc+"/"+m_ntupleID;
    NTuplePtr nt(ntupleSvc(),ntupleCompleteID);

    if (!nt) {
      ATH_MSG_ERROR(
          "Failed to book or to retrieve ntuple " << ntupleCompleteID );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO( "Reaccessing ntuple " << ntupleCompleteID );
      m_ntuplePtr = nt;
    }

  }
  
  CHECK( m_ntuplePtr->addItem("TileHit/nhit",m_nchan,0,m_maxLength) );
  CHECK( m_ntuplePtr->addItem("TileHit/totalE",m_tolE) );

  CHECK( m_ntuplePtr->addItem("TileHit/energy",m_nchan,m_energy) );
  CHECK( m_ntuplePtr->addItem("TileHit/time",m_nchan,m_time) );

  CHECK( m_ntuplePtr->addItem("TileHit/detector",m_nchan,m_detector,0,3) );
  CHECK( m_ntuplePtr->addItem("TileHit/side",m_nchan,m_side,-1,1) );
  CHECK( m_ntuplePtr->addItem("TileHit/sample",m_nchan,m_sample,0,3) );
  CHECK( m_ntuplePtr->addItem("TileHit/pmt", m_nchan, m_pmt,0,1) );
  CHECK( m_ntuplePtr->addItem("TileHit/eta",m_nchan,m_eta,0,15) );
  CHECK( m_ntuplePtr->addItem("TileHit/phi",m_nchan,m_phi,0,63) );

  ATH_MSG_INFO( "Initialization completed" );
  return StatusCode::SUCCESS;
}

//****************************************************************************
//* Execution
//****************************************************************************

StatusCode TileHitVecToNtuple::execute()
{
  
  // step1: read TileHits from TDS
  const TileHitVector * inputHits;
  CHECK( evtStore()->retrieve(inputHits, m_hitVector) );
  
  // step2: put items in ntuple
  TileHitVecConstIterator it = inputHits->begin();
  TileHitVecConstIterator end = inputHits->end();
  
  m_nchan = 0;
  m_tolE = 0.0;
  int n_hit = 0;
  for (; it != end; ++it) {

    const TileHit * cinp = &(*it);
    Identifier id = cinp->identify();

    int size = cinp->size();
    for (int i = 0; i < size; ++i) {
      m_tolE += cinp->energy(i);
      m_energy[m_nchan] = cinp->energy(i);
      m_time[m_nchan] = cinp->time(i);

      m_detector[m_nchan] = m_tileID->section(id);
      m_side[m_nchan] = m_tileID->side(id);
      m_sample[m_nchan] = m_tileID->sample(id);
      m_pmt[m_nchan] = m_tileID->pmt(id);
      m_eta[m_nchan] = m_tileID->tower(id);
      m_phi[m_nchan] = m_tileID->module(id);

      m_nchan++;
      if (m_nchan >= m_maxLength)
        break;
    }

    if (msgLvl(MSG::VERBOSE)) {
      msg(MSG::VERBOSE) << " iHit=" << n_hit << " id="
          << m_tileID->to_string(id, -1) << " ener=";

      for (int i = 0; i < size; ++i)
        msg(MSG::VERBOSE) << cinp->energy(i) << " ";

      msg(MSG::VERBOSE) << "time=";
      for (int i = 0; i < size; ++i)
        msg(MSG::VERBOSE) << cinp->time(i) << " ";

      msg(MSG::VERBOSE)  << endmsg;
    }

    ++n_hit;
    if (m_nchan >= m_maxLength) {
      ATH_MSG_DEBUG( "Number of hits exceeds maximum (" << m_maxLength
          << "), ignore all the rest" );
      break;
    }
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

StatusCode TileHitVecToNtuple::finalize()
{
  ATH_MSG_INFO( "finalize() completed successfully" );
  return StatusCode::SUCCESS;
}
