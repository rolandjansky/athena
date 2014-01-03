// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonFeature4MomFillerTool.h 310395 2010-07-14 05:38:11Z kanno $
#ifndef TRIGMUOND3PDMAKER_MUONFEATURE4MOMFILLERTOOL_H
#define TRIGMUOND3PDMAKER_MUONFEATURE4MOMFILLERTOOL_H

// EDM include(s):
#include "TrigMuonEvent/MuonFeature.h"

// D3PD include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Tool filling 4-momentum information about MuonFeature objects
    *
    *         This tool can be used to save some of the most basic information
    *         about MuonFeature objects into the D3PD. The saved variables
    *         define the position of the muon reconstructed by muFast.
    *
    * @author Attila Krasznahorkay Jr.
    *
    * $Revision: 310395 $
    * $Date: 2010-07-14 07:38:11 +0200 (Wed, 14 Jul 2010) $
    */
   class MuonFeature4MomFillerTool : public BlockFillerTool< MuonFeature > {

   public:
      /// Regular AlgTool constructor
      MuonFeature4MomFillerTool( const std::string& type, const std::string& name,
                                 const IInterface* parent );

      /// Function declaring the output variables
      virtual StatusCode book();
      /// Function filling the output variables
      virtual StatusCode fill( const MuonFeature& muon );

   private:
      float* m_pt;
      float* m_eta;
      float* m_phi;

   }; // class MuonFeature4MomFillerTool

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_MUONFEATURE4MOMFILLERTOOL_H
