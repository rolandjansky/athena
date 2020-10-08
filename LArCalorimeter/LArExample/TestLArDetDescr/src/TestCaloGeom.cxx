/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// INCLUDE HEADER FILES:
#include "TestLArDetDescr/TestCaloGeom.h"

// Athena related 
#include "GaudiKernel/MsgStream.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IToolSvc.h"
#include <algorithm>
#include <cmath>
#include "StoreGate/StoreGateSvc.h"

#include "CLHEP/Units/SystemOfUnits.h"

// specific :
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloDetDescr/ICaloCoordinateTool.h"

#include "boost/io/ios_state.hpp"

// -------------------------------------------------------------
// Constructor 
// -------------------------------------------------------------
TestCaloGeom::TestCaloGeom(const std::string& name, 
				   ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator),
  m_calo_dd_man(0),
  m_coord("TBCaloCoordinate")
{}

// DESTRUCTOR:
TestCaloGeom::~TestCaloGeom()
{  }

// INITIALIZE:
StatusCode TestCaloGeom::initialize()
{
  m_calo_dd_man = CaloDetDescrManager::instance();
  ATH_CHECK( m_coord.retrieve() );
  return StatusCode::SUCCESS;
}

// FINALIZE:
StatusCode TestCaloGeom::finalize()
{
  return StatusCode::SUCCESS;
}

// EXECUTE:
StatusCode TestCaloGeom::execute()
{  
  ATH_MSG_INFO ( "Executing TestCaloGeom" );
  
  // This little class handles the phi range cheching (-pi,pi)
  CaloPhiRange toto;  
  toto.print(); 

  print_beam();

  return StatusCode::SUCCESS;
}

void
TestCaloGeom::print_elt(bool em, bool hec, bool fcal, bool tile)
{
  boost::io::ios_base_all_saver coutsave (std::cout);

  ATH_MSG_INFO ( " printing CaloDDE characteristics " );

  IdentifierHash idcalohash,hash_min,hash_max ;
  Identifier id;

  const CaloCell_ID* help_all = m_calo_dd_man->getCaloCell_ID();

  //
  // Now loop on CaloCell:
  //

  int phi_num = 1;

  help_all->calo_cell_hash_range( (int) CaloCell_ID::LAREM ,
				  hash_min, hash_max);

  std::cout << " ---- EM hash range is " << hash_min << " to " 
	    << hash_max << std::endl;

  for ( unsigned int i = (unsigned int) hash_min ; 
	i < (unsigned int) hash_max ; i++ )
    {
      idcalohash = (IdentifierHash) i;
      id = help_all->cell_id(idcalohash);

      const CaloDetDescrElement* newelt = m_calo_dd_man->get_element(idcalohash); 
      if ( !newelt ) std::cout << "missing em element" 
			       << i << std::endl;
      
      else if ( em && help_all->phi(id) == phi_num && newelt->eta() >=0) { 
	double x_local =  newelt->x();
	double y_local =  newelt->y();
	double z_local =  newelt->z();
	double x,y,z;
     	std::cout  << std::setw(9) << std::setprecision(4) 
		   << "calo-hash=" << i << " region: "
		   << help_all->sampling(id)<< " " 
		   << help_all->region(id)<< " "
		   << " ieta= " <<  help_all->eta(id)
		   << " " << newelt->eta()
		   << " phi= " << newelt->phi()
		   << " r= " << newelt->r()
		   << std::endl;
	
	m_coord->local_to_ctb(x_local,y_local,z_local,x,y,z);
	double r = sqrt ( x*x+y*y);
	double bigr = sqrt ( x*x+y*y+z*z);
	double eta_ctb =  -std::log((bigr-z)/r);
     	std::cout  << std::setw(9) << std::setprecision(4) 
		   << "new cell eta_ctb=" << eta_ctb
		   << std::endl;
      }
    }


  help_all->calo_cell_hash_range( (int) CaloCell_ID::LARHEC ,
				  hash_min, hash_max);

  std::cout << " ---- HEC hash range is " << hash_min << " to " 
	    << hash_max << std::endl;

  for ( unsigned int i = (unsigned int) hash_min ; 
	i < (unsigned int) hash_max ; i++ )
    {
      idcalohash = (IdentifierHash) i;
      id = help_all->cell_id(idcalohash);

      const CaloDetDescrElement* newelt = m_calo_dd_man->get_element(idcalohash);
      if ( !newelt ) 
	std::cout << "missing hec element" 
		  << i << std::endl;
      
      else if ( hec && help_all->phi(id) == phi_num && newelt->eta() >=0) { 
	  std::cout  << std::setw(9) << std::setprecision(4) 
		  << "calo-hash=" << i << " region: "
		    << help_all->sampling(id)<< " " 
		    << help_all->region(id)<< " "
		    << " ieta= " <<  help_all->eta(id)
		    << " elt: " << newelt->eta()
		    << " phi= " << newelt->phi()
		  << " r= " << newelt->r()
		    << std::endl;
	}
    }

  help_all->calo_cell_hash_range( (int) CaloCell_ID::LARFCAL ,
				  hash_min, hash_max);

  std::cout << " ---- FCAL hash range is " << hash_min << " to " 
	    << hash_max << std::endl;

  for ( unsigned int i = (unsigned int) hash_min ; 
	i < (unsigned int) hash_max ; i++ )
    {
      idcalohash = (IdentifierHash) i;
      id = help_all->cell_id(idcalohash);

      const CaloDetDescrElement* newelt = m_calo_dd_man->get_element(idcalohash); 
      if ( !newelt ) std::cout << "missing fcal element" 
			       << i << std::endl;

      else if (fcal && help_all->phi(id) == phi_num && 
	       help_all->pos_neg(id) >=0) { 

	std::cout  << std::setw(9) << std::setprecision(4) 
		   << "calo-hash=" << i << " region: "
		  << help_all-> module(id)<< " " 
		  << " ieta= " <<  help_all->eta(id)
		  << " " << newelt->eta()
		  << " phi= " << newelt->phi()
		  << " x,y,z " << newelt->x()<< " " <<  newelt->y()
		  << " " <<  newelt->z()
		  << std::endl;

      }

    }

  if (tile) {

  help_all->calo_cell_hash_range( (int) CaloCell_ID::TILE ,
				  hash_min, hash_max);

  std::cout << " ---- Tile hash range is " << hash_min << " to " 
	    << hash_max << std::endl;

  for ( unsigned int i = (unsigned int) hash_min ; 
	i < (unsigned int) hash_max ; i++ )
    {
      idcalohash = (IdentifierHash) i;
      id = help_all->cell_id(idcalohash);

      const CaloDetDescrElement* newelt = m_calo_dd_man->get_element(idcalohash); 
      if ( !newelt ) std::cout << "missing Tile element" 
			       << i << std::endl;

      else if (tile && help_all->phi(id) == phi_num && 
	       help_all->pos_neg(id) >=0) { 

	std::cout  << std::setw(9) << std::setprecision(4) 
		   << "calo-hash=" << i 
		   << " ieta= " <<  help_all->eta(id)
		   << " " << newelt->eta()
		   << " phi= " << newelt->phi()
		   << " x,y,z " << newelt->x()<< " " <<  newelt->y()
		   << " " <<  newelt->z()
		   << std::endl;

      }
    }
  }
}

void
TestCaloGeom::print_beam()
{
  ATH_MSG_INFO ( "Executing TestCaloGeom : print_beam " );

  double eta= m_coord->beam_local_eta();
  double phi= m_coord->beam_local_phi();

  std::cout  << " " << std::endl;
  std::cout  << " " << std::endl;
  std::cout  << " The H8 beam calo-local coodinates are : eta=" << eta 
	     << " phi=" << phi << std::endl;
  std::cout  << " " << std::endl;
  std::cout  << " " << std::endl;

}
