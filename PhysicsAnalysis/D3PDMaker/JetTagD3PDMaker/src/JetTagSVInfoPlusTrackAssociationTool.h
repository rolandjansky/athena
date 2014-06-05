/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file D3PDMaker/JetTagD3PDMaker/src/JetTagSVInfoPlusTrackAssociationTool.h
 * @author Georges Aad
 * @date Oct, 2010
 * @brief association from jets SVInfoPlus object to tracks
 * 
 */

#ifndef JetTagD3PDMaker_JetTagSVInfoPlusTrackAssociationTool_H
#define JetTagD3PDMaker_JetTagSVInfoPlusTrackAssociationTool_H

#include "JetTagFinder.h"
#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "GaudiKernel/ToolHandle.h"

class Jet;
namespace Rec{
  class TrackParticle;
}

namespace Analysis{
  class SVInfoPlus;
}


namespace D3PD {


/**
 * @brief Associate all track particles in jet info (tracks used for tagging or soft lepton tracks)
 *
 */

  
class JetTagSVInfoPlusTrackAssociationTool
  : public MultiAssociationTool<Jet,Rec::TrackParticle>
{
public:

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  JetTagSVInfoPlusTrackAssociationTool (const std::string& type,
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
   * Return 0 when the association has been exhausted.
   */
  const Rec::TrackParticle* next();

  /**
   * @brief Create any needed tuple variables.
   *
   * This is called at the start of the first event.
   */

  virtual StatusCode book();


private:
  JetTagFinder m_finder;


  const Analysis::SVInfoPlus* m_svInfoPlus;

  int m_trkItr;
  int m_trkEnd;
};


} // namespace D3PD


#endif // JetTagD3PDMaker_JetTagSVInfoPlusTrackAssociationTool_H
