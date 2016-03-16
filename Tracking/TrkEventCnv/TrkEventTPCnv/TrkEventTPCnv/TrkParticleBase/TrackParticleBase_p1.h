/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TrackParticleBase_p1.h
// author: Andreas Wildauer
//
//-----------------------------------------------------------------------------

#ifndef TRACKPARTICLEBASE_P1_TRK_H
#define TRACKPARTICLEBASE_P1_TRK_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include "DataModelAthenaPool/ElementLink_p2.h"
#include "DataModelAthenaPool/ElementLinkContNames_p2.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"

namespace Trk
{
   class TrackParticleBase_p1
   {
  public:
     TrackParticleBase_p1() : 
        m_trackParticleOrigin(0), m_fitter(0), m_particleHypo(Trk::undefined), 
        m_properties(0),m_patternRecognition(0),m_extPatternRecognition(0)
     {}

     ElementLinkInt_p2         m_originalTrack;
     ElementLinkContNames_p2   m_originalTrackNames;
     
     ElementLinkInt_p2         m_elVxCandidate;
     ElementLinkContNames_p2   m_elVxCandidateNames;
    
     std::vector< TPObjRef > m_trackParameters; // mutable std::vector<const Trk::ParametersBase*>
     
     unsigned int m_trackParticleOrigin;
     
     TPObjRef m_trackSummary;   // const Trk::TrackSummary*
     TPObjRef m_fitQuality;     // const Trk::FitQuality*
     
 // data properties related to the TrackInfo    
     unsigned int	m_fitter;
     unsigned int	m_particleHypo;
 
 //bitsets
     unsigned int      m_properties;
     unsigned int      m_patternRecognition;
     unsigned int      m_extPatternRecognition;
   };
}

#endif // TRACKPARTICLEBASE_P1_TRK_H
