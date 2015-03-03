// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CombinedMuonFeatureFillerTool.h 476044 2012-01-11 10:44:26Z krasznaa $
#ifndef TRIGMUOND3PDMAKER_COMBINEDMUONFEATUREFILLERTOOL_H
#define TRIGMUOND3PDMAKER_COMBINEDMUONFEATUREFILLERTOOL_H

// EDM include(s):
#include "TrigMuonEvent/CombinedMuonFeature.h"

// D3PD include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Tool filling information about CombinedMuonFeature objects into the D3PD
    *
    *         The CombinedMuonFeature class doesn't store much information itself.
    *         It gathers most of its information from the ID track and the MuonFeature
    *         track that were combined to create it. Hence the list of variables
    *         describing a CombinedMuonFeature object specifically, is very short.
    *
    * @author Attila Krasznahorkay Jr.
    *
    * $Revision: 476044 $
    * $Date: 2012-01-11 11:44:26 +0100 (Wed, 11 Jan 2012) $
    */
   class CombinedMuonFeatureFillerTool : public BlockFillerTool< CombinedMuonFeature > {

   public:
      /// Regular AlgTool constructor
      CombinedMuonFeatureFillerTool( const std::string& type, const std::string& name,
                                     const IInterface* parent );

      /// Function declaring the output variables
      virtual StatusCode book();
      /// Function filling the output variables
      virtual StatusCode fill( const CombinedMuonFeature& muon );

   private:
      float* m_sigma_pt;
      int*   m_comb_strategy;
      int*   m_comb_errorFlag;
      int*   m_comb_matchFlag;

   }; // class CombinedMuonFeatureFillerTool

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_COMBINEDMUONFEATUREFILLERTOOL_H
