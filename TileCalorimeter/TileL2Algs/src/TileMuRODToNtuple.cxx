/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileMuRODToNtuple.cxx
//  Author   : Aranzazu Ruiz
//  Created  : March 2006
//
//  DESCRIPTION:
//     Implementation comments only.  Class level comments go in .h file.
//
//  HISTORY:
//
//  BUGS:
//    
//    
//*****************************************************************************

// Gaudi includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// Athena includes
#include "AthenaKernel/errorcheck.h"

// Tile includes
#include "TileEvent/TileContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileL2Algs/TileMuRODToNtuple.h"

TileMuRODToNtuple::TileMuRODToNtuple(std::string name, ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator)
  , m_ntuplePtr(0)
  , m_ntupleID(100)
  , m_maxNMuons(50)
  , m_close(1)
  , m_ntupleLoc("/FILE1/TileMuRODTag")
  , m_tileL2Container("TileL2Cnt")
{
  declareProperty("TileMuRODTagsOutputName", m_tileL2Container);
  declareProperty("NTupleLoc", m_ntupleLoc);
  declareProperty("NTupleID", m_ntupleID);
  declareProperty("MaxNMuons", m_maxNMuons);
  declareProperty("CloseNtuple", m_close);
}

TileMuRODToNtuple::~TileMuRODToNtuple() {
}

StatusCode TileMuRODToNtuple::initialize() {

  m_ntupleLoc = "/NTUPLES/FILE1/TileMuRODTag";

  SmartDataPtr<NTuple::Directory> DirPtr(ntupleService(), m_ntupleLoc);
  if (!DirPtr) DirPtr = ntupleService()->createDirectory(m_ntupleLoc);

  if (!DirPtr) {
    ATH_MSG_ERROR( "Invalid Ntuple Directory: " << m_ntupleLoc );
    return StatusCode::FAILURE;
  }

  m_ntuplePtr = ntupleService()->book(DirPtr.ptr(), m_ntupleID, CLID_ColumnWiseTuple
                                      , "TileMuRODTag-Ntuple");

  if (!m_ntuplePtr) {
    ATH_MSG_ERROR( "Failed to book ntuple: TileMuRODTagNtuple" );
    return StatusCode::FAILURE;
  }

  CHECK(m_ntuplePtr->addItem("TileMuROD/NMuons", m_ntag, 0, m_maxNMuons));
  CHECK(m_ntuplePtr->addItem("TileMuROD/EtaMuons", m_ntag, m_eta, -1.5, 1.5));
  CHECK(m_ntuplePtr->addItem("TileMuROD/PhiMuons", m_ntag, m_phi, 0., 6.3));

  ATH_MSG_INFO( "Initialization completed" );

  return StatusCode::SUCCESS;
}

StatusCode TileMuRODToNtuple::execute() {

  // step1: read from TDS
  const TileL2Container* mutags_ROD;
  CHECK( evtStore()->retrieve(mutags_ROD, m_tileL2Container) );

  TileL2Container::const_iterator it = mutags_ROD->begin();
  TileL2Container::const_iterator end = mutags_ROD->end();

  m_ntag = 0;

  for (; it != end; ++it) {
    for (unsigned int a = 0; a < ((*it)->NMuons()); ++a) {
      m_eta[m_ntag] = ((*it)->eta(a));
      m_phi[m_ntag] = ((*it)->phi(a));
      m_ntag++;
    }
    if (m_ntag >= m_maxNMuons) break;
  }

  if (m_close == 1) {
    CHECK( ntupleService()->writeRecord(m_ntuplePtr) );
  }

  ATH_MSG_DEBUG( "execute() completed successfully" );

  return StatusCode::SUCCESS;
}

StatusCode TileMuRODToNtuple::finalize() {

  ATH_MSG_INFO( "finalize() successfully" );

  return StatusCode::SUCCESS;
}
