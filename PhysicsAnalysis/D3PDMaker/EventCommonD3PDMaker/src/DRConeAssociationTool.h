// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file EventCommonD3PDMaker/src/DRConeAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Associate all particles within a DR cut.
 */


#ifndef EVENTCOMMOND3PDMAKER_DRCONEASSOCIATIONTOOL_H
#define EVENTCOMMOND3PDMAKER_DRCONEASSOCIATIONTOOL_H


#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "GaudiKernel/ToolHandle.h"
class INavigable4Momentum;


namespace D3PD {


/**
 * @brief Associate all particles within a DR cut.
 *
 * This is a multiple association tool.
 * Given an @c INavigable4Momentum object and a container of such objects,
 * return the all objects in the container within a DR cone of the
 * input object.
 *
 * Parameters:
 *  Getter - ICollectionGetterTool instance defining the target collection.
 *  DRCut  - DR radius of the cone.
 */
class DRConeAssociationTool
  : public MultiAssociationTool<INavigable4Momentum>
{
public:
  typedef MultiAssociationTool<INavigable4Momentum> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  DRConeAssociationTool (const std::string& type,
                         const std::string& name,
                         const IInterface* parent);


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize();


  /**
   * @brief Start the iteration for a new association.
   * @param p The object from which to associate.
   */
  virtual StatusCode reset (const INavigable4Momentum& p);


  /**
   * @brief Return a pointer to the next element in the association.
   *
   * Return 0 when the association has been exhausted.
   */
  virtual const INavigable4Momentum* next();


private:
  /// Property: The getter for the target collection.
  ToolHandle<ICollectionGetterTool> m_getter;

  /// Property: DR cut.
  float m_drcut;

  /// The input object.
  const INavigable4Momentum* m_p;
};


} // namespace D3PD


#endif // not EVENTCOMMOND3PDMAKER_DRCONEASSOCIATIONTOOL_H

