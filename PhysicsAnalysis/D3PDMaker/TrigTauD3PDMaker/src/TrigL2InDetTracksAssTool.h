/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//
// Associate a vector of tracks to each trigtau

#ifndef TRIGTAUD3PDMAKER_TRIGL2INDETTRACKSASSTOOL_H
#define TRIGTAUD3PDMAKER_TRIGL2INDETTRACKSASSTOOL_H

#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigParticle/TrigTau.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrack.h"

#include "TrigTauD3PDMaker/TrigReadNavTool.h"

#include <vector>


namespace D3PD {

class TrigL2InDetTracksAssTool
  : public MultiAssociationTool<TrigTau, TrigInDetTrack>
{
public:
  typedef MultiAssociationTool<TrigTau, TrigInDetTrack> Base;

  TrigL2InDetTracksAssTool (const std::string& type,
			    const std::string& name,
			    const IInterface* parent);

  StatusCode initialize();
  /**
   * @brief Start the iteration for a new association.
   * @param p The object from which to associate.
   */

   /**
   * @brief Create any needed tuple variables.
   */

  virtual StatusCode reset (const TrigTau& p);

  /**
   * @brief Return a pointer to the next element in the association.
   *
   * Return 0 when the association has been exhausted.
   */
  virtual const TrigInDetTrack* next();

private:
  /// Property: The trigger object matching tool.
  ToolHandle<TrigTauTool::TrigReadNavTool> m_matchTool;
  /// Property: Chain pattern.
  std::string m_chainPattern;

  std::vector<const TrigInDetTrack*> m_tracks;
  int m_trackCounter;
  int m_numTracks;
};


} // namespace D3PD


#endif // 
