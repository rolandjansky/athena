/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file D3PDMaker/JetTagD3PDMaker/src/JetTagMSVVtxInfoTrackAssociationTool.h
 * @author Georges Aad
 * @date Apr, 2011
 * @brief association from MSVVtxInfo object to tracks
 * 
 */

#ifndef JetTagD3PDMaker_JetTagMSVVtxInfoTrackAssociationTool_H
#define JetTagD3PDMaker_JetTagMSVVtxInfoTrackAssociationTool_H

#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "GaudiKernel/ToolHandle.h"


namespace Rec{
  class TrackParticle;
}

namespace Analysis{
  class MSVVtxInfo;
}


namespace D3PD {


/**
 * @brief Associate all track particles used to for a vertex for the MultiSV tagger
 *
 */

  
class JetTagMSVVtxInfoTrackAssociationTool
  : public MultiAssociationTool<Analysis::MSVVtxInfo,Rec::TrackParticle>
{
public:

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  JetTagMSVVtxInfoTrackAssociationTool (const std::string& type,
                         const std::string& name,
                         const IInterface* parent);


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize();


  /**
   * @brief Start the iteration for a new association.
   * @param p The object from which to associate.
   */
  virtual StatusCode reset (const Analysis::MSVVtxInfo& p);


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


  const Analysis::MSVVtxInfo* m_mSVVtxInfo;

  unsigned int m_trkItr;
  unsigned int m_trkEnd;


};


} // namespace D3PD


#endif // JetTagD3PDMaker_JetTagMSVVtxInfoTrackAssociationTool_H
