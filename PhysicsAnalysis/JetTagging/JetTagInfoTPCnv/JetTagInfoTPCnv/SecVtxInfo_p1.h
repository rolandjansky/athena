/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_SECVTXINFO_P1_H
#define JETTAGINFOTPCNV_SECVTXINFO_P1_H

///
/// Cache all info having to do with the secondary vertex.
///

namespace Analysis
{
  class SecVtxInfo_p1
    {
      friend class SecVtxInfoCnv_p1;

    private:
      /// Base class info
      TPObjRef m_BaseTagInfo;

      /// Info stored in the SecVtxInfo subclass:

      int m_numSelTracksForFit;
      float m_dist;
      float m_rphidist;
      float m_prob;
      float m_mass;
      float m_energyFraction;
      int m_mult;
      int m_NGood2TrackVertices;

      /// Translates to enum FitTYpe.
      int m_fitType;

      /// Translates to a Trk::RecVertex
      TPObjRef m_secVtxPos;

      /// Note that the TrackVec of fitted tracks is declared transient,
      /// and so isn't written out.
    };

}

#endif
