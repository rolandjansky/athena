/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//
// Associate a vector of tracks to each tau

#ifndef TAUD3PDMAKER_TAUOTHERTRACKASSOCIATIONTOOL_H
#define TAUD3PDMAKER_TAUOTHERTRACKASSOCIATIONTOOL_H

#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "tauEvent/TauJet.h"

#include <vector>

namespace Rec{
class TrackParticle;
}

namespace D3PD {



class TauOtherTrackAssociationTool
  : public MultiAssociationTool<Analysis::TauJet,Rec::TrackParticle>
{
public:
  typedef MultiAssociationTool<Analysis::TauJet, Rec::TrackParticle> Base;

  TauOtherTrackAssociationTool (const std::string& type,
				   const std::string& name,
				   const IInterface* parent);

  /**
   * @brief Start the iteration for a new association.
   * @param p The object from which to associate.
   */
  virtual StatusCode reset (const Analysis::TauJet& p);

  /**
   * @brief Return a pointer to the next element in the association.
   *
   * Return 0 when the association has been exhausted.
   */
  virtual const Rec::TrackParticle* next();


private:
  const Analysis::TauCommonDetails* m_taudetails;
  unsigned int m_trackCounter;
  unsigned int m_nOtherTrk;
};


} // namespace D3PD


#endif // 
