// This file's extension implies that it's C, but it is really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloDummyCorrection.h,v 1.1 2009-04-19 03:58:47 ssnyder Exp $
/* @file  CaloDummyCorrection.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2009
 * @brief Dummy correction tool, used to get dummy constants objects
 *        written to the database.
 */


#ifndef CALODUMMYCORRECTION_H
#define CALODUMMYCORRECTION_H


#include "CaloClusterCorrection/CaloClusterCorrection.h"


/*
 * @brief Dummy correction tool, used to get dummy constants objects
 *        written to the database.
 *
 * Each hierarchical tag needs to be defined in each folder, regardless
 * of whether or not that tag actually uses the correction from that folder.
 * To deal with this, we create a dummy object for each folder, with
 * the constant isDummy set to 1.  The tool here is used to set this up
 * when we write to the database.
 */
class CaloDummyCorrection
  : public CaloClusterCorrection
{
public:
  /// Inherit constructor
  using CaloClusterCorrection::CaloClusterCorrection;


  /**
   * @brief Standard initialization method.
   */
  virtual StatusCode initialize() override;


  // derived class implement the real correction.
  // (Does nothing here.)
  virtual void makeCorrection (const Context& myctx,
                               xAOD::CaloCluster*) const override;
};


#endif // not CALODUMMYCORRECTION_H
