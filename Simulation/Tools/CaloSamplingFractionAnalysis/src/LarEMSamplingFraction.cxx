/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LarEMSamplingFraction.h"

#include "CaloSimEvent/CaloCalibrationHit.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "LArSimEvent/LArHitContainer.h"
#include "TileSimEvent/TileHitVector.h"

#include "GeoAdaptors/GeoLArHit.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/Property.h"

#include "GeneratorObjects/McEventCollection.h"

#include "TString.h"
#include <iterator>
#include <cmath>
#include <map>

using namespace std;

//###############################################################################
LarEMSamplingFraction::LarEMSamplingFraction(const std::string& name, ISvcLocator* pSvcLocator)  : AthAlgorithm(name, pSvcLocator),
    m_docells(true),
    m_calo_dd_man(0),
    m_calo_id(0)
{
  
  // Name of ClusterContainer to use
  declareProperty("DoCells",m_docells); 
  declareProperty("CalibrationHitContainerNames",m_CalibrationHitContainerNames); 
}

//###############################################################################

LarEMSamplingFraction::~LarEMSamplingFraction()
{ }

//###############################################################################

StatusCode LarEMSamplingFraction::initialize()
{
  //---- initialize the StoreGateSvc ptr ----------------
  
  ServiceHandle<ITHistSvc> histSvc("THistSvc",name()); 
  ATH_CHECK( histSvc.retrieve() );
	
	mytree = new TTree("mytree","mytree");
	mytree->Branch("energy_reco",          &energy_reco);
	mytree->Branch("energy_hit",           &energy_hit);
	mytree->Branch("energy_inactive_total",&energy_inactive_total);
	mytree->Branch("energy_inactive_em",   &energy_inactive_em);
	mytree->Branch("energy_inactive_nonem",&energy_inactive_nonem);
	mytree->Branch("energy_inactive_inv",  &energy_inactive_inv);
	mytree->Branch("energy_inactive_esc",  &energy_inactive_esc);
	mytree->Branch("energy_active_total_corrected",&energy_active_total_corrected);
	mytree->Branch("energy_active_total",&energy_active_total);
	mytree->Branch("energy_active_em",   &energy_active_em);
	mytree->Branch("energy_active_nonem",&energy_active_nonem);
	mytree->Branch("energy_active_inv",  &energy_active_inv);
	mytree->Branch("energy_active_esc",  &energy_active_esc);
	mytree->Branch("mc_pdg", &mc_pdg);
	mytree->Branch("mc_eta", &mc_eta);
	mytree->Branch("mc_phi", &mc_phi);
	mytree->Branch("mc_e",   &mc_e);
	mytree->Branch("mc_pt",  &mc_pt);

  if(m_docells) {
	  mytree->Branch("cell_identifier",&m_cell_identifier);
	  mytree->Branch("cell_energy_reco",&m_cell_energy_reco);
	  mytree->Branch("cell_energy_inactive_total",&m_cell_energy_inactive_total);
  	mytree->Branch("cell_energy_active_total_corrected",&m_cell_energy_active_total_corrected);
	  mytree->Branch("cell_energy_active_total",&m_cell_energy_active_total);
	  mytree->Branch("cell_sampling",&m_cell_sampling);
	  mytree->Branch("cell_eta",&m_cell_eta);
	  mytree->Branch("cell_phi",&m_cell_phi);
  }  

	histSvc->regTree("/MYSTREAM/myTree",mytree).ignore();
    
  // pointer to detector manager:
  m_calo_dd_man = CaloDetDescrManager::instance(); 
  m_calo_id = m_calo_dd_man->getCaloCell_ID();
  
  const DataHandle<CaloIdManager> caloIdManager;
  StatusCode sc=detStore()->retrieve(caloIdManager);
  if(sc.isSuccess())
   ATH_MSG_DEBUG("CaloIDManager retrieved.");
  else
   throw std::runtime_error("ISF_HitAnalysis: Unable to retrieve CaloIDManeger");

  m_tileID=caloIdManager->getTileID();
  if(m_tileID==0) throw std::runtime_error("ISF_HitAnalysis: Invalid Tile ID helper");

  sc=detStore()->regHandle(m_dd_fSampl,"LArfSampl");
  if(sc.isFailure())
  {
    ATH_MSG_ERROR("Unable to register handle for LArfSampl");
    return StatusCode::FAILURE;
  }

  detStore()->retrieve(m_tileInfo,"TileInfo");
  if(sc.isFailure())
  {
    ATH_MSG_ERROR("Unable to retrieve TileInfo from DetectorStore");
    return StatusCode::FAILURE;
  }
  

  return StatusCode::SUCCESS;
}

//###############################################################################

StatusCode LarEMSamplingFraction::finalize()
{

  return StatusCode::SUCCESS;
}

//###############################################################################

StatusCode LarEMSamplingFraction::execute()
{
  const DataHandle<CaloCalibrationHitContainer> cchc;
  std::vector<const CaloCalibrationHitContainer *> v_cchc;
  std::vector<std::string>::iterator iter;
  for (iter=m_CalibrationHitContainerNames.begin();iter!=m_CalibrationHitContainerNames.end();iter++)
  {
    if ( !evtStore()->contains<CaloCalibrationHitContainer>(*iter))
    {
      msg(MSG::ERROR) << "SG does not contain calibration hit container " << *iter << endmsg;
      return StatusCode::FAILURE;
    }
    else
    {
      StatusCode sc = evtStore()->retrieve(cchc,*iter);
      if (sc.isFailure() )
      {
				msg(MSG::ERROR) << "Cannot retrieve calibration hit container " << *iter << endmsg;
				return sc;
      } 
      else
      {
				v_cchc.push_back(cchc);
			}
    }
  }
	
	const McEventCollection* truthEvent=0;
	StatusCode sc = evtStore()->retrieve(truthEvent, "TruthEvent");
	if (sc.isFailure()||!truthEvent)
	{
		msg(MSG::ERROR) << "No McEventCollection found"<< endmsg;
		return StatusCode::FAILURE;
	}
	HepMC::GenEvent::particle_const_iterator pit  = truthEvent->at(0)->particles_begin();
	const HepMC::GenParticle * gen  = *pit;
	mc_pdg = gen->pdg_id();
	mc_eta = gen->momentum().pseudoRapidity();
	mc_phi = gen->momentum().phi();
	mc_e = gen->momentum().e();
	mc_pt = sqrt(pow(gen->momentum().px(),2)+pow(gen->momentum().py(),2));
	
	//inspiration:
	//see https://gitlab.cern.ch/atlas/athena/blob/master/Calorimeter/CaloCalibHitRec/src/CalibHitToCaloCell.cxx
	//and https://gitlab.cern.ch/atlas/athena/blob/master/Calorimeter/CaloCalibHitRec/src/CaloDmEnergy.cxx
	
	const CaloDetDescrManager* caloDDMgr = nullptr;
	ATH_CHECK(detStore()->retrieve(caloDDMgr, "CaloMgr"));
	
	energy_reco          =new vector<float>;
	energy_hit          =new vector<float>;
  
 	energy_inactive_total=new vector<float>;
	energy_inactive_em   =new vector<float>;
	energy_inactive_nonem=new vector<float>;
	energy_inactive_inv  =new vector<float>;
	energy_inactive_esc  =new vector<float>;
	
	energy_active_total_corrected=new vector<float>;
	energy_active_total=new vector<float>;
	energy_active_em   =new vector<float>;
	energy_active_nonem=new vector<float>;
	energy_active_inv  =new vector<float>;
	energy_active_esc  =new vector<float>;
	
  if(m_docells) {
    m_cell_identifier                   =new std::vector<Long64_t>;
    m_cell_energy_reco                  =new std::vector<float>;
    m_cell_energy_active_total_corrected=new std::vector<float>;
    m_cell_energy_active_total          =new std::vector<float>;
    m_cell_energy_inactive_total        =new std::vector<float>;
    m_cell_sampling                     =new std::vector<int>;
    m_cell_eta                          =new std::vector<float>;
    m_cell_phi                          =new std::vector<float>;
  } 

  struct cell_info {
    Long64_t cell_identifier=0;
    float    cell_energy_reco=0;
    float    cell_energy_active_total_corrected=0;
    float    cell_energy_active_total=0;
    float    cell_energy_inactive_total=0;
    int      cell_sampling=0;
    float    cell_eta=0;
    float    cell_phi=0;
  };
  
  std::map< Long64_t , cell_info > cell_info_map;

	for(int s=0;s<24;s++)
	{
		energy_reco->push_back(0.0);
		energy_hit->push_back(0.0);
    
		energy_inactive_total->push_back(0.0);
		energy_inactive_em   ->push_back(0.0);
		energy_inactive_nonem->push_back(0.0);
		energy_inactive_inv  ->push_back(0.0);
		energy_inactive_esc  ->push_back(0.0);
		
		energy_active_total_corrected->push_back(0.0);
		energy_active_total->push_back(0.0);
		energy_active_em   ->push_back(0.0);
		energy_active_nonem->push_back(0.0);
		energy_active_inv  ->push_back(0.0);
		energy_active_esc  ->push_back(0.0);
	}
	
	std::vector<const CaloCalibrationHitContainer * >::const_iterator it;
	int count=0;
  for (it=v_cchc.begin();it!=v_cchc.end();it++)
  {
		ATH_MSG_DEBUG( "loop on "<<m_CalibrationHitContainerNames[count]);
		
		CaloCalibrationHitContainer::const_iterator chIter  = (*it)->begin();
		CaloCalibrationHitContainer::const_iterator chIterE = (*it)->end();
		
		for(;chIter!=chIterE;chIter++)
		{
			Identifier id=(*chIter)->cellID();
			
			double Etot   = (*chIter)->energyTotal();
			double Eem    = (*chIter)->energyEM();
			double Enonem = (*chIter)->energyNonEM();
			double Einv   = (*chIter)->energyInvisible();
			double Eesc   = (*chIter)->energyEscaped();
 
      double Efactor=1.0;
		
			const CaloDetDescrElement* caloDDE = caloDDMgr->get_element(id);
			int sampling=-1;
			if(caloDDE) {
				sampling = caloDDE->getSampling();

  	    if((sampling>=0 && sampling<=11) || (sampling>=21 && sampling<=23)) Efactor=1/m_dd_fSampl->FSAMPL(id);
  	    if((sampling>=12 && sampling<=20)) {
          Identifier cell_id = m_tileID->cell_id(id);
          if(m_calo_dd_man->get_element(cell_id)) {
            Efactor = m_tileInfo->HitCalib(cell_id);
            id=cell_id;
          } else {
            Efactor = m_tileInfo->HitCalib(id);
          }  
        }  
      }  
			
			ATH_MSG_VERBOSE( "cellID "<<id<<" layer "<<sampling<<" energyTotal "<<Etot<<" Eem "<<Eem<<" Enonem "<<Enonem<<" Einv "<<Einv<<" Eesc "<<Eesc<<" Efactor="<<Efactor);
			
			if(sampling>=0 && sampling<=23)
			{
        if(m_docells) {
          cell_info_map[id.get_compact()].cell_identifier=id.get_compact();
          cell_info_map[id.get_compact()].cell_sampling=sampling;
          cell_info_map[id.get_compact()].cell_eta=caloDDE->eta_raw();
          cell_info_map[id.get_compact()].cell_phi=caloDDE->phi_raw();
        }  

				if(m_CalibrationHitContainerNames[count]=="LArCalibrationHitInactive" || m_CalibrationHitContainerNames[count]=="TileCalibHitInactiveCell")
				{
					energy_inactive_total->at(sampling)+=Etot;
					energy_inactive_em   ->at(sampling)+=Eem;
					energy_inactive_nonem->at(sampling)+=Enonem;
					energy_inactive_inv  ->at(sampling)+=Einv;
					energy_inactive_esc  ->at(sampling)+=Eesc;

          if(m_docells) cell_info_map[id.get_compact()].cell_energy_inactive_total+=Etot;
				}
				
				if(m_CalibrationHitContainerNames[count]=="LArCalibrationHitActive" || m_CalibrationHitContainerNames[count]=="TileCalibHitActiveCell")
				{
					energy_active_total_corrected->at(sampling)+=Etot*Efactor;
					energy_active_total->at(sampling)+=Etot;
					energy_active_em   ->at(sampling)+=Eem;
					energy_active_nonem->at(sampling)+=Enonem;
					energy_active_inv  ->at(sampling)+=Einv;
					energy_active_esc  ->at(sampling)+=Eesc;

          if(m_docells) {
            cell_info_map[id.get_compact()].cell_energy_active_total_corrected+=Etot*Efactor;
            cell_info_map[id.get_compact()].cell_energy_active_total+=Etot;
          }  
				}
				
			}
			
		}
    
    count++;
	}

  //Get reco cells if available
  const CaloCellContainer *cellColl = 0;
  sc = evtStore()->retrieve(cellColl, "AllCalo");

  if (sc.isFailure()) {
    ATH_MSG_WARNING( "Couldn't read AllCalo cells from StoreGate");
    //return NULL;
  } else {
    ATH_MSG_DEBUG( "Found: "<<cellColl->size()<<" calorimeter cells");
    CaloCellContainer::const_iterator itrCell = cellColl->begin();
    CaloCellContainer::const_iterator itrLastCell = cellColl->end();
    for ( ; itrCell!=itrLastCell; ++itrCell) {
			Identifier id=(*itrCell)->ID();
      const CaloDetDescrElement* caloDDE = caloDDMgr->get_element(id);
			int sampling=-1;
			if(caloDDE) {
				sampling = caloDDE->getSampling();
        energy_reco->at(sampling)+=(*itrCell)->energy();
  	    if((sampling>=12 && sampling<=20)) {
          Identifier cell_id = m_tileID->cell_id(id);
          if(m_calo_dd_man->get_element(cell_id)) {
            id=cell_id;
          }  
        }  
      } 
      if(m_docells) {
        cell_info_map[id.get_compact()].cell_identifier=id.get_compact();
        cell_info_map[id.get_compact()].cell_sampling=sampling;
        cell_info_map[id.get_compact()].cell_eta=caloDDE->eta_raw();
        cell_info_map[id.get_compact()].cell_phi=caloDDE->phi_raw();
        cell_info_map[id.get_compact()].cell_energy_reco+=(*itrCell)->energy();
      }  
    }
  } //calorimeter cells


 //Get all G4Hits (from CaloHitAnalysis)
 std::string  lArKey [4] = {"LArHitEMB", "LArHitEMEC", "LArHitFCAL", "LArHitHEC"};
 for (unsigned int i=0;i<4;i++)
 {
  const DataHandle<LArHitContainer> iter;
  ATH_MSG_DEBUG( "Checking G4Hits: "<<lArKey[i]);
  if(evtStore()->retrieve(iter,lArKey[i])==StatusCode::SUCCESS)
  {
   LArHitContainer::const_iterator hi;
   int hitnumber = 0;
   for (hi=(*iter).begin();hi!=(*iter).end();hi++)
   {
    hitnumber++;
    GeoLArHit ghit(**hi);
    if (!ghit) continue;
    const CaloDetDescrElement *hitElement = ghit.getDetDescrElement();
    if(!hitElement) continue;
    Identifier larhitid = hitElement->identify();
    if(m_calo_dd_man->get_element(larhitid))
    {
     CaloCell_ID::CaloSample larlayer = m_calo_dd_man->get_element(larhitid)->getSampling();
     energy_hit->at(larlayer)+=ghit.Energy();
    }
   } // End while LAr hits
   ATH_MSG_DEBUG( "Read "<<hitnumber<<" G4Hits from "<<lArKey[i]);
  }
  else
  {
   ATH_MSG_INFO( "Can't retrieve LAr hits");
  }// End statuscode success upon retrieval of hits
 }// End detector type loop

 const TileHitVector * hitVec;
 if (evtStore()->retrieve(hitVec,"TileHitVec")==StatusCode::SUCCESS &&  m_tileID )
 {
  int hitnumber = 0;
  for(TileHitVecConstIterator i_hit=hitVec->begin() ; i_hit!=hitVec->end() ; ++i_hit)
  {
   hitnumber++;
   Identifier pmt_id = (*i_hit).identify();
   Identifier cell_id = m_tileID->cell_id(pmt_id);

   if (m_calo_dd_man->get_element(cell_id))
   {
    CaloCell_ID::CaloSample layer = m_calo_dd_man->get_element(cell_id)->getSampling();

    //could there be more subhits??
    for (int tilesubhit_i = 0; tilesubhit_i<(*i_hit).size(); tilesubhit_i++)
    {
     //!!
     ATH_MSG_DEBUG( "Tile subhit: "<<tilesubhit_i<<"/"<<(*i_hit).size()<< " E: "<<(*i_hit).energy(tilesubhit_i) );
     energy_hit->at(layer) += (*i_hit).energy(tilesubhit_i);
    }
   }
  }
  ATH_MSG_DEBUG( "Read "<<hitnumber<<" G4Hits from TileHitVec");
 }

	for(auto& cell:cell_info_map) {
    m_cell_identifier                   ->push_back(cell.second.cell_identifier);
    m_cell_sampling                     ->push_back(cell.second.cell_sampling);
    m_cell_eta                          ->push_back(cell.second.cell_eta);
    m_cell_phi                          ->push_back(cell.second.cell_phi);
    m_cell_energy_reco                  ->push_back(cell.second.cell_energy_reco);
    m_cell_energy_active_total_corrected->push_back(cell.second.cell_energy_active_total_corrected);
    m_cell_energy_active_total          ->push_back(cell.second.cell_energy_active_total);
	  m_cell_energy_inactive_total        ->push_back(cell.second.cell_energy_inactive_total);
	}
	
	mytree->Fill();
	
	delete energy_reco;
	delete energy_hit;
	delete energy_inactive_total;
	delete energy_inactive_em;
	delete energy_inactive_nonem;
	delete energy_inactive_inv;
	delete energy_inactive_esc;

	delete energy_active_total_corrected;
	delete energy_active_total;
	delete energy_active_em;
	delete energy_active_nonem;
	delete energy_active_inv;
	delete energy_active_esc;
	  
  if(m_docells) {
    delete m_cell_identifier;
    delete m_cell_energy_reco;
    delete m_cell_energy_active_total_corrected;
    delete m_cell_energy_active_total;
    delete m_cell_energy_inactive_total;
    delete m_cell_sampling;
    delete m_cell_eta;
    delete m_cell_phi;
  }  

  return StatusCode::SUCCESS;
}
