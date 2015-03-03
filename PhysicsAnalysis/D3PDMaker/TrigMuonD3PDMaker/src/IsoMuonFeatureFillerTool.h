// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IsoMuonFeatureFillerTool.h 476044 2012-01-11 10:44:26Z krasznaa $
#ifndef TRIGMUOND3PDMAKER_ISOMUONFEATUREFILLERTOOL_H
#define TRIGMUOND3PDMAKER_ISOMUONFEATUREFILLERTOOL_H

// EDM include(s):
#include "TrigMuonEvent/IsoMuonFeature.h"

// D3PD include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Tool filling information about IsoMuonFeature objects into the D3PD
    *
    * @author Takayuki Kanno.
    *
    * $Revision: 476044 $
    * $Date: 2012-01-11 11:44:26 +0100 (Wed, 11 Jan 2012) $
    */
   class IsoMuonFeatureFillerTool : public BlockFillerTool< IsoMuonFeature > {

   public:
      /// Regular AlgTool constructor
      IsoMuonFeatureFillerTool( const std::string& type, const std::string& name,
                                const IInterface* parent );

      /// Function declaring the output variables
      virtual StatusCode book();
      /// Function filling the output variables
      virtual StatusCode fill( const IsoMuonFeature& muon );

   private:
      bool m_writeHypoInfo;
      bool m_writeDetailedInfo;
      /*
       * IsoMuonFeature variables. Note that IsoMuonFeature inherits
       * INavigable4Momentum, so 4 momentum is filled with FourMomFillerTool,
       * like in case of CombinedMuonFeatureFillerTool.
       *
       * I kept the comment of variables found in IsoMuonFeature.h
       * for clarification.
       */
      /** Transverse energy sum in 0.1 cone */
      float* m_sumet01;
      /** Transverse energy sum in 0.2 cone */
      float* m_sumet02;
      /** Transverse energy sum in 0.3 cone */
      float* m_sumet03;
      /** Transverse energy sum in 0.4 cone */
      float* m_sumet04;

      /** Transverse p<sub>T</sub> sum in 0.1 cone */
      float* m_sumpt01;
      /** Transverse p<sub>T</sub> sum in 0.2 cone */
      float* m_sumpt02;
      /** Transverse p<sub>T</sub> sum in 0.3 cone */
      float* m_sumpt03;
      /** Transverse p<sub>T</sub> sum in 0.4 cone */
      float* m_sumpt04;

      /** Pt of charged track associated to muon */
      float* m_PtMuID;
      /** Pt of max pt ID track in cone (muon excluded) */
      float* m_MaxPtID;

      /**  Isolation strategy (1->CombinedMuon seeded, 2->muFast Muon seeded, 3->L1 muon seeded) */
      int*   m_iso_strategy;
      /**  Error flag */
      int*   m_iso_errorFlag;
    
      /**  Muon RoI Id*/
      int*   m_RoiIdMu;

   }; // class IsoMuonFeatureFillerTool

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_ISOMUONFEATUREFILLERTOOL_H
