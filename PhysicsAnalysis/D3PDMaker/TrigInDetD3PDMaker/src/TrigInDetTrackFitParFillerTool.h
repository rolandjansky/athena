// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigInDetTrackFitParFillerTool.h 340544 2011-01-18 12:51:04Z krasznaa $
#ifndef TRIGINDETD3PDMAKER_TRIGINDETTRACKFITPARFILLERTOOL_H
#define TRIGINDETD3PDMAKER_TRIGINDETTRACKFITPARFILLERTOOL_H

// EDM include(s):
#include "TrigInDetEvent/TrigInDetTrackFitPar.h"

// D3PD include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Filler tool saving information about TrigInDetTrackFitPar objects
    *
    * @author Dmitry Emeliyanov
    * @author Attila Krasznahorkay
    *
    * $Revision: 340544 $
    * $Date: 2011-01-18 13:51:04 +0100 (Tue, 18 Jan 2011) $
    */
   class TrigInDetTrackFitParFillerTool :
      public BlockFillerTool< TrigInDetTrackFitPar > {

   public:
      /// Regular AlgTool constructor
      TrigInDetTrackFitParFillerTool( const std::string& type, const std::string& name,
                                      const IInterface* parent );

      /// Function declaring the output variables
      virtual StatusCode book();
      /// Function filling the output variables
      virtual StatusCode fill( const TrigInDetTrackFitPar& fp );

   private:
      bool m_writeBasicInfo; ///< Write basic information about the object
      bool m_writeHypoInfo; ///< Write the variables needed to re-do the hypo cuts
      bool m_writeDetailedInfo; ///< Write the most detailed information

      float* m_a0; ///< Transverse impact parameter
      float* m_z0; ///< Longitudinal impact parameter
      float* m_phi0; ///< Azimuthal angle of the momentum
      float* m_eta; ///< Pseudorapidity
      float* m_pT; ///< Transverse momentum
      int* m_surfaceType; ///< Surface type
      float* m_surfaceCoordinate; ///< surface reference coordinate
      std::vector< float >* m_covariance; ///< Covariance of track parameters

   }; // class TrigInDetTrackFitParFillerTool

} // namespace D3PD

#endif // TRIGINDETD3PDMAKER_TRIGINDETTRACKFITPARFILLERTOOL_H
