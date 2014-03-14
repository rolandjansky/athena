/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Section of includes for LAr calo tests
#include "LArSimEvent/LArHitContainer.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "GeoAdaptors/GeoLArHit.h"

// Section of includes for tile calo tests
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "TileConditions/TileInfo.h"

#include "TileDetDescr/TileDetDescrManager.h"
#include "CaloIdentifier/TileID.h"
#include "TileSimEvent/TileHit.h"
#include "TileSimEvent/TileHitVector.h"

//CaloCell
#include "CaloEvent/CaloCellContainer.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ITHistSvc.h"

#include "StoreGate/StoreGateSvc.h"

#include "ISF_Event/FCS_StepInfoCollection.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TTree.h"
#include "TString.h"

#include "ISF_FastCaloSimParametrization/ISF_HitAnalysis.h"
// For MC Truth information:
#include "GeneratorObjects/McEventCollection.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>



ISF_HitAnalysis::ISF_HitAnalysis(const std::string& name, ISvcLocator* pSvcLocator)
   : AthAlgorithm(name, pSvcLocator)
   , m_storeGate(0)
   , m_tileID(0)
   , m_tileMgr(0)
   , m_hit_x(0)
   , m_hit_y(0)
   , m_hit_z(0)
   , m_hit_e(0)
   , m_hit_time(0)
   , m_detector(0)
   , m_identifier(0)
   , m_identifier_alt(0)
   , m_islarbarrel(0)
   , m_islarendcap(0)
   , m_islarhec(0)
   , m_islarfcal(0)
   , m_istile(0)
   , m_sampling(0)
   , m_samplingfraction(0)
   , m_truth_e(0)
   , m_truth_px(0)
   , m_truth_py(0)
   , m_truth_pz(0)
   , m_truth_pdg(0)
   , m_cell_identifier(0)
   , m_cell_e(0)
   , m_cell_sampling(0)
   , m_tree(0)
   , m_ntupleFileName("/ntuples/file1")
   , m_ntupleDirName("ISF_HitAnalysis")
   , m_ntupleTreeName("CaloHitAna")
   , m_thistSvc(0)
   , m_calo_dd_man(0)

{
  declareProperty("NtupleFileName", m_ntupleFileName); 
  declareProperty("NtupleDirectoryName", m_ntupleDirName); 
  declareProperty("NtupleTreeName", m_ntupleTreeName); 
}


ISF_HitAnalysis::~ISF_HitAnalysis() 
{;}

StatusCode ISF_HitAnalysis::initialize() {
  ATH_MSG_DEBUG( "Initializing ISF_HitAnalysis" );
  std::cout <<"ZH ahoj"<<std::endl;
  StoreGateSvc* detStore;
  StatusCode sc = service("DetectorStore",detStore);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Unable to get pointer to Detector Store Service" );
    return sc;
  }

  sc = detStore->retrieve(m_tileMgr);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Unable to retrieve TileDetDescrManager from DetectorStore" );
    m_tileMgr=0;
  }

  sc = detStore->retrieve(m_tileID);
  if (sc.isFailure()) { 
    ATH_MSG_ERROR( "Unable to retrieve TileID helper from DetectorStore" );
    m_tileID=0;
  }

  /** get a handle of StoreGate for access to the Event Store */
  sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure()) {
     ATH_MSG_ERROR( "Unable to retrieve pointer to StoreGateSvc" );
     return sc;
  }
  sc = service("DetectorStore", m_detStore);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "ZH Unable to retrieve pointer to Detector Store");
    return sc;
  }

  // Grab the Ntuple and histogramming service for the tree
  sc = service("THistSvc",m_thistSvc);
  if (sc.isFailure()) {
     ATH_MSG_ERROR( "Unable to retrieve pointer to THistSvc" );
     return sc;
  }

  const DataHandle<CaloIdManager> caloIdManager;
  sc=m_detStore->retrieve(caloIdManager);
  if(sc.isSuccess())
    ATH_MSG_DEBUG("CaloIDManager retrieved.");
  else
    throw std::runtime_error("ISF_HitAnalysis: Unable to retrieve CaloIDManeger");
  m_larEmID=caloIdManager->getEM_ID();
  if(m_larEmID==0)
    throw std::runtime_error("ISF_HitAnalysis: Invalid LAr EM ID helper");
  m_larFcalID=caloIdManager->getFCAL_ID();
  if(m_larFcalID==0)
    throw std::runtime_error("ISF_HitAnalysis: Invalid FCAL ID helper");
  m_larHecID=caloIdManager->getHEC_ID();
  if(m_larHecID==0)
    throw std::runtime_error("ISF_HitAnalysis: Invalid HEC ID helper");
  m_tileID=caloIdManager->getTileID();
  if(m_tileID==0)
    throw std::runtime_error("ISF_HitAnalysis: Invalid Tile ID helper");
  sc=m_detStore->regHandle(m_dd_fSampl,"LArfSampl");
  if(sc.isFailure())
    {
      ATH_MSG_ERROR("Unable to register handle for LArfSampl");
      return StatusCode::FAILURE;
    }
  
  m_detStore->retrieve(m_tileInfo,"TileInfo");
  if(sc.isFailure())
    {
      ATH_MSG_ERROR("Unable to retrieve TileInfo from DetectorStore");
      return StatusCode::FAILURE;
    }
  m_calo_dd_man  = CaloDetDescrManager::instance();
  // 
  m_tree = new TTree( TString(m_ntupleTreeName), "CaloHitAna" );
  std::string fullNtupleName =  "/"+m_ntupleFileName+"/"+m_ntupleDirName+"/"+m_ntupleTreeName; 
  sc = m_thistSvc->regTree(fullNtupleName, m_tree); 
  if (sc.isFailure()) { 
     ATH_MSG_ERROR("Unable to register TTree: " << fullNtupleName); 
     return sc; 
  }  

  /** now add branches and leaves to the tree */
  if (m_tree){
    m_tree->Branch("HitX", &m_hit_x);
    m_tree->Branch("HitY", &m_hit_y);
    m_tree->Branch("HitZ", &m_hit_z);
    m_tree->Branch("HitE", &m_hit_e);
    m_tree->Branch("HitTime",&m_hit_time);
    m_tree->Branch("NHit", &m_detector);
    m_tree->Branch("HitIdentifier",&m_identifier);
    m_tree->Branch("HitIdentifierAlt",&m_identifier_alt);
    m_tree->Branch("HitIsLArBarrel",&m_islarbarrel);
    m_tree->Branch("HitIsLArEndCap",&m_islarendcap);
    m_tree->Branch("HitIsHEC", &m_islarhec);
    m_tree->Branch("HitIsFCAL",&m_islarfcal);
    m_tree->Branch("HitIsTile",&m_istile);
    m_tree->Branch("HitSampling",&m_sampling);
    m_tree->Branch("HitSamplingFraction",&m_samplingfraction);

    m_tree->Branch("TruthE", &m_truth_e);
    m_tree->Branch("TruthPx",&m_truth_px);
    m_tree->Branch("TruthPy",&m_truth_py);
    m_tree->Branch("TruthPz",&m_truth_pz);
    m_tree->Branch("TruthPDG",&m_truth_pdg);

    m_tree->Branch("CellIdentifier", &m_cell_identifier);
    m_tree->Branch("CellE",&m_cell_e);
    m_tree->Branch("CellSampling",&m_cell_sampling);

  }

  return StatusCode::SUCCESS;
}		 

StatusCode ISF_HitAnalysis::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode ISF_HitAnalysis::execute() {
  ATH_MSG_DEBUG( "In ISF_HitAnalysis::execute()" );

  m_hit_x->clear();
  m_hit_y->clear();
  m_hit_z->clear();
  m_hit_e->clear();
  m_hit_time->clear();
  m_detector->clear();
  m_identifier->clear();
  m_identifier_alt->clear();
  m_islarbarrel->clear();
  m_islarendcap->clear();
  m_islarhec->clear();
  m_islarfcal->clear();
  m_istile->clear();
  m_sampling->clear();
  m_samplingfraction->clear();
  m_truth_e->clear();
  m_truth_px->clear();
  m_truth_py->clear();
  m_truth_pz->clear();
  m_truth_pdg->clear();
  m_cell_identifier->clear();
  m_cell_e->clear();
  m_cell_sampling->clear();
  //Get the FastCaloSim step info collection from store
  const ISF_FCS_Parametrization::FCS_StepInfoCollection* eventStepsES;
  StatusCode sc = evtStore()->retrieve(eventStepsES, "ZHMergedEventSteps");
  if (sc.isFailure())
    {
      std::cout <<"Warning: ISF_HitAnalysis: No FastCaloSim steps read from StoreGate??"<<std::endl;
      //return StatusCode::FAILURE;
    }
  else
    {
      std::cout <<"ISF_HitAnalysis: Read: "<<eventStepsES->size()<<" hits"<<std::endl;
      double total_e = 0.0;
      for (ISF_FCS_Parametrization::FCS_StepInfoCollection::const_iterator it = eventStepsES->begin(); it != eventStepsES->end(); ++it)
        {
	  m_hit_x->push_back( (*it)->x() );
	  m_hit_y->push_back( (*it)->y() );
	  m_hit_z->push_back( (*it)->z() );
	  m_hit_e->push_back( (*it)->energy() );
	  total_e+= (*it)->energy();
	  //Try to get the sampling fraction
	  bool larbarrel=false;
	  bool larendcap=false;
	  bool larhec=false;
	  bool larfcal=false;
	  bool tile=false;
	  int sampling=-1;
	  double sampfrac=0.0;
	  m_hit_time->push_back( (*it)->time());
	  m_detector->push_back( (*it)->detector() );
	  Identifier id = (*it)->identify();
	  m_identifier->push_back(id.getString());
	  if (m_calo_dd_man->get_element(id))
	    {
	      CaloCell_ID::CaloSample layer = m_calo_dd_man->get_element(id)->getSampling();
	      sampling = layer; //use CaloCell layer immediately
	    }
	  
	  if (m_larEmID->is_lar_em(id) || m_larHecID->is_lar_hec(id) || m_larFcalID->is_lar_fcal(id))
	    {
	      //This should get the sampling fraction from LAr 
	      //sampling = m_larEmID->sampling(id);
	      sampfrac=m_dd_fSampl->FSAMPL(id);
	    }
	  if (m_larEmID->is_lar_em(id))
	    {
	      //std::cout <<"This hit is in LAr EM ";
	      if (m_larEmID->is_em_barrel(id))
		{
		  larbarrel=true;
		  //ISF_FCS_Parametrization::FCS_StepInfo* si = new ISF_FCS_Parametrization::FCS_StepInfo(*(*it)); 
		  //std::cout <<"Barrel, sampling: "<<m_larEmID->sampling(id)<< " eta: "<<m_larEmID->eta(id)<< " Sampling frac: "<<SampFrac<<std::endl;
		}
	      else if (m_larEmID->is_em_endcap(id))
		{
		  larendcap=true;
		  //std::cout <<"End cap"<<std::endl;
		}
	    }
	  else if (m_larHecID->is_lar_hec(id))
	    {
	      larhec = true;
	      //std::cout <<"This hit is in HEC, phi: "<<m_larHecID->phi(id)<<std::endl;    
	    }
	  else if (m_larFcalID->is_lar_fcal(id))
	    {
	      larfcal = true;
	      //std::cout <<"This hit is in FCAL, module:  "<<m_larFcalID->module(id)<<std::endl;
	    }
	  else if (m_tileID->is_tile_barrel(id) || m_tileID->is_tile_extbarrel(id) || m_tileID->is_tile_gap(id) || m_tileID->is_tile_gapscin(id) || m_tileID->is_tile_aux(id))
	    {
	      // reserve for tile
	      // std::cout <<"This hit is somewhere"<<std::endl;
	      sampfrac = m_tileInfo->HitCalib(id);
	      tile = true;
	      //std::cout <<"Tile info: "<<sampfrac<<std::endl;
	    }
	  else
	    {
	      // std::cout <<"This hit is somewhere"<<std::endl;
	    }	  
	  //if tile, store also alternative identifier
	  Identifier idalt; //invalid
	  if (tile)
	    {
	      //NB, this might not be optimal/still missing some cases (?)
	      if (m_tileID->pmt_id(id,0) == m_tileID->pmt_id(id,1) )
                {
		  //ok, this is a test that the identifier id is not the correct one
		  Identifier cell_id = m_tileID->cell_id(id); //convert to cell identifier
                  if (m_tileID->pmt_id(cell_id,0)== id)
		    {
		      idalt = m_tileID->pmt_id(cell_id,1);
		    }
		  else
		    {
		      idalt = m_tileID->pmt_id(cell_id,0);
		    }
		  //std::cout <<"ZH: Tile info: Id: "<<id.getString()<<" is the same, using Alt:"<< idalt.getString()<<std::endl;
                }
              else
                {
		  if (m_tileID->pmt_id(id,0)==id)
		    {
		      idalt = m_tileID->pmt_id(id,1);
		      //std::cout <<"ZH: AltID: V1: "<<id.getString()<<" / "<<idalt.getString()<<std::endl;
		    }
		  else
		    {
		      //never here?? because pmt_id(id,1) is the same as reco cell id?
		      idalt = m_tileID->pmt_id(id,0);
		      //std::cout <<"ZH: AltID: V2: "<<id.getString()<<" / "<<idalt.getString()<<std::endl;
		    }
                }
	    }
	  m_identifier_alt->push_back(idalt.getString()); //hm, might be too many unnecessary strings...
	  //push things into vectors:
	  m_islarbarrel->push_back(larbarrel);
	  m_islarendcap->push_back(larendcap);
	  m_islarhec->push_back(larhec);
	  m_islarfcal->push_back(larfcal);
	  m_istile->push_back(tile);
	  m_sampling->push_back(sampling);
	  m_samplingfraction->push_back(sampfrac);

	  /*
	    //ok these seem to be tile gap cells
	  if (id.getString() == "0x4c8ecd3000000000" ||
	      id.getString() == "0x4c030f3000000000" ||
	      id.getString() == "0x4c090f3000000000" ||
	      id.getString() == "0x4c858f3000000000" ||
	      id.getString() == "0x4c8acd3000000000" ||
	      id.getString() == "0x4c8d0f3000000000" ||
	      id.getString() == "0x4c8ecd3000000000")
	    {
	      //id.show();
	      std::cout <<"ZH: Special tile: "<<id<<std::endl;
	      std::cout <<"ZH: Tile info: region: "<<m_tileID->region(id)<<" system: "<<m_tileID->system(id)<<"\
 section: "<<m_tileID->section(id)<<" side: "<<m_tileID->side(id)<<" module: "<<m_tileID->module(id)<<" tower: "<<m_tileID->tower(id)<<" sample: "<<m_tileID->sample(id)<<" pmt: "<<m_tileID->pmt(id)<<" adc: "<<m_tileID->adc(id)<<std::endl;
	    }
	  */
	  /*	
	  if (id.getString() =="0x2c6027b200000000"||
	      id.getString() =="0x2c60239a00000000"||
	      id.getString() =="0x2c443f9800000000"||
	      id.getString() =="0x2c2c0a5a00000000"||
	      id.getString() =="0x2c24003a00000000")
	    {
	      if (larbarrel || larendcap)
		{
		  std::cout <<"ZH: Special LAr: "<<id.getString()<<" "<<larbarrel <<" "<< larendcap<<std::endl;
		  std::cout <<"ZH: LAr info: barrel ec: "<<m_larEmID->barrel_ec(id)<<" sampling: "<<m_larEmID->sampling(id)<<" region: "<<m_larEmID->region(id)<<" eta: "<<m_larEmID->eta(id)<<" phi: "<<m_larEmID->phi(id)<<std::endl;
		}
	    }
	  */      
	}
    } //event steps

  //Get truth particle info
  const DataHandle<McEventCollection> mcEvent;
  sc = evtStore()->retrieve(mcEvent,"TruthEvent");
  if (sc.isFailure())
    {
      std::cout <<"Warning: ISF_HitAnalysis: No truth event!!"<<std::endl;
    }
  else
    {
      if (mcEvent)
	{
	  //std::cout<<"ISF_HitAnalysis: MC event size: "<<mcEvent->size()<<std::endl;
	  if (mcEvent->size())
	    {
	      //std::cout <<"ISF_HitAnalysis: MC first truth event size: "<<(*mcEvent->begin())->particles_size()<<std::endl;
	      for (HepMC::GenEvent::particle_const_iterator it = (*mcEvent->begin())->particles_begin(); it != (*mcEvent->begin())->particles_end(); ++it)
		{
		  m_truth_e->push_back((*it)->momentum().e());
		  m_truth_px->push_back((*it)->momentum().px());
		  m_truth_py->push_back((*it)->momentum().py());
		  m_truth_pz->push_back((*it)->momentum().pz());
		  m_truth_pdg->push_back((*it)->pdg_id());
		}
	    }
	}
    }//truth event
  
  //Get reco cells if available
  const CaloCellContainer *cellColl = 0;
  sc = evtStore()->retrieve(cellColl, "AllCalo");

  if (sc.isFailure())
    {
      std::cout <<"Warning: ISF_HitAnalysis: Couldn't read AllCalo cells from StoreGate"<<std::endl;
      //return NULL;
    }
  else
    {
      std::cout <<"ISF_HitAnalysis: Found: "<<cellColl->size()<<" calorimeter cells"<<std::endl;
      CaloCellContainer::const_iterator itrCell = cellColl->begin();
      CaloCellContainer::const_iterator itrLastCell = cellColl->end();      
      for ( ; itrCell!=itrLastCell; ++itrCell){
	m_cell_e->push_back((*itrCell)->energy());
	m_cell_identifier->push_back((*itrCell)->ID().getString());
	if (m_calo_dd_man->get_element((*itrCell)->ID()))
	  {
	    CaloCell_ID::CaloSample layer = m_calo_dd_man->get_element((*itrCell)->ID())->getSampling();
	    m_cell_sampling->push_back(layer);
	  }
	  else m_cell_sampling->push_back(-1);

      }
    }//calorimeter cells

  //Fill the tree and finish
  if (m_tree) m_tree->Fill();

  return StatusCode::SUCCESS;
}

