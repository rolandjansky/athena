/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileMuToNtuple.cxx
//*****************************************************************************

//Tile includes
#include "TileMuId/TileMuToNtuple.h"

// Athena incldues
#include "StoreGate/ReadHandle.h"

//Gaudi Includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"


// Constructor & deconstructor
TileMuToNtuple::TileMuToNtuple(std::string name, ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator)
  , m_ntuplePtr(0)
  , m_ntupleID(100)
  , m_maxNtag(50)
  , m_close(1)
  , m_ntupleLoc("/FILE1/TileMuTag")
  , m_tileMuContainer("TileMuObj")
{

  declareProperty("NTupleLoc", m_ntupleLoc);
  declareProperty("NTupleID", m_ntupleID);
  declareProperty("MaxNtag", m_maxNtag);
  declareProperty("CloseNtuple", m_close);
}

TileMuToNtuple::~TileMuToNtuple() {
}

// Alg standard interface function
StatusCode TileMuToNtuple::initialize() {

  m_ntupleLoc = "/NTUPLES/FILE1/TileMuTag";
  // + m_ntupleLoc;

  SmartDataPtr<NTuple::Directory> DirPtr(ntupleSvc(), m_ntupleLoc);
  if (!DirPtr) DirPtr = ntupleSvc()->createDirectory(m_ntupleLoc);
  if (!DirPtr) {
    ATH_MSG_ERROR( "Invalid Ntuple Directory: " << m_ntupleLoc );
    return StatusCode::FAILURE;
  }

  m_ntuplePtr = ntupleSvc()->book(DirPtr.ptr(), m_ntupleID, CLID_ColumnWiseTuple,
      "TileMuTag-Ntuple");
  if (!m_ntuplePtr) {
    ATH_MSG_ERROR( "Failed to book ntuple: TileMuTagNtuple" );
    return StatusCode::FAILURE;
  }

  CHECK( m_ntuplePtr->addItem("TileMu/ntag", m_ntag, 0, m_maxNtag) );
  CHECK( m_ntuplePtr->addItem("TileMu/etatag", m_ntag, m_eta, -1.5, 1.5) );
  CHECK( m_ntuplePtr->addItem("TileMu/phitag", m_ntag, m_phi, 0., 6.3) );
  CHECK( m_ntuplePtr->addItem("TileMu/energydepVec", m_ntag, m_energy, 4) );
  CHECK( m_ntuplePtr->addItem("TileMu/quality", m_ntag, m_quality, 0., 1.) );

  ATH_CHECK( m_muContainerKey.initialize() );

  ATH_MSG_INFO( "initialisation completed" );

  return StatusCode::SUCCESS;
}

StatusCode TileMuToNtuple::execute() {

  // step1: read  from TDS
  SG::ReadHandle<TileMuContainer> muContainer(m_muContainerKey);
  ATH_CHECK( muContainer.isValid() );

  m_ntag = 0;

  for (const TileMu* mu : *muContainer) {

    m_eta[m_ntag] = mu->eta();

    m_energy[m_ntag][0] = (mu->enedep())[0];
    m_energy[m_ntag][1] = (mu->enedep())[1];
    m_energy[m_ntag][2] = (mu->enedep())[2];
    m_energy[m_ntag][3] = (mu->enedep())[3];
    m_phi[m_ntag] = mu->phi();
    m_quality[m_ntag] = mu->quality();
    m_ntag++;

    if (m_ntag >= m_maxNtag) break;
  }

  //  write  ntuple (is useful to keep it  open to write also MC truth)
  if (m_close == 1) {
    CHECK( ntupleSvc()->writeRecord(m_ntuplePtr) );
  }

  // Execution completed.
  ATH_MSG_DEBUG( "execute() completed successfully" );

  return StatusCode::SUCCESS;
}

StatusCode TileMuToNtuple::finalize() {
  ATH_MSG_INFO( "finalize() successfully" );

  return StatusCode::SUCCESS;
}

