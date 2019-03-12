/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENASERVICES_DECISIONALG_H
#define ATHENASERVICES_DECISIONALG_H

#include "AthenaBaseComps/FilteredAlgorithm.h"

/** @class DecisionAlg
   * @brief Call through interface to FilteredAlgorithm
   * 
   * @author cranshaw@anl.gov
   * $Id: DecisionAlg.h,v 1.12 2008-12-15 19:14:58 binet Exp $
   */
class DecisionAlg : public FilteredAlgorithm
{
public:
   
   /// Standard algorithm Constructor
   DecisionAlg(const std::string& name, ISvcLocator* pSvcLocator); 
   /// Standard Destructor
   virtual ~DecisionAlg();

};

#endif // ATHENASERVICES_DECISIONALG_H
