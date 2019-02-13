/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// class TBNoiseWrite // Write ROOT Tree with noise in randomly triggered 
//                       events
//
// author: A.Minaenko
// date:  08/05/2008

#include "TBNoiseWrite.h"

#include "PathResolver/PathResolver.h"

#include <TFile.h>
#include <TTree.h>

#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"

#include "TBEvent/TBEventInfo.h"

#include <fstream>

TBNoiseWrite::TBNoiseWrite(const std::string& name, 
				   ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator),
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
  //m_calo_id(0),
  //m_calo_dd_man(0),
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
  // Cell vectors
  m_cell_id = new std::vector<unsigned int>;
  m_cell_energy = new std::vector<float>;

  // Open file and create TTrees
  m_rootfile = new TFile(m_rootfile_name.data(), "RECREATE");
  if (!m_rootfile->IsOpen()) {
    ATH_MSG_FATAL ( "Cann't open Root file" );
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
  
  ATH_MSG_INFO ( "end of initialize()" );
  return StatusCode::SUCCESS;
}


StatusCode TBNoiseWrite::finalize()
{
  ATH_MSG_INFO ( "finalize(): (invoked/random) ("<< m_nEvent
                 << "/" << m_nEventRandomTrigger << ")" );
  ATH_MSG_INFO ( "Print contents of " << m_noiseTreeName );
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
  ATH_MSG_DEBUG ( "Executing TBNoiseWrite " );

  // Retrieve Event Info
  const TBEventInfo* theEventInfo;
  ATH_CHECK( evtStore()->retrieve(theEventInfo,"TBEventInfo") );
  unsigned short evType = theEventInfo->getEventType();
  ATH_MSG_DEBUG ( "Event Type found " << evType );
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
      ATH_MSG_ERROR ( "xCryo and yTable are not found for run " <<
                      m_header.m_nRun << " in file " << m_txtFileWithXY );
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO ( "nRun = " << m_header.m_nRun << ", beamMomentum = "
	<< m_header.m_beamMom << " GeV, CryoX = " << m_header.m_xCryo 
                   << ", tableY = " << m_header.m_yTable );
    m_header_tree->Fill();
  }  

  this->clear();

  // Get cell information
  const CaloCellContainer* cellContainer = 0;
  ATH_CHECK( evtStore()->retrieve(cellContainer, m_caloCellContainerName) );

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

  ATH_MSG_DEBUG ( "in getXcryoYtable(float x, float y)" );

  std::ifstream xyFile;
  std::string line;
  std::string filename = PathResolver::find_file(m_txtFileWithXY, "DATAPATH");
  xyFile.open(filename.c_str());
  if (!xyFile.is_open()) {
    ATH_MSG_ERROR ( "File " << m_txtFileWithXY << " fail to open in $DATAPATH");
    return StatusCode::FAILURE;
  }

  while ( getline(xyFile, line, '\n') ) {
    int run;
    std::istringstream buf(line);
    e = 0;
    buf >> run >> x >> y >> e;
    ATH_MSG_DEBUG ( "run,x,y,e= "<<run<<" "<<x<<" "<<y<<" "<<e);
    if (run == m_header.m_nRun && xyFile.good()) return StatusCode::SUCCESS;
  }

  return StatusCode::FAILURE;
}
