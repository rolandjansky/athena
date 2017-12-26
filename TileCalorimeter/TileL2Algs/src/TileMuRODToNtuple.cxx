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

// Tile includes
#include "TileL2Algs/TileMuRODToNtuple.h"

// Athena includes
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/ReadHandle.h"

// Gaudi includes
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"


TileMuRODToNtuple::TileMuRODToNtuple(std::string name, ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator)
  , m_ntuplePtr(0)
  , m_ntupleID(100)
  , m_maxNMuons(50)
  , m_close(1)
  , m_ntupleLoc("/FILE1/TileMuRODTag")
{
  declareProperty("NTupleLoc", m_ntupleLoc);
  declareProperty("NTupleID", m_ntupleID);
  declareProperty("MaxNMuons", m_maxNMuons);
  declareProperty("CloseNtuple", m_close);
}

TileMuRODToNtuple::~TileMuRODToNtuple() {
}

StatusCode TileMuRODToNtuple::initialize() {

  m_ntupleLoc = "/NTUPLES/FILE1/TileMuRODTag";

  SmartDataPtr<NTuple::Directory> DirPtr(ntupleSvc(), m_ntupleLoc);
  if (!DirPtr) DirPtr = ntupleSvc()->createDirectory(m_ntupleLoc);

  if (!DirPtr) {
    ATH_MSG_ERROR( "Invalid Ntuple Directory: " << m_ntupleLoc );
    return StatusCode::FAILURE;
  }

  m_ntuplePtr = ntupleSvc()->book(DirPtr.ptr(), m_ntupleID, CLID_ColumnWiseTuple
                                      , "TileMuRODTag-Ntuple");

  if (!m_ntuplePtr) {
    ATH_MSG_ERROR( "Failed to book ntuple: TileMuRODTagNtuple" );
    return StatusCode::FAILURE;
  }

  CHECK(m_ntuplePtr->addItem("TileMuROD/NMuons", m_ntag, 0, m_maxNMuons));
  CHECK(m_ntuplePtr->addItem("TileMuROD/EtaMuons", m_ntag, m_eta, -1.5, 1.5));
  CHECK(m_ntuplePtr->addItem("TileMuROD/PhiMuons", m_ntag, m_phi, 0., 6.3));

  ATH_CHECK( m_l2ContainerKey.initialize() );

  ATH_MSG_INFO( "Initialization completed" );

  return StatusCode::SUCCESS;
}

StatusCode TileMuRODToNtuple::execute() {

  // step1: read from TDS
  SG::ReadHandle<TileL2Container> l2Container(m_l2ContainerKey);
  ATH_CHECK( l2Container.isValid() );

  m_ntag = 0;
  for (const TileL2* l2 : *l2Container) {
    for (unsigned int a = 0; a < l2->NMuons(); ++a) {
      m_eta[m_ntag] = l2->eta(a);
      m_phi[m_ntag] = l2->phi(a);
      m_ntag++;
    }
    if (m_ntag >= m_maxNMuons) break;
  }

  if (m_close == 1) {
    CHECK( ntupleSvc()->writeRecord(m_ntuplePtr) );
  }

  ATH_MSG_DEBUG( "execute() completed successfully" );

  return StatusCode::SUCCESS;
}

StatusCode TileMuRODToNtuple::finalize() {

  ATH_MSG_INFO( "finalize() successfully" );

  return StatusCode::SUCCESS;
}
