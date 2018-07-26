/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXEL_CLUSTER_ON_TRACK_p2_TRK_H
#define PIXEL_CLUSTER_ON_TRACK_p2_TRK_H

#include "AthenaPoolUtilities/TPObjRef.h"

#include "DataModelAthenaPool/ElementLink_p1.h"

namespace InDet
{
    class PixelClusterOnTrack_p2
    {
    public:
        PixelClusterOnTrack_p2():m_isbroad(false)  {}


        unsigned long long            m_id{};
        TPObjRef                      m_localParams;
        TPObjRef                      m_localErrMat;
        
        IdentifierHash::value_type     m_idDE{};
        bool                           m_isbroad{};


        bool			m_hasClusterAmbiguity{};
        bool			m_isFake{};
        float                      m_energyLoss{};     
      /** Link to PRD */
        ElementLinkInt_p1  m_prdLink;
    };
}

#endif 
