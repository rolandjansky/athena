/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloCellContainerMergerTool
PACKAGE:  offline/Calorimeter/CaloRec

AUTHORS:  David Rousseau
CREATED:  May 12,2004

PURPOSE:  merge the CaloCellContainer into the main one

********************************************************************/

#include "CaloCellContainerMergerTool.h"

#include "StoreGate/StoreGateSvc.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloEvent/CaloCellContainer.h"


/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

CaloCellContainerMergerTool::CaloCellContainerMergerTool(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent)
  :base_class (type, name, parent),
   m_caloCellsKey("")
{
  declareProperty ("CaloCellsName",m_caloCellsKey);
  declareProperty ("CaloNums",m_caloNums);
  m_caloNums.clear();
}




/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
/////////////////////////////////////////////////////////////////////

StatusCode CaloCellContainerMergerTool::initialize()
{
  ATH_CHECK(m_caloCellsKey.initialize());
  //check calo number specified
  if (m_caloNums.size()==0) {
    ATH_MSG_WARNING( " no calo specified. bool hasCalo(caloNum) will not be set!!!  "  );
  }     
  else if  (m_caloNums.size() > static_cast<unsigned int>(CaloCell_ID::NSUBCALO) ) 
    {
      ATH_MSG_ERROR( " More than " 
                     << m_caloNums.size() 
                     << " calos specified. Must be wrong. Stop."  );
      return StatusCode::FAILURE;
    }
  else 
    {
      for (unsigned int index=0; index < m_caloNums.size() ; ++index) {
	if (m_caloNums[index]<0 || m_caloNums[index]>=static_cast<int>(CaloCell_ID::NSUBCALO) ) {
	  ATH_MSG_ERROR( "Invalid calo specification:" 
                         << m_caloNums[index] << "Stop."  );
	  return StatusCode::FAILURE;	
	}	
      }
    }


  return StatusCode::SUCCESS;

}

StatusCode
CaloCellContainerMergerTool::process (CaloCellContainer* theCont,
                                      const EventContext& ctx) const
{
  SG::ReadHandle<CaloCellContainer> theCellContainer (m_caloCellsKey, ctx);


  if (!theCellContainer.isValid()) {
    ATH_MSG_ERROR( "Cannot retrieve CaloCellContainer "
                   << m_caloCellsKey.key()
                   << ". Ignore." );
    return StatusCode::FAILURE;
  } 

  for (const CaloCell* cell : *theCellContainer) {
    //copy cell with the correct concrete type (LAr and Tile)
    CaloCell * theCaloCell=cell->clone();

    //copy each cell with the correct concrete type
    theCont->push_back(theCaloCell);
  }

  //set hasCalo(caloNum) bools
  for (unsigned int index=0; index < m_caloNums.size() ; ++index) {
    CaloCell_ID::SUBCALO caloNum=static_cast<CaloCell_ID::SUBCALO>(m_caloNums[index]);
  
    //specify that a given calorimeter has been filled
    if (theCellContainer->hasCalo(caloNum) )
      {
	ATH_MSG_WARNING( "CaloCellContainer has already been filled with calo " 
                         << caloNum  );
      }
      
    theCont->setHasCalo(caloNum);
  }


  return StatusCode::SUCCESS ;
}








