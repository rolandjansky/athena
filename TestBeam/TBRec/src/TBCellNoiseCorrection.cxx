/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TBCellNoiseCorrection.h"

#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloEvent/CaloCell.h"
#include "AthenaKernel/RNGWrapper.h"

#include "PathResolver/PathResolver.h"

#include "CLHEP/Random/RandFlat.h"

#include <TFile.h>
#include <TChain.h>


using CLHEP::RandFlat;


TBCellNoiseCorrection::TBCellNoiseCorrection(const std::string& type, 
						 const std::string& name,
						 const IInterface* parent)
  : CaloCellCorrection(type, name, parent),
    m_noise_correction(false),m_noise_file(),
    m_engine(0),
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
  ATH_MSG_DEBUG( " TBCellNoiseCorrection initialization " );

  // RandomNumbers
  ATH_CHECK( m_rndmSvc.retrieve() );
  m_engine = m_rndmSvc->getEngine(this, "TB_NOISE");

  if(m_energy > 0.) { // Do we have run parameters (x,y,energy) ?
     ATH_MSG_DEBUG( " Trying to patch the noise file name " );
     std::string fname = PathResolver::find_file ("xcryo_ytable.txt", "DATAPATH");
     std::ifstream xfile;
     xfile.open(fname.c_str());
     if(!xfile.good()){
        ATH_MSG_FATAL( "Could not open file  xcryo_ytable.txt"  );
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
        ATH_MSG_FATAL( " Do not have X: "<<m_xcryo<<" Y: "<<m_ytable<< " E: "<<m_energy<<" in 	xcryo_ytable.txt"  );
	return StatusCode::FAILURE;
     }
     // we have run number - let's change the file name (and take only one):
     ATH_MSG_DEBUG( "Found run number: "<<runnumber  );
     unsigned pos = m_noise_file[0].find_last_of("0123456789");
     std::ostringstream buf;
     buf<<runnumber;
     m_noise_file[0].replace(pos-3,4,buf.str());
     ATH_MSG_DEBUG( "patched name: "<<m_noise_file[0].c_str() );
  }

  // Open file and get Tree
  /*
  m_root = new TFile(m_noise_file.c_str(),"READ"); 
  if(!m_root->IsOpen()){
     ATH_MSG_FATAL( "Could not open root file: "<<m_noise_file );
     return StatusCode::FAILURE; 
  }
  m_tree = (TTree*) m_root->Get("NoiseTree");
  if(!m_tree) {
     ATH_MSG_FATAL( "No NoiseTree in root file: "<<m_noise_file );
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
     ATH_MSG_FATAL( "Noise chain has less then 100 events !!!!"  );
     return StatusCode::FAILURE; 
  }
  m_tree->SetBranchAddress("cell_id", &m_cell_id);
  m_tree->SetBranchAddress("cell_ener", &m_cell_energy);
  ATH_MSG_DEBUG(" Got NoiseTree with "<<m_entries<<" entries" );

   /// Incident Service
  IIncidentSvc* pIncSvc(0);

  // set up the incident service:
  if (!(service("IncidentSvc", pIncSvc, true)).isSuccess()) {
    ATH_MSG_ERROR( "Could not locate IncidentSvc "  );
    return StatusCode::FAILURE;
  }
  
  //start listening to "BeginEvent"
  static const int PRIORITY = 100;
  pIncSvc->addListener(this, "BeginEvent", PRIORITY);

  ATH_MSG_DEBUG( " TBCellNoiseCorrection initialization finished"  );
  
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

void TBCellNoiseCorrection::MakeCorrection (CaloCell* theCell,
                                            const EventContext& /*ctx*/) const
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
      ATH_MSG_ERROR( "Could not find a noise value for cell: "<<std::hex<<cid<<std::dec );
    }

    theCell->setEnergy(e);
  }

}  

void TBCellNoiseCorrection::handle(const Incident &inc) {
  ATH_MSG_DEBUG( " Handle BeginEvent "  );

  if ( inc.type() == "BeginEvent") {
     m_engine->setSeed ("TBCellNoseCorrection", inc.context());
     unsigned int random = int(RandFlat::shoot(m_engine->getEngine (inc.context()), 0., m_entries-1.));
     ATH_MSG_DEBUG( "Reading noise event: "<<random );
     m_tree->GetEntry(random);
     unsigned int size = m_cell_id->size();
     unsigned int sizee = m_cell_energy->size();
     if(size != sizee) {
       ATH_MSG_ERROR( "Not equal size of noise vectors !!!! Something wrong !!!"  );
     }
  }
}
