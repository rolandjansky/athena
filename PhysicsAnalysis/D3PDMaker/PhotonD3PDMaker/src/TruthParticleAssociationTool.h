// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id $
/**
 * @file PhotonD3PDMaker/src/TruthParticleAssociationTool.h
 * @author Mike Hance <hance@hep.upenn.edu>, based on similar tools by scott snyder <snyder@bnl.gov>
 * @date December, 2011
 * @brief Associate an object to a TruthParticle using PAU tools
 */


#ifndef D3PDMAKER_GENPARTICLEASSOCIATIONTOOL_H
#define D3PDMAKER_GENPARTICLEASSOCIATIONTOOL_H


#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "EventKernel/INavigable4Momentum.h"
#include "GaudiKernel/ToolHandle.h"

#include "PhotonAnalysisUtils/IPAUtruthTool.h"

#include "McParticleEvent/TruthParticle.h"

namespace D3PD {


/**
 * @brief Associate to particle closest in DR.
 *
 * This is a single association tool.
 * Given an @c INavigable4Momentum object and a container of objects,
 * return the object in the container that is associated with the original object.
 *
 * Parameters:
 *  Getter - ICollectionGetterTool instance defining the target collection.
 *
 */
class TruthParticleAssociationTool
  : public SingleAssociationTool<INavigable4Momentum, TruthParticle>
{
public:
  typedef SingleAssociationTool<INavigable4Momentum, TruthParticle> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TruthParticleAssociationTool (const std::string& type,
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
  virtual const TruthParticle* get (const INavigable4Momentum& p);


private:
  ToolHandle<IPAUtruthTool>         m_PAUtruthTool;
};


} // namespace D3PD



#endif
