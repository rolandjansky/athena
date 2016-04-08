/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLE_P3_REC_H
#define TRACKPARTICLE_P3_REC_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include "DataModelAthenaPool/AthenaBarCode_p1.h"

#include "DataModelAthenaPool/ElementLink_p3.h"

namespace Rec
{
    class TrackParticle_p3
    {
    public:
        TrackParticle_p3():m_fitQuality_m_chiSquared{},m_fitQuality_m_numberDoF{}{/* nop */}

        ElementLinkInt_p3         m_originalTrack;
        ElementLinkInt_p3         m_elVxCandidate;
        std::vector< TPObjRef >   m_trackParameters; 

        TPObjRef                  m_trackSummary; 
        float                     m_fitQuality_m_chiSquared;
        float                     m_fitQuality_m_numberDoF;

        /** bitsAndPieces contains the following:
       *     unsigned int m_trackParticleOrigin;
       * Data properties related to the TrackInfo    
         *     unsigned int    m_fitter;
         *     unsigned int    m_particleHypo;
       * bitsets
         *     unsigned int      m_properties;
         *     unsigned int      m_patternRecognition;
         *     unsigned int      m_extPatternRecognition;
       */
        unsigned int              bitsAndPieces[6]; // contains the 6 things below in the same order    



        AthenaBarCode_p1          m_abc;
    };
}

#endif 
