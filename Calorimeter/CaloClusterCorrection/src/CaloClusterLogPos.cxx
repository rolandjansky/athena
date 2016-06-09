/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloClusterLogPos.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "GaudiKernel/MsgStream.h"
#include <CLHEP/Units/SystemOfUnits.h>
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/StoreGate.h"

using CLHEP::deg;

/// Standard AlgTool constructor
CaloClusterLogPos::CaloClusterLogPos(const std::string& type,
				     const std::string& name,
				     const IInterface* parent)
  :CaloClusterProcessor(type, name, parent),
   m_offset(4.7),
   m_calo_id(0),
   m_calo_dd_man(0)
{
  declareProperty ("LogPosOffset",m_offset) ;
}


StatusCode CaloClusterLogPos::initialize() {

  const IGeoModelSvc *geoModel=0;
  StatusCode sc = service("GeoModelSvc", geoModel);
  if(sc.isFailure())
  {
    msg( MSG::ERROR )  << "Could not locate GeoModelSvc" << endreq;
    return sc;
  }

  // dummy parameters for the callback:
  int dummyInt=0;
  std::list<std::string> dummyList;

  if (geoModel->geoInitialized())
  {
    return geoInit(dummyInt,dummyList);
  }
  else
  {
    sc = detStore()->regFcn(&IGeoModelSvc::geoInit,
			  geoModel,
			  &CaloClusterLogPos::geoInit,this);
    if(sc.isFailure())
    {
      msg(  MSG::ERROR ) << "Could not register geoInit callback" << endreq;
      return sc;
    }
  }
  return sc;
}

StatusCode
CaloClusterLogPos::geoInit(IOVSVC_CALLBACK_ARGS)
{

  ATH_MSG_DEBUG("Initializing " << name() << endreq ) ;

  // pointer to detector manager:
  m_calo_dd_man = CaloDetDescrManager::instance(); 
  m_calo_id   = m_calo_dd_man->getCaloCell_ID();

  return StatusCode::SUCCESS;
}
   
StatusCode  CaloClusterLogPos::execute(xAOD::CaloCluster* theCluster) {
  
  if ( msgSvc()->outputLevel(name()) <= MSG::DEBUG ) {

    msg(MSG::DEBUG) << " old cluster eta = " << theCluster->eta() 
		    << " phi = " << theCluster->phi() << endreq;

    // std::vector<double> theEtas;
    // std::vector<double> thePhis;
    // theCluster->getEtaInSamples(theEtas);
    // theCluster->getPhiInSamples(thePhis);
    // for(int i=0;i<CaloCell_ID::Unknown;i++) {
    //   report << MSG::DEBUG << " old sampling " << i << " eta = " << theEtas[i]
    // 	     << " phi = " << thePhis[i] << endreq;
    //}
  }

  const double absEClus = fabs(theCluster->e());

  if ( absEClus > 0 ) {
    const double inv_absEClus = 1. / absEClus;
    std::vector<double> weightSample(CaloCell_ID::Unknown,0);
    std::vector<double> etaSample(CaloCell_ID::Unknown,0);
    std::vector<double> phiSample(CaloCell_ID::Unknown,0);
    
    double weightAll(0),etaAll(0),phiAll(0);
    
    xAOD::CaloCluster::cell_iterator itrCell    = theCluster->cell_begin();
    xAOD::CaloCluster::cell_iterator itrCellEnd = theCluster->cell_end();
    for (;itrCell!=itrCellEnd; ++itrCell) {
      const CaloCell* thisCell = *itrCell;
      double weight = itrCell.weight();//theCluster->getCellWeight(itrCell);
      //      Identifier myId = thisCell->ID();
      int theSample = thisCell->caloDDE()->getSampling();
      double absE = fabs(weight*thisCell->e());
      
      if ( absE > 0 ) {
	double lw = m_offset + log(absE*inv_absEClus);
	if ( lw > 0 ) {

	  ATH_MSG_VERBOSE(" add cell with eta " 
			  << thisCell->eta() << " phi " << thisCell->phi() 
			  << " energy " << thisCell->e() << " weight " << weight 
			  << " sampling " << theSample << " cluster e " 
			  << theCluster->e() << " log weight " << lw);
	  

	  etaAll    += thisCell->eta()*lw;
	  
	  // first cell decides the sign in order to avoid
	  // overlap problem at phi = -pi == +pi
	  // need to be normalized to the range [-pi,+pi] in the end
	  
	  if ( weightAll > 0 
	       && phiAll/weightAll < CaloPhiRange::phi_min() + 90*deg
	       && thisCell->phi() > CaloPhiRange::phi_max() - 90*deg)
	    phiAll    += (thisCell->phi()-360*deg)*lw;
	  else if ( weightAll > 0 
		    && phiAll/weightAll > CaloPhiRange::phi_max() - 90*deg
		    && thisCell->phi() < CaloPhiRange::phi_min() + 90*deg)
	    phiAll    += (thisCell->phi()+360*deg)*lw;
	  else
	    phiAll    += thisCell->phi()*lw;

	  weightAll += lw;

	  etaSample[theSample]    += thisCell->eta()*lw;
	  if ( weightSample[theSample] > 0 
	       && phiSample[theSample]/weightSample[theSample] 
	       < CaloPhiRange::phi_min() + 90*deg
	       && thisCell->phi() > CaloPhiRange::phi_max() - 90*deg)
	    phiSample[theSample]    += (thisCell->phi()-360*deg)*lw;
	  else if ( weightSample[theSample] > 0 
		    && phiSample[theSample]/weightSample[theSample] 
		    > CaloPhiRange::phi_max() - 90*deg
		    && thisCell->phi() < CaloPhiRange::phi_min() + 90*deg)
	    phiSample[theSample]    += (thisCell->phi()+360*deg)*lw;
	  else
	    phiSample[theSample]    += thisCell->phi()*lw;
	  weightSample[theSample] += lw;
	  
	}
      }
    }
    
    if ( weightAll > 0 ) {
      const double inv_weightAll = 1. / weightAll;
      theCluster->setEta(etaAll * inv_weightAll);
      theCluster->setPhi(CaloPhiRange::fix(phiAll * inv_weightAll));
    }
    
    // std::vector<double> theEtas(CaloCell_ID::Unknown,0);
    // std::vector<double> thePhis(CaloCell_ID::Unknown,0);
    // for (int i=0;i<CaloCell_ID::Unknown;i++) {
    //   if (theCluster->hasSampling(i)) {
    // 	theEtas[i]=theCluster->etaSampl(i);
    // 	thePhis[i]=theCluster->phiSampl(i);
    //   }

    for(int i=0;i<CaloCell_ID::Unknown;i++) {
      CaloSampling::CaloSample s=(CaloSampling::CaloSample)i;
      if ( theCluster->hasSampling(s) && weightSample[i] > 0 ) {
	float etaSample=theCluster->etaSample(s);
	float phiSample=theCluster->phiSample(s);
	etaSample /= weightSample[i];
	phiSample /= weightSample[i];
	phiSample = CaloPhiRange::fix(phiSample);
	theCluster->setEta(s,etaSample);
	theCluster->setPhi(s,phiSample);
      }
    }//end loop over samples
  }

  if ( msgSvc()->outputLevel(name()) <= MSG::DEBUG ) {

    ATH_MSG_DEBUG(" new cluster eta = " << theCluster->eta()
		  << " phi = " << theCluster->phi());

    // std::vector<double> theEtas;
    // std::vector<double> thePhis;

    // theCluster->getEtaInSamples(theEtas);
    // theCluster->getPhiInSamples(thePhis);
    // for(int i=0;i<CaloCell_ID::Unknown;i++) {
    //   msg(MSG::DEBUG) << " new sampling " << i << " eta = " << theEtas[i]
    // 		      << " phi = " << thePhis[i] << endreq;
    // }
  }

  return StatusCode::SUCCESS;
}



