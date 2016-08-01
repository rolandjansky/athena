/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWREC_EFLOWBUILDER_H
#define EFLOWREC_EFLOWBUILDER_H
/********************************************************************

NAME:     eflowBuilder.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  D.R.Tovey
CREATED:  10th November, 2001

********************************************************************/


#include "AthenaBaseComps/AthAlgorithm.h"

class StoreGateSvc;

class eflowBuilder : public AthAlgorithm
{

 public:

  // contructor
  eflowBuilder(const std::string& name, ISvcLocator* pSvcLocator);

  ~eflowBuilder();
  // Gaudi algorithm hooks

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

  std::string m_eflowPreparationAlgName;
  std::string m_eflowObjectBuilderAlgName;
  std::string m_eflowQuantitiesAlgName;

  // Pointers to the corresponding sub-algorithms:
  AthAlgorithm* m_eflowPreparationAlg;   // Build calo objects
  AthAlgorithm* m_eflowObjectBuilderAlg;       // Build eflow objects
  AthAlgorithm* m_eflowQuantitiesAlg;          // Calculate eflow, pTmiss

};
#endif
