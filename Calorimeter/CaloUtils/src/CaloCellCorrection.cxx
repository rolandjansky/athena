/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloCellCorrection.h used to be LArCellCorrection.h
PACKAGE:  offline/Calorimeter/CaloUtils

AUTHORS:  H. Ma, S. Rajagopalan
CREATED:  Dec. 15, 1999

PURPOSE:  Performs cell corrections for CaloCell objects
          This is the base class for individual cell correction classes.


Updated: May 10, 2000    (SR, HM)
         Migrated to Athena Framework from PASO

Updated: Jan. 2, 2001    (SR)
         Adhere to QC standards (based on comments from S. Albrand)

Updated: Jan 2003
	 Made to an AlgTool, 
	 Moved to LArCalorimeter/LArRecUtils

Update : June 2004 David Rousseau
         Change name 
         Work off CaloCell, not LArCell anymore, without friend mechanism
       
********************************************************************/

// Calo Header files:

#include "CaloUtils/CaloCellCorrection.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"

#include "AthenaBaseComps/AthAlgTool.h"

// CONSTRUCTOR:

CaloCellCorrection::CaloCellCorrection(const std::string& type, 
				     const std::string& name, 
				     const IInterface* parent) 
  : AthAlgTool(type, name, parent)
{ }

// DESTRUCTOR:

CaloCellCorrection::~CaloCellCorrection()
{ }

//////////////////////////////////////////////////////////////
// EXECUTE method: Correct cells in input cell container
//////////////////////////////////////////////////////////////

StatusCode CaloCellCorrection::execute (CaloCellContainer* cellCollection,
                                        const EventContext& ctx) const
{

  ATH_MSG_DEBUG("Executing CaloCellCorrection");

  if (!cellCollection) {
    ATH_MSG_ERROR( "Cell Correction tool receives invalid cell Collection"  );
    return StatusCode::FAILURE;
  }

  // Loop over all the CaloCell Objects and call Make Correction.
  // Note that this is the base class of all the concrete correction
  // classes which implement a Make Correction method.

  for (CaloCell* cell : *cellCollection) {
    MakeCorrection ( cell, ctx );
  }

  // Done, Return success

  return StatusCode::SUCCESS;

}

//////////////////////////////////////////////////////////////
// Other methods:
// The following methods "correct" the CaloCell data members.
//////////////////////////////////////////////////////////////


void CaloCellCorrection::setenergy(CaloCell* theCell, float energy) 
{
  theCell->setEnergy( energy );
}

void CaloCellCorrection::addenergy(CaloCell* theCell, float energy) 
{
  theCell->addEnergy( energy );
}

void CaloCellCorrection::scaleenergy(CaloCell* theCell, float scale) 
{
  theCell->scaleEnergy( scale );
}
