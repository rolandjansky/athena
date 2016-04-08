/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class CBNT_DetailedCellInfo 
// modified for my purpose using CellInformations 14. april 2008 
//  
//                    make ntuple for CaloCell
//  
//                       

//   Using StoreGate now. 
//
// 23 April 2004: add simple detector information, J.Tanaka

#include "FastCaloSim/CBNTAA_DetailedCellInfo.h"
#include "CaloEvent/CaloCellContainer.h" // Container of all LAr and Tile cells. http://alxr.usatlas.bnl.gov/lxr/source/atlas/Calorimeter/CaloEvent/CaloEvent/CaloCellContainer.h

#include "CaloEvent/CaloCell.h" // added 21 april  2008 for using  cell information

#include "CaloDetDescr/CaloDetDescrElement.h"  // added 21 april  2008 for using caloDDE() method

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
 
#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h" 

#include "CaloIdentifier/CaloID.h" // includes all CaloIdetifier

#include "CLHEP/Units/SystemOfUnits.h"
#include "CaloSimEvent/CaloCalibrationHit.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"

#include "CaloDetDescr/CaloDetDescrManager.h"

#include "FastCaloSim/FastCaloSimIsGenSimulStable.h"

#include "TruthHelper/GenAll.h"

#include "HepMC/GenParticle.h"

#include "GaudiKernel/IPartPropSvc.h"
#include "HepPDT/ParticleData.hh"

#include "TrkParameters/Perigee.h"

#include <TVector3.h>

#include <map>
#include <iomanip>
#include <fstream>
//#include "CaloConditions/ICaloBadChanTool.h"

CBNTAA_DetailedCellInfo::CBNTAA_DetailedCellInfo(const std::string& name, ISvcLocator* pSvcLocator) :
			    CBNT_AthenaAwareBase(name, pSvcLocator),
			    m_cellsName("AllCalo"),  //Cell container
			    m_saveTimeInfo(false),m_saveQInfo(false),
			    m_saveId(false),m_savePosition(false),
			    m_FastShowerCellBuilderTool("FastShowerCellBuilderTool"),
			    m_gentesIO(0),m_particleDataTable(0),
                            m_suffix("Par"),m_cellsoutfile(""),m_mcLocation("TruthEvent")
{ // string property for Cell container 
  declareProperty("CellsName",m_cellsName);
// list of calo to treat
  declareProperty("CaloNums",m_caloNums);
// string property for Cell container 

  declareProperty("CellEnergyThreshold",m_cellEnergyThreshold);
// suffix for all the variables
  declareProperty("Suffix",m_suffix);

  declareProperty("SaveTimeInfo",m_saveTimeInfo);
  declareProperty("SaveQualityInfo",m_saveQInfo); 
  declareProperty("SaveId",m_saveId);

  declareProperty("CellinfoOutfile",m_cellsoutfile);


  declareProperty("SavePositionInfo",m_savePosition);
  declareProperty("CalibrationContainers",m_CalibrationContainerNames);
  declareProperty("CalibrationContainersInactive",m_CalibrationContainerInactiveNames);

  declareProperty("FastShowerCellBuilderTool",m_FastShowerCellBuilderTool);

  declareProperty("McLocation",m_mcLocation);
} 

CBNTAA_DetailedCellInfo::~CBNTAA_DetailedCellInfo()
{ }



// INITIALIZE
StatusCode
CBNTAA_DetailedCellInfo::CBNT_initialize()
{


   MsgStream log(messageService(), name());
   log << MSG::INFO << " in CBNT_DetailedCellInfo_initialize" << endreq;
   log << MSG::INFO << " reading CaloCellContainer " << m_cellsName << endreq ;



// VARIABLE m_saveDetInfo() NOT USED SO FAR 15.04.2008


   


   StatusCode sc ;
   

       if ( sc == StatusCode::FAILURE ) {
	log<<MSG::ERROR << "   could not add item to col wise ntuple" << endreq;
        return StatusCode::FAILURE;
       }

       addBranch ("ecell"+m_suffix,m_eCell,"ecell"+m_suffix+"/F");


       addBranch ("NCells"+m_suffix,m_nhit,"NCells"+m_suffix+"/I");
       addBranch ("ECells"+m_suffix,m_eCells);
//############
       addBranch ("CalEM_energy"+m_suffix,EM_energy);
       addBranch ("CalNonEM_energy"+m_suffix,NonEM_energy);
       addBranch ("CalInvisible_energy"+m_suffix,Invisible_energy);
       addBranch ("CalEscaped_energy"+m_suffix,Escaped_energy);

       addBranch ("CalEMInactive_energy"+m_suffix,EMInactive_energy);
       addBranch ("CalNonEMInactive_energy"+m_suffix,NonEMInactive_energy);
       addBranch ("CalInvisibleInactive_energy"+m_suffix,InvisibleInactive_energy);
       addBranch ("CalEscapedInactive_energy"+m_suffix,EscapedInactive_energy);

       addBranch ("Layer"+m_suffix,Layer);
       addBranch ("CellID"+m_suffix,CellID);
//############
       addBranch ("EtaCells"+m_suffix,m_etaCells);
       addBranch ("PhiCells"+m_suffix,m_phiCells);
       if  (m_savePosition) {
	 addBranch ("xCells"+m_suffix,m_xCells);
	 addBranch ("yCells"+m_suffix,m_yCells);
	 addBranch ("zCells"+m_suffix,m_zCells);
       }
       addBranch ("QCells"+m_suffix,m_fitQCells);
       addBranch ("GainCells"+m_suffix,m_gainCells);
       if (m_saveDetInfo) addBranch ("DetCells"+m_suffix,m_detCells);
       if (m_saveTimeInfo) addBranch ("TimeCells"+m_suffix,m_timeCells);
       if (m_saveQInfo) addBranch ("BadCells"+m_suffix,m_qCells); 
       if (m_saveId) addBranch("IdCells"+m_suffix,m_offId);       

//#########################################################
       addBranch ("d_calo_surf"+m_suffix,dcalosurf,"d_calo_surf"+m_suffix+"/F");
       addBranch ("eta_calo_surf"+m_suffix,etacalosurf,"eta_calo_surf"+m_suffix+"/F");
       addBranch ("phi_calo_surf"+m_suffix,phicalosurf,"phi_calo_surf"+m_suffix+"/F");
       addBranch ("ld_calo_layercenter"+m_suffix,ldcalolayercenter);
       addBranch ("leta_calo_layercenter"+m_suffix,letacalolayercenter);
       addBranch ("lphi_calo_layercenter"+m_suffix,lphicalolayercenter);
       addBranch ("Layerok"+m_suffix,lok);
 
// get StoreGate 
/*  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure()) {
     mLog << MSG::ERROR
          << "Unable to retrieve pointer to StoreGateSvc"
          << endreq;
     return sc;
  }*/


  sc = service("StoreGateSvc", m_storeGate);
  
  if ( sc == StatusCode::FAILURE ) {
    log<<MSG::ERROR << "   could not access SotreGate " << endreq;
    return StatusCode::FAILURE;
  }
// Bad Channel Tool not used so far 15.04.2008

/*  if (m_saveQInfo) {
    sc=m_pb_tool.retrieve();
    if (sc.isFailure()) {
      log << MSG::ERROR << "Could not retrieve bad channel tool " << m_pb_tool << endreq;
      m_saveQInfo=false;
      return sc;
    }
  }*/

  unsigned int nSubCalo=static_cast<int>(CaloCell_ID::NSUBCALO) ;
  
  //check calo number specified
  m_caloSelection = false ;
  if (m_caloNums.size()==0) {
    log << MSG::INFO << " No calorimeter selection " << endreq;
    //return StatusCode::SUCCESS;
  } else if  (m_caloNums.size()>nSubCalo ) {
    log << MSG::ERROR << " More than " 
	<< nSubCalo << " calo specified. Must be wrong. Stop." << endreq;
    return StatusCode::FAILURE;
  } else {
    m_caloSelection = true ;
    
    for (unsigned int index=0; index < m_caloNums.size() ; ++index) {
      if (m_caloNums[index]>=nSubCalo ) {
	log << MSG::ERROR << "Invalid calo specification:" 
	    << m_caloNums[index] << "Stop." << endreq ;
	return StatusCode::FAILURE;
	
      } else
	{
	  log << MSG::INFO << " Select calorimeter " << m_caloNums[index] << endreq ;
	}
      
    }
    
  }

//--------------------------------------------------
// Cell information for AddNoiseCellBuilderTool

  m_caloDDM = CaloDetDescrManager::instance() ;
  if(m_cellsoutfile!="") {
    log << MSG::INFO << "Creating cell output file "<< m_cellsoutfile << endreq ;

    std::ofstream outfile(m_cellsoutfile.c_str());

    for(CaloDetDescrManager::calo_element_const_iterator calo_iter=m_caloDDM->element_begin();calo_iter<m_caloDDM->element_end();++calo_iter) {
      const CaloDetDescrElement* theDDE=*calo_iter;
      if(theDDE) {
        Identifier cellID=theDDE->identify();
        int sample=(int)theDDE->getSampling();

        outfile<<cellID<<" "<<sample<<" "<<theDDE->eta()<<" "<<theDDE->phi()<<" "<<theDDE->r()<<" "<<theDDE->x()<<" "<<theDDE->y()<<" "<<theDDE->z()
                                    <<" "<<theDDE->deta()<<" "<<theDDE->dphi()<<" "<<theDDE->dr()<<" "<<theDDE->dx()<<" "<<theDDE->dy()<<" "<<theDDE->dz()<<std::endl;
      }  
    }
    
    outfile.close();
  } else {
    log << MSG::INFO << "skip creating cell output file"<< endreq ;
  }
//_______________________________________________________________________
// end Cell information  

  // access compact cell tools
  sc=m_FastShowerCellBuilderTool.retrieve();
  if (sc.isFailure()) {
    log << MSG::ERROR << "Failed to retrieve FastShowerCellBuilderTool: " 
        << m_FastShowerCellBuilderTool << endreq;
    return sc;
  } else {
    log << MSG::DEBUG << "Successfully retrieve FastShowerCellBuilderTool: " 
        << m_FastShowerCellBuilderTool << endreq;
  }

  m_gentesIO = new GenAccessIO();

  IPartPropSvc* p_PartPropSvc;
  sc=service("PartPropSvc",p_PartPropSvc);
  if (sc.isFailure() || 0 == p_PartPropSvc) {
    log<<MSG::ERROR << "could not find PartPropService"<<endreq;
    return StatusCode::FAILURE;
  }
  
  m_particleDataTable = (HepPDT::ParticleDataTable*) p_PartPropSvc->PDT();
  if(m_particleDataTable == 0){
    log<<MSG::ERROR << "PDG table not found"<<endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

  
  


StatusCode CBNTAA_DetailedCellInfo::CBNT_finalize()
{

 return StatusCode::SUCCESS;
}


StatusCode CBNTAA_DetailedCellInfo::CBNT_execute()
{

   MsgStream log( messageService(), name() );
   log << MSG::INFO << " in CBNTAA_DetailedCellInfo_execute" << endreq;

//  typedef  CaloCellContainer CONTAINER; 
  

  const CaloCellContainer* cellcoll(0);

  StatusCode sc=m_storeGate->retrieve(cellcoll,"AllCalo");
  if (sc.isFailure()) {
     log << MSG::ERROR
	 << "" << endreq;
     return sc;
   }
   const DataHandle<CaloCalibrationHitContainer> clbc;
   std::vector<const CaloCalibrationHitContainer * > v_clbc;
   std::vector<const CaloCalibrationHitContainer * > v_clbcInactive;
   std::vector<std::string>::iterator iter;


   for (iter=m_CalibrationContainerNames.begin();iter!=m_CalibrationContainerNames.end();iter++) {
     sc = m_storeGate->retrieve(clbc,*iter);
     if (sc.isFailure()) {
       log << MSG::ERROR << "Cannot retrieve calibration container" << endreq;
     } else {
       v_clbc.push_back(clbc);
     }
   }

   for (iter=m_CalibrationContainerInactiveNames.begin();iter!=m_CalibrationContainerInactiveNames.end();iter++) {
     sc = m_storeGate->retrieve(clbc,*iter);
     if (sc.isFailure()) {
       log << MSG::ERROR << "Cannot retrieve calibration container (Inactive)" << endreq;
     } else {
       v_clbcInactive.push_back(clbc);
     }
   }


  CaloCellContainer::const_iterator f_cell = cellcoll->begin();
  CaloCellContainer::const_iterator l_cell = cellcoll->end();

  m_nhit = 0;
  //int index = 0 ;

//  float e  = f_cell->energy() ;


      
  for ( ; f_cell!=l_cell; ++f_cell)
    {
 
      float CalEM = 0;
      float CalNONEM = 0;
      float CalInv = 0;
      float CalEsc = 0;

      float CalEMInactive = 0;
      float CalNONEMInactive= 0;
      float CalInvInactive = 0;
      float CalEscInactive = 0;

      const CaloCell* cell = (*f_cell) ;
      
      ++m_nhit ;
      float e  = cell->energy() ; 

      m_eCell +=e;



      //if(cell->energy()>0){
   
  /*    log << MSG::INFO << " cellenergie: " << cell->energy()  <<" Layer " << cell->caloDDE()->getSampling() <<endreq;
      log << 	MSG::INFO << " eta " << cell->eta() << " phi " << cell->phi() << endreq ;		
      log << 	MSG::INFO << "--------------------------------------------" << endreq;  */

	//} // added on 23. april use the calibaration hits info 	  
         

         std::vector<const CaloCalibrationHitContainer * >::const_iterator it;
         for (it=v_clbc.begin();it!=v_clbc.end();it++) {
           CaloCalibrationHitContainer::const_iterator first_calib_cell;
           CaloCalibrationHitContainer::const_iterator last_calib_cell  = (*it)->end();
           //loop over cells in calibration container
           for(first_calib_cell = (*it)->begin();first_calib_cell!=last_calib_cell;first_calib_cell++) 
           {
             if ((*first_calib_cell)->cellID()==cell->ID())
             {
               // CalibrationHit which correspond to cell in ClusterContainer has been found
               CaloCalibrationHit *calibration_hit=*first_calib_cell;
//                log << MSG::INFO << calibration_hit->energyEM()<< endreq ;
//                log << MSG::INFO <<calibration_hit->energyNonEM()<< endreq ;
//                log << MSG::INFO <<calibration_hit->energyInvisible()<< endreq ;
//                log << MSG::INFO <<calibration_hit->energyEscaped()<< endreq ;
//                log << MSG::INFO <<calibration_hit->energyTotal()<< endreq ; 

               CalEM += calibration_hit->energyEM();
	       CalNONEM += calibration_hit->energyNonEM();
               CalInv += calibration_hit->energyInvisible();
               CalEsc += calibration_hit->energyEscaped();

             } // end of if condition
           }
         }  //end of loop over all calibration containers

         std::vector<const CaloCalibrationHitContainer * >::const_iterator itInactive;
         for (itInactive=v_clbcInactive.begin();itInactive!=v_clbcInactive.end();itInactive++) {
           CaloCalibrationHitContainer::const_iterator first_calib_cell;
           CaloCalibrationHitContainer::const_iterator last_calib_cell  = (*itInactive)->end();
           //loop over cells in calibration container
           for(first_calib_cell = (*itInactive)->begin();first_calib_cell!=last_calib_cell;first_calib_cell++) 
           {
             if ((*first_calib_cell)->cellID()==cell->ID())
             {
               // CalibrationHit which correspond to cell in ClusterContainer has been found
               CaloCalibrationHit *calibration_hit=*first_calib_cell;
//                log << MSG::INFO << calibration_hit->energyEM()<< endreq ;
//                log << MSG::INFO <<calibration_hit->energyNonEM()<< endreq ;
//                log << MSG::INFO <<calibration_hit->energyInvisible()<< endreq ;
//                log << MSG::INFO <<calibration_hit->energyEscaped()<< endreq ;
//                log << MSG::INFO <<calibration_hit->energyTotal()<< endreq ; 

               CalEMInactive += calibration_hit->energyEM();
	       CalNONEMInactive += calibration_hit->energyNonEM();
               CalInvInactive += calibration_hit->energyInvisible();
               CalEscInactive += calibration_hit->energyEscaped();

             } // end of if condition
           }
         }  //end of loop over all calibration containers



// 	log << MSG::INFO << "cell=" << (int)cell<< endreq ;
// 	log << MSG::INFO << "m_eCells=" << (int)m_eCells<< endreq ;
// 	log << MSG::INFO << "m_eCells" << cell->energy()<< endreq ;

	m_eCells->push_back( cell->energy());
// 	log << MSG::INFO << "m_etaCells" << endreq ;
	m_etaCells->push_back(cell->eta());
// 	log << MSG::INFO << "m_phiCells" << endreq ;
	m_phiCells->push_back(cell->phi()) ;


// 	log << MSG::INFO << "Layer" << endreq ;
        Layer->push_back(cell->caloDDE()->getSampling());
// 	log << MSG::INFO << "CellID" << endreq ;
	CellID->push_back(cell->ID().get_identifier32().get_compact());
 
// 	log << MSG::INFO << "EM_energy" << endreq ;
        EM_energy->push_back(CalEM);
// 	log << MSG::INFO << "NonEM_energy" << endreq ;
        NonEM_energy->push_back(CalNONEM);
// 	log << MSG::INFO << "Invisible_energy" << endreq ;
        Invisible_energy->push_back(CalInv);
// 	log << MSG::INFO << "Escaped_energy" << endreq ;
        Escaped_energy->push_back(CalEsc);                     

        EMInactive_energy->push_back(CalEMInactive);
// 	log << MSG::INFO << "NonEM_energy" << endreq ;
        NonEMInactive_energy->push_back(CalNONEMInactive);
// 	log << MSG::INFO << "Invisible_energy" << endreq ;
        InvisibleInactive_energy->push_back(CalInvInactive);
// 	log << MSG::INFO << "Escaped_energy" << endreq ;
        EscapedInactive_energy->push_back(CalEscInactive);
        
    } // end of loop over all cells
    
  //typedef std::map<int,int> MCdo_simul_state;

//  FastCaloSimIsGenSimulStable ifs;
  GenAll ifs;
  
  MCparticleCollection particles;


  log <<MSG::INFO<<"Start getting particles"<<endreq;
  sc = m_gentesIO->getMC(particles, &ifs, m_mcLocation );
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "getMC from "<<m_mcLocation<<" failed "<< endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO <<"start finding partilces n="<<particles.size()<< endreq;

  MCparticleCollectionCIter ip;

 for(ip=particles.begin();ip<particles.end();++ip){

	log << MSG::INFO << "entering the for loop" << endreq;
	const HepMC::GenParticle* par=*ip;
	double charge = 0;

	HepPDT::ParticleData* ap = m_particleDataTable->particle( abs( par->pdg_id() ) );

	if(!ap){ 
	  log<<MSG::WARNING<<" id="<<par->pdg_id()<<" stat="<<par->status()<<" pt="<<par->momentum().perp()<<" eta="<<par->momentum().eta()<<" phi="<<par->momentum().phi()<<" : particle info not found"<<endreq;
      //return StatusCode::SUCCESS;
	} else {
	  charge = ap->charge();
	  if(par->pdg_id()<0) charge = -charge;
	}

 	log << MSG::INFO <<" id="<<par->pdg_id()<<" stat="<<par->status()<<" pt="<<par->momentum().perp()<<" eta="<<par->momentum().eta()<<" phi="<<par->momentum().phi()<<" charge="<<charge<< endreq;

  HepMC::GenVertex* pvtx = par->production_vertex();
//    double eta=(*par)->particle()->momentum().eta();
//    double phi=(*par)->particle()->momentum().phi();


  Trk::GlobalPosition pos(HepLorentzVector(pvtx->position().x(),pvtx->position().y(),pvtx->position().z(),pvtx->position().t()));
  Trk::GlobalMomentum mom(HepLorentzVector(par->momentum().px(),par->momentum().py(),par->momentum().pz(),par->momentum().e()));
//  Trk::GlobalPosition pos(pvtx->position());
//  Trk::GlobalMomentum mom(par->momentum());
  Trk::Perigee candidatePerigee(pos,mom,charge,pos);
  FastShowerCellBuilderTool* the_FastShowerCellBuilderTool=dynamic_cast<FastShowerCellBuilderTool*>(&(*m_FastShowerCellBuilderTool));

  log<<MSG::INFO<<"============= Getting Calo Surface ================="<<endreq;
  const Trk::TrackParameters* params_on_surface=the_FastShowerCellBuilderTool->get_calo_surface(candidatePerigee,charge);
  if(!params_on_surface) {
    return StatusCode::FAILURE;
  }
  
  TVector3 surface;
  surface.SetPtEtaPhi(1,the_FastShowerCellBuilderTool->get_eta_calo_surf(),the_FastShowerCellBuilderTool->get_phi_calo_surf());
  surface.SetMag(the_FastShowerCellBuilderTool->get_d_calo_surf());

  log<<MSG::INFO<< "eta_calo_surf:  "<< the_FastShowerCellBuilderTool->get_eta_calo_surf() << "phi_calo_surf(): "<< the_FastShowerCellBuilderTool->get_phi_calo_surf() << endreq;

  dcalosurf=the_FastShowerCellBuilderTool->get_d_calo_surf();
  etacalosurf=the_FastShowerCellBuilderTool->get_eta_calo_surf();
  phicalosurf=the_FastShowerCellBuilderTool->get_phi_calo_surf();

  for(int sample=CaloCell_ID_FCS::FirstSample;sample<CaloCell_ID_FCS::MaxSample;++sample) {
          log<<MSG::INFO<<"============= Getting Calo position for sample "<<sample<<endreq;
          lok->push_back(the_FastShowerCellBuilderTool->get_calo_etaphi(params_on_surface,(CaloCell_ID_FCS::CaloSample)sample)); 
	  ldcalolayercenter->push_back(the_FastShowerCellBuilderTool->get_d_calo_surf(sample));
	  letacalolayercenter->push_back(the_FastShowerCellBuilderTool->get_eta_calo_surf(sample));
	  lphicalolayercenter->push_back(the_FastShowerCellBuilderTool->get_phi_calo_surf(sample));
; 
  }

  break;  
 }

//return StatusCode::SUCCESS;
//    int* test=0; // Durch diese Zeile bricht die Reconstrucktion nach demn 1. Event ab
//    *test=0; 	

return StatusCode::SUCCESS; 


}












StatusCode CBNTAA_DetailedCellInfo::CBNT_clear()
{
  MsgStream log( messageService(), name() );
  log << MSG::INFO << " in CBNT_DetailedCellInfo_clear" << endreq;
  
  m_nhit=0 ; 
  m_eCell=0 ;
  

  m_eCells->clear() ;
  m_etaCells->clear() ;
  m_phiCells->clear() ;      
  EM_energy->clear() ;
  NonEM_energy->clear() ;
  Invisible_energy->clear() ;
  Escaped_energy->clear() ;

  EMInactive_energy->clear() ;
  NonEMInactive_energy->clear() ;
  InvisibleInactive_energy->clear() ;
  EscapedInactive_energy->clear() ;

  Layer->clear();
  CellID->clear();         
  dcalosurf=0;
  etacalosurf=0;
  phicalosurf=0;
  ldcalolayercenter->clear();
  letacalolayercenter->clear();
  lphicalolayercenter->clear();
  lok->clear();
                  if (m_savePosition) {
    m_xCells->clear() ;
    m_yCells->clear() ;
    m_zCells->clear() ;
  }
  m_fitQCells->clear() ;
  m_gainCells->clear() ;
  if (m_saveDetInfo) m_detCells->clear() ;
  if (m_saveTimeInfo) m_timeCells->clear() ;
  if (m_saveId) m_offId->clear();
  if (m_saveQInfo) m_qCells->clear() ;

  

  



 return StatusCode::SUCCESS;
}

