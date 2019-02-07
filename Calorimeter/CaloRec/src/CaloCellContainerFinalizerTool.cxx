/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloCellContainerFinalizerTool
PACKAGE:  offline/Calorimeter/CaloRec

AUTHORS:  David Rousseau
CREATED:  May 12,2004

PURPOSE:  Apply necessary finalising operation to CaloCellContainer

********************************************************************/

#include "CaloCellContainerFinalizerTool.h"

#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"

#include "StoreGate/StoreGateSvc.h"


#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloConstCellContainer.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"


/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

CaloCellContainerFinalizerTool::CaloCellContainerFinalizerTool(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent)
  :base_class (type, name, parent),
   m_theCaloCCIDM (nullptr)
{
}



/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//CaloCellContainerFinalizerTool::~CaloCellContainerFinalizerTool()
//{  }

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
/////////////////////////////////////////////////////////////////////

StatusCode CaloCellContainerFinalizerTool::initialize()
{

  ATH_CHECK(detStore()->retrieve(m_theCaloCCIDM,"CaloCell_ID"));
  return StatusCode::SUCCESS;
}

template <class CONTAINER>
StatusCode CaloCellContainerFinalizerTool::doProcess(CONTAINER* theCont ) const
{

  const unsigned int hashMax=m_theCaloCCIDM->calo_cell_hash_max();
  if (theCont->size()<hashMax) {
    ATH_MSG_DEBUG("CaloCellContainer size " << theCont->size() << " smaller than hashMax: " << hashMax);
  }
  else if (theCont->size()==hashMax)  {
    ATH_MSG_DEBUG("CaloCellContainer size " << theCont->size() << " correspond to hashMax : " << hashMax);
    theCont->setHasTotalSize(true);
  } 	
  else {
    msg(MSG::WARNING) << "CaloCellContainer size " << theCont->size() 
	<< " larger than hashMax ! Too many cells ! " << hashMax << endmsg ;

    }
  
  
  // check whether in order
  if (theCont->checkOrdered()){
    ATH_MSG_DEBUG("CaloCellContainer ordered");
    theCont->setIsOrdered(true);
  } else {	
    ATH_MSG_DEBUG("CaloCellContainer not ordered");
    theCont->setIsOrdered(false);
  }

  /*
  CaloCellContainer::const_iterator itrCell=theCont->begin();
  unsigned int index=0;
  
  for (; itrCell!=theCont->end();++itrCell){
    const CaloDetDescrElement * theDDE=(*itrCell)->caloDDE();
    std::cout << " index " << index << " hash " << theDDE->calo_hash() << std::endl;
    
    ++index ;
  }
  */


  // check whether in order and complete
  if (theCont->checkOrderedAndComplete()){
    ATH_MSG_DEBUG("CaloCellContainer ordered and complete");
    theCont->setIsOrderedAndComplete(true);
  } else {	
    ATH_MSG_DEBUG("CaloCellContainer not ordered or incomplete");
    theCont->setIsOrderedAndComplete(false);
  }
  
  if (!theCont->isOrdered()) {
    ATH_MSG_DEBUG("Now ordering CaloCellContainer");
    theCont->order();
  
  //FIXME check again whether in order
    ATH_MSG_DEBUG("Now check CaloCellContainer ordered");
    if (theCont->checkOrdered()){
      ATH_MSG_DEBUG( "CaloCellContainer ordered");
      theCont->setIsOrdered(true);
    } else {	
      ATH_MSG_DEBUG( "CaloCellContainer not ordered");
      theCont->setIsOrdered(false);
    }


    // check whether in order and complete
    if (theCont->checkOrderedAndComplete()){
      ATH_MSG_DEBUG( " CaloCellContainer ordered and complete");
      theCont->setIsOrderedAndComplete(true);
    } else {	
	ATH_MSG_DEBUG("CaloCellContainer not ordered or incomplete");
	theCont->setIsOrderedAndComplete(false);
    }
  }
  
  theCont->updateCaloIterators();

  return StatusCode::SUCCESS;
}


StatusCode
CaloCellContainerFinalizerTool::process (CaloCellContainer * theCont,
                                         const EventContext& /*ctx*/) const
{
  CHECK( doProcess (theCont) );
  return StatusCode::SUCCESS;
}


StatusCode
CaloCellContainerFinalizerTool::process (CaloConstCellContainer * theCont,
                                         const EventContext& /*ctx*/) const
{
  // Container will automatically be locked when recorded.
  return doProcess (theCont);
}


