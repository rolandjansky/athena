/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     LocalNoiseSuppressionTool.h
PACKAGE:  offline/Calorimeter/CaloUtils

AUTHORS:  Kyle Cranmer <cranmer@cern.ch>
CREATED:  Feb 8, 2005

PURPOSE: Performs Local Noise Suppression CaloCell objects Inherits
          from CaloCellCorrection, which is the base class for
          individual cell correction classes.
       
********************************************************************/

// Calo Header files:

#include "CaloUtils/LocalNoiseSuppressionTool.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

// Event Header Files:
#include "GeoModelInterfaces/IGeoModelSvc.h"

// For Gaudi
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IService.h"

// CONSTRUCTOR:

LocalNoiseSuppressionTool::LocalNoiseSuppressionTool(const std::string& type, 
				     const std::string& name, 
				     const IInterface* parent) 
  : AthAlgTool(type, name, parent),
    m_maxPrior(.999), m_cutThresholdOnPrior(999.),
    m_cutInSigma(3.), m_symmetricCut(true),
    m_usePileUp(false),
    m_caloCellContainerName(""), 
    m_testStatistic("max"), m_neighborOption("super3D"),
    m_s0(1.05), m_s1(0.017), m_o0(1.1), m_o1(0.035),
    m_nNeighbors(0),
    m_noiseTool("CaloNoiseTool/CaloNoiseToolDefault"),
    m_calo_dd_man(nullptr),
    m_calo_id(nullptr),
    m_caloCellContainer(nullptr)
{

  declareInterface<ICellWeightTool>(this);

  declareProperty( "maxPrior", m_maxPrior);
  declareProperty( "cutThresholdOnPrior", m_cutThresholdOnPrior);
  declareProperty( "cutInSigma", m_cutInSigma);
  declareProperty( "symmetricCut", m_symmetricCut);

  declareProperty( "usePileUp", m_usePileUp);  
  
  declareProperty("CaloCellContainerName",m_caloCellContainerName);

  declareProperty("CaloNoiseTool", m_noiseTool);  
  declareProperty("testStatisticOption",m_testStatistic);
  declareProperty("neighborOption",m_neighborOption);

  declareProperty("scaleConst", m_s0);
  declareProperty("scaleSlope", m_s1);
  declareProperty("offsetConst",m_o0);
  declareProperty("offsetSlope",m_o1);


 }

// DESTRUCTOR:

LocalNoiseSuppressionTool::~LocalNoiseSuppressionTool()
{ 

  m_caloCellContainer = 0;
  
}

//////////////////////////////////////////////////////////////
// Gaudi INITIALIZE method
//////////////////////////////////////////////////////////////

StatusCode LocalNoiseSuppressionTool::initialize() {

 // MSGStream object to output messages from your algorithm

  MsgStream log( msgSvc(), name() );
  StoreGateSvc* detStore;
  if (service("DetectorStore", detStore).isFailure()) {
    log << MSG::ERROR   << "Unable to access DetectoreStore" << endreq ;
    return StatusCode::FAILURE;
  }

  const IGeoModelSvc *geoModel=0;
  StatusCode sc = service("GeoModelSvc", geoModel);
  if(sc.isFailure())
  {
    log << MSG::ERROR << "Could not locate GeoModelSvc" << endreq;
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
    sc = detStore->regFcn(&IGeoModelSvc::geoInit,
			  geoModel,
			  &LocalNoiseSuppressionTool::geoInit,this);
    if(sc.isFailure())
    {
      log << MSG::ERROR << "Could not register geoInit callback" << endreq;
      return sc;
    }
  }
  return sc;
}

StatusCode
LocalNoiseSuppressionTool::geoInit(IOVSVC_CALLBACK_ARGS)
{
  MsgStream log( msgSvc(), name() );

//---- retrieve the noisetool ----------------

  if(m_noiseTool.retrieve().isFailure()){
    log << MSG::INFO
	<< "Unable to find tool for CaloNoiseTool"
	<< endreq;
  }    else {
    log << MSG::INFO << "Noise Tool retrieved" << endreq;
  } 


  // pointer to detector manager:
  m_calo_dd_man  = CaloDetDescrManager::instance(); 
  m_calo_id   = m_calo_dd_man->getCaloCell_ID();


  log << MSG::INFO << "The tool " << name() << " has been initialized with the following settings: " <<
   "\n\tmaxPrior = " << m_maxPrior<<
   "\n\tcutThresholdOnPrior = " << m_cutThresholdOnPrior<<
   "\n\tcutInSigma = " << m_cutInSigma<<
   "\n\tsymmetricCut = " << m_symmetricCut<<
   "\n\tusePileUp = " << m_usePileUp<<  
   "\n\tCaloCellContainerName = " <<m_caloCellContainerName<<
   "\n\ttestStatisticOption = " <<m_testStatistic<<
   "\n\tneighborOption = " <<m_neighborOption<<
   "\n\tscaleConst = " << m_s0<<
   "\n\tscaleSlope = " << m_s1<<
   "\n\toffsetConst = " <<m_o0<<
   "\n\toffsetSlope = " <<m_o1<< endreq;
  


  // Return status code.
  return StatusCode::SUCCESS;
}



void LocalNoiseSuppressionTool::setCaloCellContainer(const CaloCellContainer* cellColl ){
  m_caloCellContainer = cellColl;
  
}

double LocalNoiseSuppressionTool::wtCell( const CaloCellContainer* cellColl, 
					  const CaloCell* theCell){
  m_caloCellContainer = cellColl;
  
  return this->wtCell(theCell);
}

double LocalNoiseSuppressionTool::wtCell( const CaloCell* theCell ) 
{
  
  double prior = this->getPrior( theCell );
  double Em = theCell->e();

  double rmsNoise = cellSigma(theCell);
  double weight = (1-prior) / ( (1.-prior) + prior*exp(-Em*Em/(2.*rmsNoise*rmsNoise)) ); 

  if ( this->cutCell(theCell) )    weight = 0;

  return weight;
  
}

bool LocalNoiseSuppressionTool::cutCell( const CaloCellContainer* cellColl, 
					 const CaloCell* theCell){
  m_caloCellContainer = cellColl;
  
  return this->cutCell(theCell);
}

bool LocalNoiseSuppressionTool::cutCell( const CaloCell* theCell) {

  double prior = this->getPrior( theCell );
  double Em = theCell->e();
  double rmsNoise = cellSigma(theCell);

  if (Em < 0 && !m_symmetricCut) 
    return true;

  else if (prior > m_cutThresholdOnPrior && ( m_symmetricCut && fabs(Em) < m_cutInSigma*rmsNoise ) )
    return true	;
  
  else if (prior > m_cutThresholdOnPrior && (!m_symmetricCut &&      Em  < m_cutInSigma*rmsNoise)  )
    return true	;

  else
    return false;
  
  
}


double LocalNoiseSuppressionTool::getPrior( const CaloCellContainer* cellColl, 
					    const CaloCell* theCell){
  m_caloCellContainer = cellColl;
  
  return this->getPrior(theCell);
}

double LocalNoiseSuppressionTool::getPrior( const CaloCell* theCell) {
  
  MsgStream log( msgSvc(), name() );

  double ret =0;
  
  double testStat = this->getTestStatistic(theCell);
  
  if ( m_testStatistic == "sum" )        // For sum E/sigma
    ret = erfc(testStat / sqrt(2. * m_nNeighbors ) );

  else if ( m_testStatistic == "max" ) { // For max E/sigma
    ret = .5*( 1. - erf( this->scale() * (testStat - this->offset()) ) );
    
  }
  else{
    
    log << MSG::ERROR << "TestStatistic not set correctly.  Value = " << 
      m_testStatistic << endreq;

    ret  = 0;
  }
  
  
  if (ret > m_maxPrior ) ret = m_maxPrior;
  
  return ret;

}




double LocalNoiseSuppressionTool::getTestStatistic(const CaloCellContainer* cellColl,
						   const CaloCell* theCell){

  m_caloCellContainer = cellColl;
  return this->getTestStatistic(theCell);
  
  
}

double LocalNoiseSuppressionTool::getTestStatistic( const CaloCell* theCell) 
{
  

  MsgStream log( msgSvc(), name() );
  // this tool needs access to the neighboring cells via the container
  // The interface of ICellWeightTool does not support that, so until
  // a better solution is found, we will retrieve the container in
  // initialize if the caloCellContainerName is specified.
  if (m_caloCellContainerName != "" ) {
    StatusCode sc = evtStore()->retrieve(m_caloCellContainer, m_caloCellContainerName);

    if(sc.isFailure()  ||  !m_caloCellContainer) {
      log << MSG::ERROR 
	  << "the CaloCellContainer " << m_caloCellContainerName 
	  << "was not found in TDS"
	  << endreq; 
      return StatusCode::SUCCESS;
    }    
  }


  LArNeighbours::neighbourOption nOption;
  
  if (m_neighborOption == "all2D") 
    nOption = LArNeighbours::all2D;
  else if (m_neighborOption == "all3D") 
    nOption = LArNeighbours::all3D;
  else// (m_neighborOption == "super3D") 
    nOption = LArNeighbours::super3D;

  std::vector<IdentifierHash> theNNeighbors;
  Identifier myId = theCell->ID();
  IdentifierHash myHashId;
  int otherSubDet;
  myHashId = m_calo_id->subcalo_cell_hash(myId,otherSubDet);
  m_calo_id->get_neighbours(myHashId,nOption,theNNeighbors);

  double ret = 0;
  
  if ( m_testStatistic == "sum" ) {      // For sum E/sigma
    double sumEoverSigma = 0;
    m_nNeighbors = 0;
    for (unsigned int i=0; i<theNNeighbors.size(); i++) {
      const CaloCell* aNeighbor = m_caloCellContainer->findCell(theNNeighbors[i]);

      if (aNeighbor != 0) {
	m_nNeighbors++;  // needed in getPrior
	sumEoverSigma  += aNeighbor->e()
	  / this->cellSigma(aNeighbor);
      }
    }
    ret = sumEoverSigma;    
  } 


  else if ( m_testStatistic == "max" ) {   //	 For max E/sigma
    double maxEoverSigma = -9999.;
    double thisEoverSigma = -9999.;
  
    m_nNeighbors = 0;
    for (unsigned int i=0; i<theNNeighbors.size(); i++) {
      const CaloCell* aNeighbor = m_caloCellContainer->findCell(theNNeighbors[i]);

      if (aNeighbor != 0) {      
	m_nNeighbors++; // needed in getPrior
	thisEoverSigma = aNeighbor->e() 
	  / this->cellSigma(aNeighbor);

	if ( thisEoverSigma > maxEoverSigma )
	  maxEoverSigma = thisEoverSigma;
      }    	
    }
    ret = maxEoverSigma;
  }

  else {
  
    // log oops
    
  }

  return ret;
  
}


double LocalNoiseSuppressionTool::cellSigma( const CaloCell* theCell) {

  double ret = 0;
  
  if (m_usePileUp == false) 
    //    ret = m_noiseTool->elecNoiseRMSHighestGain(theCell);  
    ret = m_noiseTool->getNoise(theCell,ICalorimeterNoiseTool::ELECTRONICNOISE_HIGHESTGAIN);  
  //  else if (m_usePileUp == true && m_nMinBias >0 ) 
  //    ret = m_noiseTool->totalNoiseRMSHighestGain(theCell,m_nMinBias);  
  //    ret = m_noiseTool->getNoise(theCell,ICalorimeterNoiseTool::TOTALNOISE_HIGHESTGAIN);  
  else 
    // use noise tool's configuration
    //    ret = m_noiseTool->totalNoiseRMSHighestGain(theCell, -1);  
    ret = m_noiseTool->getNoise(theCell,ICalorimeterNoiseTool::TOTALNOISE_HIGHESTGAIN);  
    
  return ret;  
}


double LocalNoiseSuppressionTool::scale() {
  // the distribution of the "max" test statistic is complicated, 
  //  but has been from Toy MC so to linear functions. 
  //  The scale() and offset() methods are used in the transformation 
  //  of the test statistic to a prior.
  return m_s0 + m_s1*m_nNeighbors;  
}

double LocalNoiseSuppressionTool::offset() {
  // the distribution of the "max" test statistic is complicated, 
  //  but has been from Toy MC so to linear functions. 
  //  The scale() and offset() methods are used in the transformation 
  //  of the test statistic to a prior.

  return m_o0 + m_o1*m_nNeighbors;  
}




