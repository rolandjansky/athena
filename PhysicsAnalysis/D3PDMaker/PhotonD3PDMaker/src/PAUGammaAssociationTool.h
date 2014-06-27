// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/PAUGammaAssociationTool.h
 * @author Mike Hance <hance@hep.upenn.edu>, based on similar tools by scott snyder <snyder@bnl.gov>
 * @date March, 2010
 * @brief Associate photon to PAU gamma object
 */


#ifndef EGAMMAD3PDMAKER_PAUGAMMAASSOCIATIONTOOL_H
#define EGAMMAD3PDMAKER_PAUGAMMAASSOCIATIONTOOL_H


#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "EventKernel/INavigable4Momentum.h"
#include "GaudiKernel/ToolHandle.h"

namespace PAU {
  class egamma;
}

namespace D3PD {


/**
 * @brief Associate to particle closest in DR.
 *
 * This is a single association tool.
 * Given an @c INavigable4Momentum object and a container of PAU::egamma objects,
 * return the object in the container that is associated with the original object.
 *
 * Parameters:
 *  Getter - ICollectionGetterTool instance defining the target collection.
 *
 */
class PAUGammaAssociationTool
  : public SingleAssociationTool<INavigable4Momentum, PAU::egamma>
{
public:
  typedef SingleAssociationTool<INavigable4Momentum, PAU::egamma> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  PAUGammaAssociationTool (const std::string& type,
			   const std::string& name,
			   const IInterface* parent);


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize();


  /**
   * @brief Create any needed tuple variables.
   */
  virtual StatusCode book();


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const PAU::egamma* get (const INavigable4Momentum& p);


private:
  /// Property: The getter for the target collection.
  ToolHandle<ICollectionGetterTool> m_getter;
  float                             m_dr_cut;
};


} // namespace D3PD



#endif // not EGAMMAD3PDMAKER_PAUGAMMAASSOCIATIONTOOL_H
