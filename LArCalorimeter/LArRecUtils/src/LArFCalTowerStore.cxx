/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     LArFCalTowerStore
PACKAGE:  offline/LArCalorimeter/LArRecUtils

AUTHORS:  P. Loch
CREATED:  May 2001

PURPOSE:  Intermediate store for cell/tower maps

Updated:  

********************************************************************/
#include "LArFCalTowerStore.h"

// include header files
#include "Identifier/IdentifierHash.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloGeoHelpers/CaloPhiRange.h"

#include "CaloEvent/CaloTowerContainer.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/Service.h"

#include <cmath>

////////////////////////////////
// Constructor and Destructor //
////////////////////////////////

// constructor
LArFCalTowerStore::LArFCalTowerStore()
  : m_indxOffset (0),
    m_indxBound (0)
{
  // some parameters
  m_ndxFCal.resize(3);
  m_ndyFCal.resize(3);
  // set default
  m_ndxFCal[0] = 4;
  m_ndxFCal[1] = 4;
  m_ndxFCal[2] = 6;
  m_ndyFCal[0] = 4;
  m_ndyFCal[1] = 6;
  m_ndyFCal[2] = 6;
  
 }

// destructor
LArFCalTowerStore::~LArFCalTowerStore(){
//  m_dataMatrix.clear(); 
	m_TTCmatrix.clear();
}

////////////////////////
// Build LookUp Table //
////////////////////////

bool LArFCalTowerStore::buildLookUp(const CaloCell_ID& cellIdHelper,
                                    const CaloDetDescrManager& theManager,
                                    CaloTowerContainer* theTowers)
{
  ///////////////////////
  // Store Preparation //
  ///////////////////////

  // messaging
  IMessageSvc* theMsgSvc;
  StatusCode sc = Gaudi::svcLocator()->service("MessageSvc",theMsgSvc);
  if(sc.isFailure()){
    std::cout << "LArFCalTowerStore: could not initialize the MessageSvc " << std::endl;
  }
  MsgStream msg(theMsgSvc,"LArFCalTowerStore");
  
  // get cell id helper
  const LArFCAL_ID& fcalIdHelper = *cellIdHelper.fcal_idHelper();

  // find numerical ranges
  IdentifierHash firstIndex, lastIndex;
  cellIdHelper.calo_cell_hash_range((int)CaloCell_ID::LARFCAL, firstIndex, lastIndex);
  m_indxOffset = (size_t)firstIndex;
  m_indxBound  = (size_t)lastIndex;

  // check
  if ( m_indxBound <= m_indxOffset ){
      msg << MSG::ERROR  << "cannot initialize internal store properly, index offset = " << m_indxOffset << ", index boundary = "
	  << m_indxBound<< " -> module inactivated!"<< endmsg;
      return false;
    }

  // calculate number of rows in store matrix an initialize
//  m_rowsMatrix = m_indxBound - m_indxOffset + 1;
//  for ( size_t iRow=0; iRow < m_rowsMatrix; iRow++ ){
//      m_dataMatrix.push_back(tower_data_store());
//    }

  // report data store size
//  msg << MSG::INFO << "internal matrix set up with " << m_dataMatrix.size()
 //     << " rows: Index Offset: " << m_indxOffset << " Index Boundary: "<< m_indxBound << endmsg;

//  m_weightInModule.resize(m_rowsMatrix);
//  m_binDescriptor.resize(m_rowsMatrix);
 
  m_TTCmatrix.clear();
  m_TTCmatrix.resize(theTowers->size());

  ///////////////
  // Cell Loop //
  ///////////////

  // consistent phi convention
  CaloPhiRange correctPhi;
  
  // some stats 
/*   unsigned int aveTowers = 0;
  unsigned int maxTowers = 0;
  unsigned int minTowers = size_t(-1); */

  for( size_t cellIndex = m_indxOffset;  cellIndex <= m_indxBound; cellIndex++){
	  
      //size_t anIndex = this->getLookupIndex(cellIndex);
	
	  //std::cout<<"cell:"<<anIndex<<std::endl;
      
	  // get cell geometry
      const CaloDetDescrElement* theElement = theManager.get_element(cellIndex);
      if (!theElement) {
        msg << MSG::ERROR<< "Can't find element for index " << cellIndex
            << endmsg;
        return false;
      }
      double xCell  = theElement->x();
      double yCell  = theElement->y();
      double zCell  = theElement->z();
      double dxCell = theElement->dx();
      double dyCell = theElement->dy();

      // get cell logical location
      int thisModule = fcalIdHelper.module(theElement->identify());
      // get cell splitting
      thisModule--;
      double theXBin   = dxCell / (double)m_ndxFCal[thisModule];
      double theYBin   = dyCell / (double)m_ndyFCal[thisModule];
      double theWeight = 1. / ((double)  m_ndxFCal[thisModule] * m_ndyFCal[thisModule]);

//      std::pair<size_t,double> wMod(thisModule,theWeight);
//      m_weightInModule[anIndex] = wMod;
//      std::pair<size_t,size_t> iBin(m_ndxFCal[thisModule],m_ndyFCal[thisModule]);
//      m_binDescriptor[anIndex]  = iBin;

      // loop the cell fragments
      for ( double x = xCell-dxCell/2.;  x < xCell+(dxCell-theXBin/8.)/2.;  x += theXBin ){
	  for ( double y = yCell-dyCell/2.;y < yCell+(dyCell-theYBin/8.)/2.;y += theYBin ){
	      // eta,phi of fragment
	      double r   = sqrt( x * x + y * y + zCell * zCell );
	      double eta = -0.5 * log((r-zCell)/(r+zCell));
	      double phi = correctPhi.fix(atan2(y,x));
	      // get indices
	      CaloTowerContainer::index_t etaIndex = theTowers->flagOutOfRange();
	      CaloTowerContainer::index_t phiIndex =theTowers->flagOutOfRange();
	      if ( theTowers->getTowerIndices(eta,phi,etaIndex,phiIndex) ){
		  	
			  	CaloTowerContainer::index_t towerIndex = theTowers->getTowerIndex(etaIndex,phiIndex);
		  		std::vector<std::pair<unsigned int,double> >::iterator i = m_TTCmatrix[towerIndex].begin();
				std::vector<std::pair<unsigned int,double> >::iterator e = m_TTCmatrix[towerIndex].end();
				bool found=false;
				for (;i<e;++i){
					if (i->first==cellIndex){
						i->second+=theWeight;
						found=true;
						break;
						}
					}
				if (!found)	
				m_TTCmatrix[towerIndex].push_back(std::make_pair(cellIndex,theWeight));
			
			/*
			// check indices in look-up
		  	if ( (m_dataMatrix[anIndex]).find(towerIndex) != (m_dataMatrix[anIndex]).end() ){
		      (m_dataMatrix[anIndex])[towerIndex] += theWeight;
		    }
		  	else{
		      (m_dataMatrix[anIndex])[towerIndex] = theWeight;
		    }
			*/
					
		  	} // index retrieval check
			
	    } // cell y loop
	} // cell x loop
	
/*       // collect some stats
      if ( (m_dataMatrix[anIndex]).size() < minTowers ) minTowers = (m_dataMatrix[anIndex]).size();
      if ( (m_dataMatrix[anIndex]).size() > maxTowers )maxTowers = (m_dataMatrix[anIndex]).size();
      aveTowers += (m_dataMatrix[anIndex]).size(); */
	  
    } // cell index loop

/*   // report
  double cellTowers = (double)aveTowers / (double)m_dataMatrix.size();
  msg << MSG::INFO
      << "FCal cell/tower lookup done - min/ave/max towers/cell: "
      << minTowers << "/" << cellTowers << "/" << maxTowers
      << ", total number of cell fragments "
      << aveTowers
      << endmsg; */
  
		  

  return 1;

}



LArFCalTowerStore::tower_subseg_iterator
LArFCalTowerStore::towers (const CaloTowerSeg::SubSeg& subseg) const
{
  return tower_subseg_iterator::make (towers(), subseg);
}
