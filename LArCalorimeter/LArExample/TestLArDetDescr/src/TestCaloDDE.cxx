/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// INCLUDE HEADER FILES:
#include "TestLArDetDescr/TestCaloDDE.h"

// Athena related 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include <algorithm>
#include "StoreGate/StoreGateSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Geometry/Transform3D.h"

// specific :
#include "CaloIdentifier/LArID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "LArIdentifier/LArIdManager.h"

#include "CaloDetDescr/CaloSubdetNames.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloDetDescr/ICaloRecoMaterialTool.h"
#include "CaloDetDescr/ICaloRecoSimpleGeomTool.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include <cmath>

#include "boost/io/ios_state.hpp"

using HepGeom::Transform3D;
using HepGeom::RotateZ3D;

// -------------------------------------------------------------
// Constructor 
// -------------------------------------------------------------
TestCaloDDE::TestCaloDDE(const std::string& name, 
				   ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator),
  m_calo_id_man(0),
  m_calo_dd_man(0),
  m_lar_id_man(0),
  m_lar_mat(0),
  m_lar_simplegeom(0)
{}

// DESTRUCTOR:
TestCaloDDE::~TestCaloDDE()
{  }

// INITIALIZE:
StatusCode TestCaloDDE::initialize()
{
  ATH_CHECK( detStore()->retrieve(m_calo_id_man) );
  ATH_MSG_DEBUG ( "Successfully retrieved CaloIdManager from DetectorStore" );
  
  ATH_CHECK( detStore()->retrieve(m_lar_id_man) );
  ATH_MSG_DEBUG ( "Successfully retrieved LArIdManager from DetectorStore" );

  ATH_CHECK( detStore()->retrieve(m_calo_dd_man) );
  ATH_MSG_DEBUG ( "Successfully retrieved CaloDetDescrManager from DetectorStore" );

  return StatusCode::SUCCESS;
}

// FINALIZE:
StatusCode TestCaloDDE::finalize()
{
  return StatusCode::SUCCESS;
}

// EXECUTE:
StatusCode TestCaloDDE::execute()
{  
  ATH_MSG_INFO ( "Executing TestCaloDDE with geometry : " 
                 << m_calo_dd_man->lar_geometry());

  CaloPhiRange range;
  range.print(); 

  // Print Regions and/or CaloDDE :

  //update();
  //if(m_lar_simplegeom) m_lar_simplegeom->print();

  print_eta_line( 1, true, true, true);
  //print_phi_line( 56, true, false, false);

  //print_edges_via_SubCalo();
  //print_edges_via_CaloSample();

  //  try_each_descr_zone();
  //  try_zone();

  //print_elt_HW( true, true, true);

  /*
  CaloCell_ID::CaloSample sample = CaloCell_ID::PreSamplerB;
  print_subcalo( sample );

  sample = CaloCell_ID::EMB1;
  print_subcalo( sample );
  sample = CaloCell_ID::EMB2;
  print_subcalo( sample );
  sample = CaloCell_ID::EMB3;
  print_subcalo( sample );
  */

  //read_volumes();

  /*
  float min =  (float)toto.phi_min();
  float max =  (float)toto.phi_max();

  double phi =  toto.twopi() +  .5;
  float phif =  (float) toto.fix(phi);
  std::cout << " 2*pi+.5   fix :" << toto.fix(phi) 
	    << " modulo min =" << std::fmod( phif,min)
	    << " modulo max =" << std::fmod( phif,max)
	    << std::endl;
  
  phi = toto.twopi() - .5;
  phif =  (float)  toto.fix(phi);
  std::cout << " 2*pi-.5   fix :" << toto.fix(phi) 
	    << " modulo min= " << std::fmod( phif,min)
	    << " modulo max= " << std::fmod( phif,max)
	    << std::endl;
  
  phi = toto.twopi()/2. + .5;
  phif =  (float)  toto.fix(phi);
  std::cout << " pi+.5   fix :" << toto.fix(phi) 
	    << " modulo min= " << std::fmod( phif,min)
	    << " modulo max= " << std::fmod( phif,max)
	    << std::endl;

  phi = toto.twopi()/2.- .5;
  phif =  (float)  toto.fix(phi);
  std::cout << " pi-.5   fix :" << toto.fix(phi) 
	    << " modulo min= " << std::fmod( phif,min)
	    << " modulo max= " << std::fmod( phif,max)
	    << std::endl;

  */

  return StatusCode::SUCCESS;
}

void
TestCaloDDE::print_subcalo( CaloCell_ID::CaloSample sample )
{
  ATH_MSG_INFO ( " printing CaloDDE characteristics " );

  CaloCell_ID::SUBCALO subcalo;
  bool barrel;
  int sampling_or_module;

  double eta = 0.437500;
  double phi = 0.;

  m_calo_dd_man->decode_sample (subcalo, barrel, sampling_or_module, sample);

  std::cout << " CaloCell_ID::CaloSample " << sample << " eta " << eta << " phi " << phi << std::endl;

  const CaloDetDescrElement* dde =
   m_calo_dd_man->get_element(subcalo,sampling_or_module,barrel,eta,phi);

  std::cout << " = subcalo " << subcalo << " barrel " << barrel 
	    << " sampling_or_module " << sampling_or_module
	    << std::endl;

  if (!dde) 
    std::cout << " no dde ??? " << std::endl;
  else
    std::cout << " deta= " << dde->deta() << std::endl;

}

void
TestCaloDDE::print_eta_line(int phi_num, bool em, bool hec, bool fcal)
{
  boost::io::ios_base_all_saver coutsave (std::cout);

  ATH_MSG_INFO ( " printing CaloDDE eta line " );

  IdentifierHash idhash;
  IdentifierHash idcalohash;
  Identifier id;
  int sub_calo_num;

  //retrieves helpers 
  const LArEM_ID*    help_em = m_calo_id_man->getEM_ID();
  const LArHEC_ID*   help_hec = m_calo_id_man->getHEC_ID();
  const LArFCAL_ID*  help_fcal = m_calo_id_man->getFCAL_ID();
  const CaloCell_ID* help_all = m_calo_id_man->getCaloCell_ID();

  //
  // Now loop :
  //

  sub_calo_num = (int) CaloCell_ID::LAREM;

  std::cout << " ---- EM ---- num=" << sub_calo_num 
	    << " hash_max=" << help_em->channel_hash_max() 
	    << std::endl;

  for ( unsigned int i = 0 ; i < help_em->channel_hash_max(); i++ )
    {
      idhash = (IdentifierHash) i;
      idcalohash = help_all->calo_cell_hash (sub_calo_num, idhash);
      id = help_em->channel_id(idhash);

      const CaloDetDescrElement* newelt = m_calo_dd_man->get_element(idcalohash); 
      if ( !newelt ) std::cout << "missing em element" 
			       << i << std::endl;
      
      //else if ( em && help_em->phi(id) == phi_num && newelt->eta() >=0) { 
      else if ( em && help_em->phi(id) == phi_num ) { 
     	std::cout  << std::setw(9) << std::setprecision(4) 
		   << "calo-hash=" << (unsigned int) idcalohash 
		   << " sub-hash=" << i << " region: "
		   << help_em->sampling(id)<< " " 
		   << help_em->region(id)<< " "
		   << " ieta= " <<  help_em->eta(id)
		   << " " << newelt->eta()
		   << " phi= " << newelt->phi()
		   << " r= " << newelt->r() 
	  //<< " volume= " << newelt->volume()
		   << " deta= " << newelt->deta()
		   << " grann= " << newelt->descriptor()->deta()
		   << " sampling= " << (int) newelt->getSampling()
		   << " layer= " << newelt->getLayer()
		   << std::endl;

		  }
      
    }


  sub_calo_num = (int) CaloCell_ID::LARHEC;
  std::cout << " ---- HEC ---- num=" << sub_calo_num
	    << " hash_max=" << help_hec->channel_hash_max() 
	    << std::endl;

  for ( unsigned int i = 0 ; i < help_hec->channel_hash_max(); i++ )
    {
      idhash = (IdentifierHash) i;
      idcalohash = help_all->calo_cell_hash (sub_calo_num, idhash);
      
      id = help_hec->channel_id(idhash);
      
      const CaloDetDescrElement* newelt = m_calo_dd_man->get_element(idcalohash); 
      if ( !newelt ) 
	std::cout << "missing hec element" 
		  << idhash << std::endl;
      
      else if ( hec && help_hec->phi(id) == phi_num && newelt->eta() >=0) { 
	
	std::cout  << std::setw(9) << std::setprecision(4) 
		   << "calo-hash=" << (unsigned int) idcalohash 
		   << " sub-hash=" << i << " region: "
		   << help_hec->sampling(id)<< " " 
		   << help_hec->region(id)<< " "
		   << " ieta= " <<  help_hec->eta(id)
		   << " elt: " << newelt->eta()
		   << " phi= " << newelt->phi()
		   << " r= " << newelt->r()
		   << " volume= " << newelt->volume()
		   << std::endl;
      }
    }
  
  
  sub_calo_num = (int) CaloCell_ID::LARFCAL;
  std::cout << " ---- FCAL ---- num=" << sub_calo_num
	    << " hash_max=" << help_fcal->channel_hash_max() 
	     << std::endl;

  for ( unsigned int i = 0 ; i < help_fcal->channel_hash_max();	i++ )
    {
      idhash = (IdentifierHash) i;
      idcalohash = help_all->calo_cell_hash (sub_calo_num, idhash);
      id = help_fcal->channel_id(idhash);

      const CaloDetDescrElement* newelt = m_calo_dd_man->get_element(idcalohash); 
      if ( !newelt ) std::cout << "missing fcal element" 
			       << i << std::endl;

      else if (fcal && help_fcal->phi(id) == phi_num && help_fcal->pos_neg(id) >=0) { 

	
     	std::cout  << std::setw(9) << std::setprecision(4) 
		   << "calo-hash=" << (unsigned int) idcalohash 
		  << " sub-hash=" << i << " region: "
		  << help_fcal-> module(id)<< " " 
		  << " ieta= " <<  help_fcal->eta(id)<< " layer= " << newelt->getLayer()
		  << " " << newelt->eta()
		  << " phi= " << newelt->phi()
		  << " x,y,z " << newelt->x()<< " " <<  newelt->y()
		  << " " <<  newelt->z()
		  << " volume= " << newelt->volume()
		  << std::endl;

      }

    }
}

void
TestCaloDDE::print_phi_line(int eta_num, bool em, bool hec, bool fcal)
{
  boost::io::ios_base_all_saver coutsave (std::cout);

  ATH_MSG_INFO ( " printing CaloDDE phi line " );

  IdentifierHash idhash;
  IdentifierHash idcalohash;
  Identifier id;
  int sub_calo_num;

  //retrieves helpers 
  const LArEM_ID*    help_em = m_calo_id_man->getEM_ID();
  const LArHEC_ID*   help_hec = m_calo_id_man->getHEC_ID();
  const LArFCAL_ID*  help_fcal = m_calo_id_man->getFCAL_ID();
  const CaloCell_ID* help_all = m_calo_id_man->getCaloCell_ID();

  //
  // Now loops :
  //

  sub_calo_num = (int) CaloCell_ID::LAREM;

  std::cout << " ---- EM ---- num=" << sub_calo_num 
	    << " hash_max=" << help_em->channel_hash_max() 
	    << std::endl;

  for ( unsigned int i = 0 ; i < help_em->channel_hash_max(); i++ )
    {
      idhash = (IdentifierHash) i;
      idcalohash = help_all->calo_cell_hash (sub_calo_num, idhash);
      id = help_em->channel_id(idhash);

      const CaloDetDescrElement* newelt = m_calo_dd_man->get_element(idcalohash); 
      if ( !newelt ) std::cout << "missing em element" 
			       << i << std::endl;
      
      else if ( em && help_em->eta(id) == eta_num ) { 
     	std::cout  << std::setw(9) << std::setprecision(4) 
		  << "calo-hash=" << (unsigned int) idcalohash 
		  << " sub-hash=" << i << " region: "
		  << help_em->sampling(id)<< " " 
		  << help_em->region(id)<< " "
		   << " sampling= " << (int) newelt->getSampling()
		  << " eta=" << newelt->eta()
		  << " phi= " << newelt->phi()
		  << " r= " << newelt->r() 
		  << " deta= " << newelt->deta() 
	  //<< " phi_raw = " << newelt->phi_raw()
	  //<< " diff = " << newelt->phi() - newelt->phi_raw()
		  << std::endl;

		  }
      
    }


  sub_calo_num = (int) CaloCell_ID::LARHEC;
  std::cout << " ---- HEC ---- num=" << sub_calo_num
	    << " hash_max=" << help_hec->channel_hash_max() 
	    << std::endl;

  for ( unsigned int i = 0 ; i < help_hec->channel_hash_max(); i++ )
    {
      idhash = (IdentifierHash) i;
      idcalohash = help_all->calo_cell_hash (sub_calo_num, idhash);
      
      id = help_hec->channel_id(idhash);
      
      const CaloDetDescrElement* newelt = m_calo_dd_man->get_element(idcalohash); 
      if ( !newelt ) 
	std::cout << "missing hec element" 
		  << idhash << std::endl;
      
      else if ( hec && help_hec->eta(id) == eta_num ) { 
	
	std::cout  << std::setw(9) << std::setprecision(4) 
		   << "calo-hash=" << (unsigned int) idcalohash 
		   << " sub-hash=" << i << " region: "
		   << help_hec->sampling(id)<< " " 
		   << help_hec->region(id)<< " "
		   << " ieta= " <<  help_hec->eta(id)
		   << " elt: " << newelt->eta()
		   << " phi= " << newelt->phi()
		   << " r= " << newelt->r()
		   << " volume= " << newelt->volume()
		   << std::endl;
      }
    }
  
  
  sub_calo_num = (int) CaloCell_ID::LARFCAL;
  std::cout << " ---- FCAL ---- num=" << sub_calo_num
	    << " hash_max=" << help_fcal->channel_hash_max() 
	     << std::endl;

  for ( unsigned int i = 0 ; i < help_fcal->channel_hash_max();	i++ )
    {
      idhash = (IdentifierHash) i;
      idcalohash = help_all->calo_cell_hash (sub_calo_num, idhash);
      id = help_fcal->channel_id(idhash);

      const CaloDetDescrElement* newelt = m_calo_dd_man->get_element(idcalohash); 
      if ( !newelt ) std::cout << "missing fcal element" 
			       << i << std::endl;

      else if ( fcal && help_fcal->eta(id) == eta_num ) { 
	
     	std::cout  << std::setw(9) << std::setprecision(4) 
		   << "calo-hash=" << (unsigned int) idcalohash 
		  << " sub-hash=" << i << " region: "
		  << help_fcal-> module(id)<< " " 
		  << " ieta= " <<  help_fcal->eta(id)
		  << " " << newelt->eta()
		  << " phi= " << newelt->phi()
		  << " x,y,z " << newelt->x()<< " " <<  newelt->y()
		  << " " <<  newelt->z()
		  << " volume= " << newelt->volume()
		  << std::endl;

      }

    }
}

void
TestCaloDDE::print_elt_HW(bool em, bool hec, bool fcal)
{
  boost::io::ios_base_all_saver coutsave (std::cout);

  ATH_MSG_INFO ( " printing CaloDDE characteristics " );

  IdentifierHash idhash;
  IdentifierHash idcalohash;
  Identifier id;
  int sub_calo_num;

  //retrieves helpers 
  const LArEM_ID*    help_em = m_calo_id_man->getEM_ID();
  const LArHEC_ID*   help_hec = m_calo_id_man->getHEC_ID();
  const LArFCAL_ID*  help_fcal = m_calo_id_man->getFCAL_ID();
  const CaloCell_ID* help_all = m_calo_id_man->getCaloCell_ID();

  //
  // Now look for holes :
  //

  int phi_num = 1;

  sub_calo_num = (int) CaloCell_ID::LAREM;

  std::cout << " ---- EM ---- num=" << sub_calo_num 
	    << " hash_max=" << help_em->channel_hash_max() 
	    << std::endl;

  for ( unsigned int i = 0 ; i < help_em->channel_hash_max(); i++ )
    {
      idhash = (IdentifierHash) i;
      idcalohash = help_all->calo_cell_hash (sub_calo_num, idhash);
      id = help_em->channel_id(idhash);

      const CaloDetDescrElement* newelt = m_calo_dd_man->get_element(idcalohash); 
      if ( !newelt ) std::cout << "missing em element" 
			       << i << std::endl;
      
      else if ( em && help_em->phi(id) == phi_num && newelt->eta() >=0) { 
     	std::cout  << std::setw(9) << std::setprecision(4) 
		  << "calo-hash=" << (unsigned int) idcalohash 
		  << " sub-hash=" << i << " region: "
		  << help_em->sampling(id)<< " " 
		  << help_em->region(id)<< " "
		   << " online id " <<  m_lar_id_man->get_HWId(idcalohash)
		  << std::endl;
		  }
      
    }


  sub_calo_num = (int) CaloCell_ID::LARHEC;
  std::cout << " ---- HEC ---- num=" << sub_calo_num
	    << " hash_max=" << help_hec->channel_hash_max() 
	    << std::endl;

  for ( unsigned int i = 0 ; i < help_hec->channel_hash_max(); i++ )
    {
      idhash = (IdentifierHash) i;
      idcalohash = help_all->calo_cell_hash (sub_calo_num, idhash);
      
      id = help_hec->channel_id(idhash);
      
      const CaloDetDescrElement* newelt = m_calo_dd_man->get_element(idcalohash); 
      if ( !newelt ) 
	std::cout << "missing hec element" 
		  << idhash << std::endl;
      
      else if ( hec && help_hec->phi(id) == phi_num && newelt->eta() >=0) { 
	
	std::cout  << std::setw(9) << std::setprecision(4) 
		   << "calo-hash=" << (unsigned int) idcalohash 
		   << " sub-hash=" << i << " region: "
		   << help_hec->sampling(id)<< " " 
		   << help_hec->region(id)<< " "
		   << " online id " <<  m_lar_id_man->get_HWId(idcalohash)
		   << std::endl;
      }
    }
  
  
  sub_calo_num = (int) CaloCell_ID::LARFCAL;
  std::cout << " ---- FCAL ---- num=" << sub_calo_num
	    << " hash_max=" << help_fcal->channel_hash_max() 
	     << std::endl;

  for ( unsigned int i = 0 ; i < help_fcal->channel_hash_max();	i++ )
    {
      idhash = (IdentifierHash) i;
      idcalohash = help_all->calo_cell_hash (sub_calo_num, idhash);
      id = help_fcal->channel_id(idhash);

      const CaloDetDescrElement* newelt = m_calo_dd_man->get_element(idcalohash); 
      if ( !newelt ) std::cout << "missing fcal element" 
			       << i << std::endl;

      else if (fcal && help_fcal->phi(id) == 
	       phi_num && help_fcal->pos_neg(id) >=0) { 

	
     	std::cout  << std::setw(9) << std::setprecision(4) 
		   << "calo-hash=" << (unsigned int) idcalohash 
		  << " sub-hash=" << i << " region: "
		  << help_fcal-> module(id)<< " " 
		   << " online id " <<  m_lar_id_man->get_HWId(idcalohash)
		  << std::endl;

      }

    }
}

void
TestCaloDDE::try_zone()
{

  ATH_MSG_INFO ( "Executing TestCaloDDE : try_zone " );

  double eta, deta, phi, dphi;
  int sampling_or_module;

  // Try cellsInZone methods : eta min/max, phi min/max, subdet, sampling

  eta = -1.;
  deta = 0.1;
  phi = .3;
  dphi = 0.1;
  sampling_or_module = 1;

  ATH_MSG_INFO ( " ------------------------------------------------------- " );
  ATH_MSG_INFO ( " Negative EMB : should find = 192 strips " );
  ATH_MSG_INFO ( " ------------------------------------------------------- " );
  ATH_MSG_INFO ( " " );
  try_zone(eta, deta, phi, dphi, sampling_or_module);
  ATH_MSG_INFO ( " " );

  eta = 1.;
  deta = 0.1;
  phi = .3;
  dphi = 0.1;
  sampling_or_module = 1;

  ATH_MSG_INFO ( " ------------------------------------------------------- " );
  ATH_MSG_INFO ( " Positive EMB : should find = 192 strips " );
  ATH_MSG_INFO ( " ------------------------------------------------------- " );
  ATH_MSG_INFO ( " " );
  try_zone(eta, deta, phi, dphi, sampling_or_module);
  ATH_MSG_INFO ( " " );

  eta = -1.;
  deta = 0.1;
  phi = .3;
  dphi = 0.1;
  sampling_or_module = 2;

  ATH_MSG_INFO ( " ------------------------------------------------------- " );
  ATH_MSG_INFO ( " Negative EMB : should find = 72 middle " );
  ATH_MSG_INFO ( " ------------------------------------------------------- " );
  ATH_MSG_INFO ( " " );
  try_zone(eta, deta, phi, dphi, sampling_or_module);
  ATH_MSG_INFO ( " " );

  eta = 1.;
  deta = 0.1;
  phi = .3;
  dphi = 0.1;
  sampling_or_module = 2;

  ATH_MSG_INFO ( " ------------------------------------------------------- " );
  ATH_MSG_INFO ( " Positive EMB : should find = 72 middle " );
  ATH_MSG_INFO ( " ------------------------------------------------------- " );
  ATH_MSG_INFO ( " " );
  try_zone(eta, deta, phi, dphi, sampling_or_module);
  ATH_MSG_INFO ( " " );

  eta = -1.;
  deta = 0.1;
  phi = .3;
  dphi = 0.1;
  sampling_or_module = 3;

  ATH_MSG_INFO ( " ------------------------------------------------------- " );
  ATH_MSG_INFO ( " Negative EMB : should find = 36 back " );
  ATH_MSG_INFO ( " ------------------------------------------------------- " );
  ATH_MSG_INFO ( " " );
  try_zone(eta, deta, phi, dphi, sampling_or_module);
  ATH_MSG_INFO ( " " );

  eta = 1.;
  deta = 0.1;
  phi = .3;
  dphi = 0.1;
  sampling_or_module = 3;

  ATH_MSG_INFO ( " ------------------------------------------------------- " );
  ATH_MSG_INFO ( " Positive EMB : should find = 36 back " );
  ATH_MSG_INFO ( " ------------------------------------------------------- " );
  ATH_MSG_INFO ( " " );
  try_zone(eta, deta, phi, dphi, sampling_or_module);
  ATH_MSG_INFO ( " " );

  // -----------------------------------------------------------

  eta = -2.05;
  deta = 0.1;
  phi = -1.64;
  dphi = 0.1;
  sampling_or_module = 1;

  ATH_MSG_INFO ( " ------------------------------------------------------- " );
  ATH_MSG_INFO ( " Negative EMEC : should find 3* ( 15 + 23 ) = 114 strips " );
  ATH_MSG_INFO ( " ------------------------------------------------------- " );
  ATH_MSG_INFO ( " " );
  try_zone(eta, deta, phi, dphi, sampling_or_module);
  ATH_MSG_INFO ( " " );

  eta = 1.81035;
  deta = 0.1;
  phi = 0.5;
  dphi = 0.1;
  sampling_or_module = 1;

  ATH_MSG_INFO ( " " );
  ATH_MSG_INFO ( " -------------------------------------------------------- " );
  ATH_MSG_INFO ( " Positive EMEC : should find 3 * ( 32 + 25 ) = 171 strips " );
  ATH_MSG_INFO ( " -------------------------------------------------------- " );
  ATH_MSG_INFO ( " " );
  try_zone(eta, deta, phi, dphi, sampling_or_module);
  ATH_MSG_INFO ( " " );

  eta = 0.;
  deta = 0.1;
  phi = 0.01;
  dphi = 0.1;
  sampling_or_module = 1;

  ATH_MSG_INFO ( " " );
  ATH_MSG_INFO ( " ---------------------------------------- " );
  ATH_MSG_INFO ( " EMB, across z=0 : strip 0 disconnected   " );
  ATH_MSG_INFO ( " should find 3 * ( 31 + 31 ) = 186 strips " );
  ATH_MSG_INFO ( " ---------------------------------------- " );
  ATH_MSG_INFO ( " " );
  try_zone(eta, deta, phi, dphi, sampling_or_module);
  ATH_MSG_INFO ( " " );

  eta = 1.4;
  deta = 0.1;
  phi = 0.5;
  dphi = 0.1;
  sampling_or_module = 2;

  ATH_MSG_INFO ( " " );
  ATH_MSG_INFO ( " ---------------------------------------- " );
  ATH_MSG_INFO ( " Crack, middle layer : should find 9 x 9 = 81 cells " );
  ATH_MSG_INFO ( " where : 9 = 4 EMB2 + 1 barrel-end + 1 emec-begin + 3 EMEC2 " );
  ATH_MSG_INFO ( " ---------------------------------------- " );
  ATH_MSG_INFO ( " " );
  try_zone(eta, deta, phi, dphi, sampling_or_module);
  ATH_MSG_INFO ( " " );
  ATH_MSG_INFO ( " " );
  ATH_MSG_INFO ( " " );

  eta = -1.4;
  deta = 0.1;
  phi = 0.5;
  dphi = 0.1;
  sampling_or_module = 2;

  ATH_MSG_INFO ( " " );
  ATH_MSG_INFO ( " ----------------------------------------------------------- " );
  ATH_MSG_INFO ( " Negative crack, middle layer : should find 9 x 9 = 81 cells " );
  ATH_MSG_INFO ( " where : 9 = 4 EMB2 + 1 barrel-end + 1 emec-begin + 3 EMEC2  " );
  ATH_MSG_INFO ( " ----------------------------------------------------------- " );
  ATH_MSG_INFO ( " " );
  try_zone(eta, deta, phi, dphi, sampling_or_module);
  ATH_MSG_INFO ( " " );
  ATH_MSG_INFO ( " " );
  ATH_MSG_INFO ( " " );

  eta = 1.4;
  deta = 0.1;
  phi = 0.01;
  dphi = 0.1;
  sampling_or_module = 1;

  ATH_MSG_INFO ( " " );
  ATH_MSG_INFO ( " ---------------------------------------- " );
  ATH_MSG_INFO ( " Crack, strips : should find 135 cells :" );
  ATH_MSG_INFO ( " 3 x ( 32 EMB1 + 1 EMEC1-reg0 + 3 EMEC1-reg1 ) " );
  ATH_MSG_INFO ( " + 9 x ( 3 barrel-end, which has a different phi grannularity ) " );
  ATH_MSG_INFO ( " ---------------------------------------- " );
  ATH_MSG_INFO ( " " );
  try_zone(eta, deta, phi, dphi, sampling_or_module);
  ATH_MSG_INFO ( " " );
  ATH_MSG_INFO ( " " );
  ATH_MSG_INFO ( " " );

  eta = 1.4;
  deta = 0.2;
  phi = 0.01;
  dphi = 0.02;
  sampling_or_module = 3;

  ATH_MSG_INFO ( " " );
  ATH_MSG_INFO ( " ----------------------------------  " );
  ATH_MSG_INFO ( " Crack, back : should find 15 cells :" );
  ATH_MSG_INFO ( " 3 x ( 3 EMB3 + 2 EMEC3 )            " );
  ATH_MSG_INFO ( " ----------------------------------  " );
  ATH_MSG_INFO ( " " );
  try_zone(eta, deta, phi, dphi, sampling_or_module);
  ATH_MSG_INFO ( " " );
  ATH_MSG_INFO ( " " );
  ATH_MSG_INFO ( " " );

  eta = 0.966;
  deta = 0.05;
  phi = -3.1;
  dphi = 0.05;
  sampling_or_module = 2;

  ATH_MSG_INFO ( " " );
  ATH_MSG_INFO ( " ---------------------------------------  " );
  ATH_MSG_INFO ( " Phi wrapping : should find 25 cells :    " );
  ATH_MSG_INFO ( "     from phi_min (-3.15, i.e. 3.133 ) to +pi = 0.3 cell  " );
  ATH_MSG_INFO ( "     from -pi to phi_max (-3.05) : 0.0916 = 3.7 cells " );
  ATH_MSG_INFO ( " 5 in phi ( 4 ~ -pi, 1 ~ +pi ) x  5 EMB2  " );
  ATH_MSG_INFO ( " ---------------------------------------  " );
  ATH_MSG_INFO ( " " );
  try_zone(eta, deta, phi, dphi, sampling_or_module);
  ATH_MSG_INFO ( " " );
  ATH_MSG_INFO ( " " );
  ATH_MSG_INFO ( " " );

}


void
TestCaloDDE::try_each_descr_zone()
{
  ATH_MSG_INFO( "" );
  ATH_MSG_INFO ( "Executing TestCaloDDE : try_zone for each descriptor " );
  ATH_MSG_INFO( "" );

  std::vector<IdentifierHash> cell_list;
  int num = 0;
  
  IdentifierHash min,max;
  //retrieves helpers 
  const CaloCell_ID* help_all = m_calo_id_man->getCaloCell_ID();
  const LArEM_ID*    help_em = m_calo_id_man->getEM_ID();
  const LArHEC_ID*   help_hec = m_calo_id_man->getHEC_ID();

  help_all->calo_reg_hash_range(CaloCell_ID::LAREM, min,max);
  int em_nb = (int)max - (int)min;
  help_all->calo_reg_hash_range(CaloCell_ID::LARHEC, min,max);
  int hec_nb = (int)max - (int)min;
  help_all->calo_reg_hash_range(CaloCell_ID::LARFCAL, min,max);
  int fcal_nb = (int)max - (int)min;
  help_all->calo_reg_hash_range(CaloCell_ID::TILE, min,max);
  int tile_nb = (int)max - (int)min;

  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << " I'm expecting : " << em_nb << " LAREM " << hec_nb << " LARHEC "
	    << fcal_nb << " LARFCAL " << tile_nb << " TILE regions " << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;

  for (const CaloDetDescriptor* descr : m_calo_dd_man->calo_descriptors_range())
    {
      if ( num == em_nb || num == (em_nb+hec_nb) || num == (em_nb+hec_nb+fcal_nb)
	   || num == (em_nb+hec_nb+fcal_nb+tile_nb) ) 
	std::cout << " ----------------------------------------------------------- " << std::endl;

      if (!descr) std::cout << " Invalid pointer for (tile) region number : " << num << std::endl;
      else {

	// take ideal position :
	//double eta_middle = descr->calo_sign()*( descr->calo_eta_min() + descr->calo_eta_max() )/2.;
	// take misaligned positions : 
	double eta_middle = ( descr->reg_min() + descr->reg_max() )/2.;

	// for eta take 2 cells on each side of middle, and avoid to shoot between 2 cells
	// ( will catch 4 or 5 cells, depending on where you shoot )
	double eta_min = eta_middle + 0.001 - 1.8*descr->deta();
	double eta_max = eta_middle + 0.001 + 1.8*descr->deta();
	
	// for phi take 1 cell around phi=0 ( will catch 2 cells )
	double phi_min = 0.001 - 0.8*descr->dphi();
	double phi_max = 0.001 + 0.8*descr->dphi();

	std::cout << " REGION number : " << num << std::endl;
	if (num < em_nb) help_em->print(descr->identify());
	else if (num < em_nb+hec_nb) help_hec->print(descr->identify());

	std::cout << " eta_min " << eta_min 
		  << " eta_max " << eta_max 
		  << " phi_min " << phi_min 
		  << " phi_max " << phi_max
		  << " CaloSample " << descr->getSampling(0)
		  << std::endl;
	
	m_calo_dd_man->cellsInZone(eta_min,eta_max,phi_min,phi_max,descr,cell_list);
	std::cout << " ==> found :" << cell_list.size() << " cells  " ;
	if( cell_list.size() != 10 && cell_list.size() != 8 ) std::cout << " <----- ??? " ;      
	std::cout << std::endl;
	std::cout << std::endl;
      }
      
      num++;
    }
  std::cout << std::endl;
  std::cout << std::endl;
}

void
TestCaloDDE::try_zone(double eta, double deta, double phi, double dphi, int sampling_or_module)
{

  ATH_MSG_INFO( "" );
  ATH_MSG_INFO ( "Executing TestCaloDDE : try_zone for eta= " << eta << " +- " 
                 << deta << " phi= " << phi << " +- " << dphi << " sampling " << sampling_or_module
                 );
  ATH_MSG_INFO( "" );

  IdentifierHash idhash;
  Identifier id;

  const LArEM_ID*    help_em = m_calo_id_man->getEM_ID();
  //const LArHEC_ID*   help_hec = m_calo_id_man->getHEC_ID();
  //const LArFCAL_ID*  help_fcal = m_calo_id_man->getFCAL_ID();
  //const CaloCell_ID* help_all = m_calo_id_man->getCaloCell_ID();

  double eta_min = eta - deta;
  double eta_max = eta + deta;
  double phi_min  = phi - dphi;
  double phi_max = phi + dphi;

  std::vector<IdentifierHash> cell_list;

  std::cout << " EM REGION : eta_min " << eta_min 
	    << " eta_max " << eta_max 
	    << " phi_min " << phi_min 
	    << " phi_max " << phi_max
	    << " sampling_or_module " << sampling_or_module
	    << std::endl;

  m_calo_dd_man->cellsInZone(eta_min,eta_max,phi_min,phi_max,
		     CaloCell_ID::LAREM,sampling_or_module,cell_list);
  //			    CaloCell_ID::LAREM,cell_list);
  //		cell_list );

  
  //m_calo_dd_man->cellsInZone(eta,phi,5,5,CaloCell_ID::LAREM,
  //			     sampling_or_module,barrel,cell_list);

  std::cout << " ==> with Calo found :" << cell_list.size() 
	    << " cells !  " << std::endl;

  for ( unsigned int i = 0; i<cell_list.size(); i++ )
    {
      idhash = cell_list[i];
      id = help_em->channel_id(idhash);
      const CaloDetDescrElement* newelt = m_calo_dd_man->get_element(idhash); 
      if ( !newelt ) std::cout << "  missing element" 
			       << i << std::endl;
      /*
      else {
        std::cout << " sub-hash=" << idhash ;
     	std::cout << "  sampling/region: "
		  << help_em->sampling(id)<< " " 
		  << help_em->region(id)<< " "
		  << " ieta= " <<  help_em->eta(id)
		  << " eta= " << newelt->eta()
		  << " iphi= " <<  help_em->phi(id)
		  << " phi= " << newelt->phi()
		  << std::endl;
            }
      */
    }
}

void
TestCaloDDE::where_am_I(double eta, double phi)
{
  
  ATH_MSG_INFO ( "Executing TestCaloDDE : where am I ? eta, phi = " 
                 << eta << " " << phi );

  bool barrel = true;
  const CaloDetDescriptor* descr;

  for (int nlay = 0 ; nlay<4; nlay++) {
    descr = m_calo_dd_man->
      get_descriptor(CaloCell_ID::LAREM,nlay,barrel,eta,phi);
    if(descr) std::cout << "barrel EM layer " <<  nlay 
			<< std::endl;
  }

  barrel = false;

  for (int nlay = 0 ; nlay<4; nlay++) {
    descr = m_calo_dd_man->
      get_descriptor(CaloCell_ID::LAREM,nlay,barrel,eta,phi);
    if(descr) std::cout << "endcap EM layer " <<  nlay 
			<< std::endl;
  }

  for (int nlay = 0 ; nlay<4; nlay++) {
    descr = m_calo_dd_man->
      get_descriptor(CaloCell_ID::LARHEC,barrel,nlay,eta,phi);
    if(descr)  std::cout << "HEC layer " <<  nlay 
			 << std::endl;
  }
  for (int nlay = 0 ; nlay<4; nlay++) {
    descr = m_calo_dd_man->
      get_descriptor(CaloCell_ID::LARFCAL,barrel,nlay,eta,phi);
    if(descr)  std::cout << "FCAL layer " <<  nlay 
			 << std::endl;
  }

}

void
TestCaloDDE::read_volumes()
{
  ATH_MSG_INFO ( "Executing TestCaloDDE : read_volumes " );

  IdentifierHash caloHashId = 86725;

  
  std::cout << " subCaloHash = " << (unsigned int)caloHashId << " -------> got it !";
  const CaloDetDescrElement* dde = m_calo_dd_man->get_element(caloHashId);
  if (dde) 
    std::cout << dde->volume();
  std::cout << std::endl;
}

void
TestCaloDDE::update()
{
  Transform3D delta =  RotateZ3D(0.01);
}

void 
TestCaloDDE::print_edges_via_SubCalo( CaloCell_ID::CaloSample sample, double eta, double phi )
{
  const LArEM_ID*    help_em = m_calo_id_man->getEM_ID();

  CaloCell_ID::SUBCALO subcalo;
  bool barrel;
  int sampling_or_module;

  std::cout << std::endl;
  m_calo_dd_man->decode_sample (subcalo, barrel, sampling_or_module, sample);
  std::cout << " subcalo " << subcalo << " barrel " << barrel << " sampling_or_module "
	    << sampling_or_module << " eta " << eta << " phi " << phi << std::endl;

  const CaloDetDescriptor* reg = 
    m_calo_dd_man->get_descriptor (subcalo,sampling_or_module,barrel,eta,phi);
  if (!reg)
    std::cout << " no region for that value !" << std::endl;
  else
    help_em->print(reg->identify());

  const CaloDetDescrElement* newelt = 
    m_calo_dd_man->get_element(subcalo,sampling_or_module, barrel, eta, phi);

  if (newelt) {
    boost::io::ios_base_all_saver coutsave (std::cout);

    Identifier id = newelt->identify();
    std::cout  << std::setw(9) << std::setprecision(4) 
	       << " Region: " << help_em->sampling(id)<< " " 
	       << help_em->region(id)<< " "
	       << " ieta= " <<  help_em->eta(id)
	       << " " << newelt->eta()
	       << " phi= " << newelt->phi()
	       << " r= " << newelt->r() 
      //<< " volume= " << newelt->volume()
	       << " deta= " << newelt->deta()
	       << " grann= " << newelt->descriptor()->deta()
	       << " sampling= " << (int) newelt->getSampling()
      //	       << " layer= " << newelt->getLayer()
	       << std::endl;

  }
  else
    std::cout << " no element for that value !" << std::endl;

  std::cout << std::endl;

}

void
TestCaloDDE::print_edges_via_SubCalo()
{


  std::cout << std::endl;
  std::cout << " ------------ PS -------------- " << std::endl;

  print_edges_via_SubCalo(CaloCell_ID::PreSamplerB, -1.6, 0.);
  print_edges_via_SubCalo(CaloCell_ID::PreSamplerB, -1.55, 0.);
  print_edges_via_SubCalo(CaloCell_ID::PreSamplerB, -1.53, 0.);
  print_edges_via_SubCalo(CaloCell_ID::PreSamplerB, -1.52, 0.);
  print_edges_via_SubCalo(CaloCell_ID::PreSamplerB, -1.48, 0.);
  print_edges_via_SubCalo(CaloCell_ID::PreSamplerB, -1.47, 0.);
  print_edges_via_SubCalo(CaloCell_ID::PreSamplerB, -1.45, 0.);
  print_edges_via_SubCalo(CaloCell_ID::PreSamplerB, -1.3, 0.);
  print_edges_via_SubCalo(CaloCell_ID::PreSamplerB, -0.1, 0.);
  print_edges_via_SubCalo(CaloCell_ID::PreSamplerB,  0., 0.);

  print_edges_via_SubCalo(CaloCell_ID::PreSamplerB, 1.6, 0.);
  print_edges_via_SubCalo(CaloCell_ID::PreSamplerB, 1.5, 0.);
  print_edges_via_SubCalo(CaloCell_ID::PreSamplerB, 1.45, 0.);
  print_edges_via_SubCalo(CaloCell_ID::PreSamplerB, 1.4, 0.);
  print_edges_via_SubCalo(CaloCell_ID::PreSamplerB, 1.3, 0.);
  print_edges_via_SubCalo(CaloCell_ID::PreSamplerB, 0.1, 0.);
  print_edges_via_SubCalo(CaloCell_ID::PreSamplerB, 0.002854, 0.);

  print_edges_via_SubCalo(CaloCell_ID::PreSamplerE, -1.4, 0.);
  print_edges_via_SubCalo(CaloCell_ID::PreSamplerE, -1.45, 0.);
  print_edges_via_SubCalo(CaloCell_ID::PreSamplerE, -1.49, 0.);
  print_edges_via_SubCalo(CaloCell_ID::PreSamplerE, -1.5, 0.);
  print_edges_via_SubCalo(CaloCell_ID::PreSamplerE, -1.52, 0.);
  print_edges_via_SubCalo(CaloCell_ID::PreSamplerE, -1.6, 0.);
  print_edges_via_SubCalo(CaloCell_ID::PreSamplerE, -1.79, 0.);
  print_edges_via_SubCalo(CaloCell_ID::PreSamplerE, -1.8, 0.);
  print_edges_via_SubCalo(CaloCell_ID::PreSamplerE, -1.81, 0.);

  print_edges_via_SubCalo(CaloCell_ID::PreSamplerE, 1.3, 0.);
  print_edges_via_SubCalo(CaloCell_ID::PreSamplerE, 1.4, 0.);
  print_edges_via_SubCalo(CaloCell_ID::PreSamplerE, 1.45, 0.);
  print_edges_via_SubCalo(CaloCell_ID::PreSamplerE, 1.6, 0.);
  print_edges_via_SubCalo(CaloCell_ID::PreSamplerE, 2.45, 0.);
  print_edges_via_SubCalo(CaloCell_ID::PreSamplerE, 2.5, 0.);
  print_edges_via_SubCalo(CaloCell_ID::PreSamplerE, 2.6, 0.);

  std::cout << std::endl;
  std::cout << " ------------ Strips -------------- " << std::endl;

  print_edges_via_SubCalo(CaloCell_ID::EMB1, -1.48, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB1, -1.47, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB1, -1.45, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB1, -1.41, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB1, -1.4, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB1, -1.39, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB1, -1.3, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB1, -0.1, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB1, -0.01, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB1,  0., 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB1,  0.01, 0.);

  print_edges_via_SubCalo(CaloCell_ID::EMB1, 1.6, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB1, 1.5, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB1, 1.45, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB1, 1.4, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB1, 1.3, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB1, 0.1, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB1, 0.002854, 0.);

  print_edges_via_SubCalo(CaloCell_ID::EME1, -1.3, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME1, -1.4, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME1, -1.45, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME1, -1.6, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME1, -2.45, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME1, -2.5, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME1, -2.6, 0.);

  print_edges_via_SubCalo(CaloCell_ID::EME1, 1.3, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME1, 1.4, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME1, 1.45, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME1, 1.49, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME1, 1.5, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME1, 1.505, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME1, 1.51, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME1, 1.515, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME1, 1.6, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME1, 1.795, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME1, 1.799, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME1, 1.8, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME1, 1.801, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME1, 1.805, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME1, 1.81, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME1, 1.815, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME1, 1.82, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME1, 2.45, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME1, 2.5, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME1, 2.6, 0.);


  std::cout << std::endl;
  std::cout << " ------------ Middle -------------- " << std::endl;

  print_edges_via_SubCalo(CaloCell_ID::EMB2, -1.6, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB2, -1.5, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB2, -1.45, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB2, -1.4, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB2, -1.3, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB2, -0.1, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB2,  0., 0.);

  print_edges_via_SubCalo(CaloCell_ID::EMB2, 1.6, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB2, 1.5, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB2, 1.45, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB2, 1.4, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB2, 1.3, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB2, 0.1, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB2, 0.002854, 0.);

  print_edges_via_SubCalo(CaloCell_ID::EME2, -1.3, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME2, -1.4, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME2, -1.45, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME2, -1.6, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME2, -2.45, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME2, -2.5, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME2, -2.6, 0.);

  print_edges_via_SubCalo(CaloCell_ID::EME2, 1.3, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME2, 1.4, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME2, 1.45, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME2, 1.6, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME2, 2.45, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME2, 2.5, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME2, 2.6, 0.);

  std::cout << std::endl;
  std::cout << " ------------ Back -------------- " << std::endl;

  print_edges_via_SubCalo(CaloCell_ID::EMB3, -1.4, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB3, -1.36, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB3, -1.35, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB3, -1.34, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB3, -1.3, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB3, -0.1, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB3,  0., 0.);

  print_edges_via_SubCalo(CaloCell_ID::EMB3, 1.6, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB3, 1.5, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB3, 1.45, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB3, 1.4, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB3, 1.3, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB3, 0.1, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EMB2, 0.002854, 0.);

  print_edges_via_SubCalo(CaloCell_ID::EME3, -1.4, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME3, -1.49, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME3, -1.5, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME3, -1.505, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME3, -1.51, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME3, -1.6, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME3, -2.45, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME3, -2.5, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME3, -2.6, 0.);

  print_edges_via_SubCalo(CaloCell_ID::EME3, 1.3, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME3, 1.4, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME3, 1.45, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME3, 1.6, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME3, 2.45, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME3, 2.5, 0.);
  print_edges_via_SubCalo(CaloCell_ID::EME3, 2.6, 0.);

}

void 
TestCaloDDE::print_edges_via_CaloSample( CaloCell_ID::CaloSample sample, double eta, double phi )
{
  const LArEM_ID*    help_em = m_calo_id_man->getEM_ID();

  std::cout << " CaloCell_ID::CaloSample " << sample << " eta " << eta << " phi " << phi << std::endl;

  const CaloDetDescriptor* reg = 
    m_calo_dd_man->get_descriptor (sample,eta,phi);
  if (!reg)
    std::cout << " no region for that value !" << std::endl;
  else
    help_em->print(reg->identify());

  const CaloDetDescrElement* newelt = 
    m_calo_dd_man->get_element(sample, eta, phi);

  if (newelt) {
    boost::io::ios_base_all_saver coutsave (std::cout);

    Identifier id = newelt->identify();
    std::cout  << std::setw(9) << std::setprecision(4) 
	       << " Region: " << help_em->sampling(id)<< " " 
	       << help_em->region(id)<< " "
	       << " ieta= " <<  help_em->eta(id)
	       << " " << newelt->eta()
	       << " phi= " << newelt->phi()
	       << " r= " << newelt->r() 
      //<< " volume= " << newelt->volume()
	       << " deta= " << newelt->deta()
	       << " grann= " << newelt->descriptor()->deta()
	       << " sampling= " << (int) newelt->getSampling()
      //	       << " layer= " << newelt->getLayer()
	       << std::endl;

  }
  else
    std::cout << " no element for that value !" << std::endl;

  std::cout << std::endl;

}

void
TestCaloDDE::print_edges_via_CaloSample()
{


  std::cout << std::endl;
  std::cout << " ------------ PS -------------- " << std::endl;

  print_edges_via_CaloSample(CaloCell_ID::PreSamplerB, -1.6, 0.);
  print_edges_via_CaloSample(CaloCell_ID::PreSamplerB, -1.55, 0.);
  print_edges_via_CaloSample(CaloCell_ID::PreSamplerB, -1.53, 0.);
  print_edges_via_CaloSample(CaloCell_ID::PreSamplerB, -1.52, 0.);
  print_edges_via_CaloSample(CaloCell_ID::PreSamplerB, -1.48, 0.);
  print_edges_via_CaloSample(CaloCell_ID::PreSamplerB, -1.47, 0.);
  print_edges_via_CaloSample(CaloCell_ID::PreSamplerB, -1.45, 0.);
  print_edges_via_CaloSample(CaloCell_ID::PreSamplerB, -1.3, 0.);
  print_edges_via_CaloSample(CaloCell_ID::PreSamplerB, -0.1, 0.);
  print_edges_via_CaloSample(CaloCell_ID::PreSamplerB,  0., 0.);

  print_edges_via_CaloSample(CaloCell_ID::PreSamplerB, 1.6, 0.);
  print_edges_via_CaloSample(CaloCell_ID::PreSamplerB, 1.5, 0.);
  print_edges_via_CaloSample(CaloCell_ID::PreSamplerB, 1.45, 0.);
  print_edges_via_CaloSample(CaloCell_ID::PreSamplerB, 1.4, 0.);
  print_edges_via_CaloSample(CaloCell_ID::PreSamplerB, 1.3, 0.);
  print_edges_via_CaloSample(CaloCell_ID::PreSamplerB, 0.1, 0.);
  print_edges_via_CaloSample(CaloCell_ID::PreSamplerB, 0.002854, 0.);

  print_edges_via_CaloSample(CaloCell_ID::PreSamplerE, -1.4, 0.);
  print_edges_via_CaloSample(CaloCell_ID::PreSamplerE, -1.45, 0.);
  print_edges_via_CaloSample(CaloCell_ID::PreSamplerE, -1.49, 0.);
  print_edges_via_CaloSample(CaloCell_ID::PreSamplerE, -1.5, 0.);
  print_edges_via_CaloSample(CaloCell_ID::PreSamplerE, -1.52, 0.);
  print_edges_via_CaloSample(CaloCell_ID::PreSamplerE, -1.6, 0.);
  print_edges_via_CaloSample(CaloCell_ID::PreSamplerE, -1.79, 0.);
  print_edges_via_CaloSample(CaloCell_ID::PreSamplerE, -1.8, 0.);
  print_edges_via_CaloSample(CaloCell_ID::PreSamplerE, -1.81, 0.);

  print_edges_via_CaloSample(CaloCell_ID::PreSamplerE, 1.3, 0.);
  print_edges_via_CaloSample(CaloCell_ID::PreSamplerE, 1.4, 0.);
  print_edges_via_CaloSample(CaloCell_ID::PreSamplerE, 1.45, 0.);
  print_edges_via_CaloSample(CaloCell_ID::PreSamplerE, 1.6, 0.);
  print_edges_via_CaloSample(CaloCell_ID::PreSamplerE, 2.45, 0.);
  print_edges_via_CaloSample(CaloCell_ID::PreSamplerE, 2.5, 0.);
  print_edges_via_CaloSample(CaloCell_ID::PreSamplerE, 2.6, 0.);

  std::cout << std::endl;
  std::cout << " ------------ Strips -------------- " << std::endl;

  print_edges_via_CaloSample(CaloCell_ID::EMB1, -1.48, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB1, -1.47, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB1, -1.45, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB1, -1.41, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB1, -1.4, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB1, -1.39, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB1, -1.3, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB1, -0.1, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB1, -0.01, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB1,  0., 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB1,  0.01, 0.);

  print_edges_via_CaloSample(CaloCell_ID::EMB1, 1.6, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB1, 1.5, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB1, 1.45, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB1, 1.4, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB1, 1.3, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB1, 0.1, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB1, 0.002854, 0.);

  print_edges_via_CaloSample(CaloCell_ID::EME1, -1.3, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME1, -1.4, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME1, -1.45, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME1, -1.6, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME1, -2.45, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME1, -2.5, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME1, -2.6, 0.);

  print_edges_via_CaloSample(CaloCell_ID::EME1, 1.3, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME1, 1.4, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME1, 1.45, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME1, 1.49, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME1, 1.5, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME1, 1.505, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME1, 1.51, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME1, 1.515, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME1, 1.6, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME1, 1.795, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME1, 1.799, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME1, 1.8, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME1, 1.801, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME1, 1.805, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME1, 1.81, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME1, 1.815, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME1, 1.82, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME1, 2.45, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME1, 2.5, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME1, 2.6, 0.);


  std::cout << std::endl;
  std::cout << " ------------ Middle -------------- " << std::endl;

  print_edges_via_CaloSample(CaloCell_ID::EMB2, -1.6, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB2, -1.5, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB2, -1.45, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB2, -1.4, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB2, -1.3, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB2, -0.1, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB2,  0., 0.);

  print_edges_via_CaloSample(CaloCell_ID::EMB2, 1.6, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB2, 1.5, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB2, 1.45, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB2, 1.4, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB2, 1.3, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB2, 0.1, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB2, 0.002854, 0.);

  print_edges_via_CaloSample(CaloCell_ID::EME2, -1.3, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME2, -1.4, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME2, -1.45, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME2, -1.6, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME2, -2.45, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME2, -2.5, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME2, -2.6, 0.);

  print_edges_via_CaloSample(CaloCell_ID::EME2, 1.3, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME2, 1.4, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME2, 1.45, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME2, 1.6, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME2, 2.45, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME2, 2.5, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME2, 2.6, 0.);

  std::cout << std::endl;
  std::cout << " ------------ Back -------------- " << std::endl;

  print_edges_via_CaloSample(CaloCell_ID::EMB3, -1.4, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB3, -1.36, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB3, -1.35, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB3, -1.34, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB3, -1.3, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB3, -0.1, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB3,  0., 0.);

  print_edges_via_CaloSample(CaloCell_ID::EMB3, 1.6, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB3, 1.5, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB3, 1.45, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB3, 1.4, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB3, 1.3, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB3, 0.1, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EMB3, 0.002854, 0.);

  print_edges_via_CaloSample(CaloCell_ID::EME3, -1.4, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME3, -1.49, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME3, -1.5, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME3, -1.505, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME3, -1.51, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME3, -1.6, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME3, -2.45, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME3, -2.5, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME3, -2.6, 0.);

  print_edges_via_CaloSample(CaloCell_ID::EME3, 1.3, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME3, 1.4, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME3, 1.45, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME3, 1.6, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME3, 2.45, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME3, 2.5, 0.);
  print_edges_via_CaloSample(CaloCell_ID::EME3, 2.6, 0.);

}
