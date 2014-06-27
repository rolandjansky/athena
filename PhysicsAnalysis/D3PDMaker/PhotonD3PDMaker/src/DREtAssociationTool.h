// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DREtAssociationTool.h 354541 2011-03-26 04:55:52Z mhance $
/**
 * @file EventCommonD3PDMaker/src/DREtAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Associate to particle closest in DR.
 */


#ifndef PHOTOND3PDMAKER_DRASSOCIATIONTOOL_H
#define PHOTOND3PDMAKER_DRASSOCIATIONTOOL_H


#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "EventKernel/INavigable4Momentum.h"
#include "GaudiKernel/ToolHandle.h"


namespace D3PD {


/**
 * @brief Associate to particle closest in DR.
 *
 * This is a single association tool.
 * Given an @c INavigable4Momentum object and a container of such objects,
 * return the object in the container closest in DR to the input object.
 * Optionally, a DR cut may be set; the association fails if there
 * are no objects within that cut.
 *
 * Parameters:
 *  Getter - ICollectionGetterTool instance defining the target collection.
 *  DRCut  - DR cut for the association.  (default: 999)
 *
 * This tool will create a tuple variable `dr' giving the distance
 * in DR to the associated object.
 */
class DREtAssociationTool
  : public SingleAssociationTool<INavigable4Momentum>
{
public:
  typedef SingleAssociationTool<INavigable4Momentum> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  DREtAssociationTool (const std::string& type,
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
  virtual const INavigable4Momentum* get (const INavigable4Momentum& p);


private:
  /// Property: The getter for the target collection.
  ToolHandle<ICollectionGetterTool> m_getter;

  /// Property: DR cut.
  float m_drcut;

  /// Variable: DR to associated object.
  float* m_dr;
};


} // namespace D3PD



#endif // not PHOTOND3PDMAKER_DRASSOCIATIONTOOL_H
