/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOREC_CALOCELLCONTAINERCHECKERTOOL_H
#define CALOREC_CALOCELLCONTAINERCHECKERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "CaloInterface/ICaloConstCellMakerTool.h"

class CaloCellContainerCheckerTool: public AthAlgTool,
				    virtual public ICaloCellMakerTool,
				    virtual public ICaloConstCellMakerTool 

{
 
public:    
  
  CaloCellContainerCheckerTool(const std::string& type, 
			       const std::string& name, 
			       const IInterface* parent) ;


  virtual StatusCode initialize() ; 

  virtual StatusCode process( CaloCellContainer * theCellContainer) ;
  virtual StatusCode process( CaloConstCellContainer * theCellContainer) ;

 private:
  StatusCode doProcess( const CaloCellContainer * theCellContainer) ;

  int m_eventsToCheck;
  int m_eventsLeftToCheck;
};

#endif

