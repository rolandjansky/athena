// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigElectronFillerTool.h 288000 2010-03-25 18:09:04Z krasznaa $
#ifndef __D3PD_TrigElectronFillerTool_h__
#define __D3PD_TrigElectronFillerTool_h__
/*
  D3PD::TrigElectronFillerToo.h
*/
#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TrigParticle/TrigElectron.h"

namespace D3PD {

   /**
    * @short Tool saving the properties of TrigElectron objects
    *
    * @author Takanori Kohno
    *
    * $Revision: 288000 $
    * $Date: 2010-03-25 19:09:04 +0100 (Thu, 25 Mar 2010) $
    */
   class TrigElectronFillerTool : public BlockFillerTool< TrigElectron > {

   public:
      /// Regular AlgTool constructor
      TrigElectronFillerTool( const std::string& type, const std::string& name, 
                              const IInterface* parent);

      /// Function booking the variables
      StatusCode book();
      /// Function filling the variables
      StatusCode fill( const TrigElectron& x );

   private:
      // Properties
      bool m_writeBasics; ///< Write basic information about the object
      bool m_writeHypoVars; ///< Write the variables needed to re-do the hypo cuts
      bool m_writeDetails; ///< Write the most detailed information

      // D3PD variables
      int* m_roiWord; ///< Internal RoI index to which this object belongs
      float* m_Zvtx; ///< Distance in Z from the vertex
      int* m_charge; ///< Charge of the associated track

      int* m_trackAlgo; ///< Algorithm Id of the associated track
      float* m_nTRTHiTHitsRatio; ///< Ratio of high-threshold TRT hits
      float* m_DeltaEta1; ///< Eta difference between the cluster and the extrapolated track pos.
      float* m_DeltaPhi2; ///< Phi difference between the cluster and the extrapolater track pos.
      float* m_EtOverPt; ///< Ratio of cluster Et to track pT
      float* m_Reta; ///< Shower shape quantity
      float* m_Eratio; ///< Shower shape quantity
      float* m_Ethad1; ///< Shower shape quantity

      int* m_nTRTHits; ///< Number of TRT hits associated with track
      int* m_nTRTHiThresholdHits; ///< Number of high-threshold TRT hits associated with track
      int* m_trackIndx; ///< Index of the associated ID track inside the track collection
      float* m_trkPt; ///< pT of the associated track
      float* m_err_trkPt; ///< Track pT uncertainty
      float* m_err_eta; ///< Track eta uncertainty
      float* m_err_phi; ///< Track phi uncertainty
      float* m_err_Zvtx; ///< Track z0 uncertainty
      float* m_trkEtaAtCalo; ///< Eta of track extrapolated at the calorimeter
      float* m_trkPhiAtCalo; ///< Phi of track extrapolated at the calorimeter
      float* m_caloEta; ///< Eta of the calorimeter cluster
      float* m_caloPhi; ///< Phi of the calorimeter cluster
      float* m_F0; ///< Calibrated energy fraction in sampling 0
      float* m_F1; ///< Calibrated energy fraction in sampling 1
      float* m_F2; ///< Calibrated energy fraction in sampling 2
      float* m_F3; ///< Calibrated energy fraction in sampling 3

   }; // class TrigElectronFillerTool

} // namespace D3PD

#endif // __D3PD_TrigElectronFillerTool_h__

