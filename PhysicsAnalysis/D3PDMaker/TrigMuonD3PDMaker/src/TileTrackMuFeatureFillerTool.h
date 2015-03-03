// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TileTrackMuFeatureFillerTool.h 307534 2010-06-28 16:16:41Z krasznaa $
#ifndef TRIGMUOND3PDMAKER_TILETRACKMUFEATUREFILLERTOOL_H
#define TRIGMUOND3PDMAKER_TILETRACKMUFEATUREFILLERTOOL_H

// EDM include(s):
#include "TrigMuonEvent/TileTrackMuFeature.h"

// D3PD include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Tool filling information about TileTrackMuFeature objects into the D3PD
    *
    *
    * @author Takayuki Kanno
    *
    * $Revision: 307534 $
    * $Date: 2010-06-28 18:16:41 +0200 (Mon, 28 Jun 2010) $
    */
   class TileTrackMuFeatureFillerTool : public BlockFillerTool< TileTrackMuFeature > {

   public:
      /// Regular AlgTool constructor
      TileTrackMuFeatureFillerTool( const std::string& type, const std::string& name,
                                    const IInterface* parent );

      /// Function declaring the output variables
      virtual StatusCode book();
      /// Function filling the output variables
      virtual StatusCode fill( const TileTrackMuFeature& muon );

   private:
      /*
       * TileTrackMuFeature variables. Note that TileTrackMuFeature inherits
       * INavigable4Momentum, so 4 momentum is filled with FourMomFillerTool,
       * like in case of CombinedMuonFeatureFillerTool.
       */
      float* m_PtTR_Trk;
      float* m_EtaTR_Trk;
      float* m_PhiTR_Trk;
      int*   m_Typ_IDTrk;

   }; // class TileTrackMuFeatureFillerTool

} // namespace D3PD

#endif //TRIGMUOND3PDMAKER_TILETRACKMUFEATUREFILLERTOOL_H
