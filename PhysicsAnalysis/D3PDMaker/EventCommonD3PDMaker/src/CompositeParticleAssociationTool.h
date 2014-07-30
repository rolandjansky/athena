// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file EventCommonD3PDMaker/src/CompositeParticleAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2011
 * @brief Associate from a CompositeParticle to its constituents.
 */


#ifndef EVENTCOMMOND3PDMAKER_COMPOSITEPARTICLEASSOCIATIONTOOL_H
#define EVENTCOMMOND3PDMAKER_COMPOSITEPARTICLEASSOCIATIONTOOL_H


#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "ParticleEvent/CompositeParticle.h"
class INavigable4Momentum;


namespace D3PD {


/**
 * @brief Associate from a CompositeParticle to its constituents.
 */
class CompositeParticleAssociationTool
  : public MultiAssociationTool<CompositeParticle, INavigable4Momentum>
{
public:
  typedef MultiAssociationTool<CompositeParticle, INavigable4Momentum> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  CompositeParticleAssociationTool (const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent);


  /**
   * @brief Start the iteration for a new association.
   * @param p The object from which to associate.
   */
  virtual StatusCode reset (const CompositeParticle& p);


  /**
   * @brief Return a pointer to the next element in the association.
   *
   * Return 0 when the association has been exhausted.
   */
  virtual const INavigable4Momentum* next();


private:
  /// Keep track of the iteration.
  CompositeParticle::navigable_type::object_iter m_it;
  CompositeParticle::navigable_type::object_iter m_end;
};


} // namespace D3PD


#endif // not EVENTCOMMOND3PDMAKER_COMPOSITEPARTICLEASSOCIATIONTOOL_H
