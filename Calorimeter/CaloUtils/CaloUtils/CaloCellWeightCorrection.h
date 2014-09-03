/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOUTILS_CALOCELLWEIGHTCORRECTION_H
#define CALOUTILS_CALOCELLWEIGHTCORRECTION_H


/********************************************************************

NAME:     CaloCellWeightCorrection.h 
PACKAGE:  offline/Calorimeter/CaloUtils

AUTHORS:  Kyle Cranmer <cranmer@cern.ch>
CREATED:  February, 2005

PURPOSE: Loops over list of ICellWeightTools and applies weight to cell

********************************************************************/

class CaloCell;
class CaloCellContainer;

#include "CaloUtils/CaloCellCorrection.h"
#include "CaloInterface/ICellWeightTool.h"


class CaloCellWeightCorrection : public CaloCellCorrection
{

 public:

  CaloCellWeightCorrection(const std::string& type, const std::string& name,
		    const IInterface* parent);
  virtual ~CaloCellWeightCorrection();

  // Main access method: Correct cells in cellCollection:
  StatusCode initialize() ;  
  StatusCode execute(CaloCellContainer* cellCollection);

  // All derived class must implement the following method:
  virtual void   MakeCorrection(CaloCell*); 


 protected:

  // All derived classes can call the following to correct CaloCell object:
  //  void setenergy(CaloCell* theCell, float energy);
  //  void addenergy(CaloCell* theCell, float energy);

 private:

  // properties

  std::vector< std::string > m_cellWeightToolNames;
  std::vector< ICellWeightTool* > m_cellWeightTools;
  

};

#endif
