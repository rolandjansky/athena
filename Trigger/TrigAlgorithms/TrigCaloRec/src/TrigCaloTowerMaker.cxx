/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigCaloTowerMaker.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigCaloRec
//
// AUTHOR:   P.A. Delsart
//           This is an Hlt algorithm that creates Calorimeter towers
//           It assumes CaloCellContainers are already produced and accessable
//           through the input TriggerElement. Based on TrigCaloRec
//
// ********************************************************************
//
//
#include <sstream>
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/ThreadLocalContext.h"
//
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
//
#include "CaloEvent/CaloCellContainer.h"

//
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
//
#include "CaloUtils/CaloCollectionHelper.h"
//

#include "CaloUtils/CaloTowerBuilderToolBase.h"
#include "CaloEvent/CaloTowerSeg.h"
#include "CaloEvent/CaloTowerContainer.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "TrigCaloRec/TrigCaloTowerMaker.h"
//
class ISvcLocator;



/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigCaloTowerMaker::TrigCaloTowerMaker(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::FexAlgo(name, pSvcLocator)
  , m_nEtaTowers(50)
  , m_nPhiTowers(64)
  , m_minEta(-2.5)
  , m_maxEta(2.5)
  , m_deta(0.5)
  , m_dphi(0.5)
  , m_totalTimer(NULL)
  , m_towerContTimer(NULL)
  , m_pCaloTowerContainer(NULL)
  , m_includeFcal(false)
{


  // Name of Tower Maker Tools
  declareProperty( "TowerMakerTools",   m_towerMakerNames );
  declareProperty( "NumberOfEtaTowers", m_nEtaTowers );
  declareProperty( "NumberOfPhiTowers", m_nPhiTowers );
  declareProperty( "EtaMin", m_minEta );
  declareProperty( "EtaMax", m_maxEta );
  declareProperty( "DeltaEta", m_deta );
  declareProperty( "DeltaPhi", m_dphi );

  declareMonitoredVariable("TowerContainerSize", m_TowerContainerSize);


}

/////////////////////////////////////////////////////////////////////
    // DESTRUCTOR:
    /////////////////////////////////////////////////////////////////////
    //
    TrigCaloTowerMaker::~TrigCaloTowerMaker()
{  }

/////////////////////////////////////////////////////////////////////
    // INITIALIZE:
    // The initialize method will create all the required algorithm objects
    // Note that it is NOT NECESSARY to run the initialize of individual
    // sub-algorithms.  The framework takes care of it.
    /////////////////////////////////////////////////////////////////////


    HLT::ErrorCode TrigCaloTowerMaker::hltInitialize()
{
  msg() << MSG::DEBUG << "in initialize()" << endmsg;

  // Cache pointer to ToolSvc
  IToolSvc* toolSvc = 0;// Pointer to Tool Service
  if (service("ToolSvc", toolSvc).isFailure()) {
    msg() << MSG::FATAL << " Tool Service not found " << endmsg;
    return HLT::BAD_JOB_SETUP;
  }

  std::vector<std::string>::iterator itrName;
  std::vector<std::string>::iterator endName;

  itrName = m_towerMakerNames.begin();
  endName = m_towerMakerNames.end();

  CaloTowerSeg theTowerSeg(m_nEtaTowers,m_nPhiTowers,m_minEta,m_maxEta);
  
  for (; itrName!=endName; ++itrName) {
    
    ListItem theItem(*itrName);
    IAlgTool* algtool;
    if ( (*itrName).find("FC") != std::string::npos ) m_includeFcal=true;
    
    if( toolSvc->retrieveTool(theItem.type(), theItem.name(), algtool,this).isFailure() ) {
      msg() << MSG::FATAL << "Unable to find tool for " << (*itrName) << endmsg;
      return HLT::BAD_JOB_SETUP;
    } else {
      CaloTowerBuilderToolBase* mytool = dynamic_cast<CaloTowerBuilderToolBase*>(algtool);
      if(mytool == NULL) {
	ATH_MSG_FATAL("dynamic cast to CaloTowerBuilderToolBase failed for" << theItem.name());
	return HLT::BAD_JOB_SETUP;
      }
      else {
	msg() << MSG::DEBUG << " set towerSet for this tool " << theItem.name() << endmsg;
	mytool->setTowerSeg(theTowerSeg);
	m_towerMakerPointers.push_back(mytool);
      }
    }
  }

  // end of helpers...
 
  if (timerSvc()) {
    m_totalTimer     = addTimer("TimeTot");
    m_towerContTimer = addTimer("TCRRTowerContainer");
  }

  msg() << MSG::DEBUG
        << "Initialization of TrigCaloTowerMaker completed successfully"
        << endmsg;

  return HLT::OK;
}


HLT::ErrorCode TrigCaloTowerMaker::hltFinalize()
{

  msg() << MSG::DEBUG << "in finalize()" << endmsg;

  return HLT::OK;
}


HLT::ErrorCode TrigCaloTowerMaker::hltExecute(const HLT::TriggerElement* inputTE,
					      HLT::TriggerElement* outputTE)
{
  // Time total TrigCaloTowerMaker execution time.
  if (timerSvc()) m_totalTimer->start();    
  
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "in execute()" << endmsg;
    
  // Monitoring initialization...
  m_TowerContainerSize = 0.;

  // Some debug output:
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "outputTE->getId(): " << outputTE->getId() << endmsg;
    msg() << MSG::DEBUG << "inputTE->getId(): " << inputTE->getId() << endmsg;
  }
    
  // Get RoiDescriptor
  
  const IRoiDescriptor*    roiDescriptor = 0;
  const TrigRoiDescriptor* tmproi   = 0;

  HLT::ErrorCode sc = getFeature(inputTE, tmproi, "");
  if (sc != HLT::OK || tmproi==0 ) return sc;
  roiDescriptor = tmproi;

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << " RoI id " << roiDescriptor->roiId()
	  << " located at   phi = " <<  roiDescriptor->phi()
	  << ", eta = " << roiDescriptor->eta() << endmsg;
    
  /// ho hum, this needs a flag for using own wdiths rather than those from the roiDescriptor  
  /// in addition, this will *not* work properly for composite RoIs
    
  if ( roiDescriptor->composite() ) { 
    msg() << MSG::WARNING << " Attempting to use composite RoI as a normal RoI - this is probably *not* what you want to do "
	  << *roiDescriptor << endmsg;
  }
  
  double   eta0 = roiDescriptor->eta();
  double   phi0 = roiDescriptor->phi();
      
  while (phi0 > 2.*M_PI) phi0 -= 2. * M_PI;
  while (phi0 < 0. )     phi0 += 2. * M_PI;
  double etamin = eta0-m_deta/2.;
  double etamax = eta0+m_deta/2.;
  // What if phimin < 0 or phimax > 2Pi ??  question
  double phimin = phi0 - m_dphi/2.;
  double phimax = phi0 + m_dphi/2.;

  if (msgLvl() <= MSG::DEBUG) {
    // set up the sampling windows (only for samp2??):
    msg() << MSG::DEBUG << " eta0 = "<< eta0 << endmsg;
    msg() << MSG::DEBUG << " phi0 = "<< phi0 << endmsg;
    msg() << MSG::DEBUG << " etamin = "<< etamin << endmsg;
    msg() << MSG::DEBUG << " etamax = "<< etamax << endmsg;
    msg() << MSG::DEBUG << " phimin = "<< phimin << endmsg;
    msg() << MSG::DEBUG << " phimax = "<< phimax << endmsg;
  }

  if (timerSvc()) m_towerContTimer->start();  // Measures the time to retrieve the cells in the RoI  


  // We retrieve the TowerContainer from the Trigger Element...
  std::vector<const CaloCellContainer*> vectorOfCellContainers;
   
  sc = getFeatures(outputTE, vectorOfCellContainers, "");
  if (sc != HLT::OK) {
    if(msgLvl() <= MSG::WARNING) {
      msg() << MSG::WARNING << "Failed to get TrigCells" << endmsg;
    }

    // return OK anyways, for some reason...
    return HLT::OK;
  }
    
  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Got vector with " << vectorOfCellContainers.size()
          << " CellContainers" << endmsg;
  }
    
  // if no containers were found, just leave the vector empty and leave
  if ( vectorOfCellContainers.size() < 1) {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "No cells to analyse, leaving!" << endmsg;
    return HLT::OK;
  }
    
  // Get the last container in the vector. Should be th one produced by the previous TrigCaloCellMaker.
  const CaloCellContainer* theCellCont = vectorOfCellContainers.back();
  if(!theCellCont){
    if(msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "No Cells found in the RoI" << endmsg;
    return HLT::OK;
  }

  // All this only to rebuild the key :     
  std::string cellCollKey ;
  getStoreGateKey( theCellCont, cellCollKey );
    

  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << " REGTEST: Retrieved the cell container in the RoI " << endmsg;
    msg() << MSG::DEBUG << " REGTEST: Retrieved a Cell Container of Size= " << theCellCont->size() << endmsg;
  }

  // Now Build the towers -----------------------------------------------------------------

  const EventContext& ctx = Gaudi::Hive::currentContext();
  if(!m_towerMakerNames.empty()) {

    //CaloTowerSeg theTowerSeg(m_nEtaTowers,m_nPhiTowers,m_minEta,m_maxEta);
    CaloTowerSeg myseg (m_nEtaTowers,m_nPhiTowers,m_minEta,m_maxEta);
    CaloTowerSeg::SubSeg subseg = myseg.subseg ( roiDescriptor->eta(), m_deta,
		roiDescriptor->phi(), m_dphi );
    //CaloTowerContainer* m_pCaloTowerContainer = new CaloTowerContainer(theTowerSeg);
    if ( m_includeFcal )
    m_pCaloTowerContainer = new CaloTowerContainer(myseg);
    else
    m_pCaloTowerContainer = new CaloTowerContainer( subseg.segmentation() );


    std::vector<CaloTowerBuilderToolBase*>::const_iterator itrtwr=m_towerMakerPointers.begin();
    std::vector<CaloTowerBuilderToolBase*>::const_iterator endtwr=m_towerMakerPointers.end();
    int index=0;
    for (; itrtwr!=endtwr; ++itrtwr) {
      //if((*itrtwr)->setProperty( StringProperty("CellContainerName",cellCollKey) ).isFailure()) {
	//msg() << MSG::ERROR << "ERROR setting the CaloCellContainer name in the offline tool" << endmsg;
	//return HLT::TOOL_FAILURE;
      //}
      if ( m_includeFcal ) {
        if ( (*itrtwr)->execute(ctx, m_pCaloTowerContainer, theCellCont).isFailure() ) {
         	msg() << MSG::ERROR << "Error executing tool " << m_towerMakerNames[index] << endmsg;
        } else {
         	if(msgLvl() <= MSG::DEBUG)
	          msg() << MSG::DEBUG << "Executed tool " << m_towerMakerNames[index] << endmsg;
        }
      } else {
        if ( (*itrtwr)->execute(ctx, m_pCaloTowerContainer,theCellCont, &subseg).isFailure() ) {
         	msg() << MSG::ERROR << "Error executing tool " << m_towerMakerNames[index] << endmsg;
        } else {
         	if(msgLvl() <= MSG::DEBUG)
	          msg() << MSG::DEBUG << "Executed tool " << m_towerMakerNames[index] << endmsg;
        }
      }
      ++index;
    }
    if(msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << " REGTEST: Produced a Tower Container of Size= " << m_pCaloTowerContainer->size() << endmsg;
    
    std::string towerCollKey;
    sc = recordAndAttachFeature(outputTE, m_pCaloTowerContainer, towerCollKey, "TrigCaloTowerMaker");
    if (sc != HLT::OK) {
      msg() << MSG::ERROR << "Could not record a tower container in the RoI with key " <<  towerCollKey << endmsg;
      return sc;
    } else {
      if(msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << " REGTEST: Recorded the Tower container in SG " << endmsg;
      const INavigable4MomentumCollection* theNav4Coll = 0;

      if ((store()->setConst(m_pCaloTowerContainer)).isSuccess()) {
	if((store()->symLink(m_pCaloTowerContainer,theNav4Coll)).isFailure()) {
	  msg() << MSG::ERROR << "Could not link the Tower Container to the INavigable4MomentumCollection " << endmsg;
	  return HLT::NAV_ERROR;
	}
      } else {
	msg() << MSG::ERROR << "Could not link the Tower Container to the INavigable4MomentumCollection " << endmsg;
	return HLT::NAV_ERROR;
      }
    }
    
    if(msgLvl() <= MSG::DEBUG){
      msg() << MSG::DEBUG << " REGTEST: Produced a Tower Container of Size= " << m_pCaloTowerContainer->size() << endmsg;
      //CaloTowerContainer::const_iterator itrTow= m_pCaloTowerContainer->begin();
      //CaloTowerContainer::const_iterator endTow= m_pCaloTowerContainer->end();
      /*for (; itrTow!=endTow; ++itrTow) {
         if((*itrTow)->e()!=0) msg() << MSG::DEBUG << " Tower with energy... " << (*itrTow)->e() << endmsg;
      }*/
    }

    m_TowerContainerSize = (float)m_pCaloTowerContainer->size() ;
  }

  if (timerSvc()) m_towerContTimer->stop();
  if (timerSvc()){
    m_totalTimer->stop();
  }

    return HLT::OK;
}
