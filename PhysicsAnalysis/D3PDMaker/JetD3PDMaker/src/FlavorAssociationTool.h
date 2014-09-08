// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMaker/JetD3PDMaker/src/FlavorAssociationTool.cxx
 * @author Erik Devetak <erik.devetak@cern.ch>
 * @date Mar, 2009
 * @brief setting up an association of jets to tracks
 *        represented by indices. 
 */

#ifndef FLAVORASSOCIATIONTOOL_H
#define FLAVORASSOCIATIONTOOL_H

#include "JetEvent/Jet.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include <Particle/TrackParticle.h>


namespace Rec{
class TrackParticle;
}

namespace D3PD {


  //class Jet;
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

//class Jet;
  
typedef D3PD::MultiAssociationTool<Jet,Rec::TrackParticle> TrackToJetBase;


class FlavorAssociationTool
  : public TrackToJetBase 
{
public:

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  FlavorAssociationTool (const std::string& type,
                         const std::string& name,
                         const IInterface* parent);


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize();


  /**
   * @brief Start the iteration for a new association.
   * @param p The object from which to associate.
   */
  virtual StatusCode reset (const Jet& p);


  /**
   * @brief Return a pointer to the next element in the association.
   *
   * Return 0 when the association has been exhausted.
   */
  const Rec::TrackParticle* next();


private:
   /// The input object.
  std::vector<const Rec::TrackParticle*>::iterator m_trkItr;
  std::vector<const Rec::TrackParticle*>::iterator m_trkEnd;
};


} // namespace D3PD


#endif // not FLAVORASSOCIATIONTOOL_H
