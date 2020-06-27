/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloDummyCorrection.cxx,v 1.1 2009-04-19 03:58:47 ssnyder Exp $
/* @file  CaloDummyCorrection.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2009
 * @brief Dummy correction tool, used to get dummy constants objects
 *        written to the database.
 */


#include "CaloDummyCorrection.h"
#include "AthenaKernel/errorcheck.h"


/**
 * @brief Standard initialization method.
 */
StatusCode CaloDummyCorrection::initialize()
{
  // Need the explicit std::string here to avoid matching
  // the template in AlgTool.
  CHECK( setProperty ("isDummy", std::string("1")) );
  return CaloClusterCorrection::initialize();
}


// derived class implement the real correction.
// (Does nothing here.)
void CaloDummyCorrection::makeCorrection (const Context& /*myctx*/,
                                          xAOD::CaloCluster*) const
{
}
