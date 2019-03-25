/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloCellContainerFCSFinalizerTool
PACKAGE:  athena/Simulation/FastShower/FastCaloSim

AUTHORS:  David Rousseau (modified by Xiaozhong Huang)
CREATED:  Jan 25,2019

PURPOSE:  Apply necessary finalising operation to CaloCellContainer
	  Remove any checks since the CaloCellContainer is complete 
	  and ordered from the beginning
********************************************************************/

#include "FastCaloSim/CaloCellContainerFCSFinalizerTool.h"

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

CaloCellContainerFCSFinalizerTool::CaloCellContainerFCSFinalizerTool(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent)
  :base_class(type, name, parent) 
{
}


template <class CONTAINER>
StatusCode CaloCellContainerFCSFinalizerTool::doProcess(CONTAINER* theCont ) const
{
  
  theCont->updateCaloIterators();

  return StatusCode::SUCCESS;
}


StatusCode
CaloCellContainerFCSFinalizerTool::process (CaloCellContainer * theCont,
                                         const EventContext& /*ctx*/) const
{
  CHECK( doProcess (theCont) );
  return StatusCode::SUCCESS;
}


StatusCode
CaloCellContainerFCSFinalizerTool::process (CaloConstCellContainer * theCont,
                                         const EventContext& /*ctx*/) const
{
  // Container will automatically be locked when recorded.
  return doProcess (theCont);
}
