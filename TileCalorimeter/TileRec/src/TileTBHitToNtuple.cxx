/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileTBHitToNtuple.cxx
//  Author   : Anna
//  Created  : June, 2004
//
//  DESCRIPTION:
//     Implement the algorithm
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

//Gaudi Includes
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

//Atlas include
#include "AthenaKernel/errorcheck.h"
#include "EventContainers/SelectAllObject.h"


//TileCalo include
#include "CaloIdentifier/TileTBID.h"
#include "TileEvent/TileHitContainer.h"  
#include "TileRec/TileTBHitToNtuple.h"
#include "TileSimEvent/TileHitVector.h"

const int max_chan=99999;

// Constructor & deconstructor
TileTBHitToNtuple::TileTBHitToNtuple(std::string name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_ntuplePtr(0)
  , m_tileTBID(0)
{
  m_hitContainer="TileTBHits"; //name of hit vector for ancillary detectors
  m_ntupleLoc="/FILE1/TileRec";
  m_ntupleID="h31";

  declareProperty("TileHitContainer", m_hitContainer);    
  declareProperty("NTupleLoc", m_ntupleLoc);
  declareProperty("NTupleID", m_ntupleID);
}

TileTBHitToNtuple::~TileTBHitToNtuple()
{
}

// Alg standard interface function
StatusCode TileTBHitToNtuple::initialize() 
{ 

  ATH_MSG_INFO( "Initialization started" );

  // retrieve TileTBID helper from det store

  CHECK( detStore()->retrieve(m_tileTBID) );

  m_ntupleLoc="/NTUPLES" + m_ntupleLoc;

  SmartDataPtr<NTuple::Directory> DirPtr(ntupleService(), m_ntupleLoc);
  if (!DirPtr) DirPtr = ntupleService()->createDirectory(m_ntupleLoc);
  if(!DirPtr) {
    ATH_MSG_ERROR( "Invalid Ntuple Directory: " );
    return StatusCode::FAILURE;
  }
  m_ntuplePtr=ntupleService()->book(DirPtr.ptr(), m_ntupleID, 
                                    CLID_ColumnWiseTuple, "TileTBHit-Ntuple");
  if(!m_ntuplePtr) {
    ATH_MSG_ERROR( "Failed to book ntuple: TileTBHitToNtuple" );
    return StatusCode::FAILURE;
  }

  CHECK( m_ntuplePtr->addItem("TileTBHit/nchan",m_nchan,0,max_chan) );
  CHECK( m_ntuplePtr->addItem("TileTBHit/energy",m_nchan,m_energy) );
  CHECK( m_ntuplePtr->addItem("TileTBHit/time",m_nchan,m_time) );
  CHECK( m_ntuplePtr->addItem("TileTBHit/type",m_nchan,m_type,-1,6) );
  CHECK( m_ntuplePtr->addItem("TileTBHit/module", m_nchan, m_module,0,7) );
  CHECK( m_ntuplePtr->addItem("TileTBHit/channel",m_nchan,m_channel,0,15) );
  CHECK( m_ntuplePtr->addItem("TileTBHit/totalE",m_tolE) );

  ATH_MSG_INFO( "Initialization completed" );
  return StatusCode::SUCCESS;
}

StatusCode TileTBHitToNtuple::execute()
{

  // step1: read Hit for ancillary
  const TileHitVector* HitCnt; 
  CHECK( evtStore()->retrieve(HitCnt, m_hitContainer) );

  // step2: to fill items in ntuple
  TileHitVecConstIterator it=HitCnt->begin();  
  TileHitVecConstIterator end=HitCnt->end();  


  m_nchan=0;
  m_tolE=0.0;
  for(; it != end; ++it) {
    const TileHit * cinp = &(*it);
    m_tolE+=cinp->energy();
    m_energy[m_nchan]=cinp->energy();
    m_time[m_nchan]=cinp->time();

    Identifier id=cinp->identify();
    m_type[m_nchan]=m_tileTBID->type(id);
    m_channel[m_nchan]=m_tileTBID->channel(id);
    m_module[m_nchan]=m_tileTBID->module(id);

    m_nchan++;

    if (m_nchan >= max_chan) break;
  }      
 
  // step3: commit ntuple
  CHECK( ntupleService()->writeRecord(m_ntuplePtr) );

  // Execution completed.
  ATH_MSG_DEBUG( "execute() completed successfully" );
  return StatusCode::SUCCESS;
}

StatusCode TileTBHitToNtuple::finalize()
{
  ATH_MSG_INFO( "finalize() successfully" );
  return StatusCode::SUCCESS;
}

