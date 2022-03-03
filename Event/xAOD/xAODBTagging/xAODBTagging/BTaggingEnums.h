// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BTaggingEnums.h 644962 2015-02-07 13:11:34Z filthaut $
#ifndef XAODBTAGGING_BTAGGINGENUMS_H
#define XAODBTAGGING_BTAGGINGENUMS_H

namespace xAOD
{

  /// @name b-tagging information
  enum BTagInfo
    {
      /// @brief SV0 : Number of good tracks in jet
      SV0_NGTinJet = 100,
      /// @brief SV0 : Number of good tracks in vertex
      SV0_NGTinSvx = 101,
      /// @brief SV0 : Number of 2-track pairs
      SV0_N2Tpair = 102,
      /// @brief SV0 : vertex mass
      SV0_masssvx = 103,
      /// @brief SV0 : energy fraction
      SV0_efracsvx = 104,
      /// @brief SV0 : 3D vertex significance
      SV0_normdist = 105,

      /// @brief SV1 : Number of good tracks in jet
      SV1_NGTinJet = 200,
      /// @brief SV1 : Number of good tracks in vertex
      SV1_NGTinSvx = 201,
      /// @brief SV1 : Number of 2-track pairs
      SV1_N2Tpair = 202,
      /// @brief SV1 : vertex mass
      SV1_masssvx = 203,
      /// @brief SV1 : energy fraction
      SV1_efracsvx = 204,
      /// @brief SV1 : 3D vertex significance
      SV1_normdist = 205,

      /// @brief JetFitter : Number of vertices
      JetFitter_nVTX = 300,
      /// @brief JetFitter : Number of single tracks
      JetFitter_nSingleTracks = 301,
      /// @brief JetFitter : Number of tracks at vertex
      JetFitter_nTracksAtVtx = 302,
      /// @brief JetFitter : Jet mass
      JetFitter_mass = 303,
      /// @brief JetFitter : Jet efrac
      JetFitter_energyFraction = 304,
      /// @brief JetFitter : 3D vertex significance
      JetFitter_significance3d = 305,
      /// @brief JetFitter : Delta eta between jet and momentum sum of all tracks associated with displaced vertices reconstructed by JetFitter
      JetFitter_deltaeta = 306,
      /// @brief JetFitter : Delta phi between jet and momentum sum of all tracks associated with displaced vertices reconstructed by JetFitter
      JetFitter_deltaphi = 307,
      /// @brief JetFitter :  Number of 2-track pairs
      JetFitter_N2Tpair = 308,

      /// @brief IP2D : number of tracks
      IP2D_ntrk = 400,
      /// @brief IP2D: track grade
      IP2D_gradeOfTracks = 401,
      /// @brief IP2D : tracks from V0
      IP2D_flagFromV0ofTracks = 404,
      /// @brief IP2D : d0 value with respect to primary vertex
      IP2D_valD0wrtPVofTracks = 405,
      /// @brief IP2D : d0 significance with respect to primary vertex
      IP2D_sigD0wrtPVofTracks = 406,
      /// @brief IP2D : track contribution to B likelihood
      IP2D_weightBofTracks = 409,
      /// @brief IP2D : track contribution to U likelihood
      IP2D_weightUofTracks = 410,
      /// @brief IP2D : track contribution to C likelihood
      IP2D_weightCofTracks = 411,

      /// @brief IP3D : number of tracks
      IP3D_ntrk = 500,
      /// @brief IP3D: track grade
      IP3D_gradeOfTracks = 501,
      /// @brief IP3D : tracks from V0
      IP3D_flagFromV0ofTracks = 504,
      /// @brief IP3D : d0 value with respect to primary vertex
      IP3D_valD0wrtPVofTracks = 505,
      /// @brief IP3D : d0 significance with respect to primary vertex
      IP3D_sigD0wrtPVofTracks = 506,
      /// @brief IP3D : z0 value with respect to primary vertex
      IP3D_valZ0wrtPVofTracks = 507,
      /// @brief IP3D : z0 significance with respect to primary vertex
      IP3D_sigZ0wrtPVofTracks = 508,
      /// @brief IP3D : track contribution to B likelihood
      IP3D_weightBofTracks = 509,
      /// @brief IP3D : track contribution to U likelihood
      IP3D_weightUofTracks = 510,
      /// @brief IP3D : track contribution to C likelihood
      IP3D_weightCofTracks = 511
      

    }; //enum BTagInfo
  ///@}


} // End namespace xAOD

#endif // XAODBTAGGING_BTAGGINGENUMS_
