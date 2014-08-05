/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class TBNoiseWrite // Write ROOT Tree with noise in randomly triggered 
//                       events
//
// author: A.Minaenko
// date:  08/05/2008

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"

#include "PathResolver/PathResolver.h"

#include <TFile.h>
#include <TTree.h>

#include "StoreGate/StoreGate.h" 
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"

#include "TBEvent/TBEventInfo.h"

#include "TBRec/TBNoiseWrite.h"

#include <fstream>

TBNoiseWrite::TBNoiseWrite(const std::string& name, 
				   ISvcLocator* pSvcLocator) : 
  Algorithm(name, pSvcLocator),
  m_nEvent(0), m_nEventRandomTrigger(0), m_first(true),
  m_cell_id(0),
  m_cell_energy(0),
  m_caloCellContainerName("AllCalo"),
  m_headerTreeName("HeaderTree"),
  m_noiseTreeName("NoiseTree"),
  m_rootfile_name("tbh6tree_noise.root"),
  m_rootfile(0),
  m_header_tree(0),
  m_tree(0),
  m_eventStore(0),
  m_detStore(0),
  m_calo_id(0),
  m_calo_dd_man(0),
  m_larCablingSvc(0),
  m_txtFileWithXY("xcryo_ytable.txt")
{ 
  declareProperty("CellContainer",m_caloCellContainerName);
  declareProperty("RootFileName",m_rootfile_name);
  declareProperty("NoiseTreeName",m_noiseTreeName);

  m_header.m_nRun = 0;
  m_header.m_beamMom = 0;
  m_header.m_xCryo = 0;
  m_header.m_yTable = 0;
}

TBNoiseWrite::~TBNoiseWrite()
{ }

StatusCode TBNoiseWrite::initialize()
{
  
  MsgStream log(messageService(), name());
  log << MSG::INFO << "in initialize()" << endreq;

  // Cell vectors
  m_cell_id = new std::vector<unsigned int>;
  m_cell_energy = new std::vector<float>;

  // Open file and create TTrees
  m_rootfile = new TFile(m_rootfile_name.data(), "RECREATE");
  if (!m_rootfile->IsOpen()) {
    log << MSG::FATAL << "Cann't open Root file" << endreq;
    return StatusCode::FAILURE;
  }
  m_header_tree = 
    new TTree(m_headerTreeName.c_str(), m_headerTreeName.c_str());
  m_tree = new TTree(m_noiseTreeName.c_str(), m_noiseTreeName.c_str());

  // Define branches
  // Header
  m_header_tree->
    Branch("Header",&m_header.m_nRun,"nrun/I:ener/F:xcryo:ytable");
  // Cell parameters
  m_tree->Branch("cell_id", &m_cell_id);
  m_tree->Branch("cell_ener", &m_cell_energy);
  
  // Get StoreGateSvc //
  StatusCode sc = service ( "StoreGateSvc" , m_eventStore ) ;  
  if( sc.isFailure() ) {
    log<<MSG::FATAL<<" Cannot locate StoreGateSvc " << endreq ;
    sc = StatusCode::FAILURE ;
    return sc ;
  }

  log << MSG::INFO << "end of initialize()" << endreq;
  return StatusCode::SUCCESS;
}


StatusCode TBNoiseWrite::finalize()
{
  MsgStream log( messageService(), name() );

  log << MSG::INFO << "finalize(): (invoked/random) ("<< m_nEvent
      << "/" << m_nEventRandomTrigger << ")" << endreq;
  log << MSG::INFO << "Print contents of " << m_noiseTreeName << endreq;
  m_rootfile->Print();
  m_rootfile->Write();
  m_rootfile->Close();
  return StatusCode::SUCCESS;
}

void TBNoiseWrite::clear()
{
  m_cell_id->clear(); 
  m_cell_energy->clear(); 
}

StatusCode TBNoiseWrite::execute()
{
  m_nEvent++;
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << "Executing TBNoiseWrite " << endreq;

  StatusCode sc;
  // Retrieve Event Info
  const TBEventInfo* theEventInfo;
  sc = m_eventStore->retrieve(theEventInfo,"TBEventInfo");
  if ( sc.isFailure() ) {
    log << MSG::ERROR
	<< "Cannot retrieve TBEventInfo from StoreGate" << endreq;
    return StatusCode::FAILURE;
  }
  unsigned short evType = theEventInfo->getEventType();
  log << MSG::DEBUG << "Event Type found " << evType << endreq;
  if (evType != 3) return StatusCode::FAILURE;

  // Do first event initialization (run header filling)
  if (m_first) {
    m_first = false;
    // Fill run header
    m_header.m_nRun = theEventInfo->getRunNum();
    m_header.m_beamMom = theEventInfo->getBeamMomentum();
    // Get xcryo and ytable from a file
    if (!this->getXcryoYtable(m_header.m_xCryo,m_header.m_yTable,
			      m_header.m_beamMom)) {
      log << MSG::ERROR << "xCryo and yTable are not found for run " <<
	m_header.m_nRun << " in file " << m_txtFileWithXY << endreq;
      return StatusCode::FAILURE;
    }
    log << MSG::INFO << "nRun = " << m_header.m_nRun << ", beamMomentum = "
	<< m_header.m_beamMom << " GeV, CryoX = " << m_header.m_xCryo 
	<< ", tableY = " << m_header.m_yTable << endreq;
    m_header_tree->Fill();
  }  

  this->clear();

  // Get cell information
  const CaloCellContainer* cellContainer;
  sc = m_eventStore->retrieve(cellContainer, m_caloCellContainerName);
  if (sc.isFailure()) {
    log << MSG::ERROR
	<< "cannot allocate CaloCellContainer with key <"
	<< m_caloCellContainerName
	<< "> for random trigger event"
	<< endreq;
    return sc;
  }
  // Cell loop
  CaloCellContainer::const_iterator itc = cellContainer->begin();
  for (;itc!=cellContainer->end(); itc++) {
    const CaloCell* cell = (*itc);
    unsigned int id = cell->ID().get_identifier32().get_compact();
    m_cell_id->push_back(id);
    m_cell_energy->push_back((float)cell->energy());
  }
  // Fill the tree
  m_tree->Fill();
  m_nEventRandomTrigger++;
  return StatusCode::SUCCESS;

}

StatusCode TBNoiseWrite::getXcryoYtable(float &x, float &y, float &e) {

  MsgStream log(messageService(),name());
  log << MSG::DEBUG << "in getXcryoYtable(float x, float y)" << endreq;

  std::ifstream xyFile;
  std::string line;
  std::string filename = PathResolver::find_file(m_txtFileWithXY, "DATAPATH");
  xyFile.open(filename.c_str());
  if (!xyFile.is_open()) {
    log << MSG::ERROR << "File " << m_txtFileWithXY << " fail to open in $DATAPATH" 
	<< endreq;
    return StatusCode::FAILURE;
  }

  while ( getline(xyFile, line, '\n') ) {
    int run;
    std::istringstream buf(line);
    e = 0;
    buf >> run >> x >> y >> e;
    log << MSG::DEBUG << "run,x,y,e= "<<run<<" "<<x<<" "<<y<<" "<<e<<endreq;
    if (run == m_header.m_nRun && xyFile.good()) return StatusCode::SUCCESS;
  }

  return StatusCode::FAILURE;
}
