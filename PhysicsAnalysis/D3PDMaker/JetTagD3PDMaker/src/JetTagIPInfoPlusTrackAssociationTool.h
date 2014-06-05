/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file D3PDMaker/JetTagD3PDMaker/src/JetTagIPInfoPlusTrackAssociationTool.h
 * @author Georges Aad
 * @date Oct, 2010
 * @brief association from jets IPInfoPlus object to tracks
 * 
 */

#ifndef JetTagD3PDMaker_JetTagIPInfoPlusTrackAssociationTool_H
#define JetTagD3PDMaker_JetTagIPInfoPlusTrackAssociationTool_H

#include "JetTagFinder.h"
#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "GaudiKernel/ToolHandle.h"

class Jet;
namespace Rec{
  class TrackParticle;
}

namespace Analysis{
  class IPInfoPlus;
}


namespace D3PD {


/**
 * @brief Associate tracks in IPInfoPlus to track containers
 *
 */

  
class JetTagIPInfoPlusTrackAssociationTool
  : public MultiAssociationTool<Jet,Rec::TrackParticle>
{
public:

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  JetTagIPInfoPlusTrackAssociationTool (const std::string& type,
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

  float *m_d0val; 
  float *m_d0sig; 
  float *m_z0val; 
  float *m_z0sig; 
  float *m_w2D;
  float *m_w3D;
  float *m_pJP;
  float *m_pJPneg;
  int   *m_grd;
  bool  *m_vzero; 

  const Analysis::IPInfoPlus* m_ipInfoPlus;

  int m_trkItr;
  int m_trkEnd;

  bool m_fillVariables;
};


} // namespace D3PD


#endif // JetTagD3PDMaker_JetTagIPInfoPlusTrackAssociationTool_H
