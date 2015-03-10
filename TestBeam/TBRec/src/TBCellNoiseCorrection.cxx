/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBRec/TBCellNoiseCorrection.h"

#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloEvent/CaloCell.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "PathResolver/PathResolver.h"

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "CLHEP/Random/RandFlat.h"

#include <TFile.h>
#include <TChain.h>


using CLHEP::RandFlat;


TBCellNoiseCorrection::TBCellNoiseCorrection(const std::string& type, 
						 const std::string& name,
						 const IInterface* parent)
  : CaloCellCorrection(type, name, parent),
    m_noise_correction(false),m_noise_file(),
    m_rndmSvc ("AtRanluxGenSvc", name), m_engine(0), m_log(0), 
    m_tree(0), m_xcryo(0.), m_ytable(0.), m_energy(-1.), m_cell_id(0), m_cell_energy(0),m_entries(0)
    //m_myevtnum(-1)
{ 
  declareInterface<CaloCellCorrection>(this);
  declareProperty("NoiseFile", m_noise_file);
  declareProperty("NoiseAdd", m_noise_correction);
  declareProperty("NoiseX",   m_xcryo);
  declareProperty("NoiseY",   m_ytable);
  declareProperty("NoiseE",   m_energy);
}

// Desctructor

TBCellNoiseCorrection::~TBCellNoiseCorrection()
{  }


StatusCode TBCellNoiseCorrection::initialize()
{
  m_log = new MsgStream(msgSvc(), name());

  *m_log << MSG::DEBUG << " TBCellNoiseCorrection initialization " << endreq ;

  // RandomNumbers
  if (!m_rndmSvc.retrieve().isSuccess()) {
     *m_log << MSG::FATAL << "Could not initialize find Random Number Service." <<endreq;
     return StatusCode::FAILURE; 
  } else {
     m_engine = m_rndmSvc->GetEngine("TB_NOISE");
     if(!m_engine) {
        *m_log << MSG::FATAL << "Could not get the TB_NOISE engine..." <<endreq;
        return StatusCode::FAILURE; 
     }
  }

  if(m_energy > 0.) { // Do we have run parameters (x,y,energy) ?
     *m_log << MSG::DEBUG << " Trying to patch the noise file name "<< endreq;
     std::string fname = PathResolver::find_file ("xcryo_ytable.txt", "DATAPATH");
     std::ifstream xfile;
     xfile.open(fname.c_str());
     if(!xfile.good()){
        *m_log << MSG::FATAL << "Could not open file  xcryo_ytable.txt" << endreq;
	return StatusCode::FAILURE;
     }
     int runnumber;
     float x,y,en=-1;
     std::string line;
     while (getline(xfile, line, '\n')) {
	std::istringstream buf(line);
        buf >> runnumber >> x >> y >> en;
        if(en < 0. || !(x == m_xcryo && y == m_ytable && en == m_energy)) continue;
        break;
     }
     if(en < 0.) {
        *m_log << MSG::FATAL << " Do not have X: "<<m_xcryo<<" Y: "<<m_ytable<< " E: "<<m_energy<<" in 	xcryo_ytable.txt" << endreq;
	return StatusCode::FAILURE;
     }
     // we have run number - let's change the file name (and take only one):
     *m_log << MSG::DEBUG << "Found run number: "<<runnumber <<endreq;
     unsigned pos = m_noise_file[0].find_last_of("0123456789");
     std::ostringstream buf;
     buf<<runnumber;
     m_noise_file[0].replace(pos-3,4,buf.str());
     *m_log << MSG::DEBUG << "patched name: "<<m_noise_file[0].c_str()<< endreq;
  }

  // Open file and get Tree
  /*
  m_root = new TFile(m_noise_file.c_str(),"READ"); 
  if(!m_root->IsOpen()){
     *m_log << MSG::FATAL << "Could not open root file: "<<m_noise_file<<endreq;
     return StatusCode::FAILURE; 
  }
  m_tree = (TTree*) m_root->Get("NoiseTree");
  if(!m_tree) {
     *m_log << MSG::FATAL << "No NoiseTree in root file: "<<m_noise_file<<endreq;
     return StatusCode::FAILURE; 
  }
  */
  // Make a chain
  m_tree = new TChain("NoiseTree");
  for(unsigned int l=0; l<m_noise_file.size(); ++l) {
     m_tree->AddFile(m_noise_file[l].c_str());
  }
  m_entries = m_tree->GetEntries();
  if(m_entries < 100) {
     *m_log << MSG::FATAL << "Noise chain has less then 100 events !!!!" <<endreq;
     return StatusCode::FAILURE; 
  }
  m_tree->SetBranchAddress("cell_id", &m_cell_id);
  m_tree->SetBranchAddress("cell_ener", &m_cell_energy);
  *m_log << MSG::DEBUG <<" Got NoiseTree with "<<m_entries<<" entries"<<endreq;

   /// Incident Service
  IIncidentSvc* pIncSvc(0);

  // set up the incident service:
  if (!(service("IncidentSvc", pIncSvc, true)).isSuccess()) {
    *m_log << MSG::ERROR << "Could not locate IncidentSvc " << endreq;
    return StatusCode::FAILURE;
  }
  
  //start listening to "BeginEvent"
  static const int PRIORITY = 100;
  pIncSvc->addListener(this, "BeginEvent", PRIORITY);

  *m_log << MSG::DEBUG << " TBCellNoiseCorrection initialization finished" << endreq ;
  
  return StatusCode::SUCCESS; 
}

StatusCode TBCellNoiseCorrection::finalize()
{
//  m_root->Close();
//  m_root = NULL;
  if(m_tree) m_tree->Delete() ;
//  m_cell_id = NULL;
//  m_cell_energy = NULL;

  return StatusCode::SUCCESS; 
} 
// MakeCorrection:  This is called with a pointer to the Cell Object.

void TBCellNoiseCorrection::MakeCorrection(CaloCell* theCell)
{

//   const CaloDetDescrElement* elt = theCell->caloDDE();

   if (m_noise_correction) {
    unsigned int cid = theCell->ID().get_identifier32().get_compact();    
    double e = theCell->energy();
    unsigned int size = m_cell_id->size();
    unsigned int i;
    // removing this, should use the BadChannel now
    // if(cid == 810049536)  e = 0.; // Not working cell in data
    for(i = 0; i < size; ++i) {
       if((*m_cell_id)[i] == cid) {
          e += (*m_cell_energy)[i];
          break;
       }
    }
    if(i == size) {
       *m_log << MSG::ERROR << "Could not find a noise value for cell: "<<std::hex<<cid<<std::dec<<endreq;
    }

    theCell->setEnergy(e);
  }

}  

void TBCellNoiseCorrection::handle(const Incident &inc) {
  *m_log << MSG::DEBUG << " Handle BeginEvent " << endreq;

  if ( inc.type() == "BeginEvent") {
     unsigned int random = int(RandFlat::shoot(m_engine, 0., m_entries-1.));
     *m_log << MSG::DEBUG << "Reading noise event: "<<random<<endreq;
     m_tree->GetEntry(random);
     unsigned int size = m_cell_id->size();
     unsigned int sizee = m_cell_energy->size();
     if(size != sizee) {
        *m_log << MSG::ERROR << "Not equal size of noise vectors !!!! Something wrong !!!" <<endreq;
     }
  }
}
