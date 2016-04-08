/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileMuToNtuple.cxx
//*****************************************************************************

//Gaudi Includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

//TileCalo include
#include "TileEvent/TileMuContainer.h"
#include "TileMuId/TileMuToNtuple.h"

//const int max_ntag=50;

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

  declareProperty("TileMuTagsOutputName", m_tileMuContainer);
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

  SmartDataPtr<NTuple::Directory> DirPtr(ntupleService(), m_ntupleLoc);
  if (!DirPtr) DirPtr = ntupleService()->createDirectory(m_ntupleLoc);
  if (!DirPtr) {
    ATH_MSG_ERROR( "Invalid Ntuple Directory: " << m_ntupleLoc );
    return StatusCode::FAILURE;
  }

  m_ntuplePtr = ntupleService()->book(DirPtr.ptr(), m_ntupleID, CLID_ColumnWiseTuple,
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

  ATH_MSG_INFO( "initialisation completed" );

  return StatusCode::SUCCESS;
}

StatusCode TileMuToNtuple::execute() {

  // step1: read  from TDS
  const TileMuContainer* mutags_cont;
  CHECK( evtStore()->retrieve(mutags_cont, m_tileMuContainer) );

  TileMuContainer::const_iterator it = mutags_cont->begin();
  TileMuContainer::const_iterator end = mutags_cont->end();
  m_ntag = 0;
  for (; it != end; ++it) {
    m_eta[m_ntag] = (*it)->eta();
    //   std::vector<double> ene;
    // ene.reserve(4);
    // ene.push_back( ((*it)->enedep()) [0]);

    m_energy[m_ntag][0] = ((*it)->enedep())[0];
    m_energy[m_ntag][1] = ((*it)->enedep())[1];
    m_energy[m_ntag][2] = ((*it)->enedep())[2];
    m_energy[m_ntag][3] = ((*it)->enedep())[3];
    m_phi[m_ntag] = (*it)->phi();
    m_quality[m_ntag] = (*it)->quality();
    m_ntag++;

    if (m_ntag >= m_maxNtag) break;
  }

  //  write  ntuple (is useful to keep it  open to write also MC truth)
  if (m_close == 1) {
    CHECK( ntupleService()->writeRecord(m_ntuplePtr) );
  }

  // Execution completed.
  ATH_MSG_DEBUG( "execute() completed successfully" );

  return StatusCode::SUCCESS;
}

StatusCode TileMuToNtuple::finalize() {
  ATH_MSG_INFO( "finalize() successfully" );

  return StatusCode::SUCCESS;
}

