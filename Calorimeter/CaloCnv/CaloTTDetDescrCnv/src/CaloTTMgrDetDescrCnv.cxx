/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Calo Trigger Tower Manager converter package
 --------------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: CaloTTMgrDetDescrCnv.cxx,v 1.21 2008-12-14 01:56:04 ssnyder Exp $
//<version>	$Name: not supported by cvs2svn $


#include "CaloTTMgrDetDescrCnv.h"

// infrastructure includes
#include "DetDescrCnvSvc/DetDescrConverter.h"
#include "DetDescrCnvSvc/DetDescrAddress.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGate.h" 
#include "StoreGate/StoreGateSvc.h"

// detdescr includes
#include "CaloTTDetDescr/CaloTTDescrManager.h"
#include "CaloTTDetDescr/CaloTTDescrRegion.h"
#include "CaloTTDetDescr/CaloTTDescriptor.h"
#include "CaloIdentifier/CaloLVL1_ID.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloTriggerTool/CaloTriggerTowerService.h" 

// Instantiation of a static factory class used by clients to create
// instances of this service

long int 
CaloTTMgrDetDescrCnv::repSvcType() const
{
  return (storageType());
}

StatusCode 
CaloTTMgrDetDescrCnv::initialize()
{
    // First call parent init
    StatusCode sc = DetDescrConverter::initialize();
    MsgStream log(msgSvc(), "CaloTTMgrDetDescrCnv");
    if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "in initialize" << endmsg;

    if (sc.isFailure()) {
        log << MSG::ERROR << "DetDescrConverter::initialize failed" << endmsg;
	return sc;
    }
    
    // The following is an attempt to "bootstrap" the loading of a
    // proxy for CaloTTDescrManager into the detector store. However,
    // CaloTTMgrDetDescrCnv::initialize is NOT called by the conversion
    // service.  So for the moment, this cannot be use. Instead the
    // DetDescrCnvSvc must do the bootstrap from a parameter list.


//      // Add CaloTTDescrManager proxy as entry point to the detector store
//      // - this is ONLY needed for the manager of each system
//      sc = addToDetStore(classID(), "CaloTTDescrManager");
//      if (sc.isFailure()) {
//  	log << MSG::FATAL << "Unable to add proxy for CaloTTDescrManager to the Detector Store!" << endmsg;
//  	return StatusCode::FAILURE;
//      } else {}

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode 
CaloTTMgrDetDescrCnv::finalize()
{
    MsgStream log(msgSvc(), "CaloTTMgrDetDescrCnv");
    if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "in finalize" << endmsg;

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode
CaloTTMgrDetDescrCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{
    MsgStream log(msgSvc(), "CaloTTMgrDetDescrCnv");
    log << MSG::INFO << "in createObj: creating a CaloTTDescrManager object in the detector store" << endmsg;
    int outputLevel = msgSvc()->outputLevel( "CaloTTMgrDetDescrCnv" );

    // Create a new CaloTTDescrManager

    DetDescrAddress* ddAddr;
    ddAddr = dynamic_cast<DetDescrAddress*> (pAddr);
    if(!ddAddr) {
	log << MSG::FATAL << "Could not cast to DetDescrAddress." << endmsg;
	return StatusCode::FAILURE;
    }

    // Get the StoreGate key of this container.
    std::string mgrKey  = *( ddAddr->par() );
    
    if (outputLevel <= MSG::DEBUG) {
    if ("" == mgrKey) {
	log << MSG::DEBUG << "No Manager key " << endmsg;
    }
    else {
	log << MSG::DEBUG << "Manager key is " << mgrKey << endmsg;
    }
    }
    
    // Create the manager
    CaloTTDescrManager* caloTTMgr = new CaloTTDescrManager(); 

    // Pass a pointer to the container to the Persistency service by reference.
    pObj = StoreGateSvc::asStorable(caloTTMgr);

    // get DetectorStore service
    StoreGateSvc * detStore;
    StatusCode status = serviceLocator()->service("DetectorStore", detStore);
    if (status.isFailure()) {
	log << MSG::FATAL << "DetectorStore service not found !" << endmsg;
	return StatusCode::FAILURE;
    } else {}
 
    // Get idhelper from detector store and add to mgr
    //const DataHandle<CaloLVL1_ID> lvl1_id;
    const CaloLVL1_ID* lvl1_id;
    status = detStore->retrieve(lvl1_id, "CaloLVL1_ID");
    if (status.isFailure()) {
	log << MSG::FATAL << "Could not get CaloLVL1_ID helper !" << endmsg;
	return StatusCode::FAILURE;
    } 
    else {
      if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG << " Found the CaloLVL1_ID helper. " << endmsg;
    }
    caloTTMgr->set_helper(lvl1_id);
    log << MSG::INFO << "Set CaloLVL1_ID helper in CaloTTMgr " 
	<< endmsg;

    // Get CaloDetDescrManager from detector store 
    // to build geometry of trigger towers
    const CaloDetDescrManager* caloMgr;
    status = detStore->retrieve(caloMgr);
    if (status.isFailure()) {
	log << MSG::FATAL << "Could not get CaloDetDescr manager !" << endmsg;
	return StatusCode::FAILURE;
    } 
    else {
      if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG << " Retrieved CaloDetDescr Manager " << endmsg;
    }

    IToolSvc* toolSvc;
    status   = service( "ToolSvc",toolSvc  );
    if(! status.isSuccess()) { 
      return status;
    }  

    CaloTriggerTowerService*    ttSvc; 
    status =toolSvc->retrieveTool("CaloTriggerTowerService",ttSvc);
    if(!status.isSuccess() ) return status;
    

    // Initialize the caloTT mgr
    //  We protect here in case this has been initialized elsewhere

    if (!caloTTMgr->is_initialized()) {

      if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG << "Initializing CaloTTMgr from values in CaloTTMgrDetDescrCnv " 
	  << endmsg;

      int numberOfIdRegions=0;
      int numberOfDescrRegions=0;
      int nEmb=0;
      int nEmec=0;
      int nHec=0;
      int nFcal=0;

      // Initialize the manager ...

      const CaloLVL1_ID* lvl1_helper = lvl1_id;
      
      std::vector<Identifier>::const_iterator itId = lvl1_id->region_begin();
      std::vector<Identifier>::const_iterator itIdEnd = lvl1_id->region_end();
      
      for(; itId!=itIdEnd;++itId){
	Identifier regId = *itId;
	++numberOfIdRegions;
	int posNeg   =  lvl1_helper->pos_neg_z(regId);
	//	int sampling = lvl1_helper->sampling(regId);
	int region   =  lvl1_helper->region(regId);
	int halfNphi = (lvl1_helper->phi_max(regId)+1)/2;
	int nLay     =  lvl1_helper->layer_max(regId)+1;
	
	// create Descriptors for both LAr and Tile
	CaloTTDescriptor* calo_descriptor = new CaloTTDescriptor() ;
	calo_descriptor->set(regId);

	if( 0 == region ) {
	  //	  calo_descriptor->set(0. ,2.5,0.1,-M_PI,M_PI,M_PI/halfNphi,posNeg,nLay);
	  calo_descriptor->set(0. ,2.5,0.1,0.,2.*M_PI,M_PI/halfNphi,posNeg,nLay);
	} else if( 1 == region ) {
	  //	  calo_descriptor->set(2.5,3.1,0.2,-M_PI,M_PI,M_PI/halfNphi,posNeg,nLay);
	  calo_descriptor->set(2.5,3.1,0.2,0.,2.*M_PI,M_PI/halfNphi,posNeg,nLay);
	} else if( 2 == region ) {
	  //	  calo_descriptor->set(3.1,3.2,0.1,-M_PI,M_PI,M_PI/halfNphi,posNeg,nLay);
	  calo_descriptor->set(3.1,3.2,0.1,0.,2.*M_PI,M_PI/halfNphi,posNeg,nLay);
	} else if( 3 == region ) {
	  //	  calo_descriptor->set(3.2,4.9,0.4,-M_PI,M_PI,M_PI/halfNphi,posNeg,nLay);
	  calo_descriptor->set(3.2,4.9,0.4,0.,2.*M_PI,M_PI/halfNphi,posNeg,nLay);
	}

	// Initialize 
	caloTTMgr->add(calo_descriptor);
	
	// now, create CaloTTDescrRegions for LAr only
	// Loop on TTs of this region and create CaloTTDescrRegion's (equiv. to cell's CaloDDE for TT)
	int maxEta=calo_descriptor->nEta();
	for(int iEta=0;iEta<maxEta;++iEta) {
	    
	  int maxPhi=calo_descriptor->nPhi();
	  for(int iPhi=0;iPhi<maxPhi;++iPhi) {
	      
	    Identifier TTid = lvl1_helper->tower_id(regId,iEta,iPhi);
	    if(!lvl1_helper->is_tile(TTid)) {

	      int maxLay=calo_descriptor->nLay();
	      for(int iLay=0;iLay<maxLay;++iLay) {

		Identifier layId = lvl1_helper->layer_id(TTid,iLay);
		bool isFCAL(0), isEC(0);
		if(lvl1_helper->is_fcal(layId)) {
		  isFCAL=true;
		}
		else if(lvl1_helper->is_emec(layId) || lvl1_helper->is_hec(layId)) {
		  isEC=true;
		}
		if (outputLevel <= MSG::DEBUG) {
		  if(lvl1_helper->is_emb(layId) || lvl1_helper->is_barrel_end(layId) ) {
		    ++nEmb;
		    log << MSG::DEBUG << " Found EMB TT " << lvl1_helper->show_to_string(layId) << endmsg;
		  }
		  else if(lvl1_helper->is_emec(layId)) {
		    ++nEmec;
		    log << MSG::DEBUG << " Found EMEC TT " << lvl1_helper->show_to_string(layId) << endmsg;
		  }
		  else if(lvl1_helper->is_hec(layId)) {
		    ++nHec;
		    log << MSG::DEBUG << " Found HEC TT " << lvl1_helper->show_to_string(layId) << endmsg;
		  }
		  else {  // FCAL
		    ++nFcal;
		    log << MSG::DEBUG << " Found FCAL TT " << lvl1_helper->show_to_string(layId) << endmsg;
		  }
		}

		double dEta=calo_descriptor->deta();
		double eta=calo_descriptor->eta_min()+(iEta+0.5)*dEta;
		eta*=calo_descriptor->sign_eta();
		
		double dPhi=calo_descriptor->dphi();
		// beware of the -pi,pi convention
		// fixed thanks to Denis Damazio when integrating LArRegionSelector
		//		float orig=-M_PI; 
		float orig=0.;
		double phi=calo_descriptor->phiMin() + (iPhi+0.5)*dPhi + orig;
		if(phi>=M_PI) {phi-=2.*M_PI;}
		if(phi<-M_PI) {phi+=2.*M_PI;}
		
		
		float rhoMin=99999.;
		float rhoMax=0.;
		float zMin=99999.;
		float zMax=-99999.;
		
		//int atlas_tb=0;
		std::vector<Identifier> vec = ttSvc->createCellIDvecLayer(layId);
		
		if(vec.size() > 0) {
		  numberOfDescrRegions++;
		  std::vector<Identifier>::const_iterator it     = vec.begin();
		  std::vector<Identifier>::const_iterator it_end = vec.end();
		  for(;it!=it_end;++it) {
		    Identifier offId = (*it);
		    const CaloDetDescrElement* caloDDE = 
                      caloMgr->get_element(offId);
		    double rho  =caloDDE->r_raw();
		    double cDrho=caloDDE->dr();
		    double z    =caloDDE->z_raw();
		    double cDz  =caloDDE->dz();
		    
		    if( (rho-cDrho/2.)<rhoMin ) {rhoMin=std::max(rho-cDrho/2.,0.);}
		    if( (rho+cDrho/2.)>rhoMax ) {rhoMax=rho+cDrho/2.;}
		    if( (z-cDz/2.)<zMin ) {zMin=z-cDz/2.;}
		    if( (z+cDz/2.)>zMax ) {zMax=z+cDz/2.;}
		  } // end loop on vector elements
		
		
		  CaloTTDescrRegion* tt_region = new CaloTTDescrRegion(layId,calo_descriptor);
		  if(isFCAL) {
		    // FIX ME !?
		    tt_region->set_cylindric(eta,phi,(zMax+zMin)/2.);
		    tt_region->set_cylindric_size(dEta,dPhi,fabs(zMax-zMin));
		  } else if(isEC) {
		    tt_region->set_cylindric(eta,phi,(zMax+zMin)/2.);
		    tt_region->set_cylindric_size(dEta,dPhi,fabs(zMax-zMin));
		  } else {
		    tt_region->set_spherical(eta,phi,(rhoMax+rhoMin)/2.);
		    tt_region->set_spherical_size(dEta,dPhi,fabs(rhoMax-rhoMin));
		  }
		
		  caloTTMgr->add(tt_region);

		} // end condition of vec size
		else {
		  if (outputLevel <= MSG::DEBUG) log << MSG::DEBUG << " Found no cell for TT " << lvl1_helper->show_to_string(layId) << endmsg;
		}
	      } // end loop on layers
	    } // end condition against tile
	  } // end loop on phi
	} // end loop on eta
      } // end loop on descr_regions
      
      // Set to initialized state only if descriptors have been found
      if (caloTTMgr->calo_descriptors_size () > 0) caloTTMgr->initialize();
      
	
      log << MSG::INFO << " Initialized CaloTTMgr, number of descr regions is " << numberOfDescrRegions << endmsg;
      if (outputLevel <= MSG::DEBUG) {
	log << MSG::DEBUG << " including " 
	    << nEmb << " Em Barrel " 
	    << nEmec << " Em EC "
	    << nHec << " HEC "
	    << nFcal << " FCAL "
	    << endmsg;
	log << MSG::DEBUG << " number of helper regions= " << numberOfIdRegions << endmsg; 
      }
    } // end of condition !is_initialized()


    return StatusCode::SUCCESS; 

}

//--------------------------------------------------------------------

long 
CaloTTMgrDetDescrCnv::storageType()
{
    return DetDescr_StorageType;
}

//--------------------------------------------------------------------
const CLID& 
CaloTTMgrDetDescrCnv::classID() { 
    return ClassID_traits<CaloTTDescrManager>::ID(); 
}

//--------------------------------------------------------------------
CaloTTMgrDetDescrCnv::CaloTTMgrDetDescrCnv(ISvcLocator* svcloc) 
    :
    DetDescrConverter(ClassID_traits<CaloTTDescrManager>::ID(), svcloc)
{}

