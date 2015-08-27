/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VXTRACKATVERTEX_P1_H
#define VXTRACKATVERTEX_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include "DataModelAthenaPool/ElementLink_p2.h"
#include "DataModelAthenaPool/ElementLinkContNames_p2.h"

namespace Trk
{
 class VxTrackAtVertex_p1
 
 {
   public:

    VxTrackAtVertex_p1() : m_trkWeight(0.0), m_VertexCompatibility(0.0), m_typeOfLink(0) {}
  
    float m_trkWeight;
    float m_VertexCompatibility;  
    TPObjRef m_perigeeAtVertex;
   
    // some sort of unnice hack to store what type the link is
    // 0 ... ElementLink< TrackCollection >
    // 1 ... ElementLink< TrackParticleContainer >
    unsigned short int        m_typeOfLink;
    ElementLinkInt_p2         m_origTrack;
    ElementLinkContNames_p2   m_origTrackNames;
    // FitQuality_p1
    TPObjRef		m_fitQuality;  
 }; //end of class defintions

}//end of namespace defintions

#endif //VXTRACKATVERTEX_P1_H
