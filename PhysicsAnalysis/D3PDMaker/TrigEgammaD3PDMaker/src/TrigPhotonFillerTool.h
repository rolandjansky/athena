// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigPhotonFillerTool.h 288000 2010-03-25 18:09:04Z krasznaa $
#ifndef __D3PD_TrigPhotonFillerTool_h__
#define __D3PD_TrigPhotonFillerTool_h__
/*
  D3PD::TrigPhotonFillerToo.h
*/
#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TrigParticle/TrigPhoton.h"

namespace D3PD {

   /**
    *  @short Tool saving the properties of TrigPhoton objects
    *
    * @author Takanori Kohno
    *
    * $Revision: 288000 $
    * $Date: 2010-03-25 19:09:04 +0100 (Thu, 25 Mar 2010) $
    */
   class TrigPhotonFillerTool : public BlockFillerTool<TrigPhoton> {

   public:
      /// Regular AlgTool constructor
      TrigPhotonFillerTool( const std::string& type, const std::string& name, 
                            const IInterface* parent );

      /// Function booking the variables
      StatusCode book();
      /// Function filling the variables
      StatusCode fill( const TrigPhoton& x );

   private:
      // Properties
      bool m_writeBasics; ///< Write basic information about the object
      bool m_writeHypoVars; ///< Write the variables needed to re-do the hypo cuts
      bool m_writeDetails; ///< Write the most detailed information

      // D3PD variables
      int* m_roiWord; ///< Internal RoI index to which this object belongs

      float* m_HadEt1; ///< Hadronic energy in 1st hadronic sampling behind cluster
      float* m_Eratio; ///< Fraction difference of EM cluster emaxs1 and e2tsts1
      float* m_Reta; ///< Cluster energy in 3x7 cells / energy in 7x7 cells in 2nd sampling
      float* m_dPhi; ///< Difference between cluster and RoI phi
      float* m_dEta; ///< Difference between cluster and RoI eta
      float* m_F1; ///< Calibrated energy fraction in sampling 1

      /// Energy in 7 strips (around hottest strip) minus energy in 3 strips
      /// divided by energy in 3 strips
      float* m_Fside;
      float* m_Weta2; ///< Cluster width (based on a 3x5 cluster - 2nd layer)
      float* m_F0; ///< Calibrated energy fraction in sampling 0
      float* m_F2; ///< Calibrated energy fraction in sampling 2
      float* m_F3; ///< Calibrated energy fraction in sampling 3

   }; // class TrigPhotonFillerTool

} // namespace D3PD

#endif // __D3PD_TrigPhotonFillerTool_h__
