// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TileMuFeatureFillerTool.h 310395 2010-07-14 05:38:11Z kanno $
#ifndef TRIGMUOND3PDMAKER_TILEMUFEATUREFILLERTOOL_H
#define TRIGMUOND3PDMAKER_TILEMUFEATUREFILLERTOOL_H

// EDM include(s):
#include "TrigMuonEvent/TileMuFeature.h"

// D3PD include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

// STL include
#include <vector>

namespace D3PD {

   /**
    *  @short Tool filling information about TileMuFeature objects into the D3PD
    *
    * @author Takayuki Kanno
    *
    * $Revision: 310395 $
    * $Date: 2010-07-14 07:38:11 +0200 (Wed, 14 Jul 2010) $
    */
   class TileMuFeatureFillerTool : public BlockFillerTool< TileMuFeature > {

   public:
      /// Regular AlgTool constructor
      TileMuFeatureFillerTool( const std::string& type, const std::string& name,
                               const IInterface* parent );

      /// Function declaring the output variables
      virtual StatusCode book();
      /// Function filling the output variables
      virtual StatusCode fill( const TileMuFeature& muon );

   private:
      bool m_writeAngle;
      bool m_writeDetails;

      float*              m_eta;
      float*              m_phi;
      std::vector<float>* m_enedep;
      float*              m_quality;

   }; // class TileMuFeatureFillerTool

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_TILEMUFEATUREFILLERTOOL_H
