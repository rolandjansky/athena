/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloCellContaineCorrectorTool
PACKAGE:  offline/Calorimeter/CaloRec

AUTHORS:  David Rousseau
CREATED:  June 5,2004

PURPOSE:  Apply cell correction to CaloCellContainer

********************************************************************/

#include "CaloRec/CaloCellContainerCorrectorTool.h"

#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"

#include "StoreGate/StoreGateSvc.h"


#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"
#include "CaloIdentifier/CaloCell_ID.h"

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

CaloCellContainerCorrectorTool::CaloCellContainerCorrectorTool(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent)
  :AthAlgTool(type, name, parent),
   m_caloSelection(false)
{
  declareInterface<ICaloCellMakerTool>(this); 
  declareProperty("CaloNums",m_caloNums);
  declareProperty("CellCorrectionToolNames",m_cellCorrectionTools);
  m_caloNums.clear();
  //default: process all calo
  m_caloNums.push_back( static_cast<int>(CaloCell_ID::NSUBCALO) );
}




/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
/////////////////////////////////////////////////////////////////////

StatusCode CaloCellContainerCorrectorTool::initialize() {

  //Retrieve tools
  StatusCode sc = m_cellCorrectionTools.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to retrieve tools " << m_cellCorrectionTools << endreq;
    } 

  unsigned int nSubCalo=static_cast<unsigned int>(CaloCell_ID::NSUBCALO) ;

  //check calo number specified
  m_caloSelection = true ;
  if (m_caloNums.size()==0) {
    msg(MSG::WARNING) << " no calo specified for correction. Will do nothing. " << endreq;
    return StatusCode::SUCCESS;
  } else if  (m_caloNums.size()>nSubCalo ) {
    msg(MSG::ERROR) << " More than " 
		    << nSubCalo << " calo specified. Must be wrong. Stop." << endreq;
    return StatusCode::FAILURE;
  }  else if  (m_caloNums.size()==1 && m_caloNums[0]==static_cast<int>(nSubCalo)) {
    m_caloSelection = false ;
    ATH_MSG_INFO("Correction will be applied on all calo.");
  } else {
    for (unsigned int index=0; index < m_caloNums.size() ; ++index) {
      if (m_caloNums[index]<0 || m_caloNums[index]>=static_cast<int>(nSubCalo) ) {
	msg(MSG::ERROR) << "Invalid calo specification:" << m_caloNums[index] << "Stop." << endreq ;
	return StatusCode::FAILURE;
      } else {
	ATH_MSG_INFO("Correction will be applied on calo:" << static_cast<int>(m_caloNums[index]));
      }
    }
  }
  

  ATH_MSG_INFO("Initialization completed successfully");
  return StatusCode::SUCCESS;

}

StatusCode CaloCellContainerCorrectorTool::process(CaloCellContainer * theCont )
{
  if (!m_caloSelection) {
    // no selection mode (faster)
    CaloCellContainer::iterator itrCellBeg=theCont->begin();
    CaloCellContainer::iterator itrCellEnd=theCont->end();
    
    StatusCode sc = processOnCellIterators(itrCellBeg, itrCellEnd );
    if (sc.isFailure()) 
      msg(MSG::WARNING) << "Failure from processOnCellIterators" << endreq ;
  }else {
    // selection mode 

    for (std::vector<int>::const_iterator itrCalo=m_caloNums.begin();itrCalo!=m_caloNums.end();++itrCalo){

      CaloCell_ID::SUBCALO caloNum=static_cast<CaloCell_ID::SUBCALO>(*itrCalo);

      CaloCellContainer::iterator itrCellBeg=theCont->beginCalo(caloNum);
      CaloCellContainer::iterator itrCellEnd=theCont->endCalo(caloNum);

      if (!theCont->hasCalo(caloNum))
      {
	msg(MSG::WARNING) << " Attempt to apply correction but CaloCellContainer has not been filled for this calo : " 
	    << *itrCalo << endreq ;
      } else 
      {
	StatusCode sc=processOnCellIterators(itrCellBeg, itrCellEnd );
	if (sc.isFailure()) 
	  msg(MSG::WARNING) << "Failure from processOnCellIterators for calo "
	      << static_cast<int> (caloNum)
	      << endreq ;

      }
	  

    } 

  }

  

  return StatusCode::SUCCESS ;
}

StatusCode CaloCellContainerCorrectorTool::processOnCellIterators(const CaloCellContainer::iterator &  itrCellBeg, const CaloCellContainer::iterator & itrCellEnd )
{
  // not clear what s the best way to do the loop

  CellCorrectionToolIterator itrToolBeg = m_cellCorrectionTools.begin();
  CellCorrectionToolIterator itrToolEnd = m_cellCorrectionTools.end();
  CellCorrectionToolIterator itrTool ;


  CaloCellContainer::iterator itrCell;

  //if only one tool do not iterate (optimisation)
  if (m_cellCorrectionTools.size()==1) {
    for (itrCell=itrCellBeg;itrCell!=itrCellEnd;++itrCell) {
      (*itrToolBeg)->MakeCorrection(*itrCell);
    }
  }else {
    for (itrCell=itrCellBeg;itrCell!=itrCellEnd;++itrCell) {
      for (itrTool=itrToolBeg;itrTool!=itrToolEnd;++itrTool) {
	(*itrTool)->MakeCorrection(*itrCell);
      }
    }
  }

  return StatusCode::SUCCESS;
  

}






