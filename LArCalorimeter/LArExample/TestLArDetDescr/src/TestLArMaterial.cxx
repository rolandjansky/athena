/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// INCLUDE HEADER FILES:
#include "TestLArDetDescr/TestLArMaterial.h"

// Athena related 
#include "GaudiKernel/Property.h"
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
#include "CaloDetDescr/ICaloRecoMaterialTool.h"

#include "CaloTrackingGeometry/ICaloSurfaceBuilder.h"

#include "boost/io/ios_state.hpp"


// -------------------------------------------------------------
// Constructor 
// -------------------------------------------------------------
TestLArMaterial::TestLArMaterial(const std::string& name, 
				   ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator),
  m_calo_dd_man(0),
  m_surfbuild(0),
  m_lar_names(0),
  m_lar_mat(0)
{}

// DESTRUCTOR:
TestLArMaterial::~TestLArMaterial()
{  }

// INITIALIZE:
StatusCode TestLArMaterial::initialize()
{
  m_calo_dd_man = CaloDetDescrManager::instance();
  return StatusCode::SUCCESS;
}

// FINALIZE:
StatusCode TestLArMaterial::finalize()
{
  return StatusCode::SUCCESS;
}

// EXECUTE:
StatusCode TestLArMaterial::execute()
{  
  ATH_MSG_INFO ( "Executing TestLArMaterial" );
  
  // This little class handles the phi range cheching (-pi,pi)
  CaloPhiRange toto;  
  toto.print(); 
  m_lar_names = new CaloSubdetNames();

  // General access to Tools :
  IToolSvc* p_toolSvc = nullptr;
  ATH_CHECK( service("ToolSvc", p_toolSvc) );

  // This tool handles the conversion between local and ctb coordinates
  IAlgTool* tool = nullptr;
  ATH_CHECK( p_toolSvc->retrieveTool("LArRecoMaterialTool",tool) );
  m_lar_mat = dynamic_cast<ICaloRecoMaterialTool*>(tool);
  ATH_MSG_INFO ( "Found LArRecoMaterialTool tool");

  // For checks : the surface builder 
  StatusCode sc = toolSvc()->retrieveTool("ICaloSurfaceBuilder",m_surfbuild);
  if (sc.isFailure())
    ATH_MSG_WARNING("Could not find Tool CaloSurfaceBuilder" );
  else {
    m_surfbuild->setCaloDepth(0);
  }

  //print_elt( true, false, false, false);
  print_Layers();

  return StatusCode::SUCCESS;
}

void
TestLArMaterial::print_elt(bool em, bool hec, bool fcal, bool tile)
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
	//double x =  newelt->x();
	//double y =  newelt->y();
	//double z =  newelt->z();
     	std::cout  << std::setw(9) << std::setprecision(4) 
		   << "calo-hash=" << i << " region: "
		   << help_all->sampling(id)<< " " 
		   << help_all->region(id)<< " "
		   << " ieta= " <<  help_all->eta(id)
		   << " " << newelt->eta()
		   << " phi= " << newelt->phi()
		   << " r= " << newelt->r()
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
TestLArMaterial::print_Layers()
{
  
  Amg::Transform3D htrans;
  double hphi;
  std::vector<double> radius;  
  std::vector<double> depth;
  std::vector<double> hlength;
  std::vector<double> z;  
  std::vector<double> rmin;
  std::vector<double> rmax;
  bool result;

  for ( unsigned int i=0; i<CaloSubdetNames::UNKNOWN; i++) {

    CaloSubdetNames::ALIGNVOL alvol = (CaloSubdetNames::ALIGNVOL) i;
  
    std::cout << "  " << std::endl;
    std::cout << "Vol " << i << " is " << m_lar_names->alignVolName(alvol) 
	      <<" and is made of ";

    result = m_surfbuild->get_cylinder_surface (alvol,&htrans,hphi,radius,depth,hlength );
    if (result) {
      std::cout << radius.size() << " cylindrical layers " << std::endl;
      for ( unsigned int i = 0; i<radius.size(); i++ ) {
	std::cout << "layer number " << i << std::endl;
	std::cout << " hphi= " << hphi << " radius= " << radius[i] 
		  << " depth= " << depth[i] << " hlength= " << hlength[i] << std::endl;
      }  
    }
    else {

      result = m_surfbuild->get_disk_surface (alvol,&htrans,hphi,z,depth,rmin,rmax );
      if (result) {
	std::cout << z.size() << " disk layers " << std::endl;
	for ( unsigned int i = 0; i<z.size(); i++ ) {
	  std::cout << "layer number " << i << std::endl;
	  std::cout << " hphi= " << hphi << " z= " << z[i] 
		    << " depth= " << depth[i] 
		    << " rmin= " << rmin[i] 
		    << " rmax= " << rmax[i] 
		    << std::endl;
	}  
      }
    }
    
  }
  
  std::cout << " thats it folks !  " << std::endl;
}

