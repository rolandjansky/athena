// This file's extension implies that it's C, but it is really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  /**
   * @brief Constructor.
   * @param type The type of this tool.
   * @param name The name of this tool.
   * @param parent The parent of this tool.
   */
  CaloDummyCorrection(const std::string& type,
                      const std::string& name,
                      const IInterface* parent);


  /**
   * @brief Standard initialization method.
   */
  virtual StatusCode initialize() override;


  // derived class implement the real correction.
  // (Does nothing here.)
  virtual void makeCorrection(const EventContext& ctx,
                              xAOD::CaloCluster*) const override;


  /// Ignore non-existing properties.
  using CaloClusterCorrection::setProperty;
  virtual StatusCode setProperty (const Property& p) override;
};


#endif // not CALODUMMYCORRECTION_H
