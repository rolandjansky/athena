/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOUTILS_CALOCELLCORRECTION_H
#define CALOUTILS_CALOCELLCORRECTION_H


/********************************************************************

NAME:     CaloCellCorrection.h use to be LArCellCorrection.h
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

class CaloCell;
class CaloCellContainer;

// Includes for Gaudi

//#include "GaudiKernel/Algorithm.h"
//#include "GaudiKernel/AlgTool.h"

#include "AthenaBaseComps/AthAlgTool.h"

static const InterfaceID IID_CaloCellCorrection("CaloCellCorrection", 1 , 0);

class CaloCellCorrection : public AthAlgTool
{

 public:

  CaloCellCorrection(const std::string& type, const std::string& name,
		    const IInterface* parent);
  virtual ~CaloCellCorrection();

  // Main access method: Correct cells in cellCollection:
  StatusCode execute(CaloCellContainer* cellCollection);

  // All derived class must implement the following method:
  virtual void MakeCorrection(CaloCell*) = 0 ; 

  static const InterfaceID& interfaceID() { return IID_CaloCellCorrection;}

 protected:

  // All derived classes can call the following to correct CaloCell object:
  void setenergy(CaloCell* lar_cell, float energy);

  void addenergy(CaloCell* lar_cell, float energy);

  void scaleenergy(CaloCell* lar_cell, float scale);

};

#endif
