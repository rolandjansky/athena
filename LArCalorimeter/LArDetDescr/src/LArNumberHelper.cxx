/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArDetDescr/LArNumberHelper.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"

#include "StoreGate/StoreGate.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/ISvcLocator.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "CLHEP/Geometry/Point3D.h"

#include "CaloIdentifier/CaloID.h"
#include "CaloIdentifier/CaloIdManager.h"

#include "LArDetDescr/LArCellVolumes.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "GaudiKernel/SystemOfUnits.h"

LArNumberHelper::LArNumberHelper(const std::string geometry) :
  m_geometry(geometry),
  m_pDetStore(0),
  m_iAccessSvc(0),
  m_rec(0),
  m_geoModelSvc(0),
  m_cellVolumes(0),
  m_emb_psin(0),
  m_accg_rin_ac(0),
  m_accg_rout_ac(0),
  m_endg_zorig(0),
  m_emb_iwout(0),
  m_emec_psin(0),
  m_esep_zsep12(0),
  m_emec_out(0),
  m_hec_in0(0),
  m_hec_in1(0),
  m_hec_in2(0),
  m_hec_in3(0),
  m_hec_out(0),
  m_hec_gap(0)
{

  IMessageSvc*  msgSvc;

  //retrieves helpers 
  const CaloIdManager*  calo_id_man = CaloIdManager::instance();
  m_em_id = calo_id_man->getEM_ID();
  m_hec_id = calo_id_man->getHEC_ID();
  m_fcal_id = calo_id_man->getFCAL_ID();

  ISvcLocator* svcLoc = Gaudi::svcLocator( );
  StatusCode sc = svcLoc->service( "MessageSvc", msgSvc );

  if ( sc.isSuccess( ) ) {

    MsgStream log(msgSvc, "LArNumberHelper" );

    sc = svcLoc->service( "DetectorStore", m_pDetStore );

    if (!sc.isSuccess() || 0 == m_pDetStore)  {
      log <<MSG::FATAL <<"Could not find DetectorStore" <<endmsg;
      // do not try to go further: nothing will work anyway
      return;
    }
    
    // Decode tag ( via GeoModel ) and fix it for TestBeam :
    m_tag = "ATLAS-00";
    m_node = "ATLAS";
    m_geoModelSvc  = 0;

    sc = svcLoc->service("GeoModelSvc",m_geoModelSvc);
    if (sc.isFailure()) 
      log << MSG::ERROR << "Unable to get pointer to GeoModel service" << endmsg;
    else
    {
      DecodeVersionKey detectorKey = DecodeVersionKey(m_geoModelSvc, "LAr");
      log <<MSG::INFO << "DecodeVersionKey found : " << detectorKey.tag() 
	  << " " << detectorKey.tag()<<endmsg;

      if ( detectorKey.tag() != "LAr-H8-00" &&  detectorKey.tag() != "LAr-H6-00"
	   && detectorKey.tag() != "LAr-G3-00")
	{
	  m_tag =  detectorKey.tag();
	  m_node =  detectorKey.node();
	}
    }

    log <<MSG::INFO <<"Readout geometry will use : " << m_tag << " " << m_node <<endmsg;

    // Acess the DB service :

    sc = svcLoc->service("RDBAccessSvc",m_iAccessSvc);
    if (sc.isFailure()) 
      {
	log << MSG::ERROR << "Unable to get RDBAccessSvc." << endmsg;
      }
    else
      {
	log <<MSG::INFO <<" did access RDBAccessSvc " <<endmsg;

	prepare_arrays();
	
	db_nb_em();
	db_nb_hec();
	db_nb_fcal();
	
	IRDBRecordset_ptr recCellVolumes = m_iAccessSvc->getRecordsetPtr("LArCellVolumes",m_tag,m_node);
	
	if(recCellVolumes->size()==0)
	  {
	    // Error occured in detting the recordset with detectorKey
	    // That probably means the LArCellVolumes node does not exist for this
	    // particular version of atlas geometry. Make another try
	    
	    recCellVolumes = m_iAccessSvc->getRecordsetPtr("LArCellVolumes","LArCellVolumes-00");
	  }
	
	if(recCellVolumes->size())
	  m_cellVolumes = new LArCellVolumes(recCellVolumes);
	else 
	  log << MSG::ERROR << "Unable to get LArCellVolumes" << endmsg;

      }

    // now use all the info to finish up :
    hard_em();
    hard_hec();
    hard_fcal();
    
    //print_em();
    //print_hec();
    //print_fcal();
  }
}


LArNumberHelper::~LArNumberHelper() 
{
  if(m_cellVolumes)
    delete m_cellVolumes;
}

int  
LArNumberHelper::nb_of_em_regions()
{  return m_region_id_em.size();
}

int  
LArNumberHelper::nb_of_hec_regions()
{  return m_region_id_hec.size();
}

int  
LArNumberHelper::nb_of_fcal_regions()
{  return m_region_id_fcal.size();
}
  
void  
LArNumberHelper::print_em()
{

    std::cout << "LArNumberHelper::print_em" << std::endl;

  for ( unsigned int i=0; i < m_em_id->region_hash_max(); i++ ) {
    
    int barrel_ec = abs( m_em_id->barrel_ec (m_region_id_em[i]) );
    int samp = m_em_id->sampling (m_region_id_em[i]);
    int region = m_em_id->region (m_region_id_em[i]);
    std::cout << "Region #" << i << " is : " << barrel_ec 
	      << " " << samp << " " << region << std::endl;

    std::cout << "number of chan in eta, phi "
	      <<  m_n_eta_em[i] << " " << m_n_phi_em[i] << std::endl;

    std::cout << "eta_min eta_max deta phi_min phi_max dphi"
	      << std::endl;

    std::cout << m_eta_min_em[i] << " " << m_eta_max_em[i]
	      << " " << m_deta_em[i] << " "
              << m_phi_min_em[i] << " " << m_phi_max_em[i] 
	      << " " << m_dphi_em[i] << std::endl;

    if ( m_n_depth_em[i] != 0 ) {
      std::cout << "EMEC Z locations : in" ;
      for ( int j = 0 ; j < m_n_depth_em[i]; j++ ) 
	std::cout << m_depth_em_in[i][j] << " " ;
      std::cout <<  std::endl;
      
      std::cout << "EMEC Z locations : out" ;
      for ( int j = 0 ; j < m_n_depth_em[i]; j++ ) 
	std::cout << m_depth_em_out[i][j] << " " ;
      std::cout <<  std::endl;
    }
  }
}

void  
LArNumberHelper::print_hec()
{

    std::cout << "LArNumberHelper::print_hec" << std::endl;

  for ( unsigned int i=0; i < m_hec_id->region_hash_max(); i++ ) {
    
    int pos_neg = abs( m_hec_id->pos_neg (m_region_id_hec[i]) );
    int samp = m_hec_id->sampling (m_region_id_hec[i]);
    int region = m_hec_id->region (m_region_id_hec[i]);
    std::cout << "Region #" << i << " is : " << pos_neg 
	      << " " << samp << " " << region << std::endl;

    std::cout << "number of chan in eta, phi "
	      <<  m_n_eta_hec[i] << " " << m_n_phi_hec[i] << std::endl;

    std::cout << "eta_min eta_max deta phi_min phi_max dphi"
	      << std::endl;

    std::cout << m_eta_min_hec[i] << " " << m_eta_max_hec[i]
	      << " " << m_deta_hec[i] << " "
              << m_phi_min_hec[i] << " " << m_phi_max_hec[i] 
	      << " " << m_dphi_hec[i] << std::endl;

    if ( m_n_depth_hec[i] != 0 ) {
      std::cout << "HEC Z locations: in" ;
      for ( int j = 0 ; j < m_n_depth_hec[i]; j++ ) 
	std::cout << m_depth_hec_in[i][j] << " " ;
      std::cout <<  std::endl;
      std::cout << "HEC Z locations: out" ;
      for ( int j = 0 ; j < m_n_depth_hec[i]; j++ ) 
	std::cout << m_depth_hec_out[i][j] << " " ;
      std::cout <<  std::endl;
    }
  }
}

void  
LArNumberHelper::print_fcal()
{

    std::cout << "LArNumberHelper::print_fcal" << std::endl;

  for ( unsigned int i=0; i < m_fcal_id->module_hash_max(); i++ ) {
    
    int pos_neg = abs( m_fcal_id->pos_neg (m_region_id_fcal[i]) );
    int module = m_fcal_id->module (m_region_id_fcal[i]);
    std::cout << "Module #" << i << " is : " <<  pos_neg
	      << " " << module << std::endl;

    std::cout << "channels : n_eta, n_phi, deta, dphi "
	      <<  m_n_eta_fcal[i] << " " << m_n_phi_fcal[i] << " "  
	      <<  m_deta_fcal[i] << " " << m_dphi_fcal[i] 
	      << std::endl;

    std::cout << "x_min x_max dx y_min y_max dy "
	      << std::endl;

    std::cout << m_x_min_fcal[i] << " " << m_x_max_fcal[i]
	      << " " << m_dx_fcal[i] << " "
              << m_y_min_fcal[i] << " " << m_y_max_fcal[i] 
	      << " " << m_dy_fcal[i] << std::endl;

    if ( m_n_depth_fcal[i] != 0 ) {
      std::cout << "FCAL Z locations : in" ;
      for ( int j = 0 ; j < m_n_depth_fcal[i]; j++ ) 
	std::cout << m_depth_fcal_in[i][j] << " " ;
      std::cout <<  std::endl;
      
      std::cout << "FCAL Z locations : out" ;
      for ( int j = 0 ; j < m_n_depth_fcal[i]; j++ ) 
	std::cout << m_depth_fcal_out[i][j] << " " ;
      std::cout <<  std::endl;
    }
  }
}

LArCellVolumes* 
LArNumberHelper::VolumeHelper()
{
  return  m_cellVolumes;
}

void 
LArNumberHelper::prepare_arrays()
{
  // Arrays are ordered by the region hash index.

  // em :  2*18 regions
  //            EM PS  1 per +- side
  //            EMB    5 per +- side
  //            EC PS  1 per +- side
  //            EMEC  11 per +- side

  int Ntot =  (int) m_em_id->region_hash_max();
  //std::cout << "LArNumberHelper::prepare_arrays for em.  Ntot = " 
  //	    << Ntot << std::endl;

  // these come from the identier package
  m_region_id_em.resize(Ntot);
  m_n_eta_em.resize(Ntot,0);
  m_n_phi_em.resize(Ntot,0);
  m_deta_em.resize(Ntot,(double) 0.);
  m_dphi_em.resize(Ntot, (double) 0.);
  m_eta_min_em.resize(Ntot,(double) 0.);
  m_eta_max_em.resize(Ntot,(double) 0.);
  m_phi_min_em.resize(Ntot,(double) 0.);
  m_phi_max_em.resize(Ntot,(double) 0.);
  m_sign_em.resize(Ntot,0);

  // 2 "facts of life" : 
  //   in H8, the EMB module is centered around phi=0
  //        = rotated by 1/2 module compared to Atlas
  //        compute the shift here, using the middle layer,
  //        and ignore the standard  DDDb value. 

  double phiEMBShift = 0.;

  if ( m_geometry == "H8" ) {
    Identifier id = m_em_id->region_id(1,2,0);
    IdentifierHash i = m_em_id->region_hash(id);
    double cellnb =  m_em_id->phi_max(id) + 1 -  m_em_id->phi_min(id);
    double cellsize = (double)  m_em_id->phiGranularity(i);
    phiEMBShift = cellnb * cellsize / 2. ;
  }  
  else {
    m_lar = m_iAccessSvc->getRecordsetPtr("BarrelGeometry",m_tag,m_node);
    if (m_lar->size()) {
      m_rec = (*m_lar)[0];
      phiEMBShift = -1.*m_rec->getDouble("PHIFIRST");
    }
  }

  //   in G3, the EMEC phi origin was the 1rst absorber, 
  //   in G4, it is defined as the 1rst electrode
  double phiEMECShift = 0.;
  if ( m_geometry == "Atlas" ) {
    Identifier id = m_em_id->region_id(2,2,0);
    IdentifierHash i = m_em_id->region_hash(id);
    double cellsize = (double)  m_em_id->phiGranularity(i);
    phiEMECShift = cellsize * (1./ 6.) ;
  }

  for ( unsigned int i=0; i < m_em_id->region_hash_max(); i++ ) {
    Identifier id = m_em_id->region_id ( i );
    m_region_id_em[i] = id;
    m_n_eta_em [i] =  m_em_id->eta_max(id) + 1;
    m_n_phi_em [i] =  m_em_id->phi_max(id) + 1;
    m_deta_em [i] = (double)  m_em_id->etaGranularity(i);
    m_dphi_em [i] = (double)  m_em_id->phiGranularity(i);
    int barrel_ec = abs( m_em_id->barrel_ec (id) );
    if (barrel_ec == 0) barrel_ec = 1;
    m_sign_em[i] = m_em_id->barrel_ec (id) /  barrel_ec;
    m_eta_min_em [i] = (double) (m_em_id->eta0(i) - 
	      m_em_id->eta_min(id)*m_em_id->etaGranularity(i));  // ideal !
    m_eta_max_em [i] = m_eta_min_em[i] + m_n_eta_em[i]*m_deta_em[i];
    m_phi_min_em [i] = m_em_id->phi0(i) - 
              m_em_id->phi_min(id)*m_em_id->phiGranularity(i);  // ideal !;
    m_phi_max_em [i] = m_phi_min_em[i] + m_n_phi_em[i]*m_dphi_em[i] -
              0.01*m_dphi_em[i];  // when = 2pi get rounding pb.

    // apply phi shifts mentioned above :

    if ( m_em_id->barrel_ec (id) == 1 ) {
      m_phi_min_em [i] = m_phi_min_em [i] - phiEMBShift ;
      m_phi_max_em [i] = m_phi_max_em [i] - phiEMBShift;
    }
    else if ( m_em_id->barrel_ec (id) == -1) {
      m_phi_min_em [i] = m_phi_min_em [i] + phiEMBShift ;
      m_phi_max_em [i] = m_phi_max_em [i] + phiEMBShift;
    }
    else if (m_em_id->barrel_ec (id) < 0) {
      m_phi_min_em [i] = m_phi_min_em [i] + phiEMECShift ;
      m_phi_max_em [i] = m_phi_max_em [i] + phiEMECShift;
    }
    else {
      m_phi_min_em [i] = m_phi_min_em [i] - phiEMECShift ;
      m_phi_max_em [i] = m_phi_max_em [i] - phiEMECShift;
    }
  }

  // these will come from the geometry numbers
  m_n_depth_em.resize(Ntot,0);
  m_depth_em_in.resize(Ntot);
  m_depth_em_out.resize(Ntot);

  // hec :  2*8 regions
  // ------------------

  Ntot =  (int) m_hec_id->region_hash_max();
  //std::cout << "LArNumberHelper::prepare_arrays for hec.  Ntot = " 
  //	    << Ntot << std::endl;

  // these come from the identier package
  m_region_id_hec.resize(Ntot);
  m_n_eta_hec.resize(Ntot,0);
  m_n_phi_hec.resize(Ntot,0);
  m_deta_hec.resize(Ntot,(double) 0.);
  m_dphi_hec.resize(Ntot, (double) 0.);
  m_eta_min_hec.resize(Ntot,(double) 0.);
  m_eta_max_hec.resize(Ntot,(double) 0.);
  m_phi_min_hec.resize(Ntot,(double) 0.);
  m_phi_max_hec.resize(Ntot,(double) 0.);
  m_sign_hec.resize(Ntot,0);

  for ( unsigned int i=0; i < m_hec_id->region_hash_max(); i++ ) {
    Identifier id = m_hec_id->region_id ( i );
    m_region_id_hec[i] = id;
    m_n_eta_hec [i] =  m_hec_id->eta_max(id) + 1;
    m_n_phi_hec [i] = m_hec_id->phi_max(id) + 1;
    m_deta_hec [i] = (double) m_hec_id->etaGranularity(i);
    m_dphi_hec [i] = (double) m_hec_id->phiGranularity(i);
    int pos_neg = abs( m_hec_id->pos_neg (id) );
    if (pos_neg == 0) pos_neg = 1;
    m_eta_min_hec [i] = (double)(m_hec_id->eta0(i) - 
        m_hec_id->eta_min(id)*m_hec_id->etaGranularity(i));  // ideal !
    m_eta_max_hec [i] = m_eta_min_hec[i] + m_n_eta_hec[i]*m_deta_hec[i];
    m_phi_min_hec [i] =  m_hec_id->phi0(i) - 
	      m_hec_id->phi_min(id)*m_hec_id->phiGranularity(i);  // ideal !;
    m_phi_max_hec[i] = m_phi_min_hec[i] + m_n_phi_hec[i]*m_dphi_hec[i] -
              0.01*m_dphi_hec[i];  // when = 2pi get rounding pb.;
    m_sign_hec[i] = m_hec_id->pos_neg (id) /  pos_neg;
  }

  // these will come from the geometry numbers
  m_n_depth_hec.resize(Ntot,0);
  m_depth_hec_in.resize(Ntot);
  m_depth_hec_out.resize(Ntot);
  
  // fcal :  2*3 regions
  // ---------------------
  // N.B. : FCAL is a mess, because information always came
  // from a specific class and FCAL_ID only knows about eta/phi

  Ntot =  (int) m_fcal_id->module_hash_max();
  //std::cout << "LArNumberHelper::prepare_arrays for fcal.  Ntot = " 
  //	    << Ntot << std::endl;

  // these come from the identier package
  m_region_id_fcal.resize(Ntot);
  m_n_eta_fcal.resize(Ntot,0);
  m_n_phi_fcal.resize(Ntot,0);
  m_deta_fcal.resize(Ntot,(double) 0.);
  m_dphi_fcal.resize(Ntot, (double) 0.);
  m_sign_fcal.resize(Ntot,0);
  m_n_depth_fcal.resize(Ntot,0);
  m_depth_fcal_in.resize(Ntot);
  m_depth_fcal_out.resize(Ntot);

  for ( unsigned int i=0; i < m_fcal_id->module_hash_max(); i++ ) {

    Identifier id = m_fcal_id->module_id ( i );
    m_region_id_fcal[i] = id;

    // these are in fact number of channels in eta/phi...
    m_n_eta_fcal [i] = m_fcal_id->eta_max(id) + 1;
    m_n_phi_fcal [i] = m_fcal_id->phi_max(id) + 1;
    m_deta_fcal [i] = (double)  m_fcal_id->etaGranularity(i);
    m_dphi_fcal [i] = (double)  m_fcal_id->phiGranularity(i);

    int pos_neg = abs( m_fcal_id->pos_neg (id) );
    m_sign_fcal[i] = pos_neg ? m_fcal_id->pos_neg (id) /  pos_neg : 0;
  }

  // these will come from  geometry numbers
  m_x_min_fcal.resize(Ntot,(double) 0.);
  m_x_max_fcal.resize(Ntot,(double) 0.);
  m_dx_fcal.resize(Ntot,(double) 0.);
  m_y_min_fcal.resize(Ntot,(double) 0.);
  m_y_max_fcal.resize(Ntot,(double) 0.);
  m_dy_fcal.resize(Ntot, (double) 0.);
  m_z_min_fcal.resize(Ntot,(double) 0.);
  m_z_max_fcal.resize(Ntot,(double) 0.);
  m_eta_min_fcal.resize(Ntot,(double) 0.);
  m_eta_max_fcal.resize(Ntot,(double) 0.);
  m_phi_min_fcal.resize(Ntot,(double) 0.);
  m_phi_max_fcal.resize(Ntot,(double) 0.);
  
  //std::cout << "LArNumberHelper::prepare_arrays finished " 
  //          << std::endl;
}

void 
LArNumberHelper::db_nb_em()
{
  // ---- Defaults are the values taken from Nova a long time
  //      ago -> this is where their name comes from
  // ---- Now all overwritten by DB value

  //std::cout << " ----- in db_nb_em tags are : " << m_tag << " " << m_node << std::endl;

  // PS
  // m_emb_psin = 141.23*Gaudi::Units::cm;    // this is the TDR number 1385 mm + 27.3 mm   
  //  ----> overwritten

  m_lar = m_iAccessSvc->getRecordsetPtr("PresamplerGeometry","ATLAS-00","ATLAS");

  if (m_lar->size()) {
    m_rec = (*m_lar)[0];
    m_emb_psin = m_rec->getDouble("RACTIVE")*Gaudi::Units::cm;
  }

  // ACCG :
  // m_accg_rin_ac = 144.73*Gaudi::Units::cm; // 1385mm + 27.3mm + 35mm
  // m_accg_rout_ac = 200.35*Gaudi::Units::cm; // end of active material
  //  ----> overwritten
  m_lar = m_iAccessSvc->getRecordsetPtr("BarrelGeometry",m_tag,m_node);
  if (m_lar->size()) {
    m_rec = (*m_lar)[0];  
    m_accg_rin_ac = m_rec->getDouble("RMIN") *Gaudi::Units::cm;
    m_accg_rout_ac = m_rec->getDouble("RMAX") *Gaudi::Units::cm;
  }

  // ACCO :
  m_acco_rmx12.resize (8,(double) 0.); 
  // m_acco_rmx12[0] = 158.6*Gaudi::Units::cm;
  // m_acco_rmx12[1] = 158.6*Gaudi::Units::cm;
  // m_acco_rmx12[2] = 157.07*Gaudi::Units::cm;
  // m_acco_rmx12[3] = 157.07*Gaudi::Units::cm;
  // m_acco_rmx12[4] = 154.83*Gaudi::Units::cm;
  // m_acco_rmx12[5] = 154.83*Gaudi::Units::cm;
  // m_acco_rmx12[6] = 153.23*Gaudi::Units::cm;
  // m_acco_rmx12[7] = 153.23*Gaudi::Units::cm;
  //  ----> overwritten
  m_lar = m_iAccessSvc->getRecordsetPtr("BarrelLongDiv",m_tag,m_node);
  if (m_lar->size()) {
    m_rec = (*m_lar)[0];  
    m_acco_rmx12[0] = m_rec->getDouble("RMX12_0")*Gaudi::Units::cm;
    m_acco_rmx12[1] = m_rec->getDouble("RMX12_1")*Gaudi::Units::cm;
    m_acco_rmx12[2] = m_rec->getDouble("RMX12_2")*Gaudi::Units::cm;
    m_acco_rmx12[3] = m_rec->getDouble("RMX12_3")*Gaudi::Units::cm;
    m_acco_rmx12[4] = m_rec->getDouble("RMX12_4")*Gaudi::Units::cm;
    m_acco_rmx12[5] = m_rec->getDouble("RMX12_5")*Gaudi::Units::cm;
    m_acco_rmx12[6] = m_rec->getDouble("RMX12_6")*Gaudi::Units::cm;
    m_acco_rmx12[7] = m_rec->getDouble("RMX12_7")*Gaudi::Units::cm;
  }

  m_acco_ee12.resize (8,(double) 0.); 
  // m_acco_ee12[0] = 0.;
  // m_acco_ee12[1] = 0.275;
  // m_acco_ee12[2] = 0.6;
  // m_acco_ee12[3] = 0.799;
  // m_acco_ee12[4] = 0.8;
  // m_acco_ee12[5] = 1.1312;
  // m_acco_ee12[6] = 1.3531;
  // m_acco_ee12[7] = 1.6;
  //  ----> overwritten
  m_lar = m_iAccessSvc->getRecordsetPtr("BarrelLongDiv",m_tag,m_node);
  if (m_lar->size()) {
    m_rec = (*m_lar)[0];  
    m_acco_ee12[0] = m_rec->getDouble("EE_0");
    m_acco_ee12[1] = m_rec->getDouble("EE_1");
    m_acco_ee12[2] = m_rec->getDouble("EE_2");
    m_acco_ee12[3] = m_rec->getDouble("EE_3");
    m_acco_ee12[4] = m_rec->getDouble("EE_4");
    m_acco_ee12[5] = m_rec->getDouble("EE_5");
    m_acco_ee12[6] = m_rec->getDouble("EE_6");
    m_acco_ee12[7] = m_rec->getDouble("EE_7");
  }

  m_acco_rmx23.resize (53,(double) 0.); 
  /*
    m_acco_rmx23[0] = 192.83*Gaudi::Units::cm;  ... up to :
    m_acco_rmx23[52] = 178.89*Gaudi::Units::cm;
  */
  //  ----> overwritten
  m_lar = m_iAccessSvc->getRecordsetPtr("BarrelLongDiv",m_tag,m_node);
  if (m_lar->size()) {
    m_rec = (*m_lar)[0];  
    m_acco_rmx23[0] = m_rec->getDouble("RMX23_0")*Gaudi::Units::cm;
    m_acco_rmx23[1] = m_rec->getDouble("RMX23_1")*Gaudi::Units::cm;
    m_acco_rmx23[2] = m_rec->getDouble("RMX23_2")*Gaudi::Units::cm;
    m_acco_rmx23[3] = m_rec->getDouble("RMX23_3")*Gaudi::Units::cm;
    m_acco_rmx23[4] = m_rec->getDouble("RMX23_4")*Gaudi::Units::cm;
    m_acco_rmx23[5] = m_rec->getDouble("RMX23_5")*Gaudi::Units::cm;
    m_acco_rmx23[6] = m_rec->getDouble("RMX23_6")*Gaudi::Units::cm;
    m_acco_rmx23[7] = m_rec->getDouble("RMX23_7")*Gaudi::Units::cm;
    m_acco_rmx23[8] = m_rec->getDouble("RMX23_8")*Gaudi::Units::cm;
    m_acco_rmx23[9] = m_rec->getDouble("RMX23_9")*Gaudi::Units::cm;
    m_acco_rmx23[10] = m_rec->getDouble("RMX23_10")*Gaudi::Units::cm;
    m_acco_rmx23[11] = m_rec->getDouble("RMX23_11")*Gaudi::Units::cm;
    m_acco_rmx23[12] = m_rec->getDouble("RMX23_12")*Gaudi::Units::cm;
    m_acco_rmx23[13] = m_rec->getDouble("RMX23_13")*Gaudi::Units::cm;
    m_acco_rmx23[14] = m_rec->getDouble("RMX23_14")*Gaudi::Units::cm;
    m_acco_rmx23[15] = m_rec->getDouble("RMX23_15")*Gaudi::Units::cm;
    m_acco_rmx23[16] = m_rec->getDouble("RMX23_16")*Gaudi::Units::cm;
    m_acco_rmx23[17] = m_rec->getDouble("RMX23_17")*Gaudi::Units::cm;
    m_acco_rmx23[18] = m_rec->getDouble("RMX23_18")*Gaudi::Units::cm;
    m_acco_rmx23[19] = m_rec->getDouble("RMX23_19")*Gaudi::Units::cm;
    m_acco_rmx23[20] = m_rec->getDouble("RMX23_20")*Gaudi::Units::cm;
    m_acco_rmx23[21] = m_rec->getDouble("RMX23_21")*Gaudi::Units::cm;
    m_acco_rmx23[22] = m_rec->getDouble("RMX23_22")*Gaudi::Units::cm;
    m_acco_rmx23[23] = m_rec->getDouble("RMX23_23")*Gaudi::Units::cm;
    m_acco_rmx23[24] = m_rec->getDouble("RMX23_24")*Gaudi::Units::cm;
    m_acco_rmx23[25] = m_rec->getDouble("RMX23_25")*Gaudi::Units::cm;
    m_acco_rmx23[26] = m_rec->getDouble("RMX23_26")*Gaudi::Units::cm;
    m_acco_rmx23[27] = m_rec->getDouble("RMX23_27")*Gaudi::Units::cm;
    m_acco_rmx23[28] = m_rec->getDouble("RMX23_28")*Gaudi::Units::cm;
    m_acco_rmx23[29] = m_rec->getDouble("RMX23_29")*Gaudi::Units::cm;
    m_acco_rmx23[30] = m_rec->getDouble("RMX23_30")*Gaudi::Units::cm;
    m_acco_rmx23[31] = m_rec->getDouble("RMX23_31")*Gaudi::Units::cm;
    m_acco_rmx23[32] = m_rec->getDouble("RMX23_32")*Gaudi::Units::cm;
    m_acco_rmx23[33] = m_rec->getDouble("RMX23_33")*Gaudi::Units::cm;
    m_acco_rmx23[34] = m_rec->getDouble("RMX23_34")*Gaudi::Units::cm;
    m_acco_rmx23[35] = m_rec->getDouble("RMX23_35")*Gaudi::Units::cm;
    m_acco_rmx23[36] = m_rec->getDouble("RMX23_36")*Gaudi::Units::cm;
    m_acco_rmx23[37] = m_rec->getDouble("RMX23_37")*Gaudi::Units::cm;
    m_acco_rmx23[38] = m_rec->getDouble("RMX23_38")*Gaudi::Units::cm;
    m_acco_rmx23[39] = m_rec->getDouble("RMX23_39")*Gaudi::Units::cm;
    m_acco_rmx23[40] = m_rec->getDouble("RMX23_40")*Gaudi::Units::cm;
    m_acco_rmx23[41] = m_rec->getDouble("RMX23_41")*Gaudi::Units::cm;
    m_acco_rmx23[42] = m_rec->getDouble("RMX23_42")*Gaudi::Units::cm;
    m_acco_rmx23[43] = m_rec->getDouble("RMX23_43")*Gaudi::Units::cm;
    m_acco_rmx23[44] = m_rec->getDouble("RMX23_44")*Gaudi::Units::cm;
    m_acco_rmx23[45] = m_rec->getDouble("RMX23_45")*Gaudi::Units::cm;
    m_acco_rmx23[46] = m_rec->getDouble("RMX23_46")*Gaudi::Units::cm;
    m_acco_rmx23[47] = m_rec->getDouble("RMX23_47")*Gaudi::Units::cm;
    m_acco_rmx23[48] = m_rec->getDouble("RMX23_48")*Gaudi::Units::cm;
    m_acco_rmx23[49] = m_rec->getDouble("RMX23_49")*Gaudi::Units::cm;
    m_acco_rmx23[50] = m_rec->getDouble("RMX23_50")*Gaudi::Units::cm;
    m_acco_rmx23[51] = m_rec->getDouble("RMX23_51")*Gaudi::Units::cm;
    m_acco_rmx23[52] = m_rec->getDouble("RMX23_52")*Gaudi::Units::cm;
  }

  // ENDG
  // m_endg_zorig = 369.1*Gaudi::Units::cm; // this is the NOVA/Oracle number
  // m_emb_iwout = 422.7*Gaudi::Units::cm;  // 369.1*Gaudi::Units::cm + 53.6*Gaudi::Units::cm is the end of the active part
  // m_emec_out = 422.7*Gaudi::Units::cm;  // 369.1*Gaudi::Units::cm + 53.6*Gaudi::Units::cm is the end of the active part
  m_lar = m_iAccessSvc->getRecordsetPtr("EmecGeometry",m_tag,m_node);
  if (m_lar->size()) {
    m_rec = (*m_lar)[0];
    m_endg_zorig = m_rec->getDouble("Z1")*Gaudi::Units::cm;
    double epaisseurTotale =  m_rec->getDouble("ETOT")*Gaudi::Units::cm;
    m_emb_iwout = m_endg_zorig + epaisseurTotale;
    m_emec_out  = m_endg_zorig + epaisseurTotale;

  } 

  // Cryostat
  // m_emec_psin = 362.5*Gaudi::Units::cm; // notch in cold wall of cryostat
  if ( m_geometry == "Atlas" ) {
    DecodeVersionKey detectorKeyAtl = DecodeVersionKey(m_geoModelSvc, "ATLAS");
    m_lar = m_iAccessSvc->getRecordsetPtr("PresamplerPosition",detectorKeyAtl.tag(),detectorKeyAtl.node());
  }
  else {
    m_lar = m_iAccessSvc->getRecordsetPtr("PresamplerPosition",m_tag,m_node);
  }
  if (m_lar->size()) {
    m_rec = (*m_lar)[0];
    m_emec_psin = m_rec->getDouble("ZPOS")*Gaudi::Units::cm;
  } 

  // ESEP
  m_esep_iw23.resize(7, (double) 0.); 
  // m_esep_iw23[0] = 413.934*Gaudi::Units::cm;
  // m_esep_iw23[1] = 412.518*Gaudi::Units::cm;
  // m_esep_iw23[2] = 411.792*Gaudi::Units::cm;
  // m_esep_iw23[3] = 409.545*Gaudi::Units::cm;
  // m_esep_iw23[4] = 407.987*Gaudi::Units::cm;
  // m_esep_iw23[5] = 407.510*Gaudi::Units::cm;
  // m_esep_iw23[6] = 404.730*Gaudi::Units::cm;
  //  ----> overwritten
  m_lar = m_iAccessSvc->getRecordsetPtr("EmecSamplingSep",m_tag,m_node);
  if (m_lar->size()) {
    m_rec = (*m_lar)[0];  
    m_esep_iw23[0] = m_rec->getDouble("ZIW_0")*Gaudi::Units::cm;
    m_esep_iw23[1] = m_rec->getDouble("ZIW_1")*Gaudi::Units::cm;
    m_esep_iw23[2] = m_rec->getDouble("ZIW_2")*Gaudi::Units::cm;
    m_esep_iw23[3] = m_rec->getDouble("ZIW_3")*Gaudi::Units::cm;
    m_esep_iw23[4] = m_rec->getDouble("ZIW_4")*Gaudi::Units::cm;
    m_esep_iw23[5] = m_rec->getDouble("ZIW_5")*Gaudi::Units::cm;
    m_esep_iw23[6] = m_rec->getDouble("ZIW_6")*Gaudi::Units::cm;
}

  // m_esep_zsep12 = 378.398*Gaudi::Units::cm;   
  // Note that in the gometryDB this is an array, but
  // of very similar numbers -> Zebra was using 1rst value only
  //  ----> overwritten
  m_lar = m_iAccessSvc->getRecordsetPtr("EmecSamplingSep",m_tag,m_node);
  if (m_lar->size()) {
    m_rec = (*m_lar)[0];  
    m_esep_zsep12 = m_rec->getDouble("ZSEP12_0")*Gaudi::Units::cm;
  }

  m_esep_zsep23.resize(22, (double) 0.); 
  /*
  m_esep_zsep23 [0] = 999.999*Gaudi::Units::cm;       // inheritance from Zebra.
  m_esep_zsep23 [1] = 999.999*Gaudi::Units::cm;       // will be skipped in hard_em
  m_esep_zsep23 [2] = 413.205*Gaudi::Units::cm;   ... up to :
  m_esep_zsep23 [21] = 401.153*Gaudi::Units::cm;
  */
  //  ----> overwritten
  m_lar = m_iAccessSvc->getRecordsetPtr("EmecSamplingSep",m_tag,m_node);
  if (m_lar->size()) {
    m_rec = (*m_lar)[0];  
    m_esep_zsep23 [0] = m_rec->getDouble("ZSEP23_0")*Gaudi::Units::cm;
    m_esep_zsep23 [1] = m_rec->getDouble("ZSEP23_1")*Gaudi::Units::cm;
    m_esep_zsep23 [2] = m_rec->getDouble("ZSEP23_2")*Gaudi::Units::cm;
    m_esep_zsep23 [3] = m_rec->getDouble("ZSEP23_3")*Gaudi::Units::cm;
    m_esep_zsep23 [4] = m_rec->getDouble("ZSEP23_4")*Gaudi::Units::cm;
    m_esep_zsep23 [5] = m_rec->getDouble("ZSEP23_5")*Gaudi::Units::cm;
    m_esep_zsep23 [6] = m_rec->getDouble("ZSEP23_6")*Gaudi::Units::cm;
    m_esep_zsep23 [7] = m_rec->getDouble("ZSEP23_7")*Gaudi::Units::cm;
    m_esep_zsep23 [8] = m_rec->getDouble("ZSEP23_8")*Gaudi::Units::cm;
    m_esep_zsep23 [9] = m_rec->getDouble("ZSEP23_9")*Gaudi::Units::cm;
    m_esep_zsep23 [10] = m_rec->getDouble("ZSEP23_10")*Gaudi::Units::cm;
    m_esep_zsep23 [11] = m_rec->getDouble("ZSEP23_11")*Gaudi::Units::cm;
    m_esep_zsep23 [12] = m_rec->getDouble("ZSEP23_12")*Gaudi::Units::cm;
    m_esep_zsep23 [13] = m_rec->getDouble("ZSEP23_13")*Gaudi::Units::cm;
    m_esep_zsep23 [14] = m_rec->getDouble("ZSEP23_14")*Gaudi::Units::cm;
    m_esep_zsep23 [15] = m_rec->getDouble("ZSEP23_15")*Gaudi::Units::cm;
    m_esep_zsep23 [16] = m_rec->getDouble("ZSEP23_16")*Gaudi::Units::cm;
    m_esep_zsep23 [17] = m_rec->getDouble("ZSEP23_17")*Gaudi::Units::cm;
    m_esep_zsep23 [18] = m_rec->getDouble("ZSEP23_18")*Gaudi::Units::cm;
    m_esep_zsep23 [19] = m_rec->getDouble("ZSEP23_19")*Gaudi::Units::cm;
    m_esep_zsep23 [20] = m_rec->getDouble("ZSEP23_20")*Gaudi::Units::cm;
    m_esep_zsep23 [21] = m_rec->getDouble("ZSEP23_21")*Gaudi::Units::cm;
  }

}

void 
LArNumberHelper::db_nb_hec()  
{
  
  // ---- Set default :
  /*  
  m_hec_in0 = 427.70*Gaudi::Units::cm;  // z_start
  m_hec_in1 = 455.75*Gaudi::Units::cm;  // z_start+ 28.05*Gaudi::Units::cm
  m_hec_in2 = 513.40*Gaudi::Units::cm;  // z_start + 28.05*Gaudi::Units::cm + 26.8*Gaudi::Units::cm + 26.8*Gaudi::Units::cm + 4.05*Gaudi::Units::cm
  m_hec_in3 = 562.70*Gaudi::Units::cm;  // z_start + 28.05*Gaudi::Units::cm + 26.8*Gaudi::Units::cm + 26.8*Gaudi::Units::cm + 4.05*Gaudi::Units::cm 
                          //        + 25.9*Gaudi::Units::cm + 23.4*Gaudi::Units::cm
  m_hec_gap = 4.05*Gaudi::Units::cm;    // gap between the two HEC wheels  
  
  // Comment from Sven Menke :
  // I don't know why the the Nova Z_end is 2.5cm more, but the active
  // volume must be the sum of all blocks plus the gap - thus it's 609.5*cm
  m_hec_out = 609.5*Gaudi::Units::cm;  // z_end - 2.5*Gaudi::Units::cm (or z_orig + all blocks)
  */

  //std::cout << " ----- in db_nb_hec tags are : " << m_tag << " " << m_node << std::endl;

 
  // ---- Overwrite with real NOVA/Oracle numbers :
  
  m_lar = m_iAccessSvc->getRecordsetPtr("HadronicEndcap",m_tag,m_node);
  
  if (m_lar->size()) {
    m_rec = (*m_lar)[0];
    
    // HEC0 is Block0     
    // Block0 = 1.25 cm Front Plate + 
    //          8 times (0.85 cm LAr gap + 2.50 cm Plate) = 28.05 cm 
    double Block0 = ( m_rec->getDouble("PLATE_0")/2. 
		      + 8*(m_rec->getDouble("LARG") + m_rec->getDouble("PLATE_0")))*Gaudi::Units::cm;
    
    // HEC1 is Block1 + Block2
    // Block1 = 8 times (0.85 cm LAr gap + 2.50 cm Plate) 
    //         = 26.80 cm
    double Block1 = 8*(m_rec->getDouble("LARG") + m_rec->getDouble("PLATE_0"))*Gaudi::Units::cm;
    double Block2 = Block1 ;

    // Gap     = 4.05 cm
    m_hec_gap = m_rec->getDouble("GAPWHL") *Gaudi::Units::cm;
    
    // HEC2 is  Block 3 + Block 4
    // Block3 = 2.5 cm Front Plate + 
    //      4 times (0.85 cm LAr gap + 5.00 cm Plate) = 25.90 cm 
    double Block3 =  ( m_rec->getDouble("PLATE_1")/2. 
		       + 4*(m_rec->getDouble("LARG") + m_rec->getDouble("PLATE_1")))*Gaudi::Units::cm;
    
    // Block4 = 4 times (0.85 cm LAr gap + 5.00 cm Plate) = 23.40 cm 
    double Block4 = 4*(m_rec->getDouble("LARG") + m_rec->getDouble("PLATE_1"))*Gaudi::Units::cm;
    
    // HEC3 is  Block 5 + Block 6
    double Block5 = Block4 ;
    double Block6 = Block4;
    
    double zstart =  m_rec->getDouble("ZSTART") *Gaudi::Units::cm;
    
    m_hec_in0 = zstart;
    m_hec_in1 = m_hec_in0 + Block0  ;
    m_hec_in2 = m_hec_in1 + Block1 + Block2 + m_hec_gap ;
    m_hec_in3 = m_hec_in2 + Block3 + Block4;
    m_hec_out = m_hec_in3 + Block5 + Block6;
  }
}

void 
LArNumberHelper::db_nb_fcal()
{
  //    decode_fcal();    //  FCAL   3 per +- side
  //std::cout << " ----- in db_nb_fcal tags are : " << m_tag << " " << m_node << std::endl;

}

void
LArNumberHelper::cart_to_cyl (  double x,    double y,    double z,
                                double& eta, double& phi, double& r )
{
  HepGeom::Point3D<double> pt(x,y,z);
  eta = pt.eta();
  phi = pt.phi();
  r = pt.perp();
}

double
LArNumberHelper::eta ( double r, double z )
{
  return -log(tan(atan2(r,z)/2));
}

  
void  
LArNumberHelper::hard_em()
{

  // Numbers are taken from DB, but still have to define which ones are used
  // for what !

  std::vector <double> z_loc_in;
  std::vector <double> z_loc_out;

  for ( unsigned int i=0; i < m_em_id->region_hash_max(); i++ ) {

    int barrel_ec = abs( m_em_id->barrel_ec (m_region_id_em[i]) );
    int samp = m_em_id->sampling (m_region_id_em[i]);
    int region = m_em_id->region (m_region_id_em[i]);
    //int pos_neg = m_em_id->barrel_ec (m_region_id_em[i]);

    // Default
    m_n_depth_em [i] = 0;
    z_loc_in.resize( 0 );
    z_loc_out.resize( 0 );

    // Barrel 
    if ( barrel_ec == 1 )  {

      if ( samp == 0) {
	m_n_depth_em [i] = 1;
	z_loc_in.resize( m_n_depth_em [i], (double) 0. );
	z_loc_out.resize( m_n_depth_em [i], (double) 0. );

	z_loc_in [0] = m_emb_psin;
	z_loc_out [0] = m_accg_rin_ac;

      } 

      else if ( samp == 1) {
	m_n_depth_em [i] =  m_n_eta_em [i];
	z_loc_in.resize( m_n_depth_em [i], (double) 0. );
	z_loc_out.resize( m_n_depth_em [i], (double) 0. );

	for (int n=0; n<m_n_depth_em [i]; n++) {
	  z_loc_in[n] = m_accg_rin_ac;
	  double where = m_eta_min_em[i]+n*m_deta_em[i];
	  if ( where < m_acco_ee12[1] ) z_loc_out [n] = m_acco_rmx12[0];
	  else if ( where < m_acco_ee12[2] ) z_loc_out [n] = m_acco_rmx12[1];
	  else if ( where < m_acco_ee12[4] ) z_loc_out [n] = m_acco_rmx12[2];
	  else if ( where < m_acco_ee12[5] ) z_loc_out [n] = m_acco_rmx12[4];
	  else if ( where < m_acco_ee12[6] ) z_loc_out [n] = m_acco_rmx12[5];
	  else z_loc_out[n] = m_acco_rmx12[6];
	}

      }

      else if ( samp == 2) {
	m_n_depth_em [i] =  m_n_eta_em [i];
	z_loc_in.resize( m_n_depth_em [i], (double) 0. );
	z_loc_out.resize( m_n_depth_em [i], (double) 0. );

	for (int n=0; n<m_n_depth_em [i]; n++) {
	  double where = m_eta_min_em[i]+(n+0.5)*m_deta_em[i];
	  if ( where <  m_acco_ee12[1] ) z_loc_in [n] = m_acco_rmx12[0];
	  else if ( where < m_acco_ee12[2] ) z_loc_in [n] = m_acco_rmx12[1];
	  else if ( where < m_acco_ee12[4] ) z_loc_in [n] = m_acco_rmx12[2];
	  else if ( where < m_acco_ee12[5] ) z_loc_in [n] = m_acco_rmx12[4];
	  else if ( where < m_acco_ee12[6] ) z_loc_in [n] = m_acco_rmx12[5];
	  else z_loc_in[n] = m_acco_rmx12[6];
	}

	for (int n=0; n<m_n_depth_em [i]; n++) {
	  if (n<53) z_loc_out[n] = m_acco_rmx23[n];
	  else z_loc_out[n] = m_acco_rmx23[52];
	  // this is wrong for the last 3 channels (z edge) !
	}
      }

      else if ( samp == 3) {
	m_n_depth_em [i] =  m_n_eta_em [i];
	z_loc_in.resize( m_n_depth_em [i], (double) 0. );
	z_loc_out.resize( m_n_depth_em [i], (double) 0. );
	for (int n=0; n<m_n_depth_em [i]; n++) {
	  z_loc_out[n] =  m_accg_rout_ac;
	      // this is wrong for the last 2 channels (z edge) !
	  if (2*n<52) z_loc_in[n] = 
			(m_acco_rmx23[2*n]+m_acco_rmx23[2*n+1])/2.;
	  else z_loc_in[n] = m_acco_rmx23[52];          
	}
      }
    }

    // EMEC-OW  : numbers are from RD

    if ( barrel_ec == 2 )  {

      if ( samp == 0 ) {
	m_n_depth_em [i] = 1;
	z_loc_in.resize( m_n_depth_em [i], (double) 0.);
	z_loc_out.resize( m_n_depth_em [i], (double) 0.);
	z_loc_in [0] = m_emec_psin;
	z_loc_out [0] = m_endg_zorig;
      }

      if ( samp == 1 ) {
	m_n_depth_em [i] = 1;
      z_loc_in.resize( m_n_depth_em [i] , (double) 0.);
      z_loc_out.resize( m_n_depth_em [i] , (double) 0.);
      z_loc_in [0] = m_endg_zorig;
      z_loc_out [0] = m_esep_zsep12;
      }

      if ( samp == 2 && region == 0 ) {
	m_n_depth_em [i] = 1;
	z_loc_in.resize( m_n_depth_em [i] );
	z_loc_out.resize( m_n_depth_em [i] );
	z_loc_in [0] = m_esep_zsep12;
	z_loc_out [0] = m_emec_out;
      }

      if ( samp == 2 && region != 0 ) {
	m_n_depth_em [i] =  m_n_eta_em [i];
	z_loc_in.resize( m_n_depth_em [i], (double) 0.);
	z_loc_out.resize( m_n_depth_em [i], (double) 0.);
	for (int n=0; n<3; n++) {
	  z_loc_in [n] = m_esep_zsep12;
	  z_loc_out [n] = m_emec_out;
	}
	for (int n=3; n<m_n_depth_em [i]; n++) {
	  z_loc_in [n] = m_esep_zsep12;
	  int num = (n-3)/2;
	  if (num<20) z_loc_out [n] = m_esep_zsep23[num+2];
	  else z_loc_out [n] = m_esep_zsep23[22];
	}
      }

      if ( samp == 3 ) {
	m_n_depth_em [i] =  m_n_eta_em [i];
	z_loc_in.resize( m_n_depth_em [i], (double) 0.);
	z_loc_out.resize( m_n_depth_em [i], (double) 0.);
	for (int n=0; n<m_n_depth_em [i]; n++) {
	  z_loc_out [n] = m_emec_out;
	  if (n<20) z_loc_in [n] = m_esep_zsep23[n+2];
	  else z_loc_in [n] = m_esep_zsep23[22];
	}
      }
    }

    // EMEC-IW

    if ( barrel_ec == 3 )  {

      if ( samp == 1 ) {
	m_n_depth_em [i] =  m_n_eta_em [i];
	z_loc_in.resize( m_n_depth_em [i], (double) 0.);
	z_loc_out.resize( m_n_depth_em [i], (double) 0.);
	for (int n=0; n<m_n_depth_em [i]; n++) {
	  z_loc_in [n] = m_endg_zorig;
	  if (n<7) z_loc_out [n] = m_esep_iw23[n];
	  else z_loc_out [n] = m_esep_iw23[6];
	}
      }
      else if ( samp == 2 ) {
	m_n_depth_em [i] =  m_n_eta_em [i];
	z_loc_in.resize( m_n_depth_em [i], (double) 0.);
	z_loc_out.resize( m_n_depth_em [i], (double) 0.);
	for (int n=0; n<m_n_depth_em [i]; n++) {
	  if (n<7) z_loc_in [n] = m_esep_iw23[n];
	  else z_loc_in [n] = m_esep_iw23[6];
	  z_loc_out [n] = m_emb_iwout;
	}
      }
    }

    // That's it : store result in official arrays 
    m_depth_em_in [i] = z_loc_in ;
    m_depth_em_out [i] = z_loc_out;

  }
  //std::cout << "LArNumberHelper::hard_em finished " << std::endl;

}

void  
LArNumberHelper::hard_hec()
{

  // Numbers are taken from DB, but still have to define which ones are used
  // for what !

  std::vector <double> z_loc_in;
  std::vector <double> z_loc_out;

  for ( unsigned int i=0; i < m_hec_id->region_hash_max(); i++ ) {

    int samp = m_hec_id->sampling (m_region_id_hec[i]);
    //int region = m_hec_id->region (m_region_id_hec[i]);
    //int pos_neg = m_hec_id->pos_neg (m_region_id_hec[i]);

    m_n_depth_hec [i] = 1;
    z_loc_in.resize( 1, (double) 0.);
    z_loc_out.resize( 1, (double) 0.);

    if      ( samp == 0 ) {
      z_loc_in [0] = m_hec_in0;
      z_loc_out [0] = m_hec_in1;

    }
    else if ( samp == 1 ) {
      z_loc_in [0] = m_hec_in1;
      z_loc_out [0] = m_hec_in2-m_hec_gap;

    }
    else if ( samp == 2 ) {
      z_loc_in [0] = m_hec_in2;
      z_loc_out [0] = m_hec_in3;

    }
    else if ( samp == 3 ) {
      z_loc_in [0] = m_hec_in3;
      z_loc_out [0] = m_hec_out;
    }

    // that's it :
    m_depth_hec_in [i] = z_loc_in ;
    m_depth_hec_out [i] = z_loc_out;
  }
}

void  
LArNumberHelper::hard_fcal()
{

  // Numbers are taken from DB, but still have to define which ones are used
  // for what !

  std::vector <double> z_loc_in;
  z_loc_in.resize( 1, (double) 0.);
  std::vector <double> z_loc_out;
  z_loc_out.resize( 1, (double) 0.);

  // x and y are taken from drawings in the TDR
  for ( unsigned int i=0; i < m_fcal_id->module_hash_max(); i++ ) {
    m_x_min_fcal [i] = 8.6*Gaudi::Units::cm;
    m_x_max_fcal [i] = 47.5*Gaudi::Units::cm;
    m_y_min_fcal [i] = 8.6;
    m_y_max_fcal [i] = 47.5*Gaudi::Units::cm;
    m_phi_min_fcal[i] = 0.;
    m_phi_max_fcal[i] = 6.28;   // when too close to 2pi pb

    m_n_depth_fcal [i] = 1;
    
    int mod = m_fcal_id->module (m_region_id_fcal[i]);
    //int pos_neg = m_fcal_id->pos_neg (m_region_id_fcal[i]);
    
    if ( mod == 1 ) {
      m_dx_fcal [i] = 3.*Gaudi::Units::cm;
      m_dy_fcal [i] = 2.598*Gaudi::Units::cm;
      z_loc_in [0] = 466.85*Gaudi::Units::cm;
      z_loc_out [0] = z_loc_in [0]+45.*Gaudi::Units::cm;

    }
    else if ( mod == 2 ) {
      m_dx_fcal [i] = 3.272*Gaudi::Units::cm;
      m_dy_fcal [i] = 4.25*Gaudi::Units::cm;
      z_loc_in [0] = 512.3*Gaudi::Units::cm;
      z_loc_out [0] = z_loc_in [0]+45.*Gaudi::Units::cm;

    }
    else if ( mod == 3 ) {
      m_dx_fcal [i] = 5.4*Gaudi::Units::cm;
      m_dy_fcal [i] = 4.677*Gaudi::Units::cm;
      z_loc_in [0] = 559.75*Gaudi::Units::cm;
      z_loc_out [0] = z_loc_in [0]+45.*Gaudi::Units::cm;

    }
    else  {
      m_dx_fcal [i] = 0.;
      m_dy_fcal [i] = 0.;
      z_loc_in [0] = 0.;
      z_loc_out [0] = z_loc_in [0]+45.*Gaudi::Units::cm;
    }

    m_z_min_fcal [i] = z_loc_in [0];
    m_z_max_fcal [i] = m_z_min_fcal [i] + 45.*Gaudi::Units::cm ;

    double z = m_z_min_fcal [i];
    double r = m_x_max_fcal [i];
    m_eta_min_fcal[i] = -log(tan(atan2(r,z)/2));
    z = m_z_max_fcal [i];
    r = m_x_min_fcal [i];
    m_eta_max_fcal[i] = -log(tan(atan2(r,z)/2));

    m_depth_fcal_in [i] =  z_loc_in ;
    m_depth_fcal_out [i] = z_loc_out;
    
  }
}

// Parameters needed for sagging
void   
LArNumberHelper::sagging_param( std::vector<double>& Rhocen, std::vector<double>& Sag )
{
  Rhocen.clear();
  Sag.clear();

  m_lar = m_iAccessSvc->getRecordsetPtr("BarrelGeometry",m_tag,m_node);
  
  if (m_lar->size()) {
    m_rec = (*m_lar)[0];

    Rhocen.push_back(m_rec->getDouble("RHOCEN_0")*Gaudi::Units::cm);
    Rhocen.push_back(m_rec->getDouble("RHOCEN_1")*Gaudi::Units::cm);
    Rhocen.push_back(m_rec->getDouble("RHOCEN_2")*Gaudi::Units::cm);
    Rhocen.push_back(m_rec->getDouble("RHOCEN_3")*Gaudi::Units::cm);
    Rhocen.push_back(m_rec->getDouble("RHOCEN_4")*Gaudi::Units::cm);
    Rhocen.push_back(m_rec->getDouble("RHOCEN_5")*Gaudi::Units::cm);
    Rhocen.push_back(m_rec->getDouble("RHOCEN_6")*Gaudi::Units::cm);
    Rhocen.push_back(m_rec->getDouble("RHOCEN_7")*Gaudi::Units::cm);
    Rhocen.push_back(m_rec->getDouble("RHOCEN_8")*Gaudi::Units::cm);
    Rhocen.push_back(m_rec->getDouble("RHOCEN_9")*Gaudi::Units::cm);
    Rhocen.push_back(m_rec->getDouble("RHOCEN_10")*Gaudi::Units::cm);
    Rhocen.push_back(m_rec->getDouble("RHOCEN_11")*Gaudi::Units::cm);
    Rhocen.push_back(m_rec->getDouble("RHOCEN_12")*Gaudi::Units::cm);
    Rhocen.push_back(m_rec->getDouble("RHOCEN_13")*Gaudi::Units::cm);
    Rhocen.push_back(m_rec->getDouble("RHOCEN_14")*Gaudi::Units::cm);
  }
 
  m_lar = m_iAccessSvc->getRecordsetPtr("BarrelSagging",m_tag,m_node);
  
  if (m_lar->size()) {
    m_rec = (*m_lar)[0];

    Sag.push_back(m_rec->getDouble("SAG_0")*Gaudi::Units::cm);
    Sag.push_back(m_rec->getDouble("SAG_1")*Gaudi::Units::cm);
    Sag.push_back(m_rec->getDouble("SAG_2")*Gaudi::Units::cm);
    Sag.push_back(m_rec->getDouble("SAG_3")*Gaudi::Units::cm);
    Sag.push_back(m_rec->getDouble("SAG_4")*Gaudi::Units::cm);
    Sag.push_back(m_rec->getDouble("SAG_5")*Gaudi::Units::cm);
    Sag.push_back(m_rec->getDouble("SAG_6")*Gaudi::Units::cm);
    Sag.push_back(m_rec->getDouble("SAG_7")*Gaudi::Units::cm);
    Sag.push_back(m_rec->getDouble("SAG_8")*Gaudi::Units::cm);
    Sag.push_back(m_rec->getDouble("SAG_9")*Gaudi::Units::cm);
    Sag.push_back(m_rec->getDouble("SAG_10")*Gaudi::Units::cm);
    Sag.push_back(m_rec->getDouble("SAG_11")*Gaudi::Units::cm);
    Sag.push_back(m_rec->getDouble("SAG_12")*Gaudi::Units::cm);
    Sag.push_back(m_rec->getDouble("SAG_13")*Gaudi::Units::cm);
    Sag.push_back(m_rec->getDouble("SAG_14")*Gaudi::Units::cm);

  }

}

Identifier  
LArNumberHelper::region_id_em(int i)
{ return m_region_id_em[i]; }

int  
LArNumberHelper::n_eta_em(int i)
{ return m_n_eta_em[i]; }

int  
LArNumberHelper::n_phi_em(int i)
{ return m_n_phi_em[i]; }

double  
LArNumberHelper::deta_em(int i)
{ return m_deta_em[i]; }

double  
LArNumberHelper::dphi_em(int i)
{ return m_dphi_em[i]; }

int     
LArNumberHelper::sign_em(int i)
{ return m_sign_em[i]; }

double  
LArNumberHelper::eta_min_em(int i)
{ return m_eta_min_em[i]; }

double  
LArNumberHelper::eta_max_em(int i)
{ return m_eta_max_em[i]; }

double  
LArNumberHelper::phi_min_em(int i)
{ return m_phi_min_em[i]; }

double  
LArNumberHelper::phi_max_em(int i)
{ return m_phi_max_em[i]; }

int   
LArNumberHelper::n_depth_em(int i)
{ return m_n_depth_em[i]; }

double   
LArNumberHelper::depth_em_in(int i, int j)
{ return m_depth_em_in[i][j]; }

double   
LArNumberHelper::depth_em_out(int i, int j)
{ return m_depth_em_out[i][j]; }

Identifier      
LArNumberHelper::region_id_hec(int i)
{ return m_region_id_hec[i]; }

int   
LArNumberHelper::n_eta_hec(int i)
{ return m_n_eta_hec[i]; }

int   
LArNumberHelper::n_phi_hec(int i)
{ return m_n_phi_hec[i]; }

double  
LArNumberHelper::deta_hec(int i)
{ return m_deta_hec[i]; }

double   
LArNumberHelper::dphi_hec(int i)
{ return m_dphi_hec[i]; }

int    
LArNumberHelper::sign_hec(int i)
{ return m_sign_hec[i]; }

double  
LArNumberHelper::eta_min_hec(int i)
{ return m_eta_min_hec[i]; }

double   
LArNumberHelper::eta_max_hec(int i)
{ return m_eta_max_hec[i]; }

double   
LArNumberHelper::phi_min_hec(int i)
{ return m_phi_min_hec[i]; }

double  
LArNumberHelper::phi_max_hec(int i)
{ return m_phi_max_hec[i]; }

int      
LArNumberHelper::n_depth_hec(int i)
{ return m_n_depth_hec[i]; }

double   
LArNumberHelper::depth_hec_in(int i, int j)
{ return m_depth_hec_in[i][j]; }

double   
LArNumberHelper::depth_hec_out(int i, int j)
{ return m_depth_hec_out[i][j]; }

Identifier    
LArNumberHelper::region_id_fcal(int i)
{ return m_region_id_fcal[i]; }

int  
LArNumberHelper::n_eta_fcal(int i)
{ return m_n_eta_fcal[i]; }

int   
LArNumberHelper::n_phi_fcal(int i)
{ return m_n_phi_fcal[i]; }

double   
LArNumberHelper::deta_fcal(int i)
{ return m_deta_fcal[i]; }

double   
LArNumberHelper::dphi_fcal(int i)
{ return m_dphi_fcal[i]; }

int      
LArNumberHelper::sign_fcal(int i)
{ return m_sign_fcal[i]; }

double     
LArNumberHelper::x_min_fcal(int i)
{ return m_x_min_fcal[i]; }

double     
LArNumberHelper::x_max_fcal(int i)
{ return m_x_max_fcal[i]; }

double   
LArNumberHelper::dx_fcal(int i)
{ return m_dx_fcal[i]; }

double     
LArNumberHelper::y_min_fcal(int i)
{ return m_y_min_fcal[i]; }

double     
LArNumberHelper::y_max_fcal(int i)
{ return m_y_max_fcal[i]; }

double   
LArNumberHelper::dy_fcal(int i)
{ return m_dy_fcal[i]; }

double     
LArNumberHelper::z_min_fcal(int i)
{ return m_z_min_fcal[i]; }

double     
LArNumberHelper::z_max_fcal(int i)
{ return m_z_max_fcal[i]; }

double  
LArNumberHelper::eta_min_fcal(int i)
{ return m_eta_min_fcal[i]; }

double  
LArNumberHelper::eta_max_fcal(int i)
{ return m_eta_max_fcal[i]; }

double  
LArNumberHelper::phi_min_fcal(int i)
{ return m_phi_min_fcal[i]; }

double  
LArNumberHelper::phi_max_fcal(int i)
{ return m_phi_max_fcal[i]; }

int   
LArNumberHelper::n_depth_fcal(int i)
{ return m_n_depth_fcal[i]; }

double   
LArNumberHelper::depth_fcal_in(int i, int j)
{ return m_depth_fcal_in[i][j]; }

double   
LArNumberHelper::depth_fcal_out(int i, int j)
{ return m_depth_fcal_out[i][j]; }
